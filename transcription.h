#ifndef __TRANSCRIPTION_H__
#define __TRANSCRIPTION_H__

#include "note.h"

struct liste_note_t* transcrire(double * donnee_son, double * instant_son, int taille, struct clavier_t * clavier);

#endif
