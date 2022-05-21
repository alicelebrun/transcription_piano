#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <SDL.h>
#include "note.h"

struct interface_t {
  SDL_Window *fenetre;
  SDL_Renderer *renderer;
  SDL_Texture *clavier;
  SDL_Texture *touches[5];
};

int creer_interface(struct interface_t * interface);

void animer_interface(struct interface_t* interface, struct liste_note_t * liste, Uint32 durée); //durée sera un miliseconde

void liberer_interface(struct interface_t* interface);

#endif
