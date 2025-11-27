/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 3
 * Fichier : bloc.c
 * Module de gestion des blocs de données.
 **/

#include "bloc.h"
#include <stdlib.h>   // pour malloc / free
#include <stdio.h>    // pour fprintf(stderr)
#include <string.h>   // pour memcpy

// Dans le .h : typedef unsigned char *tBloc;

/* V1
 * Crée et retourne un nouveau bloc de données.
 * Entrée : Aucune
 * Retour : le bloc créé ou NULL en cas de problème
 */
tBloc CreerBloc (void) {
  // Allocation d’un bloc de TAILLE_BLOC octets
  tBloc bloc = malloc(TAILLE_BLOC);

  // Vérification d’erreur
  if (bloc == NULL) {
    fprintf(stderr, "CreerBloc : probleme creation\n");
    return NULL;
  }

  // On initialise le bloc à zéro pour éviter des valeurs aléatoires
  memset(bloc, 0, TAILLE_BLOC);

  return bloc;
}

/* V1
 * Détruit un bloc de données.
 * Entrée : le bloc à détruire (libération mémoire allouée)
 * Retour : aucun
 */
void DetruireBloc(tBloc *pBloc) {
  // Vérifie qu'il existe bien quelque chose à détruire
  if (pBloc != NULL && *pBloc != NULL) {
    free(*pBloc);   // libération mémoire
    *pBloc = NULL;  // on met le pointeur à NULL pour éviter les pointeurs pendants
  }
}

/* V1
 * Copie dans un bloc les taille octets situés à l’adresse contenu.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : le bloc, l'adresse du contenu à copier et sa taille en octets
 * Retour : le nombre d'octets effectivement écrits dans le bloc
 */
long EcrireContenuBloc (tBloc bloc, unsigned char *contenu, long taille) {
  // Si bloc ou contenu n'existent pas, erreur
  if (bloc == NULL || contenu == NULL || taille <= 0) {
    return 0;
  }

  // On limite la taille au maximum possible
  long nbOctets = (taille > TAILLE_BLOC) ? TAILLE_BLOC : taille;

  // Copie en mémoire
  memcpy(bloc, contenu, nbOctets);

  return nbOctets;
}

/* V1
 * Copie à l'adresse contenu, les taille octets stockés dans un bloc.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : le bloc, l'adresse contenu à laquelle recopier et la taille en octets du bloc
 * Retour : le nombre d'octets effectivement lus dans le bloc
 */
long LireContenuBloc(tBloc bloc, unsigned char *contenu, long taille) {
  // Vérifications de sécurité
  if (bloc == NULL || contenu == NULL || taille <= 0) {
    return 0;
  }

  // Limite de lecture
  long nbOctets = (taille > TAILLE_BLOC) ? TAILLE_BLOC : taille;

  // Copie en mémoire
  memcpy(contenu, bloc, nbOctets);

  return nbOctets;
}

/* V3
 * Sauvegarde les données d'un bloc en les écrivant dans un fichier (sur disque).
 * Entrées : le bloc à sauvegarder, sa taille en octets, le nom du fichier cible
 * Retour : 0 en cas de succès, -1 en cas d'erreur
 */
int SauvegarderBloc(tBloc bloc, long taille, FILE *fichier){
  if (bloc == NULL || fichier == NULL || taille <= 0) {
    return -1;
  }

  /* On n'écrit jamais plus qu'un bloc */
  long nbOctets = (taille > TAILLE_BLOC) ? TAILLE_BLOC : taille;

  /* fwrite écrit des éléments de taille 1, nbOctets éléments */
  size_t nbEcrits = fwrite(bloc, 1, (size_t)nbOctets, fichier);
  if ((long)nbEcrits != nbOctets) {
    /* Erreur d'écriture */
    return -1;
  }

  return 0;
}

/* V3
 * Charge dans un bloc les données lues dans un fichier (sur disque).
 * Entrées : le bloc dans lequel charger, la taille en octets du fichier contenant les données, son nom (sur le disque))
 * Retour : 0 en cas de succès, -1 en cas d'erreur
 */
int ChargerBloc(tBloc bloc, long taille, FILE *fichier){
  if (bloc == NULL || fichier == NULL || taille <= 0) {
    return -1;
  }

  /* On ne lit jamais plus qu'un bloc */
  long nbOctets = (taille > TAILLE_BLOC) ? TAILLE_BLOC : taille;

  /* Lire depuis le fichier dans le bloc */
  size_t nbLus = fread(bloc, 1, (size_t)nbOctets, fichier);
  if ((long)nbLus != nbOctets) {
    /* Si on n'a pas lu autant d'octets que prévu, c'est considéré comme erreur ici */
    return -1;
  }

  return 0;
}
