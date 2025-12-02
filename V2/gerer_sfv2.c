/**
 * Programme de test pour la version 2 du système de fichiers
 * gerer_sfv2.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sf.h"
#include "inode.h"
#include "bloc.h"

int main(void) {
    tSF sf = NULL;
    long nb;

    // 1) Création du système de fichiers
    sf = CreerSF("disque_test.sf");
    if (sf == NULL) {
        printf("Erreur lors de la creation du SF.\n");
        return 1;
    }
    printf("SF cree avec succes.\n");

    // 2) Ajout d'un fichier ORDINAIRE
    nb = Ecrire1BlocFichierSF(sf, "testV2.txt", ORDINAIRE);
    if (nb < 0) {
        printf("Erreur lors de l'ajout de fichier1.txt\n");
    } else {
        printf("Fichier 'testV2.txt' ajoute, %ld octets ecrits.\n", nb);
    }

    // 3) Ajout d'un fichier REPERTOIRE
    nb = Ecrire1BlocFichierSF(sf, "dossierV2", REPERTOIRE);
    if (nb < 0) {
        printf("Erreur lors de l'ajout de dossier1\n");
    } else {
        printf("Repertoire 'dossierV2' ajoute, %ld octets ecrits.\n", nb);
    }

    // 4) Affichage du système de fichiers
    printf("\n=== Contenu du SF ===\n");
    AfficherSF(sf);

    // 5) Destruction du SF
    DetruireSF(&sf);
    printf("\nSF detruit.\n");

    return 0;
}
