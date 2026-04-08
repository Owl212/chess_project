# Installation automatisee de MSYS2 + Raylib + Configuration VS Code
# Executez dans PowerShell (admin si possible)

Write-Host "Installation MSYS2 + Raylib pour jeu d'Echecs VS Code" -ForegroundColor Cyan
Write-Host ""

$home = $env:USERPROFILE
$msys2Path = "C:\msys64"
$installerPath = "$home\msys2-setup.exe"

# ETAPE 1: Verifier MSYS2
Write-Host "Verification de MSYS2..." -ForegroundColor Yellow

if (Test-Path $msys2Path) {
    Write-Host "   OK - MSYS2 deja installe" -ForegroundColor Green
}
else {
    Write-Host "   MSYS2 non trouve. Telechargement..." -ForegroundColor Red
    
    Write-Host "   Telechargement de MSYS2 (130MB)..."
    $url = "https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe"
    $ProgressPreference = 'SilentlyContinue'
    Invoke-WebRequest -Uri $url -OutFile $installerPath -UseBasicParsing
    
    Write-Host "   Execution de l'installeur..." -ForegroundColor Cyan
    Write-Host "   (Cliquez sur Next et acceptez les valeurs par defaut)" -ForegroundColor Gray
    & $installerPath
    
    Start-Sleep -Seconds 5
    
    if (Test-Path $msys2Path) {
        Write-Host "   OK - MSYS2 installe avec succes" -ForegroundColor Green
    }
    else {
        Write-Host "   PROBLEME - Installation incomplete. Relancez l'installeur manuellement." -ForegroundColor Yellow
        exit 1
    }
}

# ETAPE 2: Installer Raylib
Write-Host ""
Write-Host "Installation de raylib..." -ForegroundColor Yellow

$pacmanPath = "$msys2Path\usr\bin\pacman.exe"

if (Test-Path $pacmanPath) {
    Write-Host "   Mise a jour de pacman..."
    & $pacmanPath -Syy --noconfirm
    
    Write-Host "   Installation de raylib et outils..."
    & $pacmanPath -S mingw-w64-i686-raylib mingw-w64-i686-make --noconfirm
    
    $raylibHeader = "$msys2Path\mingw32\include\raylib.h"
    if (Test-Path $raylibHeader) {
        Write-Host "   OK - Raylib installe" -ForegroundColor Green
    }
    else {
        Write-Host "   PROBLEME - Raylib introuvable" -ForegroundColor Yellow
        exit 1
    }
}
else {
    Write-Host "   ERREUR - Pacman non trouve (MSYS2 pas complet)" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Installation Terminie!" -ForegroundColor Green
Write-Host ""
Write-Host "Prochaines etapes:" -ForegroundColor Cyan
Write-Host "   1. Fermez et reouvrez VS Code"
Write-Host "   2. Appuyez sur Ctrl+Maj+B pour compiler"
Write-Host "   3. Appuyez sur F5 pour lancer"
Write-Host ""

Remove-Item $installerPath -ErrorAction SilentlyContinue

Read-Host "Appuyez sur Entree pour terminer"
