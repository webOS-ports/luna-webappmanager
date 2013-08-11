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

namespace luna
{

WebApplication::WebApplication(ApplicationDescription *desc, quint64 processId)
    : mDescription(desc),
      mProcessId(processId)
{
    setTitle(mDescription->title());
    setResizeMode(QQuickView::SizeRootObjectToView);
    rootContext()->setContextProperty("webapp", this);
    setSource(QUrl("qrc:///qml/main.qml"));
}

WebApplication::~WebApplication()
{
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
    return mDescription->id();
}

quint64 WebApplication::processId() const
{
    return mProcessId;
}

QUrl WebApplication::url() const
{
    return mDescription->entryPoint();
}

} // namespace luna
