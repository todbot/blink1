import QtQuick 2.5
import QtQuick.Controls 1.4

Image {
    id: popup
    width: 427
    height: 311
    z: 99999
    source: "qrc:/images/layout/popup-hardware.png"

    property alias name: name
    property alias type: hType
    property alias activity: activity
    property alias alert: alert
    property alias action: hAction
    property alias spinbox: alertinput

    property string oldname: ""

    function clearData(){
        name.text=""
        type.currentIndex=0
        activity.checked=true
        alert.checked=false
        action.currentIndex=0
        spinbox.text="0"
        spinbox.readOnly=true

    }
    function editData(arg1,arg2,arg3,arg4,arg5){
        clearData();

        name.text=arg1
        type.currentIndex=arg2
        if(arg3===0){
            activity.checked=true
            alert.checked=false
        }else if(arg3===1){
            alert.checked=true
            activity.checked=false
            spinbox.readOnly=false
        }else{
            alert.checked=true
            activity.checked=true
            spinbox.readOnly=false
        }

        action.currentIndex=arg4
        spinbox.text=arg5
    }

    MouseArea{
        anchors.fill: parent
    }
    MouseArea{
        z:0
        width: parent.width
        height: 35
        anchors.top: parent.top
        anchors.left: parent.left
        property variant previousPosition
        onPressed: {
            previousPosition = Qt.point(mouseX, mouseY)
        }
        onPositionChanged: {
            if (pressedButtons == Qt.LeftButton) {
                var dx = mouseX - previousPosition.x
                var dy = mouseY - previousPosition.y
                popup.x=popup.x+dx;
                popup.y=popup.y+dy;
            }
        }
    }
    Text{
        color: "white"
        //font.pointSize: (!mw.mac())?10:13
        font.pixelSize:13
        text: "Hardware - Properties"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 40
        anchors.topMargin: 12
    }
    PushButton{
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 3
        label.text: ""
        upSrc: "qrc:/images/layout/close-single-up.png"
        downSrc: "qrc:/images/layout/close-single-hover.png"
        onClicked: {
            mw.markHardwareEditing(popup.oldname,false)
            if(popup.oldname!="")
                mw.checkHardwareMonitor(popup.oldname)
            popup.oldname=""
            parent.visible=false
        }

    }

    Item{
        id: nameI
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: parent.top
        anchors.topMargin: 55
        height: 30
        width: 300
        Text{
            id: nameN
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Name:"
            color: "black"
            //font.pointSize: (!mw.mac())?8:11
            font.pixelSize:11
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            radius: 3
            border.color: "#CDCFD2"
            width: parent.width
            height: parent.height
            TextInput{
                id: name
                color: "black"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                width: 290
                height: 30
                //font.pointSize: (!mw.mac())?10:12
                font.pixelSize:12
                selectByMouse: true
                maximumLength: 24
            }
        }
    }

    Item{
        id: types
        anchors.left: nameI.left
        anchors.top: nameI.bottom
        anchors.topMargin: 7
        height: 30
        width: 300
        z: popup.z+1

        Text{
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Type:"
            color: "black"
            //font.pointSize: (!mw.mac())?8:11
            font.pixelSize:11
        }
        ComboBox {
            id: hType
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            width: 300
            //currentIndex: 2
            model: ListModel {
                id: hTypes
                ListElement { text: "Battery"; }
                ListElement { text: "CPU"; }
                ListElement { text: "RAM"; }
            }  
            //onCurrentIndexChanged: console.debug(hType.get(currentIndex).text);
        }
    }

    Item{
        id: act
        anchors.left: types.left
        anchors.top: types.bottom
        anchors.topMargin: 50
        height: 30
        width: 20
        Image{
            id: activity
            property bool checked: false
            source: checked?"qrc:/images/layout/radio-selected.png":"qrc:/images/layout/radio-normal.png"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                onClicked: {
                    if(parent.checked){
                        if(alert.checked)
                            parent.checked=false
                    }else{
                        parent.checked=true
                    }
                }
            }
            onCheckedChanged: {
                if(checked){
                    alertinput.readOnly=false
                    source="qrc:/images/layout/radio-selected.png"
                }else{
                    alertinput.readOnly=true
                    source="qrc:/images/layout/radio-normal.png"
                }
            }
        }
        Text{
            text: "Activity"
            anchors.left: activity.right
            anchors.leftMargin: 15
            anchors.verticalCenter: activity.verticalCenter
            //font.pointSize: (!mw.mac())?8:11
            font.pixelSize:11
        }
    }

    Item{
        id: al
        anchors.left: types.left
        anchors.top: act.bottom
        anchors.topMargin: 5
        height: 30
        width: 20
        Image{
            id: alert
            property bool checked: true
            source: checked?"qrc:/images/layout/radio-selected.png":"qrc:/images/layout/radio-normal.png"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                onClicked: {
                    if(parent.checked){
                        if(activity.checked){
                            parent.checked=false
                            alertinput.readOnly=true
                        }
                    }else{
                        alertinput.readOnly=false
                        parent.checked=true
                    }
                }
            }
            onCheckedChanged: {
                if(checked){
                    source="qrc:/images/layout/radio-selected.png"
                }else{
                    source="qrc:/images/layout/radio-normal.png"
                }
            }
        }
        Text{
            text: "Alert"
            id: alerttitle
            anchors.left: alert.right
            anchors.leftMargin: 15
            anchors.verticalCenter: alert.verticalCenter
            //font.pointSize: (!mw.mac())?8:11
            font.pixelSize:11
        }

        ComboBox {
            id: hAction
            anchors.left: alerttitle.left
            anchors.leftMargin: 45
            anchors.verticalCenter: alert.verticalCenter
            width: 180
            //height: parent.height; // looks ugly
            //disabled: !alert.checked  // not present in real combobox
            model: ListModel {
                id: actions
                ListElement { text: "< (lower than)"; }
                ListElement { text: "<= (lower than or equal)"; }
                ListElement { text: "= (equal)"; }
                ListElement { text: "> (higher than)"; }
                ListElement { text:  "> (higher than or equal)"; }
            }  
            //onCurrentIndexChanged: console.debug(hType.get(currentIndex).text);
        }
        Item{
            anchors.left: hAction.right
            anchors.leftMargin: 4
            anchors.verticalCenter: alert.verticalCenter
            height: 30
            width: 114
            Image{
                id: ale
                Rectangle{
                    visible: !alert.checked
                    anchors.left: parent.left
                    anchors.top: parent.top
                    width: parent.width-2
                    height: parent.height-2
                    color: "#E6E6E6"
                    radius: 3
                    border.width: 1
                    border.color: "#CDCFD2"
                    opacity: 0.5
                    z: 9999
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                        }
                    }
                }

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 0
                source: "qrc:images/layout/spinbox-bg.png"
                width: parent.width
                height: parent.height
                TextInput{
                    id: alertinput
                    color: "black"
                    text: "1"
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 5
                    width: 54
                    height: 30
                    //font.pointSize: (!mw.mac())?10:12
                    font.pixelSize:12
                    selectByMouse: true
                    validator: IntValidator{}
                    onTextChanged: {
                        if(text==="" || text==="-") text="1";
                        text=parseInt(text)
                        if(parseInt(text)<1) text="1";
                        if(parseInt(text)>100) text="100";

                    }
                    maximumLength: 8
                }
                Column{
                    spacing: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.top: parent.top
                    Rectangle{
                        width: 10
                        height: 12
                        color: "transparent"
                        Image{
                            id: arrowup
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenterOffset: 2
                            source: "qrc:images/layout/colorpicker/arrow-up.png"
                        }
                        MouseArea{
                            cursorShape: (alert.checked)?Qt.PointingHandCursor:Qt.ArrowCursor
                            anchors.fill: parent
                            onPressed: arrowup.source="qrc:images/layout/colorpicker/arrow-down.png"
                            onReleased: arrowup.source="qrc:images/layout/colorpicker/arrow-up.png"
                            onClicked: {
                                if(!alertinput.readOnly && parseInt(alertinput.text)<100) alertinput.text=parseInt(alertinput.text)+1;
                            }
                        }
                    }
                    Rectangle{
                        width: 10
                        height: 12
                        color: "transparent"
                        Image{
                            id: arrowdown
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenterOffset: 2
                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                        }
                        MouseArea{
                            cursorShape: (alert.checked)?Qt.PointingHandCursor:Qt.ArrowCursor
                            anchors.fill: parent
                            onPressed: arrowdown.source="qrc:images/layout/colorpicker/arrow-2-down.png"
                            onReleased: arrowdown.source="qrc:images/layout/colorpicker/arrow-2-up.png"
                            onClicked: {
                                if(!alertinput.readOnly && parseInt(alertinput.text)>1) alertinput.text=parseInt(alertinput.text)-1;
                            }
                        }
                    }
                }
            }
        }
    }

    PushButton{
        id: okButton
        upSrc: "qrc:/images/layout/button-ok-up.png"
        downSrc: "qrc:/images/layout/button-ok-down.png"
        onClicked: {
            console.log("OK CLICKED")
            popup.visible=false
            mw.markHardwareEditing(popup.oldname,false)
            if(popup.oldname===""){
                var tmp=0;
                if(alert.checked){
                    tmp=1;
                    if(activity.checked) tmp+=1;   // FIXME: wtf marcin
                }
                mw.add_new_hardwaremonitor(name.text,hType.currentIndex,parseInt(alertinput.text),hAction.currentIndex,tmp)
            }else{
                console.log("EDIT")
                var tmp=0;
                if(alert.checked){
                    tmp=1;
                    if(activity.checked) tmp+=1;
                }
                mw.edit_hardwaremonitor(popup.oldname,name.text,hType.currentIndex,parseInt(alertinput.text),hAction.currentIndex,tmp)
            }
            mw.hardwareUpdate();

            popup.oldname="";
        }
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 22
        label.text: "OK"
        label.color: "black"
        //label.font.pointSize: (!mw.mac())?9:12
        label.font.pixelSize:12
    }
    PushButton{
        id: cancelButton
        upSrc: "qrc:/images/layout/button-cancel-up.png"
        downSrc: "qrc:/images/layout/button-cancel-down.png"
        onClicked: {
            console.log("CANCEL CLICKED")
            popup.visible=false
            mw.markHardwareEditing(popup.oldname,false)
            if(popup.oldname!="")
                mw.checkHardwareMonitor(popup.oldname)
            popup.oldname="";
        }
        anchors.right: okButton.left
        anchors.rightMargin: 5
        anchors.bottom: okButton.bottom
        label.text: "Cancel"
        label.color: "black"
        //label.font.pointSize: (!mw.mac())?9:12
        label.font.pixelSize:12
    }

}
