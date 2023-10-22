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

#include "RB_MAP_compare.h"
#include "RB_MAP.h"

int MAP_countPolygons(PolygonData *obj, unsigned int nObj)
{
    int i, count;
    
    if(obj == NULL)
        return 0;
    
    count = 0;
    for(i = 0; i < nObj; i++) {
        count += (obj + i)->data.nbPolygon;
    }

    return count;
}


// to verify the conversion
ReturnValue MAP_compareMapsMinMax(MapData *map1, MapData *map2)
{
    int i, j;
    ReturnValue ret = RETURN_OK;
    FIXED min1[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED min2[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED max1[XYZ] = {0x80000000, 0x80000000, 0x80000000};
    FIXED max2[XYZ] = {0x80000000, 0x80000000, 0x80000000};
    
    if((map1 == NULL) || (map2 == NULL))
        return RETURN_ERR_NULLARG;

    printf("checkin for differences: min max point locations...\n");
    // find min max bounds
    for(i = 0; i < (map1->mapSize.x * map1->mapSize.y); i++) {
        for(j = 0; j < (map1->tiles + i)->nObj; j++) {
            ret = POL_updateMinMax(((map1->tiles + i)->obj + j)->data.pntbl, ((map1->tiles + i)->obj + j)->data.nbPoint, min1, max1, XYZ);
            if(ret != RETURN_OK) {
                printf("Error finding minmax!\n");
                continue;
            }
        }
    }
    for(i = 0; i < (map2->mapSize.x * map2->mapSize.y); i++) {
        for(j = 0; j < (map2->tiles + i)->nObj; j++) {
            ret = POL_updateMinMax(((map2->tiles + i)->obj + j)->data.pntbl, ((map2->tiles + i)->obj + j)->data.nbPoint, min2, max2, XYZ);
            if(ret != RETURN_OK) {
                printf("Error finding minmax!\n");
                continue;
            }
        }
    }
    if((min1[X] != min2[X]) || (min1[Y] != min2[Y]) || (min1[Z] != min2[Z]) || (max1[X] != max2[X]) || (max1[Y] != max2[Y]) || (max1[Z] != max2[Z])) {
            printf("Difference: maps got different min and max point locations!\n");
            printf("min1(%f, %f, %f), max1(%f, %f, %f)!\n", toFLOAT(min1[X]), toFLOAT(min1[Y]), toFLOAT(min1[Z]), toFLOAT(max1[X]), toFLOAT(max1[Y]), toFLOAT(max1[Z]));
            printf("min2(%f, %f, %f), max2(%f, %f, %f)!\n", toFLOAT(min2[X]), toFLOAT(min2[Y]), toFLOAT(min2[Z]), toFLOAT(max2[X]), toFLOAT(max2[Y]), toFLOAT(max2[Z]));
    }
    printf("finished!\n");
    return ret;
}



ReturnValue MAP_compareMapsAttributes(MapData *map1, MapData *map2)
{
    if((map1 == NULL) || (map2 == NULL))
        return RETURN_ERR_NULLARG;
        
    printf("checkin for differences: map attributes...\n");
    if((map1->mapSize.x != map2->mapSize.x) || (map1->mapSize.y != map2->mapSize.y)) {
        printf("Difference: mapSize\n");
        printf("map1(%i, %i), map2(%i, %i)!\n", map1->mapSize.x, map1->mapSize.y, map2->mapSize.x, map2->mapSize.y);
    }
    if((map1->tileSize.x != map2->tileSize.x) || (map1->tileSize.y != map2->tileSize.y)) {
        printf("Difference: tileSize\n");
        printf("map1(%f, %f), map2(%f, %f)!\n", toFLOAT(map1->tileSize.x), toFLOAT(map1->tileSize.y), toFLOAT(map2->tileSize.x), toFLOAT(map2->tileSize.y));
    }
    if((map1->tileSize.x != map2->tileSize.x) || (map1->tileSize.y != map2->tileSize.y)) {
        printf("Difference: tileSize\n");
        printf("map1(%f, %f), map2(%f, %f)!\n", toFLOAT(map1->tileSize.x), toFLOAT(map1->tileSize.y), toFLOAT(map2->tileSize.x), toFLOAT(map2->tileSize.y));
    }
    if((map1->mapOrigin.x != map2->mapOrigin.x) || (map1->mapOrigin.y != map2->mapOrigin.y)) {
        printf("Difference: mapOrigin\n");
        printf("map1(%f, %f), map2(%f, %f)!\n", toFLOAT(map1->mapOrigin.x), toFLOAT(map1->mapOrigin.y), toFLOAT(map2->mapOrigin.x), toFLOAT(map2->mapOrigin.y));
    }
    printf("finished!\n");
    
    return RETURN_OK;
}

ReturnValue MAP_compareMaps(MapData *map1, MapData *map2)
{
    ReturnValue ret;
    int failed_tests = 0;
    
    if((map1 == NULL) || (map2 == NULL))
        return RETURN_ERR_NULLARG;
        
    // light tests to heavier tests:
    if(MAP_compareMapsAttributes(map1, map2) != RETURN_OK) {
        printf("Error!\n");
        failed_tests++;
    }
    if(MAP_compareMapsMinMax(map1, map2) != RETURN_OK) {
        printf("Error!\n");
        failed_tests++;
    }
    
    printf("Result: %i tests failed!\n", failed_tests);
    if(failed_tests > 0)
        return RETURN_ERR_UNKNOWN;
    else return RETURN_OK;
}
