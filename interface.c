#include <stdbool.h>
#include <SDL.h>
#include "son.h"
#include "graphique.h"
#include "interface.h"

// Crée l'interface graphique en retournant une structure interface
int creer_interface(struct interface_t *interface) {
  // Initialisation du moteur graphique
  if (initialiser_graphique() != 0) {
    return 1;
  }
  // Initialisation de la gestion des événements
  if (SDL_Init(SDL_INIT_EVENTS) != 0) {
    printf("Erreur: impossible d'initialiser la gestion des événements : %s\n", SDL_GetError());
    return 1;
  }
  // Création de la fenêtre de l'application
  interface->fenetre = SDL_CreateWindow("Transcription piano", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_INTERFACE, HAUTEUR_INTERFACE, SDL_WINDOW_OPENGL);
  if (interface->fenetre == NULL) {
    SDL_Log("Erreur: impossible de créer la fenêtre.\n");
    return 1;
  }
  // Création du moteur de rendu
  interface->renderer = SDL_CreateRenderer(interface->fenetre, -1, SDL_RENDERER_ACCELERATED);
  if (interface->renderer == NULL) {
    SDL_Log("Erreur: impossible de créer le moteur de rendu.\n");
    SDL_DestroyWindow(interface->fenetre);
    return 1;
  }
  // La surface associée au clavier n'est plus utile
  if (charger_texture("Images/clavier.png", interface->renderer, &interface->texture_clavier) != 0) {
    SDL_Log("Erreur: impossible de charger la texture du clavier.\n");
    SDL_DestroyWindow(interface->fenetre);    
  }
  interface->position_clavier.x = 0;
  interface->position_clavier.y = Y_CLAVIER;
  interface->position_clavier.w = 800;
  interface->position_clavier.h = 88;
  // On lit les textures associées aux différents types de touches
  // Pour chacune des textures de touche active
  // Charge l'image correspondante
  char *noms_textures_touches[5] = {"Images/touche_centre.png",
    "Images/touche_droite.png",
    "Images/touche_gauche.png",
    "Images/touche_noire.png",
    "Images/touche_pleine.png"};
  for (int i = 0; i < 5; ++i) {
    if (charger_texture(noms_textures_touches[i], interface->renderer, &interface->textures_touches[i]) != 0) {
      for (int j = 0; j < i; ++j)
        liberer_texture(interface->textures_touches[j]);
      SDL_Log("Erreur: impossible de charger la texture des touches.\n");
      liberer_texture(interface->texture_clavier);
      SDL_DestroyWindow(interface->fenetre);    
      return 1;
    } // charger_texture
  } // for i
  return 0;
}

// Anime l'interface à l'aide de la séquence de notes et du fichier son
void animer_interface(struct interface_t *interface, struct liste_note_t *liste, Uint32 duree, SDL_AudioSpec *wav_spec, Uint8 *wav_buffer, Uint32 wav_length) {
  Uint32 delai_interface = DELAI_INTERFACE;;
  Uint32 t_demarrage = SDL_GetTicks();
  bool son_demarre = false;
  SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, wav_spec, NULL, 0);
  Uint32 t_actuel;
  bool fini = false;
  while (!fini) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        fini = true;
        break;
      }
    }
    struct liste_note_t * tete = liste;
    t_actuel = SDL_GetTicks() - t_demarrage;
    if (!son_demarre && t_actuel >= delai_interface) {
      if (jouer_son(wav_spec, wav_buffer, wav_length, deviceId) != 0) {
        fini = true;
        break;
      }
      son_demarre = true;
    }
    Uint32 t_horizon = t_actuel + delai_interface;
    SDL_SetRenderDrawColor(interface->renderer, 0, 0, 0, 255);
    SDL_RenderClear(interface->renderer);
    SDL_RenderCopy(interface->renderer, interface->texture_clavier, NULL, &interface->position_clavier);
    // On arrête le parcours de la liste à la fin de la liste
    // ou dès qu'on rencontre une note trop dans le futur
    // car les notes sont triées par date de début croissantes
    bool trop_tot = false;
    bool trop_tard = false;
    while (!fini) {
      t_actuel = SDL_GetTicks() - t_demarrage;
      t_horizon = t_actuel + delai_interface;
      struct note_t * note = tete->note;
      Uint32 t_debut = note->t_debut + delai_interface;
      Uint32 t_fin   = note->t_fin + delai_interface;
      trop_tot  = t_debut > t_actuel + delai_interface;
      trop_tard = t_fin <= t_actuel;
      //printf("trop tot=%s trop tard=%s\n", trop_tot ? "true" : "false", trop_tard ? "true" : "false");
      // Quelque chose à afficher
      if (!trop_tot && !trop_tard) {
        // La touche est active
        if (t_actuel >= t_debut) {
          copier_texture(interface->textures_touches[note->touche->type], &note->touche->position, interface->renderer);
        }
        SDL_Rect rect;
        // X rectangle
        rect.x = note->touche->position.x + (note->touche->position.w - LARGEUR_NOTE) / 2;
        // w rectangle
        rect.w = LARGEUR_NOTE;
        // y et h rectangle
        Uint32 t_min = (t_actuel  > t_debut ?  t_actuel : t_debut);
        Uint32 t_max = (t_horizon < t_fin   ? t_horizon :   t_fin);
        rect.h = ((t_max - t_min) * VITESSE_NOTE) / 1000;
        if (rect.h > 0) {
          rect.y = ((t_horizon - t_max) * VITESSE_NOTE) / 1000;
          if (rect.y + rect.h >= Y_CLAVIER) {
            rect.h = Y_CLAVIER - rect.y;
          }
          if (note->touche->type == NOIRE) {
            SDL_SetRenderDrawColor(interface->renderer, 170, 255, 136, 255);
          }
          else {
            SDL_SetRenderDrawColor(interface->renderer, 255, 238, 51, 255);
          }
          SDL_RenderFillRect(interface->renderer, &rect);
        } // rect.h > 0
      }
      // On supprime la note de la liste si sa date de fin est antérieure à l'instant présent
      if (trop_tard) {
        liste = supprimer_note(liste, note);
      }
      tete = tete->suivant;
      fini = (liste == NULL) || (tete == liste) || trop_tot;
    } // while tete != NULL
    SDL_RenderPresent(interface->renderer);
    Uint32 t_fin_anim = SDL_GetTicks() - t_demarrage;
    if (t_fin_anim < t_actuel + 33) {
      SDL_Delay(t_actuel + 33 - t_fin_anim);
    }
    fini = liste == NULL;
  } // while (!fini)
  if (t_actuel > duree + delai_interface) {
    SDL_Delay(t_actuel - (duree + delai_interface));
  }
  fermer_son(deviceId);
}

// Détruit l'interface et les éléments qui lui sont associés
void liberer_interface(struct interface_t * interface) {
  SDL_DestroyRenderer(interface->renderer);
  SDL_DestroyWindow(interface->fenetre);
}
