import QtQuick 2.0

Image {
    id: main
    width: 144
    height: 82
    source: "qrc:images/layout/dropdown-bg.png"

    property string pattern_name: ""
    property bool checked: false

    signal click()

    MouseArea{
        anchors.fill: parent
        onClicked:{

        }
    }

    Column{
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.top: parent.top
        anchors.topMargin: 8
        width: parent.width
        height: parent.height
        spacing: 2
        Rectangle{
            width: parent.width-20
            height: 20
            color: "transparent"
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="lightgrey"
                }
                onExited: {
                    parent.color="transparent"
                }
                onClicked: {
                    click()
                    mw.changePatternReadOnly(pattern_name,!checked)
                }
            }

            Image{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source: {
                    if(checked)
                        "qrc:images/layout/unlock.png"
                    else
                        "qrc:images/layout/lock.png"
                }
            }
            Text{
                anchors.left: parent.left
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                text: {
                    if(checked)
                        "Unlock pattern"
                    else
                        "Lock pattern"
                }
            }
        }
        Rectangle{
            width: parent.width-20
            height: 20
            color: "transparent"
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.color="lightgrey"
                }
                onExited: {
                    parent.color="transparent"
                }
                onClicked: {
                    click()
                    mw.copyPattern(pattern_name)
                }
            }

            Image{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:images/layout/copy.png"
            }
            Text{
                anchors.left: parent.left
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                text: "Copy pattern"
            }
        }
    }
}
