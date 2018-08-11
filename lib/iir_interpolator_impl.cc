/* -*- c++ -*- */
/* 
 * Copyright 2018 Ashish Chaudhari.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "iir_interpolator_impl.h"

namespace gr {
  namespace guitar {

    iir_interpolator::sptr
    iir_interpolator::make(int interpolation, const std::vector<double> &fftaps, const std::vector<double> &fbtaps)
    {
      return gnuradio::get_initial_sptr
        (new iir_interpolator_impl(interpolation, fftaps, fbtaps));
    }

    /*
     * The private constructor
     */
    iir_interpolator_impl::iir_interpolator_impl(int interpolation, const std::vector<double> &fftaps, const std::vector<double> &fbtaps)
      : gr::sync_interpolator("iir_interpolator",
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float)), interpolation),
        d_updated(false)
    {
      d_iir = new filter::kernel::iir_filter<float,float,double,double>(fftaps, fbtaps, false);
    }

    /*
     * Our virtual destructor.
     */
    iir_interpolator_impl::~iir_interpolator_impl()
    {
      delete d_iir;
    }

    void
    iir_interpolator_impl::set_taps(const std::vector<double> &fftaps,
          const std::vector<double> &fbtaps)
    {
      d_new_fftaps = fftaps;
      d_new_fbtaps = fbtaps;
      d_updated = true;
    }

    int
    iir_interpolator_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      float *out = (float*)output_items[0];

      if(d_updated) {
        d_iir->set_taps(d_new_fftaps, d_new_fbtaps);
        d_updated = false;
      }

      int ninput_items = noutput_items / interpolation();
      int ninterp = interpolation();

      for (int i = 0; i < (ninput_items * ninterp); i++) {
        out[i] = d_iir->filter((i % ninterp == 0) ? in[i/ninterp] : 0.0);
      }

      return (ninput_items * ninterp);
    }

  } /* namespace guitar */
} /* namespace gr */

