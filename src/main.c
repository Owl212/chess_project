#include "chess.h"

#define TAILLE_CASE 80

static bool PositionValide(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

int main(void) {
    InitWindow(8 * TAILLE_CASE, 8 * TAILLE_CASE, "Lichess Clone - structure initiale");
    SetTargetFPS(60);

    Partie partie;
    InitPartie(&partie);

    Texture2D textures[12] = { 0 };
    ChargerTextures(textures);

    int caseSelX = -1;
    int caseSelY = -1;

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        if (partie.tourActuel == BLANC) {
            partie.tempsBlanc -= delta;
        } else {
            partie.tempsNoir -= delta;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX() / TAILLE_CASE;
            int mouseY = GetMouseY() / TAILLE_CASE;

            if (PositionValide(mouseX, mouseY)) {
                if (caseSelX == mouseX && caseSelY == mouseY) {
                    caseSelX = -1;
                    caseSelY = -1;
                } else if (caseSelX >= 0 && caseSelY >= 0) {
                    partie.cases[mouseY][mouseX] = partie.cases[caseSelY][caseSelX];
                    partie.cases[caseSelY][caseSelX] = (Piece){ VIDE, AUCUNE, false };
                    caseSelX = -1;
                    caseSelY = -1;
                } else if (partie.cases[mouseY][mouseX].type != VIDE) {
                    caseSelX = mouseX;
                    caseSelY = mouseY;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DessinerPlateau(&partie, caseSelX, caseSelY, textures);
        EndDrawing();
    }

    FreePartie(&partie);
    CloseWindow();
    return 0;
}
