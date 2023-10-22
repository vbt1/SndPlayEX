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

#ifndef HAVE_RB_TXT_SCROLL_INT_H
#define HAVE_RB_TXT_SCROLL_INT_H

#include <RB_CONFIG.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


typedef struct TXT_SCREEN {
    struct {
        unsigned char x;
        unsigned char y;
    } scrRes, fontRes;
    
#if defined(SDL_PC)
    SDL_Surface *surface;
#	if defined(SDL_FONTLIB_SFONT)
    SFont_Font* font;
#	elif defined(SDL_FONTLIB_KBFLITE)
    KBF_Font *font;
#	endif
#endif    
} Txt_Screen;

/* No hierarchy of nested scrolls yet
 * scroll position is relative to screen
 * don't cares if parts of scroll are outside screen
 * printf poritions are relative to scroll
 */

typedef struct TXT_SCROLL {
    struct {
        unsigned char x;
        unsigned char y;
    } pos, res, fontRes;
    char *txt;
    // links to the TxtScreen
#if defined(SDL_PC)
    SDL_Surface *surface;
#	if defined(SDL_FONTLIB_SFONT)
    SFont_Font* font;
#	elif defined(SDL_FONTLIB_KBFLITE)
    KBF_Font *font;
#	endif
#endif    
} Txt_Scroll;

#endif

