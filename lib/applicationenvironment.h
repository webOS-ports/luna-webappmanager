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

#ifndef SCRIPTEXECUTOR_H
#define SCRIPTEXECUTOR_H

#include <QObject>

namespace luna
{

class ApplicationEnvironment : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationEnvironment(QObject *parent = 0);

    virtual void executeScript(const QString &script) = 0;
    virtual void registerUserScript(const QUrl &path) = 0;
};

} // namespace luna

#endif // SCRIPTEXECUTOR_H
