/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 3
 * Fichier : inode.c
 * Module de gestion des inodes.
 **/
#include "inode.h"
#include "bloc.h"
#include <stdlib.h>   // malloc / free
#include <stdio.h>    // fprintf stderr
#include <string.h>   // pour strcpy / memcpy
#include <time.h>     // time()

#define NB_BLOCS_DIRECTS 10

struct sInode
{
  // Numéro de l'inode
  unsigned int numero;
  // Le type du fichier : ordinaire, répertoire ou autre
  natureFichier type;
  // La taille en octets du fichier
  long taille;
  // Les adresses directes vers les blocs (NB_BLOCS_DIRECTS au maximum)
  tBloc blocDonnees[NB_BLOCS_DIRECTS];
  // Les dates : dernier accès à l'inode, dernière modification du fichier
  // et de l'inode
  time_t dateDerAcces, dateDerModif, dateDerModifInode;
};

/* V1
 * Crée et retourne un inode.
 * Entrées : numéro de l'inode et le type de fichier qui y est associé
 * Retour : l'inode créé ou NULL en cas de problème
 */
tInode CreerInode(int numInode, natureFichier type) {
  // Allocation de la structure
    tInode inode = malloc(sizeof(struct sInode));
    if (inode == NULL){
      fprintf(stderr, "CreerInode : probleme creation\n");
      return NULL;
    }

    // Initialisation des champs simples
    (*inode).numero = (unsigned int)numInode;
    (*inode).type = type;
    (*inode).taille = 0L;

    // Initialiser tous les blocs à NULL (aucun bloc utilisé au début)
    for (int i = 0; i < NB_BLOCS_DIRECTS; i++){
      (*inode).blocDonnees[i] = NULL;
    }

    // Initialiser les dates
    time_t maintenant = time(NULL);
    (*inode).dateDerAcces = maintenant;
    (*inode).dateDerModif = maintenant;
    (*inode).dateDerModifInode = maintenant;

    return inode;
}

/* V1
 * Détruit un inode.
 * Entrée : l'inode à détruire
 * Retour : aucun
 */
void DetruireInode(tInode *pInode) {
  if (pInode == NULL || *pInode == NULL){
    return;
  }
  // Libérer chaque bloc si il existe
  for (int i = 0; i < NB_BLOCS_DIRECTS; i++){
    if ((*(*pInode)).blocDonnees[i] != NULL){
      DetruireBloc(&((*(*pInode)).blocDonnees[i]));
    }
  }

  free(*pInode);
  *pInode = NULL;
}

/* V1
 * Récupère la date de dernier accès à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernier accès
 * Retour : la date de dernier accès à l'inode
 */
time_t DateDerAcces(tInode inode) {
  if (inode == NULL){
    return (time_t)0;
  }
  
  return (*inode).dateDerAcces;
}


/* V1
 * Récupère la date de dernière modification d'un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernière modification
 * Retour : la date de dernière modification de l'inode
 */
time_t DateDerModif(tInode inode) {
  if (inode == NULL){
    return (time_t)0;
  }
  return (*inode).dateDerModifInode;
}

/* V1
 * Récupère la date de dernière modification d'u fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernière modification du fichier associé
 * Retour : la date de dernière modification du fichier associé à l'inode
 */
time_t DateDerModifFichier(tInode inode) {
  if (inode == NULL){
    return (time_t)0;
  }

  return (*inode).dateDerModif;
}

/* V1
 * Récupère le numéro d'un inode.
 * Entrée : l'inode pour lequel on souhaite connaître le numéro
 * Retour : le numéro de l'inode
 */
unsigned int Numero(tInode inode) {
  if (inode == NULL){
    return 0u;
  }
  return (*inode).numero;
}

/* V1
 * Récupère la taille en octets du fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la taille
 * Retour : la taille en octets du fichier associé à l'inode
 */
long Taille(tInode inode) {
  if (inode == NULL){
    return 0L;
  }

  return (*inode).taille;
}

/* V1
 * Récupère le type du fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître le tyep de fichier associé
 * Retour : le type du fichier associé à l'inode
 */
natureFichier Type(tInode inode) {
  if (inode == NULL){
    return AUTRE;
  }
  return (*inode).type;
}

/* V1 & V3
 * Affiche les informations d'un inode
 * Entrée : l'inode dont on souhaite afficher les informations
 * Retour : aucun
 */
void AfficherInode(tInode inode) {

    if (inode == NULL) {
        printf("Inode : vide\n");
        return;
    }

    printf("---- INODE %u ----\n", (*inode).numero);
    printf("Type : %d\n", (*inode).type);
    printf("Taille : %ld octets\n", (*inode).taille);

    // Conversion des timestamps en dates lisibles pour la v3 car sinon ça écrivait des timestamps UNIX.
    char buf[26];
    struct tm *tm_info;

    tm_info = localtime(&(*inode).dateDerAcces);
    strftime(buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Date accès : %s\n", buf);

    tm_info = localtime(&(*inode).dateDerModif);
    strftime(buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Date modif fichier : %s\n", buf);

    tm_info = localtime(&(*inode).dateDerModifInode);
    strftime(buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Date modif inode : %s\n", buf);
    
    // Blocs utilisés
    printf("Blocs utilises :\n");
    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
        if ((*inode).blocDonnees[i] != NULL) {
            printf("  Bloc %d : alloue\n", i);
        }
    }

    printf("-------------------\n");
}

/* V1
 * Copie dans un inode les taille octets situés à l’adresse contenu.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone à recopier et sa taille en octets
 * Retour : le nombre d'octets effectivement écrits dans l'inode ou -1 en cas d'erreur
 */
long LireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
  if (inode == NULL){
    return -1;
  }

  if (contenu == NULL){
    return -1;
  }

  if ((*inode).blocDonnees[0] == NULL){
    return 0;
  }

  (*inode).dateDerAcces = time(NULL);

  return LireContenuBloc((*inode).blocDonnees[0], contenu, taille);
}

/* V1
 * Copie à l'adresse contenu les taille octets stockés dans un inode.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone où recopier et la taille en octets de l'inode
 * Retour : le nombre d'octets effectivement lus dans l'inode ou -1 en cas d'erreur
 */
long EcrireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
  if (inode == NULL){
    return -1;
  }

  if (contenu == NULL){
    return -1;
  }

  if ((*inode).blocDonnees[0] == NULL){
    (*inode).blocDonnees[0] = CreerBloc();

    if ((*inode).blocDonnees[0] == NULL){
      return -1;
    }
  }

  time_t maintenant = time(NULL);
  (*inode).dateDerAcces = maintenant;
  (*inode).dateDerModif = maintenant;
  (*inode).dateDerModifInode = maintenant;

  long nb = EcrireContenuBloc((*inode).blocDonnees[0], contenu, taille);

  (*inode).taille = nb;

  return nb;
}

/* V3
 * Lit les données d'un inode avec décalage, et les stocke à une adresse donnée
 * Entrées : l'inode d'où les données sont lues, la zone où recopier ces données, la taille en octets
 * des données à lire et le décalage à appliquer (voir énoncé)
 * Sortie : le nombre d'octets effectivement lus, 0 si le décalage est au-delà de la taille
 */
long LireDonneesInode(tInode inode, unsigned char *contenu, long taille, long decalage) {

    if (inode == NULL || contenu == NULL) return -1;

    long tailleFichier = (*inode).taille;

    if (decalage >= tailleFichier) {
        return 0;  
    }

    long maxPossible = tailleFichier - decalage;
    if (taille > maxPossible) {
        taille = maxPossible;
    }

    long totalLus = 0;
    long pos = decalage;

    while (totalLus < taille) {

        int indiceBloc = pos / TAILLE_BLOC;
        long offsetBloc = pos % TAILLE_BLOC;

        if (indiceBloc >= NB_BLOCS_DIRECTS) break;

        tBloc bloc = (*inode).blocDonnees[indiceBloc];
        if (bloc == NULL) break;

        long restantDansBloc = TAILLE_BLOC - offsetBloc;
        long restantALire = taille - totalLus;

        long quantite = restantALire;
        if (quantite > restantDansBloc) {
            quantite = restantDansBloc;
        }

        memcpy(contenu + totalLus, bloc + offsetBloc, quantite);

        totalLus += quantite;
        pos += quantite;
    }

    (*inode).dateDerAcces = time(NULL);

    return totalLus;
}

/* V3
 * Ecrit dans un inode, avec décalage, ls données stockées à une adresse donnée
 * Entrées : l'inode où écrire le contenu, l'adesse de la zone depuis laquelle lire les données, la taille en octets
 * de ces données et le décalage à appliquer (voir énoncé)
 * Sortie : le nombre d'octets effectivement écrits, ou -1 en cas d'erreur
 */
long EcrireDonneesInode(tInode inode, unsigned char *contenu, long taille, long decalage) {

    if (inode == NULL || contenu == NULL) return -1;

    long pos = decalage;
    long totalEcrits = 0;

    long tailleMax = NB_BLOCS_DIRECTS * TAILLE_BLOC;
    if (decalage + taille > tailleMax) {
        taille = tailleMax - decalage;
    }

    if (taille <= 0) return 0;

    while (totalEcrits < taille) {

        int indiceBloc = pos / TAILLE_BLOC;
        long offsetBloc = pos % TAILLE_BLOC;

        if (indiceBloc >= NB_BLOCS_DIRECTS) break;

        if ((*inode).blocDonnees[indiceBloc] == NULL) {
            (*inode).blocDonnees[indiceBloc] = CreerBloc();
            if ((*inode).blocDonnees[indiceBloc] == NULL) return -1;
        }

        tBloc bloc = (*inode).blocDonnees[indiceBloc];

        long restantDansBloc = TAILLE_BLOC - offsetBloc;
        long restantAEcrire = taille - totalEcrits;

        long quantite = restantAEcrire;
        if (quantite > restantDansBloc) {
            quantite = restantDansBloc;
        }

        memcpy(bloc + offsetBloc, contenu + totalEcrits, quantite);

        pos += quantite;
        totalEcrits += quantite;
    }

    long nouvelleTaille = decalage + totalEcrits;
    if (nouvelleTaille > (*inode).taille) {
        (*inode).taille = nouvelleTaille;
    }

    time_t maintenant = time(NULL);
    (*inode).dateDerAcces = maintenant;
    (*inode).dateDerModif = maintenant;
    (*inode).dateDerModifInode = maintenant;

    return totalEcrits;
}

/* V3
 * Sauvegarde toutes les informations contenues dans un inode dans un fichier (sur disque,
 * et préalablement ouvert en écriture et en mode binaire)
 * Entrées : l'inode concerné, l'identificateur du fichier
 * Sortie : 0 en cas de succès, -1 en cas d'erreur
 */
int SauvegarderInode(tInode inode, FILE *fichier) {

    if (inode == NULL || fichier == NULL) return -1;

    if (fwrite(inode, sizeof(struct sInode), 1, fichier) != 1) {
        return -1;
    }

    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {

        int existe = ((*inode).blocDonnees[i] != NULL ? 1 : 0);

        if (fwrite(&existe, sizeof(int), 1, fichier) != 1) {
            return -1;
        }

        if (existe) {
            if (SauvegarderBloc((*inode).blocDonnees[i], TAILLE_BLOC, fichier) != 0) {
                return -1;
            }
        }
    }

    return 0;
}

/* V3
 * Charge toutes les informations d'un inode à partir d'un fichier (sur disque,
 * et préalablement ouvert en lecture et en mode binaire)
 * Entrées : l'inode concerné, l'identificateur du fichier
 * Sortie : 0 en cas de succès, -1 en cas d'erreur
 */
int ChargerInode(tInode *pInode, FILE *fichier) {

    if (pInode == NULL || fichier == NULL) return -1;

    *pInode = malloc(sizeof(struct sInode));
    if (*pInode == NULL) return -1;

    if (fread(*pInode, sizeof(struct sInode), 1, fichier) != 1) {
        free(*pInode);
        *pInode = NULL;
        return -1;
    }

    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {

        int existe = 0;

        if (fread(&existe, sizeof(int), 1, fichier) != 1) {
            DetruireInode(pInode);
            return -1;
        }

        if (existe == 1) {
            (*(*pInode)).blocDonnees[i] = CreerBloc();
            if ((*(*pInode)).blocDonnees[i] == NULL) {
                DetruireInode(pInode);
                return -1;
            }

            if (ChargerBloc((*(*pInode)).blocDonnees[i], TAILLE_BLOC, fichier) != 0) {
                DetruireInode(pInode);
                return -1;
            }
        } else {
            (*(*pInode)).blocDonnees[i] = NULL;
        }
    }

    return 0;
}
