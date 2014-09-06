/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
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
#include <glib.h>

#include "activity.h"

namespace luna
{

Activity::Activity(const QString& identifier, const QString& appId, const int64_t processId) :
    mHandle(0),
    mToken(LSMESSAGE_TOKEN_INVALID),
    mId(-1),
    mIdentifier(identifier),
    mAppId(appId),
    mProcessId(processId),
    mFocus(false)
{
    setup();
}

Activity::~Activity()
{
    LSError lserror;
    LSErrorInit(&lserror);

    if (mToken != LSMESSAGE_TOKEN_INVALID) {
        if (!LSCallCancel(mHandle, mToken, &lserror)) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }
    }

    if (mHandle) {
        if (LSUnregister(mHandle, &lserror)) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }
    }
}

void Activity::setup()
{
    LSError lserror;
    LSErrorInit(&lserror);

    if (!LSRegister(NULL, &mHandle, NULL)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return;
    }

    GMainLoop *mainloop = g_main_loop_new(g_main_context_default(), TRUE);
    if (!LSGmainAttach(mHandle, mainloop, &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return;
    }

    QJsonObject activity;
    activity.insert("name", mAppId);
    char *description = g_strdup_printf("%i", mProcessId);
    activity.insert("description", (qint64) mProcessId);

    QJsonObject activityType;
    activityType.insert("foreground", true);

    activity.insert("type", activityType);

    QJsonObject request;
    request.insert("activity", activity);
    request.insert("subscribe", true);
    request.insert("start", true);
    request.insert("replace", true);

    QJsonDocument payload(request);

    if (!LSCallFromApplication(mHandle, "palm://com.palm.activitymanager/create", payload.toJson().constData(),
                               mIdentifier.toUtf8().constData(), Activity::activityCallback, this, &mToken, &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return;
    }

    g_free(description);
}

bool Activity::activityCallback(LSHandle *handle, LSMessage *message, void *user_data)
{
    Activity *activity = static_cast<Activity*>(user_data);
    activity->handleActivityResponse(message);
    return true;
}

void Activity::handleActivityResponse(LSMessage *message)
{
    QJsonDocument payload = QJsonDocument::fromJson(QByteArray(LSMessageGetPayload(message)));
    if (!payload.isObject())
        return;

    QJsonObject response = payload.object();

    if (!response.value("returnValue").toBool(false))
        return;

    mId = response.value("activityId").toInt(-1);
}

int Activity::id() const
{
    return mId;
}

void Activity::focus()
{
    if (mFocus || mId == LSMESSAGE_TOKEN_INVALID)
        return;

    LSError lserror;
    LSErrorInit(&lserror);

    QJsonObject request;
    request.insert("activityId", mId);

    QJsonDocument payload(request);

    if (!LSCallFromApplication(mHandle, "palm://com.palm.activitymanager/focus", payload.toJson().constData(),
                               mIdentifier.toUtf8().constData(), 0, 0, 0, &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return;
    }

    mFocus = true;
}

void Activity::unfocus()
{
    if (!mFocus || mId == LSMESSAGE_TOKEN_INVALID)
        return;

    LSError lserror;
    LSErrorInit(&lserror);

    QJsonObject request;
    request.insert("activityId", mId);

    QJsonDocument payload(request);

    if (!LSCallFromApplication(mHandle, "palm://com.palm.activitymanager/unfocus", payload.toJson().constData(),
                               mIdentifier.toUtf8().constData(), 0, 0, 0, &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return;
    }

    mFocus = false;
}

} // namespace luna
