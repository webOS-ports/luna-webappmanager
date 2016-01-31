/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2015 Nikolay Nizov <nizovn@gmail.com>
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

#ifndef BLUTOOTHMANAGER_H
#define BLUTOOTHMANAGER_H

#include <QObject>
#include <QList>
#include <networkmanager.h>
#include <networktechnology.h>
#include <networkservice.h>
#include <useragent.h>
#include <baseextension.h>
#include "bluetooth/bluetooth.h"

typedef QPair<int,int> CallbackHandle;

class BluetoothManager : public luna::BaseExtension
{
    Q_OBJECT
public:
    explicit BluetoothManager(luna::ApplicationEnvironment *environment, QObject *parent = 0);
    ~BluetoothManager();

    void initialize();

public Q_SLOTS:
    void setPowered(bool powered);
    void discover(bool value);
    void connectDevice(const QString &address);
    void disconnectDevice(const QString &address);
    void removeDevice(const QString &address);
    void resetDevicesList();

    void providePinCode(uint tag, bool provided, const QString &code);
    void providePasskey(uint tag, bool provided, const uint passkey);
    void confirmPasskey(uint tag, bool confirmed);
    void displayPasskeyCallback(uint tag);

private Q_SLOTS:
    void technologiesChanged();
    void deviceFound(QSharedPointer<Device> &device);
    void deviceChanged(QSharedPointer<Device> &device);
    void deviceRemoved(QString address);
    void deviceDisappeared(QString address);
    void propertyChanged(const QString &key, const QVariant &value);

    void pinCodeNeeded(int tag, Device* device);
    void passkeyNeeded(int tag, Device* device);
    void passkeyConfirmationNeeded(int tag, Device* device, QString passkey);
    void displayPasskeyNeeded(int tag, Device* device, QString passkey, ushort entered);
    void pairingDone();

private:
    NetworkManager *mManager;
    NetworkTechnology *mTechnology;
    Bluetooth *mBluetooth;
    Agent *mBtAgent;
};

#endif // BLUETOOTHMANAGER_H
