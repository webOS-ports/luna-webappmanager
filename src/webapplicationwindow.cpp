/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2015 Christophe Chapuis <chris.chapuis@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <QDebug>
#include <QQmlContext>
#include <QQmlComponent>
#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QDir>

#include <QtWebEngine/private/qquickwebengineview_p.h>
#include <QtWebEngine/private/qquickwebenginescript_p.h>
#include <QtWebEngine/private/qquickwebengineloadrequest_p.h>
#include <QtWebEngine/private/qquickwebenginenewviewrequest_p.h>

#include <QScreen>

#include <Settings.h>

#include "applicationdescription.h"
#include "webapplication.h"
#include "webapplicationwindow.h"

#include "extensions/palmsystemextension.h"
#include "extensions/wifimanager.h"
#include "extensions/bluetoothmanager.h"
#include "extensions/inappbrowserextension.h"

namespace luna
{

WebApplicationWindow::WebApplicationWindow(WebApplication *application, const QUrl& url,
                                           const QString& windowType, const QSize& size,
                                           bool headless, const QVariantMap &windowAttributesMap,
                                           int parentWindowId,
                                           QObject *parent) :
    ApplicationEnvironment(parent),
    mApplication(application),
    mEngine(0),
    mRootItem(0),
    mWindow(0),
    mHeadless(headless),
    mUrl(url),
    mWindowType(windowType),
    mKeepAlive(false),
    mStagePreparing(true),
    mStageReady(false),
    mStageReadyTimer(this),
    mSize(size),
    mWindowId(0),
    mParentWindowId(parentWindowId),
    mLoadingAnimationDisabled(false),
    mLaunchedHidden(application->id() == "com.palm.launcher")
{
    qDebug() << __PRETTY_FUNCTION__ << this << size;

    connect(&mStageReadyTimer, SIGNAL(timeout()), this, SLOT(onStageReadyTimeout()));
    mStageReadyTimer.setSingleShot(true);

    assignCorrectTrustScope();

    createAndSetup(windowAttributesMap);
}

WebApplicationWindow::~WebApplicationWindow()
{
    qDebug() << __PRETTY_FUNCTION__ << this;

    Q_FOREACH(BaseExtension *extension, mExtensions.values())
        delete extension;

    mExtensions.clear();

    if (mHeadless)
        delete mEngine;

    if (mWindow)
        delete mWindow;
}

void WebApplicationWindow::destroy()
{
    if (mWindow)
        mWindow->destroy();
}

void WebApplicationWindow::assignCorrectTrustScope()
{
    if (mUrl.scheme() == "file")
        mTrustScope = TrustScopeSystem;
    else
        mTrustScope = TrustScopeRemote;
}

void WebApplicationWindow::setWindowProperty(const QString &name, const QVariant &value)
{
    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    nativeInterface->setWindowProperty(mWindow->handle(), name, value);
}

QVariant WebApplicationWindow::getWindowProperty(const QString &name)
{
    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    return nativeInterface->windowProperty(mWindow->handle(), name);
}

void WebApplicationWindow::updateWindowProperty(const QString &name)
{
    qDebug() << Q_FUNC_INFO << "Window property" << name << "was updated";

    if (name == "_LUNE_WINDOW_ID")
        mWindowId = getWindowProperty("_LUNE_WINDOW_ID").toInt();
    else if (name == "_LUNE_WINDOW_PARENT_ID")
        mParentWindowId = getWindowProperty("_LUNE_WINDOW_PARENT_ID").toInt();
}

void WebApplicationWindow::onWindowPropertyChanged(QPlatformWindow *window, const QString &name)
{
    if (!mWindow)
        return;

    if (window != mWindow->handle())
        return;

    updateWindowProperty(name);
}

void WebApplicationWindow::configureQmlEngine()
{
    if (!mEngine)
        return;

    mEngine->rootContext()->setContextProperty("webApp", mApplication);
    mEngine->rootContext()->setContextProperty("webAppWindow", this);
    if( QDir().mkpath("/media/internal/.app-storage") )
        mEngine->setOfflineStoragePath("/media/internal/.app-storage");

}

QQuickWebEngineScript *WebApplicationWindow::getScriptFromUrl(const QString &iscriptName, QString iUrl, QQuickWebEngineScript::InjectionPoint injectionPoint, bool forAllFrames)
{
    QFile f(iUrl);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open user script " << iUrl;
        return 0;
    }

    QQuickWebEngineScript *newScript = new QQuickWebEngineScript();

    newScript->setName(iscriptName);
    newScript->setSourceCode(QString::fromUtf8(f.readAll()));
    newScript->setInjectionPoint(injectionPoint);
    newScript->setRunOnSubframes(forAllFrames);
    newScript->setWorldId(QQuickWebEngineScript::MainWorld);

    return newScript;
}

void WebApplicationWindow::createAndSetup(const QVariantMap &windowAttributesMap)
{
    if (mTrustScope == TrustScopeSystem) {
        mUserScripts.append(getScriptFromUrl("webosAPI", QString("://qml/webos-api.js"), QQuickWebEngineScript::DocumentCreation, false));
        createDefaultExtensions();
    }

    if (mWindowType == "dashboard")
        mLoadingAnimationDisabled = true;

    if (mHeadless) {
        qDebug() << __PRETTY_FUNCTION__ << "Creating application container for headless ...";

        mEngine = new QQmlEngine;
        configureQmlEngine();

        QQmlComponent component(mEngine, QUrl(QString("qrc:///qml/ApplicationContainer.qml")));
        mRootItem = qobject_cast<QQuickItem*>(component.create());
    }
    else {
        mWindow = new QQuickView;
        mWindow->installEventFilter(this);


        mEngine = mWindow->engine();
        configureQmlEngine();

        connect(mWindow, &QObject::destroyed,  [=](QObject *obj) {
            qDebug() << "Window destroyed";
        });

        mWindow->setColor(Qt::transparent);

        mWindow->reportContentOrientationChange(QGuiApplication::primaryScreen()->primaryOrientation());

        mWindow->setSurfaceType(QSurface::OpenGLSurface);
        QSurfaceFormat surfaceFormat = mWindow->format();
        surfaceFormat.setAlphaBufferSize(8);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
        mWindow->setFormat(surfaceFormat);

        // make sure the platform window gets created to be able to set it's
        // window properties
        mWindow->create();

        // set different information bits for our window
        foreach(QString attrKey, windowAttributesMap.keys()) {
            setWindowProperty("LuneOS_"+attrKey,windowAttributesMap.value(attrKey));
        }

        setWindowProperty(QString("_LUNE_WINDOW_TYPE"), QVariant(mWindowType));
        setWindowProperty(QString("_LUNE_WINDOW_PARENT_ID"), QVariant(mParentWindowId));
        setWindowProperty(QString("_LUNE_WINDOW_LOADING_ANIMATION_DISABLED"), QVariant(mApplication->loadingAnimationDisabled()));
        setWindowProperty(QString("_LUNE_APP_ICON"), QVariant(mApplication->icon()));
        setWindowProperty(QString("_LUNE_APP_ID"), QVariant(mApplication->id()));

        connect(mWindow, SIGNAL(visibleChanged(bool)), this, SLOT(onVisibleChanged(bool)));

        QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
        connect(nativeInterface, SIGNAL(windowPropertyChanged(QPlatformWindow*, const QString&)),
                this, SLOT(onWindowPropertyChanged(QPlatformWindow*, const QString&)));

        mWindow->setSource(QUrl(QString("qrc:///qml/ApplicationContainer.qml")));

        mRootItem = mWindow->rootObject();

        mWindow->resize(mSize);
    }
}

void WebApplicationWindow::configureWebView(QQuickItem *webViewItem)
{
    qDebug() << __PRETTY_FUNCTION__ << "Configuring application webview ...";

    mWebView = qobject_cast<QQuickWebEngineView*>(webViewItem);

    if (!mWebView) {
        qWarning() << __PRETTY_FUNCTION__ << "Couldn't find webView";
        return;
    }

    connect(mWebView, SIGNAL(loadingChanged(QQuickWebEngineLoadRequest*)),
            this, SLOT(onLoadingChanged(QQuickWebEngineLoadRequest*)));

    connect(mWebView, SIGNAL(newViewRequested(QQuickWebEngineNewViewRequest*)),
            this, SLOT(onCreateNewPage(QQuickWebEngineNewViewRequest*)));
    connect(mWebView, SIGNAL(windowCloseRequested()), this, SLOT(onClosePage()));

    if (mTrustScope == TrustScopeSystem)
        loadAllExtensions();

   mWebView->setUrl(mUrl);

    /* If we're running a remote site mark the window as fully loaded */
    if (mTrustScope == TrustScopeRemote)
        stageReady();
}

void WebApplicationWindow::onStageReadyTimeout()
{
    qDebug() << __PRETTY_FUNCTION__;

    stageReady();
}

void WebApplicationWindow::onVisibleChanged(bool visible)
{
    qDebug() << __PRETTY_FUNCTION__ << visible;

    emit visibleChanged();
}

void WebApplicationWindow::setupPage()
{
    // We need to finish the stage preparation in case of a remote entry point
    // otherwise it will never stop loading
    if (mApplication->hasRemoteEntryPoint())
        stageReady();
}

double WebApplicationWindow::devicePixelRatio() const
{
    qreal zoomFactor = Settings::LunaSettings()->layoutScale;

    // correct zoom factor for some applications which are not scaled properly (aka
    // the Open webOS core-apps ...)
    if (Settings::LunaSettings()->compatApps.find(mApplication->id().toStdString()) !=
        Settings::LunaSettings()->compatApps.end())
        zoomFactor = Settings::LunaSettings()->layoutScaleCompat;

    return zoomFactor;
}

void WebApplicationWindow::notifyAppAboutFocusState(bool focus)
{
    qDebug() << "DEBUG: We become" << (focus ? "focused" : "unfocused");

    QString action = focus ? "stageActivated" : "stageDeactivated";

    emit focusChanged();

    if (mTrustScope == TrustScopeSystem)
        executeScript(QString("if (window.Mojo && Mojo.%1) Mojo.%1()").arg(action));

    mApplication->changeActivityFocus(focus);
}

void WebApplicationWindow::onLoadingChanged(QQuickWebEngineLoadRequest *request)
{
    qDebug() << Q_FUNC_INFO << "id" << mApplication->id() << "status" << request->status();

    switch (request->status()) {
    case QQuickWebEngineView::LoadStartedStatus:
        setupPage();
        return;
    case QQuickWebEngineView::LoadStoppedStatus:
    case QQuickWebEngineView::LoadFailedStatus:
        return;
    case QQuickWebEngineView::LoadSucceededStatus:
        break;
    }

    Q_FOREACH(BaseExtension *extension, mExtensions.values())
        extension->initialize();

    // Fix the viewport of the app
    QFile f("://qml/setupViewport.js");
    if (f.open(QIODevice::ReadOnly)) {
        mWebView->runJavaScript(QString::fromUtf8(f.readAll()));
    }

    // If we're a headless app we don't show the window and in case of an
    // application with an remote entry point it's already visible at
    // this point
    if (mHeadless || mApplication->hasRemoteEntryPoint())
        return;

    // if the framework  called us with an explicit stagePreparing call we
    // will wait for the call to stageReady to come in
    if (mStagePreparing && !mStageReady) {
        if (!mWindow->isVisible() && !mStageReadyTimer.isActive()) {
            qDebug() << Q_FUNC_INFO << "id" << mApplication->id() << "kicking stage ready timer";
            mStageReadyTimer.start(3000);
        }
        else {
            qDebug() << Q_FUNC_INFO << "id" << mApplication->id() << "omitting stage ready timer as alreay active or window visible";
        }
        return;
    }

    if (!mWindow->isVisible())
        mWindow->show();
}

void WebApplicationWindow::onCreateNewPage(QQuickWebEngineNewViewRequest *request)
{
    mApplication->createWindow(request);
}

void WebApplicationWindow::onClosePage()
{
    qDebug() << __PRETTY_FUNCTION__;
    mApplication->closeWindow(this);
}

void WebApplicationWindow::createDefaultExtensions()
{
    addExtension(new PalmSystemExtension(this));
    addExtension(new InAppBrowserExtension(this));

    if (mApplication->id() == "org.webosports.app.settings") {
        addExtension(new WiFiManager(this));
        addExtension(new BluetoothManager(this));
    }
}

void WebApplicationWindow::addExtension(BaseExtension *extension)
{
    qDebug() << "Adding extension" << extension->name();
    mExtensions.insert(extension->name(), extension);
}

void WebApplicationWindow::loadAllExtensions()
{
    foreach(BaseExtension *extension, mExtensions.values()) {
        qDebug() << "Initializing extension" << extension->name();
        emit extensionWantsToBeAdded(extension->name(), extension);
    }
}

bool WebApplicationWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == mWindow) {
        switch (event->type()) {
        case QEvent::Close:
            mWindow->setVisible(false);
            mApplication->closeWindow(this);
            break;
        case QEvent::FocusIn:
            notifyAppAboutFocusState(true);
            break;
        case QEvent::FocusOut:
            notifyAppAboutFocusState(false);
            break;
        default:
            break;
        }
    }

    return false;
}

QString WebApplicationWindow::getIdentifierForFrame(const QString& id, const QString& url)
{
    QString identifier = mApplication->identifier();

    if (url.startsWith("file:///usr/palm/applications/com.palm.systemui"))
        identifier = QString("com.palm.systemui %1").arg(mApplication->processId());

    qDebug() << __PRETTY_FUNCTION__ << "Decided identifier for frame" << id << "is" << identifier;

    return identifier;
}

void WebApplicationWindow::stagePreparing()
{
    qDebug() << __PRETTY_FUNCTION__ << "id" << mApplication->id();

    mStagePreparing = true;
    emit readyChanged();
}

void WebApplicationWindow::stageReady()
{
    qDebug() << __PRETTY_FUNCTION__ << "id" << mApplication->id();

    mStagePreparing = false;
    mStageReady = true;

    if (mWindow && !mLaunchedHidden && !mWindow->isVisible())
        mWindow->show();

    emit readyChanged();

    mStageReadyTimer.stop();
}

void WebApplicationWindow::show()
{
    if (!mWindow)
        return;

    qDebug() << __PRETTY_FUNCTION__ << "id" << mApplication->id();

    mWindow->show();
}

void WebApplicationWindow::hide()
{
    if (!mWindow)
        return;

    qDebug() << __PRETTY_FUNCTION__ << "id" << mApplication->id();

    mWindow->hide();
}

void WebApplicationWindow::focus()
{
    if (!mWindow)
        return;

    qDebug() << __PRETTY_FUNCTION__ << "id" << mApplication->id();

    /* When we're closed we have to make sure we're visible before
     * raising ourself */
    if (!mWindow->isVisible())
        mWindow->show();

    mWindow->raise();
}

void WebApplicationWindow::unfocus()
{
    if (!mWindow)
        return;

    qDebug() << __PRETTY_FUNCTION__ << "id" << mApplication->id();

    mWindow->lower();
}

void WebApplicationWindow::executeScript(const QString &script)
{
    emit javaScriptExecNeeded(script);
}

void WebApplicationWindow::registerUserScript(const QString &path)
{
    mUserScripts.append(getScriptFromUrl(QString("userScript%1").arg(mUserScripts.size()),
                                         path,
                                         mTrustScope == TrustScopeSystem ? QQuickWebEngineScript::DocumentCreation : QQuickWebEngineScript::Deferred,
                                         false));
    emit userScriptsChanged();
}

void WebApplicationWindow::clearMemoryCaches()
{
    if (!mWebView)
        return;

    // Didn't find yet any equivalent for QtWebEngine.
    // There is a WebCache::clearCache() in Blink, but I didn't see where that was exposed in chromium.
    // mWebView->clearMemoryCaches();
}

WebApplication* WebApplicationWindow::application() const
{
    return mApplication;
}

QQuickWebEngineView *WebApplicationWindow::webView() const
{
    return mWebView;
}

void WebApplicationWindow::setKeepAlive(bool keepAlive)
{
    mKeepAlive = keepAlive;
}

bool WebApplicationWindow::keepAlive() const
{
    return mKeepAlive;
}

bool WebApplicationWindow::headless() const
{
    return mHeadless;
}

QQmlListProperty<QQuickWebEngineScript> WebApplicationWindow::userScripts()
{
    return QQmlListProperty<QQuickWebEngineScript>(this, mUserScripts);
}

bool WebApplicationWindow::ready() const
{
    return mStageReady && !mStagePreparing;
}

QSize WebApplicationWindow::size() const
{
    return mSize;
}

bool WebApplicationWindow::active() const
{
    if (mWindow)
        return mWindow->isActive();

    return true;
}

QString WebApplicationWindow::trustScope() const
{
    if (mTrustScope == TrustScopeSystem)
        return QString("system");

    return QString("remote");
}

QUrl WebApplicationWindow::url() const
{
    return mUrl;
}

int WebApplicationWindow::windowId() const
{
    return mWindowId;
}

int WebApplicationWindow::parentWindowId() const
{
    return mParentWindowId;
}

bool WebApplicationWindow::loadingAnimationDisabled() const
{
    return mLoadingAnimationDisabled;
}

QString WebApplicationWindow::windowType() const
{
    return mWindowType;
}

bool WebApplicationWindow::visible() const
{
    return mWindow ? mWindow->isVisible() : false;
}

QQmlEngine* WebApplicationWindow::qmlEngine() const
{
    return mEngine;
}

QQuickItem* WebApplicationWindow::rootItem() const
{
    return mRootItem;
}

bool WebApplicationWindow::hasFocus() const
{
    if (!mWindow)
        return false;

    return mWindow->isActive();
}

bool WebApplicationWindow::isMainWindow() const
{
    return mApplication->isMainWindow(this);
}

} // namespace luna
