#include "chess.h"

static Color CouleurCase(int x, int y) {
    Color claire = (Color){ 240, 217, 181, 255 };
    Color foncee = (Color){ 181, 136, 99, 255 };
    return ((x + y) % 2 == 0) ? claire : foncee;
}

static char SymbolePiece(Piece piece) {
    switch (piece.type) {
        case PION: return 'P';
        case CAVALIER: return 'C';
        case FOU: return 'F';
        case TOUR: return 'T';
        case REINE: return 'D';
        case ROI: return 'R';
        default: return ' ';
    }
}

void ChargerTextures(Texture2D textures[]) {
    (void)textures;
}

void DessinerPlateau(Partie* p, int caseSelX, int caseSelY, Texture2D textures[]) {
    (void)textures;

    const int tailleCase = 80;
    const int margeTexteX = 30;
    const int margeTexteY = 20;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Color couleurCase = CouleurCase(x, y);
            if (x == caseSelX && y == caseSelY) {
                couleurCase = (Color){ 205, 210, 106, 255 };
            }

            DrawRectangle(x * tailleCase, y * tailleCase, tailleCase, tailleCase, couleurCase);

            Piece piece = p->cases[y][x];
            if (piece.type != VIDE) {
                Color couleurPiece = (piece.couleur == BLANC) ? WHITE : BLACK;
                DrawText(TextFormat("%c", SymbolePiece(piece)), x * tailleCase + margeTexteX, y * tailleCase + margeTexteY, 40, couleurPiece);
            }
        }
    }
}
