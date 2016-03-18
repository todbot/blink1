import QtQuick 2.0

Image {
    id: popup
    width: 427
    height: 191
    source: "qrc:/images/layout/popup-input.png"
    visible: false
    z: 9999
    property alias hostId: hostId.text
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

    PushButton{
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 3
        label.text: ""
        upSrc: "qrc:/images/layout/close-single-up.png"
        downSrc: "qrc:/images/layout/close-single-hover.png"
        onClicked: parent.visible=false
    }

    Text{
        id: ehtl
        anchors.left: parent.left
        anchors.leftMargin: 35
        width: 75
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -10
        text: "Host ID"
        //font.pointSize: (!mw.mac())?9:12
        font.pixelSize:12
    }
    Rectangle{
        anchors.left: ehtl.right
        anchors.verticalCenter: ehtl.verticalCenter
        anchors.verticalCenterOffset: 3
        radius: 3
        border.color: "#CDCFD2"
        width: 291
        height: 30
        TextInput{
            id: hostId
            color: "black"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 5
            anchors.topMargin: 5
            width: parent.width-10
            height: 30
            //font.pointSize: (!mw.mac())?10:13
            font.pixelSize:13
            selectByMouse: true
            maximumLength: 30
            inputMask: "XXXXXXXX"
            property string oldText: text
            font.letterSpacing: 1
            onDisplayTextChanged: {
                var tmp= cursorPosition
                console.log(displayText)
                text=displayText.replace(" ","_")
                if(!mw.checkHex(text)){
                    text=oldText
                    tmp--;
                }
                cursorPosition=tmp
                oldText=text
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
            mw.setHostId(hostId.text)
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
