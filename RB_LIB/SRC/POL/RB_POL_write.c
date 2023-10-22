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
#include "RB_POL_write.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void POL_swapATTR(ATTR *attr)
{
    if(attr == NULL)
        return;
        
    attr->texno = SWAP2(attr->texno);
    attr->atrb = SWAP2(attr->atrb);
    attr->colno = SWAP2(attr->colno);
    attr->gstb = SWAP2(attr->gstb);
    attr->dir = SWAP2(attr->dir);
}

void POL_swapPOLYGON(POLYGON *pol)
{
    if(pol == NULL)
        return;
        
    pol->norm[X] = SWAP4(pol->norm[X]);
    pol->norm[Y] = SWAP4(pol->norm[Y]);
    pol->norm[Z] = SWAP4(pol->norm[Z]);

    pol->Vertices[0] = SWAP2(pol->Vertices[0]);
    pol->Vertices[1] = SWAP2(pol->Vertices[1]);
    pol->Vertices[2] = SWAP2(pol->Vertices[2]);
    pol->Vertices[3] = SWAP2(pol->Vertices[3]);
}

void POL_swapXPDATA(XPDATA *data, unsigned char type)
{
    if(data == NULL)
        return;
        
    data->pntbl = (POINT *)SWAP4((unsigned int)data->pntbl);
    data->nbPoint = SWAP4(data->nbPoint);
    data->pltbl = (POLYGON *)SWAP4((unsigned int)data->pltbl);
    data->nbPolygon = SWAP4(data->nbPolygon);
    data->attbl = (ATTR *)SWAP4((unsigned int)data->attbl);
    if(type == IS_XPDATA)
        data->vntbl = (VECTOR *)SWAP4((unsigned int)data->vntbl);
}

void POL_swapGroundData(GroundData *ground)
{
    int i;
    
    if(ground == NULL)
        return;
        
    ground->typeID = SWAP4(ground->typeID);
#ifdef GROUND_SATURN
    ground->normal.x = SWAP4(ground->normal.x);
    ground->normal.z = SWAP4(ground->normal.z);
    ground->direction.x = SWAP4(ground->direction.x);
    ground->direction.z = SWAP4(ground->direction.z);
    for(i = 0; i < 3; i++) {
        ground->center[i] = SWAP4(ground->center[i]);
    }
#else
    ground->min.x = SWAP4(ground->min.x);
    ground->min.z = SWAP4(ground->min.z);
    ground->max.x = SWAP4(ground->max.x);
    ground->max.z = SWAP4(ground->max.z);
    for(i = 0; i < 4; i++) {
        ground->sideFactors[i].x = SWAP4(ground->sideFactors[i].x);
        ground->sideFactors[i].z = SWAP4(ground->sideFactors[i].z);
    }
    for(i = 0; i < 3; i++) {
        ground->heightFactors[i] = SWAP4(ground->heightFactors[i]);
    }
#endif
}

ReturnValue alignResizeBuffer(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int mySize, unsigned char alignBytes)
{
    unsigned char *ptr;
    
    if(((*startOffset) & (alignBytes - 1)) != 0)
        mySize += 4 - (*startOffset & (alignBytes - 1));
    // adjust buffer size
    if((*startOffset + mySize) > *bufSize) {
        if(*buf == NULL)
            ptr = (unsigned char *)malloc(mySize);
        else ptr = (unsigned char *)realloc(*buf, *bufSize + mySize);
        if(ptr == NULL) {
            error_printf("Error allocating XPDATA!\n");
            return RETURN_ERR_ALLOC;
        }
        *buf = ptr;
        *bufSize += mySize;
    }
    // clear alignment padding
    for(*startOffset = *startOffset; (*startOffset & (alignBytes - 1)) != 0; (*startOffset)++) {
        *(*buf + *startOffset) = 0;
    }
    return RETURN_OK;
}



ReturnValue POL_writeFIXEDarray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, FIXED *data, unsigned int n)
{
    int i;
    ReturnValue ret;
    FIXED *data_ptr, data_val;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, n*sizeof(FIXED), 4);
    if(ret != RETURN_OK)
        return ret;
        
    // copy data
    memcpy(*buf + *startOffset, data, n*sizeof(FIXED));
    
    // write linked data
    
    // adapt data
#ifdef SWAP_BYTES    
    data_ptr = (FIXED *)(*buf + *startOffset);
    for(i = 0; i < n; i++) {
        data_val = *(data_ptr + i);
        *(data_ptr + i) = SWAP4(data_val);
    }
#endif
    
    // set end offset
    *endOffset = *startOffset + n*sizeof(FIXED);
    
    return RETURN_OK;
}



ReturnValue POL_writePOLYGONarray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, POLYGON *data, unsigned int n)
{
    int i;
    ReturnValue ret;
    POLYGON *data_ptr;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, n*sizeof(POLYGON), 4);
    if(ret != RETURN_OK)
        return ret;
        
    // copy data
    memcpy(*buf + *startOffset, data, n*sizeof(POLYGON));
    
    // write linked data
    
    // adapt data
#ifdef SWAP_BYTES    
    data_ptr = (POLYGON *)(*buf + *startOffset);
    for(i = 0; i < n; i++) {
        POL_swapPOLYGON(data_ptr + i);
#if 0        
        data_val = *(data_ptr + i);
        data_ptr->norm[X] = SWAP4(data_val.norm[X]);
        data_ptr->norm[Y] = SWAP4(data_val.norm[Y]);
        data_ptr->norm[Z] = SWAP4(data_val.norm[Z]);

        data_ptr->Vertices[0] = SWAP2(data_val.Vertices[0]);
        data_ptr->Vertices[1] = SWAP2(data_val.Vertices[1]);
        data_ptr->Vertices[2] = SWAP2(data_val.Vertices[2]);
        data_ptr->Vertices[3] = SWAP2(data_val.Vertices[3]);
#endif
    }
#endif
    
    // set end offset
    *endOffset = *startOffset + n*sizeof(POLYGON);
    
    return RETURN_OK;
}


ReturnValue POL_writeATTRarray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, ATTR *data, unsigned int n)
{
    int i;
    ReturnValue ret;
    ATTR *data_ptr;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, n*sizeof(ATTR), 2);
    if(ret != RETURN_OK)
        return ret;
        
    // copy data
    memcpy(*buf + *startOffset, data, n*sizeof(ATTR));
    
    // write linked data
    
    // adapt data
#ifdef SWAP_BYTES    
    data_ptr = (ATTR *)(*buf + *startOffset);
    for(i = 0; i < n; i++) {
        POL_swapATTR(data_ptr + i);
    }
#endif
    
    // set end offset
    *endOffset = *startOffset + n*sizeof(ATTR);
    
    return RETURN_OK;
}



ReturnValue POL_writeXPDATA(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, XPDATA *data, unsigned char type)
{
    ReturnValue ret;
    XPDATA *data_ptr;
    unsigned int startPNTBL, startPLTBL, startATTBL, startVNTBL, endLinked;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, sizeof(XPDATA), 4);
    if(ret != RETURN_OK)
        return ret;

    // copy data
    memcpy(*buf + *startOffset, data, sizeof(XPDATA));

    // write linked data
        // pntbl
    startPNTBL = *startOffset + sizeof(XPDATA);
	ret = POL_writeFIXEDarray(buf, bufSize, &startPNTBL, &endLinked, (FIXED *)data->pntbl, data->nbPoint * (sizeof(POINT) / sizeof(FIXED)));
    if(ret != RETURN_OK)
        return ret;
        // pltbl
    startPLTBL = endLinked;
	ret = POL_writePOLYGONarray(buf, bufSize, &startPLTBL, &endLinked, data->pltbl, data->nbPolygon);
    if(ret != RETURN_OK)
        return ret;
        // attbl
    startATTBL = endLinked;
	ret = POL_writeATTRarray(buf, bufSize, &startATTBL, &endLinked, data->attbl, data->nbPolygon);
    if(ret != RETURN_OK)
        return ret;
        // vntbl
    if(type == IS_XPDATA) {
        startVNTBL = endLinked;
        ret = POL_writeFIXEDarray(buf, bufSize, &startVNTBL, &endLinked, (FIXED *)data->vntbl, data->nbPoint * (sizeof(VECTOR) / sizeof(FIXED)));
        if(ret != RETURN_OK)
            return ret;
    }
    // adapt data
    data_ptr = (XPDATA *)(*buf + *startOffset);
    data_ptr->pntbl = (POINT *)startPNTBL;
    data_ptr->pltbl = (POLYGON *)startPLTBL;
    data_ptr->attbl = (ATTR *)startATTBL;
    if(type == IS_XPDATA)
        data_ptr->vntbl = (VECTOR *)startVNTBL;
#ifdef SWAP_BYTES
    POL_swapXPDATA(data_ptr, type);
#endif
    
    // set end offset
    *endOffset = endLinked;
    
    return RETURN_OK;
}


ReturnValue POL_writeGroundDataArray(unsigned char **buf, unsigned int *bufSize, unsigned int *startOffset, unsigned int *endOffset, GroundData *data, unsigned int n)
{
    ReturnValue ret;
    int j;
    GroundData *data_ptr;
    
    if((buf == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
    
    // care for alignments for myself
    ret = alignResizeBuffer(buf, bufSize, startOffset, n*sizeof(GroundData), 4);
    if(ret != RETURN_OK)
        return ret;

    // copy data
    memcpy(*buf + *startOffset, data, n*sizeof(GroundData));

    // write linked data

    // adapt data
#ifdef SWAP_BYTES
    data_ptr = (GroundData *)(*buf + *startOffset);
    for(j = 0; j < n; j++) {
        POL_swapGroundData(data_ptr + j);
    }
#endif
    
    // set end offset
    *endOffset = *startOffset + n*sizeof(GroundData);
    
    return RETURN_OK;
}

