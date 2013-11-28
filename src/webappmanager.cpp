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

#include <QtWebKit/private/qquickwebview_p.h>

#include "applicationdescription.h"
#include "webappmanager.h"
#include "webappmanagerservice.h"
#include "webapplication.h"

namespace luna
{

WebAppManager::WebAppManager(int &argc, char **argv)
    : QGuiApplication(argc, argv),
      mMainLoop(0),
      mService(0),
      mNextProcessId(1000)
{
    setApplicationName("WebAppMgr");
    setQuitOnLastWindowClosed(false);

    mMainLoop = g_main_loop_new(g_main_context_default(), TRUE);

    mService = new WebAppManagerService(this, mMainLoop);

    QQuickWebViewExperimental::setFlickableViewportEnabled(false);
}

WebAppManager::~WebAppManager()
{
    delete mService;
    g_main_loop_unref(mMainLoop);
}

WebAppManagerService* WebAppManager::service() const
{
    return mService;
}

bool WebAppManager::validateApplication(const ApplicationDescription& desc)
{
    if (desc.id().length() == 0)
        return false;

    if (desc.entryPoint().isLocalFile() && !QFile::exists(desc.entryPoint().toLocalFile()))
        return false;

    return true;
}

WebApplication* WebAppManager::launchApp(const QString &appDesc, const QString &parameters)
{
    ApplicationDescription desc(appDesc);

    if (!validateApplication(desc)) {
        qWarning("Got invalid application description for app %s",
                 desc.id().toUtf8().constData());
        return 0;
    }

    if (mApplications.contains(desc.id())) {
        WebApplication *application = mApplications.value(desc.id());
        application->relaunch(parameters);
        return application;
    }

    QString processId = QString("%0").arg(mNextProcessId++);
    QString windowType = "card";
    QUrl entryPoint = desc.entryPoint();
    WebApplication *app = new WebApplication(this, entryPoint, windowType,
                                             desc, parameters, processId);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationWindowClosed()));

    // FIXME revisit wether we allow only one instance per application (e.g. whats
    // with multiple windows per application?)
    mApplications.insert(app->id(), app);

    return app;
}

WebApplication* WebAppManager::launchUrl(const QUrl &url, const QString &windowType,
                                         const QString &appDesc, const QString &parameters)
{
    ApplicationDescription desc(appDesc);

    if (!validateApplication(desc)) {
        qWarning("Got invalid application description for app %s",
                 desc.id().toUtf8().constData());
        return 0;
    }

    if (mApplications.contains(desc.id())) {
        WebApplication *application = mApplications.value(desc.id());
        application->relaunch(parameters);
        return application;
    }

    QString processId = QString("%0").arg(mNextProcessId++);
    WebApplication *app = new WebApplication(this, url, windowType, desc, parameters, processId);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationWindowClosed()));

    // FIXME revisit wether we allow only one instance per application (e.g. whats
    // with multiple windows per application?)
    mApplications.insert(app->id(), app);

    return app;
}

void WebAppManager::onApplicationWindowClosed()
{
    WebApplication *app = static_cast<WebApplication*>(sender());

    if (!mApplications.contains(app->id())) {
        qWarning("Got close event from not running application!?");
        return;
    }

    mApplications.remove(app->id());

    qDebug() << "Application" << app->id() << "was closed";
    delete app;
}

} // namespace luna
