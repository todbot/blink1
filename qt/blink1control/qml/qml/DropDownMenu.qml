import QtQuick 2.0

Image {
    id: main
    width: 144
    height: !checked?107:82
    source: "qrc:images/layout/dropdown-bg.png"

    property string pattern_name: ""
    property bool checked: false

    signal click()
    signal editPattern()

    MouseArea{
        anchors.fill: parent
        onClicked:{
            console.log(main.width+" "+main.height)
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
            visible: !checked
            width: parent.width-20
            height: 20
            color: "transparent"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
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
                    if(checked) return;
                    editPattern();
                    //mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                    //editMode=true
                    //editModeIndex = index
                }
            }

            Image{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:images/layout/colorpicker/ico-edit-up.png"
            }
            Text{
                anchors.left: parent.left
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                text: "Edit pattern"
                font.pointSize: (!mw.mac())?8:12
            }
        }

        Rectangle{
            width: parent.width-20
            height: 20
            color: "transparent"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
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
                font.pointSize: (!mw.mac())?8:12
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
                cursorShape: Qt.PointingHandCursor
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
                font.pointSize: (!mw.mac())?8:12
            }
        }
        Rectangle{
            width: parent.width-20
            height: 20
            color: "transparent"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
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
                    mw.stopPattern(pattern_name)
                    mw.removePattern(pattern_name)
                    mw.updatePatternsList()
                }
            }

            Image{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:images/layout/colorpicker/ico-delete-down.png"
            }
            Text{
                anchors.left: parent.left
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                text: "Delete pattern"
                font.pointSize: (!mw.mac())?8:12
            }
        }
    }
}
