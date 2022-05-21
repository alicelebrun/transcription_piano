#include "note.h"

// Crée une note et l'ajoute dans la liste par ordre croissant de date de début
struct liste_note_t * ajouter_note(struct liste_note_t * liste, Uint32 t_debut, Uint32 t_fin, struct touche_t * touche) {
  struct note_t * note = malloc(sizeof(struct note_t));
  if (note == NULL) {
    return NULL;
  }
  struct liste_note_t *nouvelle_liste = malloc(sizeof(struct liste_note_t));
  if (nouvelle_liste == NULL) {
    free(note);
    return NULL;
  }
  // Remplir la note
  note->t_debut = t_debut;
  note->t_fin = t_fin;
  note->touche = touche;
  nouvelle_liste->note = note;
  // Si la liste est vide elle devient égale à la liste constituée de la nouvelle note
  if (liste == NULL) {
    // On boucle sur le maillon unique
    nouvelle_liste->suivant = nouvelle_liste;
    nouvelle_liste->precedent = nouvelle_liste;
    return nouvelle_liste;
  }
  /* Sinon
     Insérer la note à la bonne place. On parcourt la liste en remontant par les antécédents car les notes
     sont produites essentiellement dans l'ordre chronologique, les nouvelles notes ont donc plus de chance
     de se ranger en fin de liste
     On part du précédent de la tête, c'est-à dire la queue (qui peut être égale à la tête si la liste ne
     contient qu'un élément) */
  struct liste_note_t * courant = liste;
  // Tant qu'on n'a pas bouclé et que la note démarre avant la note courante on remonte la liste
  while (courant->precedent != liste && t_debut < courant->note->t_debut) {
    courant = courant->precedent;
  }
  // Arrivé ici on a bouclé ou on a trouvé une note de la liste actuelle postérieure à la note courante
  // Si on a bouclé c'est qu'il faut mettre la note à la fin
  if (courant == liste) {
    liste->precedent->suivant = nouvelle_liste;
    nouvelle_liste->precedent = liste->precedent;
    nouvelle_liste->suivant = liste;
    liste->precedent = nouvelle_liste;
  }
  else {
    nouvelle_liste->suivant = courant;
    nouvelle_liste->precedent = courant->precedent;
    courant->precedent->suivant = nouvelle_liste;
    courant->precedent = nouvelle_liste;
  }
  return liste;
}

// Supprime une note de la liste et détruit la note
struct liste_note_t * supprimer_note(struct liste_note_t * liste, struct note_t * note) {
  if (note == NULL || liste == NULL) {
    return liste;
  }
  // Si la liste ne contient qu'un élément...
  if (liste->suivant == liste) {
    // ...et qu'il correspond à la note
    if (liste->note == note) {
      free(liste->note);
      free(liste);
      return NULL;
    }
    // Sinon rien à faire
    else return liste;
  } // Un seul élément
  // Sinon on parcourt les éléments
  struct liste_note_t * courant = liste;
  while (courant->note != note) {
    courant = courant->suivant;
    // Si on est revenu en tete de liste on quitte la boucle
    if (courant == liste) {
      break;
    }
  } // while
  // A ce point on est sorti soit parce qu'on a parcouru toute la liste en vain soit parce que la note correspond au maillon courant
  if (courant->note == note) {
    courant->suivant->precedent = courant->precedent;
    courant->precedent->suivant = courant->suivant;
    // Si on supprime la tête, la nouvelle tête est le suivant
    if (courant == liste) {
      liste = courant->suivant;
    }
    free(courant->note);
    free(courant);
  }
  return liste;
}

// Affiche les caractéristiques d'une note
void afficher_note(struct note_t * note) {
  if (note == NULL) {
    printf("note=NULL\n");
  } else {
    printf("note=(%s t_debut=%dms t_fin=%dms)\n", note->touche->nom, note->t_debut, note->t_fin);
  }
}

// Affiche la liste des notes
void afficher_liste_notes(struct liste_note_t * liste) {
  int nombre_notes = 0;
  if (liste == NULL) {
    printf("liste=NULL\n");
  } else {
    struct liste_note_t * courant = liste;
    do {
      ++nombre_notes;
      afficher_note(courant->note);
      courant = courant->suivant;
    } while(courant != liste);
  } // else
  printf("Nombre de notes=%d\n", nombre_notes);
}

// Crée un clavier
int creer_clavier(struct clavier_t *clavier) {
  // On construit maintenant les 88 touches du piano...
  clavier->touches[ 0].nom = "La-1";
  clavier->touches[ 0].frequence = 27.5;
  clavier->touches[ 0].type = GAUCHE;
  clavier->touches[ 0].position.x = 14;
  clavier->touches[ 0].position.y = 0;
  clavier->touches[ 0].position.w = L_BLANCHE;
  clavier->touches[ 0].position.h = H_BLANCHE;
  //
  clavier->touches[ 1].nom = "La-1dièse";
  clavier->touches[ 1].frequence = 29.135235094880564;
  clavier->touches[ 1].type = NOIRE;
  clavier->touches[ 1].position.x = 800.0 / 1750.0 * 37;
  clavier->touches[ 1].position.y = 0;
  clavier->touches[ 1].position.w = L_NOIRE;
  clavier->touches[ 1].position.h = H_NOIRE;
  //
  clavier->touches[ 2].nom = "Si-1";
  clavier->touches[ 2].frequence = 30.867706328507698;
  clavier->touches[ 2].type = DROITE;
  clavier->touches[ 2].position.x = 800.0 / 1750.0 * 46;
  clavier->touches[ 2].position.y = 0;
  clavier->touches[ 2].position.w = L_BLANCHE;
  clavier->touches[ 2].position.h = H_BLANCHE;
  //
  clavier->touches[ 3].nom = "Do0";
  clavier->touches[ 3].frequence = 32.70319566257477;
  clavier->touches[ 3].type = GAUCHE;
  clavier->touches[ 3].position.x = 800.0 / 1750.0 * 79;
  clavier->touches[ 3].position.y = 0;
  clavier->touches[ 3].position.w = L_BLANCHE;
  clavier->touches[ 3].position.h = H_BLANCHE;
  //
  clavier->touches[ 4].nom = "Do0dièse";
  clavier->touches[ 4].frequence = 34.64782887210895;
  clavier->touches[ 4].type = NOIRE;
  clavier->touches[ 4].position.x = 800.0 / 1750.0 * 103;
  clavier->touches[ 4].position.y = 0;
  clavier->touches[ 4].position.w = L_NOIRE;
  clavier->touches[ 4].position.h = H_NOIRE;
  //
  clavier->touches[ 5].nom = "Ré0";
  clavier->touches[ 5].frequence = 36.70809598967588;
  clavier->touches[ 5].type = CENTRE;
  clavier->touches[ 5].position.x = 800.0 / 1750.0 * 113;
  clavier->touches[ 5].position.y = 0;
  clavier->touches[ 5].position.w = L_BLANCHE;
  clavier->touches[ 5].position.h = H_BLANCHE;
  //
  clavier->touches[ 6].nom = "Ré0dièse";
  clavier->touches[ 6].frequence = 38.89087296526005;
  clavier->touches[ 6].type = NOIRE;
  clavier->touches[ 6].position.x = 800.0 / 1750.0 * 136;
  clavier->touches[ 6].position.y = 0;
  clavier->touches[ 6].position.w = L_NOIRE;
  clavier->touches[ 6].position.h = H_NOIRE;
  //
  clavier->touches[ 7].nom = "Mi0";
  clavier->touches[ 7].frequence = 41.203444614108676;
  clavier->touches[ 7].type = DROITE;
  clavier->touches[ 7].position.x = 800.0 / 1750.0 * 145;
  clavier->touches[ 7].position.y = 0;
  clavier->touches[ 7].position.w = L_BLANCHE;
  clavier->touches[ 7].position.h = H_BLANCHE;
  //
  clavier->touches[ 8].nom = "Fa0";
  clavier->touches[ 8].frequence = 43.65352892912542;
  clavier->touches[ 8].type = GAUCHE;
  clavier->touches[ 8].position.x = 800.0 / 1750.0 * 179;
  clavier->touches[ 8].position.y = 0;
  clavier->touches[ 8].position.w = L_BLANCHE;
  clavier->touches[ 8].position.h = H_BLANCHE;
  //
  clavier->touches[ 9].nom = "Fa0dièse";
  clavier->touches[ 9].frequence = 46.249302838954236;
  clavier->touches[ 9].type = NOIRE;
  clavier->touches[ 9].position.x = 800.0 / 1750.0 * 202;
  clavier->touches[ 9].position.y = 0;
  clavier->touches[ 9].position.w = L_NOIRE;
  clavier->touches[ 9].position.h = H_NOIRE;
  //
  clavier->touches[10].nom = "Sol0";
  clavier->touches[10].frequence = 48.999429497718594;
  clavier->touches[10].type = CENTRE;
  clavier->touches[10].position.x = 800.0 / 1750.0 * 211;
  clavier->touches[10].position.y = 0;
  clavier->touches[10].position.w = L_BLANCHE;
  clavier->touches[10].position.h = H_BLANCHE;
  //
  clavier->touches[11].nom = "Sol0dièse";
  clavier->touches[11].frequence = 51.91308719749307;
  clavier->touches[11].type = NOIRE;
  clavier->touches[11].position.x = 800.0 / 1750.0 * 235;
  clavier->touches[11].position.y = 0;
  clavier->touches[11].position.w = L_NOIRE;
  clavier->touches[11].position.h = H_NOIRE;
  //
  clavier->touches[12].nom = "La0";
  clavier->touches[12].frequence = 55.0;
  clavier->touches[12].type = CENTRE;
  clavier->touches[12].position.x = 800.0 / 1750.0 * 246;
  clavier->touches[12].position.y = 0;
  clavier->touches[12].position.w = L_BLANCHE;
  clavier->touches[12].position.h = H_BLANCHE;
  //
  clavier->touches[13].nom = "La0dièse";
  clavier->touches[13].frequence = 58.27047018976117;
  clavier->touches[13].type = NOIRE;
  clavier->touches[13].position.x = 800.0 / 1750.0 * 269;
  clavier->touches[13].position.y = 0;
  clavier->touches[13].position.w = L_NOIRE;
  clavier->touches[13].position.h = H_NOIRE;
  //
  clavier->touches[14].nom = "Si0";
  clavier->touches[14].frequence = 61.735412657015445;
  clavier->touches[14].type = DROITE;
  clavier->touches[14].position.x = 800.0 / 1750.0 * 278;
  clavier->touches[14].position.y = 0;
  clavier->touches[14].position.w = L_BLANCHE;
  clavier->touches[14].position.h = H_BLANCHE;
  //
  clavier->touches[15].nom = "Do1";
  clavier->touches[15].frequence = 65.40639132514958;
  clavier->touches[15].type = GAUCHE;
  clavier->touches[15].position.x = 800.0 / 1750.0 * 311;
  clavier->touches[15].position.y = 0;
  clavier->touches[15].position.w = L_BLANCHE;
  clavier->touches[15].position.h = H_BLANCHE;
  //
  clavier->touches[16].nom = "Do1dièse";
  clavier->touches[16].frequence = 69.29565774421795;
  clavier->touches[16].type = NOIRE;
  clavier->touches[16].position.x = 800.0 / 1750.0 * 335;
  clavier->touches[16].position.y = 0;
  clavier->touches[16].position.w = L_NOIRE;
  clavier->touches[16].position.h = H_NOIRE;
  //
  clavier->touches[17].nom = "Ré1";
  clavier->touches[17].frequence = 73.41619197935181;
  clavier->touches[17].type = CENTRE;
  clavier->touches[17].position.x = 800.0 / 1750.0 * 345;
  clavier->touches[17].position.y = 0;
  clavier->touches[17].position.w = L_BLANCHE;
  clavier->touches[17].position.h = H_BLANCHE;
  //
  clavier->touches[18].nom = "Ré1dièse";
  clavier->touches[18].frequence = 77.78174593052015;
  clavier->touches[18].type = NOIRE;
  clavier->touches[18].position.x = 800.0 / 1750.0 * 368;
  clavier->touches[18].position.y = 0;
  clavier->touches[18].position.w = L_NOIRE;
  clavier->touches[18].position.h = H_NOIRE;
  //
  clavier->touches[19].nom = "Mi1";
  clavier->touches[19].frequence = 82.4068892282174;
  clavier->touches[19].type = DROITE;
  clavier->touches[19].position.x = 800.0 / 1750.0 * 377;
  clavier->touches[19].position.y = 0;
  clavier->touches[19].position.w = L_BLANCHE;
  clavier->touches[19].position.h = H_BLANCHE;
  //
  clavier->touches[20].nom = "Fa1";
  clavier->touches[20].frequence = 87.30705785825089;
  clavier->touches[20].type = GAUCHE;
  clavier->touches[20].position.x = 800.0 / 1750.0 * 411;
  clavier->touches[20].position.y = 0;
  clavier->touches[20].position.w = L_BLANCHE;
  clavier->touches[20].position.h = H_BLANCHE;
  //
  clavier->touches[21].nom = "Fa1dièse";
  clavier->touches[21].frequence = 92.49860567790851;
  clavier->touches[21].type = NOIRE;
  clavier->touches[21].position.x = 800.0 / 1750.0 * 434;
  clavier->touches[21].position.y = 0;
  clavier->touches[21].position.w = L_NOIRE;
  clavier->touches[21].position.h = H_NOIRE;
  //
  clavier->touches[22].nom = "Sol1";
  clavier->touches[22].frequence = 97.99885899543723;
  clavier->touches[22].type = CENTRE;
  clavier->touches[22].position.x = 800.0 / 1750.0 * 443;
  clavier->touches[22].position.y = 0;
  clavier->touches[22].position.w = L_BLANCHE;
  clavier->touches[22].position.h = H_BLANCHE;
  //
  clavier->touches[23].nom = "Sol1dièse";
  clavier->touches[23].frequence = 103.8261743949862;
  clavier->touches[23].type = NOIRE;
  clavier->touches[23].position.x = 800.0 / 1750.0 * 467;
  clavier->touches[23].position.y = 0;
  clavier->touches[23].position.w = L_NOIRE;
  clavier->touches[23].position.h = H_NOIRE;
  //
  clavier->touches[24].nom = "La1";
  clavier->touches[24].frequence = 110.0;
  clavier->touches[24].type = CENTRE;
  clavier->touches[24].position.x = 800.0 / 1750.0 * 478;
  clavier->touches[24].position.y = 0;
  clavier->touches[24].position.w = L_BLANCHE;
  clavier->touches[24].position.h = H_BLANCHE;
  //
  clavier->touches[25].nom = "La1dièse";
  clavier->touches[25].frequence = 116.5409403795224;
  clavier->touches[25].type = NOIRE;
  clavier->touches[25].position.x = 800.0 / 1750.0 * 501;
  clavier->touches[25].position.y = 0;
  clavier->touches[25].position.w = L_NOIRE;
  clavier->touches[25].position.h = H_NOIRE;
  //
  clavier->touches[26].nom = "Si1";
  clavier->touches[26].frequence = 123.47082531403095;
  clavier->touches[26].type = DROITE;
  clavier->touches[26].position.x = 800.0 / 1750.0 * 510;
  clavier->touches[26].position.y = 0;
  clavier->touches[26].position.w = L_BLANCHE;
  clavier->touches[26].position.h = H_BLANCHE;
  //
  clavier->touches[27].nom = "Do2";
  clavier->touches[27].frequence = 130.81278265029923;
  clavier->touches[27].type = GAUCHE;
  clavier->touches[27].position.x = 800.0 / 1750.0 * 543;
  clavier->touches[27].position.y = 0;
  clavier->touches[27].position.w = L_BLANCHE;
  clavier->touches[27].position.h = H_BLANCHE;
  //
  clavier->touches[28].nom = "Do2dièse";
  clavier->touches[28].frequence = 138.59131548843595;
  clavier->touches[28].type = NOIRE;
  clavier->touches[28].position.x = 800.0 / 1750.0 * 567;
  clavier->touches[28].position.y = 0;
  clavier->touches[28].position.w = L_NOIRE;
  clavier->touches[28].position.h = H_NOIRE;
  //
  clavier->touches[29].nom = "Ré2";
  clavier->touches[29].frequence = 146.83238395870367;
  clavier->touches[29].type = CENTRE;
  clavier->touches[29].position.x = 800.0 / 1750.0 * 577;
  clavier->touches[29].position.y = 0;
  clavier->touches[29].position.w = L_BLANCHE;
  clavier->touches[29].position.h = H_BLANCHE;
  //
  clavier->touches[30].nom = "Ré2dièse";
  clavier->touches[30].frequence = 155.56349186104035;
  clavier->touches[30].type = NOIRE;
  clavier->touches[30].position.x = 800.0 / 1750.0 * 600;
  clavier->touches[30].position.y = 0;
  clavier->touches[30].position.w = L_NOIRE;
  clavier->touches[30].position.h = H_NOIRE;
  //
  clavier->touches[31].nom = "Mi2";
  clavier->touches[31].frequence = 164.81377845643485;
  clavier->touches[31].type = DROITE;
  clavier->touches[31].position.x = 800.0 / 1750.0 * 609;
  clavier->touches[31].position.y = 0;
  clavier->touches[31].position.w = L_BLANCHE;
  clavier->touches[31].position.h = H_BLANCHE;
  //
  clavier->touches[32].nom = "Fa2";
  clavier->touches[32].frequence = 174.61411571650183;
  clavier->touches[32].type = GAUCHE;
  clavier->touches[32].position.x = 800.0 / 1750.0 * 643;
  clavier->touches[32].position.y = 0;
  clavier->touches[32].position.w = L_BLANCHE;
  clavier->touches[32].position.h = H_BLANCHE;
  //
  clavier->touches[33].nom = "Fa2dièse";
  clavier->touches[33].frequence = 184.9972113558171;
  clavier->touches[33].type = NOIRE;
  clavier->touches[33].position.x = 800.0 / 1750.0 * 666;
  clavier->touches[33].position.y = 0;
  clavier->touches[33].position.w = L_NOIRE;
  clavier->touches[33].position.h = H_NOIRE;
  //
  clavier->touches[34].nom = "Sol2";
  clavier->touches[34].frequence = 195.99771799087455;
  clavier->touches[34].type = CENTRE;
  clavier->touches[34].position.x = 800.0 / 1750.0 * 675;
  clavier->touches[34].position.y = 0;
  clavier->touches[34].position.w = L_BLANCHE;
  clavier->touches[34].position.h = H_BLANCHE;
  //
  clavier->touches[35].nom = "Sol2dièse";
  clavier->touches[35].frequence = 207.65234878997248;
  clavier->touches[35].type = NOIRE;
  clavier->touches[35].position.x = 800.0 / 1750.0 * 699;
  clavier->touches[35].position.y = 0;
  clavier->touches[35].position.w = L_NOIRE;
  clavier->touches[35].position.h = H_NOIRE;
  //
  clavier->touches[36].nom = "La2";
  clavier->touches[36].frequence = 220.0;
  clavier->touches[36].type = CENTRE;
  clavier->touches[36].position.x = 800.0 / 1750.0 * 710;
  clavier->touches[36].position.y = 0;
  clavier->touches[36].position.w = L_BLANCHE;
  clavier->touches[36].position.h = H_BLANCHE;
  //
  clavier->touches[37].nom = "La2dièse";
  clavier->touches[37].frequence = 233.08188075904488;
  clavier->touches[37].type = NOIRE;
  clavier->touches[37].position.x = 800.0 / 1750.0 * 733;
  clavier->touches[37].position.y = 0;
  clavier->touches[37].position.w = L_NOIRE;
  clavier->touches[37].position.h = H_NOIRE;
  //
  clavier->touches[38].nom = "Si2";
  clavier->touches[38].frequence = 246.94165062806198;
  clavier->touches[38].type = DROITE;
  clavier->touches[38].position.x = 800.0 / 1750.0 * 742;
  clavier->touches[38].position.y = 0;
  clavier->touches[38].position.w = L_BLANCHE;
  clavier->touches[38].position.h = H_BLANCHE;
  //
  clavier->touches[39].nom = "Do3";
  clavier->touches[39].frequence = 261.62556530059857;
  clavier->touches[39].type = GAUCHE;
  clavier->touches[39].position.x = 800.0 / 1750.0 * 775;
  clavier->touches[39].position.y = 0;
  clavier->touches[39].position.w = L_BLANCHE;
  clavier->touches[39].position.h = H_BLANCHE;
  //
  clavier->touches[40].nom = "Do3dièse";
  clavier->touches[40].frequence = 277.182630976872;
  clavier->touches[40].type = NOIRE;
  clavier->touches[40].position.x = 800.0 / 1750.0 * 799;
  clavier->touches[40].position.y = 0;
  clavier->touches[40].position.w = L_NOIRE;
  clavier->touches[40].position.h = H_NOIRE;
  //
  clavier->touches[41].nom = "Ré3";
  clavier->touches[41].frequence = 293.6647679174075;
  clavier->touches[41].type = CENTRE;
  clavier->touches[41].position.x = 800.0 / 1750.0 * 809;
  clavier->touches[41].position.y = 0;
  clavier->touches[41].position.w = L_BLANCHE;
  clavier->touches[41].position.h = H_BLANCHE;
  //
  clavier->touches[42].nom = "Ré3dièse";
  clavier->touches[42].frequence = 311.12698372208087;
  clavier->touches[42].type = NOIRE;
  clavier->touches[42].position.x = 800.0 / 1750.0 * 832;
  clavier->touches[42].position.y = 0;
  clavier->touches[42].position.w = L_NOIRE;
  clavier->touches[42].position.h = H_NOIRE;
  //
  clavier->touches[43].nom = "Mi3";
  clavier->touches[43].frequence = 329.6275569128699;
  clavier->touches[43].type = DROITE;
  clavier->touches[43].position.x = 800.0 / 1750.0 * 841;
  clavier->touches[43].position.y = 0;
  clavier->touches[43].position.w = L_BLANCHE;
  clavier->touches[43].position.h = H_BLANCHE;
  //
  clavier->touches[44].nom = "Fa3";
  clavier->touches[44].frequence = 349.2282314330039;
  clavier->touches[44].type = GAUCHE;
  clavier->touches[44].position.x = 800.0 / 1750.0 * 875;
  clavier->touches[44].position.y = 0;
  clavier->touches[44].position.w = L_BLANCHE;
  clavier->touches[44].position.h = H_BLANCHE;
  //
  clavier->touches[45].nom = "Fa3dièse";
  clavier->touches[45].frequence = 369.9944227116344;
  clavier->touches[45].type = NOIRE;
  clavier->touches[45].position.x = 800.0 / 1750.0 * 898;
  clavier->touches[45].position.y = 0;
  clavier->touches[45].position.w = L_NOIRE;
  clavier->touches[45].position.h = H_NOIRE;
  //
  clavier->touches[46].nom = "Sol3";
  clavier->touches[46].frequence = 391.9954359817493;
  clavier->touches[46].type = CENTRE;
  clavier->touches[46].position.x = 800.0 / 1750.0 * 907;
  clavier->touches[46].position.y = 0;
  clavier->touches[46].position.w = L_BLANCHE;
  clavier->touches[46].position.h = H_BLANCHE;
  //
  clavier->touches[47].nom = "Sol3dièse";
  clavier->touches[47].frequence = 415.3046975799452;
  clavier->touches[47].type = NOIRE;
  clavier->touches[47].position.x = 800.0 / 1750.0 * 931;
  clavier->touches[47].position.y = 0;
  clavier->touches[47].position.w = L_NOIRE;
  clavier->touches[47].position.h = H_NOIRE;
  //
  clavier->touches[48].nom = "La3";
  clavier->touches[48].frequence = 440.0;
  clavier->touches[48].type = CENTRE;
  clavier->touches[48].position.x = 800.0 / 1750.0 * 942;
  clavier->touches[48].position.y = 0;
  clavier->touches[48].position.w = L_BLANCHE;
  clavier->touches[48].position.h = H_BLANCHE;
  //
  clavier->touches[49].nom = "La3dièse";
  clavier->touches[49].frequence = 466.16376151809;
  clavier->touches[49].type = NOIRE;
  clavier->touches[49].position.x = 800.0 / 1750.0 * 965;
  clavier->touches[49].position.y = 0;
  clavier->touches[49].position.w = L_NOIRE;
  clavier->touches[49].position.h = H_NOIRE;
  //
  clavier->touches[50].nom = "Si3";
  clavier->touches[50].frequence = 493.8833012561242;
  clavier->touches[50].type = DROITE;
  clavier->touches[50].position.x = 800.0 / 1750.0 * 974;
  clavier->touches[50].position.y = 0;
  clavier->touches[50].position.w = L_BLANCHE;
  clavier->touches[50].position.h = H_BLANCHE;
  //
  clavier->touches[51].nom = "Do4";
  clavier->touches[51].frequence = 523.2511306011974;
  clavier->touches[51].type = GAUCHE;
  clavier->touches[51].position.x = 800.0 / 1750.0 * 1007;
  clavier->touches[51].position.y = 0;
  clavier->touches[51].position.w = L_BLANCHE;
  clavier->touches[51].position.h = H_BLANCHE;
  //
  clavier->touches[52].nom = "Do4dièse";
  clavier->touches[52].frequence = 554.3652619537443;
  clavier->touches[52].type = NOIRE;
  clavier->touches[52].position.x = 800.0 / 1750.0 * 1031;
  clavier->touches[52].position.y = 0;
  clavier->touches[52].position.w = L_NOIRE;
  clavier->touches[52].position.h = H_NOIRE;
  //
  clavier->touches[53].nom = "Ré4";
  clavier->touches[53].frequence = 587.3295358348153;
  clavier->touches[53].type = CENTRE;
  clavier->touches[53].position.x = 800.0 / 1750.0 * 1041;
  clavier->touches[53].position.y = 0;
  clavier->touches[53].position.w = L_BLANCHE;
  clavier->touches[53].position.h = H_BLANCHE;
  //
  clavier->touches[54].nom = "Ré4dièse";
  clavier->touches[54].frequence = 622.253967444162;
  clavier->touches[54].type = NOIRE;
  clavier->touches[54].position.x = 800.0 / 1750.0 * 1064;
  clavier->touches[54].position.y = 0;
  clavier->touches[54].position.w = L_NOIRE;
  clavier->touches[54].position.h = H_NOIRE;
  //
  clavier->touches[55].nom = "Mi4";
  clavier->touches[55].frequence = 659.2551138257401;
  clavier->touches[55].type = DROITE;
  clavier->touches[55].position.x = 800.0 / 1750.0 * 1073;
  clavier->touches[55].position.y = 0;
  clavier->touches[55].position.w = L_BLANCHE;
  clavier->touches[55].position.h = H_BLANCHE;
  //
  clavier->touches[56].nom = "Fa4";
  clavier->touches[56].frequence = 698.456462866008;
  clavier->touches[56].type = GAUCHE;
  clavier->touches[56].position.x = 800.0 / 1750.0 * 1107;
  clavier->touches[56].position.y = 0;
  clavier->touches[56].position.w = L_BLANCHE;
  clavier->touches[56].position.h = H_BLANCHE;
  //
  clavier->touches[57].nom = "Fa4dièse";
  clavier->touches[57].frequence = 739.988845423269;
  clavier->touches[57].type = NOIRE;
  clavier->touches[57].position.x = 800.0 / 1750.0 * 1130;
  clavier->touches[57].position.y = 0;
  clavier->touches[57].position.w = L_NOIRE;
  clavier->touches[57].position.h = H_NOIRE;
  //
  clavier->touches[58].nom = "Sol4";
  clavier->touches[58].frequence = 783.9908719634989;
  clavier->touches[58].type = CENTRE;
  clavier->touches[58].position.x = 800.0 / 1750.0 * 1139;
  clavier->touches[58].position.y = 0;
  clavier->touches[58].position.w = L_BLANCHE;
  clavier->touches[58].position.h = H_BLANCHE;
  //
  clavier->touches[59].nom = "Sol4dièse";
  clavier->touches[59].frequence = 830.6093951598906;
  clavier->touches[59].type = NOIRE;
  clavier->touches[59].position.x = 800.0 / 1750.0 * 1163;
  clavier->touches[59].position.y = 0;
  clavier->touches[59].position.w = L_NOIRE;
  clavier->touches[59].position.h = H_NOIRE;
  //
  clavier->touches[60].nom = "La4";
  clavier->touches[60].frequence = 880.0;
  clavier->touches[60].type = CENTRE;
  clavier->touches[60].position.x = 800.0 / 1750.0 * 1174;
  clavier->touches[60].position.y = 0;
  clavier->touches[60].position.w = L_BLANCHE;
  clavier->touches[60].position.h = H_BLANCHE;
  //
  clavier->touches[61].nom = "La4dièse";
  clavier->touches[61].frequence = 932.3275230361802;
  clavier->touches[61].type = NOIRE;
  clavier->touches[61].position.x = 800.0 / 1750.0 * 1197;
  clavier->touches[61].position.y = 0;
  clavier->touches[61].position.w = L_NOIRE;
  clavier->touches[61].position.h = H_NOIRE;
  //
  clavier->touches[62].nom = "Si4";
  clavier->touches[62].frequence = 987.7666025122486;
  clavier->touches[62].type = DROITE;
  clavier->touches[62].position.x = 800.0 / 1750.0 * 1206;
  clavier->touches[62].position.y = 0;
  clavier->touches[62].position.w = L_BLANCHE;
  clavier->touches[62].position.h = H_BLANCHE;
  //
  clavier->touches[63].nom = "Do5";
  clavier->touches[63].frequence = 1046.502261202395;
  clavier->touches[63].type = GAUCHE;
  clavier->touches[63].position.x = 800.0 / 1750.0 * 1239;
  clavier->touches[63].position.y = 0;
  clavier->touches[63].position.w = L_BLANCHE;
  clavier->touches[63].position.h = H_BLANCHE;
  //
  clavier->touches[64].nom = "Do5dièse";
  clavier->touches[64].frequence = 1108.7305239074888;
  clavier->touches[64].type = NOIRE;
  clavier->touches[64].position.x = 800.0 / 1750.0 * 1263;
  clavier->touches[64].position.y = 0;
  clavier->touches[64].position.w = L_NOIRE;
  clavier->touches[64].position.h = H_NOIRE;
  //
  clavier->touches[65].nom = "Ré5";
  clavier->touches[65].frequence = 1174.6590716696307;
  clavier->touches[65].type = CENTRE;
  clavier->touches[65].position.x = 800.0 / 1750.0 * 1273;
  clavier->touches[65].position.y = 0;
  clavier->touches[65].position.w = L_BLANCHE;
  clavier->touches[65].position.h = H_BLANCHE;
  //
  clavier->touches[66].nom = "Ré5dièse";
  clavier->touches[66].frequence = 1244.5079348883241;
  clavier->touches[66].type = NOIRE;
  clavier->touches[66].position.x = 800.0 / 1750.0 * 1296;
  clavier->touches[66].position.y = 0;
  clavier->touches[66].position.w = L_NOIRE;
  clavier->touches[66].position.h = H_NOIRE;
  //
  clavier->touches[67].nom = "Mi5";
  clavier->touches[67].frequence = 1318.5102276514804;
  clavier->touches[67].type = DROITE;
  clavier->touches[67].position.x = 800.0 / 1750.0 * 1305;
  clavier->touches[67].position.y = 0;
  clavier->touches[67].position.w = L_BLANCHE;
  clavier->touches[67].position.h = H_BLANCHE;
  //
  clavier->touches[68].nom = "Fa5";
  clavier->touches[68].frequence = 1396.9129257320162;
  clavier->touches[68].type = GAUCHE;
  clavier->touches[68].position.x = 800.0 / 1750.0 * 1339;
  clavier->touches[68].position.y = 0;
  clavier->touches[68].position.w = L_BLANCHE;
  clavier->touches[68].position.h = H_BLANCHE;
  //
  clavier->touches[69].nom = "Fa5dièse";
  clavier->touches[69].frequence = 1479.9776908465383;
  clavier->touches[69].type = NOIRE;
  clavier->touches[69].position.x = 800.0 / 1750.0 * 1362;
  clavier->touches[69].position.y = 0;
  clavier->touches[69].position.w = L_NOIRE;
  clavier->touches[69].position.h = H_NOIRE;
  //
  clavier->touches[70].nom = "Sol5";
  clavier->touches[70].frequence = 1567.981743926998;
  clavier->touches[70].type = CENTRE;
  clavier->touches[70].position.x = 800.0 / 1750.0 * 1371;
  clavier->touches[70].position.y = 0;
  clavier->touches[70].position.w = L_BLANCHE;
  clavier->touches[70].position.h = H_BLANCHE;
  //
  clavier->touches[71].nom = "Sol5dièse";
  clavier->touches[71].frequence = 1661.2187903197814;
  clavier->touches[71].type = NOIRE;
  clavier->touches[71].position.x = 800.0 / 1750.0 * 1395;
  clavier->touches[71].position.y = 0;
  clavier->touches[71].position.w = L_NOIRE;
  clavier->touches[71].position.h = H_NOIRE;
  //
  clavier->touches[72].nom = "La5";
  clavier->touches[72].frequence = 1760.0;
  clavier->touches[72].type = CENTRE;
  clavier->touches[72].position.x = 800.0 / 1750.0 * 1406;
  clavier->touches[72].position.y = 0;
  clavier->touches[72].position.w = L_BLANCHE;
  clavier->touches[72].position.h = H_BLANCHE;
  //
  clavier->touches[73].nom = "La5dièse";
  clavier->touches[73].frequence = 1864.6550460723606;
  clavier->touches[73].type = NOIRE;
  clavier->touches[73].position.x = 800.0 / 1750.0 * 1429;
  clavier->touches[73].position.y = 0;
  clavier->touches[73].position.w = L_NOIRE;
  clavier->touches[73].position.h = H_NOIRE;
  //
  clavier->touches[74].nom = "Si5";
  clavier->touches[74].frequence = 1975.5332050244976;
  clavier->touches[74].type = DROITE;
  clavier->touches[74].position.x = 800.0 / 1750.0 * 1438;
  clavier->touches[74].position.y = 0;
  clavier->touches[74].position.w = L_BLANCHE;
  clavier->touches[74].position.h = H_BLANCHE;
  //
  clavier->touches[75].nom = "Do6";
  clavier->touches[75].frequence = 2093.0045224047904;
  clavier->touches[75].type = GAUCHE;
  clavier->touches[75].position.x = 800.0 / 1750.0 * 1471;
  clavier->touches[75].position.y = 0;
  clavier->touches[75].position.w = L_BLANCHE;
  clavier->touches[75].position.h = H_BLANCHE;
  //
  clavier->touches[76].nom = "Do6dièse";
  clavier->touches[76].frequence = 2217.4610478149784;
  clavier->touches[76].type = NOIRE;
  clavier->touches[76].position.x = 800.0 / 1750.0 * 1495;
  clavier->touches[76].position.y = 0;
  clavier->touches[76].position.w = L_NOIRE;
  clavier->touches[76].position.h = H_NOIRE;
  //
  clavier->touches[77].nom = "Ré6";
  clavier->touches[77].frequence = 2349.3181433392624;
  clavier->touches[77].type = CENTRE;
  clavier->touches[77].position.x = 800.0 / 1750.0 * 1505;
  clavier->touches[77].position.y = 0;
  clavier->touches[77].position.w = L_BLANCHE;
  clavier->touches[77].position.h = H_BLANCHE;
  //
  clavier->touches[78].nom = "Ré6dièse";
  clavier->touches[78].frequence = 2489.0158697766497;
  clavier->touches[78].type = NOIRE;
  clavier->touches[78].position.x = 800.0 / 1750.0 * 1528;
  clavier->touches[78].position.y = 0;
  clavier->touches[78].position.w = L_NOIRE;
  clavier->touches[78].position.h = H_NOIRE;
  //
  clavier->touches[79].nom = "Mi6";
  clavier->touches[79].frequence = 2637.020455302962;
  clavier->touches[79].type = DROITE;
  clavier->touches[79].position.x = 800.0 / 1750.0 * 1537;
  clavier->touches[79].position.y = 0;
  clavier->touches[79].position.w = L_BLANCHE;
  clavier->touches[79].position.h = H_BLANCHE;
  //
  clavier->touches[80].nom = "Fa6";
  clavier->touches[80].frequence = 2793.825851464034;
  clavier->touches[80].type = GAUCHE;
  clavier->touches[80].position.x = 800.0 / 1750.0 * 1571;
  clavier->touches[80].position.y = 0;
  clavier->touches[80].position.w = L_BLANCHE;
  clavier->touches[80].position.h = H_BLANCHE;
  //
  clavier->touches[81].nom = "Fa6dièse";
  clavier->touches[81].frequence = 2959.9553816930784;
  clavier->touches[81].type = NOIRE;
  clavier->touches[81].position.x = 800.0 / 1750.0 * 1594;
  clavier->touches[81].position.y = 0;
  clavier->touches[81].position.w = L_NOIRE;
  clavier->touches[81].position.h = H_NOIRE;
  //
  clavier->touches[82].nom = "Sol6";
  clavier->touches[82].frequence = 3135.963487853998;
  clavier->touches[82].type = CENTRE;
  clavier->touches[82].position.x = 800.0 / 1750.0 * 1603;
  clavier->touches[82].position.y = 0;
  clavier->touches[82].position.w = L_BLANCHE;
  clavier->touches[82].position.h = H_BLANCHE;
  //
  clavier->touches[83].nom = "Sol6dièse";
  clavier->touches[83].frequence = 3322.4375806395647;
  clavier->touches[83].type = NOIRE;
  clavier->touches[83].position.x = 800.0 / 1750.0 * 1627;
  clavier->touches[83].position.y = 0;
  clavier->touches[83].position.w = L_NOIRE;
  clavier->touches[83].position.h = H_NOIRE;
  //
  clavier->touches[84].nom = "La6";
  clavier->touches[84].frequence = 3520.0;
  clavier->touches[84].type = CENTRE;
  clavier->touches[84].position.x = 800.0 / 1750.0 * 1638;
  clavier->touches[84].position.y = 0;
  clavier->touches[84].position.w = L_BLANCHE;
  clavier->touches[84].position.h = H_BLANCHE;
  //
  clavier->touches[85].nom = "La6dièse";
  clavier->touches[85].frequence = 3729.310092144724;
  clavier->touches[85].type = NOIRE;
  clavier->touches[85].position.x = 800.0 / 1750.0 * 1661;
  clavier->touches[85].position.y = 0;
  clavier->touches[85].position.w = L_NOIRE;
  clavier->touches[85].position.h = H_NOIRE;
  //
  clavier->touches[86].nom = "Si6";
  clavier->touches[86].frequence = 3951.066410048998;
  clavier->touches[86].type = DROITE;
  clavier->touches[86].position.x = 800.0 / 1750.0 * 1670;
  clavier->touches[86].position.y = 0;
  clavier->touches[86].position.w = L_BLANCHE;
  clavier->touches[86].position.h = H_BLANCHE;
  //
  clavier->touches[87].nom = "Do7";
  clavier->touches[87].frequence = 4186.009044809583;
  clavier->touches[87].type = PLEINE;
  clavier->touches[87].position.x = 800.0 / 1750.0 * 1703;
  clavier->touches[87].position.y = 0;
  clavier->touches[87].position.w = L_BLANCHE;
  clavier->touches[87].position.h = H_BLANCHE;
  return 0;
}
