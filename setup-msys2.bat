@echo off
cd /d C:\msys64
echo Installation de MinGW + Raylib dans MSYS2...
echo.

rem Update pacman
echo Mise a jour des bases de donnees...
msys2_shell.cmd -mingw64 -c "pacman -Sy --noconfirm" >nul 2>&1

echo Installation de gcc et outils (mingw64)...
msys2_shell.cmd -mingw64 -c "pacman -S --noconfirm mingw-w64-x86_64-toolchain mingw-w64-x86_64-raylib mingw-w64-x86_64-make" 

echo.
echo Installation terminee!
echo.
pause
