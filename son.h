#ifndef __SON_H__
#define __SON_H__

#include <SDL.h>

/* Initialisation du son */
int initialiser_son();

/* Chargement du fichier son */
int charger_son(char * nom_fichier, SDL_AudioSpec *wav_spec, Uint8 **wav_buffer, Uint32 *wav_length);

/* Conversion du fichier son */
int convertir_son(SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length, double ** donnee_son, double ** instant_son, int * taille);
// On veut pouvoir modifier les 3 derniers arguments, donc on entre des pointeurs vers ces éléments en argument -> une * en +. Lors de l'appel de la fonction, on entrera les adresses mémoires (donc &argument). On passe un pointeur pour wav_spec pour ne pas avoir à recopier la structure

/* Libération du son */
void liberer_son(Uint8 *wav_buffer);

/* Fermer le moteur audio */
void fermer_son(SDL_AudioDeviceID deviceID);

/* Jouer son */
int jouer_son(SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length, SDL_AudioDeviceID * deviceID);

#endif
