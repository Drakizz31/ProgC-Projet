/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 1
 * Fichier : bloc.c
 * Module de gestion des blocs de données.
 **/

#include "bloc.h"
#include <stdlib.h>   // pour malloc / free
#include <stdio.h>    // pour fprintf(stderr, ...)
#include <string.h>   // pour memcpy


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
