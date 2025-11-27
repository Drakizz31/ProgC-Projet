/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 3
 * Fichier : sf.c
 * Module de gestion d'un systèmes de fichiers (simulé)
 **/

#include "sf.h"
#include "bloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_FICHIER_OCTETS (10 * TAILLE_BLOC)


// Taille maximale du nom du SF (ou nom du disque)
#define TAILLE_NOM_DISQUE 24

// Définition du super-bloc
struct sSuperBloc
{
  // Le nom du disque (ou du système de fichiers) (avec le '\0')
  char nomDisque[TAILLE_NOM_DISQUE+1];
  // La date de dernière modification du système de fichiers
  time_t dateDerModif;
};

// Type représentant le super-bloc
typedef struct sSuperBloc *tSuperBloc;

// Définition de la liste chaînée des inodes
struct sListeInodes
{
  // Références vers le premier et le dernier élément de la liste chaînée
  struct sListeInodesElement *premier, *dernier;
  // Nombre d'inodes dans la liste
  int nbInodes;
};

// Définition d'un élement de la liste chaînée des inodes dans le SF
struct sListeInodesElement
{
  // L'inode proprement dit
  tInode inode;
  // L'élément suivant dans la liste chaînée
  struct sListeInodesElement *suivant;
};

// Définition d'un système de fichiers (simplifié)
struct sSF
{
  // Le super-bloc
  tSuperBloc superBloc;
  // La liste chaînée des inodes
  struct sListeInodes listeInodes;
};

/* V2
*  Crée et retourne un super-bloc.
* Fonction non publique (static)
* Entrée : le nom du disque (ou du SF)
* Sortie : le super-bloc, ou NULL en cas de problème
*/
static tSuperBloc CreerSuperBloc(char nomDisque[])
{
    tSuperBloc sb = malloc(sizeof(struct sSuperBloc));
    if (sb == NULL) {
        fprintf(stderr, "CreerSuperBloc : probleme creation\n");
        return NULL;
    }

    strncpy(sb->nomDisque, nomDisque, TAILLE_NOM_DISQUE);
    sb->nomDisque[TAILLE_NOM_DISQUE] = '\0';

    sb->dateDerModif = time(NULL);

    return sb;
}

/* V2
*  Détruit un super-bloc.
* Fonction non publique (static)
* Entrée : le super-bloc à détruire
* Sortie : aucune
*/
static void DetruireSuperBloc(tSuperBloc *pSuperBloc)
{
    if (pSuperBloc != NULL && *pSuperBloc != NULL) {
        free(*pSuperBloc);
        *pSuperBloc = NULL;
    }
}

/* V2
*  Affiche le contenu d'un super-bloc.
* Fonction non publique (static)
* Entrée : le super-bloc à afficher
* Sortie : aucune
*/
static void AfficherSuperBloc(tSuperBloc superBloc)
{
    if (superBloc == NULL) {
        printf("Super-bloc : (NULL)\n");
        return;
    }

    char buf[32];
    strcpy(buf, ctime(&(superBloc->dateDerModif)));
    buf[strlen(buf)-1] = '\0';

    printf("Super-bloc :\n");
    printf("  Nom du disque       : %s\n", superBloc->nomDisque);
    printf("  Date derniere modif : %s\n", buf);
}

/* V2
 * Crée un nouveau système de fichiers.
 * Entrée : nom du disque à associer au système de fichiers créé
 * Retour : le système de fichiers créé, ou NULL en cas d'erreur
 */
tSF CreerSF(char nomDisque[])
{
    tSF sf = malloc(sizeof(struct sSF));
    if (sf == NULL) {
        fprintf(stderr, "CreerSF : probleme creation\n");
        return NULL;
    }

    sf->superBloc = CreerSuperBloc(nomDisque);
    if (sf->superBloc == NULL) {
        free(sf);
        return NULL;
    }

    sf->listeInodes.premier = NULL;
    sf->listeInodes.dernier = NULL;
    sf->listeInodes.nbInodes = 0;

    return sf;
}

/* V2
 * Détruit un système de fichiers et libère la mémoire associée.
 * Entrée : le SF à détruire
 * Sortie : aucune
 */
void DetruireSF(tSF *pSF)
{
    if (pSF == NULL || *pSF == NULL) return;

    tSF sf = *pSF;

    struct sListeInodesElement *elem = sf->listeInodes.premier;

    while (elem != NULL) {
        struct sListeInodesElement *next = elem->suivant;
        DetruireInode(&elem->inode);
        free(elem);
        elem = next;
    }

    DetruireSuperBloc(&sf->superBloc);

    free(sf);
    *pSF = NULL;
}

/* V2
 * Affiche les informations relative à un système de fichiers i.e;
 * le contenu du super-bloc et celui des différents inodes du SF
 * Entrée : le SF à afficher
 * Sortie : aucune
 */
void AfficherSF(tSF sf)
{
    if (sf == NULL) {
        printf("Systeme de fichiers NULL\n");
        return;
    }

    printf("===== SYSTEME DE FICHIERS =====\n");
    AfficherSuperBloc(sf->superBloc);
    printf("\nInodes (%d) :\n", sf->listeInodes.nbInodes);

    struct sListeInodesElement *elem = sf->listeInodes.premier;
    int i = 0;
    while (elem != NULL) {
        printf("---- Inode %d ----\n", i);
        AfficherInode(elem->inode);
        elem = elem->suivant;
        i++;
    }
}

/* V2
 * Ecrit un fichier d'un seul bloc dans le système de fichiers.
 * Entrées : le système de fichiers, le nom du fichier (sur disque) et son type dans le SF (simulé)
 * Sortie : le nombre d'octets effectivement écrits, -1 en cas d'erreur.
 */
long Ecrire1BlocFichierSF(tSF sf, char nomFichier[], natureFichier type)
{
    if (sf == NULL || nomFichier == NULL) {
        fprintf(stderr, "Ecrire1BlocFichierSF : parametres invalides\n");
        return -1;
    }

    /* Étape 1 : lecture du fichier réel */
    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) {
        fprintf(stderr, "Ecrire1BlocFichierSF : impossible d'ouvrir %s\n", nomFichier);
        return -1;
    }

    unsigned char buffer[TAILLE_BLOC];
    long nbLus = fread(buffer, 1, TAILLE_BLOC, f);
    fclose(f);

    if (nbLus < 0) nbLus = 0;

    /* Étape 2 : création d’un inode */
    int numInode = sf->listeInodes.nbInodes;
    tInode inode = CreerInode(numInode, type);
    if (inode == NULL) {
        fprintf(stderr, "Ecrire1BlocFichierSF : echec creation inode\n");
        return -1;
    }

    /* Étape 3 : écriture dans l’inode */
    long nbEcrits = EcrireDonneesInode1bloc(inode, buffer, nbLus);
    if (nbEcrits < 0) {
        fprintf(stderr, "Ecrire1BlocFichierSF : erreur écriture inode\n");
        DetruireInode(&inode);
        return -1;
    }

    /* Étape 4 : ajout dans la liste chaînée */
    struct sListeInodesElement *elem = malloc(sizeof(struct sListeInodesElement));
    if (elem == NULL) {
        fprintf(stderr, "Ecrire1BlocFichierSF : probleme allocation liste\n");
        DetruireInode(&inode);
        return -1;
    }

    elem->inode = inode;
    elem->suivant = NULL;

    if (sf->listeInodes.premier == NULL) {
        sf->listeInodes.premier = elem;
        sf->listeInodes.dernier = elem;
    } else {
        sf->listeInodes.dernier->suivant = elem;
        sf->listeInodes.dernier = elem;
    }

    sf->listeInodes.nbInodes++;

    /* Étape 5 : mise à jour du super-bloc */
    sf->superBloc->dateDerModif = time(NULL);

    return nbEcrits;
}


/* V3
 * Ecrit un fichier (d'un nombre de blocs quelconque) dans le système de fichiers.
 * Si la taille du fichier à écrire dépasse la taille maximale d'un fichier dans le SF(10 x 64 octets),
 * seuls les 640 premiers octets seront écrits dans le système de fichiers.
 * Entrées : le système de fichiers, le nom du fichier (sur disque) et son type dans le SF (simulé)
 * Sortie : le nombre d'octets effectivement écrits, -1 en cas d'erreur.
 */
long EcrireFichierSF(tSF sf, char nomFichier[], natureFichier type) {

    if (sf == NULL || nomFichier == NULL) return -1;

    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) return -1;

    unsigned char buffer[MAX_FICHIER_OCTETS];
    long nbLus = fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    if (nbLus < 0) nbLus = 0;

    int numInode = sf->listeInodes.nbInodes;
    tInode inode = CreerInode(numInode, type);
    if (inode == NULL) return -1;

    long nbEcrits = EcrireDonneesInode(inode, buffer, nbLus, 0);
    if (nbEcrits < 0) {
        DetruireInode(&inode);
        return -1;
    }

    struct sListeInodesElement *elem = malloc(sizeof(struct sListeInodesElement));
    if (elem == NULL) {
        DetruireInode(&inode);
        return -1;
    }

    elem->inode = inode;
    elem->suivant = NULL;

    if (sf->listeInodes.premier == NULL) {
        sf->listeInodes.premier = elem;
        sf->listeInodes.dernier = elem;
    } else {
        sf->listeInodes.dernier->suivant = elem;
        sf->listeInodes.dernier = elem;
    }

    sf->listeInodes.nbInodes++;
    sf->superBloc->dateDerModif = time(NULL);

    return nbEcrits;
}

/* V3
 * Sauvegarde un système de fichiers dans un fichier (sur disque).
 * Entrées : le système de fichiers, le nom du fichier sauvegarde (sur disque)
 * Sortie : 0 en cas de succèe, -1 en cas d'erreur
 */
int SauvegarderSF(tSF sf, char nomFichier[]) {

    if (sf == NULL || nomFichier == NULL) return -1;

    FILE *f = fopen(nomFichier, "wb");
    if (f == NULL) return -1;

    if (fwrite(sf->superBloc, sizeof(struct sSuperBloc), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    if (fwrite(&(sf->listeInodes.nbInodes), sizeof(int), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    struct sListeInodesElement *elem = sf->listeInodes.premier;

    while (elem != NULL) {

        if (SauvegarderInode(elem->inode, f) != 0) {
            fclose(f);
            return -1;
        }

        elem = elem->suivant;
    }

    fclose(f);
    return 0;
}

/* V3
 * Restaure le contenu d'un système de fichiers depuis un fichier sauvegarde (sur disque).
 * Entrées : le système de fichiers où restaurer, le nom du fichier sauvegarde (sur disque)
 * Sortie : 0 en cas de succèe, -1 en cas d'erreur
 */
int ChargerSF(tSF *pSF, char nomFichier[]) {

    if (pSF == NULL || nomFichier == NULL) return -1;

    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) return -1;

    *pSF = malloc(sizeof(struct sSF));
    if (*pSF == NULL) {
        fclose(f);
        return -1;
    }

    tSF sf = *pSF;

    sf->superBloc = malloc(sizeof(struct sSuperBloc));
    if (sf->superBloc == NULL) {
        free(sf);
        *pSF = NULL;
        fclose(f);
        return -1;
    }

    if (fread(sf->superBloc, sizeof(struct sSuperBloc), 1, f) != 1) {
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        *pSF = NULL;
        fclose(f);
        return -1;
    }

    int nbInodes = 0;
    if (fread(&nbInodes, sizeof(int), 1, f) != 1) {
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        *pSF = NULL;
        fclose(f);
        return -1;
    }

    sf->listeInodes.premier = NULL;
    sf->listeInodes.dernier = NULL;
    sf->listeInodes.nbInodes = 0;

    for (int i = 0; i < nbInodes; i++) {

        tInode inode = NULL;
        if (ChargerInode(&inode, f) != 0) {
            DetruireSF(pSF);
            fclose(f);
            return -1;
        }

        struct sListeInodesElement *elem = malloc(sizeof(struct sListeInodesElement));
        if (elem == NULL) {
            DetruireInode(&inode);
            DetruireSF(pSF);
            fclose(f);
            return -1;
        }

        elem->inode = inode;
        elem->suivant = NULL;

        if (sf->listeInodes.premier == NULL) {
            sf->listeInodes.premier = elem;
            sf->listeInodes.dernier = elem;
        } else {
            sf->listeInodes.dernier->suivant = elem;
            sf->listeInodes.dernier = elem;
        }

        sf->listeInodes.nbInodes++;
    }

    fclose(f);
    return 0;
}
