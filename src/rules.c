#include "chess.h"
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
    const int maxCoups = 256;
    for (int sy = 0; sy < 8; sy++) {
        for (int sx = 0; sx < 8; sx++) {
            if (p->cases[sy][sx].couleur == couleur) {
                for (int ey = 0; ey < 8; ey++) {
                    for (int ex = 0; ex < 8; ex++) {
                        if (EstMouvementLegal(p, sx, sy, ex, ey)) {
                            if (count >= maxCoups) {
                                return count;
                            }
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
