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

#ifndef HAVE_RB_TXT_STRING_INT_H
#define HAVE_RB_TXT_STRING_INT_H

#include <RB_RETVAL.h>
#include <RB_TXT_SCROLL.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define	TXTSTR_CHAR_ALLOC_UNIT	5


typedef struct CHAR_TABLE {
    unsigned char width, height;
    char **chars;
    struct {
        unsigned char x, y;
    } pos, spacing;
} CharTable;

typedef struct STRING_BUFFER {
    char *string;
    struct {
        unsigned char string, buffer, max;
    } length;
    struct {
        unsigned char x, y;
    } pos;
    unsigned char cursor;
} StringBuffer;

#if 1
typedef struct NAME {
    struct {
            unsigned char x, y;
    } pos;
    char *string; 
} Name;
#endif



#ifdef __cplusplus
extern "C" {
#endif

ReturnValue TXTSTR_delCharTbl(CharTable *chartbl);
ReturnValue TXTSTR_getCharTblSize(char *string, unsigned char *w, unsigned char *h);
ReturnValue TXTSTR_setCharTbl(CharTable *chartbl, char *string, unsigned char xPos, unsigned char yPos, unsigned char width, unsigned char height);
ReturnValue TXTSTR_displayChartbl(CharTable *chartbl, void *scroll, TxtDrawMode drawMode);

ReturnValue TXTSTR_deleteCursor(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode);
ReturnValue TXTSTR_displayCursor(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode);
ReturnValue TXTSTR_displayString(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode);
ReturnValue TXTSTR_deleteString(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode);
ReturnValue TXTSTR_redrawString(StringBuffer *strbuf, void *scroll, TxtDrawMode drawMode);
ReturnValue TXTSTR_setString(StringBuffer *strbuf, char *string, unsigned char xPos, unsigned char yPos, unsigned char maxLength);

ReturnValue TXTSTR_addChar(StringBuffer *strbuf, char c, unsigned char *redrawCursor, unsigned char *redrawString);
ReturnValue TXTSTR_removeChar(StringBuffer *strbuf, unsigned char *redrawCursor, unsigned char *redrawString);
ReturnValue TXTSTR_cursorLeft(StringBuffer *strbuf, unsigned char *redrawCursor);
ReturnValue TXTSTR_cursorRight(StringBuffer *strbuf, unsigned char *redrawCursor);

ReturnValue TXTSTR_setName(Name *name, char *string, unsigned char xPos, unsigned char yPos);
ReturnValue TXTSTR_displayName(Name *name, void *scroll, TxtDrawMode drawMode);

#ifdef __cplusplus
}
#endif

#endif

