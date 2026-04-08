@echo off
REM Installer MSYS2 et raylib - Solution rapide pour le compilateur

echo Installation de MSYS2...
cd %USERPROFILE%
powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; (New-Object Net.WebClient).DownloadFile('https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe', 'msys2-installer.exe')"

echo Execution de l'installeur MSYS2...
msys2-installer.exe

echo Installation de raylib via pacman dans MSYS2...
C:\msys64\usr\bin\pacman.exe -S mingw-w64-i686-raylib --noconfirm

echo.
echo Installation MSYS2 et raylib terminée!
echo VS Code utilisera maintenant C:\msys64\mingw32\bin\gcc.exe
echo.
pause
