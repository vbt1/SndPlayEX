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

// GROUND_SATURN
// inspired by DRIVING example

#include <SGL.H>
#include "RB_POL.h"
#include "RB_MAP_def.h"
#include "RB_MAP.h"
#include <RB_RETVAL.H>

#define slMulFX_Ground(a, b) slMulFX(a, b)
// neccessary on Saturn
#define slDivFX_Ground(a, b) toFIXED(toFLOAT(a) / toFLOAT(b))
//#define slDivFX_Ground(a, b) slDivFX(a, b)

#ifdef GROUND_SATURN
ReturnValue MAP_checkGround(MapData *map, FIXED *pos, FIXED r2, FIXED sineY, FIXED cosineY, unsigned char *hit, unsigned char *typeID, FIXED *height, FIXED *directionX, FIXED *directionZ, FIXED *angleX, FIXED *angleZ)
#else
ReturnValue MAP_checkGround(MapData *map, FIXED *pos, unsigned char *hit, unsigned char *typeID, FIXED *height)
#endif
{
    TileData *tile;
    FIXED mapRelX, mapRelZ;
    int minX, maxX, minZ, maxZ;
    int x, z, g;
#ifdef GROUND_SATURN
    GroundData *ground = NULL;
    FIXED dist, dist_min = r2;
    FIXED dx, dz, DX = 0, DZ = 0;
#endif    
    
    if((map == NULL) || (pos == NULL))
        return RETURN_ERR_NULLARG;

        
	// find out the tile I'm in
	   // OPTIMZABLE
//	mapRelX = slDivFX_Ground(pos[X] - map->mapOrigin.x, map->tileSize.x);
//	mapRelZ = slDivFX_Ground(pos[Z] - map->mapOrigin.z, map->tileSize.z);
	mapRelX = (pos[X] - map->mapOrigin.x) >> (map->tileSize.x - 1);
	mapRelZ = (pos[Z] - map->mapOrigin.z) >> (map->tileSize.z - 1);
	
	// out
	// check the 4 nearest tiles
//	minX = (mapRelX - toFIXED(0.5)) >> 16;
	minX = (mapRelX - 1) >> 1;
    if(minX < 0)
        minX = 0;
//	maxX = (mapRelX + toFIXED(0.5)) >> 16;
	maxX = (mapRelX + 1) >> 1;
	if(maxX >= map->mapSize.x)
	   maxX = map->mapSize.x - 1;
//    // on map?
//    if(minX > maxX) {
//        *hit = 0;
//        return RETURN_OK;
//    }

//	minZ = (mapRelZ - toFIXED(0.5)) >> 16;
	minZ = (mapRelZ - 1) >> 1;
    if(minZ < 0)
        minZ = 0;
//	maxZ = (mapRelZ + toFIXED(0.5)) >> 16;
	maxZ = (mapRelZ + 1) >> 1;
	if(maxZ >= map->mapSize.z)
	   maxZ = map->mapSize.z - 1;
//    // on map?
//    if(minZ > maxZ) {
//        *hit = 0;
//        return RETURN_OK;
//    }
	   
//	were_here("mapRelPos (%f, %f)", toFLOAT(mapRelX), toFLOAT(mapRelZ));
//    were_here("searching: x(%i, %i), z(%i, %i)!", minX, maxX, minZ, maxZ);
	// search all 4 tiles
    for(z = minZ; z <= maxZ; z++) {
        for(x = minX; x <= maxX; x++) {
            tile = *(map->tiles + z*map->mapSize.x + x);
            if(tile == NULL)
                continue;
            // search all ground data
//            were_here("searching %i grounds!", tile->nGround);
            for(g = 0; g < tile->nGround; g++) {
#ifdef GROUND_SATURN
                // 2D squared circle distance
                dx = pos[X] - (tile->ground + g)->center[X];
                dz = pos[Z] - (tile->ground + g)->center[Z];
                dist = slMulFX_Ground(dx, dx) + slMulFX_Ground(dz, dz);
                // track shortest distance
                if(dist < dist_min) {
                    dist_min = dist;
                    DX = dx;
                    DZ = dz;
                    ground = tile->ground + g;
                }
#else            
                // check hit
                ret = POL_checkGround(tile->ground + g, pos, hit, typeID, height);
                if(ret != RETURN_OK)
                    return ret;
                if(*hit == 1) {
//                    were_here("Found a hit in %i grounds!", tile->nGround);
                    return RETURN_OK;
                }    
#endif
            }
        }                  
    }
    
#ifdef GROUND_SATURN
    // found a closest ground face?
    if(ground == NULL) {
        *hit = 0;
        return RETURN_OK;
    }
    *hit = 1;
    // now compute results
    if(typeID != NULL)
        *typeID = ground->typeID;
    if(height != NULL) {
        *height = slMulFX_Ground(ground->normal.x, DX) + slMulFX_Ground(ground->normal.z, DZ) + ground->center[Y];
//        note_printf("Center at (%f, %f, %f), height %f\n", toFLOAT(ground->center[X]), toFLOAT(ground->center[Y]), toFLOAT(ground->center[Z]), toFLOAT(*height));
    }
    if(directionX != NULL)
        *directionX = ground->direction.x;
    if(directionZ != NULL)
        *directionZ = ground->direction.z;
    
    if(angleX != NULL) {
        dx = slMulFX_Ground(ground->normal.x, sineY) + slMulFX_Ground(ground->normal.z, cosineY);
        *angleX = slAtan(dx, toFIXED(1.0));
    }
    if(angleZ != NULL) {
        dz = slMulFX_Ground(ground->normal.x, cosineY) + slMulFX_Ground(ground->normal.z, sineY);
        *angleZ = slAtan(dz, toFIXED(1.0));
    }
#endif    
    // no hit
    return RETURN_OK;                  
}


