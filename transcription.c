#include <math.h>
#include <fftw3.h>
#include "transcription.h"
#define HARMONIQUES 5
#define EPS_AMPLITUDE 0.01

int calculer_spectrogramme(double * donnees_son, double * instant_son, int debut, int fin, double ** spectrogramme, double ** frequences) { //int début et int fin sont des indices qui permettront de découper le signal en bloc ne contenant qu'une note chacun. *spectrogramme est le spectrogramme qui sera rempli dans la fonction (c'est un tableau qui contiendra l'amplitude de la fft), et frequences est le tableau contenant les fréquences associées au spectrogramme.
	int bloc = fin - début; //longueur du segment de signal qu'on étudie (nombre d'indices considérés)
	fftw_complex * signal_complexe = malloc( bloc * sizeof(fftw_complex)); //On alloue la mémoire pour exprimer le signal réel comme un complexe de partie imaginaire nulle.
	if(signal_complexe == NULL) {
		return 1;
		}
	fftw_complex * fft = malloc( bloc * sizeof(fftw_complex)); //On alloue la mémoire pour la transformée de Fourier.
	if(fft == NULL) {
		free(signal_complexe);
		return 1;
		}
	//Conversion du signal réel en signal complexe de partie imaginaire nulle.
	for(int i = 0; i<bloc; ++i){
		signal_complexe[i][0] = donnees_son[i]; //partie réelle
		signal_complexe[i][1] = 0;//partie imaginaire
		}
	//On construit le tableau des fréquences.
	double tMax = instant_son[bloc -1]; // tMax est la durée du segment de signal considéré: la portion comprise entre instant_son[début] et instant_son[fin].
	//les instants sont répartis sur une grille régulière, donc on a translaté fin - début à bloc-0 pour déterminer l'indice de la première valeur qu'on ne prend pas. On prend donc bloc-1 comme indice.
	double df = 1.0/tMax; //pas fréquentiel

	//Comme le signal est réel, sa fft sera symétrique (des infos utiles et leur conjugué). On peut donc ne garder que la moitié des informations, on définit donc l'indice demiBloc. 
	int demiBloc = bloc/2;


	for(int i = 0; i<demiBloc; ++1) {
		(*frequences)[i] = (i+0.5) * df; //demiBloc car la transformée de Fourier d'un signal réel est paire donc les infos sont redondantes
		}
	
	
	//On réalise la transformée de Fourier du signal.
	fftw_plan plan = fftw_plan_dft_1d(bloc, signal_complexe, fft, FFTW_FORWARD, FFT_ESTIMATE); //bloc = taille du signal qu'on manipule,  fft = pointeur vers le résultat de la transformée de Fourier, FFT_FORWARD signifie qu'on passe du domaine temporel au domaine fréquentiel (et pas l'inverse), FFT_ESTIMATE = moyen efficace de trouver les paramètres de l'algoritme de FFT.
	fftw_execute(plan);//Calcul effectif de la FFT
	
	for(int i = 0; i< demiBloc; ++i) {//on part de 0 jusu'à demiBloc-1, on a donc bien demiBloc indices à balayer
		(*spectrogramme)[i] = fft[i][0]*fft[i][0] +fft[i][1]*fft[i][1]; //partie réelle ^2 + partie im. ^2
		}
return 0;
}

int trouver_indice(double x, double * frequences, int taille) {
	//Si x est en dehors de la table des fréquences:
	if(x<=frequences[0]) {
		return 0;
		}
	if(x>=frequences[taille-1]) { //les tableaux vont de 0 à taille-1
		return taille -1;
		}
	//On est sûrs que x tombe dans la grille des fréquences
	//calcul du pas de la grille
	double dx = gille[1] - grille[0];
	//Calcul du nombre de segments entre 2 fréquences du tableau Fréquences qu'on a à gauche de x
	double ratio = (x- grille[0]) /dx;
	int n = ratio; //nn*dx < x < (n+1)*dx;
	//On teste quel élément du tableau Fréquences est le plus proche de x:
	if( ratio-n >= 05) {
		++n;
		}
	return n;
	}

	
//Produit spectral = log(produit de i = 1 à H de mod(X(if))^2

double calculer_produit_spectral(double f, double * spectrogramme, double * frequences, int taille) { //f = fréquence fondamentale qui sert à calculer le spectrogramme, taille = taille du spectrogramme (et de fréquences): on mettra demiBloc
	double produit_spectral = 0.0;
	for(int i=1; i<=H; ++i) {
		int index = trouver_indice(i*f, frequences, taille);
		produit_spectral += log(spectrogramme[index]);
		}
	return produit_spectral;
	}

//On maximise le produit spectral pour trouver la fréquence fondamentale. On renvoie l'indice correspondant à la bonne fréquence dans le tableau de Fréquences
int maximiser_produit_spectral(double * spectrogramme, double* frequences, int taille, struct clavier_t* clavier) {
	double fo = clavier->touche[0].frequence; //clavier -> touche[0]: on met une flèche parce qu'on a entré un POINTEUR vers le clavier en argument. clavier -> touche[0] donne directement une touche (et pas un pointeur vers une touche), du coup pour accéder au champ fréquence, on met un .frequence 
	int indice = 0;
	double produit_max = calculer_produit_spectral(fo, spectrogramme, frequences, taille);
	
	for(int i = 1; i<=NOMBRE_TOUCHES; ++i) {
		double f = clavier->touche[i].frequence;
		double produit = calculer_produit_spectral(f, spectrogramme, frequences, taille);
		if(produit > produit_max) {
			produit_max = produit;
			indice = i;
			}
		}//for
	return indice;
	}

struct liste_note_t* transcrire(double * donnees_son, double * instant_son, int taille, struct clavier_t* clavier) {
	double epsilon = EPS_AMPLITUDE;
	double carre_amplitude_max = 0.0;
	double somme_carre_amplitude = 0.0;
	for(int  i = 0; i<taille ; ++i) {
	  	double carre_amplitude = donnees_son[i]*donnees_son[i];
	  	somme_carre_amplitude += carre_amplitude;
	  	if(carre_amplitude > carre_amplitude_max) {
	  		carre_amplitude_max = carre_amplitude;
  		}
  	}//for i
  //On recherche l'instant de début de la note ( la première fois que carre_amplitude > eps*amplitude max)
	int i = 0;
	while(donnee_son[i]*donnee_son[i] < epsilon * carre_amplitude_max) {
	  	++i;
	  	}
  //On a trouvé l'instant de début de la note
	double t_debut = instant_son[i];
  
  //On recherche l'instant de fin de note. On prend le carré de sa norme L2 et on décide que lorsqu'on a calculé (1-eps) de sa vraie norme L2, on peut s'arrêter et on a essentiellement joué toute la note.
  
  //POURQUOI NE PAS PARTIR DE LA FIN VU QU'ON TRAITE LES NOTES 1 PAR 1?? -> il faut comparer les sommes, on compare les proportions d'énergie. On aurait aussi pu partir de la fin. Le test aurait été: while(somme_carre_amplitude < epsilon * somme_carre_amplitude_max)  (tant que tu ne vois passer que moins de epsilon fois l'énergie totale de la note, on continue.
  
	i=0; //Réinitialisation du compteur
	somme_carre_amplitude = 0;
	while(somme_carre_amplitude < (1 - epsilon ) * somme_carre_amplitude_max) {
		double carre_amplitude = donnees_son[i]*donnees_son[i];
	  	somme_carre_amplitude += carre_amplitude;
	  	++i;
	  	}
	double t_fin = instant_son[i];
	
  //On recherche la touche dont la fréquence maximise le produit spectRal du signal.
	int iMax = maximiser_produit_spectral(spectrogramme, fréquence, taille, clavier); //spectrogramme = Transformée de Fourier du signal
	struct touche_t touche_freq_max = clavier->touches[iMax];
	return(ajouter_note(NULL, (Uint32)(1000*t_debut), (Uint32)(1000*t_fin), &touche_freq_max)); //NULL parce que la liste est vide au début (on ne gère qu'une note), 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
	}
	
