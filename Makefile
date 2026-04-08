.PHONY: help compile run clean

# Paramètres
PROGRAM = chess.exe
SOURCE = src/main.c src/board.c src/moves.c src/ui.c
OUTPUT = ${PROGRAM}

# Détectez automatiquement le compilateur
ifeq ($(OS),Windows_NT)
    # Essayer MSYS2 en premier
    ifeq ($(shell where C:\msys64\mingw32\bin\gcc.exe 2>nul),)
        # Sinon, utiliser mingw par défaut
        CC = gcc
        RAYLIB_PATH = $(USERPROFILE)\raylib\raylib-4.5.0_win32_mingw-w64
    else
        CC = C:\msys64\mingw32\bin\gcc.exe
        RAYLIB_PATH = C:\msys64\mingw32
    endif
    
    CFLAGS = -Wall -Wextra -g3
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lm
	INCLUDES = -Iinclude -I"$(RAYLIB_PATH)\include"
    LDFLAGS = -L"$(RAYLIB_PATH)\lib"
else
    # Linux/Mac
    CC = gcc
    CFLAGS = -Wall -Wextra -g3 -std=c99
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lX11
	INCLUDES = -Iinclude
    LDFLAGS = 
endif

# Cibles
all: compile

compile: $(OUTPUT)
	@echo.
	@echo ✓ Compilation terminée: $(OUTPUT)
	@echo.

$(OUTPUT): $(SOURCE)
	@echo Compilation avec $(CC)...
	@echo CFLAGS: $(CFLAGS)
	@echo INCLUDES: $(INCLUDES)
	@echo LDFLAGS: $(LDFLAGS)
	@echo LIBS: $(LIBS)
	@echo.
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCE) $(LDFLAGS) $(LIBS) -o $(OUTPUT)

run: $(OUTPUT)
	@echo Lancement de $(OUTPUT)...
	./$(OUTPUT)

clean:
	@echo Suppression des fichiers compilés...
	del /Q $(OUTPUT) 2>nul || rm -f $(OUTPUT)
	@echo ✓ Nettoyage terminé

help:
	@echo Commandes disponibles:
	@echo   make compile    - Compiler le jeu
	@echo   make run        - Compiler et lancer
	@echo   make clean      - Supprimer les fichiers compilés
	@echo   make help       - Afficher cette aide
	@echo.
	@echo Variables d'environnement détectées:
	@echo   Compilateur: $(CC)
	@echo   Chemin raylib: $(RAYLIB_PATH)

.SILENT:

