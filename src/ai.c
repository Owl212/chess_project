#include "chess.h"
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
    if (p == NULL || p->etat != EN_JEU || p->partieTerminee) {
        return;
    }

    bool tourBot = false;
    if (p->mode == MODE_PVE_BLANC && p->tourActuel == NOIR) tourBot = true;
    if (p->mode == MODE_PVE_NOIR && p->tourActuel == BLANC) tourBot = true;
    if (tourBot) {
        return;
    }

    int sauvegardeNiveau = p->niveauBot;
    p->niveauBot = 4;
    
    Coup c = TrouverMeilleurCoupBot(p);
    
    p->niveauBot = sauvegardeNiveau;

    if (c.startX != -1) {
        p->meilleurCoupHint = c;
        p->afficherHint = true;
        p->tempsHintRestant = 3.0f; 
    }
}
