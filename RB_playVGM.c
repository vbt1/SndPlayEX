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

#define SOUND_BUFFER_SIZE2   (0x40000) // + 0x4d000)

//static char sound[SOUND_BUFFER_SIZE];
//static char *sound=NULL;
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
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)NULL, SOUND_BUFFER_SIZE2, -1}, 0 },
    // low work RAM, 1MByte
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)0x200000, 0x100000, -1}, 0 },
    // CD-ROM file slot
    { {0, (void *)0, {(_Mono | _PCM16Bit) , 0 , 127 , 0 , 0x7000 , 0 , 0 , 0 , 0} }, {(void *)0, 0, -1}, 0 }
};


//extern PDATA PD_Cube;

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
static unsigned int drvLoaded=0;
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
                    {"*.mod", {0, sndSlots[PLAYVGM_SLOT_HIGHMEM].buffer.size}, "MOD Music file", FIO_OP_LOAD},
                    {"*.vgm", {0, sndSlots[PLAYVGM_SLOT_HIGHMEM].buffer.size}, "Video Game Music file", FIO_OP_LOAD},
                    {"*.mdx", {0, sndSlots[PLAYVGM_SLOT_HIGHMEM].buffer.size}, "Sharp X68000 Music file", FIO_OP_LOAD},
                };

                // now here we use the selected sound slot size
                ReturnValue ret;
                
                // don't access CD while file playback in process
                // or the sound will loop strange
                if(sndSlots[PLAYVGM_SLOT_FILE].playing == 1)
                    return;
                // start CD browser
                TXTSCR_clear(my_scroll, TXT_DRAW_BOTH);
 
				if(drvLoaded==0)
				{
					MINCD_init(); // vbt ajout 					
					DrvInitDamageXsound(); 
					Uint32 end = *(Uint32 *)(0x25A00100);
					*(Uint32 *)(0x25A00104)=end;				
					sndSlots[slot].buffer.address=0x5A00000+end;
					
					drvLoaded = 1;
				}
				
                FIO_in in = {ftypes, sizeof(ftypes)/sizeof(FIO_ftype), sndSlots[slot].buffer.address, sndSlots[slot].buffer.size};

			*(Uint16 *)(0x25A00108)= 2; // stop
			sndSlots[slot].playing = 0;
			Uint32 msk;

			msk = get_imask();
			set_imask(15);
			
//			while(*(Uint16 *)(0x25A0010C)!=0)
			while(*(Uint16 *)(0x25A00108)!=0)
			{
				for(int w=0;w<500;w++)
				{
					asm("nop\n"); // waste time
				}
			}
		
			set_imask(msk);	

/*
				switch (slot)
				{
					case 0: // VGM
						*(Uint16 *)(0x25A00108)=0;
						break;
					case 1: // MDX
						*(Uint16 *)(0x25A00108)=3;						
						break;						
					case 2: // MOD
						*(Uint16 *)(0x25A00108)=4;
						break;						
				}
*/			
				ret = MINCD_load("Select a sound:", &in, &out);
                if(ret == RETURN_OK) {
                    // file data is in memory
#if 1
                    if(out.otype == FIO_LOADED) 
					{
#else   // no wave support untill one buffer conversion is finished
                    if((out.otype == FIO_LOADED) && (out.fti != 0) && (out.fti != 4)) {
#endif
                        // save the size
                        sndSlots[slot].sound.size = out.value.size;
                            // take care of wave header
                        sndSlots[slot].sound.address = sndSlots[slot].buffer.address;
                        // immediately start playing
                        PLAYVGM_MENU_callback(PLAYVGM_MENU_PLAY, ptr);
                    } else if(out.otype == FIO_GOT_FID) {
                        // only take over file id
                        // if the file slot is selected
                        if(slot == PLAYVGM_SLOT_FILE) {
                            sndSlots[slot].buffer.fid = out.value.fid;
                            // immediately start playing
                            PLAYVGM_MENU_callback(PLAYVGM_MENU_PLAY, ptr);
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

        case PLAYVGM_MENU_STOPCD:
            {
                CdcPos poswk;
                // seek home position
                CDC_POS_PTYPE(&poswk) = CDC_PTYPE_DFL;
                CDC_CdSeek(&poswk);
            }
            return;

        case PLAYVGM_MENU_PLAY:
			if(sndSlots[slot].playing == 0) {
//			*(Uint16 *)(0x25A00108)=1; // start				
			
				switch (slot)
				{
					case 0: // VGM
						*(Uint16 *)(0x25A00108)=4;
						break;
					case 1: // MDX
						*(Uint16 *)(0x25A00108)=4;						
						break;						
					case 2: // MOD
						*(Uint16 *)(0x25A00108)=4;
						break;						
				}			
			
			
				sndSlots[slot].playing = 1;
				sndSlots[slot].frame = 0;
			}   
            return;
            
        case PLAYVGM_MENU_STOP:
	
			*(Uint16 *)(0x25A00108)= 2; // stop
			sndSlots[slot].playing = 0;
			Uint32 msk;

			msk = get_imask();
			set_imask(15);
			
//			while(*(Uint16 *)(0x25A0010C)!=0)
			while(*(Uint16 *)(0x25A00108)!=0)
			{
				for(int w=0;w<500;w++)
				{
					asm("nop\n"); // waste time
				}
			}
		
			set_imask(msk);					
            return;
            
        case PLAYVGM_MENU_REPEAT:
            sndSlots[slot].repeat = TXTMEN_getIntValueAt(ptr, PLAYVGM_MENU_REPEAT);
            return;

        case PLAYVGM_MENU_SLOT:
            return;

        case PLAYVGM_MENU_EXIT:
			drvLoaded=0;		
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
                MINCD_init(); // vbt on vire
            }
        }    
        return;
    }
}

void PLAYVGM_MENU_init(void **menu, void *scroll, unsigned char *exitText)
{
    my_scroll = scroll;	
	
    TXTMEN_init(menu, scroll, "DamageX's VGM Player");

    TXTMEN_addButton(*menu, "load from CD");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_LOADCD, *menu, PLAYVGM_MENU_callback);
//    TXTMEN_addButton(*menu, "rescan CD");
//    TXTMEN_setCallback(*menu, PLAYVGM_MENU_RESCAN, *menu, PLAYVGM_MENU_callback);
    TXTMEN_addButton(*menu, "play");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_PLAY, *menu, PLAYVGM_MENU_callback);
    TXTMEN_addButton(*menu, "stop");
    TXTMEN_setCallback(*menu, PLAYVGM_MENU_STOP, *menu, PLAYVGM_MENU_callback);
//    TXTMEN_addCheckbox(*menu, "repeat", 0);
//    TXTMEN_setCallback(*menu, PLAYVGM_MENU_REPEAT, *menu, PLAYVGM_MENU_callback);

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
//--------------------------------------------------------------------------------------------------------------------------------------
Sint32 GetFileSize(int file_id)
{
#ifndef ACTION_REPLAY	
	GfsHn gfs;
    Sint32 sctsize, nsct, lastsize;
    
    gfs = GFS_Open(file_id);
    GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);

    GFS_Close(gfs);
//	return sctsize*nsct;
	return (sctsize*(nsct-1) + lastsize);
#else
    return 300;
#endif	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitDamageXsound()
{
// intsamples=46        ; 60Hz default interrupt rate
// en dur voir comment gérer le 50hz	
	
// init controller port
	*(Uint8 *)(0x2010007F)=0;
	*(Uint8 *)(0x2010007D)=3;
	*(Uint8 *)(0x20100079)=96;
    *(Uint8 *)(0x2010007B)=96;

// turn off the 68000
	*(Uint8 *)(0x2010001F)=7;
	for(int w=0;w<500;w++)
	{
		asm("nop\n"); // waste time
	}
	*(Uint16 *)(0x5B00400)=0x20F;

	for(int w=0;w<500;w++)
	{
		asm("nop\n"); // waste time
	}

	*(Uint16 *)(0x5B00402)=0x3F;

	for(int w=0;w<500;w++)
	{
		asm("nop\n"); // waste time
	}
	signed int fid = GFS_NameToId("VGM68.BIN");
	signed int sz = GetFileSize(fid);
	GFS_Load(fid,0,(void *)0x5A00000,sz);
	
	sndSlots[0].buffer.size = 0x800000-sz;
	sndSlots[1].buffer.size = 0x800000-sz;
	sndSlots[2].buffer.size = 0x800000-sz;	

// turn on the 68000
	*(Uint8 *)(0x2010001F)=6;

	while(*(Uint16 *)(0x25A0010C)!=0)
	{
		for(int w=0;w<500;w++)
		{
			asm("nop\n"); // waste time
		}
	}
	
	Uint32 end = *(Uint32 *)(0x25A00100);
//	GFS_Load(GFS_NameToId("035.VGM"),0,(void *)(0x5A00000+end),14624);

 //   memcpy((UINT16 *)(0x25A00104),(UINT16 *)(0x25A00100),sizeof(UINT16));//
 /*
 *(Uint32 *)(0x25A00104)=end;
 *(Uint16 *)(0x25A00108)=1; // start	
*/
}
//-------------------------------------------------------------------------------------------------------------------------------------
	
