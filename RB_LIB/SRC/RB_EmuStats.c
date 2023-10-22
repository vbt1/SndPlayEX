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

#include <RB_EmuStats.h>



#ifdef EMST_USE_CLOCK
    // use glibc's clock: returns 0xFFFF on Saturn
#   define EMST_CLOCK() clock()

#else
#   ifdef EMST_USE_FRT
    // use FRT
#       include <SEGA_TIM.H>
#       define EMST_CLOCK() TIM_FRT_GET_16()

#   endif
#endif


// my object
EmuStats emuStats;
ReturnValue EMST_return;

void EMST_init(unsigned char cycleCounters, unsigned short opcodeCounters, unsigned char stackDepth)
{
    unsigned short oID;

    memset(&emuStats, 0, sizeof(EmuStats));
    // Cycle Counters
    emuStats.ccs.cc = (CycleCounter *)malloc(cycleCounters * sizeof(CycleCounter));
    if(emuStats.ccs.cc == NULL) {
        EMST_return = RETURN_ERR_ALLOC;
        return;
    }    
    memset(emuStats.ccs.cc, 0, cycleCounters * sizeof(CycleCounter));
    emuStats.ccs.n = cycleCounters;
    // Opcode Counters
    emuStats.ocs.oc = (unsigned int *)malloc(opcodeCounters * sizeof(unsigned int));
    emuStats.ocs.oID = (unsigned short *)malloc(opcodeCounters * sizeof(unsigned short));
    if((emuStats.ocs.oc == NULL) || (emuStats.ocs.oID == NULL)) {
        EMST_return = RETURN_ERR_ALLOC;
        return;
    }    
        // opcode ids for sorting
    for(oID = 0; oID < opcodeCounters; oID++) {
        *(emuStats.ocs.oID + oID) = oID;
    }
    memset(emuStats.ocs.oc, 0, opcodeCounters * sizeof(unsigned int));
    emuStats.ocs.n = opcodeCounters;
    // Counter Stack
    emuStats.ccs.cs.cID = (unsigned char *)malloc(stackDepth * sizeof(unsigned char));
    if(emuStats.ccs.cs.cID == NULL) {
        EMST_return = RETURN_ERR_ALLOC;
        return;
    }    
    emuStats.ccs.cs.n = stackDepth;
    
    EMST_return = RETURN_OK;
    return;
}


void EMST_reset()
{
    emuStats.ccs.cs.i = 0;
    emuStats.ccs.frame = 0;
    memset(emuStats.ccs.cc, 0, emuStats.ccs.n * sizeof(CycleCounter));
    memset(emuStats.ocs.oc, 0, emuStats.ocs.n * sizeof(unsigned int));
}


void EMST_name(unsigned char counterID, char *name)
{
    if(counterID < emuStats.ccs.n) {
        EMST_return = RETURN_OK;
        (emuStats.ccs.cc + counterID)->name = name;
    } else EMST_return = RETURN_ERR_OOB;
    
    return;
}    


void EMST_exit()
{
    if(emuStats.ccs.cc != NULL)
        free(emuStats.ccs.cc);
    if(emuStats.ocs.oc != NULL)
        free(emuStats.ocs.oc);
    if(emuStats.ocs.oID != NULL)
        free(emuStats.ocs.oID);
    if(emuStats.ccs.cs.cID != NULL)
        free(emuStats.ccs.cs.cID);
    memset(&emuStats, 0, sizeof(EmuStats));

    EMST_return =  RETURN_OK;
    return;
}


void EMST_frames(unsigned char frames)
{
    emuStats.ccs.frame += frames;
}


void EMST_start(unsigned char counterID)
{
    // check counterID
    if(counterID >= emuStats.ccs.n) {
        EMST_return = RETURN_ERR_OOB;
        return;
    }    
    // push on stack
    if(emuStats.ccs.cs.i < emuStats.ccs.cs.n) {
        *(emuStats.ccs.cs.cID + emuStats.ccs.cs.i++) = counterID;
    } else {
        EMST_return = RETURN_ERR_OOB;
        return;
    }    
    // start counter
    (emuStats.ccs.cc + counterID)->sub = 0;
    (emuStats.ccs.cc + counterID)->start = EMST_CLOCK();

    EMST_return = RETURN_OK;
    return;
}


// stop last started counter
void EMST_stop()
{
    ClockValue cycles;
    CycleCounter *cc;
        
    // check stack
    if(emuStats.ccs.cs.i == 0) {
        EMST_return = RETURN_ERR_UNKNOWN;
        return;
    }    
    // pop stack
    cc = (emuStats.ccs.cc + *(emuStats.ccs.cs.cID + --emuStats.ccs.cs.i));

    // stop counting
    cycles = EMST_CLOCK();
    if(cycles >= cc->start)
      cycles -= cc->start;
    else cycles = 0;    // maybe overflow in the supplied EMST_CLOCK() function
    
    // acknowledge parent counter in stack
    if(emuStats.ccs.cs.i > 0)
        (emuStats.ccs.cc + *(emuStats.ccs.cs.cID + emuStats.ccs.cs.i - 1))->sub += cycles;

    // update own stats
    // with overflow detection
        // absolute
    if((cc->abs_low + cycles) < cc->abs_low)
        cc->abs_high++;
    cc->abs_low += cycles;
        // subcounters eliminated
    cycles -= cc->sub;
    if((cc->low + cycles) < cc->low)
        cc->high++;
    cc->low += cycles;

    EMST_return = RETURN_OK;
    return;
}

#define EMST_SORT_VALUE(i)    *(emuStats.ocs.oc + *(emuStats.ocs.oID + i))

void EMST_opcodeSort(int l, int r)
{
    int i = l, j = r, pivot = EMST_SORT_VALUE(((l+r) >> 1));
    unsigned short tmp;
    
    do {
        while(EMST_SORT_VALUE(i) < pivot)
            i++;
        while(EMST_SORT_VALUE(j) > pivot)
            j--;
        if(i <= j) {
            tmp = *(emuStats.ocs.oID + j);
            *(emuStats.ocs.oID + j) = *(emuStats.ocs.oID + i);
            *(emuStats.ocs.oID + i) = tmp;
            i++;
            j--;
        }
    } while(i <= j);
    if(l < j)
        EMST_opcodeSort(l, j);
    if(i < r)
        EMST_opcodeSort(i, r);
}

void EMST_display()
{
    unsigned char nCounter, nOpcodes;
    int i, opc;
    
    EMST_opcodeSort(0, emuStats.ocs.n-1);
    
    slPrint("counter:   cycles:", slLocate(0, 1));
    if(emuStats.ccs.n <= 13)
        nCounter = emuStats.ccs.n;
    else nCounter = 13;
    for(i = 0; i < nCounter; i++) {
        if((emuStats.ccs.cc + i)->name == NULL)
            slPrintHex(i, slLocate(0, 2+i));
        else slPrint((emuStats.ccs.cc + i)->name, slLocate(0, 2+i));
        slPrintHex((emuStats.ccs.cc + i)->low / (emuStats.ccs.frame == 0 ? 1 : emuStats.ccs.frame), slLocate(10, 2+i));    
//        slPrintHex((emuStats.ccs.cc + i)->low, slLocate(22, 2+i));    
    }


    slPrint(" opcode:   cycles:", slLocate(20, 1));
    if(emuStats.ocs.n <= 13)
        nOpcodes = emuStats.ocs.n;
    else nOpcodes = 13;
    for(i = 0; i < nOpcodes; i++) {
        opc = emuStats.ocs.n - 1 - i;
        slPrintHex(*(emuStats.ocs.oID + opc), slLocate(20, 2+i));    
        slPrintHex(EMST_SORT_VALUE(opc), slLocate(30, 2+i));    
    }
    
    // care about overflow in the opcode counters
    if(EMST_SORT_VALUE(emuStats.ocs.n - 1) >= 0x80000000) {
        for(i = 0; i < emuStats.ocs.n; i++)
           *(emuStats.ocs.oc + i) >>= 1;
    }   
}

