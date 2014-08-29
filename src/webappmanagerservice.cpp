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
#include <QJsonArray>

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
 * - \ref org_webosports_webappmanager_launch_url
 * - \ref org_webosports_webappmanager_kill_app
 * - \ref org_webosports_webappmanager_is_app_running
 * - \ref org_webosports_webappmanager_list_running_apps
 */

WebAppManagerService::WebAppManagerService(WebAppManager *webAppManager)
    : LS::Handle(LS::registerService(WEBAPPMANAGER_SERVICE_ID, false)),
      mWebAppManager(webAppManager)
{
    attachToLoop(g_main_loop_new(g_main_context_default(), FALSE));

    LS_CATEGORY_BEGIN(WebAppManagerService, "/")
        LS_CATEGORY_METHOD(launchApp)
        LS_CATEGORY_METHOD(launchUrl)
        LS_CATEGORY_METHOD(killApp)
        LS_CATEGORY_METHOD(isAppRunning)
        LS_CATEGORY_METHOD(listRunningApps)
        LS_CATEGORY_METHOD(registerForAppEvents)
        LS_CATEGORY_METHOD(relaunch)
    LS_CATEGORY_END

    mAppEventSubscriptions.setServiceHandle(this);
}

WebAppManagerService::~WebAppManagerService()
{
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
bool WebAppManagerService::launchApp(LSMessage &message)
{
    LS::Message request(&message);

    QByteArray payload(request.getPayload());
    if (payload.isEmpty()) {
        request.respond("{\"returnValue\":false,\"errorText\":\"Bad JSON\"}");
        return true;
    }

    QJsonDocument requestDocument = QJsonDocument::fromJson(payload);
    if (!requestDocument.isObject()) {
        request.respond("{\"returnValue\":false,\"errorText\":\"Bad JSON\"}");
        return true;
    }

    QJsonObject rootObject = requestDocument.object();
    if (!(rootObject.contains("appDesc") && rootObject.value("appDesc").isObject())) {
        request.respond("{\"returnValue\":false,\"errorText\":\"No application description provided\"}");
        return true;
    }

    if (!rootObject.contains("processId")) {
        request.respond("{\"returnValue\":false,\"errorText\":\"No process id provided\"}");
        return true;
    }

    QString appDesc = jsonObjectToString(rootObject.value("appDesc").toObject());
    QString params = "";

    if (rootObject.contains("params")) {
        if (rootObject.value("params").isObject())
            params = jsonObjectToString(rootObject.value("params").toObject());
        else
            params = rootObject.value("params").toString();
    }

    int processId = rootObject.value("processId").toInt();

    WebApplication *app = mWebAppManager->launchApp(appDesc, params, processId);

    QJsonObject response;

    response.insert("returnValue", QJsonValue(app != 0));

    if (!app)
        response.insert("errorText", QJsonValue(QString("Failed to launch application")));
    else
        response.insert("processId", QJsonValue(app->processId()));

    response.insert("returnValue", QJsonValue(app != 0));

    QJsonDocument responseDocument(response);

    request.respond(responseDocument.toJson().constData());

    return true;
}

bool WebAppManagerService::launchUrl(LSMessage &message)
{
    LS::Message request(&message);

    QByteArray payload(request.getPayload());
    if (payload.isEmpty()) {
        request.respond("{\"returnValue\":false,\"errorText\":\"Bad JSON\"}");
        return true;
    }

    QJsonDocument requestDocument = QJsonDocument::fromJson(payload);
    if (!requestDocument.isObject()) {
        request.respond("{\"returnValue\":false,\"errorText\":\"Bad JSON\"}");
        return true;
    }

    QJsonObject rootObject = requestDocument.object();

    if (!(rootObject.contains("url") && rootObject.value("url").isString())) {
        request.respond("{\"returnValue\":false,\"errorText\":\"No URL to launch provided\"}");
        return true;
    }

    if (!rootObject.contains("processId")) {
        request.respond("{\"returnValue\":false,\"errorText\":\"No process id provided\"}");
        return true;
    }

    QUrl url(rootObject.value("url").toString());

    QString windowType = "card";
    if (rootObject.contains("windowType") && rootObject.value("windowType").isString())
        windowType = rootObject.value("windowType").toString();

    QString appDesc = "";
    if (rootObject.contains("appDesc") && rootObject.value("appDesc").isObject())
        appDesc = jsonObjectToString(rootObject.value("appDesc").toObject());

    QString params = "";
    if (rootObject.contains("params") && rootObject.value("params").isObject())
        params = jsonObjectToString(rootObject.value("params").toObject());

    int processId = rootObject.value("processId").toInt();

    WebApplication *app = mWebAppManager->launchUrl(url, windowType, appDesc, params, processId);

    QJsonObject response;

    response.insert("returnValue", QJsonValue(app != 0));

    if (!app)
        response.insert("errorText", QJsonValue(QString("Failed to launch application")));
    else
        response.insert("processId", QJsonValue(app->processId()));

    response.insert("returnValue", QJsonValue(app != 0));

    QJsonDocument responseDocument(response);

    request.respond(responseDocument.toJson().constData());

    return true;
}

bool WebAppManagerService::killApp(LSMessage &message)
{
    LS::Message request(&message);

    QJsonDocument document = QJsonDocument::fromJson(request.getPayload());

    QJsonObject root = document.object();

    if (root.contains("processId")) {
        int64_t processId = root.value("processId").toInt();
        mWebAppManager->killApp(processId);
    }
    else if (root.contains("appId")) {
        QString appId = root.value("appId").toString();
        mWebAppManager->killApp(appId);
    }
    else {
        request.respond("\"returnValue\":false,\"errorText\":\"Missing appId or processId parameter\"}");
        return true;
    }

    request.respond("{\"returnValue\":true}");

    return true;
}

bool WebAppManagerService::listRunningApps(LSMessage &message)
{
    LS::Message request(&message);

    QJsonObject rootObj;

    QJsonArray runningApps;
    Q_FOREACH(WebApplication *app, mWebAppManager->applications()) {
        QJsonObject appObj;
        appObj.insert("appId", app->id());
        appObj.insert("processId", app->processId());
        runningApps.append(QJsonValue(appObj));
    }

    rootObj.insert("apps", runningApps);

    QJsonDocument document(rootObj);

    request.respond(document.toJson().constData());

    return true;
}

bool WebAppManagerService::isAppRunning(LSMessage &message)
{
    LS::Message request(&message);

    QJsonDocument document = QJsonDocument::fromJson(QByteArray(request.getPayload()));

    QJsonObject root = document.object();

    if (!root.contains("appId")) {
        request.respond("{\"returnValue\":false,\"errorText\":\"Missing appId parameter\"}");
        return true;
    }

    QString appId = root.value("appId").toString();

    bool running = mWebAppManager->isAppRunning(appId);
    QString response = QString("{\"returnValue\":true,\"running\":%1}").arg(running ? "true" : "false");

    request.respond(response.toUtf8().constData());

    return true;
}

bool WebAppManagerService::registerForAppEvents(LSMessage &message)
{
    LS::Message request(&message);

    if (!request.isSubscription()) {
        request.respond("{\"returnValue\":false,\"errorText\":\"You can only subscribe to this method\"}");
        return true;
    }

    mAppEventSubscriptions.subscribe(request);

    request.respond("{\"returnValue\":true}");

    return true;
}

void WebAppManagerService::notifyAppHasStarted(const QString &appId, int64_t processId)
{
    QString payload = QString("{\"event\":\"start\",\"appId\":\"%1\",\"processId\":%2}")
                        .arg(appId)
                        .arg(processId);

    mAppEventSubscriptions.post(payload.toUtf8().constData());
}

void WebAppManagerService::notifyAppHasFinished(const QString &appId, int64_t processId)
{
    QString payload = QString("{\"event\":\"close\",\"appId\":\"%1\",\"processId\":%2}")
                        .arg(appId)
                        .arg(processId);

    mAppEventSubscriptions.post(payload.toUtf8().constData());
}

bool WebAppManagerService::relaunch(LSMessage &message)
{
    LS::Message request(&message);

    QJsonDocument document = QJsonDocument::fromJson(QByteArray(request.getPayload()));

    QJsonObject root = document.object();

    if (!root.contains("appId")) {
        request.respond("{\"returnValue\":false,\"errorText\":\"Missing appId parameter\"}");
        return true;
    }

    QString appId = root.value("appId").toString();

    QString params = "{}";
    if (root.contains("params") && root.value("params").isString())
        params = root.value("params").toString();

    bool success = mWebAppManager->relaunch(appId, params);
    if (!success)
        request.respond("{\"returnValue\":false,\"errorText\":\"Failed to relaunch application\"}");
    else
        request.respond("{\"returnValue\":true}");

    return true;
}

} // namespace luna
