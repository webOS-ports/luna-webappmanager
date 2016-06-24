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

#ifndef WEBAPPLICATIONREDIRECTHANDLER_H
#define WEBAPPLICATIONREDIRECTHANDLER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QWebEngineUrlSchemeHandler>

#include <luna-service2++/handle.hpp>

namespace luna
{

class WebApplicationRedirectHandler : public QWebEngineUrlSchemeHandler
{
public:
    WebApplicationRedirectHandler(const QString &appId);
    virtual ~WebApplicationRedirectHandler();

    virtual void requestStarted(QWebEngineUrlRequestJob *request);
private:
    QString mAppId;
    LS::Handle mLunaPubHandle;
};

} // namespace luna

#endif // WEBAPPLICATIONREDIRECTHANDLER_H
