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
#include <QStringList>
#include <systemd/sd-daemon.h>

#include "webappmanager.h"

#define XDG_RUNTIME_DIR_DEFAULT "/tmp/luna-session"

int main(int argc, char **argv)
{
    if (qgetenv("DISPLAY").isEmpty()) {
        setenv("EGL_PLATFORM", "wayland", 1);
        setenv("QT_QPA_PLATFORM", "wayland", 1);
        setenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1", 1);
        setenv("XDG_RUNTIME_DIR", XDG_RUNTIME_DIR_DEFAULT, 0);
        setenv("QT_IM_MODULE", "Maliit", 0);
    }

    luna::WebAppManager webAppManager(argc, argv);

    if (webAppManager.arguments().indexOf("--debug") >= 0)
        setenv("QTWEBKIT_INSPECTOR_SERVER", "1122", 0);

    if (webAppManager.arguments().indexOf("--systemd") >= 0)
        sd_notify(0, "READY=1");

    return webAppManager.exec();
}
