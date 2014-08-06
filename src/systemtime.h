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

#ifndef SYSTEMTIME_H_
#define SYSTEMTIME_H_

#include <QString>

#include <luna-service2++/handle.hpp>
#include <luna-service2++/call.hpp>
#include <luna-service2++/server_status.hpp>

namespace luna
{

class SystemTime
{
public:
    static SystemTime* instance();

    QString timezone() const;

private:
    SystemTime();

    void updateFromService(LSMessage *message);
    static bool updateCallback(LSHandle *handle, LSMessage *message, void *context);

private:
    LS::Handle mLunaPrivHandle;
    LS::ServerStatus mServerStatus;
    LS::Call mSubscriptionCall;
    QString mTimezone;
};

} // namespace luna

#endif
