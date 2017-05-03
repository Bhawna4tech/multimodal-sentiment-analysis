/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


/*  SmileUtil
    =========

contains modular DSP functions
and other utility functions

*/


#ifndef __SMILE_UTIL_H
#define __SMILE_UTIL_H


/* you may remove this include if you are using smileUtil outside of openSMILE */
//#include <smileCommon.hpp>
/* --------------------------------------------------------------------------- */
#ifndef __SMILE_COMMON_H
#define __SMILE_COMMON_H

// this is a minimal set of defines if we are using smileUtil outside of openSMILE
// on linux you should consider compiling with -DHAVE_INTTYPES_H option (see smileTypes.hpp)

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER // Visual Studio specific macro
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
//    #define class class __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
//    #define class class __declspec(dllimport)
  #endif
  #define DLLLOCAL 
#else 
    #define DLLEXPORT 
    #define DLLLOCAL  
#endif

#include <stdlib.h>
#include <stdio.h>

#endif  // __SMILE_COMMON_H


#ifndef __SMILE_LOGGER_HPP
#define __SMILE_LOGGER_HPP
#define LOGLEVEL 2
#define SMILE_PRINT(...) fprintf(stderr,__VA_ARGS__)
#define SMILE_PRINTL(level, ...) { if (level <= LOGLEVEL) fprintf(stderr,__VA_ARGS__); }

#define SMILE_MSG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"MSG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IMSG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"MSG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }

#define SMILE_ERR(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"ERR(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IERR(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"ERR(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }

#define SMILE_WRN(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"WRN(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IWRN(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"WRN(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }


#ifdef DEBUG
#define SMILE_DBG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"DBG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IDBG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"DBG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#endif
#endif  // __SMILE_LOGGER_HPP

#include <core/smileTypes.hpp>
#include <smileutil/smileUtilSpline.h>

#ifdef AUDEERING_PROP
#include <smileutil/smileUtilDspaudeering.h>
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

// --- mathematics ----::

#define MIN( a, b ) ((a < b) ? a : b)
#define MAX( a, b ) ((a > b) ? a : b)


/*******************************************************************************************
 ***********************=====   Misc functions   ===== **************************************
 *******************************************************************************************/

DLLEXPORT int smileUtil_stripline(char ** _line);

/*******************************************************************************************
 ***********************=====   Sort functions   ===== **************************************
 *******************************************************************************************/

  /* NOTE: the quicksort functions sort in ASCENDING order */

/* QuickSort algorithm for a double array with nEl elements */
DLLEXPORT void smileUtil_quickSort_double(double *arr, long nEl);

/* QuickSort algorithm for a float array with nEl elements */
DLLEXPORT void smileUtil_quickSort_float(float *arr, long nEl);

/* QuickSort algorithm for a FLOAT_DMEM array with nEl elements */
DLLEXPORT void smileUtil_quickSort_FLOATDMEM(FLOAT_DMEM *arr, long nEl);

/* Reverse the order in an array of elements, i.e. swap first and last element, etc. */
DLLEXPORT void smileUtil_reverseOrder_FLOATDMEM(FLOAT_DMEM *arr, long nEl);


/*******************************************************************************************
 ***********************=====   Filter functions   ===== **************************************
 *******************************************************************************************/


/* allocate workspace (history matrix) for a temporal median filter */
DLLEXPORT FLOAT_DMEM * smileUtil_temporalMedianFilterInit(long N, long T);
DLLEXPORT FLOAT_DMEM * smileUtil_temporalMedianFilterInitSl(long N, long Ns, long T);

/* free the temporal median filter workspace and return NULL */
DLLEXPORT FLOAT_DMEM * smileUtil_temporalMedianFilterFree(FLOAT_DMEM *workspace);

/*
  Perform median filter of each element in frame x (over time) using a history matrix given in *workspace
  The workspace must be created with smileUtil_temporalMedianFilterInit
*/
DLLEXPORT void smileUtil_temporalMedianFilter(FLOAT_DMEM *x, long N, FLOAT_DMEM *workspace);

/*
  Perform median filter of each element in frame x (over time) using a history matrix given in *workspace
  The workspace must be created with smileUtil_temporalMedianFilterInit.
  workspace : ptr el0 el0 el0(t-1)... el0(t) ; ptr el1 el1 el1(t-1) ... el1(t)
  **> Filter with slave data (Ns is number of slave elements for each element in x (total size of x thus is N*Ns))
*/
DLLEXPORT void smileUtil_temporalMedianFilterWslave(FLOAT_DMEM *x, long N, long Ns, FLOAT_DMEM *workspace);

/*******************************************************************************************
 ***********************=====   FIR filters   ===== ****************************************
 *******************************************************************************************/

typedef struct {
  int offset;   // location of t=0 in impulse response, for non-causal filters
                // actually all implemented filters are causal, but offset will be the delay of the output..
  int N;  // impulse response length
  FLOAT_DMEM * h;  // impulse response
} sSmileDspImpulseResponse;

typedef struct {
  sSmileDspImpulseResponse ir;
  int blocksize;  // processing blocksize (minimum)
  int bufsize;  // input sample buffer size
  FLOAT_DMEM * x_buf;  // input sample buffer
  int x_buf_ptr;
} sSmileDspConvolverState;

// FIR impulse responses are clipped to a finite length N
// This function fades out the impulse response linearly in the
// left and right border regions (10% of the full length at each side)
void smileDsp_impulse_response_linearFadeout(sSmileDspImpulseResponse *ir);

// FIR impulse responses are clipped to a finite length N
// This function fades out the impulse response linearly in the
// left and right border regions (10% of the full length at each side)
// NOTE: the fadeout function breaks normalisation!
// Use normalise = true to normalise the output.
void smileDsp_impulse_response_gaussFadeout(sSmileDspImpulseResponse *ir,
    float sigma, bool normalise);

// Initialises an impulse response structure for N taps
sSmileDspImpulseResponse *smileDsp_impulse_response_init(int N, sSmileDspImpulseResponse *s);

// Normalises an impulse response, so that the convolution of a signal
// with the impulse response has the same gain/amplitude as the input signal
void smileDsp_normalise_impulse_response(sSmileDspImpulseResponse *ir);

// Sinc impulse response for a low-pass filter
// with Gaussian fadeout (default sigma 0.5) (preferred!).
// Standard deviation (sigma) of Gaussian fadeout is relative to
// window length of filter (0.5 spans half the window).
// If highpass is set to true, the filter is "inverted"
// in the frequency domain, i.e. it becomes a high-pass filter.
// Defaults for optional parameters: bool highpass=false, bool fadeout=true
void smileDsp_sincGauss_impulse_response(sSmileDspImpulseResponse *ir,
    float cutoffHz, float Ts, float sigma, bool highpass, bool fadeout);

// Creates a gammatone bandpass impulse response of length ir->N
// with center frequency fc and bandwidth bw for a sampling period of Ts
// set N in ir before calling this function!
void smileDsp_gammatone_impulse_response(sSmileDspImpulseResponse *ir,
    float fc, float bw, float Ts, float a, int order, bool fadeout);

// Creates a gabor bandpass impulse response of length ir->N
// with center frequency fc and bandwidth bw for a sampling period of Ts
// set N in ir before calling this function!
void smileDsp_gabor_impulse_response(sSmileDspImpulseResponse *ir, float fc, float bw, float Ts, bool fadeout);

// Initializes a block convolver work area
sSmileDspConvolverState *smileDsp_block_convolve_init(int blocksize, sSmileDspImpulseResponse *ir,
    sSmileDspConvolverState *s);

// Frees a block convolver work area (if noFreeIr is true, then the impulse
// response ir.h is not freed - use if IRs are shared across multiple filter states)
// if noFreePtr = true, then free(s) won't be called. Only the internal data will be freed.
void smileDsp_block_convolve_destroy(sSmileDspConvolverState * s, bool noFreeIr, bool noFreePtr);

// Performs a discrete convolution on a block of N input samples in *x.
// The output will be saved in *y.
// Using a larger blocksize will reduce the overhead in
//   a) calling this function
//   b) rotating the buffer of past samples
// N must be >= s->blocksize
// outputStep: the distance (in samples between two output samples (for matrix output))
// outputOffset: the index of the first output
void smileDsp_block_convolve(sSmileDspConvolverState *s, FLOAT_DMEM *x,
    FLOAT_DMEM *y, int N, int outputStep, int outputOffset);


/*******************************************************************************************
 ***********************=====   Math functions   ===== **************************************
 *******************************************************************************************/

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

/*
  median of vector x
  (workspace can be a pointer to an array of N FLOAT_DMEMs which is used to sort the data in x without changing x)
  (if workspace is NULL , the function will allocate and free the workspace internally)
*/
DLLEXPORT FLOAT_DMEM smileMath_median(const FLOAT_DMEM *x, long N, FLOAT_DMEM *workspace);

/*
  median of vector x
  (workspace can be a pointer to an array of 2*N (!) FLOAT_DMEMs which is used to sort the data in x without changing x)
  (if workspace is NULL , the function will allocate and free the workspace internally)
  THIS function should return the original vector index of the median in workspace[0] (and workspace[1] if N is even), to use this functionality you must provide a workspace pointer!
*/
DLLEXPORT FLOAT_DMEM smileMath_medianOrdered(const FLOAT_DMEM *x, long N, FLOAT_DMEM *workspace);

/* check if number x is power of 2 (positive or negative) */
DLLEXPORT long smileMath_isPowerOf2(long x);

/* round x to nearest power of two */
DLLEXPORT long smileMath_roundToNextPowOf2(long x);

/* round up x to next power of 2 */
DLLEXPORT long smileMath_ceilToNextPowOf2(long x);

/* round down x to next power of two */
DLLEXPORT long smileMath_floorToNextPowOf2(long x);

/* compute log to base 2 */
__inline double smileMath_log2(double x)
{
  return log(x)/log(2.0);
}


/***** vector math *******/


/* compute euclidean norm of given vector x */
DLLEXPORT FLOAT_DMEM smileMath_vectorLengthEuc(FLOAT_DMEM *x, long N);

/* compute L1 norm (sum of absoulte values) of given vector x */
DLLEXPORT FLOAT_DMEM smileMath_vectorLengthL1(FLOAT_DMEM *x, long N);

/* normalise euclidean length of x to 1 */
DLLEXPORT FLOAT_DMEM smileMath_vectorNormEuc(FLOAT_DMEM *x, long N);

/* normalise vector sum to 1 */
DLLEXPORT FLOAT_DMEM smileMath_vectorNormL1(FLOAT_DMEM *x, long N);

/* normalise values of vector x to range [min - max] */
DLLEXPORT void smileMath_vectorNormMax(FLOAT_DMEM *x, long N, FLOAT_DMEM min, FLOAT_DMEM max);

/* compute the arithmetic mean of vector x */
DLLEXPORT FLOAT_DMEM smileMath_vectorAMean(FLOAT_DMEM *x, long N);

/* root of each element in a vector */
DLLEXPORT void smileMath_vectorRoot(FLOAT_DMEM *x, long N);
/* root of each element in a vector */
DLLEXPORT void smileMath_vectorRootD(double *x, long N);

/****** complex number math ****/

/* compute A/B , store in C */
DLLEXPORT void smileMath_complexDiv(double ReA, double ImA, double ReB, double ImB, double *ReC, double *ImC);

/* absolute value of a complex number */
__inline double smileMath_complexAbs(double Re, double Im)
{
  return sqrt (Re*Re + Im*Im);
}

/* fix roots to inside the unit circle */
// ensure all roots are within the unit circle
// if abs(root) > 1  (outside circle)
// then root = 1 / root*
//
// *roots is an array of n complex numbers (2*n doubles)
DLLEXPORT void smileMath_complexIntoUnitCircle(double *roots, int n);


/***** interpolation ****/

// constructs a parabola from three points
// returns: peak x of parabola, and optional (if not NULL) the y value of the peak in *y and the steepness in *_a
DLLEXPORT double smileMath_quadFrom3pts(double x1, double y1, double x2, double y2, double x3, double y3, double *y, double *_a);


/*******************************************************************************************
 ***********************=====   DSP functions   ===== **************************************
 *******************************************************************************************/

/* compute the equal loudness curve weight, which can be applied to a log-power spectrum
   See Hermansky's 1990 JASA-Article on PLP for details and equations.
*/
double smileDsp_equalLoudnessWeight(double frequency);
double smileDsp_equalLoudnessWeight_htk(double frequency);

/* frequency weighting for computing the psychoacoustic measure of spectral sharpness */
double smileDsp_getSharpnessWeightG(double frq, int frqScale, double param);

/* convert LP coefficients to cepstral representation 
   lpGain is used returned by the Durbin recursion function (smileDsp_calcLpcAcf) 
     for computing LP coefficients and is used herein for computation
     of the "0th" cepstral coefficient, which is the return value of this function
   Details: HTKbook 3.4, equation 5.11
*/
FLOAT_DMEM smileDsp_lpToCeps(const FLOAT_DMEM *lp, int nLp, FLOAT_DMEM lpGain, FLOAT_DMEM *ceps, int firstCC, int lastCC);

  /***** spectral scales (Mel/Bark/etc.) *****/

#define SPECTSCALE_LINEAR   0
#define SPECTSCALE_LOG      1
#define SPECTSCALE_BARK     2
#define SPECTSCALE_BARK_OLD     7
#define SPECTSCALE_MEL      3
#define SPECTSCALE_SEMITONE 4
#define SPECTSCALE_BARK_SCHROED     5
#define SPECTSCALE_BARK_SPEEX       6

// equivalent rectangular bandwidth (in linear frequency)
// of critical band filter at a given linear centre frequency
double smileDsp_ERB(double x);

// +++ forward scale transformation function (source (linear) -> target) +++
// 'x' : is the (linear) source frequency in Hertz
// 'scale' : is one of the following:
//   SPECTSCALE_LOG, SPECTSCALE_SEMITONE, SPECTSCALE_BARK, 
//   SPECTSCALE_BARK_SCHROED, SPECTSCALE_BARK_SPEEX, SPECTSCALE_MEL
// 'param' is either:
//   logScaleBase if scale == SPECTSCALE_LOG
//   firstNote    if scale == SPECTSCALE_SEMITONE
double smileDsp_specScaleTransfFwd(double x, int scale, double param);

// +++ inverse scale transformation function (linear -> target) +++
// 'x' : is the (non-linear) source frequency in Bark/Mel/etc. scale ...
// 'scale' : is one of the following:
//   SPECTSCALE_LOG, SPECTSCALE_SEMITONE, SPECTSCALE_BARK, 
//   SPECTSCALE_BARK_SCHROED, SPECTSCALE_BARK_SPEEX, SPECTSCALE_MEL
// 'param' is either:
//   logScaleBase if scale == SPECTSCALE_LOG
//   firstNote    if scale == SPECTSCALE_SEMITONE
// return value: linear frequency in Hertz
double smileDsp_specScaleTransfInv(double x, int scale, double param);

  /***** window functions *****/

/* definition of window function types */
#define WINF_HANNING    0
#define WINF_HAMMING    1
#define WINF_RECTANGLE  2
#define WINF_RECTANGULAR 2
#define WINF_SINE       3
#define WINF_COSINE     3
#define WINF_GAUSS      4
#define WINF_TRIANGULAR 5
#define WINF_TRIANGLE   5
#define WINF_BARTLETT   6
#define WINF_LANCZOS    7
#define WINF_BARTHANN   8
#define WINF_BLACKMAN   9
#define WINF_BLACKHARR  10
#define WINF_TRIANGULAR_POWERED 11
#define WINF_TRIANGLE_POWERED   11
#define WINF_UNKNOWN  9999


#ifdef _MSC_VER // Visual Studio specific macro
#define inline __inline
#endif

/* sinc function (modified) : (sin 2pi*x) / x */
inline double smileDsp_lcSinc(double x)
{
  double y = M_PI * x;
  return sin(y)/(y);
}

/* sinc function : (sin x) / x  */
inline double smileDsp_sinc(double x)
{
  return sin(x)/(x);
}

#ifdef _MSC_VER // Visual Studio specific macro
#undef inline
#endif

/* Rectangular window */
DLLEXPORT double * smileDsp_winRec(long N_);

/* Triangular window (non-zero endpoint) */
DLLEXPORT double * smileDsp_winTri(long N_);

/* Powered triangular window (non-zero endpoint) */
DLLEXPORT double * smileDsp_winTrP(long N_);

/* Bartlett window (triangular, zero endpoint) */
DLLEXPORT double * smileDsp_winBar(long N_);

/* Hann window */
DLLEXPORT double * smileDsp_winHan(long N_);

/* Hamming window */
DLLEXPORT double * smileDsp_winHam(long N_);

/* Sine window */
DLLEXPORT double * smileDsp_winSin(long N_);

/* Gauss window */
DLLEXPORT double * smileDsp_winGau(long N_, double sigma);

/* Lanczos window */
DLLEXPORT double * smileDsp_winLac(long N_);

/* Blackman window */
DLLEXPORT double * smileDsp_winBla(long N_, double alpha0, double alpha1, double alpha2);

/* Bartlett-Hann window */
DLLEXPORT double * smileDsp_winBaH(long N_, double alpha0, double alpha1, double alpha2);

/* Blackman-Harris window */
DLLEXPORT double * smileDsp_winBlH(long N_, double alpha0, double alpha1, double alpha2, double alpha3);

/* convert string window function name (from config file) to integer constant */
DLLEXPORT int winFuncToInt(const char * winF);

/***** other dsp functions ****/

/* compute harmonic product spectrum from a magnitude spectrum, use up to maxMul down-scaled spectra */
DLLEXPORT long smileDsp_harmonicProductLin(const FLOAT_DMEM *src, long Nsrc, FLOAT_DMEM *dst, long Ndst, int maxMul);

/* compute harmonic sum spectrum from a magnitude spectrum, use up to maxMul down-scaled spectra */
DLLEXPORT long smileDsp_harmonicSumLin(const FLOAT_DMEM *src, long Nsrc, FLOAT_DMEM *dst, long Ndst, int maxMul);

/* LPC analysis via ACF (=implementation of Durbin recursion) */
DLLEXPORT int smileDsp_calcLpcAcf(FLOAT_DMEM * acf, FLOAT_DMEM *lpc, int _p, FLOAT_DMEM *gain, FLOAT_DMEM *refl);

/* smileDsp_calcLpcBurg:
 * Computes LPC coefficients with Burg's method (N. Anderson (1978)):
     N. Anderson (1978), "On the calculation of filter coefficients for maximum entropy spectral analysis", in Childers, Modern Spectrum Analysis, IEEE Press, 252-255.
 Parameters:
   *samples : wave samples,
   N : number of samples
   *coeffs : array which will hold the LPC coefficients
   M : (maximum) number of coefficients to compute
   lpcgain : optional pointer to a FLOAT_DMEM variable, which will be filled with the computed LPC gain
   gbb, gb2, gaa : work area memory,
     pointers will be automatically initialised on the first use,
     they must be freed by the calling code on exit ;
     if these pointers are NULL, smileDsp_calcLpcBurg will allocate
     the work area at the beginning of the function and
     free it at the end of the function
     (the latter is inefficient if memory allocation is slow)
 */
DLLEXPORT int smileDsp_calcLpcBurg (const FLOAT_DMEM *samples, long N,
    FLOAT_DMEM *coeffs, int M, FLOAT_DMEM *lpcgain,
    FLOAT_DMEM **gbb, FLOAT_DMEM **gb2, FLOAT_DMEM **gaa);


/* Autocorrelation in the time domain (for ACF LPC method) */
/* x is signal to correlate, n is number of samples in input buffer, 
   *outp is array to hold #lag output coefficients, lag is # of output coefficients (= max. lag) */
DLLEXPORT void smileDsp_autoCorr(const FLOAT_DMEM *x, const int n, FLOAT_DMEM *outp, int lag);

/* computes formant frequencies and bandwidths from lpc polynomial roots 
   return value: number of valid formants detected from given lpc polynomial roots
r is n complex roots (2*n 'doubles'), nR is number of roots (=n),
*fc is array that will hold formant frequencies (non valid formant entries (index > return value) will be zero),
*bc is array that will hold formant bandwidths (non valid formant entries (index > return value) will be zero),
nF is max. # of formants wanted (must match size of fc and bc array),
samplePeriod is 1.0 / sampleFrequency,
fLow is min. formant frequency (upper and lower margin for valid formant frequency range),
fHigh is max. formant frequency  (if  -1 (or < fLow), "samplingFrequency / 2 - fLow" will be used)
 */
DLLEXPORT int smileDsp_lpcrootsToFormants(double *r, int nR, double *fc, double *bc, int nF, double samplePeriod, double fLow, double fHigh);


/* Implementation of a lattice filter, processes a single value per call */
/* k is filter coefficients, *b is work area (initialise with 0 when calling! (size: sizeof(FLOAT_DMEM)*M )),
   M is filter order (number of coefficients),  *in is input samples array (will contain in-place filtered outputs),
   *bM is an optional b(M) result
   returns: f(M), and filtered data in *in
 */
DLLEXPORT FLOAT_DMEM smileDsp_lattice(FLOAT_DMEM *k, FLOAT_DMEM *b, int M, FLOAT_DMEM in, FLOAT_DMEM *bM);

/* Implementation of an inverse lattice filter, processes a single value per call */
/* k is filter coefficients, *b is work area (initialise with 0 when calling! (size: sizeof(FLOAT_DMEM)*M )),
   M is filter order (number of coefficients),  *out is input samples array (will contain in-place filtered outputs),
   returns: f(M), and filtered data in *out
 */
DLLEXPORT FLOAT_DMEM smileDsp_invLattice(FLOAT_DMEM *k, FLOAT_DMEM *b, int M, FLOAT_DMEM out);

/* peak enhancement in a linear magnitude spectrum */
DLLEXPORT int smileDsp_specEnhanceSHS(double *a, long n);

/* smooth a magnitude spectrum (linear) */
DLLEXPORT void smileDsp_specSmoothSHS(double *a, long n);


/****** simple, slow & full inverse DFT ******/

typedef struct {
  FLOAT_DMEM *costable;  // K*I matrix
  FLOAT_DMEM *sintable;
  long K; // Nsrc
  long kMax; // MIN(Nsrc,Ndst) 
  long I; // Ndst
  int antiAlias;
} sDftWork;

// initialise a inverse RDFT work area
// K is number of frequency bins (input)
// I is the number of actual output samples to compute
// nI is the denominator of the sin/cos functions (usually =I) 
DLLEXPORT sDftWork * smileDsp_initIrdft(long K, long I, double nI, int antialias);

// free a DFT work area
DLLEXPORT sDftWork * smileDsp_freeDftwork(sDftWork * w);

// perform an arbitrary inverse RDFT (slow version)
DLLEXPORT void smileDsp_irdft(const FLOAT_DMEM * inData, FLOAT_DMEM *out, sDftWork *w);

#include <dspcore/fftXg.h>

typedef struct {
  double *winF, *winFo;
  FLOAT_DMEM *x;
  FLOAT_TYPE_FFT * _w;
  int *_ip;
  sDftWork *irdftWork;
} sResampleWork;

DLLEXPORT sResampleWork * smileDsp_resampleWorkFree(sResampleWork * work);
DLLEXPORT sResampleWork * smileDsp_resampleWorkInit(long Nin);
DLLEXPORT int smileDsp_doResample(FLOAT_TYPE_FFT *x, long Nin, FLOAT_DMEM *y, long Nout, double nd, sResampleWork ** _work);

// converts an amplitude ratio a to decibel (dB) 
//     using the equation: 20*log(a)
// a must be > 0
DLLEXPORT double smileDsp_amplitudeRatioToDB(double a);


/*******************************************************************************************
 ***********************=====   Statistics functions   ===== *******************************
 *******************************************************************************************/

/* compute entropy of a "pmf" (the given pmf (in _vals) will be normalised by this function internally) */
DLLEXPORT FLOAT_DMEM smileStat_entropy(const FLOAT_DMEM *vals, long N);

/* compute relative entropy of a "pmf" (may also be unnormalised) */
DLLEXPORT FLOAT_DMEM smileStat_relativeEntropy(const FLOAT_DMEM *vals, long N);

typedef struct {
  long Nbins;
  FLOAT_DMEM min, max;
  FLOAT_DMEM stepsize;
  FLOAT_DMEM *bins;
  FLOAT_DMEM weight; // = number of samples from which the current histogram was built
} sHistogram;

typedef struct {
  uint32_t Nbins;
  float min, max;
  float stepsize;
} sHistogram_file;

#define PROBFLOOR       0.0000000000000000000000001   // 1.0e-25
#define PROBFLOOR_LOG   -1000.0

/* computes a PMF from a sample sequence using a histogram sampling method.
if a valid pointer is given in *_pmf, then the histogram will be added to the existing data,
the resulting histogram will then be unnormalised, you will have to call smileMath_vectorNormEuc in the end.. 
the memory pointed to by *h must be initialised with 0s! (at least h->bins must be NULL...)
*/
DLLEXPORT void smileStat_getPMF(FLOAT_DMEM *_vals, long N, sHistogram *h);

/* Estimate the probability of a vector x with a given pmf (the pmf must be normalised to sum 1!) */
DLLEXPORT FLOAT_DMEM smileStat_probEstim(FLOAT_DMEM x, sHistogram *h, FLOAT_DMEM probFloor);

/* get a PMF vector from a data matrix (result is a pmf histogram for each row of the matrix) */
/* N: number of colums in matrix  , R: number of rows in the matrix (must match the size of *h) */
/* matrix in _vals is read rowwise.. cMatrix in openSMILE is read columnwise, so you must transpose here!
the memory pointed to by *h must be initialised with 0s! (at least h->bins must be NULL...)
*/
void smileStat_getPMFvec(FLOAT_DMEM *_vals, long N, long R, sHistogram *h);

/* get a PMF vector from a data matrix (result is a pmf histogram for each row of the matrix) */
/* *h must point to an allocated array of R x sHistogram 
the memory pointed to by *h must be initialised with 0s! (at least h->bins must be NULL...)
*/
/* N: number of colums in matrix  , R: number of rows in the matrix (must match the size of *h) */
/* matrix in _vals is read columnwise.. use this for compatiblity with openSMILE cMatrix!*/
void smileStat_getPMFvecT(FLOAT_DMEM *_vals, long N, long R, sHistogram *h);

/* estimate probability of a vector belonging to a pmf array */
void smileStat_probEstimVec(FLOAT_DMEM *x, sHistogram *h, FLOAT_DMEM **p, long R, FLOAT_DMEM probFloor);

/* estimate probability of a vector belonging to a pmf array (linear multiplication) */
FLOAT_DMEM smileStat_probEstimVecLin(FLOAT_DMEM *x, sHistogram *h, long R, FLOAT_DMEM probFloor);

/* estimate probability of a vector belonging to a pmf array (log prob addition)*/
FLOAT_DMEM smileStat_probEstimVecLog(FLOAT_DMEM *x, sHistogram *h, long R, FLOAT_DMEM probFloorLog);

/*******************************************************************************************
 ***********************=====   WAVE file I/O   ===== **************************************
 *******************************************************************************************/

#define BYTEORDER_LE    0
#define BYTEORDER_BE    1
#define MEMORGA_INTERLV 0
#define MEMORGA_SEPCH   1


typedef struct {
  long sampleRate;
  int sampleType;
  int nChan;
  int blockSize;
  int nBPS;       // actual bytes per sample
  int nBits;       // bits per sample (precision)
  int byteOrder;  // BYTEORDER_LE or BYTEORDER_BE
  int memOrga;    // MEMORGA_INTERLV  or MEMORGA_SEPCH
  long nBlocks;  // nBlocks in buffer
} sWaveParameters;

/* 
  read the wave file header from fileHandle, store parameters in struct pointed to by pcmParam 
  the file must be opened via fopen()
 */
DLLEXPORT int smilePcm_readWaveHeader(FILE *filehandle, sWaveParameters *pcmParam, const char *filename);

/* parse a wave header from a wave file in memory */
DLLEXPORT int smilePcm_parseWaveHeader(void *raw, long long N, sWaveParameters *pcmParam);

// Convert samples from binary buffer to float array, given the wave parameters
//  *buf : the raw data buffer
//  *pcmParam : parameter struct specifying the sample format
//  *a : The array where the converted buffer will be stored in. It must be pre-allocated to the right size! [nChan * nSamples * sizeof(float)]
//  nChan : number of audio channels,
//  nSamples : number of audio samples in the buffers
//  monoMixdown : 1 = convert from multi-channel recording to mono (1 channel)
//  Return value: number of samples processed (= nSamples)
DLLEXPORT int smilePcm_convertSamples(uint8_t *buf, sWaveParameters *pcmParam, float *a, int nChan, int nSamples, int monoMixdown);

/*read samples from wave file (after reading header). 
  *a  is a pointer to a float array which will receive the data as:  c1s1 c2s1 .. c1s2 c2s2 .... (c1 : channel1, s1 sample1, ...)
  *pcmParam points to the struct filled by smilePcm_readWaveHeader(...)
  nChan and nSamples specify the size of the memory pointed to by *a  (which is sizeof(float)*nChan*nSamples)
     nChan should match the value in pcmParam! (except when monoMixdown = 1, then nChan must always be 1)
  return value : -1 eof (or filehandle==NULL), 0 error, > 0 , num samples read
  the filehandle will automatically be closed and set to NULL at the end of file
*/
DLLEXPORT int smilePcm_readSamples(FILE **filehandle, sWaveParameters *pcmParam, float *a, int nChan, int nSamples, int monoMixdown);


/*******************************************************************************************
 *******************=====   Vector save/load debug helpers   ===== *************************
 *******************************************************************************************/

/* these functions are not safe and should only be used for data output during debugging ! */

DLLEXPORT void saveDoubleVector_csv(const char * filename, double * vec, long N, int append);
DLLEXPORT void saveFloatVector_csv(const char * filename, float * vec, long N, int append);
DLLEXPORT void saveFloatDmemVector_csv(const char * filename, FLOAT_DMEM * vec, long N, int append);
DLLEXPORT void saveDoubleVector_bin(const char * filename, double * vec, long N, int append);
DLLEXPORT void saveFloatVector_bin(const char * filename, float * vec, long N, int append);
DLLEXPORT void saveFloatDmemVector_bin(const char * filename, FLOAT_DMEM * vec, long N, int append);
DLLEXPORT void saveFloatDmemVectorWlen_bin(const char * filename, FLOAT_DMEM * vec, long N, int append);


/** HTK functions **/

typedef struct {
  uint32_t nSamples;
  uint32_t samplePeriod;
  uint16_t sampleSize;
  uint16_t parmKind;
} sHTKheader;

void smileHtk_prepareHeader( sHTKheader *h );
int smileHtk_readHeader(FILE *filehandle, sHTKheader *head);
int smileHtk_writeHeader(FILE *filehandle, sHTKheader *_head);
int smileHtk_IsVAXOrder ();
void smileHtk_SwapFloat( float *p );


//#ifdef __cplusplus
//}
//#endif

#endif  // __SMILE_UTIL_H
