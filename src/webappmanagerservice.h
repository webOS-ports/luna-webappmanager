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

#ifndef WEBAPPMANAGERSERVICE_H_
#define WEBAPPMANAGERSERVICE_H_

#include <glib.h>
#include <luna-service2/lunaservice.hpp>

namespace luna
{

class WebAppManager;

class WebAppManagerService : private LS::Handle
{
public:
    WebAppManagerService(WebAppManager *webAppManager);
    ~WebAppManagerService();

    void notifyAppHasStarted(const QString& appId, int64_t processId);
    void notifyAppHasFinished(const QString& appId, int64_t processId);

private:
    bool launchApp(LSMessage &message);
    bool launchUrl(LSMessage &message);
    bool killApp(LSMessage &message);
    bool isAppRunning(LSMessage &message);
    bool listRunningApps(LSMessage &message);
    bool registerForAppEvents(LSMessage &message);
    bool relaunch(LSMessage &message);

private:
    WebAppManager *mWebAppManager;
    LS::SubscriptionPoint mAppEventSubscriptions;
};

} // namespace luna

#endif
