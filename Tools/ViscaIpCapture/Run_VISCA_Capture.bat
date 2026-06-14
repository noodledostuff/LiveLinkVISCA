@echo off
setlocal
cd /d "%~dp0"

where py >nul 2>nul
if %errorlevel%==0 (
    py -3 visca_capture.py
    goto :eof
)

where python >nul 2>nul
if %errorlevel%==0 (
    python visca_capture.py
    goto :eof
)

echo Python 3 was not found. Install Python 3 or run this tool with the Python bundled in your environment.
pause
