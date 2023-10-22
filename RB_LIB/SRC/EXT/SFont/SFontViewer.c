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
    screen = SDL_SetVideoMode(640, 480, 0, 16);
    if ( screen == NULL ) {
	fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
		640, 480, 16, SDL_GetError());
	exit(1);
    }
	
    /* Set the window manager title bar */
    SDL_WM_SetCaption("SFont Font Viewer", "SFontViewer");
}

int main(int argc, char *argv[])
{
    SDL_Event event;
    SFont_Font* Font;

    if (argc < 2 || argc > 3) {
	puts("SFont Font Viewer");
	puts("Usage:\n");
	puts("SFontViewer FONTFILE [TEXTTOPRINT]\n");
	exit(0);
    }

    init_SDL();

    // Load and Prepare the font - You don't have to use the IMGlib for this
    Font = SFont_InitFont(IMG_Load(argv[1]));
    if(!Font) {
	fprintf(stderr, "An error occured while loading the font '%s'.\n",
		argv[1]);
	exit(1);
    }
	
    if (argc > 2) {
	// a simple text blit to (0/0)
	SFont_Write(screen, Font, 0, 0, argv[2]);
    } else {
	SFont_Write(screen, Font, 0, 0, argv[1]);
	SFont_WriteCenter(screen,Font,60,"SFont Font Viewer");
	SFont_WriteCenter(screen,Font,300,"This text is for testing purposes.");
	SFont_WriteCenter(screen,Font,360,"This one, too <>%&-~|/_#+");
    }

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    // Let the user time to look at the font
    SDL_EventState( SDL_KEYUP, SDL_IGNORE );
    SDL_EventState( SDL_ACTIVEEVENT, SDL_IGNORE );
    SDL_EventState( SDL_MOUSEMOTION, SDL_IGNORE );
    SDL_EventState( SDL_SYSWMEVENT, SDL_IGNORE );
    SDL_WaitEvent(&event);

    // Don't forget to free our font
    SFont_FreeFont(Font);
    
    // Bye
    exit(0);
}

