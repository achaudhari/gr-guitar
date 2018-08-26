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
#include "wah_filter_impl.h"

#define PI  3.14159265358979323846

namespace gr {
  namespace guitar {

  wah_filter::sptr
    wah_filter::make(bool enabled,
        double samp_rate,
        std::string envelope_src,
        double cutoff_freq_min,
        double cutoff_freq_max,
        double lfo_freq,
        double damp)
    {
      return gnuradio::get_initial_sptr
        (new wah_filter_impl(enabled, samp_rate, envelope_src, cutoff_freq_min, cutoff_freq_max, lfo_freq, damp));
    }

    /*
     * The private constructor
     */
    wah_filter_impl::wah_filter_impl(bool enabled,
        double samp_rate,
        std::string envelope_src,
        double cutoff_freq_min,
        double cutoff_freq_max,
        double lfo_freq,
        double damp)
      : gr::sync_block("wah_filter",
        gr::io_signature::make(1, 2, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float))),
        d_samp_rate(samp_rate), d_use_sidechain(envelope_src == "S"),
        d_enabled(enabled),
        d_cutoff_freq_min(cutoff_freq_min), d_cutoff_freq_max(cutoff_freq_max),
        d_lfo_freq(lfo_freq), d_damp(damp),
        d_y_lp(0.0), d_y_bp(0.0), d_y_hp(0.0),
        d_lfo_phase(0.0)
    {
    }

    /*
     * Our virtual destructor.
     */
    wah_filter_impl::~wah_filter_impl()
    {
    }

    void
    wah_filter_impl::set_enabled(double enabled)
    {
      d_enabled = enabled;
    }

    void
    wah_filter_impl::set_cutoff_freq_min(double cutoff_freq_min)
    {
      d_cutoff_freq_min = cutoff_freq_min;
    }

    void
    wah_filter_impl::set_cutoff_freq_max(double cutoff_freq_max)
    {
      d_cutoff_freq_max = cutoff_freq_max;
    }

    void
    wah_filter_impl::set_lfo_freq(double lfo_freq)
    {
      d_lfo_freq = lfo_freq;
    }

    void
    wah_filter_impl::set_damp(double damp)
    {
      d_damp = damp;
    }

    double
    wah_filter_impl::_gen_lfo_next()
    {
      d_lfo_phase += (2 * PI) / d_samp_rate;
      double lfo_val = 0.5 + (0.5 * sin(d_lfo_phase * d_lfo_freq));
      if (d_lfo_phase > (2 * PI) / d_lfo_freq) {
        d_lfo_phase = 0;
      }
      return lfo_val;
    }

    double
    wah_filter_impl::_gen_svf_fval(double envelope)
    {
      double curr_freq = d_cutoff_freq_min + ((d_cutoff_freq_max - d_cutoff_freq_min) * envelope);
      return 2 * sin((PI * curr_freq) / d_samp_rate);
    }

    int
    wah_filter_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      const float *sc = (const float *) input_items[1];
      float *out = (float *) output_items[0];

      double Qval = d_damp / sqrt(2);

      for (int i = 0; i < noutput_items; i++) {
        double envelope = d_use_sidechain ? sc[i] : _gen_lfo_next();
        double Fval = _gen_svf_fval(envelope);

        d_y_hp = in[i] - d_y_lp - (Qval * d_y_bp);
        d_y_bp = (Fval * d_y_hp) + d_y_bp;
        d_y_lp = (Fval * d_y_bp) + d_y_lp;
        // Output is the bandpass output of the SVF
        out[i] = d_enabled ? static_cast<float>(d_y_bp) : in[i];
      }

      return noutput_items;
    }

  } /* namespace guitar */
} /* namespace gr */

