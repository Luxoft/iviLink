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
import QtQuick 1.0


Rectangle {
    id: view_control
    width: 550
    height: 106
    color: "transparent"
    //color: "#ffffff"
    //border.color: "#000000"
    z: -2147483647
    //color: "#000000"
    //border.width: 23
    //border.color: "#ff0505"

    //buttons property
    property string src: ""
    property string empty_btn: "empty_btn.png"
    property string active_btn: "btn_orange.png"
    property string grayed_btn: "btn_disabled.png"

    Connections
    {
        target: videoWidget
        onStartTimerForView:
        {
            view_control_timer.running = true;
        }
    }
    
    Connections
    {
        target: CEmitter
        
        onSignalSetSync:{
            syncButton.source = active_btn;
        }
        onSignalSetUnsync:{
            syncButton.source = empty_btn;
        }
        onSignalSetSyncDisabled:{
            syncButton.source = grayed_btn;
        }

        onSignalActivateToggle:{
            toggleButton.source = empty_btn;
            toggle.color = "#ffffff"
        }

        onSignalHighlightToggle:{
             toggleButton.source = active_btn;
             toggle.color = "#ffffff"
        }

        onSignalDeactivateToggle:{
            toggleButton.source = grayed_btn;
            toggle.color = "#000000"
        }
    }

    function close_view(){
        videoWidget.closeControlView();
    }

    function stop_pr(){
        videoWidget.video_hide();
        CPlayerLogic.localStop();
    }

    function pause_pr(){
        CPlayerLogic.localPause();
    }

    function play_pr(){
        CPlayerLogic.localPlay();
    }

    function next_pr(){
        videoWidget.video_hide();
        CPlayerLogic.localNext();
    }

    function prev_pr(){
        videoWidget.video_hide();
        CPlayerLogic.localPrev();
    }

    function vol_up_pr(){
        CPlayerLogic.localVolumeUp();
    }

    function toggle_pr(){
        CPlayerLogic.localToggle();
    }

    function sync_pr(){
        CPlayerLogic.localSync();
    }

    function vol_down_pr(){
        CPlayerLogic.localVolumeDown();
    }

    function swapplay(){
        play.visible = false;
        pause.visible = true;
    }

    function swappause(){
        play.visible = true;
        pause.visible = false;
    }


    Timer {
    id: view_control_timer
    interval: 5000; running: false; repeat: false
    onTriggered: close_view();
    }

    Image {
        id: play
        x: 203
        y: 6
        width: 96
        height: 99
        z: 1
        source: "vb-pl-2.png"

        MouseArea {
            id: mouse_play
            x: 0
            y: 0
            width: 96
            height: 99
            z: 1
            onClicked:{
		swapplay();
                play_pr();
                
                view_control_timer.running = false;
                view_control_timer.running = true;

            }
            onPressed:
            {
                src =  play.source;
                play.source = "vb-pl-3.png"
                play.visible = true;
            }
            onReleased:
            {
                play.source = src;
            }
        }
    }

    Image {
        id: stop
        x: 106
        y: 6
        width: 91
        height: 99
        source: "vb-st-2.png"


        MouseArea {
            id: mouse_stop
            anchors.fill: parent
            z: 1
            onClicked: {
		swappause();
                stop_pr();
               
                view_control_timer.running = false;
                view_control_timer.running = true;

            }
            onPressed:
            {
                src =  stop.source;
                stop.source = "vb-st-3.png"
                stop.visible = true;
            }
            onReleased:
            {
                stop.source = src;
            }

        }

    }

    Image {
        id: vol_up
        x: 484
        y: 6
        width: 66
        height: 46
        source: "3/pl/gr-pl-01.png"

        MouseArea {
            id: vol_up_m
            anchors.fill: parent
            onClicked:{
                vol_up_pr();
                view_control_timer.running = false;
                view_control_timer.running = true;
            	}
            }
    }

    Image {
        id: vol_down
        x: 484
        y: 59
        width: 66
        height: 46
        source: "3/pl/gr-min-01.png"

        MouseArea {
            id: vol_down_m
            anchors.fill: parent
            onClicked: {
                vol_down_pr();
                view_control_timer.running = false;
                view_control_timer.running = true;
            }
        }
    }

    Image {
        id: prev
        x: 9
        y: 6
        width: 91
        height: 99
        source: "vb-r-2.png"

        MouseArea {
            id: prev_m
            anchors.fill: parent
            onClicked:{
                prev_pr();
                view_control_timer.running = false;
                view_control_timer.running = true;
            }
            onPressed:
            {
                src =  prev.source;
                prev.source = "vb-r-3.png"
                prev.visible = true;
            }
            onReleased:
            {
                prev.source = src;
            }
        }
    }

    Image {
        id: next
        x: 305
        y: 6
        width: 91
        height: 99
        source: "vb-n-2.png"

        MouseArea {
            id: next_m
            anchors.fill: parent
            onClicked: {
                next_pr();
                view_control_timer.running = false;
                view_control_timer.running = true;
            }
            onPressed:
            {
                src =  next.source;
                next.source = "vb-n-3.png"
                next.visible = true;
            }
            onReleased:
            {
                next.source = src;
            }
        }

    }

    Image {
        id: pause
        x: 203
        y: 6
        width: 96
        height: 99
        z: 2
        source: "vb-p-2.png"

        MouseArea {
            id: pause_m
            anchors.fill: parent
            onClicked:{
		swappause();
                pause_pr();
                
                view_control_timer.running = false;
                view_control_timer.running = true;
            }
            onPressed:
            {
                src =  pause.source;
                pause.source = "vb-p-3.png"
                pause.visible = true;
            }
            onReleased:
            {
                pause.source = src;
            }

        }
    }

    Rectangle {
        id: background
        x: 0
        y: 0
        width: 550
        height: 110
        color: "#000000"
        z: -2
    }

    Image {
                    id: syncButton
                    x: 401
                    y: 6
                    width: 80
                    height: 46
                    source: active_btn

                    MouseArea {
                        id: sync_mouse
                        anchors.fill: parent
                        onClicked:{
                            sync_pr();
                        }
                        onPressed:
                        {
                            src =  syncButton.source;
                            syncButton = grayed_btn;
                        }
                        onReleased:
                        {
                            syncButton.source = src;
                        }

                        Text {
                            id: sync
                            x: 19
                            y: 12
                            width: 48
                            height: 23
                            color: "#ffffff"
                            text: qsTr("sync")
                            font.bold: true
                            font.pixelSize: 20
                        }
                    }
                }

                Image {
                    id: toggleButton
                    x: 401
                    y: 59
                    width: 80
                    height: 46
                    source: grayed_btn

                    MouseArea {
                        id: toggle_mouse
                        anchors.fill: parent
                        onClicked:{
                            toggle_pr();
                        }
                        onPressed:
                        {
                            src =  toggleButton.source;
                            toggleButton = grayed_btn;
                        }
                        onReleased:
                        {
                            toggleButton.source = src;
                        }

                        Text {
                            id: toggle
                            x: 9
                            y: 11
                            color: "#000000"
                            text: qsTr("toggle")
                            font.bold: true
                            font.pixelSize: 20
                        }
                    }
                }
}

