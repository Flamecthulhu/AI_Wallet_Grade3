@echo off
for /f "delims=" %%i in ('powershell -NoProfile -Command "Get-Date -Format \"yyyy-MM-dd HH:mm:ss\""') do set datetime=%%i
set current_date=%datetime:~0,10%
set current_time=%datetime:~11,8%
for /f "delims=" %%i in ('powershell -NoProfile -Command "(Get-CimInstance Win32_OperatingSystem).Caption -replace \"Microsoft \""') do set current_os=%%i
for /f "delims=" %%u in ('git config --global user.name') do set gitname=%%u
if "%gitname%"=="" (
    set /p gitname=Enter your git user name: 
    git config --global user.name "%gitname%"
)
for /f "delims=" %%e in ('git config --global user.email') do set gitemail=%%e
if "%gitemail%"=="" (
    set /p gitemail=Enter your git user email: 
    git config --global user.email "%gitemail%"
)
set /p UserUpdate=Enter your update message: 
git config --global --list
git add .
git commit -m "[%current_date% %current_time% From %current_os%]: %UserUpdate%"
git push origin main