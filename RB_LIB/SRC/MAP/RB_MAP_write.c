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
#include "RB_POL_write.h"
#include "RB_MAP_write.h"


void MAP_swapPolygonData(PolygonData *pol)
{
    if(pol == NULL)
        return;
        
    pol->data.pntbl = (POINT *)SWAP4((unsigned int)pol->data.pntbl);
    pol->data.nbPoint = SWAP4(pol->data.nbPoint);
    pol->data.pltbl = (POLYGON *)SWAP4((unsigned int)pol->data.pltbl);
    pol->data.nbPolygon = SWAP4(pol->data.nbPolygon);
    pol->data.attbl = (ATTR *)SWAP4((unsigned int)pol->data.attbl);
    if(pol->type == IS_XPDATA)
        pol->data.vntbl = (VECTOR *)SWAP4((unsigned int)pol->data.vntbl);
}

void MAP_swapTileData(TileData *tile)
{
    if(tile == NULL)
        return;

    tile->obj = (PolygonData *)SWAP4((unsigned int)tile->obj);
    tile->ground = (GroundData *)SWAP4((unsigned int)tile->ground);
}


void MAP_swapMapData(MapData *map)
{
    if(map == NULL)
        return;
        
    map->mapSize.x = SWAP2(map->mapSize.x);
    map->mapSize.z = SWAP2(map->mapSize.z);
//    map->tileSize.x = SWAP4(map->tileSize.x);
//    map->tileSize.z = SWAP4(map->tileSize.z);
    map->mapOrigin.x = SWAP4(map->mapOrigin.x);
    map->mapOrigin.z = SWAP4(map->mapOrigin.z);
    map->tiles = (TileData **)SWAP4((unsigned int)map->tiles);
}



ReturnValue MAP_writePolygonDataArray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, PolygonData *data, unsigned int n)
{
    ReturnValue ret;
    int j;
    PolygonData *data_ptr;
    unsigned int startPNTBL, startPLTBL, startATTBL, startVNTBL, endLinked;

    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, n*sizeof(PolygonData), 4);
    if(ret != RETURN_OK)
        return ret;

    // copy data
    memcpy(*buf + *startOffset, data, n*sizeof(PolygonData));

    // write linked data
    // for every element
    endLinked = *startOffset + n*sizeof(PolygonData);
    for(j = 0; j < n; j++) {
            // pntbl
        startPNTBL = endLinked;
        ret = POL_writeFIXEDarray(buf, bufSize, &startPNTBL, &endLinked, (FIXED *)(data + j)->data.pntbl, (data + j)->data.nbPoint * (sizeof(POINT) / sizeof(FIXED)));
        if(ret != RETURN_OK)
            return ret;
            // pltbl
        startPLTBL = endLinked;
        ret = POL_writePOLYGONarray(buf, bufSize, &startPLTBL, &endLinked, (data + j)->data.pltbl, (data + j)->data.nbPolygon);
        if(ret != RETURN_OK)
            return ret;
            // attbl
        startATTBL = endLinked;
        ret = POL_writeATTRarray(buf, bufSize, &startATTBL, &endLinked, (data + j)->data.attbl, (data + j)->data.nbPolygon);
        if(ret != RETURN_OK)
            return ret;
            // vntbl
        if((data + j)->type == IS_XPDATA) {
            startVNTBL = endLinked;
            ret = POL_writeFIXEDarray(buf, bufSize, &startVNTBL, &endLinked, (FIXED *)(data + j)->data.vntbl, (data + j)->data.nbPoint * (sizeof(VECTOR) / sizeof(FIXED)));
            if(ret != RETURN_OK)
                return ret;
        }
        // adapt data
        data_ptr = (PolygonData *)(*buf + *startOffset + j*sizeof(PolygonData));
        data_ptr->data.pntbl = (POINT *)startPNTBL;
        data_ptr->data.pltbl = (POLYGON *)startPLTBL;
        data_ptr->data.attbl = (ATTR *)startATTBL;
        if(data_ptr->type == IS_XPDATA)
            data_ptr->data.vntbl = (VECTOR *)startVNTBL;
#ifdef SWAP_BYTES
        MAP_swapPolygonData(data_ptr);
#endif
    }
    
    // set end offset
    *endOffset = endLinked;
    
    return RETURN_OK;
}


ReturnValue MAP_writeTileData(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, TileData *data)
{
    ReturnValue ret;
    TileData *data_ptr;
    unsigned int startObj, startGround, endLinked;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, sizeof(TileData), 4);
    if(ret != RETURN_OK)
        return ret;
        
    // copy data
    memcpy(*buf + *startOffset, data, sizeof(TileData));
    
    // write linked data
    endLinked = *startOffset + sizeof(TileData);
        // obj
    if(data->nObj > 0) {
        startObj = endLinked;
        ret = MAP_writePolygonDataArray(buf, bufSize, &startObj, &endLinked, data->obj, data->nObj);
        if(ret != RETURN_OK)
            return ret;
    }
        // ground
    if(data->nGround > 0) {
        startGround = endLinked;
        ret = POL_writeGroundDataArray(buf, bufSize, &startGround, &endLinked, data->ground, data->nGround);
        if(ret != RETURN_OK)
            return ret;
    }

    // adapt data
    data_ptr = (TileData *)(*buf + *startOffset);
    data_ptr->obj = (PolygonData *)startObj;
    data_ptr->ground = (GroundData *)startGround;
#ifdef SWAP_BYTES    
    MAP_swapTileData(data_ptr);
#endif
    
    // set end offset
    *endOffset = endLinked;
    
    return RETURN_OK;
}


ReturnValue MAP_writeTileDataPointersArray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, TileData **data, unsigned int n)
{
    int i;
    ReturnValue ret;
    TileData **data_ptr;
    unsigned int startTile, endLinked;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, n*sizeof(TileData **), 4);
    if(ret != RETURN_OK)
        return ret;
        
    // copy data
    
    // for every element
    endLinked = *startOffset + n*sizeof(TileData **);
    for(i = 0; i < n; i++) {
        // write linked data
        if(*(data + i) != NULL) {
            startTile = endLinked;
            ret = MAP_writeTileData(buf, bufSize, &startTile, &endLinked, *(data + i));
            if(ret != RETURN_OK)
                return ret;
        } else startTile = 0;
        // adapt data
        data_ptr = (TileData **)(*buf + *startOffset);
        *(data_ptr + i) = (TileData *)SWAP4(startTile);
    }
    
    // set end offset
    *endOffset = endLinked;
    
    return RETURN_OK;
}


ReturnValue MAP_writeMapData(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, MapData *data)
{
    ReturnValue ret;
    unsigned int startLinked, endLinked;
    MapData *data_ptr;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, sizeof(MapData), 4);
    if(ret != RETURN_OK)
        return ret;

    // copy data
    memcpy(*buf + *startOffset, data, sizeof(MapData));

    // write linked data
    startLinked = *startOffset + sizeof(MapData);
	ret = MAP_writeTileDataPointersArray(buf, bufSize, &startLinked, &endLinked, data->tiles, data->mapSize.x * data->mapSize.z);
    if(ret != RETURN_OK)
        return ret;
        
    // adapt data
    data_ptr = (MapData *)(*buf + *startOffset);
    data_ptr->tiles = (TileData **)startLinked;
#ifdef SWAP_BYTES    
    MAP_swapMapData((MapData *)(*buf + *startOffset));
#endif    
    // set end offset
    *endOffset = endLinked;
    
    return RETURN_OK;
}

ReturnValue MAP_write(MapData *map, char *filename)
{
    unsigned char *buf = NULL;
    unsigned int bufSize = 0, startOffset = 0, endOffset = 0;
    FILE *file;
    ReturnValue ret;
    size_t bytesWritten;
    
    if((map == NULL) || (filename == NULL))
        return RETURN_ERR_NULLARG;

    // write MapData to buffer
    ret = MAP_writeMapData(&buf, &bufSize, &startOffset, &endOffset, map);
    if(ret != RETURN_OK)
        return ret;

    // write buffer to file
    file = fopen(filename, "w");
    if(file == NULL) {
        error_printf("Error opening file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    
    bytesWritten = fwrite(buf, bufSize, 1, file);
    if(bytesWritten < 0) {
        error_printf("Error writing file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    
    if(fclose(file) != 0) {
        error_printf("Error closing file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    
    note_printf("Size of binary map file is %i bytes.\n", bufSize);
    return RETURN_OK;
}


// allocates a copy
ReturnValue MAP_getWriteable(MapData **target, MapData *source)
{
    int i, j, size;
    void *ptr;
    
    if((source == NULL))
        return RETURN_ERR_NULLARG;
        
    // duplicate MapData
    ptr = malloc(sizeof(MapData));
    if(ptr == NULL)
        return RETURN_ERR_ALLOC;
    memcpy(ptr, source, sizeof(MapData));
    *target = (MapData *)ptr;    

    // duplicate all tiles
    ptr = malloc(source->mapSize.x * source->mapSize.z * sizeof(TileData *));
    if(ptr == NULL)
        return RETURN_ERR_ALLOC;
    (*target)->tiles = (TileData **)ptr;
    for(i = 0; i < source->mapSize.x*source->mapSize.z; i++) {
        // duplicate NULL TileData pointer
        if((*(source->tiles + i)) == NULL) {
            *((*target)->tiles + i) = NULL;
            continue;
        }

        // or duplicate TileData
        ptr = (TileData *)malloc(sizeof(TileData));
        if(ptr == NULL)
            return RETURN_ERR_ALLOC;
        memcpy(ptr, *(source->tiles + i), sizeof(TileData));
        *((*target)->tiles + i) = ptr;

        // duplicate GroundData
        ptr = malloc((*(source->tiles + i))->nGround * sizeof(GroundData));
        if(ptr == NULL)
            return RETURN_ERR_ALLOC;
        memcpy(ptr, (*(source->tiles + i))->ground, (*(source->tiles + i))->nGround * sizeof(GroundData));
        (*((*target)->tiles + i))->ground = (GroundData *)ptr;

        // duplicate PolygonData
        ptr = malloc((*(source->tiles + i))->nObj * sizeof(PolygonData));
        if(ptr == NULL)
            return RETURN_ERR_ALLOC;
        memcpy(ptr, (*(source->tiles + i))->obj, (*(source->tiles + i))->nObj * sizeof(PolygonData));
        (*((*target)->tiles + i))->obj = (PolygonData *)ptr;
        // duplicate all XPDATA
        for(j = 0; j < (*(source->tiles + i))->nObj; j++) {
            // duplicate pntbl
            size = ((*(source->tiles + i))->obj + j)->data.nbPoint * sizeof(POINT);
            ptr = malloc(size);
            if(ptr == NULL)
                return RETURN_ERR_ALLOC;
            memcpy(ptr, ((*(source->tiles + i))->obj + j)->data.pntbl, size);
            ((*((*target)->tiles + i))->obj + j)->data.pntbl = (POINT *)ptr;
            // duplicate vntbl
            if(((*(source->tiles + i))->obj + j)->type == IS_XPDATA) {
                size = ((*(source->tiles + i))->obj + j)->data.nbPoint * sizeof(VECTOR);
                ptr = malloc(size);
                if(ptr == NULL)
                    return RETURN_ERR_ALLOC;
                memcpy(ptr, ((*(source->tiles + i))->obj + j)->data.vntbl, size);
                ((*((*target)->tiles + i))->obj + j)->data.vntbl = (VECTOR *)ptr;
            }
            // duplicate pltbl
            size = ((*(source->tiles + i))->obj + j)->data.nbPolygon * sizeof(POLYGON);
            ptr = malloc(size);
            if(ptr == NULL)
                return RETURN_ERR_ALLOC;
            memcpy(ptr, ((*(source->tiles + i))->obj + j)->data.pltbl, size);
            ((*((*target)->tiles + i))->obj + j)->data.pltbl = (POLYGON *)ptr;
            // duplicate attbl
            size = ((*(source->tiles + i))->obj + j)->data.nbPolygon * sizeof(ATTR);
            ptr = malloc(size);
            if(ptr == NULL)
                return RETURN_ERR_ALLOC;
            memcpy(ptr, ((*(source->tiles + i))->obj + j)->data.attbl, size);
            ((*((*target)->tiles + i))->obj + j)->data.attbl = (ATTR *)ptr;
        }
    }
    
    return RETURN_OK;
}

ReturnValue MAP_swapSetPointers(MapData *map, unsigned int maxOffset)
{
    int i, j, k, l;
    PolygonData *pol;
    GroundData *ground;
    FIXED fixed; //*pntblX, *vntblX
    
    if(map == NULL)
        return RETURN_ERR_NULLARG;
        
#ifdef SWAP_BYTES
    MAP_swapMapData(map);
#endif
    if(((unsigned int)map->tiles) > maxOffset) {
        error_printf("Pointer corrupted!\n");
        return RETURN_ERR_OOB;
    }
    map->tiles = (TileData **)( ((unsigned char *)map) + ((unsigned int)map->tiles) );
    for(i = 0; i < map->mapSize.x * map->mapSize.z; i++) {
        // all tiles pointers
        if(*(map->tiles + i) != NULL) {
#ifdef SWAP_BYTES
            *(map->tiles + i) = (TileData *)SWAP4((unsigned int)*(map->tiles + i));
#endif
            if(((unsigned int)*(map->tiles + i)) > maxOffset) {
                error_printf("Pointer corrupted!\n");
                return RETURN_ERR_OOB;
            }

            *(map->tiles + i) = (TileData *)( ((unsigned char *)map) + ((unsigned int)*(map->tiles + i)));
#ifdef SWAP_BYTES
            MAP_swapTileData(*(map->tiles + i));
#endif
            // obj pointer
            if(((unsigned int)(*(map->tiles + i))->obj) > maxOffset) {
                error_printf("Pointer corrupted!\n");
                return RETURN_ERR_OOB;
            }
            (*(map->tiles + i))->obj = (PolygonData *)( ((unsigned char *)map) + ((unsigned int)(*(map->tiles + i))->obj));
            // ground pointer
            if(((unsigned int)(*(map->tiles + i))->ground) > maxOffset) {
                error_printf("Pointer corrupted!\n");
                return RETURN_ERR_OOB;
            }
            (*(map->tiles + i))->ground = (GroundData *)( ((unsigned char *)map) + ((unsigned int)(*(map->tiles + i))->ground));
            // all pointers inside objects 
            pol = (*(map->tiles + i))->obj;
            for(j = 0; j < (*(map->tiles + i))->nObj; j++) {
#ifdef SWAP_BYTES
                MAP_swapPolygonData(pol + j);
#endif
                if((((unsigned int)(pol + j)->data.pntbl) > maxOffset) || (((unsigned int)(pol + j)->data.pltbl) > maxOffset) || (((unsigned int)(pol + j)->data.attbl) > maxOffset)) {
                    error_printf("Pointer corrupted!\n");
                    return RETURN_ERR_OOB;
                }
                (pol + j)->data.pntbl = (POINT *)( ((unsigned char *)map) + ((unsigned int)(pol + j)->data.pntbl));
                (pol + j)->data.pltbl = (POLYGON *)( ((unsigned char *)map) + ((unsigned int)(pol + j)->data.pltbl));
                (pol + j)->data.attbl = (ATTR *)( ((unsigned char *)map) + ((unsigned int)(pol + j)->data.attbl));
                if((pol + j)->type == IS_XPDATA) {
                    if(((unsigned int)(pol + j)->data.vntbl) > maxOffset) {
                        error_printf("Pointer corrupted!\n");
                        return RETURN_ERR_OOB;
                    }
                    (pol + j)->data.vntbl = (VECTOR *)( ((unsigned char *)map) + ((unsigned int)(pol + j)->data.vntbl));
                }
                // all linked data inside
#ifdef SWAP_BYTES
//                pntblX = (FIXED *)((pol + j)->data.pntbl);
//                if((pol + j)->type == IS_XPDATA)
//                    vntblX = (FIXED *)((pol + j)->data.vntbl);
                for(k = 0; k < (pol + j)->data.nbPoint; k++) {
                    for(l = 0; l < 3; l++) {
//                    *(pntblX + k) = SWAP4(*(pntblX + k));
                        fixed = (*((pol + j)->data.pntbl + k))[l];
                        (*((pol + j)->data.pntbl + k))[l] = SWAP4(fixed);
                        if((pol + j)->type == IS_XPDATA) {
//                        *(vntblX + k) = SWAP4(*(vntblX + k));
                            fixed = (*((pol + j)->data.vntbl + k))[l];
                            (*((pol + j)->data.vntbl + k))[l] = SWAP4(fixed);
                        }
                    }
                }
                for(k = 0; k < (pol + j)->data.nbPolygon; k++) {
                    POL_swapPOLYGON((pol + j)->data.pltbl + k);
                    POL_swapATTR((pol + j)->data.attbl + k);
                }
#endif                
            }
#ifdef SWAP_BYTES
            ground = (*(map->tiles + i))->ground;
            for(j = 0; j < (*(map->tiles + i))->nGround; j++) {
                POL_swapGroundData(ground + j);
            }
#endif
        }
    }
        
    return RETURN_OK;
}

#include <sys/stat.h>
ReturnValue MAP_read(MapData **map, char *filename)
{
    unsigned char *buf = NULL;
    unsigned int bufSize = 0;
    FILE *file;
    struct stat fileAttr;
    size_t bytesRead;
    
    if((map == NULL) || (filename == NULL))
        return RETURN_ERR_NULLARG;

    // write buffer to file
    file = fopen(filename, "r");
    if(file == NULL) {
        error_printf("Error opening file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }

    // get filesize
    if(stat(filename, &fileAttr) != 0) {
        error_printf("Error reading attributes of file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    
    // allocate buffer
    buf = (unsigned char *)malloc(fileAttr.st_size);
    if(buf == NULL) {
        error_printf("Error allocating file read buffer!\n");
        return RETURN_ERR_ALLOC;
    }
    bufSize = fileAttr.st_size;
    // read the data
    bytesRead = fread(buf, bufSize, 1, file);
    if(bytesRead < 0) {
        error_printf("Error reading file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    
    if(fclose(file) != 0) {
        error_printf("Error closing file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    
    *map = (MapData *)buf;
    // apply pointer correction
    return MAP_swapSetPointers(*map, bufSize-1);
}

