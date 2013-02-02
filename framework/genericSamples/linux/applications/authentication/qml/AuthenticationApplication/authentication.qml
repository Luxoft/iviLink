/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 
import QtQuick 1.1
import QtQuick 1.0

Rectangle {
    id: authHost
    x: 0
    y: 0
    width: 316
    height: 346
    color: "#00FFFFFF"
    visible: false

    Connections {
        target: QMLAuthenticationDialog

        onSigShowNumPad:{
            authHost.visible = true;
        }

        onSigHideNumPad:{
            authHost.visible = false;
        }

        onSigShowRetryPopup:{
            authFailedBackground.visible = true
            authFailedHideTimer.start()
        }

        onSigResetNumpad:{
            pinText.text = "";
        }

        onSigLockInput:{
            okButtonMouse.enabled = false
            okButton.source = "images/pin/ok-grey.png"
            backspaceMouse.enabled = false
            backspace.source = "images/pin/backspace-grey.png"
            zeroMouse.enabled = false
            zero.source = "images/pin/0-grey.png"
            oneMouse.enabled = false
            one.source = "images/pin/1-grey.png"
            twoMouse.enabled = false
            two.source = "images/pin/2-grey.png"
            threeMouse.enabled = false
            three.source = "images/pin/3-grey.png"
            fourMouse.enabled = false
            four.source = "images/pin/4-grey.png"
            fiveMouse.enabled = false
            five.source = "images/pin/5-grey.png"
            sixMouse.enabled = false
            six.source = "images/pin/6-grey.png"
            sevenMouse.enabled = false
            seven.source = "images/pin/7-grey.png"
            eightMouse.enabled = false
            eight.source = "images/pin/8-grey.png"
            nineMouse.enabled = false
            nine.source = "images/pin/9-grey.png"
        }

        onSigUnlockInput:{
            zeroMouse.enabled = true
            zero.source = "images/pin/0.png"
            oneMouse.enabled = true
            one.source = "images/pin/1.png"
            twoMouse.enabled = true
            two.source = "images/pin/2.png"
            threeMouse.enabled = true
            three.source = "images/pin/3.png"
            fourMouse.enabled = true
            four.source = "images/pin/4.png"
            fiveMouse.enabled = true
            five.source = "images/pin/5.png"
            sixMouse.enabled = true
            six.source = "images/pin/6.png"
            sevenMouse.enabled = true
            seven.source = "images/pin/7.png"
            eightMouse.enabled = true
            eight.source = "images/pin/8.png"
            nineMouse.enabled = true
            nine.source = "images/pin/9.png"
        }

        onSigLockOkButton:{
            okButtonMouse.enabled = false
            okButton.source = "images/pin/ok-grey.png"
        }

        onSigUnlockOkButton:{
            okButtonMouse.enabled = true
            okButton.source = "images/pin/ok.png"
        }

        onSigLockBackspaceButton:{
            backspaceMouse.enabled = false
            backspace.source = "images/pin/backspace-grey.png"
        }

        onSigUnlockBackspaceButton:{
            backspaceMouse.enabled = true
            backspace.source = "images/pin/backspace.png"
        }

        onSigSetTexbox:{
            pinText.text = text
        }
    }

    Image {
        id: background
        fillMode: Image.TileHorizontally
        anchors.fill: parent
        anchors.centerIn: parent
        source: "images/pin/background.png"

        Image {
            id: backspace
            x: 204
            y: 64
            source: "images/pin/backspace.png"

            Image {
                id: backspaceLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: backspaceMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onBackspaceButtonClicked();
                }
                onPressed:
                {
                    backspaceLight.visible = true;
                }

                onReleased:
                {
                    backspaceLight.visible = false;
                }
            }
        }

        Image {
            id: pinTextBoxBackground
            x: 20
            y: 64
            source: "images/pin/input.png"

            Text {
                id: pinText
                anchors.fill: parent
                color: "#ffffff"
                text: qsTr("1234")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 30
                font.bold: false
            }
        }

        Image {
            id: zero
            x: 111
            y: 279
            source: "images/pin/0.png"

            Image {
                id: zeroLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: zeroMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(0);
                }
                onPressed:
                {
                    zeroLight.visible = true;
                }
                onReleased:
                {
                    zeroLight.visible = false;
                }

            }
        }

        Image {
            id: nine
            x: 211
            y: 226
            source: "images/pin/9.png"
            Image {
                id: nineLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: nineMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(9);
                }
                onPressed:
                {
                    nineLight.visible = true;
                }
                onReleased:
                {
                    nineLight.visible = false;
                }

            }
        }

        Image {
            id: two
            x: 111
            y: 120
            source: "images/pin/2.png"
            Image {
                id: twoLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: twoMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(2);
                }
                onPressed:
                {
                    twoLight.visible = true;
                }
                onReleased:
                {
                    twoLight.visible = false;
                }

            }
        }

        Image {
            id: three
            x: 211
            y: 120
            source: "images/pin/3.png"
            Image {
                id: threeLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: threeMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(3);
                }
                onPressed:
                {
                    threeLight.visible = true;
                }
                onReleased:
                {
                    threeLight.visible = false;
                }

            }
        }

        Image {
            id: four
            x: 11
            y: 173
            source: "images/pin/4.png"
            Image {
                id: fourLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: fourMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(4);
                }
                onPressed:
                {
                    fourLight.visible = true;
                }
                onReleased:
                {
                    fourLight.visible = false;
                }

            }
        }

        Image {
            id: one
            x: 11
            y: 120
            source: "images/pin/1.png"
            Image {
                id: oneLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: oneMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(1);
                }
                onPressed:
                {
                    oneLight.visible = true;
                }
                onReleased:
                {
                    oneLight.visible = false;
                }

            }
        }

        Image {
            id: five
            x: 111
            y: 173
            source: "images/pin/5.png"
            Image {
                id: fiveLight
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: fiveMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(5);
                }
                onPressed:
                {
                    fiveLight.visible = true;
                }
                onReleased:
                {
                    fiveLight.visible = false;
                }

            }
        }

        Image {
            id: six
            x: 211
            y: 173
            source: "images/pin/6.png"
            Image {
                id: sixLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: sixMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(6);
                }
                onPressed:
                {
                    sixLight.visible = true;
                }
                onReleased:
                {
                    sixLight.visible = false;
                }

            }
        }

        Image {
            id: seven
            x: 11
            y: 226
            source: "images/pin/7.png"
            Image {
                id: sevenLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: sevenMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(7);
                }
                onPressed:
                {
                    sevenLight.visible = true;
                }
                onReleased:
                {
                    sevenLight.visible = false;
                }

            }
        }

        Image {
            id: eight
            x: 111
            y: 226
            source: "images/pin/8.png"
            Image {
                id: eightLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: eightMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onNumberClicked(8);
                }
                onPressed:
                {
                    eightLight.visible = true;
                }
                onReleased:
                {
                    eightLight.visible = false;
                }

            }
        }

        Image {
            id: okButton
            x: 11
            y: 279
            width: 95
            height: 53
            source: "images/pin/ok.png"

            Image {
                id: okButtonLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible: false
            }

            MouseArea {
                id: okButtonMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onOKButtonClicked();
                }
                onPressed:
                {
                    okButtonLight.visible = true;
                }
                onReleased:
                {
                    okButtonLight.visible = false;
                }
            }
        }

        Image {
            id: cancelButton
            x: 211
            y: 279
            width: 95
            height: 53
            source: "images/pin/cancel.png"

            Image {
                id: cancelButtonLight
                anchors.fill: parent
                source: "images/pin/light.png"
                visible:false
            }

            MouseArea {
                id: cancelButtonMouse
                anchors.fill: parent
                onClicked: {
                    QMLAuthenticationDialog.onCancelButtonClicked();
                }
                onPressed:
                {
                    cancelButtonLight.visible = true;
                }
                onReleased:
                {
                    cancelButtonLight.visible = false;
                }
            }
        }
    }
    Image{
        id: authFailedBackground
        anchors.fill: parent
        anchors.centerIn: parent
        source: "images/pin/background-gray.png"
        visible: false
        Image{
            id: authFailedImage
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            anchors.centerIn: parent
            source: "images/pin/auth_failed.png"
        }
        MouseArea{
            id: authFailedMouse
            anchors.fill: parent
            onClicked: {
                console.log("nop")
            }
        }
        Timer{
            id: authFailedHideTimer
            running: false
            interval: 2500
            repeat: false
            onTriggered: {
                authFailedBackground.visible = false
            }
        }

    }
}
