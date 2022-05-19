#include <SDL.h>

#include "son.h"
#include "interface.h"
#include "transcription.h"
#include "note.h"

int main(int argc, char ** argv) {
	if(argc != 2) { //Il faut 2 arguments: le nom du programme et le nom de l'audio à traiter
		printf("même pas en rêve Raymond");
		return 1;
		}
	char* nom_fichier = argv[1]; //nom de l'audio
	
	//On initialise le son en vérifiant qu'il n'y a pas eu d'erreur.
	if(initialiser_son() != 0) {
		return 1;
		}
	
	SDL_AudioSpec wav_spec;
	Uint8 *wav_buffer;
	Uint32 wav_length;
	
	//On charge le son en vérifiant qu'il n'y a pas eu d'erreur.
	if(charger_son(nom_fichier, &wav_spec, &wav_buffer, &wav_length) != 0) {
		return 1;
		}
	
	double * donnee_son; //amplitude du signal à chaque instant
	double * instant_son; //instants reliés aux amplitudes
	int taille;
	
	//On convertit le son en vérifiant qu'il n'y a pas eu d'erreur.
	if(convertir_son(&wav_spec, wav_buffer, wav_length, &donnee_son, &instant_son, &taille) !=0) {
		return 1;
		}
	Uint32 duree = 1000 * instant_son[taille - 1];
	//On cree le clavier
	struct clavier_t clavier;
	
	creer_clavier(&clavier);
	
	//On crée une liste de notes
	struct liste_note_t * notes;
	notes = transcrire(donnee_son, instant_son, taille, &clavier);
	free(donnee_son);
	free(instant_son);
	printf("notes=%p\n", notes);
    SDL_AudioDeviceID deviceID;
    if (jouer_son(&wav_spec, wav_buffer, wav_length, &deviceID) != 0) {
      return 1;
    }
    SDL_Delay(duree);
    return 0;
}
