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
#include "iir_decimator_impl.h"

namespace gr {
  namespace guitar {

    iir_decimator::sptr
    iir_decimator::make(int decimation, const std::vector<double> &fftaps, const std::vector<double> &fbtaps)
    {
      return gnuradio::get_initial_sptr
        (new iir_decimator_impl(decimation, fftaps, fbtaps));
    }

    /*
     * The private constructor
     */
    iir_decimator_impl::iir_decimator_impl(int decimation, const std::vector<double> &fftaps, const std::vector<double> &fbtaps)
      : gr::sync_decimator("iir_decimator",
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float)), decimation),
        d_updated(false)
    {
      d_iir = new filter::kernel::iir_filter<float,float,double,double>(fftaps, fbtaps, false);
    }

    /*
     * Our virtual destructor.
     */
    iir_decimator_impl::~iir_decimator_impl()
    {
      delete d_iir;
    }

    void
    iir_decimator_impl::set_taps(const std::vector<double> &fftaps,
          const std::vector<double> &fbtaps)
    {
      d_new_fftaps = fftaps;
      d_new_fbtaps = fbtaps;
      d_updated = true;
    }

    int
    iir_decimator_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      if(d_updated) {
        d_iir->set_taps(d_new_fftaps, d_new_fbtaps);
        d_updated = false;
      }

      int ninput_items = noutput_items * decimation();
      int ndecim = decimation();

      for (int i = 0; i < ninput_items; i++) {
        // Filter even if we are discarding to ensure that
        // the delay-lines have the correct values
        float tmp = d_iir->filter(in[i]);
        if (i % ndecim == 0) {
          out[i/ndecim] = tmp;
        }
      }

      return noutput_items;
    }

  } /* namespace guitar */
} /* namespace gr */

