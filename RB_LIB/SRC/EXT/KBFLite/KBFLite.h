/*
	Little Font Routines, ripped from Kawaii
*/

#include "SDL.h"

#ifndef _KBFLITE_H
#define _KBFLITE_H

#define KBF_USE_SURFACE_ALPHA     0
#define KBF_ALPHA_FROM_RGB        1

typedef struct {
	SDL_Rect FontData[256];
	SDL_Surface *Font[256];
	int maxheight;
} KBF_Font;

void KBF_SetMode(int mode);
void KBF_Write(KBF_Font *font, SDL_Surface *surf, Sint32 x, Sint32 y, unsigned char *text);
KBF_Font * KBF_LoadFont(SDL_Surface *Temp, char *defname);
void KBF_FreeFont(KBF_Font *font);
SDL_Rect KBF_GetMetrics(KBF_Font *font, unsigned char *text);

/* 
Note: This only has an affect on fonts loaded with KBF_ALPHA_FROM_RGB
      and only affects the KBF_LoadFont routine

	  Therefore call it before KBF_LoadFont, free and reload if you want a different color
*/
void KBF_SetColor(Uint8 r, Uint8 g, Uint8 b);

#endif
