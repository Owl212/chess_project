const fs = require('fs');

const board_code = `#include "chess.h"
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
`;

const rules_code = `#include "chess.h"
#include <stdlib.h>
#include <math.h>

static bool CheminLibre(Partie* p, int sx, int sy, int ex, int ey) {
    int dx = ex - sx;
    int dy = ey - sy;
    int stepX = (dx > 0) ? 1 : ((dx < 0) ? -1 : 0);
    int stepY = (dy > 0) ? 1 : ((dy < 0) ? -1 : 0);

    int x = sx + stepX;
    int y = sy + stepY;

    while (x != ex || y != ey) {
        if (p->cases[y][x].type != VIDE) return false;
        x += stepX;
        y += stepY;
    }
    return true;
}

bool EstMouvementPseudoLegal(Partie* p, int sx, int sy, int ex, int ey) {
    Piece piece = p->cases[sy][sx];
    Piece cible = p->cases[ey][ex];

    if (cible.type != VIDE && cible.couleur == piece.couleur) return false;

    int dx = ex - sx;
    int dy = ey - sy;
    int absDx = abs(dx);
    int absDy = abs(dy);

    switch (piece.type) {
        case PION: {
            int dir = (piece.couleur == BLANC) ? -1 : 1;
            int startCol = (piece.couleur == BLANC) ? 6 : 1;

            if (dx == 0 && dy == dir && cible.type == VIDE) return true;
            if (dx == 0 && dy == 2 * dir && sy == startCol && cible.type == VIDE && p->cases[sy + dir][sx].type == VIDE) return true;
            if (absDx == 1 && dy == dir && cible.type != VIDE) return true;
            if (absDx == 1 && dy == dir && cible.type == VIDE && ex == p->enPassantX && ey == p->enPassantY) return true;
            return false;
        }
        case CAVALIER:
            return (absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2);
        case FOU:
            if (absDx != absDy) return false;
            return CheminLibre(p, sx, sy, ex, ey);
        case TOUR:
            if (dx != 0 && dy != 0) return false;
            return CheminLibre(p, sx, sy, ex, ey);
        case REINE:
            if (absDx != absDy && dx != 0 && dy != 0) return false;
            return CheminLibre(p, sx, sy, ex, ey);
        case ROI:
            if (absDy == 0 && absDx == 2 && !piece.aBouge) {
                int dirX = (dx > 0) ? 1 : -1;
                int rx = (dx > 0) ? 7 : 0;
                Piece tour = p->cases[sy][rx];
                if (tour.type != TOUR || tour.aBouge || tour.couleur != piece.couleur) return false;
                
                int step = sx + dirX;
                while (step != rx) {
                    if (p->cases[sy][step].type != VIDE) return false;
                    step += dirX;
                }
                return true;
            }
            return (absDx <= 1 && absDy <= 1);
        default:
            return false;
    }
}

bool EstEnEchec(Partie* p, Couleur c) {
    int roiX = -1, roiY = -1;
    Couleur adversaire = (c == BLANC) ? NOIR : BLANC;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (p->cases[y][x].type == ROI && p->cases[y][x].couleur == c) {
                roiX = x; roiY = y;
                break;
            }
        }
        if (roiX != -1) break;
    }
    
    if (roiX == -1) return false;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (p->cases[y][x].couleur == adversaire) {
                if (EstMouvementPseudoLegal(p, x, y, roiX, roiY)) return true;
            }
        }
    }
    return false;
}

bool EstMouvementLegal(Partie* p, int sx, int sy, int ex, int ey) {
    if (!EstMouvementPseudoLegal(p, sx, sy, ex, ey)) return false;

    Piece pieceSource = p->cases[sy][sx];
    Couleur c = pieceSource.couleur;

    if (pieceSource.type == ROI && abs(ex - sx) == 2) {
        if (EstEnEchec(p, c)) return false;
        int dirX = (ex - sx > 0) ? 1 : -1;
        p->cases[sy][sx + dirX] = p->cases[sy][sx];
        p->cases[sy][sx].type = VIDE;
        p->cases[sy][sx].couleur = AUCUN;
        bool traverseEchec = EstEnEchec(p, c);
        p->cases[sy][sx] = p->cases[sy][sx + dirX];
        p->cases[sy][sx + dirX].type = VIDE;
        p->cases[sy][sx + dirX].couleur = AUCUN;

        if (traverseEchec) return false;
    }

    Piece pieceDest = p->cases[ey][ex];
    p->cases[ey][ex] = pieceSource;
    p->cases[sy][sx].type = VIDE;
    p->cases[sy][sx].couleur = AUCUN;

    bool enEchecApresCoup = EstEnEchec(p, c);

    p->cases[sy][sx] = pieceSource;
    p->cases[ey][ex] = pieceDest;

    return !enEchecApresCoup;
}

int GenererCoupsLegaux(Partie* p, Couleur couleur, Coup* coupsLegaux) {
    int count = 0;
    for (int sy = 0; sy < 8; sy++) {
        for (int sx = 0; sx < 8; sx++) {
            if (p->cases[sy][sx].couleur == couleur) {
                for (int ey = 0; ey < 8; ey++) {
                    for (int ex = 0; ex < 8; ex++) {
                        if (EstMouvementLegal(p, sx, sy, ex, ey)) {
                            coupsLegaux[count].startX = sx;
                            coupsLegaux[count].startY = sy;
                            coupsLegaux[count].endX = ex;
                            coupsLegaux[count].endY = ey;
                            count++;
                        }
                    }
                }
            }
        }
    }
    return count;
}

bool EstEchecEtMat(Partie* p, Couleur c) {
    Coup buffer[256];
    return EstEnEchec(p, c) && (GenererCoupsLegaux(p, c, buffer) == 0);
}

bool EstPat(Partie* p, Couleur c) {
    Coup buffer[256];
    return !EstEnEchec(p, c) && (GenererCoupsLegaux(p, c, buffer) == 0);
}
`;

const moves_code = `#include "chess.h"
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
`;

const ai_code = `#include "chess.h"
#include <stdlib.h>
#include <limits.h>
#include <time.h>

static const int VALEUR_PIECE[] = { 0, 100, 320, 330, 500, 900, 20000 };

static const int PION_PST[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    { 50, 50, 50, 50, 50, 50, 50, 50},
    { 10, 10, 20, 30, 30, 20, 10, 10},
    {  5,  5, 10, 25, 25, 10,  5,  5},
    {  0,  0,  0, 20, 20,  0,  0,  0},
    {  5, -5,-10,  0,  0,-10, -5,  5},
    {  5, 10, 10,-20,-20, 10, 10,  5},
    {  0,  0,  0,  0,  0,  0,  0,  0}
};

static const int CAVALIER_PST[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

int EvaluerPlateau(Partie* p, bool utiliserPST) {
    int score = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece pc = p->cases[y][x];
            if (pc.type != VIDE) {
                int val = VALEUR_PIECE[pc.type];
                
                if (utiliserPST) {
                    int pstY = (pc.couleur == BLANC) ? y : (7 - y);
                    if (pc.type == PION) val += PION_PST[pstY][x];
                    else if (pc.type == CAVALIER) val += CAVALIER_PST[pstY][x];
                }

                if (pc.couleur == BLANC) score += val;
                else score -= val;
            }
        }
    }
    return score; 
}

int ScoreCoup(Partie* p, Coup c) {
    int guess = 0;
    if (c.pieceCapturee.type != VIDE) {
        guess = 10 * VALEUR_PIECE[c.pieceCapturee.type] - VALEUR_PIECE[c.pieceDeplacee.type];
    }
    if (c.pieceDeplacee.type == PION && (c.endY == 0 || c.endY == 7)) {
        guess += 900; 
    }
    return guess;
}

void TrierCoups(Partie* p, Coup* coups, int nombre) {
    int scores[256];
    for (int i = 0; i < nombre; i++) scores[i] = ScoreCoup(p, coups[i]);
    
    for (int i = 0; i < nombre - 1; i++) {
        for (int j = i + 1; j < nombre; j++) {
            if (scores[j] > scores[i]) {
                int tempS = scores[i]; scores[i] = scores[j]; scores[j] = tempS;
                Coup tempC = coups[i]; coups[i] = coups[j]; coups[j] = tempC;
            }
        }
    }
}

int AlphaBeta(Partie* p, int profondeur, int alpha, int beta, bool maximisant, bool usePST, bool useOrder) {
    if (profondeur == 0 || p->partieTerminee) {
        return EvaluerPlateau(p, usePST);
    }

    Coup coups[256];
    int nbCoups = GenererCoupsLegaux(p, p->tourActuel, coups);

    if (nbCoups == 0) {
        if (EstEnEchec(p, p->tourActuel)) return maximisant ? -99999 : 99999;
        return 0; 
    }

    if (useOrder) TrierCoups(p, coups, nbCoups);

    Partie tempPartie;
    if (maximisant) {
        int maxEval = -INT_MAX;
        for (int i = 0; i < nbCoups; i++) {
            CopierPartie(p, &tempPartie);
            JouerCoup(&tempPartie, coups[i].startX, coups[i].startY, coups[i].endX, coups[i].endY, REINE);
            
            int eval = AlphaBeta(&tempPartie, profondeur - 1, alpha, beta, false, usePST, useOrder);
            NettoyerPartie(&tempPartie); 

            if (eval > maxEval) maxEval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break; 
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int i = 0; i < nbCoups; i++) {
            CopierPartie(p, &tempPartie);
            JouerCoup(&tempPartie, coups[i].startX, coups[i].startY, coups[i].endX, coups[i].endY, REINE);
            
            int eval = AlphaBeta(&tempPartie, profondeur - 1, alpha, beta, true, usePST, useOrder);
            NettoyerPartie(&tempPartie);

            if (eval < minEval) minEval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break; 
        }
        return minEval;
    }
}

Coup TrouverMeilleurCoupBot(Partie* p) {
    Coup coupsLegaux[256];
    int nbCoups = GenererCoupsLegaux(p, p->tourActuel, coupsLegaux);

    Coup coupInvalide = {-1,-1,-1,-1, {VIDE, AUCUN, false}, {VIDE, AUCUN, false}, false};
    if (nbCoups == 0) return coupInvalide;

    if (p->niveauBot == 1) {
        srand(time(NULL));
        return coupsLegaux[rand() % nbCoups];
    }

    int profondeur = 1;
    bool usePST = false;
    bool useOrder = false;

    switch (p->niveauBot) {
        case 2: profondeur = 1; break; 
        case 3: profondeur = 2; break; 
        case 4: profondeur = 3; break; 
        case 5: profondeur = 4; usePST = true; break; 
        case 6: profondeur = 4; usePST = true; useOrder = true; break; 
        case 7: profondeur = 5; usePST = true; useOrder = true; break; 
        case 8: profondeur = 6; usePST = true; useOrder = true; break; 
    }

    bool maximisant = (p->tourActuel == BLANC);
    int meilleurScore = maximisant ? -INT_MAX : INT_MAX;
    Coup meilleurCoup = coupsLegaux[0];

    if (useOrder) TrierCoups(p, coupsLegaux, nbCoups);

    Partie tempPartie;
    for (int i = 0; i < nbCoups; i++) {
        CopierPartie(p, &tempPartie);
        JouerCoup(&tempPartie, coupsLegaux[i].startX, coupsLegaux[i].startY, coupsLegaux[i].endX, coupsLegaux[i].endY, REINE);
        
        int eval = AlphaBeta(&tempPartie, profondeur - 1, -INT_MAX, INT_MAX, !maximisant, usePST, useOrder);
        NettoyerPartie(&tempPartie);

        if (maximisant && eval > meilleurScore) {
            meilleurScore = eval;
            meilleurCoup = coupsLegaux[i];
        } else if (!maximisant && eval < meilleurScore) {
            meilleurScore = eval;
            meilleurCoup = coupsLegaux[i];
        }
    }

    return meilleurCoup;
}

void JouerCoupBot(Partie* p) {
    Coup c = TrouverMeilleurCoupBot(p);
    if (c.startX != -1) {
        JouerCoup(p, c.startX, c.startY, c.endX, c.endY, REINE);
    }
}

void DemanderHint(Partie* p) {
    int sauvegardeNiveau = p->niveauBot;
    p->niveauBot = 6; 
    
    Coup c = TrouverMeilleurCoupBot(p);
    
    p->niveauBot = sauvegardeNiveau;

    if (c.startX != -1) {
        p->meilleurCoupHint = c;
        p->afficherHint = true;
        p->tempsHintRestant = 3.0f; 
    }
}
`;

const ui_code = `#include "chess.h"
#include <stdio.h>

#define COULEUR_CLAIRE (Color){ 240, 217, 181, 255 }
#define COULEUR_FONCEE (Color){ 181, 136, 99, 255 }
#define COULEUR_SELECT (Color){ 130, 151, 105, 255 }
#define FOND_UI        (Color){ 38, 36, 33, 255 }

static int selectionMode = 0;
static int selectionNiveauBot = 3; 

void ChargerAssets(UIAssets* assets) {
    int codepoints[600];
    int count = 0;
    for (int i = 32; i < 127; i++) codepoints[count++] = i;
    assets->fontMenu = LoadFontEx("assets/chess_font.ttf", 80, codepoints, count);

    for (int c = 0; c < 3; c++) {
        for (int t = 0; t < 7; t++) {
            assets->texPieces[c][t].id = 0;
        }
    }

    assets->texPieces[BLANC][PION]     = LoadTexture("assets/pieces/wP.png");   
    assets->texPieces[BLANC][CAVALIER] = LoadTexture("assets/pieces/wN.png");   
    assets->texPieces[BLANC][FOU]      = LoadTexture("assets/pieces/wB.png");   
    assets->texPieces[BLANC][TOUR]     = LoadTexture("assets/pieces/wR.png");   
    assets->texPieces[BLANC][REINE]    = LoadTexture("assets/pieces/wQ.png");   
    assets->texPieces[BLANC][ROI]      = LoadTexture("assets/pieces/wK.png");   

    assets->texPieces[NOIR][PION]      = LoadTexture("assets/pieces/bP.png");   
    assets->texPieces[NOIR][CAVALIER]  = LoadTexture("assets/pieces/bN.png");   
    assets->texPieces[NOIR][FOU]       = LoadTexture("assets/pieces/bB.png");
    assets->texPieces[NOIR][TOUR]      = LoadTexture("assets/pieces/bR.png");
    assets->texPieces[NOIR][REINE]     = LoadTexture("assets/pieces/bQ.png");
    assets->texPieces[NOIR][ROI]       = LoadTexture("assets/pieces/bK.png");
}

void DechargerAssets(UIAssets* assets) {
    UnloadFont(assets->fontMenu);
    for (int c = 0; c < 3; c++) {
        for (int t = 0; t < 7; t++) {
            if (assets->texPieces[c][t].id != 0) {
                UnloadTexture(assets->texPieces[c][t]);
            }
        }
    }
}

void DessinerTimer(float tempsRestant, int posX, int posY, bool estBlanc, bool estSonTour) {
    if (tempsRestant > 999000.0f) { 
        DrawRectangleRounded((Rectangle){ (float)posX, (float)posY, 120, 50 }, 0.2f, 10, DARKGRAY);
        DrawText("  8  ", posX + 20, posY + 10, 30, WHITE); 
        return;
    }

    int minutes = (int)tempsRestant / 60;
    int secondes = (int)tempsRestant % 60;

    char texteTimer[10];
    sprintf(texteTimer, "%02d:%02d", minutes, secondes);

    Color fond = estBlanc ? RAYWHITE : DARKGRAY;
    Color texteCol = estBlanc ? BLACK : WHITE;

    if (tempsRestant > 0.0f && tempsRestant <= 10.0f) {
        fond = (Color){ 200, 50, 50, 255 };
        texteCol = WHITE;
    }

    if (estSonTour) fond = estBlanc ? (Color){ 255, 255, 230, 255 } : (Color){ 100, 100, 100, 255 };

    DrawRectangleRounded((Rectangle){ (float)posX, (float)posY, 120, 50 }, 0.2f, 10, fond);
    
    int largeurTexte = MeasureText(texteTimer, 30);
    DrawText(texteTimer, posX + (120 - largeurTexte) / 2, posY + 10, 30, texteCol);
}

void DessinerMenuPrincipal(Partie* p) {
    ClearBackground(FOND_UI);
    int screenW = GetScreenWidth();

    DrawText("Lichess Grand-Master C", screenW / 2 - 220, 30, 40, GOLD);

    DrawText("MODE DE JEU :", 50, 120, 20, LIGHTGRAY);
    const char* modes[3] = {"Joueur vs Joueur", "Joueur (Blancs) vs Ordinateur", "Ordinateur vs Joueur (Noirs)"};
    for (int i = 0; i < 3; i++) {
        Rectangle btn = { 50, 150 + i * 45, 280, 35 };
        Color bcol = (selectionMode == i) ? GREEN : DARKGRAY;
        DrawRectangleRec(btn, bcol);
        DrawText(modes[i], 60, 158, 18, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btn)) {
            selectionMode = i;
            if (i == 0) p->mode = MODE_PVP;
            else if (i == 1) p->mode = MODE_PVE_BLANC;
            else p->mode = MODE_PVE_NOIR;
        }
    }

    if (selectionMode != 0) {
        DrawText("NIVEAU DU BOT (1-8) :", 400, 120, 20, LIGHTGRAY);
        for (int i = 1; i <= 8; i++) {
            Rectangle btn = { 400 + ((i-1)%4) * 65, 150 + ((i-1)/4) * 45, 55, 35 };
            Color bcol = (selectionNiveauBot == i) ? ORANGE : DARKGRAY;
            DrawRectangleRec(btn, bcol);
            DrawText(TextFormat("%d", i), 400 + ((i-1)%4) * 65 + 20, 158 + ((i-1)/4) * 45, 20, WHITE);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btn)) {
                selectionNiveauBot = i;
                p->niveauBot = i;
            }
        }
    }

    DrawText("LANCER LA PARTIE (CADENCE) :", screenW / 2 - 150, 280, 20, LIGHTGRAY);
    int btnX = screenW / 2 - 150;
    int btnY = 320;

    struct { char* label; int t; Color c;} cadences[] = {
        {"Bullet (1 min)", 60, (Color){200, 60, 60, 255}},
        {"Blitz (3 min)", 180, (Color){220, 110, 40, 255}},
        {"Rapide (10 min)", 600, (Color){80, 160, 200, 255}},
        {"Classique (1h)", 3600, (Color){50, 180, 100, 255}},
        {"Sans Limite", 0, (Color){100, 100, 100, 255}}
    };

    for (int i = 0; i < 5; i++) {
        Rectangle btn = { btnX, btnY + i * 55, 300, 45 };
        DrawRectangleRec(btn, cadences[i].c);
        DrawText(cadences[i].label, btnX + 70, btnY + i * 55 + 12, 20, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btn)) {
            p->tempsInitial = cadences[i].t;
            p->tempsBlanc = (p->tempsInitial == 0) ? 999999.0f : (float)p->tempsInitial;
            p->tempsNoir = (p->tempsInitial == 0) ? 999999.0f : (float)p->tempsInitial;
            p->etat = EN_JEU;
        }
    }
}

void DessinerPlateau(Partie* p, int caseSelX, int caseSelY, UIAssets* assets) {
    int tailleCase = 80;
    int decalageX = 20;
    int decalageY = 20;

    ClearBackground(FOND_UI);

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Color couleurCase = ((x + y) % 2 == 0) ? COULEUR_CLAIRE : COULEUR_FONCEE;
            if (x == caseSelX && y == caseSelY) couleurCase = COULEUR_SELECT;

            if (p->afficherHint && ((x == p->meilleurCoupHint.startX && y == p->meilleurCoupHint.startY) || 
                                    (x == p->meilleurCoupHint.endX && y == p->meilleurCoupHint.endY))) {
                couleurCase = (Color){ 100, 255, 100, 200 };
            }

            int posX = decalageX + x * tailleCase;
            int posY = decalageY + y * tailleCase;

            DrawRectangle(posX, posY, tailleCase, tailleCase, couleurCase);

            Piece piece = p->cases[y][x];
            if (piece.type != VIDE) {
                Texture2D tex = assets->texPieces[piece.couleur][piece.type];
                if (tex.id != 0) {
                    DrawTexturePro(tex, 
                        (Rectangle){0, 0, tex.width, tex.height}, 
                        (Rectangle){posX, posY, tailleCase, tailleCase}, 
                        (Vector2){0,0}, 0.0f, WHITE);
                }
            }
        }
    }

    int panneauX = decalageX + 8 * tailleCase + 20;
    
    DrawText("NOIR", panneauX + 78, decalageY + 10, 18, LIGHTGRAY);
    DessinerTimer(p->tempsNoir, panneauX + 40, decalageY + 35, false, p->tourActuel == NOIR && !p->partieTerminee);
    
    DrawText("BLANC", panneauX + 72, decalageY + 8 * tailleCase - 90, 18, LIGHTGRAY);
    DessinerTimer(p->tempsBlanc, panneauX + 40, decalageY + 8 * tailleCase - 65, true, p->tourActuel == BLANC && !p->partieTerminee);

    DrawText(TextFormat("TourActuel: %s", (p->tourActuel==BLANC)?"BLANCS":"NOIRS"), panneauX, decalageY + 250, 20, GOLD);

    if (p->mode != MODE_PVP) {
        DrawText(TextFormat("Mode: Vs BOT (Niv.%d)", p->niveauBot), panneauX, decalageY + 300, 18, ORANGE);
    }

    DrawRectangleRec((Rectangle){panneauX, decalageY + 400, 200, 40}, LIGHTGRAY);
    DrawText("Prendre Takeback (T)", panneauX + 15, decalageY + 410, 17, BLACK);

    DrawRectangleRec((Rectangle){panneauX, decalageY + 460, 200, 40}, GREEN);
    DrawText("Hint 1 Coup (H)", panneauX + 35, decalageY + 470, 17, BLACK);
    
    Rectangle btnRejouer = { panneauX, decalageY + 520, 200, 40 };
    DrawRectangleRec(btnRejouer, (Color){100, 150, 250, 255}); 
    DrawRectangleLinesEx(btnRejouer, 2, DARKBLUE);
    DrawText("Rejouer (Menu)", panneauX + 35, decalageY + 532, 17, WHITE);       

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnRejouer)) {
        InitPartie(p); 
    }

    if (p->partieTerminee) {
        DrawText("PARTIE TERMINEE !", panneauX, decalageY + 580, 20, RED);
    }
}

void DessinerMenuPromotion(Partie* p) {
    if (p->etat != ECRAN_PROMOTION) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 180 });
    
    int w = 400, h = 150;
    int rx = GetScreenWidth()/2 - w/2;
    int ry = GetScreenHeight()/2 - h/2;
    
    DrawRectangle(rx, ry, w, h, RAYWHITE);
    DrawRectangleLines(rx, ry, w, h, BLACK);
    DrawText("Choisissez la promotion :", rx + 75, ry + 20, 20, BLACK);

    const char* labels[] = {"Reine (1)", "Tour (2)", "Fou (3)", "Cavalier (4)"};
    Color cols[] = { GOLD, LIGHTGRAY, LIGHTGRAY, LIGHTGRAY };
    
    for (int i = 0; i < 4; i++) {
        Rectangle btn = { rx + 20 + i*90, ry + 80, 80, 40 };
        DrawRectangleRec(btn, cols[i]);
        DrawRectangleLinesEx(btn, 1, BLACK);
        DrawText(labels[i], rx + 25 + i*90, ry + 90, 12, BLACK);
    }
}

void DessinerMenuTakeback(Partie* p) {
    if (p->etatTakeback == PAS_DE_DEMANDE || p->mode != MODE_PVP) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 150 });
    DrawRectangle(200, 250, 450, 150, RAYWHITE);
    const char* d = (p->etatTakeback == DEMANDE_PAR_BLANC) ? "BLANC" : "NOIR";
    DrawText(TextFormat("%s demande un takeback. Accepter ?", d), 220, 270, 18, BLACK);
    
    DrawRectangle(280, 340, 100, 40, GREEN);DrawText("OUI (Y)", 295, 350, 20, WHITE);
    DrawRectangle(470, 340, 100, 40, RED);  DrawText("NON (N)", 485, 350, 20, WHITE);
}
`;

const main_code = `#include "raylib.h"
#include "chess.h"

#define LARGEUR_FENETRE 950
#define HAUTEUR_FENETRE 700

void GererPromotion(Partie* p) {
    TypePiece choix = AUCUN;
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) choix = REINE;
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) choix = TOUR;
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) choix = FOU;
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) choix = CAVALIER;

    if (choix != AUCUN) {
        p->cases[p->promoY][p->promoX].type = choix;
        
        p->enPassantX = -1; p->enPassantY = -1;
        p->tourActuel = (p->tourActuel == BLANC) ? NOIR : BLANC;
        p->etat = EN_JEU; 
        
        if (EstEchecEtMat(p, p->tourActuel) || EstPat(p, p->tourActuel)) {
            p->partieTerminee = true;
        }
    }
}

void GererInput(Partie* p, int* cx, int* cy) {
    if (p->afficherHint) {
        p->tempsHintRestant -= GetFrameTime();
        if (p->tempsHintRestant <= 0.0f) p->afficherHint = false;
    }

    if (p->etatTakeback != PAS_DE_DEMANDE) {
        if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_O)) { AccepterTakeback(p); *cx = -1; *cy = -1; return; }
        if (IsKeyPressed(KEY_N)) { RefuserTakeback(p); return; }
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 pos = GetMousePosition();
            if (CheckCollisionPointRec(pos, (Rectangle){ 280, 340, 100, 40 })) { AccepterTakeback(p); *cx = -1; *cy = -1; }
            if (CheckCollisionPointRec(pos, (Rectangle){ 470, 340, 100, 40 })) { RefuserTakeback(p); }
        }
        return; 
    }

    if (p->partieTerminee) return;

    if (IsKeyPressed(KEY_T) && p->historique.taille > 0) {
        DemanderTakeback(p, p->tourActuel);
        return;
    }
    if (IsKeyPressed(KEY_H) && p->historique.taille >= 0) { 
        DemanderHint(p);
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    
    Vector2 pos = GetMousePosition();
    int pX = 20 + 8 * 80 + 20; 
    
    if (CheckCollisionPointRec(pos, (Rectangle){ pX, 400, 200, 40 }) && p->historique.taille > 0) {
        DemanderTakeback(p, p->tourActuel); return;
    }
    if (CheckCollisionPointRec(pos, (Rectangle){ pX, 460, 200, 40 })) { 
        DemanderHint(p); return;
    }

    int gx = (pos.x - 20) / 80;
    int gy = (pos.y - 20) / 80;

    if (gx >= 0 && gx < 8 && gy >= 0 && gy < 8) {
        if (*cx == -1) {
            if (p->cases[gy][gx].type != VIDE && p->cases[gy][gx].couleur == p->tourActuel) {
                *cx = gx; *cy = gy;
            }
        } 
        else { 
            if (gx == *cx && gy == *cy) {
                *cx = -1; *cy = -1; 
            } else if (p->cases[gy][gx].type != VIDE && p->cases[gy][gx].couleur == p->tourActuel) {
                *cx = gx; *cy = gy; 
            } else {
                if (EstMouvementLegal(p, *cx, *cy, gx, gy)) {
                    JouerCoup(p, *cx, *cy, gx, gy, AUCUN);
                    p->afficherHint = false;
                    *cx = -1; *cy = -1;
                    
                    if (EstEchecEtMat(p, p->tourActuel) || EstPat(p, p->tourActuel)) {
                        p->partieTerminee = true;
                    }
                } else {
                    *cx = -1; *cy = -1; 
                }
            }
        }
    }
}

int main(void) {
    InitWindow(LARGEUR_FENETRE, HAUTEUR_FENETRE, "Lichess Grand-Master C - Minimax Alpha-Beta");
    SetTargetFPS(60);

    Partie partie;
    InitPartie(&partie);

    int caseSelX = -1;
    int caseSelY = -1;

    UIAssets assets;
    ChargerAssets(&assets);

    while (!WindowShouldClose()) {
        if (partie.etat == ECRAN_PROMOTION) {
            GererPromotion(&partie);
        }
        else if (partie.etat == EN_JEU && !partie.partieTerminee && partie.etatTakeback == PAS_DE_DEMANDE) {
            
            if (partie.historique.taille > 0 && partie.tempsInitial > 0) {
                if (partie.tourActuel == BLANC) partie.tempsBlanc -= GetFrameTime();
                else partie.tempsNoir -= GetFrameTime();
                
                if (partie.tempsBlanc <= 0.0f) { partie.tempsBlanc = 0.0f; partie.partieTerminee = true; }
                if (partie.tempsNoir <= 0.0f) { partie.tempsNoir = 0.0f; partie.partieTerminee = true; }
            }

            bool botDoitJouer = false;
            if (partie.mode == MODE_PVE_BLANC && partie.tourActuel == NOIR) botDoitJouer = true;
            if (partie.mode == MODE_PVE_NOIR && partie.tourActuel == BLANC) botDoitJouer = true;

            if (botDoitJouer) {
                JouerCoupBot(&partie);
                if (EstEchecEtMat(&partie, partie.tourActuel) || EstPat(&partie, partie.tourActuel)) {
                    partie.partieTerminee = true;
                }
            } else {
                GererInput(&partie, &caseSelX, &caseSelY);
            }
            
        } 
        else if (partie.etat == EN_JEU && partie.etatTakeback != PAS_DE_DEMANDE) {
            GererInput(&partie, &caseSelX, &caseSelY); 
        }

        BeginDrawing();
        
        if (partie.etat == MENU_PRINCIPAL) {
            DessinerMenuPrincipal(&partie); 
        } else {
            DessinerPlateau(&partie, caseSelX, caseSelY, &assets);
            
            if (partie.etatTakeback != PAS_DE_DEMANDE) {
                DessinerMenuTakeback(&partie);
            } else if (partie.etat == ECRAN_PROMOTION) {
                DessinerMenuPromotion(&partie);
            }
        }
        
        EndDrawing();
    }

    DechargerAssets(&assets);
    NettoyerPartie(&partie); 
    CloseWindow();

    return 0;
}
`;

fs.writeFileSync('src/board.c', board_code);
fs.writeFileSync('src/rules.c', rules_code);
fs.writeFileSync('src/moves.c', moves_code);
fs.writeFileSync('src/ai.c', ai_code);
fs.writeFileSync('src/ui.c', ui_code);
fs.writeFileSync('src/main.c', main_code);

console.log("All files updated successfully!");
