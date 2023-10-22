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

#include <stdlib.h>
#include <math.h>
#include "RB_PCM.h"
#include "WaveFile.h"

//#include <RB_were_here.h>

// for info on pitch, FNS and OCT,
// look into SCSP user manual: ST-077-R2(-052594).pdf
// it got an error on page 86:
// equation should be:
// Fn = Fo * 2^(n/1200)
// Fo - original frequency
// Fn - n cent high tone frequency 


// from SGL: SOUND.DOC
//Function:
//Plays back PCM music (sound effects). data should be set in the PCM playback 
//data table. In stereo playback, the first half is assigned to the right 
//channel and the second half for the left.
//
//        +-------------------------+
//        |                         |
//        | R data                  |
//        | (frame * datasize byte) |
//        +-------------------------+
//        |                         |
//        | L data                  |
//        |                         |
//        +-------------------------+
//
//PCM data is a structure in the following format.
//
//  typedef struct {
//      Uint8   mode;        /* Playback mode */
//      Uint8   channel;     /* Playback channel */
//      Uint8   level;       /* Playback level */
//      Sint8   pan;         /* Playback pan */
//      Uint16  pitch;       /* Playback pitch */
//      Uint8   eflevelR;    /* Effect level (right and mono) */
//      Uint8   efselectR;   /* Effect number (right and mono) */
//      Uint8   eflevelL;    /* Effect level (left) */
//      Uint8   efselectL;   /* Effect number (left) */
//  } PCM;
//
//The following are set for the playback mode:
//<_Stereo or _Mono> and <_PCM16Bit or _PCM8Bit>
//
//The playback channel is set by slPCMOn (return value for normal termination). 
//Playback level is set from 0 to 127 (lower 4 bits are ignored), and playback 
//pan is set from -127 Left to 0 to +127 Right (lower 3 bits are ignored). 
//Playback pitch is set at 16 bit, 44 KHz. The effect level can be specified 
//from 0 to 7, and the effect number can be specified from 0 to 15.
//
//The return value is 0 to 7 for normal completion, -1 when there is not enough 
//room in the command buffer, -2 when there is no PCM channel available, and -3 
//when there is not enough room in the PCM buffer.
//The PCM playback started with this function stops with the end of data.
//
//Note:
//
//When the playback data resides in work RAM-L, the following restrictions are
//in effect.
//
//  - Playback data must be located from an even address.
//    (The lower 1 bit of the address is used as 0.)
//    -> Place data from an even address.
//  
//  - When playback data is stereo 8 bit, the left channel data must be
//    located from an even address.
//    -> Adjust the number of samples to be even.
//  
//  - A noise glitch occurs when playback data is over 8192 frames and
//    overflows the buffer.
//    -> Define the data so that 1 byte is duplicated every 8192 frames.
//       However, the start of data will begin with a frame offset of 1536
//       frames (600H).  Accordingly, the first data that is duplicated is
//       at 6656 (1A00H).


void PCM_swapBytes(unsigned char *data, unsigned int length)
{
     unsigned char temp;
     int i;
     
     for(i = 0; i < length - 1; i += 2) {
          temp = *(data + i);
          *(data + i) = *(data + i + 1);
          *(data + i + 1) = temp;
     }
}

/* Saturn 8 bit PCM data is in 2's complement notation, on PC it's unsigned */
void PCM_toUnsigned(unsigned char *data, int length)
{
     for(--length; length >= 0; length--) {
          *(data + length) = (unsigned char)(((signed short)*(data + length)) + 128);
     }
}

void PCM_toSigned(unsigned char *data, int length)
{
     for(--length; length >= 0; length--) {
          *(data + length) = (unsigned char)((signed short)*(data + length) - 128);
     }
}

/* mux channels
 * -> used when converting Saturn PCM(rrr lll) to WAV(lr lr lr)
 *
 * stereo assumed
 * length in bytes
 * bytes is number of bytes per sample(1 or 2)
 */
ReturnValue PCM_muxChannels(unsigned char *to, unsigned char *from, unsigned int length, unsigned char bits)
{
     unsigned int i, j = 0;

     if(bits == 8) {
          /* read source consequtively,
           * write target partially */
          /* right channel first */
          for(i = 1; i < length; i += 2) {
               *(to + i) = *(from + j++);
          }
          /* then the left channel */
          for(i = 0; i < length; i += 2) {
               *(to + i) = *(from + j++);
          }

     } else if(bits == 16) {
          /* read file consequtively, write data partially */
          /* right channel first */
          for(i = 2; i < length - 1; i += 4) {
               *(to + i) = *(from + j++);
               *(to + i + 1) = *(from + j++);
          }
          /* then the left channel */
          for(i = 0; i < length - 1; i += 4) {
               *(to + i) = *(from + j++);
               *(to + i + 1) = *(from + j++);
          }
     } else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;
}
#if 0
// Saturn PCM(rrr lll) to WAV(lr lr lr)
// only works on one buffer!
// using some kind of bubble sort
// very very very slow
ReturnValue PCM_muxChannels2(unsigned char *data, unsigned int length, unsigned char bits)
{
    unsigned int i, j = 0;
    if(bits == 8) {
        unsigned char *l, *r, *t, tmp;
        l = data;
        r = data + (length>>1);
//printf("muxChannels2 8!\n");
        // shifting all right channel samples to first position
        for(i = 0; i < length/2; i++) {
            // shifting this sample from right position to left
            for(t = r; l < t; t--) {
                // swap
                tmp = *(t-1);
                *(t-1) = *t;
                *t = tmp;
            }
            // advance pointers
            l += 2;
            r += 1;
        }
    } else if(bits == 16) {
        unsigned short *l, *r, *t, tmp;
        l = (unsigned short *)data;
        r = ((unsigned short *)data) + (length>>2);
//printf("muxChannels2 16!\n");
        // shifting all right channel samples to first position
        for(i = 0; i < (length>>2); i++) {
            // shifting this sample from right position to left
            for(t = r; l < t; t--) {
                // swap
                tmp = *(t-1);
                *(t-1) = *t;
                *t = tmp;
            }
            // advance pointers
            l += 2;
            r += 1;
//            if((i & 0xFF) == 0)
//                printf("i: %i, l: %i, r: %i len1: %i, len2: %i\n", i, l, r, length, (length>>2));
        }
    } else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;
}


// Saturn PCM(rrr lll) to WAV(lr lr lr)
// only works on one buffer!
// takes one sample and places it where it belongs
ReturnValue PCM_muxChannels3(unsigned char *data, unsigned int length, unsigned char bits)
{
    unsigned int o, i;
    
    if(bits == 8) {
        unsigned char s, tmp;
        // take an initial sample
        s = *(data);
        o = 1;
        // every sample is processed one time
        for(i = 0; i < (length >> 0); i++) {
            // got a sample and it's position
            // so put it there
            tmp = *(data + o);
            *(data + o) = s;
            s = tmp;
            // where does the new sample belong?
            if(o < (length >> 1)) {
                // right channel
                o = (o << 1) + 1;
            } else {
                // left channel
                o = (o - (length >> 1)) << 1;                
            }
        }
    } else if(bits == 16) {
        unsigned short s, tmp;
        unsigned short *ptr = (unsigned short *)data;
        // take an initial sample
        s = *(ptr);
        o = 1;
        // every sample is processed one time
        for(i = 0; i < (length >> 1); i++) {
            // got a sample and it's position
            // so put it there
            tmp = *(ptr + o);
            *(ptr + o) = s;
            s = tmp;
            // where does the new sample belong?
            if(o < (length >> 2)) {
                // right channel
                o = (o << 1) + 1;
            } else {
                // left channel
                o = (o - (length >> 2)) << 1;                
            }
        }
    } else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;
}
#endif


/* demux channels while writing 
 * -> used when converting WAV(lr lr lr) to Saturn PCM(rrr lll)
 *
 * stereo assumed
 * length in bytes
 * bytes is number of bytes per sample(1 or 2)
 */
ReturnValue PCM_demuxChannels(unsigned char *to, unsigned char *from, unsigned int length, unsigned char bits)
{
     unsigned int i, j = 0;

     if(bits == 8) {
          /* write file consequtively, read data partially */
          /* right channel first */
          for(i = 1; i < length; i += 2) {
               *(to + j++) = *(from + i);
          }
          /* then the left channel */
          for(i = 0; i < length; i += 2) {
               *(to + j++) = *(from + i);
          }
     } else if(bits == 16) {
          /* write file consequtively, read data partially */
          /* right channel first */
          for(i = 2; i < length - 1; i += 4) {
               *(to + j++) = *(from + i);
               *(to + j++) = *(from + i + 1);
          }
          /* then the left channel */
          for(i = 0; i < length - 1; i += 4) {
               *(to + j++) = *(from + i);
               *(to + j++) = *(from + i + 1);
          }
     } else return RETURN_ERR_INVALIDARG;
     
    return RETURN_OK;        
}
#if 0
//WAV(lr lr lr) to Saturn PCM(rrr lll)
// only works on one buffer
// using permutations
// using some kind of bubble sort
// very very very slow
ReturnValue PCM_demuxChannels2(unsigned char *data, unsigned int length, unsigned char bits)
{
    unsigned int i, j = 0;
    // does not refer to left and right channel,
    // but to left and right pointer

    if(bits == 8) {
        unsigned char *l, *r, *t, tmp;
        l = data;
        r = data + 1;
//printf("demuxChannels2 8!\n");
        // shifting all right channel samples to first position
        for(i = 0; i < length/2; i++) {
            // shifting this sample from right position to left
            for(t = r; l < t; t--) {
                // swap
                tmp = *(t-1);
                *(t-1) = *t;
                *t = tmp;
            }
            // advance pointers
            l += 1;
            r += 2;
        }
    } else if(bits == 16) {
        unsigned short *l, *r, *t, tmp;
        l = (unsigned short *)data;
        r = ((unsigned short *)data) + 1;
//printf("demuxChannels2 16!\n");
        // shifting all right channel samples to first position
        for(i = 0; i < (length>>2); i++) {
            // shifting this sample from right position to left
            for(t = r; l < t; t--) {
                // swap
                tmp = *(t-1);
                *(t-1) = *t;
                *t = tmp;
            }
            // advance pointers
            l += 1;
            r += 2;
//            if((i & 0xFF) == 0)
//                printf("i: %i, l: %i, r: %i len1: %i, len2: %i\n", i, l, r, length, (length>>2));
        }
    } else return RETURN_ERR_INVALIDARG;
     
    return RETURN_OK;        
}


//WAV(lr lr lr) to Saturn PCM(rrr lll)
// only works on one buffer
// LOGOFF.WAV, 313024 bytes
//longest cycle: runs 156511 steps from 156175
// note: 156175 + 336 = 156511
// 336 being the first double written value
ReturnValue PCM_demuxChannels3(unsigned char *data, unsigned int length, unsigned char bits)
{
    unsigned int o, i;
    
    unsigned int maxPos, maxRun, j; 

    if(bits == 8) {
        unsigned char s, tmp;
        // take an initial sample
        s = *(data + 1);
        o = 0;
//printf("bits: %i, length: %i\n", bits, (length >> 1));
        // every sample is processed one time
        for(i = 0; i < (length >> 0); i++) {
//printf("i: %i: o: %i, s: %i\n", i, o, s);            
            // got a sample and it's position
            // so put it there
            tmp = *(data + o);
            *(data + o) = s;
            s = tmp;
            // where does the new sample belong?
            if((o & 1) == 1) {
                // right channel
                o = (o - 1) >> 1;
            } else {
                // left channel
                o = (o >> 1) + (length >> 1);
            }
        }
    } else if(bits == 16) {
        unsigned short s, tmp;
        unsigned short *ptr = (unsigned short *)data;
unsigned short *dirty = (unsigned short *)calloc(length, 1);
        // take an initial sample
        s = *(ptr + 1);
        o = 0;
//printf("bits: %i, length: %i\n", bits, (length >> 1));
    maxRun = maxPos = 0;
    for(j = 0; j < (length >> 1); j++) {
        memset(dirty, 0, length);
        s = 0; // nonsense
        o = j;

        // every sample is processed one time
        for(i = 0; i < (length >> 1); i++) {
//printf("i: %i: o: %i, s: %i\n", i, o, s);            
            // got a sample and it's position
            // so put it there
            if(*(dirty + o) > 0) {
//                printf("! writing dirty sample\n");
                if(maxRun < i) {
                    maxRun = i;
                    maxPos = j;
//                    if((maxRun & 0xFF) == 0)
//                    printf("new longest cycle: runs %i steps from %i\n", maxRun, maxPos);
                    break;
                }
            }    
            tmp = *(ptr + o);
            *(ptr + o) = s;
            s = tmp;
            *(dirty + o) += 1;
            // where does the new sample belong?
            if((o & 1) == 1) {
                // right channel
                o = (o - 1) >> 1;
            } else {
                // left channel
                o = (o >> 1) + (length >> 2);
            }
        }
    }
//    printf("longest cycle: runs %i steps from %i\n", maxRun, maxPos);
 
//        for(i = 0; i < (length >> 1); i++) {
//            if(*(dirty + i) > 1)
//                printf("sample %i written %i times!\n", i, *(dirty + i));
//        }
    } else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;        
}


void PCM_demuxQSortB(unsigned char *data, int l, int r)
{
    int i = l, j = r;
    unsigned char tmp;
    
    do {
        // swap
        tmp = *(data + j);
        *(data + j) = *(data + i);
        *(data + i) = tmp;
        // advance pointers
        i += 2;
        j -= 2;
    } while(i <= j);
    if((r-l) > 1) {
        PCM_demuxQSortB(data, l, l + ((r-l)>>1));
        PCM_demuxQSortB(data, l + ((r-l)>>1) + 1, r);
    }    
}

void PCM_demuxQSortW(unsigned short *data, int l, int r)
{
    int i = l, j = r;
    unsigned short tmp;
    
    do {
        // swap
        tmp = *(data + j);
        *(data + j) = *(data + i);
        *(data + i) = tmp;
        // advance pointers
        i += 2;
        j -= 2;
    } while(i <= j);
    if((r-l) > 1) {
        PCM_demuxQSortW(data, l, l + ((r-l)>>1));
        PCM_demuxQSortW(data, l + ((r-l)>>1) + 1, r);
    }    
}

//WAV(lr lr lr) to Saturn PCM(rrr lll)
// only works on one buffer
// using quicksort
// with sort value implicit
ReturnValue PCM_demuxChannels4(unsigned char *data, unsigned int length, unsigned char bits)
{
    unsigned int o, i;
    
    unsigned int maxPos, maxRun, j; 

    if(bits == 8) {
        PCM_demuxQSortB(data, 0, length-1);
    } else if(bits == 16) {
        PCM_demuxQSortW((unsigned short *)data, 0, (length >> 1)-1);
    } else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;        
}

#endif
// invariant:
// - left and right channels are mixed
// - even positions in left half belong to right half
// - odd positions in right half belong to left half
// - values are increasing by 1 each
// needed for correct division:
// - right half is one bigger per default
// - if the intervall starts at odd position and is not dividable by 2
//   the left half must be bigger 
void PCM_demuxQSort2B(unsigned char *data, int l, int r)
{
    int i, j, r2, l2;
    unsigned char tmp;
    
    // devide intervall
    l2 = (r+l+1) >> 1;      // right half is bigger per default
    // find pointers start position
        // left side
    i = l;
    if((i & 1) != 0) {
        i++;
        if((r & 1) != 0)
            l2++;   // make left half bigger
    }    
        // right side
    j = l2;
    r2 = l2-1;
    if((j & 1) == 0)
        j++;
        
    // check if it has to be sorted
    if((i >= j) || (j > r)) {
//        printf("Intervall [%i, %i] not need sorting!\n", l, r);
//        if(i >= j)
//            printf("i >= j : %i >= %i!\n", i, j);
//        if(j > r)
//            printf("j > r : %i > %i!\n", j, r);
        return;
    }    
    
//    printf("Start demux [%i, %i] at i %i, j %i\n", l, r, i, j);
    // sort to intervall mid
    while((i <= r2) && (j <= r)) {
        // swap
        tmp = *(data + i);
        *(data + i) = *(data + j);
        *(data + j) = tmp;
        // advance offsets
        i += 2;
        j += 2;
    }
    
    // check to sort left half
    if((r2 - l) >= 1) {
//        printf("Sorting left...\n");
        PCM_demuxQSort2B(data, l, r2);
    } //else printf("NOT sorting left...\n");

    // check to sort left half
    if((r - l2) >= 1) {
//        printf("Sorting right...\n");
        PCM_demuxQSort2B(data, l2, r);
    } //else printf("NOT sorting right...\n");
}

void PCM_demuxQSort2W(unsigned short *data, int l, int r)
{
    int i, j, r2, l2;
    unsigned short tmp;
    
    // devide intervall
    l2 = (r+l+1) >> 1;      // right half is bigger per default
    // find pointers start position
        // left side
    i = l;
    if((i & 1) != 0) {
        i++;
        if((r & 1) != 0)
            l2++;   // make left half bigger
    }    
        // right side
    j = l2;
    r2 = l2-1;
    if((j & 1) == 0)
        j++;
        
    // check if it has to be sorted
    if((i >= j) || (j > r)) 
        return;  
    
    // sort to intervall mid
    while((i <= r2) && (j <= r)) {
        // swap
        tmp = *(data + i);
        *(data + i) = *(data + j);
        *(data + j) = tmp;
        // advance offsets
        i += 2;
        j += 2;
    }
    
    // check to sort left half
    if((r2 - l) >= 1) 
        PCM_demuxQSort2W(data, l, r2);

    // check to sort left half
    if((r - l2) >= 1) 
        PCM_demuxQSort2W(data, l2, r);
}

// WAV(lr lr lr) to Saturn PCM(rrr lll)
// only works on one buffer
// using some sort of quicksort
// 
ReturnValue PCM_demuxChannels5(unsigned char *data, unsigned int length, unsigned char bits)
{
    if(bits == 8) {
        PCM_demuxQSort2B(data, 0, length-1);
    } else if(bits == 16) {
        PCM_demuxQSort2W((unsigned short *)data, 0, (length >> 1)-1);
    } else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;        
}

//When the playback data resides in work RAM-L, the following restrictions are
//in effect.
//
//  - Playback data must be located from an even address.
//    (The lower 1 bit of the address is used as 0.)
//    -> Place data from an even address.
//  
//  - When playback data is stereo 8 bit, the left channel data must be
//    located from an even address.
//    -> Adjust the number of samples to be even.
//  
//  - A noise glitch occurs when playback data is over 8192 frames and
//    overflows the buffer.
//    -> Define the data so that 1 byte is duplicated every 8192 frames.
//       However, the start of data will begin with a frame offset of 1536
//       frames (600H).  Accordingly, the first data that is duplicated is
//       at 6656 (1A00H).
//
// input is Saturn raw PCM data
void PCM_shiftB(unsigned char *data, unsigned int start, unsigned int end, signed int offset)
{
    int i;
    
    if(offset > 0) {
        for(i = end; i >= start; i--) {
            *(data + i + offset) = *(data + i);
        }
    } else {
        for(i = start; i <= end; i++) {
            *(data + i + offset) = *(data + i);
        }
    }    
}
void PCM_shiftW(unsigned short *data, unsigned int start, unsigned int end, signed int offset)
{
    int i;
    
    if(offset > 0) {
        for(i = end; i >= start; i--) {
            *(data + i + offset) = *(data + i);
        }
    } else {
        for(i = start; i <= end; i++) {
            *(data + i + offset) = *(data + i);
        }
    }    
}


ReturnValue PCM_toLOWRAM(unsigned char *data, unsigned int *data_length, unsigned int buffer_length, unsigned char stereo, unsigned char bits)
{
    unsigned int dbytes;        // number of duplicated bytes
    int i, start;
    
    // fix case 2
    if((stereo == 1) && (bits == 8)) {
        // number of samples odd?
        if(((*data_length >> 1) & 1) == 1) {
            // cut last sample
            unsigned int i;
            unsigned char tmp;
            // shift (left channel) left one sample
            PCM_shiftB(data, (*data_length >> 1), *data_length, -1);
            // correct size
            *data_length -= 2;
        }
    }
    
    // fix case 3
    // don't know what is meant exactly by frames
    // I assume samples per channel
    //
    if(stereo == 0) {
        if(bits == 8) {
            dbytes = (*data_length + 1536) / 8192 - 1;
            // got enough space?
            if((*data_length + dbytes) > buffer_length)
                return RETURN_ERR_OOB;
            if(dbytes > 0) {
                start = (dbytes * 8192) - 1536;
                // shift last block
                PCM_shiftB(data, start, *data_length, dbytes);
                    // duplicate
                *(data + start - 1) = *(data + start);
                // shift other blocks
                for(i = dbytes-1; i > 0; i--) {
                    PCM_shiftB(data, start - 8192, start, i);
                    start -= 8192;
                        // duplicate
                    *(data + start - 1) = *(data + start);
                }
            }
            
        } else if(bits == 16) {
        } else return RETURN_ERR_INVALIDARG;
    } else {
    }
}



#define PCM_TIMES   1.000577789
unsigned short PCM_computePitch(double sampleRate)
{
    double c, s;
    int OCT, FNS;
    
    // Fn = Fo * 2^(n/1200)

    // compute necessary slowdown / speedup
    // assuming a base frequency of 44100 Hz
    // Fn / Fo = 2^(n/1200)
    // with Fo = 44100 Hz, the saturn base sample frequency
    s = sampleRate / 44100.0;
//    printf("s = %f\n", s);
    
    // compute the pitch to this(cents)
//    n = log(s) / log(PCM_TIMES);
    // log2(Fn / Fo) * 1200 = n
    c = (log(s) / log(2)); // * 1200.0;
//    printf("c1 = %f\n", c);
    if(c >= 0) {
        // compute OCT
        OCT = (int)c;
        // compute FNS
        c -= (double)OCT;
        
    } else {
        // compute OCT
        OCT = (int)c;
        if(c < OCT)
            OCT -= 1;
        // compute FNS
        c = c - ((double)OCT);
            // respect to new base frequency
//        s = sampleRate / (44100.0 / (1 - OCT));
            // n is now >= 0
//        c = (log(s) / log(2));
    }
    
    OCT &= 0xF;
//    printf("OCT = %x\n", OCT);
    // compute FNS
    c *= 1200.0;
//    printf("c2 = %f\n", c);
        // remove OCT from cents,
        // it's a seperate value
        // the argument needs to be in range 0 <= a < 1200
//    printf("(c / 1200.0) = %f\n", (c / 1200.0));
//    printf("(double)((int)(c / 1200.0)) = %f\n", (double)((int)(c / 1200.0)));
//    printf("pow arg = %f\n", (c / 1200.0) - (double)((int)(c / 1200.0)));
//    printf("pow() = %f\n", pow(2, (c / 1200.0) - (double)((int)(c / 1200.0))));
    FNS = ((int)(1024.0 * (pow(2, (c / 1200.0) ) - 1.0))) & 0x3FF;
//    printf("FNS = %i\n", FNS);
// interpret
//    return ((int)(1024.0 * (pow(2, c/1200.0) - 1.0))) & (0x3FF + (0xF << 11));
    
    return PCM_toPitch(OCT, FNS);
}

// reverse operation to above
unsigned int PCM_computeSampleRate(unsigned short pitch)
{
    double s, c;
    int OCT, FNS;

    // extract OCT and FNS
    OCT = PCM_toSXT4(PCM_toOCT(pitch));
        // OCT
    FNS = PCM_toFNS(pitch);

//    printf("OCT %i, FNS %i\n", OCT, FNS);
    // make it signed
//    if((pitch > 0) && ((pitch & (1 << 13)) != 0))
//        pitch |= 3 << 14
    // recompute cent
    c = 1200.0 * (log((1024.0 + ((double)FNS)) / 1024.0) / log(2.0));
//    c = 1200.0 * (log((1024.0 + ((double)pitch)) / 1024) / log(2.0));
//    printf("c1 %f\n", c);
        // OCT is signed
    c += ((double)OCT) * 1200.0;
//    printf("c2 %f\n", c);
    
    // recompute speedup / slowdown
//    s = exp(c * log(PCM_TIMES));
    s = pow(2, c / 1200.0);    
//    printf("s %f\n", s * 44100.0);
    // recompute sampleRate
    return (unsigned int)(s * 44100.0);
}


unsigned short PCM_computePitch_old(float sampleRate)
{
    int OCT, FNS, itemp, i;
    float ftemp;
    // compute pitch
    i = 0;
    // OCT first
    // do we have to increase or decrease playing speed?
    if(sampleRate <= 44100.0) {
        ftemp = 44100.0 / sampleRate;
        // decrease playing speed
        while(ftemp > 1.0) {
//            // look if it's smaller 1
//            if(ftemp <= 1.0)
//                break;
            ftemp = ftemp / 2;
            i++;
        }
        OCT = (0 - i) & 0xF;
        // then FNS
        // if there is rest, there has to be FNS
        itemp = (int)(ftemp * 65536.0) & 0xFFFF;
        if(itemp != 0) {
            // divident
            ftemp = 44100.0 / sampleRate;
            // divisor
            itemp = 1 << i;
            ftemp = ((ftemp / (float)itemp) - 0.5) * 2048;
            FNS = (int)ftemp & 0x3FF;
        } else FNS = 0;
    } else {
        // increase playing speed
        ftemp = sampleRate / 44100.0;
        while(ftemp >= 2.0) {
//            // look if it's smaller 2
//            if(ftemp < 2.0)
//                break;
            ftemp = ftemp / 2;
            i++;
        }
        OCT = i & 0xF;
        // then FNS
        // if there is rest, there has to be FNS
        itemp = (int)(ftemp * 65536.0) & 0xFFFF;
        if(itemp != 0)
            FNS = (int)((ftemp - 1) * 1024) & 0x3FF;     
        else FNS = 0;
    }     
//          OCT = (44100 / (float)(waveFile.format.nSamplesPerSec)) / 2;
//          temp = (2*(int)OCT);
//          FNS = ((44100 / (float)(waveFile.format.nSamplesPerSec)) / (float)temp) / ((float)2/(float)1024);

//          if(OCT < 1)
//               OCT = -(1 / OCT) + 1;
               
//printf("OCT:%x, FNS:%x\n", OCT, FNS);
    return PCM_toPitch(OCT, FNS); //(FNS | ((OCT & 0xF) << 11));
}      


ReturnValue PCM_toWAV(unsigned char *to, unsigned char *from, unsigned int length, unsigned char stereo, unsigned char bits)
{
    ReturnValue ret;

    /* do the data conversion */
    if(stereo) {
        // use two buffers ?
        if((to == NULL) || (to == from)) {
// not supported yet
            return RETURN_ERR_UNKNOWN;
//            ret = PCM_muxChannels3(from, length, bits);
            to = from;
        } else ret = PCM_muxChannels(to, from, length, bits);
        
        if(ret != RETURN_OK)
            return ret;
    } else if((to != NULL) && (to != from))
        memcpy(to, from, length);
        
    if(bits == 8)
        PCM_toUnsigned(to, length);  
    else if(bits == 16)
        PCM_swapBytes(to, length);
    else return RETURN_ERR_INVALIDARG;

    return RETURN_OK;
}


ReturnValue PCM_toPCM2(unsigned char *to, unsigned char *from, unsigned int length, unsigned char stereo, unsigned char bits)
{
    ReturnValue ret;
    
    if(from == NULL)
        return RETURN_ERR_NULLARG;
    
    /* do the data conversion */
    if(bits == 8)
        PCM_toSigned(from, length);  
    else if(bits == 16)     
        PCM_swapBytes(from, length);
    else return RETURN_ERR_INVALIDARG;

    if(stereo) {
        // use one buffer only?
        if((to == NULL) || (to == from)) 
            return PCM_demuxChannels5(from, length, bits);               
        else return PCM_demuxChannels(to, from, length, bits);               
    } else if((to != NULL) && (to != from))
        memcpy(to, from, length);
            
    return RETURN_OK;
}

#define SWAP16(w)    (((w & 0x00FF) << 8) | ((w & 0xFF00) >> 8))
#define SWAP32(w)    (((w & 0x000000FF) << 24) | ((w & 0x0000FF00) << 8) | ((w & 0x00FF0000) >> 8) | ((w & 0xFF000000) >> 24))

ReturnValue PCM_toPCM(unsigned char *to, unsigned char *from, unsigned int *length, unsigned short *pitch, unsigned char *stereo, unsigned char *bits, unsigned char MSB)
{
    WaveFile *waveFile = (WaveFile *)from;
    
    if((to == NULL) || (from == NULL) || (length == NULL))
        return RETURN_ERR_NULLARG;

    if(MSB) {
//    were_here("0 tag %x ch %x bi %x sa %x", waveFile->format.wFormatTag, waveFile->format.nChannels, waveFile->format.wBitsPerSample, waveFile->format.nSamplesPerSec);
        waveFile->format.wFormatTag = SWAP16(waveFile->format.wFormatTag);
        waveFile->format.nChannels = SWAP16(waveFile->format.nChannels);
        waveFile->format.wBitsPerSample = SWAP16(waveFile->format.wBitsPerSample);
        waveFile->format.nSamplesPerSec = SWAP32(waveFile->format.nSamplesPerSec);
        waveFile->ch_data.chunksize = SWAP32(waveFile->ch_data.chunksize);
    }
//    were_here("1 tag %x ch %x bi %x sa %x", waveFile->format.wFormatTag, waveFile->format.nChannels, waveFile->format.wBitsPerSample, waveFile->format.nSamplesPerSec);
    /* check if audio got PCM format */
    if(waveFile->format.wFormatTag != 1) {
//        printf("Audio is not in PCM format!\n");
        *length = 0;
        return RETURN_ERR_UNKNOWN;
    }

    /* wrong size error message 
     * solve problems with win98 audio recorder */
    if(waveFile->ch_data.chunksize > (*length - 44)) {
//        printf("Wrong length in wavefile, taking real file size!\n");
        *length -= 44;
    } else
        *length = waveFile->ch_data.chunksize;
        
    if(stereo != NULL)
        *stereo = (waveFile->format.nChannels == 2);
    if(bits != NULL)
        *bits = waveFile->format.wBitsPerSample;
    // set pitch
    if(pitch != NULL)
        *pitch = PCM_computePitch(waveFile->format.nSamplesPerSec);
    
//    were_here("before 2nd conversion");
    // this is dirty: to == from
    return PCM_toPCM2((to == from ? to+44 : to), from+44, *length, (waveFile->format.nChannels == 2), waveFile->format.wBitsPerSample);
}
