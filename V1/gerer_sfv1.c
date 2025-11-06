/* gerer_sfv1.c
 * Programme de test pour la VERSION 1 du projet.
 * Compile avec : make -f Makefile1
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bloc.h"
#include "inode.h"

int main(void)
{
    /* 1) Créer un inode numéro 1 de type ORDINAIRE */
    tInode inode = CreerInode(1, ORDINAIRE);
    if (inode == NULL)
    {
        fprintf(stderr, "Erreur : impossible de creer l'inode\n");
        return 1;
    }

    /* 2) Préparer une petite chaîne à écrire (moins qu'un bloc) */
    unsigned char message[] = "Bonjour, test inode V1";
    long tailleMessage = (long)strlen((char *)message);

    /* 3) Écrire dans l'inode (version 1 : 1 bloc) */
    long nbEcrits = EcrireDonneesInode1bloc(inode, message, tailleMessage);
    if (nbEcrits < 0)
    {
        fprintf(stderr, "Erreur : ecriture dans l'inode impossible\n");
        DetruireInode(&inode);
        return 1;
    }

    printf("Bytes écrits dans l'inode : %ld\n", nbEcrits);

    /* 4) Afficher l'inode (informations générales) */
    AfficherInode(inode);

    /* 5) Lire les données depuis l'inode dans un buffer */
    long bufferSize = TAILLE_BLOC + 1; /* +1 pour terminer en string si on veut */
    unsigned char *buffer = malloc((size_t)bufferSize);
    if (buffer == NULL)
    {
        fprintf(stderr, "Erreur : allocation buffer\n");
        DetruireInode(&inode);
        return 1;
    }

    /* initialiser à zéro pour garantir une terminaison de chaîne */
    memset(buffer, 0, (size_t)bufferSize);

    long nbLus = LireDonneesInode1bloc(inode, buffer, TAILLE_BLOC);
    if (nbLus < 0)
    {
        fprintf(stderr, "Erreur : lecture depuis l'inode impossible\n");
        free(buffer);
        DetruireInode(&inode);
        return 1;
    }

    printf("Bytes lus depuis l'inode : %ld\n", nbLus);
    printf("Contenu lu (interprete comme chaine) : %s\n", (char *)buffer);

    /* 6) Nettoyage : libérer le buffer et détruire l'inode */
    free(buffer);
    DetruireInode(&inode);

    printf("Test termine avec succes.\n");

    return 0;
}
