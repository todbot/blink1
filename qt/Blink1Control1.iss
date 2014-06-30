; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Blink1Control
AppVersion=1.8
DefaultDirName={pf}\Blink1Control
DefaultGroupName=Blink1Control
UninstallDisplayIcon={app}\Blink1Control.exe
Compression=lzma2
SolidCompression=yes
;OutputDir=userdocs:Inno Setup Examples Output
OutputDir=..
SourceDir=Y:\blink1\qt\windeploy\Blink1Control
OutputBaseFilename=Blink1Control-win-setup

[Files]
Source: "*"; DestDir: "{app}"; Flags: recursesubdirs
;Source: "MyProg.chm"; DestDir: "{app}"
;Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\Blink1Control"; Filename: "{app}\Blink1Control.exe"

[Run]
;Filename: "{app}\README.TXT"; Description: "View the README file"; Flags: postinstall shellexec skipifsilent
Filename: "{app}\Blink1Control.exe"; Description: "Launch Blink1Control"; Flags: postinstall nowait skipifsilent
