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


char *TXTMEN_getVersion()
{
    return TXTMEN_VERSION;
}

ReturnValue TXTMEN_init(void **menu, void *scroll, char *name)
{
    TxtMenu *txtmen = (TxtMenu *)malloc(sizeof(TxtMenu));
    if(txtmen == NULL)
        return RETURN_ERR_ALLOC;
    
    txtmen->nEntries = 0;
    txtmen->entries = NULL;
    txtmen->name = (char *)TXTMEN_STRDUP(name);
    if(txtmen->name == NULL)
        return RETURN_ERR_ALLOC;
        
    txtmen->status.page = 0;
    txtmen->status.entry = 0;
    
    // set default layout
    txtmen->layout.name.align = TXTMEN_ALIGN_LEFT;
    txtmen->layout.name.xPos = 1;
    txtmen->layout.name.width = (TXTSCR_getWidth(scroll) >> 1) - 2;
    txtmen->layout.value.align = TXTMEN_ALIGN_LEFT;
    txtmen->layout.value.xPos = 1 + (TXTSCR_getWidth(scroll) >> 1);
    txtmen->layout.value.width = (TXTSCR_getWidth(scroll) >> 1) - 2;
    
    txtmen->layout.y.start = 3;
    txtmen->layout.y.end = TXTSCR_getHeight(scroll) - 1 - 1;
    // center title
    txtmen->layout.title.x = (TXTSCR_getWidth(scroll) - strlen(txtmen->name)) >> 1;
    txtmen->layout.title.y = 1;

    txtmen->layout.cursorX = (TXTSCR_getWidth(scroll) >> 1) - 1;
    
    txtmen->drawMode = TXT_DRAW_BOTH;
    txtmen->scroll = scroll;
    
    *menu = txtmen;

    return RETURN_OK;
}



void *TXTMEN_addContentAt(unsigned char entry, TxtMenu *txtmen, TxtMenuEntryType type, char *name)
{
    void *content;
    
    if(entry >= txtmen->nEntries)
        return NULL;
    
    switch(type) {
        case TXTMEN_SPINNER_INT:
            content = (void *)calloc(1, sizeof(TxtSpinnerInt));
            break;

        case TXTMEN_SPINNER_FLOAT:
            content = (void *)calloc(1, sizeof(TxtSpinnerFloat));
            break;
            
        case TXTMEN_LISTBOX:
            content = (void *)calloc(1, sizeof(TxtListbox));
            break;

        case TXTMEN_BUTTON:
//            content = calloc(1, sizeof(TxtButton));
            content = (void *)1; // not NULL...
            break;

        case TXTMEN_CHECKBOX:
            content = (void *)calloc(1, sizeof(TxtCheckbox));
            break;
    }

    if(content == NULL)
        return NULL;
    
    (txtmen->entries + entry)->content = content;
    (txtmen->entries + entry)->type = type;
    (txtmen->entries + entry)->enabled = 1;
    (txtmen->entries + entry)->name = (char *)TXTMEN_STRDUP(name);
    if((txtmen->entries + entry)->name == NULL) {
        free(content);
        return NULL;
    }
    /// default for callback
    (txtmen->entries + entry)->callback.item = -1;
    (txtmen->entries + entry)->callback.func = NULL;
    
    return content;
}

// add content to last entry
void *TXTMEN_addContent(void *menu, TxtMenuEntryType type, char *name)
{
	return TXTMEN_addContentAt(((TxtMenu *)menu)->nEntries-1, menu, type, name);
}

ReturnValue TXTMEN_addEntries(void *menu, unsigned char entries)
{
    TxtMenu *txtmen = (TxtMenu *)menu;
    
    if(txtmen == NULL)
        return RETURN_ERR_NULLARG;
    
    if(txtmen->nEntries == 0)
        txtmen->entries = (TxtMenuEntry *)malloc(entries * sizeof(TxtMenuEntry));
    else
        txtmen->entries = (TxtMenuEntry *)realloc(txtmen->entries, (txtmen->nEntries + entries) * sizeof(TxtMenuEntry));

    if(txtmen->entries == NULL)
        return RETURN_ERR_ALLOC;
        
    txtmen->nEntries += entries;
    
    return RETURN_OK;
}

ReturnValue TXTMEN_addEntry(void *menu)
{
	return TXTMEN_addEntries(menu, 1);
}

ReturnValue TXTMEN_addSpinnerIntAt(void *menu, unsigned char entry, char *name, int min, int max, int diff, int value)
{
    TxtSpinnerInt *content;

    if(menu == NULL)
        return RETURN_ERR_NULLARG;

    if((min > max) || (value < min) || (value > max) || (diff > (max - min)))
        return RETURN_ERR_INVALIDARG;
    
    // performs out-of-bounds check
    content = (TxtSpinnerInt *)TXTMEN_addContentAt(entry, menu, TXTMEN_SPINNER_INT, name);
    if(content == NULL)
        return RETURN_ERR_ALLOC;        
        
    content->value = value;
    content->min = min;
    content->max = max;
    content->diff = diff;

    return RETURN_OK;
}

ReturnValue TXTMEN_addSpinnerInt(void *menu, char *name, int min, int max, int diff, int value)
{
    ReturnValue ret;
    
    ret = TXTMEN_addEntry(menu);
    if(ret != RETURN_OK)
        return ret;
    else
        return TXTMEN_addSpinnerIntAt(menu, ((TxtMenu *)menu)->nEntries-1, name, min, max, diff, value);
}


ReturnValue TXTMEN_addSpinnerFloatAt(void *menu, unsigned char entry, char *name, float min, float max, float diff, float value)
{
    TxtSpinnerFloat *content;

    if(menu == NULL)
        return RETURN_ERR_NULLARG;
    
    if((min > max) || (value < min) || (value > max) || (diff > (max - min)))
        return RETURN_ERR_INVALIDARG;

    // performs out-of-bounds check
    content = (TxtSpinnerFloat *)TXTMEN_addContentAt(entry, menu, TXTMEN_SPINNER_FLOAT, name);
    if(content == NULL)
        return RETURN_ERR_ALLOC;
                
    content->value = value;
    content->min = min;
    content->max = max;
    content->diff = diff;
    
    return RETURN_OK;
}

ReturnValue TXTMEN_addSpinnerFloat(void *menu, char *name, float min, float max, float diff, float value)
{
    ReturnValue ret;
    
    ret = TXTMEN_addEntry(menu);
    if(ret != RETURN_OK)
        return ret;
    else
        return TXTMEN_addSpinnerFloatAt(menu, ((TxtMenu *)menu)->nEntries-1, name, min, max, diff, value);
}

ReturnValue TXTMEN_addListboxAt(void *menu, unsigned char entry, char *name)
{
    TxtListbox *content;

    if(menu == NULL)
        return RETURN_ERR_NULLARG;
    
    // performs out-of-bounds check
    content = (TxtListbox *)TXTMEN_addContentAt(entry, menu, TXTMEN_LISTBOX, name);
    if(content == NULL)
        return RETURN_ERR_ALLOC;
        
    content->nEntries = 0;
    content->value = 0;
    
    return RETURN_OK;
}

ReturnValue TXTMEN_addListbox(void *menu, char *name)
{
    ReturnValue ret;
    
    ret = TXTMEN_addEntry(menu);
    if(ret != RETURN_OK)
        return ret;
    else
        return TXTMEN_addListboxAt(menu, ((TxtMenu *)menu)->nEntries-1, name);
}


ReturnValue TXTMEN_addItemAt(void *menu, unsigned char entry, char *name)
{
    char **names;
    TxtListbox *content;

    if(menu == NULL)
        return RETURN_ERR_NULLARG;
        
    if(entry >= ((TxtMenu *)menu)->nEntries)
        return RETURN_ERR_OOB;
    
    
    content = (TxtListbox *)(((TxtMenu *)menu)->entries + entry)->content;
    
    if(content->nEntries == 0)
        names = (char **)malloc(sizeof(char *));
    else names = (char **)realloc(content->names, (content->nEntries + 1) * sizeof(char *));
    
    if(names == NULL)
        return RETURN_ERR_ALLOC;
        
    content->names = names;
    *(content->names + content->nEntries) = (char *)TXTMEN_STRDUP(name);
    if(*(content->names + content->nEntries) == NULL)
        return RETURN_ERR_ALLOC;

    content->nEntries++;
    return RETURN_OK;
}


ReturnValue TXTMEN_addItem(void *menu, char *name)
{
    if(menu == NULL)
        return RETURN_ERR_NULLARG;
        
    return TXTMEN_addItemAt(menu, ((TxtMenu *)menu)->nEntries-1, name);
}


ReturnValue TXTMEN_addButtonAt(void *menu, unsigned char entry, char *name)
{
    void *content;

    if(menu == NULL)
        return RETURN_ERR_NULLARG;
    
    // performs out-of-bounds check
    content = (void *)TXTMEN_addContentAt(entry, menu, TXTMEN_BUTTON, name);
    if(content == NULL)
        return RETURN_ERR_ALLOC;        

    return RETURN_OK;
}

ReturnValue TXTMEN_addButton(void *menu, char *name)
{
    ReturnValue ret;
    
    ret = TXTMEN_addEntry(menu);
    if(ret != RETURN_OK)
        return ret;
    else
        return TXTMEN_addButtonAt(menu, ((TxtMenu *)menu)->nEntries-1, name);
}


ReturnValue TXTMEN_addCheckboxAt(void *menu, unsigned char entry, char *name, unsigned char value)
{
    TxtCheckbox *content;

    if(menu == NULL)
        return RETURN_ERR_NULLARG;
    
    // performs out-of-bounds check
    content = (TxtCheckbox *)TXTMEN_addContentAt(entry, menu, TXTMEN_CHECKBOX, name);
    if(content == NULL)
        return RETURN_ERR_ALLOC;        
        
    content->value = value;
    
    return RETURN_OK;
}

ReturnValue TXTMEN_addCheckbox(void *menu, char *name, unsigned char value)
{
    ReturnValue ret;
    
    ret = TXTMEN_addEntry(menu);
    if(ret != RETURN_OK)
        return ret;
    else
        return TXTMEN_addCheckboxAt(menu, ((TxtMenu *)menu)->nEntries-1, name, value);
}



ReturnValue TXTMEN_ableAt(void *menu, unsigned char entry, unsigned char value)
{
    if(menu == NULL)
        return RETURN_ERR_NULLARG;

    if(entry >= ((TxtMenu *)menu)->nEntries)
        return RETURN_ERR_OOB;

    (((TxtMenu *)menu)->entries + entry)->enabled = value;

    return RETURN_OK;
}

ReturnValue TXTMEN_able(void *menu, unsigned char value)
{
    if(menu == NULL)
        return RETURN_ERR_NULLARG;

    (((TxtMenu *)menu)->entries + ((TxtMenu *)menu)->nEntries - 1)->enabled = value;

    return RETURN_OK;
}

ReturnValue TXTMEN_disableAt(void *menu, unsigned char entry)
{
    return TXTMEN_ableAt(menu, entry, 0);
}

ReturnValue TXTMEN_disable(void *menu)
{
    return TXTMEN_able(menu, 0);
}

ReturnValue TXTMEN_enableAt(void *menu, unsigned char entry)
{
    return TXTMEN_ableAt(menu, entry, 1);
}

ReturnValue TXTMEN_enable(void *menu)
{
    return TXTMEN_able(menu, 1);
}


ReturnValue TXTMEN_setThisCallback(TxtMenuCallback *callback, int item, void *ptr, void (*func)(int item, void *ptr))
{
    if(callback == NULL)
        return RETURN_ERR_NULLARG;
        
    callback->func = func;        
    callback->item = item;
    callback->ptr = ptr;
    
    return RETURN_OK;
}

ReturnValue TXTMEN_setCallbackAt(void *menu, unsigned char entry, int item, void *ptr, void (*func)(int item, void *ptr))
{
    if(menu == NULL)
        return RETURN_ERR_NULLARG;

    if(entry >= ((TxtMenu *)menu)->nEntries)
        return RETURN_ERR_OOB;
    
    return TXTMEN_setThisCallback(&((((TxtMenu *)menu)->entries + entry)->callback), item, ptr, func);
}

ReturnValue TXTMEN_setCallback(void *menu, int item, void *ptr, void (*func)(int item, void *ptr))
{
    if(menu == NULL)
        return RETURN_ERR_NULLARG;

    return TXTMEN_setThisCallback(&((((TxtMenu *)menu)->entries + ((TxtMenu *)menu)->nEntries - 1)->callback), item, ptr, func);
}

ReturnValue TXTMEN_setCallafter(void *menu, int item, void *ptr, void (*func)(int item, void *ptr))
{
    if(menu == NULL)
        return RETURN_ERR_NULLARG;

    return TXTMEN_setThisCallback(&(((TxtMenu *)menu)->callafter), item, ptr, func);
}



