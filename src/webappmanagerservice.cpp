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

#include <QJsonDocument>
#include <QJsonObject>

#include "utils.h"
#include "webapplication.h"
#include "webapplauncher.h"
#include "webappmanagerservice.h"
#include "lunaserviceutils.h"

#define WEBAPPMANAGER_SERVICE_ID    "org.webosports.webappmanager"

namespace luna
{

WebAppManagerService::WebAppManagerService(WebAppLauncher *webAppLauncher, GMainLoop *mainLoop)
    : mWebAppLauncher(webAppLauncher),
      mMainLoop(mainLoop),
      mService(0),
      mPrivateBus(0)
{
    startService();
}

WebAppManagerService::~WebAppManagerService()
{
}

LSHandle* WebAppManagerService::privateBus() const
{
    return mPrivateBus;
}

void WebAppManagerService::startService()
{
    LSError lserror;
    LSErrorInit(&lserror);

    if (!LSRegisterPalmService(WEBAPPMANAGER_SERVICE_ID, &mService, &lserror)) {
        g_warning("Failed to register %s as service", WEBAPPMANAGER_SERVICE_ID);
        goto failed;
    }

    mPrivateBus = LSPalmServiceGetPrivateConnection(mService);
    if (!mPrivateBus) {
        g_warning("Unable to get private bus handle");
        goto failed;
    }

    if (!LSGmainAttach(mPrivateBus, mMainLoop, &lserror)) {
        g_warning("Could not attach private service to our main loop");
        goto failed;
    }

    g_message("Successfully initialized %s service", WEBAPPMANAGER_SERVICE_ID);

    return;

failed:
    if (LSErrorIsSet(&lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    if (mPrivateBus && !LSUnregister(mPrivateBus, &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }
}

} // namespace luna
