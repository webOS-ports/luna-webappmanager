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

#include <set>
#include <string>

#include <QtWebEngine/private/qquickwebenginenewviewrequest_p.h>

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
    mIdentifier(QString("%1 %2").arg(mDescription.getId()).arg(mProcessId)),
    mParameters(parameters),
    mMainWindow(0),
    mMainUrl(url),
    mMainWindowType(windowType),
    mLaunchedAtBoot(false),
    mPrivileged(false),
    mActivity(mIdentifier, desc.getId(), processId)
{
    qDebug() << __PRETTY_FUNCTION__ << this;

    // Only system applications with a specific id prefix are privileged to access
    // the private luna bus
    if (mDescription.getId().startsWith("org.webosports") || mDescription.getId().startsWith("com.palm") ||
        mDescription.getId().startsWith("org.webosinternals"))
        mPrivileged = true;

    createMainWindow();

    processParameters();
}

WebApplication::~WebApplication()
{
    qDebug() << __PRETTY_FUNCTION__ << this;

    Q_FOREACH(WebApplicationWindow *window, mAppWindows) {
        mAppWindows.removeAll(window);
        delete window;
    }
    mMainWindow = NULL;
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
    qDebug() << __PRETTY_FUNCTION__ << "Relaunching application" << mDescription.getId() << "with parameters" << parameters;

    mParameters = parameters;
    emit parametersChanged();

    if( !mMainWindow ) {
        createMainWindow();
    }

    mMainWindow->executeScript(QString("Mojo.relaunch();"));
}

void WebApplication::createMainWindow()
{
    if( mMainWindow ) return;

    mMainWindow = new WebApplicationWindow(this, mMainUrl, mMainWindowType,
            QSize(Settings::LunaSettings()->displayWidth, Settings::LunaSettings()->displayHeight),
            headless());

    mAppWindows.append(mMainWindow);
}

void WebApplication::createWindow(QQuickWebEngineNewViewRequest *request)
{
    int width = Settings::LunaSettings()->displayWidth;
    int height = Settings::LunaSettings()->displayHeight;

    qDebug() << __PRETTY_FUNCTION__ << "Creating new window for url" << request->url();

    const QStringList &additionalFeatures = request->additionalFeatures();
    // The list could be something like a,b={titi},attributes={"window":"card","height":"150"}
    // Let's convert it to a Map for more convenient parsing
    QVariantMap windowFeatures;
    foreach(const QString &elt, additionalFeatures) {
        int indexSep = elt.indexOf('=');
        if( indexSep < 0 )
            windowFeatures.insert(elt, "1");
        else
            windowFeatures.insert(elt.left(indexSep), elt.mid(indexSep+1));
    }

    QString windowType = "card";
    QString windowMetrics = "";

    // check if we got supplied with a different window type
    QJsonDocument attributesJsonDocument;
    if (windowFeatures.contains("attributes")) {
        QString attributes = windowFeatures["attributes"].toString();
        attributesJsonDocument = QJsonDocument::fromJson(attributes.toUtf8());

        QString windowTypeAttrib = attributesJsonDocument.object().value("window").toString();
        if (windowTypeAttrib.length() > 0)
            windowType = windowTypeAttrib;

        windowMetrics = attributesJsonDocument.object().value("metrics").toString();
        
        qDebug() << __PRETTY_FUNCTION__ << "windowMetric metrics value: " << windowMetrics;
    }

    if (windowFeatures.contains("height")) {
        QVariant::Type type = windowFeatures["height"].type();
        if (type == QVariant::Int) {
            height = windowFeatures["height"].toInt();
            qDebug() << __PRETTY_FUNCTION__ << "height is in int value: " << height;
        }
        
        else if (type == QVariant::Double) {
            height = static_cast<int>(windowFeatures["height"].toDouble());
            qDebug() << __PRETTY_FUNCTION__ << "height is in double value: " << height;
        }

        if (windowMetrics == "units") {
            qDebug() << __PRETTY_FUNCTION__ << "windowMetrics == \"units\" Settings::LunaSettings()->gridUnit: " << Settings::LunaSettings()->gridUnit;
            float gridUnit = Settings::LunaSettings()->gridUnit;
            height = static_cast<int>(qRound(height * gridUnit));
            qDebug() << __PRETTY_FUNCTION__ << "height: " << height;
        }
    }

    QVariantMap lWindowAttributesMap;
    if( !attributesJsonDocument.isEmpty() )
    {
        lWindowAttributesMap = attributesJsonDocument.object().toVariantMap();
    }

    int launchedFromWindowId = 0;
    if( mMainWindow ) {
        launchedFromWindowId = mMainWindow->windowId();
    }

    qDebug() << Q_FUNC_INFO << "Setting parent window id" << launchedFromWindowId << "for new window";
    WebApplicationWindow *window = new WebApplicationWindow(this, request->url(),
                                                            windowType, QSize(width, height),
                                                            false, lWindowAttributesMap,
                                                            launchedFromWindowId);

    request->openIn(window->webView());

    mAppWindows.append(window);
}

void WebApplication::closeWindow(WebApplicationWindow *window)
{
    // if the window is marked as keep alive we don't close it
    if (window->keepAlive()) {
        qDebug() << "Not closing window cause it was configured to be kept alive";
        return;
    }

    // if it's a child window we remove it but have to take care about
    // some special conditions
    if (mAppWindows.contains(window)) {
        mAppWindows.removeOne(window);
        window->destroy();
        window->deleteLater();

        if( window == mMainWindow ) mMainWindow = 0;

        if( mAppWindows.size() == 0 ) {
            emit closed();
        }
        // if the last remaining window is an headless window, close it too
        else if (mAppWindows.count() == 1 && mAppWindows.at(0)->headless() && !mLaunchedAtBoot) {
            qDebug() << "All visible windows of app" << id()
                     << "were closed so closing the main window too";

            closeWindow(mAppWindows.at(0));
        }
    }
}

void WebApplication::kill()
{
    emit closed();
}

void WebApplication::clearMemoryCaches()
{
    mMainWindow->clearMemoryCaches();

    foreach (WebApplicationWindow *window, mAppWindows)
        window->clearMemoryCaches();
}

bool WebApplication::validateResourcePath(const QString &path)
{
    return ResourcePathValidator::instance().validate(path, mPrivileged);
}

QString WebApplication::id() const
{
    return mDescription.getId();
}

int64_t WebApplication::processId() const
{
    return mProcessId;
}

QUrl WebApplication::url() const
{
    return mDescription.getEntryPoint();
}

QUrl WebApplication::icon() const
{
    return mDescription.getIcon();
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
    return mDescription.isHeadLess();
}

bool WebApplication::privileged() const
{
    return mPrivileged;
}

bool WebApplication::internetConnectivityRequired() const
{
    return mDescription.isInternetConnectivityRequired();
}

QStringList WebApplication::urlsAllowed() const
{
    return mDescription.getUrlsAllowed();
}

bool WebApplication::hasRemoteEntryPoint() const
{
    return mDescription.hasRemoteEntryPoint();
}

QString WebApplication::userAgent() const
{
    return mDescription.getUserAgent();
}

int WebApplication::activityId() const
{
    return mActivity.id();
}

bool WebApplication::loadingAnimationDisabled() const
{
    return mDescription.isLoadingAnimationDisabled();
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

bool WebApplication::isLauncher() const
{
    return mDescription.getId() == "com.palm.launcher";
}

} // namespace luna
