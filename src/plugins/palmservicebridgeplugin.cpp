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
#include <QtGlobal>

#include "webapplication.h"
#include "webapplicationwindow.h"
#include "palmservicebridgeplugin.h"

namespace luna
{

PalmServiceBridge::PalmServiceBridge(const QString& identifier, bool usePrivateBus, QObject *parent) :
    QObject(parent),
    mCanceled(false),
    mUsePrivateBus(usePrivateBus),
    mIdentifier(identifier),
    mSuccessCallbackId(0),
    mErrorCallbackId(0),
    mCallActive(false)
{
}

void PalmServiceBridge::serviceResponse(const char *body)
{
    QString arguments = QString("'%1'").arg((body == NULL ? "" : body));
    callback(mSuccessCallbackId, arguments);
    mCallActive = false;
}

void PalmServiceBridge::call(int successCallbackId, int errorCallbackId, const QString &uri, const QString &payload)
{
    if (mCallActive) {
        callback(errorCallbackId, "");
        mCallActive = false;
        return;
    }

    LunaServiceManager *mgr = LunaServiceManager::instance();

    mCanceled = false;
    mSuccessCallbackId = successCallbackId;
    mErrorCallbackId = errorCallbackId;

    mgr->call(uri.toUtf8().constData(), payload.toUtf8().constData(),
              this, mIdentifier.toUtf8().constData(), mUsePrivateBus);

    if (LSMESSAGE_TOKEN_INVALID == listenerToken) {
        cancel(0, 0);
        callback(errorCallbackId, "");
        mCallActive = false;
        return;
    }
}

void PalmServiceBridge::cancel(int successCallbackId, int errorCallbackId)
{
    Q_UNUSED(successCallbackId);
    Q_UNUSED(errorCallbackId);

    if (mCanceled)
        return;

    mCanceled = true;
    if (listenerToken)
        LunaServiceManager::instance()->cancel(this);

    mCallActive = false;
}

PalmServiceBridgePlugin::PalmServiceBridgePlugin(WebApplicationWindow *applicationWindow, QObject *parent) :
    BasePlugin("PalmServiceBridge", applicationWindow, parent)
{
}

bool PalmServiceBridgePlugin::isPrivilegedApplcation(const QString& id)
{
    return id.startsWith("com.palm.") ||
           id.startsWith("com.webos.") ||
           id.startsWith("org.webosports.") ||
           id.startsWith("org.webosinternals.");
}

void PalmServiceBridgePlugin::createInstance(int successCallbackId, int errorCallbackId, unsigned int instanceId)
{
    if (mBridgeInstances.contains(instanceId)) {
        callback(errorCallbackId, "Can't create another instance with an already existing id");
        return;
    }

    PalmServiceBridge *bridge = new PalmServiceBridge(mApplicationWindow->application()->id(),
                                                      isPrivilegedApplcation(mApplicationWindow->application()->id()));
    connect(bridge, SIGNAL(callback(int,QString)), this, SLOT(callbackFromBridge(int,QString)));
    mBridgeInstances.insert(instanceId, bridge);
}

void PalmServiceBridgePlugin::releaseInstance(int successCallbackId, int errorCallbackId, unsigned int instanceId)
{
    Q_UNUSED(successCallbackId);
    Q_UNUSED(errorCallbackId);

    if (!mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = mBridgeInstances.take(instanceId);
    bridge->deleteLater();
}

void PalmServiceBridgePlugin::callbackFromBridge(int id, const QString &arguments)
{
    callback(id, arguments);
}

void PalmServiceBridgePlugin::call(int successCallbackId, int errorCallbackId, unsigned int instanceId,
                                   const QString& uri, const QString& payload)
{
    if (!mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = mBridgeInstances.value(instanceId);
    bridge->call(successCallbackId, errorCallbackId, uri, payload);
}

void PalmServiceBridgePlugin::cancel(int successCallbackId, int errorCallbackId, unsigned int instanceId)
{
    if (!mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = mBridgeInstances.value(instanceId);
    bridge->cancel(successCallbackId, errorCallbackId);
}

} // namespace luna
