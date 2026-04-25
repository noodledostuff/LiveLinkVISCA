@echo off
setlocal EnableExtensions

REM ============================================================================
REM Build LiveLinkVISCA by compiling the host Unreal Editor target.
REM The plugin is built as part of VISCALivelinkDevEditor (Win64).
REM ============================================================================
cd /d "%~dp0"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Development"

REM --- Unreal Engine install (folder that CONTAINS "Engine", e.g. UE_5.7) ---
if not defined UE_ROOT (
	if exist "E:\UE_5.7\Engine\Build\BatchFiles\Build.bat" set "UE_ROOT=E:\UE_5.7"
)
if not defined UE_ROOT (
	if exist "%ProgramFiles%\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" (
		set "UE_ROOT=%ProgramFiles%\Epic Games\UE_5.7"
	)
)
if not defined UE_ROOT (
	echo ERROR: Unreal Engine not found. Set UE_ROOT to your engine root, for example:
	echo   set UE_ROOT=E:\UE_5.7
	echo   set UE_ROOT=C:\Program Files\Epic Games\UE_5.7
	exit /b 1
)

set "BUILD_BAT=%UE_ROOT%\Engine\Build\BatchFiles\Build.bat"
if not exist "%BUILD_BAT%" (
	echo ERROR: Build.bat not found:
	echo   "%BUILD_BAT%"
	exit /b 1
)

REM --- Host project: Plugins\LiveLinkVISCA -> parent of Plugins contains .uproject ---
REM Optional: set VISCA_UPROJECT to a full path if the plugin is not under Project\Plugins\...
set "UPROJECT="
if defined VISCA_UPROJECT (
	if exist "%VISCA_UPROJECT%" (
		set "UPROJECT=%VISCA_UPROJECT%"
		goto :have_uproject
	)
	echo ERROR: VISCA_UPROJECT is set but file not found:
	echo   "%VISCA_UPROJECT%"
	exit /b 1
)
for %%F in ("%~dp0..\..\*.uproject") do (
	set "UPROJECT=%%~fF"
	goto :have_uproject
)
:have_uproject
if not defined UPROJECT (
	echo ERROR: No .uproject found in:
	echo   "%~dp0..\.."
	echo Place this plugin under YourProject\Plugins\LiveLinkVISCA
	echo Or set VISCA_UPROJECT to the full path of your host .uproject file.
	exit /b 1
)

echo Engine:  "%UE_ROOT%"
echo Project: "%UPROJECT%"
echo Target:  VISCALivelinkDevEditor Win64 %CONFIG%
echo.

call "%BUILD_BAT%" VISCALivelinkDevEditor Win64 %CONFIG% -Project="%UPROJECT%" -WaitMutex
set "ERR=%ERRORLEVEL%"
if not "%ERR%"=="0" (
	echo.
	echo Build failed with code %ERR%.
	exit /b %ERR%
)

echo.
echo Build finished successfully.
exit /b 0
