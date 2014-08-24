import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.1


Window {
    id: prefsWindow

    width: 570
    height: 420
    color: "#f9f9f9"

    title: "Blink1Control Preferences"
    flags: Qt.Dialog
    
    onClosing: { 
        console.log("** Closing Prefs Window");
    }

    Item {
        id: flickable
        anchors.fill: parent
        anchors.margins: 10
        
        ColumnLayout {
            id: mainLayout
            spacing: 10
        RowLayout { 
        ColumnLayout { 
            id: leftcolumn
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 280

        GroupBox {
            id: groupGeneral
            title: "General"
            //Layout.fillWidth: true
            //Layout.alignment: Qt.AlignTop
            ColumnLayout {
                //Layout.fillWidth: true
                CheckBox {
                    id: minimizedCheckbox
                    text: "Start Minimized"
                    checked: mw.startmin
                    //Layout.minimumWidth: 100
                }
                CheckBox {
                    id: loginCheckbox
                    text: "Start at login"
                    checked: mw.autorun
                    //Layout.minimumWidth: 100
                }
            }
        } // general groupbox

        GroupBox {
            id: groupAPIServer
            title: "API Server Configuration"
            //Layout.fillWidth: true
            //Layout.alignment: Qt.AlignTop
            GridLayout {
                id: proxyApiServerLayout
                anchors.fill: parent
                columns: 2
                CheckBox {
                    id: enableServerCheckbox
                    text: "Start API Server"
                    checked: mw.enableServer
                    Layout.columnSpan: 2
                    //Layout.minimumWidth: 100
                }

                Label { text: "serverHost:" }
                TextField { id: serverHostText; text: mw.serverHost }
                Label { text: "serverPort:" }
                TextField { id: serverPortText; text: mw.serverPort }
            }
        } // api server groupbox

        GroupBox { 
            id: groupProxy
            title: "Proxy Configuration"
            //Layout.fillWidth: true
            
            GridLayout {
                id: proxygridLayout
                anchors.fill: parent
                columns: 2
                //flow: GridLayout.TopToBottom
                RowLayout { 
                    Layout.columnSpan: 2
                    ExclusiveGroup { id: proxyTypeGroup }
                    RadioButton {
                        id: proxyType0
                        text: "none"
                        checked: true
                        exclusiveGroup: proxyTypeGroup
                        //Layout.minimumWidth: 100
                    }
                    RadioButton {
                        id: proxyType1
                        text: "SOCKS5"
                        exclusiveGroup: proxyTypeGroup
                    }
                    RadioButton { 
                        id: proxyType2
                        text: "HTTP"
                        exclusiveGroup: proxyTypeGroup
                    }
                } // rowlayout

                Label { text: "proxyHost:" }
                TextField { id: proxyHostText;  text: mw.proxyHost }
                Label { text: "proxyPort:" }
                TextField { id: proxyPortText;  text: mw.proxyPort }
                Label { text: "proxyUser:" }
                TextField { id: proxyUserText;  text: mw.proxyUser }
                Label { text: "proxyPass:" }
                TextField { id: proxyPassText;  text: mw.proxyPass }
            }
        } // proxy groupbox

        } // left column
        ColumnLayout { 
            id: rightcolumn
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 280
            //Layout.fillWidth: true

        GroupBox { 
            id: groupBlink1ToUse
            title: "blink(1) device to use"
            ColumnLayout { 
                ExclusiveGroup { id: blink1touseGroup }
                RadioButton {  exclusiveGroup: blink1touseGroup
                    text: "First Avaialble"
                    checked: true
                }
                RowLayout { 
                    RadioButton {  exclusiveGroup: blink1touseGroup
                        id: blink1tousedeviceButton
                        text: "Use device:"
                    }
                    ComboBox {
                        id: blink1touseComboBox
                        model: mw.getBlink1Serials
                        onCurrentIndexChanged: { 
                        //onActivated: { 
                            console.log("blink1 serial: " + currentIndex + " , "+ currentText);
                            mw.blink1Blink( currentText, "#333333", 500);
                        }
                    }
                }
            }
        } // groupBlink1ToUse

        GroupBox { 
            id: groupBlink1Startup
            title: "blink(1) device no-computer behavior"
            Layout.alignment: Qt.AlignTop
            ColumnLayout { 
                ExclusiveGroup { id: blink1startup }
                RadioButton {  exclusiveGroup: blink1startup
                    text: "No Change"
                    checked: true
                }
                RadioButton {  exclusiveGroup: blink1startup
                    text: "Off"
                }
                RadioButton { exclusiveGroup: blink1startup
                    text: "Default"
                }
                RowLayout { 
                    RadioButton { exclusiveGroup: blink1startup
                        text: "Pattern:"
                    }
                    ComboBox {
                        model: mw.getPatternsNames
                    }
                }
                Button { 
                    Layout.alignment: Qt.AlignRight;
                    text: "Set"
                }

            } // radiobutton column ayout
        } // blink1 startup groupbox

        } // right column
        } // main rowlayout
 
        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Button { 
                text: "Cancel"
                onClicked: prefsWindow.visible = false;
            }
            Button { 
                text: "Ok"
                onClicked: { 
                    console.log("saving preferences...");
                    mw.enableServer = enableServerCheckbox.checked;
                    mw.startmin = minimizedCheckbox.checked;
                    mw.autorun = loginCheckbox.checked;

                    mw.serverHost = serverHostText.text;
                    mw.serverPort = serverPortText.text;

                    mw.proxyHost = proxyHostText.text;
                    mw.proxyPort = proxyPortText.text;
                    mw.proxyUser = proxyUserText.text;
                    mw.proxyPass = proxyPassText.text;
                    
                    if( blink1touseGroup.current == blink1tousedeviceButton ) { 
                      console.log(" changing blink1 to use: "+ blink1touseComboBox.currentText );
                      mw.setBlink1Index( blink1touseComboBox.currentText );
                    }

                    prefsWindow.visible = false
                }
            }
        } // rowlayout buttons

        } // main columnlayout
        
    }// item

    onVisibilityChanged: { 
        console.log("** Visbility changes");
        if( visible ) { 
            console.log("help i'm visible!") 
            // load up values because bindings break?
            enableServerCheckbox.checked = mw.enableServer;
            minimizedCheckbox.checked = mw.startmin;
            loginCheckbox.checked = mw.autorun;
            
            if( mw.proxyType == "" || mw.proxyType == "none" ) { 
              proxyType0.checked = true
            }
            else if( mw.proxyType == "socks5" ) {
              proxyType1.checked = true
            }
            else if( mw.proxyType == "http" ) { 
              proxyType2.checked = true
            }
            
        }
    }

} // window

