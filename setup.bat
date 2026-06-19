@echo off
setlocal enabledelayedexpansion

:: Get the directory where this script is located
set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

:: AR Integration - Quest 3 / UE 5.7.4 Migration Setup
:: This script initializes the Python environment and runs the dependency setup.

echo ========================================================
echo   AR Integration Project Setup - Quest 3 / UE 5.7.4
echo ========================================================
echo.

:: 1. Check for Python Launcher
where py >nul 2>nul
if errorlevel 1 (
    echo [INFO] Python Launcher py.exe not found.
    echo [INFO] Attempting to install Python via winget...
    
    where winget >nul 2>nul
    if not errorlevel 1 (
        echo [INFO] Installing Python 3...
        winget install --id Python.Python.3.12 --exact --silent --accept-source-agreements --accept-package-agreements
        if !ERRORLEVEL! equ 0 (
            echo.
            echo [SUCCESS] Python has been installed. 
            echo [IMPORTANT] Please CLOSE this window and run setup.bat again to refresh your PATH.
            pause
            exit /b 0
        ) else (
            echo [ERROR] winget installation failed with code !ERRORLEVEL!.
        )
    ) else (
        echo [ERROR] winget not found.
    )

    echo.
    echo [MANUAL FIX] Please install Python manually from https://www.python.org/
    echo Ensure "Install launcher for all users" and "Add Python to PATH" are checked.
    pause
    exit /b 1
)

:: 2. Create Virtual Environment if it doesn't exist
if not exist "venv" (
    echo [INFO] Creating virtual environment...
    py -m venv venv
    if errorlevel 1 (
        echo [ERROR] Failed to create virtual environment.
        pause
        exit /b 1
    )
    echo [SUCCESS] Virtual environment created.
) else (
    echo [INFO] Virtual environment already exists.
)

:: 3. Update pip and install requirements
echo [INFO] Installing requirements...
venv\Scripts\python.exe -m pip install --upgrade pip --quiet
if exist "requirements.txt" (
    venv\Scripts\python.exe -m pip install -r requirements.txt --quiet
) else (
    echo [WARNING] requirements.txt not found. Installing defaults...
    venv\Scripts\python.exe -m pip install pathlib requests urllib3 --quiet
)

if errorlevel 1 (
    echo [ERROR] Failed to install dependencies.
    pause
    exit /b 1
)
echo [SUCCESS] Dependencies installed.

:: 4. Run the main dependency setup script
echo.
echo [INFO] Running dependency setup script...
venv\Scripts\python.exe setup_dependencies.py

if errorlevel 1 (
    echo.
    echo [ERROR] Setup script failed.
    pause
    exit /b 1
)

echo.
echo ========================================================
echo   SETUP COMPLETE
echo ========================================================
echo.
echo You can now:
echo 1. Right-click ar_integration.uproject
echo 2. Select 'Generate Visual Studio project files'
echo 3. Open the .sln and build for Development Editor x64
echo.
pause
