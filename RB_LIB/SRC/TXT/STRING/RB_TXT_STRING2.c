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

#include <RB_TXT_STRING.h>
#include "RB_TXT_STRING2_INT.h"


char *TXTSTR2_getVersion()
{
    return TXTSTR2_VERSION;
}

ReturnValue TXTSTR2_init(void **ptr, void *scroll)
{
    StringInput2 *strinp = (StringInput2 *)calloc(1, sizeof(StringInput2));
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

    strinp->scroll = scroll;
    strinp->drawMode = TXT_DRAW_BOTH;
    
    *ptr = strinp;

    return RETURN_OK;
}

ReturnValue TXTSTR2_setName(void *ptr, char *name, unsigned char xPos, unsigned char yPos)
{
    StringInput2 *strinp = (StringInput2 *)ptr;
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

	return TXTSTR_setName(&(strinp->name), name, xPos, yPos);
}


ReturnValue TXTSTR2_setString(void *ptr, char *string, unsigned char xPos, unsigned char yPos, unsigned char maxLength)
{
    StringInput2 *strinp = (StringInput2 *)ptr;
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

	return TXTSTR_setString(&(strinp->strbuf), string, xPos, yPos, maxLength);
}


ReturnValue TXTSTR2_setCharTbl(void *ptr, char *string, unsigned char xPos, unsigned char yPos)
{
    StringInput2 *strinp = (StringInput2 *)ptr;
    unsigned char width, height;
    
    if((strinp == NULL) || (string == NULL))
        return RETURN_ERR_NULLARG;

    TXTSTR_getCharTblSize(string, &width, &height);
    
	return TXTSTR_setCharTbl(&(strinp->chartbl), string, xPos, yPos, width, height);
}


ReturnValue TXTSTR2_deleteCursorH(StringInput2 *strinp)
{
    // horizontal cursor
    return TXTSCR_clearRectangle(strinp->scroll, strinp->chartbl.pos.x, strinp->chartbl.pos.y - 1, strinp->chartbl.width, 1, strinp->drawMode);
}

ReturnValue TXTSTR2_deleteCursorV(StringInput2 *strinp)
{
    // vertical cursor
    return TXTSCR_clearRectangle(strinp->scroll, strinp->chartbl.pos.x - 1, strinp->chartbl.pos.y, 1, strinp->chartbl.height, strinp->drawMode);
}

ReturnValue TXTSTR2_displayCursorH(StringInput2 *strinp)
{
    // horizontal cursor
    return TXTSCR_putc(strinp->scroll, strinp->chartbl.pos.x + strinp->cursor.x, strinp->chartbl.pos.y - 1, strinp->drawMode, '+');
}

ReturnValue TXTSTR2_displayCursorV(StringInput2 *strinp)
{
    // vertical cursor
    return TXTSCR_putc(strinp->scroll, strinp->chartbl.pos.x - 1, strinp->chartbl.pos.y + strinp->cursor.y, strinp->drawMode, '+');
}




ReturnValue TXTSTR2_redrawAll(void *ptr)
{
    StringInput2 *strinp = (StringInput2 *)ptr;
    
    if(strinp == NULL)
        return RETURN_ERR_NULLARG;

    // clear
    TXTSCR_clear(strinp->scroll, TXT_DRAW_BOTH);

    // draw name
	TXTSTR_displayName(&(strinp->name), strinp->scroll, strinp->drawMode);
    
    // draw string
    TXTSTR_redrawString(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
    TXTSTR_displayCursor(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
    
     /* print menu */
    TXTSTR_displayChartbl(&(strinp->chartbl), strinp->scroll, strinp->drawMode);
//    TXTSTR2_displayChartbl(strinp);
    TXTSTR2_displayCursorH(strinp);
    TXTSTR2_displayCursorV(strinp);
       
    return RETURN_OK;
}



void TXTSTR2_executeInputs(StringInput2 *strinp)
{
    SET_COMMAND_VAR;

    GET_COMMAND;
    
    /* examine the pressed keys */
    if(COMMAND_L_PRESSED) {
        /* move stringcursor left */
        TXTSTR_cursorLeft(&(strinp->strbuf), &(strinp->status.redrawCursor));
    }else if(COMMAND_R_PRESSED) {
        /* move stringcursor right */
        TXTSTR_cursorRight(&(strinp->strbuf), &(strinp->status.redrawCursor));
    }


    if(COMMAND_UP_PRESSED) {
        /* item up */
        TXTSTR2_deleteCursorV(strinp);
        if(strinp->cursor.y == 0)
            strinp->cursor.y = strinp->chartbl.height - 1;
        else strinp->cursor.y--;
        TXTSTR2_displayCursorV(strinp);
    }else if(COMMAND_DOWN_PRESSED) {
        /* item down */
        TXTSTR2_deleteCursorV(strinp);
        if(strinp->cursor.y == (strinp->chartbl.height - 1))
            strinp->cursor.y = 0;
        else strinp->cursor.y++;
        TXTSTR2_displayCursorV(strinp);
    }
    if(COMMAND_LEFT_PRESSED) {
        /* item left */
        TXTSTR2_deleteCursorH(strinp);
        if(strinp->cursor.x == 0)
            strinp->cursor.x = strinp->chartbl.width - 1;
        else strinp->cursor.x--;
        TXTSTR2_displayCursorH(strinp);
    } else if(COMMAND_RIGHT_PRESSED) {
        /* item right */
        TXTSTR2_deleteCursorH(strinp);
        if(strinp->cursor.x == (strinp->chartbl.width - 1))
            strinp->cursor.x = 0;
        else strinp->cursor.x++;
        TXTSTR2_displayCursorH(strinp);
    }
    if(COMMAND_B_PRESSED) {
        TXTSTR_removeChar(&(strinp->strbuf), &(strinp->status.redrawCursor), &(strinp->status.redrawString));
    }
    if(COMMAND_A_PRESSED || COMMAND_C_PRESSED) {
        TXTSTR_addChar(&(strinp->strbuf), *(*(strinp->chartbl.chars + strinp->cursor.y) + strinp->cursor.x), &(strinp->status.redrawCursor), &(strinp->status.redrawString));
#if 0        
        // have to get more memory?
        if((strinp->strbuf.cursor == (strinp->strbuf.length.buffer - 1)) && (strinp->strbuf.length.buffer < strinp->strbuf.length.max)) {
            unsigned char nChars;
            char *ptr;
            // determine number of chars to allocate
            if((strinp->strbuf.length.buffer + TXTSTR2_CHAR_ALLOC_UNIT) > strinp->strbuf.length.max)
                nChars = strinp->strbuf.length.max - strinp->strbuf.length.buffer;
            else nChars = TXTSTR2_CHAR_ALLOC_UNIT;
            // allocate
            ptr = (char *)realloc(strinp->strbuf.string, (strinp->strbuf.length.buffer + nChars) * sizeof(char));

            if(ptr == NULL)
                return;
            
            strinp->strbuf.string = ptr;
            strinp->strbuf.length.buffer += nChars;
        }
        // add selected char to string
        *(strinp->strbuf.string + strinp->strbuf.cursor) = *(*(strinp->chartbl.chars + strinp->cursor.y) + strinp->cursor.x);
        strinp->status.redrawString = 1;
        // do we have increased the string length?
        if((strinp->strbuf.cursor == ((strinp->strbuf.length.string > 0 ? strinp->strbuf.length.string-1 : 0)  )) && (strinp->strbuf.length.string < strinp->strbuf.length.buffer)) {
            strinp->strbuf.length.string++;
        }
        // can we move the cursor right
        if(strinp->strbuf.cursor < (strinp->strbuf.length.buffer - 1)) {
            strinp->status.redrawCursor = 1;
            strinp->strbuf.cursor++;
        }
#endif        
    }
    if(COMMAND_START_PRESSED) {
            /* wait for release */
//            waitRelease();
        strinp->status.leave = 1;
    }
    return;
}


ReturnValue TXTSTR2_execute(void *ptr)
{
    StringInput2 *strinp = (StringInput2 *)ptr;
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;
     
    TXTSTR2_redrawAll(ptr);
    strinp->status.leave = 0;
    
//    TXTSCR_printf(strinp->scroll, 5, 10, TXT_DRAW_BOTH, "Hello, I'm here!");

//	waitRelease();
	while(1) {
        strinp->status.redrawString = strinp->status.redrawCursor = 0;
        
        TXTSTR2_executeInputs(strinp);
        // now do further reactions on input
        if(strinp->status.leave == 1)
        	break;
            
        if(strinp->status.redrawString == 1)
            TXTSTR_redrawString(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
        
        if(strinp->status.redrawCursor == 1) {
            TXTSTR_deleteCursor(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
            TXTSTR_displayCursor(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
        }
	}
    return RETURN_OK;
}



