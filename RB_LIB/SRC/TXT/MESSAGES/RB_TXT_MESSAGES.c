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

#include <RB_TXT_MESSAGES.h>
#include "RB_TXT_MESSAGES_INT.h"


char *MSGS_getVersion()
{
    return MSGS_VERSION;
}

ReturnValue MSGS_init(void **box, unsigned short n, unsigned short length)
{
    int i;
    MessageBox *msgbox = (MessageBox *)malloc(sizeof(MessageBox));
    
    if(msgbox == NULL)
        return RETURN_ERR_ALLOC;

    msgbox->messages = (char **)malloc(n * sizeof(char *));
    if(msgbox->messages == NULL)
        return RETURN_ERR_ALLOC;
        
    for(i = 0; i < n; i++) {
        *(msgbox->messages + i) = (char *)calloc(length, sizeof(char));
        if(*(msgbox->messages + i) == NULL)
            return RETURN_ERR_ALLOC;
    }
    
    msgbox->n = n;
    msgbox->length = length;
    msgbox->current = 0;
    msgbox->newMsg = 0;
    msgbox->page = 0;
    
    *box = msgbox;
    
    return RETURN_OK;
}


ReturnValue MSGS_exit(void *box)
{
    int i;
    MessageBox *msgbox = (MessageBox *)box;
    if(msgbox == NULL)
        return RETURN_ERR_NULLARG;

    /* free message stuff */
    for(i = 0; i < msgbox->n; i++) {
        free(*(msgbox->messages + i));
    }
    free(msgbox->messages);
    
    memset(msgbox, 0, sizeof(MessageBox));
    
    return RETURN_OK;
}


ReturnValue MSGS_printf(void *box, const char *fmt, ...)
{
    va_list ap;
    MessageBox *msgbox = (MessageBox *)box;

    if(msgbox == NULL)
        return RETURN_ERR_NULLARG;

    va_start (ap, fmt);
    vsnprintf(*(msgbox->messages + msgbox->current), msgbox->length, fmt, ap);
    va_end (ap);
    
    if(msgbox->current == (msgbox->n-1))
        msgbox->current = 0;
    else msgbox->current++;
    
    msgbox->newMsg = 1;
    msgbox->page = 0;
    
    return RETURN_OK;
}

ReturnValue MSGS_display(void *box, void *scroll)
{
    int i;
    ReturnValue ret;
    MessageBox *msgbox = (MessageBox *)box;

    // clear
    ret = TXTSCR_clear(scroll, TXT_DRAW_LATER);
    if(ret != RETURN_OK)
        return ret;
    
    for(i = 0; (i < TXTSCR_getHeight(scroll)) && (i < msgbox->n); i++) {
        ret = TXTSCR_printf(scroll, 0, i, TXT_DRAW_BOTH, *(msgbox->messages + i));
        if(ret != RETURN_OK)
            return ret;
    }
    return RETURN_OK;    
}

