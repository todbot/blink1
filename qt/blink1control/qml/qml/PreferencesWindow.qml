import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.1


Window {
    id: prefsWindow

    width: mw.mac() ? 580 : 550 // Win version experimentally determined
    height: mw.mac() ? 440 : 390
    // FIXME: surely there's a shorter way of saying "no resize"?
    maximumWidth: width
    maximumHeight: height
    minimumWidth: width
    minimumHeight: height

    color: "#f9f9f9"

    title: "Blink1Control Preferences"
    flags: Qt.Dialog

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
            ColumnLayout {
                CheckBox {
                    id: minimizedCheckbox
                    text: "Start minimized"
                    checked: mw.startmin
                }
                CheckBox {
                    id: loginCheckbox
                    text: "Start at login"
                    checked: mw.autorun
                }
                CheckBox {
                    id: gammaCheckbox
                    text: "Enable gamma correction"
                    checked: mw.enableGamma
                }
            }
        } // general groupbox

        GroupBox {
            id: groupAPIServer
            title: "API server configuration"
            GridLayout {
                anchors.fill: parent
                columns: 2
                CheckBox {
                    id: enableServerCheckbox
                    text: "Start API Server"
                    checked: mw.enableServer
                    Layout.columnSpan: 2
                }

                Label { text: "serverHost:" }
                TextField { id: serverHostText; text: mw.serverHost }
                Label { text: "serverPort:" }
                TextField { id: serverPortText; text: mw.serverPort }
            }
        } // api server groupbox

        GroupBox {
            id: groupProxyConfig
            title: "Proxy configuration"
            ColumnLayout { 
                spacing: 10
                RowLayout {
                    Layout.columnSpan: 2
                    ExclusiveGroup { id: proxyTypeGroup }
                    RadioButton {
                        id: proxyType0
                        text: "none"
                        checked: true
                        exclusiveGroup: proxyTypeGroup
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

                GridLayout {
                    id: proxygridLayout
                    columns: 2
                    enabled: proxyTypeGroup.current != proxyType0
                    Label { text: "proxyHost:" }
                    TextField { id: proxyHostText;  text: mw.proxyHost }
                    Label { text: "proxyPort:" }
                    TextField { id: proxyPortText;  text: mw.proxyPort }
                    Label { text: "proxyUser:" }
                    TextField { id: proxyUserText;  text: mw.proxyUser }
                    Label { text: "proxyPass:" }
                    TextField { id: proxyPassText;  text: mw.proxyPass }
                }
            }
        } // proxy groupbox

        } // left column
        ColumnLayout {
            id: rightcolumn
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 280

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
                        text: "Use device id:"
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
                ExclusiveGroup { id: blink1startupGroup }
                RadioButton {  exclusiveGroup: blink1startupGroup
                    id: blink1startupNone
                    text: "No Change"
                    checked: true
                }
                RadioButton {  exclusiveGroup: blink1startupGroup
                    id: blink1startupOff
                    text: "Off"
                }
                RadioButton { exclusiveGroup: blink1startupGroup
                    id: blink1startupDefault
                    text: "Default"
                }
                RowLayout {
                    RadioButton { exclusiveGroup: blink1startupGroup
                        id: blink1startupPattern
                        text: "Pattern:"
                    }
                    ComboBox {
                        id: blink1startupPatternComboBox
                        model: mw.getPatternsNames
                    }
                }
                RowLayout {
                    spacing: 10
                    Button {
                        Layout.alignment: Qt.AlignRight;
                        text: "Set"
                        enabled: blink1startupGroup.current != blink1startupNone
                        onClicked: {
                            if( blink1startupGroup.current == blink1startupOff ) {
                              blink1startupStatus.text = "Set to Off";
                              mw.setStartupPattern("_OFF");
                            } else if( blink1startupGroup.current == blink1startupDefault ) {
                              blink1startupStatus.text = "Set to Default";
                              mw.setStartupPattern("_DEFAULT");
                            } else {
                              blink1startupStatus.text = "Not implemented";
                            }
                        }
                    }
                    Label { id: blink1startupStatus }
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
                    mw.enableGamma = gammaCheckbox.checked;

                    mw.serverHost = serverHostText.text;
                    mw.serverPort = serverPortText.text;

                    mw.proxyHost = proxyHostText.text;
                    mw.proxyPort = proxyPortText.text;
                    mw.proxyUser = proxyUserText.text;
                    mw.proxyPass = proxyPassText.text;

                    if( blink1touseGroup.current == blink1tousedeviceButton ) {
                      mw.setBlink1Index( blink1touseComboBox.currentText );
                    } else {
                      mw.setBlink1Index( 0 );
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
            // load up values because bindings break? FIXME: don't understand this fully
            enableServerCheckbox.checked = mw.enableServer;
            minimizedCheckbox.checked = mw.startmin;
            loginCheckbox.checked = mw.autorun;

            // FIXME: make proxyType an enumeration
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
