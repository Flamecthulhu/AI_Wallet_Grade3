@echo off
for /f "delims=" %%a in ('time /t') do set current_time=%%a
for /f "delims=" %%a in ('date /t') do set current_date=%%a
git config --global user.name "Flamecthulhu"
git config --global user.email "allancosmo107001@gmail.com"
set /p UserUpdate=Enter your update message: 
git config --global --list
git add .
git commit -m  "[%current_date% %current_time%]: %UserUpdate%"
git push origin main