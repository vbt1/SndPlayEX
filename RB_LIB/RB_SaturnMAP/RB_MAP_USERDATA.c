#include <stdlib.h>
#include "RB_SGL_def.h"
#include "RB_POL_def.h"
#include "RB_MAP_def.h"


// IMPORTAND:
// when compiler gives an error because of #IND
// found in your .MDL file, 
// you MUST delete these polygons, as they don't are correct polygons
// and will cause verification errors here
// (which probably are wrong, but anyways, these polygons will display corrupted on Saturn)



// all controll is supplied in an extern file
    // filenames
        // open and append the shit to this
char filename_read[] = "default_0_0_0.bin";    // with .BIN
char filename_prefix[] = "driving";    // without .MDL
char variable_prefix[] = "driving";    
    // flags
        // want to append to a binary MapData file(filename_read)?
        // NOTE: verify not supported
        // NOTE: supplied binary map must be large enough
unsigned char flag_input_binary = 0;	
unsigned char flag_output_binary = 1;
unsigned char flag_output_C = 1;
    // breaks an object apart at tile boundaries,
    // so that objects in a tile don't occupy space of another tile
    // !	this increases 3D data size
    // !	may increase or decrease rendering time on saturn
    //		decrease because of duplicate points and polygons
    //		increase because avoiding to draw stuff located in other tiles
unsigned char flag_break = 1;
    // merge all objects of a tile together
    // !	this can gain speed on saturn because of
    // 		duplicate point and polygon removal
unsigned char flag_merge = 0;
    // how to map an object or a polygon
    // to a tile?
    // either by mapping it's mean point or
    // by it's mapping middle point
unsigned char flag_mean = 1;
unsigned char flag_middle = 0;
    // loops through all possible map insert options
    // so you run the program once
    // and choose the setting that fits your needs most
unsigned char flag_try_all_flags = 0;
    // to verify the generated map to the original data
    // only for debugging purposes
    // !	will slow down the conversion
unsigned char flag_verify = 1;
    // makes ground collision data
    // redundant, but priority
unsigned char flag_compute_ground = 1;
    // recommended, as you might not exactly know the space occupation of the world
//unsigned char flag_compute_mapOrigin = 1;
    // only one of both allowed
unsigned char flag_compute_mapSize = 1;
unsigned char flag_compute_tileSize = 0;
    // text output slows down the conversion
unsigned char flag_print_notes = 1;
unsigned char flag_print_warnings = 1;
unsigned char flag_print_errors = 1;
    // to specify map attributes
MapData map = {
    {32, 32},								// mapSize(int)
    {16 + 5, 16 + 5},                       // tileSize(bit shifts)
    {toFIXED(0), toFIXED(0)},				// mapOrigin(FIXED)
    NULL
};

    // 3D world data
#if 0	// single file method with renaming
// name conversion to standard
#	define map_data	pdata_array
//#	include "DRIVING.MDL"

#else	// multiple files method with cut and paste
//#	include "file1.MDL"
//#	include "file2.MDL"
//#	include "file3.MDL"
#	include "myTrack.MDL"
    PDATA *pdata_array[] = {
        (PDATA *)&xpdata_Meshes
    };
#endif
unsigned int pdata_array_length = sizeof(pdata_array) / sizeof(PDATA *);
#if 0	// single file method with renaming
// name conversion to standard
//#	define xmap_data	xpdata_array
//#	include "DRIVING.MDL"

#else	// multiple files method with cut and paste
//#	include "file1.MDL"
//#	include "file2.MDL"
//#	include "file3.MDL"
    XPDATA *xpdata_array[] = {
    };
#endif
unsigned int xpdata_array_length = sizeof(xpdata_array) / sizeof(XPDATA *);
// collision extraction information
// ground specification for collision data generation
// IMPORTAND:
// SearchConstraint searchConstraint[]
// is met, if ALL entries are met, it's AND, not OR
    // track
SearchConstraint searchConstraint0[] = {
    { SEARCH_ATTR_COLNO, 0xFFFF, C_RGB(20,20,21)},
};
SearchConstraint searchConstraint1[] = {
    { SEARCH_ATTR_COLNO, 0xFFFF, C_RGB(25,27, 4)},
};
SearchConstraint searchConstraint2[] = {
    { SEARCH_ATTR_COLNO, 0xFFFF, C_RGB(16, 0, 0)},
};
    // side
SearchConstraint searchConstraint3[] = {
    { SEARCH_ATTR_COLNO, 0xFFFF, C_RGB(11,31, 9)},
};
SearchConstraint searchConstraint4[] = {
    { SEARCH_ATTR_COLNO, 0xFFFF, C_RGB(18, 7, 0)},
};
SearchConstraint searchConstraint5[] = {
    { SEARCH_ATTR_COLNO, 0xFFFF, C_RGB( 8,21,31)},
};
CollisionType collisionType[] = {
// NOTE: if you don't want the border collision data centers to be corrected,
// just don't mark isBorder as 1
    // track
    {0, 0, { sizeof(searchConstraint0) / sizeof(SearchConstraint),  searchConstraint0 } },
    {0, 0, { sizeof(searchConstraint1) / sizeof(SearchConstraint),  searchConstraint1 } },
    {0, 0, { sizeof(searchConstraint2) / sizeof(SearchConstraint),  searchConstraint2 } },
    // side
    {1, 1, { sizeof(searchConstraint3) / sizeof(SearchConstraint),  searchConstraint3 } },
    {1, 1, { sizeof(searchConstraint4) / sizeof(SearchConstraint),  searchConstraint4 } },
    {1, 1, { sizeof(searchConstraint5) / sizeof(SearchConstraint),  searchConstraint5 } },
//    {1, 1, { sizeof(searchConstraint3) / sizeof(SearchConstraint),  searchConstraint3 } },
//    {1, 1, { sizeof(searchConstraint4) / sizeof(SearchConstraint),  searchConstraint4 } },
//    {1, 1, { sizeof(searchConstraint5) / sizeof(SearchConstraint),  searchConstraint5 } },
};
CollisionTypes collisionTypes = {
    sizeof(collisionType) / sizeof(CollisionType),
    collisionType
};
