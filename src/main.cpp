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

#include "webapplauncher.h"

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

    luna::WebAppLauncher webAppManager(argc, argv);

    QStringList lArgs = webAppManager.arguments();

    if (lArgs.indexOf("--debug") >= 0)
        setenv("QTWEBKIT_INSPECTOR_SERVER", "1122", 0);

    bool correctParameters = true;
    QStringListIterator argsIterator(lArgs);
    argsIterator.next(); // skip program name
    while (correctParameters && argsIterator.hasNext()) {
        const QString option = argsIterator.next();
        QString value;
        if (option == "-a" || option.left(10) == "--appinfo=") {
            if (option == "-a") {
                value = argsIterator.next();
            }
            else {
                value = option.right(option.size()-10);
            }
            if (!value.isEmpty()) {
                webAppManager.setAppDesc(value);
            }
            else {
                correctParameters = false;
            }
        }
        else if (option == "-u" || option.left(6) == "--url=") {
            if (option == "-u") {
                value = argsIterator.next();
            }
            else {
                value = option.right(option.size()-6);
            }
            if (!value.isEmpty()) {
                webAppManager.setUrl(value);
            }
            else {
                correctParameters = false;
            }
        }
        else if (option == "-p" || option.left(13) == "--parameters=") {
            if (option == "-p") {
                value = argsIterator.next();
            }
            else {
                value = option.right(option.size()-6);
            }
            if (!value.isEmpty()) {
                webAppManager.setParameters(value);
            }
            else {
                correctParameters = false;
            }
        }
        else if (option == "-w" || option.left(14) == "--window-type=") {
            if (option == "-w") {
                value = argsIterator.next();
            }
            else {
                value = option.right(option.size()-6);
            }
            if (!value.isEmpty()) {
                webAppManager.setWindowType(value);
            }
            else {
                correctParameters = false;
            }
        }
        else {
            correctParameters = false;
        }
    }

    if( !correctParameters )
        qWarning() << "ERROR: incorrect parameters. Usage is:";

    if (!correctParameters || lArgs.indexOf("--help") >= 0) {
        qDebug() << "webapp-launcher [options]";
        qDebug() << "    -a/--appinfo= : path to appinfo.json file for the app [example: /usr/palm/applications/test/appinfo.json";
        qDebug() << "    -u/--url= : url of the main entry point. If  not provided the one from the app manifest (appinfo.json is used)";
        qDebug() << "    -p/--parameters= : parameters in json format";
        qDebug() << "    -w/--window-type= : type of the application window [card|launcher]. will default to card and is optional";

        return 0;
    }

    return webAppManager.exec();
}
