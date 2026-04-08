# DEMARRAGE RAPIDE (2 minutes)

## Les 3 étapes (faciles!)

### 1️⃣ Installer les outils
**Exécutez en admin** (clic droit → Admin):
```
C:\msys64\msys2_shell.cmd -mingw64
```

Puis tapez dans la fenêtre (copier-coller):
```bash
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-raylib --noconfirm
exit
```

*(Ça télécharge ~800MB, ça prend 5-10 min...)*

### 2️⃣ Compiler
Depuis VS Code:
- Appuyez sur **`Ctrl+Maj+B`**

Ou depuis le terminal:
- Allez dans le dossier du projet  
- Exécutez `test-build.bat`

### 3️⃣ Lancer
* **Depuis VS Code** : Appuyez sur **`F5`**
* **Depuis Explorer** : Double-cliquez sur `chess.exe`

---

## ✅ Vérification Rapide

```powershell
C:\msys64\mingw64\bin\gcc --version
Test-Path "C:\msys64\mingw64\include\raylib.h"
```

Si les deux fonctionnent → C'est prêt!

---

**Besoin d'aide supplémentaire ?** Voir `INSTALLATION.md`
