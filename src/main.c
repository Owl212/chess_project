#include "chess.h"

#define TAILLE_CASE 80
#define LARGEUR_PLATEAU (8 * TAILLE_CASE)
#define LARGEUR_PANNEAU 340
#define LARGEUR_FENETRE (LARGEUR_PLATEAU + LARGEUR_PANNEAU)
#define HAUTEUR_INFO 80
#define HAUTEUR_FENETRE (8 * TAILLE_CASE + HAUTEUR_INFO)

static bool PositionValide(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

int main(void) {
    InitWindow(LARGEUR_FENETRE, HAUTEUR_FENETRE, "Lichess Clone - structure initiale");
    SetTargetFPS(60);

    Partie partie;
    InitPartie(&partie);

    Texture2D textures[12] = { 0 };
    ChargerTextures(textures);

    int caseSelX = -1;
    int caseSelY = -1;

    while (!WindowShouldClose()) {
        bool mat = EstEchecEtMat(&partie, partie.tourActuel);
        bool pat = EstPat(&partie, partie.tourActuel);
        bool tempsEcoule = partie.tempsBlanc <= 0.0f || partie.tempsNoir <= 0.0f;
        bool partieTerminee = mat || pat || tempsEcoule;

        float delta = GetFrameTime();
        if (!partieTerminee) {
            if (partie.tourActuel == BLANC) {
                partie.tempsBlanc -= delta;
                if (partie.tempsBlanc < 0.0f) {
                    partie.tempsBlanc = 0.0f;
                }
            } else {
                partie.tempsNoir -= delta;
                if (partie.tempsNoir < 0.0f) {
                    partie.tempsNoir = 0.0f;
                }
            }
        }

        if (!partieTerminee && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX() / TAILLE_CASE;
            int mouseY = GetMouseY() / TAILLE_CASE;

            if (PositionValide(mouseX, mouseY)) {
                Piece pieceCliquee = partie.cases[mouseY][mouseX];

                if (caseSelX == mouseX && caseSelY == mouseY) {
                    caseSelX = -1;
                    caseSelY = -1;
                } else if (caseSelX >= 0 && caseSelY >= 0 && EstMouvementLegal(&partie, caseSelX, caseSelY, mouseX, mouseY)) {
                    JouerCoup(&partie, caseSelX, caseSelY, mouseX, mouseY);
                    caseSelX = -1;
                    caseSelY = -1;
                } else if (pieceCliquee.type != VIDE && pieceCliquee.couleur == partie.tourActuel) {
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
