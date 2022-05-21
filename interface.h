#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <SDL.h>
#include "note.h"

// Vitesse de défilement en pixels par seconde
#define VITESSE_NOTE 100
#define LARGEUR_NOTE 6
#define LARGEUR_INTERFACE 800

struct interface_t {
  SDL_Window *fenetre;
  SDL_Renderer *renderer;
  SDL_Rect position_clavier;
  SDL_Texture * texture_clavier;
  SDL_Texture * textures_touches[5];
};

// Crée l'interface graphique en retournant une structure interface
int creer_interface(struct interface_t *interface, char * nom_fichier);

// Anime l'interface à l'aide de la séquence de notes et du fichier son, pour une durée en milliseconde
void animer_interface(struct interface_t *interface, struct liste_note_t *notes, Uint32 duree, SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length);

// Détruit l'interface et les éléments qui lui sont associés
void liberer_interface(struct interface_t * interface);

#endif
