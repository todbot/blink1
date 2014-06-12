import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Component {


    ScrollViewStyle {

        transientScrollBars : false

        scrollBarBackground: Rectangle{
            id : m_background

            opacity : 0.8
            implicitHeight:   18
            implicitWidth:   18

            color : "transparent"

        }
        handle : Rectangle {

            color : "#474644"
            radius : 5
            smooth : true
            opacity : 0.8

            implicitHeight:   8
            implicitWidth:   8

            Component.onCompleted: {
                if( styleData.horizontal === true  ) {
                    anchors.top  = parent.top
                    anchors.topMargin=  5
                    }
                else {
                    anchors.left = parent.left
                    anchors.leftMargin= 5
                    anchors.bottom = parent.bottom
                    anchors.bottomMargin= 3
                    anchors.top = parent.top
                    anchors.topMargin = 3
                }
            }
        }

        incrementControl: Rectangle {
            implicitHeight:   0
            implicitWidth:   0
            visible : false
        }

        decrementControl: Rectangle {
            implicitHeight:   0
            implicitWidth:   0
            visible : false
        }

    }
}
