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

#ifndef PALMSERVICEBRIDGEPLUGIN_H
#define PALMSERVICEBRIDGEPLUGIN_H

#include <QObject>
#include <QMap>

#include "baseplugin.h"
#include "lunaservicemgr.h"

namespace luna
{

class PalmServiceBridge : public QObject,
                          public LunaServiceManagerListener
{
    Q_OBJECT
public:
    explicit PalmServiceBridge(const QString& identifier = "", bool usePrivateBus = false, QObject *parent = 0);

    void call(int successCallbackId, int errorCallbackId, const QString &uri, const QString &payload);
    void cancel(int successCallbackId, int errorCallbackId);

    virtual void serviceResponse(const char* body);

signals:
    void callback(int id, const QString &arguments);

private:
    bool mCanceled;
    bool mUsePrivateBus;
    QString mIdentifier;
    int mSuccessCallbackId;
    int mErrorCallbackId;
    bool mCallActive;
};

class PalmServiceBridgePlugin : public BasePlugin
{
    Q_OBJECT
public:
    explicit PalmServiceBridgePlugin(WebApplicationWindow *applicationWindow, QObject *parent = 0);

public slots:
    void createInstance(int successCallbackId, int errorCallbackId, unsigned int instanceId);
    void releaseInstance(int successCallbackId, int errorCallbackId, unsigned int instanceId);
    void call(int successCallbackId, int errorCallbackId, unsigned int instanceId, const QString &uri, const QString &payload);
    void cancel(int successCallbackId, int errorCallbackId, unsigned int instanceId);

private slots:
    void callbackFromBridge(int id, const QString &arguments);

private:
    QMap<unsigned int, PalmServiceBridge*> mBridgeInstances;

    bool isPrivilegedApplcation(const QString& id);
};

} // namespace luna

#endif // PALMSERVICEBRIDGEPLUGIN_H
