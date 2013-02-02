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

    property string src: ""
    property int driver_back_angle:    0   //-60 - +80 grad
    property int passen_back_angle:    0   //-60 - +80 grad
    property int initial_driver_back_angle:  0  //drive_back.rotation    //-60 - +80 grad
    property int angle_const:  20               //drive_back.rotation    //-60 - +80 grad
    property string empty_btn: "empty_btn.png"


    Connections {
        target: CStateUpdater

        onInitRequestDone:{

            timer_start.running = false;
        }        

        onShowSeat:{
            chair_mode.visible = true
        }

        onHeaterDriver:{
            //console.log("drv heater="+heat);
            var i=0;
            driver_heater_img_off.visible = true
            for(i=0;i<3;i++){
                driver_heater_img_storage.images[i].visible=(i<heat);
            }
        }
        onHeaterPass:{
            //console.log("pass heater="+heat);
            var i=0;
            pass_heater_img_off.visible = true
            for(i=0;i<3;i++){
                pass_heater_img_storage.images[i].visible=(i<heat);
            }
        }
        onCurrentSeatViewPass: {
            pass_seat_btn_pressed.visible = true;
            pass_seat_btn.visible = false;
            drive_seat_btn_pressed.visible = false;
            drive_seat_btn.visible = true;
        }
        onCurrentSeatViewDriver:{
            pass_seat_btn_pressed.visible = false;
            pass_seat_btn.visible = true;
            drive_seat_btn_pressed.visible = true;
            drive_seat_btn.visible = false;
        }
        onBottomX: {
            console.log("onDriver_bottom_xChanged");
            console.log(pos.toString());
            drive_bottom.x = pos;
        }
        onBottomY: {
            console.log("onDriver_bottom_yChanged");
            console.log(pos.toString());
            drive_bottom.y = pos;
        }
        onBackX: {
            console.log("onDriver_bottom_xChanged");
            console.log(pos.toString());
            drive_back.x = pos;
        }
        onBackY: {
            console.log("onDriver_bottom_yChanged");
            console.log(pos.toString());
            drive_back.y = pos;
        }
        onBackAngle: {
            console.log("onDriver_back_angleChanged");
            console.log(driver_back_angle.toString());
            drive_back.rotation = angle - angle_const;
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
        target: CRequestProcessor
    }

    function driverHeaterTog()
    {
        CRequestProcessor.driverHeaterRequest();
    }

    function passHeaterTog()
    {
        CRequestProcessor.passHeaterRequest();
    }

    function driverSeatTog()
    {
        CRequestProcessor.driverSeatRequest();
    }

    function passSeatTog()
    {
        CRequestProcessor.passSeatRequest();
    }

    function seatBackLeft()
    {
        CRequestProcessor.backLeftRequest();
    }

    function seatBackRight()
    {
        CRequestProcessor.backRightRequest();
    }

    function seatBottomUp()
    {
        CRequestProcessor.bottomUpRequest();
    }

    function seatBottomDown()
    {
        CRequestProcessor.bottomDownRequest();
    }

    function seatBottomLeft()
    {
        CRequestProcessor.bottomLeftRequest();
    }

    function seatBottomRight()
    {
        CRequestProcessor.bottomRightRequest();
    }

    function exitBtnPressed()
    {
        console.log("exitBtnPressed()");
        CRequestProcessor.exitRequest();
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

    Rectangle { //chair_mode
        id: chair_mode
        x: 0
        y: 0
        width: 800
        height: 600
        color: "#ffffff"
        z: 93
        visible: true

        Image {
            id: bg_chair
            x: 0
            y: 0
            width: 800
            height: 600
            visible: true
            smooth: true
            source: "b-01.png"
        }

        Image {
            id: driver_heater_img_off
            x: 262
            y: 243
            width: 161
            height: 60
            z: 11
            source: "t-1-01.png"
        }

        Image {
            id: driver_heater_img_1
            x: 262
            y: 243
            width: 161
            height: 60
            z: 12
            source: "t-2-01.png"
        }

        Image {
            id: driver_heater_img_2
            x: 262
            y: 243
            width: 161
            height: 60
            z: 13
            source: "t-3-01.png"
        }

        Image {
            id: driver_heater_img_3
            x: 262
            y: 243
            width: 161
            height: 60
            z: 14
            source: "t-4-01.png"
        }

        Item{
            id:driver_heater_img_storage
            property variant images: [driver_heater_img_1, driver_heater_img_2,driver_heater_img_3]
            visible: true
        }

        MouseArea {
            id: driver_heater_mouse
            x: 262
            y: 243
            width: 161
            height: 60
            z: 15
            onClicked: {
                //set the climate mode
                driverHeaterTog();
            }
            /*onPressed: {
                driver_heater_img_off.source = "";
            }
            onReleased: {
                driver_heater_img_off.source = "h1-06.png";
            }*/
        }

        Image {
            id: pass_heater_img_off
            x: 262
            y: 322
            width: 161
            height: 60
            z: 6
            source: "t-1-01.png"
        }

        Image {
            id: pass_heater_img_1
            x: 262
            y: 322
            width: 161
            height: 60
            z: 7
            source: "t-2-01.png"
        }

        Image {
            id: pass_heater_img_2
            x: 262
            y: 322
            width: 161
            height: 60
            z: 8
            source: "t-3-01.png"
        }

        Image {
            id: pass_heater_img_3
            x: 262
            y: 322
            width: 161
            height: 60
            z: 9
            source: "t-4-01.png"
        }

        Item{
            id:pass_heater_img_storage
            property variant images: [pass_heater_img_1, pass_heater_img_2,pass_heater_img_3]
            x: 99
            y: -43
            visible: false
        }

        MouseArea {
            id: pass_heater_mouse
            x: 262
            y: 323
            width: 161
            height: 60
            z: 10
            onClicked: {
                //set the climate mode
                passHeaterTog();
            }
            /*onPressed: {
                 pass_heater_img_off.source = "";
            }
            onReleased: {
                 pass_heater_img_off.source = "h1-06.png";
            }*/
        }

        Image { //choose driver inactive
            id: drive_seat_btn
            x: 262
            y: 197
            width: 161
            height: 59
            z: 5
            source: "dr-01.png"
        }

        Image { //choose driver active
            id: drive_seat_btn_pressed
            x: 262
            y: 197
            width: 161
            height: 59
            z: 106
            source: "dr-a-01.png"
        }

        MouseArea {
            id: drivers_seat_mouse
            x: 262
            y: 197
            width: 161
            height: 59
            z: 107
            onClicked: {
                //seat events
                driverSeatTog()
            }
            /*onPressed: {
                drive_seat_btn_pressed.source = "";
            }
            onReleased: {
                 drive_seat_btn_pressed.source = "driversseat_active.png";
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

        Image { //chair driver back
            id: drive_back
            x: 535
            y: 197
            width: 106
            height: 239
            z: 96
            rotation: 0
            source: "seat_back_2_big.png"

            //            Image {
            //                id: drive_back_gray
            //                x: 14
            //                y: 0d
            //                visible: false
            //                z: -1
            //                opacity: 0.500
            //                source: "seat_back_gray.png"
            //            }
        }

        Image { //chaitr driver bottom
            id: drive_bottom
            x: 474
            y: 322
            width: 127
            height: 30
            z: 94
            source: "seat_bottom_1.png"

            //            Image {
            //                id: drive_bottom_gray
            //                x: -53
            //                y: 0
            //                visible: false
            //                z: -1
            //                opacity: 0.500
            //                source: "seat_bottom_gray.png"
            //            }
        }

        Image { //chair back move left
            id: chair_back_left
            x: 408
            y: 65
            width: 78
            height: 78
            rotation: -105
            z: 102
            source: "str-01.png"
            signal repeatedClick();

            MouseArea {
                id: chair_back_left_mouse
                x: 0
                y: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                onClicked: {
                    seatBackLeft()
                }
                onPressed:
                {
                    src =  chair_back_left.source;
                    chair_back_left.source = "str-a-01.png"
                    chair_back_left.visible = true;
                    timer_back_left.running = true;
                }
                onReleased:
                {
                    timer_back_left.running = false;
                    chair_back_left.source = src;
                }
            }

            Timer{
                id: timer_back_left
                interval: 100; running: false; repeat: true
                onTriggered: chair_back_left.repeatedClick();
            }

            onRepeatedClick: seatBackLeft();
        }


        Image { //chair move up
            id: bottom_up
            x: 674
            y: 180
            width: 78
            height: 78
            z: 101
            source: "str-01.png"
            signal repeatedClick();

            MouseArea {
                id: chair_back_up_mouse
                anchors.fill: parent

                onClicked:
                {
                    seatBottomUp()
                }
                onPressed:
                {
                    src =  bottom_up.source;
                    bottom_up.source =  "str-a-01.png"
                    bottom_up.visible = true;
                    timer_up.running = true;
                }
                onReleased:
                {
                    timer_up.running = false;
                    bottom_up.source = src;
                }
            }

            Timer{
                id: timer_up
                interval: 100; running: false; repeat: true
                onTriggered: bottom_up.repeatedClick();
            }

            onRepeatedClick: seatBottomUp();
        }

        Image { //chair move down
            id: bottom_down
            x: 674
            y: 354
            width: 78
            height: 78
            rotation: 180
            z: 100
            source: "str-01.png"
            signal repeatedClick();

            MouseArea {
                id: chair_back_down_mouse
                x: 0
                y: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                onClicked: {
                    seatBottomDown()
                }
                onPressed:
                {
                    src =  bottom_down.source;
                    bottom_down.source =  "str-a-01.png"
                    bottom_down.visible = true;
                    timer_down.running = true;
                }
                onReleased:
                {
                    timer_down.running = false;
                    bottom_down.source = src;
                }

            }

            Timer{
                id: timer_down
                interval: 100; running: false; repeat: true
                onTriggered: bottom_down.repeatedClick();
            }
            onRepeatedClick: seatBottomDown();
        }

        Image { //chair bottom move left
            id: bottom_left
            x: 426
            y: 453
            width: 78
            height: 78
            rotation: -90
            z: 99
            source: "str-01.png"
            signal repeatedClick();

            MouseArea {
                id: bottom_left_mouse
                x: 0
                y: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                onClicked:
                {
                    seatBottomLeft()
                }
                onPressed:
                {
                    src =  bottom_left.source;
                    bottom_left.source =  "str-a-01.png"
                    bottom_left.visible = true;
                    timer_left.running = true;
                }
                onReleased:
                {
                    timer_left.running = false;
                    bottom_left.source = src;
                }
            }

            Timer{
                id: timer_left
                interval: 100; running: false; repeat: true
                onTriggered: bottom_left.repeatedClick();
            }
            onRepeatedClick: seatBottomLeft();
        }

        Image { //chair bottom move rtight
            id: bottom_right
            x: 587
            y: 453
            width: 78
            height: 78
            rotation: 90
            z: 98
            source: "str-01.png"
            signal repeatedClick();

            MouseArea {
                id: bottom_right_mouse
                anchors.fill: parent
                onClicked: {
                    seatBottomRight()
                }
                onPressed:
                {
                    src =  bottom_right.source;
                    bottom_right.source =   "str-a-01.png"
                    bottom_right.visible = true;
                    timer_right.running = true;
                }
                onReleased:
                {
                    timer_right.running = false;
                    bottom_right.source = src;
                }
            }
            Timer{
                id: timer_right
                interval: 100; running: false; repeat: true
                onTriggered: bottom_right.repeatedClick();
            }
            onRepeatedClick: seatBottomRight();
        }

        Image { //chair back move right
            id: chair_back_right
            x: 579
            y: 65
            width: 78
            height: 78
            rotation: 105
            z: 97
            source: "str-01.png"
            signal repeatedClick();


            MouseArea {
                id: chair_back_right_mouse
                anchors.fill: parent
                onClicked: {
                    seatBackRight()
                }
                onPressed:
                {
                    src =  chair_back_right.source;
                    chair_back_right.source =   "str-a-01.png"
                    chair_back_right.visible = true;
                    timer_back_right.running = true;
                }
                onReleased:
                {
                    timer_back_right.running = false;
                    chair_back_right.source = src;
                }
            }

            Timer{
                id: timer_back_right
                interval: 100; running: false; repeat: true
                onTriggered: chair_back_right.repeatedClick();
            }
            onRepeatedClick: seatBackRight();
        }

        Image { //pass seat
            id: pass_seat_btn
            x: 262
            y: 371
            width: 161
            height: 59
            z: 103
            source: "pass-01.png"
        }

        Image { //pass seat
            id: pass_seat_btn_pressed
            x: 262
            y: 370
            width: 161
            height:59
            source: "pass-a-01.png"
            z: 3
        }

        MouseArea {
            id: passengers_seat_mouse
            x: 262
            y: 371
            width: 161
            height: 59
            z: 105
            onClicked: {
                passSeatTog()
            }
        }

    }
    //EOF CHAIR MODE
}
