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
	SFont_Write(screen, Font, 0, 0, "Top Left");
	// License Info...
	SFont_Write(screen, Font, 60, 120, "SFont by Karl Bartel is GPL'ed!");
	// show some special chars
	SFont_Write(screen, Font, 300, 260, "@--~!%&'_*,.:;");
	// demonstrates the use of TextWidth
	SFont_Write(screen, Font, 640-SFont_TextWidth(Font, "Bottom Right!"),
							  480-SFont_TextHeight(Font), "Bottom Right!");
	// Update the screen
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	SFont_FreeFont(Font);

	// wait a bit
	SDL_Delay(4000);
	
	// Bye
	exit(0);
}
