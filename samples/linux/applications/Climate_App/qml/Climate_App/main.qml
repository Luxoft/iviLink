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


// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1


Rectangle {
    id: page
    width: 800
    height: 600
    z: -2147483647

    //buttons property
    property string src: ""
    property string empty_btn: "empty_btn.png"

    Connections {
        target: ClimateStateUpdater

        onShowClimate:{
            console.log("onShowClimate");
            ac_mode.visible = true;
        }
        onAutoState:{
            console.log("onAutoState");
            auto_btn_on.visible = status;
        }
        onRecircState:{
            console.log("onRecircState");
            recirc.visible = status;
        }
        onAcState:{
            console.log("onAcState");
            ac_btn.visible = status;
        }
        onFrontState:{
            console.log("onFrontState");
            front_btn.visible = status;
        }
        onRearState:{
            console.log("onRearState");
            rear_btn.visible = status;
        }
        onFanLowState:{
            console.log("onFanLowState");
            mode_low_btn.visible = status;
        }
        onFanMidState:{
            console.log("onFanMidState");
            mode_mid_btn.visible = status;
        }
        onFanHighState:{
            console.log("onFanHighState");
            mode_top_btn.visible = status;
        }
        onPassTemp:{
            console.log("onPassTemp");
            pass_temp.text = temp+" C";
        }
        onDriverTemp:{
            console.log("onDriverTemp");
            drive_temp.text = temp + " C"
        }
        onFanSpeed:{
            var i=0;
            for(i=0;i<7;i++){
                speed_storage.images[i].visible=(i<speed);
            }
        }
        onSyncState:{
            console.log("onSyncState");
            sync_btn.visible = status;
        }
        onFanOffState:{
            console.log("onFanOffState");
            off_btn.visible = status;
        }

        onSigShowLoadingScreen:{
            console.log("SHOW LOADING SCREEN");
            waiting_rectangle.visible = true
        }

        onSigHideLoadingScreen:{
            console.log("HIDE LOADING SCREEN");
            waiting_rectangle.visible = false
        }
    }

    Connections {
        target: ClimateRequestProcessor
    }

    function tempDecDrTog()
    {
        ClimateRequestProcessor.tempDecDrRequest();
    }

    function tempIncDrTog()
    {
        ClimateRequestProcessor.tempIncDrRequest();
    }

    function tempDecPassTog()
    {
        ClimateRequestProcessor.tempDecPassRequest();
    }

    function tempIncPassTog()
    {
        ClimateRequestProcessor.tempIncPassRequest();
    }

    function syncTog()
    {
        ClimateRequestProcessor.syncRequest();
    }

    function fanLowTog()
    {
        ClimateRequestProcessor.fanLowRequest();
    }

    function fanMidTog()
    {
        ClimateRequestProcessor.fanMidRequest();
    }

    function fanHighTog()
    {
        ClimateRequestProcessor.fanHighRequest();
    }

    function fanIncTog()
    {
        ClimateRequestProcessor.fanIncRequest();
    }

    function fanDecTog()
    {
        ClimateRequestProcessor.fanDecRequest();
    }

    function rearTog()
    {
        ClimateRequestProcessor.rearRequest();
    }

    function frontTog()
    {
        ClimateRequestProcessor.frontRequest();
    }

    function acTog()
    {
        ClimateRequestProcessor.acRequest();
    }

    function recircTog()
    {
        ClimateRequestProcessor.recircRequest();
    }

    function autoTog()
    {
        ClimateRequestProcessor.autoRequest();
    }

    function offTog()
    {
        ClimateRequestProcessor.offRequest();
    }

    function exitBtnPressed()
    {
        console.log("exitBtnPressed()");
        ClimateRequestProcessor.exitRequest();
    }


    Rectangle {
        id: waiting_rectangle
        x: 0
        y: 0
        width: 800
        height: 600
        color: "#112036"
        z: 1000
        visible: false

        Text {
            id: waiting_text
            x: 104
            y: 252
            width: 577
            height: 85
            text: qsTr("waiting for connection...")
            font.pixelSize: 53
        }
        MouseArea{
            anchors.fill: parent
        }
    }

    Rectangle { //ac_mode
        id: ac_mode
        x: 0
        y: 0
        width: 800
        height: 600
        color: "#00000000"
        z: 100
        visible: true

        Text {
            id: drive_temp
            x: 270
            y: 287
            width: 67
            height: 41
            color: "#ffffff"
            text: qsTr("20 C")
            z: 1
            style: Text.Normal
            font.family: "Ubuntu Condensed"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 31
        }

        MouseArea {
            id: mode_top_mouse
            x: 597
            y: 190
            width: 95
            height: 119
            z: 97
            onClicked: {
                fanHighTog();
            }
            /*onPressed:
            {
                src =  mode_top_btn.source;
                mode_top_btn.source = "gl-p-05.png"
                mode_top_btn.visible = true;
            }
            onReleased:
            {
                mode_top_btn.visible = false;
                 mode_top_btn.source = src;
            }*/

        }

	Image {
            id: exitButton
            x: 710
            y: 541
            source: "back.png"

            MouseArea {
                id: exit_mouse
                x: 0
                y: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                onClicked:{
                    exitBtnPressed();
                }
            }
        }


        Image {
            id: drive_temp_down
            x: 256
            y: 362
            width: 95
            height: 95
            rotation: 180
            z: 3
            source: "str-01.png"

            MouseArea {
                id: drive_temp_down_mouse
                anchors.fill: parent
                z: 96
                onClicked:  {
                    tempDecDrTog();
                }
                onPressed:
                {
                    src =  drive_temp_down.source;
                    drive_temp_down.source = "str-a-01.png"
                }
                onReleased:
                {
                    drive_temp_down.source = src;
                }
            }
        }

        Image {
            id: drive_temp_up
            x: 256
            y: 151
            width: 95
            height: 95
            z: 3
            source: "str-01.png"
            visible: true

            MouseArea {
                id: drive_temp_up_mouse
                anchors.fill: parent
                z: 95
                visible: true
                onClicked:  {
                    tempIncDrTog();
                }
                onPressed:
                {
                    src =  drive_temp_up.source;
                    drive_temp_up.source = "str-a-01.png"
                }
                onReleased:
                {
                    drive_temp_up.source = src;
                }
            }
        }

        Image {
            id: pass_temp_down
            x: 705
            y: 362
            width: 95
            height: 95
            rotation: 180
            z: 3
            source: "str-01.png"

            MouseArea {
                id: pass_temp_down_mouse
                anchors.fill: parent
                z: 98
                onClicked:  {
                    tempDecPassTog();
                }
                onPressed:
                {
                    src =  pass_temp_down.source;
                    pass_temp_down.source = "str-a-01.png"
                }
                onReleased:
                {
                    pass_temp_down.source = src;
                }
            }
        }

        Image {
            id: pass_temp_up
            x: 705
            y: 151
            width: 95
            height: 95
            z: 2
            visible: true
            source: "str-01.png"

            MouseArea {
                id: pass_temp_up_mouse
                anchors.fill: parent
                z: 106
                visible: true
                onClicked:  {
                    tempIncPassTog();
                }
                onPressed:
                {
                    src =  pass_temp_up.source;
                    pass_temp_up.source = "str-a-01.png"
                }
                onReleased:
                {
                    pass_temp_up.source = src;
                }
            }
        }


        Image {
            id: mode_top_btn
            x: 546
            y: 151
            width: 197
            height: 217
            source: "g-a-01.png"
        }

        MouseArea {
            id: mode_low_mouse
            x: 361
            y: 190
            width: 95
            height: 119
            z: 92
            onClicked: {
                fanLowTog();
            }
            /*onPressed:
            {
                src =  mode_low_btn.source;
                mode_low_btn.source = "f-p-05.png"
                mode_low_btn.visible = true;
            }
            onReleased:
            {
                mode_low_btn.visible = false;
                 mode_low_btn.source = src;
            }*/
        }

        Image {
            id: mode_low_btn
            x: 310
            y: 151
            width: 197
            height: 217
            scale: 1
            rotation: 0
            opacity: 1

            source: "f-a-01.png"
        }

        Image {
            id: mode_mid_btn
            x: 429
            y: 151
            width: 197
            height: 217
            source: "h-a-01.png"
        }

        MouseArea {
            id: rear_mouse
            x: 642
            y: 1
            width: 156
            height: 68
            z: 119
            onClicked:  {
                rearTog();
            }
            /*onPressed:
            {
                src =  rear_btn.source;
                rear_btn.source = "rear-p-04.png"
                rear_btn.visible = true;
            }
            onReleased:
            {
                rear_btn.visible = false;
                 rear_btn.source = src;
            }*/
        }

        Image {
            id: rear_btn
            x: 642
            y: 0
            width: 157
            height: 74
            opacity: 1
            z: 118
            source: "rear-a-01.png"
        }

        MouseArea {
            id: front_mouse
            x: 482
            y: 0
            width: 157
            height: 74
            z: 117
            onClicked:  {
                frontTog();
            }
            /*onPressed:
            {
                src =  front_btn.source;
                front_btn.source = "front-p-04.png"
                front_btn.visible = true;
            }
            onReleased:
            {
                front_btn.visible = false;
                 front_btn.source = src;
            }*/
        }

        Image {
            id: front_btn
            x: 482
            y: 0
            width: 157
            height: 74
            z: 116
            source: "front-a-01.png"
        }

        MouseArea {
            id: ac_mouse
            x: 320
            y: 0
            width: 157
            height: 74
            z: 115
            onClicked:  {
                acTog();
            }
            /*onPressed:
            {
                src =  ac_btn.source;
                ac_btn.source = "ac-a-01.png"
                ac_btn.visible = true;
            }
            onReleased:
            {
                ac_btn.visible = false;
                 ac_btn.source = src;
            }*/
        }

        Image {
            id: ac_btn
            x: 320
            y: 0
            width: 157
            height: 74
            z: 112
            source: "ac-a-01.png"
        }

        MouseArea {
            id: recirc_mouse
            x: 160
            y: 0
            width: 156
            height: 74
            z: 114
            onClicked:  {
                recircTog();
            }
            /*onPressed:
            {
                src =  recirc.source;
                recirc.source = "in-p-04.png"
                recirc.visible = true;
            }
            onReleased:
            {
                recirc.visible = false;
                 recirc.source = src;
            }*/

        }

        Image {
            id: recirc
            x: 160
            y: 0
            width: 157
            height: 74
            z: 120
            source: "in-a-01.png"
        }

        MouseArea {
            id: auto_mouse
            x: 0
            y: 1
            width: 156
            height: 73
            z: 111
            onClicked:  {
                autoTog();
            }
            /*onPressed:
            {
                src =  auto_btn_on.source;
                auto_btn_on.source = "auto-p-04.png"
                auto_btn_on.visible = true;
            }
            onReleased:
            {
                auto_btn_on.visible = false;
                 auto_btn_on.source = src;
            }*/

        }

        Image {
            id: auto_btn_on
            x: 0
            y: 0
            width: 157
            height: 74
            z: 110
            anchors.rightMargin: -235
            anchors.bottomMargin: -132
            anchors.leftMargin: 235
            anchors.topMargin: 132
            source: "auto-a-01.png"


        }


        Image {
            id: bg_image
            x: 0
            y: -1
            width: 800
            height: 601
            z: -1
            source: "b-01.png"


        }

        Text {
            id: pass_temp
            x: 719
            y: 287
            width: 67
            height: 41
            color: "#ffffff"
            text: qsTr("20 C")
            z: 93
            font.family: "Ubuntu Condensed"
            font.pixelSize: 31
            horizontalAlignment: Text.AlignHCenter
        }

        Image {
            id: dr_temp_bord
            x: 265
            y: 280
            width: 78
            height: 48
            source: "border-01.png"
        }

        Image {
            id: pass_temp_bord
            x: 716
            y: 280
            source: "border-01.png"
        }

        Image {
            id: dug
            x: 284
            y: 410
            width: 487
            height: 131
            source: "dug-01.png"

            Image {
                id: fan_fast_im
                x: 301
                y: 93
                width: 55
                height: 55
                source: "pl-01.png"

                MouseArea {
                    id: fan_fast_mouse
                    anchors.fill: parent
                    onClicked: {
                        fanIncTog();
                    }
                }
            }

            Image {
                id: fan_slow_im
                x: 134
                y: 93
                width: 55
                height: 55
                source: "min-01.png"

                MouseArea {
                    id: fan_slow_mouse
                    x: 0
                    y: 0
                    anchors.rightMargin: 0
                    anchors.bottomMargin: 0
                    anchors.leftMargin: 0
                    anchors.topMargin: 0
                    anchors.fill: parent
                    onClicked: {
                        fanDecTog();
                    }
                }
            }

            Image {
                id: fan
                x: 197
                y: 65
                source: "kl-01.png"
            }

            Image {
                id: sync_btn
                x: 374
                y: 57
                width: 60
                height: 59
                source: "sync-03.png"
                visible: false
            }

            MouseArea {
                id: sync_mouse
                clip: true

                x: 374
                y: 57
                width: 60
                height: 59
                z: 100
                onClicked: {
                    syncTog();
                }
            }


            Image {
                id: sync_btn_back
                x: 374
                y: 57
                width: 60
                height: 59
                source: "sync-01.png"
                visible: true
            }

            Image {
                id: off_btn
                x: 51
                y: 57
                width: 60
                height: 59

                source: "off-03.png"
            }

            MouseArea {
                id: off_mouse
                x: 51
                y: 57
                width: 60
                height: 59
                z: 100
                onClicked: {
                    offTog();
                }
            }

            Image {
                id: off_btn_back
                x: 51
                y: 57
                width: 60
                height: 59
                source: "off-01.png"
                visible: true
            }

            Image {
                id: speed_1
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v1-01.png"
            }

            Image {
                id: speed_2
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v2-01.png"
            }

            Image {
                id: speed_3
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v3-01.png"
            }

            Image {
                id: speed_4
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v4-01.png"
            }

            Image {
                id: speed_5
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v5-01.png"
            }

            Image {
                id: speed_6
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v6-01.png"
            }

            Image {
                id: speed_7
                x: 106
                y: 6
                width: 276
                height: 80
                source: "v7-01.png"
            }

            Image {
                id: speed_0
                x: 106
                y: 6
                source: "v0-01.png"
            }

            Item{
                id: speed_storage
                property variant images: [speed_1, speed_2, speed_3, speed_4, speed_5, speed_6, speed_7]
            }
        }

        Image {
            id: image1
            x: 310
            y: 151
            width: 197
            height: 217
            source: "f-01.png"
        }

        Image {
            id: image2
            x: 429
            y: 151
            width: 197
            height: 217
            source: "h-01.png"
        }

        Image {
            id: image3
            x: 546
            y: 151
            width: 197
            height: 217
            source: "g-01.png"
            visible: true
        }

        MouseArea {
            id: mode_mid_mouse
            x: 480
            y: 190
            width: 95
            height: 119
            z: 94
            onClicked: {
                fanMidTog();
            }
            /*onPressed:
        {
            src =  mode_mid_btn.source;
            mode_mid_btn.source = "l-p-05.png"
            mode_mid_btn.visible = true;
        }
        onReleased:
        {
            mode_mid_btn.visible = false;
            mode_mid_btn.source = src;
        }*/
        }


        Image {
            id: auto_back
            x: 0
            y: 0
            source: "auto-01.png"
        }

        Image {
            id: rear_back
            x: 642
            y: 0
            source: "rear-01.png"
        }

        Image {
            id: recir_back
            x: 160
            y: 0
            source: "in-01.png"
        }

        Image {
            id: front_back
            x: 482
            y: 0
            source: "front-01.png"
        }

        Image {
            id: ac_back
            x: 320
            y: 0
            source: "ac-01.png"
        }

    }
}
