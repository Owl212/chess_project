# Publier le jeu sur GitHub + Telechargement

## 1) Configurer le lien GitHub du projet

Si votre repo local n'a pas encore de remote:

```powershell
git remote add origin https://github.com/Owl212/chess_project.git
git branch -M main
git push -u origin main
```

Ensuite, remplacez les placeholders dans README.md:
- https://github.com/Owl212/chess_project
- https://github.com/Owl212/chess_project/releases/latest
- https://github.com/Owl212/chess_project/releases/latest/download/chess-windows.zip

## 2) Generer le fichier telechargeable (ZIP)

Depuis la racine du projet:

```powershell
.\build-release.bat
```

Le script cree:
- dist/chess-windows/chess.exe
- dist/chess-windows/assets/...
- dist/chess-windows/*.dll necessaires
- dist/chess-windows.zip

## 3) Publier la release sur GitHub

1. Ouvrez votre repository GitHub.
2. Allez dans Releases -> Draft a new release.
3. Creez un tag (ex: v1.0.0).
4. Uploadez dist/chess-windows.zip dans Assets.
5. Publiez la release.

Les joueurs pourront ensuite telecharger le jeu via:
- https://github.com/Owl212/chess_project/releases/latest
- Lien direct a partager: https://github.com/Owl212/chess_project/releases/latest/download/chess-windows.zip

Important:
- Conservez exactement le nom de fichier chess-windows.zip dans les assets de release.
- Ce nom permet au lien direct /download/chess-windows.zip de toujours fonctionner.
