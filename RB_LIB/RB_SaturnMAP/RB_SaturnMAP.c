#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// note: wrapping printf makes text display corrupted
// with Dev-Cpp only, not on Mac

#include "RB_SGL_def.h"
#include "RB_MAP_def.h"
#include "RB_MAP.h"
#include "RB_MAP_print.h"
#include "RB_MAP_write.h"
#include <RB_RETVAL.H>

// to test compilability
//#include "default_0_0_0.mdl"

#define MIN(a, b)	( ((a) < (b)) ? (a) : (b) )
#define MAX(a, b)	( ((a) > (b)) ? (a) : (b) )


// all controll is supplied in an extern file
    // filenames
        // open and append the shit to this
extern char filename_read[];    // with .BIN
extern char filename_prefix[];    // without .MDL
extern char variable_prefix[];    
    // flags
extern unsigned char flag_input_binary;
extern unsigned char flag_output_binary;
extern unsigned char flag_output_C;
extern unsigned char flag_break;
extern unsigned char flag_merge;
extern unsigned char flag_mean;		// default
extern unsigned char flag_middle;	// priority
extern unsigned char flag_try_all_flags;
extern unsigned char flag_verify;
extern unsigned char flag_compute_ground;
//extern unsigned char flag_compute_mapOrigin;
    // only one of both allowed
extern unsigned char flag_compute_mapSize;
extern unsigned char flag_compute_tileSize;
extern unsigned char flag_print_notes;
extern unsigned char flag_print_warnings;
extern unsigned char flag_print_errors;
    // to specify map attributes
extern MapData map;
    // 3D world data
extern PDATA *pdata_array[];
extern unsigned int pdata_array_length;
extern XPDATA *xpdata_array[];
extern unsigned int xpdata_array_length;
    // collision extraction information
extern CollisionTypes collisionTypes;


#ifdef WRAP_PRINTF
#include <stdarg.h>
int note_printf(const char *fmt, ...) 
{
#if 1
    // other methods fail on Dev-Cpp
    void *arg, *ret;

    if(flag_print_notes == 1) {
        arg = __builtin_apply_args();
        ret = __builtin_apply((void *)printf, arg, 100);
        __builtin_return(ret);
    } else return 1;
#endif
#if 0
    va_list ap;
    if(flag_print_notes == 1) {
        va_start (ap, fmt);
        printf(fmt, ap);
        va_end (ap);
    }    
    return 1;
#endif
#if 0
    if(flag_print_notes == 1)
        return printf(fmt);
    else return 1;
#endif    
}
int warning_printf(const char *fmt, ...) 
{
#if 1
    // other methods fail on Dev-Cpp
    void *arg, *ret;

    if(flag_print_warnings == 1) {
        arg = __builtin_apply_args();
        ret = __builtin_apply((void *)printf, arg, 100);
        __builtin_return(ret);
    } else return 1;
#endif
}

int error_printf(const char *fmt, ...) 
{
#if 1
    // other methods fail on Dev-Cpp
    void *arg, *ret;

    if(flag_print_errors == 1) {
        arg = __builtin_apply_args();
        ret = __builtin_apply((void *)printf, arg, 100);
        __builtin_return(ret);
    } else return 1;
#endif
}
#endif


void testCheckGround()
{
    ReturnValue ret;
    GroundData ground;
    FIXED pos[XYZ];
    int i, j;
    POINT	point_0006[]={
        POStoFIXED( 0,   0,    0),
        POStoFIXED( 0,   0,    1),
        POStoFIXED( 1,   1,    1),
        POStoFIXED( 1,   1,    0),
    };
    POLYGON polygon_0006[]={
        NORMAL(0.076840784,-0.451823808,0.888791731),VERTICES(   0,   1,   2,   3),
    };
    ATTR attr_0006[]={
        ATTRIBUTE(Single_Plane,SORT_CEN,  3+0,No_Palet,No_Gouraud,No_Window|MESHoff|HSSon|ECdis|SPdis|CL32KRGB,sprNoflip,No_Option|UseNearClip|UseLight),
    };
    PDATA	PD_0006={
        point_0006,sizeof(point_0006)/sizeof(POINT),
        polygon_0006,sizeof(polygon_0006)/sizeof(POLYGON),
        attr_0006
    };
    unsigned char hit = 0;
    unsigned char typeID = 255;
    FIXED height = toFIXED(0.0);

    memset(&ground, 0, sizeof(GroundData));

    ret = POL_computeGround(&ground, (void *)&PD_0006, IS_PDATA, 0, 17);
    if(ret != RETURN_OK)
        error_printf("Error computing ground!\n");
        
    // checking for inside
    note_printf("starting!\n");
    for(j = 0; j < 8; j++) {
        for(i = 0; i < 8; i++) {
            // generate point position
            pos[X] = toFIXED(-0.2) + i*toFIXED(0.2);
            pos[Y] = j*toFIXED(0.2);
            pos[Z] = toFIXED(-0.2) + j*toFIXED(0.2);
            // check ground
//        	ret = POL_checkGround(&ground, pos, &hit, &typeID, &height);
            if(ret != RETURN_OK)
                error_printf("Error checking ground!\n");
            // print result
            note_printf("hit = %i at pos (%f, %f, %f)\n", hit, toFLOAT(pos[X]), toFLOAT(pos[Y]), toFLOAT(pos[Z]));
            if(hit == 1) {
                note_printf("typeID = %i, height = %f\n", typeID, toFLOAT(height));
            }
        }
    }
    note_printf("finished!\n");
}


void checkGroundMap(MapData *map, unsigned int xRes, unsigned int zRes, FIXED r2)
{
    unsigned char hit;
    unsigned char typeID;
    FIXED height;  
    FIXED pos[XYZ];
    ReturnValue ret;
    int i, j;
    FIXED addX, addZ;
    
    addX = toFIXED(((float)map->mapSize.x)*toFLOAT(1 << map->tileSize.x) / ((float)xRes));
    addZ = toFIXED(((float)map->mapSize.z)*toFLOAT(1 << map->tileSize.z) / ((float)zRes));
    
    note_printf("o(%f,%f) add(%f,%f)\n", toFLOAT(map->mapOrigin.x), toFLOAT(map->mapOrigin.z), toFLOAT(addX), toFLOAT(addZ));

    pos[Z] = map->mapOrigin.z;
    for(i = 0; i <= zRes; i++) {
        pos[Z] += addZ;
        pos[X] = map->mapOrigin.x;
        for(j = 0; j <= xRes; j++) {
            pos[X] += addX;
#ifdef GROUND_SATURN            
            ret = MAP_checkGround(map, pos, r2, 0, 0, &hit, &typeID, &height, NULL, NULL, NULL, NULL);
#else
            ret = MAP_checkGround(map, pos, &hit, &typeID, &height);
#endif
            if(ret != RETURN_OK) {
                error_printf("Error checking map ground!\n");
            }
            if(hit == 1)
                note_printf("hit! of type %i at pos(%f, %f) with height %f\n", typeID, toFLOAT(pos[X]), toFLOAT(pos[Z]), toFLOAT(height));
        }
    }
    note_printf("Finished!\n");
}

void testBinary(char *file_in, char *file_out)
{
    ReturnValue ret;
    MapData *map;
    
    ret = MAP_read(&map, file_in);
    if(ret != RETURN_OK)
        error_printf("Error reading binary file %s!\n", file_in);
        
    ret = MAP_print(map, "testmap", file_out);
    if(ret != RETURN_OK)
        error_printf("Error writing C file %s!\n", file_out);
    
    // as this map is not allocated normally,
    // but as a whole buffer, 
    // I cannot use MAP_clearMapData();
    if(map != NULL)
        free(map);
    note_printf("Finished!\n");
}


void PCM_demuxChannels4(void *data, unsigned int length, unsigned char bits)
{
    if(bits == 8) {
        int leftSource = length-2, leftTarget = length-1, rightSource = 1, rightTarget = 0;	// refers to channel, not side
        unsigned char *bytes = (unsigned char *)data;
        unsigned char temp;
        while(1) {
            // check to reset offsets
            // such that nothing already sorted is swapped again
            if((rightSource > leftTarget) || (leftSource < rightTarget)) {
                // check if finished
                if((rightTarget+1) == leftTarget)
                    break;
                    
                // re-init offsets to new smaller interval
                rightSource = rightTarget + 1;
                leftSource = leftTarget - 1;
            }
            
            // swap data
                // right channel
            temp = *(bytes + rightSource);
            *(bytes + rightSource) = *(bytes + rightTarget);
            *(bytes + rightTarget) = temp;
            // fix: if both want to perform the same swapping,
            // don't do it twice
            if((rightTarget != leftSource) || (leftTarget != rightSource)) {
                    // left channel
                temp = *(bytes + leftSource);
                *(bytes + leftSource) = *(bytes + leftTarget);
                *(bytes + leftTarget) = temp;
            }
            
            // advance offsets
            rightTarget += 1;
            rightSource += 2;
            leftTarget -= 1;
            leftSource -= 2;
        }
    
    } else {
    }
}


void initMuxedArray(unsigned char *bytes, unsigned int length)
{
    unsigned char leftValue = length / 2, rightValue = 0;
    int i;
    
    for(i = 0; i < length; i += 2) {
        *(bytes + i) = leftValue;
        *(bytes + i + 1) = rightValue;
        leftValue++;
        rightValue++;
    }
}

void printArrays(unsigned char *bytes, unsigned int length)
{
    int i;
    
    for(i = 0; i < length; i++) {
        printf("should be %i, is %i\n", i, *(bytes + i));
    }
}


void testPCM(int length) 
{
    unsigned char bytes[length];
    
    printf("\n\ntesting demux of %i bytes...\n", length);
    initMuxedArray(bytes, length);
    PCM_demuxChannels4(bytes, length, 8);
    printArrays(bytes, length);
    printf("Finished!\n\n");
}


int to2sPower(FIXED fx)
{
    int min = 32, max = 0;
    int i;
    
    // search first and last 1 bit
    for(i = 0; i < 32; i++) {
        if(((1 << i) & fx) != 0) {
            if(i < min)
                min = i;
            if(max < i)
                max = i;
        }
    }
    // round up
    if(min < max)
        max++;
        
    return max;
}

int main(int argc, const char *argv[])
{
    int i;
    MapData *mapPtr = &map;
    MapData *mapPtrReadOnly = NULL;
    VerifyData *verify = NULL;
    ReturnValue ret;
    char filename[40];
//    FIXED fx;
        // initialization performed by finMapSize
    FIXED min[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED max[XYZ] = {0x80000000, 0x80000000, 0x80000000};

#if 0
    testPCM(4);
    testPCM(6);
    testPCM(8);
    testPCM(10);
    testPCM(12);
    return 0;
#endif
    
    note_printf("defining map\n");
    if(flag_verify == 1)
        verify = (VerifyData *)calloc(1, sizeof(VerifyData));
    // determine map origin
        // PDATA
    ret = MAP_findMapSize((void **)pdata_array, pdata_array_length, IS_PDATA, min, max, XYZ);
    if(ret != RETURN_OK)
        error_printf("Error finding map size!\n");
        // XPDATA
    ret = MAP_updateMapSize((void **)xpdata_array, xpdata_array_length, IS_XPDATA, min, max, XYZ);
    if(ret != RETURN_OK)
        error_printf("Error finding map size!\n");
        
    printf("Map min(%f, %f, %f), max(%f, %f, %f)!\n", toFLOAT(min[X]), toFLOAT(min[Y]), toFLOAT(min[Z]), toFLOAT(max[X]), toFLOAT(max[Y]), toFLOAT(max[Z]));
    if(flag_compute_mapSize == 1) {
        map.mapSize.x = (int)(toFLOAT(max[X] - min[X]) / toFLOAT(1 << map.tileSize.x) + 0.9999999);
        map.mapSize.z = (int)(toFLOAT(max[Z] - min[Z]) / toFLOAT(1 << map.tileSize.z) + 0.9999999);
        note_printf("Computed optimal fitting map size: (%i, %i).\n", map.mapSize.x, map.mapSize.z);
    }
    if(flag_compute_tileSize == 1) {
        map.tileSize.x = to2sPower((max[X] - min[X]) / map.mapSize.x);
        map.tileSize.z = to2sPower((max[Z] - min[Z]) / map.mapSize.z);
        note_printf("Computed optimal fitting tile size: (%f, %f).\n", toFLOAT(1 << map.tileSize.x), toFLOAT(1 << map.tileSize.z));
    }
//    if(flag_compute_mapOrigin == 1) {
    // round down
    map.mapOrigin.x = min[X] & (0 - (1 << map.tileSize.x));
    map.mapOrigin.z = min[Z] & (0 - (1 << map.tileSize.x));
    note_printf("Computed map origin: (%f, %f).\n", toFLOAT(map.mapOrigin.x), toFLOAT(map.mapOrigin.z));
//    }
    // check if the map is big enough
    if(((min[X] < map.mapOrigin.x) || (min[Z] < map.mapOrigin.z)) || ((max[X] > (map.mapOrigin.x + map.mapSize.x * (1 << map.tileSize.x))) || (max[Y] > (map.mapOrigin.z + map.mapSize.z * (1 << map.tileSize.z)))))
        warning_printf("Warning: some objects will be outside the map!\n");
    
    if(flag_input_binary == 1) {
        ret = MAP_read(&mapPtrReadOnly, filename_read);
        if(ret != RETURN_OK) {
            error_printf("Error reading binary file %s!\n", filename_read);
            flag_input_binary = 0;
        }
    }
    // now is it loaded successfully?
    if(flag_input_binary == 0) {
        // get CLEARED memory
        map.tiles = (TileData **)calloc(map.mapSize.x * map.mapSize.z, sizeof(TileData *));
        if(map.tiles == NULL)
            return -1;
    }

    for(i = 0; i < 8; i++) {
        // generate flags
        if(flag_try_all_flags == 1) {
            // overwrite flags
            flag_break = (i & 2) >> 1;
            flag_merge = i & 1;
            flag_middle = (i & 4) >> 2;
        };

        // append to the binary map file?
        if(flag_input_binary == 1) {
            ret = MAP_getWriteable(&mapPtr, mapPtrReadOnly);
            if(ret != RETURN_OK) {
                error_printf("Error making binary file %s writeable!\n", filename_read);
                mapPtr = &map;
                flag_input_binary = 0;
            } else {
                // adapt map attributes NOT supported
                note_printf("All attributes of the supplied binary map\n");
                note_printf("will be used without change and overwrite the user supplied attributes!\n");
                    // check if the map is big enough
                if((mapPtr->mapSize.x < map.mapSize.x) || (mapPtr->mapSize.z < map.mapSize.z)) {
                    error_printf("The supplied binary map size is too small!\n");
                    error_printf("Resizing not supported yet, I won't use the map!\n");
                    // delete the map
                    MAP_clearMapData(mapPtr);
                    free(mapPtr);
                    mapPtr = &map;
                    flag_input_binary = 0;
                }
        //        mapPtr->mapSize.x = MAX(mapPtr->mapSize.x, map.mapSize.x);
        //        mapPtr->mapSize.z = MAX(mapPtr->mapSize.z, map.mapSize.z);
            }
        }

        // bit 0 of i: break_over_tiles
        // bit 1 of i: merge_in_tile
        if(flag_try_all_flags == 1)
            note_printf("\n\nConversion number %i\n", i);
// Dev-Cpp compiled makes this output look messed up
        note_printf("Now inserting into map (map %s, %s break, %s merge)...\n", ((flag_middle == 1) ? "middle" : "mean"), flag_break == 0 ? "no" : "do", flag_merge == 0 ? "no": "do");
//        ret = MAP_insertInMap3(mapPtr, (void **)pdata_array, 30, IS_PDATA, (i & 4) == 0 ? MAPING_TYPE_MIDDLE : MAPING_TYPE_MEAN, (i & 1), (i & 2) >> 1, verify);
//        ret = MAP_insertInMap2(mapPtr, (void *)(pdata_array[18]), IS_PDATA, (i & 4) == 0 ? MAPING_TYPE_MIDDLE : MAPING_TYPE_MEAN, (i & 1), (i & 2) >> 1, verify);
        note_printf("pdata_array[%i]...\n", pdata_array_length);
        ret = MAP_insertInMap3(mapPtr, (void **)pdata_array, pdata_array_length, IS_PDATA, flag_middle == 1 ? MAPING_TYPE_MIDDLE : MAPING_TYPE_MEAN, flag_break, flag_merge, verify);
        if(ret != RETURN_OK) {
            error_printf("Error!\n");
            return -1;
        } else note_printf("Success!\n");
        note_printf("xpdata_array[%i]...\n", xpdata_array_length);
        ret = MAP_insertInMap3(mapPtr, (void **)xpdata_array, xpdata_array_length, IS_XPDATA, flag_middle == 1 ? MAPING_TYPE_MIDDLE : MAPING_TYPE_MEAN, flag_break, flag_merge, verify);
        if(ret != RETURN_OK) {
            error_printf("Error!\n");
            return -1;
        } else note_printf("Success!\n");

        if(flag_verify == 1) {
            note_printf("Now verifying map...\n");
            ret = MAP_verify(mapPtr, verify);
            if(ret != RETURN_OK) {
                error_printf("Error!\n");
            } else note_printf("Success!\n");
        }
        
        if(flag_compute_ground == 1) {
            note_printf("Now computing ground info...\n");
            ret = MAP_computeGround(mapPtr, &collisionTypes);	//CollisionTypes *collisionTypes
            if(ret != RETURN_OK) {
                error_printf("Error!\n");
            } else note_printf("Success!\n");
        }

        if(flag_output_binary == 1) {
            snprintf(filename, 40, "%s_%i_%i_%i.bin", filename_prefix, flag_middle, flag_merge, flag_break);
            note_printf("Now writing binary output to file %s\n", filename);
            ret = MAP_write(mapPtr, filename);
            if(ret != RETURN_OK)
                error_printf("Error writing binary file %s!\n", filename);
            else note_printf("Success!\n");
        }
        if(flag_output_C == 1) {
            snprintf(filename, 40, "%s_%i_%i_%i.mdl", filename_prefix, flag_middle, flag_merge, flag_break);
            note_printf("Now writing SGL C output to file %s\n", filename);
        	ret = MAP_print(mapPtr, variable_prefix, filename);
            if(ret != RETURN_OK)
                error_printf("Error writing SGL C file %s!\n", filename);
            else note_printf("Success!\n");
        }

//        checkGroundMap(mapPtr, 100, 100, toFIXED(20));

        // free data
        note_printf("Now freeing data...\n");
            // this doesn't destroy attributes
        MAP_clearMapData(mapPtr);
        if(flag_verify == 1) 
            MAP_clearVerifyData(verify);
        // generated again with next loop:
        if(flag_input_binary == 1)             
            free(mapPtr);
        
        // proceed?
        if(flag_try_all_flags == 0)
            break;

    }
    
    // delete the loaded binary file
    if(mapPtrReadOnly != NULL)
        free(mapPtrReadOnly);
    
    note_printf("Finished without errors!\n");
    
//    testBinary("default_0_0_0.bin", "default_0_0_0_bin.mdl");
//    testCheckGround();
    return 0;

    return 0;
}
