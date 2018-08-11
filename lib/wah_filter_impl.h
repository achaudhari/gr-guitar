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

#ifndef INCLUDED_GUITAR_WAH_FILTER_IMPL_H
#define INCLUDED_GUITAR_WAH_FILTER_IMPL_H

#include <guitar/wah_filter.h>

namespace gr {
  namespace guitar {

    //  The wah-wah filter block implements a state-variable filter with dynamic cutoff frequency
    //  where we use the bandpass output and vary its center frequency. The
    //  filter is defined using the following difference equation:
    //  y_l[n] = F*y_b[n] + y_l[n-1]
    //  y_b[n] = F*y_h[n] + y_b[n-1]
    //  y_h[n] = x[n] - y_l[n-1] - Q*y_b[n-1]
    //  %
    //  F = 2*sin(pi*f_cutoff/f_samp)
    //  Q = damp / sqrt(2)
    //
    //  where:
    //  - y_l is the lowpass output
    //  - y_b is the bandpass output
    //  - y_h is the highpass output
    //  - F and Q are damping coefficients
    //  %
    //  Now, f_cutoff varies with time so the values of F vary with time
    //  based on the F = 2*sin(pi*f_cutoff/f_samp) model.
    class wah_filter_impl : public wah_filter
    {
     private:
      const double d_samp_rate;
      const bool d_use_sidechain;
      double d_enabled;
      double d_cutoff_freq_min;
      double d_cutoff_freq_max;
      double d_lfo_freq;
      double d_damp;

      double d_y_lp, d_y_bp, d_y_hp;
      double d_lfo_phase;

      double _gen_lfo_next();
      double _gen_svf_fval(double envelope);

     public:
      wah_filter_impl(bool enabled,
          double samp_rate,
          std::string envelope_src,
          double cutoff_freq_min,
          double cutoff_freq_max,
          double lfo_freq,
          double damp);
      ~wah_filter_impl();

      void set_enabled(double enabled);
      void set_cutoff_freq_min(double cutoff_freq_min);
      void set_cutoff_freq_max(double cutoff_freq_max);
      void set_lfo_freq(double lfo_freq);
      void set_damp(double damp);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_WAH_FILTER_IMPL_H */

