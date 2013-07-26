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

#ifndef BUNDLE_H_
#define BUNDLE_H_

#include <WebKit2/WKBundle.h>
#include <WebKit2/WKBundlePage.h>

namespace luna
{

class WebkitBundle
{
public:
    WebkitBundle();
    ~WebkitBundle();

    void initialize(WKBundleRef);

    static void didCreatePageCb(WKBundleRef, WKBundlePageRef, const void*);
    static void willDestroyPageCb(WKBundleRef, WKBundlePageRef, const void*);
    static void didReceiveMessageCb(WKBundleRef, WKStringRef messageName, WKTypeRef messageBody, const void *);

    void didCreatePage(WKBundlePageRef page);
    void willDestroyPage(WKBundlePageRef page);
    void didReceiveMessage(WKStringRef messageName, WKTypeRef messageBody);

private:
    WKBundleRef mBundle;
};

} // namespace luna

#endif
