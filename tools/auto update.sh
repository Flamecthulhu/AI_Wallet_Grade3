current_time=$(date +"%H:%M:%S")
current_date=$(date +"%Y-%m-%d")
current_os="$(sw_vers -productName) $(sw_vers -productVersion)"
echo "$current_time $current_date $current_os"
if ! git config --global user.name > /dev/null; then
    read -p "Enter your git user name: " gitname
    git config --global user.name "$gitname"
fi

if ! git config --global user.email > /dev/null; then
    read -p "Enter your git user email: " gitemail
    git config --global user.email "$gitemail"
fi

