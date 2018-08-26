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
#include "reverb_impl.h"

namespace gr {
  namespace guitar {

    reverb::sptr
    reverb::make(bool enabled, double samp_rate,
        const std::string& comb_coeff_mode, const std::string& allpass_coeff_mode,
        double wet_gamma)
    {
      return gnuradio::get_initial_sptr
        (new reverb_impl(enabled, samp_rate, comb_coeff_mode, allpass_coeff_mode, wet_gamma));
    }

    /*
     * The private constructor
     */
    reverb_impl::reverb_impl(bool enabled, double samp_rate,
        const std::string& comb_coeff_mode, const std::string& allpass_coeff_mode,
        double wet_gamma)
      : gr::sync_block("reverb",
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float))),
        d_samp_rate(samp_rate), d_enabled(enabled),
        d_comb_coeff_mode(comb_coeff_mode), d_allpass_coeff_mode(allpass_coeff_mode),
        d_wet_gamma(wet_gamma)
    {
      _recompute_filters();
    }

    /*
     * Our virtual destructor.
     */
    reverb_impl::~reverb_impl()
    {
      for (size_t i = 0; i < d_comb_filters.size(); i++) {
        delete d_comb_filters[i];
      }
      for (size_t i = 0; i < d_allpass_filters.size(); i++) {
        delete d_allpass_filters[i];
      }
    }

    void
    reverb_impl::set_enabled(bool enabled)
    {
      d_enabled = enabled;
    }

    void
    reverb_impl::set_comb_coeff_mode(const std::string& comb_coeff_mode)
    {
      d_comb_coeff_mode = comb_coeff_mode;
      d_changed = true;
    }

    void
    reverb_impl::set_allpass_coeff_mode(const std::string& allpass_coeff_mode)
    {
      d_allpass_coeff_mode = allpass_coeff_mode;
      d_changed = true;
    }

    void
    reverb_impl::set_wet_gamma(double wet_gamma)
    {
      d_wet_gamma = wet_gamma;
    }

    sparse_iir_filter<float,float,double>*
    reverb_impl::_design_filter(const filt_config& cfg)
    {
      const size_t num_taps = static_cast<size_t>(cfg.delay * d_samp_rate);
      const double ff_first = (cfg.type == COMB) ? 0.0 : cfg.gain;
      const double ff_last  = 1.0;
      const double fb_last  = (cfg.type == COMB) ? -cfg.gain : cfg.gain;
      return new sparse_iir_filter<float,float,double>(num_taps, ff_first, ff_last, fb_last);
    }

    void
    reverb_impl::_recompute_filters()
    {
      for (size_t i = 0; i < d_comb_filters.size(); i++) {
        delete d_comb_filters[i];
      }
      d_comb_filters.clear();
      for (size_t i = 0; i < d_allpass_filters.size(); i++) {
        delete d_allpass_filters[i];
      }
      d_allpass_filters.clear();

      if (d_comb_coeff_mode == "P") {
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, 0.805, 0.0204)));
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, 0.827, 0.0176)));
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, 0.783, 0.0229)));
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, 0.764, 0.0254)));
      } else {
        auto rand_gain = []() -> float {
          return ((rand() / (float)RAND_MAX * 0.300) + 0.600);
        };
        auto rand_del = []() -> float {
          return ((rand() / (float)RAND_MAX * 0.015) + 0.015);
        };
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, rand_gain(), rand_del())));
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, rand_gain(), rand_del())));
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, rand_gain(), rand_del())));
        d_comb_filters.push_back(   _design_filter(filt_config(COMB, rand_gain(), rand_del())));
      }

      if (d_allpass_coeff_mode == "P") {
        d_allpass_filters.push_back(_design_filter(filt_config(ALLPASS, 0.700, 0.0028)));
        d_allpass_filters.push_back(_design_filter(filt_config(ALLPASS, 0.700, 0.0009)));
        d_allpass_filters.push_back(_design_filter(filt_config(ALLPASS, 0.700, 0.0003)));
      } else {
        auto rand_del = []() -> float {
          return ((rand() / (float)RAND_MAX * 0.0100) + 0.0001);
        };
        d_allpass_filters.push_back(_design_filter(filt_config(ALLPASS, 0.700, rand_del())));
        d_allpass_filters.push_back(_design_filter(filt_config(ALLPASS, 0.700, rand_del())));
        d_allpass_filters.push_back(_design_filter(filt_config(ALLPASS, 0.700, rand_del())));
      }
    }

    int
    reverb_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      if (d_changed) {
        _recompute_filters();
        d_changed = false;
      }

      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        double acc = 0.0;
        // Parallel comb filters
        for (int c = 0; c < d_comb_filters.size(); c++) {
          acc += d_comb_filters[c]->filter(in[i]);
        }
        // Serial allpass filters
        for (int a = 0; a < d_allpass_filters.size(); a++) {
          acc += d_allpass_filters[a]->filter(acc);
        }
        float wet = static_cast<float>(acc);
        out[i] = d_enabled ? ((d_wet_gamma * wet) + ((1.0 - d_wet_gamma) * in[i])) : in[i];
      }

      return noutput_items;
    }

  } /* namespace guitar */
} /* namespace gr */

