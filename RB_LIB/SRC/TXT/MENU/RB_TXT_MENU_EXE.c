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

#include <RB_TXT_MENU.h>
#include "RB_TXT_MENU_INT.h"

void TXTMEN_executeCallback(TxtMenu *txtmen, TxtMenuCallback *callback)
{
    if(callback->func != NULL) {
        (*callback->func)(callback->item, callback->ptr);
        // need to redraw?
//        if(callback->redraw == 1)
//            txtmen->status.changedPage = 1;
    }    
}


unsigned char TXTMEN_entryIsOnPage(TxtMenu *txtmen, unsigned char entry)
{
    return ((txtmen->status.page * (txtmen->layout.y.end - txtmen->layout.y.start + 1)) <= entry) && ( ((txtmen->status.page + 1) * (txtmen->layout.y.end - txtmen->layout.y.start + 1)) > entry);
}

void TXTMEN_entryOnPage(TxtMenu *txtmen)
{
    // is marked entry outside this page?
    // mark middle entry
//    if( ((txtmen->status.page * (txtmen->layout.y.end - txtmen->layout.y.start + 1)) > txtmen->status.entry) || ( ((txtmen->status.page + 1) * (txtmen->layout.y.end - txtmen->layout.y.start + 1)) <= txtmen->status.entry) )
    if(!TXTMEN_entryIsOnPage(txtmen, txtmen->status.entry)) {
        txtmen->status.entry = txtmen->status.page * (txtmen->layout.y.end - txtmen->layout.y.start + 1) + ((txtmen->layout.y.end - txtmen->layout.y.start + 1) >> 1);
        if(txtmen->status.entry >= txtmen->nEntries)
            txtmen->status.entry = txtmen->nEntries-1;
    }    
}

void TXTMEN_pageDown(TxtMenu *txtmen)
{
    if(txtmen->status.page > 0) {
        txtmen->status.page--;
        txtmen->status.changedPage = 1;
    }
}

void TXTMEN_pageUp(TxtMenu *txtmen)
{
    if(((txtmen->status.page + 1) * (txtmen->layout.y.end - txtmen->layout.y.start + 1)) < txtmen->nEntries) {
        txtmen->status.page++;
        txtmen->status.changedPage = 1;
    }
}

ReturnValue TXTMEN_displayCursor(TxtMenu *txtmen)
{
    return TXTSCR_printf(txtmen->scroll, txtmen->layout.cursorX, txtmen->layout.y.start + txtmen->status.entry - (txtmen->status.page * (txtmen->layout.y.end - txtmen->layout.y.start + 1)), txtmen->drawMode, "<>");
}

ReturnValue TXTMEN_deleteCursor(TxtMenu *txtmen)
{
    return TXTSCR_clearRectangle(txtmen->scroll, txtmen->layout.cursorX, txtmen->layout.y.start + txtmen->status.entry - (txtmen->status.page * (txtmen->layout.y.end - txtmen->layout.y.start + 1)), 2, 1, txtmen->drawMode);
}

void TXTMEN_executeInputs(TxtMenu *txtmen)
{
    txtmen->status.changedValue = txtmen->status.changedPage = 0;
    TxtMenuEntry *entry = (txtmen->entries + txtmen->status.entry);
    SET_COMMAND_VAR;
    
    GET_COMMAND;
    
    if(COMMAND_START_PRESSED || COMMAND_A_PRESSED || COMMAND_C_PRESSED) {
        // press button or quit
        if(entry->type == TXTMEN_BUTTON) {
            if(entry->enabled == 1)
                txtmen->status.changedValue = 1;
        } //else txtmen->status.leaveMenu = 1;
//    } else if(COMMAND_L_PRESSED) {
//        TXTMEN_pageDown(txtmen);
//        TXTMEN_entryOnPage(txtmen);
//    } else if(COMMAND_R_PRESSED) {
//        TXTMEN_pageUp(txtmen);
//        TXTMEN_entryOnPage(txtmen);
    } else if(COMMAND_B_PRESSED || COMMAND_Y_PRESSED) {
        txtmen->status.leaveMenu = 1;
    } else if(COMMAND_UP_PRESSED) {
//        if(txtmen->status.entry >= 0) {
            TXTMEN_deleteCursor(txtmen);
            if(txtmen->status.entry == 0) {
                txtmen->status.entry = txtmen->nEntries - 1;
                // have to change page?
                if(TXTMEN_entryIsOnPage(txtmen, txtmen->status.entry)) 
                    TXTMEN_displayCursor(txtmen);
                else {
                    txtmen->status.page = txtmen->nEntries / (txtmen->layout.y.end - txtmen->layout.y.start + 1);
                    txtmen->status.changedPage = 1;
                }
            } else {
                txtmen->status.entry--;
                // have to change page?
                if(TXTMEN_entryIsOnPage(txtmen, txtmen->status.entry)) 
                    TXTMEN_displayCursor(txtmen);
                else TXTMEN_pageDown(txtmen);
            }
//        }
    } else if(COMMAND_DOWN_PRESSED) {
        if(txtmen->status.entry <= (txtmen->nEntries - 1)) {
            TXTMEN_deleteCursor(txtmen);
            if(txtmen->status.entry == (txtmen->nEntries - 1)) {
                txtmen->status.entry = 0;
                // have to change page?
                if(TXTMEN_entryIsOnPage(txtmen, txtmen->status.entry)) 
                    TXTMEN_displayCursor(txtmen);
                else {
                    txtmen->status.page = 0;
                    txtmen->status.changedPage = 1;
                }
            } else {
                txtmen->status.entry++;
                // have to change page?
                if(TXTMEN_entryIsOnPage(txtmen, txtmen->status.entry)) 
                    TXTMEN_displayCursor(txtmen);
                else TXTMEN_pageUp(txtmen);
            }
        }
    } else if(COMMAND_LEFT_PRESSED || COMMAND_L) {
        if(entry->enabled == 0)
            return;
            
        switch(entry->type) {
            case TXTMEN_SPINNER_INT:
                if((((TxtSpinnerInt *)entry->content)->value - ((TxtSpinnerInt *)entry->content)->diff) >= ((TxtSpinnerInt *)entry->content)->min) {
                    ((TxtSpinnerInt *)entry->content)->value -= ((TxtSpinnerInt *)entry->content)->diff;
                    txtmen->status.changedValue = 1;
                }
                break;

            case TXTMEN_SPINNER_FLOAT:
                if((((TxtSpinnerFloat *)entry->content)->value - ((TxtSpinnerFloat *)entry->content)->diff) >= ((TxtSpinnerFloat *)entry->content)->min) {
                    ((TxtSpinnerFloat *)entry->content)->value -= ((TxtSpinnerFloat *)entry->content)->diff;
                    txtmen->status.changedValue = 1;
                }
                break;

            case TXTMEN_LISTBOX:
                if(((TxtListbox *)entry->content)->value > 0) {
                    ((TxtListbox *)entry->content)->value--;
                    txtmen->status.changedValue = 1;
                }
                break;

            case TXTMEN_CHECKBOX:
                if(((TxtCheckbox *)entry->content)->value != 0) {
                    ((TxtCheckbox *)entry->content)->value = 0;
                    txtmen->status.changedValue = 1;
                }
                break;

            default:
                break;
        }
    } else if(COMMAND_RIGHT_PRESSED || COMMAND_R) {
        if(entry->enabled == 0)
            return;

        switch(entry->type) {
            case TXTMEN_SPINNER_INT:
                if((((TxtSpinnerInt *)entry->content)->value + ((TxtSpinnerInt *)entry->content)->diff) <= ((TxtSpinnerInt *)entry->content)->max) {
                    ((TxtSpinnerInt *)entry->content)->value += ((TxtSpinnerInt *)entry->content)->diff;
                    txtmen->status.changedValue = 1;
                }
                break;

            case TXTMEN_SPINNER_FLOAT:
                if((((TxtSpinnerFloat *)entry->content)->value + ((TxtSpinnerFloat *)entry->content)->diff) <= ((TxtSpinnerFloat *)entry->content)->max) {
                    ((TxtSpinnerFloat *)entry->content)->value += ((TxtSpinnerFloat *)entry->content)->diff;
                    txtmen->status.changedValue = 1;
                }
                break;

            case TXTMEN_LISTBOX:
                if(((TxtListbox *)entry->content)->value < (((TxtListbox *)entry->content)->nEntries-1)) {
                    ((TxtListbox *)entry->content)->value++;
                    txtmen->status.changedValue = 1;
                }
                break;

            case TXTMEN_CHECKBOX:
                if(((TxtCheckbox *)entry->content)->value != 1) {
                    ((TxtCheckbox *)entry->content)->value = 1;
                    txtmen->status.changedValue = 1;
                }
                break;

            default:
                break;
        }
    }

    return;
}


ReturnValue TXTMEN_drawNameAt(TxtMenu *txtmen, unsigned char entry, unsigned char line)
{
    unsigned char xPos;
    
    switch(txtmen->layout.name.align) {
        case TXTMEN_ALIGN_LEFT:
            xPos = txtmen->layout.name.xPos;
            break;

        case TXTMEN_ALIGN_CENTER:
            xPos = txtmen->layout.name.xPos - (strlen((txtmen->entries + entry)->name) >> 2);
            break;

        case TXTMEN_ALIGN_RIGHT:
            xPos = txtmen->layout.name.xPos - strlen((txtmen->entries + entry)->name);
            break;
    }
    return TXTSCR_nprintf(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.name.width, txtmen->drawMode, (txtmen->entries + entry)->name);
}


ReturnValue TXTMEN_drawValueAt(TxtMenu *txtmen, unsigned char entry, unsigned char line)
{
    unsigned char xPos;
    char *ptr;
//    TxtMenu *txtmen = (TxtMenu *)menu;
    
    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;

    // determine xPos from alignment
    switch(txtmen->layout.value.align) {
        case TXTMEN_ALIGN_LEFT:
            xPos = txtmen->layout.value.xPos;
            break;

        case TXTMEN_ALIGN_CENTER:
            xPos = txtmen->layout.value.xPos - (strlen((txtmen->entries + entry)->name) >> 2);
            break;

        case TXTMEN_ALIGN_RIGHT:
            xPos = txtmen->layout.value.xPos - strlen((txtmen->entries + entry)->name);
            break;
    }
    // print value according to type
    switch((txtmen->entries + entry)->type) {
        case TXTMEN_SPINNER_INT:
            return TXTSCR_nprintf(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.name.width, txtmen->drawMode, "%i", ((TxtSpinnerInt *)(txtmen->entries + entry)->content)->value);

        case TXTMEN_SPINNER_FLOAT:
            return TXTSCR_nprintf(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.name.width, txtmen->drawMode, "%f", ((TxtSpinnerFloat *)(txtmen->entries + entry)->content)->value);

        case TXTMEN_LISTBOX:
            if(((TxtListbox *)(txtmen->entries + entry)->content)->nEntries == 0)
                ptr = "empty";
            else ptr = *(((TxtListbox *)(txtmen->entries + entry)->content)->names + ((TxtListbox *)(txtmen->entries + entry)->content)->value);
            return TXTSCR_nprintf(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.name.width, txtmen->drawMode, ptr);

        case TXTMEN_BUTTON:
            return TXTSCR_nprintf(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.name.width, txtmen->drawMode, "press button");

        case TXTMEN_CHECKBOX:
            return TXTSCR_nprintf(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.name.width, txtmen->drawMode, ( ((TxtCheckbox *)(txtmen->entries + entry)->content)->value == 0 ? "OFF" : "ON"));

    }
    return RETURN_ERR_UNKNOWN;
}


ReturnValue TXTMEN_clearValueAt(TxtMenu *txtmen, unsigned char entry, unsigned char line)
{
    unsigned char xPos;
//    TxtMenu *txtmen = (TxtMenu *)menu;
    
    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;

    // clear value area
    switch(txtmen->layout.value.align) {
        case TXTMEN_ALIGN_LEFT:
            xPos = txtmen->layout.value.xPos;
            break;

        case TXTMEN_ALIGN_CENTER:
            xPos = txtmen->layout.value.xPos - (strlen((txtmen->entries + entry)->name) >> 2);
            break;

        case TXTMEN_ALIGN_RIGHT:
            xPos = txtmen->layout.value.xPos - strlen((txtmen->entries + entry)->name);
            break;
    }
    TXTSCR_clearRectangle(txtmen->scroll, xPos, txtmen->layout.y.start + line, txtmen->layout.value.width, 1, TXT_DRAW_BOTH);

    return RETURN_OK;
}


ReturnValue TXTMEN_redrawValueAt(void *menu, unsigned char entry)
{
    TxtMenu *txtmen = (TxtMenu *)menu;
    unsigned char line;
    
    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;

    if(!TXTMEN_entryIsOnPage(txtmen, entry))
        return RETURN_OK;
        
    line = entry % (txtmen->layout.y.end - txtmen->layout.y.start + 1);

	TXTMEN_clearValueAt(txtmen, entry, line);
    TXTMEN_drawValueAt(txtmen, entry, line);
    
    return RETURN_OK;
}

ReturnValue TXTMEN_redrawMenu(void *menu)
{
    unsigned char line, entry;
    TxtMenu *txtmen = (TxtMenu *)menu;
    
    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;

    // clear
    TXTSCR_clearRectangle(txtmen->scroll, 0, 0, TXTSCR_getWidth(txtmen->scroll), TXTSCR_getHeight(txtmen->scroll), TXT_DRAW_BOTH);

    // draw title
    TXTSCR_printf(txtmen->scroll, txtmen->layout.title.x, txtmen->layout.title.y, txtmen->drawMode, txtmen->name);

    // draw page stuff, only when multiple pages are used
    // !!! somehow printf's strlen produces an mem access error
    if(txtmen->nEntries > (txtmen->layout.y.end - txtmen->layout.y.start + 1))
        TXTSCR_nprintf(txtmen->scroll, txtmen->layout.title.x, txtmen->layout.title.y + 1, 11, txtmen->drawMode, "%s page %i %s", (txtmen->status.page > 0 ? "<" : " "), txtmen->status.page, (txtmen->nEntries > ((txtmen->status.page + 1) * (txtmen->layout.y.end - txtmen->layout.y.start + 1)) ? ">" : " "));    

    // draw entries
    for(entry = txtmen->status.page * (txtmen->layout.y.end - txtmen->layout.y.start + 1), line = 0; (entry < txtmen->nEntries) && (line <= (txtmen->layout.y.end - txtmen->layout.y.start)) ; line++, entry++) {
        TXTMEN_drawNameAt(txtmen, entry,  line);
        TXTMEN_drawValueAt(txtmen, entry,  line);
    }
    // draw cursor, is always visible
    TXTMEN_displayCursor(txtmen);
    
    return RETURN_OK;
}

ReturnValue TXTMEN_leave(void *menu)
{
    TxtMenu *txtmen = (TxtMenu *)menu;
    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;

    txtmen->status.leaveMenu = 1;

    return RETURN_OK;
}

ReturnValue TXTMEN_execute(void *menu)
{
    TxtMenu *txtmen = (TxtMenu *)menu;

    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;
        
    TXTMEN_redrawMenu(txtmen);
    txtmen->status.leaveMenu = 0;

    while(1) {
        txtmen->status.changedPage = txtmen->status.changedValue = 0;
        // get inputs, check if we leave menu
        TXTMEN_executeInputs(txtmen);
            // a callback can set leave menu
            // we exit after callafter
            // to get a new button state
            // reduces problems
        // now do further reactions on input
        if(txtmen->status.changedValue == 1)
            TXTMEN_executeCallback(txtmen, &((txtmen->entries + txtmen->status.entry)->callback));

        // determine redrawing
        if(txtmen->status.changedPage == 1)
            TXTMEN_redrawMenu(txtmen);
        else if(txtmen->status.changedValue == 1) {
            TXTMEN_redrawValueAt(menu, txtmen->status.entry);
        }
        // execute further stuff
        TXTMEN_executeCallback(txtmen, &(txtmen->callafter));
        // leave after everything is done
        // eases button release handling
        if(txtmen->status.leaveMenu == 1)
        	break;

#if defined(SATURN_SGL)
//   slSynch();
#endif

    }
    return RETURN_OK;
}
