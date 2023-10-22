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

#include "RB_POL.h"


int compare_PointEntry(const void *a, const void *b)
{
    const PointEntry *pea = (const PointEntry *)a;
    const PointEntry *peb = (const PointEntry *)b;

    return pea->old_index - peb->old_index;
}


int POL_comparePoints(FIXED *a, FIXED *b, unsigned char dim)
{
    int i;
    unsigned char match = 0;
    
    if((a == NULL) || (b == NULL))
        return 0;
    
    for(i = 0; i < dim; i++) {
        if(a[i] != b[i])
            break;
        else match++;
    }
    return (match == dim);
}

int POL_comparePolygons(XPDATA *a, Uint16 indexa, XPDATA *b, Uint16 indexb)
{
    int i, j;
    unsigned char match = 0;
    
    if((a == NULL) || (b == NULL) || (indexa >= a->nbPolygon) || (indexb >= b->nbPolygon))
        return 0;
    
    // compare points
    // index order in Vertices does not matter
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            if(POL_comparePoints(*(a->pntbl + (a->pltbl + indexa)->Vertices[i]), *(b->pntbl + (b->pltbl + indexb)->Vertices[j]), XYZ) == 1) {
                match++;
                break;
            }
        }
    }
    if(match != 4)
        return -2;

    return 1;
}

void POL_clearXPDATA(XPDATA *data, unsigned char type)
{
    if(data == NULL)
        return;
        // POINT & VECTOR & PointEntry array
    if(data->nbPoint > 0) {
        free(data->pntbl);
        if(type == IS_XPDATA)
            free(data->vntbl);
    }
        // POLYGON & ATTR
    if(data->nbPolygon > 0) {
        free(data->pltbl);
        free(data->attbl);
    }
    memset(data, 0, (type == IS_PDATA ? sizeof(PDATA) : sizeof(XPDATA)));        
}

ReturnValue POL_updateMinMax(POINT *pntbl, Uint32 nbPoint, FIXED *min, FIXED *max, unsigned char dim)
{
    int i, j;
    
    if((pntbl == NULL) || (min == NULL) || (max == NULL))
        return RETURN_ERR_NULLARG;
    
    // search all points
    for(i = 0; i < nbPoint; i++) {
        // update min and max
        for(j = 0; j < dim; j++) {
            if((*(pntbl + i))[j] < min[j])
                min[j] = (*(pntbl + i))[j];
            // no else, since min <= max isn't true at start
            if((*(pntbl + i))[j] > max[j])
                max[j] = (*(pntbl + i))[j];
        }
    }
    
    return RETURN_OK;
}


ReturnValue POL_findObjectMid(XPDATA *data, FIXED *mid, unsigned char dim)
{
    FIXED min[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED max[XYZ] = {0x80000000, 0x80000000, 0x80000000};
    int i;
    ReturnValue ret;
    
    if((data == NULL) || (mid == NULL))
        return RETURN_ERR_NULLARG;
    
    // find min max
    ret = POL_updateMinMax(data->pntbl, data->nbPoint, min, max, dim);
    if(ret != RETURN_OK)
        return ret;
        
    // compute mid
    for(i = 0; i < dim; i++)
        mid[i] = (min[i] + max[i]) >> 1;
    
    return RETURN_OK;
}

ReturnValue POL_findPolygonMid(XPDATA *data, Uint16 index, FIXED *mid, unsigned char dim)
{
    FIXED min[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    FIXED max[XYZ] = {0x80000000, 0x80000000, 0x80000000};
    int i;
    ReturnValue ret;
    
    if((data == NULL) || (mid == NULL))
        return RETURN_ERR_NULLARG;
    
    if(index >= data->nbPolygon)
        return RETURN_ERR_INVALIDARG;
    
    // find min max
    for(i = 0; i < 4; i++) {
        ret = POL_updateMinMax(data->pntbl + (data->pltbl + index)->Vertices[i], 1, min, max, dim);
        if(ret != RETURN_OK)
            return ret;
    }
    
    // compute mid
    for(i = 0; i < dim; i++)
        mid[i] = (min[i] + max[i]) >> 1;
        
    return RETURN_OK;
}


ReturnValue POL_findObjectMean(XPDATA *data, FIXED *mean, unsigned char dim)
{
    int i, j;
    double dmean[dim];
    
    if((data == NULL) || (mean == NULL))
        return RETURN_ERR_NULLARG;
    
    // init dmean
    for(j = 0; j < dim; j++) 
        dmean[j] = 0.0;

    // accumulate
    for(i = 0; i < data->nbPoint; i++) {
        for(j = 0; j < dim; j++) {
            dmean[j] += toFLOAT((*(data->pntbl + i))[j]);
        }
    }
    // compute mean
    for(j = 0; j < dim; j++) {
        mean[j] = toFIXED(dmean[j] / ((double)data->nbPoint));
    }
    return RETURN_OK;
}

ReturnValue POL_findPolygonMean(XPDATA *data, Uint16 index, FIXED *mean, unsigned char dim)
{
    int i, j;
    double dmean[dim];
    
    if((data == NULL) || (mean == NULL))
        return RETURN_ERR_NULLARG;
    
    // initialize dmean
    for(j = 0; j < dim; j++)
        dmean[j] = 0.0;

    // accumulate
    for(i = 0; i < 4; i++) {
        for(j = 0; j < dim; j++) {
            dmean[j] += toFLOAT((*(data->pntbl + (data->pltbl + index)->Vertices[i]))[j]);
        }
    }
    // compute mean
    for(j = 0; j < dim; j++) {
        mean[j] = toFIXED(dmean[j] / 4.0);
    }
    return RETURN_OK;
}


// Copies the point from xpdata2 at old_index
// to xpdata1 at new_index.
// If the same point is already in xpdata1, it's index is returned,
// otherwise a new point is appened to xpdata1
//
// The search for equal points can be very much faster,
// if pe_array is supplied, which performs the mapping of
// old_index to new_index
// !!! Caution using pe_array: you need a seperate pe_array for every xpdata2,
// because when adding points to xpdata1 from xpdata2 and xpdata3, there are points with the same index,
// which will cause errors!
ReturnValue POL_copyPoint(XPDATA *xpdata1, XPDATA *xpdata2, unsigned char type, PointEntry **pe_array, Uint16 old_index, Uint16 *new_index)
{
    PointEntry pe, *pe_ptr = NULL;
    POINT *pntbl = NULL;
    VECTOR *vntbl = NULL;
    
    if((xpdata1 == NULL) || (xpdata2 == NULL))
        return RETURN_ERR_NULLARG;
// redundant: oe->pe_count == oe->pol.data.nbPoint
    // has been registered before?
    if(xpdata1->nbPoint > 0) {
        // how to map indices?
        if(pe_array == NULL) {
            // raw points cannot be sorted,
            // search all points the hard way
            int i;
            pe_ptr = NULL;
            for(i = 0; i < xpdata1->nbPoint; i++) {
                if(POL_comparePoints(*(xpdata1->pntbl + i), *(xpdata2->pntbl + old_index), XYZ) == 1) {
                    // behave like if no_pe == 0
                    pe.new_index = i;
                    pe_ptr = &pe;
                    break;
                }
            }
            
        } else {
            pe.old_index = old_index;
            pe_ptr = bsearch(&pe, *pe_array, xpdata1->nbPoint, sizeof(PointEntry), compare_PointEntry);
        }
        if(pe_ptr == NULL) {
            // no match: append point
            if(pe_array != NULL)
                pe_ptr = (PointEntry *)realloc(*pe_array, (xpdata1->nbPoint+1) * sizeof(PointEntry));
            pntbl = (POINT *)realloc(xpdata1->pntbl, (xpdata1->nbPoint+1) * sizeof(POINT));
            if(type == IS_XPDATA)
                vntbl = (VECTOR *)realloc(xpdata1->vntbl, (xpdata1->nbPoint+1) * sizeof(VECTOR));
        } else {
            // found it
            if(new_index != NULL)
                *new_index = pe_ptr->new_index;
            return RETURN_OK;
        }
    } else {	// first element
        if(pe_array != NULL)
            pe_ptr = (PointEntry *)malloc(sizeof(PointEntry));
        pntbl = (POINT *)malloc(sizeof(POINT));
        if(type == IS_XPDATA)
            vntbl = (VECTOR *)malloc(sizeof(VECTOR));
    }
    // now here, insert new point into last element
    if(pntbl == NULL)
        return RETURN_ERR_ALLOC;
    xpdata1->pntbl = pntbl;

        // the index mapping
    if(pe_array != NULL) {
        if(pe_ptr == NULL)
            return RETURN_ERR_ALLOC;
        (pe_ptr + xpdata1->nbPoint)->old_index = old_index;
        (pe_ptr + xpdata1->nbPoint)->new_index = xpdata1->nbPoint;	// implicit
        *pe_array = pe_ptr;
    }
    
    if(type == IS_XPDATA) {
        if(vntbl == NULL)
            return RETURN_ERR_ALLOC;
        xpdata2->vntbl = vntbl;
    }
        // the POINT itself
    memcpy(*(xpdata1->pntbl + xpdata1->nbPoint), *(xpdata2->pntbl + old_index), sizeof(POINT));
    if(type == IS_XPDATA)
        memcpy(xpdata1->vntbl + xpdata1->nbPoint, xpdata2->vntbl + old_index, sizeof(VECTOR));
    if(new_index != NULL)
        *new_index = xpdata1->nbPoint;
    xpdata1->nbPoint += 1;
    // need to keep it sorted
    if(pe_array != NULL)
        qsort(*pe_array, xpdata1->nbPoint, sizeof(PointEntry), compare_PointEntry);

    return RETURN_OK;
}

ReturnValue POL_copyPolygon(XPDATA *xpdata1, XPDATA *xpdata2, unsigned char type, PointEntry **pe_array, unsigned int index)
{
    POLYGON *pltbl;
    ATTR *attbl;
    ReturnValue ret;
    int i;
    
    if((xpdata1 == NULL) || (xpdata2 == NULL))
        return RETURN_ERR_NULLARG;

    if(index > xpdata2->nbPolygon)
        return RETURN_ERR_INVALIDARG;
    
    // is this polygon already inside?
    for(i = 0; i < xpdata1->nbPolygon; i++) {
        if(POL_comparePolygons(xpdata1, i, xpdata2, index) == 1)
            return RETURN_OK;
    }
    // add storage
    if(xpdata1->nbPolygon == 0) {
        pltbl = (POLYGON *)malloc(sizeof(POLYGON));
        attbl = (ATTR *)malloc(sizeof(ATTR));
    } else {
        pltbl = (POLYGON *)realloc(xpdata1->pltbl, (xpdata1->nbPolygon+1) * sizeof(POLYGON));
        attbl = (ATTR *)realloc(xpdata1->attbl, (xpdata1->nbPolygon+1) * sizeof(ATTR));
    }
    if((pltbl == NULL) || (attbl == NULL))
        return RETURN_ERR_ALLOC;

    xpdata1->pltbl = pltbl;
    xpdata1->attbl = attbl;
    // insert ATTR
    memcpy(attbl + xpdata1->nbPolygon, xpdata2->attbl + index, sizeof(ATTR));
    // insert POLYGON normals
    memcpy((pltbl + xpdata1->nbPolygon)->norm, (xpdata2->pltbl + index)->norm, sizeof(VECTOR));
    // insert POLYGON Vertices (new point indices)
    for(i = 0; i < 4; i++) {
        ret = POL_copyPoint(xpdata1, xpdata2, type, pe_array, (xpdata2->pltbl + index)->Vertices[i], &((pltbl + xpdata1->nbPolygon)->Vertices[i]));
        if(ret != RETURN_OK)
            return ret;
    }
    xpdata1->nbPolygon += 1;
    
    return RETURN_OK;
}

ReturnValue POL_removeDoubles(XPDATA *data, unsigned char type, unsigned char free_old)
{
    XPDATA data2;
    ReturnValue ret;
    int i;
    PointEntry *pe_array = NULL;
    
    if(data == NULL)
        return RETURN_ERR_NULLARG;
        
    if((data->nbPoint == 0) || (data->nbPolygon == 0))
        return RETURN_OK;
    
    // create a new object without duplicates
    memset(&data2, 0, (type == IS_PDATA ? sizeof(PDATA) : sizeof(XPDATA))); 
    for(i = 0; i < data->nbPolygon; i++) {
        ret = POL_copyPolygon(&data2, data, type, &pe_array, i);
        if(ret != RETURN_OK)
            return ret;
    }
    //
    if(free_old)
        POL_clearXPDATA(data, type);
    // make the given object the new one
    memcpy(data, &data2, (type == IS_PDATA ? sizeof(PDATA) : sizeof(XPDATA))); 

    return RETURN_OK;
}

// returns the positive index of the first polygon
// which matches all constraints
// or a negative error code
// NOTE: type not used by now
int POL_searchPolygon(XPDATA *data, unsigned char type, Uint16 start, Uint16 end, SearchData *search)
{
    int i, j, matches;
    
    if((data == NULL) || (search == NULL))
        return -2;
    
    if((start > end) || (end >= data->nbPolygon))
        return -3;
        
    // linear search through all polygons between start and end
    for(i = start; i < end; i++) {
        // check all contraints
        matches = 0;
        for(j = 0; j < search->n; j++) {
            switch((search->constraint + j)->type) {
                case SEARCH_ATTR_FLAG:
                    if(((data->attbl + i)->flag & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
                    
                case SEARCH_ATTR_SORT:
                    if(((data->attbl + i)->sort & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
                    
                case SEARCH_ATTR_TEXNO:
                    if(((data->attbl + i)->texno & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
                    
                case SEARCH_ATTR_ATRB:
                    if(((data->attbl + i)->atrb & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
                    
                case SEARCH_ATTR_COLNO:
                    if(((data->attbl + i)->colno & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
                    
                case SEARCH_ATTR_GSTB:
                    if(((data->attbl + i)->gstb & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
                    
                case SEARCH_ATTR_DIR:
                    if(((data->attbl + i)->dir & (search->constraint + j)->checkBits) == ((search->constraint + j)->checkValue & (search->constraint + j)->checkBits))
                        matches++;
                    break;
            }
        }
        // found a matching polygon?
        if(matches == search->n)
            return i;
    }
    // none found
    return -1;
}

// NOTE: type not used by now
ReturnValue POL_computeGround(GroundData *ground, XPDATA *data, unsigned char type, Uint16 index, unsigned char typeID)
{
    if((ground == NULL) || (data == NULL))
        return RETURN_ERR_NULLARG;
        
    ground->typeID = typeID;

#ifdef GROUND_SATURN
        POINT *p0 = NULL, *p1 = NULL, *p2 = NULL;
        
        // 3 points
        p0 = data->pntbl + (data->pltbl + index)->Vertices[0];
        p1 = data->pntbl + (data->pltbl + index)->Vertices[1];
        p2 = data->pntbl + (data->pltbl + index)->Vertices[2];
        
        // only test for difference
        if((POL_comparePoints(*p0, *p1, XYZ) == 1) || (POL_comparePoints(*p0, *p2, XYZ) == 1) || (POL_comparePoints(*p1, *p2, XYZ) == 1)) {
            error_printf("The first three points of polygon %i are not different\nthis is not a regular triangle or quad!\n", index);
            return RETURN_ERR_UNKNOWN;
        }
        
        // compute the normal
        {
            double a[XYZ], b[XYZ], n[XYZ], l;
            
            // two vectors
            a[X] = toFLOAT((*p1)[X] - (*p0)[X]);
            a[Y] = toFLOAT((*p1)[Y] - (*p0)[Y]);
            a[Z] = toFLOAT((*p1)[Z] - (*p0)[Z]);

            b[X] = toFLOAT((*p2)[X] - (*p0)[X]);
            b[Y] = toFLOAT((*p2)[Y] - (*p0)[Y]);
            b[Z] = toFLOAT((*p2)[Z] - (*p0)[Z]);
            
            // cross product of a, b is perpendicular normal
            n[X] = a[Y]*b[Z] - a[Z]*b[Y];
            n[Y] = a[Z]*b[X] - a[X]*b[Z];
            n[Z] = a[X]*b[Y] - a[Y]*b[X];
            
            // normal the normal
            l = sqrt( n[X]*n[X] + n[Y]*n[Y] + n[Z]*n[Z]);
            n[X] = n[X] / l;
            n[Y] = n[Y] / l;
            n[Z] = n[Z] / l;

            // make a fast test if the ground is facing up
            if(n[Y] >= 0) {
                warning_printf("Normal(%f, %f, %f) does not point upwards in Saturn coo.-system!\nFlipping the normal...\n", n[X], n[Y], n[Z]);
                n[X] = -n[X];
                n[Y] = -n[Y];
                n[Z] = -n[Z];
            }
            
            // insert the normal to ground data
            ground->direction.x = toFIXED(n[X]);
            ground->direction.z = toFIXED(n[Z]);
            if(n[Y] == 0) {
                ground->normal.x = toFIXED(0);
                ground->normal.z = toFIXED(0);
            } else {
                ground->normal.x = toFIXED(n[X] / (-n[Y]));
                ground->normal.z = toFIXED(n[Z] / (-n[Y]));
            }
            
            // compute and insert center
            if(POL_findPolygonMid(data, index, ground->center, XYZ) != RETURN_OK) {
                error_printf("Error computing ground mid!\n");
            }
            note_printf("Center at (%f, %f, %f)\n", toFLOAT(ground->center[X]), toFLOAT(ground->center[Y]), toFLOAT(ground->center[Z]));
        }
        

#else   
    int i, diffPoints;
    
    // axis aligned bounding rectangle
    {
        FIXED min[XYZ] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
        FIXED max[XYZ] = {0x80000000, 0x80000000, 0x80000000};
        int i;
        
        for(i = 0; i < 4; i++) {
            ret = POL_updateMinMax(data->pntbl + (data->pltbl + index)->Vertices[i], 1, min, max, XYZ);
            if(ret != RETURN_OK)
                return ret;
        }
        ground->min.x = min[X];
        ground->max.x = max[X];
        ground->min.z = min[Z];
        ground->max.z = max[Z];
    }
    
    // side factors for inside check
    {
        POINT *p0, *p1;
        double A, B, C;
        int i;
        
        for(i = 0; i < 4; i++) {
            p0 = data->pntbl + (data->pltbl + index)->Vertices[i];
            if(i == 3)	// last one is first one
                p1 = data->pntbl + (data->pltbl + index)->Vertices[0];
            else p1 = data->pntbl + (data->pltbl + index)->Vertices[i+1];
            A = toFLOAT((*p1)[X]) - toFLOAT((*p0)[X]);
            B = toFLOAT((*p1)[Z]) - toFLOAT((*p0)[Z]);
            C = toFLOAT((*p0)[X]) * B - toFLOAT((*p0)[Z]) * A;
            // check division by zero
            if(C == 0) {
                ground->sideFactors[i].x = toFIXED( 0 );
                ground->sideFactors[i].z = toFIXED( 0 );
            } else {
                ground->sideFactors[i].x = toFIXED( B / (-C) );
                ground->sideFactors[i].z = toFIXED( A / (-C) );
            }
            note_printf("sideFactors(%f, %f)\n", toFLOAT(ground->sideFactors[i].x), toFLOAT(ground->sideFactors[i].z));
        }
    }
    
    
    // height factors for height computation
    // assume the first three points are different
    {
        double D1x, D1y, D1z, D2x, D2y, D2z, A; //, B, C, D, E, F, G, H, I, J, K, L, M, N;
        POINT *p0 = NULL, *p1 = NULL, *p2 = NULL;
        int i, diffPoints;
        
        // be flexible and search 3 differnt points
        p0 = data->pntbl + (data->pltbl + index)->Vertices[0];
        diffPoints = 1;
            // search second point
        for(i = 1; i < 4; i++) {
            if(POL_comparePoints(*p0, *(data->pntbl + (data->pltbl + index)->Vertices[i]), XYZ) != 1) {
                p1 = data->pntbl + (data->pltbl + index)->Vertices[i];
                diffPoints++;
                i++;
                break;
            }
        }
        if(diffPoints < 2)
            return RETURN_ERR_UNKNOWN;
        for( i = i; i < 4; i++) {
            if((POL_comparePoints(*p0, *(data->pntbl + (data->pltbl + index)->Vertices[i]), XYZ) != 1) && (POL_comparePoints(*p1, *(data->pntbl + (data->pltbl + index)->Vertices[i]), XYZ) != 1)) {
                p2 = data->pntbl + (data->pltbl + index)->Vertices[i];
                diffPoints++;
                break;
            }
        }
        if(diffPoints < 3)
            return RETURN_ERR_UNKNOWN;
        
        // WARNING!
        // because of the special 3D coordinate system of the saturn
        // Y axis is assigned as height
        // and Z is second coordinate to X
        D1x = toFLOAT((*p1)[X] - (*p0)[X]);
        D1z = toFLOAT((*p1)[Y] - (*p0)[Y]);
        D1y = toFLOAT((*p1)[Z] - (*p0)[Z]);
        D2x = toFLOAT((*p2)[X] - (*p0)[X]);
        D2z = toFLOAT((*p2)[Y] - (*p0)[Y]);
        D2y = toFLOAT((*p2)[Z] - (*p0)[Z]);
        
        A = (D1x * D2y) - (D2x * D1y);
#if 0
        B = toFLOAT((*p0)[X]) * D1y - toFLOAT((*p0)[Z]) * D1x;
        if(D1x == 0)
            C = 0;
        else C = 1 / D1x;
        if(A == 0)
            D = 0;
        else D = 1 / A;
        E = D * D2x * C;
        F = D1x * E;
        G = C + D1y  * E;
        H = -(B*E) - (toFLOAT((*p0)[X]) * C);
        I = D1x * D;
        J = - D1y * D;
        K = B * D;
        L = J*D2z + G*D1z;
        M = I*D2z - F*D1z;
        N = H*D1z + K*D2z + toFLOAT((*p0)[Y]);
        
        ground->heightFactors[X] = toFIXED(L);
        ground->heightFactors[Y] = toFIXED(N);
        ground->heightFactors[Z] = toFIXED(M);
#else
        ground->heightFactors[X] = toFIXED( (D1z * D2y - D2z * D1y) / A );
        ground->heightFactors[Y] = toFIXED( (D1z * (toFLOAT((*p0)[Z]) * D2x - toFLOAT((*p0)[X]) * D2y) + D2z * (toFLOAT((*p0)[X]) * D1y - toFLOAT((*p0)[Z]) * D1x)) / A) + (*p0)[Y];
        ground->heightFactors[Z] = toFIXED( (D2z * D1x - D1z * D2x) / A );

#endif
        note_printf("heightFactors(%f, %f, %f)\n", toFLOAT(ground->heightFactors[X]), toFLOAT(ground->heightFactors[Y]), toFLOAT(ground->heightFactors[Z]));

    }
#endif    
    
    return RETURN_OK;
}


#ifdef GROUND_SATURN

#else
ReturnValue POL_checkGround(GroundData *ground, FIXED *pos, unsigned char *hit, unsigned char *typeID, FIXED *height)
{
    int i;
    FIXED val;
    
    if((ground == NULL) || (pos == NULL))
        return RETURN_ERR_NULLARG;
        
    *hit = 0;

    // bounding rectangle check
    if((pos[X] < ground->min.x) || (pos[X] > ground->max.x) || (pos[Z] < ground->min.z) || (pos[Z] > ground->max.z)) {
        return RETURN_OK;
    }
    
    // side check
    for(i = 0; i < 4; i++) {
        val = slMulFX(pos[X], ground->sideFactors[i].x) - slMulFX(pos[Z], ground->sideFactors[i].z);
        if(val > toFIXED(1))
            return RETURN_OK;
    }
    // passed, so it's definitely inside
    *hit = 1;
    if(typeID != NULL)
        *typeID = ground->typeID;
    // compute height
    if(height != NULL)
        *height = slMulFX(pos[X], ground->heightFactors[X]) + slMulFX(pos[Z], ground->heightFactors[Z]) + ground->heightFactors[Y];

    return RETURN_OK;
}
#endif

