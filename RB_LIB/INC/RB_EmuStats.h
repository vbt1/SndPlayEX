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

#ifndef HAVE_RB_EMST_H
#define HAVE_RB_EMST_H

// define outside above #include <RB_EmuStats.h>
//#define EMST_DISABLE

#define	EMST_VERSION	"050128"

#include <RB_RETVAL.h>

// choose ONLY ONE option:
//#define EMST_USE_CLOCK
#define EMST_USE_FRT


// keep the interface free of SEGA_TIM.H,
// because it defines SEGA_XPT, so that
// #include <SGL.H> does not work 
#ifdef EMST_USE_CLOCK
    // use glibc's clock: returns 0xFFFF on Saturn
#   include <time.h>
typedef clock_t ClockValue;

#else
#   ifdef EMST_USE_FRT
    // use FRT
typedef unsigned int ClockValue;

#   endif
#endif

typedef struct CYCLE_COUNTER {
    // messuring an interval
    ClockValue start;
    // sub cycles which can be subtracted
    ClockValue sub;
    // accumulate result
        // the absolute value
    ClockValue abs_low, abs_high;
        // sub cycle counters eliminated
    ClockValue low, high;
    char *name;
} CycleCounter;

typedef struct COUNTER_STACK {
    unsigned char n;    // size of stack
    unsigned char i;    // i entries in stack
    unsigned char *cID;
} CounterStack;

typedef struct CYCLE_COUNTERS {
    unsigned int n;
    CycleCounter *cc;
    // need a reference;
    unsigned int frame;
    CounterStack cs;
} CycleCounters;

typedef struct OPCODE_COUNTERS {
    unsigned int n;
    unsigned int *oc;
    // for sorting: opcode ids
    unsigned short *oID;
} OpcodeCounters;

typedef struct EMU_STATS {
    CycleCounters ccs;
    OpcodeCounters ocs;
} EmuStats;

// my object
extern EmuStats emuStats;
// holds return value
// with all functions returning void,
// we can easily disable all stats
extern ReturnValue EMST_return;

#ifdef EMST_DISABLE

#define EMST_opcode(oc)
#define EMST_init(cycleCounters, opcodeCounters, stackDepth)
#define EMST_reset()
#define EMST_name(counterID, name)
#define EMST_exit()
#define EMST_start(counterID)
#define EMST_stop()
#define EMST_frames(frames)
#define EMST_display()

#else

#define EMST_opcode(opc)     *(emuStats.ocs.oc + opc) += 1;

#ifdef __cplusplus
extern "C" {
#endif

void EMST_init(unsigned char cycleCounters, unsigned short opcodeCounters, unsigned char stackDepth);
void EMST_reset();
void EMST_name(unsigned char counterID, char *name);
void EMST_exit();
void EMST_start(unsigned char counterID);
// stop last started counter
void EMST_stop();
// frames passed
void EMST_frames(unsigned char frames);
void EMST_display();

#ifdef __cplusplus
}
#endif

#endif  // EMST_ENABLE

#endif  // HAVE_RB_EMST_H
