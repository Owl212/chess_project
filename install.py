#!/usr/bin/env python3

"""
Installer MSYS2 et MinGW-w64 avec raylib pour bien compiler le projet Chess.
Ce script automatise l'installation de tout ce qui est nécessaire.
"""

import os
import subprocess
import requests
import zipfile
import shutil
from pathlib import Path

def main():
    home = Path.home()
    msys2_path = home / "msys64"
    
    print("=" * 60)
    print("Configuration raylib + MinGW pour VS Code")
    print("=" * 60)
    
    if msys2_path.exists():
        print(f"\n✓ MSYS2 trouvé à {msys2_path}")
    else:
        print(f"\n✗ MSYS2 non trouvé. Téléchargement...")
        
        installer_url = "https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe"
        installer_path = home / "msys2-installer.exe"
        
        print(f"Téléchargement depuis {installer_url}...")
        response = requests.get(installer_url)
        with open(installer_path, 'wb') as f:
            f.write(response.content)
        
        print("Exécution de l'installeur...")
        subprocess.run([str(installer_path)], check=True)
        
        installer_path.unlink()
        print(f"✓ MSYS2 installé à {msys2_path}")
    
    # Mettre à jour mingw et installer raylib
    print("\n✓ Installation de raylib via pacman...")
    pacman_exe = msys2_path / "usr" / "bin" / "pacman.exe"
    
    subprocess.run([
        str(pacman_exe), 
        "-Syy", 
        "--noconfirm"
    ], check=True)
    
    subprocess.run([
        str(pacman_exe),
        "-S",
        "mingw-w64-i686-raylib",
        "mingw-w64-i686-cmake",
        "--noconfirm"
    ], check=True)
    
    print("\n✓ Installation terminée!")
    print("\nFichiers importants:")
    print(f"  GCC: {msys2_path / 'mingw32' / 'bin' / 'gcc.exe'}")
    print(f"  Raylib: {msys2_path / 'mingw32' / 'include' / 'raylib.h'}")
    
    print("\nMettez à jour VS Code tasks.json avec les chemins MSYS2.")
    print("=" * 60)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"✗ Erreur: {e}")
        exit(1)
