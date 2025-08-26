import subprocess
import datetime

current_time = datetime.datetime.now().strftime("%H:%M:%S")
current_date = datetime.datetime.now().strftime("%Y-%m-%d")
current_os = subprocess.getoutput("sw_vers -productName") + " " + subprocess.getoutput("sw_vers -productVersion")
if current_os == "'sw_vers' is not recognized as an internal or external command,\noperable program or batch file. 'sw_vers' is not recognized as an internal or external command,\noperable program or batch file.":
    current_os = (subprocess.getoutput('powershell -NoProfile -Command "(Get-CimInstance Win32_OperatingSystem).Caption"')).strip("Microsoft")

def set_git_config(key, prompt):
    try:
        value = subprocess.check_output(["git", "config", "--global", key]).decode().strip()
    except subprocess.CalledProcessError:
        value = input(prompt)
        subprocess.run(["git", "config", "--global", key, value])

set_git_config("user.name", "Enter your git user name: ")
set_git_config("user.email", "Enter your git user email: ")
user_update = input("Enter your update message: ")
subprocess.run(["git", "add", "."])
commit_msg = f"[{current_date} {current_time} From {current_os}]: {user_update}"
subprocess.run(["git", "commit", "-m", commit_msg])
subprocess.run(["git", "push", "origin", "main"])