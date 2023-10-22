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
	SFont_Font* Font;

	init_SDL();

	// Load and Prepare the font - You don't have to use the IMGlib for this
	Font = SFont_InitFont(IMG_Load("24P_Copperplate_Blue.png"));
	if(!Font) {
		fprintf(stderr, "An error occured while loading the font.");
		exit(1);
	}
	
	// a simple text blit to (0/0)
	SFont_Write (screen, Font, 0,0,"Welcome to SFontTest1!");

	// Update the screen
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	// Let the user time to look at the font
	SDL_Delay(4000);

	// Don't forget to free our font
	SFont_FreeFont(Font);
	
	// Bye
	exit(0);
}

