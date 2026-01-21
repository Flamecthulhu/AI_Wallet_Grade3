Set-Location "c:\Users\allan\OneDrive\Documents\AI_Wallet_grade3\AI_Wallet_Grade3"
$currentDateTime = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$currentOS = (Get-CimInstance Win32_OperatingSystem).Caption -replace "Microsoft ", ""
$gitName = git config --global user.name
if (-not $gitName) {
    $gitName = Read-Host "Enter your git user name"
    git config --global user.name $gitName
}
$gitEmail = git config --global user.email
if (-not $gitEmail) {
    $gitEmail = Read-Host "Enter your git user email"
    git config --global user.email $gitEmail
}
git add .
git commit -m "[$currentDateTime From $currentOS]: Windows auto commit"
git push origin main