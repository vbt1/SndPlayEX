/*
        File: WaveFile.Hpp

        By Yung-hsiang Lee, Univ. of Southern California

        All Copyright reserved.

        Header file for .Wav File I/Os

        Distribution of the source file is permitted in original form.
          Modified version of the source file distribution is permitted
          if author's name is retained.

          Further modification or incorporation into other programs are 
          allowed without obtaining further agreement from author.

*/
#ifndef _WAVEFILE_
  #define _WAVEFILE_

#define WAVE_FORMAT_PCM 1

typedef struct {
/* File header */
  char riff[4];
  long filesize;
  char rifftype[4];
} RiffHeader;

typedef struct {
  char chunk_id[4];
  long chunksize;
} Chunk; 

typedef struct {
  short wFormatTag;
  short nChannels;
  long nSamplesPerSec;
  long nAvgBytesPerSec;
  short nBlockAlign;
} WAVEFORMAT;

typedef struct {
//  WAVEFORMAT wf;
// new
  short wFormatTag;
  short nChannels;
  long nSamplesPerSec;
  long nAvgBytesPerSec;
  short nBlockAlign;

  short wBitsPerSample;
} PCMWAVEFORMAT;

typedef struct {
  RiffHeader header;
  Chunk ch_format;
  PCMWAVEFORMAT format;
  Chunk ch_data;
  // Data follows here.
} WaveFile;

void WaveFile_Init(WaveFile *wf);

void WaveFile_Set(WaveFile *wf,
  short channels,
  long samplerate,
  short datasize);

void WaveFile_Fin(WaveFile *wf,
  short channels,
  long samplerate,
  short datasize,
  long totalsamples);

short WaveFile_Write(const char *filename, WaveFile *wf, void *data);

short WaveFile_Read(const char *filename, WaveFile *wf, void **data);

float wave_length(WaveFile *wf);

#endif //_WAVEFILE_
