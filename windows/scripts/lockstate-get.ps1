#
# lockstate-get.ps1
#
# Windows Powershell script to fetch workstation locked / unlock state
# Returns a string "locked" or "unlocked"
#
# Used with "lockstate-test.bat" with Blink1Control2
#
# https://github.com/todbot/blink1/windows
#
# 2018, Tod E. Kurt / todbot.com
#

function islocked () {

  $currentuser = gwmi -Class win32_computersystem | select -ExpandProperty username
  $process = get-process logonui -ea silentlycontinue

  if($currentuser -and $process) {
    return "locked"
  }

  return "unlocked"
}

$lockstate = islocked

Write-Output "$lockstate"
