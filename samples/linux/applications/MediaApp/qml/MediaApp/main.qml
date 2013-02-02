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
    id: page
    width: 800
    height: 600
    //z: 2147483646
    property string src: ""
    property int playerState_interval : 500

    property string empty_btn: "empty_btn.png"
    property string active_btn: "btn_orange.png"
    property string grayed_btn: "btn_disabled.png"
    property string playback: "stop"

    Connections {
        target: CEmitter

        onSignalAudioListPopulate:
        {   
            if (isLocal)
            {
                playListModel.append({"name":songName+ " - local"});                //fill-up the list
            }
            else
            {
                playListModel.append({"name":songName+ " - remote"});                //fill-up the list
            }
            console.log("onAudioListPopulate - got name "+songName);
        }
        onSignalAudioListClear:
        {
	        playListModel.clear(); 
        }

        onSignalPlaylistPosChanged:
        {  
            console.log("onPlaylistPosChanged "+ pos.toString());
            playlistSongs.currentIndex = pos;
        }

        onSignalShowPlayButton:{
            play_im_au.visible = true;
            pause_im_au.visible = false;
        }

        onSignalShowPauseButton:{
            play_im_au.visible = false;
            pause_im_au.visible = true;
        }
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
        onSignalShowPauseIcon: {
            playback = "pause";
        }
        onSignalShowPlayIcon: {
             playback = "play";
        }
        onSignalHideIcon: {
             playback = "stop";
        }
    }

        //media control when local changing tabs

       function showPlayBtn()  //!
        {
            console.log("show play btn");
            pause_im_au.visible=false;
            play_im_au.visible=true;
        }
        function showPauseBtn ()   //!
        {
            console.log("show pause btn");
            pause_im_au.visible=true;
            play_im_au.visible=false;
        }

        function getPlaylist()  //!
        {
            state_video_playlist_id = 0;                    //to make lists clean
            state_audio_playlist_id = 0;                    //to make lists clean

            if(state_Sharing == 0 || state_Sharing == 2)    //no share or we do share
            {
                BtnProcessor.audioPopulateList();
            }
        }

        //Update playlist
        function updateAudioPlaylist()  //!
        {

        }

        //audio functions
        function playAudioBtnPressed()
        {
            CPlayerLogic.localPlay();
        }
        function stopAudioBtnPressed()
        {
            CPlayerLogic.localStop();
        }

        function pauseAudioBtnPressed()
        {
            CPlayerLogic.localPause();
        }

        function nextAudioBtnPressed()
        {
            CPlayerLogic.localNext();
        }

        function prevAudioBtnPressed()
        {
            CPlayerLogic.localPrev();
        }

        function syncAudioBtnPressed()
        {
            CPlayerLogic.localSync();
        }

        function toggleAudioBtnPressed()
        {
            CPlayerLogic.localToggle();
        }

        function volUp()
        {
            CPlayerLogic.localVolumeUp();
        }

        function volDn()
        {
            CPlayerLogic.localVolumeDown();
        }

        function fullscreen_mode()
        {

        }
        function exitBtnPressed()
        {
            CPlayerLogic.localExit();
        }

        function posClicked(newpos){ //!
	       CPlayerLogic.localPosClicked(newpos);
        }
        //end of audio functions

       Timer 
       {
            id: timer_list_update
            interval: 100 ; running: true; repeat: false
            onTriggered: CPlayerLogic.updateListView();
       }

       Timer
       {
            id: timer_player_state 
            interval: playerState_interval; running: true; repeat: true
            onTriggered: CPlayerLogic.obtainPlayerState();
        }
        

        //AUDIO
        Rectangle { //player_mode_audio
            id: player_mode_audio
            x: 0
            y: 0
            width: 800
            height: 600
            color: "#000000"
            visible: true
            z: 91

            Image {
                id: player_audio
                x: 0
                y: -1
                width: 800
                height: 600
                smooth: true
                source: "3/b-01.png"

                Image { //! play btn
                    id: play_im_au
                    x: 520
                    y: 433
                    width: 103
                    height: 96
                    sourceSize.height: 111
                    sourceSize.width: 111
                    z: 112
                    source: "play.png"
                    visible: true


                    MouseArea {
                        id: share_mouse_au_a
                        x: 0
                        y: 0
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 0
                        anchors.leftMargin: 0
                        anchors.topMargin: 0
                        anchors.fill: parent

                        onClicked:{

                            playAudioBtnPressed();
                        }
                        onPressed:
                        {
                            src =  play_im_au.source;
                            play_im_au.source = "play_pressed.png"
                            play_im_au.visible = true;
                        }
                        onReleased:
                        {
                            play_im_au.source = src;
                        }
                    }
                }

                Image { //! pause btn
                    id: pause_im_au
                    x: 520
                    y: 433
                    width: 103
                    height: 96
                    z: 111
                    source: "pause.png"
                    visible:false

                    MouseArea {
                        id: pause_mouse_au
                        x: 0
                        y: 0
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 0
                        anchors.leftMargin: 0
                        anchors.topMargin: 0
                        anchors.fill: parent
                        onClicked: {
                            pauseAudioBtnPressed();
                        }
                        onPressed:
                        {
                            src =  pause_im_au.source;
                            pause_im_au.source = "pause_pressed.png"
                            pause_im_au.visible = true;
                        }
                        onReleased:
                        {
                            pause_im_au.source = src;
                        }
                    }
                }

                Image { //prev btn
                    id: prev_im_au
                    x: 417
                    y: 433
                    width: 103
                    height: 96
                    sourceSize.height: 108
                    sourceSize.width: 108
                    z: 110
                    source: "previous.png"

                    MouseArea {
                        id: prev_mouse_au
                        x: 0
                        y: 0
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 0
                        anchors.leftMargin: 0
                        anchors.topMargin: 0
                        anchors.fill: parent
                        onClicked:{
                            prevAudioBtnPressed();
                        }
                        onPressed:
                        {
                            src =  prev_im_au.source;
                            prev_im_au.source =  "previous_pressed.png"
                            prev_im_au.visible = true;
                        }
                        onReleased:
                        {
                            prev_im_au.source = src;
                        }
                    }
                }

                Image { //next btn
                    id: forward_im_au
                    x: 623
                    y: 446
                    width: 103
                    height: 88
                    sourceSize.height: 108
                    sourceSize.width: 108
                    z: 113
                    source:  "next.png"

                    MouseArea {
                        id: forward_mouse_au
                        x: 0
                        y: -12
                        width: 103
                        height: 94
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 6
                        anchors.leftMargin: 0
                        anchors.topMargin: -12
                        anchors.fill: parent
                        onClicked:{
                            nextAudioBtnPressed();
                        }
                        onPressed:
                        {
                            src =  forward_im_au.source;
                            forward_im_au.source =  "next_pressed.png"
                            forward_im_au.visible = true;
                        }
                        onReleased:
                        {
                            forward_im_au.source = src;
                        }
                    }
                }

                            Image { //stop btn
                                id: stop_im_au
                                x: 324
                                y: 433
                                width: 109
                                height: 96
                                z: 116
                                source: "stop.png"

                                MouseArea {
                                    id: stop_mouse_au
                                    x: 5
                                    y: 0
                                    width: 95
                                    height: 96
                                    anchors.rightMargin: 9
                                    anchors.bottomMargin: 0
                                    anchors.leftMargin: 5
                                    anchors.topMargin: 0
                                    anchors.fill: parent
                                    onClicked:{
                                        stopAudioBtnPressed();
                                    }
                                    onPressed:
                                    {
                                        src =  stop_im_au.source;
                                        stop_im_au.source = "stop_pressed.png"
                                        stop_im_au.visible = true;
                                    }
                                    onReleased:
                                    {
                                        stop_im_au.source = src;
                                    }
                                }
                            }

                Image { //!vol dn btn
                    id: vol_down_au
                    x: 351
                    y: 87
                    width: 66
                    height: 46
                    z: 114
                    sourceSize.height: 46
                    sourceSize.width: 66
                    source: "3/pl/gr-min-01.png"

                    MouseArea {
                        id: vol_down_mouse_au
                        x: 0
                        y: 0
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 0
                        anchors.leftMargin: 0
                        anchors.topMargin: 0
                        anchors.fill: parent
                        onClicked:{
                            volDn();
                        }
                    }

                }

                Image { //! vol up btn
                    id: vol_up_au
                    x: 417
                    y: 87
                    width: 66
                    height: 46
                    z: 115
                    sourceSize.height: 46
                    sourceSize.width: 66
                    source: "3/pl/gr-pl-01.png"

                    MouseArea {
                        id: vol_up_mouse_au
                        x: 0
                        y: 0
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 0
                        anchors.leftMargin: 0
                        anchors.topMargin: 0
                        anchors.fill: parent
                        onClicked:{
                            volUp();
                        }

                    }

                }


                ListView {
                    id: playlistSongs
                    x: 351
                    y: 171
                    width: 357
                    height: 245
                    keyNavigationWraps: true
                    z: 109
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
                                color: "#ffffff"
                                width:575
                                elide:Text.ElideRight
                                font.pointSize: 16

                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                            }

                        }
                        Row {
                            id: row3
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            
                            spacing: 3
                            Image {
                                id: icon_image
                                source:  (playlistSongs.currentIndex == index && playback != "stop")? (playback == "play"? "play_icon.png":"pause_icon.png" ) : ""
                            }
                        }
                        MouseArea {
                            anchors.fill: parent;
                            onPressed: {

                                console.log("index: " + index.toString() );
                                console.log("currentIndex: " + (playlistSongs.currentIndex).toString());

                                posClicked(index);
                                if(playlistSongs.currentIndex !== index)
                                {
                                    playlistSongs.currentIndex = index
                                }

                            }
                        }
                    }
                    currentIndex: -1
                    highlight: Rectangle { color: "#50d29b00"; radius: 5 }
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

                ListModel {
                    id:playListModel
                    ListElement {
                        name: "Grey"
                    }
                }

                Image { //background
                    id: tracklist_bg
                    x: 330
                    y: 74
                    width: 396
                    height: 455
                    z: 0
                    sourceSize.height: 455
                    sourceSize.width: 396
                    source: "3/pl/tracklist_bg.png"
                }

                Image {
                    id: syncButton
                    x: 7
                    y: 381
                    source: active_btn

                    MouseArea {
                        id: sync_mouse
                        x: 0
                        y: 0
                        anchors.rightMargin: 0
                        anchors.bottomMargin: 0
                        anchors.leftMargin: 0
                        anchors.topMargin: 0
                        anchors.fill: parent
                        onClicked:{
                            syncAudioBtnPressed();
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
                            x: 40
                            y: 17
                            color: "#ffffff"
                            text: qsTr("sync")
                            font.bold: true
                            font.pixelSize: 36
                        }
                    }
                }

                Image {
                    id: toggleButton
                    x: 7
                    y: 455
                    source: grayed_btn

                    MouseArea {
                        id: toggle_mouse
                        anchors.fill: parent
                        onClicked:{
                            toggleAudioBtnPressed();
                        }
                        Text {
                            id: toggle
                            x: 23
                            y: 17
                            color: "#000000"
                            text: qsTr("toggle")
                            font.bold: true
                            font.pixelSize: 36
                        }
                    }
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

            }

        }
        //eof audio

    //  z: 92
    }


