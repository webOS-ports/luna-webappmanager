/*
 * Copyright (C) 2016 Christophe Chapuis <chris.chapuis@gmail.com>
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

#include <QString>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestJob>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include <luna-service2++/message.hpp>
#include <luna-service2++/call.hpp>

#include "webapplicationredirecthandler.h"

namespace luna
{

WebApplicationRedirectHandler::WebApplicationRedirectHandler(const QString &appId) : QWebEngineUrlSchemeHandler(),
    mAppId(appId),
    mLunaPubHandle(NULL, true)
{
    mLunaPubHandle.attachToLoop(g_main_context_default());
}

WebApplicationRedirectHandler::~WebApplicationRedirectHandler()
{
}

void WebApplicationRedirectHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    QString targetURI = request->requestUrl().toString();
    qDebug() << __PRETTY_FUNCTION__ << "requestStarted for " << targetURI;

    LS::Call call = mLunaPubHandle.callOneReply("luna://com.palm.applicationManager/open",
                                                QString("{\"target\":\"%1\"}").arg(targetURI).toUtf8().constData(),
                                                mAppId.toUtf8().constData());

    request->fail(QWebEngineUrlRequestJob::NoError);
}

}
