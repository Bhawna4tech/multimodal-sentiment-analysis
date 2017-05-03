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

functional: dct of input data

*/


#include <functionals/functionalDCT.hpp>

#define MODULE "cFunctionalDCT"

#define N_FUNCTS  1

#define NAMES     "dct"

const char *dctNames[] = {NAMES};  

SMILECOMPONENT_STATICS(cFunctionalDCT)

SMILECOMPONENT_REGCOMP(cFunctionalDCT)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALDCT;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALDCT;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("firstCoeff","The first DCT coefficient to compute (coefficient 0 corresponds to the DC component)",1);
    ct->setField("lastCoeff","The last DCT coefficient to compute",6);
	  ct->setField("nCoeffs","An alternative option to lastCoeff (this option overwrites lastCoeff, if it is set): the number DCT coefficient to compute (lastCoeff = firstCoeff+nCoeffs-1).",6);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalDCT);
}

SMILECOMPONENT_CREATE(cFunctionalDCT)

//-----

cFunctionalDCT::cFunctionalDCT(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,dctNames),
  firstCoeff(1),
  lastCoeff(6),
  nCo(0),
  costable(NULL),
  factor(0.0),
  tmpstr(NULL),
  costableNin(0)
{
}

void cFunctionalDCT::fetchConfig()
{
  firstCoeff = getInt("firstCoeff");
  SMILE_IDBG(2,"firstCoeff = %i",firstCoeff);
  if (firstCoeff < 0) {
    SMILE_IWRN(2,"firstCoeff < 0 in config. forcing firstCoeff=0 !");
	  firstCoeff = 0;
  }
  
  if (isSet("nCoeffs")) {
    lastCoeff = firstCoeff + getInt("nCoeffs") - 1;
	  SMILE_IDBG(2,"using nCoeffs = %i to overwrite lastCoeff setting",getInt("nCoeffs"));
  } else {
    lastCoeff = getInt("lastCoeff");
  }
  SMILE_IDBG(2,"lastCoeff = %i",lastCoeff);
  enab[0] = 1;

  cFunctionalComponent::fetchConfig();
  nEnab += lastCoeff - firstCoeff;
}

void cFunctionalDCT::initCostable(long Nin, long Nout)
{
  if ((Nin>0)&&(Nout>0)) {
    if (costable != NULL) free(costable);
    costable = (FLOAT_DMEM * )malloc(sizeof(FLOAT_DMEM)*Nin*Nout);
    costableNin = Nin;
    if (costable==NULL) OUT_OF_MEMORY;
    int i,m;
    nCo = lastCoeff - firstCoeff + 1;
    N=Nin;
    /* TODO: norm frequency of DCT components to be independent of input length!! Possible ?? */
    for (i=firstCoeff; i<=lastCoeff; i++) {
      for (m=0; m<Nin; m++) {
        costable[m + (i-firstCoeff)*Nin] = (FLOAT_DMEM)cos(M_PI*(double)i/(double)(N) * ((FLOAT_DMEM)(m) + 0.5) );
      }
    }
    factor = (FLOAT_DMEM)sqrt((double)2.0/(double)(Nin));
  }
}

const char* cFunctionalDCT::getValueName(long i)
{
  const char *n = cFunctionalComponent::getValueName(0);
  // append coefficient number
  if (tmpstr != NULL) free(tmpstr);
  tmpstr = myvprint("%s%i",n,i+firstCoeff);
  return tmpstr;
}

long cFunctionalDCT::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  // (DONE) FIXME: nonZeroFuncts, breaks DCT, because multiple costables will be needed in this case...
  //        only solution, don't use costables in this case, the cos functions must be computed on the fly
  //        therefore we have to find out from the parent component if the nonZeroFuncts option is set.
  // DONE! FIXME 2: for on-line operation the size of the costable might also change... so we have to recompute the costable everytime the Nin is different than the size of the costable... its quite easy, this will also solve fixme 1 ;)

  int i,m;
  if ((Nin>0)&&(out!=NULL)) {
	
    if (costable == NULL || costableNin != Nin) {
      initCostable(Nin,Nout);
	    if (costable == NULL) SMILE_IERR(1,"error initialising costable, probably Nin or Nout == 0 in cFunctionalDCT::process");
	  }

	for (i=0; i < nCo; i++) {
	  out[i] = 0.0;
    for (m=0; m<Nin; m++) {
      out[i] +=  in[m] * costable[m+i*Nin];
    }
    out[i] *= factor;
    if (!finite(out[i])) {
      SMILE_IERR(1,"non-finite value dct[%i] as output, please help to solve this bug... . The value will be set to 0.",i);
      out[i] = 0.0;
    }
  }

    return nCo;
  }
  return 0;
}

/*
long cFunctionalDCT::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalDCT::~cFunctionalDCT()
{
  if (tmpstr != NULL) free(tmpstr);
}

