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

#ifndef HAVE_RB_INPUT_H
#define HAVE_RB_INPUT_H

#define	INP_VERSION	"050129"

#include <RB_CONFIG.h>

/****************************************************************************************************/
/* input handling - portable ************************************************************************/
/****************************************************************************************************/

#if defined(PC_SHELL)

#define		SET_COMMAND_VAR				char INP_c;
#define		GET_COMMAND					do{\
        									INP_c = getchar();\
    									}while(!isalnum(INP_c));
#define		COMMAND_START_PRESSED		(INP_c == 's')
#define		COMMAND_START				(INP_c == 's')
#define		COMMAND_START_RELEASED		(INP_c == 's')
#define		COMMAND_UP_PRESSED			(INP_c == 'u')
#define		COMMAND_UP					(INP_c == 'u')
#define		COMMAND_UP_RELEASED			(INP_c == 'u')
#define		COMMAND_DOWN_PRESSED		(INP_c == 'j')
#define		COMMAND_DOWN				(INP_c == 'j')
#define		COMMAND_DOWN_RELEASED		(INP_c == 'j')
#define		COMMAND_RIGHT_PRESSED		(INP_c == 'k')
#define		COMMAND_RIGHT				(INP_c == 'k')
#define		COMMAND_RIGHT_RELEASED		(INP_c == 'k')
#define		COMMAND_LEFT_PRESSED		(INP_c == 'h')
#define		COMMAND_LEFT				(INP_c == 'h')
#define		COMMAND_LEFT_RELEASED		(INP_c == 'h')
#define		COMMAND_A_PRESSED			(INP_c == 'a')
#define		COMMAND_A					(INP_c == 'a')
#define		COMMAND_A_RELEASED			(INP_c == 'a')
#define		COMMAND_B_PRESSED			(INP_c == 'b')
#define		COMMAND_B					(INP_c == 'b')
#define		COMMAND_B_RELEASED			(INP_c == 'b')
#define		COMMAND_C_PRESSED			(INP_c == 'c')
#define		COMMAND_C					(INP_c == 'c')
#define		COMMAND_C_RELEASED			(INP_c == 'c')
#define		COMMAND_X_PRESSED			(INP_c == 'x')
#define		COMMAND_X					(INP_c == 'x')
#define		COMMAND_X_RELEASED			(INP_c == 'x')
#define		COMMAND_Y_PRESSED			(INP_c == 'y')
#define		COMMAND_Y					(INP_c == 'y')
#define		COMMAND_Y_RELEASED			(INP_c == 'y')
#define		COMMAND_Z_PRESSED			(INP_c == 'z')
#define		COMMAND_Z					(INP_c == 'z')
#define		COMMAND_Z_RELEASED			(INP_c == 'z')
#define		COMMAND_L_PRESSED			(INP_c == 'l')
#define		COMMAND_L					(INP_c == 'l')
#define		COMMAND_L_RELEASED			(INP_c == 'l')
#define		COMMAND_R_PRESSED			(INP_c == 'r')
#define		COMMAND_R					(INP_c == 'r')
#define		COMMAND_R_RELEASED			(INP_c == 'r')


#elif defined(SDL_PC)

#define		SET_COMMAND_VAR				SDL_Event INP_event;
#define		GET_COMMAND					do{\
        									SDL_WaitEvent(&INP_event);\
                                            if(INP_event.type == SDL_QUIT) {\
                                                SDL_Quit();\
                                                exit(0);\
                                            }\
    									}while((INP_event.type != SDL_KEYDOWN) && (INP_event.type != SDL_KEYUP));
#define		COMMAND(s, t)				((INP_event.key.keysym.sym == s) && (INP_event.type == t))
#define		COMMAND_START_PRESSED		COMMAND(SDLK_SPACE, SDL_KEYDOWN)
#define		COMMAND_START				COMMAND(SDLK_SPACE, SDL_KEYDOWN)
#define		COMMAND_START_RELEASED		COMMAND(SDLK_SPACE, SDL_KEYUP)
#define		COMMAND_UP_PRESSED			COMMAND(SDLK_UP, SDL_KEYDOWN)
#define		COMMAND_UP					COMMAND(SDLK_UP, SDL_KEYDOWN)
#define		COMMAND_UP_RELEASED			COMMAND(SDLK_UP, SDL_KEYUP)
#define		COMMAND_DOWN_PRESSED		COMMAND(SDLK_DOWN, SDL_KEYDOWN)
#define		COMMAND_DOWN				COMMAND(SDLK_DOWN, SDL_KEYDOWN)
#define		COMMAND_DOWN_RELEASED		COMMAND(SDLK_DOWN, SDL_KEYUP)
#define		COMMAND_RIGHT_PRESSED		COMMAND(SDLK_RIGHT, SDL_KEYDOWN)
#define		COMMAND_RIGHT				COMMAND(SDLK_RIGHT, SDL_KEYDOWN)
#define		COMMAND_RIGHT_RELEASED		COMMAND(SDLK_RIGHT, SDL_KEYUP)
#define		COMMAND_LEFT_PRESSED		COMMAND(SDLK_LEFT, SDL_KEYDOWN)
#define		COMMAND_LEFT				COMMAND(SDLK_LEFT, SDL_KEYDOWN)
#define		COMMAND_LEFT_RELEASED		COMMAND(SDLK_LEFT, SDL_KEYUP)
#define		COMMAND_A_PRESSED			COMMAND(SDLK_s, SDL_KEYDOWN)
#define		COMMAND_A_RELEASED			COMMAND(SDLK_s, SDL_KEYUP)
#define		COMMAND_B_PRESSED			COMMAND(SDLK_d, SDL_KEYDOWN)
#define		COMMAND_B_RELEASED			COMMAND(SDLK_d, SDL_KEYUP)
#define		COMMAND_C_PRESSED			COMMAND(SDLK_f, SDL_KEYDOWN)
#define		COMMAND_C_RELEASED			COMMAND(SDLK_f, SDL_KEYUP)
#define		COMMAND_X_PRESSED			COMMAND(SDLK_w, SDL_KEYDOWN)
#define		COMMAND_X_RELEASED			COMMAND(SDLK_w, SDL_KEYUP)
#define		COMMAND_Y_PRESSED			COMMAND(SDLK_e, SDL_KEYDOWN)
#define		COMMAND_Y_RELEASED			COMMAND(SDLK_e, SDL_KEYUP)
#define		COMMAND_Z_PRESSED			COMMAND(SDLK_r, SDL_KEYDOWN)
#define		COMMAND_Z_RELEASED			COMMAND(SDLK_r, SDL_KEYUP)
#define		COMMAND_L_PRESSED			COMMAND(SDLK_a, SDL_KEYDOWN)
#define		COMMAND_L					COMMAND(SDLK_a, SDL_KEYDOWN)
#define		COMMAND_L_RELEASED			COMMAND(SDLK_a, SDL_KEYUP)
#define		COMMAND_R_PRESSED			COMMAND(SDLK_g, SDL_KEYDOWN)
#define		COMMAND_R					COMMAND(SDLK_g, SDL_KEYDOWN)
#define		COMMAND_R_RELEASED			COMMAND(SDLK_g, SDL_KEYUP)

#elif defined(SATURN_SGL)

#define		SET_COMMAND_VAR				Uint16 INP_push, INP_data, INP_pull;
#define		GET_COMMAND					INP_data = Smpc_Peripheral[0].data; \
                                        INP_push = Smpc_Peripheral[0].push; \
                                        INP_pull = Smpc_Peripheral[0].pull;

#define		COMMAND_(var, cmd)			((var & cmd) == 0)
#define		COMMAND(cmd)			    COMMAND_(INP_data, cmd)
#define		COMMAND_PRESSED(cmd)	    COMMAND_(INP_push, cmd)
#define		COMMAND_RELEASED(cmd)	    COMMAND_(INP_pull, cmd)
#define		COMMAND_START_PRESSED		COMMAND_PRESSED(PER_DGT_ST)
#define		COMMAND_START				COMMAND(PER_DGT_ST)
#define		COMMAND_START_RELEASED		COMMAND_RELEASED(PER_DGT_ST)
#define		COMMAND_UP_PRESSED			COMMAND_PRESSED(PER_DGT_KU)
#define		COMMAND_UP					COMMAND(PER_DGT_KU)
#define		COMMAND_UP_RELEASED			COMMAND_RELEASED(PER_DGT_KU)
#define		COMMAND_DOWN_PRESSED		COMMAND_PRESSED(PER_DGT_KD)
#define		COMMAND_DOWN				COMMAND(PER_DGT_KD)
#define		COMMAND_DOWN_RELEASED		COMMAND_RELEASED(PER_DGT_KD)
#define		COMMAND_RIGHT_PRESSED		COMMAND_PRESSED(PER_DGT_KR)
#define		COMMAND_RIGHT				COMMAND(PER_DGT_KR)
#define		COMMAND_RIGHT_RELEASED		COMMAND_RELEASED(PER_DGT_KR)
#define		COMMAND_LEFT_PRESSED		COMMAND_PRESSED(PER_DGT_KL)
#define		COMMAND_LEFT				COMMAND(PER_DGT_KL)
#define		COMMAND_LEFT_RELEASED		COMMAND_RELEASED(PER_DGT_KL)
#define		COMMAND_A_PRESSED			COMMAND_PRESSED(PER_DGT_TA)
#define		COMMAND_A			        COMMAND(PER_DGT_TA)
#define		COMMAND_A_RELEASED			COMMAND_RELEASED(PER_DGT_TA)
#define		COMMAND_B_PRESSED			COMMAND_PRESSED(PER_DGT_TB)
#define		COMMAND_B			        COMMAND(PER_DGT_TB)
#define		COMMAND_B_RELEASED			COMMAND_RELEASED(PER_DGT_TB)
#define		COMMAND_C_PRESSED			COMMAND_PRESSED(PER_DGT_TC)
#define		COMMAND_C			        COMMAND(PER_DGT_TC)
#define		COMMAND_C_RELEASED			COMMAND_RELEASED(PER_DGT_TC)
#define		COMMAND_X_PRESSED			COMMAND_PRESSED(PER_DGT_TX)
#define		COMMAND_X			        COMMAND(PER_DGT_TX)
#define		COMMAND_X_RELEASED			COMMAND_RELEASED(PER_DGT_TX)
#define		COMMAND_Y_PRESSED			COMMAND_PRESSED(PER_DGT_TY)
#define		COMMAND_Y			        COMMAND(PER_DGT_TY)
#define		COMMAND_Y_RELEASED			COMMAND_RELEASED(PER_DGT_TY)
#define		COMMAND_Z_PRESSED			COMMAND_PRESSED(PER_DGT_TZ)
#define		COMMAND_Z			        COMMAND(PER_DGT_TZ)
#define		COMMAND_Z_RELEASED			COMMAND_RELEASED(PER_DGT_TZ)
#define		COMMAND_L_PRESSED			COMMAND_PRESSED(PER_DGT_TL)
#define		COMMAND_L			        COMMAND(PER_DGT_TL)
#define		COMMAND_L_RELEASED			COMMAND_RELEASED(PER_DGT_TL)
#define		COMMAND_R_PRESSED			COMMAND_PRESSED(PER_DGT_TR)
#define		COMMAND_R			        COMMAND(PER_DGT_TR)
#define		COMMAND_R_RELEASED			COMMAND_RELEASED(PER_DGT_TR)

#endif

#ifdef __cplusplus
extern "C" {
#endif

char *INP_getVersion();

#ifdef __cplusplus
}
#endif

#endif
