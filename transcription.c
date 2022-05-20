#include <math.h>
#include <fftw3.h>
#include <stdbool.h>

#include "transcription.h"

#define HARMONIQUES 5
#define EPS_AMPLITUDE 0.01
#define DUREE_BLOC 0.05

int calculer_spectrogramme(double * donnees_son, double * instant_son, int debut, int fin, double ** spectrogramme, double ** frequences) { //int début et int fin sont des indices qui permettront de découper le signal en bloc ne contenant qu'une note chacun. *spectrogramme est le spectrogramme qui sera rempli dans la fonction (c'est un tableau qui contiendra l'amplitude de la fft), et frequences est le tableau contenant les fréquences associées au spectrogramme.
  int bloc = fin - debut; //longueur du segment de signal qu'on étudie (nombre d'indices considérés)
  double * signal_bloc = malloc(bloc * sizeof(double)); // On alloue la mémoire pour stocker le bloc de signal réel.
  if (signal_bloc == NULL) {
    return 1;
  }
  fftw_complex * fft = malloc(bloc * sizeof(fftw_complex)); //On alloue la mémoire pour la transformée de Fourier.
  if(fft == NULL) {
    free(signal_bloc);
    return 1;
  }
  //Application de la fenêtre de Hamming
  for(int i = 0; i < bloc; ++i){
    signal_bloc[i] = donnees_son[debut + i] * (25.0 / 46.0 - 21.0 / 46.0 * cos(2 * M_PI * i / (bloc - 1.0))); // fenêtre de Hamming
  }
  //On construit le tableau des fréquences.
  double tMax = instant_son[bloc - 1]; // tMax est la durée du segment de signal considéré: la portion comprise entre instant_son[début] et instant_son[fin].
  //les instants sont répartis sur une grille régulière, donc on a translaté fin - début à bloc-0 pour déterminer l'indice de la première valeur qu'on ne prend pas. On prend donc bloc-1 comme indice.
  double df = 1.0 / tMax; //pas fréquentiel

  //Comme le signal est réel, sa fft sera symétrique (des infos utiles et leur conjugué). On peut donc ne garder que la moitié des informations, on définit donc l'indice demiBloc.
  int demiBloc = bloc / 2;


  for(int i = 0; i < demiBloc; ++i) {
    (*frequences)[i] = (i+0.5) * df; //demiBloc car la transformée de Fourier d'un signal réel est paire donc les infos sont redondantes
  }


  //On réalise la transformée de Fourier du signal.
  fftw_plan plan = fftw_plan_dft_r2c_1d(bloc, signal_bloc, fft, FFTW_ESTIMATE); //bloc = taille du signal qu'on manipule,  fft = pointeur vers le résultat de la transformée de Fourier, FFT_ESTIMATE = moyen efficace de trouver les paramètres de l'algoritme de FFT.
  fftw_execute(plan);//Calcul effectif de la FFT

  for(int i = 0; i < demiBloc; ++i) {//on part de 0 jusu'à demiBloc-1, on a donc bien demiBloc indices à balayer
    (*spectrogramme)[i] = fft[i][0]*fft[i][0] + fft[i][1]*fft[i][1]; //partie réelle ^2 + partie im. ^2
  }
  return 0;
}

int trouver_indice(double x, double * grille, int taille) {
  //Si x est en dehors de la table des fréquences:
  if (x <= grille[0]) {
    return 0;
  }
  if (x >= grille[taille - 1]) { //les tableaux vont de 0 à taille-1
    return taille - 1;
  }
  //On est sûrs que x tombe dans la grille des fréquences
  //calcul du pas de la grille
  double dx = grille[1] - grille[0];
  //Calcul du nombre de segments entre 2 fréquences du tableau Fréquences qu'on a à gauche de x
  double ratio = (x - grille[0]) / dx;
  int n = ratio; //nn*dx < x < (n+1)*dx;
  //On teste quel élément du tableau Fréquences est le plus proche de x:
  if (ratio - n >= 05) {
    ++n;
  }
  return n;
}


//Produit spectral = log(produit de i = 1 à H de mod(X(if))^2

double calculer_produit_spectral(double f, double * spectrogramme, double * frequences, int taille) { //f = fréquence fondamentale qui sert à calculer le spectrogramme, taille = taille du spectrogramme (et de fréquences): on mettra demiBloc
  double produit_spectral = 0.0;
  for(int i = 1; i <= HARMONIQUES; ++i) {
    int index = trouver_indice(i * f, frequences, taille);
    double spectre = spectrogramme[index];
    //printf("index=%d spectre=%e\n", index, spectre);
    produit_spectral += log(spectre);
  }
  return produit_spectral;
}

//On maximise le produit spectral pour trouver la fréquence fondamentale. On renvoie l'indice correspondant à la bonne fréquence dans le tableau de Fréquences
int maximiser_produit_spectral(double * spectrogramme, double* frequences, int taille, struct clavier_t* clavier) {
  double fo = clavier->touches[0].frequence; //clavier -> touche[0]: on met une flèche parce qu'on a entré un POINTEUR vers le clavier en argument. clavier -> touche[0] donne directement une touche (et pas un pointeur vers une touche), du coup pour accéder au champ fréquence, on met un .frequence
  int indice = 0;
  double produit_max = calculer_produit_spectral(fo, spectrogramme, frequences, taille);
  //printf("fréquence=%e produit=%e\n", fo, produit_max);
  for(int i = 1; i < NOMBRE_TOUCHES; ++i) {
    double f = clavier->touches[i].frequence;
    double produit = calculer_produit_spectral(f, spectrogramme, frequences, taille);
    //printf("fréquence=%e produit=%e\n", f, produit);
    if (produit > produit_max) {
      produit_max = produit;
      indice = i;
    }
  }//for
  return indice;
}

// Convertit un signal audio en une séquence de notes. La stratégie consiste à
// partitionner le signal en blocs de durée DUREE_BLOC. Si le signal contient assez
// d'énergie on identifie la fréquence dominante dans le bloc. Si elle correspond
// à la fréquence dominante du bloc précédent on considère qu'il s'agit de la même
// note qui se prolonge
struct liste_note_t* transcrire(double * donnees_son, double * instant_son, int taille, struct clavier_t* clavier) {
  printf("Dans transcrire\n");
  //
  // Initialisation des structures pour le calcul de note
  //
  double * carre_signal = malloc(taille * sizeof(double));
  if (carre_signal == NULL) {
    printf("Erreur: impossible d'allouer le signal au carré.\n");
    return NULL;
  }
  double dt = instant_son[1] - instant_son[0];
  int taille_bloc = DUREE_BLOC / dt;
  // On alloue un bloc éventuellement plus grand pour le dernier bloc
  int taille_allocation = taille_bloc + (taille % taille_bloc);
  double *spectrogramme = malloc(taille_allocation * sizeof(double));
  if (spectrogramme == NULL) {
    printf("Erreur: impossible d'allouer le spectrogramme.\n");
    free(carre_signal);
    return NULL;
  }
  double *frequences = malloc(taille_allocation * sizeof(double));
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
  printf("Energie=%e\n", energie);
  //
  // Détection des notes
  //
  // On va parcourir le signal par blocs de DUREE_BLOC pour chercher une note dans chaque bloc
  int debut = 0;
  struct liste_note_t *liste = NULL;
  bool fini = false;
  // Ces variables permettent de savoir si la note détectée sur le bloc courant prolonge la note précédente
  int indice_note_en_cours = -1;
  double t_debut_en_cours = 0.0;
  double t_fin_en_cours = 0.0;
  while (!fini) {
    int fin = debut + taille_bloc;
    // Le dernier bloc est plus grand si nécessaire, pour éviter d'avoir un dernier bloc trop petit
    if (fin + taille_bloc >= taille) {
      fin = taille - 1;
      fini = true;
    }
    int taille_bloc_courant = fin - debut;
    double energie_moyenne_bloc = energie * taille_bloc_courant / taille;
    //printf("Bloc %d %d energie moyenne=%e\n", debut, fin, energie_moyenne_bloc);
    // On vérifie que le bloc contient assez d'énergie pour qu'on y cherche une note
    double energie_bloc = 0.0;
    for (int i = debut; i < fin; ++i) {
      energie_bloc += carre_signal[i];
    }
    //printf("Energie bloc=%e\n", energie_bloc);
    // S'il n'y a pas assez d'énergie pour chercher une note dans le bloc c'est un silence
    if (energie_bloc < EPS_AMPLITUDE * energie_moyenne_bloc) {
      //printf("Pas assez d'énergie\n");
      // S'il y a une note en attente on l'ajoute à la liste
      if (indice_note_en_cours != -1) {
        //printf("Indice note en cours=%d\n", indice_note_en_cours);
        struct touche_t touche_freq_max = clavier->touches[indice_note_en_cours];
        printf("On stocke la note en cours, touche=%s\n", touche_freq_max.nom);
        liste = ajouter_note(liste, (Uint32)(1000 * t_debut_en_cours), (Uint32)(1000 * t_fin_en_cours), &touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
      }
      // On réinitialise la note en cours
      t_debut_en_cours = 0.0;
      t_fin_en_cours = 0.0;
      indice_note_en_cours = -1;
    } // Pas assez d'énergie dans le bloc
    else { // Assez d'énergie dans le bloc
      //printf("Calcul du spectrogramme du bloc\n");
      if (calculer_spectrogramme(donnees_son, instant_son, debut, fin, &spectrogramme, &frequences) == 0) {//spectrogramme = Transformée de Fourier du signal
        /*
        printf("[");
        for (int i = 0; i < taille_bloc_courant / 2; ++i) {
          if (i > 0) printf(", ");
          printf("(%e,%e)", spectrogramme[i], frequences[i]);
        }
        printf("]\n");
        */
        int indice_note_bloc = maximiser_produit_spectral(spectrogramme, frequences, taille_bloc_courant / 2, clavier);
        printf("indice_note_bloc=%d\n", indice_note_bloc);
        // Vérifier si la note trouvée complète la note courante
        if (indice_note_bloc == indice_note_en_cours) {
          //printf("On prolonge la note en cours\n");
          t_fin_en_cours = instant_son[fin];
          //printf("Le nouvel instant de fin de la note en cours est %e\n", t_fin_en_cours);
        } // On continue la note en cours
        // Sinon on stocke la note en cours et la note en cours passe à la note courante
        else {
          //printf("Démarrage d'une nouvelle note\n");
          if (indice_note_en_cours != -1) {
            struct touche_t touche_freq_max = clavier->touches[indice_note_en_cours];
            printf("On stocke la note en cours, touche=%s\n", touche_freq_max.nom);
            liste = ajouter_note(liste, (Uint32)(1000 * t_debut_en_cours), (Uint32)(1000 * t_fin_en_cours), &touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
          }
          t_debut_en_cours = instant_son[debut];
          t_fin_en_cours = instant_son[fin];
          indice_note_en_cours = indice_note_bloc;
          //printf("La nouvelle note en cours est %s\n", clavier->touches[indice_note_en_cours].nom);
        } // On passe à une nouvelle note en cours
      } // Si le calcul du spectrogramme s'est bien passé
    } // Assez d'énergie dans le bloc
    debut = fin;
  } // while
  // S'il reste une note en cours on l'ajoute à la liste
  if (indice_note_en_cours != -1) {
    struct touche_t touche_freq_max = clavier->touches[indice_note_en_cours];
    printf("On stocke la note en cours, touche=%s\n", touche_freq_max.nom);
    liste = ajouter_note(liste, (Uint32)(1000 * t_debut_en_cours), (Uint32)(1000 * t_fin_en_cours), &touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
  }
  return liste;
}
