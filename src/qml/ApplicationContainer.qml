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
import QtWebEngine 1.1
import QtWebEngine.experimental 1.0
import QtWebChannel 1.0
import QtWebSockets 1.0
import Qt.labs.settings 1.0
import LunaNext.Common 0.1
import LuneOS.Components 1.0
import Connman 0.2
import "."

Flickable {
   id: webViewContainer

   anchors.fill: parent

   property int numRestarts: 0
   property int maxRestarts: 3

   NetworkManager {
       id: networkManager

       property string oldState: "unknown"

       onStateChanged: {
           // When we are online again reload the web view in order to start the application
           // which is still visible to the user
           if (webApp.internetConnectivityRequired &&
               oldState !== networkManager.state &&
               networkManager.state === "online")
               webView.reload();
       }
   }
   
   WebSocketServer {
      id: webAppMgrSockerServer
      listen: true
   }

    Rectangle {
        id: offlinePanel

        color: "white"
        visible: webApp.internetConnectivityRequired && networkManager.state !== "online"
        anchors.fill: parent

        z: 10

        Text {
            anchors.centerIn: parent
            color: "black"
            text: "Internet connectivity is required but not available"
            font.pixelSize: 20
            font.family: "Prelude"
        }
    }

    Component.onCompleted: {
        if (webApp.isLauncher())
            return;

        webViewLoader.sourceComponent = webViewComponent;
    }

    Loader {
        id: webViewLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: keyboardContainer.top
    }

    Connections {
        target: webAppWindow
        onVisibleChanged: {
            if (!webApp.isLauncher())
                return;

            if (!webAppWindow.visible)
                return;

            if (webViewLoader.sourceComponent !== null)
                return;

            webViewLoader.sourceComponent = webViewComponent;
        }
    }

    Component {
        id: webViewComponent

        LunaWebEngineView {
            id: webView
            objectName: "webView"

            function _updateWebViewSize() {
                    // beware: async call
                    webView.runJavaScript("if (window.Mojo && window.Mojo.keyboardShown) {" +
                                                            "window.Mojo.keyboardShown(" + Qt.inputMethod.visible + ");}");

                    var positiveSpace = {
                        width: webViewContainer.width,
                        height: webViewContainer.height - (Qt.inputMethod ? Qt.inputMethod.keyboardRectangle.height : 0)
                    };

                    // beware: async call
                    webView.runJavaScript("if (window.Mojo && window.Mojo.positiveSpaceChanged) {" +
                                                            "window.Mojo.positiveSpaceChanged(" + positiveSpace.width +
                                                            "," + positiveSpace.height + ");}");

                    if (Qt.inputMethod.visible && webAppWindow.focus)
                        keyboardContainer.height = Qt.inputMethod.keyboardRectangle.height;
                    else
                        keyboardContainer.height = 0;
            }

            Connections {
                target: Qt.inputMethod
                onVisibleChanged: _updateWebViewSize();
                onKeyboardRectangleChanged: _updateWebViewSize();
            }

            UserAgent {
                id: userAgent
            }

            //experimental.transparentBackground: (webAppWindow.windowType === "dashboard" ||
            //                                     webAppWindow.windowType === "popupalert")

            function getUserAgentForApp(url) {
                /* if the app wants a specific user agent assign it instead of the default one */
                if (webApp.userAgent.length > 0)
                    return webApp.userAgent;

                return userAgent.defaultUA;
            }

           profile.httpUserAgent: getUserAgentForApp(null)
           userScripts: webAppWindow.userScripts;
           property alias devicePixelRatio: experimental.viewport.devicePixelRatio

            /*
            onNavigationRequested: {
                var action = WebView.AcceptRequest;
                var url = request.url.toString();

                if (webApp.urlsAllowed && webApp.urlsAllowed.length !== 0) {
                    action = WebView.IgnoreRequest;
                    for (var i = 0; i < webApp.urlsAllowed.length; ++i) {
                        var pattern = webApp.urlsAllowed[i];
                        if (url.match(pattern)) {
                            action = WebView.AcceptRequest;
                            break;
                        }
                    }
                }

                request.action = action;

                // If we're not handling the URL forward it to be opened within the system
                // default web browser in a safe environment
                if (request.action === WebView.IgnoreRequest) {
                    Qt.openUrlExternally(url);
                    return;
                }

                webView.experimental.userAgent = getUserAgentForApp(url);
            }
            */
            Component.onCompleted: {
                // Let the native side configure us as needed
                webAppWindow.configureWebView(webView);
                /*
                // Only when we have a system application we enable the webOS API and the
                // PalmServiceBridge to avoid remote applications accessing unwanted system
                // internals
                if (webAppWindow.trustScope === "system") {
                    if (experimental.preferences.hasOwnProperty("palmServiceBridgeEnabled"))
                        experimental.preferences.palmServiceBridgeEnabled = true;

                    if (experimental.preferences.hasOwnProperty("privileged"))
                        experimental.preferences.privileged = webApp.privileged;

                    if (experimental.preferences.hasOwnProperty("identifier"))
                        experimental.preferences.identifier = webApp.identifier;

                    if (webApp.allowCrossDomainAccess) {
                        if (experimental.preferences.hasOwnProperty("appRuntime"))
                            experimental.preferences.appRuntime = false;

                        experimental.preferences.universalAccessFromFileURLsAllowed = true;
                        experimental.preferences.fileAccessFromFileURLsAllowed = true;
                    }
                    else {
                        if (experimental.preferences.hasOwnProperty("appRuntime"))
                            experimental.preferences.appRuntime = true;

                        experimental.preferences.universalAccessFromFileURLsAllowed = false;
                        experimental.preferences.fileAccessFromFileURLsAllowed = false;
                    }
                }

                if (experimental.preferences.hasOwnProperty("logsPageMessagesToSystemConsole"))
                    experimental.preferences.logsPageMessagesToSystemConsole = true;

                if (experimental.preferences.hasOwnProperty("suppressIncrementalRendering"))
                    experimental.preferences.suppressIncrementalRendering = true;
                */
            }

            webChannel: WebChannel {
               id: webViewChannel
            }
            
            WebSocket {
               id: webViewSocket
               url: webAppMgrSockerServer.url
               active: true
               onStatusChanged: if(status === WebSockets.Open) webViewChannel.connectTo(webViewSocket);
            }
            
            Connections {
                target: webAppWindow

                onJavaScriptExecNeeded: {
                    // beware: async call
                    webView.runJavaScript(script);
                }

                onExtensionWantsToBeAdded: {
                    webView.webChannel.registerObject(name, object);
                }
            }

            Connections {
                target: webView.experimental
                onProcessDidCrash: {
                    if (numRestarts < maxRestarts) {
                        console.log("ERROR: The web process has crashed. Restart it ...");
                        webView.url = webAppWindow.url;
                        webView.reload();
                        numRestarts += 1;
                    }
                    else {
                        console.log("CRITICAL: restarted application " + numRestarts
                                    + " times. Closing it now");
                        Qt.quit();
                    }
                }
            }
        }
    }

    Item {
        id: keyboardContainer
        height: 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
