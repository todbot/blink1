import QtQuick 2.0
import CursorTools 1.0

import "Base"


MyComboBoxComponent{
    CursorShapeArea{
        cursorShape: Qt.PointingHandCursor
        anchors.fill: parent
    }
    height: 29
    signal click(string txt)
    signal click2(int idx)
    signal curIndexChange(int currentIndex)
    property bool disabled: false

    onCurrentIndexChanged: {
        curIndexChange(currentIndex)
        click2(currentIndex)
    }
    onCurrentTextChanged: {
        click(currentText)
    }

    MouseArea{
        visible: disabled
        anchors.fill: parent
        onClicked:{

        }
        Rectangle{
            anchors.fill: parent
            color: "#E6E6E6"
            radius: 3
            border.width: 1
            border.color: "#CDCFD2"
            opacity: 0.5
        }
    }

    style: ComboBoxStyle{

    }
}
