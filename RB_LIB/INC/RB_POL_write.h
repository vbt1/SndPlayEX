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

#ifndef HAVE_RB_POL_WRITE_H
#define HAVE_RB_POL_WRITE_H

#include <RB_RETVAL.H>
#include "RB_POL_def.h"
#include "RB_MAP_def.h"

// to comply withe SEGA Saturn the
// output binary format is 
// - big endian
// - aligned to avoid unaligned accesses


// swap bytes if this machine got little endian
#define SWAP_BYTES

#ifdef SWAP_BYTES
#	define SWAP2(d)	( (((d) & 0xFF00) >> 8) | (((d) & 0x0000FF) << 8) )
#	define SWAP4(d)	( (((d) & 0xFF000000) >> 24) | (((d) & 0x00FF0000) >> 8) | (((d) & 0x0000FF00) << 8) | (((d) & 0x000000FF) << 24) )
#else
#	define SWAP2(d)	(d)
#	define SWAP4(d)	(d)
#endif


#ifdef __cplusplus
extern "C" {
#endif

void POL_swapATTR(ATTR *attr);
void POL_swapPOLYGON(POLYGON *pol);
void POL_swapXPDATA(XPDATA *data, unsigned char type);
void POL_swapGroundData(GroundData *ground);
ReturnValue alignResizeBuffer(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int mySize, unsigned char alignBytes);
ReturnValue POL_writeFIXEDarray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, FIXED *data, unsigned int n);
ReturnValue POL_writePOLYGONarray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, POLYGON *data, unsigned int n);
ReturnValue POL_writeATTRarray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, ATTR *data, unsigned int n);
ReturnValue POL_writeXPDATA(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, XPDATA *data, unsigned char type);
ReturnValue POL_writeGroundDataArray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, GroundData *data, unsigned int n);


#ifdef __cplusplus
}
#endif

#endif // HAVE_RB_POL_WRITE_H
