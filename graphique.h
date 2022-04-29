#ifndef __GRAPHIQUE_H__
#define __GRAPHIQUE_H__
#include <SDL.h>

//Cette fonction vérifie qu'on peut lire/écrire des fichiers image et d'afficher des pixels à l'écran.
int initialiser_graphique();

// Cette fonction permet de charger une image
int charger_image(char* nom_image, SDL_Surface **surface);

// Conversion image -> texture (=image traitée pour être affichée efficacement à l'écran)
int convertir_vers_texture(SDL_Surface * image, SDL_Renderer * renderer, SDL_Texture ** texture);

/* Charge le fichier png dans une texture SDL2 */
int charger_texture(char *nom_image, SDL_Renderer * renderer, SDL_Texture **texture);
/* Copie une texture à une position donnée dans le moteur de rendu */
int copier_texture(SDL_Texture *texture, SDL_Rect *position, SDL_Renderer *renderer);
/* Libère une texture SDL2 */
void liberer_texture(SDL_Texture *texture);

/* Libère une image SDL2 */
void liberer_surface(SDL_Surface *image);

/* Libère le moteur audio SDL2 */
void fermer_graphique();

#endif
