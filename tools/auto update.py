#!/usr/bin/env python3
import subprocess
import datetime
import getpass

# 取得時間與 macOS 版本
current_time = datetime.datetime.now().strftime("%H:%M:%S")
current_date = datetime.datetime.now().strftime("%Y-%m-%d")
current_os = subprocess.getoutput("sw_vers -productName") + " " + subprocess.getoutput("sw_vers -productVersion")

# 設定 git 使用者名稱與 email
def set_git_config(key, prompt):
    try:
        value = subprocess.check_output(["git", "config", "--global", key]).decode().strip()
    except subprocess.CalledProcessError:
        value = input(prompt)
        subprocess.run(["git", "config", "--global", key, value])

set_git_config("user.name", "Enter your git user name: ")
set_git_config("user.email", "Enter your git user email: ")

# 輸入更新訊息
user_update = input("Enter your update message: ")

# 執行 git 指令
subprocess.run(["git", "add", "."])
commit_msg = f"[{current_date} {current_time} From {current_os}]: {user_update}"
subprocess.run(["git", "commit", "-m", commit_msg])
subprocess.run(["git", "push", "origin", "main"])
