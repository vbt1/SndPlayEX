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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "RB_POL.h"
#include "RB_MAP_def.h"
#include "RB_MAP.h"
#include <RB_RETVAL.H>


/***************************************************************************************************************/
/* for breaking an object apart ********************************************************************************/
/***************************************************************************************************************/

int compare_ObjectEntry(const void *a, const void *b)
{
    const ObjectEntry *oea = (const ObjectEntry *)a;
    const ObjectEntry *oeb = (const ObjectEntry *)b;

    return oea->tile_offset - oeb->tile_offset;
}
/***************************************************************************************************************/
/* end: for breaking an object apart ***************************************************************************/
/***************************************************************************************************************/



void MAP_clearPolygonData(PolygonData *pol)
{
    if(pol == NULL)
        return;

    POL_clearXPDATA(&(pol->data), pol->type);
    memset(pol, 0, sizeof(PolygonData));        
}


void MAP_clearTileData(TileData *tile)
{
    int i;
    
    if(tile == NULL)
        return;

    // clear all objects of a tile
    for(i = 0; i < tile->nObj; i++) {
        MAP_clearPolygonData(tile->obj + i);
    }
    if(tile->nObj > 0)
        free(tile->obj);
    if(tile->nGround > 0)
        free(tile->ground);
    memset(tile, 0, sizeof(TileData));
}

void MAP_clearMapData(MapData *map)
{
    int i;
    
    if(map == NULL)
        return;
    
    // clear all tiles content
    for(i = 0; i < (map->mapSize.x * map->mapSize.z); i++) {
        MAP_clearTileData(*(map->tiles + i));
    }
}

void MAP_clearVerifyData(VerifyData *verify)
{
    if(verify == NULL)
        return;
    
    MAP_clearPolygonData(&(verify->pol));
    memset(verify, 0, sizeof(VerifyData));
}

void MAP_clearObjectEntry(ObjectEntry *oe)
{
    if(oe == NULL)
        return;
    
    if(oe->pol.data.nbPoint > 0)
        free(oe->pe_array);

    MAP_clearPolygonData(&(oe->pol));
    memset(oe, 0, sizeof(ObjectEntry));
}



ReturnValue MAP_updateMapSize(void **xpdata, unsigned int n, unsigned char type, FIXED *min, FIXED *max, unsigned char dim)
{
    int i;
    ReturnValue ret;
    
    if((xpdata == NULL) || (min == NULL) || (max == NULL))
        return RETURN_ERR_NULLARG;
    
    if((type != IS_PDATA) && (type != IS_XPDATA))
        return RETURN_ERR_INVALIDARG;
        
    // find min max bounds
    for(i = 0; i < n; i++) {
        ret = POL_updateMinMax( type == IS_PDATA ? (*(((PDATA **)xpdata) + i))->pntbl : (*(((XPDATA **)xpdata) + i))->pntbl, type == IS_PDATA ? (*(((PDATA **)xpdata) + i))->nbPoint : (*(((XPDATA **)xpdata) + i))->nbPoint, min, max, dim);
        if(ret != RETURN_OK)
            return ret;
    }
    return RETURN_OK;
}

ReturnValue MAP_findMapSize(void **xpdata, unsigned int n, unsigned char type, FIXED *min, FIXED *max, unsigned char dim)
{
    int i;
    
    if((xpdata == NULL) || (min == NULL) || (max == NULL))
        return RETURN_ERR_NULLARG;
    
    if((type != IS_PDATA) && (type != IS_XPDATA))
        return RETURN_ERR_INVALIDARG;
    
    // init min and max
    for(i = 0; i < dim; i++) {
    	min[i] = 0x7FFFFFFF;
    	max[i] = 0x80000000;
    }

    return MAP_updateMapSize(xpdata, n, type, min, max, dim);
}


ReturnValue MAP_mapToGrid(MapData *map, FIXED *pos, unsigned short *mapX, unsigned short *mapZ)
{
    if((pos == NULL) || (mapX == NULL) || (mapZ == NULL))
        return RETURN_ERR_NULLARG;
    
            // convert to mapOrigin
    pos[X] -= map->mapOrigin.x;
    pos[Z] -= map->mapOrigin.z;
        // check on map #1
    if((pos[X] < 0) || (pos[Z] < 0))
        return RETURN_ERR_OOB;
        // map to correct tile
        // NOTE: this converts FIXED to int
//    *mapX = pos[X] / map->tileSize.x;
//    *mapZ = pos[Z] / map->tileSize.z;
    *mapX = pos[X] >> map->tileSize.x;
    *mapZ = pos[Z] >> map->tileSize.z;
        // check on map #2
    if((*mapX >= map->mapSize.x) || (*mapZ >= map->mapSize.z))
        return RETURN_ERR_OOB;

    return RETURN_OK;
}




// merge(alloc & copy) pol2 to pol1
ReturnValue MAP_copyToObj(PolygonData *pol1, PolygonData *pol2)
{
    ReturnValue ret;
    int i;
    
    if((pol1 == NULL) || (pol2 == NULL))
        return RETURN_ERR_NULLARG;

    if(pol1->type != pol2->type)
        return RETURN_ERR_INVALIDARG;

#if 1
    
    for(i = 0; i < pol2->data.nbPolygon; i++) {
        ret = POL_copyPolygon(&(pol1->data), &(pol2->data), pol2->type, NULL, i);
        if(ret != RETURN_OK)
            return ret;
    }

#else	// only append without check for duplicates
    // append POINT
    if(pol1->data.nbPoint == 0)
        pol1->data.pntbl = (POINT *)malloc(pol2->data.nbPoint * sizeof(POINT));
    else
        pol1->data.pntbl = (POINT *)realloc(pol1->data.pntbl, (pol1->data.nbPoint + pol2->data.nbPoint) * sizeof(POINT));
        
    if(pol1->data.pntbl == NULL)
        return RETURN_ERR_ALLOC;
        
    memcpy(pol1->data.pntbl + pol1->data.nbPoint, pol2->data.pntbl, pol2->data.nbPoint * sizeof(POINT));
    // check append VECTOR
    if(pol2->type == IS_XPDATA) {
        if(pol1->data.nbPoint == 0)
            pol1->data.vntbl = (VECTOR *)malloc(pol2->data.nbPoint * sizeof(VECTOR));
        else
            pol1->data.vntbl = (VECTOR *)realloc(pol1->data.vntbl, (pol1->data.nbPoint + pol2->data.nbPoint) * sizeof(VECTOR));
            
        if(pol1->data.vntbl == NULL)
            return RETURN_ERR_ALLOC;
            
        memcpy(pol1->data.vntbl + pol1->data.nbPoint, pol2->data.vntbl, pol2->data.nbPoint * sizeof(VECTOR));
    }

    // append POLYGON + ATTR
    if(pol1->data.nbPolygon == 0) {
        pol1->data.pltbl = (POLYGON *)malloc(pol2->data.nbPolygon * sizeof(POLYGON));
        pol1->data.attbl = (ATTR *)malloc(pol2->data.nbPolygon * sizeof(ATTR));
    } else {
        pol1->data.pltbl = (POLYGON *)realloc(pol1->data.pltbl, (pol1->data.nbPolygon + pol2->data.nbPolygon) * sizeof(POLYGON));
        pol1->data.attbl = (ATTR *)realloc(pol1->data.attbl, (pol1->data.nbPolygon + pol2->data.nbPolygon) * sizeof(ATTR));
        
    }
    if((pol1->data.pltbl == NULL) || (pol1->data.attbl == NULL))
        return RETURN_ERR_ALLOC;
        
    memcpy(pol1->data.pltbl + pol1->data.nbPolygon, pol2->data.pltbl, pol2->data.nbPolygon * sizeof(POLYGON));
    memcpy(pol1->data.attbl + pol1->data.nbPolygon, pol2->data.attbl, pol2->data.nbPolygon * sizeof(ATTR));
    // correct polygon indices
// no check for double points
    for(i = 0; i < pol2->data.nbPolygon; i++) {
        for(j = 0; j < 4; j++) {
            (pol1->data.pltbl + pol1->data.nbPolygon + i)->Vertices[j] += pol1->data.nbPoint;
        }
    }
    pol1->data.nbPoint += pol2->data.nbPoint;
    pol1->data.nbPolygon += pol2->data.nbPolygon;
#endif
    
    return RETURN_OK;
}


// add a new object
// or merge to an object of same type(PDATA or XPDATA)
// (alloc & copy)
ReturnValue MAP_copyToTile(TileData *tile, PolygonData *pol, unsigned char merge_in_tile)
{
    ReturnValue ret;
    PolygonData *obj;
    
    if((tile == NULL) || (pol == NULL))
        return RETURN_ERR_NULLARG;
        
    if(merge_in_tile > 1)
        return RETURN_ERR_INVALIDARG;
    
    if(merge_in_tile == 0) {
        // append this object to tile
        if(tile->nObj == 0)
            obj = (PolygonData *)malloc(sizeof(PolygonData));
        else obj = (PolygonData *)realloc((void *)(tile->obj), (tile->nObj + 1) * sizeof(PolygonData));
        if(obj == NULL)
            return RETURN_ERR_ALLOC;
        
        // copy (allocated)
        memcpy((obj + tile->nObj), pol, sizeof(PolygonData));
        // copy POINT table
        (obj + tile->nObj)->data.pntbl = (POINT *)malloc(pol->data.nbPoint * sizeof(POINT));
        if((obj + tile->nObj)->data.pntbl == NULL)
            return RETURN_ERR_ALLOC;
        memcpy((obj + tile->nObj)->data.pntbl, pol->data.pntbl, pol->data.nbPoint * sizeof(POINT));
        // copy POLYGON table
        (obj + tile->nObj)->data.pltbl = (POLYGON *)malloc(pol->data.nbPolygon * sizeof(POLYGON));
        if((obj + tile->nObj)->data.pltbl == NULL)
            return RETURN_ERR_ALLOC;
        memcpy((obj + tile->nObj)->data.pltbl, pol->data.pltbl, pol->data.nbPolygon * sizeof(POLYGON));
        // copy ATTR table
        (obj + tile->nObj)->data.attbl = (ATTR *)malloc(pol->data.nbPolygon * sizeof(ATTR));
        if((obj + tile->nObj)->data.attbl == NULL)
            return RETURN_ERR_ALLOC;
        memcpy((obj + tile->nObj)->data.attbl, pol->data.attbl, pol->data.nbPolygon * sizeof(ATTR));
        if(obj->type == IS_XPDATA) {
            // copy VECTOR table
            (obj + tile->nObj)->data.vntbl = (VECTOR *)malloc(pol->data.nbPoint * sizeof(VECTOR));
            if((obj + tile->nObj)->data.vntbl == NULL)
                return RETURN_ERR_ALLOC;
            memcpy((obj + tile->nObj)->data.vntbl, pol->data.vntbl, pol->data.nbPoint * sizeof(VECTOR));
        }
        tile->obj = obj;
        tile->nObj += 1;
    } else if(merge_in_tile == 1) {
        // merge all PDATA to one object
        // and all XPDATA to another
            // need 2 objects at most
        if(tile->nObj == 0) {
            tile->obj = (PolygonData *)calloc(1, sizeof(PolygonData));
            if(tile->obj == NULL)  
                return RETURN_ERR_ALLOC;

            tile->nObj = 1;
            (tile->obj + 0)->type = pol->type;
            return MAP_copyToObj(tile->obj + 0, pol);        
        } else {
            int i;
            // binary search untill type is found
            for(i = 0; i < tile->nObj; i++) {
                if((tile->obj + i)->type == pol->type)
                    return MAP_copyToObj(tile->obj + i, pol);
            }
            // still not found, append new
            tile->obj = (PolygonData *)realloc(tile->obj, (tile->nObj + 1) * sizeof(PolygonData));
            if(tile->obj == NULL)  
                return RETURN_ERR_ALLOC;
            (tile->obj + tile->nObj)->type = pol->type;
            ret = MAP_copyToObj(tile->obj + tile->nObj, pol);
            tile->nObj++;
            return ret;
        }
        return ret;
    }
    
    return RETURN_OK;
}


ReturnValue MAP_insertInMap(MapData *map, PolygonData *pol, MapingType mapingType, unsigned char break_over_tiles, unsigned char merge_in_tile, VerifyData *verify)
{
    ReturnValue ret = RETURN_OK;
    FIXED pos[XYZ];
    unsigned short mapX, mapZ;
    
    if((map == NULL) || (pol == NULL))
        return RETURN_ERR_NULLARG;
        
    if((break_over_tiles > 1) || (merge_in_tile > 1))
        return RETURN_ERR_INVALIDARG;
        
    // handle empty objects
    // although it should be handled later, too
    // (but there it might return an error)
    if((pol->data.nbPoint == 0) || (pol->data.nbPolygon == 0))
        return RETURN_OK;
    
    // collect verify data ?
    if(verify != NULL) {
        // append / merge this object with our verify object
        int i;
        
        verify->totalPoints += pol->data.nbPoint;
        verify->totalPolygons += pol->data.nbPolygon;
        for(i = 0; i < pol->data.nbPolygon; i++) {
            ret = POL_copyPolygon(&(verify->pol.data), &(pol->data), IS_PDATA, NULL, i);
            if(ret != RETURN_OK)
                error_printf("Error collecting verify data!\n");
        }
    }
    
    // don't break the object apart?
    if(break_over_tiles == 0) {
        // find reference point to map
        if(mapingType == MAPING_TYPE_MIDDLE) {
            ret = POL_findObjectMid(&(pol->data), pos, XYZ);
        } else if(mapingType == MAPING_TYPE_MEAN) {
            ret = POL_findObjectMean(&(pol->data), pos, XYZ);        
        } else {
            warning_printf("Mapping type %i not supported!\nTaking object mean point per default.\n", mapingType);
            ret = POL_findObjectMean(&(pol->data), pos, XYZ);
        }
        if(ret != RETURN_OK)
            return ret;
        // find the tile it's in
        ret = MAP_mapToGrid(map, pos, &mapX, &mapZ);
        if(ret != RETURN_OK)
            return ret;
        // tile allocated ?
        if(*(map->tiles + mapZ*map->mapSize.x + mapX) == NULL) {
            *(map->tiles + mapZ*map->mapSize.x + mapX) = (TileData *)calloc(1, sizeof(TileData));
            if(*(map->tiles + mapZ*map->mapSize.x + mapX) == NULL)
                return RETURN_ERR_ALLOC;
        }

        return MAP_copyToTile(*(map->tiles + mapZ*map->mapSize.x + mapX), pol, merge_in_tile);
        
    }else if(break_over_tiles == 1) {
        int i;
        // tree stuff to create new objects
        ObjectEntry oe, *oe_ptr = NULL, *oe_array = NULL;
        int oe_count = 0, oe_new;
        // split to multiple objects
            // asign each polygon to a tile
        for(i = 0; i < pol->data.nbPolygon; i++) {
            if(mapingType == MAPING_TYPE_MIDDLE) {
                ret = POL_findPolygonMid(&(pol->data), i, pos, XYZ);
            } else if(mapingType == MAPING_TYPE_MEAN) {
                ret = POL_findPolygonMean(&(pol->data), i, pos, XYZ);        
            }
            if(ret != RETURN_OK)
                return ret;
            // find the tile it's in
            ret = MAP_mapToGrid(map, pos, &mapX, &mapZ);
            if(ret != RETURN_OK)
                return ret;
            // assign polygon to tile
                // insert into object array
            oe.tile_offset = mapZ*map->mapSize.x + mapX;
                // is there already a polygon in the same tile?
            oe_new = 0;
            if(oe_count > 0) {
                oe_ptr = (ObjectEntry *)bsearch(&oe, oe_array, oe_count, sizeof(ObjectEntry), compare_ObjectEntry);
                // found a match?
                if(oe_ptr == NULL) {
                    // append new entry
                    oe_ptr = (ObjectEntry *)realloc(oe_array, (oe_count + 1) * sizeof(ObjectEntry));
                    oe_new = 1;
                }
            } else {
                oe_ptr = (ObjectEntry *)malloc(sizeof(ObjectEntry));
                oe_new = 1;
            }
            if(oe_new == 1) {	// oe_ptr points to new ObjectEntry
                if(oe_ptr == NULL)
                    return RETURN_ERR_ALLOC;
                oe_array = oe_ptr;
                // init
                oe_ptr = oe_array + oe_count;
                memset(oe_ptr, 0, sizeof(ObjectEntry));
                oe_ptr->tile_offset = oe.tile_offset;
                oe_ptr->pol.type = pol->type;
                oe_count += 1;
                // sort and find again
                qsort(oe_array, oe_count, sizeof(ObjectEntry), compare_ObjectEntry);
                oe_ptr = (ObjectEntry *)bsearch(&oe, oe_array, oe_count, sizeof(ObjectEntry), compare_ObjectEntry);
                // found a match?
                if(oe_ptr == NULL)
                    return RETURN_ERR_UNKNOWN;

            }
                // insert polygon to oe_ptr ObjectEntry position
            ret = POL_copyPolygon(&(oe_ptr->pol.data), &(pol->data), pol->type, &(oe_ptr->pe_array), i);
            if(ret != RETURN_OK)
                return ret;
        }
        // and insert the new objects into the map
        for(i = 0; i < oe_count; i++) {
            // check to allocate tile
            // although each ObjectEntry is in a different tile
            // WE NEED TO CHECK
            // because the tile could already been allocated by
            // a previous call of MAP_insertInMap()
            if(*(map->tiles + (oe_array + i)->tile_offset) == NULL) {
                *(map->tiles + (oe_array + i)->tile_offset) = (TileData *)calloc(1, sizeof(TileData));
                if(*(map->tiles + (oe_array + i)->tile_offset) == NULL)
                    return RETURN_ERR_ALLOC;
            }
            ret = MAP_copyToTile(*(map->tiles + (oe_array + i)->tile_offset), &((oe_array + i)->pol), merge_in_tile);
            if(ret != RETURN_OK)
                return ret;
        }
        // and delete all allocated data
        for(i = 0; i < oe_count; i++) {
            MAP_clearObjectEntry(oe_array + i);
        }
        // the ObjectEntry array
        if(oe_count > 0)
            free(oe_array);
    }
    
    return RETURN_OK;
}


// encapsulates usual PDATA and XPDATA
ReturnValue MAP_insertInMap2(MapData *map, void *xpdata, unsigned char type, MapingType mapingType, unsigned char break_over_tiles, unsigned char merge_in_tile, VerifyData *verify)
{
    PolygonData pol;

    if((map == NULL) || (xpdata == NULL))
        return RETURN_ERR_NULLARG;

    if((type != IS_PDATA) && (type != IS_XPDATA))
        return RETURN_ERR_INVALIDARG;
        
    // need to encapsulate
    pol.type = type;
    memcpy(&pol.data, xpdata, type == IS_PDATA ? sizeof(PDATA) : sizeof(XPDATA));
    return MAP_insertInMap(map, &pol, mapingType, break_over_tiles, merge_in_tile, verify);
}


// converts an PDATA or XPDATA array
// like PDATA *map_data[]
ReturnValue MAP_insertInMap3(MapData *map, void **xpdata, unsigned int n, unsigned char type, MapingType mapingType, unsigned char break_over_tiles, unsigned char merge_in_tile, VerifyData *verify)
{
    int i;
    ReturnValue ret;
    
    if((map == NULL) || (xpdata == NULL))
        return RETURN_ERR_NULLARG;

    if((type != IS_PDATA) && (type != IS_XPDATA))
        return RETURN_ERR_INVALIDARG;
        
    for(i = 0; i < n; i++) {
        ret = MAP_insertInMap2(map, type == IS_PDATA ? ((void *)*(((PDATA **)xpdata) + i)) : ((void *)*(((XPDATA **)xpdata) + i)), type, mapingType, break_over_tiles, merge_in_tile, verify);
        if(ret != RETURN_OK) {
            error_printf("Error inserting object %i!\n", i);
//            return ret;
        }
    }
    
    return RETURN_OK;
}



ReturnValue MAP_verify(MapData *map, VerifyData *verify)
{
    int i, j, k, l;
    ReturnValue ret;
    FIXED min1[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED min2[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED max1[XYZ] = {0x80000000, 0x80000000, 0x80000000};
    FIXED max2[XYZ] = {0x80000000, 0x80000000, 0x80000000};
    int totalMapPoints = 0, totalMapPolygons = 0, missingPoints = 0, missingPolygons = 0, unknownPoints = 0, unknownPolygons = 0;
    unsigned char dirtyPoints[verify->pol.data.nbPoint], dirtyPolygons[verify->pol.data.nbPolygon], match;

    if((map == NULL) || (verify == NULL))
        return RETURN_ERR_NULLARG;

    if((verify->pol.data.nbPoint == 0) || (verify->pol.data.nbPolygon == 0)) {
        note_printf("Verify data is empty, nothing to verify with!\n");
        return RETURN_OK;
    }
    
    note_printf("Verifying map...\n");
    // init dirty table: no point compared
    memset(dirtyPoints, 0, verify->pol.data.nbPoint);
    memset(dirtyPolygons, 0, verify->pol.data.nbPolygon);
    
    // find min max bounds
    for(i = 0; i < (map->mapSize.x * map->mapSize.z); i++) {
        if(*(map->tiles + i) == NULL)
            continue;
        for(j = 0; j < (*(map->tiles + i))->nObj; j++) {
            // skip empty objects
            if((((*(map->tiles + i))->obj + j)->data.nbPoint == 0) || (((*(map->tiles + i))->obj + j)->data.nbPolygon == 0))
                continue;
                
            // count stats
            totalMapPoints += ((*(map->tiles + i))->obj + j)->data.nbPoint;
            totalMapPolygons += ((*(map->tiles + i))->obj + j)->data.nbPolygon;
            // lookup all points
            for(k = 0; k < ((*(map->tiles + i))->obj + j)->data.nbPoint; k++) {
                match = 0;
                // search all verify points for a match
                for(l = 0; l < verify->pol.data.nbPoint; l++) {
                    match = POL_comparePoints(*(verify->pol.data.pntbl + l), *(((*(map->tiles + i))->obj + j)->data.pntbl + k), XYZ);
                    if(match == 1) {
                        dirtyPoints[l] += 1;
                        break;
                    }
                }
                if(match != 1) {
                    error_printf("Error: point %i of object %i in tile %i not found in original!\n", k, j, i);
                    unknownPoints++;
                }
                
            }
            // lookup all polygons
            for(k = 0; k < ((*(map->tiles + i))->obj + j)->data.nbPolygon; k++) {
                match = 0;
                // search all verify polygons for a match
                for(l = 0; l < verify->pol.data.nbPolygon; l++) {
//                    if(l >= 72) {
//                        match = 20;
//                    }
                    match = POL_comparePolygons(&(((*(map->tiles + i))->obj + j)->data), k, &verify->pol.data, l);
                    if(match == 1) {
                        dirtyPolygons[l] += 1;
                        break;
                    }
                }
                if(match != 1) {
                    error_printf("Error: polygon %i of object %i in tile %i not found in original!\n", k, j, i);
                    unknownPolygons++;
                }
                
            }
            ret = POL_updateMinMax(((*(map->tiles + i))->obj + j)->data.pntbl, ((*(map->tiles + i))->obj + j)->data.nbPoint, min1, max1, XYZ);
            if(ret != RETURN_OK) {
                error_printf("Error finding minmax!\n");
            }
        }
    }
    ret = POL_updateMinMax(verify->pol.data.pntbl, verify->pol.data.nbPoint, min2, max2, XYZ);
    if(ret != RETURN_OK) {
        error_printf("Error finding minmax!\n");
    }
    if((min1[X] != min2[X]) || (min1[Y] != min2[Y]) || (min1[Z] != min2[Z]) || (max1[X] != max2[X]) || (max1[Y] != max2[Y]) || (max1[Z] != max2[Z])) {
            warning_printf("Difference: maps got different min and max point locations!\n");
            warning_printf("map: min(%f, %f, %f), max(%f, %f, %f)!\n", toFLOAT(min1[X]), toFLOAT(min1[Y]), toFLOAT(min1[Z]), toFLOAT(max1[X]), toFLOAT(max1[Y]), toFLOAT(max1[Z]));
            warning_printf("original: min(%f, %f, %f), max(%f, %f, %f)!\n", toFLOAT(min2[X]), toFLOAT(min2[Y]), toFLOAT(min2[Z]), toFLOAT(max2[X]), toFLOAT(max2[Y]), toFLOAT(max2[Z]));
    }
    // print some stats
    note_printf("total Points: %i(original) %li(no duplicates) %i(map)\n", verify->totalPoints, verify->pol.data.nbPoint, totalMapPoints);
    note_printf("total Polygons: %i(original) %li(no duplicates) %i(map)\n", verify->totalPolygons, verify->pol.data.nbPolygon, totalMapPolygons);
    
    // look for multiple defined points and undefined ones
    for(i = 0; i < verify->pol.data.nbPoint; i++) {
        if(dirtyPoints[i] > 1)
            note_printf("Note: point %i defined %i times!\n", i, dirtyPoints[i]);
        if(dirtyPoints[i] == 0) {
            error_printf("Error: point %i missing in map!\n", i);
            missingPoints++;
        }
    }
    if(missingPoints > 0)
        error_printf("Error: %i points missing in map!\n", missingPoints);
    if(unknownPoints > 0)
        error_printf("Error: %i unknown points in map!\n", unknownPoints);
        
    // look for multiple defined polygons and undefined ones
    for(i = 0; i < verify->pol.data.nbPolygon; i++) {
        if(dirtyPolygons[i] > 1)
            note_printf("Note: polygon %i defined %i times!\n", i, dirtyPolygons[i]);
        if(dirtyPolygons[i] == 0) {
            error_printf("Error: polygon %i missing in map!\n", i);
            missingPolygons++;
        }
    }
    if(missingPolygons > 0)
        error_printf("Error: %i polygons missing in map!\n", missingPolygons);
    if(unknownPolygons > 0)
        error_printf("Error: %i unknown polygons in map!\n", unknownPolygons);
        
    note_printf("finished!\n");
    
    return ret;
}

typedef struct SIDE_REF {
    GroundData *ground;
    // that's the polygon it's made from
    POINT   *pntbl;
    POLYGON *pltbl;
    Uint32 index;
} SideRef;

ReturnValue MAP_computeGround(MapData *map, CollisionTypes *collisionTypes)
{
    int x, z, i, j, match;
    Uint16 start, end;
    TileData *tile;
    GroundData *ground = NULL;
    ReturnValue ret;
    // if I describe one collision type with multiple collisionTypes search constraints
    // e.g. there exist collisionTypes with same typeID,
    // then the first matching collisionType gets it
    unsigned char *dirty = NULL;
    // for side optimization:
    // set up a list of side and ground data
    SideRef *sides = NULL;
    int nSides = 0;
    GroundData **grounds = NULL;
    int nGrounds = 0;
    
    if((map == NULL) || (collisionTypes == NULL))
        return RETURN_OK;
    
    // process all tiles
    for(z = 0; z < map->mapSize.z; z++) {
        for(x = 0; x < map->mapSize.x; x++) {
            tile = *(map->tiles + z*map->mapSize.x + x);
            if(tile == NULL)
                continue;
            // search all objects of a tile
            for(i = 0; i < tile->nObj; i++) {
                // init dirty table for that object
                dirty = (unsigned char *)calloc((tile->obj + i)->data.nbPolygon, sizeof(unsigned char));
                if(dirty == NULL) {
                    return RETURN_ERR_ALLOC;
                }
                // search for every collision type
                for(j = 0; j < collisionTypes->n; j++) {
                    start = 0;
                    end = (tile->obj + i)->data.nbPolygon - 1;
                    do {
                        match = POL_searchPolygon(&((tile->obj + i)->data), (tile->obj + i)->type, start, end, &((collisionTypes->collisionType + j)->search));
                        if(match < 0)
                            break;
                        
                        // only make ground if not used as ground before
                        if(*(dirty + match) == 0) {
                            *(dirty + match) = 1;
                            // insert the ground data of that typeID
                                // get memory: append
                            if(tile->nGround == 0)
                                ground = (GroundData *)malloc(sizeof(GroundData));
                            else ground = (GroundData *)realloc(tile->ground, (tile->nGround + 1) * sizeof(GroundData)); 
                            if(ground == NULL)
                                return RETURN_ERR_ALLOC;
                                // insert data
                            ret = POL_computeGround(ground + tile->nGround, &((tile->obj + i)->data), (tile->obj + i)->type, match, (collisionTypes->collisionType + j)->typeID);
                            if(ret != RETURN_OK)
                                return ret;
                                // collect side / ground info
                            if((collisionTypes->collisionType + j)->isBorder == 1) {
                                // get memory
                                if(nSides == 0)
                                    sides = (SideRef *)malloc(sizeof(SideRef));
                                else sides = (SideRef *)realloc(sides, (nSides + 1) * sizeof(SideRef));
                                if(sides == NULL)
                                    return RETURN_ERR_ALLOC;
                                // insert reference
                                (sides + nSides)->ground = ground + tile->nGround;
                                (sides + nSides)->pntbl = (tile->obj + i)->data.pntbl;
                                (sides + nSides)->pltbl = (tile->obj + i)->data.pltbl;
                                (sides + nSides)->index = match;
                                // finish
                                nSides++;
                            } else {
                                // get memory
                                if(nGrounds == 0)
                                    grounds = (GroundData **)malloc(sizeof(GroundData *));
                                else grounds = (GroundData **)realloc(grounds, (nGrounds + 1) * sizeof(GroundData *));
                                if(grounds == NULL)
                                    return RETURN_ERR_ALLOC;
                                // insert reference
                                *(grounds + nGrounds) = ground + tile->nGround;
                                // finish
                                nGrounds++;
                            }
                                // finish insert
                            tile->ground = ground;
                            tile->nGround++;
                        }    
                        // prepare to continue search
                        start = match+1;
                    } while(start <= end);
                }
                free(dirty);
            }
        }
    }
    // postprocess the side borders
    note_printf("Now correcting %i sides to %i grounds...\n", nSides, nGrounds);
    for(i = 0; i < nSides; i++) {
        FIXED li_min;
        FIXED li, sx, sz;
        POINT *p0 = NULL, *p1 = NULL;
        double dpx, dpz, dcx, dcz, divident, divisor;
        int firstP;
        // find nearest ground
        li_min = 0x7FFF0000;
        ground = NULL;
        for(j = 0; j < nGrounds; j++) {
            sx = (*(grounds + j))->center[X] - (sides + i)->ground->center[X];
            sz = (*(grounds + j))->center[Z] - (sides + i)->ground->center[Z];
            li = slMulFX(sx, sx) + slMulFX(sz, sz);
            if(li < li_min) {
                li_min = li;
                ground = *(grounds + j);
            }
        }
        // determine necessary shifting of the side center
            // find side between the centers
#if 0
// this is not always correct
// better use intersection test
        firstP = 0;
                // find nearest point
        li_min = 0x7FFF0000;
        for(j = 0; j < 4; j++) {
            sx = ground->center[X] - (*((sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j]))[X];
            sz = ground->center[Z] - (*((sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j]))[Z];
            li = slMulFX(sx, sx) + slMulFX(sz, sz);
            if(li < li_min) {
                li_min = li;
                firstP = j;
                p0 = (sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j];
            }
        }
            // find second nearest point
        li_min = 0x7FFF0000;
        for(j = 0; j < 4; j++) {
            sx = ground->center[X] - (*((sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j]))[X];
            sz = ground->center[Z] - (*((sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j]))[Z];
            li = slMulFX(sx, sx) + slMulFX(sz, sz);
            if((j != firstP) && (li < li_min)) {
                li_min = li;
                p1 = (sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j];
            }
        }    
#else
        // check all 4 sides if the line segment intersects the center <-> center segment
            // assume that only one intersection exists
        match = 0;
        p0 = (sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[3];
        for(j = 0; j < 3; j++) {
            p1 = (sides + i)->pntbl + ((sides + i)->pltbl + (sides + i)->index)->Vertices[j];
            // Paul Bourke:
            // ua = (x4 - x3)(y1 - y3) - (y4 - y3)(x1 - x3) / (y4 - y3)(x2 - x1) - (x4 - x3)(y2 - y1)
            // P1 is side center
            // P2 is ground center
            // P3 is p0 of side edge
            // P4 is p1 of side edge
                // divident
            sx = slMulFX((*p1)[X] - (*p0)[X], (sides + i)->ground->center[Z] - (*p0)[Z]) - slMulFX((*p1)[Z] - (*p0)[Z], (sides + i)->ground->center[X] - (*p0)[X]);
                // divisor
            sz = slMulFX((*p1)[Z] - (*p0)[Z], ground->center[X] - (sides + i)->ground->center[X]) - slMulFX((*p1)[X] - (*p0)[X], ground->center[Z] - (sides + i)->ground->center[Z]);
            // intersection ?
            if((sx < 0 && sz < 0 && sz <= sx) || (sx > 0 && sz > 0 && sz >= sx)) {
                match = 1;
                break;
            }
            // advance the edge
            p0 = p1;
        
        }
        if(match != 1) {
            error_printf("ERROR: couldn't find border line!\n");
            note_printf("Correction aborted for side %i!\n", i);
        } else
#endif
        {
            note_printf("c0(%f, %f) c1(%f, %f)\n", toFLOAT(ground->center[X]), toFLOAT(ground->center[Z]), toFLOAT((sides + i)->ground->center[X]), toFLOAT((sides + i)->ground->center[Z]));
            note_printf("p0(%f, %f) p1(%f, %f)\n", toFLOAT((*p0)[X]), toFLOAT((*p0)[Z]), toFLOAT((*p1)[X]), toFLOAT((*p1)[Z]));
            // compute shift distance
            dpx = toFLOAT((*p1)[X] - (*p0)[X]);
            dpz = toFLOAT((*p1)[Z] - (*p0)[Z]);
            dcx = toFLOAT((sides + i)->ground->center[X] - ground->center[X]);
            dcz = toFLOAT((sides + i)->ground->center[Z] - ground->center[Z]);
            divisor = dcx * dpz - dcz * dpx;
            if(divisor != 0.0) {
                divident = dpx * toFLOAT(ground->center[Z] - (*p0)[Z]) + dpz * toFLOAT((*p0)[X] - ground->center[X]);
                dcx = 2*dcx*divident/divisor;
                dcz = 2*dcz*divident/divisor;
// ERROR: doing this assignment gives a malloc error later(MacOS X)
// *** malloc[1114]: error for object 0xa79f0:
// Incorrect checksum for freed object - object was probably modified after being freed;
// break at szone_error
                (sides + i)->ground->center[X] = ground->center[X] + toFIXED(dcx);
                (sides + i)->ground->center[Z] = ground->center[Z] + toFIXED(dcz);
                // mid point
                dcx = toFLOAT(ground->center[X]) + dcx/2.0;
                dcz = toFLOAT(ground->center[Z]) + dcz/2.0;
                note_printf("mid(%f, %f)\n", dcx, dcz);
                note_printf("new c1(%f, %f)\n", toFLOAT((sides + i)->ground->center[X]), toFLOAT((sides + i)->ground->center[Z]));
                note_printf("|c0 - m| = %f\n", (dcx - toFLOAT(ground->center[X]))*(dcx - toFLOAT(ground->center[X])) + (dcz - toFLOAT(ground->center[Z]))*(dcz - toFLOAT(ground->center[Z])));
                note_printf("|c1 - m| = %f\n", (dcx - toFLOAT((sides + i)->ground->center[X]))*(dcx - toFLOAT((sides + i)->ground->center[X])) + (dcz - toFLOAT((sides + i)->ground->center[Z]))*(dcz - toFLOAT((sides + i)->ground->center[Z])));
            }    
        }
        
    }
  
    // free list data
    if(nSides > 0)
        free(sides);
        
    if(nGrounds > 0)
        free(grounds);
    
    return RETURN_OK;
}


