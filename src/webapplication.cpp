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

#include "applicationdescription.h"
#include "webapplication.h"

#include "plugins/baseplugin.h"
#include "plugins/palmsystemplugin.h"
#include "plugins/palmservicebridgeplugin.h"

namespace luna
{

WebApplication::WebApplication(const ApplicationDescription& desc, const QString& processId)
    : mDescription(desc),
      mProcessId(processId)
{
    setTitle(mDescription.title());
    setResizeMode(QQuickView::SizeRootObjectToView);

    rootContext()->setContextProperty("webapp", this);
    setSource(QUrl("qrc:///qml/main.qml"));

    createPlugins();
}

WebApplication::~WebApplication()
{
}

void WebApplication::createPlugins()
{
    createAndInitializePlugin(new PalmSystemPlugin(this));
    createAndInitializePlugin(new PalmServiceBridgePlugin(this));
}

void WebApplication::createAndInitializePlugin(BasePlugin *plugin)
{
    mPlugins.insert(plugin->name(), plugin);
    emit pluginWantsToBeAdded(plugin->name(), plugin);
}

void WebApplication::run()
{
    showMaximized();
}

bool WebApplication::event(QEvent *event)
{
    if (event->type() == QEvent::Close)
        emit closed();
    return QQuickWindow::event(event);
}

void WebApplication::executeScript(const QString &script)
{
    emit javaScriptExecNeeded(script);
}

void WebApplication::loadFinished()
{
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

} // namespace luna
