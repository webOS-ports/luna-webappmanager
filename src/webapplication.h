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
#include <luna-service2/lunaservice.h>

#include "applicationdescription.h"

namespace luna
{

class WebAppManager;
class BasePlugin;

class WebApplication : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString processId READ processId CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(QString identifier READ identifier CONSTANT)
    Q_PROPERTY(int activityId READ activityId CONSTANT)

public:
    WebApplication(WebAppManager *manager, const ApplicationDescription& desc, const QString& processId);
    virtual ~WebApplication();

    void run();

    virtual bool event(QEvent *event);

    QString id() const;
    QString processId() const;
    QUrl url() const;
    QString identifier() const;
    int activityId() const;

    void setActivityId(int activityId);

    static bool activityManagerCallback(LSHandle *handle, LSMessage *message, void *user_data);

signals:
    void javaScriptExecNeeded(const QString &script);
    void pluginWantsToBeAdded(const QString &name, QObject *object);
    void closed();

public slots:
    void loadFinished();
    void executeScript(const QString &script);

private:
    WebAppManager *mManager;
    QMap<QString, BasePlugin*> mPlugins;
    ApplicationDescription mDescription;
    QString mProcessId;
    LSMessageToken mActivityManagerToken;
    QString mIdentifier;
    int mActivityId;

    void createPlugins();
    void createAndInitializePlugin(BasePlugin *plugin);

    void createActivity();
    void destroyActivity();
    void changeActivityFocus(bool focus);
};

} // namespace luna

#endif
