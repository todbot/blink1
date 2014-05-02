import QtQuick 2.0
import QtQuick.Controls 1.0

Image {
    id: popup
    width: 435
    height: 481
    z: 99999
    source: "qrc:/images/layout/popup-bg-mail.png"

    property alias name: description
    property alias type: comboPattern.lv
    property alias lv: comboPattern.lv
    property alias mailserver: mailserver
    property alias username: username
    property alias password: password
    property alias port: port
    property alias ssl: ssl
    property alias unreads: unreads
    property alias subject: subject
    property alias sender: sender
    property alias unreadscount: unreadinput
    property alias subjectinput: subjectinput
    property alias senderinput: senderinput
    property string oldname: ""

    signal openPopup()
    signal closePopup()
    property alias combo: comboPattern

    function clearData(){
        name.text=""
        type.currentIndex=0
        accounttype.text="IMAP"
        mailserver.text=""
        username.text=""
        password.text=""
        port.text="143"
        ssl.checked=false
        unreads.checked=true
        subject.checked=false
        sender.checked=false
        unreadscount.text="1"
        subjectinput.text=""
        senderinput.text=""
        subjectinput.readOnly=true
        senderinput.readOnly=true
    }
    function editData(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9){
        clearData();
        unreads.checked=false
        subject.checked=false
        sender.checked=false
        unreadinput.readOnly=true
        subjectinput.readOnly=true
        senderinput.readOnly=true

        name.text=arg1
        type.currentIndex=arg2
        accounttype.text=type.currentItem.text
        mailserver.text=arg3
        username.text=arg4
        password.text=arg5
        ssl.checked=arg7        

        port.text=arg6        
        if(arg8===0){
            unreads.checked=true
            unreadinput.readOnly=false
            unreadscount.text=parseInt(arg9)
        }else if(arg8===1){
            subject.checked=true
            subjectinput.readOnly=false
            subjectinput.text=arg9
        }else{
            sender.checked=true
            senderinput.readOnly=false
            senderinput.text=arg9
        }
    }

MouseArea{
    anchors.fill: parent
}
    MouseArea{
        z:0
        width: parent.width
        height: 35
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
                    popup.x=popup.x+dx;
                    popup.y=popup.y+dy;
                }
            }
    }
Text{
    color: "white"
    font.pointSize: (!mw.mac())?10:13
    text: "Mail Settings"
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: 40
    anchors.topMargin: 15
}
    PushButton{
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 6
        label.text: ""
        upSrc: "qrc:/images/layout/close-single-up.png"
        downSrc: "qrc:/images/layout/close-single-hover.png"
        onClicked: {
            mw.markEditing(popup.oldname,false)
            popup.oldname=""
            parent.visible=false
        }

    }

    Item{
        id: desc
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: parent.top
        anchors.topMargin: 60
        height: 30
        width: 300
        Text{
            id: descN
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Description:"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            radius: 3
            border.color: "#CDCFD2"
            width: parent.width
            height: parent.height
            TextInput{
                id: description
                color: "black"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                width: 290
                height: 30
                font.pointSize: (!mw.mac())?10:12
                //clip: true
                selectByMouse: true
                maximumLength: 24
                KeyNavigation.tab: mailserver
            }
        }
    }

    Item{
        id: accounttypes
        anchors.left: desc.left
        anchors.top: desc.bottom
        anchors.topMargin: 7
        height: 30
        width: 300
z: popup.z+1

        Text{
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Account type:"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }

        Text{
            id: accounttype
            wrapMode: TextInput.WrapAnywhere
            height: 30
            width: 300
            text: "IMAP"
            color: "black"
            font.pointSize: (!mw.mac())?9:12
            anchors.left: parent.left
            anchors.leftMargin: 82
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 8
            MouseArea {
                cursorShape: Qt.PointingHandCursor
                id: ma2
                anchors.fill: parent
                onClicked: {
                    comboPattern.visible=true
                    comboPattern.z=popup.z+9999
                    comboPattern.x=accounttypes.x+accounttype.x-8
                    comboPattern.y=accounttypes.y
                    for(var i=0;i<comboPattern.items.count;i++){
                        if(comboPattern.items.get(i).name===accounttype.text){
                            comboPattern.curIn=i;
                            break;
                        }
                    }
                }

            }
            Image{
                height: 29
                width: 300
                source: "qrc:images/layout/colorpicker/select-300-bg.png"
                anchors.left: parent.left
                anchors.leftMargin: -7
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
    }
    Item{
        id: mails
        anchors.left: accounttypes.left
        anchors.top: accounttypes.bottom
        anchors.topMargin: 7
        height: 30
        width: 300
        Text{
            id: mailsN
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Mail server:"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            radius: 3
            border.color: "#CDCFD2"
            width: parent.width
            height: parent.height
            TextInput{
                id: mailserver
                color: "black"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                width: 290
                height: 30
                font.pointSize: (!mw.mac())?10:12
                //clip: true
                selectByMouse: true
                maximumLength: 24
                KeyNavigation.tab: username
            }
        }
    }
    Item{
        id: usern
        anchors.left: mails.left
        anchors.top: mails.bottom
        anchors.topMargin: 7
        height: 30
        width: 300
        Text{
            id: usernN
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Username:"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            radius: 3
            border.color: "#CDCFD2"
            width: parent.width
            height: parent.height
            TextInput{
                id: username
                color: "black"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                width: 290
                height: 30
                font.pointSize: (!mw.mac())?10:12
                //clip: true
                selectByMouse: true
                maximumLength: 24
                KeyNavigation.tab: password
            }
        }
    }
    Item{
        id: pass
        anchors.left: usern.left
        anchors.top: usern.bottom
        anchors.topMargin: 7
        height: 30
        width: 300
        Text{
            id: passN
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Password:"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            radius: 3
            border.color: "#CDCFD2"
            width: parent.width
            height: parent.height
            TextInput{
                echoMode: TextInput.Password
                id: password
                color: "black"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                width: 290
                height: 30
                font.pointSize: (!mw.mac())?10:12
                //clip: true
                selectByMouse: true
                maximumLength: 24
                KeyNavigation.tab: port
            }
        }
    }
    Item{
        id: ports
        anchors.left: pass.left
        anchors.top: pass.bottom
        anchors.topMargin: 7
        height: 30
        width: 150
        Text{
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Port:"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 75
            anchors.verticalCenter: parent.verticalCenter
            radius: 3
            border.color: "#CDCFD2"
            width: parent.width
            height: parent.height
            TextInput{
                id: port
                color: "black"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                width: 290
                height: 30
                font.pointSize: (!mw.mac())?10:12
                //clip: true
                selectByMouse: true
                validator: IntValidator{}
                maximumLength: 8
                KeyNavigation.tab: {
                    var tmp=0;
                    if(subject.checked) tmp=1;
                    else if(sender.checked) tmp=2;
                    if(tmp==0){
                        unreadinput
                    }else if(tmp==1){
                        senderinput
                    }else if(tmp==2){
                        subjectinput
                    }
                }
            }
        }
    }
    Item{
        id: usessl
        anchors.left: parent.left
        anchors.leftMargin: 270
        anchors.top: ports.top
        anchors.topMargin: 5
        height: 30
        width: 100
        Image{
            id: ssl
            property bool checked: false
            source: "qrc:/images/layout/checkbox-normal.png"
            onCheckedChanged: {
                if(checked){
                    if(lv.currentIndex==1){
                        port.text="995"
                    }else if(lv.currentIndex==0 || lv.currentIndex==2){
                        port.text="993"
                    }

                    source="qrc:/images/layout/checkbox-selected.png"
                }else{
                    if(lv.currentIndex==1){
                        port.text="110"
                    }else if(lv.currentIndex==0 || lv.currentIndex==2){
                        port.text="143"
                    }

                    source="qrc:/images/layout/checkbox-normal.png"
                }
            }
        }

        Text{
            anchors.left: ssl.right
            anchors.leftMargin: 5
            anchors.top: ssl.top
            anchors.topMargin: 3
            text: "Use SSL"
            color: "black"
            font.pointSize: (!mw.mac())?8:10
        }
        MouseArea{
            cursorShape: Qt.PointingHandCursor
            anchors.fill: parent
            onClicked: {
                ssl.checked=!ssl.checked;
            }
        }
    }
    Text{
        id: blinkwhen
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: ports.bottom
        anchors.topMargin: 15
        text: "Blink when:"
        color: "black"
        font.pointSize: (!mw.mac())?8:10
        width: 75
    }

    Item{
        id: unreadsm
        anchors.left: blinkwhen.right
        anchors.top: blinkwhen.top
        height: 30
        width: 20
        Image{
            id: unreads
            property bool checked: true
            source: checked?"qrc:/images/layout/radio-selected.png":"qrc:/images/layout/radio-normal.png"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                onClicked: {
                    sender.checked=false
                    subject.checked=false
                    parent.checked=true                   
                    port.KeyNavigation.tab=unreadinput
                }
            }
            onCheckedChanged: {
                if(checked){
                    if(lv.currentIndex!=1){
                        unreadinput.readOnly=false
                        unreadinput.parent.source="qrc:images/layout/spinbox-bg.png"
                        unreadinput.forceActiveFocus()
                    }
                    source="qrc:/images/layout/radio-selected.png"
                }else{
                    unreadinput.readOnly=true
                    unreadinput.focus=false
                    unreadinput.parent.source="qrc:images/layout/spinbox-bg-disable.png"
                    source="qrc:/images/layout/radio-normal.png"
                }
            }
        }
        Item{
            anchors.left: unreadsm.right
            anchors.leftMargin: 5
            anchors.verticalCenter: unreadsm.verticalCenter
            height: 30
            width: 114
            Image{
                id: unread
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -5
                //radius: 3
                //border.color: "#CDCFD2"
                source: unreads.checked?"qrc:images/layout/spinbox-bg.png":"qrc:images/layout/spinbox-bg-disable.png"
                width: parent.width
                height: parent.height
                TextInput{
                    id: unreadinput
                    color: "black"
                    text: "1"
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 5
                    width: 54
                    height: 30
                    font.pointSize: (!mw.mac())?10:12
                    //clip: true
                    selectByMouse: true
                    validator: IntValidator{}
                    onTextChanged: {
                        if(text==="" || text==="-") text="1";
                        text=parseInt(text)
                        if(parseInt(text)<1) text="1";
                    }
                    //readOnly: !unreads.checked
                    maximumLength: 8
                    KeyNavigation.tab: description
                }
                Column{
                    spacing: 0
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.top: parent.top
                    Rectangle{
                        width: 10
                        height: 12
                        color: "transparent"
                        Image{
                            id: arrowup
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenterOffset: 2
                            source: "qrc:images/layout/colorpicker/arrow-up.png"
                        }
                        MouseArea{
                            cursorShape: (unreads.checked)?Qt.PointingHandCursor:Qt.ArrowCursor
                            anchors.fill: parent
                            onPressed: arrowup.source="qrc:images/layout/colorpicker/arrow-down.png"
                            onReleased: arrowup.source="qrc:images/layout/colorpicker/arrow-up.png"
                            onClicked: {
                                if(!unreadinput.readOnly) unreadinput.text=parseInt(unreadinput.text)+1;
                            }
                        }
                    }
                    Rectangle{
                        width: 10
                        height: 12
                        color: "transparent"
                        Image{
                            id: arrowdown
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenterOffset: 2
                            source: "qrc:images/layout/colorpicker/arrow-2-up.png"
                        }
                        MouseArea{
                            cursorShape: (unreads.checked)?Qt.PointingHandCursor:Qt.ArrowCursor
                            anchors.fill: parent
                            onPressed: arrowdown.source="qrc:images/layout/colorpicker/arrow-2-down.png"
                            onReleased: arrowdown.source="qrc:images/layout/colorpicker/arrow-2-up.png"
                            onClicked: {
                                if(!unreadinput.readOnly && parseInt(unreadinput.text)>1) unreadinput.text=parseInt(unreadinput.text)-1;
                            }
                        }
                    }
                }
            }
            Text{
                id: txt0
                anchors.left: unread.right
                anchors.leftMargin: 10
                anchors.top: parent.top
                text: "Unread e-mails"
                color: "black"
                font.pointSize: (!mw.mac())?8:10
            }
        }
    }
    Item{
        id: subjects
        anchors.left: blinkwhen.right
        anchors.top: unreadsm.bottom
        anchors.topMargin: 8
        height: 30
        width: 20
        Image{
            id: subject
            property bool checked: false
            source: checked?"qrc:/images/layout/radio-selected.png":"qrc:/images/layout/radio-normal.png"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                onClicked: {
                    sender.checked=false
                    unreads.checked=false
                    parent.checked=true
                    port.KeyNavigation.tab=subjectinput
                }
            }
            onCheckedChanged: {
                if(checked){
                    subjectinput.readOnly=false
                    subjectinput.forceActiveFocus()
                    subjectinput.parent.color="white"
                    source="qrc:/images/layout/radio-selected.png"
                }else{
                    subjectinput.readOnly=true
                    subjectinput.focus=false
                    subjectinput.parent.color="#E6E6E6"
                    source="qrc:/images/layout/radio-normal.png"
                }
            }
        }
        Item{
            anchors.left: subjects.right
            anchors.leftMargin: 5
            anchors.verticalCenter: subjects.verticalCenter
            height: 30
            width: 150
            Text{
                id: txt
                anchors.left: parent.left
                anchors.top: parent.top
                text: "Subject contains:"
                color: "black"
                font.pointSize: (!mw.mac())?8:10
                width: 120
            }
            Rectangle{
                anchors.left: txt.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -5
                radius: 3
                border.color: "#CDCFD2"
                color: subject.checked?"white":"#E6E6E6"
                width: parent.width
                height: parent.height
                TextInput{
                    id: subjectinput
                    color: "black"
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 5
                    width: 140
                    height: 30
                    font.pointSize: (!mw.mac())?10:12
                    selectByMouse: true
                    //readOnly: !subject.checked
                    maximumLength: 12
                    KeyNavigation.tab: description
                }
            }
        }
    }
    Item{
        id: senders
        anchors.left: blinkwhen.right
        anchors.top: subjects.bottom
        anchors.topMargin: 8
        height: 30
        width: 20
        Image{
            id: sender
            property bool checked: false
            source: checked?"qrc:/images/layout/radio-selected.png":"qrc:/images/layout/radio-normal.png"
            MouseArea{
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                onClicked: {
                    unreads.checked=false
                    subject.checked=false
                    parent.checked=true
                    port.KeyNavigation.tab=senderinput
                }
            }
            onCheckedChanged: {
                if(checked){
                    senderinput.readOnly=false
                    senderinput.parent.color="white"
                    senderinput.forceActiveFocus()
                    source="qrc:/images/layout/radio-selected.png"
                }else{
                    senderinput.readOnly=true
                    senderinput.focus=false
                    senderinput.parent.color="#E6E6E6"
                    source="qrc:/images/layout/radio-normal.png"
                }
            }
        }
        Item{
            anchors.left: senders.right
            anchors.leftMargin: 5
            anchors.verticalCenter: senders.verticalCenter
            height: 30
            width: 150
            Text{
                id: txt2
                anchors.left: parent.left
                anchors.top: parent.top
                text: "Sender contains:"
                color: "black"
                font.pointSize: (!mw.mac())?8:10
                width: 120
            }
            Rectangle{
                anchors.left: txt2.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -5
                radius: 3
                border.color: "#CDCFD2"
                color: sender.checked?"white":"#E6E6E6"
                width: parent.width
                height: parent.height
                TextInput{
                    id: senderinput
                    color: "black"
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 5
                    width: 140
                    height: 30
                    font.pointSize: (!mw.mac())?10:12
                    //clip: true
                    selectByMouse: true
                    //readOnly: !sender.checked
                    maximumLength: 12
                    KeyNavigation.tab: description
                }
            }
        }
    }
    PushButton{
        id: okButton
        upSrc: "qrc:/images/layout/button-ok-up.png"
        downSrc: "qrc:/images/layout/button-ok-down.png"
        onClicked: {
            mw.markEditing(popup.oldname,false)
            console.log("OK CLICKED")
            popup.visible=false
            if(popup.oldname===""){
                var tmp=0;
                if(subject.checked) tmp=1;
                else if(sender.checked) tmp=2;
                var tmp2=unreadscount.text;
                if(tmp===1) tmp2=subjectinput.text;
                else if(tmp===2) tmp2=senderinput.text
                console.log("PARSER : "+tmp2)
                mw.add_new_mail(name.text,type.currentIndex,mailserver.text,username.text,password.text,parseInt(port.text),ssl.checked,tmp,tmp2)
            }else{
                console.log("EDIT")
                var tmpp=0;
                if(subject.checked) tmpp=1;
                else if(sender.checked) tmpp=2;
                var tmpp2=unreadscount.text;
                if(tmpp===1) tmpp2=subjectinput.text;
                else if(tmpp===2) tmpp2=senderinput.text
                console.log("PARSER : "+tmpp2)
                mw.edit_mail(popup.oldname,name.text,type.currentIndex,mailserver.text,username.text,password.text,parseInt(port.text),ssl.checked,tmpp,tmpp2)
            }
            mw.emailsUpdate();

            popup.oldname="";
        }
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        label.text: "OK"
        label.color: "black"
        label.font.pointSize: (!mw.mac())?9:12
    }
    PushButton{
        id: cancelButton
        upSrc: "qrc:/images/layout/button-cancel-up.png"
        downSrc: "qrc:/images/layout/button-cancel-down.png"
        onClicked: {
            console.log("CANCEL CLICKED")
            popup.visible=false
            mw.markEditing(popup.oldname,false)
            if(popup.oldname!="")
                mw.checkMail(popup.oldname)
            popup.oldname="";            
        }
        anchors.right: okButton.left
        anchors.rightMargin: 5
        anchors.bottom: okButton.bottom
        label.text: "Cancel"
        label.color: "black"
        label.font.pointSize: (!mw.mac())?9:12
    }

    ComboBox1{
        id: comboPattern
        width: 300
        items: types
        ListModel{
            id: types
            ListElement{
                name: "IMAP"
            }
            ListElement{
                name: "POP3"
            }
            ListElement{
                name: "GMAIL"
            }
        }
        height: types.count*20
        z: 99999999
        visible: false

        onVisibleChanged: {
            if(visible){
                openPopup()
                background.visible=true
            }
        }

        onClick2: {
            accounttype.text=types.get(idx).name
            if(idx==1){
                unreadinput.readOnly=true
                unreadinput.text="1"
                unreadinput.parent.source="qrc:images/layout/spinbox-bg-disable.png"
                if(ssl.checked){
                    port.text="995"
                }else{
                    port.text="110"
                }
            }else if(idx==0){
                if(unreads.checked){
                    unreadinput.readOnly=false
                    unreadinput.parent.source="qrc:images/layout/spinbox-bg.png"
                }
                if(ssl.checked){
                    port.text="993"
                }else{
                    port.text="143"
                }
            }else if(idx==2){
                if(unreads.checked){
                    unreadinput.readOnly=false
                    unreadinput.parent.source="qrc:images/layout/spinbox-bg.png"
                }
                ssl.checked=true
                port.text="993";
                mailserver.text="imap.gmail.com"
            }
        }
        onHiden: {
            closePopup()
            background.visible=false
        }
    }
    Rectangle{
        id: background
        color: "lightgrey"
        anchors.fill: parent
        z: popup.z+999
        visible: false
        opacity: 0.1
        MouseArea{
            anchors.fill: parent
            onClicked: {
                combo.hide()
            }
        }
    }
}
