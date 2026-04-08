# GUIDE D'INSTALLATION MANUEL - JEUX D'ECHECS RAYLIB

## RÉSUMÉ DU PROBLÈME

Votre code est **correct**, mais raylib n'est pas configuré pour compiler. Votre MinGW 6.3.0 (très ancien) n'est pas compatible avec las bibliothèques raylib moderna.

## SOLUTION RAPIDE : MSYS2 (5 minutes)

### Étape 1 : Installer MinGW + Raylib
1. **Ouvrez PowerShel en tant qu'administrateur** (clic droit → Exécuter en tant qu'administrateur)
2. **Exécutez cette commande** :
```powershell
C:\msys64\msys2_shell.cmd -mingw64 -no-start
```
3. Dans le terminal MSYS2, tapez :
```bash
pacman -Sy
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-raylib --noconfirm
exit
```

### Étape 2 : Mettre à jour VS Code
1. Ouvrez VS Code
2. Appuyez sur `Ctrl+Maj+P` → tapez "settings"
3. Ouvrez "Preferences: Open Settings (JSON)"
4. Ajoutez cette ligne (ou modifiez-la si elle existe) :
```json
"C_Cpp.default.compilerPath": "C:\\msys64\\mingw64\\bin\\gcc.exe"
```
5. Sauvegardez (Ctrl+S)

### Étape 3 : Compiler et Exécuter
* **Compiler** : Appuyez sur `Ctrl+Maj+B`
* **Lancer** : Appuyez sur `F5` (ou `Ctrl+Maj+D`)

---

## ALTERNATIVE : Compiler manuellement depuis le terminal

Après avoir installé MinGW + Raylib (Étape 1 ci-dessus), exécutez:

```bash
cd "c:\Users\iphon\Desktop\TP 3\chess_project"
C:\msys64\mingw64\bin\gcc -Wall -Wextra -g3 Untitled-1.c -o chess.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lm
.\chess.exe
```

---

## VÉ RIFICATIONS

### Vérifier l'installation de gcc
```powershell
C:\msys64\mingw64\bin\gcc --version
```

### Vérifier l'installation de raylib
```powershell
Test-Path "C:\msys64\mingw64\include\raylib.h"
```

Si le résultat est `True`, raylib est installé. Si c'est `False`, réexécutez :
```bash
pacman -S mingw-w64-x86_64-raylib
```

---

## EN CAS DE PROBLÈME

### "gcc : Le terme n'est pas reconnu..."
→ Raylib/MinGW n'est pas installé. Exécutez de nouveau l'Étape 1

### "fatal error: raylib.h: No such file"  
→ Raylib n'est pas installé. Dans MSYS2, tapez :
```bash
pacman -S mingw-w64-x86_64-raylib
```

### "undefined reference to `DrawRectangle`"
→ C'est normal en cas de problème de linking. Assurez-vous que toutes les bibliothèques sont ajoutées à la compilation (-lraylib -lopengl32 -lgdi32 -lwinmm).

---

## ✅ FICHIERS FOURNIS

* `Untitled-1.c` - Votre code du jeu
* `README.md` - Documentation du projet
* `Makefile` - Compilation avec `make`
* `.vscode/tasks.json` - Configuration VS Code
* `setup-msys2.bat` - Installation automatique (à exécuter comme admin)

---

## 🎮 UTILISATION DU JEU

* **Clic gauche** sur une pièce → Sélectionner
* **Clic gauche** sur une case → Déplacer
* **Majuscules** = Pièces blanches
* **Minuscules** = Pièces noires

---

**Fait le:** 1er avril 2026  
**Status:** Prêt à compiler dès que raylib est installé

