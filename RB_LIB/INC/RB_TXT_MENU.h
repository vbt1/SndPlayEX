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

#ifndef HAVE_RB_TXT_MENU_H
#define HAVE_RB_TXT_MENU_H

#define	TXTMEN_VERSION	"050308"

// caution:
// on Saturn, you should set
// void callafter(TxtMenu *txtmen)
// {
// #if defined(SATURN_SGL)
//    slSynch();
// #endif
// }
//
// otherwise emulator Satourne will crash for some reason

#include <RB_RETVAL.h>





#ifdef __cplusplus
extern "C" {
#endif

char *TXTMEN_getVersion();
ReturnValue TXTMEN_init(void **menu, void *scroll, char *name);

// insert an entry at specified position
// need to alloc before
ReturnValue TXTMEN_addEntries(void *menu, unsigned char entries);
ReturnValue TXTMEN_addSpinnerIntAt(void *menu, unsigned char entry, char *name, int min, int max, int diff, int value);
ReturnValue TXTMEN_addSpinnerFloatAt(void *menu, unsigned char entry, char *name, float min, float max, float diff, float value);
ReturnValue TXTMEN_addListboxAt(void *menu, unsigned char entry, char *name);
ReturnValue TXTMEN_addCheckboxAt(void *menu, unsigned char entry, char *name, unsigned char value);

// add an entry of this type as new last entry
ReturnValue TXTMEN_addSpinnerInt(void *menu, char *name, int min, int max, int diff, int value);
ReturnValue TXTMEN_addSpinnerFloat(void *menu, char *name, float min, float max, float diff, float value);
ReturnValue TXTMEN_addListbox(void *menu, char *name);
ReturnValue TXTMEN_addButton(void *menu, char *name);
ReturnValue TXTMEN_addCheckbox(void *menu, char *name, unsigned char value);

ReturnValue TXTMEN_addItemAt(void *menu, unsigned char entry, char *name);
ReturnValue TXTMEN_addItem(void *menu, char *name);

ReturnValue TXTMEN_disableAt(void *menu, unsigned char entry);
ReturnValue TXTMEN_disable(void *menu);

ReturnValue TXTMEN_enableAt(void *menu, unsigned char entry);
ReturnValue TXTMEN_enable(void *menu);

ReturnValue TXTMEN_setCallbackAt(void *menu, unsigned char entry, int item, void *ptr, void (*func)(int item, void *ptr));
ReturnValue TXTMEN_setCallback(void *menu, int item, void *ptr, void (*func)(int item, void *ptr));
ReturnValue TXTMEN_setCallafter(void *menu, int item, void *ptr, void (*func)(int item, void *ptr));

// execution interface
ReturnValue TXTMEN_redrawValueAt(void *menu, unsigned char entry);
ReturnValue TXTMEN_redrawMenu(void *menu);
ReturnValue TXTMEN_execute(void *menu);
ReturnValue TXTMEN_leave(void *menu);


// value interface
int TXTMEN_getIntValueAt(void *menu, unsigned char entry);
float TXTMEN_getFloatValueAt(void *menu, unsigned char entry);
ReturnValue TXTMEN_setIntValueAt(void *menu, unsigned char entry, int value);
ReturnValue TXTMEN_setFloatValueAt(void *menu, unsigned char entry, float value);

#ifdef __cplusplus
}
#endif

#endif
