/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 4
 * Fichier : repertoire.c
 * Module de gestion d'un répertoire d'un systèmes de fichiers (simulé)
 **/
#include "repertoire.h"
#include "inode.h"
#include "bloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Taille max d’un fichier dans un inode = 10 blocs */
#define MAX_FICHIER_OCTETS (10 * TAILLE_BLOC)

/* Définition d'un répertoire */
struct sRepertoire
{
    tEntreesRepertoire *table;   /* tableau d'entrées */
    int nb;                      /* nombre d'entrées occupées */
    int capacite;                /* nombre maximal d'entrées */
};

/* Calcule le nombre maximal d'entrées pouvant tenir dans un inode */
static int NbMaxEntrees(void)
{
    int tailleEntree = sizeof(struct sEntreesRepertoire);
    return MAX_FICHIER_OCTETS / tailleEntree;
}


/* V4
 * Crée un nouveau répertoire.
 * Entrée : aucune
 * Sortie : le répertoire créé, ou NULL si problème
 */
tRepertoire CreerRepertoire(void)
{
    int nbMax = NbMaxEntrees();
    if (nbMax <= 0) {
        fprintf(stderr, "CreerRepertoire : probleme creation\n");
        return NULL;
    }

    tRepertoire rep = malloc(sizeof(struct sRepertoire));
    if (rep == NULL) {
        fprintf(stderr, "CreerRepertoire : probleme creation\n");
        return NULL;
    }

    rep->table = malloc(nbMax * sizeof(tEntreesRepertoire));
    if (rep->table == NULL) {
        fprintf(stderr, "CreerRepertoire : probleme creation\n");
        free(rep);
        return NULL;
    }

    rep->nb = 0;
    rep->capacite = nbMax;

    for (int i = 0; i < nbMax; i++) {
        rep->table[i] = malloc(sizeof(struct sEntreesRepertoire));
        if (rep->table[i] == NULL) {
            for (int j = 0; j < i; j++) free(rep->table[j]);
            free(rep->table);
            free(rep);
            fprintf(stderr, "CreerRepertoire : probleme creation\n");
            return NULL;
        }
        rep->table[i]->nomEntree[0] = '\0';
        rep->table[i]->numeroInode = 0u;
    }

    return rep;
}

/* V4
 * Détruit un répertoire et libère la mémoire associée.
 * Entrée : le répertoire à détruire
 * Sortie : aucune
 */
void DetruireRepertoire(tRepertoire *pRep)
{
    if (pRep == NULL || *pRep == NULL) return;

    tRepertoire rep = *pRep;

    for (int i = 0; i < rep->capacite; i++) {
        free(rep->table[i]);
    }
    free(rep->table);
    free(rep);

    *pRep = NULL;
}

/* V4
 * Écrit une entrée dans un répertoire.
 * Si l'entrée existe déjà dans le répertoire, le numéro d'inode associé est mis à jour.
 * Si l'entrée n'existe pas, elle est ajoutée dans le répertoire.
 * Entrées : le répertoire destination, le nom de l'entrée à écrire,
 *           le numéro d'inode associé à l'entrée
 * Retour : 0 si l'entrée est écrite avec succès, -1 en cas d'erreur
 */
int EcrireEntreeRepertoire(tRepertoire rep, char nomEntree[], unsigned int numeroInode)
{
    if (rep == NULL || nomEntree == NULL) return -1;

    /* Existe déjà ? */
    for (int i = 0; i < rep->capacite; i++) {
        if (strcmp(rep->table[i]->nomEntree, nomEntree) == 0) {
            rep->table[i]->numeroInode = numeroInode;
            return 0;
        }
    }

    /* Sinon, ajouter */
    for (int i = 0; i < rep->capacite; i++) {
        if (rep->table[i]->nomEntree[0] == '\0') {
            strncpy(rep->table[i]->nomEntree, nomEntree, TAILLE_NOM_FICHIER);
            rep->table[i]->nomEntree[TAILLE_NOM_FICHIER] = '\0';
            rep->table[i]->numeroInode = numeroInode;
            rep->nb++;
            return 0;
        }
    }

    return -1;
}

/* V4
 * Lit le contenu d'un répertoire depuis un inode.
 * Entrées : le répertoire mis à jour avec le contenu lu,
 *           l'inode source.
 * Retour : 0 si le répertoire est lu avec succès, -1 en cas d'erreur
 */
int LireRepertoireDepuisInode(tRepertoire *pRep, tInode inode)
{
    if (pRep == NULL || inode == NULL) return -1;

    long taille = Taille(inode);
    if (taille <= 0) {
        if (*pRep == NULL) *pRep = CreerRepertoire();
        (*pRep)->nb = 0;
        return 0;
    }

    int nbEntrees = taille / sizeof(struct sEntreesRepertoire);
    int max = NbMaxEntrees();
    if (nbEntrees > max) nbEntrees = max;

    if (*pRep == NULL) {
        *pRep = CreerRepertoire();
        if (*pRep == NULL) return -1;
    }

    tRepertoire rep = *pRep;

    unsigned char *buffer = malloc(taille);
    if (buffer == NULL) return -1;

    long lus = LireDonneesInode(inode, buffer, taille, 0);
    if (lus < 0) {
        free(buffer);
        return -1;
    }

    for (int i = 0; i < rep->capacite; i++) {
        rep->table[i]->nomEntree[0] = '\0';
        rep->table[i]->numeroInode = 0;
    }

    int pos = 0;
    int count = 0;

    for (int i = 0; i < nbEntrees; i++) {
        struct sEntreesRepertoire tmp;
        memcpy(&tmp, buffer + pos, sizeof(struct sEntreesRepertoire));
        pos += sizeof(struct sEntreesRepertoire);

        if (tmp.nomEntree[0] == '\0') continue;

        strncpy(rep->table[count]->nomEntree, tmp.nomEntree, TAILLE_NOM_FICHIER);
        rep->table[count]->nomEntree[TAILLE_NOM_FICHIER] = '\0';
        rep->table[count]->numeroInode = tmp.numeroInode;
        count++;
    }

    rep->nb = count;
    free(buffer);
    return 0;
}

/* V4
 * Écrit un répertoire dans un inode.
 * Entrées : le répertoire source et l'inode destination
 * Sortie : 0 si le répertoire est écrit avec succès, -1 en cas d'erreur
 */
int EcrireRepertoireDansInode(tRepertoire rep, tInode inode)
{
    if (rep == NULL || inode == NULL) return -1;

    long taille = rep->nb * sizeof(struct sEntreesRepertoire);

    if (rep->nb == 0) {
        long nb = EcrireDonneesInode(inode, (unsigned char *)"", 0, 0);
        return (nb >= 0) ? 0 : -1;
    }

    unsigned char *buffer = malloc(taille);
    if (buffer == NULL) return -1;

    int pos = 0;
    for (int i = 0; i < rep->nb; i++) {
        struct sEntreesRepertoire tmp;
        strncpy(tmp.nomEntree, rep->table[i]->nomEntree, TAILLE_NOM_FICHIER);
        tmp.nomEntree[TAILLE_NOM_FICHIER] = '\0';
        tmp.numeroInode = rep->table[i]->numeroInode;

        memcpy(buffer + pos, &tmp, sizeof(struct sEntreesRepertoire));
        pos += sizeof(struct sEntreesRepertoire);
    }

    long nb = EcrireDonneesInode(inode, buffer, taille, 0);

    free(buffer);
    return (nb >= 0) ? 0 : -1;
}

/* V4
 * Récupère les entrées contenues dans un répertoire.
 * Entrées : le répertoire source, un tableau récupérant les numéros d'inodes des entrées du répertoire
 * Retour : le nombre d'entrées dans le répertoire
 */
int EntreesContenuesDansRepertoire(tRepertoire rep, struct sEntreesRepertoire tab[])
{
    if (rep == NULL || tab == NULL) return 0;

    for (int i = 0; i < rep->nb; i++) {
        tab[i] = *(rep->table[i]);
    }

    return rep->nb;
}

/* V4
 * Compte le nombre d'entrées d'un répertoire.
 * Entrée : le répertoire source
 * Retour : le nombre d'entrées du répertoire
 */
int NbEntreesRepertoire(tRepertoire rep)
{
    if (rep == NULL) return 0;
    return rep->nb;
}

/* V4
 * Affiche le contenu d'un répertoire.
 * Entrée : le répertoire à afficher
 * Retour : aucun
 */
void AfficherRepertoire(tRepertoire rep)
{
    if (rep == NULL) {
        printf("(repertoire NULL)\n");
        return;
    }

    printf("Repertoire (%d entrees) :\n", rep->nb);
    for (int i = 0; i < rep->nb; i++) {
        printf("  - %s  (inode %u)\n",
               rep->table[i]->nomEntree,
               rep->table[i]->numeroInode);
    }
}
