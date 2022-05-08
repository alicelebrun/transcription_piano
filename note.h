#ifndef __NOTE_H__
#define __NOTE_H__

#include <SDL.h>

#define NOMBRE_TOUCHES 88
#define Y_CLAVIER 1729 // coordonée du bord supérieur du clavier
#define H_BLANCHE 170 //hauteur d'une touche blanche
#define L_BLANCHE 33 //largeur d'une touche blanche
#define H_NOIRE 111
#define L_NOIRE 20
#define CENTRE 0 // Quand la touche blanche est au centre de 2 noires
#define DROITE 1 // Quand la touche blanche est à droite d'une touche noire
#define GAUCHE 2 // Quand la touche blanche est à gauche d'une touche noire
#define NOIRE 3
#define PLEINE 4 // La note blanche la plus à droite est à droite d'une blanche mais n'a pas de touche noire à sa droite.

struct note_t {
  Uint32 t_debut;
  Uint32 t_fin;
  struct touche_t * touche;
};

struct liste_note_t {
  struct note_t * note;
  struct liste_note_t * precedent;
  struct liste_note_t * suivant;
};

struct touche_t {
  char* nom;
  double frequence;
  int type; //Pour savoir si c'est une blanche à gauche / à droite / au centre par rapport à une touche noire, ou si c'est une noire.
  SDL_Rect position;
};

struct clavier_t {
  struct touche_t touches[NOMBRE_TOUCHES]; //Un clavier contient 88 touches
};

//Dans la fontion ajouter_note, on entre en argument les éléments constitutifs d'une note pour créer une note dans la fonction, puis créer une liste de note contenant la liste passée en argument, + la note nouvellement créée.

struct liste_note_t* ajouter_note(struct liste_note_t * liste, Uint32 t_debut, Uint32 t_fin, struct touche_t* touche);

int creer_clavier(struct clavier_t * clavier);

#endif
