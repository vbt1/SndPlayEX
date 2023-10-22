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

#ifndef HAVE_RB_TXT_SCROLL_H
#define HAVE_RB_TXT_SCROLL_H

#define	TXTSCR_VERSION	"050209"

#include <RB_RETVAL.h>


// !!! assumes a SDL font with constant spacing, maybe height = width (KBF)

// draw modes for TXTSCR_printf()
typedef enum TXT_DRAW {
    TXT_DRAW_NOW,					// draw string directly to screen
    TXT_DRAW_LATER,					// draw only to txtscr
    TXT_DRAW_BOTH
} TxtDrawMode;

#ifdef __cplusplus
extern "C" {
#endif

char *TXTSCR_getVersion();
ReturnValue TXTSCR_initScreen(void **screen, unsigned char xRes, unsigned char yRes);
ReturnValue TXTSCR_initScroll(void **scroll, void *screen, unsigned char xRes, unsigned char yRes, unsigned char xPos, unsigned char yPos);

ReturnValue TXTSCR_exitScroll(void *scroll);
ReturnValue TXTSCR_exitScreen(void *screen);
ReturnValue TXTSCR_clearRectangle(void *scroll, unsigned char xPos, unsigned char yPos, unsigned char width, unsigned char height, TxtDrawMode drawMode);
ReturnValue TXTSCR_clear(void *scroll, TxtDrawMode drawMode);
ReturnValue TXTSCR_nprintf(void *scroll, unsigned char xPos, unsigned char yPos, unsigned char n, TxtDrawMode drawMode, const char *fmt, ...);
ReturnValue TXTSCR_printf(void *scroll, unsigned char xPos, unsigned char yPos, TxtDrawMode drawMode, const char *fmt, ...);
ReturnValue TXTSCR_putc(void *scroll, unsigned char xPos, unsigned char yPos, TxtDrawMode drawMode, char c);
ReturnValue TXTSCR_display(void *scroll);

int TXTSCR_getWidth(void *scroll);
int TXTSCR_getHeight(void *scroll);

#ifdef __cplusplus
}
#endif

#endif
