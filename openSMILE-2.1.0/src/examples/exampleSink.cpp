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

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#include <examples/exampleSink.hpp>

#define MODULE "cExampleSink"


SMILECOMPONENT_STATICS(cExampleSink)

SMILECOMPONENT_REGCOMP(cExampleSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CEXAMPLESINK;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLESINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","The name of a text file to dump values to (this file will be overwritten, if it exists)",(const char *)NULL);
    ct->setField("lag","Output data <lag> frames behind",0,0,0);
  )

  SMILECOMPONENT_MAKEINFO(cExampleSink);
}

SMILECOMPONENT_CREATE(cExampleSink)

//-----

cExampleSink::cExampleSink(const char *_name) :
  cDataSink(_name),
  fHandle(NULL)
{
  // ...
}

void cExampleSink::fetchConfig()
{
  cDataSink::fetchConfig();
  
  filename = getStr("filename");
  SMILE_DBG(2,"filename = '%s'",filename);
  lag = getInt("lag");
  SMILE_DBG(2,"lag = %i",lag);
}


/*
int cExampleSink::myConfigureInstance()
{
  return  cDataSink::myConfigureInstance();
  
}
*/


int cExampleSink::myFinaliseInstance()
{
  // FIRST call cDataSink myFinaliseInstance, this will finalise our dataWriter...
  int ret = cDataSink::myFinaliseInstance();

  /* if that was SUCCESSFUL (i.e. ret == 1), then do some stuff like
     loading models or opening output files: */

  if ((ret)&&(filename != NULL)) {
    fHandle = fopen(filename,"w");
    if (fHandle == NULL) {
      SMILE_IERR(1,"failed to open file '%s' for writing!",filename);
      COMP_ERR("aborting");
	    //return 0;
    }
  }

  return ret;
}


int cExampleSink::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, reading value vector:",t);
  cVector *vec= reader_->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  
  // now print the vector:
  int i;
  for (i=0; i<vec->N; i++) {
    //SMILE_PRINT("  (a=%i vi=%i, tm=%fs) %s.%s = %f",reader->getCurR(),vi,tm,reader->getLevelName(),vec->name(i),vec->dataF[i]);
    printf("  %s.%s = %f\n",reader_->getLevelName(),vec->name(i),vec->dataF[i]);
  }
  
  if (fHandle != NULL) {
    for (i=0; i<vec->N; i++) {
      fprintf(fHandle, "%s = %f\n",vec->name(i),vec->dataF[i]);
    }
  }

  // tick success
  return 1;
}


cExampleSink::~cExampleSink()
{
  if (fHandle != NULL) {
    fclose(fHandle);
  }
}

