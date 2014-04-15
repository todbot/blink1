import QtQuick 2.0

Image {
    property string upSrc: "../../images/button-up.png"
    property string downSrc: "../../images/button-down.png"
    property alias label: label

    signal clicked

    Text {
        id: label
        anchors.centerIn: parent
        font.pixelSize: 20
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked()
        onPressed: source = downSrc
        onReleased: source = upSrc
    }

    Component.onCompleted: {
        source = upSrc
    }
}
