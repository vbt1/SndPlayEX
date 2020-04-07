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


#ifndef HAVE_RB_MENU3D_H
#define HAVE_RB_MENU3D_H

#define	MENU3D_VERSION	"050308"

enum {
    // 3D debug  
    MENU3D_MENU_GOURAUD,  
    MENU3D_MENU_SCAX,
    MENU3D_MENU_SCAY,
    MENU3D_MENU_SCAZ,
    MENU3D_MENU_ANGX,
    MENU3D_MENU_ANGY,
    MENU3D_MENU_ANGZ,
    MENU3D_MENU_POSX,
    MENU3D_MENU_POSY,
    MENU3D_MENU_POSZ,
    MENU3D_MENU_LIGHTX,
    MENU3D_MENU_LIGHTY,
    MENU3D_MENU_LIGHTZ,
    // exit button
    MENU3D_MENU_EXIT,
    MENU3D_MENU_AFTER,
};


#ifdef __cplusplus
extern "C" {
#endif

void MENU3D_defaults();
void MENU3D_MENU_callback(int item, void *ptr);
void MENU3D_MENU_init(void **menu, void *scroll, unsigned char *exitText);

#ifdef __cplusplus
}
#endif

#endif //HAVE_RB_MENU3D_H

