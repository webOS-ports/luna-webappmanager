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
#include <QJsonObject>
#include <QJsonDocument>

#include <QtWebKit/private/qquickwebview_p.h>

#include "webappmanager.h"
#include "webappmanagerservice.h"
#include "applicationdescription.h"
#include "webapplication.h"

#include "plugins/baseplugin.h"
#include "plugins/palmsystemplugin.h"
#include "plugins/palmservicebridgeplugin.h"

#include <Settings.h>

namespace luna
{

WebApplication::WebApplication(WebAppManager *manager, const ApplicationDescription& desc, const QString& parameters, const QString& processId) :
    mManager(manager),
    mDescription(desc),
    mProcessId(processId),
    mActivityManagerToken(LSMESSAGE_TOKEN_INVALID),
    mIdentifier(mDescription.id() + "-" + mProcessId),
    mActivityId(-1),
    mReady(false),
    mParameters(parameters)
{
    setTitle(mDescription.title());
    setResizeMode(QQuickView::SizeRootObjectToView);

    rootContext()->setContextProperty("webapp", this);
    setSource(QUrl("qrc:///qml/main.qml"));

    QQuickWebView *webView = rootObject()->findChild<QQuickWebView*>("webView");

    qreal zoomFactor = Settings::LunaSettings()->layoutScale;

    // correct zoom factor for some applications which are not scaled properly (aka
    // the Open webOS core-apps ...)
    if (Settings::LunaSettings()->compatApps.find(desc.id().toStdString()) !=
        Settings::LunaSettings()->compatApps.end())
        zoomFactor = Settings::LunaSettings()->layoutScaleCompat;

    webView->setZoomFactor(zoomFactor);

    createActivity();
    createPlugins();
}

WebApplication::~WebApplication()
{
    destroyActivity();
}

void WebApplication::createPlugins()
{
    createAndInitializePlugin(new PalmSystemPlugin(this));
    createAndInitializePlugin(new PalmServiceBridgePlugin(this));
}

void WebApplication::createAndInitializePlugin(BasePlugin *plugin)
{
    mPlugins.insert(plugin->name(), plugin);
    emit pluginWantsToBeAdded(plugin->name(), plugin);
}

void WebApplication::setActivityId(int activityId)
{
    mActivityId = activityId;
}

bool WebApplication::activityManagerCallback(LSHandle *handle, LSMessage *message, void *user_data)
{
    WebApplication *application = static_cast<WebApplication*>(user_data);

    QString payload(LSMessageGetPayload(message));
    QJsonDocument document = QJsonDocument::fromJson(payload.toUtf8());

    if (!document.isObject()) {
        qWarning("Got malformed json response from activity manager: %s",
                 payload.toUtf8().constData());
        return true;
    }

    QJsonObject rootObject = document.object();

    if (!rootObject.contains("returnValue") || rootObject.value("returnValue").isBool()) {
        qWarning("Got malformed json response from activity manager: %s",
                 payload.toUtf8().constData());
        return true;
    }

    bool returnValue = rootObject.value("returnValue").toBool();
    if (!returnValue) {
        qWarning("Failed to create activity for application %s",
                 application->id().toUtf8().constData());
        return true;
    }

    if (!rootObject.contains("activityId")) {
        qWarning("Got malformed json response from activity manager: %s",
                 payload.toUtf8().constData());
        return true;
    }

    application->setActivityId((int) rootObject.value("activityId").toDouble());

    return true;
}

void WebApplication::createActivity()
{
    if (mActivityManagerToken != LSMESSAGE_TOKEN_INVALID) {
        qWarning("Already registered with activitiy manager for application %s",
                 mDescription.id().toUtf8().constData());
        return;
    }

    LSHandle *privateBus = mManager->service()->privateBus();

    LSError error;
    LSErrorInit(&error);

    QJsonObject activityObject;
    activityObject.insert("name", QJsonValue(mDescription.id()));
    activityObject.insert("description", QJsonValue(mProcessId));

    QJsonObject activityTypeObject;
    activityTypeObject.insert("foreground", QJsonValue(true));

    activityObject.insert("type", QJsonValue(activityTypeObject));

    QJsonObject rootObject;
    rootObject.insert("activity", QJsonValue(activityObject));
    rootObject.insert("subscribe", QJsonValue(true));
    rootObject.insert("start", QJsonValue(true));
    rootObject.insert("replace", QJsonValue(true));

    QJsonDocument document(rootObject);

    if (!LSCallFromApplication(privateBus, "palm://com.palm.activitymanager/create",
                               document.toJson().constData(), mIdentifier.toUtf8().constData(),
                               WebApplication::activityManagerCallback, this,
                               &mActivityManagerToken, &error)) {
        qWarning("Failed to register application %s with activity manager: %s",
                 mDescription.id().toUtf8().constData(), error.message);
        LSErrorFree(&error);
    }
}

void WebApplication::destroyActivity()
{
    if (mActivityManagerToken == LSMESSAGE_TOKEN_INVALID)
        return;

    LSError error;
    LSErrorInit(&error);

    LSHandle *privateBus = mManager->service()->privateBus();

    if (!LSCallCancel(privateBus, mActivityManagerToken, &error)) {
        qWarning("Failed to cancel activity for application %s: %s",
                 mDescription.id().toUtf8().constData(), error.message);
        LSErrorFree(&error);
        return;
    }

    mActivityManagerToken = LSMESSAGE_TOKEN_INVALID;
}

void WebApplication::changeActivityFocus(bool focus)
{
    if (mActivityId < 0)
        return;

    LSError error;
    LSErrorInit(&error);

    QJsonObject rootObject;
    rootObject.insert("activityId", QJsonValue(mActivityId));

    QJsonDocument document(rootObject);

    LSHandle *privateBus = mManager->service()->privateBus();

    QString method = "palm://com.palm.activitymanager/";
    method += focus ? "focus" : "unfocus";

    if (!LSCallFromApplication(privateBus, method.toUtf8().constData(),
                document.toJson().constData(), mIdentifier.toUtf8().constData(),
                0, 0, 0, &error)) {
        qWarning("Failed to %s application %s through activity manager: %s",
                 focus ? "focus" : "unfocus", mDescription.id().toUtf8().constData(),
                 error.message);
        LSErrorFree(&error);
    }
}

void WebApplication::run()
{
    showMaximized();
}

void WebApplication::relaunch(const QString &parameters)
{
    mParameters = parameters;
    executeScript(QString("_webOS.relaunch(\"%1\");").arg(parameters));
}

bool WebApplication::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Close:
        emit closed();
        break;
    case QEvent::FocusIn:
        changeActivityFocus(true);
        break;
    case QEvent::FocusOut:
        changeActivityFocus(false);
        break;
    default:
        break;
    }

    return QQuickWindow::event(event);
}

void WebApplication::executeScript(const QString &script)
{
    emit javaScriptExecNeeded(script);
}

void WebApplication::loadFinished()
{
}

void WebApplication::stagePreparing()
{
    mReady = false;
}

void WebApplication::stageReady()
{
    mReady = true;
    readyChanged();
}

QString WebApplication::id() const
{
    return mDescription.id();
}

QString WebApplication::processId() const
{
    return mProcessId;
}

QUrl WebApplication::url() const
{
    return mDescription.entryPoint();
}

QUrl WebApplication::icon() const
{
    return mDescription.icon();
}

QString WebApplication::identifier() const
{
    return mIdentifier;
}

int WebApplication::activityId() const
{
    return mActivityId;
}

bool WebApplication::ready() const
{
    return mReady;
}

QString WebApplication::parameters() const
{
    return mParameters;
}

bool WebApplication::headless() const
{
    return mDescription.headless();
}

} // namespace luna
