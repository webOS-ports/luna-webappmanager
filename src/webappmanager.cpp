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
      mNextProcessId(1)
{
    setApplicationName("WebAppMgr");
    setQuitOnLastWindowClosed(false);

    mMainLoop = g_main_loop_new(g_main_context_default(), TRUE);

    mService = new WebAppManagerService(this, mMainLoop);
}

WebAppManager::~WebAppManager()
{
    delete mService;
    g_main_loop_unref(mMainLoop);
}

void WebAppManager::launchApp(const QString &appDesc, const QString &arguments)
{
    ApplicationDescription *desc = new ApplicationDescription(appDesc);

    if (mApplications.contains(desc->id())) {
        qWarning("Application %s is already running; preventing "
                 "it from being started again", desc->id().toUtf8().constData());
        desc->deleteLater();
        return;
    }

    WebApplication *app = new WebApplication(desc, mNextProcessId++);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationWindowClosed()));

    qDebug() << "Starting application" << app->id();
    app->run();

    // FIXME revisit wether we allow only one instance per application (e.g. whats
    // with multiple windows per application?)
    mApplications.insert(app->id(), app);
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
    app->deleteLater();
}

} // namespace luna
