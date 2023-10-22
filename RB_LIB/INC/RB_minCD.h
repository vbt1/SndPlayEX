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

#ifndef HAVE_RB_MINCD_H
#define HAVE_RB_MINCD_H

#define	MINCD_VERSION	"050218"

//Controls are:
//    B               -   exit without loading/opening a file
//    A,C,START       -   load/open the selected file or change to selected directory
//    D-PAD L,R       -   change file slow
//    SHOULDER L,R    -   change file fast
//    X               -   rescan CD
//    Y               -   stop CD spinning

#include <RB_RETVAL.H>

extern char *gfsFileName;
//extern char *gfsFileExtension;


//#include "L:\saturn\INC\sgl_cd.h"
#include <SEGA_GFS.H>
// these flags set allowed file operations
#define FIO_OP_COPY        (1 << 0)
#define FIO_OP_DELETE      (1 << 1)
#define FIO_OP_RENAME      (1 << 2)
    // only one of these
#define FIO_OP_LOAD        (1 << 3)
#define FIO_OP_GET_FID     (1 << 4)
#define FIO_OP_OPEN_GFS    (1 << 5)
//#define FIO_OP_OPEN_SGL    (1 << 6)
typedef unsigned char FIO_ops;

typedef struct FIO_FTYPE {
    // this type's filenames match this pattern
    char *pattern;
    // and this size constraints
    struct {
        unsigned int min, max;
    } size;
    char *description;
    FIO_ops ops;
} FIO_ftype;

typedef struct FIO_IN {
    FIO_ftype *ftypes;
    unsigned char nftypes;
    // location to load file to
    unsigned char *dest;
    unsigned int destSize;
} FIO_in;

// the output of fileio
typedef enum {
    FIO_NONE,
    FIO_LOADED,
    FIO_GOT_FID,
    FIO_OPENED_GFS //,
//    FIO_OPENED_SGL
} FIO_otype;

typedef struct FIO_OUT {
    union {
        unsigned int size;      // FIO_LOADED
        Sint32 fid;             // FIO_FID
        GfsHn gfs;              // FIO_OPENED_GFS
     //   CDHN cdhn;              // FIO_OPENED_SGL
    } value;
    FIO_otype otype;
    unsigned char fti;        // file type index, which pattern does this file match
} FIO_out;


#ifdef __cplusplus
extern "C" {
#endif

int CdUnlock();
Sint32 MINCD_init();
ReturnValue MINCD_load(char *title, FIO_in *in, FIO_out *out);

#ifdef __cplusplus
}
#endif

#endif //HAVE_RB_MINCD_H
