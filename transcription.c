#include "transcription.h"

int calculer_spectogramme(double * donnees_son, double * instant_son, int debut, int fin, double ** spectogramme, double ** frequences) { //int début et int fin sont des indices qui permettront de découper le signal en bloc ne contenant qu'une note chacun. *spectogramme est le spectogramme qui sera rempli dans la fonction (c'est un tableau qui contiendra l'amplitude de la fft), et frequences est le tableau contenant les fréquences associées au spectrogramme.
	int bloc = fin - début; //longueur du segment de signal qu'on étudie (nombre d'indices considérés)
	fftw_complex * signal_complexe = malloc( bloc * sizeof(fftw_complex)); //On exprime le signal réel comme un complexe de partie imaginaire nulle.
	if(signal_complexe == NULL) {
		return 1;
		}
	fftw_complex * fft = malloc( bloc * sizeof(fftw_complex)); //On alloue la mémoire pour la transformée de Fourier.
	if(fft == NULL) {
		free(signal_complexe);
		return 1;
		}
	//Conversion du dignal réel en signal complexe de partie imaginaire nulle.
	for(int i = 0; i<bloc; ++i){
		signal_complexe[i][0] = donnees_son[i]; //partie réelle
		signal_complexe[i][1] = 0;//partie imaginaire
	
	//On construit le tableau des fréquences.
	double tMax = instant[bloc -1]; // tMax est la durée du segment de signal considéré: la portion comprise entre instant_son[début] et instant_son[fin].
	//les instants sont répartis sur une grille régulière, donc on a translaté fin - début à bloc-0 pour déterminer l'indice de la première valeur qu'on ne prend pas. On prend donc bloc-1 comme indice.
	double df = 1.0/tMax; //pas fréquentiel
	for(int i = 0; i<demiBloc; ++1) {
		(*frequences)[i] = (i+0.5) * df;
		}
	
	
	
	//On réalise la transformée de Fourier du signal.
	fftw_plan plan = fftw_plan_dft_1d(bloc, signal_complexe, fft, FFTW_FORWARD, FFT_ESTIMATE); //bloc = taille du signal qu'on manipule,  fft = pointeur vers le résultat de la transformée de Fourier, FFT_FORWARD signifie qu'on passe du domaine temporel au domaine fréquentiel (et pas l'inverse), FFT_ESTIMATE = moyen efficace de trouver les paramètres de l'algoritme de FFT.
	fftw_execute(plan);//Calcul effecti de la FFT
	for(int i = 0; i< demiBloc; ++i) {//on part de 0 jusu'à demiBloc-1, on a donc bien demiBloc indices à balayer
		(*spectogramme)[i] = fft[i][0]*fft[i][0] +fft[i][1]*fft[i][1]; //partie réelle ^2 + partie im. ^2
		}
	//Comme le signal est réel, sa fft sera symétrique (des infos utiles et leur conjugué). On peut donc ne garder que la moitié des informations, on définit donc l'indice demiBloc. 
	int demiBloc = bloc/2;

	

struct liste_note_t* transcrire(double * donnees_son, double * instant_son, int taille, struct clavier_t clavier) {
	double epsilon = 0.01;
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
	i=0; //Réinitialisation du compteur
	while(donnee_son[i]*donnee_son[i] > (1 - epsilon ) * carre_amplitude_max) {
	  	++i;
	  	}
	double t_fin = instant_son[i];
	
  //On recherche la touche dont la fréquence maximise le produit spectal du signal.
	int iMax = maximiser_produit_spectral(spectrogramme, fréquence, taille, clavier); //spectrogramme = Transformée de Fourier du signal
	struct touche_t touche_freq_max = clavier->touches[iMax];
	return(inserer_note(NULL, (Uint32)(1000*t_debut), (Uint32)(1000*t_fin), &touche_freq_max)); //NULL parce que la liste est vide au début (on ne gère qu'une note), 1000*t_. parce qu'on convertit les ms en s, et (Uint32) fait une conversion de double en Uint32.
	}
