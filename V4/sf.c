/**
 * ProgC - Projet Automne 25-26 : Gestion d'un systèmes de fichiers
 * VERSION 4
 * Fichier : sf.c
 * Module de gestion d'un systèmes de fichiers (simulé)
 **/

#include "sf.h"
#include "bloc.h"
#include "repertoire.h"
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
    strncpy(buf, ctime(&(superBloc->dateDerModif)), sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    if (strlen(buf) > 0 && buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = '\0';

    printf("Super-bloc :\n");
    printf("  Nom du disque       : %s\n", superBloc->nomDisque);
    printf("  Date derniere modif : %s\n", buf);
}

/* V4
 * Crée un nouveau système de fichiers.
 * Entrée : nom du disque à associer au système de fichiers créé
 * Retour : le système de fichiers créé, ou NULL en cas d'erreur
 *
 * Remarque : on crée aussi l'inode 0 (répertoire racine, vide).
 */
tSF CreerSF(char nomDisque[])
{
    if (nomDisque == NULL) return NULL;

    tSF sf = malloc(sizeof(struct sSF));
    if (sf == NULL) {
        fprintf(stderr, "CreerSF : probleme allocation SF\n");
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

    /* Créer inode 0 : répertoire racine */
    tInode inode0 = CreerInode(0, REPERTOIRE);
    if (inode0 == NULL) {
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        return NULL;
    }

    /* Écrire un répertoire vide dans inode 0 */
    tRepertoire rep = CreerRepertoire();
    if (rep == NULL) {
        DetruireInode(&inode0);
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        return NULL;
    }

    if (EcrireRepertoireDansInode(rep, inode0) != 0) {
        DetruireRepertoire(&rep);
        DetruireInode(&inode0);
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        return NULL;
    }

    DetruireRepertoire(&rep);

    /* Ajouter inode0 à la liste chaînée */
    struct sListeInodesElement *elem = malloc(sizeof(struct sListeInodesElement));
    if (elem == NULL) {
        DetruireInode(&inode0);
        DetruireSuperBloc(&(sf->superBloc));
        free(sf);
        return NULL;
    }

    elem->inode = inode0;
    elem->suivant = NULL;
    sf->listeInodes.premier = elem;
    sf->listeInodes.dernier = elem;
    sf->listeInodes.nbInodes = 1;

    sf->superBloc->dateDerModif = time(NULL);

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
 *
 * (conserve la version précédente)
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

/* V3 & V4
 * Ecrit un fichier (d'un nombre de blocs quelconque) dans le système de fichiers.
 * Si la taille du fichier à écrire dépasse la taille maximale d'un fichier dans le SF(10 x 64 octets),
 * seuls les 640 premiers octets seront écrits dans le système de fichiers.
 * Entrées : le système de fichiers, le nom du fichier (sur disque) et son type dans le SF (simulé)
 * Sortie : le nombre d'octets effectivement écrits, -1 en cas d'erreur.
 *
 * En V4 : met aussi à jour le répertoire racine (inode 0).
 */
long EcrireFichierSF(tSF sf, char nomFichier[], natureFichier type)
{
    if (sf == NULL || nomFichier == NULL) {
        fprintf(stderr, "EcrireFichierSF : parametres invalides\n");
        return -1;
    }

    FILE *f = fopen(nomFichier, "rb");
    if (f == NULL) {
        fprintf(stderr, "EcrireFichierSF : impossible d'ouvrir %s\n", nomFichier);
        return -1;
    }

    long tailleMax = TailleMaxFichier();
    unsigned char *buffer = malloc(tailleMax);
    if (buffer == NULL) {
        fclose(f);
        return -1;
    }

    long nbLus = fread(buffer, 1, tailleMax, f);
    fclose(f);

    if (nbLus < 0) nbLus = 0;

    /* Créer nouvel inode */
    int numInode = sf->listeInodes.nbInodes;
    tInode inode = CreerInode(numInode, type);
    if (inode == NULL) {
        free(buffer);
        return -1;
    }

    long nbEcrits = EcrireDonneesInode(inode, buffer, nbLus, 0);
    free(buffer);

    if (nbEcrits < 0) {
        DetruireInode(&inode);
        return -1;
    }

    /* Ajouter inode à la liste chaînée */
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

    /* Mettre à jour le répertoire racine (inode 0) */
    /* Trouver inode 0 dans la liste */
    struct sListeInodesElement *it = sf->listeInodes.premier;
    tInode inode0 = NULL;
    while (it != NULL) {
        if (Numero(it->inode) == 0u) {
            inode0 = it->inode;
            break;
        }
        it = it->suivant;
    }

    if (inode0 == NULL) {
        /* Pas d'inode 0 : erreur (normalement créé dans CreerSF) */
        return (long)nbEcrits; /* fichier écrit mais pas ajouté au répertoire */
    }

    /* Lire répertoire depuis inode0 */
    tRepertoire rep = NULL;
    if (LireRepertoireDepuisInode(&rep, inode0) != 0) {
        /* n'arrive pas à lire le répertoire -> on laisse le fichier écrit */
        return (long)nbEcrits;
    }

    /* Ajouter l'entrée (nomFichier tel quel dans la racine) */
    if (EcrireEntreeRepertoire(rep, nomFichier, (unsigned int)numInode) != 0) {
        DetruireRepertoire(&rep);
        return (long)nbEcrits;
    }

    /* Écrire le répertoire modifié dans inode0 */
    if (EcrireRepertoireDansInode(rep, inode0) != 0) {
        DetruireRepertoire(&rep);
        return (long)nbEcrits;
    }

    DetruireRepertoire(&rep);

    /* Mise à jour du super-bloc */
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

/* V4
 * Affiche le détail du répertoire racine d'un système de fichiers.
 * Entrées : le système de fichiers,
 * et un booléen indiquant si l'affichage doit être détaillé (true) ou non (false),
 * voir l'énoncé pour les consignes d'affichage à suivre
 * Sortie : 0 en cas de succès, -1 en cas d'erreur
 */
int Ls(tSF sf, bool detail) {
    if (sf == NULL) return -1;

    /* 1) trouver l'inode numéro 0 dans la liste chaînée */
    struct sListeInodesElement *elem = sf->listeInodes.premier;
    tInode inode0 = NULL;
    while (elem != NULL) {
        if (Numero(elem->inode) == 0u) {
            inode0 = elem->inode;
            break;
        }
        elem = elem->suivant;
    }
    if (inode0 == NULL) {
        /* pas de répertoire racine trouvé */
        return -1;
    }

    /* 2) lire le répertoire depuis l'inode 0 */
    tRepertoire rep = NULL;
    if (LireRepertoireDepuisInode(&rep, inode0) != 0) {
        return -1;
    }

    int nb = NbEntreesRepertoire(rep);

    printf("Nombre de fichiers dans le répertoire racine : %d\n", nb);

    if (nb == 0) {
        /* nettoyer et sortir */
        DetruireRepertoire(&rep);
        return 0;
    }

    /* récupérer les entrées */
    struct sEntreesRepertoire *tab = malloc(sizeof(struct sEntreesRepertoire) * nb);
    if (tab == NULL) {
        DetruireRepertoire(&rep);
        return -1;
    }
    int nbRecup = EntreesContenuesDansRepertoire(rep, tab);
    if (nbRecup < 0) {
        free(tab);
        DetruireRepertoire(&rep);
        return -1;
    }

    if (!detail) {
        /* affichage simple : un nom par ligne */
        for (int i = 0; i < nbRecup; i++) {
            printf("%s\n", tab[i].nomEntree);
        }
    } else {
        /* affichage détaillé : pour chaque entrée afficher
           %-3u %-12s %6ld %s %s
           numéro inode, type, taille, date modif (format ctime sans \n), nom */
        for (int i = 0; i < nbRecup; i++) {
            unsigned int num = tab[i].numeroInode;

            /* trouver l'inode correspondant au numéro 'num' */
            struct sListeInodesElement *e2 = sf->listeInodes.premier;
            tInode inodeEntry = NULL;
            while (e2 != NULL) {
                if (Numero(e2->inode) == num) {
                    inodeEntry = e2->inode;
                    break;
                }
                e2 = e2->suivant;
            }

            /* si inode introuvable, on affiche des valeurs par défaut */
            natureFichier nf = AUTRE;
            long taille = 0;
            time_t tm = (time_t)0;
            if (inodeEntry != NULL) {
                nf = Type(inodeEntry);
                taille = Taille(inodeEntry);
                tm = DateDerModifFichier(inodeEntry);
            }

            const char *typeStr = "AUTRE";
            if (nf == ORDINAIRE) typeStr = "ORDINAIRE";
            else if (nf == REPERTOIRE) typeStr = "REPERTOIRE";

            char datebuf[32];
            if (tm == (time_t)0) {
                datebuf[0] = '\0';
            } else {
                char *c = ctime(&tm);
                if (c == NULL) datebuf[0] = '\0';
                else {
                    /* ctime rend une chaîne qui se termine par '\n' */
                    strncpy(datebuf, c, sizeof(datebuf)-1);
                    datebuf[sizeof(datebuf)-1] = '\0';
                    size_t L = strlen(datebuf);
                    if (L > 0 && datebuf[L-1] == '\n') datebuf[L-1] = '\0';
                }
            }

            printf("%-3u %-12s %6ld %s %s\n",
                   num,
                   typeStr,
                   taille,
                   datebuf,
                   tab[i].nomEntree);
        }
    }
    free(tab);
    DetruireRepertoire(&rep);
    return 0;
}
