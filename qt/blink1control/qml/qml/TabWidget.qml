/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Item {
    id: tabWidget


    default property alias content: stack.children
    property int current: 0

    onCurrentChanged: setOpacities()
    Component.onCompleted: setOpacities()

    function setOpacities() {
        for (var i = 0; i < stack.children.length; ++i) {
            stack.children[i].opacity = (i == current ? 1 : 0)
            stack.children[i].visible = (i == current ? 1 : 0)
        }
    }

    Row {
        id: header
        anchors.left: parent.left
        anchors.leftMargin: -1

        anchors.top: parent.top
        anchors.topMargin: -1
        spacing: -1
        Repeater {
            model: stack.children.length
            delegate: Rectangle {                
                width: {//(index!=3)?154:(tabWidget.width-3*154)/*tabWidget.width / stack.children.length*/;
                    140;
                }
                height: 46
                color: "transparent"
                BorderImage {
                    anchors { fill: parent; leftMargin: 0; topMargin: 0; rightMargin: 0 }
                    border { left: 0; right: 0}
                    source: "qrc:images/layout/tab-selected.png"
                    visible: tabWidget.current == index
                }
                Image{
                    source: "qrc:images/layout/"+stack.children[index].src
                    anchors.right: tit.left
                    anchors.rightMargin: 5
                    anchors.verticalCenter: tit.verticalCenter
                }

                Text {
                    id: tit
                    horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter;
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: 10
                    text: stack.children[index].title
                    elide: Text.ElideRight
                    font.bold: tabWidget.current == index
                    //font.pointSize: (!mw.mac())?12:16
                    font.pixelSize:16
                }
                MouseArea {
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill: parent
                    onClicked: tabWidget.current = index
                }
            }
        }
    }

    Item {
        id: stack
        width: tabWidget.width
        anchors.top: header.bottom; anchors.bottom: tabWidget.bottom
    }    
}
