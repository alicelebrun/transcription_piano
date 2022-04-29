#include "son.h"

/* Initialisation du son */
int initialiser_son() {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    SDL_Log("Erreur dans l'initialisation du son %s", SDL_GetError());
    return 1;
  }
  return 0;
}


/* Chargement du fichier son */
int charger_son(char * nom_fichier, SDL_AudioSpec *wav_spec, Uint8 **wav_buffer, Uint32 *wav_length) {
  if (SDL_LoadWAV(nom_fichier, wav_spec, wav_buffer, wav_length) == NULL) {
    SDL_Log("Le fichier %s n'a pas pu être ouvert: %s\n", nom_fichier, SDL_GetError());
    return 1;
  }
  if (wav_spec -> format != AUDIO_S16LSB) {
    SDL_Log("Le format n'est pas supporté.\n"); //SDL_Log est équivalent à un printf
    return 1;
  }
  return 0;
}

/* Conversion du fichier son */
int convertir_son(SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length, double ** donnee_son, double ** instant_son, int * taille) {
  *taille = wav_length / (2 * wav_spec -> channels);
  //on divise par 2 pour passer de 16 bits à 8 bits, et par wav_spec -> channels car c'est le nombre de cannaux.
  *donnee_son = malloc(*taille * sizeof(double));
  if (*donnee_son == NULL) {
    SDL_Log("Erreur: la mémoire n'a pas pu être allouée");
    return 1;
  }
  *instant_son = malloc(*taille * sizeof(double));
  if (*instant_son == NULL) {
    SDL_Log("Erreur: la mémoire n'a pas pu être allouée");
    free(*donnee_son);
    return 1;
  }
  double duree = (double)(*taille) / wav_spec->freq; //La durée n'est pas forcément un entier, on convertit donc la taille en double pour avoir une division entre flottants.

  //On teste si le fichier est en mono ou stéréo

  switch (wav_spec->channels) {
  case 1: // On est en mono.
    for (int i = 0; i < *taille; ++i) {
      (*instant_son)[i] = (i * duree) / (*taille - 1); //On parenthèse (i*duree) pour être sûr de faire une division entre double.
      (*donnee_son[i]) = (double)(*(short *)&wav_buffer[2 * i]) / (1 << 15); //conversion des données audio en valeurs entre -1 et 1
    } // mono
    break;
  case 2: // On est en stéréo.
    for(int i = 0; i< *taille; ++i) {
      (*instant_son)[i] = (i * duree) / (*taille - 1); //On parenthèse (i*duree) pour être sûr de faire une division entre double.
      short gauche = *(short *)&wav_buffer[4 * i];
      short droite = *(short *)&wav_buffer[4 * i + 2];
      //Cette fois, on lit toujours les octets par 2 mais tous les 2 paquets, donc 4i->4i+2 puis 4i+4->4i+6... pour la gauche et 4i+2->4i+4 puis 4i+6->4i+8... pour la droite
      (*donnee_son)[i] = (double)((int)gauche + droite) / (1<<16); //conversion des données audio en valeurs entre -1 et 1. On convertit la moyenne entre gauche et droite, d'où le (1<<16) et pas (1<<15).
    }
    break;
  default:
    SDL_Log("Le format n'est pas supporté. \n");
    return 1;
  } // switch
  return 0;
}


/*Libération du son*/
void liberer_son(Uint8 *wav_buffer){
  SDL_FreeWAV(wav_buffer);
}

/*Libération du moteur audio*/
void fermer_son(SDL_AudioDeviceID deviceID) {
  SDL_CloseAudioDevice(deviceID);
}

/*Jouer son*/
int jouer_son(SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length, SDL_AudioDeviceID * deviceID) {
  *deviceID = SDL_OpenAudioDevice(NULL, 0, wav_spec, NULL, 0);
  if (SDL_QueueAudio(*deviceID, wav_buffer, wav_length) != 0) {
    SDL_Log("Erreur: impossible de mettre le morceau de musique dans la file audio: %s\n", SDL_GetError());
    return 1;
  }
  SDL_PauseAudioDevice(*deviceID, 0);
  return 0;
}
