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
#include "shelving_filter_impl.h"

#define PI  3.14159265358979323846

namespace gr {
  namespace guitar {

    shelving_filter::sptr
    shelving_filter::make(double samp_rate, std::string type, double gain, double cutoff_freq)
    {
      return gnuradio::get_initial_sptr
        (new shelving_filter_impl(samp_rate, type, gain, cutoff_freq));
    }

    /*
     * The private constructor
     */
    shelving_filter_impl::shelving_filter_impl(double samp_rate, std::string type, double gain, double cutoff_freq)
      : gr::sync_block("shelving_filter",
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float))),
        d_samp_rate(samp_rate),
        d_type(type), d_gain(gain), d_cutoff_freq(cutoff_freq),
        d_b0(1.0), d_b1(0.0), d_b2(0.0),
        d_a1(0.0), d_a2(0.0),
        d_z1(0.0), d_z2(0.0)
    {
      _design_sos_filter(d_type, d_gain, d_cutoff_freq);
    }

    /*
     * Our virtual destructor.
     */
    shelving_filter_impl::~shelving_filter_impl()
    {
    }

    void
    shelving_filter_impl::set_type(const std::string& type) {
      _design_sos_filter(type, d_gain, d_cutoff_freq);
      d_type = type;
    }

    void
    shelving_filter_impl::set_gain(const double& gain) {
      _design_sos_filter(d_type, gain, d_cutoff_freq);
      d_gain = gain;
    }

    void
    shelving_filter_impl::set_cutoff_freq(const double& cutoff_freq) {
      _design_sos_filter(d_type, d_gain, cutoff_freq);
      d_cutoff_freq = cutoff_freq;
    }

    void
    shelving_filter_impl::_design_sos_filter(std::string type,
        double gain,
        double cutoff_freq)
    {
      // Validate parameters
      if (cutoff_freq <= 0.0) {
        throw std::invalid_argument("shelving_filter: cutoff_freq must be greater than 0");
      }
      if (cutoff_freq >= d_samp_rate/2) {
        throw std::invalid_argument("shelving_filter: cutoff_freq must be less than half the samp_rate");
      }
      bool low_shelf = false;
      if (type == "low-shelf" or type == "high-shelf") {
        low_shelf = (type == "low-shelf");
      } else {
        throw std::invalid_argument("shelving_filter: Invalid filter type. Must be in {low-shelf, high-shelf}");
      }

      // Resonance
      double Q = 1 / sqrt(2);
      double Q_inv = 1 / Q;

      double K = tan(PI * (cutoff_freq/d_samp_rate));
      double K_sq = K * K;
      double V0 = pow(10.0, (gain / 20));
      if (V0 < 1) V0 = 1/V0;  // Invert gain if a cut

      if (low_shelf) {
        if (gain >= 0.0) {
          // Bass boost
          d_b0 = (1 + sqrt(V0)*Q_inv*K + V0*K_sq) / (1 + Q_inv*K + K_sq);
          d_b1 = (2 * (V0*K_sq - 1) ) / (1 + Q_inv*K + K_sq);
          d_b2 = (1 - sqrt(V0)*Q_inv*K + V0*K_sq) / (1 + Q_inv*K + K_sq);
          d_a1 = (2 * (K_sq - 1) ) / (1 + Q_inv*K + K_sq);
          d_a2 = (1 - Q_inv*K + K_sq) / (1 + Q_inv*K + K_sq);
        } else {
          // Bass cut
          d_b0 = (1 + Q_inv*K + K_sq) / (1 + Q_inv*sqrt(V0) *K + V0*K_sq);
          d_b1 = (2 * (K_sq - 1) ) / (1 + Q_inv*sqrt(V0) *K + V0*K_sq);
          d_b2 = (1 - Q_inv*K + K_sq) / (1 + Q_inv*sqrt(V0) *K + V0*K_sq);
          d_a1 = (2 * (V0*K_sq - 1) ) / (1 + Q_inv*sqrt(V0) *K + V0*K_sq);
          d_a2 = (1 - Q_inv*sqrt(V0) *K + V0*K_sq) / (1 + Q_inv*sqrt(V0) *K + V0*K_sq);
        }
      } else {
        if (gain > 0 && !low_shelf) {
          // Treble boost
          d_b0 = (V0 + Q_inv*sqrt(V0) *K + K_sq) / (1 + Q_inv*K + K_sq);
          d_b1 = (2 * (K_sq - V0) ) / (1 + Q_inv*K + K_sq);
          d_b2 = (V0 - Q_inv*sqrt(V0) *K + K_sq) / (1 + Q_inv*K + K_sq);
          d_a1 = (2 * (K_sq - 1) ) / (1 + Q_inv*K + K_sq);
          d_a2 = (1 - Q_inv*K + K_sq) / (1 + Q_inv*K + K_sq);
        } else {
          // Treble cut
          d_b0 = (1 + Q_inv*K + K_sq) / (V0 + Q_inv*sqrt(V0) *K + K_sq);
          d_b1 = (2 * (K_sq - 1) ) / (V0 + Q_inv*sqrt(V0) *K + K_sq);
          d_b2 = (1 - Q_inv*K + K_sq) / (V0 + Q_inv*sqrt(V0) *K + K_sq);
          d_a1 = (2 * ((K_sq)/V0 - 1) ) / (1 + Q_inv/sqrt(V0) *K + (K_sq)/V0);
          d_a2 = (1 - Q_inv/sqrt(V0) *K + (K_sq)/V0) / (1 + Q_inv/sqrt(V0) *K + (K_sq)/V0);
        }
      }
    }

    int
    shelving_filter_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        // Compute SOS filter using using the
        // transposed direct form II representation
        out[i] = d_z1 + (in[i] * d_b0);
        d_z1 = d_z2 + (in[i] * d_b1) - (out[i] * d_a1);
        d_z2 = (in[i] * d_b2) - (out[i] * d_a2);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace guitar */
} /* namespace gr */

