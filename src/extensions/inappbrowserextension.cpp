/*
 * Copyright (C) 2015 Simon Busch <morphis@gravedo.de>
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

#include <QUrl>
#include <QQmlComponent>
#include <QQuickItem>

#include "../webapplicationwindow.h"
#include "inappbrowserextension.h"

namespace luna
{

InAppBrowserExtension::InAppBrowserExtension(WebApplicationWindow *applicationWindow, QObject *parent) :
    BaseExtension("InAppBrowser", applicationWindow, parent),
    mApplicationWindow(applicationWindow),
    mItem(0)
{
    applicationWindow->registerUserScript(QString("://extensions/InAppBrowser.js"), true);
}

InAppBrowserExtension::~InAppBrowserExtension()
{
    if (mItem)
        delete mItem;
}

void InAppBrowserExtension::open(const QString &url, const QString &frameName)
{
    if (mItem)
        return;

    if (mApplicationWindow->headless())
        return;

    qDebug() << Q_FUNC_INFO << url << frameName;

    QQmlComponent component(mApplicationWindow->qmlEngine(),
                            QUrl("qrc:///qml/InAppBrowser.qml"));
    mItem = qobject_cast<QQuickItem *>(component.create());
    mItem->setParentItem(mApplicationWindow->rootItem());
    mItem->setProperty("url", QVariant(url));
    mItem->setProperty("frameName", QVariant(frameName));

    connect(mItem, SIGNAL(done(const QString &)), this, SLOT(onDone(const QString &)));
    connect(mItem, SIGNAL(titleModified(const QString &)), this, SLOT(onTitleChanged(const QString &)));
}

void InAppBrowserExtension::close()
{
    if (!mItem)
        return;

    mItem->setProperty("visible", QVariant(false));
    mItem->deleteLater();
    mItem = 0;
}

void InAppBrowserExtension::onDone(const QString &frameName)
{
    mAppEnvironment->executeScript(QString("__InAppBrowser.userClickedDone(\"%1\");").arg(frameName));
    close();
}

void InAppBrowserExtension::onTitleChanged(const QString &frameName)
{
    QString title = mItem->property("title").toString();
    mAppEnvironment->executeScript(QString("__InAppBrowser.setTitle(\"%1\",\"%2\");").arg(title).arg(frameName));
}

} // namespace luna
