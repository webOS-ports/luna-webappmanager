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

#include <applicationenvironment.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDBusAbstractAdaptor>

#include "bluetoothmanager.h"

BluetoothManager::BluetoothManager(luna::ApplicationEnvironment *environment, QObject *parent) :
    luna::BaseExtension("BluetoothManager", environment, parent),
    mManager(0),
    mTechnology(0),
    mBluetooth(0),
    mBtAgent(0)
{
    mManager = NetworkManagerFactory::createInstance();
    connect(mManager, SIGNAL(technologiesChanged()), this, SLOT(technologiesChanged()));

    mTechnology = mManager->getTechnology("bluetooth");
    mBluetooth = new Bluetooth();

    DeviceModel *mDeviceModel = mBluetooth->getDeviceModel();
    connect(mDeviceModel, SIGNAL(deviceFound(QSharedPointer<Device> &)),
            this, SLOT(deviceFound(QSharedPointer<Device> &)));
    connect(mDeviceModel, SIGNAL(deviceChanged(QSharedPointer<Device> &)),
            this, SLOT(deviceChanged(QSharedPointer<Device> &)));
    connect(mDeviceModel, SIGNAL(deviceRemoved(QString)),
            this, SLOT(deviceRemoved(QString)));
    connect(mDeviceModel, SIGNAL(deviceDisappeared(QString)),
            this, SLOT(deviceDisappeared(QString)));
    connect(mDeviceModel, SIGNAL(propertyChanged(const QString &, const QVariant &)),
            this, SLOT(propertyChanged(const QString &, const QVariant &)));

    mBtAgent = mBluetooth->getAgent();
    connect(mBtAgent, SIGNAL(pinCodeNeeded(int, Device*)),
            this, SLOT(pinCodeNeeded(int, Device*)));
    connect(mBtAgent, SIGNAL(passkeyNeeded(int, Device*)),
            this, SLOT(passkeyNeeded(int, Device*)));
    connect(mBtAgent, SIGNAL(passkeyConfirmationNeeded(int, Device*, QString)),
            this, SLOT(passkeyConfirmationNeeded(int, Device*, QString)));
    connect(mBtAgent, SIGNAL(displayPasskeyNeeded(int, Device*, QString, ushort)),
            this, SLOT(displayPasskeyNeeded(int, Device*, QString, ushort)));
    connect(mBtAgent, SIGNAL(pairingDone()),
            this, SLOT(pairingDone()));

    qDebug() << "Registering BluetoothManager extension ...";
    environment->registerUserScript(QUrl("qrc:///extensions/BluetoothManager.js"));
}

void BluetoothManager::initialize()
{
    bool Powered = mTechnology ? mTechnology->powered() : false;
    mAppEnvironment->executeScript(QString("__BluetoothManager.setPowered(%1);").arg(Powered ? "true" : "false"));
}

void BluetoothManager::setPowered(bool powered)
{
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mTechnology->setPowered(powered);
}

void BluetoothManager::discover(bool value)
{
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBluetooth->trySetDiscoverable(value);
    if (value)
        mBluetooth->startDiscovery();
    else
        mBluetooth->stopDiscovery();
}

void BluetoothManager::connectDevice(const QString &address)
{
    qDebug() << __PRETTY_FUNCTION__ << address;
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBluetooth->connectDevice(address);
}

void BluetoothManager::disconnectDevice(const QString &address)
{
    qDebug() << __PRETTY_FUNCTION__ << address;
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBluetooth->setSelectedDevice(address);
    mBluetooth->disconnectDevice();
}

void BluetoothManager::removeDevice(const QString &address)
{
    qDebug() << __PRETTY_FUNCTION__ << address;
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBluetooth->setSelectedDevice(address);
    mBluetooth->removeDevice();
}

void BluetoothManager::providePinCode(uint tag, bool provided, const QString &code)
{
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBtAgent->providePinCode(tag, provided, code);
}

void BluetoothManager::providePasskey(uint tag, bool provided, const uint passkey)
{
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBtAgent->providePasskey(tag, provided, passkey);
}

void BluetoothManager::confirmPasskey(uint tag, bool confirmed)
{
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBtAgent->confirmPasskey(tag, confirmed);
}

void BluetoothManager::displayPasskeyCallback(uint tag)
{
    if (!mTechnology) {
        qDebug() << "Bluetooth is not available";
        return;
    }

    mBtAgent->displayPasskeyCallback(tag);
}

void BluetoothManager::pinCodeNeeded(int tag, Device* device)
{
    qDebug() << __PRETTY_FUNCTION__ << device->getAddress();

    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert("name", QJsonValue(device->getName()));
        btObj.insert("address", QJsonValue(device->getAddress()));
        btObj.insert("type", QJsonValue(device->getType()));
        btObj.insert("tag", QJsonValue(tag));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.requestPinCode(%1);").arg(payload));
}

void BluetoothManager::passkeyNeeded(int tag, Device* device)
{
    qDebug() << __PRETTY_FUNCTION__ << device->getAddress();

    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert("name", QJsonValue(device->getName()));
        btObj.insert("address", QJsonValue(device->getAddress()));
        btObj.insert("type", QJsonValue(device->getType()));
        btObj.insert("tag", QJsonValue(tag));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.requestPasskey(%1);").arg(payload));
}

void BluetoothManager::passkeyConfirmationNeeded(int tag, Device* device, QString passkey)
{
    qDebug() << __PRETTY_FUNCTION__ << device->getAddress();

    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert("name", QJsonValue(device->getName()));
        btObj.insert("address", QJsonValue(device->getAddress()));
        btObj.insert("type", QJsonValue(device->getType()));
        btObj.insert("tag", QJsonValue(tag));
        btObj.insert("passkey", QJsonValue(passkey));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.requestConfirmPasskey(%1);").arg(payload));
}

void BluetoothManager::displayPasskeyNeeded(int tag, Device* device, QString passkey, ushort entered)
{
    qDebug() << __PRETTY_FUNCTION__ << device->getAddress() << entered;

    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert("name", QJsonValue(device->getName()));
        btObj.insert("address", QJsonValue(device->getAddress()));
        btObj.insert("type", QJsonValue(device->getType()));
        btObj.insert("tag", QJsonValue(tag));
        btObj.insert("passkey", QJsonValue(passkey));
        btObj.insert("entered", QJsonValue(entered));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.requestDisplayPasskey(%1);").arg(payload));
}

void BluetoothManager::technologiesChanged()
{
    if (mTechnology && mManager->getTechnology("bluetooth") == NULL) {
        mTechnology = NULL;
        initialize();
    }
    else if (mTechnology == NULL) {
        mTechnology = mManager->getTechnology("bluetooth");
        if (mTechnology) {
            initialize();
        }
    }
}

void BluetoothManager::deviceFound(QSharedPointer<Device> &device)
{
    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert("name", QJsonValue(device->getName()));
        btObj.insert("address", QJsonValue(device->getAddress()));
        btObj.insert("type", QJsonValue(device->getType()));
        btObj.insert("paired", QJsonValue(device->isPaired()));
        btObj.insert("trusted", QJsonValue(device->isTrusted()));
        btObj.insert("connection", QJsonValue(device->getConnection()));
        btObj.insert("strength", QJsonValue(device->getStrength()));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.deviceFound(%1);").arg(payload));
}

void BluetoothManager::deviceChanged(QSharedPointer<Device> &device)
{
    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert("name", QJsonValue(device->getName()));
        btObj.insert("address", QJsonValue(device->getAddress()));
        btObj.insert("type", QJsonValue(device->getType()));
        btObj.insert("paired", QJsonValue(device->isPaired()));
        btObj.insert("trusted", QJsonValue(device->isTrusted()));
        btObj.insert("connection", QJsonValue(device->getConnection()));
        btObj.insert("strength", QJsonValue(device->getStrength()));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.deviceChanged(%1);").arg(payload));
}

void BluetoothManager::deviceRemoved(QString address)
{
    mAppEnvironment->executeScript(QString("__BluetoothManager.deviceRemoved(%1);").arg(address));
}

void BluetoothManager::deviceDisappeared(QString address)
{
    mAppEnvironment->executeScript(QString("__BluetoothManager.deviceDisappeared(%1);").arg(address));
}

void BluetoothManager::propertyChanged(const QString &key, const QVariant &value)
{
    QJsonDocument document;

        QJsonObject btObj;

        btObj.insert(key, QJsonValue::fromVariant(value));

    document.setObject(btObj);
    QString payload = document.toJson();
    mAppEnvironment->executeScript(QString("__BluetoothManager.propertyChanged(%1,%2);").arg(key, payload));
}

void BluetoothManager::pairingDone()
{
    mAppEnvironment->executeScript(QString("__BluetoothManager.pairingDone();"));
}

