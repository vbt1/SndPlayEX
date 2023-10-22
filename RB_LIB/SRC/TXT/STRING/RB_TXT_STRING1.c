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
#include "RB_TXT_STRING1_INT.h"


char *TXTSTR1_getVersion()
{
    return TXTSTR1_VERSION;
}

ReturnValue TXTSTR1_init(void **ptr, void *scroll)
{
    StringInput1 *strinp = (StringInput1 *)calloc(1, sizeof(StringInput1));
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

    strinp->scroll = scroll;
    strinp->drawMode = TXT_DRAW_BOTH;
    
    *ptr = strinp;

    return RETURN_OK;
}


ReturnValue TXTSTR1_setCharTbl(void *ptr, char *string, unsigned char xPos, unsigned char yPos, AttachTo attTo, AttachAs attAs)
{
    StringInput1 *strinp = (StringInput1 *)ptr;
    unsigned char width, height;
    
    if((strinp == NULL) || (string == NULL))
        return RETURN_ERR_NULLARG;

    if((attTo > ATTACH_TO_Z) || (attAs > ATTACH_AS_SECOND))
        return RETURN_ERR_INVALIDARG;

    TXTSTR_getCharTblSize(string, &width, &height);
    if((width != 3) || (height != 3))
        return RETURN_ERR_INVALIDARG;
    
    // spaces for cursor
    strinp->chartbls[attTo][attAs].spacing.x = 1;
    strinp->chartbls[attTo][attAs].spacing.y = 1;
    
	return TXTSTR_setCharTbl(&(strinp->chartbls[attTo][attAs]), string, xPos, yPos, width, height);
}


ReturnValue TXTSTR1_setString(void *ptr, char *string, unsigned char xPos, unsigned char yPos, unsigned char maxLength)
{
    StringInput1 *strinp = (StringInput1 *)ptr;
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

	return TXTSTR_setString(&(strinp->strbuf), string, xPos, yPos, maxLength);
}

ReturnValue TXTSTR1_setName(void *ptr, char *name, unsigned char xPos, unsigned char yPos)
{
    StringInput1 *strinp = (StringInput1 *)ptr;
    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

	return TXTSTR_setName(&(strinp->name), name, xPos, yPos);
}



/* draw this: ---
 *            | |
 *            ---
 */
 #if 0
void drawCursor(char *target)
{
    *(target - 1) = '|';
    *(target + 1) = '|';
    *(target - EDITOR_WIDTH - 1 - 1) = '-';
    *(target - EDITOR_WIDTH - 1 + 0) = '-';
    *(target - EDITOR_WIDTH - 1 + 1) = '-';
    *(target + EDITOR_WIDTH + 1 - 1) = '-';
    *(target + EDITOR_WIDTH + 1 + 0) = '-';
    *(target + EDITOR_WIDTH + 1 + 1) = '-';
    
    return;
}
#endif

ReturnValue TXTSTR1_displayCharTbl(CharTable *chartbl, StringInput1 *strinp, unsigned char active)
{
	int i;
    
    if((strinp == NULL) || (chartbl == NULL))
        return RETURN_ERR_NULLARG;
	
    TXTSTR_displayChartbl(chartbl, strinp->scroll, strinp->drawMode);

	/* char table cursor */
	if(active == 1) {
        // surrounding active char table
 		for(i = 0; i < (chartbl->width + 2); i++) {
            TXTSCR_putc(strinp->scroll, chartbl->pos.x - 1 + i, chartbl->pos.y - 1, strinp->drawMode, '-');
            TXTSCR_putc(strinp->scroll, chartbl->pos.x - 1 + i, chartbl->pos.y + chartbl->height, strinp->drawMode, '-');
 		}
 		for(i = 0; i < chartbl->height; i++) {
            TXTSCR_putc(strinp->scroll, chartbl->pos.x - 1, chartbl->pos.y + i, strinp->drawMode, '|');
            TXTSCR_putc(strinp->scroll, chartbl->pos.x + chartbl->width, chartbl->pos.y + i, strinp->drawMode, '|');
 		}
        // surrounding active char
#if 0        
    *(target - 1) = '|';
    *(target + 1) = '|';
    *(target - EDITOR_WIDTH - 1 - 1) = '-';
    *(target - EDITOR_WIDTH - 1 + 0) = '-';
    *(target - EDITOR_WIDTH - 1 + 1) = '-';
    *(target + EDITOR_WIDTH + 1 - 1) = '-';
    *(target + EDITOR_WIDTH + 1 + 0) = '-';
    *(target + EDITOR_WIDTH + 1 + 1) = '-';
        drawCursor(target);
#endif
 	}

 	return RETURN_OK;
}


ReturnValue TXTSTR1_redrawAll(void *ptr)
{
    int i;
    StringInput1 *strinp = (StringInput1 *)ptr;
    
    if(strinp == NULL)
        return RETURN_ERR_NULLARG;

    // clear
    TXTSCR_clear(strinp->scroll, TXT_DRAW_BOTH);

    // draw name
	TXTSTR_displayName(&(strinp->name), strinp->scroll, strinp->drawMode);

    // draw charset information
    TXTSCR_printf(strinp->scroll, strinp->name.pos.x, strinp->name.pos.y + 1, strinp->drawMode, "charset: %i", strinp->status.charset);
//        if(caps == 0)
//            sprintf(editorText + (EDITOR_WIDTH + 1) + 5, " OFF <- caps -> on");
//        else sprintf(editorText + (EDITOR_WIDTH + 1) + 5, " off <- caps -> ON");
    

    // draw charsets
    for(i = 0; i < 6; i++) {
        if((strinp->chartbls[i][strinp->status.charset].width > 0) && (strinp->chartbls[i][strinp->status.charset].height > 0))
            TXTSTR1_displayCharTbl(&(strinp->chartbls[i][strinp->status.charset]), strinp, (strinp->status.state == i ? 1 : 0));
    }
    
    // draw string
    TXTSTR_redrawString(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
    TXTSTR_displayCursor(&(strinp->strbuf), strinp->scroll, strinp->drawMode);
           
    return RETURN_OK;
}

void TXTSTR1_executeInputs(StringInput1 *strinp)
{
	SET_COMMAND_VAR;

	GET_COMMAND;
    
	/* state independend */
	if(COMMAND_L_PRESSED) {
        /* move stringcursor left */
        TXTSTR_cursorLeft(&(strinp->strbuf), &(strinp->status.redrawCursor));
 	} else if(COMMAND_R_PRESSED) {
        /* move stringcursor right */
        TXTSTR_cursorRight(&(strinp->strbuf), &(strinp->status.redrawCursor));
  	}

    if(strinp->status.state == STATE_PASSIVE)
	{	/* switch to an active state when button is pressed */
        strinp->cursor.x = 1;
        strinp->cursor.y = 1;
		if(COMMAND_START_PRESSED) {
            strinp->status.leave = 1;
            /* may need to cut off the spare character */
#if 0
            if(cursor == (length - 1)) {
                *txt = (char *)realloc(*txt, (length--) * sizeof(char));
                if(*txt == NULL)
                    return -1;
                *(*txt + length) = '\0';
            }else *(*txt + cursor) = cback;
			return 1;
#endif        
        } else if(COMMAND_A_PRESSED)
			strinp->status.state = STATE_ACTIVE_A;
		else if(COMMAND_B_PRESSED)
			strinp->status.state = STATE_ACTIVE_B;
		else if(COMMAND_C_PRESSED)
			strinp->status.state = STATE_ACTIVE_C;
		else if(COMMAND_X_PRESSED)
			strinp->status.state = STATE_ACTIVE_X;
		else if(COMMAND_Y_PRESSED)
			strinp->status.state = STATE_ACTIVE_Y;
		else if(COMMAND_Z_PRESSED)
			strinp->status.state = STATE_ACTIVE_Z;
		/* or alter characters */
		else if(COMMAND_RIGHT_PRESSED)
			strinp->status.charset = ATTACH_AS_SECOND;
		else if(COMMAND_LEFT_PRESSED)
			strinp->status.charset = ATTACH_AS_FIRST;
		else if(COMMAND_DOWN_PRESSED) {
            TXTSTR_removeChar(&(strinp->strbuf), &(strinp->status.redrawCursor), &(strinp->status.redrawString));
        }
		 
 	} else {
 		/* determine cursor position */
 		if(COMMAND_UP && strinp->cursor.x > 0)
   			strinp->cursor.x--;
		else if(COMMAND_DOWN && strinp->cursor.x < 2)
			strinp->cursor.x++;

 		if(COMMAND_LEFT && strinp->cursor.y > 0)
   			strinp->cursor.y--;
		else if(COMMAND_RIGHT && strinp->cursor.y < 2)
			strinp->cursor.y++;
		/* determine selected char */	
  		if(	((strinp->status.state == STATE_ACTIVE_A) && (COMMAND_A_RELEASED)) || 
    		((strinp->status.state == STATE_ACTIVE_B) && (COMMAND_B_RELEASED)) ||
      		((strinp->status.state == STATE_ACTIVE_C) && (COMMAND_C_RELEASED)) ||
        	((strinp->status.state == STATE_ACTIVE_X) && (COMMAND_X_RELEASED)) ||
         	((strinp->status.state == STATE_ACTIVE_Y) && (COMMAND_Y_RELEASED)) ||
          	((strinp->status.state == STATE_ACTIVE_Z) && (COMMAND_Z_RELEASED))) {
            /* insert at cursor position */
            TXTSTR_addChar(&(strinp->strbuf), *(*(strinp->chartbls[strinp->status.state][strinp->status.charset].chars + strinp->cursor.y) + strinp->cursor.x), &(strinp->status.redrawCursor), &(strinp->status.redrawString));
			strinp->status.state = STATE_PASSIVE;
  		}
  	}
}


ReturnValue TXTSTR1_execute(void *ptr)
{
    StringInput1 *strinp = (StringInput1 *)ptr;

    if(strinp == NULL)
        return RETURN_ERR_ALLOC;

    strinp->cursor.x = 1;
    strinp->cursor.y = 1;
    strinp->status.state = STATE_PASSIVE;
    strinp->status.charset = 0;

    TXTSTR1_redrawAll(ptr);
    strinp->status.leave = 0;
        
	while(1) {
        strinp->status.redrawString = strinp->status.redrawCursor = 0;
        
        TXTSTR1_executeInputs(strinp);
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

