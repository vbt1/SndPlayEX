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


#include <SGL.H>
#include <sega_cdc.h>
//#include <RB_TXT_SCROLL.h>
#include <RB_TXT_MENU.h>
#include "RB_playVGM.h"
#include <RB_minCD.h>
#include <RB_PCM.h>
#include <RB_were_here.h>


extern PDATA PD_Cube;

static CdcPly plywk;
static CdcPos poswk;
static CdcStat stat;
static Uint32 toc[102];

static ANGLE ang[XYZ];
static ANGLE t_ang[XYZ];
static FIXED pos[XYZ];
static FIXED sca[XYZ];
static FIXED light[XYZ];

//static void *my_scroll;



// uses last CdcStat stat
int PLAYVGM_checkContinue()
{
    if(((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_PAUSE) || ((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_SCAN)) {
        // restart from current position
         /* set data */
         CDC_PLY_STYPE(&plywk) = CDC_PTYPE_NOCHG;
         CDC_PLY_ETYPE(&plywk) = CDC_PTYPE_NOCHG;
        // this needs to be investigated:
         CDC_PLY_PMODE(&plywk) = CDC_PM_NOCHG;
         /* start playing */
         CDC_CdPlay(&plywk);
         return 1;
    }
    return 0;
}

void PLAYVGM_updateTracks(void *ptr)
{
    // check bounds
    if(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_TRACK) < ((toc[99] & 0x00FF0000) >> 16)) {
        // set first track
        TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_TRACK, (toc[99] & 0x00FF0000) >> 16);
        TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_TRACK);
    } else if(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_TRACK) > ((toc[100] & 0x00FF0000) >> 16)) {
        // set last track
        TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_TRACK, (toc[100] & 0x00FF0000) >> 16);
        TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_TRACK);
    }
}

void PLAYVGM_MENU_callback(int item, void *ptr)
{
    if(ptr == NULL)
        return;
    
    switch(item) {
        case PLAYVGM_MENU_TRACK:
            PLAYVGM_updateTracks(ptr);
            PLAYVGM_MENU_callback(PLAYVGM_MENU_PLAY, ptr);  
//            // set laser position
//            CDC_POS_PTYPE(&poswk) = CDC_PTYPE_TNO;
//            CDC_POS_TNO(&poswk) = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_TRACK);
//            CDC_POS_IDX(&poswk) = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SIDX);
//            CDC_CdSeek(&poswk);
//
            return;

        case PLAYVGM_MENU_RESCAN:
            // not neccessary, when
            // Saturn recognizes the CD Open status
#if 0
//            were_here("CD rescan #1");
//            CdUnlock();
#else
//            were_here("CD rescan #2");
            // should at least enable a CD change
            // without opening the tray manually
            // but it may not unlock the CD for data access
            // because when you insert a new CD with manually opened tray
            // it's locked and data cannot be accessed
            // !!! sets drive to BUSY status, untill opened manually
            // -> maybe one is enough
            CDC_CdOpen();   // open
//if((Smpc_Peripheral[0].data & PER_DGT_TZ) == 0) {
//            CDC_CdOpen();   // close
//            CDC_TgetToc(toc);
//}
#endif            
            // the rest is done in callafter,
            // signaled by HIRQ_DCHG
            return;

        case PLAYVGM_MENU_VOLUME:
            slSndVolume(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_VOLUME));
            return;
            
        case PLAYVGM_MENU_PLAY:
            CDC_GetCurStat(&stat);
            if(((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_PLAY) || ((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_STANDBY) || ((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_SEEK)) {
                // start playing track
                 /* set data */
                 CDC_PLY_STYPE(&plywk) = CDC_PTYPE_TNO;
                 CDC_PLY_STNO(&plywk)  = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_TRACK);
                 CDC_PLY_SIDX(&plywk) = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SIDX);
                 CDC_PLY_ETYPE(&plywk) = CDC_PTYPE_TNO;
                    // this should play all tracks up to the last one
                    // if you just want one track, input same track as for CDC_PLY_STNO(&plywk):
                    // TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_TRACK);                 
                 CDC_PLY_ETNO(&plywk)  = (toc[100] & 0x00FF0000) >> 16;
                 CDC_PLY_EIDX(&plywk) = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_EIDX);
                // this needs to be investigated:
                 CDC_PLY_PMODE(&plywk) = CDC_PM_DFL + TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_REPEATS);
                 /* start playing */
                 CDC_CdPlay(&plywk);
            } else PLAYVGM_checkContinue(); 

            return;
            
        case PLAYVGM_MENU_PAUSE:
//          4. Pause CD Play (No changed selection: seek current position) ......Pause               
            CDC_GetCurStat(&stat);
            if(((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_PLAY) || ((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_SCAN)) {
                CDC_POS_PTYPE(&poswk) = CDC_PTYPE_NOCHG;
                CDC_CdSeek(&poswk);
            } else PLAYVGM_checkContinue();
            return;

        case PLAYVGM_MENU_STOP:
            // seek home position
            CDC_POS_PTYPE(&poswk) = CDC_PTYPE_DFL;
            CDC_CdSeek(&poswk);
                // although it's not neccessary
//            slCDDAOff();
            return;
            
        case PLAYVGM_MENU_FWD:
            // start CD scan / fast forward
            // or continue playing
            CDC_GetCurStat(&stat);
            if((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_PLAY) {
                CDC_CdScan(CDC_SCAN_FWD);
            } else PLAYVGM_checkContinue(); 
            return;

        case PLAYVGM_MENU_RVS:
            // start CD scan / fast rewind/reverse
            // or continue playing
            CDC_GetCurStat(&stat);
            if((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_PLAY) {
                CDC_CdScan(CDC_SCAN_RVS);
            } else PLAYVGM_checkContinue(); 
            return;

        case PLAYVGM_MENU_REPEATS:
            return;
        
        case PLAYVGM_MENU_SIDX:
            if(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SIDX) > TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_EIDX)) {
                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_SIDX, TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_EIDX));
                TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_SIDX);
            }    
            return;

        case PLAYVGM_MENU_EIDX:
            if(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_EIDX) < TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SIDX)) {
                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_EIDX, TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SIDX));
                TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_EIDX);
            }    
            return;

        case PLAYVGM_MENU_LLEVEL:
        case PLAYVGM_MENU_RLEVEL:
        case PLAYVGM_MENU_LPAN:
        case PLAYVGM_MENU_RPAN:
            slCDDAOn(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_LLEVEL), TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_RLEVEL), TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_LPAN), TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_RPAN));
            return;

        case PLAYVGM_MENU_EXIT:
            TXTMEN_leave(ptr);
            return;

        case PLAYVGM_MENU_AFTER:
            if((Smpc_Peripheral[0].data & PER_DGT_TX) == 0)
                CDC_ClrHirqReq(~CDC_HIRQ_DCHG);


            // check if to rescan cd
            // don't care if a CD is inserted
            if((CDC_GetHirqReq() & CDC_HIRQ_DCHG) != 0) {
//                slPrint("Got IRQ DCHG!", slLocate(5, 25));

                // experiments showed that
                // seeking home position makes the TOC read correctly
                // (play and pause could have the same effect,
                // untested because of BUSY state)
                PLAYVGM_MENU_callback(PLAYVGM_MENU_STOP, ptr);
                // need to wait to get a valid TOC
                do {
                    CDC_GetCurStat(&stat);
                } while((CDC_STAT_STATUS(&stat) & 0xF) == CDC_ST_BUSY);
                // this one waits in the open and nodisc status
                CDC_TgetToc(toc);
                CDC_ClrHirqReq(~CDC_HIRQ_DCHG);
                
                // maybe there is further stuff we want to
                // perform in this app

                // updates menu track number
                PLAYVGM_updateTracks(ptr);
                // !!! and starts playing immediately
//                PLAYVGM_MENU_callback(PLAYVGM_MENU_TRACK, ptr);
            } //else slPrint("No IRQ DCHG!", slLocate(5, 25));

            // update CD status
            int cdstatus = 0;
            CDC_GetCurStat(&stat);
            cdstatus = CDC_STAT_STATUS(&stat) & 0xF;
            if(!((cdstatus >= 0) && (cdstatus <= CDC_ST_FATAL)))
                cdstatus = CDC_ST_FATAL + 1;
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_CDSTATUS, cdstatus);
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_CDSTATUS);

            return;
    }
}



void PLAYVGM_MENU_init(void **menu, void *scroll, unsigned char *exitText)
{
    TXTMEN_init(menu, scroll, "DamageX's VGM Player");

    TXTMEN_addButton(*menu, "load from CD");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_LOADCD, *menu, PLAYVGM_MENU_callback);
    TXTMEN_addButton(*menu, "rescan CD");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_RESCAN, *menu, PLAYVGM_MENU_callback);
    TXTMEN_addButton(*menu, "play");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_PLAY, *menu, PLAYVGM_MENU_callback);
    TXTMEN_addButton(*menu, "stop");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_STOP, *menu, PLAYVGM_MENU_callback);
    TXTMEN_addCheckbox(*menu, "repeat", 0);
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_REPEAT, *menu, PLAYVGM_MENU_callback);

    TXTMEN_disable(*menu);
    // exit button ?
    if(exitText != NULL) {
        TXTMEN_addButton(*menu, exitText);
        TXTMEN_setCallback(*menu, PLAYVGM_MENU_EXIT, *menu, PLAYVGM_MENU_callback);
    }    
/*
load the driver
*/
}

