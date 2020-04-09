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

#define SOUND_BUFFER_SIZE   (0x40000) // + 0x4d000)

//static char sound[SOUND_BUFFER_SIZE];
static char *sound=NULL;
static unsigned int sound_size;
static PCM sound_dat = {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0};

/********************************************************************************/
/* sound slot/buffer structures *************************************************/
/********************************************************************************/
typedef struct SOUND_SLOT_INFO {
    struct {
        unsigned int size;
        void *address;      // differs for converted wave files
        PCM dat;
    } sound;
    struct {
        void *address;
        unsigned int size;
        // in case of file playback
        Sint32 fid;
    } buffer;
    unsigned char playing;
    unsigned char repeat;
    // for sound length counting
    // because length varies with playing parameters
    Uint32 frame;
} SndSlotInfo;
enum {
    PLAYVGM_SLOT_HIGHMEM,
    PLAYVGM_SLOT_LOWMEM,
    PLAYVGM_SLOT_FILE,
    PLAYVGM_SLOT_NUM
};
static SndSlotInfo sndSlots[] = {
    // high work RAM
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)NULL, SOUND_BUFFER_SIZE, -1}, 0 },
    // low work RAM, 1MByte
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)0x200000, 0x100000, -1}, 0 },
    // CD-ROM file slot
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)0, 0, -1}, 0 }
};


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

/********************************************************************************/
/* other variables **************************************************************/
/********************************************************************************/
static FIO_out out;

static void *my_scroll;
static unsigned char my_vblanks;

// draw modes for TXTSCR_printf()
typedef enum TXT_DRAW {
    TXT_DRAW_NOW,					// draw string directly to screen
    TXT_DRAW_LATER,					// draw only to txtscr
    TXT_DRAW_BOTH
} TxtDrawMode;

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
/*
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
}*/

void PLAYVGM_MENU_callback(int item, void *ptr)
{
	unsigned char slot;
	
    if(ptr == NULL)
        return;
    
    slot = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SLOT);
    switch(item) { 
        case PLAYVGM_MENU_LOADCD:
            {
                // configure MINCD_load
                // decreasing priority
                FIO_ftype ftypes[] = {
                    // files that fit into high work RAM
                    {"*.vgm", {0, sndSlots[PLAYVGM_SLOT_HIGHMEM].buffer.size}, "Video Game Music file", FIO_OP_LOAD},
                    {"*.mdx", {0, sndSlots[PLAYVGM_SLOT_HIGHMEM].buffer.size}, "Sharp X68000 Music file", FIO_OP_LOAD},
                    {"*.mod", {0, sndSlots[PLAYVGM_SLOT_HIGHMEM].buffer.size}, "MOD Music file", FIO_OP_LOAD},
                };
                // now here we use the selected sound slot size
                FIO_in in = {ftypes, sizeof(ftypes)/sizeof(FIO_ftype), sndSlots[slot].buffer.address, sndSlots[slot].buffer.size};
                ReturnValue ret;
                
                // don't access CD while file playback in process
                // or the sound will loop strange
                if(sndSlots[PLAYVGM_SLOT_FILE].playing == 1)
                    return;
                // start CD browser
                TXTSCR_clear(my_scroll, TXT_DRAW_BOTH);
                
				MINCD_init();
				ret = MINCD_load("Select a sound:", &in, &out);
                if(ret == RETURN_OK) {
                    // file data is in memory
#if 1
                    if(out.otype == FIO_LOADED) {
#else   // no wave support untill one buffer conversion is finished
                    if((out.otype == FIO_LOADED) && (out.fti != 0) && (out.fti != 4)) {
#endif
                        // save the size
                        sndSlots[slot].sound.size = out.value.size;
                            // take care of wave header
                        sndSlots[slot].sound.address = sndSlots[slot].buffer.address;
                        // convert WAV on the fly                    
                        /*if((out.fti == 0) || (out.fti == 4)) {
                            // it's a wave file
                            // convert it without using a second buffer
                            if(VGM_toPCM(sndSlots[slot].buffer.address, sndSlots[slot].buffer.address, &(sndSlots[slot].sound.size), &(sndSlots[slot].sound.dat.pitch), &stereo, &bits, 1) == RETURN_OK) {
                                sndSlots[slot].sound.address += 44;
                                // update menu
                                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_SAMPLERATE, VGM_computeSampleRate(sound_dat.pitch));
                                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_OCT, VGM_toSXT4(VGM_toOCT(sound_dat.pitch)));
                                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_FNS, VGM_toFNS(sound_dat.pitch));
                                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_CHANNELS, stereo);
                                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_RESOLUTION, (bits == 8 ? 0 : 1));
                                // update sound_dat
                                sndSlots[slot].sound.dat.mode = (stereo == 0 ? _Mono : _Stereo);
                                sndSlots[slot].sound.dat.mode |= (bits == 8 ? _PCM8Bit : _PCM16Bit);
                                // already adjusted: sound_dat.pitch
                            }
                        }*/       
                        // immediately start playing
                        PLAYVGM_MENU_callback(PLAYVGM_MENU_PLAY, ptr);
                    } else if(out.otype == FIO_GOT_FID) {
                        // only take over file id
                        // if the file slot is selected
                      /*  if(slot == PLAYVGM_SLOT_FILE) {
                            sndSlots[slot].buffer.fid = out.value.fid;
                            // immediately start playing
                            PLAYVGM_MENU_callback(PLAYVGM_MENU_PLAY, ptr);
                        }    */
                    }
                }        
                // continue menu
               TXTSCR_clear(my_scroll, TXT_DRAW_BOTH);
               TXTMEN_redrawMenu(ptr);
                // wait release
                while(((Smpc_Peripheral[0].data & PER_DGT_ST) == 0) || ((Smpc_Peripheral[0].data & PER_DGT_TA) == 0) || ((Smpc_Peripheral[0].data & PER_DGT_TC) == 0))
                    slSynch();
                return;
            }    

        case PLAYVGM_MENU_STOPCD:
            {
                CdcPos poswk;
                // seek home position
                CDC_POS_PTYPE(&poswk) = CDC_PTYPE_DFL;
                CDC_CdSeek(&poswk);
            }
            return;

        case PLAYVGM_MENU_PLAY:
            /*switch(slot) {
                case PLAYVGM_SLOT_HIGHMEM:
                case PLAYVGM_SLOT_LOWMEM:
                    // can play the same sound
                    // multiple times simultaneously
                    if(sndSlots[slot].sound.size > 0) {
                        slPCMOn(&(sndSlots[slot].sound.dat), sndSlots[slot].sound.address, sndSlots[slot].sound.size);
                        sndSlots[slot].playing = 1;
                        sndSlots[slot].frame = 0;
                    }    
                    break;

                case PLAYVGM_SLOT_FILE:
                    if(sndSlots[slot].playing == 0) {
                        PLAYVGM_MENU_playFile(slot, ptr);
                        sndSlots[slot].playing = 1;
                        sndSlots[slot].frame = 0;
                    }    
                    break;
            }*/
            return;
            
        case PLAYVGM_MENU_STOP:
/*            // stop deletes repeat
            if((sndSlots[slot].playing == 1) && (sndSlots[slot].repeat == 1)) {
                sndSlots[slot].repeat = 0;
                TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_REPEAT, sndSlots[slot].repeat);
                TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_REPEAT);
            }
            // stop sound    
            switch(slot) {
                case PLAYVGM_SLOT_HIGHMEM:
                case PLAYVGM_SLOT_LOWMEM:
                    slPCMOff(&(sndSlots[slot].sound.dat));
                    sndSlots[slot].playing = 0;
                    break;

                case PLAYVGM_SLOT_FILE:
                    // don't know exactly if the check is neccessary,
                    // but the pcmhn is only valid after being created for playing
                    // so I guess it is
                    if(sndSlots[slot].playing == 1) {
                        VGM_Stop(pcmhn);
            			VGM_DestroyGfsHandle(pcmhn);
            			GFS_Close(gfshn);
                        sndSlots[slot].playing = 0;
                    }    
                    break;
            }
*/            
            return;
            
        case PLAYVGM_MENU_REPEAT:
            sndSlots[slot].repeat = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_REPEAT);
            return;

        case PLAYVGM_MENU_SLOT:
/*            // update sound attributes in menu
                // repeat
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_REPEAT, sndSlots[slot].repeat);
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_REPEAT);
                // channels
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_CHANNELS, ((sndSlots[slot].sound.dat.mode & _Stereo) != 0 ? 1 : 0));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_CHANNELS);
                // bit resolution
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_RESOLUTION, ((sndSlots[slot].sound.dat.mode & _PCM8Bit) != 0 ? 0 : 1));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_RESOLUTION);
                // samplerate
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_SAMPLERATE, VGM_computeSampleRate(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_SAMPLERATE);
                // OCT
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_OCT, VGM_toSXT4(VGM_toOCT(sndSlots[slot].sound.dat.pitch)));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_OCT);
                // FNS
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_FNS, VGM_toFNS(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_FNS);
                // level
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_LEVEL, sndSlots[slot].sound.dat.level);
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_LEVEL);
                // pan
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_PAN, sndSlots[slot].sound.dat.pan);
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_PAN);
                // length
            // in callafter
//            TXTMEN_setFloatValueAt(ptr, PLAYVGM_MENU_LENGTH, ((float)(sndSlots[slot].frame * my_vblanks) / 60.0));
//            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_LENGTH);
*/                
            return;
/*
        case PLAYVGM_MENU_CHANNELS:
        case PLAYVGM_MENU_RESOLUTION:
            sndSlots[slot].sound.dat.mode = (TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_CHANNELS) == 1 ? _Stereo : _Mono);
            sndSlots[slot].sound.dat.mode |= (TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_RESOLUTION) == 1 ? _PCM16Bit : _PCM8Bit);
            return;
            
        case PLAYVGM_MENU_SAMPLERATE:
            sndSlots[slot].sound.dat.pitch = VGM_computePitch(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_SAMPLERATE));
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_OCT, VGM_toSXT4(VGM_toOCT(sndSlots[slot].sound.dat.pitch)));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_OCT);
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_FNS, VGM_toFNS(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_FNS);
            return;
            
        case PLAYVGM_MENU_OCT:
        case PLAYVGM_MENU_FNS:
            sndSlots[slot].sound.dat.pitch = VGM_toPitch(TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_OCT), TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_FNS));
            TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_SAMPLERATE, VGM_computeSampleRate(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_SAMPLERATE);
            return;
            
        case PLAYVGM_MENU_LEVEL:
            if((slot == PLAYVGM_SLOT_HIGHMEM) || (slot == PLAYVGM_SLOT_LOWMEM)) {
                sndSlots[slot].sound.dat.level = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_LEVEL);
                // this somehow (stops or )mutes the sound
                // untill next repeat
                if(sndSlots[slot].playing == 1)
                    slPCMParmChange(&(sndSlots[slot].sound.dat));
            } else if(slot == PLAYVGM_SLOT_FILE) {
                // level value range differs
                VGM_SetVolume(pcmhn, TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_LEVEL) >> 4);
                if(sndSlots[slot].playing == 1)
                    VGM_ChangePcmPara(pcmhn);
            }
            return;

        case PLAYVGM_MENU_PAN:
            if((slot == PLAYVGM_SLOT_HIGHMEM) || (slot == PLAYVGM_SLOT_LOWMEM)) {
                sndSlots[slot].sound.dat.pan = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_PAN);
                // this somehow (stops or )mutes the sound
                // untill next repeat
                if(sndSlots[slot].playing == 1)
                    slPCMParmChange(&(sndSlots[slot].sound.dat));
            } else if(slot == PLAYVGM_SLOT_FILE) {
                // pan only valid for mono
                // ignored for stereo
                // pan value range differs
                VGM_SetPan(pcmhn, (128 + TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_PAN)) >> 3);
                if(sndSlots[slot].playing == 1)
                    VGM_ChangePcmPara(pcmhn);
            }
            return;

        case PLAYVGM_MENU_LENGTH:
            return;
*/        

        case PLAYVGM_MENU_EXIT:
            TXTMEN_leave(ptr);
            return;

        case PLAYVGM_MENU_AFTER:
        {
            int i;
            int status;
            
            // first, check if the CD has been changed
            // if that would be done only in MINCD and CDDA player,
            // no datatrack would be recognized when changing to CDDA player,
            // because it clears the IRQ
// ??? does it even work
// doesn't seem to
// like in MINCD, have to manually rescan CD
            if((CDC_GetHirqReq() & CDC_HIRQ_DCHG) != 0) {
// have to check if this works,
// it freezes the Saturn for 20 seconds
//                if((Smpc_Peripheral[0].data & PER_DGT_TX) == 0) {
//                    CDC_CdOpen();
//                    CDC_CdOpen();
//                }    
                // after every CD change, the data track cannot be accessed
                // but CDDA instead can
 //               CdUnlock();
                MINCD_init();
            }
/*
            // need to call VGM_Task often...
                // file slot
            if(sndSlots[PLAYVGM_SLOT_FILE].playing == 1) {
                VGM_Task(pcmhn);
            }

            // pcm playing status..
            for(i = 0; i < PLAYVGM_SLOT_NUM; i++) {
                // count playlength
                if(sndSlots[i].playing == 1) {
                    // we may want to play a memory and a file slot
                    // simultaneously
                    
                    // memory slots
                    if((i == PLAYVGM_SLOT_HIGHMEM) || (i == PLAYVGM_SLOT_LOWMEM)) {
                        // pcm finished playing?
                        if(slPCMStat(&(sndSlots[i].sound.dat)) != 1) {
                            // restart if repeat is selected
                            if(sndSlots[i].repeat == 1) {
                                slPCMOn(&(sndSlots[i].sound.dat), sndSlots[i].sound.address, sndSlots[i].sound.size);
                                sndSlots[i].frame = 0;
                            } else {
                                sndSlots[i].playing = 0;
                                status = VGM_STAT_PLAY_END;
                            }    
                        } else {
                            sndSlots[i].frame++;
                            status = VGM_STAT_PLAY_TIME;
                        }    
                    }    
                    // file slot
                    if(i == PLAYVGM_SLOT_FILE) {
                        VGM_Task(pcmhn);
                        status = VGM_GetPlayStatus(pcmhn);
                        // the file slot doesn't start playing immediately
                        // because of CD access
                        if(status == VGM_STAT_PLAY_TIME)
                            sndSlots[i].frame++;
                		if(status == VGM_STAT_PLAY_END) {
                		    // must destroy everything
                			VGM_DestroyGfsHandle(pcmhn);
                			GFS_Close(gfshn);
                			// check repeat
                            if(sndSlots[i].repeat == 1) {
                                PLAYVGM_MENU_playFile(slot, ptr);
                                sndSlots[i].frame = 0;
                                status = VGM_GetPlayStatus(pcmhn);
                            } else sndSlots[i].playing = 0;    
                		}
 //                       if(status == VGM_STAT_PLAY_ERR_STOP)
 //                           status = VGM_STAT_PLAY_END+2;
                    }    
                } else status = VGM_STAT_PLAY_END+1;
                
                // redraw for activated slot
                if(i == slot) {
                    // frame
                    TXTMEN_setFloatValueAt(ptr, PLAYVGM_MENU_LENGTH, ((float)(sndSlots[i].frame * my_vblanks) / 60.0));
                    TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_LENGTH);
                    // status
                    TXTMEN_setIntValueAt(ptr, PLAYVGM_MENU_STATUS, status);
                    TXTMEN_redrawValueAt(ptr, PLAYVGM_MENU_STATUS);
                }    
            }     
*/			
        }    
            return;
    }
}



void PLAYVGM_MENU_init(void **menu, void *scroll, unsigned char *exitText)
{
    TXTMEN_init(menu, scroll, "DamageX's VGM Player");

    TXTMEN_addButton(*menu, "load from CD");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_LOADCD, *menu, PLAYVGM_MENU_callback);
//    TXTMEN_addButton(*menu, "rescan CD");
//    TXTMEN_setCallback(*menu, PLAYVGM_MENU_RESCAN, *menu, PLAYVGM_MENU_callback);
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

