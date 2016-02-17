; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppPublisher=ThingM Corporation
AppPublisherURL=http://blink1.thingm.com/
AppName=Blink1Control
AppVersion=1.98
DefaultDirName={pf}\Blink1Control
DefaultGroupName=Blink1Control
UninstallDisplayIcon={app}\Blink1Control.exe
Compression=lzma2
SolidCompression=yes
;OutputDir=userdocs:Inno Setup Examples Output
OutputDir=..\..
;SourceDir=Y:\blink1\qt\windeploy\Blink1Control
SourceDir=windeploy\Blink1Control
OutputBaseFilename=Blink1Control-win-install

[Files]
Source: "*"; DestDir: "{app}"; Flags: recursesubdirs
;Source: "MyProg.chm"; DestDir: "{app}"
;Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\Blink1Control"; Filename: "{app}\Blink1Control.exe"

[Run]
;Filename: "{app}\README.TXT"; Description: "View the README file"; Flags: postinstall shellexec skipifsilent
Filename: "{app}\Blink1Control.exe"; Description: "Launch Blink1Control"; Flags: postinstall nowait skipifsilent

;[UninstallRun] 
;Filename: {cmd}; parameters: /c taskkill /f /im notepad.exe;Flags: runhidden
;Filename: "taskkill.exe"; Parameters: "/IM:Blink1Control.exe /F"; Flags: runhidden

