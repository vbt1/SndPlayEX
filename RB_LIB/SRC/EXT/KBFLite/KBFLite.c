#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KBFLite.h"

int KBFmode = KBF_USE_SURFACE_ALPHA;
Uint8 fr=0xff, fg=0xff, fb=0xff;

void KBF_SetMode(int mode) {
	KBFmode = mode;
}

// Sets color of next loaded font, only good for mode KBF_ALPHA_FROM_RGB
void KBF_SetColor(Uint8 r, Uint8 g, Uint8 b) {
	fr = r;
	fg = g;
	fb = b;
}

// Ripped getpixel and putpixel from the SDL doc project

Uint32 KBF_getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return p[0] << 16 | p[1] << 8 | p[2];
#else
        return p[0] | p[1] << 8 | p[2] << 16;
#endif

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void KBF_putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
#else
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
#endif

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void KBF_Write(KBF_Font *font, SDL_Surface *surf, Sint32 x, Sint32 y, unsigned char *text) {
	unsigned int i;
	SDL_Rect src, dst;
	unsigned char *t;

	int len;

	dst.x = x;
	dst.y = y;

	len = strlen(text);
	
	t = text;

	i=0;
	do {
		src = font->FontData[*t];

		dst.w=font->Font[*t]->w;
		dst.h=font->Font[*t]->h;
			
		SDL_BlitSurface(font->Font[*t], NULL, surf, &dst);

		dst.x+=font->Font[*t]->w;
		t++;
	} while (t<(text + len));

}


KBF_Font * KBF_LoadFont(SDL_Surface *Temp, char *defname) {
	FILE *fp;
	KBF_Font *new_font=NULL;
	
	int i, x, y, w, h, lx, ly;
	Uint8 r, g, b, a;
	Uint32 pixel;
	int result;
	SDL_Rect src, dst;

	if(!Temp) {
		SDL_SetError("Error accessing font Surface\n");
		exit(0);
	}
	
	fp = fopen(defname, "r");
	
	if(!fp) {
		SDL_SetError("Error loading font .DEF file\n");
		exit(0);
	}

	dst.x = 0;
	dst.y = 0;

	if(fp!=NULL) {
		new_font = (KBF_Font *)malloc(sizeof(KBF_Font));
		
		for(i=0; i<128; i++) {
			new_font->Font[i] = NULL;
		}

		result=fscanf(fp, "%i %i %i %i %i\n", &i, &x, &y, &w, &h);
		while(result!=EOF) {
			if(i<256) {
				if(h > new_font->maxheight) new_font->maxheight = h;

				new_font->FontData[i].x=x;
				new_font->FontData[i].y=y;
				new_font->FontData[i].w=w;
				new_font->FontData[i].h=h;

				new_font->Font[i] = SDL_CreateRGBSurface(SDL_SRCALPHA,
					w,h,32,0xff,0xff00,0xff0000,0xff000000);

				SDL_DisplayFormatAlpha(new_font->Font[i]);
				SDL_FillRect(new_font->Font[i], NULL, 0xffffffff);

				src = new_font->FontData[i];
				/* Curse SDL's buggy Alpha blitter */

				dst.w = w;
				dst.h = h;
				SDL_BlitSurface(Temp, &src, new_font->Font[i], &dst);
				
				if(SDL_MUSTLOCK(Temp)) SDL_LockSurface(Temp);
				if(SDL_MUSTLOCK(new_font->Font[i])) SDL_LockSurface(new_font->Font[i]);

				for(ly=0; ly<h; ly++) {
					for(lx=0; lx<w; lx++) {
						pixel = KBF_getpixel(Temp, x+lx, y+ly);
						SDL_GetRGBA(pixel, Temp->format, &r, &g, &b, &a);
						if(KBFmode==KBF_ALPHA_FROM_RGB) {
							a = (r + b + g) / 3;
							r=fr;
							g=fg;
							b=fb;
						}
						pixel = SDL_MapRGBA(new_font->Font[i]->format, r, g, b, a);
						KBF_putpixel(new_font->Font[i], lx, ly, pixel);
					}
				}

				if(SDL_MUSTLOCK(Temp)) SDL_UnlockSurface(Temp);
				if(SDL_MUSTLOCK(new_font->Font[i])) SDL_UnlockSurface(new_font->Font[i]);
			}
			result=fscanf(fp, "%i %i %i %i %i\n", &i, &x, &y, &w, &h);
		}
		fclose(fp);
	}
	else {
		SDL_SetError("Error loading font data");
	}

	return(new_font);
}

void KBF_FreeFont(KBF_Font *font) {
	int i = 0;

	if(font != NULL) {
	}

	for(i=0; i<128; i++) {
		if(font->Font[i]) {
			SDL_FreeSurface(font->Font[i]);
		}
	}

	free(font);
	font = NULL;
}

SDL_Rect KBF_GetMetrics(KBF_Font *font, unsigned char *text) {
	SDL_Rect ret;
	unsigned int i;

	ret.x = ret.y = ret.w = ret.h = 0; // clear our rect out
	if(strlen(text) > 0) {
		for(i=0; i<strlen(text); i++) {
			if(font->Font[i]->h > ret.h) ret.h = font->Font[i]->h;
			ret.w += font->Font[i]->w;
		}
	}
	return ret;
}
