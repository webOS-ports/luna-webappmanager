/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
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

#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QString>
#include <luna-service2/lunaservice.h>

namespace luna
{

class Activity
{
public:
    Activity(const QString& identifier, const QString& appId, const int64_t processId);
    ~Activity();

    int id() const;

    void focus();
    void unfocus();

    static bool activityCallback(LSHandle *handle, LSMessage *message, void *user_data);

private:
    LSHandle *mHandle;
    LSMessageToken mToken;
    int mId;
    QString mAppId;
    int64_t mProcessId;
    QString mIdentifier;
    bool mFocus;

    void setup();
    void handleActivityResponse(LSMessage *message);
};

} // namespace

#endif // ACTIVITY_H
