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

#ifndef WEBAPPLICATIONWINDOWFACTORY_H
#define WEBAPPLICATIONWINDOWFACTORY_H

#include <QUrl>
#include <QString>
#include <QSize>

namespace luna
{

class WebApplication;
class WebApplicationWindow;
class WebApplicationWindowBase;

class WebApplicationWindowFactory
{
public:
    static WebApplicationWindow *createWindow(WebApplication *application, const QUrl& url, const QString& windowType,
                                              const QSize& size, bool headless = false, int parentWindowId = 0);
};

} // namespace luna

#endif // WEBAPPLICATIONWINDOWFACTORY_H
