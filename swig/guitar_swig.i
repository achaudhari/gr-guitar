/* -*- c++ -*- */

#define GUITAR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "guitar_swig_doc.i"

%{
#include "guitar/iir_interpolator.h"
#include "guitar/iir_decimator.h"
#include "guitar/shelving_filter.h"
#include "guitar/distortion.h"
#include "guitar/wah_filter.h"
#include "guitar/flanger.h"
#include "guitar/reverb.h"
%}


%include "guitar/iir_interpolator.h"
GR_SWIG_BLOCK_MAGIC2(guitar, iir_interpolator);
%include "guitar/iir_decimator.h"
GR_SWIG_BLOCK_MAGIC2(guitar, iir_decimator);
%include "guitar/shelving_filter.h"
GR_SWIG_BLOCK_MAGIC2(guitar, shelving_filter);
%include "guitar/distortion.h"
GR_SWIG_BLOCK_MAGIC2(guitar, distortion);
%include "guitar/wah_filter.h"
GR_SWIG_BLOCK_MAGIC2(guitar, wah_filter);
%include "guitar/flanger.h"
GR_SWIG_BLOCK_MAGIC2(guitar, flanger);

%include "guitar/reverb.h"
GR_SWIG_BLOCK_MAGIC2(guitar, reverb);
