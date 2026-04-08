#include "chess.h"

#include <stdio.h>

static Color CouleurCase(int x, int y) {
    Color claire = (Color){ 240, 217, 181, 255 };
    Color foncee = (Color){ 181, 136, 99, 255 };
    return ((x + y) % 2 == 0) ? claire : foncee;
}

static const char* SymbolePiece(Piece piece) {
    if (piece.type == VIDE) return " ";
    
    if (piece.couleur == BLANC) {
        switch (piece.type) {
            case PION: return "♙";
            case CAVALIER: return "♘";
            case FOU: return "♗";
            case TOUR: return "♖";
            case REINE: return "♕";
            case ROI: return "♔";
            default: return " ";
        }
    } else {
        switch (piece.type) {
            case PION: return "♟";
            case CAVALIER: return "♞";
            case FOU: return "♝";
            case TOUR: return "♜";
            case REINE: return "♛";
            case ROI: return "♚";
            default: return " ";
        }
    }
}

static bool PositionValide(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

static const char* NomPiece(TypePiece type) {
    switch (type) {
        case PION: return "P";
        case CAVALIER: return "C";
        case FOU: return "F";
        case TOUR: return "T";
        case REINE: return "D";
        case ROI: return "R";
        default: return "";
    }
}

static void FormatCase(int x, int y, char out[3]) {
    out[0] = (char)('a' + x);
    out[1] = (char)('8' - y);
    out[2] = '\0';
}

static void FormatCoup(const Coup* coup, char out[48]) {
    char depart[3];
    char arrivee[3];
    FormatCase(coup->startX, coup->startY, depart);
    FormatCase(coup->endX, coup->endY, arrivee);

    if (coup->estRoque) {
        if (coup->endX == 6) {
            snprintf(out, 48, "O-O");
        } else {
            snprintf(out, 48, "O-O-O");
        }
        return;
    }

    snprintf(
        out,
        48,
        "%s%s%s%s",
        NomPiece(coup->pieceDeplacee.type),
        depart,
        (coup->pieceCapturee.type != VIDE || coup->estPriseEnPassant) ? "x" : "-",
        arrivee
    );
}

void ChargerTextures(Texture2D textures[]) {
    (void)textures;
}

void DessinerPlateau(Partie* p, int caseSelX, int caseSelY, Texture2D textures[]) {
    (void)textures;

    const int tailleCase = 80;
    const int margeTexteX = 30;
    const int margeTexteY = 20;
    const int largeurPlateau = 8 * tailleCase;
    const int barreInfosY = 8 * tailleCase;
    const int panneauX = largeurPlateau;
    const int largeurPanneau = 340;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Color couleurCase = CouleurCase(x, y);
            if (x == caseSelX && y == caseSelY) {
                couleurCase = (Color){ 205, 210, 106, 255 };
            }

            DrawRectangle(x * tailleCase, y * tailleCase, tailleCase, tailleCase, couleurCase);
        }
    }

    if (PositionValide(caseSelX, caseSelY)) {
        Piece pieceSelectionnee = p->cases[caseSelY][caseSelX];
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (EstMouvementLegal(p, caseSelX, caseSelY, x, y)) {
                    bool caseOccupee = p->cases[y][x].type != VIDE;
                    Color cercle = caseOccupee ? (Color){ 20, 20, 20, 120 } : (Color){ 20, 20, 20, 90 };
                    DrawCircle(x * tailleCase + tailleCase / 2, y * tailleCase + tailleCase / 2, caseOccupee ? 14.0f : 10.0f, cercle);
                }
            }
        }

        if (pieceSelectionnee.type != VIDE) {
            DrawRectangleLines(caseSelX * tailleCase, caseSelY * tailleCase, tailleCase, tailleCase, BLACK);
        }
    }

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece piece = p->cases[y][x];
            if (piece.type != VIDE) {
                Color couleurPiece = (piece.couleur == BLANC) ? WHITE : BLACK;
                DrawText(SymbolePiece(piece), x * tailleCase + margeTexteX - 8, y * tailleCase + margeTexteY - 5, 50, couleurPiece);
            }
        }
    }

    DrawRectangle(0, barreInfosY, largeurPlateau + largeurPanneau, 80, (Color){ 28, 28, 28, 255 });
    DrawText(TextFormat("Blanc: %.0fs", p->tempsBlanc), 18, barreInfosY + 12, 24, RAYWHITE);
    DrawText(TextFormat("Noir: %.0fs", p->tempsNoir), 220, barreInfosY + 12, 24, RAYWHITE);
    DrawText((p->tourActuel == BLANC) ? "Tour: Blanc" : "Tour: Noir", 420, barreInfosY + 12, 24, RAYWHITE);

    bool mat = EstEchecEtMat(p, p->tourActuel);
    bool pat = EstPat(p, p->tourActuel);
    if (mat) {
        DrawText((p->tourActuel == BLANC) ? "CHECKMATE - Noir gagne" : "CHECKMATE - Blanc gagne", 18, barreInfosY + 42, 20, ORANGE);
    } else if (pat) {
        DrawText("STALEMATE - Nulle", 18, barreInfosY + 42, 20, (Color){100, 150, 200, 255});
    } else if (p->tempsBlanc <= 0.0f) {
        DrawText("Flag: Noir gagne", 18, barreInfosY + 42, 20, ORANGE);
    } else if (p->tempsNoir <= 0.0f) {
        DrawText("Flag: Blanc gagne", 18, barreInfosY + 42, 20, ORANGE);
    } else if (EstEnEchec(p, p->tourActuel)) {
        DrawText("CHECK", 18, barreInfosY + 42, 20, RED);
    }

    DrawRectangle(panneauX, 0, largeurPanneau, largeurPlateau, (Color){ 244, 244, 244, 255 });
    DrawLine(panneauX, 0, panneauX, largeurPlateau, (Color){ 200, 200, 200, 255 });
    DrawText("Historique", panneauX + 16, 16, 30, (Color){ 35, 35, 35, 255 });

    int coupsAffichables = 22;
    int debut = 0;
    if (p->historique.taille > coupsAffichables) {
        debut = p->historique.taille - coupsAffichables;
    }

    int ligneY = 60;
    for (int i = debut; i < p->historique.taille; i++) {
        char notation[48];
        FormatCoup(&p->historique.coups[i], notation);
        DrawText(TextFormat("%d. %s", i + 1, notation), panneauX + 16, ligneY, 20, (Color){ 50, 50, 50, 255 });
        ligneY += 24;
    }
}
