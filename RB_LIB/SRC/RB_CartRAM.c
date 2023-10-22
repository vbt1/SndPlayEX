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

#include "RB_CartRAM.h"

unsigned int CartRAMsize;

// fixed errors:
//      long word access to get id instead of byte
//      long word access to write 1, instead of word
//      wrong A-Bus set register location:
//          0x25fe0080(is DSP program control port)
//          instead of 0x25fe00B0(CS0 and CS1 spaces)
//      forgot to set A-Bus refresh register at 0x25fe00B8   
//      memory map for DRAM0 and DRAM1 of manual seems to be correct
//      --> all stuff into CS0

//  RB_initRAMCart(Uint8 cs, Uint8 quiet)
//  - checks for present RAM cart, inits and verifies it
//
//  inputs:
//      cs          -   select whether to set CS0 or CS1 space 
//                      (both works, don't know where the difference is)   
//      quiet       -   don't print messages
//
//  returns:
//      cartridge id    -   on success
//                          (8MB: 0x5a or 32MB: 0x5c)
//      -1              -   no cart
//      -2              -   data verify failed 

Sint16 RB_CartRAM_init(Uint8 cs, Uint8 quiet)
{
    Uint32 setReg, refReg, *DRAM0, *DRAM1, DRAMsize, i, ok;
    Uint8 id;

    // cartridge initialization: as in ST-TECH-47.pdf
    // 1. verify cartridge id
    // 5A - 8 MB
    // 5C - 32 MB
    //! is last byte of A-BUS CS1 area
    id = *((Uint8 *)0x24ffffff);
    if(id == 0x5a) {
        CartRAMsize = 0x80000;
        if(quiet == 0)
            slPrint("8 MB RAM cart detected!", slLocate(8,5));
    } else if(id == 0x5c) {
        CartRAMsize = 0x200000;
        if(quiet == 0)
            slPrint("32 MB RAM cart detected!", slLocate(8,5));
    } else {
        CartRAMsize = 0x0;      
        // 2. prompt proper connection
        if(quiet == 0) {
            slPrint("The extended RAM", slLocate(11,5));
            slPrint("cartridge is not", slLocate(11,6));
            slPrint("inserted properly.", slLocate(11,7));

            slPrint("Please turn off", slLocate(11,9));
            slPrint("power and reinsert", slLocate(11,10));
            slPrint("the extended RAM", slLocate(11,11));
            slPrint("cartridge.", slLocate(11,12));
        }    
        return -1;
    }

    // 3. write 1 to initialize
    // is at the end of A-BUS Dummy area
    // !!! word format !!! (but what is meant?)
    *((Uint16 *)0x257efffe) = 1;
    
    // 4. set A-bus set and refresh registers
    setReg = refReg = 0;
    if(cs == 0) {
        // set cs0 for 32MBit
        setReg |= 1 << 29;  // After-READ precharge insert bit
        setReg |= 3 << 24;  // Burst cycle wait number
        setReg |= 3 << 20;  // Normal cycle wait number
    } else {
        // set cs1 for 16MBit ??
        setReg |= 1 << 28;  // external wait effective bit
        setReg |= 15 << 24; // Burst cycle wait number
        setReg |= 15 << 20; // Normal cycle wait number
    }
    *((Uint32 *)0x25fe00B0) = setReg;
        // A-Bus refresh register
    refReg |= 1 << 4;       // A-Bus refresh enable bit
            // I've just take this value
            // see SCU user manual for value range
    refReg |= 1;            // A-Bus refresh cycle wait
    *((Uint32 *)0x25fe00B8) = refReg;
    
    // 5. verify data before proceeding
        // cache-through DRAM0 and DRAM1
    DRAMsize = CartRAMsize >> 2; // byte length -> longword length
    DRAM0 = (Uint32 *)0x22400000;
    DRAM1 = (Uint32 *)0x22600000;
        // write
    for(i = 0; i < DRAMsize; i++) {
        *(DRAM0 + i) = i;
        *(DRAM1 + i) = DRAMsize - 1 - i;
    }    
        // read
    ok = 1;
    for(i = 0; i < DRAMsize; i++) {
        if(*(DRAM0 + i) != i) {
//            were_here("DRAM0 %i != %i", *(DRAM0 + i), i);
            ok = 0;
        }    
        if(*(DRAM1 + i) != (DRAMsize - 1 - i)) {
//            were_here("DRAM1 %i != %i", *(DRAM1 + i), (DRAMsize - 1 - i));
            ok = 0;
        }    
    } 
       
    if(ok == 1) {                        
        if(quiet == 0)
            slPrint("verifying RAM cart OK!", slLocate(8,7));
        return id;
    } else {
        if(quiet == 0)
            slPrint("verifying RAM cart FAILED!", slLocate(8,7));
        return -2;
    }    
}

