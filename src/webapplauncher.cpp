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

#include <QtWebKit/private/qquickwebview_p.h>
#include <QTimer>

#include "applicationdescription.h"
#include "webapplauncher.h"
#include "webapplication.h"

namespace luna
{

WebAppLauncher::WebAppLauncher(int &argc, char **argv)
    : QGuiApplication(argc, argv),
      mLaunchedApp(0),
      mWindowType("card")
{
    setApplicationName("WebAppLauncher");

    QQuickWebViewExperimental::setFlickableViewportEnabled(false);

    QTimer::singleShot(0, this, SLOT(initializeApp()));

    connect(this, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
}

WebAppLauncher::~WebAppLauncher()
{
    onAboutToQuit();
}

bool WebAppLauncher::validateApplication(const ApplicationDescription& desc)
{
    if (desc.id().length() == 0)
        return false;

    if (desc.entryPoint().isLocalFile() && !QFile::exists(desc.entryPoint().toLocalFile()))
        return false;

    return true;
}

void WebAppLauncher::initializeApp()
{
    if( mUrl.isEmpty() )
    {
        qDebug() << "Calling launchApp(" << mAppDesc << ", " << mParameters << ")";
        mLaunchedApp = launchApp(mAppDesc, mParameters);
    }
    else
    {
        qDebug() << "Calling launchUrl(" << mUrl << ", " << mWindowType << ", " << mAppDesc << ", " << mParameters << ")";
        mLaunchedApp = launchUrl(mUrl, mWindowType, mAppDesc, mParameters);
    }
}

WebApplication* WebAppLauncher::launchApp(const QString &appDesc, const QString &parameters)
{
    ApplicationDescription desc(appDesc);

    if (!validateApplication(desc)) {
        qWarning("Got invalid application description for app %s",
                 desc.id().toUtf8().constData());
        return 0;
    }

    QString processId = QString("%0").arg(applicationPid());
    QString windowType = "card";
    QUrl entryPoint = desc.entryPoint();
    WebApplication *app = new WebApplication(this, entryPoint, windowType,
                                             desc, parameters, processId);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationWindowClosed()));

    return app;
}

WebApplication* WebAppLauncher::launchUrl(const QUrl &url, const QString &windowType,
                                         const QString &appDesc, const QString &parameters)
{
    ApplicationDescription desc(appDesc);

    if (!validateApplication(desc)) {
        qWarning("Got invalid application description for app %s",
                 desc.id().toUtf8().constData());
        return 0;
    }

    QString processId = QString("%0").arg(applicationPid());
    WebApplication *app = new WebApplication(this, url, windowType, desc, parameters, processId);
    connect(app, SIGNAL(closed()), this, SLOT(onApplicationWindowClosed()));

    return app;
}

void WebAppLauncher::onAboutToQuit()
{
    if( mLaunchedApp )
        delete mLaunchedApp;
    mLaunchedApp = NULL;
}

void WebAppLauncher::onApplicationWindowClosed()
{
    WebApplication *app = static_cast<WebApplication*>(sender());

    qDebug() << "Application" << app->id() << "was closed";
    delete app;
}

} // namespace luna
