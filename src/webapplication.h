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
#include <QtWebKit/private/qwebnewpagerequest_p.h>

#include "applicationdescription.h"

namespace luna
{

class WebAppLauncher;
class BasePlugin;
class WebApplicationWindow;

class WebApplication : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString processId READ processId CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(QUrl icon READ icon CONSTANT)
    Q_PROPERTY(QString identifier READ identifier CONSTANT)
    Q_PROPERTY(int activityId READ activityId CONSTANT)
    Q_PROPERTY(QString parameters READ parameters CONSTANT)
    Q_PROPERTY(bool headless READ headless CONSTANT)
    Q_PROPERTY(bool privileged READ privileged CONSTANT)

public:
    WebApplication(WebAppLauncher *launcher, const QUrl& url, const QString& windowType,
                   const ApplicationDescription& desc, const QString& parameters,
                   const QString& processId, QObject *parent = 0);
    virtual ~WebApplication();

    void relaunch(const QString& parameters);

    QString id() const;
    QString processId() const;
    QUrl url() const;
    QUrl icon() const;
    QString identifier() const;
    int activityId() const;
    QString parameters() const;
    bool headless() const;
    bool privileged() const;

    void setActivityId(int activityId);

    void changeActivityFocus(bool focus);

    void createWindow(QWebNewPageRequest *request);

signals:
    void closed();

public slots:
    void windowClosed();

private:
    WebAppLauncher *mLauncher;
    ApplicationDescription mDescription;
    QString mProcessId;
    QString mIdentifier;
    int mActivityId;
    QString mParameters;
    WebApplicationWindow *mMainWindow;
    QList<WebApplicationWindow*> mChildWindows;
    bool mLaunchedAtBoot;
    bool mPrivileged;
};

} // namespace luna

#endif
