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
#include <sega_pcm.h>
#include <RB_TXT_SCROLL.h>
#include <RB_TXT_MENU.h>
#include "RB_playPCM.h"
#include <RB_minCD.h>
#include <RB_PCM.h>
#include <RB_were_here.h>
//#include <stdlib.h>

// why do I have to reduce the size so much???
// there seems much space left in SoundPlayer.map
#define SOUND_BUFFER_SIZE   (0x44000) // + 0x4d000)

//static char sound[SOUND_BUFFER_SIZE];
char *sound=NULL;
//static unsigned int sound_size;
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
    PLAYPCM_SLOT_HIGHMEM,
    PLAYPCM_SLOT_LOWMEM,
    PLAYPCM_SLOT_FILE,
    PLAYPCM_SLOT_NUM
};
static SndSlotInfo sndSlots[] = {
    // high work RAM
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)NULL, SOUND_BUFFER_SIZE, -1}, 0 },
    // low work RAM, 1MByte
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)0x200000, 0x100000, -1}, 0 },
    // CD-ROM file slot
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)0, 0, -1}, 0 }
};
/********************************************************************************/
/* SBL PCM stuff ****************************************************************/
/********************************************************************************/
#define	PCM_ADDR        ((void*)0x25a20000)
    // that's not totaly clear, my interpretation of ST-135-R4....pdf:
    // as 4096*2 is the minimum, I suppose S[byte] = 4096*2
    // and the range 4096*2 - 4096*4    sample/1channel
    // seems like 4096*2 for each 8 bit channel
    // and 4096*4 for each 16 bit channel
    // 8 bit mono       S[byte]
    // 8 bit stereo     2 X S[byte]
    // 16 bit mono      2 X S[byte]
    // 16 bit stereo    4 X S[byte]
    // for safe, I take 8
#define	PCM_SIZE        (4096*8)			/* 2.. */
    // minimum: sector size *10, with sector size 2048 or 2324 for CD-XA
    // let's take more
#define	RING_BUF_SIZE   (2048*20)			/* 10.. */

static PcmWork pcmwork;
static Uint32 pcmbuf[RING_BUF_SIZE / sizeof(Uint32)];

static GfsHn gfshn;
static PcmHn pcmhn;
static PcmCreatePara pcmpara;

/********************************************************************************/
/* other variables **************************************************************/
/********************************************************************************/
static FIO_out out;

static void *my_scroll;
static unsigned char my_vblanks;
static unsigned int drvLoaded=0;

/********************************************************************************/
/* main PCM player application **************************************************/
/********************************************************************************/
void PLAYPCM_MENU_callback(int item, void *ptr);

// Don't call while playing,
// otherwise the Saturn might freeze
void PLAYPCM_MENU_playFile(unsigned char slot, void *ptr)
{
    // open file
    gfshn = GFS_Open(sndSlots[slot].buffer.fid);
    if(gfshn == NULL)
        return;
    // create pcm handle
    PCM_PARA_WORK(&pcmpara) = (struct PcmWork *)&pcmwork;
    PCM_PARA_RING_ADDR(&pcmpara) = (Sint8 *)pcmbuf;
    PCM_PARA_RING_SIZE(&pcmpara) = RING_BUF_SIZE;
    PCM_PARA_PCM_ADDR(&pcmpara) = PCM_ADDR;
    PCM_PARA_PCM_SIZE(&pcmpara) = PCM_SIZE;
    pcmhn = PCM_CreateGfsHandle(&pcmpara, gfshn);
    if(pcmhn == NULL)
        return;
    // start playing
        // doesn't start immediately, of course
        // frame counter handles this
    PCM_Start(pcmhn);
    // trying to retrieve pcm information
    // and convert it to SGL PCM format
        // just using it as common intermediate format
        // for display
    sndSlots[slot].sound.dat.mode = (pcmwork.status.info.channel == 2 ? _Stereo : _Mono);
    sndSlots[slot].sound.dat.mode |= (pcmwork.status.info.sampling_bit == 16 ? _PCM16Bit : _PCM8Bit);
    sndSlots[slot].sound.dat.pitch = PCM_computePitch(pcmwork.status.info.sampling_rate);
        // update display
    PLAYPCM_MENU_callback(PLAYPCM_MENU_SLOT, ptr);
}


void PLAYPCM_MENU_callback(int item, void *ptr)
{
    unsigned char stereo, bits;
    unsigned char slot;
        
    if(ptr == NULL)
        return;

    slot = TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_SLOT);
    switch(item) { 
        case PLAYPCM_MENU_LOADCD:
            {
                // configure MINCD_load
                // decreasing priority
                FIO_ftype ftypes[] = {
                    // files that fit into high work RAM
                    {"*.wav", {0, sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size}, "(small)Windows wave file", FIO_OP_LOAD},
                    {"*.pcm", {0, sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size}, "(small)Saturn raw pcm", FIO_OP_LOAD},
                    {"*.snd", {0, sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size}, "(small)Saturn pcm collection", FIO_OP_LOAD},
                    {"*.ton", {0, sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size}, "(small)Saturn pcm collection", FIO_OP_LOAD},
                    // files that fit into low work RAM
                    {"*.wav", {sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size+1, sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size}, "(med)Windows wave file", FIO_OP_LOAD},
                    {"*.pcm", {sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size+1, sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size}, "(med)Saturn raw pcm", FIO_OP_LOAD},
                    {"*.snd", {sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size+1, sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size}, "(med)Saturn pcm collection", FIO_OP_LOAD},
                    {"*.ton", {sndSlots[PLAYPCM_SLOT_HIGHMEM].buffer.size+1, sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size}, "(med)Saturn pcm collection", FIO_OP_LOAD},
                    // files that don't fit into memory
                        // wave files only supported for memory playback
//                    {"*.wav", {sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size+1, 0x7FFFFFFF}, "(large)Windows wave file", FIO_OP_GET_FID},
                    {"*.aif", {0, 0x7FFFFFFF}, "Saturn AIFF file", FIO_OP_GET_FID},
                        // ADPCM files only supported for file playback
                    {"*.adp", {0, 0x7FFFFFFF}, "Saturn ADPCM file", FIO_OP_GET_FID},
//                    {"*.snd", {sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size+1, 0x7FFFFFFF}, "(large)Saturn pcm collection", FIO_OP_GET_FID},
//                    {"*.ton", {sndSlots[PLAYPCM_SLOT_LOWMEM].buffer.size+1, 0x7FFFFFFF}, "(large)Saturn pcm collection", FIO_OP_GET_FID},
                };
                // now here we use the selected sound slot size
                FIO_in in = {ftypes, sizeof(ftypes)/sizeof(FIO_ftype), sndSlots[slot].buffer.address, sndSlots[slot].buffer.size};
                ReturnValue ret;
                
                // don't access CD while file playback in process
                // or the sound will loop strange
                if(sndSlots[PLAYPCM_SLOT_FILE].playing == 1)
                    return;
                // start CD browser
                TXTSCR_clear(my_scroll, TXT_DRAW_BOTH);

				if(drvLoaded==0)
				{
//					MINCD_init();
					init_sound();
					sndInit();
					PCM_Init();
					PCM_DeclareUseAdpcm();
					drvLoaded = 1;					
				}
				
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
                        if((out.fti == 0) || (out.fti == 4)) {
                            // it's a wave file
                            // convert it without using a second buffer
                            if(PCM_toPCM(sndSlots[slot].buffer.address, sndSlots[slot].buffer.address, &(sndSlots[slot].sound.size), &(sndSlots[slot].sound.dat.pitch), &stereo, &bits, 1) == RETURN_OK) {
                                sndSlots[slot].sound.address += 44;
                                // update menu
                                TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_SAMPLERATE, PCM_computeSampleRate(sound_dat.pitch));
                                TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_OCT, PCM_toSXT4(PCM_toOCT(sound_dat.pitch)));
                                TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_FNS, PCM_toFNS(sound_dat.pitch));
                                TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_CHANNELS, stereo);
                                TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_RESOLUTION, (bits == 8 ? 0 : 1));
                                // update sound_dat
                                sndSlots[slot].sound.dat.mode = (stereo == 0 ? _Mono : _Stereo);
                                sndSlots[slot].sound.dat.mode |= (bits == 8 ? _PCM8Bit : _PCM16Bit);
                                // already adjusted: sound_dat.pitch
                            }
                        }       
                        // immediately start playing
                        PLAYPCM_MENU_callback(PLAYPCM_MENU_PLAY, ptr);
                    } else if(out.otype == FIO_GOT_FID) {
                        // only take over file id
                        // if the file slot is selected
                        if(slot == PLAYPCM_SLOT_FILE) {
                            sndSlots[slot].buffer.fid = out.value.fid;
                            // immediately start playing
                            PLAYPCM_MENU_callback(PLAYPCM_MENU_PLAY, ptr);
                        }    
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
/*
        case PLAYPCM_MENU_RESCAN:
            // needed:
            // if previously a CDDA only CD was inserted,
            // it wouldn't unlock the data track, because
            // the audio CD is assumed to be still inserted
// have to check if this works,
// it freezes the Saturn for 20 seconds
//            if((Smpc_Peripheral[0].data & PER_DGT_TX) == 0) {
//                CDC_CdOpen();
//                CDC_CdOpen();
//            }    
            // for swap trick performers
            // to signal a CD change to Saturn manually
            // after every CD change, the data track cannot be accessed
            // but CDDA instead can
//            CdUnlock();
            // erases all sector data in CD buffer
            MINCD_init();
            return;
*/
        case PLAYPCM_MENU_STOPCD:
            {
                CdcPos poswk;
                // seek home position
                CDC_POS_PTYPE(&poswk) = CDC_PTYPE_DFL;
                CDC_CdSeek(&poswk);
            }
            return;

        case PLAYPCM_MENU_PLAY:
            switch(slot) {
                case PLAYPCM_SLOT_HIGHMEM:
                case PLAYPCM_SLOT_LOWMEM:
                    // can play the same sound
                    // multiple times simultaneously
                    if(sndSlots[slot].sound.size > 0) {
                        slPCMOn(&(sndSlots[slot].sound.dat), sndSlots[slot].sound.address, sndSlots[slot].sound.size);
                        sndSlots[slot].playing = 1;
                        sndSlots[slot].frame = 0;
                    }    
                    break;

                case PLAYPCM_SLOT_FILE:
                    if(sndSlots[slot].playing == 0) {
                        PLAYPCM_MENU_playFile(slot, ptr);
                        sndSlots[slot].playing = 1;
                        sndSlots[slot].frame = 0;
                    }    
                    break;
            }
            return;
            
        case PLAYPCM_MENU_STOP:
            // stop deletes repeat
            if((sndSlots[slot].playing == 1) && (sndSlots[slot].repeat == 1)) {
                sndSlots[slot].repeat = 0;
                TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_REPEAT, sndSlots[slot].repeat);
                TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_REPEAT);
            }
            // stop sound    
            switch(slot) {
                case PLAYPCM_SLOT_HIGHMEM:
                case PLAYPCM_SLOT_LOWMEM:
                    slPCMOff(&(sndSlots[slot].sound.dat));
                    sndSlots[slot].playing = 0;
                    break;

                case PLAYPCM_SLOT_FILE:
                    // don't know exactly if the check is neccessary,
                    // but the pcmhn is only valid after being created for playing
                    // so I guess it is
                    if(sndSlots[slot].playing == 1) {
                        PCM_Stop(pcmhn);
            			PCM_DestroyGfsHandle(pcmhn);
            			GFS_Close(gfshn);
                        sndSlots[slot].playing = 0;
                    }    
                    break;
            }
            
            return;
            
        case PLAYPCM_MENU_REPEAT:
            sndSlots[slot].repeat = TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_REPEAT);
            return;

        case PLAYPCM_MENU_SLOT:
            // update sound attributes in menu
                // repeat
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_REPEAT, sndSlots[slot].repeat);
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_REPEAT);
                // channels
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_CHANNELS, ((sndSlots[slot].sound.dat.mode & _Stereo) != 0 ? 1 : 0));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_CHANNELS);
                // bit resolution
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_RESOLUTION, ((sndSlots[slot].sound.dat.mode & _PCM8Bit) != 0 ? 0 : 1));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_RESOLUTION);
                // samplerate
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_SAMPLERATE, PCM_computeSampleRate(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_SAMPLERATE);
                // OCT
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_OCT, PCM_toSXT4(PCM_toOCT(sndSlots[slot].sound.dat.pitch)));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_OCT);
                // FNS
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_FNS, PCM_toFNS(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_FNS);
                // level
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_LEVEL, sndSlots[slot].sound.dat.level);
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_LEVEL);
                // pan
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_PAN, sndSlots[slot].sound.dat.pan);
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_PAN);
                // length
            // in callafter
//            TXTMEN_setFloatValueAt(ptr, PLAYPCM_MENU_LENGTH, ((float)(sndSlots[slot].frame * my_vblanks) / 60.0));
//            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_LENGTH);
                
            return;

        case PLAYPCM_MENU_CHANNELS:
        case PLAYPCM_MENU_RESOLUTION:
            sndSlots[slot].sound.dat.mode = (TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_CHANNELS) == 1 ? _Stereo : _Mono);
            sndSlots[slot].sound.dat.mode |= (TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_RESOLUTION) == 1 ? _PCM16Bit : _PCM8Bit);
            return;
            
        case PLAYPCM_MENU_SAMPLERATE:
            sndSlots[slot].sound.dat.pitch = PCM_computePitch(TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_SAMPLERATE));
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_OCT, PCM_toSXT4(PCM_toOCT(sndSlots[slot].sound.dat.pitch)));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_OCT);
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_FNS, PCM_toFNS(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_FNS);
            return;
            
        case PLAYPCM_MENU_OCT:
        case PLAYPCM_MENU_FNS:
            sndSlots[slot].sound.dat.pitch = PCM_toPitch(TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_OCT), TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_FNS));
            TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_SAMPLERATE, PCM_computeSampleRate(sndSlots[slot].sound.dat.pitch));
            TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_SAMPLERATE);
            return;
            
        case PLAYPCM_MENU_LEVEL:
            if((slot == PLAYPCM_SLOT_HIGHMEM) || (slot == PLAYPCM_SLOT_LOWMEM)) {
                sndSlots[slot].sound.dat.level = TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_LEVEL);
                // this somehow (stops or )mutes the sound
                // untill next repeat
                if(sndSlots[slot].playing == 1)
                    slPCMParmChange(&(sndSlots[slot].sound.dat));
            } else if(slot == PLAYPCM_SLOT_FILE) {
                // level value range differs
                PCM_SetVolume(pcmhn, TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_LEVEL) >> 4);
                if(sndSlots[slot].playing == 1)
                    PCM_ChangePcmPara(pcmhn);
            }
            return;

        case PLAYPCM_MENU_PAN:
            if((slot == PLAYPCM_SLOT_HIGHMEM) || (slot == PLAYPCM_SLOT_LOWMEM)) {
                sndSlots[slot].sound.dat.pan = TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_PAN);
                // this somehow (stops or )mutes the sound
                // untill next repeat
                if(sndSlots[slot].playing == 1)
                    slPCMParmChange(&(sndSlots[slot].sound.dat));
            } else if(slot == PLAYPCM_SLOT_FILE) {
                // pan only valid for mono
                // ignored for stereo
                // pan value range differs
                PCM_SetPan(pcmhn, (128 + TXTMEN_getIntValueAt(ptr, PLAYPCM_MENU_PAN)) >> 3);
                if(sndSlots[slot].playing == 1)
                    PCM_ChangePcmPara(pcmhn);
            }
            return;

        case PLAYPCM_MENU_LENGTH:
            return;
        

        case PLAYPCM_MENU_EXIT:
			drvLoaded=0;
            TXTMEN_leave(ptr);
            return;

        case PLAYPCM_MENU_AFTER:
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
//            if((CDC_GetHirqReq() & CDC_HIRQ_DCHG) != 0) 
			{
// have to check if this works,
// it freezes the Saturn for 20 seconds
//                if((Smpc_Peripheral[0].data & PER_DGT_TX) == 0) {
//                    CDC_CdOpen();
//                    CDC_CdOpen();
//                }    
                // after every CD change, the data track cannot be accessed
                // but CDDA instead can
 //               CdUnlock();
               MINCD_init();  // vbt on vire
            }

            // need to call PCM_Task often...
                // file slot
            if(sndSlots[PLAYPCM_SLOT_FILE].playing == 1) {
                PCM_Task(pcmhn);
            }

            // pcm playing status..
            for(i = 0; i < PLAYPCM_SLOT_NUM; i++) {
                // count playlength
                if(sndSlots[i].playing == 1) {
                    // we may want to play a memory and a file slot
                    // simultaneously
                    
                    // memory slots
                    if((i == PLAYPCM_SLOT_HIGHMEM) || (i == PLAYPCM_SLOT_LOWMEM)) {
                        // pcm finished playing?
                        if(slPCMStat(&(sndSlots[i].sound.dat)) != 1) {
                            // restart if repeat is selected
                            if(sndSlots[i].repeat == 1) {
                                slPCMOn(&(sndSlots[i].sound.dat), sndSlots[i].sound.address, sndSlots[i].sound.size);
                                sndSlots[i].frame = 0;
                            } else {
                                sndSlots[i].playing = 0;
                                status = PCM_STAT_PLAY_END;
                            }    
                        } else {
                            sndSlots[i].frame++;
                            status = PCM_STAT_PLAY_TIME;
                        }    
                    }    
                    // file slot
                    if(i == PLAYPCM_SLOT_FILE) {
                        PCM_Task(pcmhn);
                        status = PCM_GetPlayStatus(pcmhn);
                        // the file slot doesn't start playing immediately
                        // because of CD access
                        if(status == PCM_STAT_PLAY_TIME)
                            sndSlots[i].frame++;
                		if(status == PCM_STAT_PLAY_END) {
                		    // must destroy everything
                			PCM_DestroyGfsHandle(pcmhn);
                			GFS_Close(gfshn);
                			// check repeat
                            if(sndSlots[i].repeat == 1) {
                                PLAYPCM_MENU_playFile(slot, ptr);
                                sndSlots[i].frame = 0;
                                status = PCM_GetPlayStatus(pcmhn);
                            } else sndSlots[i].playing = 0;    
                		}
                        if(status == PCM_STAT_PLAY_ERR_STOP)
                            status = PCM_STAT_PLAY_END+2;
                    }    
                } else status = PCM_STAT_PLAY_END+1;
                
                // redraw for activated slot
                if(i == slot) {
                    // frame
                    TXTMEN_setFloatValueAt(ptr, PLAYPCM_MENU_LENGTH, ((float)(sndSlots[i].frame * my_vblanks) / 60.0));
                    TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_LENGTH);
                    // status
                    TXTMEN_setIntValueAt(ptr, PLAYPCM_MENU_STATUS, status);
                    TXTMEN_redrawValueAt(ptr, PLAYPCM_MENU_STATUS);
                }    
            }     
        }    
            return;
    }
}


/********************************************************************************/
/* textual menu construction ****************************************************/
/********************************************************************************/
void PLAYPCM_MENU_init(void **menu, void *scroll, unsigned char *exitText, unsigned char vblanks)
{
    int i;
    
    my_scroll = scroll;
    my_vblanks = vblanks;

//    sound_size = 0;
  
	if ((sound = (Uint8 *)malloc(SOUND_BUFFER_SIZE)) == NULL)
		return;
	sndSlots[0].buffer.address = (Uint8 *)&sound[0];
    // init slots
    for(i = 0; i < PLAYPCM_SLOT_NUM; i++) {
        sndSlots[i].repeat = 0;
        sndSlots[i].frame = 0;
    }    

    TXTMEN_init(menu, scroll, "Rockin'-B's PCM Player");
    TXTMEN_addButton(*menu, "load from CD");
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_LOADCD, *menu, PLAYPCM_MENU_callback);
//    TXTMEN_addButton(*menu, "rescan CD");
//    TXTMEN_setCallback(*menu, PLAYPCM_MENU_RESCAN, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addButton(*menu, "stop CD");
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_STOPCD, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addButton(*menu, "play");
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_PLAY, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addButton(*menu, "stop");
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_STOP, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addCheckbox(*menu, "repeat", 0);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_REPEAT, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addListbox(*menu, "select sound slot");
    TXTMEN_addItem(*menu, "high work RAM");
    TXTMEN_addItem(*menu, "low work RAM");
    TXTMEN_addItem(*menu, "CD-ROM file");
    TXTMEN_setIntValueAt(*menu, PLAYPCM_MENU_SLOT, 0);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_SLOT, *menu, PLAYPCM_MENU_callback);

    TXTMEN_addListbox(*menu, "channels");
    TXTMEN_addItem(*menu, "mono");
    TXTMEN_addItem(*menu, "stereo");
    TXTMEN_setIntValueAt(*menu, PLAYPCM_MENU_CHANNELS, 0);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_CHANNELS, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addListbox(*menu, "resolution");
    TXTMEN_addItem(*menu, "8 bit");
    TXTMEN_addItem(*menu, "16 bit");
    TXTMEN_setIntValueAt(*menu, PLAYPCM_MENU_RESOLUTION, 1);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_RESOLUTION, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addSpinnerInt(*menu, "samplerate", 1000, 100000, 10, 11025);
    sound_dat.pitch = PCM_computePitch(11025);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_SAMPLERATE, *menu, PLAYPCM_MENU_callback);
    // convert OCT to signed
    TXTMEN_addSpinnerInt(*menu, "OCT", -8, 7, 1, PCM_toSXT4(PCM_toOCT(sound_dat.pitch)));
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_OCT, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addSpinnerInt(*menu, "FNS", 0, 0x3FF, 1, PCM_toFNS(sound_dat.pitch));
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_FNS, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addSpinnerInt(*menu, "level", 0, 127, 16, sound_dat.level);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_LEVEL, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addSpinnerInt(*menu, "pan", -128, 127, 8, sound_dat.pan);
    TXTMEN_setCallback(*menu, PLAYPCM_MENU_PAN, *menu, PLAYPCM_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "length", 0.0, 60.0, 0.01, 0.0);
    TXTMEN_disable(*menu);
    // PCM status    
    TXTMEN_addListbox(*menu, "status");
    TXTMEN_addItem(*menu, "create");
    TXTMEN_addItem(*menu, "pause");
    TXTMEN_addItem(*menu, "start");
    TXTMEN_addItem(*menu, "header");
    TXTMEN_addItem(*menu, "playing");
    TXTMEN_addItem(*menu, "end");
    TXTMEN_addItem(*menu, "stop");
    TXTMEN_addItem(*menu, "abnormal stop");
    TXTMEN_disable(*menu);
    
    // exit button ?
    if(exitText != NULL) {
        TXTMEN_addButton(*menu, exitText);
        TXTMEN_setCallback(*menu, PLAYPCM_MENU_EXIT, *menu, PLAYPCM_MENU_callback);
    }    
}
