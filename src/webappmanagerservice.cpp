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
#include "webapplication.h"
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

/*!
\page org_webosports_webappmanager
\n
\section org_webosports_webappmanager_launch_app launchApp

\e Private

org.webosports.webappmanager/launchApp

Launch an web application.

\subsection org_webosports_webappmanager_launch_app_syntax Syntax:
\code
{
    "appDesc": string,
    "params": string,
    "launchingAppId": string,
    "launchingProcId": string
}
\endcode

\param appDesc Application description
\param params Application parameters
\param launchingAppId Application id of the application launching the new one

\subsection org_webosports_webappmanager_launch_app_returns Returns:
\code
{
    "returnValue": boolean,
    "errorText": string,
    "processId": string
}
\endcode

\param returnValue Indicates if the call was successful.
\param errorText Describes the error if call was not successful.
\param processId Id of the new application process

\subsection org_webosports_webappmanager_launch_app_examples Examples:
\code
luna-send -n 1 luna-send -n 1 palm://org.webosports.webappmanager/launchApp '{"appDesc":{"title":"Memos","icon":"","noWindow":false,"main":"/usr/palm/applications/org.webosports.app.memos/index.html","id":"org.webosports.app.memos"}}'
\endcode

Example response of a successful call:
\code
{
    "returnValue": true,
    "processId": "1001"
}
\endcode

Example response for a failed call:
\code
{
    "returnValue": false,
    "errorText": "Can't find entry point for app"
}
\endcode
*/
bool WebAppManagerService::onLaunchApp(LSHandle *handle, LSMessage *message)
{
    QByteArray payload(LSMessageGetPayload(message));
    if (payload.isEmpty()) {
        qWarning("No payload provided");
        luna_service_message_reply_error_bad_json(handle, message);
        return true;
    }

    QJsonDocument requestDocument = QJsonDocument::fromJson(payload);
    if (!requestDocument.isObject()) {
        luna_service_message_reply_error_bad_json(handle, message);
        return true;
    }

    QJsonObject rootObject = requestDocument.object();
    if (!(rootObject.contains("appDesc") && rootObject.value("appDesc").isObject())) {
        luna_service_message_reply_error_bad_json(handle, message);
        return true;
    }

    QString appDesc = jsonObjectToString(rootObject.value("appDesc").toObject());
    QString params = "";

    if (rootObject.contains("params") && rootObject.value("params").isObject()) {
        params = jsonObjectToString(rootObject.value("params").toObject());
    }

    WebApplication *app = mWebAppManager->launchApp(appDesc, params);

    QJsonObject response;

    if (app)
        response.insert("processId", QJsonValue(app->processId()));

    response.insert("returnValue", QJsonValue(app != 0));

    if (!app)
        response.insert("errorText", QJsonValue(QString("Failed to launch application")));

    QJsonDocument responseDocument(response);

    LSError error;
    LSErrorInit(&error);

    if (!LSMessageReply(handle, message, responseDocument.toJson().constData(), &error)) {
        LSErrorPrint(&error, stderr);
        LSErrorFree(&error);
    }

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
