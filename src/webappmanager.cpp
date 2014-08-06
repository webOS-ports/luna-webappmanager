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

    // We're using a static list here to mark specific applications allowed to run in
    // headless mode (primary window will be not visible). The list should only contain
    // legacy applications. All new applications should not use the headless mode anymore
    // and will refuse to start. There should really no need to extend this list and
    // therefore it will be kept static forever.
    mAllowedHeadlessApps << "com.palm.app.email";
    mAllowedHeadlessApps << "com.palm.app.calendar";
    mAllowedHeadlessApps << "com.palm.app.clock";
    mAllowedHeadlessApps << "com.palm.systemui";
    mAllowedHeadlessApps << "org.webosinternals.tweaks";
    mAllowedHeadlessApps << "org.webosports.app.calendar";

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

    if (desc.headless() && !mAllowedHeadlessApps.contains(desc.id()))
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

    // We set the application id as application name so that locally stored things for
    // each application are separated and remain after the application was stopped.
    QCoreApplication::setApplicationName(desc.id());

    QQuickWebViewExperimental::setFlickableViewportEnabled(desc.flickable());

    QString windowType = "card";
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

} // namespace luna
