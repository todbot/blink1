--
-- simple example of telling Blink1Control to change blink(1) via Applescript
-- run it on the commandline with "osascript ./blink1control-test0.applescript"
--
tell application "Blink1Control"
	-- do command with args "the parameter" ivalue 7 prose "testing testing" rvalue 12 with blinking
    do command with args "fadeToRGB" color "#ffcc00" time 0.3
end tell