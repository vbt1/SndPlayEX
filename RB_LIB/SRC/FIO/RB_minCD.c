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

//!!!!!!!!! upper and lower case of extension needs to be fixed
#include <SGL.H>

#include <RB_minCD.h>
#include <RB_were_here.h>

#define   OPEN_MAX  (Sint32)5
#define   DIR_MAX   (Sint32)100    

#include <fnmatch.h>

//extern GfsMng *gfs_mng_ptr;
// keep this info global
static GfsDirTbl gfsDirTbl;
static GfsDirId gfsDirId[DIR_MAX];
static Uint32 gfsLibWork[GFS_WORK_SIZE(OPEN_MAX)/sizeof(Uint32)];
// number of files in current directory
static Sint32 gfsDirN = 0;
static Uint16 cursor = 2;
// the name of the file loaded
//char *gfsFileName;
//char *gfsFileExtension;


/* for initializing the CD-ROM when testing with PAR */
/*
#define SYS_CDINIT1(i) \
     ((**(void(**)(int))0x60002dc)(i))

#define SYS_CDINIT2() \
     ((**(void(**)(void))0x600029c)())

int CdUnlock (void)
{
     Sint32 ret;
     CdcStat stat;
     volatile int delay;

     SYS_CDINIT1(3);

     SYS_CDINIT2();

     do {
          for(delay = 100000; delay; delay--);

          ret = CDC_GetCurStat(&stat);

     } while ((ret != 0) || (CDC_STAT_STATUS(&stat) == 0xff));

     return (int) CDC_STAT_STATUS(&stat);
}
*/

//-------------------------------------------------------------------------------------------------------------------------------------
/*void errGfsFunc(void *obj, Sint32 ec)
{
	char texte[50];
	sprintf(texte, "ErrGfs        %X %X",obj, ec); 
	slPrint(texte, slLocate(2, 8));
	do
	{
		Sint32      ret;
		GfsErrStat  stat;
		GFS_GetErrStat(&stat);
		ret = GFS_ERR_CODE(&stat);
		sprintf(texte, "ErrGfsCode %X",ret); 

		slPrint(texte, slLocate(2, 9));
//		slSynch();
	}while(1);
}*/
//-------------------------------------------------------------------------------------------------------------------------------------
Sint32 MINCD_init()
{
//	CDC_CdInit(0x00,0x00,0x05,0x0f);
//	GFS_DIRTBL_TYPE(&gfsDirTbl) = GFS_DIR_ID;
	GFS_DIRTBL_TYPE(&gfsDirTbl) = GFS_DIR_NAME;
	
    GFS_DIRTBL_DIRID(&gfsDirTbl) = gfsDirId;
    GFS_DIRTBL_NDIR(&gfsDirTbl) = DIR_MAX;
    gfsDirN = GFS_Init(OPEN_MAX, gfsLibWork, &gfsDirTbl);

    return gfsDirN;        
}

//FIO_findMatch:
//ignores case
//returns
//    ftype index     - on success
//    -1              - else

signed short FIO_findMatch(char *fname, unsigned char fsize, FIO_ftype *ftypes, unsigned char nftypes)
{
    unsigned char i;
    signed short match = -1;
    
    for(i = 0; i < nftypes; i++) {
        // check size constraint
        if(((ftypes + i)->size.min <= fsize) && (fsize <= (ftypes + i)->size.max)) {
            // check name constraint
            if(fnmatch((ftypes + i)->pattern, fname, FNM_CASEFOLD) == 0) {
                match = i;
                break;
            }
        }    
    }
    return match;
}

/* 
 * fileExt      -   string with valid file extionsions like
 *                  ".bin.smd.rom"
 *                  !!! don't forget the dot .
 *
 * return       -   bytes loaded on success, 0 otherwise
 */
//output screen:
//    line    what
//---------------------
//    6       max size:
//    11      File num:
//    13      File id:
//    15      File size:  TOO BIG!
//    17      File type:  file / directory
//    19      File name:  name 
//    21      File descr: description / INVALID!
//    23      Loading File... / Changing dir...  OK! / FAILED!   
ReturnValue MINCD_load(char *title, FIO_in *in, FIO_out *out)
{
    Uint16 data;
    Uint8 validName;
    Sint32 fid, load, set;
    signed short fti;   // file type index


    slPrint(title, slLocate(2, 3));   
    // restrictions
    slPrint("max size:                               ", slLocate(2, 6));   
    slPrintHex(in->destSize, slLocate(14, 6));   

    // wait for the release of important keys
    do {
        data = Smpc_Peripheral[0].push;
        slSynch();
    } while(((data & PER_DGT_TB) == 0) || ((data & PER_DGT_TA) == 0) || ((data & PER_DGT_TC) == 0) || ((data & PER_DGT_ST) == 0));

    while(1) {
        // process inputs
        data = Smpc_Peripheral[0].push;
            // first, check if the CD has been changed
            // don't care if a CD is inserted
        if(((CDC_GetHirqReq() & CDC_HIRQ_DCHG) != 0) || ((data & PER_DGT_TX) == 0)) {
            // after every CD change, the data track cannot be accessed
            // but CDDA instead can
 //           CdUnlock();
            MINCD_init();
        }
            // stop CD
        if((data & PER_DGT_TY) == 0) {
            CdcPos poswk;
            // seek home position
            CDC_POS_PTYPE(&poswk) = CDC_PTYPE_DFL;
            CDC_CdSeek(&poswk);
        }    
        
            // exit, no file loaded
        if((data & PER_DGT_TB) == 0) {
            out->otype = FIO_NONE;
            return RETURN_OK;;
        }    
            // file change: fast with L and R
        if((((data & PER_DGT_KL) == 0) || ((Smpc_Peripheral[0].data & PER_DGT_TL) == 0)) && (cursor > 0)) {
            cursor--;
            slPrint("                                        ", slLocate(2, 23));
        }    
        if((((data & PER_DGT_KR) == 0) || ((Smpc_Peripheral[0].data & PER_DGT_TR) == 0)) && (cursor < (gfsDirN-1))) {
            cursor++;
            slPrint("                                        ", slLocate(2, 23));
        }    
//----------------------------------------------------		
		Sint32 tfid, fn, fsize, atr;
		GfsHn gfs = GFS_Open(cursor);		
		GFS_GetFileInfo(gfs,&tfid,&fn,&fsize,&atr);		
		GFS_Close(gfs);	
//----------------------------------------------------		
        fti = FIO_findMatch(GFS_IdToName(cursor), fsize, in->ftypes, in->nftypes);
        out->fti = fti;

        // file select
        if(((data & PER_DGT_TA) == 0) || ((data & PER_DGT_TC) == 0) || ((data & PER_DGT_ST) == 0)) {
            // load file or change dir?
            if((atr & GFS_ATR_DIR) == 0) {
                if(fti >= 0) {
                    // what to do with this file type
                    if(((in->ftypes + fti)->ops & FIO_OP_LOAD) != 0) {
                        // file valid?
                        if(fsize <= in->destSize) {
                            out->otype = FIO_LOADED;
                            slPrint("Loading File...", slLocate(2, 23));
                            slSynch();
                                // need this really?
                            load = GFS_Load(cursor, 0, in->dest, in->destSize);
					
                            if((fid >= 0) && (load > 0)) {
                                slPrint("OK!", slLocate(14, 23));
                                out->value.size = load;
                                return RETURN_OK;
                            } else slPrint("FAILED!", slLocate(14, 23));
                        }    
                    } else if(((in->ftypes + fti)->ops & FIO_OP_GET_FID) != 0) {
                        out->otype = FIO_GOT_FID;
                            // need this really?
                        fid = cursor;
                        out->value.fid = fid;
                        if(fid >= 0) {
                            slPrint("FID OK!", slLocate(14, 23));
                            return RETURN_OK;
                        } else slPrint("FID FAILED!", slLocate(14, 23));
                    } else if(((in->ftypes + fti)->ops & FIO_OP_OPEN_GFS) != 0) {
                        out->otype = FIO_OPENED_GFS;
                            // need this really?
                        fid = cursor;
                            slPrint("GFS OPEN OK!", slLocate(14, 23));
                            return RETURN_OK;
                    }
                }    
            } else {
				
                slPrint("Changing dir...     ", slLocate(2, 23));
//				slPrint(GFS_IdToName(cursor), slLocate(20, 23));
				fid = cursor;
                gfsDirN = load = GFS_LoadDir(fid, &gfsDirTbl);
                set = GFS_SetDir(&gfsDirTbl);
                if((fid >= 0) && (load >= 0) && (set >= 0))
                    slPrint("OK!     ", slLocate(14, 23));
                else slPrint("FAILED!", slLocate(14, 23));
                fid = cursor = 0;
            }
        }
        // redraw 
		
	
		
        slPrint("File num:                       ", slLocate(2, 11));   
        slPrintHex(fn, slLocate(14, 11));   
        slPrint("File id:                       ", slLocate(2, 13));   
		slPrintHex(tfid, slLocate(14, 13)); 
            // file size: valid?
        slPrint("File size:                      ", slLocate(2, 15));   
            // printf filesize only, if
            // no match, or
            // matched file doesn't have to be loaded
            // file to load fits into buffer
        if((fti < 0) || (((in->ftypes + fti)->ops & FIO_OP_LOAD) == 0) || (fsize <= in->destSize)) {
            slPrintHex(fsize, slLocate(14, 15));   
        } else {
            slPrint("TOO BIG!   ", slLocate(14, 15));   
        }
            // file type: directory or file
        slPrint("File type:                              ", slLocate(2, 17));   
        if((atr & GFS_ATR_DIR) == 0) {
            slPrint("file", slLocate(14, 17));   
        } else {
            slPrint("directory", slLocate(14, 17));   
        }
            // file name:
        slPrint("File name:                      ", slLocate(2, 19));   
	
        slPrint(GFS_IdToName(cursor), slLocate(14, 19));

            // description (only on filenames)
        slPrint("File descr:                             ", slLocate(2, 21));
        if((atr & GFS_ATR_DIR) == 0) {
            if(fti >= 0) {
                slPrint((in->ftypes + fti)->description, slLocate(14, 21));
            } else slPrint("INVALID!", slLocate(14, 21));
        }    
    
        slSynch();
    }
}

