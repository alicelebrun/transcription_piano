#include <SDL.h>
#include "son.h"
#include "transcription.h"
#include "interface.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: transcription_piano fichier.wav\n");
    return 1;
  }
  char* nom_fichier = argv[1]; //nom de l'audio

  //On initialise le son en vérifiant qu'il n'y a pas eu d'erreur.
  if (initialiser_son() != 0) {
    return 1;
  }

  SDL_AudioSpec wav_spec;
  Uint8 *wav_buffer;
  Uint32 wav_length;

  //On charge le son en vérifiant qu'il n'y a pas eu d'erreur.
  if (charger_son(nom_fichier, &wav_spec, &wav_buffer, &wav_length) != 0) {
    return 1;
  }

  double * donnees_son; //amplitude du signal à chaque instant
  double * instants_son; //instants reliés aux amplitudes
  int taille;

  //On convertit le son en vérifiant qu'il n'y a pas eu d'erreur.
  if(convertir_son(&wav_spec, wav_buffer, wav_length, &donnees_son, &instants_son, &taille) !=0) {
    return 1;
  }
  Uint32 duree = 1000 * instants_son[taille - 1];
  //On cree le clavier
  struct clavier_t clavier;

  creer_clavier(&clavier);

  // Crée la transcription du signal en liste de notes
  printf("Transcrire musique\n");
  struct liste_note_t * notes = transcrire(donnees_son, instants_son, taille, &clavier);
  // On libère les structures du signal
  free(donnees_son);
  free(instants_son);
  afficher_liste_notes(notes);
  // Crée l'interface graphique
  struct interface_t interface;
  printf("Créer interface\n");
  if (creer_interface(&interface) != 0) {
    printf("Erreur: impossible de créer l'interface graphique.\n");
    return 1;
  };
  // Lance l'animation des notes transcrites
  printf("Animer interface\n");
  animer_interface(&interface, notes, duree, &wav_spec, wav_buffer, wav_length);
  // Libère toutes les ressources
  liberer_son(wav_buffer);
  liberer_interface(&interface);
  SDL_Quit();
  return 0;
}
