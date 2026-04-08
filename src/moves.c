#include "chess.h"

#include <stdlib.h>

static bool DansPlateau(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

static int Signe(int valeur) {
    if (valeur > 0) {
        return 1;
    }
    if (valeur < 0) {
        return -1;
    }
    return 0;
}

static Piece PieceVide(void) {
    return (Piece){ VIDE, AUCUNE, false };
}

static bool PieceAdverse(Piece a, Couleur couleur) {
    return a.type != VIDE && a.couleur != couleur && a.couleur != AUCUNE;
}

static bool CheminLibre(Partie* p, int sx, int sy, int ex, int ey) {
    int pasX = Signe(ex - sx);
    int pasY = Signe(ey - sy);
    int x = sx + pasX;
    int y = sy + pasY;

    while (x != ex || y != ey) {
        if (p->cases[y][x].type != VIDE) {
            return false;
        }
        x += pasX;
        y += pasY;
    }

    return true;
}

static bool CaseMenaceeParCouleur(Partie* p, int x, int y, Couleur couleurAttaquante) {
    for (int sy = 0; sy < 8; sy++) {
        for (int sx = 0; sx < 8; sx++) {
            Piece piece = p->cases[sy][sx];
            if (piece.type == VIDE || piece.couleur != couleurAttaquante) {
                continue;
            }

            int dx = x - sx;
            int dy = y - sy;

            switch (piece.type) {
                case PION: {
                    int direction = (piece.couleur == BLANC) ? -1 : 1;
                    if (dy == direction && (dx == -1 || dx == 1)) {
                        return true;
                    }
                    break;
                }
                case CAVALIER:
                    if ((abs(dx) == 1 && abs(dy) == 2) || (abs(dx) == 2 && abs(dy) == 1)) {
                        return true;
                    }
                    break;
                case FOU:
                    if (abs(dx) == abs(dy) && CheminLibre(p, sx, sy, x, y)) {
                        return true;
                    }
                    break;
                case TOUR:
                    if ((dx == 0 || dy == 0) && CheminLibre(p, sx, sy, x, y)) {
                        return true;
                    }
                    break;
                case REINE:
                    if (((abs(dx) == abs(dy)) || dx == 0 || dy == 0) && CheminLibre(p, sx, sy, x, y)) {
                        return true;
                    }
                    break;
                case ROI:
                    if (abs(dx) <= 1 && abs(dy) <= 1) {
                        return true;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return false;
}

static bool MouvementRoquePossible(Partie* p, Piece roi, int sx, int sy, int ex, int ey) {
    if (roi.type != ROI || roi.aBouge || sy != ey) {
        return false;
    }

    if (roi.couleur == BLANC && sy == 7 && sx == 4 && ey == 7 && (ex == 6 || ex == 2)) {
        if (EstEnEchec(p, BLANC)) {
            return false;
        }

        if (ex == 6) {
            Piece tour = p->cases[7][7];
            if (tour.type != TOUR || tour.couleur != BLANC || tour.aBouge) {
                return false;
            }
            if (p->cases[7][5].type != VIDE || p->cases[7][6].type != VIDE) {
                return false;
            }
            return !CaseMenaceeParCouleur(p, 5, 7, NOIR) && !CaseMenaceeParCouleur(p, 6, 7, NOIR);
        }

        if (ex == 2) {
            Piece tour = p->cases[7][0];
            if (tour.type != TOUR || tour.couleur != BLANC || tour.aBouge) {
                return false;
            }
            if (p->cases[7][1].type != VIDE || p->cases[7][2].type != VIDE || p->cases[7][3].type != VIDE) {
                return false;
            }
            return !CaseMenaceeParCouleur(p, 3, 7, NOIR) && !CaseMenaceeParCouleur(p, 2, 7, NOIR);
        }
    }

    if (roi.couleur == NOIR && sy == 0 && sx == 4 && ey == 0 && (ex == 6 || ex == 2)) {
        if (EstEnEchec(p, NOIR)) {
            return false;
        }

        if (ex == 6) {
            Piece tour = p->cases[0][7];
            if (tour.type != TOUR || tour.couleur != NOIR || tour.aBouge) {
                return false;
            }
            if (p->cases[0][5].type != VIDE || p->cases[0][6].type != VIDE) {
                return false;
            }
            return !CaseMenaceeParCouleur(p, 5, 0, BLANC) && !CaseMenaceeParCouleur(p, 6, 0, BLANC);
        }

        if (ex == 2) {
            Piece tour = p->cases[0][0];
            if (tour.type != TOUR || tour.couleur != NOIR || tour.aBouge) {
                return false;
            }
            if (p->cases[0][1].type != VIDE || p->cases[0][2].type != VIDE || p->cases[0][3].type != VIDE) {
                return false;
            }
            return !CaseMenaceeParCouleur(p, 3, 0, BLANC) && !CaseMenaceeParCouleur(p, 2, 0, BLANC);
        }
    }

    return false;
}

bool EstMouvementPseudoLegal(Partie* p, int sx, int sy, int ex, int ey) {
    if (p == NULL || !DansPlateau(sx, sy) || !DansPlateau(ex, ey) || (sx == ex && sy == ey)) {
        return false;
    }

    Piece piece = p->cases[sy][sx];
    Piece destination = p->cases[ey][ex];

    if (piece.type == VIDE || piece.couleur == AUCUNE) {
        return false;
    }

    if (destination.type != VIDE && destination.couleur == piece.couleur) {
        return false;
    }

    int dx = ex - sx;
    int dy = ey - sy;

    switch (piece.type) {
        case PION: {
            int direction = (piece.couleur == BLANC) ? -1 : 1;
            int ligneDepart = (piece.couleur == BLANC) ? 6 : 1;

            if (dx == 0 && dy == direction && destination.type == VIDE) {
                return true;
            }

            if (dx == 0 && dy == 2 * direction && sy == ligneDepart && destination.type == VIDE) {
                int caseIntermediaireY = sy + direction;
                if (p->cases[caseIntermediaireY][sx].type == VIDE) {
                    return true;
                }
            }

            if (abs(dx) == 1 && dy == direction) {
                if (PieceAdverse(destination, piece.couleur)) {
                    return true;
                }

                if (p->enPassantX == ex && p->enPassantY == ey) {
                    int pionCaptureY = sy;
                    if (DansPlateau(ex, pionCaptureY)) {
                        Piece pionCapture = p->cases[pionCaptureY][ex];
                        if (pionCapture.type == PION && pionCapture.couleur != piece.couleur) {
                            return true;
                        }
                    }
                }
            }

            return false;
        }
        case CAVALIER:
            return (abs(dx) == 1 && abs(dy) == 2) || (abs(dx) == 2 && abs(dy) == 1);
        case FOU:
            return abs(dx) == abs(dy) && CheminLibre(p, sx, sy, ex, ey);
        case TOUR:
            return (dx == 0 || dy == 0) && CheminLibre(p, sx, sy, ex, ey);
        case REINE:
            return ((abs(dx) == abs(dy)) || dx == 0 || dy == 0) && CheminLibre(p, sx, sy, ex, ey);
        case ROI:
            if (abs(dx) <= 1 && abs(dy) <= 1) {
                return true;
            }
            return MouvementRoquePossible(p, piece, sx, sy, ex, ey);
        default:
            return false;
    }
}

bool EstEnEchec(Partie* p, Couleur c) {
    if (p == NULL || c == AUCUNE) {
        return false;
    }

    int roiX = -1;
    int roiY = -1;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece piece = p->cases[y][x];
            if (piece.type == ROI && piece.couleur == c) {
                roiX = x;
                roiY = y;
                break;
            }
        }
        if (roiX != -1) {
            break;
        }
    }

    if (roiX == -1) {
        return false;
    }

    Couleur adversaire = (c == BLANC) ? NOIR : BLANC;
    return CaseMenaceeParCouleur(p, roiX, roiY, adversaire);
}

bool EstMouvementLegal(Partie* p, int sx, int sy, int ex, int ey) {
    if (!EstMouvementPseudoLegal(p, sx, sy, ex, ey)) {
        return false;
    }

    Piece piece = p->cases[sy][sx];
    Piece destination = p->cases[ey][ex];
    Piece pieceCapturee = destination;
    int captureX = ex;
    int captureY = ey;
    bool priseEnPassant = false;

    if (piece.type == PION && destination.type == VIDE && sx != ex && p->enPassantX == ex && p->enPassantY == ey) {
        priseEnPassant = true;
        captureX = ex;
        captureY = sy;
        pieceCapturee = p->cases[captureY][captureX];
    }

    Piece sauvegardeSource = piece;
    Piece sauvegardeDestination = destination;

    p->cases[ey][ex] = piece;
    p->cases[sy][sx] = PieceVide();

    if (priseEnPassant) {
        p->cases[captureY][captureX] = PieceVide();
    }

    bool enEchec = EstEnEchec(p, piece.couleur);

    p->cases[sy][sx] = sauvegardeSource;
    p->cases[ey][ex] = sauvegardeDestination;

    if (priseEnPassant) {
        p->cases[captureY][captureX] = pieceCapturee;
    }

    return !enEchec;
}

void JouerCoup(Partie* p, int sx, int sy, int ex, int ey) {
    if (p == NULL) {
        return;
    }

    if (!EstMouvementLegal(p, sx, sy, ex, ey)) {
        return;
    }

    Piece piece = p->cases[sy][sx];
    Piece destination = p->cases[ey][ex];
    bool priseEnPassant = false;
    bool roque = false;
    Piece pieceCapturee = destination;
    int captureX = ex;
    int captureY = ey;

    p->enPassantX = -1;
    p->enPassantY = -1;

    if (piece.type == PION && destination.type == VIDE && sx != ex) {
        priseEnPassant = true;
        captureX = ex;
        captureY = sy;
        pieceCapturee = p->cases[captureY][captureX];
        p->cases[captureY][captureX] = PieceVide();
    }

    if (piece.type == ROI && abs(ex - sx) == 2) {
        roque = true;
        if (ex == 6) {
            p->cases[ey][5] = p->cases[ey][7];
            p->cases[ey][7] = PieceVide();
            p->cases[ey][5].aBouge = true;
        } else if (ex == 2) {
            p->cases[ey][3] = p->cases[ey][0];
            p->cases[ey][0] = PieceVide();
            p->cases[ey][3].aBouge = true;
        }
    }

    if (piece.type == PION && abs(ey - sy) == 2) {
        p->enPassantX = sx;
        p->enPassantY = sy + ((ey - sy) / 2);
    }

    Coup coup;
    coup.startX = sx;
    coup.startY = sy;
    coup.endX = ex;
    coup.endY = ey;
    coup.pieceDeplacee = piece;
    coup.pieceCapturee = pieceCapturee;
    coup.estPriseEnPassant = priseEnPassant;
    coup.estRoque = roque;

    if (p->historique.taille == p->historique.capacite) {
        int nouvelleCapacite = p->historique.capacite * 2;
        Coup* nouveauxCoups = (Coup*)realloc(p->historique.coups, (size_t)nouvelleCapacite * sizeof(Coup));
        if (nouveauxCoups != NULL) {
            p->historique.coups = nouveauxCoups;
            p->historique.capacite = nouvelleCapacite;
        }
    }

    if (p->historique.taille < p->historique.capacite) {
        p->historique.coups[p->historique.taille] = coup;
        p->historique.taille++;
    }

    p->cases[ey][ex] = piece;
    p->cases[sy][sx] = PieceVide();
    p->cases[ey][ex].aBouge = true;

    if (piece.type == PION && (ey == 0 || ey == 7)) {
        p->cases[ey][ex].type = REINE;
    }

    if (piece.type == ROI) {
        p->cases[ey][ex].aBouge = true;
    }

    if (piece.type == TOUR) {
        p->cases[ey][ex].aBouge = true;
    }

    p->tourActuel = (p->tourActuel == BLANC) ? NOIR : BLANC;
}

bool EstEchecEtMat(Partie* p, Couleur c) {
    if (p == NULL) {
        return false;
    }

    if (!EstEnEchec(p, c)) {
        return false;
    }

    for (int sy = 0; sy < 8; sy++) {
        for (int sx = 0; sx < 8; sx++) {
            if (p->cases[sy][sx].type == VIDE || p->cases[sy][sx].couleur != c) {
                continue;
            }

            for (int ey = 0; ey < 8; ey++) {
                for (int ex = 0; ex < 8; ex++) {
                    if (EstMouvementLegal(p, sx, sy, ex, ey)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool EstPat(Partie* p, Couleur c) {
    if (p == NULL) {
        return false;
    }

    if (EstEnEchec(p, c)) {
        return false;
    }

    for (int sy = 0; sy < 8; sy++) {
        for (int sx = 0; sx < 8; sx++) {
            if (p->cases[sy][sx].type == VIDE || p->cases[sy][sx].couleur != c) {
                continue;
            }

            for (int ey = 0; ey < 8; ey++) {
                for (int ex = 0; ex < 8; ex++) {
                    if (EstMouvementLegal(p, sx, sy, ex, ey)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
