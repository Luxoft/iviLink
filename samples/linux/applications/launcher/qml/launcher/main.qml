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
    width: 800
    height: 600
    color: "#d33333"

    function mediaPressed()
    {
        BtnProcessor.mediaPressed();
    }

    function seatPressed()
    {
        BtnProcessor.seatPressed();
    }

    function climatePressed()
    {
        BtnProcessor.climatePressed();
    }

    function resetPressed()
    {
        BtnProcessor.resetPressed();
    }
    
    function shutdownPressed()
    {
        BtnProcessor.shutdownPressed();
    }

    function unpairPressed()
    {
        BtnProcessor.unpairPressed();
    }

    Image {
        id: background
        x: 0
        y: 0
        z: 0
        source: "Media_Player.jpg"

        MouseArea {
            id: climate_mouse
            x: 0
            y: 250
            width: 210
            height: 100
            anchors.leftMargin: 0
            anchors.topMargin: 250
            anchors.rightMargin: 590
            anchors.bottomMargin: 250
            anchors.fill: parent
            onPressed:
            {
                climatePressed();
                climate_pressed.visible = true;
            }
            onReleased:
            {
                climate_pressed.visible = false;
            }
        }

        MouseArea {
            id: seat_mouse
            x: 0
            y: 368
            width: 210
            height: 100
            anchors.leftMargin: 0
            anchors.topMargin: 368
            anchors.rightMargin: 590
            anchors.bottomMargin: 132
            anchors.fill: parent
            onPressed:
            {
                seatPressed();
                seat_pressed.visible = true;
            }
            onReleased:
            {
                seat_pressed.visible = false;
            }
        }

        MouseArea {
            id: media_mouse
            x: 0
            y: 135
            width: 210
            height: 100
            anchors.leftMargin: 0
            anchors.topMargin: 135
            anchors.rightMargin: 590
            anchors.bottomMargin: 365
            anchors.fill: parent
            onPressed:
            {
                mediaPressed();
                media_pressed.visible = true;
            }
            onReleased:
            {
                media_pressed.visible = false;
            }
        }

        MouseArea {
            id: reset
            x: 324
            y: 221
            width: 162
            height: 155
 			onClicked:
                resetPressed();
        }

        MouseArea {
            id: shutdown
            x: 600
            y: 221
            width: 162
            height: 155
            onClicked:
                shutdownPressed();
        }

        Image {
            id: seat_pressed
            x: 0
            y: 362
            source: "seat.png"
            visible: false
        }

        Image {
            id: climate_pressed
            x: 0
            y: 244
            source: "climate.png"
            visible: false
        }

        Image {
            id: media_pressed
            x: 0
            y: 128
            source: "media.png"
            visible: false
        }

        Image {
            id: unpair
            x: 710
            y: 541
            source: "unpair.png"
            visible: true

            MouseArea {
                id: unpair_mouse
                x: 0
                y: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.fill: parent
                onClicked:
                {
                     unpairPressed();
                }
                onPressed:
                {
                    unpair_pressed.visible = true;               
                }
                onReleased:
                {
                    unpair_pressed.visible = false;
                }
            }
        }

        Image {
            id: unpair_pressed
            x: 710
            y: 541
            source: "unpair_pressed.png"
            visible: false
        }
    }


}
