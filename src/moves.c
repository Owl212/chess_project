#include "chess.h"
#include <stdlib.h>
#include <math.h>

void JouerCoup(Partie* p, int sx, int sy, int ex, int ey, TypePiece promotion) {
    Piece pieceDeplacee = p->cases[sy][sx];
    Piece pieceCapturee = p->cases[ey][ex];
    bool premierMouv = !pieceDeplacee.aBouge;

    if (p->historique.taille >= p->historique.capacite) {
        p->historique.capacite *= 2;
        p->historique.coups = (Coup*)realloc(p->historique.coups, sizeof(Coup) * p->historique.capacite);
    }

    Coup* c = &p->historique.coups[p->historique.taille];
    c->startX = sx; c->startY = sy;
    c->endX = ex; c->endY = ey;
    c->pieceDeplacee = pieceDeplacee;
    c->pieceCapturee = pieceCapturee;
    c->premierMouvement = premierMouv;
    p->historique.taille++;

    p->cases[ey][ex] = pieceDeplacee;
    p->cases[sy][sx].type = VIDE;
    p->cases[sy][sx].couleur = AUCUN;
    p->cases[ey][ex].aBouge = true;

    if (pieceDeplacee.type == ROI && abs(ex - sx) == 2) {
        if (ex == 6) {
            p->cases[sy][5] = p->cases[sy][7];
            p->cases[sy][7].type = VIDE;
            p->cases[sy][5].aBouge = true;
        } else if (ex == 2) {
            p->cases[sy][3] = p->cases[sy][0];
            p->cases[sy][0].type = VIDE;
            p->cases[sy][3].aBouge = true;
        }
    }

    if (pieceDeplacee.type == PION && sx != ex && pieceCapturee.type == VIDE) {
        p->cases[sy][ex].type = VIDE; 
        p->cases[sy][ex].couleur = AUCUN;
    }

    if (pieceDeplacee.type == PION && (ey == 0 || ey == 7)) {
        if (promotion != AUCUN) {
            p->cases[ey][ex].type = promotion; 
        } else {
            p->etat = ECRAN_PROMOTION;
            p->promoX = ex;
            p->promoY = ey;
            p->promoCouleur = pieceDeplacee.couleur;
            return; 
        }
    }

    if (pieceDeplacee.type == PION && abs(ey - sy) == 2) {
        p->enPassantX = sx;
        p->enPassantY = (sy + ey) / 2; 
    } else {
        p->enPassantX = -1;
        p->enPassantY = -1;
    }

    p->tourActuel = (p->tourActuel == BLANC) ? NOIR : BLANC;
}

void AnnulerDernierCoup(Partie* p) {
    if (p->historique.taille <= 0) return;

    p->historique.taille--;
    Coup c = p->historique.coups[p->historique.taille];

    p->cases[c.startY][c.startX] = c.pieceDeplacee;
    p->cases[c.endY][c.endX] = c.pieceCapturee;

    if (c.premierMouvement) {
        p->cases[c.startY][c.startX].aBouge = false;
    }

    if (c.pieceDeplacee.type == ROI && abs(c.endX - c.startX) == 2) {
        if (c.endX == 6) {
            p->cases[c.startY][7] = p->cases[c.startY][5];
            p->cases[c.startY][5].type = VIDE;
            p->cases[c.startY][7].aBouge = false;
        } else if (c.endX == 2) {
            p->cases[c.startY][0] = p->cases[c.startY][3];
            p->cases[c.startY][3].type = VIDE;
            p->cases[c.startY][0].aBouge = false;
        }
    }

    if (c.pieceDeplacee.type == PION && c.startX != c.endX && c.pieceCapturee.type == VIDE) {
        p->cases[c.startY][c.endX].type = PION;
        p->cases[c.startY][c.endX].couleur = (c.pieceDeplacee.couleur == BLANC) ? NOIR : BLANC;
        p->cases[c.startY][c.endX].aBouge = true;
    }

    if (p->historique.taille > 0) {
        Coup prec = p->historique.coups[p->historique.taille - 1];
        if (prec.pieceDeplacee.type == PION && abs(prec.endY - prec.startY) == 2) {
            p->enPassantX = prec.startX;
            p->enPassantY = (prec.startY + prec.endY) / 2;
        } else {
            p->enPassantX = -1; p->enPassantY = -1;
        }
    } else {
        p->enPassantX = -1; p->enPassantY = -1;
    }

    p->tourActuel = (p->tourActuel == BLANC) ? NOIR : BLANC;
    p->etatTakeback = PAS_DE_DEMANDE;
    p->partieTerminee = false;
    if (p->etat == ECRAN_FIN || p->etat == ECRAN_PROMOTION) p->etat = EN_JEU;
}

void DemanderTakeback(Partie* p, Couleur demandeur) {
    if (p->mode == MODE_PVP) {
        p->etatTakeback = (demandeur == BLANC) ? DEMANDE_PAR_BLANC : DEMANDE_PAR_NOIR;
    } else {
        if (p->historique.taille >= 2) {
            AnnulerDernierCoup(p);
            AnnulerDernierCoup(p);
        } else if (p->historique.taille == 1) {
            AnnulerDernierCoup(p);
        }
    }
}

void AccepterTakeback(Partie* p) {
    if (p->historique.taille >= 2) {
        AnnulerDernierCoup(p); 
        AnnulerDernierCoup(p); 
    } else if (p->historique.taille == 1) {
        AnnulerDernierCoup(p);
    }
    p->etatTakeback = PAS_DE_DEMANDE;
}

void RefuserTakeback(Partie* p) {
    p->etatTakeback = PAS_DE_DEMANDE;
}
