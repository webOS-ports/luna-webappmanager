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

#include "../webapplication.h"
#include "../webapplicationwindow.h"
#include "palmservicebridgeextension.h"

namespace luna
{

PalmServiceBridge::PalmServiceBridge(int instanceId, const QString& identifier, bool usePrivateBus, QObject *parent) :
    QObject(parent),
    mInstanceId(instanceId),
    mCanceled(false),
    mUsePrivateBus(usePrivateBus),
    mIdentifier(identifier),
    mCallActive(false)
{
}

void PalmServiceBridge::serviceResponse(const char *body)
{
    QString arguments = QString("'%1'").arg((body == NULL ? "" : body));
    callback(arguments);
    mCallActive = false;
}

void PalmServiceBridge::call(const QString &uri, const QString &payload)
{
    if (mCallActive) {
        callback("");
        mCallActive = false;
        return;
    }

    LunaServiceManager *mgr = LunaServiceManager::instance();

    mCanceled = false;

    mgr->call(uri.toUtf8().constData(), payload.toUtf8().constData(),
              this, mIdentifier.toUtf8().constData(), mUsePrivateBus);

    if (LSMESSAGE_TOKEN_INVALID == listenerToken) {
        cancel();
        callback("");
        mCallActive = false;
        return;
    }
}

void PalmServiceBridge::cancel()
{
    if (mCanceled)
        return;

    mCanceled = true;
    if (listenerToken)
        LunaServiceManager::instance()->cancel(this);

    mCallActive = false;

    callback("");
}

int PalmServiceBridge::instanceId() const
{
    return mInstanceId;
}

PalmServiceBridgeExtension::PalmServiceBridgeExtension(WebApplicationWindow *applicationWindow, QObject *parent) :
    BaseExtension("PalmServiceBridge", applicationWindow, parent),
    mApplicationWindow(applicationWindow)
{
    applicationWindow->registerUserScript(QUrl("qrc:///extensions/PalmServiceBridge.js"));
}

bool PalmServiceBridgeExtension::isPrivilegedApplcation(const QString& id)
{
    return id.startsWith("com.palm.") ||
           id.startsWith("com.webos.") ||
           id.startsWith("org.webosports.");
}

void PalmServiceBridgeExtension::createInstance(unsigned int instanceId)
{
    if (mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = new PalmServiceBridge(instanceId, mApplicationWindow->application()->id(),
                                                      isPrivilegedApplcation(mApplicationWindow->application()->id()));
    connect(bridge, SIGNAL(callback(QString)), this, SLOT(callbackFromBridge(QString)));
    mBridgeInstances.insert(instanceId, bridge);
}

void PalmServiceBridgeExtension::releaseInstance(unsigned int instanceId)
{
    if (!mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = mBridgeInstances.take(instanceId);
    bridge->deleteLater();
}

void PalmServiceBridgeExtension::callbackFromBridge(const QString &arguments)
{
    PalmServiceBridge *bridge = static_cast<PalmServiceBridge*>(sender());
    int instanceId = bridge->instanceId();

    QString command = QString("__PalmServiceBridge_handleServiceResponse(%1, %2);").arg(instanceId).arg(arguments);
    mAppEnvironment->executeScript(command);
}

void PalmServiceBridgeExtension::call(unsigned int instanceId, const QString& uri, const QString& payload)
{
    if (!mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = mBridgeInstances.value(instanceId);
    bridge->call(uri, payload);
}

void PalmServiceBridgeExtension::cancel(unsigned int instanceId)
{
    if (!mBridgeInstances.contains(instanceId))
        return;

    PalmServiceBridge *bridge = mBridgeInstances.value(instanceId);
    bridge->cancel();
}

} // namespace luna
