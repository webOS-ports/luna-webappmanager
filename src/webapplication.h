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

#ifndef WINDOWEDWEBAPP_H_
#define WINDOWEDWEBAPP_H_

#include <QQuickView>
#include <QMap>
#ifndef WITH_UNMODIFIED_QTWEBKIT
#include <QtWebKit/private/qwebnewpagerequest_p.h>
#endif

#include "applicationdescription.h"
#include "activity.h"

namespace luna
{

class WebAppManager;
class BaseExtension;
class WebApplicationWindow;

class WebApplication : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(int64_t processId READ processId CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(QUrl icon READ icon CONSTANT)
    Q_PROPERTY(QString identifier READ identifier CONSTANT)
    Q_PROPERTY(int activityId READ activityId CONSTANT)
    Q_PROPERTY(QString parameters READ parameters NOTIFY parametersChanged)
    Q_PROPERTY(bool headless READ headless CONSTANT)
    Q_PROPERTY(bool privileged READ privileged CONSTANT)
    Q_PROPERTY(bool internetConnectivityRequired READ internetConnectivityRequired CONSTANT)
    Q_PROPERTY(QStringList urlsAllowed READ urlsAllowed CONSTANT)
    Q_PROPERTY(QString userAgent READ userAgent CONSTANT)
    Q_PROPERTY(bool loadingAnimationDisabled READ loadingAnimationDisabled CONSTANT)

public:
    WebApplication(WebAppManager *launcher, const QUrl& url, const QString& windowType,
                   const ApplicationDescription& desc, const QString& parameters,
                   const int64_t processId, QObject *parent = 0);
    virtual ~WebApplication();

    QString id() const;
    int64_t processId() const;
    QUrl url() const;
    QUrl icon() const;
    QString identifier() const;
    int activityId() const;
    QString parameters() const;
    bool headless() const;
    bool privileged() const;
    bool internetConnectivityRequired() const;
    QStringList urlsAllowed() const;
    bool hasRemoteEntryPoint() const;
    QString userAgent() const;
    bool loadingAnimationDisabled() const;

    void changeActivityFocus(bool focus);

    bool validateResourcePath(const QString& path);

    void relaunch(const QString &parameters);

#ifndef WITH_UNMODIFIED_QTWEBKIT
    void createWindow(QWebNewPageRequest *request);
#endif

    void kill();

Q_SIGNALS:
    void closed();

    void parametersChanged();

public Q_SLOTS:
    void windowClosed();

private:
    WebAppManager *mLauncher;
    ApplicationDescription mDescription;
    int64_t mProcessId;
    QString mIdentifier;
    QString mParameters;
    WebApplicationWindow *mMainWindow;
    QList<WebApplicationWindow*> mChildWindows;
    bool mLaunchedAtBoot;
    bool mPrivileged;
    Activity mActivity;
};

} // namespace luna

#endif
