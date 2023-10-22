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

#ifndef HAVE_RB_PCM_H
#define HAVE_RB_PCM_H

#define	PCM_VERSION	"050308"

#include <RB_RETVAL.H>

#define PCM_toPitch(OCT, FNS)   ((FNS & 0x03FF) | ((OCT & 0xF) << 11))
#define PCM_toOCT(pitch)        ((pitch & 0x7800) >> 11)
    // OCT is signed, but only 4 bits wide
    // so sign extend it
#define PCM_toSXT4(OCT)         ((OCT & (1 << 3)) != 0 ? OCT | 0xFFFFFFF0 : OCT)
#define PCM_toFNS(pitch)        (pitch & 0x03FF)


#ifdef __cplusplus
extern "C" {
#endif

//void PCM_swapBytes(unsigned char *data, unsigned int length);
//void PCM_toUnsigned(unsigned char *data, int length);
//void PCM_toSigned(unsigned char *data, int length);
//ReturnValue PCM_muxChannels(unsigned char *to, unsigned char *from, unsigned int length, unsigned char bits);
//ReturnValue PCM_demuxChannels(unsigned char *to, unsigned char *from, unsigned int length, unsigned char bits)
unsigned short PCM_computePitch(double sampleRate);
unsigned int PCM_computeSampleRate(unsigned short pitch);
ReturnValue PCM_toWAV(unsigned char *to, unsigned char *from, unsigned int length, unsigned char stereo, unsigned char bits);
ReturnValue PCM_toPCM2(unsigned char *to, unsigned char *from, unsigned int length, unsigned char stereo, unsigned char bits);
ReturnValue PCM_toPCM(unsigned char *to, unsigned char *from, unsigned int *length, unsigned short *pitch, unsigned char *stereo, unsigned char *bits, unsigned char MSB);

#ifdef __cplusplus
}
#endif


#endif // HAVE_RB_PCM_H
