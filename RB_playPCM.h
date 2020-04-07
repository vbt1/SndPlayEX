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


#ifndef HAVE_RB_PLAYPCM_H
#define HAVE_RB_PLAYPCM_H

#define	PLAYPCM_VERSION	"050311"

//differences between :
//SGL raw PCM <-> SBL PCM/ADPCM <-> SBL CD-ROM XA
//- the files of the middle type got a header with sound attributes,
//  all other don't have this, so it must be supplied seperately

enum {    
// beware: order matters on some of them
    PLAYPCM_MENU_LOADCD,
    PLAYPCM_MENU_RESCAN,
    PLAYPCM_MENU_STOPCD,
    PLAYPCM_MENU_PLAY,
    PLAYPCM_MENU_STOP,
    PLAYPCM_MENU_REPEAT,
    PLAYPCM_MENU_SLOT,
    PLAYPCM_MENU_CHANNELS,
    PLAYPCM_MENU_RESOLUTION,
    PLAYPCM_MENU_SAMPLERATE,
    PLAYPCM_MENU_OCT,
    PLAYPCM_MENU_FNS,
    PLAYPCM_MENU_LEVEL,
    PLAYPCM_MENU_PAN,
    PLAYPCM_MENU_LENGTH,
    PLAYPCM_MENU_STATUS,
    // exit button
    PLAYPCM_MENU_EXIT,
    PLAYPCM_MENU_AFTER,
};




#ifdef __cplusplus
extern "C" {
#endif

void PLAYPCM_MENU_callback(int item, void *ptr);
void PLAYPCM_MENU_init(void **menu, void *scroll, unsigned char *exitText, unsigned char vblanks);

#ifdef __cplusplus
}
#endif

#endif //HAVE_RB_PLAYPCM_H

