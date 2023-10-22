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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "RB_were_here.h"


void clearText()
{
    int i;
    char emptyLine[41] = "                                        ";
    
    for(i = 0; i < 30; i++) {
        slPrint(emptyLine, slLocate(0, i));
    }
}

void were_here(const char *fmt, ...)
{
    Uint16 data;
    char string[40];

    clearText();
    slPrint("We're here:", slLocate(1, 9));
     
#if 0 // sorry, vsnprintf got other args
    {
        // other methods fail on Dev-Cpp
        void *arg;
    
        arg = __builtin_apply_args();
        __builtin_apply((void *)vsnprintf, arg, 100);
    }    
#else
    {
        va_list ap;
        va_start (ap, fmt);
        vsnprintf(string, 40, fmt, ap);
        va_end (ap);
    }
#endif
    
    slPrint(string, slLocate(1, 10));

// fast exit when start is pressed
    if((Smpc_Peripheral[0].data & PER_DGT_ST) != 0) {
        do {
            data = Smpc_Peripheral[0].push;
            slPrint("press A", slLocate(1, 11));
            slSynch();
        } while((data & PER_DGT_TA) != 0);
    
        do {
            data = Smpc_Peripheral[0].push;
            slPrint("press B", slLocate(1, 11));
            slSynch();
        } while((data & PER_DGT_TB) != 0);
    }
    slPrint("       ", slLocate(1, 11));
    slSynch();
}

