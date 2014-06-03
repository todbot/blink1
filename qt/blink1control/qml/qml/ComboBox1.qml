import QtQuick 2.0

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
    //height: (getCount()*13>118)?118:((getCount()*13<30)?30:getCount()*13)//118
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

    //source: "qrc:images/layout/combobox-big-dropdown.png"
    color: "white"
    border.width: 1
    border.color: "lightgrey"
    z: 1001
    ListView{
        id: lv
        //model: inputsList.pnm
        width: parent.width
        height: parent.height
        //spacing: 5
        clip: true
        z: parent.z+1
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
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
        /*MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onExited: {
                comboPattern.visible=false
                //pName.visible=true
            }
        }*/
        ScrollBar{
            flickable: lv
            hideScrollBarsWhenStopped: false
            visible: lv.contentHeight>118
        }

        delegate: Text{            
            width: parent.width-10
            text: model.modelData
            wrapMode: TextInput.WrapAnywhere
            anchors.left: parent.left
            anchors.leftMargin: 2
            font.pointSize: (!mw.mac())?8:12
            //clip: true
            MouseArea{
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    lv.currentIndex=index
                }

                onClicked: {
                    lv.currentIndex=index
                    //comboPattern.visible=false
                    hide()
                    click(model.modelData)
                    click2(index)
                    //mw.updateInputsPatternName(comboPattern.inputName,model.modelData)
                }
            }
            /*Rectangle{
                id: sep
                //source: separator
                color: "lightgrey"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: -1
                anchors.left: parent.left
                height: 1
                width: parent.width
            }*/
        }
    }
}
