#include <SDL.h>
#include <SDL_image.h>

#include "graphique.h"


/* Initialise le moteur graphique SDL2 */
int initialiser_graphique()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("Erreur lors de l'initialisation du graphique: %s\n", SDL_GetError());
    return 1;
  }
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Log("Error lors de l'initialisation de la gestion des images.\n");
    return 1;
  }
  return 0;
}

/* Charge le fichier png dans une texture SDL2 */
int charger_texture(char *nom_image, SDL_Renderer * renderer, SDL_Texture **texture)
{
  SDL_Surface *image = IMG_Load(nom_image);
  if (image == NULL) {
    SDL_Log("Erreur lors du chargement de l'image %s: %s\n", nom_image, IMG_GetError());
    return 1;
  }
  *texture = SDL_CreateTextureFromSurface(renderer, image);
  if (*texture == NULL) {
    SDL_Log("Erreur: impossible de créer la texture associée à la surface.\n");
    SDL_FreeSurface(image);
    return 1;
  }
  SDL_FreeSurface(image);
  if (SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND) != 0) {
    SDL_Log("Erreur: impossible de prendre en compte le cannal alpha de la texture: %s\n", SDL_GetError());
    liberer_texture(*texture);
    return 1;
  }
  return 0;
}

/* Copie une texture à une position donnée dans le moteur de rendu */
int copier_texture(SDL_Texture *texture, SDL_Rect *position, SDL_Renderer *renderer) {
  if (SDL_RenderCopy(renderer, texture, NULL, position) != 0) {
    SDL_Log("Erreur: impossible de copier la texture: %s\n", SDL_GetError());
    return 1;
  }
  return 0;
}

/* Libère une texture SDL2 */
void liberer_texture(SDL_Texture *texture) {
  SDL_DestroyTexture(texture);
}

/* Libère le moteur audio SDL2 */
void fermer_graphique() {
  IMG_Quit();
}
