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
#include <QDir>
#include <QtWebKit/private/qquickwebview_p.h>
#include <QTimer>

#include "applicationdescription.h"
#include "webappmanager.h"
#include "webapplication.h"
#include "webappmanagerservice.h"

namespace luna
{

WebAppManager::WebAppManager(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{
    setApplicationName("LunaWebAppMgr");
    setQuitOnLastWindowClosed(false);

    QQuickWebViewExperimental::setFlickableViewportEnabled(false);

    connect(this, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));

    mService = new WebAppManagerService(this);
}

WebAppManager::~WebAppManager()
{
    onAboutToQuit();
}

bool WebAppManager::validateApplication(const ApplicationDescription& desc)
{
    if (desc.id().length() == 0)
        return false;

    if (desc.entryPoint().isLocalFile() && !QFile::exists(desc.entryPoint().toLocalFile()))
        return false;

    return true;
}

WebApplication* WebAppManager::launchApp(const QString &appDesc, const QString &parameters, int64_t processId)
{
    ApplicationDescription desc(appDesc);

    if (!validateApplication(desc)) {
        qWarning("Got invalid application description for app %s",
                 desc.id().toUtf8().constData());
        return NULL;
    }

    if (mApplications.contains(desc.id())) {
        WebApplication *app = mApplications.value(desc.id());
        app->relaunch(parameters);
        return app;
    }

    QString windowType = "card";
    if (desc.id() == "com.palm.launcher")
        windowType = "launcher";

    QUrl entryPoint = desc.entryPoint();
    WebApplication *app = new WebApplication(this, entryPoint, windowType,
                                             desc, parameters, processId);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationClosed()));

    this->setQuitOnLastWindowClosed(false);

    mApplications.insert(app->id(), app);

    mService->notifyAppHasStarted(app->id(), app->processId());

    return app;
}

WebApplication* WebAppManager::launchUrl(const QUrl &url, const QString &windowType,
                               const QString &appDesc, const QString &parameters, int64_t processId)
{
    ApplicationDescription desc(appDesc);

    if (!validateApplication(desc)) {
        qWarning("Got invalid application description for app %s",
                 desc.id().toUtf8().constData());
        return NULL;
    }

    // FIXME is this correct when launching an URL?
    if (mApplications.contains(desc.id())) {
        WebApplication *application = mApplications.value(desc.id());
        application->relaunch(parameters);
        return application;
    }

    QQuickWebViewExperimental::setFlickableViewportEnabled(desc.flickable());

    WebApplication *app = new WebApplication(this, url, windowType, desc, parameters,
                                             processId);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationClosed()));

    mApplications.insert(app->id(), app);

    mService->notifyAppHasStarted(app->id(), app->processId());

    return app;
}

void WebAppManager::onAboutToQuit()
{
}

void WebAppManager::onApplicationClosed()
{
    WebApplication *app = static_cast<WebApplication*>(sender());

    if (!mApplications.contains(app->id())) {
        qWarning("BUG: Got close event from not running application!?");
        return;
    }

    mApplications.remove(app->id());

    mService->notifyAppHasFinished(app->id(), app->processId());

    qDebug() << "Application" << app->id() << "was closed";
    delete app;
}

void WebAppManager::killApp(const QString &appId)
{
    WebApplication *appToKill = 0;

    Q_FOREACH(WebApplication *app, mApplications) {
        if (app->id() == appId) {
            appToKill = app;
            break;
        }
    }

    if (appToKill)
        appToKill->kill();
}

void WebAppManager::killApp(int64_t processId)
{
    WebApplication *appToKill = 0;

    Q_FOREACH(WebApplication *app, mApplications) {
        if (app->processId() == processId) {
            appToKill = app;
            break;
        }
    }

    if (appToKill)
        appToKill->kill();
}

bool WebAppManager::isAppRunning(const QString &appId)
{
    return mApplications.contains(appId);
}

QList<WebApplication*> WebAppManager::applications() const
{
    return mApplications.values();
}

bool WebAppManager::relaunch(const QString &appId, const QString &params)
{
    WebApplication *targetApp = 0;

    Q_FOREACH(WebApplication *app, mApplications) {
        if (app->id() == appId) {
            targetApp = app;
            break;
        }
    }

    if (!targetApp)
        return false;

    targetApp->relaunch(params);

    return true;
}

void WebAppManager::clearMemoryCaches()
{
    Q_FOREACH(WebApplication *app, mApplications) {
        app->clearMemoryCaches();
    }
}

void WebAppManager::clearMemoryCaches(qint64 processId)
{
    Q_FOREACH(WebApplication *app, mApplications) {
        if (app->processId() == processId) {
            app->clearMemoryCaches();
            break;
        }
    }
}

void WebAppManager::clearMemoryCaches(const QString& appId)
{
    Q_FOREACH(WebApplication *app, mApplications) {
        if (app->id() == appId) {
            app->clearMemoryCaches();
            break;
        }
    }
}

} // namespace luna
