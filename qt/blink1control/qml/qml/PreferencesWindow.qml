import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.0

//import "ToolTipTod2Creator.js" as ToolTipCreator


Window {
    id: prefsWindow

    width:  mw.mac() ? 590 : 560 // Win version experimentally determined
    height: mw.mac() ? 520 : 450

    // FIXME: surely there's a shorter way of saying "no resize"?
    //maximumWidth: width
    //maximumHeight: height
    //minimumWidth: width
    //minimumHeight: height

    //font.pixelSize:12  // this doesn't work
    
    color: "#f9f9f9"

    title: "Blink1Control Advanced Preferences"
    flags: Qt.Dialog

    Item {
        id: flickable
        anchors.fill: parent
        anchors.margins: 10

        ColumnLayout {
            id: mainLayout
            spacing: 15
        RowLayout {
            id: mainbox
            spacing: 20
        ColumnLayout {
            id: leftcolumn
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 260
            spacing: 15
            
        GroupBox {
            id: groupGeneral
            title: "General"
            Layout.fillWidth: true
            ColumnLayout {
                CheckBox {
                    id: minimizedCheckbox
                    text: "Start minimized"
                    checked: mw.startMinimized
                    //onHoveredChanged: { console.log("hello "+hovered); }//ToolTipCreator.create("text on the tooltip, yo", parentItem).show() }
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
                CheckBox { 
                    id: dockIconCheckbox
                    //visible: mw.mac();  // messes up spacing
                    //opacity: 0.1
                    enabled: mw.mac()
                    text: "Show Dock icon (requires restart)"
                    checked: mw.dockIcon
                }    
            }
        } // general groupbox

        GroupBox {
            id: groupAPIServer
            title: "API server configuration"
            Layout.fillWidth: true
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
                //TextField { id: serverHostText; text: mw.serverHost }
                RowLayout { 
                    ExclusiveGroup { id: serverHostGroup }
                    RadioButton {
                        id: serverHostLocalhost
                        text: "localhost"
                        checked: true
                        exclusiveGroup: serverHostGroup
                    }
                    RadioButton {
                        id: serverHostAny
                        text: "any"
                        exclusiveGroup: serverHostGroup
                    }
                }
                
                Label { text: "serverPort:" }
                TextField { id: serverPortText; text: mw.serverPort }

                Label { Layout.columnSpan:2; text: "(requires restart)" } 
            }
        } // api server groupbox

        GroupBox {
            id: groupProxyConfig
            title: "Proxy configuration"
            Layout.fillWidth: true
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
                    TextField { id: proxyHostText;  } //text: mw.proxyHost }
                    Label { text: "proxyPort:" }
                    TextField { id: proxyPortText;  } //text: mw.proxyPort }
                    Label { text: "proxyUser:" }
                    TextField { id: proxyUserText;  } //text: mw.proxyUser }
                    Label { text: "proxyPass:" }
                    TextField { id: proxyPassText;  } //text: mw.proxyPass }
                }
            }
        } // proxy groupbox

        } // left column
        ColumnLayout {
            id: rightcolumn
            Layout.alignment: Qt.AlignTop
            Layout.minimumWidth: 260
            spacing: 15

        GroupBox {
            id: groupBlink1ToUse
            title: "blink(1) device to use"
            Layout.fillWidth: true
            ColumnLayout {
                ExclusiveGroup { id: blink1touseGroup }
                RadioButton {  exclusiveGroup: blink1touseGroup
                    id: blink1tousefirstavailButton
                    text: "First Available"
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
                        //onActivated: {
                        onCurrentIndexChanged: {
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
            Layout.fillWidth: true
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
        GroupBox {
            id: groupBlink1SettingsSaveLoad
            title: "Import/Export settings"
            Layout.fillWidth: true
            RowLayout {
                //Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Button {
                    text: "Export Settings "
                    onClicked: {
                        fileDialogExport.open()                        
                    }
                }
                Button {
                    text: "Import Settings "
                    onClicked: {
                        fileDialogImport.open()
                    }
                }
            }
        }
        GroupBox {
            id: groupBlink1Logging
            title: "Logging / Debug"
            Layout.fillWidth: true
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                CheckBox {
                    id: loggingCheckbox
                    text: "Enable logging"
                    checked: mw.logging
                }
                RowLayout { 
                Button {
                    text: "open log file"
                    tooltip: mw.getLogFileName()
                    onClicked: {
                        Qt.openUrlExternally("file:///"+mw.getLogFileName())
                        //Qt.openUrlExternally("file://"+mw.getLogFileName())
                    }        
                }
                Button {
                    text: "open settings file"
                    tooltip: mw.getSettingsFileName()
                    onClicked: {
                        Qt.openUrlExternally("file:///"+mw.getSettingsFileName())
                    }        
                }
                }
            }
        }
        
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
                    mw.startMinimized = minimizedCheckbox.checked;
                    mw.autorun = loginCheckbox.checked;
                    mw.enableGamma = gammaCheckbox.checked;
                    mw.dockIcon = dockIconCheckbox.checked;
                    mw.logging = loggingCheckbox.checked;
                    
                    mw.serverHost = (serverHostGroup.current == serverHostAny) ? "any" : "localhost";
                    mw.serverPort = serverPortText.text;

                    // this seems so messed up
                    if( proxyType0.checked )        mw.proxyType = "none";
                    else if( proxyType1.checked )   mw.proxyType = "socks5";
                    else if( proxyType2.checked )   mw.proxyType = "http";

                    mw.proxyHost = proxyHostText.text;
                    mw.proxyPort = proxyPortText.text;
                    mw.proxyUser = proxyUserText.text;
                    mw.proxyPass = proxyPassText.text;

                    if( blink1touseGroup.current == blink1tousedeviceButton ) {
                      mw.setBlink1Index( blink1touseComboBox.currentText );
                    } else {
                      mw.setBlink1Index( 0 );
                    }

                    console.log("qml proxyUser: "+proxyUserText.text+", proxyPass: "+proxyPassText.text);
                    mw.updatePreferences();

                    prefsWindow.visible = false
                }
            } // button
        } // rowlayout buttons

        } // main columnlayout

    }// item

    onVisibilityChanged: {
        console.log("Preferences visbility changed");
        if( !visible ) return;  // only do below when visible

        // load up values because bindings break? FIXME: don't understand this fully
        enableServerCheckbox.checked = mw.enableServer
        minimizedCheckbox.checked = mw.startMinimized
        loginCheckbox.checked = mw.autorun
        dockIconCheckbox.checked = mw.dockIcon

        if( mw.serverHost=="any" ) { 
            serverHostAny.checked = true;
        } else { 
            serverHostLocalhost.checked = true;
        }
        // FIXME: make proxyType an enumeration
        if( mw.proxyType == "" || mw.proxyType == "none" ) {
            console.log("qml proxy none");
            proxyType0.checked = true
        }
        else if( mw.proxyType == "socks5" ) {
            console.log("qml proxy socks5");
            proxyType1.checked = true
        }
        else if( mw.proxyType == "http" ) {
            console.log("qml proxy http");
            proxyType2.checked = true
        }
        proxyHostText.text = mw.proxyHost;
        proxyPortText.text = mw.proxyPort;
        proxyUserText.text = mw.proxyUser;
        proxyPassText.text = mw.proxyPass;
  
        if( mw.blink1Index == 0 ) { 
            blink1tousefirstavailButton.checked = true
        }
        else { 
            blink1tousedeviceButton.checked=true;
            var idstr = mw.blink1Index.toString(16).toUpperCase() // FIXME
            var i = blink1touseComboBox.find( idstr );  
            blink1touseComboBox.currentIndex = i;
            console.log("idx:+"+idstr +" i="+i)
        }
    } // onVisibilityChanged

    FileDialog {
        id: fileDialogExport
        title: "Select location of exported file"
        selectExisting: false
        nameFilters: ["Preferences INI files (*.ini)", "All files (*)"]
        onAccepted: {
            console.log("You chose: " + fileDialogExport.fileUrl)
            mw.settingsExport( fileDialogExport.fileUrl )
        }
        onRejected: {
            console.log("export canceled")
        }
    }

    FileDialog {
        id: fileDialogImport
        title: "Choose a settings file to import"
        selectExisting: true
        nameFilters: ["Preferences INI files (*.ini)", "All files (*)"]
        onAccepted: {
            console.log("You chose: " + fileDialogImport.fileUrl)
            mw.settingsImport( fileDialogImport.fileUrl )
        }
        onRejected: {
            console.log("import canceled")
        }
    }
 
    
} // window

