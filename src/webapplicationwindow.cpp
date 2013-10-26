/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
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
#include <QtWebKit/private/qquickwebview_p.h>
#include <QtWebKit/private/qwebnewpagerequest_p.h>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QJsonDocument>
#include <QJsonObject>

#include <Settings.h>

#include "webappmanager.h"
#include "webappmanagerservice.h"
#include "applicationdescription.h"
#include "webapplication.h"
#include "webapplicationwindow.h"

#include "plugins/baseplugin.h"
#include "plugins/palmsystemplugin.h"

namespace luna
{

WebApplicationWindow::WebApplicationWindow(WebApplication *application, const QUrl& url, const QString& windowType,
                                           bool headless, QObject *parent) :
    QObject(parent),
    mApplication(application),
    mEngine(this),
    mWindow(0),
    mHeadless(headless),
    mUrl(url),
    mWindowType(windowType),
    mKeepAlive(false)
{
    createAndSetup();
}

WebApplicationWindow::~WebApplicationWindow()
{
    // in headless case we don't have a window we can close
    if (mWindow)
        mWindow->close();
}

void WebApplicationWindow::setWindowProperty(const QString &name, const QVariant &value)
{
    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    nativeInterface->setWindowProperty(mWindow->handle(), name, value);
}

void WebApplicationWindow::createAndSetup()
{
    // FIXME evaluate window properties and configure the window accordingly

    mEngine.rootContext()->setContextProperty("webApp", mApplication);
    mEngine.rootContext()->setContextProperty("webAppWindow", this);
    mEngine.rootContext()->setContextProperty("webAppUrl", mUrl);

    QQmlComponent windowComponent(&mEngine,
        QUrl(QString("qrc:///qml/%1.qml").arg(mHeadless ? "HeadlessWindow" : "Window")));
    if (windowComponent.isError()) {
        qCritical() << "Errors while loading window component:";
        qCritical() << windowComponent.errors();
        return;
    }

    QObject *window = windowComponent.create();
    if (!window) {
        qCritical() << "Failed to create application window:";
        qCritical() << windowComponent.errors();
        return;
    }

    if (!mHeadless) {
        mWindow = static_cast<QQuickWindow*>(window);
        mWindow->installEventFilter(this);

        mWindow->setSurfaceType(QSurface::OpenGLSurface);
        QSurfaceFormat surfaceFormat = mWindow->format();
        surfaceFormat.setAlphaBufferSize(8);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
        mWindow->setFormat(surfaceFormat);

        // make sure the platform window gets created to be able to set it's
        // window properties
        mWindow->create();

        setWindowProperty(QString("type"), QVariant(mWindowType));
    }

    mWebView = window->findChild<QQuickWebView*>("webView");

    connect(mWebView->experimental(), SIGNAL(createNewPage(QWebNewPageRequest*)),
            this, SLOT(onCreateNewPage(QWebNewPageRequest*)));
    connect(mWebView->experimental(), SIGNAL(syncMessageReceived(const QVariantMap&, QString&)),
            this, SLOT(onSyncMessageReceived(const QVariantMap&, QString&)));

    qreal zoomFactor = Settings::LunaSettings()->layoutScale;

    // correct zoom factor for some applications which are not scaled properly (aka
    // the Open webOS core-apps ...)
    if (Settings::LunaSettings()->compatApps.find(mApplication->id().toStdString()) !=
        Settings::LunaSettings()->compatApps.end())
        zoomFactor = Settings::LunaSettings()->layoutScaleCompat;

    mWebView->setZoomFactor(zoomFactor);

    createPlugins();
}

void WebApplicationWindow::onCreateNewPage(QWebNewPageRequest *request)
{
    mApplication->createWindow(request);
}

void WebApplicationWindow::onSyncMessageReceived(const QVariantMap& message, QString& response)
{
    if (!message.contains("data"))
        return;

    QString data = message.value("data").toString();

    QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());

    if (!document.isObject())
        return;

    QJsonObject rootObject = document.object();

    QString messageType;
    if (!rootObject.contains("messageType") || !rootObject.value("messageType").isString())
        return;

    messageType = rootObject.value("messageType").toString();
    if (messageType != "callSyncPluginFunction")
        return;

    if (!(rootObject.contains("plugin") && rootObject.value("plugin").isString()) ||
        !(rootObject.contains("func") && rootObject.value("func").isString()) ||
        !(rootObject.contains("params") && rootObject.value("params").isArray()))
        return;

    QString pluginName = rootObject.value("plugin").toString();
    QString funcName = rootObject.value("func").toString();
    QJsonArray params = rootObject.value("params").toArray();

    if (!mPlugins.contains(pluginName))
        return;

    BasePlugin *plugin = mPlugins.value(pluginName);
    response = plugin->handleSynchronousCall(funcName, params);
}

void WebApplicationWindow::createPlugins()
{
    createAndInitializePlugin(new PalmSystemPlugin(this));
}

void WebApplicationWindow::createAndInitializePlugin(BasePlugin *plugin)
{
    mPlugins.insert(plugin->name(), plugin);
    emit pluginWantsToBeAdded(plugin->name(), plugin);
}

bool WebApplicationWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == mWindow) {
        switch (event->type()) {
        case QEvent::Close:
            emit closed();
            break;
        case QEvent::FocusIn:
            mApplication->changeActivityFocus(true);
            break;
        case QEvent::FocusOut:
            mApplication->changeActivityFocus(false);
            break;
        default:
            break;
        }
    }

    return false;
}

void WebApplicationWindow::show()
{
    if (!mWindow)
        return;

    mWindow->show();
}

void WebApplicationWindow::hide()
{
    if (!mWindow)
        return;

    mWindow->hide();
}

void WebApplicationWindow::executeScript(const QString &script)
{
    emit javaScriptExecNeeded(script);
}

WebApplication* WebApplicationWindow::application() const
{
    return mApplication;
}

QQuickWebView *WebApplicationWindow::webView() const
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

} // namespace luna
