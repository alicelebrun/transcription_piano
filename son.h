#ifndef __SON_H__
#define __SON_H__

#include <SDL.h>

/* Initialisation du son */
int initialiser_son();

/* Chargement du fichier son */
int charger_son(char * nom_fichier, SDL_AudioSpec *wav_spec, Uint8 **wav_buffer, Uint32 *wav_length); //crée une structure SDL_AudioSpec contenant les informations utiles de l'audio

/* Conversion du fichier son */
int convertir_son(SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length, double ** donnee_son, double ** instant_son, int * taille); //crée deux tableaux contenant les amplitudes de l'audio pour l'un, et les instants d'échantillonnage pour l'autre.

/* Libération du son */
void liberer_son(Uint8 *wav_buffer); //Libère la mémoire allouée pour traiter l'audio.

/* Fermer le moteur audio */
void fermer_son(SDL_AudioDeviceID deviceID);

/* Jouer son */
int jouer_son(SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length, SDL_AudioDeviceID deviceID);

#endif
