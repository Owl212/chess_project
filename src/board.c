#include "chess.h"

#include <stdlib.h>

static Piece CreerPiece(TypePiece type, Couleur couleur) {
    Piece piece;
    piece.type = type;
    piece.couleur = couleur;
    piece.aBouge = false;
    return piece;
}

static void ViderPlateau(Partie* p) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            p->cases[y][x] = CreerPiece(VIDE, AUCUNE);
        }
    }
}

void InitPartie(Partie* p) {
    if (p == NULL) {
        return;
    }

    ViderPlateau(p);

    p->cases[0][0] = CreerPiece(TOUR, NOIR);
    p->cases[0][1] = CreerPiece(CAVALIER, NOIR);
    p->cases[0][2] = CreerPiece(FOU, NOIR);
    p->cases[0][3] = CreerPiece(REINE, NOIR);
    p->cases[0][4] = CreerPiece(ROI, NOIR);
    p->cases[0][5] = CreerPiece(FOU, NOIR);
    p->cases[0][6] = CreerPiece(CAVALIER, NOIR);
    p->cases[0][7] = CreerPiece(TOUR, NOIR);

    for (int x = 0; x < 8; x++) {
        p->cases[1][x] = CreerPiece(PION, NOIR);
        p->cases[6][x] = CreerPiece(PION, BLANC);
    }

    p->cases[7][0] = CreerPiece(TOUR, BLANC);
    p->cases[7][1] = CreerPiece(CAVALIER, BLANC);
    p->cases[7][2] = CreerPiece(FOU, BLANC);
    p->cases[7][3] = CreerPiece(REINE, BLANC);
    p->cases[7][4] = CreerPiece(ROI, BLANC);
    p->cases[7][5] = CreerPiece(FOU, BLANC);
    p->cases[7][6] = CreerPiece(CAVALIER, BLANC);
    p->cases[7][7] = CreerPiece(TOUR, BLANC);

    p->tourActuel = BLANC;
    p->enPassantX = -1;
    p->enPassantY = -1;
    p->historique.capacite = 50;
    p->historique.taille = 0;
    p->historique.coups = (Coup*)malloc((size_t)p->historique.capacite * sizeof(Coup));
    p->tempsBlanc = 600.0f;
    p->tempsNoir = 600.0f;
}

void FreePartie(Partie* p) {
    if (p == NULL) {
        return;
    }

    free(p->historique.coups);
    p->historique.coups = NULL;
    p->historique.capacite = 0;
    p->historique.taille = 0;
    p->enPassantX = -1;
    p->enPassantY = -1;
}
