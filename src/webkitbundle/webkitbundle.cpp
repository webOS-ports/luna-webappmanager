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

#include "webkitbundle.h"

namespace luna
{

WebkitBundle::WebkitBundle()
{
}

WebkitBundle::~WebkitBundle()
{
}

void WebkitBundle::initialize(WKBundleRef bundle)
{
    mBundle = bundle;

    WKBundleClient client = {
        kWKBundleClientCurrentVersion,
        this,
        didCreatePageCb,
        willDestroyPageCb,
        0, // didInitializePageGroup
        didReceiveMessageCb,
        0, // didReceiveMessageToPage
    };

    WKBundleSetClient(mBundle, &client);
}

void WebkitBundle::didCreatePageCb(WKBundleRef, WKBundlePageRef page, const void *clientInfo)
{
    static_cast<WebkitBundle*>(const_cast<void*>(clientInfo))->didCreatePage(page);
}

void WebkitBundle::willDestroyPageCb(WKBundleRef, WKBundlePageRef page, const void *clientInfo)
{
    static_cast<WebkitBundle*>(const_cast<void*>(clientInfo))->willDestroyPage(page);
}

void WebkitBundle::didReceiveMessageCb(WKBundleRef bundle, WKStringRef messageName, WKTypeRef messageBody, const void *clientInfo)
{
    static_cast<WebkitBundle*>(const_cast<void*>(clientInfo))->didReceiveMessage(messageName, messageBody);
}

void WebkitBundle::didCreatePage(WKBundlePageRef page)
{
}

void WebkitBundle::willDestroyPage(WKBundlePageRef page)
{
}

void WebkitBundle::didReceiveMessage(WKStringRef messageName, WKTypeRef messageBody)
{
}

} // namespace luna
