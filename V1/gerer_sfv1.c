/* gerer_sfv1.c
 * Programme de test pour la VERSION 1 du projet.
 * Compile avec : make -f Makefile1
 */

#include <stdio.h>
#include <string.h>
#include "inode.h"

#define TAILLE_BUFFER 100

int main(void) 
{
    tInode inode;

    /* Créer un inode numéro 0 de type ORDINAIRE */
    inode = CreerInode(0, ORDINAIRE);
    printf("** Inode cree\n");
    AfficherInode(inode);

    /* Ecrire/stocker dans cet inode un "fichier" */
    char unContenu[TAILLE_BUFFER];
    strcpy(unContenu, "Bonjour, Monde !");
    long tailleOctetsContenu = sizeof(char) * (strlen(unContenu) + 1);

    long nb_ecrits =
        EcrireDonneesInode1bloc(inode, (unsigned char *)unContenu, tailleOctetsContenu);

    printf("\nNombre d'octets ecrits : %ld\n", nb_ecrits);

    printf("\n*** Inode rempli\n");
    AfficherInode(inode);

    /* Lire le contenu du fichier stocké dans l'inode */
    char buffer[TAILLE_BUFFER];
    long nb_lus = LireDonneesInode1bloc(inode, (unsigned char *)buffer, sizeof(char) * (TAILLE_BUFFER - 1));

    buffer[nb_lus] = '\0'; /* On termine en chaîne de caractères */

    printf("\n*** Lecture du contenu de l'inode :\n");
    printf("Nombre d'octets lus : %ld\n", nb_lus);
    printf("Contenu lu : %s\n", buffer);

    /* Détruire l'inode */
    DetruireInode(&inode);

    printf("\n*** Inode detruit : tests OK!\n");

    return 0;
}
