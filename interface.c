#include "interface.h"
#include "graphique.h"

int creer_interface(struct interface_t * interface){
  if (initialiser_graphique() != 0) {
    return 1;
  }
  SDL_Surface *image;
  if (charger_image("Images/clavier.png", &image) !=0) {
    return 1;
  }
  interface->fenetre = SDL_CreateWindow("Transcripton Piano", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->w, image->h, SDL_WINDOW_OPENGL);
  //SDL_Create_Window(nom de la fenêtre, centrée en x, centrée en y, de largeur la largeur de l'image, de hauteur la hauteur de l'image, moyen d'afficher l'image efficacement);
  if (interface->fenetre == NULL) {
    SDL_Log("Erreur: impossible de charger la fenêtre.\n");
    liberer_surface(image);
    return 1;
  }
  //Création du moteur de rendu
  interface->renderer = SDL_CreateRenderer(interface->fenetre, -1, SDL_RENDERER_ACCELERATED);
  //Le -1 est une valeur par défaut.
  if (interface->renderer == NULL) {
    SDL_Log("Erreur: impossible de créer le moteur de rendu.\n");
    liberer_surface(image);
    SDL_DestroyWindow(interface->fenetre);
    return 1;
  }

  if (convertir_vers_texture(image, interface->renderer, &interface->clavier) != 0){
    return 1;
  }

  char * noms_touches[5] = {"Images/touche_centre.png", "Images/touche_droite.png", "Images/touche_gauche.png", "Images/touche_noire.png", "Images/touche_pleine.png"};
  for (int i = 0; i < 5; ++i) {
    if (charger_texture(noms_touches[i], interface->renderer, &interface->touches[i])!=0) {
      for (int j = 0; j < i; ++j) {
        liberer_texture(interface->touches[j]);
      } // for j
    } // if
  } // for i
  return 0;
}

/*
void animer_interface(struct interface_t* interface, struct liste_note_t * liste, Uint32 durée); //durée sera un miliseconde

void liberer_interface(struct interface_t* interface);
*/
