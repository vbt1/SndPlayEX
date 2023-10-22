#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SFont.h"

#include "SDL_image.h"

SDL_Surface *screen;

void init_SDL()
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,
			"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);			/* Clean up on exit */

	/* Initialize the display */
	screen = SDL_SetVideoMode(640, 480, 0, 0);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
					640, 480, 16, SDL_GetError());
		exit(1);
	}
	
	/* Set the window manager title bar */
	SDL_WM_SetCaption("SFont Test", "SFont");
}

int main(int argc, char *argv[])
{
	SFont_Font* Neon;
	SFont_Font* Copper;

	init_SDL();

	// Load and Prepare the fonts - You don't have to use the IMGlib for this
	Copper = SFont_InitFont(IMG_Load("24P_Copperplate_Blue.png"));
	if(!Copper) {
		fprintf(stderr, "An error occured while loading the font.");
		exit(1);
	}
	Neon = SFont_InitFont(IMG_Load("24P_Arial_NeonYellow.png"));
	if(!Neon) {
		fprintf(stderr, "An error occured while loading the font.");
		exit(1);
	}

	// a simple text blit to (0/0) with Neon font
	SFont_Write(screen, Neon, 0, 0, "Top Left");
	// License Info...
	SFont_Write(screen, Copper, 60, 120, "SFont by Karl Bartel is GPL'ed!");
	// show some special chars
	SFont_Write(screen, Copper, 300, 260, "@--~!%&'_*,.:;");
	// demonstrates the use of TextWidth
	SFont_Write(screen, Neon, 640-SFont_TextWidth(Neon, "Bottom Right!"),
							  480-SFont_TextHeight(Neon),"Bottom Right!");
	// Update the screen
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	// Wait a bit
	SDL_Delay(4000);

	// Don't forget to free the fonts
	SFont_FreeFont(Copper);
	SFont_FreeFont(Neon);

	// Bye
	exit(0);
}
