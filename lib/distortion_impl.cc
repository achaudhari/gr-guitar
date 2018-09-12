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
#include "distortion_impl.h"

#define PI  3.14159265358979323846

namespace gr {
  namespace guitar {

    distortion::sptr
    distortion::make(bool enabled, std::string dist_func, double boost, double wet_gamma)
    {
      return gnuradio::get_initial_sptr
        (new distortion_impl(enabled, dist_func, boost, wet_gamma));
    }

    /*
     * The private constructor
     */
    distortion_impl::distortion_impl(bool enabled, std::string dist_func, double boost, double wet_gamma)
      : gr::sync_block("distortion",
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float))),
        d_enabled(enabled), d_boost(boost), d_wet_gamma(wet_gamma),
        d_dist_func(NULL)
    {
      set_dist_func(dist_func);
    }

    /*
     * Our virtual destructor.
     */
    distortion_impl::~distortion_impl()
    {
    }

    void
    distortion_impl::set_enabled(bool enabled)
    {
      d_enabled = enabled;
    }

    void
    distortion_impl::set_dist_func(std::string dist_func)
    {
      if (dist_func == "L") {
        d_dist_func = [](const float& x){
          return x;
        };
      } else if (dist_func == "Q") {
        d_dist_func = [](const float& x) -> float {
          return (1.0 - (1.0 - x) * (1.0 - x));
        };
      } else if (dist_func == "E") {
        d_dist_func = [](const float& x) -> float {
          return ((1.0 - exp(-1.0 * x)) / exp(-0.5));
        };
      } else if (dist_func == "I") {
        d_dist_func = [](const float& x) -> float {
          return ((2 * x) / (1 + x));
        };
      } else if (dist_func == "S") {
        d_dist_func = [](const float& x) -> float {
          return sin((PI / 2) * x);
        };
      } else {
        throw std::invalid_argument("distortion: Distortion function not supported.");
      }
    }

    float
    distortion_impl::wrap_and_clip(float x)
    {
        const float sign = (x >= 0.0) ? 1.0 : -1.0;
        const float dist_x = (abs(x) * d_boost < 1.0) ? d_dist_func(abs(x) * d_boost) : 1.0;
        return (sign * std::min<float>(dist_x, 1.0));
    }

    void
    distortion_impl::set_boost(double boost)
    {
      d_boost = boost;
    }

    void distortion_impl::set_wet_gamma(double wet_gamma)
    {
      d_wet_gamma = wet_gamma;
    }

    int
    distortion_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        const float dry = in[i];
        const float sign = (dry >= 0.0) ? 1.0 : -1.0;
        out[i] = d_enabled ? (d_wet_gamma*wrap_and_clip(dry) + (1-d_wet_gamma)*dry) : dry;
      }

      return noutput_items;
    }

  } /* namespace guitar */
} /* namespace gr */

