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
    sf = CreerSF("disque_test_v3.sf");
    if (!sf) {
        printf("Erreur creation SF.\n");
        return 1;
    }
    printf("SF cree avec succes.\n");

    // 2) Ajout d'un fichier ORDINAIRE (multi-blocs)
    nb = EcrireFichierSF(sf, "fichierV3.txt", ORDINAIRE);
    if (nb < 0)
        printf("Erreur lors de l'ajout de 'fichierV3.txt'\n");
    else
        printf("Fichier 'fichierV3.txt' ajoute, %ld octets ecrits.\n", nb);

    // 3) Ajout d'un répertoire (simulé)
    nb = EcrireFichierSF(sf, "dossierV3", REPERTOIRE);
    if (nb < 0)
        printf("Erreur lors de l'ajout de 'dossierV3'\n");
    else
        printf("Repertoire 'dossierV3' ajoute, %ld octets.\n", nb);

    // 4) Affichage du SF
    printf("\n=== Contenu du SF ===\n");
    AfficherSF(sf);

    // 5) Sauvegarde du SF
    if (SauvegarderSF(sf, "backup_sf_v3.sf") == 0)
        printf("\nSF sauvegarde dans 'backup_sf_v3.sf'.\n");
    else
        printf("\nErreur lors de la sauvegarde.\n");

    // 6) Destruction du SF
    DetruireSF(&sf);
    printf("\nSF detruit.\n");

    // 7) Restauration depuis la sauvegarde
    if (ChargerSF(&sf, "backup_sf_v3.sf") == 0)
        printf("\nSF restaure depuis 'backup_sf_v3.sf'.\n");
    else
        printf("\nErreur lors de la restauration.\n");

    // 8) Affichage après restauration
    printf("\n=== Contenu du SF apres restauration ===\n");
    AfficherSF(sf);

    // 9) Destruction finale
    DetruireSF(&sf);
    printf("\nSF detruit definitivement.\n");

    return 0;
}
