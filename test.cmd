for /f "delims=" %%i in ('powershell -NoProfile -Command "Get-Date -Format \"yyyy-MM-dd HH:mm:ss\""') do set datetime=%%i
set current_date=%datetime:~0,10%
set current_time=%datetime:~11,8%
for /f "delims=" %%i in ('powershell -NoProfile -Command "(Get-CimInstance Win32_OperatingSystem).Caption -replace \"Microsoft \""') do set current_os=%%i
echo %current_date% %current_time% From %current_os%