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

#ifndef HAVE_RB_TXT_MENU_INT_H
#define HAVE_RB_TXT_MENU_INT_H

#include <RB_CONFIG.h>
#include <RB_TXT_SCROLL.h>
#include <RB_INPUT.h>

#include <stdlib.h>
#include <string.h>


// for name string allocation
#define TXTMEN_STRDUP(s)    strdup(s)
//#define TXTMEN_STRDUP(s)    s     // save memory

typedef enum {
    TXTMEN_SPINNER_INT,
    TXTMEN_SPINNER_FLOAT,
    TXTMEN_LISTBOX,
    TXTMEN_BUTTON,
    TXTMEN_CHECKBOX
} TxtMenuEntryType;

typedef struct TXT_SPINNER_INT {
    int value;
    int min, max;
    // smallest difference 
    int diff;
} TxtSpinnerInt;

typedef struct TXT_SPINNER_FLOAT {
    float value;
    float min, max;
    // smallest difference 
    float diff;
} TxtSpinnerFloat;

typedef struct TXT_LISTBOX {
    unsigned short nEntries;
    int value;
    char **names;
} TxtListbox;

typedef struct TXT_CHECKBOX {
    int value;
} TxtCheckbox;

typedef struct TXT_MENU_CALLBACK {
    int item;
    void *ptr;
    // if the callback overdraws the screen
    unsigned char redraw;
    void (*func)(int item, void *ptr);
} TxtMenuCallback;

typedef struct TXT_MENU_ENTRY {
    TxtMenuEntryType type;
    unsigned char enabled;
    void *content;
    TxtMenuCallback callback;
    char *name;
} TxtMenuEntry;

typedef enum {
    TXTMEN_ALIGN_LEFT,
    TXTMEN_ALIGN_CENTER,
    TXTMEN_ALIGN_RIGHT
} TxtMenuAlign;

typedef struct TXT_MENU {
    unsigned char nEntries;
    TxtMenuEntry *entries;
    char *name;
    // a function to call repeatedly(after input evaluation) while displaying the menu
    TxtMenuCallback callafter;
    struct {
        unsigned char page, entry, changedValue, changedPage, leaveMenu;
    } status;
    struct {
        // horizontal layout of each entry
        struct {
                TxtMenuAlign align;
                // x position of alignment point
                unsigned char xPos;
                unsigned char width;
        } name, value;
        // vertical entry layout
        struct {
            unsigned char start, end;
        } y;
        // title layout
        struct {
            unsigned char x, y;
        } title;
        unsigned char cursorX;
    } layout;
    TxtDrawMode drawMode;
    void *scroll;
} TxtMenu;

#endif

