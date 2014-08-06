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

#ifndef BASEPLUGIN_H
#define BASEPLUGIN_H

#include <QObject>
#include <QString>
#include <QJsonArray>

namespace luna
{

class ApplicationEnvironment;

class BaseExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)

public:
    explicit BaseExtension(const QString &name, ApplicationEnvironment *environment, QObject *parent = 0);

    virtual void initialize();

    QString name() const;

    virtual QString handleSynchronousCall(const QString& funcName, const QJsonArray& params);

protected:
    void callbackWithoutRemove(int id, const QString &parameters);
    void callback(int id, const QString &parameters);

protected:
    ApplicationEnvironment *mAppEnvironment;

private:
    QString mName;
};

} // namespace luna

#endif // BASEPLUGIN_H
