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
#include <QQmlContext>
#include <QJsonObject>
#include <QJsonDocument>

#include <QtWebKit/private/qquickwebview_p.h>
#include <QtWebKit/private/qwebnewpagerequest_p.h>

#include <set>
#include <string>

#include "webapplauncher.h"
#include "applicationdescription.h"
#include "webapplication.h"
#include "webapplicationwindow.h"

#include <Settings.h>

namespace luna
{

WebApplication::WebApplication(WebAppLauncher *launcher, const QUrl& url, const QString& windowType,
                               const ApplicationDescription& desc, const QString& parameters,
                               const QString& processId, QObject *parent) :
    QObject(parent),
    mLauncher(launcher),
    mDescription(desc),
    mProcessId(processId),
    mIdentifier(mDescription.id() + "-" + mProcessId),
    mActivityId(-1),
    mParameters(parameters),
    mMainWindow(0),
    mLaunchedAtBoot(false),
    mPrivileged(false)
{
    mMainWindow = new WebApplicationWindow(this, url, windowType, mDescription.headless());
    connect(mMainWindow, SIGNAL(closed()), this, SLOT(windowClosed()));

    const std::set<std::string> appsToLaunchAtBoot = Settings::LunaSettings()->appsToLaunchAtBoot;
    mLaunchedAtBoot = (appsToLaunchAtBoot.find(id().toStdString()) != appsToLaunchAtBoot.end());

    if (mDescription.id().startsWith("org.webosports") || mDescription.id().startsWith("com.palm"))
        mPrivileged = true;
}

WebApplication::~WebApplication()
{
}

void WebApplication::setActivityId(int activityId)
{
    mActivityId = activityId;
}

void WebApplication::changeActivityFocus(bool focus)
{
    if (mActivityId < 0)
        return;
}

void WebApplication::relaunch(const QString &parameters)
{
    qDebug() << __PRETTY_FUNCTION__ << "Relaunching application" << mDescription.id() << "with parameters" << parameters;

    mParameters = parameters;
    mMainWindow->executeScript(QString("_webOS.relaunch(\"%1\");").arg(parameters));
}

void WebApplication::createWindow(QWebNewPageRequest *request)
{
    qDebug() << __PRETTY_FUNCTION__ << "creating new window for url" << request->url();

    // child windows can never be headless ones!
    QString windowType = "card";
    WebApplicationWindow *window = new WebApplicationWindow(this, request->url(), windowType, false);

    connect(window, SIGNAL(closed()), this, SLOT(windowClosed()));

    request->setWebView(window->webView());

    window->show();

    mChildWindows.append(window);
}

void WebApplication::windowClosed()
{
    WebApplicationWindow *window = static_cast<WebApplicationWindow*>(sender());

    // if the window is marked as keep alive we don't close it
    if (window->keepAlive()) {
        qDebug() << "Not closing window cause it was configured to be kept alive";
        return;
    }

    // if it's a child window we remove it but have to take care about
    // some special conditions
    if (mChildWindows.contains(window)) {
        mChildWindows.removeOne(window);
        delete window;

        // if no child window is left close the main (headless) window too
        if (mChildWindows.count() == 0 && !mLaunchedAtBoot) {
            qDebug() << "All child windows of app" << id()
                     << "were closed so closing the main window too";

            delete mMainWindow;
            emit closed();
        }
    }
    else if (window == mMainWindow) {
        // the main window was closed so close all child windows too
        delete mMainWindow;

        qDebug() << "The main window of app " << id()
                 << "was closed, so closing all child windows too";

        foreach(WebApplicationWindow *child, mChildWindows) {
            delete child;
        }

        emit closed();
    }
}

QString WebApplication::id() const
{
    return mDescription.id();
}

QString WebApplication::processId() const
{
    return mProcessId;
}

QUrl WebApplication::url() const
{
    return mDescription.entryPoint();
}

QUrl WebApplication::icon() const
{
    return mDescription.icon();
}

QString WebApplication::identifier() const
{
    return mIdentifier;
}

int WebApplication::activityId() const
{
    return mActivityId;
}

QString WebApplication::parameters() const
{
    return mParameters;
}

bool WebApplication::headless() const
{
    return mDescription.headless();
}

bool WebApplication::privileged() const
{
    return mPrivileged;
}

} // namespace luna
