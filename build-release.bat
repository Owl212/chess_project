@echo off
setlocal

cd /d "%~dp0"

set "MSYS_BIN=C:\msys64\mingw64\bin"
set "OUT_ROOT=dist"
set "OUT_DIR=%OUT_ROOT%\chess-windows"
set "ZIP_PATH=%OUT_ROOT%\chess-windows.zip"

echo.
echo =========================================
echo Build release package (Windows)
echo =========================================
echo.

if not exist "%MSYS_BIN%\gcc.exe" (
    echo ERREUR: gcc introuvable dans %MSYS_BIN%
    echo Installez MSYS2 et le toolchain mingw64 avant de continuer.
    exit /b 1
)

if not exist "src\main.c" (
    echo ERREUR: src\main.c introuvable.
    exit /b 1
)

if exist "%OUT_DIR%" rmdir /s /q "%OUT_DIR%"
if exist "%ZIP_PATH%" del /q "%ZIP_PATH%"

mkdir "%OUT_DIR%" >nul

echo [1/3] Compilation de chess.exe...
"%MSYS_BIN%\gcc.exe" -Wall -Wextra -g3 src\main.c src\board.c src\ui.c src\rules.c src\moves.c src\ai.c -Iinclude -o "%OUT_DIR%\chess.exe" -lraylib -lopengl32 -lgdi32 -lwinmm -lm
if errorlevel 1 (
    echo.
    echo ERREUR: compilation echouee.
    exit /b 1
)

echo [2/3] Copie des assets et des DLL runtime...
xcopy "assets" "%OUT_DIR%\assets" /E /I /Y >nul
copy /Y "README.md" "%OUT_DIR%\README.md" >nul
copy /Y "QUICK-START.md" "%OUT_DIR%\QUICK-START.md" >nul

for %%f in (libraylib.dll raylib.dll libgcc_s_seh-1.dll libwinpthread-1.dll libstdc++-6.dll) do (
    if exist "%MSYS_BIN%\%%f" copy /Y "%MSYS_BIN%\%%f" "%OUT_DIR%\%%f" >nul
)

echo [3/3] Creation de l'archive ZIP...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%OUT_DIR%\*' -DestinationPath '%ZIP_PATH%' -Force"
if errorlevel 1 (
    echo.
    echo ERREUR: creation du ZIP echouee.
    exit /b 1
)

echo.
echo OK: package cree -> %ZIP_PATH%
echo Publiez ce fichier dans GitHub Releases pour le telechargement.
echo.
endlocal
