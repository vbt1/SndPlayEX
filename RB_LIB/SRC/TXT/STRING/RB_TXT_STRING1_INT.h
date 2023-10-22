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

#ifndef HAVE_RB_TXT_STRING1_INT_H
#define HAVE_RB_TXT_STRING1_INT_H

#include <RB_CONFIG.h>
#include <RB_TXT_SCROLL.h>
#include <RB_INPUT.h>
#include "RB_TXT_STRING_INT.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


enum {
	STATE_ACTIVE_A,
	STATE_ACTIVE_B,
	STATE_ACTIVE_C,
	STATE_ACTIVE_X,
	STATE_ACTIVE_Y,
	STATE_ACTIVE_Z,
	STATE_PASSIVE,
};

typedef struct STRING_INPUT1 {
    StringBuffer strbuf;
    // every button gotup to 2 chartables attached
    CharTable chartbls[6][2];
    void *scroll;
    TxtDrawMode drawMode;
    struct {
        unsigned char x, y;
    } cursor;
    struct {
        AttachAs charset;
        unsigned char leave, redrawString, redrawCursor, state;
    } status;
    Name name;
} StringInput1;

#endif

