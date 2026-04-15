# Jeu d'Échecs Raylib - Configuration Complète

## GitHub

- Dépôt: https://github.com/Owl212/chess_project
- Releases (téléchargement): https://github.com/Owl212/chess_project/releases/latest
- Téléchargement direct de l'app (lien à partager): https://github.com/Owl212/chess_project/releases/latest/download/chess-windows.zip

Le dépôt est configuré pour GitHub: Owl212/chess_project.

Exemple de lien que vous pouvez envoyer:
https://github.com/mon-user/chess_project/releases/latest/download/chess-windows.zip

## Télécharger le jeu (Windows)

1. Ouvrez le lien direct: https://github.com/Owl212/chess_project/releases/latest/download/chess-windows.zip
2. Décompressez le ZIP.
3. Lancez chess.exe.

## Créer l'archive ZIP à publier

Depuis la racine du projet:

```powershell
.\build-release.bat
```

Le script génère `dist/chess-windows.zip` prêt à être envoyé dans GitHub Releases.

## 🔧 Installation Complète (choix simple : MSYS2)

**Le problème:** Votre MinGW 6.3.0 est trop vieux pour être compatible avec raylib moderne.

**La solution:** Installer MSYS2 (incluant MinGW-w64 récent et raylib)

### Étape 1: Installer MSYS2
1. Téléchargez: https://github.com/msys2/msys2-installer/releases
2. Exécutez le fichier `.exe`
3. Installez dans le dossier par défaut: `C:\msys64`

### Étape 2: Installer raylib
Ouvrez un PowerShell et exécutez:
```powershell
C:\msys64\msys2_shell.cmd -mingw32
pacman -Syy
pacman -S mingw-w64-i686-raylib --noconfirm
exit
```

### Étape 3: Configurer VS Code
1. Appuyez sur `Ctrl+K Ctrl+0` (zéro) pour ouvrir Paramètres
2. Recherchez "C_Cpp: Compiler Path"
3. Remplacez par: `C:\msys64\mingw32\bin\gcc.exe`

### Étape 4: Compiler et Lancer
- **Compiler**: Appuyez sur `Ctrl+Shift+B`
- **Lancer**: Appuyez sur `F5`

---

## Alternative: Installation Automatisée (Python)

Si vous avez Python installé:
```bash
python install.py
```

Cela installera MSYS2 et raylib automatiquement.

---

## 📝 Structure du Projet

```
chess_project/
├── Untitled-1.c          # Code principal du jeu
├── Makefile              # Compilation manuelle: make
├── .vscode/
│   ├── tasks.json        # Tâches VS Code
│   └── launch.json       # Configuration débogage
├── chess.exe             # Exécutable (après compilation)
└── README.md             # Ce fichier
```

---

## ⚙️ Commandes Utiles

### Avec VS Code
- `Ctrl+Shift+B` → Compiler
- `F5` → Lancer le débogueur

### Depuis le Terminal
```bash
# Compiler (avec MSYS2):
C:\msys64\mingw32\bin\gcc -Wall -Wextra -g3 Untitled-1.c -o chess.exe -lraylib -lopengl32 -lgdi32 -lwinmm

# Lancer:
.\chess.exe
```

### Avec Makefile
```bash
make              # Compiler
make run          # Compiler + Exécuter
make clean        # Supprimer les fichiers compilés
```

---

## 🐛 Dépannage

**❌ "raylib.h: No such file or directory"**
→ MSYS2 n'est pas installé ou raylib n'est pas trouvé

**❌ "undefined reference to `DrawRectangle`"**
→ Les versions MinGW ne correspondent pas (MinGW 6.3.0 + raylib 4.5.0 incompatibles)

**❌ Compilateur non trouvé dans VS Code**
→ Mettez à jour le chemin du compilateur dans les paramètres (voir Étape 3 ci-dessus)

---

## 🎮 Utilisation du Jeu

- **Clic gauche** sur une pièce pour la sélectionner
- **Clic gauche** sur une autre case pour la déplacer
- **Clic sur la même case** pour annuler la sélection

Les pièces blanches sont en MAJUSCULES, les noires en minuscules.

---

**Besoin d'aide?** Consultez la section Dépannage ou vérifiez que:
1. MSYS2 est installé
2. Raylib est installé via pacman
3. VS Code utilise le bon chemin de compilateur

