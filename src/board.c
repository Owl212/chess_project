#include "chess.h"
#include <stdlib.h>
#include <string.h>

Piece CreerPiece(TypePiece type, Couleur couleur) {
    Piece p;
    p.type = type;
    p.couleur = couleur;
    p.aBouge = false;
    return p;
}

void NettoyerPartie(Partie* p) {
    if (p->historique.coups != NULL) {
        free(p->historique.coups);
        p->historique.coups = NULL;
    }
    p->historique.taille = 0;
    p->historique.capacite = 0;
}

void InitPartie(Partie* p) {
    NettoyerPartie(p);
    p->historique.capacite = 256;
    p->historique.coups = (Coup*)malloc(sizeof(Coup) * p->historique.capacite);
    p->historique.taille = 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            p->cases[y][x] = CreerPiece(VIDE, AUCUN);
        }
    }

    p->cases[0][0] = CreerPiece(TOUR, NOIR);
    p->cases[0][1] = CreerPiece(CAVALIER, NOIR);
    p->cases[0][2] = CreerPiece(FOU, NOIR);
    p->cases[0][3] = CreerPiece(REINE, NOIR);
    p->cases[0][4] = CreerPiece(ROI, NOIR);
    p->cases[0][5] = CreerPiece(FOU, NOIR);
    p->cases[0][6] = CreerPiece(CAVALIER, NOIR);
    p->cases[0][7] = CreerPiece(TOUR, NOIR);
    for(int x = 0; x < 8; x++) p->cases[1][x] = CreerPiece(PION, NOIR);

    for(int x = 0; x < 8; x++) p->cases[6][x] = CreerPiece(PION, BLANC);
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

    p->tempsBlanc = 300.0f;
    p->tempsNoir = 300.0f;
    p->tempsInitial = 300; 
    p->etat = MENU_PRINCIPAL;
    p->mode = MODE_PVP;
    p->niveauBot = 3;
    
    p->etatTakeback = PAS_DE_DEMANDE;
    p->partieTerminee = false;
    p->afficherHint = false;
    p->tempsHintRestant = 0.0f;
    p->meilleurCoupHint.startX = -1;
}

void CopierPartie(Partie* src, Partie* dest) {
    *dest = *src; 
    if (src->historique.capacite > 0 && src->historique.coups != NULL) {
        dest->historique.coups = (Coup*)malloc(sizeof(Coup) * src->historique.capacite);
        memcpy(dest->historique.coups, src->historique.coups, sizeof(Coup) * src->historique.taille);
    } else {
        dest->historique.coups = NULL;
    }
}
