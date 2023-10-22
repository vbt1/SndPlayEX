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

#include <RB_DUAL.h>
#include <RB_DUAL_INT.h>

char *DUAL_getVersion()
{
    return DUAL_VERSION;
}



ReturnValue DUAL_addCapsuleTo(void *prev, void **next, DualCapsuleType type)
{
    DualCapsule *nxt;
    
    if(next == NULL)
        return RETURN_ERR_NULLARG;
        
    nxt = (DualCapsule *)calloc(1, sizeof(DualCapsule));
    if(nxt == NULL)
        return RETURN_ERR_ALLOC;

    // setting non zero variables
    nxt->type = type;
    nxt->prev = (DualCapsule *)prev;
    nxt->attributes.cpuConstr = DUAL_CPU_BOTH;
        
    *next = nxt;
    
    return RETURN_OK;   
}

ReturnValue DUAL_addElementTo(void *prev, void **next, void (*func)(void *ptr), unsigned int workSize)
{
    ReturnValue ret;
    
    if(func == NULL)
        return RETURN_ERR_NULLARG;
    
    ret = DUAL_addCapsuleTo(prev, next, DUAL_CAPSULE_ELEMENT);
    if(ret != RETURN_OK)
        return ret;

    ((DualCapsule *)*next)->data.element.work = (void *)calloc(1, workSize);
    if(((DualCapsule *)*next)->data.element.work == NULL)
        return RETURN_ERR_ALLOC;
        
    ((DualCapsule *)*next)->data.element.workSize = workSize;
    ((DualCapsule *)*next)->data.element.func = func;
    
    return RETURN_OK;
}

ReturnValue DUAL_addGroupTo(void *prev, void **next, DualCapsuleType type)
{
    ReturnValue ret;
    
    if(type == DUAL_CAPSULE_ELEMENT)
        return RETURN_ERR_INVALIDARG;
    
    ret = DUAL_addCapsuleTo(prev, next, type);
    if(ret != RETURN_OK)
        return ret;
  
    return RETURN_OK;
}

// !!! attention: first points to the location of a where the first
// capsule will be linked from
ReturnValue DUAL_openGroup(void *group, void **first)
{
    if(group == NULL)
        return RETURN_ERR_NULLARG;
        
    switch(((DualCapsule *)group)->type) {
        case DUAL_CAPSULE_ELEMENT:
            *first = NULL;
            return RETURN_ERR_INVALIDARG;
        
        case DUAL_CAPSULE_GROUP_SERIAL:
            *first = &(((DualCapsule *)group)->data.serial.first);
            return RETURN_OK;

        case DUAL_CAPSULE_GROUP_PARALLEL:
            *first = &(((DualCapsule *)group)->data.parallel.first);
            return RETURN_OK;
    }

    return RETURN_ERR_INVALIDARG;
}

// postprocessing to prepare runtime usage
ReturnValue DUAL_closeGroup(void *group)
{

    return RETURN_OK;
}    


void *DUAL_getPrev(void *ptr)
{
    if(ptr == NULL)
        return NULL;
        
    return ((DualCapsule *)ptr)->prev;
}

void *DUAL_getNext(void *ptr)
{
    if(ptr == NULL)
        return NULL;
        
    return ((DualCapsule *)ptr)->next;
}

void *DUAL_getWork(void *ptr)
{
    if(ptr == NULL)
        return NULL;
    
    if(((DualCapsule *)ptr)->type != DUAL_CAPSULE_ELEMENT)
        return NULL;
        
    return ((DualCapsule *)ptr)->data.element.work;
}


unsigned int DUAL_getWorkSize(void *ptr)
{
    if(ptr == NULL)
        return 0;
    
    if(((DualCapsule *)ptr)->type != DUAL_CAPSULE_ELEMENT)
        return 0;
        
    return ((DualCapsule *)ptr)->data.element.workSize;
}

