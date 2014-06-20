import QtQuick 2.0
import QtQuick.Controls 1.0
Rectangle{
    property alias items: lv.model
    signal click(string txt)
    signal click2(int idx)
    signal hiden()
    signal curIndexChange(int currentIndex)
    property alias lv: lv

    function hide(){
        comboPattern.visible=false
        hiden()
    }

    function getCount(){
        if(items.count!=undefined)
            return items.count
        else
            return items.length
    }

    id: comboPattern
    property alias curIn: lv.currentIndex
    property string inputName: ""
    visible: false
    width: 145
    height: {
        var ile=13;
        if(mw.mac()) ile=15
        if(getCount()*ile>118)
            118
        else{
            if(getCount()*ile<30)
                30
            else
                getCount()*ile
        }
    }

    color: "white"
    border.width: 1
    border.color: "lightgrey"
    z: 1001
    ScrollView {
        width: parent.width
        height: parent.height
        z: parent.z+1
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        style: MyScrollViewStyle {

        }
        ListView{
            id: lv
            clip: true
            z: parent.z+1
            anchors.fill: parent
            onCurrentIndexChanged: {
                curIndexChange(currentIndex)
            }

            Component{
                id: highp
                Rectangle{
                    y: lv.currentItem.y
                    x: lv.currentItem.x-2
                    width: lv.currentItem.width+10
                    height: lv.currentItem.height
                    color: "black"
                    opacity: 0.2
                }
            }
            highlight: highp
            highlightFollowsCurrentItem: true

            delegate: Text{
                width: parent.width-10
                text: model.modelData
                wrapMode: TextInput.WrapAnywhere
                anchors.left: parent.left
                anchors.leftMargin: 2
                font.pointSize: (!mw.mac())?8:12
                MouseArea{
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        lv.currentIndex=index
                    }

                    onClicked: {
                        lv.currentIndex=index
                        hide()
                        click(model.modelData)
                        click2(index)
                    }
                }

            }
        }
    }
}
