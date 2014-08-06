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

#include <time.h>

#include <luna-service2++/message.hpp>

#include "systemtime.h"

namespace luna
{

SystemTime* SystemTime::instance()
{
    static SystemTime* instance = 0;

    if (!instance)
        instance = new SystemTime();

    return instance;
}

SystemTime::SystemTime() :
    mLunaPrivHandle(NULL, false)
{
    qDebug() << __PRETTY_FUNCTION__ << "Registering for system time changes ...";

    mLunaPrivHandle.attachToLoop(g_main_context_default());

    LS::ServerStatusCallback callback = [&] (bool isActive) {
        if (!isActive)
            return true;

        mSubscriptionCall = mLunaPrivHandle.callMultiReply("luna://com.palm.systemservice/time/getSystemTime",
                                                       "{\"subscribe\":true}");
        mSubscriptionCall.continueWith(updateCallback, this);

        return true;
    };

    mServerStatus = mLunaPrivHandle.registerServerStatus("com.palm.systemservice", callback);
}

QString SystemTime::timezone() const
{
    return mTimezone;
}

bool SystemTime::updateCallback(LSHandle *handle, LSMessage *message, void *context)
{
    SystemTime *instance = static_cast<SystemTime*>(context);
    instance->updateFromService(message);
    return true;
}

void SystemTime::updateFromService(LSMessage *message)
{
    LS::Message msg{message};

    QJsonDocument document = QJsonDocument::fromJson(QByteArray(msg.getPayload()));

    if (!document.isObject())
        return;

    QJsonObject root = document.object();

    if (root.contains("timezone")) {
        QString timezone = root.value("timezone").toString("");
        if (timezone != mTimezone) {
            mTimezone = timezone;

            setenv("TZ", mTimezone.toUtf8().constData(), 1);
            tzset();

            qDebug() << __PRETTY_FUNCTION__ << "timezone has changed to" << mTimezone;
        }
    }
}

} // namespace luna
