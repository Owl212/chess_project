@echo off
REM Test script - Verifie si tout est installe pour compiler le jeu d'echecs

echo.
echo ==========================================
echo TEST DE CONFIGURATION - JEU D'ECHECS
echo ==========================================
echo.

setlocal enabledelayedexpansion

set "FOUND=0"

echo Verification 1: MSYS2...
if exist "C:\msys64\mingw64\bin\gcc.exe" (
    echo   OK - GCC trouve
    set "FOUND=1"
) else (
    echo   ERREUR - GCC non trouve
    echo   Installez MinGW via: C:\msys64\msys2_shell.cmd -mingw64
)

if "!FOUND!"=="0" (
    echo.
    echo Compilation impossible actuellement.
    echo.
    echo ACTION: Exécutez d'abord setup-msys2.bat en admin
    pause
    exit /b 1
)

echo.
echo Verification 2: Raylib...
if exist "C:\msys64\mingw64\include\raylib.h" (
    echo   OK - Raylib installe
) else (
    echo   ERREUR - Raylib non trouve
    echo   Installez via: pacman -S mingw-w64-x86_64-raylib
)

echo.
echo Verification 3: Code source...
if exist "Untitled-1.c" (
    echo   OK - Untitled-1.c trouve
) else (
    echo   ERREUR - Untitled-1.c manquant
)

echo.
echo ==========================================
echo TEST: Compilation du jeu
echo ==========================================
echo.

cd /d "%~dp0"

C:\msys64\mingw64\bin\gcc -Wall -Wextra -g3 Untitled-1.c -o chess.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lm

if %errorlevel% equ 0 (
    echo.
    echo ✓ Compilation reussie!
    echo.
    echo Lancement du jeu...
    echo.
    .\chess.exe
) else (
    echo.
    echo ✗ Compilation echouee!
    echo.
    echo Verifiez:
    echo  1. MSYS2 est installe
    echo  2. Raylib est installe (pacman -S mingw-w64-x86_64-raylib)
    echo  3. Les paths sont corrects
    echo.
    pause
)
