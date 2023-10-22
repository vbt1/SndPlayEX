#include "SDL.h"

#include "KBFLite.h"

int main(int argc, char *argv[]) {
	SDL_Surface *screen, *Temp;
	KBF_Font *font;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	screen = SDL_SetVideoMode(640, 480, 0, 0);

	// This says all we are really loading is the alpha channel
	KBF_SetMode(KBF_ALPHA_FROM_RGB);

	// Load the font bitmap and it's metrics
	Temp = SDL_LoadBMP("helvetica.bmp");
	font = KBF_LoadFont(Temp, "helvetica.def");
	SDL_FreeSurface(Temp); // Don't need it anymore

	KBF_Write(font, screen, 0, 0, "Hello World!");
	SDL_Flip(screen);
	SDL_Delay(5000);

	KBF_FreeFont(font);
	SDL_Quit();
	
	return 0;
}
