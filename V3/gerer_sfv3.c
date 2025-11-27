/**
 * Programme de test pour la version 3 du système de fichiers
 * gerer_sfv3.c
 *
 * Menu interactif :
 * 1) Creer un systeme de fichiers
 * 2) Ajouter un fichier (plusieurs blocs possibles)
 * 3) Afficher le SF
 * 4) Sauvegarder le SF dans un fichier
 * 5) Restaurer un SF depuis un fichier
 * 6) Detruire le SF
 * 7) Quitter
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sf.h"
#include "inode.h"
#include "bloc.h"

#define MAX_CHAINE 256

/* Lecture propre d'une ligne */
static void lire_ligne(char *buf, size_t taille) {
    if (fgets(buf, (int)taille, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    size_t L = strlen(buf);
    if (L > 0 && buf[L-1] == '\n') buf[L-1] = '\0';
}

/* Menu */
static void afficher_menu(void) {
    printf("\n=== MENU GESTION SF V3 ===\n");
    printf("1) Creer un nouveau systeme de fichiers\n");
    printf("2) Ajouter un fichier (multi-blocs) dans le SF\n");
    printf("3) Afficher le SF\n");
    printf("4) Sauvegarder le SF dans un fichier\n");
    printf("5) Restaurer un SF depuis un fichier\n");
    printf("6) Detruire le SF\n");
    printf("7) Quitter\n");
    printf("Choix: ");
}

int main(void) {

    tSF sf = NULL;
    char buffer[MAX_CHAINE];

    for (;;) {

        afficher_menu();
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        int choix = atoi(buffer);

        /* 1) Créer un SF */
        if (choix == 1) {

            if (sf != NULL) {
                printf("Un SF existe deja. Il sera detruit.\n");
                DetruireSF(&sf);
            }

            printf("Nom du disque : ");
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

        /* 2) Ajouter fichier multi-blocs */
        else if (choix == 2) {

            if (sf == NULL) {
                printf("Aucun SF present.\n");
                continue;
            }

            printf("Chemin du fichier a ajouter : ");
            lire_ligne(buffer, sizeof(buffer));

            if (strlen(buffer) == 0) {
                printf("Nom vide -> annulation.\n");
                continue;
            }

            printf("Type du fichier (0=ORDINAIRE, 1=REPERTOIRE, 2=AUTRE) : ");
            char tmp[MAX_CHAINE];
            lire_ligne(tmp, sizeof(tmp));
            int t = atoi(tmp);

            natureFichier type = AUTRE;
            if (t == 0) type = ORDINAIRE;
            else if (t == 1) type = REPERTOIRE;

            long nb = EcrireFichierSF(sf, buffer, type);

            if (nb < 0) {
                printf("Erreur d'ecriture du fichier.\n");
            } else {
                printf("%ld octets ecrits dans l'inode.\n", nb);
            }
        }

        /* 3) Afficher SF */
        else if (choix == 3) {
            AfficherSF(sf);
        }

        /* 4) Sauvegarder SF */
        else if (choix == 4) {

            if (sf == NULL) {
                printf("Aucun SF a sauvegarder.\n");
                continue;
            }

            printf("Nom du fichier de sauvegarde : ");
            lire_ligne(buffer, sizeof(buffer));

            if (SauvegarderSF(sf, buffer) == 0) {
                printf("Sauvegarde reussie.\n");
            } else {
                printf("Echec de la sauvegarde.\n");
            }
        }

        /* 5) Restaurer SF */
        else if (choix == 5) {

            printf("Nom du fichier sauvegarde : ");
            lire_ligne(buffer, sizeof(buffer));

            if (sf != NULL) {
                printf("Un SF existe deja, il sera detruit.\n");
                DetruireSF(&sf);
            }

            if (ChargerSF(&sf, buffer) == 0) {
                printf("Restauration reussie.\n");
            } else {
                printf("Erreur lors de la restauration.\n");
                sf = NULL;
            }
        }

        /* 6) Detruire SF */
        else if (choix == 6) {
            if (sf == NULL) {
                printf("Aucun SF a detruire.\n");
            } else {
                DetruireSF(&sf);
                printf("SF detruit.\n");
            }
        }

        /* 7) Quitter */
        else if (choix == 7) {
            if (sf != NULL) DetruireSF(&sf);
            printf("Au revoir.\n");
            break;
        }

        else {
            printf("Choix invalide.\n");
        }
    }

    if (sf != NULL) DetruireSF(&sf);

    return 0;
}
