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
#include <QJsonDocument>
#include <QJsonObject>

#include "utils.h"
#include "webappmanager.h"
#include "webappmanagerservice.h"
#include "lunaserviceutils.h"

#define WEBAPPMANAGER_SERVICE_ID    "org.webosports.webappmanager"

namespace luna
{

/*! \page org_webosports_webappmanager Service API org.webosports.webappmanager
 *
 * Public methods:
 * - \ref org_webosports_webappmanager_launch_app
 * - \ref org_webosports_webappmanager_kill_app
 * - \ref org_webosports_webappmanager_is_app_running
 * - \ref org_webosports_webappmanager_list_running_apps
 */
static LSMethod privateServiceMethods[] = {
    { "launchApp", WebAppManagerService::onLaunchAppCb },
    { "killApp", WebAppManagerService::onKillAppCb },
    { "isAppRunning", WebAppManagerService::onIsAppRunningCb },
    { "listRunningApps", WebAppManagerService::onListRunningAppsCb },
    { },
};

WebAppManagerService::WebAppManagerService(WebAppManager *webAppManager, GMainLoop *mainLoop)
    : mWebAppManager(webAppManager),
      mMainLoop(mainLoop),
      mService(0),
      mPrivateBus(0)
{
    startService();
}

WebAppManagerService::~WebAppManagerService()
{
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

    if (!LSRegisterCategory(mPrivateBus, "/", privateServiceMethods, NULL, NULL, &lserror)) {
        g_warning("Failed to register category / on private bus");
        goto failed;
    }

    if (!LSCategorySetData(mPrivateBus, "/", this, &lserror)) {
        g_warning("Failed to set category data for private bus");
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

bool WebAppManagerService::onLaunchAppCb(LSHandle *handle, LSMessage *message, void *data)
{
    WebAppManagerService *service = static_cast<WebAppManagerService*>(data);
    return service->onLaunchApp(handle, message);
}

bool WebAppManagerService::onLaunchApp(LSHandle *handle, LSMessage *message)
{
    QByteArray payload(LSMessageGetPayload(message));
    if (payload.isEmpty()) {
        qWarning("No payload provided");
        luna_service_message_reply_error_bad_json(handle, message);
        return true;
    }

    QJsonDocument document = QJsonDocument::fromJson(payload);
    if (!document.isObject()) {
        luna_service_message_reply_error_bad_json(handle, message);
        return true;
    }

    QJsonObject rootObject = document.object();
    if (!(rootObject.contains("appDesc") && rootObject.value("appDesc").isObject())) {
        luna_service_message_reply_error_bad_json(handle, message);
        return true;
    }

    QString appDesc = jsonObjectToString(rootObject.value("appDesc").toObject());
    QString params = "";

    if (rootObject.contains("params") && rootObject.value("params").isObject()) {
        params = jsonObjectToString(rootObject.value("params").toObject());
    }

    mWebAppManager->launchApp(appDesc, params);

    luna_service_message_reply_success(handle, message);

    return true;
}

bool WebAppManagerService::onKillAppCb(LSHandle *handle, LSMessage *message, void *data)
{
    luna_service_message_reply_error_not_implemented(handle, message);
    return true;
}

bool WebAppManagerService::onListRunningAppsCb(LSHandle *handle, LSMessage *message, void *data)
{
    luna_service_message_reply_error_not_implemented(handle, message);
    return true;
}

bool WebAppManagerService::onIsAppRunningCb(LSHandle *handle, LSMessage *message, void *data)
{
    luna_service_message_reply_error_not_implemented(handle, message);
    return true;
}

} // namespace luna
