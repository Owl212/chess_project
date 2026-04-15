#include "raylib.h"
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
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;
    
    Vector2 pos = GetMousePosition();
    int pX = 20 + 8 * 80 + 20;

    if (CheckCollisionPointRec(pos, (Rectangle){ pX, 280, 200, 40 }) && p->historique.taille > 0) {
        DemanderTakeback(p, p->tourActuel); return;
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

    Partie partie = {0};
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

            bool demandeHint = false;
            if (IsKeyPressed(KEY_H)) {
                demandeHint = true;
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 pos = GetMousePosition();
                int pX = 20 + 8 * 80 + 20;
                if (CheckCollisionPointRec(pos, (Rectangle){ pX, 340, 200, 40 })) {
                    demandeHint = true;
                }
            }
            if (demandeHint) {
                DemanderHint(&partie);
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
