//                                                                                
//    Copyright (c) 2004/2005 Thomas Fuchs / The Rockin'-B, www.rockin-b.de       
//                                                                                
//    Permission is granted to anyone to use this software for any purpose        
//    and to redistribute it freely, subject to the following restrictions:       
//                                                                                
//    1.  The origin of this software must not be misrepresented. You must not    
//        claim that you wrote the original software. If you use this software    
//        in a product, an acknowledgment in the product documentation would be   
//        appreciated but is not required.                                        
//                                                                                
//    2.  ANY COMMERCIAL USE IS PROHIBITED. This includes that you must not use   
//        this software in the production of a commercial product. Only an explicit
//        permission by the author can allow you to do that.                      
//                                                                                
//    3.  Any modification applied to the software must be marked as such clearly.
//                                                                                
//    4.  This license may not be removed or altered from any distribution.       
//                                                                                
//                                                                                
//    			    NO WARRANTY                                                 
//                                                                                
//    THERE IS NO WARRANTY FOR THE PROGRAM.                                       
//    THE PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,               
//    EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,                 
//    THE IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE.                 
//    THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.
//    SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY    
//    SERVICING, REPAIR OR CORRECTION.                                            
//                                                                                
//    IN NO EVENT WILL THE COPYRIGHT HOLDER BE LIABLE TO YOU FOR DAMAGES,         
//    INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING 
//    OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED   
//    TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY    
//    YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER  
//    PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE       
//    POSSIBILITY OF SUCH DAMAGES.                                                
//                                                                                

#include <RB_TXT_SCROLL.h>
#include "RB_TXT_SCROLL_INT.h"

char *TXTSCR_getVersion()
{
    return TXTSCR_VERSION;
}

ReturnValue TXTSCR_initScreen(void **screen, unsigned char xRes, unsigned char yRes)
{
    Txt_Screen *txtscr = (Txt_Screen *)malloc(sizeof(Txt_Screen));
    if(txtscr == NULL)
        return RETURN_ERR_ALLOC;
        
#if defined(SDL_PC)
#	if defined(SDL_FONTLIB_SFONT)
	txtscr->font = SFont_InitFont(IMG_Load(RB_TXTSCR_SFONT_IM));
	if(txtscr->font == NULL)
		return RETURN_ERR_SUBCALL;
	    
    // assume width == height
//    txtscr->fontRes.x = SFont_TextWidth(txtscr->font, " ");
    txtscr->fontRes.x = SFont_TextHeight(txtscr->font);
    txtscr->fontRes.y = SFont_TextHeight(txtscr->font);
    
    txtscr->surface = SDL_SetVideoMode(xRes*txtscr->fontRes.x, yRes*txtscr->fontRes.y, 8, SDL_SWSURFACE);
#	elif defined(SDL_FONTLIB_KBFLITE)
    KBF_SetMode(KBF_ALPHA_FROM_RGB);
	SDL_Surface *Temp = SDL_LoadBMP(RB_TXTSCR_KBF_BMP);
	txtscr->font = KBF_LoadFont(Temp, RB_TXTSCR_KBF_DEF);
	SDL_FreeSurface(Temp); // Don't need it anymore

    txtscr->fontRes.x = txtscr->font->maxheight;
    txtscr->fontRes.y = txtscr->font->maxheight;

    txtscr->surface = SDL_SetVideoMode(txtscr->fontRes.x * xRes, txtscr->fontRes.y * yRes, 8, SDL_SWSURFACE);
#	endif
    SDL_FillRect(txtscr->surface, NULL, SDL_MapRGB(txtscr->surface->format, 0, 0, 0));

    if(txtscr->surface == NULL)
        return RETURN_ERR_SUBCALL;
#endif

    txtscr->scrRes.x = xRes;
    txtscr->scrRes.y = yRes;

    *screen = txtscr;
    
    return RETURN_OK;
}

ReturnValue TXTSCR_initScroll(void **scroll, void *screen, unsigned char xRes, unsigned char yRes, unsigned char xPos, unsigned char yPos)
{
    Txt_Screen *txtscreen = (Txt_Screen *)screen;
    Txt_Scroll *txtscr = (Txt_Scroll *)malloc(sizeof(Txt_Scroll));
    if(txtscr == NULL)
        return RETURN_ERR_ALLOC;
    
    if((txtscr == NULL) || (txtscreen == NULL))
        return RETURN_ERR_NULLARG;
        
    txtscr->txt = (char *)calloc((xRes + 1) * yRes, sizeof(char));
    if(txtscr->txt == NULL)
        return RETURN_ERR_ALLOC;
        
    txtscr->res.x = xRes;
    txtscr->res.y = yRes;
    txtscr->pos.x = xPos;
    txtscr->pos.y = yPos;

#if defined(SDL_PC)
    txtscr->font = txtscreen->font;
    txtscr->surface = txtscreen->surface;
    txtscr->fontRes.x = txtscreen->fontRes.x;
    txtscr->fontRes.y = txtscreen->fontRes.y;
#endif    
    
    *scroll = txtscr;
    
    return RETURN_OK;
}


ReturnValue TXTSCR_exitScroll(void *scroll)
{
    Txt_Scroll *txtscr = (Txt_Scroll *)scroll;
    
    if(txtscr == NULL)
        return RETURN_ERR_NULLARG;
        
    if(txtscr->txt != NULL)
        free(txtscr->txt);

#if defined(SDL_PC)
#	if defined(SDL_FONTLIB_SFONT)
    SFont_FreeFont(txtscr->font);
#	elif defined(SDL_FONTLIB_KBFLITE)
    KBF_FreeFont(txtscr->font);
#	endif
#endif

    free(scroll);
    
    return RETURN_OK;
}

ReturnValue TXTSCR_exitScreen(void *screen)
{
    Txt_Screen *txtscr = (Txt_Screen *)screen;
    
    if(txtscr == NULL)
        return RETURN_ERR_NULLARG;
        
#if defined(SDL_PC)
#	if defined(SDL_FONTLIB_SFONT)
    SFont_FreeFont(txtscr->font);
#	elif defined(SDL_FONTLIB_KBFLITE)
    KBF_FreeFont(txtscr->font);
#	endif
    SDL_FreeSurface(txtscr->surface);
#endif

    free(screen);
    
    return RETURN_OK;
}

int TXTSCR_getWidth(void *scroll)
{
    if(scroll == NULL)
        return 0;
    
    return ((Txt_Scroll *)scroll)->res.x;
}

int TXTSCR_getHeight(void *scroll)
{
    if(scroll == NULL)
        return 0;
    
    return ((Txt_Scroll *)scroll)->res.y;
}

/* draws a string to the specified screen position,
 * caution:
 * - does not draw into the TXT_SCROLL buffer !
 * - must be '/0' terminated
 * - must not be longer than rest of line on screen
 * - behaves, of course, differently for console window output
 * -> use only internally
 */
ReturnValue TXTSCR_drawLine(Txt_Scroll *txtscr, char *string, unsigned char xPos, unsigned char yPos)
{
#if defined(PC_SHELL)
    printf("%s\n", string);
#elif defined(SDL_PC)
#	if defined(SDL_FONTLIB_SFONT)
    SFont_Write(txtscr->surface, txtscr->font, xPos*txtscr->fontRes.x, yPos*txtscr->fontRes.y, string);
#	elif defined(SDL_FONTLIB_KBFLITE)
    KBF_Write(txtscr->font, txtscr->surface, xPos*txtscr->fontRes.x, yPos*txtscr->fontRes.y, string);
#	endif
#
#elif defined(SATURN_SGL)
    slPrint(string, slLocate(xPos, yPos));
#endif
    return RETURN_OK;
}


// uses TXTSCR_printf() to draw empty strings
ReturnValue TXTSCR_clearRectangle(void *scroll, unsigned char xPos, unsigned char yPos, unsigned char width, unsigned char height, TxtDrawMode drawMode)
{
    int i;
    char emptyString[width+1];
#if defined(SDL_PC)
    SDL_Rect rect;
#endif
    ReturnValue ret;
    Txt_Scroll *txtscr = (Txt_Scroll *)scroll;

    if(txtscr == NULL)
        return RETURN_ERR_NULLARG;
    
    if(((xPos + width) > txtscr->res.x) || ((yPos + height) > txtscr->res.y))
        return RETURN_ERR_INVALIDARG;

    // make the string empty
    for(i = 0; i < width; i++) {
        emptyString[i] = ' ';
    }
    emptyString[width] = '\0';
    
#if defined(SDL_PC)
    // we cannot clear an SDL surface by just drawing spaces
    rect.x = (xPos + txtscr->pos.x) * txtscr->fontRes.x;
    rect.y = (yPos + txtscr->pos.y) * txtscr->fontRes.y;
    rect.w = width * txtscr->fontRes.x;
    rect.h = height * txtscr->fontRes.y;
    SDL_FillRect(txtscr->surface, &rect, SDL_MapRGB(txtscr->surface->format, 0, 0, 0));
    if(drawMode != TXT_DRAW_LATER)
        SDL_UpdateRect(txtscr->surface, rect.x, rect.y, rect.w, rect.h);
    drawMode = TXT_DRAW_LATER;
#endif

    for(i = 0; i < height; i++) {
        ret = TXTSCR_printf(txtscr, xPos, yPos + i, drawMode, emptyString);
        if(ret != RETURN_OK)
            return ret;
    }
    return RETURN_OK;
}

ReturnValue TXTSCR_clear(void *scroll, TxtDrawMode drawMode)
{
    return TXTSCR_clearRectangle(scroll, 0, 0, TXTSCR_getWidth(scroll), TXTSCR_getHeight(scroll), drawMode);
}


// beware: string end not inserted, draws rest of line to screen
// position is relative to scroll, scroll pos relative to screen
ReturnValue TXTSCR_nprintf(void *scroll, unsigned char xPos, unsigned char yPos, unsigned char n, TxtDrawMode drawMode, const char *fmt, ...)
{
    va_list ap;
    char *ptr;
#if defined(SDL_PC)
    SDL_Rect rect;
#endif
    Txt_Scroll *txtscr = (Txt_Scroll *)scroll;

    if(txtscr == NULL)
        return RETURN_ERR_NULLARG;
    
    if((xPos >= txtscr->res.x) || (yPos >= txtscr->res.y))
        return RETURN_ERR_OOB;

    ptr = txtscr->txt + yPos*(txtscr->res.x+1) + xPos;
    if(drawMode != TXT_DRAW_NOW) {
        va_start (ap, fmt);
        n = vsnprintf(ptr, n, fmt, ap);
        va_end (ap);
    }
    
    if(drawMode != TXT_DRAW_LATER) {
#if defined(SDL_PC)
        // we cannot clear an SDL surface by just drawing spaces
        rect.x = (txtscr->pos.x + xPos) * txtscr->fontRes.x;
        rect.y = (txtscr->pos.y + yPos) * txtscr->fontRes.y;
        rect.w = n * txtscr->fontRes.x;
        rect.h = txtscr->fontRes.y;
        SDL_FillRect(txtscr->surface, &rect, SDL_MapRGB(txtscr->surface->format, 0, 0, 0));
#endif
        TXTSCR_drawLine(txtscr, ptr, txtscr->pos.x + xPos, txtscr->pos.y + yPos);
#if defined(SDL_PC)
        SDL_UpdateRect(txtscr->surface, (txtscr->pos.x + xPos) * txtscr->fontRes.x, (txtscr->pos.y + yPos) * txtscr->fontRes.y, n * txtscr->fontRes.x, txtscr->fontRes.y);
#endif
    }

    return RETURN_OK;
}

ReturnValue TXTSCR_printf(void *scroll, unsigned char xPos, unsigned char yPos, TxtDrawMode drawMode, const char *fmt, ...)
{
#if 0 // sorry, different args
    // other methods fail on Dev-Cpp
    void *arg, *ret;

    arg = __builtin_apply_args();
    ret = __builtin_apply((void *)TXTSCR_nprintf, arg, 100);
    __builtin_return(ret);
#else
    return TXTSCR_nprintf(scroll, xPos, yPos, TXTSCR_getWidth(scroll) - xPos, drawMode, fmt);
#endif    
}

ReturnValue TXTSCR_putc(void *scroll, unsigned char xPos, unsigned char yPos, TxtDrawMode drawMode, char c)
{
    char string[2];
    
    string[0] = c;
    string[1] = '\0';

    return TXTSCR_printf(scroll, xPos, yPos, drawMode, string);
}



ReturnValue TXTSCR_display(void *scroll)
{
    int i, j;
    char *ptr;
#if defined(SDL_PC)
    SDL_Rect rect;
#endif
    Txt_Scroll *txtscr = (Txt_Scroll *)scroll;

    if(txtscr == NULL)
        return RETURN_ERR_NULLARG;

    ptr = txtscr->txt;
    if(ptr == NULL)
        return RETURN_ERR_NULLARG;

    /* sprintf inserts '\0' after every string
     * so delete those which don't mark an eol
     */
    for(i = 0; i < txtscr->res.y; i++) {
        for(j = 0; j < txtscr->res.x; j++) {
            if(*(ptr) == '\0')
                *(ptr) = ' ';
                    
            ptr++;
        }
        *(ptr++) = '\0';
    }

    /* finally draw the result to screen */
#if defined(SDL_PC)
    // we cannot clear an SDL surface by just drawing spaces
    rect.x = txtscr->pos.x * txtscr->fontRes.x;
    rect.y = txtscr->pos.y * txtscr->fontRes.y;
    rect.w = txtscr->res.x * txtscr->fontRes.x;
    rect.h = txtscr->res.y * txtscr->fontRes.y;
    SDL_FillRect(txtscr->surface, &rect, SDL_MapRGB(txtscr->surface->format, 0, 0, 0));
#endif
    ptr = txtscr->txt;    
    for(i = 0; i < txtscr->res.y; i++) {
        /* ensure '\0' termination */
        *(ptr + txtscr->res.x) = '\0';
        TXTSCR_drawLine(txtscr, ptr, txtscr->pos.x, txtscr->pos.y + i);
        ptr += txtscr->res.x + 1;
    }
#if defined(SDL_PC)
    SDL_UpdateRect(txtscr->surface, txtscr->pos.x * txtscr->fontRes.x, txtscr->pos.y * txtscr->fontRes.y, txtscr->res.x * txtscr->fontRes.x, txtscr->res.y * txtscr->fontRes.y);
#endif
    return RETURN_OK;
}

