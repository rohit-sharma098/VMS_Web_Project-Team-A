@echo off
cd /d "%~dp0"
echo =============================================
echo   GATEHOUSE - VISITOR MANAGEMENT SYSTEM
echo =============================================
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: g++ not found. Install MinGW-w64 and add its bin folder to PATH.
  pause
  exit /b 1
)
where node >nul 2>nul
if errorlevel 1 (
  echo ERROR: Node.js not found. Install Node.js LTS from https://nodejs.org/
  pause
  exit /b 1
)
echo Compiling C++ backend...
g++ -std=c++17 -O2 backend.cpp -o backend.exe
if errorlevel 1 (
  echo Build failed. Check the compiler errors above.
  pause
  exit /b 1
)
if not exist data mkdir data
if not exist data\visitors.txt type nul > data\visitors.txt
if not exist data\blacklist.txt type nul > data\blacklist.txt
echo Starting local website at http://localhost:3000
echo Keep this window open while using the app.
start "" cmd /c "timeout /t 2 >nul & start http://localhost:3000"
node server.js
pause
