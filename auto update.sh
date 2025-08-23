current_time=$(date +"%H:%M:%S")
current_date=$(date +"%Y-%m-%d")

git config --global user.name "Flamecthulhu"
git config --global user.email "allancosmo107001@gmail.com"

read -p "Enter your update message: " UserUpdate

git config --global --list
git add .
git commit -m "[${current_date} ${current_time} From macOS]: ${UserUpdate}"
git push origin main

#[2025-08-22 17:30:32 From macOS]: 