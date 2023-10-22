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

#include "RB_TXT_STRING_INT.h"

ReturnValue TXTSTR_delCharTbl(CharTable *chartbl)
{
    unsigned char h;
    
    if(chartbl == NULL)
        return RETURN_ERR_NULLARG;

    if((chartbl->width != 0) && (chartbl->height != 0)) {
        for(h = 0; h < chartbl->height; h++) {
            free(*(chartbl->chars + h));
        }
        free(chartbl->chars);
    }
    memset(&(chartbl), 0, sizeof(CharTable));
    
    return RETURN_OK;
}

ReturnValue TXTSTR_getCharTblSize(char *string, unsigned char *w, unsigned char *h)
{
    unsigned short o;
    unsigned char width, height, maxwidth;

    if(string == NULL)
        return RETURN_ERR_NULLARG;

    // 1st pass: parse string to determine width and height
    o = width = maxwidth = height = 0;
    while(*(string + o) != '\0') {
        switch(*(string + o)) {
            case '\n':
                if(width > maxwidth)
                    maxwidth = width;
                    
                width = 0;
                height++;
                break;

            default:
                width++;
        }
        o++;
    }
    // what if the last row was not followed by a newline:
    if(width != 0)
        height++;
    
    if(w != NULL)
        *w = maxwidth;
    if(h != NULL)
        *h = height;
    
    return RETURN_OK;
}

ReturnValue TXTSTR_setCharTbl(CharTable *chartbl, char *string, unsigned char xPos, unsigned char yPos, unsigned char width, unsigned char height)
{
    unsigned char i;
    unsigned short o;
    
    if((chartbl == NULL) || (string == NULL))
        return RETURN_ERR_NULLARG;
    
    TXTSTR_delCharTbl(chartbl);
    
    // 2 passes:
    // 1st pass: parse string to determine width and height
        // has already been done before
    // 2nd pass: create the char table
        // memory alloc
    chartbl->chars = (char **)malloc(height * sizeof(char *));
    if(chartbl->chars == NULL)
        return RETURN_ERR_ALLOC;
        
    for(o = 0; o < height; o++) {
        // skip trash
        while(!isprint(*string) && (*string != '\0')) {
            string++;
        }
        // get memory
        *(chartbl->chars + o) = (char *)malloc(width * sizeof(char));
        if(*(chartbl->chars + o) == NULL)
            return RETURN_ERR_ALLOC;
        // insert data
        for(i = 0; i < width; i++) {
            if((*string != '\0') && (*string != '\n')) {
                *(*(chartbl->chars + o) + i) = *string;
                string++;
            } else *(*(chartbl->chars + o) + i) = ' ';
        }
    }
    
    chartbl->width = width;
    chartbl->height = height;
    chartbl->pos.x = xPos;
    chartbl->pos.y = yPos;
    
    return RETURN_OK;
}


ReturnValue TXTSTR_displayChartbl(CharTable *chartbl, void *scroll, TxtDrawMode drawMode)
{
    ReturnValue ret;
    int i, j;
    
     /* print menu */
     for(i = 0; i < chartbl->height; i++) {
          for(j = 0; j < chartbl->width; j++) {
                ret = TXTSCR_putc(scroll, chartbl->pos.x + j + j*chartbl->spacing.x, chartbl->pos.y + i + i*chartbl->spacing.y, drawMode, *(*(chartbl->chars + i) + j));
                if(ret != RETURN_OK)
                    return ret;
          }
     }
     return RETURN_OK;
}

ReturnValue TXTSTR_deleteCursor(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode)
{
    // string cursor
    return TXTSCR_clearRectangle(scroll, strbuf->pos.x, strbuf->pos.y + 1, strbuf->length.max, 1, drawMode);
}


ReturnValue TXTSTR_displayCursor(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode)
{
    // string cursor
    return TXTSCR_putc(scroll, strbuf->pos.x + strbuf->cursor, strbuf->pos.y + 1, drawMode, '^');
}

ReturnValue TXTSTR_displayString(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode)
{    
//    return TXTSCR_printf(strinp->scroll, strinp->layout.string.x, strinp->layout.string.y, strinp->drawMode, "%iof%i: %s", (int)strinp->strbuf.length.string, (int)strinp->strbuf.length.max, strinp->strbuf.string);    
    return TXTSCR_printf(scroll, strbuf->pos.x, strbuf->pos.y, drawMode, strbuf->string);
}

ReturnValue TXTSTR_deleteString(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode)
{    
    return TXTSCR_clearRectangle(scroll, strbuf->pos.x, strbuf->pos.y, strbuf->length.max, 1, drawMode);
}

ReturnValue TXTSTR_redrawString(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode)
{
    ReturnValue ret;
    
    if(strbuf == NULL)
        return RETURN_ERR_NULLARG;

	ret = TXTSTR_deleteString(strbuf, scroll, drawMode);
    if(ret != RETURN_OK)
        return ret;
        
    return TXTSTR_displayString(strbuf, scroll, drawMode);
}


ReturnValue TXTSTR_setString(StringBuffer *strbuf, char *string, unsigned char xPos, unsigned char yPos, unsigned char maxLength)
{
    if(strbuf == NULL)
        return RETURN_ERR_ALLOC;

    if(string != NULL) {
        strbuf->string = (char *)strdup(string);
        if(strbuf->string == NULL)
            return RETURN_ERR_ALLOC;
        
        strbuf->length.string = strlen(string);
        strbuf->length.buffer = strbuf->length.string;
        strbuf->cursor = strbuf->length.string - 1;
    } else {
        strbuf->string = (char *)calloc((TXTSTR_CHAR_ALLOC_UNIT + 1), sizeof(char));
        if(strbuf->string == NULL)
            return RETURN_ERR_ALLOC;
        
        strbuf->length.string = 0;
        strbuf->length.buffer = TXTSTR_CHAR_ALLOC_UNIT;
        strbuf->cursor = 0;
    }
     *(strbuf->string + strbuf->length.buffer) = '\0';
     
	/* search cursor position:
	 * place it behind the existing name */
//	while((strbuf->length.string > 0) && (*(strbuf->string + strbuf->length.string) == ' ')) {
//          strbuf->length.string--;
//    }

    strbuf->length.max = maxLength;
    
    strbuf->pos.x = xPos;
    strbuf->pos.y = yPos;
    
    return RETURN_OK;
}


ReturnValue TXTSTR_addChar(StringBuffer *strbuf, char c, unsigned char *redrawCursor, unsigned char *redrawString)
{
    unsigned char nChars;
    char *ptr;
    
    if(strbuf == NULL)
        return RETURN_ERR_NULLARG;
        
    // have to get more memory?
    if((strbuf->cursor == (strbuf->length.buffer - 1)) && (strbuf->length.buffer < strbuf->length.max)) {
        // determine number of chars to allocate
        if((strbuf->length.buffer + TXTSTR_CHAR_ALLOC_UNIT) > strbuf->length.max)
            nChars = strbuf->length.max - strbuf->length.buffer;
        else nChars = TXTSTR_CHAR_ALLOC_UNIT;
        // allocate
        ptr = (char *)realloc(strbuf->string, (strbuf->length.buffer + nChars) * sizeof(char));

        if(ptr == NULL)
            return RETURN_ERR_ALLOC;
            
        strbuf->string = ptr;
        strbuf->length.buffer += nChars;
    }
    // add selected char to string
    *(strbuf->string + strbuf->cursor) = c;
    *redrawString = 1;
    // do we have increased the string length?
    if((strbuf->cursor == ((strbuf->length.string > 0 ? strbuf->length.string-1 : 0)  )) && (strbuf->length.string < strbuf->length.buffer)) {
        strbuf->length.string++;
    }
    // can we move the cursor right
    if(strbuf->cursor < (strbuf->length.buffer - 1)) {
        *redrawCursor = 1;
        strbuf->cursor++;
    }
    return RETURN_OK;
}


ReturnValue TXTSTR_removeChar(StringBuffer *strbuf, unsigned char *redrawCursor, unsigned char *redrawString)
{
    char *ptr;
    if(strbuf == NULL)
        return RETURN_ERR_NULLARG;

    /* delete one character */
    if((strbuf->length.string > 0) && (strbuf->cursor == (strbuf->length.string - 1))) {
        *(strbuf->string + strbuf->length.string - 1) = '\0';
        strbuf->length.string--;
        *redrawString = 1;
        strbuf->cursor--;
        *redrawCursor = 1;
        // is it time to perform a realloc?
        if((strbuf->length.buffer - strbuf->length.string) >= TXTSTR_CHAR_ALLOC_UNIT) {
            // allocate
            ptr = (char *)realloc(strbuf->string, (strbuf->length.buffer - TXTSTR_CHAR_ALLOC_UNIT) * sizeof(char));

            if(ptr == NULL)
                return RETURN_ERR_ALLOC;
            
            strbuf->string = ptr;
            strbuf->length.buffer -= TXTSTR_CHAR_ALLOC_UNIT;
        }
    } else *(strbuf->string + strbuf->cursor) = ' ';
    
    return RETURN_OK;
}

ReturnValue TXTSTR_cursorLeft(StringBuffer *strbuf, unsigned char *redrawCursor)
{
    if(strbuf == NULL)
        return RETURN_ERR_NULLARG;
        
    /* move stringcursor left */
    if(strbuf->length.string > 0) {
        if(strbuf->cursor == 0) 
            strbuf->cursor = strbuf->length.string - 1;
        else strbuf->cursor--;
        *redrawCursor = 1;
    }
    return RETURN_OK;
}

ReturnValue TXTSTR_cursorRight(StringBuffer *strbuf, unsigned char *redrawCursor)
{
    if(strbuf == NULL)
        return RETURN_ERR_NULLARG;
        
    /* move stringcursor right */
    if(strbuf->length.string > 0) {
        if(strbuf->cursor == (strbuf->length.string - 1))
            strbuf->cursor = 0;
        else strbuf->cursor++;
        *redrawCursor = 1;
    }
    return RETURN_OK;
}

ReturnValue TXTSTR_setName(Name *name, char *string, unsigned char xPos, unsigned char yPos)
{
    if((name == NULL) || (string == NULL))
        return RETURN_ERR_NULLARG;

    name->string = (char *)strdup(string);
    if(name->string == NULL)
        return RETURN_ERR_ALLOC;

    name->pos.x = xPos;
    name->pos.y = yPos;
    
    return RETURN_OK;
}


ReturnValue TXTSTR_displayName(Name *name, void *scroll, TxtDrawMode drawMode)
{
    if(name == NULL)
        return RETURN_ERR_NULLARG;
        
    return TXTSCR_printf(scroll, name->pos.x, name->pos.y, drawMode, name->string);
}
