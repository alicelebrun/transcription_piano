#include <string.h>
#include <math.h>
#include <fftw3.h>
#include <stdbool.h>

#include "transcription.h"

// Paramètres de l'algorithme de transcription
static int HARMONIQUES = 2; // Nombre d'harmoniques dand le produit spectral
static double EPS_AMPLITUDE = 0.02; // Seuil de détection de note dans un bloc
static int LARGEUR_SOUS_BLOC = 1536; // Taille des sous_blocs
static int NOMBRE_SOUS_BLOCS = 21; // Nombre de sous_blocs se chevauchant à 50%

//#define SAUVE_DANS_FICHIER

int calculer_spectrogramme(double * donnees_son, double * instant_son, int debut, double ** spectrogramme, double ** frequences) { //int début et int fin sont des indices qui permettront de découper le signal en bloc ne contenant qu'une note chacun. *spectrogramme est le spectrogramme qui sera rempli dans la fonction (c'est un tableau qui contiendra l'amplitude de la fft), et frequences est le tableau contenant les fréquences associées au spectrogramme.
  double * signal_bloc = malloc(LARGEUR_SOUS_BLOC * sizeof(double)); // On alloue la mémoire pour stocker le bloc de signal réel.
  if (signal_bloc == NULL) {
    printf("Erreur: impossible d'allouer le bloc de signal.");
    return 1;
  }
  double * filtre = malloc(LARGEUR_SOUS_BLOC * sizeof(double)); // On alloue la mémoire pour stocker les coefficients du filtre Hamming
  if (signal_bloc == NULL) {
    printf("Erreur: impossible d'allouer le bloc de signal.");
    free(signal_bloc);
    return 1;
  }
  fftw_complex * fft = malloc(LARGEUR_SOUS_BLOC * sizeof(fftw_complex)); //On alloue la mémoire pour la transformée de Fourier.
  if(fft == NULL) {
    printf("Erreur: impossible d'allouer le bloc de signal.");
    free(signal_bloc);
    free(filtre);
    return 1;
  }

  //On construit le tableau des fréquences.
  double tMax = instant_son[LARGEUR_SOUS_BLOC - 1]; // tMax est la durée du segment de signal considéré: la durée de LARGEUR_SOUS_BLOC
  //les instants sont répartis sur une grille régulière, donc on a translaté fin - début à bloc-0 pour déterminer l'indice de la première valeur qu'on ne prend pas. On prend donc bloc-1 comme indice.
  double df = 1.0 / tMax; //pas fréquentiel

  //Comme le signal est réel, sa fft sera symétrique (des infos utiles et leur conjugué). On peut donc ne garder que la moitié des informations, on définit donc l'indice demiBloc.
  int demiBloc = LARGEUR_SOUS_BLOC / 2;

  for(int i = 0; i < demiBloc; ++i) {
    (*frequences)[i] = i * df; //demiBloc car la transformée de Fourier d'un signal réel est paire donc les infos sont redondantes
  } // for i

  //Initialisation du spectrogramme
  for (int i = 0; i < demiBloc; ++i) {
    (*spectrogramme)[i] = 0.0;
  } // for i

  //Initialisation du filtre
  for (int i = 0; i < LARGEUR_SOUS_BLOC; ++i) {
    filtre[i] = 25.0 / 46.0 - 21.0 / 46.0 * cos(2 * M_PI * i / (LARGEUR_SOUS_BLOC - 1.0));
  } // for i

  //Boucle sur les sous-blocs
  for (int n = 0; n < NOMBRE_SOUS_BLOCS; ++n) {
    //Application de la fenêtre de Hamming
    for(int i = 0; i < LARGEUR_SOUS_BLOC; ++i) {
      signal_bloc[i] = donnees_son[debut + n * demiBloc + i] * filtre[i]; // fenêtre de Hamming
    } // for i -> fenêtrage


    //On réalise la transformée de Fourier du signal.
    fftw_plan plan;
    //Le plan peut être réutilisé tant que signal_bloc et fft sont aux mêmes adresses mémoire->gain en performance
    if (n == 0) plan = fftw_plan_dft_r2c_1d(LARGEUR_SOUS_BLOC, signal_bloc, fft, FFTW_ESTIMATE); //bloc = taille du signal qu'on manipule,  fft = pointeur vers le résultat de la transformée de Fourier, FFT_ESTIMATE = moyen efficace de trouver les paramètres de l'algoritme de FFT.
    fftw_execute(plan);//Calcul effectif de la FFT

    for(int i = 0; i < demiBloc; ++i) {//on part de 0 jusu'à demiBloc-1, on a donc bien demiBloc indices à balayer
      (*spectrogramme)[i] += fft[i][0]*fft[i][0] + fft[i][1]*fft[i][1]; //partie réelle ^2 + partie im. ^2
    } // for i -> spectrogramme
  } // for n -> moyenne sur les blocs
  return 0;
}

//Produit spectral = log(produit de i = 1 à H de mod(X(if))^2
// On ne prend que les harmoniques accessibles donc on calcule une
// moyenne pour équilibrer
double calculer_produit_spectral(int indice_f, double * spectrogramme, double * frequences, int taille) { //f = fréquence fondamentale qui sert à calculer le spectrogramme, taille = taille du spectrogramme (et de fréquences): on mettra demiBloc
  double produit_spectral = 0.0;
  for (int i = 1; i <= HARMONIQUES; ++i) {
    produit_spectral += log(spectrogramme[i * indice_f]);
  }
  return produit_spectral;
}

//On maximise le produit spectral pour trouver la fréquence fondamentale. On renvoie l'indice correspondant à la bonne fréquence dans le tableau de Fréquences
int maximiser_produit_spectral(double * spectrogramme, double* frequences, int taille, struct clavier_t* clavier) {
  int indice_max = 1;
  double produit_max = calculer_produit_spectral(indice_max, spectrogramme, frequences, taille);
  for (int indice_f = 2; indice_f < taille / HARMONIQUES; ++indice_f) {
    double produit = calculer_produit_spectral(indice_f, spectrogramme, frequences, taille);
    if (produit > produit_max) {
      produit_max = produit;
      indice_max = indice_f;
    }
  }//for
  // La fréquence qui maximise le produit
  double logF = log(frequences[indice_max]);
  // Recherche de la note de piano correspondante
  int indice_touche = 0;
  double ecart_min = fabs(logF - log(clavier->touches[indice_touche].frequence));
  for (int i = 1; i < NOMBRE_TOUCHES; ++i) {
    double ecart = fabs(logF - log(clavier->touches[i].frequence));
    if (ecart < ecart_min) {
      ecart_min = ecart;
      indice_touche = i;
    }
  }
  printf("[maximiser_produit_spectral] Fréquence=%gHz (%d) touche=%s (%d) fréquence touche=%gHz\n", exp(logF), indice_max, clavier->touches[indice_touche].nom, indice_touche, clavier->touches[indice_touche].frequence);
  return indice_touche;
}

// Convertit un signal audio en une séquence de notes. La stratégie consiste à
// partitionner le signal en blocs de largeur LARGEUR_BLOC. Si le signal contient assez
// d'énergie on identifie la fréquence dominante dans le bloc. Si elle correspond
// à la fréquence dominante du bloc précédent on considère qu'il s'agit de la même
// note qui se prolonge
struct liste_note_t* transcrire(double * donnees_son, double * instant_son, int taille, struct clavier_t* clavier) {
  //
  // Initialisation des structures pour le calcul de note
  //
  // On ajuste la taille pour qu'elle soit un multiple de LARGEUR_BLOC
  // La division est entière (pas de partie fractionnaire)
  //On lit le fichier de configuration
  FILE * fichier_configuration = fopen("transcription.cfg", "r");
  if (fichier_configuration != NULL) {
    char buffer[256];
    if (fscanf(fichier_configuration, "%s%d", buffer, &HARMONIQUES) != 2) {
      HARMONIQUES = 3;
    }
    if (fscanf(fichier_configuration, "%s%le", buffer, &EPS_AMPLITUDE) != 2) {
      EPS_AMPLITUDE = 0.02;
    }
    if (fscanf(fichier_configuration, "%s%d", buffer, &LARGEUR_SOUS_BLOC) != 2) {
      LARGEUR_SOUS_BLOC = 1024;
    }
    if (fscanf(fichier_configuration, "%s%d", buffer, &NOMBRE_SOUS_BLOCS) != 2) {
      NOMBRE_SOUS_BLOCS = 11;
    }
  } // fichier_configuration
  int LARGEUR_BLOC = (LARGEUR_SOUS_BLOC * (NOMBRE_SOUS_BLOCS + 1) / 2);
  int nombre_blocs = taille / LARGEUR_BLOC;
  taille = nombre_blocs * LARGEUR_BLOC;
  double dt = instant_son[1] - instant_son[0];
  printf("[transcrire] Taille ajustée=%gs (%d) largeur bloc=%gs (%d) largeur sous-bloc=%gs (%d) nombre sous-blocs=%d harmoniques=%d epsilon amplitude=%g\n", taille * dt, taille, LARGEUR_BLOC * dt, LARGEUR_BLOC, LARGEUR_SOUS_BLOC * dt, LARGEUR_SOUS_BLOC, NOMBRE_SOUS_BLOCS, HARMONIQUES, EPS_AMPLITUDE);
  double * carre_signal = malloc(taille * sizeof(double));
  if (carre_signal == NULL) {
    printf("Erreur: impossible d'allouer le signal au carré.\n");
    return NULL;
  }
  double *spectrogramme = malloc((LARGEUR_SOUS_BLOC / 2) * sizeof(double));
  if (spectrogramme == NULL) {
    printf("Erreur: impossible d'allouer le spectrogramme.\n");
    free(carre_signal);
    return NULL;
  }
  double *frequences = malloc((LARGEUR_SOUS_BLOC / 2) * sizeof(double));
  if (frequences == NULL) {
    printf("Erreur: impossible d'allouer les fréquences.\n");
    free(carre_signal);
    free(spectrogramme);
    return NULL;
  }
  //
  // Calcul d'énergie du signal
  //
  double energie = 0.0;
  for(int i = 0; i < taille ; ++i) {
    double carre = donnees_son[i] * donnees_son[i];
    carre_signal[i] = carre;
    energie += carre;
  }//for i
  //
  // Détection des notes
  //
  // On va parcourir le signal par blocs de DUREE_BLOC pour chercher une note dans chaque bloc
  int debut = 0;
  struct liste_note_t *liste = NULL;
  // Ces variables permettent de savoir si la note détectée sur le bloc courant prolonge la note précédente
  int indice_note_en_cours = -1;
  double t_debut_en_cours = 0.0;
  double t_fin_en_cours = 0.0;
  for (int index_bloc = 0; index_bloc < nombre_blocs; ++index_bloc) {
    int fin = debut + LARGEUR_BLOC;
    double energie_moyenne_bloc = energie * LARGEUR_BLOC / taille;
    // On vérifie que le bloc contient assez d'énergie pour qu'on y cherche une note
    double energie_bloc = 0.0;
    for (int i = debut; i < fin; ++i) {
      energie_bloc += carre_signal[i];
    }
    // S'il n'y a pas assez d'énergie pour chercher une note dans le bloc c'est un silence
    if (energie_bloc < EPS_AMPLITUDE * energie_moyenne_bloc) {
      // S'il y a une note en attente on l'ajoute à la liste
      if (indice_note_en_cours != -1) {
        struct touche_t *touche_freq_max = &clavier->touches[indice_note_en_cours];
        liste = ajouter_note(liste, (Uint32)(1000 * t_debut_en_cours), (Uint32)(1000 * t_fin_en_cours), touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
      }
      // On réinitialise la note en cours
      t_debut_en_cours = 0.0;
      t_fin_en_cours = 0.0;
      indice_note_en_cours = -1;
    } // Pas assez d'énergie dans le bloc
    else { // Assez d'énergie dans le bloc
      if (calculer_spectrogramme(donnees_son, instant_son, debut, &spectrogramme, &frequences) == 0) { // spectrogramme = module carré transformée de Fourier du signal
#ifdef SAUVE_DANS_FICHIER
        char filename[256] = "spectrogramme";
        sprintf(&filename[13], "%d", index_bloc);
        strcat(filename, ".csv");
        FILE * file = fopen(filename, "w");
        for (int i = 0; i < LARGEUR_SOUS_BLOC / 2; ++i) {
          fprintf(file, "%.20e;%.20e;%.20e\n", frequences[i], spectrogramme[i], calculer_produit_spectral(i, spectrogramme, frequences, LARGEUR_SOUS_BLOC / 2));
        }
        fclose(file);
        strcpy(filename, "signal");
        sprintf(&filename[6], "%d", index_bloc);
        strcat(filename, ".csv");
        file = fopen(filename, "w");
        for (int i = 0; i < LARGEUR_BLOC; ++i) {
          fprintf(file, "%.20e;%.20e\n", instant_son[debut + i], donnees_son[debut + i]);
        }
        fclose(file);
#endif
        printf("[transcrire] Maximiser sur le bloc %d\n", index_bloc);
        int indice_note_bloc = maximiser_produit_spectral(spectrogramme, frequences, LARGEUR_SOUS_BLOC / 2, clavier);
        // Vérifier si la note trouvée complète la note courante
        if (indice_note_bloc == indice_note_en_cours) {
          t_fin_en_cours = instant_son[fin];
        } // On continue la note en cours
        // Sinon on stocke la note en cours et la note en cours passe à la note courante
        else {
          if (indice_note_en_cours != -1) {
            struct touche_t * touche_freq_max = &clavier->touches[indice_note_en_cours];
            printf("[transcrire] On stocke la note en cours, touche=%s\n", touche_freq_max->nom);
            liste = ajouter_note(liste, (Uint32)(1000 * t_debut_en_cours), (Uint32)(1000 * t_fin_en_cours), touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
          }
          t_debut_en_cours = instant_son[debut];
          t_fin_en_cours = instant_son[fin];
          indice_note_en_cours = indice_note_bloc;
        } // On passe à une nouvelle note en cours
      } // Si le calcul du spectrogramme s'est bien passé
    } // Assez d'énergie dans le bloc
    debut = fin;
  } // while
  // S'il reste une note en cours on l'ajoute à la liste
  if (indice_note_en_cours != -1) {
    struct touche_t *touche_freq_max = &clavier->touches[indice_note_en_cours];
    printf("[transcrire] On stocke la note en cours, touche=%s\n", touche_freq_max->nom);
    liste = ajouter_note(liste, (Uint32)(1000 * t_debut_en_cours), (Uint32)(1000 * t_fin_en_cours), touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
  }
  return liste;
}
