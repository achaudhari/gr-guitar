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

#ifndef INCLUDED_GUITAR_REVERB_IMPL_H
#define INCLUDED_GUITAR_REVERB_IMPL_H

#include <guitar/reverb.h>
#include "sparse_iir_filter.h"

namespace gr {
  namespace guitar {

    class reverb_impl : public reverb
    {
     private:
      // Types
      enum filt_type { COMB, ALLPASS };

      struct filt_config {
        filt_config(filt_type type_, double gain_, double delay_):
          type(type_), gain(gain_), delay(delay_) {}

        filt_type type;
        double gain;
        double delay;
      };

      // Parameters
      const double d_samp_rate;
      double d_enabled;
      std::string d_comb_coeff_mode;
      std::string d_allpass_coeff_mode;
      double d_wet_gamma;
      bool d_changed;

      // Filters
      std::vector< sparse_iir_filter<float,float,double>* > d_comb_filters;
      std::vector< sparse_iir_filter<float,float,double>* > d_allpass_filters;

      sparse_iir_filter<float,float,double>* _design_filter(const filt_config& cfg);
      void _recompute_filters();

     public:
      reverb_impl(bool enabled, double samp_rate,
          const std::string& comb_coeff_mode, const std::string& allpass_coeff_mode,
          double wet_gamma);
      ~reverb_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void set_enabled(bool enabled);
      void set_comb_coeff_mode(const std::string& comb_coeff_mode);
      void set_allpass_coeff_mode(const std::string& allpass_coeff_mode);
      void set_wet_gamma(double wet_gamma);
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_REVERB_IMPL_H */

