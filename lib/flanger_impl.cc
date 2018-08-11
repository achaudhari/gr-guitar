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
#include "flanger_impl.h"

#define PI  3.14159265358979323846

namespace gr {
  namespace guitar {

    flanger::sptr
    flanger::make(bool enabled, double samp_rate, double max_delay, double lfo_freq, double wet_gamma)
    {
      return gnuradio::get_initial_sptr
        (new flanger_impl(enabled, samp_rate, max_delay, lfo_freq, wet_gamma));
    }

    /*
     * The private constructor
     */
    flanger_impl::flanger_impl(bool enabled, double samp_rate, double max_delay, double lfo_freq, double wet_gamma)
      : gr::sync_block("flanger",
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float))),
        d_samp_rate(samp_rate), d_enabled(enabled),
        d_max_delay(max_delay), d_lfo_freq(lfo_freq), d_wet_gamma(wet_gamma),
        d_lfo_phase(0.0), d_delay_line(NULL)
    {
      const size_t delay_line_len = static_cast<size_t>(d_samp_rate * d_max_delay);
      d_delay_line = new boost::circular_buffer<float>(delay_line_len);
      for (size_t i = 0; i < d_delay_line->capacity(); i++) {
        d_delay_line->push_back(0.0);
      }
    }

    /*
     * Our virtual destructor.
     */
    flanger_impl::~flanger_impl()
    {
    }

    double
    flanger_impl::_gen_lfo_next()
    {
      d_lfo_phase += (2.0 * PI) / d_samp_rate;
      double lfo_val = 0.5 + (-0.5 * cos(d_lfo_phase * d_lfo_freq));
      if (d_lfo_phase > (2 * PI) / d_lfo_freq) {
        d_lfo_phase = 0;
      }
      return lfo_val;
    }

    void
    flanger_impl::set_enabled(bool enabled)
    {
      d_enabled = enabled;
    }

    void
    flanger_impl::set_lfo_freq(double lfo_freq)
    {
      d_lfo_freq = lfo_freq;
    }

    void
    flanger_impl::set_wet_gamma(double wet_gamma)
    {
      d_wet_gamma = wet_gamma;
    }

    int
    flanger_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];
      float *dbg = (float *) output_items[1];

      for (int i = 0; i < noutput_items; i++) {
        const size_t curr_delay = static_cast<size_t>(
           _gen_lfo_next() * (d_delay_line->capacity()-1));
        const float dry = in[i];
        const float wet = (*d_delay_line)[curr_delay];
        out[i] = (d_wet_gamma * wet) + ((1.0 - d_wet_gamma) * dry);

        d_delay_line->push_back(dry);
      }

      return noutput_items;
    }

  } /* namespace guitar */
} /* namespace gr */

