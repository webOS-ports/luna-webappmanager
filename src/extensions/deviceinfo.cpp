/* @@@LICENSE
 *
 *      Copyright (c) 2009-2013 LG Electronics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * LICENSE@@@ */

#include "deviceinfo.h"

#include <glib.h>
#include <lunaprefs.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <Settings.h>

static DeviceInfo* s_instance = 0;
static const int kTouchableHeight = 48;

DeviceInfo* DeviceInfo::instance()
{
    if (G_UNLIKELY(s_instance == 0))
        new DeviceInfo;

    return s_instance;
}

DeviceInfo::DeviceInfo()
{
    s_instance = this;

    gatherInfo();
}

DeviceInfo::~DeviceInfo()
{
    s_instance = 0;
}

QString DeviceInfo::jsonString() const
{
    return m_jsonString;
}

static bool getLunaPrefSystemValue(const char* key, std::string& value)
{
    char* str = 0;
    if (LP_ERR_NONE == LPSystemCopyStringValue(key, &str) && str) {
        value = str;
        g_free((gchar*) str);
        return true;
    }

#if 0
    value = LOCALIZED("Unknown");
#else
    value = "unknown";
#endif
    return false;
}


void DeviceInfo::gatherInfo()
{
    // Platform Info --------------------------------------------------------------
    getLunaPrefSystemValue("com.palm.properties.ProdSN", m_serialNumber);
    getLunaPrefSystemValue("com.palm.properties.DMCARRIER", m_carrierName);

    // If getLunaPrefSystemValue fails, such as luna-prefs not yet built for the platform, return
    // a sane version number for applications
    if (!getLunaPrefSystemValue("com.palm.properties.version", m_platformVersion))
    {
        // This should only be returned if the luna-prefs package is not working, or
        // the luna-prefs system version has not been set.
        m_platformVersion = "3.5.0";
    }

    std::string platformVersion = m_platformVersion;

    size_t npos1 = 0, npos2 = 0;
    npos1 = platformVersion.find_first_of ('.');
    if (npos1 != std::string::npos && npos1 <= platformVersion.size() - 1)
        npos2 = platformVersion.find_first_of ('.', npos1 + 1);
    if (npos1 == std::string::npos || npos2 == std::string::npos)  {
        m_platformVersionMajor = m_platformVersionMinor = m_platformVersionDot = -1;
    }
    else {
        m_platformVersionMajor = atoi ((platformVersion.substr (0, npos1)).c_str());
        m_platformVersionMinor = atoi ((platformVersion.substr (npos1+1, npos2)).c_str());
        m_platformVersionDot = atoi ((platformVersion.substr (npos2+1)).c_str());
    }

    // Device model info ----------------------------------------------------------

    if (Settings::LunaSettings()->hardwareType == Settings::HardwareTypeDesktop) {
        m_modelName = "Desktop";
        m_modelNameAscii = "Desktop";
        m_keyboardAvailable = true;
        m_keyboardSlider = false;
        m_coreNaviButton = true;
        m_keyboardType = "QWERTY";
        m_swappableBattery = false;
    }
    else {
        if (!getLunaPrefSystemValue("com.palm.properties.deviceNameShortBranded", m_modelName))
            m_modelName = "Lune OS smartphone";

        if (!getLunaPrefSystemValue("com.palm.properties.deviceNameShort", m_modelNameAscii))
            m_modelNameAscii = "Lune OS smartphone";

        m_keyboardSlider = false;
        m_coreNaviButton = false;
        m_keyboardAvailable = false;
        m_swappableBattery = false;
    }

    // WIFI and bluetooth ---------------------------------------------------------

    std::string dummy;

    if (getLunaPrefSystemValue("com.palm.properties.WIFIoADDR", dummy) && dummy.length() > 0)
        m_wifiAvailable = true;
    else
        m_wifiAvailable = false;

    if (getLunaPrefSystemValue("com.palm.properties.BToADDR", dummy) && dummy.length() > 0)
        m_bluetoothAvailable = true;
    else
        m_bluetoothAvailable = false;


    // Compose json string from the parameters  -------------------------------

    QJsonObject root;

    root.insert("modelName", QJsonValue(QString::fromStdString(m_modelName)));
    root.insert("modelNameAscii", QJsonValue(QString::fromStdString(m_modelNameAscii)));
    root.insert("platformVersion", QJsonValue(QString::fromStdString(m_platformVersion)));
    root.insert("platformVersionMajor", QJsonValue((int) m_platformVersionMajor));
    root.insert("platformVersionMinor", QJsonValue((int) m_platformVersionMinor));
    root.insert("platformVersionDot", QJsonValue((int) m_platformVersionDot));
    root.insert("carrierName", QJsonValue(QString::fromStdString(m_carrierName)));
    root.insert("serialNumber", QJsonValue(QString::fromStdString(m_serialNumber)));

    root.insert("wifiAvailable", QJsonValue(m_wifiAvailable));
    root.insert("bluetoothAvailable", QJsonValue(m_bluetoothAvailable));
    root.insert("carrierAvailable", QJsonValue(carrierAvailable()));

    root.insert("swappableBattery", QJsonValue(m_swappableBattery));
    root.insert("dockModeEnabled", QJsonValue(true));

    QJsonDocument document(root);

    m_jsonString = document.toJson();
}

bool DeviceInfo::keyboardSlider() const
{
    return m_keyboardSlider;
}

bool DeviceInfo::coreNaviButton() const
{
    return m_coreNaviButton;
}

unsigned int DeviceInfo::platformVersionMajor() const
{
    return m_platformVersionMajor;
}

unsigned int DeviceInfo::platformVersionMinor() const
{
    return m_platformVersionMinor;
}

unsigned int DeviceInfo::platformVersionDot() const
{
    return m_platformVersionDot;
}
