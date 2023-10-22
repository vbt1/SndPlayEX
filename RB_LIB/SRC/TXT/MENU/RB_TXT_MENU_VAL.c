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

#include <RB_TXT_MENU.h>
#include "RB_TXT_MENU_INT.h"

int *TXTMEN_getIntPtr(void *menu, unsigned char entry)
{
    void *content;

    if((menu == NULL) || (entry >= ((TxtMenu *)menu)->nEntries))
        return NULL;
    
    content = (((TxtMenu *)menu)->entries + entry)->content;
    switch((((TxtMenu *)menu)->entries + entry)->type) {
        case TXTMEN_SPINNER_INT:
            return &((TxtSpinnerInt *)content)->value;

        case TXTMEN_LISTBOX:
            return &((TxtListbox *)content)->value;

        case TXTMEN_CHECKBOX:
            return &((TxtCheckbox *)content)->value;
        
        default:
            return NULL;
    }
}

float *TXTMEN_getFloatPtr(void *menu, unsigned char entry)
{
    void *content;
    
    if((menu == NULL) || (entry >= ((TxtMenu *)menu)->nEntries))
        return NULL;
    
    content = (((TxtMenu *)menu)->entries + entry)->content;
    switch((((TxtMenu *)menu)->entries + entry)->type) {
        case TXTMEN_SPINNER_FLOAT:
            return &((TxtSpinnerFloat *)content)->value;

        default:
            return NULL;
    }
}


int TXTMEN_getIntValueAt(void *menu, unsigned char entry)
{
    int *ptr = TXTMEN_getIntPtr(menu, entry);
    
    if(ptr == NULL)
        return 0;
    else return *ptr;
}


float TXTMEN_getFloatValueAt(void *menu, unsigned char entry)
{
    float *ptr = TXTMEN_getFloatPtr(menu, entry);
    
    if(ptr == NULL)
        return 0.0;
    else return *ptr;
}

ReturnValue TXTMEN_setIntValueAt(void *menu, unsigned char entry, int value)
{
    int *ptr = TXTMEN_getIntPtr(menu, entry);
    
    if(ptr == NULL)
        return RETURN_ERR_INVALIDARG;
    
    *ptr = value;
    return RETURN_OK;
}


ReturnValue TXTMEN_setFloatValueAt(void *menu, unsigned char entry, float value)
{
    float *ptr = TXTMEN_getFloatPtr(menu, entry);
    
    if(ptr == NULL)
        return RETURN_ERR_INVALIDARG;
    
    *ptr = value;
    return RETURN_OK;
}
