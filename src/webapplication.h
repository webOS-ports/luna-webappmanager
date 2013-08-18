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

#include "applicationdescription.h"

namespace luna
{

class BasePlugin;

class WebApplication : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString processId READ processId CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)

public:
    WebApplication(const ApplicationDescription& desc, const QString& processId);
    virtual ~WebApplication();

    void run();

    virtual bool event(QEvent *event);

    QString id() const;
    QString processId() const;
    QUrl url() const;

signals:
    void javaScriptExecNeeded(const QString &script);
    void pluginWantsToBeAdded(const QString &name, QObject *object);
    void closed();

public slots:
    void loadFinished();
    void executeScript(const QString &script);

private:
    QMap<QString, BasePlugin*> mPlugins;
    ApplicationDescription mDescription;
    QString mProcessId;

    void createPlugins();
    void createAndInitializePlugin(BasePlugin *plugin);
};

} // namespace luna

#endif
