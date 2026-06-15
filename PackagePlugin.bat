@REM Copyright (c) 2026 Timothy YU. All rights reserved.
@echo off
setlocal EnableExtensions

REM ============================================================================
REM Package LiveLinkVISCA with Unreal AutomationTool for GitHub/Fab release.
REM Produces Releases\LiveLinkVISCA.zip with a single LiveLinkVISCA folder inside.
REM ============================================================================
cd /d "%~dp0"

set "TARGET_PLATFORMS=%~1"
if "%TARGET_PLATFORMS%"=="" set "TARGET_PLATFORMS=Win64"

REM --- Unreal Engine install (folder that CONTAINS "Engine", e.g. UE_5.7) ---
if not defined UE_ROOT (
	if exist "E:\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat" set "UE_ROOT=E:\UE_5.7"
)
if not defined UE_ROOT (
	if exist "D:\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat" set "UE_ROOT=D:\UE_5.7"
)
if not defined UE_ROOT (
	if exist "%ProgramFiles%\Epic Games\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat" (
		set "UE_ROOT=%ProgramFiles%\Epic Games\UE_5.7"
	)
)
if not defined UE_ROOT (
	echo ERROR: Unreal Engine not found. Set UE_ROOT to your engine root, for example:
	echo   set UE_ROOT=E:\UE_5.7
	echo   set UE_ROOT=D:\UE_5.7
	echo   set UE_ROOT=C:\Program Files\Epic Games\UE_5.7
	exit /b 1
)

set "RUNUAT_BAT=%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat"
if not exist "%RUNUAT_BAT%" (
	echo ERROR: RunUAT.bat not found:
	echo   "%RUNUAT_BAT%"
	exit /b 1
)

set "PLUGIN_FILE=%~dp0LiveLinkVISCA.uplugin"
set "RELEASE_ROOT=%~dp0Releases"
set "PACKAGE_DIR=%RELEASE_ROOT%\LiveLinkVISCA"
set "ZIP_FILE=%RELEASE_ROOT%\LiveLinkVISCA.zip"

if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"
if not exist "%RELEASE_ROOT%" mkdir "%RELEASE_ROOT%"

echo Engine:   "%UE_ROOT%"
echo Plugin:   "%PLUGIN_FILE%"
echo Package:  "%PACKAGE_DIR%"
echo Platforms: %TARGET_PLATFORMS%
echo.

call "%RUNUAT_BAT%" BuildPlugin -Plugin="%PLUGIN_FILE%" -Package="%PACKAGE_DIR%" -TargetPlatforms=%TARGET_PLATFORMS% -Rocket
set "ERR=%ERRORLEVEL%"
if not "%ERR%"=="0" (
	echo.
	echo Package failed with code %ERR%.
	exit /b %ERR%
)

if exist "%PACKAGE_DIR%\Intermediate" rmdir /s /q "%PACKAGE_DIR%\Intermediate"
if exist "%ZIP_FILE%" del /q "%ZIP_FILE%"

powershell -NoProfile -ExecutionPolicy Bypass -Command "$Exts = @('.exe', '.bat', '.cmd', '.ps1', '.psm1', '.sh', '.bash', '.py'); Get-ChildItem -LiteralPath '%PACKAGE_DIR%' -Recurse -File | Where-Object { $Exts -contains $_.Extension.ToLowerInvariant() } | Remove-Item -Force"
if errorlevel 1 (
	echo.
	echo Executable/script cleanup failed.
	exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%PACKAGE_DIR%' -DestinationPath '%ZIP_FILE%' -Force"
if errorlevel 1 (
	echo.
	echo Zip creation failed.
	exit /b 1
)

echo.
echo Package finished successfully:
echo   "%ZIP_FILE%"
exit /b 0
