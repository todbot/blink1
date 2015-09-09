import QtQuick 2.0
Item{
    property string title: ""
    Rectangle {
        width: parent.width
        height: parent.height-5
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        color: "transparent"
        border.width: 1
        border.color: "#E0E0E0"
    }
    Rectangle{
        color: "#FBFBFB"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: -2
        anchors.leftMargin: 10
        width: txt.width+10
        height: txt.height
        Text{
            id: txt
            text: title
            color: "#D5D5D5"
            font.bold: true
            anchors.centerIn: parent
            //font.pointSize: (!mw.mac())?9:11
            font.pixelSize:12
        }
    }
}
