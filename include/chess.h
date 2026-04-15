#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include "raylib.h"

// --- ENUMS ---
typedef enum { VIDE, PION, CAVALIER, FOU, TOUR, REINE, ROI } TypePiece;
typedef enum { AUCUN, BLANC, NOIR } Couleur;

// États de la demande de Takeback (Annulation)
typedef enum {
    PAS_DE_DEMANDE,
    DEMANDE_PAR_BLANC,
    DEMANDE_PAR_NOIR
} EtatTakeback;

// Machine d'États de la partie
typedef enum {
    MENU_PRINCIPAL,
    EN_JEU,
    ECRAN_PROMOTION, // Pause pour choisir sa promotion
    ECRAN_FIN
} EtatJeu;

// Mode de jeu
typedef enum {
    MODE_PVP,        // Joueur vs Joueur (Local)
    MODE_PVE_BLANC,  // Joueur (Blancs) vs Bot (Noirs)
    MODE_PVE_NOIR    // Joueur (Noirs) vs Bot (Blancs)
} ModeJeu;

// --- STRUCTURES ---
typedef struct {
    TypePiece type;
    Couleur couleur;
    bool aBouge; // Essentiel pour gérer le roque et le double pas du pion
} Piece;

// Structure représentant un déplacement unique (pour historique dynamique)
typedef struct {
    int startX, startY;
    int endX, endY;
    Piece pieceDeplacee;
    Piece pieceCapturee;
    bool premierMouvement; // Pour restaurer aBouge proprement
} Coup;

// Historique dynamique (Allocation de mémoire stricte pour 0 fuite)
typedef struct {
    Coup* coups;
    int capacite;
    int taille;
} Historique;

// La structure globale qui contient TOUT l'état de la partie
typedef struct {
    Piece cases[8][8];
    Couleur tourActuel;

    // Historique dynamique pur
    Historique historique;

    // Règles Spéciales
    int enPassantX;
    int enPassantY;

    // Timers
    float tempsBlanc;
    float tempsNoir;
    int tempsInitial; // 0 = Sans limite

    // Machine d'États et Modes de jeu
    EtatJeu etat;
    ModeJeu mode;
    int niveauBot; // 1 (Débutant) à 8 (Dieu)

    // Informations pour la promotion en attente
    int promoX, promoY;
    Couleur promoCouleur;

    // UI Avancée
    EtatTakeback etatTakeback;
    bool partieTerminee;
    bool afficherHint;
    Coup meilleurCoupHint;
    float tempsHintRestant;
} Partie;

// --- PROTOTYPES (Séparation Modulaire Parfaite) ---

// board.c : Init, Mémoire et Copie d'État sécurisée
void InitPartie(Partie* p);
void NettoyerPartie(Partie* p); // Libère l'historique proprement
void CopierPartie(Partie* src, Partie* dest); // Crucial pour le Minimax
Piece CreerPiece(TypePiece type, Couleur couleur);

// rules.c : Vérification stricte des règles
bool EstMouvementPseudoLegal(Partie* p, int sx, int sy, int ex, int ey);
bool EstEnEchec(Partie* p, Couleur c);
bool EstEchecEtMat(Partie* p, Couleur c);
bool EstPat(Partie* p, Couleur c);
bool EstMouvementLegal(Partie* p, int sx, int sy, int ex, int ey);
int GenererCoupsLegaux(Partie* p, Couleur couleur, Coup* coupsLegaux);

// moves.c : Exécution, historique et Takeback instantané
void JouerCoup(Partie* p, int sx, int sy, int ex, int ey, TypePiece promotion);
void AnnulerDernierCoup(Partie* p);
void DemanderTakeback(Partie* p, Couleur demandeur);
void AccepterTakeback(Partie* p);
void RefuserTakeback(Partie* p);

// ai.c : Le Bot 8 niveaux et le Hint (Alpha-Beta Pruning)
Coup TrouverMeilleurCoupBot(Partie* p);
void JouerCoupBot(Partie* p);
void DemanderHint(Partie* p);

// --- ASSETS UI ---
typedef struct {
    Font fontMenu;
    Texture2D texPieces[3][7];
} UIAssets;

// ui.c : Interface Raylib et Menus
void ChargerAssets(UIAssets* assets);
void DechargerAssets(UIAssets* assets);
void DessinerMenuPrincipal(Partie* p);
void DessinerPlateau(Partie* p, int caseSelX, int caseSelY, UIAssets* assets);
void DessinerMenuTakeback(Partie* p);
void DessinerMenuPromotion(Partie* p);
void DessinerTimer(float tempsRestant, int posX, int posY, bool estBlanc, bool estSonTour);

#endif