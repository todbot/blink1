::
:: lockstate-test.bat
::
:: Windows CMD batch script to return hex color code
:: based on workstation locked or unlocked status.
:: Requires "lockstate-get.ps1" Powershell script.
::
:: Set 'colorLocked' and 'colorUnlocked' to be the colors you want
::
:: To use in Blink1Control2:
:: 1. Copy this file and "get-locked.ps1" to a directory
:: 2. In the app, choose "Add event source" and "Add Script"
:: 3. Select this file for the "Script Path"
:: 4. Choose "Parse output as color" and "Trigger on new values only"
:: 5. Click "Ok" and blink(1) should immediately turn 'colorUnlocked' color
:: 6. Lock workstation with ctrl-alt-delete and it will turn 'colorLocked'
::
:: https://github.com/todbot/blink1
:: 2018, Tod E. Kurt / todbot
::
@echo off

set colorLocked="#FF0000"
set colorUnlocked="#00FF00"

:: "%-dp0" is directory of script
for /f %%i in ('powershell -File %~dp0\lockstate-get.ps1') do set lockstate=%%i

::echo %lockstate%

if "%lockstate%" EQU "locked" (
  echo %colorLocked%
) else (
  echo %colorUnlocked%
)

