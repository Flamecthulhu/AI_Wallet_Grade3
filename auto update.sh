echo https://github.com/Flamecthulhu/AI_Wallet_Grade3
current_time=$(date +"%H:%M:%S")
current_date=$(date +"%Y-%m-%d")
current_os="$(sw_vers -productName) $(sw_vers -productVersion)"

if ! git config --global user.name > /dev/null; then
    read -p "Enter your git user name: " gitname
    git config --global user.name "$gitname"
fi

if ! git config --global user.email > /dev/null; then
    read -p "Enter your git user email: " gitemail
    git config --global user.email "$gitemail"
fi

read -p "Enter your update message: " UserUpdate

git config --global --list
git add .
git commit -m "[${current_date} ${current_time} From ${current_os}]: ${UserUpdate}"
git push origin main
