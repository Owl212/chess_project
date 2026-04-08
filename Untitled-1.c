#include "raylib.h"
#include <stdbool.h>

#define SQUARE_SIZE 80
#define BOARD_SIZE 8

// Représentation de l'échiquier (Minuscules = Noirs, Majuscules = Blancs)
char board[BOARD_SIZE][BOARD_SIZE] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

// Variables pour la sélection
bool pieceSelected = false;
int selectedX = -1;
int selectedY = -1;

void DrawChessBoard() {
    Color lightSquare = { 240, 217, 181, 255 }; // Couleur Lichess
    Color darkSquare = { 181, 136, 99, 255 };   // Couleur Lichess
    Color highlight = { 205, 210, 106, 200 };   // Couleur de sélection

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Color squareColor = ((x + y) % 2 == 0) ? lightSquare : darkSquare;
            
            // Mettre en surbrillance la case sélectionnée
            if (pieceSelected && x == selectedX && y == selectedY) {
                squareColor = highlight;
            }

            DrawRectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, squareColor);

            // Dessiner la pièce (texte pour l'instant)
            char piece = board[y][x];
            if (piece != ' ') {
                Color pieceColor = (piece >= 'A' && piece <= 'Z') ? WHITE : BLACK;
                DrawText(TextFormat("%c", piece), x * SQUARE_SIZE + 25, y * SQUARE_SIZE + 20, 40, pieceColor);
            }
        }
    }
}

int main() {
    InitWindow(BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE, "Raylib Chess - WebAssembly");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- LOGIQUE (Mise à jour) ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX() / SQUARE_SIZE;
            int mouseY = GetMouseY() / SQUARE_SIZE;

            if (mouseX >= 0 && mouseX < BOARD_SIZE && mouseY >= 0 && mouseY < BOARD_SIZE) {
                if (!pieceSelected) {
                    // Sélectionner une pièce si elle existe
                    if (board[mouseY][mouseX] != ' ') {
                        selectedX = mouseX;
                        selectedY = mouseY;
                        pieceSelected = true;
                    }
                } else {
                    // Déplacer la pièce (Mode "Bac à sable" : aucune règle stricte n'est appliquée ici)
                    if (selectedX == mouseX && selectedY == mouseY) {
                        // Annuler la sélection si on clique sur la même case
                        pieceSelected = false;
                    } else {
                        // Manger la pièce ou bouger
                        board[mouseY][mouseX] = board[selectedY][selectedX];
                        board[selectedY][selectedX] = ' ';
                        pieceSelected = false;
                    }
                }
            }
        }

        // --- AFFICHAGE ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawChessBoard();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}