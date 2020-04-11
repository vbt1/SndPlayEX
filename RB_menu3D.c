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
// !!! you should better not forget this include
// for an unknown reason, gcc doesn't complain and the
// 3D menu is corrupted
#include <RB_TXT_MENU.h>
#include "RB_menu3D.h"


#include "3D/Notes3SSX.mdl"
//extern PDATA PD_Cube;

#define toFLOAT(fx)         (((float)fx) / 65536.0)
#define ANGtoDEG(a)         ((((float)a) * 360.0) / 65536.0)

static ANGLE ang[XYZ];
static ANGLE angAdd[XYZ];
static FIXED pos[XYZ];
static FIXED sca[XYZ];
// real time gouraud:
// let the light go around the object
static FIXED light[XYZ];
static ANGLE light_ang[XYZ];
static ANGLE light_angAdd[XYZ];

unsigned char enable_gouraud = 0;


void MENU3D_computeLight()
{
    FIXED light_init[XYZ] = { toFIXED(0.57735026), toFIXED(0.57735026), toFIXED(0.57735026) };
    
    slPushUnitMatrix();
    {
        // perform rotation
        slRotX(light_ang[X]);            								 
        slRotY(light_ang[Y]);			   
        slRotZ(light_ang[Z]);
        // transform light vector
        slCalcVector(light_init, light);
    }
    slPopMatrix();
    // advance light rotation
    // !!! NO OVERFLOW CHECKING !!!
    light_ang[X] += light_angAdd[X];
    light_ang[Y] += light_angAdd[Y];
    light_ang[Z] += light_angAdd[Z];
}



void MENU3D_MENU_callback(int item, void *ptr)
{
//slPrint("MENU3D_MENU_callback",slLocate(10,3));
	
//    were_here("in callback MENU3D_MENU_AFTER, ptr %i", ptr);
    if(ptr == NULL)
	{
//slPrint("MENU3D_MENU_callback null !!!",slLocate(10,3));		
        return;
	}
    switch(item) {
        // 3D debug
        case MENU3D_MENU_GOURAUD:
            enable_gouraud = TXTMEN_getIntValueAt(ptr, MENU3D_MENU_GOURAUD);
            return;
            
        case MENU3D_MENU_SCAX:
            sca[X] = toFIXED(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_SCAX));
            return;

        case MENU3D_MENU_SCAY:
            sca[Y] = toFIXED(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_SCAY));
            return;

        case MENU3D_MENU_SCAZ:
            sca[Z] = toFIXED(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_SCAZ));
            return;

        case MENU3D_MENU_ANGX:
            angAdd[X] = DEGtoANG(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_ANGX));
            return;

        case MENU3D_MENU_ANGY:
            angAdd[Y] = DEGtoANG(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_ANGY));
            return;

        case MENU3D_MENU_ANGZ:
            angAdd[Z] = DEGtoANG(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_ANGZ));
            return;

        case MENU3D_MENU_POSX:
            pos[X] = toFIXED(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_POSX));
            return;

        case MENU3D_MENU_POSY:
            pos[Y] = toFIXED(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_POSY));
            return;

        case MENU3D_MENU_POSZ:
            pos[Z] = toFIXED(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_POSZ));
            return;

        case MENU3D_MENU_LIGHTX:
            light_angAdd[X] = DEGtoANG(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_LIGHTX));
            return;

        case MENU3D_MENU_LIGHTY:
            light_angAdd[Y] = DEGtoANG(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_LIGHTY));
            return;

        case MENU3D_MENU_LIGHTZ:
            light_angAdd[Z] = DEGtoANG(TXTMEN_getFloatValueAt(ptr, MENU3D_MENU_LIGHTZ));
            return;

        case MENU3D_MENU_EXIT:
            TXTMEN_leave(ptr);
            return;

        case MENU3D_MENU_AFTER:
//            were_here("MENU3D_MENU_AFTER");
            // should not depend on gfx_menu
            // because we may not have it created
            
            // 3D display
#if 1       
            // transform 3D model
            slUnitMatrix(CURRENT);
            // !!! for some strange reason the translation has to come first
            slTranslate(pos[X], pos[Y], pos[Z]);
            slScale(sca[X], sca[Y], sca[Z]);
            slRotX(ang[X]);            								 
            slRotY(ang[Y]);			   
            slRotZ(ang[Z]);
            // display
            if(enable_gouraud == 1) {
                MENU3D_computeLight();
                slPutPolygonX((XPDATA*)&xpdata_0, light);
            } else slPutPolygon((PDATA*)&xpdata_0);
//	           slPutPolygon(&PD_Cube) ;
            // advance movement
            ang[X] += angAdd[X];
            ang[Y] += angAdd[Y];
            ang[Z] += angAdd[Z];
#else
            // cube
            slPushUnitMatrix();
            {
	           slTranslate(toFIXED(0) , toFIXED(0) , toFIXED(500));
	           slRotY(ang[Y] + angAdd[Y]);
	           angAdd[Y] += 0x0100;
	           slPutPolygon(&PD_Cube);
            }
            slPopMatrix();
#endif
            return;
    }    
}


void MENU3D_defaults()
{
    // 3D, initialize before menu
    enable_gouraud = 1;
        // initial rotation of the object
    ang[X] = DEGtoANG(-76.0);
    ang[Y] = DEGtoANG(8.0);
    ang[Z] = DEGtoANG(313.0);
        // initial rotation speed of the object
    angAdd[X] = DEGtoANG(0.0);
    angAdd[Y] = DEGtoANG(0.0);
    angAdd[Z] = DEGtoANG(0.2);

    pos[X] = toFIXED(-5.0);
    pos[Y] = toFIXED(10.0);
    pos[Z] = toFIXED(120.0);

    sca[X] = toFIXED(2.0);
    sca[Y] = toFIXED(2.0);
    sca[Z] = toFIXED(2.0);
    // LIGHT VECTOR IN SCENE
        // initial light angle
    light_angAdd[X] = DEGtoANG(0.0);
    light_angAdd[Y] = DEGtoANG(0.0);
    light_angAdd[Z] = DEGtoANG(0.0);
        // initial light rotation speed
    light_angAdd[X] = DEGtoANG(0.1);
    light_angAdd[Y] = DEGtoANG(0.1);
    light_angAdd[Z] = DEGtoANG(0.1);
}


void MENU3D_MENU_init(void **menu, void *scroll, unsigned char *exitText)
{
    TXTMEN_init(menu, scroll, "Rockin'-B's 3D menu");
        // scale
    TXTMEN_addCheckbox(*menu, "gouraud", enable_gouraud);
    TXTMEN_setCallback(*menu, MENU3D_MENU_GOURAUD, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "scaleX", 0.0, 5.0, 0.1, toFLOAT(sca[X]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_SCAX, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "scaleY", 0.0, 5.0, 0.1, toFLOAT(sca[Y]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_SCAY, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "scaleZ", 0.0, 5.0, 0.1, toFLOAT(sca[Z]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_SCAZ, *menu, MENU3D_MENU_callback);
        // rotation
    TXTMEN_addSpinnerFloat(*menu, "angle speed X", -5, 5, 0.1, ANGtoDEG(angAdd[X]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_ANGX, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "angle speed Y", -5, 5, 0.1, ANGtoDEG(angAdd[Y]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_ANGY, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "angle speed Z", -5, 5, 0.1, ANGtoDEG(angAdd[Z]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_ANGZ, *menu, MENU3D_MENU_callback);
        // translation
    TXTMEN_addSpinnerFloat(*menu, "posX", -1000.0, 1000.0, 5, toFLOAT(pos[X]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_POSX, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "posY", -1000.0, 1000.0, 5, toFLOAT(pos[Y]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_POSY, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "posZ", -1000.0, 1000.0, 5, toFLOAT(pos[Z]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_POSZ, *menu, MENU3D_MENU_callback);
        // light rotation speed
    TXTMEN_addSpinnerFloat(*menu, "light speed X", -5, 5, 0.1, ANGtoDEG(light_angAdd[X]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_LIGHTX, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "light speed Y", -5, 5, 0.1, ANGtoDEG(light_angAdd[Y]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_LIGHTX, *menu, MENU3D_MENU_callback);
    TXTMEN_addSpinnerFloat(*menu, "light speed Z", -5, 5, 0.1, ANGtoDEG(light_angAdd[Z]));
    TXTMEN_setCallback(*menu, MENU3D_MENU_LIGHTX, *menu, MENU3D_MENU_callback);

    // exit button ?
    if(exitText != NULL) {
        TXTMEN_addButton(*menu, exitText);
        TXTMEN_setCallback(*menu, MENU3D_MENU_EXIT, *menu, MENU3D_MENU_callback);
    }    
}

