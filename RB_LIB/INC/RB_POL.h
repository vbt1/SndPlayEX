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

#ifndef HAVE_RB_POL_H
#define HAVE_RB_POL_H

#include <RB_RETVAL.H>

// reverses SGL's toFIXED()
#define toFLOAT(fx)			(((float)(fx)) / 65536.0)

#ifndef _SGL_H_
#include "RB_SGL_def.h"
#endif

#include "RB_POL_def.h"


typedef struct POINT_ENTRY {
    // the old index is search key
    Uint16 old_index;
    Uint16 new_index;
} PointEntry;


// although multiple implementations exist for that,
// it won't work always
//#define WRAP_PRINTF


#ifndef WRAP_PRINTF
extern unsigned char flag_print_notes;
extern unsigned char flag_print_warnings;
extern unsigned char flag_print_errors;
#include <stdio.h>
#	define note_printf		if(flag_print_notes == 1) printf
#	define warning_printf 	if(flag_print_warnings == 1) printf
#	define error_printf 	if(flag_print_errors == 1) printf
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WRAP_PRINTF
int note_printf(const char *fmt, ...);
int warning_printf(const char *fmt, ...);
int error_printf(const char *fmt, ...);
#endif

ReturnValue POL_updateMinMax(POINT *pntbl, Uint32 nbPoint, FIXED *min, FIXED *max, unsigned char dim);

void POL_clearXPDATA(XPDATA *data, unsigned char type);
// computing reference points
ReturnValue POL_findObjectMid(XPDATA *data, FIXED *mid, unsigned char dim);
ReturnValue POL_findPolygonMid(XPDATA *data, Uint16 index, FIXED *mid, unsigned char dim);
ReturnValue POL_findObjectMean(XPDATA *data, FIXED *mean, unsigned char dim);
ReturnValue POL_findPolygonMean(XPDATA *data, Uint16 index, FIXED *mean, unsigned char dim);

// brute force comparison
int POL_comparePoints(FIXED *a, FIXED *b, unsigned char dim);
int POL_comparePolygons(XPDATA *a, Uint16 indexa, XPDATA *b, Uint16 indexb);

// object creation from other objects
ReturnValue POL_copyPoint(XPDATA *xpdata1, XPDATA *xpdata2, unsigned char type, PointEntry **pe_array, Uint16 old_index, Uint16 *new_index);
ReturnValue POL_copyPolygon(XPDATA *xpdata1, XPDATA *xpdata2, unsigned char type, PointEntry **pe_array, unsigned int index);
ReturnValue POL_removeDoubles(XPDATA *data, unsigned char type, unsigned char free_old);

int POL_searchPolygon(XPDATA *data, unsigned char type, Uint16 start, Uint16 end, SearchData *search);
ReturnValue POL_computeGround(GroundData *ground, XPDATA *data, unsigned char type, Uint16 index, unsigned char typeID);
ReturnValue POL_checkGround(GroundData *ground, FIXED *pos, unsigned char *hit, unsigned char *typeID, FIXED *height);

#ifdef __cplusplus
}
#endif

#endif // HAVE_RB_POL_H
