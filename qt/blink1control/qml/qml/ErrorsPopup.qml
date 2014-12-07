import QtQuick 2.0

Image {
    id: popup
    width: 638
    height: 371
    source: "qrc:/images/layout/popup-errors.png"
    visible: false
    z: 9999
    property variant items
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
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 3
        label.text: ""
        upSrc: "qrc:/images/layout/close-single-up.png"
        downSrc: "qrc:/images/layout/close-single-hover.png"
        onClicked: parent.visible=false
    }
    Text{
        text: "Error type"
        anchors.left: parent.left
        anchors.leftMargin: 55
        anchors.top: parent.top
        anchors.topMargin: 70
        //font.pointSize: (!mw.mac())?8:12
        font.pixelSize:12
    }

    ListView{
        id: errorsList
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 100
        width: parent.width-28
        height: 185
        model: items
        spacing: 5
        clip: true
        ScrollBar{
            flickable: parent
            hideScrollBarsWhenStopped: false
            color: "#B2B2B2"
            visible: errorsList.contentHeight>185
        }

        delegate: Item{
            width: parent.width
            height: errorString.contentHeight>20?errorString.contentHeight:20
            Image{
                id: lp
                source: "qrc:/images/layout/ico-error.png"
                anchors.left: parent.left
                anchors.leftMargin: 30
                anchors.verticalCenter: errorString.verticalCenter
            }

            Text{
                id: errorString
                anchors.left: parent.left
                anchors.leftMargin: 55
                anchors.verticalCenter: parent.verticalCenter
                text: model.modelData
                width: 500
                wrapMode: Text.WrapAnywhere
                //font.pointSize: (!mw.mac())?8:12
                font.pixelSize:12
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
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 22
        label.text: "OK"
        label.color: "black"
        //label.font.pointSize: (!mw.mac())?9:12
        label.font.pixelSize:12
    }
}
