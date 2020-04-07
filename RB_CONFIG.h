#ifndef HAVE_RB_CONFIG_H
#define HAVE_RB_CONFIG_H

/* first define the target system */
//#define 	SATURN_SGL
//#define 	SATURN_SGL not supported yet
//#define 	SDL_PC
//#define		SDL_FONTLIB_SFONT
//#define		SDL_FONTLIB_KBFLITE
#define 	PC_SHELL

#define	RB_TXTSCR_SFONT_IM			"24P_Copperplate_Blue.png"

#define RB_TXTSCR_KBF_BMP			"courier.bmp"
#define RB_TXTSCR_KBF_DEF			"courier.def"



// don't touch

#if defined(SDL_PC)
#	include "SDL.h"
#	if defined(SDL_FONTLIB_SFONT)
#		include "SFont.h"
#		include <SDL_image.h>
#	elif defined(SDL_FONTLIB_KBFLITE)
#		include "KBFLite.h"
#	endif
#elif defined(SATURN_SGL)
#   include <SGL.H>
#endif

#endif
