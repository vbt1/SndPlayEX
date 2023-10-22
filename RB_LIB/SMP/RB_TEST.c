#include <stdio.h>
#include <stdlib.h>
//#include "SDL.h"

#include <RB_CONFIG.h>
#include <RB_RETVAL.h>
#include <RB_TXT_SCROLL.h>
#include <RB_TXT_MENU.h>
#include <RB_TXT_STRING.h>
#include <RB_TXT_MESSAGES.h>

#define	TXT_SCROLL_XRES	40
#define TXT_SCROLL_YRES 30
void *menu, *screen, *scroll1, *scroll2, *msgbox, *string1, *string2;

void callafter(int item, void *ptr)
{
#if defined(SATURN_SGL)
   slSynch();
#endif
}

void TEST_TxtMenu()
{
    ReturnValue ret;

    ret = TXTSCR_initScreen(&screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES);
    if(ret != RETURN_OK)
        MSGS_printf(msgbox, "Error initializing TXT_SCREEN!");

    ret = TXTSCR_initScroll(&scroll1, screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES-10, 0, 0);
    if(ret != RETURN_OK)
        MSGS_printf(msgbox, "Error initializing TXT_SCROLL!");

    ret = TXTSCR_initScroll(&scroll2, screen, TXT_SCROLL_XRES, 10, 0, TXT_SCROLL_YRES - 10);
    if(ret != RETURN_OK)
        MSGS_printf(msgbox, "Error initializing TXT_SCROLL2!");
        
    ret = TXTMEN_init(&menu, scroll1, "Test Menu");
    if(ret != RETURN_OK)
        MSGS_printf(msgbox, "Error initializing TXT_MENU!");
    
    MSGS_printf(msgbox, "Last message...");
    MSGS_display(msgbox, scroll2);
    
    TXTMEN_addSpinnerInt(menu, "SpinnerInt", -10, 50, 1, 0);
    TXTMEN_addSpinnerFloat(menu, "SpinnerFloat", -312.5, 120.7, 1.724365463, 4.766576);
    TXTMEN_addListbox(menu, "Listbox 1");
    TXTMEN_addItem(menu, "item 0");
    TXTMEN_addItem(menu, "item 1");
    TXTMEN_addItem(menu, "item 2");
    TXTMEN_addButton(menu, "Button 1");
    TXTMEN_addButton(menu, "Button 2");
    TXTMEN_addButton(menu, "Button 3");
    TXTMEN_addButton(menu, "Button 4");
    TXTMEN_addCheckbox(menu, "Checkbox 1", 1);
    TXTMEN_addCheckbox(menu, "Checkbox 2", 1);
    TXTMEN_addCheckbox(menu, "Checkbox 3", 1);
    TXTMEN_addListbox(menu, "Listbox 2");
    TXTMEN_addItem(menu, "item 0");
    TXTMEN_addItem(menu, "item 1");
    TXTMEN_addItem(menu, "item 2");
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    TXTMEN_addButton(menu, "Button");
    TXTMEN_addCheckbox(menu, "Checkbox", 1);
    

    TXTMEN_setCallafter(menu, 0, menu, callafter);

    TXTMEN_execute(menu);
    
//    TXTMEN_exit(menu);
    TXTSCR_exitScroll(scroll1);
    TXTSCR_exitScroll(scroll2);
    TXTSCR_exitScreen(screen);
    MSGS_exit(msgbox);
#if defined(SDL_PC)
    SDL_Quit();
#endif

}

void TEST_StringInput2()
{
    ReturnValue ret;
    
    ret = TXTSCR_initScreen(&screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES);
    if(ret != RETURN_OK)
        return;
    ret = TXTSCR_initScroll(&scroll1, screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES, 0, 0);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR2_init(&string2, scroll1);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR2_setCharTbl(string2, "ABC\nDEF\nHIJ", 10, 10);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR2_setName(string2, "Testing String 2", 10, 1);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR2_setString(string2, "a string", 10, 3, 15);
    if(ret != RETURN_OK)
        return;

    TXTSTR2_execute(string2);
}


void TEST_StringInput1()
{
    ReturnValue ret;
    
    ret = TXTSCR_initScreen(&screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES);
    if(ret != RETURN_OK)
        return;
    ret = TXTSCR_initScroll(&scroll1, screen, TXT_SCROLL_XRES, TXT_SCROLL_YRES, 0, 0);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR1_init(&string1, scroll1);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR1_setCharTbl(string1, "ABC\nDEF\nHIJ", 10, 10, ATTACH_TO_A, ATTACH_AS_FIRST);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR1_setCharTbl(string1, "KLM\nNOP\nQRS", 20, 10, ATTACH_TO_B, ATTACH_AS_FIRST);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR1_setName(string1, "Testing String 1", 10, 1);
    if(ret != RETURN_OK)
        return;

	ret = TXTSTR1_setString(string1, "a string", 10, 3, 15);
    if(ret != RETURN_OK)
        return;

    TXTSTR1_execute(string1);
}


struct POINTERS {
    void *dspdbg, *txtscreen, *scroll_top, *scroll_bottom, *menu_fb, *menu_top;
} ptrs;

enum {
    TR_MENU_0,
    TR_MENU_1,
    TR_MENU_2,
    TR_MENU_3,
    TR_MENU_4,
    TR_MENU_5,
    TR_MENU_6,
    TR_MENU_7,
    TR_MENU_8,
    TR_MENU_OPEN_FBA,
    TR_MENU_OPEN_DSPDBG,
    TR_MENU_EXIT,
    TR_MENU_SWITCH_FB
};


void TR_MENU_callback(int item, void *ptr)
{
    if(ptr == NULL)
        return;
    
    switch(item) {
// ptr = txtmen
        // changed with every frame
        case TR_MENU_0:
//            SL_LOOK_R_X0 = toFIXED(TXTMEN_getIntValueAt(ptr, TR_MENU_0));
            return;
            
        case TR_MENU_1:
//            SL_LOOK_R_Y0 = toFIXED(TXTMEN_getIntValueAt(ptr, TR_MENU_1));
            return;
            
        case TR_MENU_2:
//            SL_LOOK_R_X1 = toFIXED(TXTMEN_getIntValueAt(ptr, TR_MENU_2));
            return;

        case TR_MENU_3:
//            SL_LOOK_R_Y1 = toFIXED(TXTMEN_getIntValueAt(ptr, TR_MENU_3));
            return;

        case TR_MENU_4:
        case TR_MENU_5:
//            slDispCenterR(toFIXED(TXTMEN_getIntValueAt(ptr, TR_MENU_4)), TXTMEN_getIntValueAt(ptr, TR_MENU_5));
            return;
        // position determined by placement coo and center of rotation
        // sets position on rotation scroll map

        case TR_MENU_6:
//            slZrotR(TXTMEN_getIntValueAt(ptr, TR_MENU_6));
            return;

        case TR_MENU_7:
        case TR_MENU_8:
//            slZoomR(toFIXED(TXTMEN_getFloatValueAt(ptr, TR_MENU_7)), TXTMEN_getFloatValueAt(ptr, TR_MENU_8));
            return;

// ptr = struct POINTERS            
        case TR_MENU_OPEN_FBA:
//            waitStartRelease();
            TXTMEN_execute(((struct POINTERS *)ptr)->menu_fb);
            TXTMEN_redrawMenu(((struct POINTERS *)ptr)->menu_top);
//            waitStartRelease();
            return;
            
        case TR_MENU_OPEN_DSPDBG:
//            waitStartRelease();
//            DSPDBG_execute(((struct POINTERS *)ptr)->dspdbg);
            TXTMEN_redrawMenu(((struct POINTERS *)ptr)->menu_top);
//            waitStartRelease();
            return;

        case TR_MENU_EXIT:
            TXTMEN_leave(((struct POINTERS *)ptr)->menu_top);
//            waitStartRelease();
            return;
            
        case TR_MENU_SWITCH_FB:
//            switch_framebuffer();
//            slSynch();
            return;
    }
}


void TEST_TxtMenu2()
{
    TXTSCR_initScreen(&(ptrs.txtscreen), TXT_SCROLL_XRES, TXT_SCROLL_YRES);

    TXTSCR_initScroll(&(ptrs.scroll_top), (ptrs.txtscreen), TXT_SCROLL_XRES, TXT_SCROLL_YRES >> 1, 0, 0);
    TXTSCR_initScroll(&(ptrs.scroll_bottom), (ptrs.txtscreen), TXT_SCROLL_XRES, TXT_SCROLL_YRES >> 1, 0, TXT_SCROLL_YRES >> 1);

    // create menu for framebuffer position and such
    TXTMEN_init(&(ptrs.menu_fb), ptrs.scroll_top, "fba");
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slLookR x0", -200, 300, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_0, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slLookR y0", -200, 300, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_1, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slLookR x1", -200, 300, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_2, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slLookR y1", -200, 300, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_3, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slDispCenterR x", -200, 300, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_4, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slDispCenterR y", -200, 300, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_5, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerInt((ptrs.menu_fb), "slZrotR", -180, 180, 1, 0);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_6, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addSpinnerFloat((ptrs.menu_fb), "slZoomR x", 0.1, 2.0, 1, 0.5);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_7, (ptrs.menu_fb), TR_MENU_callback);
#if 1    
    TXTMEN_addSpinnerFloat((ptrs.menu_fb), "slZoomR y", 0.1, 2.0, 1, 0.5);
    TXTMEN_setCallback((ptrs.menu_fb), TR_MENU_8, (ptrs.menu_fb), TR_MENU_callback);
    TXTMEN_addButton((ptrs.menu_fb), "button");
#endif
    TXTMEN_setCallafter((ptrs.menu_fb), TR_MENU_SWITCH_FB, NULL, TR_MENU_callback);

    TXTMEN_execute(ptrs.menu_fb);

}

#if !defined(SATURN_SGL)
int main(int argc, char *argv[])
{
#if defined(SDL_PC)
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
    
    atexit(SDL_Quit);

	/* Enable UNICODE translation for keyboard input */
	SDL_EnableUNICODE(1);

	/* disable auto repeat for keyboard input */
	SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
#endif
    MSGS_init(&msgbox, 10, TXT_SCROLL_XRES);

//    TEST_TxtMenu();
    TEST_StringInput1();
    system("PAUSE");	
    return 0;
}
#endif



