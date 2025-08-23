@echo off
for /f "delims=" %%i in ('powershell -NoProfile -Command "Get-Date -Format \"yyyy-MM-dd HH:mm:ss\""') do set datetime=%%i

set current_date=%datetime:~0,10%
set current_time=%datetime:~11,8%

git config --global user.name "Flamecthulhu"
git config --global user.email "allancosmo107001@gmail.com"
set /p UserUpdate=Enter your update message: 
git config --global --list
git add .
git commit -m "[%current_date% %current_time% From Windows]: %UserUpdate%"
git push origin main

REM 範例輸出: [2025-08-23 20:10:32 From Windows]: