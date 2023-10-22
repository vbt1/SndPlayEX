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

#include "RB_POL.h"
#include "RB_MAP_print.h"

ReturnValue MAP_printPolygonDataArray(PolygonData *pols, unsigned int nPols, unsigned char *suffix, FILE *fp)
{
    int i, j;
    
    if((pols == NULL) || (fp == NULL))
        return RETURN_ERR_NULLARG;
        
    // print all pdata & xpdata 
    // array type content like
    // POINT, VECTOR, POLYGON, ATTR
    for(i = 0; i < nPols; i++) {
        // PolygonData valid?
        if(((pols + i)->type != IS_PDATA) && ((pols + i)->type != IS_XPDATA))
            continue;
            
        // print POINT table
        fprintf(fp, "POINT pntbl_%s_%i[] = {\n", suffix, i);
        for(j = 0; j < (pols + i)->data.nbPoint; j++) {
            fprintf(fp, "\t{%li, %li, %li},\n", (*((pols + i)->data.pntbl + j))[X], (*((pols + i)->data.pntbl + j))[Y], (*((pols + i)->data.pntbl + j))[Z]);
        }
        fprintf(fp, "};\n\n");
        
        // print VECTOR table
        if((pols + i)->type == IS_XPDATA) {
            fprintf(fp, "VECTOR vntbl_%s_%i[] = {\n", suffix, i);
            for(j = 0; j < (pols + i)->data.nbPoint; j++) {
                fprintf(fp, "\t{%li, %li, %li},\n", (*((pols + i)->data.vntbl + j))[X], (*((pols + i)->data.vntbl + j))[Y], (*((pols + i)->data.vntbl + j))[Z]);
            }
            fprintf(fp, "};\n\n");
        }
        
        // print POLYGON table
        fprintf(fp, "POLYGON pltbl_%s_%i[] = {\n", suffix, i);
        for(j = 0; j < (pols + i)->data.nbPolygon; j++) {
            fprintf(fp, "\t{\n");
                fprintf(fp, "\t\t{%li, %li, %li},\n", ((pols + i)->data.pltbl + j)->norm[X], ((pols + i)->data.pltbl + j)->norm[Y], ((pols + i)->data.pltbl + j)->norm[Z]);
                fprintf(fp, "\t\t{%i, %i, %i, %i},\n", ((pols + i)->data.pltbl + j)->Vertices[0], ((pols + i)->data.pltbl + j)->Vertices[1], ((pols + i)->data.pltbl + j)->Vertices[2], ((pols + i)->data.pltbl + j)->Vertices[3]);
            fprintf(fp, "\t},\n");
        }
        fprintf(fp, "};\n\n");
        
        // print ATTR table
        fprintf(fp, "ATTR attbl_%s_%i[] = {\n", suffix, i);
        for(j = 0; j < (pols + i)->data.nbPolygon; j++) {
            fprintf(fp, "\t{%i, %i, %i, %i, %i, %i, %i},\n", ((pols + i)->data.attbl + j)->flag, ((pols + i)->data.attbl + j)->sort, ((pols + i)->data.attbl + j)->texno, ((pols + i)->data.attbl + j)->atrb, ((pols + i)->data.attbl + j)->colno, ((pols + i)->data.attbl + j)->gstb, ((pols + i)->data.attbl + j)->dir);
        }
        fprintf(fp, "};\n\n");
    }

    // print the polygon data array
    fprintf(fp, "PolygonData polygons_%s[] = {\n", suffix);
    for(i = 0; i < nPols; i++) {
        fprintf(fp, "\t{%s,\n", ((pols + i)->type == IS_PDATA ? "IS_PDATA" : "IS_XPDATA"));
            fprintf(fp, "\t\t{\n");
                fprintf(fp, "\t\t\tpntbl_%s_%i,\n", suffix, i);
                fprintf(fp, "\t\t\tsizeof(pntbl_%s_%i) / sizeof(POINT),\n", suffix, i);
                fprintf(fp, "\t\t\tpltbl_%s_%i,\n", suffix, i);
                fprintf(fp, "\t\t\tsizeof(pltbl_%s_%i) / sizeof(POLYGON),\n", suffix, i);
                fprintf(fp, "\t\t\tattbl_%s_%i,\n", suffix, i);
                if((pols + i)->type == IS_PDATA)
                    fprintf(fp, "\t\t\tNULL,\n");
                else fprintf(fp, "\t\t\tvntbl_%s_%i,\n", suffix, i);
            fprintf(fp, "\t\t}\n");
        fprintf(fp, "\t},\n");
    }
    fprintf(fp, "};\n\n");
    
    return RETURN_OK;
}


ReturnValue MAP_printGroundDataArray(GroundData *ground, unsigned int nGround, unsigned char *suffix, FILE *fp)
{
    int i;
    
    if((ground == NULL) || (fp == NULL))
        return RETURN_ERR_NULLARG;

    // print the tile array
    fprintf(fp, "GroundData grounds_%s[] = {\n", suffix);
    for(i = 0; i < nGround; i++) {
        fprintf(fp, "\t{\n");
        fprintf(fp, "\t\t%i,\n", (ground + i)->typeID);
#ifdef GROUND_SATURN
        fprintf(fp, "\t\t{%li, %li},\n", (ground + i)->normal.x, (ground + i)->normal.z);
        fprintf(fp, "\t\t{%li, %li},\n", (ground + i)->direction.x, (ground + i)->direction.z);
        fprintf(fp, "\t\t{%li, %li, %li},\n", (ground + i)->center[X], (ground + i)->center[Y], (ground + i)->center[Z]);
#else
        fprintf(fp, "\t\t{%li, %li},\n", (ground + i)->min.x, (ground + i)->min.z);
        fprintf(fp, "\t\t{%li, %li},\n", (ground + i)->max.x, (ground + i)->max.z);
        fprintf(fp, "\t\t{{%li, %li},\n", (ground + i)->sideFactors[0].x, (ground + i)->sideFactors[0].z);
        fprintf(fp, "\t\t{%li, %li},\n", (ground + i)->sideFactors[1].x, (ground + i)->sideFactors[1].z);
        fprintf(fp, "\t\t{%li, %li},\n", (ground + i)->sideFactors[2].x, (ground + i)->sideFactors[2].z);
        fprintf(fp, "\t\t{%li, %li}},\n", (ground + i)->sideFactors[3].x, (ground + i)->sideFactors[3].z);
        fprintf(fp, "\t\t{%li, %li, %li},\n", (ground + i)->heightFactors[X], (ground + i)->heightFactors[Y], (ground + i)->heightFactors[Z]);
#endif
        fprintf(fp, "\t},\n");
    }
    fprintf(fp, "};\n\n");

    return RETURN_OK;
}

ReturnValue MAP_printTileData(TileData *tile, unsigned char *suffix, FILE *fp)
{
    ReturnValue ret;
    
    if((tile == NULL) || (fp == NULL))
        return RETURN_ERR_NULLARG;

    // print all arrays
    if(tile->nObj > 0) {
        ret = MAP_printPolygonDataArray(tile->obj, tile->nObj, suffix, fp);
        if(ret != RETURN_OK)
            return ret;
    }
    if(tile->nGround) {
        ret = MAP_printGroundDataArray(tile->ground, tile->nGround, suffix, fp);
        if(ret != RETURN_OK)
            return ret;
    }
    
    // print the tile
    fprintf(fp, "TileData tile_%s = {\n", suffix);
        fprintf(fp, "\t%i,\n", tile->nObj);
        fprintf(fp, "\t%i,\n", tile->nGround);
        if(tile->nObj == 0)
            fprintf(fp, "\tNULL,\n");
        else fprintf(fp, "\tpolygons_%s,\n", suffix);
        if(tile->nGround == 0)
            fprintf(fp, "\tNULL,\n");
        else fprintf(fp, "\tgrounds_%s,\n", suffix);            
    fprintf(fp, "};\n\n");
    
    return RETURN_OK;
}


ReturnValue MAP_printTileArray(MapData *map, unsigned char *suffix, FILE *fp)
{
    int x, z;
    ReturnValue ret;
    char tile_suffix[40];
    
    if((map == NULL) || (fp == NULL))
        return RETURN_ERR_NULLARG;
    
    // print all tiles
    for(z = 0; z < map->mapSize.z; z++) {
        for(x = 0; x < map->mapSize.x; x++) {
            if(*(map->tiles + z*map->mapSize.x + x) != NULL) {
                // prepare suffix
                snprintf(tile_suffix, 40, "%s_%i_%i", suffix, z, x);
                ret = MAP_printTileData(*(map->tiles + z*map->mapSize.x + x), tile_suffix, fp);
                if(ret != RETURN_OK)
                    return ret;
            }       
        }
    }
    
    // print the tile array
    fprintf(fp, "TileData *tiles_%s[] = {\n", suffix);
    for(z = 0; z < map->mapSize.z; z++) {
        for(x = 0; x < map->mapSize.x; x++) {
            // prepare suffix
            if(*(map->tiles + z*map->mapSize.x + x) != NULL) 
                snprintf(tile_suffix, 40, "&tile_%s_%i_%i", suffix, z, x);
            else snprintf(tile_suffix, 40, "NULL");
            fprintf(fp, "\t%s,\n", tile_suffix);
            
        }
    }
    fprintf(fp, "};\n\n");

    return RETURN_OK;
}


ReturnValue MAP_printMapData(MapData *map, unsigned char *suffix, FILE *fp)
{
    ReturnValue ret;
    
    if((map == NULL) || (fp == NULL))
        return RETURN_ERR_NULLARG;
    
    // print a tile array
    ret = MAP_printTileArray(map, suffix, fp);
    if(ret != RETURN_OK)
        return ret;
        
    // print map data
    fprintf(fp, "MapData map_%s = {\n", suffix);
    fprintf(fp, "\t{%i, %i},\n", map->mapSize.x, map->mapSize.z);
    fprintf(fp, "\t{%i, %i},\n", map->tileSize.x, map->tileSize.z);
    fprintf(fp, "\t{%li, %li},\n", map->mapOrigin.x, map->mapOrigin.z);
    fprintf(fp, "\ttiles_%s,\n", suffix);
    fprintf(fp, "};\n\n");
    
    return RETURN_OK;
}

ReturnValue MAP_print(MapData *map, char *variable_prefix, char *filename)
{
    FILE *file;
    ReturnValue ret;
    
    if((map == NULL) || (filename == NULL))
        return RETURN_ERR_NULLARG;
        
    file = fopen(filename, "w");
    if(file == NULL) {
        error_printf("Error opening file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
    ret = MAP_printMapData(map, variable_prefix, file);
    if(ret != RETURN_OK) {
        error_printf("Error printing file %s!\n", filename);
        return ret;
    }
    if(fclose(file) != 0) {
        error_printf("Error closing file %s!\n", filename);
        return RETURN_ERR_UNKNOWN;
    }
        
    return RETURN_OK;
}

