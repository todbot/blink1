' blink1DiskActivity.vbs -- Flash blink(1) to disk activity
'                           Flashes green on reads, red on writes
'
' Run this script on the command line With:
'   cscript.exe //nologo blink1DiskActivity.vbs
'
'
' And make sure "blink1-tool.exe" is in your PATH or in same dir as this script
'
' 2012, Tod E. Kurt, http://todbot.com/blog/ http://thingm.com/
'

' helper function to run a program 
Sub Run(ByVal sFile)
    Dim WShell : Set WShell = CreateObject("WScript.Shell")
    WShell.Run sFile, 0, true 
End Sub

wscript.echo "hello there"

Run "blink1-tool.exe --rgb 100,100,100 --blink 3"

' prepare the to fetch data from WMI
strComputer = "."
Set objWMIService = GetObject("winmgmts:" _
    & "{impersonationLevel=impersonate}!\\" & strComputer & "\root\cimv2")
set objRefresher = CreateObject("WbemScripting.SWbemRefresher")
Set colDisks = objRefresher.AddEnum _
    (objWMIService, "win32_perfformatteddata_perfdisk_logicaldisk"). _
        objectSet
objRefresher.Refresh

' loop forever
Do While 1
    For Each objDisk in colDisks  ' there is a better way of doing this surely
        If objDisk.Name = "_Total" Then

            red = Int( objDisk.PercentDiskWriteTime * 255 / 100 )
            grn = Int( objDisk.PercentDiskReadTime  * 255 / 100 )

            'wscript.echo "red/grn: " & red & "/" & grn

            Run "blink1-tool.exe -m 50 --rgb " & red & "," & grn & ",0"
            
            Wscript.Sleep 1000
            objRefresher.Refresh
        End If
    Next
Loop