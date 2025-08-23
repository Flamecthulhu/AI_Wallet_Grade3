@echo off
for /f "tokens=2 delims==" %%i in ('"wmic os get localdatetime /value"') do set ldt=%%i
set current_date=%ldt:~0,4%-%ldt:~4,2%-%ldt:~6,2%
set current_time=%ldt:~8,2%:%ldt:~10,2%:%ldt:~12,2%

git config --global user.name "Flamecthulhu"
git config --global user.email "allancosmo107001@gmail.com"
set /p UserUpdate=Enter your update message: 
git config --global --list
git add .
git commit -m "[%current_date% %current_time% From Windows]: %UserUpdate%"
git push origin main

REM [2025-08-23 20:10:32 From Windows]: