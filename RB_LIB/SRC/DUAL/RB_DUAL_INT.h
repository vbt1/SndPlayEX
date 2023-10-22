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

#ifndef HAVE_RB_DUAL_INT_H
#define HAVE_RB_DUAL_INT_H

// master <-> slave communication is to be minimized
// only once per frame

#include <RB_CONFIG.h>

#include <stdlib.h>

// the function only gets:
//  - a pointer to user variable area
//  - area size in bytes
//
// for runtime manipulation via dual lib calls:
// - it gets an additional pointer that must only be used by the lib itself
// - the prev and next pointers are needed, too
typedef struct DUAL_ELEMENT {
    // the function which executes this element
    void (*func)(void *ptr);
    void *work;
    unsigned int workSize;
} DualElement;

typedef struct DUAL_GROUP_SERIAL {
    void *first;
} DualGroupSerial;

typedef struct DUAL_GROUP {
    void *first;
    // these mark the entry points into the list
    // in a parallel group, they point to 2 disjoint lists
    void *master;
    void *slave;
    // registry of included capsules for runtime load balancing
    void **capsules;
    unsigned int nCapsules;
} DualGroupParallel;


// we could store the type of the linked object
// at the same place as the link,
// but that complicates manipulations at runtime 
typedef enum {
    DUAL_CAPSULE_ELEMENT,
    DUAL_CAPSULE_GROUP_SERIAL,
    DUAL_CAPSULE_GROUP_PARALLEL,
} DualCapsuleType;

typedef enum {
    // only master CPU
    DUAL_CPU_MASTER,
    // ONLY slave CPU
    DUAL_CPU_SLAVE,
    // don't care, both are equal
    DUAL_CPU_BOTH,
    // there are be parts included which force a certain CPU
    DUAL_CPU_MIXED
} DualCpuConstraint;

typedef struct DUAL_CAPSULE {
    DualCapsuleType type;
    union {
        DualElement element;
        DualGroupSerial serial;
        DualGroupParallel parallel;
    } data;
    // if next is NULL, it's the last of
    struct DUAL_CAPSULE *prev, *next;
    struct {
        DualCpuConstraint cpuConstr;
    } attributes;
} DualCapsule;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


#endif
