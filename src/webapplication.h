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
#include <luna-service2/lunaservice.h>

#include "applicationdescription.h"

namespace luna
{

class WebAppManager;
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
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QString parameters READ parameters CONSTANT)
    Q_PROPERTY(bool headless READ headless CONSTANT)

public:
    WebApplication(WebAppManager *manager, const QUrl& url, const QString& windowType,
                   const ApplicationDescription& desc, const QString& parameters,
                   const QString& processId, QObject *parent = 0);
    virtual ~WebApplication();

    void run();
    void relaunch(const QString& parameters);

    QString id() const;
    QString processId() const;
    QUrl url() const;
    QUrl icon() const;
    QString identifier() const;
    int activityId() const;
    bool ready() const;
    QString parameters() const;
    bool headless() const;

    void setActivityId(int activityId);

    void stagePreparing();
    void stageReady();

    void changeActivityFocus(bool focus);

    static bool activityManagerCallback(LSHandle *handle, LSMessage *message, void *user_data);

    void createWindow(QWebNewPageRequest *request);

signals:
    void readyChanged();
    void closed();

private:
    WebAppManager *mManager;
    ApplicationDescription mDescription;
    QString mProcessId;
    LSMessageToken mActivityManagerToken;
    QString mIdentifier;
    int mActivityId;
    bool mReady;
    QString mParameters;
    WebApplicationWindow *mMainWindow;
    QList<WebApplicationWindow*> mWindows;

    void createActivity();
    void destroyActivity();
};

} // namespace luna

#endif
