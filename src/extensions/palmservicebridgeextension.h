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

#include <baseextension.h>

#include "lunaservicemgr.h"

namespace luna
{

class WebApplicationWindow;

class PalmServiceBridge : public QObject,
                          public LunaServiceManagerListener
{
    Q_OBJECT
public:
    explicit PalmServiceBridge(int instanceId, const QString& identifier = "", bool usePrivateBus = false, QObject *parent = 0);

    void call(const QString &uri, const QString &payload);
    void cancel();

    virtual void serviceResponse(const char* body);

    int instanceId() const;

Q_SIGNALS:
    void callback(const QString &arguments);

private:
    int mInstanceId;
    bool mCanceled;
    bool mUsePrivateBus;
    QString mIdentifier;
    bool mCallActive;
};

class PalmServiceBridgeExtension : public BaseExtension
{
    Q_OBJECT
public:
    explicit PalmServiceBridgeExtension(WebApplicationWindow *applicationWindow, QObject *parent = 0);

public Q_SLOTS:
    void createInstance(unsigned int instanceId);
    void releaseInstance(unsigned int instanceId);
    void call(unsigned int instanceId, const QString &uri, const QString &payload);
    void cancel(unsigned int instanceId);

private Q_SLOTS:
    void callbackFromBridge(const QString &arguments);

private:
    QMap<unsigned int, PalmServiceBridge*> mBridgeInstances;
    WebApplicationWindow *mApplicationWindow;

    bool isPrivilegedApplcation(const QString& id);
};

} // namespace luna

#endif // PALMSERVICEBRIDGEPLUGIN_H
