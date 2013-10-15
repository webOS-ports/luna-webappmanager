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

WebApplicationWindow::WebApplicationWindow(WebApplication *application, const QUrl& url, bool headless, QObject *parent) :
    QObject(parent),
    mApplication(application),
    mEngine(this),
    mWindow(0),
    mHeadless(headless),
    mUrl(url)
{
    createAndSetup();
}

WebApplicationWindow::~WebApplicationWindow()
{
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

    mWindow = static_cast<QQuickWindow*>(window);
    mWindow->installEventFilter(this);

    if (!mHeadless) {
        mWindow->setSurfaceType(QSurface::OpenGLSurface);
        QSurfaceFormat surfaceFormat = mWindow->format();
        surfaceFormat.setAlphaBufferSize(8);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
        mWindow->setFormat(surfaceFormat);
    }

    mWebView = window->findChild<QQuickWebView*>("webView");

    connect(mWebView->experimental(), SIGNAL(createNewPage(QWebNewPageRequest*)),
            this, SLOT(onCreateNewPage(QWebNewPageRequest*)));

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

} // namespace luna
