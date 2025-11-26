/**
 * Programme de test pour la version 2 du système de fichiers
 * gerer_sfv2.c
 *
 * Menu interactif :
 * 1) Creer SF
 * 2) Ajouter un fichier (1 bloc max)
 * 3) Afficher SF
 * 4) Detruire SF
 * 5) Quitter
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sf.h"
#include "inode.h"
#include "bloc.h"

#define MAX_CHAINE 256

/* Lit une ligne depuis stdin et enlève le '\n' final si présent */
static void lire_ligne(char *buf, size_t taille) {
    if (fgets(buf, (int)taille, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    size_t L = strlen(buf);
    if (L > 0 && buf[L-1] == '\n') buf[L-1] = '\0';
}

static void afficher_menu(void) {
    printf("\n=== MENU GESTION SF V2 ===\n");
    printf("1) Creer un nouveau systeme de fichiers\n");
    printf("2) Ajouter un fichier (1 bloc max) dans le SF\n");
    printf("3) Afficher le SF\n");
    printf("4) Detruire le SF\n");
    printf("5) Quitter\n");
    printf("Choix: ");
}

int main(void) {
    tSF sf = NULL;
    char buffer[MAX_CHAINE];

    for (;;) {
        afficher_menu();
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        int choix = atoi(buffer);

        if (choix == 1) {
            if (sf != NULL) {
                printf("Un SF existe deja. Il sera detruit avant de creer le nouveau.\n");
                DetruireSF(&sf);
            }
            printf("Nom du disque (<= %d chars) : ", TAILLE_NOM_DISQUE);
            lire_ligne(buffer, sizeof(buffer));
            if (strlen(buffer) == 0) {
                printf("Nom vide -> annulation.\n");
                continue;
            }
            sf = CreerSF(buffer);
            if (sf == NULL) {
                printf("Erreur creation SF.\n");
            } else {
                printf("SF cree : %s\n", buffer);
            }
        }
        else if (choix == 2) {
            if (sf == NULL) {
                printf("Aucun SF present. Creez-en un d'abord.\n");
                continue;
            }
            printf("Chemin du fichier a ajouter (sur disque) : ");
            lire_ligne(buffer, sizeof(buffer));
            if (strlen(buffer) == 0) {
                printf("Nom de fichier vide -> annulation.\n");
                continue;
            }
            printf("Type du fichier (0=ORDINAIRE,1=REPERTOIRE,2=AUTRE) : ");
            lire_ligne(buffer, sizeof(buffer));
            int t = atoi(buffer);
            natureFichier nf = AUTRE;
            if (t == 0) nf = ORDINAIRE;
            else if (t == 1) nf = REPERTOIRE;
            else nf = AUTRE;

            long nb = Ecrire1BlocFichierSF(sf, buffer,nf);
            char fname[MAX_CHAINE];
            strcpy(fname, ""); /* ensure */
            printf("Chemin du fichier a ajouter (sur disque) : ");
            lire_ligne(fname, sizeof(fname));
            if (strlen(fname) == 0) {
                printf("Nom de fichier vide -> annulation.\n");
                continue;
            }
            printf("Type du fichier (0=ORDINAIRE,1=REPERTOIRE,2=AUTRE) : ");
            lire_ligne(buffer, sizeof(buffer));
            t = atoi(buffer);
            if (t == 0) nf = ORDINAIRE;
            else if (t == 1) nf = REPERTOIRE;
            else nf = AUTRE;

            nb = Ecrire1BlocFichierSF(sf, fname, nf);
            if (nb < 0) {
                printf("Erreur lors de l'ecriture du fichier '%s' dans le SF.\n", fname);
            } else {
                printf("Ecriture reussie : %ld octets ecrits pour '%s'.\n", nb, fname);
            }
        }
        else if (choix == 3) {
            AfficherSF(sf);
        }
        else if (choix == 4) {
            if (sf == NULL) {
                printf("Aucun SF a detruire.\n");
            } else {
                DetruireSF(&sf);
                printf("SF detruit.\n");
            }
        }
        else if (choix == 5) {
            if (sf != NULL) {
                DetruireSF(&sf);
                sf = NULL;
            }
            printf("Au revoir.\n");
            break;
        }
        else {
            printf("Choix invalide.\n");
        }
    }

    /* Sécurité : détruire si encore présent */
    if (sf != NULL) DetruireSF(&sf);

    return 0;
}