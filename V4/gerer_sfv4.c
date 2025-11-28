/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
 * Fichier : gerer_sfv4.c
 * Programme principal de test pour le SF version 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sf.h"
#include "bloc.h"
#include "inode.h"
#include "repertoire.h"


static void afficherMenu()
{
    printf("\n========= MENU SF V4 =========\n");
    printf("1 : Creer un systeme de fichiers\n");
    printf("2 : Afficher le systeme de fichiers\n");
    printf("3 : Ecrire fichier (taille illimitée)\n");
    printf("4 : Sauvegarder SF dans un fichier\n");
    printf("5 : Charger SF depuis un fichier\n");
    printf("6 : Lister le repertoire racine (ls)\n");
    printf("7 : Lister le repertoire racine (ls -l)\n");
    printf("0 : Quitter\n");
    printf("Choix : ");
}

int main()
{
    tSF sf = NULL;
    int choix = -1;
    char nom[128];

    while (choix != 0) {

        afficherMenu();
        scanf("%d", &choix);

        if (choix == 1) {
            printf("Nom du disque : ");
            scanf("%s", nom);

            if (sf != NULL) {
                DetruireSF(&sf);
            }

            sf = CreerSF(nom);

            if (sf == NULL) {
                printf("Erreur : impossible de creer le SF\n");
            } else {
                printf("Systeme de fichiers cree.\n");
            }
        }

        else if (choix == 2) {
            if (sf == NULL) {
                printf("Aucun SF charge.\n");
            } else {
                AfficherSF(sf);
            }
        }

        else if (choix == 3) {
            if (sf == NULL) {
                printf("Aucun SF charge.\n");
            } else {
                printf("Nom du fichier a ecrire : ");
                scanf("%s", nom);

                long res = EcrireFichierSF(sf, nom, ORDINAIRE);

                if (res < 0) {
                    printf("Erreur ecriture fichier.\n");
                } else {
                    printf("Fichier ecrit (%ld octets).\n", res);
                }
            }
        }

        else if (choix == 4) {
            if (sf == NULL) {
                printf("Aucun SF charge.\n");
            } else {
                printf("Nom du fichier de sauvegarde : ");
                scanf("%s", nom);

                if (SauvegarderSF(sf, nom) == 0) {
                    printf("Sauvegarde reussie.\n");
                } else {
                    printf("Erreur sauvegarde.\n");
                }
            }
        }

        else if (choix == 5) {
            printf("Nom du fichier a charger : ");
            scanf("%s", nom);

            if (sf != NULL) {
                DetruireSF(&sf);
            }

            if (ChargerSF(&sf, nom) == 0) {
                printf("Chargement reussi.\n");
            } else {
                printf("Erreur chargement.\n");
            }
        }

        else if (choix == 6) {
            if (sf == NULL) {
                printf("Aucun SF charge.\n");
            } else {
                Ls(sf, false);
            }
        }

        else if (choix == 7) {
            if (sf == NULL) {
                printf("Aucun SF charge.\n");
            } else {
                Ls(sf, true);
            }
        }

        else if (choix == 0) {
            printf("Fin du programme.\n");
        }

        else {
            printf("Choix invalide.\n");
        }
    }

    if (sf != NULL) {
        DetruireSF(&sf);
    }

    return 0;
}
