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

#ifndef INCLUDED_GUITAR_SHELVING_FILTER_IMPL_H
#define INCLUDED_GUITAR_SHELVING_FILTER_IMPL_H

#include <guitar/shelving_filter.h>

namespace gr {
  namespace guitar {

    class shelving_filter_impl : public shelving_filter
    {
    private:
      // Filter parameters
      const double d_samp_rate;
      std::string d_type;
      double d_gain;
      double d_cutoff_freq;

      double d_b0, d_b1, d_b2;  // Feedforward coefficients
      double d_a1, d_a2;        // Feedback coefficients
      double d_z1, d_z2;        // Delay line

      void _design_sos_filter(std::string type,
          double gain,
          double cutoff_freq);

    public:
      shelving_filter_impl(double samp_rate, std::string type, double gain, double cutoff_freq);
      ~shelving_filter_impl();

      virtual void set_type(const std::string& type);
      virtual void set_gain(const double& gain);
      virtual void set_cutoff_freq(const double& cutoff_freq);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_SHELVING_FILTER_IMPL_H */

