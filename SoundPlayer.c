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
//#include "sddrvs.dat"
#include <RB_TXT_SCROLL.h>
#include "RB_playPCM.h"
#include "RB_playCDDA.h"
#include "RB_playVGM.h"
#include "RB_menu3D.h"
#include <RB_were_here.h>
// debug
#include "sega_snd.h"

ReturnValue TXTMEN_setCallafter(void *menu, int item, void *ptr, void (*func)(int item, void *ptr));
Sint32 MINCD_init();
ReturnValue TXTMEN_execute(void *menu);

/*------------------------------------------------------------------------*/
#define VBLANKS     1

// !!!!! VERY IMPORTANT
// don't take different gouraud table addresses
// in the .mdl files and here
#define	GRaddr	0x1c38 //0xe000
// SGL300PV.TXT: suppose it's maximum sized gouraud table in use
#define GOUR_REAL_MAX   513     
static GOURAUDTBL		gourRealMax[GOUR_REAL_MAX];
static Uint8			vwork[GOUR_REAL_MAX];


#define	TXT_SCROLL_XRES	40
#define TXT_SCROLL_YRES 30
void *screen=NULL, *scroll=NULL, *pcm_menu=NULL, *vgm_menu=NULL, *cdda_menu=NULL, *gfx_menu=NULL;
unsigned char debug_menu=0;

#define	SDDRV_NAME	"SDDRVS.TSK"
#define	SDDRV_SIZE	26610 //0x7000
#define	SDDRV_ADDR	0x00202000//0x6080000

/*------------------------------------------------------------------------*/

void init_sound(){
    char sound_map[] = {0xff , 0xff} ;

		unsigned char *sddrvstsk;
		sddrvstsk = (unsigned char *)SDDRV_ADDR;
		
		GFS_Load(GFS_NameToId((Sint8*)SDDRV_NAME),0,(void *) sddrvstsk,SDDRV_SIZE);
		slInitSound(sddrvstsk , SDDRV_SIZE , (Uint8 *)sound_map , sizeof(sound_map)) ;
		sddrvstsk = NULL;	
	
//	GFS_Load(GFS_NameToId(SDDRV_NAME),0,(void *)SDDRV_ADDR,SDDRV_SIZE);
//	SND_INI_PRG_ADR(snd_init) = (Uint16 *)SDDRV_ADDR;
//	SND_INI_PRG_SZ(snd_init)  = (Uint16)  SDDRV_SIZE;	
//    slInitSound(sddrvstsk , sizeof(sddrvstsk) , (Uint8 *)sound_map , sizeof(sound_map)) ;
//    slInitSound(SDDRV_ADDR , SDDRV_SIZE , (Uint8 *)sound_map , sizeof(sound_map)) ;
}
// derived from SMPPCM4.C
void sndInit(void)
{
	SndIniDt 	snd_init;
    char sound_map[] = {0xff , 0xff} ;

//	GFS_Load(GFS_NameToId(SDDRV_NAME),0,(void *)SDDRV_ADDR,SDDRV_SIZE);
	SND_INI_PRG_ADR(snd_init) = (Uint16 *)SDDRV_ADDR;
	SND_INI_PRG_SZ(snd_init)  = (Uint16)  SDDRV_SIZE;	
//	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvstsk;
//	SND_INI_PRG_SZ(snd_init) 	= (Uint16 )sizeof(sddrvstsk);
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)sound_map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)sizeof(sound_map);
	SND_Init(&snd_init);
	SND_ChgMap(0);
}

/*-----------------------------------------------------------------------*/
enum {
    SOUNDPLAYER_CALLBACK_PCM,    
    SOUNDPLAYER_CALLBACK_CDDA,
	SOUNDPLAYER_CALLBACK_VGM,
    SOUNDPLAYER_CALLBACK_3D  
};

// wrapper for PCM, CDDA, 3D display callback
// to provide all of them with 3D display
void SOUNDPLAYER_callback(int item, void *ptr)
{
    switch(item) {
        case SOUNDPLAYER_CALLBACK_PCM:
            PLAYPCM_MENU_callback(PLAYPCM_MENU_AFTER, ptr);
            break; 

        case SOUNDPLAYER_CALLBACK_CDDA:
            PLAYCDDA_MENU_callback(PLAYCDDA_MENU_AFTER, ptr);
            break;
			
        case SOUNDPLAYER_CALLBACK_VGM:
            PLAYVGM_MENU_callback(PLAYVGM_MENU_AFTER, ptr);
            break;			
            
        default:
            break;
    }
    // all will get 3D display
    MENU3D_MENU_callback(MENU3D_MENU_AFTER, gfx_menu);
    slSynch();
}    



void init_player()
{
    // menu creation
    if((Smpc_Peripheral[0].data & PER_DGT_ST) == 0)
        debug_menu = 1;
    else debug_menu = 0;
//  debug_menu = 1;
 
    // prepare the TXT scroll stuff
    TXTSCR_initScreen(&screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES);
    TXTSCR_initScroll(&scroll, screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES, 0, 0);
    // init VGM player
    PLAYVGM_MENU_init(&vgm_menu, scroll, "to CDDA player");
    TXTMEN_setCallafter(vgm_menu, SOUNDPLAYER_CALLBACK_VGM, vgm_menu, SOUNDPLAYER_callback);
/* 
 // init PCM player
    PLAYPCM_MENU_init(&pcm_menu, scroll, "to VGM player", VBLANKS);
    TXTMEN_setCallafter(pcm_menu, SOUNDPLAYER_CALLBACK_PCM, pcm_menu, SOUNDPLAYER_callback);
    // init CDDA player
    PLAYCDDA_MENU_init(&cdda_menu, scroll, (debug_menu == 0 ? "to PCM player" : "to 3D menu"));
    TXTMEN_setCallafter(cdda_menu, SOUNDPLAYER_CALLBACK_CDDA, cdda_menu, SOUNDPLAYER_callback);
*/	
    // init 3D menu
        // we always use 3D display, so we always need 3D default values
        // NOTE: before MENU3D_MENU_init()
    MENU3D_defaults();
    if(debug_menu == 1) {
        MENU3D_MENU_init(&gfx_menu, scroll, "to PCM player");
        TXTMEN_setCallafter(gfx_menu, SOUNDPLAYER_CALLBACK_3D, gfx_menu, SOUNDPLAYER_callback);
    } else gfx_menu = (void *)1;    // FIX: we fake it, to enable 3D callback
}

// can only register one function with
// slIntFunction();
// so a wrapper is needed
void my_vblank()
{
    slGouraudTblCopy();
    PCM_VblIn();
}

// debug
#include <malloc.h>
void ss_main(void)
{
    slInitSystem(TV_320x240, NULL, VBLANKS);
//    slScrAutoDisp(NBG0ON | NBG1ON);
//	slTVOn();		
// 	slPrint("ss_main",slLocate(10,9));	
    // the real time gouraud function is 
    // only documented in japanese language(only little in english)...
    // instead of copying the global gouraud table to VRAM
    // only once at startup
    // slPutPolygonX() computes a new one
    // for every XPDATA object every frame
    // and copies the table to VRAM with slGouraudTblCopy()
    // For this, a local temporary gouraud table is needed
    // which is big enough to hold the biggest gouraud table in use
	slInitGouraud(gourRealMax, GOUR_REAL_MAX, GRaddr, vwork);
    // INT.TXT says
    // Function: 
    // Sets the user-defined function to be executed during V-blank-in interrupt 
    // -> so it can be used for PCM_VblIn
	slIntFunction(my_vblank);
    // further SBL PCM initializations
    // see my_vblank()

        // SBL PCM needs it's GFS_init(), too
    MINCD_init();
    init_sound();
    // this NEEDS to be done to get the SBL PCM lib to work
    // otherwise the Saturn will freeze on play start
    sndInit();
	PCM_Init();
	PCM_DeclareUseAdpcm();
    init_player();
	
    while(1) {
        TXTMEN_execute(pcm_menu);
		TXTMEN_execute(vgm_menu);
        TXTMEN_execute(cdda_menu);
        if(debug_menu == 1)
            TXTMEN_execute(gfx_menu);
    }    
    // never reach:
    PCM_Finish();
    while(1) {
        slSynch();
    }
}

/*-----------------------------------------------------------------------*/

