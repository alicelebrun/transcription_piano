#ifndef __NOTE_H__
#define __NOTE_H__

#include <SDL.h>

#define NOMBRE_TOUCHES 88
#define H_BLANCHE 80 //hauteur d'une touche blanche
#define L_BLANCHE 15 //largeur d'une touche blanche
#define H_NOIRE 51
#define L_NOIRE 9
#define CENTRE 0 // Quand la touche blanche est au centre de 2 noires
#define DROITE 1 // Quand la touche blanche est à droite d'une touche noire
#define GAUCHE 2 // Quand la touche blanche est à gauche d'une touche noire
#define NOIRE  3
#define PLEINE 4 // La note blanche la plus à droite est à droite d'une blanche mais n'a pas de touche noire à sa droite.

// Structure note: deux points dans le temps et une touche
struct note_t {
  Uint32 t_debut;
  Uint32 t_fin;
  struct touche_t * touche;
};

// Liste doublement chaînée circulaire des notes présentes dans le signal
struct liste_note_t {
  struct note_t * note;
  struct liste_note_t * precedent;
  struct liste_note_t * suivant;
};

// Structure touche: un nom, une fréquence (pour l'identification), une image à afficher si la touche est active et une position sur l'écran
struct touche_t {
  char * nom;
  double frequence;
  int type; //Pour savoir si c'est une blanche à gauche / à droite / au centre par rapport à une touche noire, ou si c'est une noire.
  SDL_Rect position;
};

struct clavier_t {
  struct touche_t touches[NOMBRE_TOUCHES]; //Un clavier contient 88 touches
};

// Ajoute une note à une liste de note de façon à ce que les notes soient
// dans l'ordre croissant d'instant de début 
struct liste_note_t * ajouter_note(struct liste_note_t * liste, Uint32 t_debut, Uint32 t_fin, struct touche_t * touche);

// Supprime une note de la liste et détruit la note
struct liste_note_t * supprimer_note(struct liste_note_t * liste, struct note_t * note);

// Affiche les caractéristiques d'une note
void afficher_note(struct note_t * note);

// Affiche la liste des notes
void afficher_liste_notes(struct liste_note_t * note);

// Crée un clavier
int creer_clavier(struct clavier_t *clavier);

#endif
