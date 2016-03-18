import QtQuick 2.0

Image {
    property string upSrc: "../../images/button-up.png"
    property string downSrc: "../../images/button-down.png"
    property alias label: label

    signal clicked
    signal pressed

    Text {
        id: label
        anchors.centerIn: parent
        font.pixelSize: 20
        color: "white"
    }

    MouseArea {
        cursorShape: Qt.PointingHandCursor
        anchors.fill: parent
        onClicked: parent.clicked()
        onPressed: {
            source = downSrc
            parent.pressed();
        }
        onReleased: source = upSrc
    }

    Component.onCompleted: {
        source = upSrc
    }
}
