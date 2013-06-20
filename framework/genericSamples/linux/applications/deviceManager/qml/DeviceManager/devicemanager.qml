import QtQuick 1.1
import QtQuick 1.0

Rectangle {

    property int    trusted_list_removal_time: 0    // removal animation time
    property int    trusted_list_add_time: 250   // add animation time
    property string src: ""
    property string btn_disc_dis: ":/images/btn_disconnect_disabled.png"
    property string btn_disc_ena: ":/images/btn_disconnect_enabled.png"
    property string btn_disc_prs: ":/images/btn_disconnect_pressed.png"
    property string btn_rmall_dis: ":/images/btn_remove_all_disabled.png"
    property string btn_rmall_ena: ":/images/btn_remove_all_enabled.png"
    property string btn_rmall_prs: ":/images/btn_remove_all_pressed.png"

    property string btn_home_ena: ":/images/home_enable.png"
    property string btn_home_prs: ":/images/home_pressed.png"
    property string btn_delete: ":/images/delete_device_btn.png"
    property string image_background: ":/images/b-01.png"

    id: trusted_list
    x: 0
    y: 0
    width: 800
    height: 600
    color: "transparent"
    z: 121
    visible: false

    Rectangle {
        id: topbar
        x:0
        y:0
        z:120
        color: "transparent"
        width: parent.width
        height: 51

        Image {
            id: exit_button
            x: 0
            y: 0
            opacity: 1
            z: 118
            source: btn_home_ena;
            visible: true

            MouseArea {
                id: exit_button_mouse
                anchors.fill: parent
                onClicked: {
                    console.log("Exit was clicked")
                    QMLDeviceManagerDialog.onExit()
                }
                onPressed: {
                    src =  exit_button.source;
                    exit_button.source = btn_home_prs;
                }
                
                onReleased: {
                    exit_button.source = btn_home_ena;
                }

            }
        }
    }

    Rectangle {
        id: application
        x:0
        y:51
        width:800
        height:(600-51)
      
        Image {
            id: form_background
            x:0
            y:1
            width:800
            source: image_background
        }

        ListView {
            id: trustedDevicesList
            x: 50
            y: 10
            width: 695
            height: 450
            visible: true
            clip:true
            flickDeceleration: 1749
            z: 2
            snapMode: ListView.SnapToItem
            highlightMoveSpeed: 836
            delegate: trustedListDelegate

            currentIndex: -1
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: false
            model: trustedListModel
            Keys.onPressed: {
                if(event.key===Qt.Key_Down){
                    console.log("overriding down key")
                } else if(event===Qt.Key_Up){
                    console.log("overriding up key")
                }
            }

        }
    
        ListView {
            id: foundDevicesList
            x: 50
            y: 108
            width: 695
            height: 379
            visible: false
            flickDeceleration: 1749
            z: 2
            snapMode: ListView.SnapToItem
            highlightMoveSpeed: 836
            delegate: foundListDelegate

            currentIndex: -1
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: false
            model: visibleListModel
            Keys.onPressed: {
                if(event.key===Qt.Key_Down){
                    console.log("overriding down key")
                } else if(event===Qt.Key_Up){
                    console.log("overriding up key")
                }
            }

        }
        
        Image {
            id: disconnect_button 
            x: 30
            y: 460
            opacity: 1
            z: 118
            source: btn_disc_ena
            MouseArea {
                id: disconnect_button_mouse
                anchors.fill: parent
                onClicked: {
                    console.log("Disconnect was clicked")
                    QMLDeviceManagerDialog.onFoundListDisconnectDevice(1);
                }
                onPressed: {
                    src =  disconnect_button.source;
                    disconnect_button.source = btn_disc_prs;
                }
                
                onReleased: {
                    disconnect_button.source = src;
                }
                
                Text {
                    id: disconnect_button_text
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "#b3b3b3"
                    text: qsTr("Disconnect")
                    font.family: "Titula"
                    font.pixelSize: 24
                }

            }
        }

        
        Image {
            id: remove_all_button
            x: 460
            y: 460
            opacity: 1
            z: 118
            source: btn_rmall_dis
            visible: true

            MouseArea {
                id: remove_all_button_mouse
                anchors.fill: parent
                enabled: false
                onClicked: {
                    console.log("Remove all onClicked()")
                    QMLDeviceManagerDialog.onRemoveAll()
                }
                onPressed: {
                    console.log("Remove all onPressed()")
                    src =  remove_all_button.source;
                    remove_all_button.source = btn_rmall_prs;
                }
                
                onReleased: {
                    console.log("Remove all onReleased()")
                    remove_all_button.source = btn_rmall_dis;
                    remove_all_button_mouse.enabled = false;
                }

                Text {
                    id: rm_all_text
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: "#b3b3b3"
                    text: qsTr("Remove All Devices")
                    font.family: "Titula"
                    font.pixelSize: 24
                }
            }
        }
        
        Component {
            id: trustedListDelegate

            Item {
                id: trustedDelegateItem
                width: 695
                height: 40
                clip: true

                Row {
                    width: 320
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 10

                    Text {
                            id: delegateTextid
                            text: name
                            width:600
                            elide:Text.ElideRight
                            font.family: "Titula"
                            font.pointSize: 20
                            color: "#b3b3b3"
                        }
                }

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    spacing: 10
                    Image {
                        source: btn_delete
                        MouseArea {
                            anchors.fill:parent;
                            onClicked: {
                                console.log("Removing or disconnecting " + index.toString())
                                QMLDeviceManagerDialog.onTrustedListRemoveDevice(index);
                                trusted_list_removal_time = 250;
                                trustedListModel.remove(index);
                            }
                        }
                    }
                }

                ListView.onRemove: SequentialAnimation {
                    PropertyAction { target: trustedDelegateItem; property: "ListView.delayRemove"; value: true }
                    NumberAnimation { target: trustedDelegateItem; property: "height"; to: 0; duration: trusted_list_removal_time; easing.type: Easing.OutCubic }
                    // Make sure delayRemove is set back to false so that the item can be destroyed
                    PropertyAction { target: trustedDelegateItem; property: "ListView.delayRemove"; value: false }
                }
                ListView.onAdd:SequentialAnimation {
                    // Make sure delayRemove is set back to false so that the item can be destroyed
                    PropertyAction { target: trustedDelegateItem; property: "height"; value: 0 }
                    NumberAnimation { target: trustedDelegateItem; property: "height"; to: 55; duration: trusted_list_add_time; easing.type: Easing.InOutQuad }

                }
            }
        }
        
        Component {
            id: foundListDelegate

            Item {
                id: foundDelegateItem
                width: 695
                height: 40
                clip: true

                Row {
                    id: textRow
                    width: 320
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 10

                    Text {
                            id: delegateTextid
                            text: name
                            width: 320
                            elide:Text.ElideRight
                            font.pointSize: 20
                            font.family: "Titula"
                            color: "#b3b3b3"
                    }
                }

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    spacing: 10
                    Image {
                        source: ":/images/connect_device_btn.png"
                        MouseArea {
                            anchors.fill:parent;
                            onClicked: {
                                console.log("Connecting " + index.toString())
                                QMLDeviceManagerDialog.onFoundListConnectAndPairDevice(index);
                            }
                        }
                    }
                    Image {
                        source: btn_delete
                        MouseArea {
                            anchors.fill:parent;
                            onClicked: {
                                console.log("DisConnecting " + index.toString())
                                QMLDeviceManagerDialog.onFoundListDisconnectDevice(index);
                            }
                        }
                    }
                }
            }
        }
        
        ListModel { //TODO refactor (idk how to specify prototype of ListElement without instantiating it)
            id:trustedListModel
            ListElement {
                name: "Grey"

            }
        }
        
        ListModel { //TODO refactor (idk how to specify prototype of ListElement without instantiating it)
            id:visibleListModel
            ListElement {
                name: "Grey"
            }
        }
    }

    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: {
            if(!QMLDeviceManagerDialog.isConnected()){
                disconnect_button.source = btn_disc_dis;
                disconnect_button_mouse.enabled = false;
            } else {
                disconnect_button.source = btn_disc_ena;
                disconnect_button_mouse.enabled = true;
            }
        }
    }


        Connections {
        
            target: QMLDeviceManagerDialog
            
            onTrustedListPopulate: {   //!            
                trustedListModel.append({"name":devName});            //fill-up the list
                console.log("onTrustedListPopulate - got name "+devName);
            }

            onFoundListPopulate: {    //!
                visibleListModel.append({"name":devName});
                console.log("onFoundListPopulate - got name "+devName);
            }

            onFoundListCleanup: {
                visibleListModel.clear();                         //cleanup the list
                console.log("onFoundListCleanup");
            }
    
            onTrustedListCleanup: {   //!
                trusted_list_removal_time = 0;
                trustedListModel.clear();                         //cleanup the list
                console.log("onTrustedListCleanup");
            }

            onShowTrustedList: {
                console.log("trusted_list.visible = true;");
                trustedDevicesList.visible = true;
                foundDevicesList.visible = false;
                trusted_list.visible = true;
            }

            onShowFoundList: {
                console.log("found_list.visible = true;");
                trustedDevicesList.visible = false;
                foundDevicesList.visible = true;
            }
            
            onRemoveItemfromList: {
                trustedListModel.remove(index);
            }
            
            onClearList: {
                trustedListModel.clear();
            }

            onEnableRemoveAllButton: {
                console.log("Trust list is NOT empty");
                remove_all_button.source = btn_rmall_ena;
                remove_all_button_mouse.enabled = true;
            }

            onDisableRemoveAllButton: {
                console.log("Trust list is empty");
                remove_all_button.source = btn_rmall_dis;
                remove_all_button_mouse.enabled = false;
            }
       }
}
