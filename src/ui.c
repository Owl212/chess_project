#include "chess.h"
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
        DrawText(modes[i], 60, 158 + i * 45, 18, WHITE);
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

    DrawText(TextFormat("TourActuel: %s", (p->tourActuel==BLANC)?"BLANCS":"NOIRS"), panneauX, 200, 20, GOLD);

    if (p->mode != MODE_PVP) {
        DrawText(TextFormat("Mode: Vs BOT (Niv.%d)", p->niveauBot), panneauX, 230, 18, ORANGE);
    }

    DrawRectangleRec((Rectangle){panneauX, 280, 200, 40}, LIGHTGRAY);
    DrawText("Prendre Takeback (T)", panneauX + 15, 290, 17, BLACK);

    DrawRectangleRec((Rectangle){panneauX, 340, 200, 40}, GREEN);
    DrawText("Hint 1 Coup (H)", panneauX + 35, 350, 17, BLACK);
    
    Rectangle btnRejouer = { panneauX, 400, 200, 40 };
    DrawRectangleRec(btnRejouer, (Color){100, 150, 250, 255}); 
    DrawRectangleLinesEx(btnRejouer, 2, DARKBLUE);
    DrawText("Rejouer (Menu)", panneauX + 35, 412, 17, WHITE);       

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnRejouer)) {
        InitPartie(p); 
    }

    if (p->partieTerminee) {
        DrawText("PARTIE TERMINEE !", panneauX, 460, 20, RED);
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
