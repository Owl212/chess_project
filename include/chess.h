#ifndef CHESS_H
#define CHESS_H

#include "raylib.h"
#include <stdbool.h>

// --- ENUMS ---
typedef enum { VIDE, PION, CAVALIER, FOU, TOUR, REINE, ROI } TypePiece;
typedef enum { AUCUNE, BLANC, NOIR } Couleur;

// --- STRUCTURES ---
typedef struct {
    TypePiece type;
    Couleur couleur;
    bool aBouge; // Crucial pour le Roque et les Pions
} Piece;

typedef struct {
    int startX, startY;
    int endX, endY;
    Piece pieceDeplacee;
    Piece pieceCapturee;
    bool estPriseEnPassant;
    bool estRoque;
} Coup;

// Historique dynamique (pour valider le critere malloc/free)
typedef struct {
    Coup* coups;
    int capacite;
    int taille;
} Historique;

typedef struct {
    Piece cases[8][8];
    Couleur tourActuel;
    Historique historique;
    int enPassantX;
    int enPassantY;
    float tempsBlanc; // Timer Lichess
    float tempsNoir;
} Partie;

// --- FONCTIONS ---
// board.c
void InitPartie(Partie* p);
void FreePartie(Partie* p);

// moves.c
bool EstMouvementPseudoLegal(Partie* p, int sx, int sy, int ex, int ey);
bool EstEnEchec(Partie* p, Couleur c);
bool EstMouvementLegal(Partie* p, int sx, int sy, int ex, int ey);
void JouerCoup(Partie* p, int sx, int sy, int ex, int ey);
bool EstEchecEtMat(Partie* p, Couleur c);
bool EstPat(Partie* p, Couleur c);

// ui.c
void DessinerPlateau(Partie* p, int caseSelX, int caseSelY, Texture2D textures[]);
void ChargerTextures(Texture2D textures[]);

#endif
