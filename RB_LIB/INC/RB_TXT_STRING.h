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

#ifndef HAVE_RB_TXT_STRING_H
#define HAVE_RB_TXT_STRING_H

#include <RB_RETVAL.h>

#define	TXTSTR1_VERSION	"041214"
#define	TXTSTR2_VERSION	"041214"


typedef enum {
    ATTACH_TO_A,
    ATTACH_TO_B,
    ATTACH_TO_C,
    ATTACH_TO_X,
    ATTACH_TO_Y,
    ATTACH_TO_Z
} AttachTo;

typedef enum {
    ATTACH_AS_FIRST,
    ATTACH_AS_SECOND
} AttachAs;

#ifdef __cplusplus
extern "C" {
#endif

char *TXTSTR1_getVersion();
ReturnValue TXTSTR1_init(void **ptr, void *scroll);
ReturnValue TXTSTR1_setCharTbl(void *ptr, char *string, unsigned char xPos, unsigned char yPos, AttachTo attTo, AttachAs attAs);
ReturnValue TXTSTR1_setString(void *ptr, char *string, unsigned char xPos, unsigned char yPos, unsigned char maxLength);
ReturnValue TXTSTR1_setName(void *ptr, char *name, unsigned char xPos, unsigned char yPos);
ReturnValue TXTSTR1_execute(void *ptr);


char *TXTSTR2_getVersion();
ReturnValue TXTSTR2_init(void **ptr, void *scroll);
ReturnValue TXTSTR2_setCharTbl(void *ptr, char *string, unsigned char xPos, unsigned char yPos);
ReturnValue TXTSTR2_setName(void *ptr, char *name, unsigned char xPos, unsigned char yPos);
ReturnValue TXTSTR2_setString(void *ptr, char *string, unsigned char xPos, unsigned char yPos, unsigned char maxLength);
//ReturnValue TXTMEN_redrawString(void *ptr);
ReturnValue TXTSTR2_redrawAll(void *ptr);
ReturnValue TXTSTR2_execute(void *ptr);

#ifdef __cplusplus
}
#endif


#endif
