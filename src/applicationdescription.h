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

#ifndef APPLICATIONDESCRIPTION_H
#define APPLICATIONDESCRIPTION_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QStringList>

#include "ApplicationDescriptionBase.h"

namespace luna
{

class ApplicationDescription : public ApplicationDescriptionBase
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId CONSTANT)
    Q_PROPERTY(QString title READ getTitle CONSTANT)
    Q_PROPERTY(QUrl icon READ getIcon CONSTANT)
    Q_PROPERTY(QUrl entryPoint READ getEntryPoint CONSTANT)
    Q_PROPERTY(bool headless READ isHeadLess CONSTANT)
    Q_PROPERTY(bool flickable READ isFlickable CONSTANT)
    Q_PROPERTY(bool internetConnectivityRequired READ isInternetConnectivityRequired CONSTANT)
    Q_PROPERTY(bool loadingAnimationDisabled READ isLoadingAnimationDisabled CONSTANT)
    Q_PROPERTY(bool allowCrossDomainAccess READ allowCrossDomainAccess CONSTANT)

public:
    ApplicationDescription();
    ApplicationDescription(const ApplicationDescription& other);
    ApplicationDescription(const QString &data);
    virtual ~ApplicationDescription();

    QString getId() const;
    QString getTitle() const;
    QUrl getIcon() const;
    QUrl getEntryPoint() const;
    QStringList getUrlsAllowed() const;
    QString getUserAgent() const;

    QString getPluginName() const;
    QString basePath() const;

    bool hasRemoteEntryPoint() const;

private:
    QString mApplicationBasePath;

    void initializeFromData(const QString &data);
    QUrl locateEntryPoint(const QString &entryPoint) const;
};

} // namespace luna

#endif // APPLICATIONDESCRIPTION_H
