/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
 * Fichier : gerer_sfv4.c
 * Programme de test simplifié (sans IHM)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sf.h"
#include "bloc.h"
#include "inode.h"
#include "repertoire.h"

/* Déclarations locales (on ne peut pas modifier sf.h) */
int SauvegarderSF(tSF sf, char nomFichier[]);
int ChargerSF(tSF *pSF, char nomFichier[]);

int main() {
    tSF sf = NULL;

    printf("=== Creation SF ===\n");
    sf = CreerSF("monDisque");
    if (!sf) {
        printf("Erreur creation SF\n");
        return 1;
    }

    printf("=== Ecriture d'un fichier dans le SF ===\n");
    long ecrit = EcrireFichierSF(sf, "test.txt", ORDINAIRE);
    printf("Octets ecrits : %ld\n", ecrit);

    printf("\n=== Affichage SF ===\n");
    AfficherSF(sf);

    printf("\n=== LS simple ===\n");
    Ls(sf, false);

    printf("\n=== LS detail ===\n");
    Ls(sf, true);

    printf("\n=== Sauvegarde SF dans fichier 'save.sf' ===\n");
    if (SauvegarderSF(sf, "save.sf") == 0)
        printf("Sauvegarde OK.\n");
    else
        printf("Erreur sauvegarde.\n");

    DetruireSF(&sf);

    printf("\n=== Chargement du SF depuis save.sf ===\n");
    if (ChargerSF(&sf, "save.sf") == 0)
        printf("Chargement OK.\n");
    else {
        printf("Erreur chargement.\n");
        return 1;
    }

    printf("\n=== LS après chargement ===\n");
    Ls(sf, true);

    DetruireSF(&sf);
    printf("\n=== Programme termine ===\n");

    return 0;
}
 