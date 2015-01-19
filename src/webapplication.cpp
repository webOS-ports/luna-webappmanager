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
#ifndef WITH_UNMODIFIED_QTWEBKI
#include <QtWebKit/private/qwebnewpagerequest_p.h>
#endif

#include <set>
#include <string>

#include "webappmanager.h"
#include "applicationdescription.h"
#include "webapplication.h"
#include "webapplicationwindow.h"

#include <Settings.h>

#include <sys/types.h>
#include <unistd.h>

namespace luna
{

class ResourcePathValidator
{
public:
    static ResourcePathValidator& instance()
    {
        static ResourcePathValidator instance;
        return instance;
    }

    bool validate(const QString &path, bool privileged)
    {
        if (findPathInList(mAllowedTargetPaths, path))
            return true;
        if (privileged && findPathInList(mPrivilegedAppPaths, path))
            return true;
        if (!privileged && findPathInList(mUnprivilegedAppPaths, path))
            return true;

        return false;
    }

private:
    ResourcePathValidator()
    {
        // NOTE: below set of paths are taken from the configuration set in the webkit used in
        // webOS 3.0.5. See http://downloads.help.palm.com/opensource/3.0.5/webcore-patch.gz

        // paths allowed for every app
        mAllowedTargetPaths << "/usr/palm/frameworks";
        mAllowedTargetPaths << "/media/internal";
        mAllowedTargetPaths << "/usr/lib/luna/luna-media";
        mAllowedTargetPaths << "/var/luna/files";
        mAllowedTargetPaths << "/var/luna/data/extractfs";
        mAllowedTargetPaths << "/var/luna/data/im-avatars";
        mAllowedTargetPaths <<  "/usr/palm/applications/com.palm.app.contacts/sharedWidgets/";
        mAllowedTargetPaths << "/usr/palm/sysmgr/";
        mAllowedTargetPaths << "/usr/palm/public";
        mAllowedTargetPaths << "/var/file-cache/";
        mAllowedTargetPaths << "/usr/lib/luna/system/luna-systemui/images/";
        mAllowedTargetPaths << "/usr/lib/luna/system/luna-systemui/app/FilePicker";

        // paths only allowed for privileged apps
        mPrivilegedAppPaths << "/usr/lib/luna/system/";   // system ui apps
        mPrivilegedAppPaths << "/usr/palm/applications/";  // Palm apps
        mPrivilegedAppPaths << "/var/usr/palm/applications/com.palm.";  // privileged apps like facebook
        mPrivilegedAppPaths << "/media/cryptofs/apps/usr/palm/applications/com.palm.";  // privileged 3rd party apps
        mPrivilegedAppPaths << "/usr/palm/sysmgr/";
        mPrivilegedAppPaths << "/var/usr/palm/applications/com/palm/";
        mPrivilegedAppPaths << "/media/cryptofs/apps/usr/palm/applications/com/palm/";

        // additional paths allowed for unprivileged apps
        mUnprivilegedAppPaths << "/var/usr/palm/applications/";
        mUnprivilegedAppPaths << "/media/cryptofs/apps/usr/palm/applications/";
    }

    bool findPathInList(const QStringList &list, const QString &path)
    {
        Q_FOREACH(QString item, list) {
            if (path.startsWith(item))
                return true;
        }
        return false;
    }

    QStringList mAllowedTargetPaths;
    QStringList mPrivilegedAppPaths;
    QStringList mUnprivilegedAppPaths;
};

WebApplication::WebApplication(WebAppManager *launcher, const QUrl& url, const QString& windowType,
                               const ApplicationDescription& desc, const QString& parameters,
                               const int64_t processId, QObject *parent) :
    QObject(parent),
    mLauncher(launcher),
    mDescription(desc),
    mProcessId(processId),
    mIdentifier(QString("%1 %2").arg(mDescription.id()).arg(mProcessId)),
    mParameters(parameters),
    mMainWindow(0),
    mLaunchedAtBoot(false),
    mPrivileged(false),
    mActivity(mIdentifier, desc.id(), processId)
{
    qDebug() << __PRETTY_FUNCTION__ << this;

    // Only system applications with a specific id prefix are privileged to access
    // the private luna bus
    if (mDescription.id().startsWith("org.webosports") || mDescription.id().startsWith("com.palm") ||
        mDescription.id().startsWith("org.webosinternals"))
        mPrivileged = true;

    mMainWindow = new WebApplicationWindow(this, url, windowType,
            QSize(Settings::LunaSettings()->displayWidth, Settings::LunaSettings()->displayHeight),
            mDescription.headless());

    processParameters();
}

WebApplication::~WebApplication()
{
    qDebug() << __PRETTY_FUNCTION__ << this;

    Q_FOREACH(WebApplicationWindow *window, mChildWindows) {
        mChildWindows.removeAll(window);
        delete window;
    }

    if (mMainWindow)
        delete mMainWindow;
}

void WebApplication::processParameters()
{
    QJsonDocument document = QJsonDocument::fromJson(mParameters.toUtf8());
    if (!document.isObject())
        return;

    QJsonObject rootObject = document.object();
    if (rootObject.contains("launchedAtBoot") && rootObject["launchedAtBoot"].isBool())
        mLaunchedAtBoot = rootObject["launchedAtBoot"].toBool();
}

void WebApplication::changeActivityFocus(bool focus)
{
    if (focus)
        mActivity.focus();
    else
        mActivity.unfocus();
}

void WebApplication::relaunch(const QString &parameters)
{
    qDebug() << __PRETTY_FUNCTION__ << "Relaunching application" << mDescription.id() << "with parameters" << parameters;

    mParameters = parameters;
    emit parametersChanged();

    mMainWindow->executeScript(QString("Mojo.relaunch();"));
}

#ifndef WITH_UNMODIFIED_QTWEBKIT

void WebApplication::createWindow(QWebNewPageRequest *request)
{
    int width = Settings::LunaSettings()->displayWidth;
    int height = Settings::LunaSettings()->displayHeight;

    qDebug() << __PRETTY_FUNCTION__ << "Creating new window for url" << request->url();

    QVariantMap windowFeatures = request->windowFeatures();
    foreach(QString key, windowFeatures.keys()) {
        qDebug() << "[" << key << "] = " << windowFeatures.value(key);
    }

    // child windows can never be headless ones!
    QString windowType = "card";

    // check if we got supplied with a different window type
    if (windowFeatures.contains("attributes")) {
        QString attributes = windowFeatures["attributes"].toString();
        QJsonDocument document = QJsonDocument::fromJson(attributes.toUtf8());
        QString windowTypeAttrib = document.object().value("window").toString();
        if (windowTypeAttrib.length() > 0)
            windowType = windowTypeAttrib;
    }

    if (windowFeatures.contains("height"))
        height = windowFeatures["attributes"].toInt();

    qDebug() << Q_FUNC_INFO << "Setting parent window id" << mMainWindow->windowId() << "for new window";
    WebApplicationWindow *window = new WebApplicationWindow(this, request->url(),
                                                            windowType, QSize(width, height), false,
                                                            mMainWindow->windowId());

    request->setWebView(window->webView());

    mChildWindows.append(window);
}

#endif

void WebApplication::closeWindow(WebApplicationWindow *window)
{
    // if the window is marked as keep alive we don't close it
    if (window->keepAlive()) {
        qDebug() << "Not closing window cause it was configured to be kept alive";
        return;
    }

    // if it's a child window we remove it but have to take care about
    // some special conditions
    if (mChildWindows.contains(window)) {
        mChildWindows.removeOne(window);
        window->destroy();
        window->deleteLater();

        // if no child window is left close the main (headless) window too
        if (mChildWindows.count() == 0 && !mLaunchedAtBoot && headless()) {
            qDebug() << "All child windows of app" << id()
                     << "were closed so closing the main window too";

            mMainWindow->destroy();
            mMainWindow->deleteLater();
            mMainWindow = 0;

            emit closed();
        }
    }
    else if (window == mMainWindow) {
        // the main window was closed so close all child windows too
        mMainWindow->destroy();
        mMainWindow->deleteLater();
        mMainWindow = 0;

        qDebug() << "The main window of app " << id()
                 << "was closed, so closing all child windows too";

        foreach(WebApplicationWindow *childWindow, mChildWindows) {
            childWindow->destroy();
            childWindow->deleteLater();
        }

        emit closed();
    }
}

void WebApplication::kill()
{
    emit closed();
}

void WebApplication::clearMemoryCaches()
{
    mMainWindow->clearMemoryCaches();

    foreach (WebApplicationWindow *window, mChildWindows)
        window->clearMemoryCaches();
}

bool WebApplication::validateResourcePath(const QString &path)
{
    return ResourcePathValidator::instance().validate(path, mPrivileged);
}

QString WebApplication::id() const
{
    return mDescription.id();
}

int64_t WebApplication::processId() const
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

bool WebApplication::internetConnectivityRequired() const
{
    return mDescription.internetConnectivityRequired();
}

QStringList WebApplication::urlsAllowed() const
{
    return mDescription.urlsAllowed();
}

bool WebApplication::hasRemoteEntryPoint() const
{
    return mDescription.hasRemoteEntryPoint();
}

QString WebApplication::userAgent() const
{
    return mDescription.userAgent();
}

int WebApplication::activityId() const
{
    return mActivity.id();
}

bool WebApplication::loadingAnimationDisabled() const
{
    return mDescription.loadingAnimationDisabled();
}

bool WebApplication::allowCrossDomainAccess() const
{
    if (!mPrivileged)
        return false;

    return mDescription.allowCrossDomainAccess();
}

ApplicationDescription WebApplication::desc() const
{
    return mDescription;
}

} // namespace luna
