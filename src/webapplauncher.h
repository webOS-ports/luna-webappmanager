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

#ifndef WEBAPPMANAGER_H_
#define WEBAPPMANAGER_H_

#include <QtGlobal>
#include <glib.h>
#include <QGuiApplication>
#include <QMap>
#include <QUrl>
#include <QFile>
#include <QTextStream>

namespace luna
{

class ApplicationDescription;
class WebApplication;

class WebAppLauncher : public QGuiApplication
{
    Q_OBJECT

public:
    WebAppLauncher(int &argc, char **argv);
    virtual ~WebAppLauncher();

    WebApplication* launchApp(const QString &appDesc, const QString &parameters);
    WebApplication* launchUrl(const QUrl &url, const QString &windowType,
                              const QString &appDesc, const QString &parameters);

    void setUrl(const QString &url) { mUrl = QUrl(url); }
    void setWindowType(const QString &windowType) { mWindowType = windowType; }
    void setAppDesc(const QString &appDescFilePath) {
        QFile appDescFile(appDescFilePath);
        if (!appDescFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        mAppDesc = QTextStream(&appDescFile).readAll();
    }
    void setParameters(const QString &parameters) { mParameters = parameters; }

private slots:
    void initializeApp();
    void onApplicationWindowClosed();
    void onAboutToQuit();

private:
    WebApplication *mLaunchedApp;

    QUrl mUrl;
    QString mWindowType;
    QString mAppDesc;
    QString mParameters;

    bool validateApplication(const ApplicationDescription& desc);
};

} // namespace luna

#endif
