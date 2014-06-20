import QtQuick 2.0
import "content"
import QtQuick.Controls 1.0
import QtGraphicalEffects 1.0
//import QtWebKit 3.0
//import QtWebKit.experimental 1.0
Image{
    id: mainWindow
    source: "qrc:images/layout/bg-new.jpg"
    property int editModeIndex: -1
/*
    MouseArea{
        id: belka
        z:0
        width: parent.width
        height: 55
        anchors.top: parent.top
        anchors.left: parent.left
        property variant previousPosition
        onPressed: {
            previousPosition = Qt.point(mouseX, mouseY)
        }
        onPositionChanged: {
            if (pressedButtons == Qt.LeftButton) {
                var dx = mouseX - previousPosition.x
                var dy = mouseY - previousPosition.y
                if(mw.checkIfCorrectPositionX(viewerWidget.x+dx)){
                    viewerWidget.setX(viewerWidget.x+dx);
                }else{
                    previousPosition.x=mouseX
                }

                if(mw.checkIfCorrectPositionY(viewerWidget.y+dy,belka.height)){
                    viewerWidget.setY(viewerWidget.y+dy);
                }else{
                    previousPosition.y=mouseY
                }
            }
        }
    }
*/
/*
    PushButton{
        z: 1
        id: minButton
        upSrc: "qrc:images/layout/minimalize-up.png"
        downSrc: "qrc:images/layout/minimalize-hover.png"
        anchors.right: closeButton.left
        anchors.top: parent.top
        onClicked: mw.showMinimize()
    }
    PushButton{
        z: 1
        id: closeButton
        upSrc: "qrc:images/layout/close-up.png"
        downSrc: "qrc:images/layout/close-hover.png"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 30
        onClicked: { mw.markViewerAsClosing(); mw.quit(); }
    }
*/
    function exitEditMode(){
        if(lista.currentIndex != -1 && lista.currentItem) {
            lista.currentItem.editMode=false;
            editModeIndex = -1;
            colorwheel1.indexOfColorPattern = -1;
            lista.restoreName()
            lista.currentIndex=-1
        }
    }
    MouseArea{
        z: -1
        anchors.fill: parent
        onClicked: {
            if(lista.currentIndex != -1 && lista.currentItem) {
                lista.currentItem.editMode=false;
                editModeIndex = -1;
                colorwheel1.indexOfColorPattern = -1;
            }
            lista.restoreName()
            lista.currentIndex=-1

            inputsList.restoreName()

            inputsList2.restoreName()
            inputsList2.restorePath()

            inputsList.currentIndex=-1
            inputsList2.currentIndex=-1

            bigButtons2.restoreName()

            inputsList2M.currentIndex=-1
            inputsList2H.currentIndex=-1

        }
    }

    focus: true
    Keys.onPressed: {
        if(lista.currentIndex!=-1){
            var tmp=lista.currentItem.children[1].children[2].currentIndex
            if (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace)
                if(lista.currentIndex!=-1 && tmp===-1){
                    mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                    mw.removePattern(inputsList.pnm[lista.currentIndex+1])
                    lista.currentIndex=-1
                    editModeIndex=-1
                    mw.updatePatternsList()
                }else if(tmp!=-1){
                    mw.removeColorAndTimeFromPattern(inputsList.pnm[lista.currentIndex+1],tmp)
                    lista.currentItem.children[1].children[2].currentIndex=-1
                    bigButtons2.updateColors();
                }
            if(lista.currentIndex==editModeIndex){
                if(event.key===Qt.Key_Escape){
                    lista.restoreName()
                    lista.currentItem.editMode=false
                    editModeIndex=-1
                }
            }
        }
    }

    function changeColor(x)
    {
        virtualBlink1Color.color=x;
    }
    function changeColor2(x,t){
        if(typeof t === "undefined")
            t=1.0;
        colorwheel1.appAction=true
        colorwheel1.setQColorAndTime(x,t)//1.0)
        colorwheel1.appAction=false
    }
    /// VIRTUAL BLINK
    Image{
        id: devicePanel
        source: "qrc:images/layout/device-bg.png"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 15
        anchors.topMargin: 12  // was 55 with bg.jpg
        Text{
            text: "Device"
            color: "white"
            font.pointSize: (!mw.mac())?10:13
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.top: parent.top
            anchors.topMargin: 22
        }
        Image{
            id: virtualBlink1
            property color col1: "black"
            property color col2: "black"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 35
            source: (mw.isMk2)?"qrc:images/layout/device/device-light-mask.png":"qrc:images/layout/device/device-mk1-light-mask.png"
            Image{
                id: pods
                anchors.centerIn: parent
                source: (mw.isMk2)?"qrc:images/layout/device/device-light-bg-top.png":"qrc:images/layout/device/device-mk1-light-bg.png"
            }
            Image{
                id: pods2
                anchors.centerIn: parent
                source: (mw.isMk2)?"qrc:images/layout/device/device-light-bg-bottom.png":""
            }

            ColorOverlay{
                source: pods
                color: if(!mw.isMk2 || (mw.led===0 || mw.led===1)){ virtualBlink1Color.color; virtualBlink1.col1=virtualBlink1Color.color}else{ virtualBlink1.col1;}
                anchors.fill: pods
                opacity: 0.5
            }
            ColorOverlay{
                source: pods2
                color: if(mw.isMk2) { if(mw.led===0 || mw.led===2){ virtualBlink1Color.color; virtualBlink1.col2=virtualBlink1Color.color;} else {virtualBlink1.col2;}}else{ "transparent"}
                anchors.fill: pods2
                opacity: 0.5
            }

            Rectangle {
                id: virtualBlink1Color
                visible: false
                width: parent.width<parent.height?parent.width:parent.height
                height: width
                color: "#000000"
                border.color: "black"
                border.width: 1
                radius: width*0.5
                anchors.centerIn: parent
            }

        }
        //// KONIEC VIRTUAL BLINK
        Text{
            text: "Status:"
            id: blinkStatuss
            font.pointSize: (!mw.mac())?10:13
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 130
            color: "grey"
        }

        Text{
            id: blinkStatus
            text: mw.blink1
            font.pointSize: (!mw.mac())?10:13
            anchors.left: devicePanel.left
            anchors.leftMargin: 130
            anchors.top: blinkStatuss.top
            color: "black"
        }
        Text{
            id: activePattern
            text: "Playing pattern:"
            anchors.left: blinkStatuss.left
            anchors.top: blinkStatuss.bottom
            anchors.topMargin: 10
            font.pointSize: (!mw.mac())?10:13
            color: "grey"
        }
        Text{
            id: activePatternName
            text: if(mw.activePattern=="") "-"; else mw.activePattern
            anchors.left: blinkStatus.left
            anchors.top: activePattern.top
            color: "black"
            font.pointSize: (!mw.mac())?10:13
            elide: Text.ElideMiddle
            width: 140
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(mw.activePattern==="") return;
                    showFullName.fullName=mw.activePattern
                    showFullName.x=devicePanel.x+activePatternName.x
                    showFullName.y=devicePanel.y+activePatternName.y+activePatternName.height
                    showFullName.width=130
                    showFullName.visible=true
                }
                onExited: {
                    showFullName.visible=false
                }
            }
        }
        Text{
            id: blinkid
            text: "Serial number: "
            anchors.left: blinkStatuss.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 55
            color: "grey"
            font.pointSize: (!mw.mac())?10:13
        }
        TextInput{
            id: blinkidText
            text:  mw.blinkKey
            anchors.left: blinkStatus.left
            anchors.top: blinkid.top
            selectByMouse: true
            readOnly: true
            color: "black"
            font.pointSize: (!mw.mac())?10:13
        }

        Text{
            id: iftttKey
            text: "IFTTT key: "
            anchors.left: blinkStatuss.left
            anchors.top: blinkid.bottom
            anchors.topMargin: 10
            color: "grey"
            font.pointSize: (!mw.mac())?10:13
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    editHostIdMenu.popup()
                }
            }
        }
        TextInput{
            id: iftttKeyName
            text: mw.iftttKey
            anchors.left: blinkStatus.left
            anchors.top: iftttKey.top
            selectByMouse: true
            readOnly: true
            color: "black"
            font.pointSize: (!mw.mac())?10:13
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    editHostIdMenu.popup()
                }
            }
        }
    }

    Image{
        id: recentEventPanel
        source: "qrc:images/layout/events-bg.png"
        anchors.left: devicePanel.left
        anchors.top: devicePanel.bottom
        anchors.topMargin: -6
        Text{
            id: recentEventsTitle
            text: "Recent Events"
            font.pointSize: (!mw.mac())?10:13
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.top: parent.top
            anchors.topMargin: 23
            color: "white"
        }
        ScrollView {
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.top:  parent.top
            anchors.topMargin: 70
            width: 255
            height: 220
            style: MyScrollViewStyle {

            }
            ListView{
                id: recentEvents
                model: mw.getRecentEvents
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width-10
                height: parent.height
                spacing: 10
                clip: true
                MouseArea{
                    anchors.fill: parent
                    z: -1
                    onClicked: exitEditMode()
                }
                onContentYChanged: {
                    if(currentItem){
                        currentItem.children[0].source=currentItem.children[0].upSrc
                    }
                }

                delegate: Row{
                    spacing: 5

                    PushButton{
                        width: 10
                        height: 10
                        upSrc: "qrc:images/layout/delete-up.png"
                        downSrc: "qrc:images/layout/delete-down.png"
                        label.text: ""
                        anchors.top: parent.top 
                        anchors.topMargin: 3
                        //anchors.verticalCenter: parent.verticalCenter
                        //anchors.verticalCenterOffset: -15 // too high
                        onClicked: {
                            recentEvents.currentIndex=index
                            exitEditMode()
                            mw.removeRecentEvent(index)
                        }
                        onPressed: {
                            recentEvents.currentIndex=index
                        }
                    }
                    Text{
                        // FIXME: really?
                        text: model.modelData.substring(model.modelData.indexOf("-")+1)
                        width: 165
                        wrapMode: Text.WordWrap    //wrapMode: Text.WrapAnywhere
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        // FIXME: really?
                        text: model.modelData.substring(0,model.modelData.indexOf("-"))
                        color: "grey"
                        wrapMode: Text.WrapAnywhere
                        font.pointSize: (!mw.mac())?8:12
                    }
                }
            }
        }
        PushButton{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            label.text: "Dismiss all"
            upSrc: "qrc:images/layout/dissmiss-all-up.png"
            downSrc: "qrc:images/layout/dissmiss-all-down.png"
            label.color: "#555555"
            label.font.pointSize: (!mw.mac())?8:10
            onClicked:{
                onClicked: mw.removeAllRecentEvents()
                exitEditMode()
            }
        }
    }
    MyGroupBox{
        id: colorPickerPanelFieldset
        title: "Color Patterns"
        anchors.right: parent.right
        anchors.rightMargin: 37
        anchors.bottom: recentEventPanel.bottom
        anchors.bottomMargin: 20
        height: 355
        width: 360
        z: 1
        visible: tabs.current!=5
    }
    MyGroupBox{
        title: "Color Picker"
        anchors.right: colorPickerPanelFieldset.left
        anchors.rightMargin: 10
        anchors.bottom: colorPickerPanelFieldset.bottom
        height: colorPickerPanelFieldset.height
        width: 450
        z: 1
        visible: tabs.current!=5
    }

    Item{
        id: colorPatternsPanel
        anchors.right: colorPickerPanelFieldset.right
        anchors.rightMargin: 5
        anchors.bottom: colorPickerPanelFieldset.bottom
        visible: tabs.current!=5
        height: 325
        width: 346

        PushButton{
            id: addPatternButton
            visible: tabs.current!=5
            label.text: "Add Pattern"
            label.color: "black"
            label.font.pointSize:  if(mw.mac()) 11; else 8;
            upSrc: "qrc:images/layout/btn-add2-up.png"
            downSrc: "qrc:images/layout/btn-add2-down.png"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: -15
            onClicked: { exitEditMode(); mw.addNewPattern(colorwheel1.getCurrentColor(), colorwheel1.getCurrentTime()); lista.currentIndex++; lista.selectedIndex++}
        }

        z: 3
        ScrollView {
            width: 365; height: 295
            anchors.right: parent.right
            anchors.rightMargin: -18
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            style: MyScrollViewStyle {

            }
            ListView {
                id: lista
                visible: tabs.current!=5
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width-60
                height: parent.height
                model: mw.getPatternsList
                property int selectedIndex: -1
                currentIndex: -1
                clip: true
                MouseArea{
                    z: -1
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onClicked: {
                        if(lista.currentIndex != -1) {
                            lista.currentItem.editMode=false;
                            editModeIndex = -1;
                            colorwheel1.indexOfColorPattern = -1;
                        }
                        lista.restoreName()
                        lista.currentIndex=-1
                    }
                }

                onSelectedIndexChanged: {
                    if(editModeIndex != -1)
                    {
                        lista.currentIndex = editModeIndex;
                        lista.currentItem.editMode = false;
                        lista.currentIndex = selectedIndex;
                    }
                    editModeIndex = -1;
                }

                Component{
                    id: high
                    Rectangle{
                        height: (lista.currentItem && lista.currentIndex!=-1)?lista.currentItem.height:0
                        y: (lista.currentItem && lista.currentIndex!=-1)?lista.currentItem.y:0
                        width: (lista.currentItem && lista.currentIndex!=-1)?lista.currentItem.width:0
                        color: "#777777"
                        opacity: 0.2
                        border.width: (lista.currentItem && lista.currentIndex!=-1 && lista.currentItem.editMode)?4:0
                        border.color: "red"
                        z: (lista.currentItem && lista.currentIndex!=-1)?lista.currentItem.z+1:0
                        radius: 4
                    }
                }
                highlight: high
                highlightFollowsCurrentItem: false

                function restoreName(){
                    if(currentIndex!=-1 && currentItem){
                        if(currentItem.children[1].children[1].focus){
                            currentItem.children[1].children[1].children[0].visible=true
                            currentItem.children[1].children[1].focus=false
                            if(currentItem.children[1].children[1].oldName!="")
                                currentItem.children[1].children[1].text=cutPath2(currentItem.children[1].children[1].oldName)
                        }
                    }
                }
                spacing: 5
                onContentYChanged: {
                    if(currentItem){
                        for(var i=0;i<currentItem.children[1].children.length;i++){
                            if(currentItem.children[1].children[i].objectName==="delButton"){
                                currentItem.children[1].children[i].source="qrc:images/layout/colorpicker/ico-delete-up.png"
                            }else if(currentItem.children[1].children[i].objectName==="addButton"){
                                currentItem.children[1].children[i].source="qrc:images/layout/colorpicker/ico-add-up.png"
                            }else if(currentItem.children[1].children[i].objectName==="repeatButton"){
                                currentItem.children[1].children[i].source=currentItem.children[1].children[i].up
                            }else if(currentItem.children[1].children[i].objectName==="editButton"){
                                currentItem.children[1].children[i].source="qrc:images/layout/colorpicker/ico-edit-up.png"
                            }
                        }
                    }
                }

                delegate: Item{
                    height: pname.height<25?29:pname.height+10
                    width: 347
                    property bool editMode: false

                    onEditModeChanged: {
                        colors.currentIndex = -1;
                        colorwheel1.indexOfColorPattern = -1;
                    }

                    MouseArea{
                        id: mousearea
                        anchors.fill: parent
                        hoverEnabled: true

                        onEntered: {
                            if(editModeIndex == -1)
                            {
                                lista.currentIndex=index
                                parent.focus=true
                            }
                        }
                        onExited: {
                            if(dropDownMenu.visible) return;

                            if(editModeIndex == -1)
                            {
                                lista.currentIndex=-1
                                colors.currentIndex=-1
                            }
                        }
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: {
                            exitEditMode()
                            if(editModeIndex != -1) return;
                            lista.currentIndex=index
                            lista.selectedIndex=index
                            colors.currentIndex=-1
                        }
                    }

                    Row{
                        z:2
                        spacing: 5
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        Item{
                            width: 22
                            height: parent.height
                            Image{
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                source: (model.modelData.playing===0)?"qrc:images/layout/colorpicker/pattern-play-up.png":"qrc:images/layout/colorpicker/pattern-stop-up.png"
                                MouseArea{
                                    cursorShape: Qt.PointingHandCursor
                                    z: 5
                                    anchors.fill: parent
                                    propagateComposedEvents: true
                                    onClicked: {
                                        colorwheel1.indexOfColorPattern=-1
                                        mw.playOrStopPattern(model.modelData.name)
                                        colors.currentIndex=-1
                                    }
                                }
                            }
                        }

                        TextInput {
                            anchors.verticalCenter: parent.verticalCenter
                            id: pname
                            property string oldName: ""
                            clip: true
                            wrapMode: TextInput.WrapAnywhere
                            width: 93
                            text: {
                                if(editMode){
                                    model.modelData.name
                                }else{
                                    cutPath2(model.modelData.name)
                                }
                            }
                            selectByMouse: true
                            maximumLength: 20
                            font.pointSize: (!mw.mac())?8:12
                            onAccepted: {
                                ma.visible=true
                                pname.focus=false
                                if(pname.text.length>=1){
                                    mw.changePatternName(pname.oldName,pname.text)
                                }else{
                                    pname.text=oldName;
                                }
                            }
                            MouseArea {
                                id: ma
                                cursorShape: (editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                                propagateComposedEvents: true
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    if(editModeIndex != -1){
                                        if(editMode) return;
                                        else{
                                            exitEditMode()
                                        }
                                    }

                                    lista.currentIndex=index
                                    colors.currentIndex=-1
                                }

                                onDoubleClicked: {
                                    if(model.modelData.isReadOnly) return;
                                    if(!editMode) return;
                                    mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                                    ma.visible=false
                                    pname.focus=true
                                    pname.forceActiveFocus()
                                    pname.oldName=pname.text
                                    colors.currentIndex=-1
                                    lista.currentIndex=index
                                }
                                hoverEnabled: true
                                onEntered: {
                                    if(editModeIndex != -1) return;
                                    lista.currentIndex=index
                                    colors.currentIndex=-1
                                    showFullName.fullName=model.modelData.name
                                    showFullName.visible=true
                                    showFullName.x=colorPatternsPanel.x+lista.x+lista.currentItem.x+30
                                    showFullName.y=colorPatternsPanel.y+lista.y+lista.currentItem.y+lista.currentItem.height-lista.contentY+20
                                    showFullName.width=100
                                }
                                onExited: {
                                    showFullName.visible=false
                                }
                            }
                        }
                        ListView{
                            anchors.verticalCenter: parent.verticalCenter
                            id: colors
                            property variant listTime: modelData.times
                            property variant listLeds: modelData.leds
                            orientation: ListView.Horizontal
                            width: modelData.colors.length*16

                            height: 10
                            model: modelData.colors
                            spacing: 1
                            currentIndex: -1
                            MouseArea{
                                anchors.fill: parent
                                z: -1
                                onClicked: {
                                    if(editModeIndex != -1) return;
                                    lista.selectedIndex=lista.currentIndex
                                }
                            }

                            delegate: Rectangle{
                                width: 15
                                height: 15
                                color: model.modelData
                                border.color: "black"
                                border.width: {
                                    if(colors.currentIndex==index)
                                        3
                                    else{
                                        if(mw.checkIfColorIsTooBright(model.modelData))
                                            1
                                        else
                                            0
                                    }
                                }
                                Rectangle{
                                    visible: mw.checkIfColorIsTooDark(model.modelData) && colors.currentIndex==index
                                    anchors.centerIn: parent
                                    color: "transparent"
                                    border.width: 2
                                    border.color: "white"
                                    width: parent.width-2
                                    height: parent.height-2
                                    radius: 3
                                }
                                radius: 3
                                anchors.verticalCenter: parent.verticalCenter
                                MouseArea{
                                    cursorShape: (editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton|Qt.RightButton
                                    onClicked: {
                                        if(model.modelData.isReadOnly) return;
                                        if(editMode) {
                                            mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                                            lista.restoreName()
                                            colors.currentIndex=index
                                            mw.setLed(colors.listLeds[index])
                                            colorwheel1.indexOfColorPattern = colors.currentIndex;
                                            colorwheel1.setQColorAndTime(color, colors.listTime[colors.currentIndex]);
                                        }

                                    }
                                }
                            }
                        }
                        Image{
                            visible: lista.currentIndex===index && editMode
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: -1
                            source: "qrc:images/layout/colorpicker/ico-add-up.png"
                            objectName: "addButton"

                            MouseArea {
                                cursorShape: (editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                                id: mouseArea
                                anchors.fill: parent
                                propagateComposedEvents: true
                                onClicked: {
                                    if(model.modelData.isReadOnly) return;
                                    mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                                    lista.restoreName()
                                    mw.addColorAndTimeToPattern(model.modelData.name,colorwheel1.getCurrentColor(),colorwheel1.getCurrentTime())
                                    bigButtons2.updateColors();
                                    colors.currentIndex=colors.model.length-1
                                    colorwheel1.indexOfColorPattern = colors.currentIndex;
                                }
                                onPressed: parent.source= "qrc:images/layout/colorpicker/ico-add-down.png"
                                onReleased: parent.source= "qrc:images/layout/colorpicker/ico-add-up.png"
                            }
                        }
                        Image{
                            anchors.verticalCenter: parent.verticalCenter
                            id: icon
                            objectName: "repeatButton"
                            property string up: "qrc:images/layout/colorpicker/ico-repeat-up.png"
                            property string down: "qrc:images/layout/colorpicker/ico-repeat-down.png"
                            source: up
                            MouseArea{
                                cursorShape: (editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                                anchors.fill: parent
                                propagateComposedEvents: true
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    if(model.modelData.isReadOnly) return;
                                    if(!editMode) return;
                                    mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                                    lista.restoreName()
                                    if(mouse.button==Qt.LeftButton){
                                        mw.changePatternRepeats(model.modelData.name)
                                        colors.currentIndex=-1
                                    }else if(mouse.button==Qt.RightButton){
                                        repeatsMenu.popup()
                                        colors.currentIndex=-1
                                    }

                                    if(model.modelData.repeats!==0){
                                        parent.up="qrc:images/layout/colorpicker/ico-repeat-up.png"
                                        parent.down="qrc:images/layout/colorpicker/ico-repeat-down.png"
                                    }else{
                                        parent.up="qrc:images/layout/colorpicker/ico-play-once-up.png"
                                        parent.down="qrc:images/layout/colorpicker/ico-play-once-down.png"
                                    }
                                    parent.source=parent.up
                                }
                                onPressed: { if(!editMode) return; parent.source=parent.down}
                            }
                            Component.onCompleted: {
                                if(model.modelData.repeats!==0){
                                    up="qrc:images/layout/colorpicker/ico-repeat-up.png"
                                    down="qrc:images/layout/colorpicker/ico-repeat-down.png"
                                }else{
                                    up="qrc:images/layout/colorpicker/ico-play-once-up.png"
                                    down="qrc:images/layout/colorpicker/ico-play-once-down.png"
                                }
                            }
                        }
                        Text{
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.verticalCenterOffset: -1
                            onTextChanged: {
                                if(model.modelData.repeats!==0){
                                    icon.up="qrc:images/layout/colorpicker/ico-repeat-up.png"
                                    icon.down="qrc:images/layout/colorpicker/ico-repeat-down.png"
                                }else{
                                    icon.up="qrc:images/layout/colorpicker/ico-play-once-up.png"
                                    icon.down="qrc:images/layout/colorpicker/ico-play-once-down.png"
                                }
                                icon.source=icon.up
                            }

                            width: 13
                            font.pointSize: (!mw.mac())?8:12
                            text: {
                                var tmp
                                var tmp2=+model.modelData.repeats
                                if(tmp2===-1) tmp=""
                                else if(tmp2==0) tmp=" "
                                else tmp="x"+tmp2
                                tmp
                            }
                            font.bold: true
                            MouseArea{
                                cursorShape: (editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                                anchors.fill: parent
                                propagateComposedEvents: true
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    if(model.modelData.isReadOnly) return;
                                    if(!editMode) return;
                                    mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                                    lista.restoreName()
                                    if(mouse.button==Qt.LeftButton){
                                        mw.changePatternRepeats(model.modelData.name)
                                        colors.currentIndex=-1
                                    }else if(mouse.button==Qt.RightButton){
                                        repeatsMenu.popup()
                                        colors.currentIndex=-1
                                    }
                                    if(model.modelData.repeats!==0){
                                        icon.up="qrc:images/layout/colorpicker/ico-repeat-up.png"
                                        icon.down="qrc:images/layout/colorpicker/ico-repeat-down.png"
                                    }else{
                                        icon.up="qrc:images/layout/colorpicker/ico-play-once-up.png"
                                        icon.down="qrc:images/layout/colorpicker/ico-play-once-down.png"
                                    }
                                    icon.source=icon.up
                                }
                            }
                        }

                    }
                    Image{
                        z: 2
                        visible: lista.currentIndex===index && editMode
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        objectName: "delButton"
                        source: "qrc:images/layout/colorpicker/ico-delete-up.png"
                        MouseArea {
                            cursorShape: (editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                            anchors.fill: parent
                            propagateComposedEvents: true
                            onPressed: parent.source="qrc:images/layout/colorpicker/ico-delete-down.png"
                            onReleased: parent.source="qrc:images/layout/colorpicker/ico-delete-up.png"
                            onClicked: {
                                if(model.modelData.isReadOnly) return;
                                mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
                                mw.removePattern(inputsList.pnm[lista.currentIndex+1])
                                lista.currentIndex=-1

                                editModeIndex = -1;
                                colors.currentIndex = -1;
                                colorwheel1.indexOfColorPattern = -1;
                                mw.updatePatternsList()
                            }
                        }
                    }
                    MouseArea{
                        cursorShape: (!editMode)?Qt.PointingHandCursor:Qt.ArrowCursor
                        z: parent.z+6
                        visible: !editMode
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        width: 27
                        height: parent.height
                        onClicked: {
                            background2.visible=true
                            dropDownMenu.visible=true
                            dropDownMenu.checked=model.modelData.isReadOnly
                            dropDownMenu.pattern_name=model.modelData.name
                            dropDownMenu.x=colorPatternsPanel.x+lista.x+lista.currentItem.x+lista.currentItem.width-dropDownMenu.width
                            var tmp=colorPatternsPanel.y+lista.y+lista.currentItem.y+lista.currentItem.height-lista.contentY
                            if(tmp+dropDownMenu.height<mainWindow.height){
                                dropDownMenu.y=tmp+20
                            }else{
                                dropDownMenu.y=tmp-40-lista.currentItem.height-lista.currentItem.height
                            }
                        }
                    }

                    Rectangle{
                        width: parent.width
                        height: parent.height
                        anchors.fill: parent
                        border.color: "#CDCFD2"
                        border.width: 1
                        radius: 4
                        color: "white"
                        z:1
                        Rectangle{
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 125
                            height: parent.height-10
                            width: 1
                            color: "#EEEEEE"
                        }
                        Rectangle{
                            visible: !editMode
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 26
                            height: parent.height-10
                            width: 1
                            color: "#CDCFD2"
                        }
                        Image{
                            visible: !editMode
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            source: "qrc:/images/layout/colorpicker/arrow-2-up.png"
                        }
                    }

                    Rectangle{
                        id: back
                        anchors.left: parent.left
                        anchors.leftMargin: 130
                        anchors.verticalCenter: parent.verticalCenter
                        z:3
                        width: 190
                        height: parent.height-5
                        opacity: 0.3
                        color: "white"
                        visible: model.modelData.isReadOnly
                    }
                    Image{
                        visible: model.modelData.isReadOnly
                        anchors.verticalCenter: back.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 30
                        source: "qrc:images/layout/lock_small.png"
                        z:4
                    }

                    Text{
                        visible: model.modelData.isReadOnly
                        id: tt
                        z:4
                        anchors.verticalCenter: back.verticalCenter
                        anchors.horizontalCenter: back.horizontalCenter
                        anchors.horizontalCenterOffset: 10
                        text: ""  // "locked"
                        color: "black"
                        font.pointSize: (!mw.mac())?8:12
                    }
                }
            }
        }
    }

    //// KONIEC PATTERN LIST

    Image{
        source: "qrc:images/layout/content-bg.png"
        anchors.top: devicePanel.top
        anchors.left: devicePanel.right

        TabWidget{
            id: tabs
            width: 832; height: 280
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.top: parent.top
            anchors.topMargin: 15
            onCurrentChanged: exitEditMode()
            Rectangle {
                property string title: "Start"
                property string src: "ico-start.png"
                anchors.fill: parent
                color: "transparent"
                MyGroupBox{
                    id: blinkControlsPanel
                    width: 822
                    height: 232
                    title: "Blink controls"

                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    ListView{
                        id: bigButtons1
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.leftMargin: 10
                        anchors.topMargin: 15
                        height: 85
                        width: parent.width
                        orientation: ListView.Horizontal
                        interactive: false
                        ListModel{
                            id: bigButton1model
                            property int currentIndex: 0
                            ListElement{
                                name: "Color Picker"
                                src: "btn-colorpicker"
                            }
                            ListElement{
                                name: "Color Cycle"
                                src: "btn-colorcycle"
                            }
                            ListElement{
                                name: "Mood light"
                                src: "btn-moodlight"
                            }
                            ListElement{
                                name: "Strobe light"
                                src: "btn-strobe"
                            }
                            ListElement{
                                name: "White"
                                src: "btn-white"
                            }
                            ListElement{
                                name: "Off"
                                src: "btn-off"
                            }
                        }
                        MouseArea{
                            anchors.fill: parent
                            z: -1
                            onClicked: exitEditMode()
                        }

                        model: bigButton1model
                        spacing: 1
                        clip: true
                        delegate: Item{
                            width: 79
                            height: 84
                            PushButton{
                                width: 64
                                height: 64
                                upSrc: "qrc:/images/layout/"+src+"-up.png"
                                downSrc: "qrc:/images/layout/"+src+"-down.png"
                                anchors.top: parent.top
                                anchors.horizontalCenter: parent.horizontalCenter
                                label.text: ""
                                z: 2
                                onClicked: {
                                    exitEditMode()
                                    bigButton1model.currentIndex = index;
                                    if(index==0){
                                        mw.on_buttonColorwheel_clicked()
                                        mw.changeColorFromQml(colorwheel1.getCurrentColor());
                                    }else if(index==1){
                                        mw.on_buttonRGBcycle_clicked()
                                    }else if(index==2){
                                        mw.on_buttonMoodlight_clicked()
                                    }else if(index==3){
                                        mw.on_buttonStrobe_clicked()
                                    }else if(index==4){
                                        mw.on_buttonWhite_clicked()
                                    }else if(index==5){
                                        mw.on_buttonOff_clicked()
                                    }
                                }
                            }
                            Text{
                                text: name
                                font.pointSize: (!mw.mac())?10:12
                                anchors.bottom: parent.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                                color: "#666666"
                            }
                            MouseArea{
                                cursorShape: Qt.PointingHandCursor
                                anchors.fill: parent
                                onClicked: {
                                    exitEditMode()
                                    bigButton1model.currentIndex = index;
                                    if(index==0){
                                        mw.on_buttonColorwheel_clicked()
                                        mw.changeColorFromQml(colorwheel1.getCurrentColor());
                                    }else if(index==1){
                                        mw.on_buttonRGBcycle_clicked()
                                    }else if(index==2){
                                        mw.on_buttonMoodlight_clicked()
                                    }else if(index==3){
                                        mw.on_buttonOff_clicked()
                                    }
                                }
                            }
                        }
                    }
                    ListView{
                        function updateColors(){
                            var tmp=bigButtons2.contentX
                            mw.updateColorsOnBigButtons2List();
                            bigButtons2.contentX=tmp
                        }

                        id: bigButtons2
                        anchors.left: bigButtons1.left
                        anchors.top: bigButtons1.bottom
                        anchors.topMargin: 15
                        height: 105
                        width: (model.length*80)>400?400:model.length*80
                        orientation: ListView.Horizontal
                        Component.onCompleted: positionViewAtEnd()
                        MouseArea{
                            anchors.fill: parent
                            z: -1
                            onClicked: exitEditMode()
                        }
                        ScrollBar{
                            flickable: bigButtons2
                            hideScrollBarsWhenStopped: false
                            vertical: false
                            visible: bigButtons2.model.length>5
                        }
                        model: mw.getBigButtons
                        spacing: 1
                        clip: true

                        function restoreName(){
                            if(currentIndex!=-1){
                                if(currentItem.children[0].focus==true){
                                    currentItem.children[0].children[0].visible=true
                                    currentItem.children[0].focus=false
                                    currentItem.children[1].visible=false
                                    if(currentItem.children[0].oldName!="")
                                        currentItem.children[0].text=currentItem.children[0].oldName
                                }
                            }
                        }
                        onContentXChanged: {
                            if(currentItem)
                                currentItem.children[2].source=currentItem.children[2].upSrc
                        }
                        delegate: Item{
                            property string patternName: model.modelData.patternName
                            width: 79
                            height: 84
                            TextInput{
                                id: bbti
                                text: model.modelData.name
                                wrapMode: Text.WrapAnywhere
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                font.pointSize: (!mw.mac())?10:12
                                maximumLength: 12
                                color: "#666666"
                                selectByMouse: true
                                anchors.top: button.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                                property string oldName:""
                                onAccepted: {
                                    if(bbti.text.length>=1)
                                        mw.updateBigButtonName(index,text)
                                    else
                                        bbti.text=bbti.oldName
                                    focus=false
                                    bbma.visible=true
                                    bor.visible=false
                                }
                                MouseArea{
                                    cursorShape: Qt.PointingHandCursor
                                    id: bbma
                                    anchors.fill: parent
                                    onDoubleClicked: {
                                        bigButtons2.restoreName()
                                        bigButtons2.currentIndex=index
                                        visible=false
                                        bbti.focus=true
                                        bbti.forceActiveFocus()
                                        bbti.oldName=bbti.text
                                        bor.visible=true
                                    }
                                    onClicked: exitEditMode()
                                }
                            }
                            Rectangle{
                                id: bor
                                visible: false
                                color: "transparent"
                                border.width: 1
                                border.color: "red"
                                width: parent.width
                                height: bbti.height
                                anchors.top: button.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            PushButton{
                                objectName: "colButton"
                                id: button
                                label.text: ""
                                anchors.top: parent.top
                                anchors.horizontalCenter: parent.horizontalCenter
                                upSrc: "qrc:images/layout/btn-color-up.png"
                                downSrc: "qrc:images/layout/btn-color-down.png"
                                onClicked: {
                                    exitEditMode()
                                    bigButtons2.restoreName()
                                    bigButtons2.currentIndex=index
                                    mw.playBigButton(index)
                                }
                                onPressed: bigButtons2.currentIndex=index

                                Rectangle{
                                    width: 54
                                    height: 54
                                    color: if(model.modelData.patternName=="") model.modelData.col; else "transparent"
                                    anchors.centerIn: parent
                                    radius: 5
                                    MouseArea{
                                        cursorShape: Qt.PointingHandCursor
                                        anchors.fill: parent
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                                        onPressed: {
                                            bigButtons2.currentIndex=index
                                            button.source=button.downSrc
                                        }
                                        onReleased: button.source=button.upSrc
                                        onClicked: {
                                            bigButtons2.restoreName()
                                            if (mouse.button == Qt.RightButton){
                                                bigButtons2.currentIndex=index
                                                bbti.oldName=bbti.text
                                                bigButtonsMenu.popup()
                                                if(mw.mac()) mw.updateBigButtons()
                                            }else{
                                                exitEditMode()
                                                bigButtons2.currentIndex=index
                                                mw.playBigButton(index)
                                            }
                                        }
                                    }
                                    Image{
                                        anchors.centerIn: parent
                                        source: "qrc:images/layout/btn-color-mask-icon.png"
                                    }
                                }
                                ListView{
                                    id: colorsList
                                    anchors.centerIn: parent
                                    visible: !(patternName=="")
                                    model: mw.getFullColorsFromPattern(patternName)
                                    interactive: false
                                    width: 54
                                    height: 54
                                    MouseArea{
                                        cursorShape: Qt.PointingHandCursor
                                        anchors.fill: parent
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                                        onPressed: {
                                            bigButtons2.currentIndex=index
                                            button.source=button.downSrc
                                        }
                                        onReleased: button.source=button.upSrc
                                        onClicked: {
                                            bigButtons2.restoreName()
                                            if (mouse.button == Qt.RightButton){
                                                bigButtons2.currentIndex=index
                                                bbti.oldName=bbti.text
                                                bigButtonsMenu.popup()
                                                if(mw.mac()) mw.updateBigButtons()
                                            }else{
                                                exitEditMode()
                                                bigButtons2.currentIndex=index
                                                mw.playBigButton(index)
                                            }
                                        }
                                    }
                                    Image{
                                        anchors.centerIn: parent
                                        source: "qrc:images/layout/btn-color-mask-icon.png"
                                    }
                                    delegate: Rectangle{
                                        width: 54
                                        height: 54.0/colorsList.model.length
                                        color: model.modelData
                                    }
                                }
                            }
                        }
                    }
                    PushButton{
                        anchors.left: bigButtons2.right
                        anchors.top: bigButtons2.top
                        anchors.leftMargin: 3
                        label.text: ""
                        upSrc: "qrc:images/layout/btn-add-up.png"
                        downSrc: "qrc:images/layout/btn-add-down.png"
                        onClicked: {
                            exitEditMode()
                            mw.addNewBigButton("BigButton", colorwheel1.getCurrentColor());
                            bigButtons2.positionViewAtEnd()
                        }
                        Text{
                            anchors.top: parent.bottom
                            anchors.topMargin: 2
                            font.pointSize: (!mw.mac())?10:12
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: "#666666"
                            text: "Add"
                            MouseArea{
                                cursorShape: Qt.PointingHandCursor
                                anchors.fill: parent
                                onClicked: {
                                    exitEditMode()
                                    mw.addNewBigButton("BigButton", colorwheel1.getCurrentColor());
                                    bigButtons2.positionViewAtEnd()
                                }
                            }
                        }
                    }
                }
            }
            Rectangle {
                id: pattlist
                property string title: "IFTTT"
                property string src: "ico-ifttt.png"
                anchors.fill: parent
                color: "transparent"

                MyGroupBox{
                    title: "IFTTT Rules"
                    width: 822
                    height: 232
                    id: iftttRulesTitle
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    Image{
                        source: "qrc:images/layout/list-bg.png"
                        anchors.centerIn: parent
                    }
                }


                ///INPUTS LIST
                Row{
                    id: tableTitle
                    spacing: 25
                    height: 25
                    width: parent.width
                    anchors.left: iftttRulesTitle.left
                    anchors.leftMargin: 25
                    anchors.top: iftttRulesTitle.top
                    anchors.topMargin: 20
                    Text {
                        text: "Name"
                        width: 170
                        font.bold: true
                        color: "#999999"
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        text: "Pattern"
                        width: 170
                        font.bold: true
                        color: "#999999"
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        text: "Last Checked"
                        width: 138
                        font.bold: true
                        color: "#999999"
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        text: "Source"
                        width: 160
                        font.bold: true
                        color: "#999999"
                        font.pointSize: (!mw.mac())?8:12
                    }
                }
                ScrollView {
                    anchors.left: tableTitle.left
                    anchors.leftMargin: -15
                    anchors.top: tableTitle.bottom
                    anchors.topMargin: 5
                    width: 800; height: 130
                    style: MyScrollViewStyle {

                    }

                    ListView {
                        id: inputsList
                        property variant pnm: mw.getPatternsNames
                        anchors.fill: parent
                        clip: true
                        model: mw.getIFTTTList//myModel
                        currentIndex: -1
                        spacing: 3

                        function restoreName(){
                            if(currentIndex!=-1 && currentItem){
                                currentItem.edit=false
                                if(currentItem.children[1].children[0].focus){
                                    currentItem.children[1].children[0].children[0].visible=true
                                    currentItem.children[1].children[0].focus=false
                                    if(currentItem.children[1].children[0].oldName!="")
                                        currentItem.children[1].children[0].text=currentItem.children[1].children[0].oldName
                                }
                            }
                        }

                        Component{
                            id: highi
                            Rectangle{
                                height: (inputsList.currentItem && inputsList.currentIndex!=-1)?inputsList.currentItem.height:0
                                y: (inputsList.currentItem && inputsList.currentIndex!=-1)?inputsList.currentItem.y:0
                                width: (inputsList.currentItem && inputsList.currentIndex!=-1)?inputsList.currentItem.width:0
                                color: "#777777"
                                opacity: 0.2
                            }
                        }
                        highlight: highi
                        highlightFollowsCurrentItem: true

                        MouseArea{
                            anchors.fill: parent
                            z: -1
                            onClicked: {exitEditMode(); inputsList.restoreName() }
                        }
                        onContentYChanged: {
                            if(currentItem){
                                var pom=currentItem.children[1].children.length-1;
                                currentItem.children[1].children[pom].source=currentItem.children[1].children[pom].upSrc
                            }
                        }

                        delegate:Item{
                            id: del
                            property bool edit: false
                            height: {
                                if(ti.height<25) 29
                                else{
                                    ti.height+7
                                }
                            }
                            width: 800
                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    exitEditMode();
                                    inputsList.restoreName();
                                    inputsList.currentIndex=index;
                                    if(mouse.button==Qt.RightButton){
                                        iftttMenu.name=ti.inputname
                                        iftttMenu.popup()
                                        inputsList.restoreName();
                                        inputsList.currentIndex=index
                                        if(mw.mac()) mw.updateInputsList()
                                    }
                                }
                            }
                            Row{
                                id: c
                                spacing: 32
                                height: parent.height
                                width: parent.width
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                TextInput {
                                    id: ti
                                    property string inputname:model.modelData.name
                                    property string oldName: ""
                                    wrapMode: TextInput.WrapAnywhere
                                    clip: true
                                    width: 160
                                    text: model.modelData.arg1
                                    selectByMouse: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    font.pointSize: (!mw.mac())?8:12
                                    onAccepted: {
                                        mai.visible=true
                                        focus=false
                                        if(ti.text.length>=1)
                                            mw.updateInputsArg1(inputname,ti.text)
                                        else
                                            ti.text=oldName
                                        del.edit=false

                                    }

                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: mai
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList.restoreName();
                                            inputsList.currentIndex=index
                                        }
                                        onDoubleClicked: {
                                            exitEditMode();
                                            mai.visible=false
                                            inputsList.restoreName();
                                            inputsList.currentIndex=index
                                            ti.focus=true
                                            ti.forceActiveFocus()
                                            ti.oldName=ti.text
                                            del.edit=true
                                        }
                                    }
                                    Rectangle{
                                        anchors.centerIn: parent
                                        width: parent.width+2
                                        height: parent.height+2
                                        border.color: "red"
                                        border.width: del.edit?2:0
                                        z: parent.z-1
                                        color: "transparent"
                                    }
                                }
                                Text{
                                    elide: Text.ElideMiddle
                                    id: pName
                                    height: 29
                                    width: 100
                                    text: if(model.modelData.patternName==="") "no pattern chosen"; else model.modelData.patternName
                                    font.underline: (model.modelData.patternName==="")?true:false
                                    color: (model.modelData.patternName==="")?"#777777":"black"
                                    font.pointSize: if(mw.mac()) 11; else 8;
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: 2
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: ma2
                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        width: parent.width+20
                                        height: parent.height
                                        onClicked: {
                                            exitEditMode();
                                            inputsList.restoreName();
                                            inputsList.currentIndex=index

                                            comboPattern.visible=true
                                            comboPattern.x=tabs.parent.x+tabs.x+pattlist.x+inputsList.x+pName.x+20
                                            comboPattern.y=tabs.parent.y+tabs.y+pattlist.y+tableTitle.y+inputsList.y+pName.parent.y+inputsList.currentItem.y-inputsList.contentY+pName.y-1+10
                                            for(var i=0;i<inputsList.pnm.length;i++)
                                                if(inputsList.pnm[i]===pName.text){
                                                    comboPattern.curIn=i;
                                                    break;
                                                }
                                            comboPattern.inputName=ti.inputname
                                        }

                                    }
                                    Image{
                                        height: 29
                                        source: "qrc:images/layout/colorpicker/select-130-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -10
                                        anchors.top: parent.top
                                        anchors.topMargin: -7
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }
                                Item{
                                    height: 1
                                    width: 31
                                }

                                Text{
                                    id: lTime
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 25
                                    width: 130
                                    text: model.modelData.time
                                    font.pointSize: (!mw.mac())?8:12
                                }
                                Text{
                                    id: lSource
                                    elide: Text.ElideMiddle
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 25
                                    width: 160
                                    text: model.modelData.arg2
                                    font.pointSize: (!mw.mac())?8:12
                                }

                                PushButton{
                                    label.text: ""
                                    visible: inputsList.currentIndex==index
                                    upSrc: "qrc:images/layout/colorpicker/ico-delete-up.png"
                                    downSrc: "qrc:images/layout/colorpicker/ico-delete-down.png"
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    onClicked: {
                                        exitEditMode();
                                        inputsList.restoreName();
                                        inputsList.currentIndex=-1
                                        mw.removeInput(model.modelData.name,true)
                                    }
                                }
                            }
                            Image{
                                anchors.left: parent.left
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: -2
                                source: "qrc:images/layout/list-row-separator.png"
                            }
                        }
                    }
                }
                PushButton{
                    label.text: ""
                    anchors.left: iftttRulesTitle.left
                    anchors.bottom: iftttRulesTitle.bottom
                    anchors.bottomMargin: 15
                    anchors.leftMargin: 10
                    z: 3
                    upSrc: "qrc:images/layout/btn-add3-up.png"
                    downSrc: "qrc:images/layout/btn-add3-down.png"
                    onClicked: {
                        exitEditMode();
                        mw.createNewIFTTTInput()
                        inputsList.restoreName();
                        inputsList.currentIndex=-1
                    }
                }
                //// KONIEC INPUTS LIST
            }

            Rectangle {
                id: toolslist
                property string title: "Tools"
                property string src: "ico-tools.png"
                anchors.fill: parent
                color: "transparent"
                MyGroupBox{
                    title: "Scripts/Files/URLs"
                    width: 822
                    height: 232
                    id: toolsTitle
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    Image{
                        source: "qrc:images/layout/list-bg.png"
                        anchors.centerIn: parent
                    }
                }

                ///INPUTS LIST
                Row{
                    id: tableTitle2
                    spacing: 25
                    height: 25
                    width: parent.width
                    anchors.left: toolsTitle.left
                    anchors.leftMargin: 25
                    anchors.top: toolsTitle.top
                    anchors.topMargin: 20
                    Text {
                        width: 133
                        text: "Name"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 70
                        text: "Type"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 183
                        text: "Path"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 128
                        text: "Last val"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 133
                        text: "Frequency"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                }
                ScrollView {
                    width: 800; height: 120
                    anchors.left: tableTitle2.left
                    anchors.leftMargin: -15
                    anchors.top: tableTitle2.bottom
                    anchors.topMargin: 5
                    style: MyScrollViewStyle {

                    }
                    ListView {
                        id: inputsList2
                        property variant pnm: mw.getPatternsNames
                        anchors.fill: parent
                        clip: true
                        model: mw.getInputsList
                        currentIndex: -1
                        spacing: 3


                        function restoreName(){
                            if(currentIndex!=-1 && currentItem){
                                currentItem.edit=false
                                if(currentItem.children[1].children[0].focus===true){
                                    currentItem.children[1].children[0].children[0].visible=true
                                    currentItem.children[1].children[0].focus=false
                                    if(currentItem.children[1].children[0].oldName!="")
                                        currentItem.children[1].children[0].text=currentItem.children[1].children[0].oldName
                                }
                            }
                        }
                        function restorePath(){
                            if(currentIndex!=-1 && currentItem){
                                currentItem.edit2=false
                                if(currentItem.children[1].children[2].focus===true){
                                    currentItem.children[1].children[2].children[0].visible=true
                                    currentItem.children[1].children[2].focus=false
                                    if(currentItem.children[1].children[2].oldPath!="")
                                        currentItem.children[1].children[2].text=currentItem.children[1].children[2].oldPath
                                }
                            }
                        }
                        Component{
                            id: highi2
                            Rectangle{
                                height: (inputsList2.currentItem && inputsList2.currentIndex!=-1)?inputsList2.currentItem.height:0
                                y: (inputsList2.currentItem && inputsList2.currentIndex!=-1)?inputsList2.currentItem.y:0
                                width: (inputsList2.currentItem && inputsList2.currentIndex!=-1)?inputsList2.currentItem.width:0
                                color: "#666666"
                                opacity: 0.2
                            }
                        }
                        highlight: highi2
                        highlightFollowsCurrentItem: true

                        MouseArea{
                            anchors.fill: parent
                            z: -1
                            onClicked: {exitEditMode(); inputsList2.restoreName(); inputsList2.restorePath() }
                        }
                        onContentYChanged: {
                            if(currentItem){
                                var pom=currentItem.children[1].children.length-1;
                                currentItem.children[1].children[pom].source=currentItem.children[1].children[pom].upSrc
                            }
                        }
                        delegate:Item{
                            id: del2
                            property bool edit:false
                            property bool edit2:false
                            height:  {
                                if(lpath.height<25 && ti2.height<25) 29
                                else{
                                    if(lpath.height>ti2.height)
                                        lpath.height+7
                                    else
                                        ti2.height+7
                                }
                            }

                            width: 800
                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    exitEditMode();
                                    inputsList2.restoreName();
                                    inputsList2.restorePath();
                                    inputsList2.currentIndex=index
                                    if(mouse.button==Qt.RightButton){
                                        toolsMenu.name=ti2.inputname
                                        toolsMenu.popup()
                                        inputsList2.restoreName();
                                        inputsList2.restorePath();
                                        inputsList2.currentIndex=index
                                        if(mw.mac()) mw.updateInputsList()
                                    }
                                }
                            }
                            Row{
                                id: c2
                                spacing: 25
                                width: parent.width
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                TextInput {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    id: ti2
                                    wrapMode: TextInput.WrapAnywhere
                                    property string inputname:model.modelData.name
                                    property string oldName: ""
                                    width: 133
                                    text: model.modelData.name
                                    selectByMouse: true
                                    clip: true
                                    font.pointSize: (!mw.mac())?8:12
                                    onAccepted: {
                                        mai2.visible=true
                                        focus=false
                                        if(ti2.text.length>=1)
                                            mw.changeInputName(oldName,ti2.text)
                                        else
                                            ti2.text=oldName
                                        del2.edit=false
                                    }
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: mai2
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2.restoreName();
                                            inputsList2.restorePath();
                                            inputsList2.currentIndex=index
                                        }
                                        onDoubleClicked: {
                                            exitEditMode();
                                            inputsList2.restoreName();
                                            inputsList2.restorePath();
                                            inputsList2.currentIndex=index
                                            mai2.visible=false
                                            ti2.focus=true
                                            ti2.forceActiveFocus()
                                            ti2.oldName=ti2.text
                                            del2.edit=true
                                        }
                                    }
                                    Rectangle{
                                        anchors.centerIn: parent
                                        width: parent.width+2
                                        height: parent.height+2
                                        border.color: "red"
                                        border.width: del2.edit?2:0
                                        z: parent.z-1
                                        color: "transparent"
                                    }
                                }
                                Text{
                                    id: pName2
                                    width: 68
                                    text: model.modelData.type
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    font.pointSize: (!mw.mac())?8:12
                                    MouseArea {
                                        id: ma22
                                        cursorShape: Qt.PointingHandCursor
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2.restoreName();
                                            inputsList2.restorePath();
                                            inputsList2.currentIndex=index

                                            comboPattern2.visible=true
                                            comboPattern2.x=tabs.parent.x+tabs.x+toolslist.x+inputsList2.x+pName2.x+25
                                            comboPattern2.y=tabs.parent.y+tabs.y+toolslist.y+tableTitle2.y+inputsList2.y+pName2.parent.y+inputsList2.currentItem.y-inputsList2.contentY+pName2.y+60
                                            for(var i=0;i<comboPattern2.items.count;i++){
                                                if(comboPattern2.items.get(i).name===pName2.text){
                                                    comboPattern2.curIn=i;
                                                    break;
                                                }
                                            }
                                            comboPattern2.inputName=ti2.inputname
                                        }
                                    }
                                    Image{
                                        height: 24
                                        source: "qrc:images/layout/colorpicker/select-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -7
                                        anchors.top: parent.top
                                        anchors.topMargin: -5
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }

                                TextInput{
                                    id: lpath
                                    property string wholepath: model.modelData.arg1
                                    property string oldPath: ""
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    width: 183
                                    clip: true
                                    wrapMode: TextInput.WrapAnywhere
                                    text: cutPath(wholepath)
                                    font.pointSize: (!mw.mac())?8:12
                                    onAccepted: {
                                        map.visible=true
                                        lpath.focus=false
                                        if(lpath.text.length>=1){
                                            lpath.wholepath=lpath.text
                                            lpath.text=cutPath(lpath.text)
                                            mw.updateInputsArg1(ti2.inputname,lpath.wholepath)
                                        }else{
                                            lpath.text=cutPath(lpath.oldPath)
                                        }
                                        del2.edit2=false
                                    }

                                    MouseArea{
                                        cursorShape: Qt.PointingHandCursor
                                        id: map
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2.restoreName();
                                            inputsList2.restorePath();
                                            inputsList2.currentIndex=index
                                        }
                                        onDoubleClicked: {
                                            exitEditMode();
                                            inputsList2.restoreName();
                                            inputsList2.restorePath();
                                            inputsList2.currentIndex=index
                                            if(pName2.text=="FILE" || pName2.text=="SCRIPT"){
                                                lpath.oldPath=lpath.wholepath
                                                lpath.text=mw.selectFile(ti2.inputname)
                                                if(lpath.text.length>=1){
                                                    lpath.wholepath=lpath.text
                                                    lpath.text=cutPath(lpath.text)
                                                    mw.updateInputsArg1(ti2.inputname,lpath.wholepath)
                                                }else{
                                                    lpath.text=cutPath(lpath.oldPath)
                                                }
                                            }else{
                                                map.visible=false
                                                lpath.focus=true
                                                lpath.forceActiveFocus()
                                                lpath.text=lpath.wholepath
                                                lpath.oldPath=lpath.text
                                                del2.edit2=true
                                            }
                                        }
                                        hoverEnabled: true
                                        onEntered: {
                                            if(!lpath.focus){
                                                lpath.text=lpath.wholepath
                                            }
                                        }
                                        onExited: {
                                            if(!lpath.focus){
                                                lpath.text=cutPath(lpath.wholepath)
                                            }
                                        }
                                    }
                                    Rectangle{
                                        anchors.centerIn: parent
                                        width: parent.width+2
                                        height: parent.height+2
                                        border.color: "red"
                                        border.width: del2.edit2?2:0
                                        z: parent.z-1
                                        color: "transparent"
                                    }

                                }
                                Text{
                                    id: llastval
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 25
                                    width: 133
                                    text: model.modelData.arg2
                                    font.pointSize: (!mw.mac())?8:12
                                }

                                Text{
                                    id: pFreq
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    width: 70
                                    font.pointSize: (!mw.mac())?8:12
                                    text: {
                                        var tmp=model.modelData.freq
                                        if(tmp==1) "5 sec";
                                        else if(tmp==3) "15 sec";
                                        else if(tmp==6) "30 sec";
                                        else if(tmp==12) "1 min";
                                        else if(tmp==60) "5 min";
                                        else ""
                                    }
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: ma222
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2.restoreName();
                                            inputsList2.restorePath();
                                            inputsList2.currentIndex=index

                                            comboFreq.visible=true
                                            comboFreq.x=tabs.parent.x+tabs.x+toolslist.x+inputsList2.x+pFreq.x+25
                                            comboFreq.y=tabs.parent.y+tabs.y+toolslist.y+tableTitle2.y+inputsList2.y+pFreq.parent.y+inputsList2.currentItem.y-inputsList2.contentY+pFreq.y+40
                                            for(var i=0;i<comboFreq.items.count;i++){
                                                if(comboFreq.items.get(i).name===pFreq.text){
                                                    comboFreq.curIn=i;
                                                    break;
                                                }
                                            }
                                            comboFreq.inputName=ti2.inputname
                                        }
                                    }
                                    Image{
                                        height: 24
                                        source: "qrc:images/layout/colorpicker/select-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -7
                                        anchors.top: parent.top
                                        anchors.topMargin: -5
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }
                                Item{
                                    width: 5
                                    height: 1
                                }

                                PushButton{
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    label.text: ""
                                    visible: inputsList2.currentIndex==index
                                    upSrc: "qrc:images/layout/colorpicker/ico-delete-up.png"
                                    downSrc: "qrc:images/layout/colorpicker/ico-delete-down.png"
                                    z: 5
                                    onClicked: {
                                        exitEditMode();
                                        inputsList2.restoreName();
                                        inputsList2.restorePath();
                                        inputsList2.currentIndex=-1
                                        mw.removeInput(model.modelData.name,true)
                                    }
                                }
                            }
                            Image{
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: -2
                                anchors.left: parent.left
                                source: "qrc:images/layout/list-row-separator.png"

                            }
                        }
                    }
                }
                PushButton{
                    label.text: ""
                    anchors.left: toolsTitle.left
                    anchors.bottom: toolsTitle.bottom
                    anchors.bottomMargin: 15

                    anchors.leftMargin: 10
                    upSrc: "qrc:images/layout/btn-add3-up.png"
                    downSrc: "qrc:images/layout/btn-add3-down.png"
                    onClicked: {
                        exitEditMode();
                        mw.createNewInput()
                        inputsList2.restoreName();
                        inputsList2.restorePath();
                        inputsList2.currentIndex=-1
                    }
                }
                //// KONIEC INPUTS LIST
            }

            //MAIL LIST
            Rectangle {
                id: maillist
                z: 10
                property string title: "Mail"
                property string src: "mail-ico.png"
                anchors.fill: parent
                color: "transparent"
                MyGroupBox{
                    title: "Mail: IMAP/POP3/Gmail"
                    width: 822
                    height: 232
                    id: toolsTitleM
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    Image{
                        source: "qrc:images/layout/list-bg.png"
                        anchors.centerIn: parent
                    }
                }

                ///INPUTS LIST
                Row{
                    id: tableTitle2M
                    spacing: 25
                    height: 25
                    width: parent.width
                    anchors.left: toolsTitleM.left
                    anchors.leftMargin: 25
                    anchors.top: toolsTitleM.top
                    anchors.topMargin: 20
                    Text {
                        width: 133
                        text: "Name"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 145
                        text: "Mail Account"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 72
                        text: "Refresh rate"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 133
                        text: "Pattern"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 133
                        text: "Last status"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                }
                ScrollView {
                    width: 800; height: 120
                    anchors.left: tableTitle2M.left
                    anchors.leftMargin: -15
                    anchors.top: tableTitle2M.bottom
                    anchors.topMargin: 5
                    style: MyScrollViewStyle {

                    }
                    ListView {
                        id: inputsList2M
                        z: 100
                        property variant pnm: mw.getPatternsNames
                        clip: true
                        model: mw.getMailsList//mw.getInputsList//myModel
                        currentIndex: -1
                        spacing: 3

                        Component{
                            id: highi2M
                            Rectangle{
                                height: (inputsList2M.currentItem && inputsList2M.currentIndex!=-1)?inputsList2M.currentItem.height:0
                                y: (inputsList2M.currentItem && inputsList2M.currentIndex!=-1)?inputsList2M.currentItem.y:0
                                width: (inputsList2M.currentItem && inputsList2M.currentIndex!=-1)?inputsList2M.currentItem.width:0
                                color: "#666666"
                                opacity: 0.2
                            }
                        }
                        highlight: highi2M
                        highlightFollowsCurrentItem: true

                        MouseArea{
                            anchors.fill: parent
                            z: -1
                            onClicked: {exitEditMode(); }
                        }
                        onContentYChanged: {
                            if(currentItem){
                                var pom=currentItem.children[1].children.length-1;
                                currentItem.children[1].children[pom].source=currentItem.children[1].children[pom].upSrc
                            }
                        }
                        delegate:Item{
                            id: del2M
                            property bool edit:false
                            property bool edit2:false
                            height:  {
                                if(ti2M.height<25 && mailaccount.height<25) 29
                                else{
                                    if(ti2M.height>mailaccount.height)
                                        ti2M.height+7
                                    else
                                        mailaccount.height+7
                                }
                            }

                            width: 800
                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    exitEditMode();
                                    inputsList2M.currentIndex=index
                                    if(mouse.button==Qt.RightButton){
                                        mailMenu.name=ti2M.inputname
                                        mailMenu.popup()
                                        inputsList2M.currentIndex=index
                                        if(mw.mac()) mw.updateMail()
                                    }
                                }
                                onDoubleClicked: {
                                    if(mailpopup.visible) return;
                                    mailpopup.oldname=model.modelData.name
                                    mw.markEmailEditing(model.modelData.name,true)
                                    mailpopup.editData(model.modelData.name,model.modelData.type,model.modelData.server,model.modelData.login,model.modelData.passwd,model.modelData.port,model.modelData.ssl,model.modelData.result,model.modelData.parser)
                                    mailpopup.visible=true
                                }
                            }
                            Row{
                                id: c2M
                                spacing: 25
                                width: parent.width
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    id: ti2M
                                    wrapMode: TextInput.WrapAnywhere
                                    property string inputname:model.modelData.name
                                    width: 133
                                    text: model.modelData.name
                                    clip: true
                                    font.pointSize: (!mw.mac())?8:12
                                }
                                Text{
                                    id: mailaccount
                                    wrapMode: TextInput.WrapAnywhere
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    width: 150
                                    text: model.modelData.email
                                    font.pointSize: (!mw.mac())?8:12
                                }
                                Text{
                                    id: pFreqM
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    width: 70
                                    font.pointSize: (!mw.mac())?8:12
                                    text: {
                                        var tmp=model.modelData.freq
                                        if(tmp===12) "1 min";
                                        else if(tmp===60) "5 min";
                                        else if(tmp===180) "15 min";
                                        else if(tmp===360) "30 min";
                                        else if(tmp===720) "1 hour";
                                        else ""
                                    }
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: ma222M
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2M.currentIndex=index

                                            comboFreqM.visible=true
                                            comboFreqM.x=tabs.parent.x+tabs.x+maillist.x+inputsList2M.x+pFreqM.x+25
                                            comboFreqM.y=tabs.parent.y+tabs.y+maillist.y+tableTitle2M.y+inputsList2M.y+pFreqM.parent.y+inputsList2M.currentItem.y-inputsList2M.contentY+2+45
                                            for(var i=0;i<comboFreqM.items.count;i++){
                                                if(comboFreqM.items.get(i).name===pFreqM.text){
                                                    comboFreqM.curIn=i;
                                                    break;
                                                }
                                            }
                                            comboFreqM.inputName=ti2M.inputname
                                        }
                                    }
                                    Image{
                                        height: 24
                                        source: "qrc:images/layout/colorpicker/select-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -7
                                        anchors.top: parent.top
                                        anchors.topMargin: -5
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }
                                Text{
                                    id: pNameM
                                    elide: Text.ElideMiddle
                                    height: 29
                                    width: 100
                                    text: if(model.modelData.patternName==="") "no pattern chosen"; else model.modelData.patternName
                                    font.underline: (model.modelData.patternName==="")?true:false
                                    color: (model.modelData.patternName==="")?"#777777":"black"
                                    font.pointSize: if(mw.mac()) 11; else 8;
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: 2
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: ma2M
                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        width: parent.width+20
                                        height: parent.height
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2M.currentIndex=index

                                            comboPatternM.visible=true
                                            comboPatternM.x=tabs.parent.x+tabs.x+maillist.x+inputsList2M.x+pNameM.x+20
                                            comboPatternM.y=tabs.parent.y+tabs.y+maillist.y+tableTitle2M.y+inputsList2M.y+pNameM.parent.y+inputsList2M.currentItem.y-inputsList2M.contentY+pNameM.y-1+20
                                            for(var i=0;i<inputsList2M.pnm.length;i++)
                                                if(inputsList2M.pnm[i]===pNameM.text){
                                                    comboPatternM.curIn=i;
                                                    break;
                                                }
                                            comboPatternM.inputName=ti2M.inputname
                                        }

                                    }
                                    Image{
                                        height: 29
                                        source: "qrc:images/layout/colorpicker/select-130-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -10
                                        anchors.top: parent.top
                                        anchors.topMargin: -7
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }
                                Item{
                                    height: 1
                                    width: 1
                                }

                                Text{
                                    id: llastvalM
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: 2
                                    height: 30
                                    width: 133
                                    text: model.modelData.value
                                    color: (text==="CONNECTION ERROR")? "#c80b0b": "black"
                                    font.underline: (model.modelData.getErrorsList.length>0)? true: false
                                    font.pointSize: (!mw.mac())?8:12
                                    MouseArea{
                                        anchors.fill: parent
                                        cursorShape: (model.modelData.getErrorsList.length>0)?Qt.PointingHandCursor:Qt.ArrowCursor
                                        onClicked: {
                                            inputsList2M.currentIndex=index
                                        }

                                        onDoubleClicked: {
                                            ep.items=model.modelData.getErrorsList
                                            if(ep.items.length>0)
                                                ep.visible=true
                                            inputsList2M.currentIndex=index
                                        }
                                    }
                                }

                                PushButton{
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    label.text: ""
                                    visible: inputsList2M.currentIndex==index
                                    upSrc: "qrc:images/layout/colorpicker/ico-delete-up.png"
                                    downSrc: "qrc:images/layout/colorpicker/ico-delete-down.png"
                                    z: 5
                                    onClicked: {
                                        exitEditMode();
                                        inputsList2M.currentIndex=-1
                                        mw.remove_email(model.modelData.name,true)
                                    }
                                }
                            }
                            Image{
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: -2
                                anchors.left: parent.left
                                source: "qrc:images/layout/list-row-separator.png"

                            }
                        }
                    }
                }
                PushButton{
                    label.text: ""
                    anchors.left: toolsTitleM.left
                    anchors.bottom: toolsTitleM.bottom
                    anchors.bottomMargin: 15
                    anchors.leftMargin: 10
                    upSrc: "qrc:images/layout/btn-add3-up.png"
                    downSrc: "qrc:images/layout/btn-add3-down.png"
                    onClicked: {
                        exitEditMode();
                        if(mailpopup.visible) return;
                        mailpopup.clearData()
                        mailpopup.visible=true
                        mailpopup.oldname=""
                        inputsList2M.currentIndex=-1
                    }
                }
                //// KONIEC Mail LIST
            }

            Rectangle {
                id: hardwarelist
                z: 10
                property string title: "Hardware"
                property string src: "ico-hardware.png"
                anchors.fill: parent
                color: "transparent"
                MyGroupBox{
                    title: "Hardware Rules"
                    width: 822
                    height: 232
                    id: toolsTitleH
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    Image{
                        source: "qrc:images/layout/list-bg.png"
                        anchors.centerIn: parent
                    }
                }

                ///INPUTS LIST
                Row{
                    id: tableTitle2H
                    spacing: 25
                    height: 25
                    width: parent.width
                    anchors.left: toolsTitleH.left
                    anchors.leftMargin: 25
                    anchors.top: toolsTitleH.top
                    anchors.topMargin: 20
                    Text {
                        width: 133
                        text: "Name"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 95
                        text: "Type"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 125
                        text: "Refresh rate"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 155
                        text: "Pattern"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                    Text{
                        width: 133
                        text: "Last status"
                        color: "#999999"
                        font.bold: true
                        font.pointSize: (!mw.mac())?8:12
                    }
                }
                ScrollView {
                    width: 800; height: 120
                    anchors.left: tableTitle2H.left
                    anchors.leftMargin: -15
                    anchors.top: tableTitle2H.bottom
                    anchors.topMargin: 5
                    style: MyScrollViewStyle {

                    }
                    ListView {
                        id: inputsList2H
                        z: 100
                        property variant pnm: mw.getPatternsNames
                        clip: true
                        model: mw.getHardwareList//mw.getInputsList//myModel
                        currentIndex: -1
                        spacing: 3

                        Component{
                            id: highi2H
                            Rectangle{
                                height: (inputsList2H.currentItem && inputsList2H.currentIndex!=-1)?inputsList2H.currentItem.height:0
                                y: (inputsList2H.currentItem && inputsList2H.currentIndex!=-1)?inputsList2H.currentItem.y:0
                                width: (inputsList2H.currentItem && inputsList2H.currentIndex!=-1)?inputsList2H.currentItem.width:0
                                color: "#666666"
                                opacity: 0.2
                            }
                        }
                        highlight: highi2H
                        highlightFollowsCurrentItem: true

                        MouseArea{
                            anchors.fill: parent
                            z: -1
                            onClicked: {exitEditMode(); }
                        }
                        onContentYChanged: {
                            if(currentItem){
                                var pom=currentItem.children[1].children.length-1;
                                currentItem.children[1].children[pom].source=currentItem.children[1].children[pom].upSrc
                            }
                        }
                        delegate:Item{
                            id: del2H
                            property bool edit:false
                            property bool edit2:false
                            height:  {
                                29
                            }

                            width: 800
                            MouseArea{
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked: {
                                    exitEditMode();
                                    inputsList2H.currentIndex=index
                                    if(mouse.button==Qt.RightButton){
                                        hardwareMenu.name=ti2H.inputname
                                        hardwareMenu.popup()
                                        inputsList2H.currentIndex=index
                                        if(mw.mac()) mw.updateHardware()
                                    }
                                }
                                onDoubleClicked: {
                                    if(hardwarepopup.visible) return;
                                    mw.markHardwareEditing(model.modelData.name,true)
                                    hardwarepopup.oldname=model.modelData.name
                                    hardwarepopup.editData(model.modelData.name,model.modelData.type,model.modelData.role,model.modelData.action,model.modelData.lvl)
                                    hardwarepopup.visible=true
                                }
                            }
                            Row{
                                id: c2H
                                spacing: 25
                                width: parent.width
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    id: ti2H
                                    wrapMode: TextInput.WrapAnywhere
                                    property string inputname:model.modelData.name
                                    width: 133
                                    text: model.modelData.name
                                    clip: true
                                    font.pointSize: (!mw.mac())?8:12
                                }
                                Text{
                                    id: typeHardware
                                    wrapMode: TextInput.WrapAnywhere
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    width: 100
                                    font.pointSize: (!mw.mac())?8:12
                                    text: {
                                        var tmp=model.modelData.type
                                        if(tmp===0){
                                            "Battery"
                                        }else if(tmp===1){
                                            "CPU"
                                        }else if(tmp===2){
                                            "RAM"
                                        }
                                    }
                                }
                                Text{
                                    id: pFreqH
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    width: 70
                                    font.pointSize: (!mw.mac())?8:12
                                    text: {
                                        var tmp=model.modelData.freq
                                        if(tmp===12) "1 min";
                                        else if(tmp===60) "5 min";
                                        else if(tmp===180) "15 min";
                                        else if(tmp===360) "30 min";
                                        else if(tmp===720) "1 hour";
                                        else ""
                                    }
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: ma222H
                                        anchors.fill: parent
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2H.currentIndex=index

                                            comboFreqH.visible=true
                                            comboFreqH.x=tabs.parent.x+tabs.x+hardwarelist.x+inputsList2H.x+pFreqH.x+25
                                            comboFreqH.y=tabs.parent.y+tabs.y+hardwarelist.y+tableTitle2H.y+inputsList2H.y+pFreqH.parent.y+inputsList2H.currentItem.y-inputsList2H.contentY+pFreqH.y+pFreqH.y-2+45
                                            for(var i=0;i<comboFreqH.items.count;i++){
                                                if(comboFreqH.items.get(i).name===pFreqH.text){
                                                    comboFreqH.curIn=i;
                                                    break;
                                                }
                                            }
                                            comboFreqH.inputName=ti2H.inputname
                                        }
                                    }
                                    Image{
                                        height: 24
                                        source: "qrc:images/layout/colorpicker/select-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -7
                                        anchors.top: parent.top
                                        anchors.topMargin: -5
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }
                                Item{
                                    height: 1
                                    width: 30
                                }

                                Text{
                                    id: pNameH
                                    elide: Text.ElideMiddle
                                    height: 29
                                    width: 100
                                    text: if(model.modelData.patternName==="") "no pattern chosen"; else model.modelData.patternName
                                    font.underline: (model.modelData.patternName==="")?true:false
                                    color: (model.modelData.patternName==="")?"#777777":"black"
                                    font.pointSize: if(mw.mac()) 11; else 8;
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: 2
                                    MouseArea {
                                        cursorShape: Qt.PointingHandCursor
                                        id: ma2H
                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        width: parent.width+20
                                        height: parent.height
                                        onClicked: {
                                            exitEditMode();
                                            inputsList2H.currentIndex=index

                                            comboPatternH.visible=true
                                            comboPatternH.x=tabs.parent.x+tabs.x+hardwarelist.x+inputsList2H.x+pNameH.x+20
                                            comboPatternH.y=tabs.parent.y+tabs.y+hardwarelist.y+tableTitle2H.y+inputsList2H.y+pNameH.parent.y+inputsList2H.currentItem.y-inputsList2H.contentY+pNameH.y-1+25
                                            for(var i=0;i<inputsList2H.pnm.length;i++)
                                                if(inputsList2H.pnm[i]===pNameH.text){
                                                    comboPatternH.curIn=i;
                                                    break;
                                                }
                                            comboPatternH.inputName=ti2H.inputname
                                        }

                                    }
                                    Image{
                                        height: 29
                                        source: "qrc:images/layout/colorpicker/select-130-bg.png"
                                        anchors.left: parent.left
                                        anchors.leftMargin: -10
                                        anchors.top: parent.top
                                        anchors.topMargin: -7
                                        z: parent.z-1
                                        Image{
                                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }
                                Item{
                                    height: parent.height
                                    width: 21
                                }

                                Text{
                                    id: llastvalH
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: 2
                                    height: 30
                                    width: 105
                                    text: model.modelData.status
                                    color: (model.modelData.done && model.modelData.status!="checking..." && model.modelData.status!="NO VALUE")? "#c80b0b": "black"
                                    font.bold: (model.modelData.done && model.modelData.status!="checking..." && model.modelData.status!="NO VALUE")
                                    font.pointSize: (!mw.mac())?8:12
                                    MouseArea{
                                        anchors.fill: parent
                                        onClicked: {
                                            inputsList2H.currentIndex=index
                                        }
                                    }
                                }

                                PushButton{
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                    label.text: ""
                                    visible: inputsList2H.currentIndex==index
                                    upSrc: "qrc:images/layout/colorpicker/ico-delete-up.png"
                                    downSrc: "qrc:images/layout/colorpicker/ico-delete-down.png"
                                    z: 5
                                    onClicked: {
                                        exitEditMode();
                                        inputsList2H.currentIndex=-1
                                        mw.remove_hardwareMonitor(model.modelData.name,true)
                                    }
                                }
                            }
                            Image{
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin: -2
                                anchors.left: parent.left
                                source: "qrc:images/layout/list-row-separator.png"

                            }
                        }
                    }
                }
                PushButton{
                    label.text: ""
                    anchors.left: toolsTitleH.left
                    anchors.bottom: toolsTitleH.bottom
                    anchors.bottomMargin: 15
                    anchors.leftMargin: 10
                    upSrc: "qrc:images/layout/btn-add3-up.png"
                    downSrc: "qrc:images/layout/btn-add3-down.png"
                    onClicked: {
                        exitEditMode();
                        if(hardwarepopup.visible) return;
                        hardwarepopup.clearData()
                        hardwarepopup.visible=true
                        hardwarepopup.oldname=""
                        inputsList2H.currentIndex=-1
                    }
                }
                //// KONIEC Hardware LIST
            }

            Rectangle {
                property string title: "Help"
                property string src: "ico-help.png"
                color: "transparent"
                anchors.top: parent.top
                width: parent.width
                height: 610  // FIXME: hack

                ScrollView { 
                    anchors.fill: parent
                    anchors.margins: 10 
                   // anchors.leftMargin: 10
                    //anchors.topMargin: 5
                    z: -1 
                    Text {
                        id: helpText
                        baseUrl: "../../help/help/"   // this works (on Mac at least)
                        //text: "<b>Hello</b> <i>World!</i> <img src=\"ifttt1a.png\"><a href=\"index.html\">click me for help</a>"
                        //text: "<b>Hello</b> <i>World!</i><p> <img src=\"ifttt1a.png\">"
                        text: helpTextString
                        textFormat: Text.RichText 
                    }
                }
/*
                    WebView {
                        id: helpWebView
                        anchors.fill: parent
                        url: "http://thingm.com/blink1/blink1control-help/"
                        //url: "../../help/help/index.html"  // doesn't  work on windows
                        //url: "file:../../help/help/index.html"
                        //url: Qt.resolvedUrl("../../help/help/index.html")
                        //url: "qrc:help/index.html"
                        //experimental.preferences.privateBrowsingEnabled: true 
                        //experimental.preferences.fileAccessFromFileURLsAllowed: true 
                       // onLoadingChanged: {
                        //    console.log("webView loadRequest:"+loadRequest.errorString+","+loadRequest.errorDomain+","+loadRequest.url);
                       // }
                    }
                }
*/
           }
        }
    }

    //drop down menus
    Menu {
        id: bigButtonsMenu
        MenuItem {
            text: "Rename button"
            onTriggered: {
                bigButtons2.currentItem.children[0].children[0].visible=false
                bigButtons2.currentItem.children[0].focus=true
                bigButtons2.currentItem.children[1].visible=true
                bigButtons2.currentItem.children[0].forceActiveFocus()
            }
        }
        MenuItem {
            text: "Set to current color"
            onTriggered: {
                mw.updateBigButtonColor(bigButtons2.currentIndex, colorwheel1.getCurrentColor());
            }
        }
        MenuItem {
            text: "Set to current pattern"
            onTriggered: {
                if(lista.currentIndex!=-1) mw.updateBigButtonPatternName(bigButtons2.currentIndex,inputsList.pnm[lista.currentIndex+1])
                else if(mw.getActivePatternName()!="") mw.updateBigButtonPatternName(bigButtons2.currentIndex,mw.getActivePatternName())
                else
                    mw.updateBigButtonColor(bigButtons2.currentIndex, colorwheel1.getCurrentColor());
            }
        }
        MenuItem {
            text: "Set to current led"
            onTriggered: {
                mw.updateBigButtonLed(bigButtons2.currentIndex,ledmodel.currentIndex)
            }
        }
        MenuItem {
            text: "Delete button"
            onTriggered: {
                mw.removeBigButton2(bigButtons2.currentIndex)
            }
        }
    }
    Menu {
        id: patternMenu
        property string pname: ""
        MenuItem {
            text: "Delete pattern"
            onTriggered: mw.removePattern(patternMenu.pname)
        }
    }
    Menu {
        id: colorMenu
        property int idx: -1
        property string pname: ""
        MenuItem {
            text: "Delete"
            onTriggered: mw.removeColorAndTimeFromPattern(colorMenu.pname,colorMenu.idx)
        }
    }
    Menu {
        id: iftttMenu
        property string name: ""
        MenuItem {
            text: "Delete ifttt tool"
            onTriggered: {  inputsList.currentIndex=-1; mw.removeInput(iftttMenu.name);}
        }
    }
    Menu {
        id: toolsMenu
        property string name: ""
        MenuItem {
            text: "Test tool"
            onTriggered: mw.checkInput(toolsMenu.name)
        }
        MenuItem {
            text: "Delete tool"
            onTriggered: {  inputsList2.currentIndex=-1; mw.removeInput(toolsMenu.name);}
        }
    }
    Menu {
        id: mailMenu
        property string name: ""
        MenuItem {
            text: "Check mail"
            onTriggered: mw.checkMail(mailMenu.name)
        }
        MenuItem {
            text: "Delete mail"
            onTriggered: { inputsList2M.currentIndex=-1; mw.remove_email(mailMenu.name);}
        }
    }
    Menu {
        id: hardwareMenu
        property string name: ""
        MenuItem {
            text: "Check hardware monitor"
            onTriggered: mw.checkHardwareMonitor(hardwareMenu.name)
        }
        MenuItem {
            text: "Delete hardware monitor"
            onTriggered: { inputsList2H.currentIndex=-1; mw.remove_hardwareMonitor(hardwareMenu.name)}
        }
    }
    Menu {
        id: repeatsMenu
        MenuItem {
            text: "inf"
            onTriggered: {
                mw.changePatternRepeatsTo(inputsList.pnm[lista.currentIndex+1],-1)
            }

        }
        MenuItem {
            text: "0"
            onTriggered: {
                mw.changePatternRepeatsTo(inputsList.pnm[lista.currentIndex+1],0)
            }
        }
        MenuItem {
            text: "x1"
            onTriggered: {
                mw.changePatternRepeatsTo(inputsList.pnm[lista.currentIndex+1],1)
            }
        }
        MenuItem {
            text: "x2"
            onTriggered: {
                mw.changePatternRepeatsTo(inputsList.pnm[lista.currentIndex+1],2)
            }
        }
        MenuItem {
            text: "x3"
            onTriggered: {
                mw.changePatternRepeatsTo(inputsList.pnm[lista.currentIndex+1],3)
            }
        }
        MenuItem {
            text: "x4"
            onTriggered: {
                mw.changePatternRepeatsTo(inputsList.pnm[lista.currentIndex+1],4)
            }
        }
    }

    Menu {
        id: editHostIdMenu
        MenuItem {
            text: "Copy IFTTT key"
            onTriggered: {
                mw.copyToClipboard( mw.getIftttKey() )
            }
        }
        MenuItem {
            text: "Edit host id"
            onTriggered: {
                hip.visible=true
                hip.hostId=mw.getHostId()
            }
        }
    }

    //color picker
    Item{
        id: colorPickerPanel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.right: colorPatternsPanel.left
        anchors.rightMargin: -30
        visible: tabs.current!=5
        height: 350
        width: 500
        z: 2
        ColorWheel {
            property bool appAction: false
            id: colorwheel1
            visible: tabs.current!=5
            editMode: lista.currentItem && lista.currentIndex != -1 && lista.currentItem.editMode//lista.currentItem.editMode
            indexOfColorPattern: -1
            onCurrentColorNameChanged: {
                if(bigButton1model.currentIndex == 0) //wtf
                    //if( colorwheel1.isUserAction() ) // this doesn't work fully
                {
                    if(!appAction)
                        mw.changeColorFromQml(getCurrentColor());  // need to only call this when user is doing it, not during patterns
                }
                if(colorwheel1.editMode && indexOfColorPattern != -1)
                {
                    var colorName =  getCurrentColor();
                    var time = getCurrentTime();
                    mw.editColorAndTimeInPattern(inputsList.pnm[lista.currentIndex+1], colorName, time, indexOfColorPattern);//lista.currentItem.color.currentIndex);
                    lista.currentItem.children[1].children[2].currentIndex = indexOfColorPattern;
                    bigButtons2.updateColors();
                }
            }
            onTimeChanged: {
                if(colorwheel1.editMode && indexOfColorPattern!=-1)
                {
                    var colorName =  getCurrentColor();
                    var time = getCurrentTime();
                    mw.editColorAndTimeInPattern(inputsList.pnm[lista.currentIndex+1], colorName, time, indexOfColorPattern);
                    lista.currentItem.children[1].children[2].currentIndex = indexOfColorPattern;
                }
            }
        }
        Image{
            id: ledtitle
            visible: tabs.current!=5
            anchors.left: colorwheel1.left
            anchors.leftMargin: 15
            anchors.top: colorwheel1.top
            source: "qrc:images/layout/colorpicker/led-bg.png"
        }
        ListView{
            id: ledmodel
            interactive: false
            visible: tabs.current!=5
            anchors.left: ledtitle.left
            anchors.top: ledtitle.top
            anchors.topMargin: 25
            anchors.leftMargin: 10
            width: ledtitle.width
            highlightMoveDuration: -1
            highlightMoveVelocity: -1
            height: 90
            clip: true
            ListModel{
                id: leds
                ListElement{
                    name: "AB"
                    src: "led-ab.jpg"
                }
                ListElement{
                    name: "A"
                    src: "led-a.jpg"
                }
                ListElement{
                    name: "B"
                    src: "led-b.jpg"
                }
            }
            model: leds
            currentIndex: mw.led
            Component{
                id: lhigh
                Rectangle{
                    color: "#777777"
                    x: ledmodel.currentItem.x
                    y: ledmodel.currentItem.y
                    width: ledmodel.currentItem.width
                    height: 30
                    opacity: 0.2
                    radius: 5
                    z: ledmodel.currentItem.z+1
                }
            }
            MouseArea{
                anchors.fill: parent
                z: -1
                onClicked: exitEditMode()
            }

            highlight: lhigh
            highlightFollowsCurrentItem: true

            delegate: Item{
                height: 30
                width: ledtitle.width-22
                MouseArea{
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill: parent
                    onClicked: {
                        mw.led=index
                        if(colorwheel1.editMode && colorwheel1.indexOfColorPattern != -1){
                            mw.setLedToPattern(inputsList.pnm[lista.currentIndex+1],colorwheel1.indexOfColorPattern,index)
                        }
                    }
                }
                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    text: name
                    font.pointSize: (!mw.mac())?8:11
                    color: "#555555"
                    verticalAlignment: Qt.AlignCenter
                }
                Image{
                    anchors.left: parent.left
                    anchors.leftMargin: 40
                    source: "qrc:images/layout/colorpicker/"+src
                    z: 3
                }
            }
        }
    }
    function cutPath(path){
        var new_path=""
        var size=8;
        if(mw.mac()) size=12;
        if(mw.checkWordWidth(path,size)>183){
            new_path=path.substring(0,10);
            new_path+="..."
            new_path+=path.substring(path.length-10);
        }else{
            new_path=path
        }
        return new_path
    }
    function cutPath2(path){
        var new_path=""
        var size=8;
        if(mw.mac()) size=12;
        var ile=mw.mac()?3:4;
        if(mw.checkWordWidth(path,size)>90){
            new_path=path.substring(0,ile);
            new_path+="..."
            new_path+=path.substring(path.length-ile);
        }else{
            new_path=path
        }
        return new_path
    }

    //popups
    MailPopup{
        x: parent.x+parent.width/2-200
        y: parent.y+50
        id: mailpopup
        visible: false
    }
    HardwarePopup{
        x: parent.x+parent.width/2-200
        y: parent.y+50
        id: hardwarepopup
        visible: false
    }
    HostIdPopup{
        x: parent.x+parent.width/2-200
        y: parent.y+50
        id: hip
        visible: false
    }
    ErrorsPopup{
        id: ep
        visible: false
        x: parent.x+parent.width/2-300
        y: parent.y+50
    }
    Rectangle{
        id: background
        color: "lightgrey"
        x: mainWindow.x
        y: mainWindow.y
        width: mainWindow.width
        height: mainWindow.height
        z: 10
        visible: false
        opacity: 0.1
        MouseArea{
            anchors.fill: parent
            onClicked: {
                parent.visible=false
                comboPatternM.hide()
                comboFreqM.hide()
                comboPatternH.hide()
                comboFreqH.hide()
                comboFreq.hide()
                comboPattern2.hide()
                comboPattern.hide()
            }
        }
    }

    //comboboxes

    ComboBox1{
        id: comboPatternM
        width: 145
        items: inputsList.pnm
        z: background.z+1000
        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick: {
            mw.setPatternNameToEmail(comboPatternM.inputName,txt)
        }
        onHiden: background.visible=false
    }
    ComboBox1{
        id: comboFreqM
        width: 100
        items: timeM
        z: background.z+1000
        visible: false
        ListModel{
            id: timeM
            ListElement{
                name: "1 min"
            }
            ListElement{
                name: "5 min"
            }
            ListElement{
                name: "15 min"
            }
            ListElement{
                name: "30 min"
            }
            ListElement{
                name: "1 hour"
            }
        }
        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick2: {
            mw.setFreqToEmail(comboFreqM.inputName,idx)
        }
        onHiden: background.visible=false
    }

    ComboBox1{
        id: comboPatternH
        width: 145
        items: inputsList.pnm
        z: background.z+1000
        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick: {
            mw.setPatternNameToHardwareMonitor(comboPatternH.inputName,txt)
        }
        onHiden: background.visible=false
    }
    ComboBox1{
        id: comboFreqH
        width: 100
        items: timeH
        z: background.z+1000
        visible: false
        ListModel{
            id: timeH
            ListElement{
                name: "1 min"
            }
            ListElement{
                name: "5 min"
            }
            ListElement{
                name: "15 min"
            }
            ListElement{
                name: "30 min"
            }
            ListElement{
                name: "1 hour"
            }
        }
        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick2: {
            mw.setFreqToHardwareMonitor(comboFreqH.inputName,idx)
        }
        onHiden: background.visible=false
    }

    ComboBox1{
        id: comboFreq
        width: 100
        items: time
        z: background.z+1000
        visible: false
        ListModel{
            id: time
            ListElement{
                name: "5 sec"
            }
            ListElement{
                name: "15 sec"
            }
            ListElement{
                name: "30 sec"
            }
            ListElement{
                name: "1 min"
            }
            ListElement{
                name: "5 min"
            }
        }
        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick2: {
            mw.setFreqToInput(comboFreq.inputName,idx)
        }
        onHiden: background.visible=false
    }
    ComboBox1{
        id: comboPattern2
        width: 100
        items: types
        z: background.z+1000
        visible: false
        ListModel{
            id: types
            ListElement{
                name: "FILE"
            }
            ListElement{
                name: "SCRIPT"
            }
            ListElement{
                name: "URL"
            }
        }
        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick: {
            mw.updateInputsType(comboPattern2.inputName,txt)
        }
        onHiden: background.visible=false
    }
    ComboBox1{
        id: comboPattern
        width: 145
        items: inputsList.pnm
        z: background.z+1000
        visible: false

        onVisibleChanged: {
            if(visible){
                background.visible=true
            }
        }

        onClick: {
            mw.updateInputsPatternName(comboPattern.inputName,txt)
        }
        onHiden: background.visible=false
    }

    Rectangle{
        id: background2
        color: "lightgrey"
        x: mainWindow.x
        y: mainWindow.y
        width: mainWindow.width
        height: mainWindow.height
        z: 10
        visible: false
        opacity: 0.1
        MouseArea{
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                parent.visible=false
                dropDownMenu.visible=false
            }
        }
    }
    DropDownMenu{
        id: dropDownMenu
        z: background2.z+1000
        visible: false
        onClick: {
            dropDownMenu.visible=false
            background2.visible=false
        }
        onEditPattern: {
            mw.stopPattern(inputsList.pnm[lista.currentIndex+1])
            lista.currentItem.editMode=true
            editModeIndex = lista.currentIndex
        }
    }
    Rectangle{
        id: showFullName
        property string fullName: ""
        color: "#F0E68C"
        border.width: 1
        border.color: "black"
        visible: false
        height: txt.height+4
        width: 100
        z: 999
        Text{
            anchors.centerIn: parent
            id: txt
            color: "black"
            width: parent.width-4
            wrapMode: Text.WrapAnywhere
            text: parent.fullName
            font.pointSize: (!mw.mac())?8:12
        }
    }
}
