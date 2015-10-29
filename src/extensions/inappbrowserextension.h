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

#ifndef INAPPBROWSEREXTENSION_H
#define INAPPBROWSEREXTENSION_H

#include <baseextension.h>

class QQuickItem;

namespace luna
{

class WebApplicationWindow;

class InAppBrowserExtension : public BaseExtension
{
    Q_OBJECT
public:
    explicit InAppBrowserExtension(WebApplicationWindow *applicationWindow, QObject *parent = 0);
    ~InAppBrowserExtension();

public Q_SLOTS:
    void open(const QString &url, const QString &frameName);
    void close();

private Q_SLOTS:
    void onDone(const QString &frameName);
    void onTitleChanged(const QString &frameName);

private:
    WebApplicationWindow *mApplicationWindow;
    QQuickItem *mItem;
};

} // namespace luna

#endif // INAPPBROWSEREXTENSION_H
