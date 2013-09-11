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
#include <QJsonValue>
#include <QQuickView>
#include "webapplication.h"
#include "palmsystemplugin.h"

namespace luna
{

PalmSystemPlugin::PalmSystemPlugin(WebApplication *application, QObject *parent) :
    BasePlugin("PalmSystem", application, parent),
    mPropertyChangeHandlerCallbackId(0)
{
}

void PalmSystemPlugin::stageReady()
{
    mApplication->stageReady();
}

void PalmSystemPlugin::activate()
{
}

void PalmSystemPlugin::deactivate()
{
}

void PalmSystemPlugin::stagePreparing()
{
    mApplication->stagePreparing();
}

void PalmSystemPlugin::show()
{
    mApplication->show();
}

void PalmSystemPlugin::hide()
{
    mApplication->hide();
}

void PalmSystemPlugin::setWindowProperties(const QString &properties)
{
}

void PalmSystemPlugin::enableFullScreenMode(bool enable)
{
}

void PalmSystemPlugin::addBannerMessage(int id, const QString &msg,
                                        const QString &params, const QString &icon, const QString &soundClass,
                                        const QString &soundFile, int duration, bool doNotSuppress)
{
}

void PalmSystemPlugin::removeBannerMessage(int id)
{
}

void PalmSystemPlugin::clearBannerMessages()
{
}

void PalmSystemPlugin::registerPropertyChangeHandler(int successCallbackId, int errorCallbackId)
{
    mPropertyChangeHandlerCallbackId = successCallbackId;
}

void PalmSystemPlugin::setProperty(const QString &name, const QVariant &value)
{
}

void PalmSystemPlugin::getProperty(int successCallbackId, int errorCallbackId, const QString &name)
{
    if (name == "launchParams") {
        callbackWithoutRemove(successCallbackId, mApplication->parameters());
    }
    else if (name == "identifier") {
        callbackWithoutRemove(successCallbackId, mApplication->id());
    }
    else if (name == "activityId") {
        callbackWithoutRemove(successCallbackId, mApplication->activityId());
    }
}

void PalmSystemPlugin::initializeProperties(int successCallbackId, int errorCallbackId)
{
    QJsonObject rootObj;

    rootObj.insert("launchParams", QJsonValue(mApplication->parameters()));
    rootObj.insert("hasAlphaHole", QJsonValue(false));
    rootObj.insert("locale", QJsonValue(QString("")));
    rootObj.insert("localeRegion", QJsonValue(QString("")));
    rootObj.insert("timeFormat", QJsonValue(QString("")));
    rootObj.insert("timeZone", QJsonValue(QString("")));
    rootObj.insert("isMinimal", QJsonValue(QString("")));
    rootObj.insert("identifier", QJsonValue(mApplication->id()));
    rootObj.insert("version", QJsonValue(QString("")));
    rootObj.insert("screenOrientation", QJsonValue(QString("")));
    rootObj.insert("windowOrientation", QJsonValue(QString("")));
    rootObj.insert("specifiedWindowOrientation", QJsonValue(QString("")));
    rootObj.insert("videoOrientation", QJsonValue(QString("")));
    rootObj.insert("deviceInfo", QJsonValue(QString("{\"modelName\":\"unknown\",\"platformVersion\":\"0.0.0\"}")));
    rootObj.insert("isActivated", QJsonValue(true));
    rootObj.insert("activityId", QJsonValue(mApplication->activityId()));
    rootObj.insert("phoneRegion", QJsonValue(QString("")));

    QJsonDocument document(rootObj);

    callback(successCallbackId, document.toJson());
}

} // namespace luna
