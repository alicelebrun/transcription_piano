#include <stdbool.h>
#include <SDL.h>
#include "son.h"
#include "graphique.h"
#include "interface.h"

// Crée l'interface graphique en retournant une structure interface
int creer_interface(struct interface_t *interface, char *nom_fichier) {
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
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  int position_fenetre = 0;
  Uint32 hauteur_ecran = DM.h-120;
  if (hauteur_ecran > 1000) {
    hauteur_ecran = 1000;
    position_fenetre = SDL_WINDOWPOS_CENTERED;
  }
  char nom_fenetre[256] = "Transcription piano - ";
  strcat(nom_fenetre, nom_fichier);
  interface->fenetre = SDL_CreateWindow(nom_fenetre, SDL_WINDOWPOS_CENTERED, position_fenetre, LARGEUR_INTERFACE, hauteur_ecran, SDL_WINDOW_OPENGL);
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
  interface->position_clavier.y = hauteur_ecran - 100;
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
  Uint32 delai_interface = (1000 * interface->position_clavier.y) / VITESSE_NOTE;
  Uint32 t_demarrage = SDL_GetTicks();
  bool son_demarre = false;
  SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, wav_spec, NULL, 0);
  Uint32 t_actuel = 0;
  bool animer_interface = true;
  while (animer_interface) {
    printf("                                               \r");
    printf("t=%dms", t_actuel);
    fflush(stdout);
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        animer_interface = true;
        break;
      }
    }
    struct liste_note_t * tete = liste;
    t_actuel = SDL_GetTicks() - t_demarrage;
    if (!son_demarre && t_actuel >= delai_interface) {
      if (jouer_son(wav_spec, wav_buffer, wav_length, deviceId) != 0) {
        animer_interface = true;
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
    bool dessiner_notes = true;
    while (dessiner_notes) {
      t_actuel = SDL_GetTicks() - t_demarrage;
      t_horizon = t_actuel + delai_interface;
      struct note_t * note = tete->note;
      Uint32 t_debut = note->t_debut + delai_interface;
      Uint32 t_fin   = note->t_fin + delai_interface;
      trop_tot  = t_debut > t_actuel + delai_interface;
      trop_tard = t_fin <= t_actuel;
      // Quelque chose à afficher
      if (!trop_tot && !trop_tard) {
        // La touche est active
        if (t_actuel >= t_debut) {
          SDL_Rect position = note->touche->position;
          position.y = interface->position_clavier.y + 6;
          copier_texture(interface->textures_touches[note->touche->type], &position, interface->renderer);
          printf(" note=%s", note->touche->nom);
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
          if (rect.y + rect.h >= interface->position_clavier.y) {
            rect.h = interface->position_clavier.y - rect.y;
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
      dessiner_notes = (liste != NULL) && (tete != liste) && !trop_tot;
    } // while tete != NULL
    SDL_RenderPresent(interface->renderer);
    printf("\r");
    fflush(stdout);
    Uint32 t_fin_anim = SDL_GetTicks() - t_demarrage;
    if (t_fin_anim < t_actuel + 33) {
      SDL_Delay(t_actuel + 33 - t_fin_anim);
    }
    animer_interface = liste != NULL;
  } // while (!fini)
  if (t_actuel > duree + delai_interface) {
    SDL_Delay(t_actuel - (duree + delai_interface));
  }
  fermer_son(deviceId);
  printf("t=%dms\n", t_actuel);
}

// Détruit l'interface et les éléments qui lui sont associés
void liberer_interface(struct interface_t * interface) {
  SDL_DestroyRenderer(interface->renderer);
  SDL_DestroyWindow(interface->fenetre);
}
