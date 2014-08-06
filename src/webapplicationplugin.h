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


#ifndef WEBAPPLICATIONPLUGIN_H
#define WEBAPPLICATIONPLUGIN_H

#include <QObject>
#include <QFileInfo>
#include <QPluginLoader>
#include <QFileInfo>

#include <applicationplugin.h>

namespace luna
{

class WebApplicationPlugin : public QObject,
                             public ApplicationPlugin
{
    Q_OBJECT
public:
    WebApplicationPlugin(const QFileInfo &path, QObject *parent = 0);

    bool load();

    QList<BaseExtension*> createExtensions(ApplicationEnvironment *environment);

private:
    QPluginLoader mLoader;
    ApplicationPlugin *mInstance;
    QFileInfo mPath;
};

} // namespace luna

#endif // WEBAPPLICATIONPLUGIN_H
