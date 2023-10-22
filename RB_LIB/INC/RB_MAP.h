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

#ifndef HAVE_RB_MAP_H
#define HAVE_RB_MAP_H

#include <stdio.h>

#include <RB_RETVAL.H>
#include "RB_POL.h"
#include "RB_MAP_def.h"

typedef struct OBJECT_ENTRY {
    // the search key
    unsigned int tile_offset;
    PolygonData pol;
    // point index reorganisation
    PointEntry *pe_array;
//    unsigned int pe_count;		// max point index
} ObjectEntry;

typedef struct VERIFY_DATA {
    // whole scene is merged to one object
    // it's nbPoint and nbPolygon
    // reflect the number of
    // different points and polygons,
    // since duplicates are removed
    PolygonData pol;
    // without duplicates removed
    unsigned int totalPoints;
    unsigned int totalPolygons;
} VerifyData;


typedef enum {
    MAPING_TYPE_MIDDLE,
    MAPING_TYPE_MEAN
} MapingType;

#ifdef __cplusplus
extern "C" {
#endif

// to find the dimensions of an PDATA or XPDATA array
ReturnValue MAP_updateMapSize(void **xpdata, unsigned int n, unsigned char type, FIXED *min, FIXED *max, unsigned char dim);
ReturnValue MAP_findMapSize(void **xpdata, unsigned int n, unsigned char type, FIXED *min, FIXED *max, unsigned char dim);


ReturnValue MAP_insertInMap(MapData *map, PolygonData *pol, MapingType mapingType, unsigned char break_over_tiles, unsigned char merge_in_tile, VerifyData *verify);
// encapsulates usual PDATA and XPDATA
ReturnValue MAP_insertInMap2(MapData *map, void *xpdata, unsigned char type, MapingType mapingType, unsigned char break_over_tiles, unsigned char merge_in_tile, VerifyData *verify);
// converts an PDATA or XPDATA array
// like PDATA *map_data[]
ReturnValue MAP_insertInMap3(MapData *map, void **xpdata, unsigned int n, unsigned char type, MapingType mapingType, unsigned char break_over_tiles, unsigned char merge_in_tile, VerifyData *verify);

// verify conversion
ReturnValue MAP_verify(MapData *map, VerifyData *verify);

// to clear and free the content
void MAP_clearPolygonData(PolygonData *pol);
void MAP_clearTileData(TileData *tile);
void MAP_clearMapData(MapData *map);
void MAP_clearVerifyData(VerifyData *verify);
void MAP_clearObjectEntry(ObjectEntry *oe);

ReturnValue MAP_computeGround(MapData *map, CollisionTypes *collisionTypes);
#ifdef GROUND_SATURN
ReturnValue MAP_checkGround(MapData *map, FIXED *pos, FIXED r2, FIXED sineY, FIXED cosineY, unsigned char *hit, unsigned char *typeID, FIXED *height, FIXED *directionX, FIXED *directionZ, FIXED *angleX, FIXED *angleZ);
#else
ReturnValue MAP_checkGround(MapData *map, FIXED *pos, unsigned char *hit, unsigned char *typeID, FIXED *height);
#endif

#ifdef __cplusplus
}
#endif

#endif // HAVE_RB_MAP_H
