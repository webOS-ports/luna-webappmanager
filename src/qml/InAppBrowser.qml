/*
 * Copyright (C) 2013-2015 Simon Busch <morphis@gravedo.de>
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
import LunaNext.Common 0.1

Item {
    id: browserView

    property string title: webView.title
    property string url: ""

    signal done

    anchors.fill: parent
    z: 10

    Rectangle {
        id: background
        anchors.fill: parent
        color: "black"
        opacity: 0.5
    }

    WebView {
        id: webView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: doneButton.top
        anchors.margins: Units.gu(1)
        url: browserView.url
        experimental.transparentBackground: false
    }

    Item {
        id: doneButton

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Units.gu(1)
        height: Units.gu(4)

        Image {
            source: "qrc:///qml/images/palm-notification-button.png"
            anchors.fill: parent
            smooth: true
        }

        Text {
            text: "Done"

            anchors.fill: parent
            anchors.margins: parent.height * 0.3

            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#FFF"
            font.bold: true
            font.pixelSize: FontUtils.sizeToPixels("medium")
            font.family: "Prelude"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                done();
            }
        }
    }
}
