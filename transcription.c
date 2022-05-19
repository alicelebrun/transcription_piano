#include <math.h>
#include <fftw3.h>
#include <stdbool.h>

#include "transcription.h"

#define HARMONIQUES 5
#define EPS_AMPLITUDE 0.01
#define DUREE_BLOC 1.0

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
    signal_bloc[i] = donnees_son[i] * (25.0 / 46.0 - 21.0 / 46.0 * cos(2 * M_PI * i / (bloc - 1.0))); // fenêtre de Hamming
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
  if(x <= grille[0]) {
    return 0;
  }
  if(x >= grille[taille-1]) { //les tableaux vont de 0 à taille-1
    return taille -1;
  }
  //On est sûrs que x tombe dans la grille des fréquences
  //calcul du pas de la grille
  double dx = grille[1] - grille[0];
  //Calcul du nombre de segments entre 2 fréquences du tableau Fréquences qu'on a à gauche de x
  double ratio = (x- grille[0]) / dx;
  int n = ratio; //nn*dx < x < (n+1)*dx;
  //On teste quel élément du tableau Fréquences est le plus proche de x:
  if(ratio - n >= 05) {
    ++n;
  }
  return n;
}


//Produit spectral = log(produit de i = 1 à H de mod(X(if))^2

double calculer_produit_spectral(double f, double * spectrogramme, double * frequences, int taille) { //f = fréquence fondamentale qui sert à calculer le spectrogramme, taille = taille du spectrogramme (et de fréquences): on mettra demiBloc
  double produit_spectral = 0.0;
  for(int i = 1; i <= HARMONIQUES; ++i) {
    int index = trouver_indice(i * f, frequences, taille);
    produit_spectral += log(spectrogramme[index]);
  }
  return produit_spectral;
}

//On maximise le produit spectral pour trouver la fréquence fondamentale. On renvoie l'indice correspondant à la bonne fréquence dans le tableau de Fréquences
int maximiser_produit_spectral(double * spectrogramme, double* frequences, int taille, struct clavier_t* clavier) {
  double fo = clavier->touches[0].frequence; //clavier -> touche[0]: on met une flèche parce qu'on a entré un POINTEUR vers le clavier en argument. clavier -> touche[0] donne directement une touche (et pas un pointeur vers une touche), du coup pour accéder au champ fréquence, on met un .frequence
  int indice = 0;
  double produit_max = calculer_produit_spectral(fo, spectrogramme, frequences, taille);

  for(int i = 1; i <= NOMBRE_TOUCHES; ++i) {
    double f = clavier->touches[i].frequence;
    double produit = calculer_produit_spectral(f, spectrogramme, frequences, taille);
    if(produit > produit_max) {
      produit_max = produit;
      indice = i;
    }
  }//for
  return indice;
}

//Cherche une note dans l'intervalle debut, fin-1$
int trouver_note(double * donnees_son, double * instant_son, int debut, int fin, double carre_amplitude_max, double energie, struct clavier_t* clavier, double *t_debut, double *t_fin, int *index) {
  //On recherche l'instant de début de la note ( la première fois que carre_amplitude > eps*amplitude max)
  int i = debut;
  while ((i < fin) && donnees_son[i] * donnees_son[i] < EPS_AMPLITUDE * carre_amplitude_max) {
    ++i;
  }
  printf("Cherche debut, i=%d\n", i);
  //Pas de note dans l'intervalle
  if (i == fin) {
    return 1;
  }
  //On a trouvé l'instant de début de la note
  *t_debut = instant_son[i];

  //On recherche l'instant de fin de note. On prend le carré de sa norme L2 et on décide que lorsqu'on a calculé (1-eps) de sa vraie norme L2, on peut s'arrêter et on a essentiellement joué toute la note.

  //POURQUOI NE PAS PARTIR DE LA FIN VU QU'ON TRAITE LES NOTES 1 PAR 1?? -> il faut comparer les sommes, on compare les proportions d'énergie. On aurait aussi pu partir de la fin. Le test aurait été: while(somme_carre_amplitude < epsilon * somme_carre_amplitude_max)  (tant que tu ne vois passer que moins de epsilon fois l'énergie totale de la note, on continue.

  i = fin - 1; //Réinitialisation du compteur
  double somme_carre_amplitude = 0.0;
  while((i > 0) && (somme_carre_amplitude < EPS_AMPLITUDE * energie)) {
    double carre_amplitude = donnees_son[i] * donnees_son[i];
    somme_carre_amplitude += carre_amplitude;
    --i;
  }
  printf("Cherche fin, i=%d\n", i);
  *t_fin = instant_son[i];

  //Si la note commence et se termine au même moment, c'est que l'intervalle ne contient pas de signal utile
  if (*t_fin == *t_debut) {
    return 1;
  }
  //On recherche la touche dont la fréquence maximise le produit spectral du signal si t_fin>t_debut
  printf("t_debut=%e t_fin=%e\n", *t_debut, *t_fin);
  int bloc = fin - debut;
  double *spectrogramme = malloc(bloc * sizeof(double));
  if (spectrogramme == NULL) {
    return 1;
  }
  double *frequences = malloc(bloc * sizeof(double));
  if (frequences == NULL) {
    free(spectrogramme);
    return 1;
  }
  if (calculer_spectrogramme(donnees_son, instant_son, debut, fin, &spectrogramme, &frequences) != 0) {
    free(spectrogramme);
    free(frequences);
    return 1;
  }
  *index = maximiser_produit_spectral(spectrogramme, frequences, bloc, clavier); //spectrogramme = Transformée de Fourier du signal
  free(spectrogramme);
  free(frequences);
  return 0;
}

struct liste_note_t* transcrire(double * donnees_son, double * instant_son, int taille, struct clavier_t* clavier) {
  printf("Dans transcrire\n");
  double carre_amplitude_max = 0.0;
  double energie = 0.0;
  for(int i = 0; i < taille ; ++i) {
    double carre_amplitude = donnees_son[i] * donnees_son[i];
    energie += carre_amplitude;
    if(carre_amplitude > carre_amplitude_max) {
      carre_amplitude_max = carre_amplitude;
    }
  }//for i
  printf("Amplitude max (carre)=%e energie=%e\n", carre_amplitude_max, energie);
  // On va parcourir le signal par blocs de DUREE_BLOC pour chercher une note dans chaque bloc
  double dt = instant_son[1] - instant_son[0];
  int bloc = DUREE_BLOC / dt;
  printf("bloc=%d taille=%d\n", bloc, taille);
  int debut = 0;
  struct liste_note_t *liste = NULL;
  bool fini = false;
  while (!fini) {
    int fin = debut + bloc;
    //Le dernier bloc est plus grand si nécessaire, pour éviter d'avoir un dernier bloc trop petit
    if (fin + bloc >= taille) {
      fin = taille - 1;
      fini = true;
    }
    printf("debut=%d fin=%d\n", debut, fin);
    double t_debut = 0.0;
    double t_fin = 0.0;
    int index = 0;
    //S'il y a une note dans l'intervalle on l'ajoute
    if (trouver_note(donnees_son, instant_son, debut, fin, carre_amplitude_max, energie * (fin - debut) / taille, clavier, &t_debut, &t_fin, &index) == 0) {
      struct touche_t touche_freq_max = clavier->touches[index];
      liste = ajouter_note(liste, (Uint32)(1000 * t_debut), (Uint32)(1000 * t_fin), &touche_freq_max); // 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
    }
    debut = fin;
  } // while
  return liste;
}
