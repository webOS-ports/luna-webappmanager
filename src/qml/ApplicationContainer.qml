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

import QtQuick 2.6
import QtWebEngine 1.4
import QtWebChannel 1.0
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
                webViewComponent.webView.reload();
        }
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
        webViewLoader.sourceComponent = webViewComponent;
    }

    Image  {
        anchors.fill: parent
        source: "images/loading-bg.png";
    }

    Loader {
        id: webViewLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: keyboardContainer.top
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
                    height: webViewContainer.height - (Qt.inputMethod ? Qt.inputMethod.keyboardRectangle.height/Screen.pixelDensity : 0)
                };

                // beware: async call
                webView.runJavaScript("if (window.Mojo && window.Mojo.positiveSpaceChanged) {" +
                                      "window.Mojo.positiveSpaceChanged(" + positiveSpace.width +
                                      "," + positiveSpace.height + ");}");

                if (Qt.inputMethod.visible && webAppWindow.focus)
                    keyboardContainer.height = Qt.inputMethod.keyboardRectangle.height/Screen.pixelDensity;
                else
                    keyboardContainer.height = 0;
            }

            Connections {
                target: Qt.inputMethod
                onVisibleChanged: webView._updateWebViewSize();
                onKeyboardRectangleChanged: webView._updateWebViewSize();
            }



            focus: true
            Connections {
                target: webAppWindow
                onFocusChanged: if(!webAppWindow.focus) webView.focus = false;
            }

            backgroundColor: (webAppWindow.windowType === "dashboard" || webAppWindow.windowType === "popupalert") ? "transparent": "white"

            userScripts: webAppWindow.userScripts;

            onNavigationRequested: {
                var action = WebEngineView.AcceptRequest;
                var url = request.url.toString();

                if (webApp.urlsAllowed && webApp.urlsAllowed.length !== 0) {
                    action = WebEngineView.IgnoreRequest;
                    for (var i = 0; i < webApp.urlsAllowed.length; ++i) {
                        var pattern = webApp.urlsAllowed[i];
                        if (url.match(pattern)) {
                            action = WebEngineView.AcceptRequest;
                            break;
                        }
                    }
                }

                request.action = action;

                // If we're not handling the URL forward it to be opened within the system
                // default web browser in a safe environment
                if (request.action === WebEngineView.IgnoreRequest) {
                    Qt.openUrlExternally(url);
                    return;
                }
            }

            Component.onCompleted: {
                // Let the native side configure us as needed
                webAppWindow.configureWebView(webView);
                webView.webChannel = webViewChannel;


                // LunaWebEngineView will set the standard UA already, we only overwrite it when appinfo.json provides one.
                if(webApp.userAgent.length > 0){
                    webView.profile.httpUserAgent = webApp.userAgent;
                }
                // Only when we have a system application we enable the webOS API and the
                // PalmServiceBridge to avoid remote applications accessing unwanted system
                // internals
                if (webAppWindow.trustScope === "system") {
                    webView.settings.localContentCanAccessFileUrls = true;
                    webView.settings.localContentCanAccessRemoteUrls = true;

                    console.warn("webApp.allowCrossDomainAccess: " + webApp.allowCrossDomainAccess);
                    if (webApp.allowCrossDomainAccess) {
                        if (webView.settings.hasOwnProperty("appRuntime"))
                            webView.settings.appRuntime = false;
                    }
                    else {
                        if (webView.settings.hasOwnProperty("appRuntime"))
                            webView.settings.appRuntime = true;
                    }
                }

                //if (webView.settings.hasOwnProperty("logsPageMessagesToSystemConsole"))
                //    webView.settings.logsPageMessagesToSystemConsole = true;

                //if (webView.settings.hasOwnProperty("suppressIncrementalRendering"))
                //    webView.settings.suppressIncrementalRendering = true;
            }

            WebChannel {
                id: webViewChannel
            }
            
            Connections {
                target: webAppWindow

                onJavaScriptExecNeeded: {
                    // beware: async call
                    webView.runJavaScript(script);
                }

                onExtensionWantsToBeAdded: {
                    console.warn("registering " + name + "to WebChannel: " + object);
                    webViewChannel.registerObject(name, object);
                }
            }

            onRenderProcessTerminated: {
                // Whatever the reason, the render process should never stop when we are still alive
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

    Item {
        id: keyboardContainer
        height: 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
