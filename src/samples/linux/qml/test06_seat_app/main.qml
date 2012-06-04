/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */



// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtQuick 1.0
import VideoContainer 1.0

Rectangle {
    id: page
    width: 800
    height: 480
    z: -2147483647


   //seat properties
    property int current_seat_view:    0    //default: 0 - driver, 1 - passen.

    property int driver_bottom_x:      0   //+-20
    property int driver_bottom_y:      0   //+-20
    property int driver_back_angle:    0   //-60 - +80 grad

    property int passen_bottom_x:      0   //+-20
    property int passen_bottom_y:      0   //+-20
    property int passen_back_angle:    0   //-60 - +80 grad

    //initial states
    property int initial_driver_bottom_x:    389//drive_bottom.x         //+-20
    property int initial_driver_bottom_y:    285//drive_bottom.y         //+-20
    property int initial_driver_back_x:      481//drive_back.x           //+-20
    property int initial_driver_back_y:      139//drive_back.y           //+-20
    property int initial_driver_back_angle:  0//drive_back.rotation    //-60 - +80 grad
    property int angle_const:  20               //drive_back.rotation    //-60 - +80 grad

    property int pin_numbers:  0
    property string pincode_visible:  ""
    property int pincode_digits:  0
    property string pincode_str: ""

    property bool startedPopulatingSongs: false;
    property bool startedPopulatingRemoteSongs: false;
    property bool startedPopulatingVids: false;


    onCurrent_seat_viewChanged: {
        if(current_seat_view == 1)  //passen
        {
            pass_seat_btn_pressed.visible = true;
            pass_seat_btn.visible = false;
            drive_seat_btn_pressed.visible = false;
            drive_seat_btn.visible = true;

            drive_back.x = initial_driver_back_x - passen_bottom_x;
            drive_bottom.x = initial_driver_bottom_x - passen_bottom_x;
            drive_back.y = initial_driver_back_y - passen_bottom_y;
            drive_bottom.y = initial_driver_bottom_y - passen_bottom_y;
            drive_back.rotation = initial_driver_back_angle + passen_back_angle - angle_const;
        }
        else    //driver
        {
            pass_seat_btn_pressed.visible = false;
            pass_seat_btn.visible = true;
            drive_seat_btn_pressed.visible = true;
            drive_seat_btn.visible = false;

            drive_back.x = initial_driver_back_x - driver_bottom_x;
            drive_bottom.x = initial_driver_bottom_x - driver_bottom_x;
            drive_back.y = initial_driver_back_y - driver_bottom_y;
            drive_bottom.y = initial_driver_bottom_y - driver_bottom_y;
            drive_back.rotation = initial_driver_back_angle + driver_back_angle - angle_const;
        }

    }

    onDriver_bottom_xChanged: {
        console.log("onDriver_bottom_xChanged");
        console.log(driver_bottom_x.toString());
        drive_back.x = initial_driver_back_x - driver_bottom_x;
        drive_bottom.x = initial_driver_bottom_x - driver_bottom_x;
    }
    onDriver_bottom_yChanged: {
        console.log("onDriver_bottom_yChanged");
        console.log(driver_bottom_y.toString());
        drive_back.y = initial_driver_back_y - driver_bottom_y;
        drive_bottom.y = initial_driver_bottom_y - driver_bottom_y;
    }
    onDriver_back_angleChanged: {
        console.log("onDriver_back_angleChanged");
        console.log(driver_back_angle.toString());
        drive_back.rotation = initial_driver_back_angle + driver_back_angle - angle_const;
    }
    onPassen_bottom_xChanged: {
        console.log("onPassen_bottom_xChanged");
        console.log(passen_bottom_x.toString());
        drive_back.x = initial_driver_back_x - passen_bottom_x;
        drive_bottom.x = initial_driver_bottom_x - passen_bottom_x;
    }
    onPassen_bottom_yChanged: {
        console.log("onPassen_bottom_yChanged");
        console.log(passen_bottom_y.toString());
        drive_back.y = initial_driver_back_y - passen_bottom_y;
        drive_bottom.y = initial_driver_bottom_y - passen_bottom_y;
    }
    onPassen_back_angleChanged: {
        console.log("onPassen_back_angleChanged");
        console.log(passen_back_angle.toString());
        drive_back.rotation = initial_driver_back_angle + passen_back_angle - angle_const;
    }
    onPin_numbersChanged: {
        if (pin_numbers >= 4)
        {
            //enough

            pin_numbers = 0;
            pincode_visible = pin_text.text;
            console.log("pin code: " + pincode_visible);
            pin_code.visible = false;
            pincode_str=pin_text.text;
            //pinReady(pin_text.text);
            pinReady();
            //pin_text.text="";
        }
    }


    ////////////////////////////////////////////

    Connections {
        target: BtnProcessor

        onRecirc: {
            //console.log("onAirRecircChanged "+status)
            recirc.visible = status;
        }
        onAutoSig:{
            //console.log("onAuto"+status)
            auto_btn_on.visible = status;
        }
        onOff:{
            //console.log("onOff"+status)
            off_btn.visible = status;
        }
        onDual:{
            //console.log("onDual "+status)
            sync_btn.visible = !status;
        }
        onFanLow:{
            //console.log("onFanlow "+status)
            mode_low_btn.visible = status;
        }
        onFanMiddle:{
            //console.log("onFanmiddle "+status)
            mode_mid_btn.visible = status;
        }
        onFanTop:{
            //console.log("onFantop "+status)
            mode_top_btn.visible = status;
        }
        onAc:{
            //console.log("onAc "+status)
            ac_btn.visible = status;
        }
        onHead:{
            //console.log("onHead "+status)
            front_btn.visible = status;
        }
        onRear:{
            //console.log("onRear "+status)
            rear_btn.visible = status;
        }
        onPassTemp:{
            //console.log("pass temp is "+temp);
            pass_temp.text = temp+" C";
        }
        onDrvTemp:{
            //console.log("drv temp is" + temp)
            drive_temp.text=temp + " C"
        }
        onFanSpeed:{
            //console.log("fna speed is "+speed)
            var i=0;
            for(i=0;i<7;i++){
                speed_storage.images[i].visible=(i<speed);
            }
        }
        onActivitySeat:{
            ac_mode.visible = false
            chair_mode.visible = true
            console.log("onActivitySeat");
        }
        onActivityClimate:{
            chair_mode.visible = false
            player_mode_audio.visible = false;
            ac_mode.visible = true
            console.log("onActivityClimate")
        }
        onPinCode:{
            pin_code.visible = true
            pin_numbers = 0;
            pin_text = "";
            pincode_digits = 0;
            pincode_visible = "";
        }
        onVnc:{
            var chairVis=chair_mode.visible;
            console.log("chair mode visibility: "+chairVis);
            vnc_dialog.visible = true;
            chair_mode.visible=chairVis; //todo something less stupid
        }
        onGetPlayerMode:{
            BtnProcessor.stateChanged(1);
            player_mode_audio.visible = true;
            player_mode_video.visible=false
            ac_mode.visible = false;
            chair_mode.visible = false;
        }
        onAudioPage:{
            player_mode_audio.visible = true;
            player_mode_video.visible = false;
            ac_mode.visible = false;
            chair_mode.visible = false;
            stopActivityVideo(); //stops all audio and video
        }
        onVideoPage:{
            player_mode_video.visible = true;
            ac_mode.visible = false;
            chair_mode.visible = false;
            player_mode_audio.visible =false;
            stopActivityAudio();
        }
        /*onDisconnectPLayer:{
            stop();
            vStop();
            ac_mode.visible = true;
        }*/


        onHeater:{
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
        //this is for audio
        onListPopulate:{
            console.log("onListPopulate");
            if(startedPopulatingSongs==false){
                playListModel.clear();
                startedPopulatingSongs=true;
                console.log(startedPopulatingSongs);
                startedPopulatingRemoteSongs=false;
            }
            console.log("got name "+songName);
            playListModel.append({"name":songName});
        }

        onTrackListShare:{
            if(startedPopulatingRemoteSongs==false){
                playListModel.clear();
                startedPopulatingSongs=false;
                startedPopulatingRemoteSongs=true;
            }
            playListModel.append({"name":songName});

        }

        onSwapPlayPause:{
            var temp = pause_im_au.visible;
            pause_im_au.visible=play_im_au.visible;
            play_im_au.visible=temp;
        }

        onPlaylistPosChanged:{
            playlistSongs.currentIndex=pos
        }
        //video logic

        onVidTrackListShare:{
            videoPlayListModel.clear();
            videoPlayListModel.append({"name":vidName});
            console.log("got name "+vidName);
            startedPopulatingVids=false;
        }

        onVideoListPopulate:{
            if(startedPopulatingVids==false){
                videoPlayListModel.clear();
                startedPopulatingVids=true;
            }
            videoPlayListModel.append({"name":vidName});
            console.log("got name "+vidName);
        }

        onVidSwapPlayPause:{
            var temp= pause_im_vi.visible
            pause_im_vi.visible=play_im_vi.visible
            play_im_vi.visible=temp
        }

        onVidSwapViews:{
            var temp = playlistVids.visible
            if(!temp){
                playlistVids.currentIndex=-1
            }

            playlistVids.visible=video.visible
            video.visible=temp

        }
    }


    ///////////////////////////////////////////////




    //seat events
    function seatBottomUp() {
        BtnProcessor.bottomUp();
    }
    function seatBottomDown(){
        BtnProcessor.bottomDown();
    }
    function seatBottomLeft(){
        //BtnProcessor.bottomLeft();
        BtnProcessor.bottomRight();
    }
    function seatBottomRight(){
        //BtnProcessor.bottomRight();
        BtnProcessor.bottomLeft();
    }
    function seatBackLeft(){
        BtnProcessor.backLeft();
    }
    function seatBackRight(){
        BtnProcessor.backRight();
    }
    function driverHeaterTog(){
        BtnProcessor.driverHeaterToggle();
    }
    function passHeaterTog(){
        BtnProcessor.passHeaterToggle();
    }
    function driverSeatTog(){
        BtnProcessor.driverSeatToggle();
    }
    function passenSeatTog(){
        BtnProcessor.passenSeatToggle();
    }


    ///////////ac events
    function passTempIncTog() {
        //text.text = BtnProcessor.echo(rect.text) // calling BtnProcessor::echo
        BtnProcessor.passTempIncToggle();
    }
    function fanIncTog() {
        BtnProcessor.fanIncToggle();
    }
    function fanDecTog() {
        BtnProcessor.fanDecToggle();
    }
    function driveTempIncTog() {
        BtnProcessor.driveTempIncToggle();
    }
    function driveTempDecTog() {
        BtnProcessor.driveTempDecToggle();
    }
    function passTempDecTog() {
        BtnProcessor.passTempDecToggle();
    }
    function acTog() {
        BtnProcessor.acToggle();
    }
    function frontWTog() {
        BtnProcessor.frontWToggle();
    }
    function rearWTog() {
        BtnProcessor.rearWToggle();
    }
    function offTog() {
        BtnProcessor.offToggle();
    }
    function dualTog() {
        BtnProcessor.dualToggle();
    }
    function autoTog() {
        BtnProcessor.autoToggle();
    }
    function fanModeLowTog() {
        BtnProcessor.fanModeLowToggle();
    }

    function fanModeMedTog() {
        BtnProcessor.fanModeMedToggle();
    }
    function fanModeHighTog() {
        BtnProcessor.fanModeHighToggle();
    }
    function recircTog() {
        BtnProcessor.recircToggle();
    }
    function climateMode() {
        BtnProcessor.climateMode();
    }
    function  getClimate() {
        chair_mode.visible = false
        player_mode_audio.visible = false;
        player_mode_video.visible = false;
        ac_mode.visible = true;
        stopActivityVideo(); //stops all audio and video
        stopActivityAudio();
    }
    function  getChair() {
        chair_mode.visible = true;
        player_mode_audio.visible = false;
        player_mode_video.visible = false;
        ac_mode.visible = false;
        stopActivityVideo(); //stops all audio and video
        stopActivityAudio();
    }

    function playerMode() {
        player_mode_audio.visible = true;
        BtnProcessor.stateChanged(1);
        player_mode_video.visible=false
        ac_mode.visible = false;
        chair_mode.visible = false;
    }

    function audioPage() {
        if(player_mode_audio.visible!=true){
            BtnProcessor.stateChanged(1);
            player_mode_audio.visible = true;
            player_mode_video.visible = false;
            ac_mode.visible = false;
            chair_mode.visible = false;
            stopActivityVideo(); //stops all audio and video
        }
    }
    function videoPage() {
        if(player_mode_video.visible!=true){
            BtnProcessor.stateChanged(2);
            player_mode_video.visible = true;
            ac_mode.visible = false;
            chair_mode.visible = false;
            player_mode_audio.visible =false;
            stopActivityAudio();
        }
    }

    function stopActivityVideo(){
        vStop();
        play_im_vi.visible=true;
        pause_im_vi.visible=false;
        video.visible=false;
        playlistVids.visible=true;
    }

    function stopActivityAudio(){
        stop();
        play_im_au.visible=true;
        pause_im_au.visible=false;
    }
    //share player

    function sharePlayer(){
        console.log("share player")
        BtnProcessor.sharePlayer();
    }

    function getTracks(){
        if(startedPopulatingSongs==false)
            BtnProcessor.populateList_share();
        if(startedPopulatingVids==false)
            BtnProcessor.videoPopulateList_share();

    }

    //audio functions

    function play(){
        BtnProcessor.play();
    }

    function stop(){
        BtnProcessor.stop();
    }

    function pause(){
        BtnProcessor.pause();
    }

    function next(){
        BtnProcessor.next();
    }

    function prev(){
        BtnProcessor.prev();
    }

    function volUp(){
        BtnProcessor.volUp();
    }

    function volDn(){
        BtnProcessor.volDn();
    }

    function posClicked(pos){
        var i=parseInt(pos);
        BtnProcessor.posClicked(i);
    }

    //end of audio functions

    //video functions

    function vPlay(){
        BtnProcessor.vPlay();
    }

    function vStop(){
        BtnProcessor.vStop();
    }

    function vPause(){
        BtnProcessor.vPause();
    }

    function vNext(){
        BtnProcessor.vNext();
    }

    function vPrev(){
        BtnProcessor.vPrev();
    }

    function vVolUp(){
        BtnProcessor.vVolUp();
    }

    function vVolDn(){
        BtnProcessor.vVolDn();
    }

    function vPosClicked(pos){
        var i=parseInt(pos);
        BtnProcessor.vPosClicked(i);
    }

    //end of video functions

    function chairMode() {
        BtnProcessor.chairMode();
    }
    function vncShare() {
        BtnProcessor.vncShare();
    }

    function pinReady() {
        BtnProcessor.pinReady();
    }

    ////////////////////////
    function vnc_vis() {
        vnc_dialog.visible = true;
    }
    function vnc_req() {
        BtnProcessor.vnc_req();
    }


    Rectangle {
        id: chair_mode
        x: 0
        y: 0
        width: 800
        height: 480
        color: "#ffffff"
        z: 93

        Rectangle {
            id: hider_driver
            x: 150
            y: 8
            width: 631
            height: 461
            color: "#585752"
            z: 1
            border.color: "#585752"
        }

        Image {
            id: bg_chair
            x: 0
            y: 1
            width: 800
            height: 480
            source: "begr-2-02.png"


        }


        MouseArea {
            id: mouse_share
            x: 5
            y: 379
            width: 135
            height: 98
            z: 107
            onClicked: {
                //set thevnc mode
                //vnc_vis();
                vnc_req();
            }
        }

        Image {
            id: share_img
            x: 5
            y: 376
            width: 135
            height: 101
            visible: true
            source: "share_main_button_inactive.png"
        }


        Image {
            id: seat_btn
            x: 5
            y: 175
            width: 135
            height: 101
            source: "seat-a-03.png"
        }

        Image {
            id: climate_btn_2
            x: 5
            y: 276
            width: 135
            height: 101
            source: "clim-03.png"

            MouseArea {
                id: climate_btn_mouse
                x: 0
                y: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                onClicked: {
                 //set the climate mode
                 climateMode();
                 }
            }


        }

        Image {
            id: driver_heater_img_off
            x: 201
            y: 112
            width: 137
            height: 59
            z: 11
            source: "h1-06.png"
        }

        Image {
            id: driver_heater_img_1
            x: 202
            y: 112
            width: 137
            height: 59
            z: 12
            source: "h2-06.png"
        }

        Image {
            id: driver_heater_img_2
            x: 201
            y: 112
            width: 137
            height: 59
            z: 13
            source: "h3-06.png"
        }

        Image {
            id: driver_heater_img_3
            x: 201
            y: 112
            width: 137
            height: 59
            z: 14
            source: "h4-06.png"
        }

        Item{
            id:driver_heater_img_storage
            property variant images: [driver_heater_img_1, driver_heater_img_2,driver_heater_img_3]
        }

        MouseArea {
            id: driver_heater_mouse
            x: 202
            y: 112
            width: 137
            height: 59
            z: 15
            onClicked: {
                //set the climate mode
                driverHeaterTog();
            }
        }

        Image {
            id: pass_heater_img_off
            x: 206
            y: 400
            width: 137
            height: 59
            z: 6
            source: "h1-06.png"
        }


        Image {
            id: pass_heater_img_1
            x: 206
            y: 400
            width: 137
            height: 59
            z: 7
            source: "h2-06.png"
        }


        Image {
            id: pass_heater_img_2
            x: 206
            y: 400
            width: 137
            height: 59
            z: 8
            source: "h3-06.png"
        }


        Image {
            id: pass_heater_img_3
            x: 206
            y: 400
            width: 137
            height: 59
            z: 9
            source: "h4-06.png"
        }

        Item{
            id:pass_heater_img_storage
            property variant images: [pass_heater_img_1, pass_heater_img_2,pass_heater_img_3]
        }


        MouseArea {
            id: pass_heater_mouse
            x: 206
            y: 400
            width: 137
            height: 59
            z: 10
            onClicked: {
             //set the climate mode
             passHeaterTog();
             }
        }

        Image {
            id: drive_seat_btn
            x: 186
            y: 42
            z: 5
            source: "driversseat.png"
        }
        Image {
            id: drive_seat_btn_pressed
            x: 186
            y: 42
            width: 168
            height: 55
            z: 106
            source: "driversseat_active.png"
        }
        MouseArea {
            id: drivers_seat_mouse
            x: 186
            y: 42
            width: 168
            height: 55
            z: 107
            onClicked: {
                //seat events
                driverSeatTog()
                }
            }

        Image {
            id: drive_back
            x: 463
            y: 132
            z: 96
            rotation: 0
            source: "seat_back_2_big.png"

            Image {
                id: drive_back_gray
                x: 14
                y: 0
                visible: false
                z: -1
                opacity: 0.500
                source: "seat_back_gray.png"
            }
        }

        Image {
            id: drive_bottom
            x: 389
            y: 285
            z: 94
            source: "seat_bottom_1.png"

            Image {
                id: drive_bottom_gray
                x: 9
                y: 9
                visible: false
                z: -1
                opacity: 0.500
                source: "seat_bottom_gray.png"
            }
        }

        Image {
            id: chair_back_left
            x: 442
            y: 38
            width: 82
            height: 77
            z: 102
            source: "l-s2-07.png"

            MouseArea {
                id: chair_back_left_mouse
                anchors.fill: parent
                onClicked: {
                    seatBackLeft()
                }
            }
        }

        Image {
            id: bottom_up
            x: 698
            y: 125
            width: 67
            height: 65
            z: 101
            source: "up-s-05.png"

            MouseArea {
                id: chair_back_up_mouse
                anchors.fill: parent
                onClicked:
                {
                    seatBottomUp()
                }
            }
        }

        Image {
            id: bottom_down
            x: 698
            y: 263
            width: 67
            height: 65
            z: 100
            source: "dn-s-05.png"

            MouseArea {
                id: chair_back_down_mouse
                anchors.fill: parent
                onClicked: {
                    seatBottomDown()
                }
            }
        }

        Image {
            id: bottom_left
            x: 442
            y: 359
            width: 67
            height: 65
            z: 99
            source: "l-s-05.png"

            MouseArea {
                id: bottom_left_mouse
                anchors.fill: parent
                onClicked:
                {
                    seatBottomLeft()
                }
            }
        }

        Image {
            id: bottom_right
            x: 645
            y: 359
            width: 67
            height: 65
            z: 98
            source: "r-s-05.png"

            MouseArea {
                id: bottom_right_mouse
                anchors.fill: parent
                onClicked: {
                    seatBottomRight()
                }
            }
        }

        Image {
            id: chair_back_right
            x: 630
            y: 38
            width: 82
            height: 77
            z: 97
            source: "r-s2-07.png"

            MouseArea {
                id: chair_back_right_mouse
                anchors.fill: parent
                onClicked: {
                    seatBackRight()
                }
            }
        }

        Image {
            id: pass_seat_btn
            x: 166
            y: 326
            z: 103
            source: "passengersseat.png"
        }
        Image {
            id: pass_seat_btn_pressed
            x: 166
            y: 326
            width: 207
            height: 55
            source: "passengersseat_active.png"
            z: 3
        }
        MouseArea {
            id: passengers_seat_mouse
            x: 166
            y: 326
            width: 207
            height: 55
            z: 105
            onClicked: {
                passenSeatTog()
            }
        }

        Image {
            id: player_image_ch
            x: 5
            y: 74
            width: 136
            height: 101
            source: "player_main_button_inactive.png"

            MouseArea {
                id: player_mouse_ch
                x: 5
                y: 1
                width: 125
                height: 100
                z: 109
                onClicked: {
                    playerMode()
                    getTracks()
                }
            }
        }


        }


    Rectangle {
            id: ac_mode
            x: 0
            y: 0
            width: 800
            height: 480
            color: "#00000000"
            z: 94


            Text {
                id: drive_temp
                x: 168
                y: 221
                width: 67
                height: 41
                color: "#ffffff"
                text: qsTr("20 C")
                style: Text.Normal
                font.family: "Ubuntu Condensed"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 31
            }

            MouseArea {
                id: fan_fast_mouse
                x: 592
                y: 397
                width: 88
                height: 73
                onClicked: {
                    fanIncTog();
                }
            }

            MouseArea {
                id: fan_slow_mouse
                x: 254
                y: 397
                width: 88
                height: 73
                onClicked: {
                    fanDecTog();
                }
            }

            MouseArea {
                id: mode_top_mouse
                x: 551
                y: 190
                width: 100
                height: 100
                z: 97
                onClicked: {
                    fanModeHighTog();
                }
            }

            MouseArea {
                id: drive_temp_down_mouse
                x: 151
                y: 313
                width: 100
                height: 80
                z: 96
                onClicked:  {
                    driveTempDecTog();
                }
            }

            Image {
                id: drive_temp_down
                x: 150
                y: 313
                width: 100
                height: 80
                source: "dn-05.png"
            }

            MouseArea {
                id: drive_temp_up_mouse
                x: 152
                y: 87
                width: 100
                height: 80
                z: 95
                visible: true
                onClicked:  {
                    driveTempIncTog();
                }
            }

            Image {
                id: drive_temp_up
                x: 151
                y: 87
                width: 100
                height: 80
                source: "up-05.png"
                visible: true
            }

            MouseArea {
                id: pass_temp_down_mouse
                x: 680
                y: 313
                width: 100
                height: 80
                z: 98
                onClicked:  {
                    passTempDecTog();
                }
            }

            Image {
                id: pass_temp_down
                x: 680
                y: 313
                width: 100
                height: 80
                source: "dn-05.png"
            }

            Image {
                id: mode_top_btn
                x: 551
                y: 190
                width: 100
                height: 100
                source: "gl-2-05.png"
            }

            MouseArea {
                id: mode_low_mouse
                x: 282
                y: 190
                width: 100
                height: 100
                z: 92
                onClicked: {
                    fanModeLowTog();
                }
            }

            Image {
                id: mode_low_btn
                x: 282
                y: 190
                width: 100
                height: 100
                z: 39
                source: "f-2-05.png"
            }

            MouseArea {
                id: mode_mid_mouse
                x: 417
                y: 190
                width: 100
                height: 100
                z: 94
                onClicked: {
                    fanModeMedTog();
                }
            }

            Image {
                id: mode_mid_btn
                x: 417
                y: 190
                width: 100
                height: 100
                source: "l-2-05.png"
            }

            MouseArea {
                id: sync_mouse
                x: 686
                y: 395
                width: 94
                height: 77
                z: 99
                onClicked: {
                    dualTog();
                }
            }

            Image {
                id: sync_btn
                x: 686
                y: 395
                width: 94
                height: 77
                visible: false
                source: "synk-a-05.png"
            }

            MouseArea {
                id: off_mouse
                x: 151
                y: 394
                width: 95
                height: 78
                z: 100
                onClicked: {
                    offTog();
                }
            }

            Image {
                id: off_btn
                x: 151
                y: 394
                width: 95
                height: 78
                source: "off-a-05.png"
            }

            MouseArea {
                id: rear_mouse
                x: 632
                y: 3
                width: 157
                height: 76
                z: 101
                onClicked:  {
                    rearWTog();
                }
            }

            Image {
                id: rear_btn
                x: 632
                y: 3
                width: 157
                height: 76
                source: "rear-2-04.png"
            }

            MouseArea {
                id: front_mouse
                x: 476
                y: 3
                width: 157
                height: 76
                z: 102
                onClicked:  {
                    frontWTog();
                }
            }

            Image {
                id: front_btn
                x: 476
                y: 3
                width: 157
                height: 76
                source: "front-2-04.png"
            }

            MouseArea {
                id: ac_mouse
                x: 320
                y: 4
                width: 157
                height: 76
                z: 103
                onClicked:  {
                    acTog();
                }
            }

            Image {
                id: ac_btn
                x: 320
                y: 2
                width: 157
                height: 76
                source: "ac-2-04.png"
            }

            MouseArea {
                id: recirc_mouse
                x: 164
                y: 2
                width: 157
                height: 76
                z: 104
                onClicked:  {
                    recircTog();
                }

            }

            Image {
                id: recirc
                x: 164
                y: 3
                width: 157
                height: 76
                source: "in-2-04.png"
            }

            MouseArea {
                id: auto_mouse
                x: 8
                y: 3
                width: 157
                height: 76
                z: 105
                onClicked:  {
                    autoTog();
                }
            }

            Image {
                id: auto_btn_on
                x: 8
                y: 3
                width: 157
                height: 76
                anchors.rightMargin: -235
                anchors.bottomMargin: -132
                anchors.leftMargin: 235
                anchors.topMargin: 132
                source: "auto-2-04.png"


            }

            MouseArea {
                id: pass_temp_up_mouse
                x: 681
                y: 87
                width: 100
                height: 80
                z: 106
                visible: true
                onClicked:  {
                    passTempIncTog();
                }
            }

            Image {
                id: pass_temp_up
                x: 680
                y: 87
                width: 100
                height: 80
                visible: true
                source: "up-05.png"
            }

            Image {
                id: climate_btn
                x: 7
                y: 279
                width: 130
                height: 98
                opacity: 1
                source: "clim-a-03.png"

                MouseArea {
                    id: mouse_area1
                    anchors.fill: parent
                }
            }

            Image {
                id: bg_image
                x: -1
                y: 0
                width: 800
                height: 480
                z: -1
                source: "ubuntu_kond-07.png"

                Image {
                    id: image1
                    x: 281
                    y: 190
                    width: 100
                    height: 100
                    source: "f-05.png"
                }

                Image {
                    id: image2
                    x: 416
                    y: 190
                    width: 100
                    height: 100
                    source: "l-05.png"
                }

                Image {
                    id: image3
                    x: 550
                    y: 190
                    width: 100
                    height: 100
                    source: "gl-05.png"
                }

                Rectangle {
                    id: rectangle2
                    x: 694
                    y: 162
                    width: 70
                    height: 155
                    color: "#585752"
                    border.color: "#585752"
                }

                Rectangle {
                    id: rectangle1
                    x: 159
                    y: 162
                    width: 78
                    height: 154
                    color: "#585752"
                    radius: 0
                    z: 0
                    border.color: "#585752"
                }

                Image {
                    id: image4
                    x: 346
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: image5
                    x: 380
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: image6
                    x: 414
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: image7
                    x: 449
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: image8
                    x: 484
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: image9
                    x: 519
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: image10
                    x: 554
                    y: 394
                    width: 35
                    height: 77
                    source: "vs-2-06.png"
                }

                Image {
                    id: speed_1
                    x: 346
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Image {
                    id: speed_2
                    x: 380
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Image {
                    id: speed_3
                    x: 414
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Image {
                    id: speed_4
                    x: 449
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Image {
                    id: speed_5
                    x: 484
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Image {
                    id: speed_6
                    x: 519
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Image {
                    id: speed_7
                    x: 554
                    y: 394
                    width: 35
                    height: 77
                    source: "vs1-06.png"
                }

                Item{
                    id: speed_storage
                    property variant images: [speed_1, speed_2, speed_3, speed_4, speed_5, speed_6, speed_7]
                }

                MouseArea {
                    id: seat_mode_mouse
                    x: 6
                    y: 179
                    width: 130
                    height: 98
                    onClicked: {
                        //set the climate mode
                        chairMode();
                    }

                }



            }

            Image {
                id: player_image_ac
                x: 4
                y: 79
                width: 135
                height: 98
                source: "player_main_button_inactive.png"

                MouseArea {
                    id: player_mouse_ac
                    x: 4
                    y: 2
                    width: 129
                    height: 95
                    onClicked: {
                        playerMode()
                        getTracks()
                    }

                }
            }

            Image {
                id: share_img1
                x: 4
                y: 377
                width: 135
                height: 98
                source: "share_main_button_inactive.png"
                visible: true

                MouseArea {
                    id: mouse_share1
                    anchors.fill: parent
                    z: 152
                    onClicked: {
                        //set the climate mode
                        //vnc_vis();
                        vnc_req();
                    }
                }
            }

            Text {
                id: pass_temp
                x: 697
                y: 221
                width: 67
                height: 41
                color: "#ffffff"
                text: qsTr("20 C")
                z: 93
                font.family: "Ubuntu Condensed"
                font.pixelSize: 31
                horizontalAlignment: Text.AlignHCenter
            }
    }





        Rectangle {
            id: pin_code
            x: 193
            y: 36
            width: 430
            height: 378
            color: "#2d2626"
            border.color: "#000000"
            z: 94
            visible: false

            Rectangle {
                id: pin_edit_box
                x: 109
                y: 115
                width: 212
                height: 62
                color: "#ffffff"
                rotation: 0

                Text {
                    id: pin_text
                    text: qsTr("")
                    font.bold: false
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    font.pixelSize: 54
                }
            }

            Text {
                id: please_pin
                x: 86
                y: 44
                width: 261
                height: 37
                color: "#ffffff"
                text: qsTr("Please enter pin")
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 32
            }

            Text {
                id: zero
                x: 109
                y: 207
                color: "#ffffff"
                text: qsTr("0")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_0
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 0;
                            //pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"0"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: one
                x: 156
                y: 207
                color: "#ffffff"
                text: qsTr("1")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_1
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 1;
                            //pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"1"
                        }
                        pin_numbers++;

                    }
                }
            }

            Text {
                id: two
                x: 204
                y: 207
                color: "#ffffff"
                text: qsTr("2")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_2
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 2;
                           // pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"2"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: three
                x: 253
                y: 207
                color: "#ffffff"
                text: qsTr("3")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_3
                    anchors.fill: parent
                    onClicked: {

                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 3;
                           // pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"3"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: four
                x: 298
                y: 207
                color: "#ffffff"
                text: qsTr("4")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_4
                    x: 0
                    y: 0
                    anchors.rightMargin: 0
                    anchors.bottomMargin: 0
                    anchors.leftMargin: 0
                    anchors.topMargin: 0
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 4;
                            //pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"4"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: five
                x: 109
                y: 278
                color: "#ffffff"
                text: qsTr("5")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_5
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 5;
                            //pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"5"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: six
                x: 157
                y: 278
                color: "#ffffff"
                text: qsTr("6")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_6
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 6;
                            //pin_text.text = pincode_digits.toString();
                            pin_text.text=pin_text.text+"6"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: seven
                x: 204
                y: 278
                color: "#ffffff"
                text: qsTr("7")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_7
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 7;
                            pin_text.text = pin_text.text+"7"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: eight
                x: 253
                y: 278
                color: "#ffffff"
                text: qsTr("8")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_8
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 8;
                            pin_text.text =pin_text.text+"8"
                        }
                        pin_numbers++;
                    }
                }
            }

            Text {
                id: nine
                x: 298
                y: 278
                color: "#ffffff"
                text: qsTr("9")
                font.pixelSize: 40

                MouseArea {
                    id: mouse_btn_9
                    anchors.fill: parent
                    onClicked: {
                        if(pin_numbers <4)
                        {
                            pincode_digits = pincode_digits*10 + 9;
                            //pin_text.text = pincode_digits.toString();
                            pint_text.text=pin_text.text+"9"
                        }
                        pin_numbers++;
                    }
                }
            }
        }



        Rectangle {
            id: vnc_dialog
            x: 193
            y: 184
            width: 421
            height: 208
            color: "#2d2626"
            border.color: "#000000"

            Text {
                id: vnc_ok
                x: 43
                y: 119
                width: 115
                height: 47
                color: "#ffffff"
                text: qsTr("Accept")
                font.pixelSize: 40
                MouseArea {
                    id: mouse_vnc_ok
                    width: 126
                    height: 47
                    anchors.rightMargin: -11
                    anchors.fill: parent
                    onClicked: {
                        vnc_dialog.visible = false;
                        vncShare();
                    }

                }
            }

            Text {
                id: vnc_cancel
                x: 254
                y: 119
                height: 47
                color: "#ffffff"
                text: qsTr("Decline")
                font.pixelSize: 40
                MouseArea {
                    id: mouse_vnc_cancel
                    anchors.fill: parent
                    onClicked: {
                        vnc_dialog.visible = false;
                    }
                }
            }

            Text {
                id: vnc_text
                x: 16
                y: 31
                width: 347
                height: 66
                color: "#ffffff"
                text: "Incoming VNC invitation"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                styleColor: "#000000"
                wrapMode: Text.NoWrap
                font.pixelSize: 33
            }
            visible: false
            z: 150
        }

        Rectangle {
            id: player_mode_audio
            x: 0
            y: 0
            width: 800
            height: 480
            color: "#ffffff"
            z: 91

            Image {
                id: player_audio
                x: 0
                y: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                source: "player_background.v1.png"

                Image {
                    id: player_image_au
                    x: 5
                    y: 77
                    width: 135
                    height: 98
                    source: "player_main_button_active.png"
                }

                Image {
                    id: climate_image_au
                    x: 5
                    y: 278
                    width: 135
                    height: 98
                    source: "clim-03.png"

                    MouseArea {
                        id: climate_mouse_au
                        anchors.fill: parent
                        onClicked: {
                         //set the climate mode
                         climateMode();
                         getClimate();
                         }

                    }
                }

                Image {
                    id: seat_image_au
                    x: 5
                    y: 175
                    width: 135
                    height: 103
                    source: "seat-p-03.png"

                    MouseArea {
                        id: seat_mouse_au
                        anchors.fill: parent
                        onClicked: {
                            //set the climate mode
                            chairMode();
                            getChair();
                        }
                    }
                }




                Image {
                    id: share_image_au
                    x: 5
                    y: 376
                    width: 135
                    height: 98
                    source: "share_main_button_inactive.png"
                    visible: true


                }

                MouseArea {
                    id: play_mouse_au
                    x: 5
                    y: 376
                    width: 135
                    height: 98
                    visible: true
                    z: 1
                    // anchors.fill: parent
                    onClicked:{
                        console.log("share image clicked");
                        sharePlayer();
                    }
                }
                Image {
                    id: play_im_au
                    x: 246
                    y: 403
                    width: 111
                    height: 71
                    source: "player_play_inactive.png"
                    visible: true
                    MouseArea {
                        id: share_mouse_au_a
                        anchors.fill: parent
                        onClicked:{
                            play();
                        }
                    }
                }




                /////////////////////////////////



                Image {
                    id: pause_im_au
                    x: 246
                    y: 403
                    width: 111
                    height: 71
                    source: "player_pause_inactive.png"
                    visible:false

                    MouseArea {
                        id: pause_mouse_au
                        anchors.fill: parent
                        onClicked: {
                            pause();
                        }
                    }
                }

                Image {
                    id: prev_im_au
                    x: 140
                    y: 403
                    width: 106
                    height: 71
                    source: "player_back_inactive.png"

                    MouseArea {
                        id: prev_mouse_au
                        anchors.fill: parent
                        onClicked:{
                            prev();
                        }
                    }
                }

                Image {
                    id: forward_im_au
                    x: 468
                    y: 403
                    width: 111
                    height: 71
                    source: "player_forward_inactive.png"

                    MouseArea {
                        id: forward_mouse_au
                        anchors.fill: parent
                        onClicked:{
                            next();
                        }
                    }
                }

                Image {
                    id: stop_im_au
                    x: 357
                    y: 403
                    width: 111
                    height: 71
                    source: "player_stop_inactive.png"

                    MouseArea {
                        id: stop_mouse_au
                        x: -111
                        y: 0
                        anchors.fill: parent
                        onClicked:{
                            stop();
                        }
                    }
                }

                Image {
                    id: vol_down_au
                    x: 579
                    y: 403
                    width: 111
                    height: 71
                    source: "player_volume-_inactive.png"

                    MouseArea {
                        id: vol_down_mouse_au
                        anchors.fill: parent
                        onClicked:{
                            volDn();
                        }
                    }
                }

                Image {
                    id: vol_up_au
                    x: 690
                    y: 403
                    width: 108
                    height: 71
                    source: "player_volume+_inactive.png"

                    MouseArea {
                        id: vol_up_mouse_au
                        anchors.fill: parent
                        onClicked:{
                            volUp();
                        }
                    }
                }

                Image {
                    id: music_im_au
                    x: 725
                    y: 5
                    width: 74
                    height: 187
                    source: "player_audio_active.png"
                }

                Image {
                    id: video_im_au
                    x: 725
                    y: 192
                    width: 74
                    height: 177
                    source: "player_video_inactive.png"

                    MouseArea {
                        id: video_mouse_au
                        anchors.fill: parent
                        onClicked: {
                            videoPage();
                        }
                    }
                }

                ListView {
                    id: playlistSongs
                    x: 150
                    y: 10
                    width: 575
                    height: 359
                    snapMode: ListView.SnapToItem
                    clip: true
                    highlightMoveSpeed: 836
                    delegate: Item {
                        x: 5
                        height: 40
                        width:parent.width
                        Row {
                            id: row2
                            spacing: 5
                            Text {
                                text: name
                                font.bold: true
                                width:575
                                elide:Text.ElideRight
                                font.pointSize: 24
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                            }
                        }
                        MouseArea {
                           anchors.fill: parent;
                           onPressed: {
                               if(playlistSongs.currentIndex!==index){
                                   playlistSongs.currentIndex = index
                               }
                               posClicked(playlistSongs.currentIndex);
                           }
                        }
                    }
                    currentIndex: -1
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                    focus: false
                    model: playListModel
                    Keys.onPressed: {
                        if(event.key===Qt.Key_Down){
                            console.log("overriding down key")
                        } else if(event===Qt.Key_Up){
                            console.log("overriding up key")
                        }
                    }

                }

                ListModel { //TODO refactor (idk how to specify prototype of ListElement without instantiating it)
                    id:playListModel
                    ListElement {
                        name: "Grey"
                    }
                }
            }

        }

        Rectangle {
            id: player_mode_video
            x: 0
            y: 0
            width: 800
            height: 480
            color: "#ffffff"
            objectName: "player_mode_video"
            Image {
                id: player_video
                x: 0
                y: 0
                anchors.fill: parent
                anchors.topMargin: 0
                Image {
                    id: player_image_vi
                    x: 5
                    y: 77
                    width: 135
                    height: 98
                    source: "player_main_button_active.png"
                }

                Image {
                    id: climate_image_vi
                    x: 5
                    y: 278
                    width: 135
                    height: 98
                    source: "clim-03.png"

                    MouseArea {
                        id: climate_mouse_vi
                        anchors.fill: parent
                        onClicked: {
                         //set the climate mode
                         climateMode();
                         getClimate();
                         }
                    }
                }

                Image {
                    id: seat_image_vi
                    x: 5
                    y: 175
                    width: 135
                    height: 103
                    source: "seat-p-03.png"

                    MouseArea {
                        id: seat_mouse_vi
                        anchors.fill: parent
                        onClicked: {
                            //set the climate mode
                            chairMode();
                            getChair();
                        }
                    }
                }

                Image {
                    id: share_image_vi
                    x: 5
                    y: 376
                    width: 135
                    height: 98
                    source: "share_main_button_inactive.png"

                    MouseArea {
                        id: share_mouse_vi
                        anchors.fill: parent
                        onClicked: {
                            sharePlayer();
                        }
                    }
                }

                Image {
                    id: play_im_vi
                    x: 246
                    y: 403
                    width: 111
                    height: 71
                    visible: true
                    source: "player_play_inactive.png"

                    MouseArea {
                        id: play_mouse_vi
                        anchors.fill: parent
                        onClicked:{
                            vPlay();
                        }
                    }
                }

                Image {
                    id: pause_im_vi
                    x: 246
                    y: 403
                    width: 111
                    height: 71
                    visible: false
                    source: "player_pause_inactive.png"

                    MouseArea {
                        id: pause_mouse_vi
                        anchors.fill: parent
                        onClicked:{
                            vPause();
                        }
                    }
                }

                Image {
                    id: prev_im_vi
                    x: 140
                    y: 403
                    width: 106
                    height: 71
                    source: "player_back_inactive.png"

                    MouseArea {
                        id: prev_mouse_vi
                        anchors.fill: parent
                        onClicked:{
                            vPrev();
                        }
                    }
                }

                Image {
                    id: forward_im_vi
                    x: 468
                    y: 403
                    width: 111
                    height: 71
                    source: "player_forward_inactive.png"

                    MouseArea {
                        id: forward_mouse_vi
                        anchors.fill: parent
                        onClicked: {
                            vNext();
                        }
                    }
                }

                Image {
                    id: stop_im_vi
                    x: 357
                    y: 403
                    width: 111
                    height: 71
                    source: "player_stop_inactive.png"

                    MouseArea {
                        id: stop_mouse_vi
                        anchors.fill: parent
                        onClicked: {
                            vStop();
                        }
                    }
                }

                Image {
                    id: vol_up_vi
                    x: 690
                    y: 403
                    width: 108
                    height: 71
                    source: "player_volume+_inactive.png"

                    MouseArea {
                        id: vol_up_mouse_vi
                        anchors.fill: parent
                        onClicked:{
                            vVolUp();
                        }
                    }
                }

                Image {
                    id: vol_down_vi
                    x: 579
                    y: 403
                    width: 111
                    height: 71
                    source: "player_volume-_inactive.png"

                    MouseArea {
                        id: vol_down_mouse_vi
                        anchors.fill: parent
                        onClicked: {
                            vVolDn();
                        }
                    }
                }

                Image {
                    id: video_im_vi
                    x: 724
                    y: 190
                    width: 74
                    height: 177
                    source: "player_video_active.png"
                }

                Image {
                    id: music_im_vi
                    x: 724
                    y: 8
                    width: 74
                    height: 182
                    source: "player_audio_inactive.png"

                    MouseArea {
                        id: music_mouse_vi
                        anchors.fill: parent
                          onClicked: {
                              audioPage();
                          }
                    }
                }


                source: "player_background.v1.png"
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0


                VideoContainer{
                    id:video
                    x: 150
                    y: 10
                    visible: false
                    objectName: "videoContainer"
                }

                ListView {
                    id: playlistVids
                    x: 150
                    y: 10
                    width: 575
                    height: 359
                    snapMode: ListView.SnapToItem
                    clip: true
                    visible: true
                    highlightMoveSpeed: 836
                    delegate: Item {
                        x: 5
                        height: 40
                        width:parent.width
                        Row {
                            id: row3
                            spacing: 5
                            Text {
                                text: name
                                font.bold: true
                                width:575
                                elide:Text.ElideRight
                                font.pointSize: 24
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                            }
                        }
                        MouseArea {
                           anchors.fill: parent;
                           hoverEnabled: true
                           onPressed: {
                               vPosClicked(playlistVids.currentIndex);
                           }
                           onEntered: {
                               if(playlistVids.currentIndex!==index){
                                   playlistVids.currentIndex = index
                               }
                           }
                           onExited: {
                               playlistVids.currentIndex = -1
                           }
                        }
                    }
                    currentIndex: -1
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                    focus: false
                    model: videoPlayListModel

                }

                ListModel { //TODO refactor (idk how to specify prototype of ListElement without instantiating it)
                    id:videoPlayListModel
                    ListElement {
                        name: "Grey"
                    }
                }
            }
            z: 92
        }
}



