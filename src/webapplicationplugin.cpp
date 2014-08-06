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

#include "webapplicationplugin.h"

namespace luna
{

WebApplicationPlugin::WebApplicationPlugin(const QFileInfo &path, QObject *parent) :
    QObject(parent),
    mPath(path)
{
}

bool WebApplicationPlugin::load()
{
    mLoader.setFileName(mPath.filePath());
    if (Q_UNLIKELY(!mLoader.load())) {
        qWarning() << "Failed to load application plugin: " << mLoader.errorString();
        return false;
    }

    mInstance = qobject_cast<ApplicationPlugin*>(mLoader.instance());
    if (Q_UNLIKELY(mInstance == 0)) {
        qWarning() << mPath.filePath() << "doesn't implement application plugin interface";
        return false;
    }

    return true;
}

QList<BaseExtension*> WebApplicationPlugin::createExtensions(ApplicationEnvironment *environment)
{
    return mInstance->createExtensions(environment);
}

} // namespace luna
