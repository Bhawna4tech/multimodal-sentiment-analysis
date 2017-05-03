/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
 ***************************************************************************E*/


/*  openSMILE component:

waveSink: writes data to an uncompressed PCM WAVE file

*/


#include <iocore/waveSink.hpp>

#define MODULE "cWaveSink"


#define SMILE_SFSTR_8BIT        "8bit"     // 8-bit signed
#define SMILE_SFSTR_16BIT       "16bit"    // 16-bit signed
#define SMILE_SFSTR_24BIT       "24bit"    // 24-bit signed sample in 4byte dword
#define SMILE_SFSTR_24BITp      "24bitp"   // 3-byte packed 24-bit signed value
#define SMILE_SFSTR_32BIT       "32bit"    // 32-bit signed integer
#define SMILE_SFSTR_32BIT_FLOAT "float"    // 32-bit float

#define SMILE_SF_8BIT        0    // 8-bit signed
#define SMILE_SF_16BIT       1    // 16-bit signed
#define SMILE_SF_24BIT       2    // 24-bit signed sample in 4byte dword
#define SMILE_SF_24BITp      3    // 3-byte packed 24-bit signed value
#define SMILE_SF_32BIT       4    // 32-bit signed integer
#define SMILE_SF_32BIT_FLOAT 5    // 32-bit float

SMILECOMPONENT_STATICS(cWaveSink)

SMILECOMPONENT_REGCOMP(cWaveSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CWAVESINK;
  sdescription = COMPONENT_DESCRIPTION_CWAVESINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->makeMandatory(ct->setField("filename","The filename of the PCM wave file to write data to","output.wav"));
    //ct->setField("buffersize","size of data to write at once",2048);
    char * sfdesc = myvprint("openSMILE uses float for all data internally. Thus you must specify your desired sample format for the wave files here. Available formats:\n   '%s' : 8-bit signed \n   '%s' : 16-bit signed\n   '%s' : 24-bit signed\n   '%s' : 24-bit signed packed in 3 bytes\n   '%s' : 32-bit signed integer\n   '%s' : 32-bit float",SMILE_SFSTR_8BIT,SMILE_SFSTR_16BIT,SMILE_SFSTR_24BIT,SMILE_SFSTR_24BITp,SMILE_SFSTR_32BIT,SMILE_SFSTR_32BIT_FLOAT);
    ct->setField("sampleFormat",sfdesc,SMILE_SFSTR_16BIT);
    //ct->setField("sampleFormat","openSMILE uses float for all data internally. Thus you must specify your desired sample format for the wave files here. Available formats:\n   '8bit' : 8-bit signed \n   '16bit' : 16-bit signed\n   '24bit' : 24-bit signed\n   '24bitp' : 24-bit signed packed in 3 bytes\n   '32bit' : 32-bit signed integer\n   'float' : 32-bit float",SMILE_SFSTR_16BIT);
    free(sfdesc);

    ct->setField("flushData","1/0 (on/off) : flush data to disk and update wave header after writing a frame to the output file (default behaviour is to flush only when the file is closed and openSMILE is being terminated via Ctrl+C or at the end-of-input in offline mode)",0);

    // overwrite cDataSink's default blocksize, enabling faster disk access:
    ct->setField("blocksize_sec", NULL , 1.0);
  )

    SMILECOMPONENT_MAKEINFO(cWaveSink);
}

SMILECOMPONENT_CREATE(cWaveSink)

//-----

cWaveSink::cWaveSink(const char *_name) :
  cDataSink(_name),
  fHandle(NULL),
  sampleBuffer(NULL),
  sampleBufferLen(NULL)
{
  // ...
}

void cWaveSink::fetchConfig()
{
  cDataSink::fetchConfig();

  filename = getStr("filename");
  SMILE_DBG(2,"filename = '%s'",filename);
  if (filename == NULL) COMP_ERR("fetchConfig: getStr(filename) returned NULL! missing option in config file?");

  /*
  lag = getInt("lag");
  SMILE_DBG(2,"lag = %i",lag);
*/
  //frameRead = getInt("frameRead");
  //SMILE_DBG(2,"frameRead = %i",frameRead);
/*
  buffersize = getInt("buffersize");
  SMILE_DBG(2,"buffersize = %i",buffersize);
*/

  const char * sampleFormatStr = getStr("sampleFormat");
  if (sampleFormatStr != NULL) {
    SMILE_DBG(2,"sampleFormat = '%s'",sampleFormatStr);
    if (!strcasecmp(sampleFormatStr,SMILE_SFSTR_8BIT)) {
      nBitsPerSample = 8;
      nBytesPerSample = 1;
      sampleFormat = SMILE_SF_8BIT;
    }
    else if (!strcasecmp(sampleFormatStr,SMILE_SFSTR_16BIT)) {
      nBitsPerSample = 16;
      nBytesPerSample = 2;
      sampleFormat = SMILE_SF_16BIT;
    }
    else if (!strcasecmp(sampleFormatStr,SMILE_SFSTR_24BIT)) {
      nBitsPerSample = 24;
      nBytesPerSample = 4;
      sampleFormat = SMILE_SF_24BIT;
    }
    else if (!strcasecmp(sampleFormatStr,SMILE_SFSTR_24BITp)) {
      nBitsPerSample = 24;
      nBytesPerSample = 3;
      sampleFormat = SMILE_SF_24BITp;
    }
    else if (!strcasecmp(sampleFormatStr,SMILE_SFSTR_32BIT)) {
      nBitsPerSample = 32;
      nBytesPerSample = 4;
      sampleFormat = SMILE_SF_32BIT;
    }
    else if (!strcasecmp(sampleFormatStr,SMILE_SFSTR_32BIT_FLOAT)) {
      nBitsPerSample = 32;
      nBytesPerSample = 4;
      sampleFormat = SMILE_SF_32BIT_FLOAT;
    }
    else
    {
      SMILE_IERR(1,"unknown sampleFormat '%s'!",sampleFormatStr);
      COMP_ERR("aborting");
    }
  }

  flushData = getInt("flushData");

}

int cWaveSink::configureReader()
{
  if (blocksizeR_ < 10) blocksizeR_ = 10;
  reader_->setupSequentialMatrixReading( blocksizeR_, blocksizeR_, 0 );
  return 1;
}

int cWaveSink::myFinaliseInstance()
{
  int ret = cDataSink::myFinaliseInstance();
  if (!ret) return 0;

  // open wave file for writing
  if (fHandle == NULL) {
    fHandle = fopen(filename, "wb");  // TODO: support append mode
    if (fHandle == NULL) COMP_ERR("failed to open output file '%s'",filename);
  }

  nBlocks = 0;

  // write dummy header...
  nChannels = reader_->getLevelN();
  curWritePos = writeWaveHeader();
  if (curWritePos == 0) COMP_ERR("failed writing initial wave header to file '%s'! Disk full or read-only filesystem?",filename);
  return ret;
}

int cWaveSink::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, reading value vector:",t);

  // read next buffer from memory:
  cMatrix *mat = reader_->getNextMatrix(0, 0, DMEM_PAD_NONE);
  int ret = writeData(mat);
  if (mat != NULL && flushData) {
    // NOTE: if the application is killed during this flush procedure the wave file might be corrput!
    writeWaveHeader();
    // set file pointer back to end of file for appending the next frame
    fseek(fHandle,0,SEEK_END);
    // flush data to disk
    fflush(fHandle);
  }
  return ret;
}


cWaveSink::~cWaveSink()
{
  if (sampleBuffer!=NULL) free(sampleBuffer);
  if (fHandle != NULL) {
    // write final wave header
    writeWaveHeader();
    fclose(fHandle);
  }
}

//----------------------------------------------------------------------------------

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* WAVE Header struct, valid only for PCM Files */
typedef struct {
  uint32_t	Riff;    /* Must be little endian 0x46464952 (RIFF) */
  uint32_t	FileSize;
  uint32_t	Format;  /* Must be little endian 0x45564157 (WAVE) */

  uint32_t	Subchunk1ID;  /* Must be little endian 0x20746D66 (fmt ) */
  uint32_t	Subchunk1Size;
  uint16_t	AudioFormat;
  uint16_t	NumChannels;
  uint32_t	SampleRate;
  uint32_t	ByteRate;
  uint16_t	BlockAlign;
  uint16_t	BitsPerSample;

  uint32_t	Subchunk2ID;  /* Must be little endian 0x61746164 (data) */
  uint32_t  Subchunk2Size;
} sRiffPcmWaveHeader;

typedef struct {
  uint32_t SubchunkID;
  uint32_t SubchunkSize;
} sRiffChunkHeader;

int cWaveSink::writeWaveHeader()
{
  if (fHandle == NULL) return 0;

  // use reader parameters to determine
  //   nChannels
  //   sampleRate
  long sampleRate;
  sampleRate = (long)( 1.0 / (reader_->getLevelT()) );
  // get sample format from config options

  sRiffPcmWaveHeader head; 
  head.Riff = 0x46464952;  // RIFF
  head.Format = 0x45564157; // WAVE
  head.Subchunk1ID = 0x20746D66; // fmt
  head.Subchunk1Size = 4*4; // size of format chunk
  head.SampleRate = sampleRate;
  head.BitsPerSample = nBitsPerSample;
  head.ByteRate = sampleRate * nChannels * nBytesPerSample;
  head.AudioFormat = 1; // !!! ??
  head.NumChannels = nChannels;
  head.BlockAlign = nChannels * nBytesPerSample;
  head.Subchunk2ID = 0x61746164; // data
  head.Subchunk2Size = nBlocks * nChannels * nBytesPerSample;  // size of wave data chunk
  head.FileSize = sizeof(sRiffPcmWaveHeader)  + head.Subchunk2Size;
  fseek(fHandle, 0, SEEK_SET);
  return (fwrite(&head, sizeof(sRiffPcmWaveHeader), 1, fHandle) == 1 ? sizeof(sRiffPcmWaveHeader) : 0 );
}

int cWaveSink::writeData(cMatrix *m) 
{
  if (m!=NULL) {
    if (m->N != nChannels) { SMILE_IERR(1,"number of chanels is inconsistent! %i <-> %i",m->fmeta->N,nChannels); return 0; }
    else {
      // convert data
      long i;
      if ((m->nT > sampleBufferLen)&&(sampleBuffer!=NULL)) free(sampleBuffer);
      sampleBufferLen = m->nT;
      if (sampleBuffer == NULL) sampleBuffer = malloc(nBytesPerSample*nChannels*m->nT);

      int8_t *b8;
      int16_t *b16;
      int32_t *b32;
      float *b32f;

      switch(sampleFormat) {
      case SMILE_SF_8BIT:
        b8 = (int8_t*)sampleBuffer;
        for (i=0; i<m->nT*nChannels; i++) { b8[i] = (int8_t)round(m->dataF[i] * 127.0); }
        break;
      case SMILE_SF_16BIT:
        b16 = (int16_t*)sampleBuffer;
        for (i=0; i<m->nT*nChannels; i++) { b16[i] = (int16_t)round(m->dataF[i] * 32767.0); }
        break;
      case SMILE_SF_24BIT:
        b32 = (int32_t*)sampleBuffer;
        for (i=0; i<m->nT*nChannels; i++) { b32[i] = (int32_t)round(m->dataF[i] * 32767.0 * 256.0); }
        break;
      case SMILE_SF_24BITp:
        COMP_ERR("24-bit wave file with 3 bytes per sample encoding not yet supported!");
        //int16_t *b16 = buf;
        //for (i=0; i<m->nT*nChannels; i++) { b16[i] = (int16_t)round(m->dataF[i] * 32767.0); }
        //break;
      case SMILE_SF_32BIT:
        b32 = (int32_t*)sampleBuffer;
        for (i=0; i<m->nT*nChannels; i++) { b32[i] = (int32_t)round(m->dataF[i] * 32767.0 * 32767.0 * 2.0); }
        break;
      case SMILE_SF_32BIT_FLOAT:
        b32f = (float*)sampleBuffer;
        for (i=0; i<m->nT*nChannels; i++) { b32f[i] = (float)(m->dataF[i]); }
        break;
      default:
        SMILE_IERR(1,"unknown sampleFormat encountered in writeData(): %i",sampleFormat);
      }

      long written = (long)fwrite(sampleBuffer, nBytesPerSample*nChannels, m->nT, fHandle);
      if (written != m->nT) {
        SMILE_IERR(2, "Data lost during write to output file (%ld of %ld records written)\n ", written, m->nT);
        // TODO: try to repeat write??
      }
      if (written > 0) {
        nBlocks += written;
        curWritePos += nBytesPerSample*nChannels * written;
        return written;
      }

    }
  }
  return 0;
}
