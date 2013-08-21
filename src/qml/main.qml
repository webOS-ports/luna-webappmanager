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

import QtQuick 2.0
import QtWebKit 3.0
import QtWebKit.experimental 1.0
import "pluginmanager.js" as PluginManager

Item {
    id: root

    signal completed

    width: 320
    height: 480

    Flickable {
        id: webViewContainer
        anchors.fill: parent

        Image {
            id: loadingBackground

            anchors.fill: parent
            visible: !webapp.ready

            // we have must be above everything else
            z: 100

            source: "images/loading-bg.png"

            Image {
                id: loadingGlow
                anchors.centerIn: parent
                source: "images/loading-glow.png"
            }

            Image {
                id: appIcon
                anchors.centerIn: loadingGlow
                source: webapp.icon
            }

            SequentialAnimation {
                id: loadingAnimation
                running: true
                loops: Animation.Infinite

                NumberAnimation {
                    target: loadingGlow
                    properties: "opacity"
                    from: 0.1
                    to: 1.0
                    easing.type: Easing.Linear
                    duration: 700
                }

                NumberAnimation {
                    target: loadingGlow
                    properties: "opacity"
                    from: 1.0
                    to: 0.1
                    easing.type: Easing.Linear
                    duration: 700
                }
            }
    }

        WebView {
            id: webView
            objectName: "webView"

            anchors.fill: parent

            experimental.preferences.navigatorQtObjectEnabled: true
            experimental.preferences.localStorageEnabled: true
            experimental.preferences.offlineWebApplicationCacheEnabled: true
            experimental.preferences.webGLEnabled: true
            experimental.preferences.developerExtrasEnabled: true

            experimental.onMessageReceived: {
                PluginManager.messageHandler(message);
            }

            onLoadingChanged: {
                if (loadRequest.status) {
                    root.completed();
                    webapp.loadFinished();
                }
            }

            Component.onCompleted: {
                webView.url = webapp.url;
            }

            Connections {
                target: webapp

                onJavaScriptExecNeeded: {
                    console.log("Running script: "+script);
                    //webView.experimental.evaluateJavaScript(script);
                }

                onPluginWantsToBeAdded: {
                    PluginManager.addPlugin(name, object);
                }
            }
        }
    }
}
