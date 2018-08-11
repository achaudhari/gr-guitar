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

#ifndef INCLUDED_GUITAR_FLANGER_IMPL_H
#define INCLUDED_GUITAR_FLANGER_IMPL_H

#include <guitar/flanger.h>
#include <boost/circular_buffer.hpp>

namespace gr {
  namespace guitar {

    class flanger_impl : public flanger
    {
     private:
      const double d_samp_rate;
      double d_enabled;
      double d_max_delay;
      double d_lfo_freq;
      double d_wet_gamma;

      double d_lfo_phase;
      boost::circular_buffer<float>* d_delay_line;

      double _gen_lfo_next();

     public:
      flanger_impl(bool enabled, double samp_rate, double max_delay, double lfo_freq, double wet_gamma);
      ~flanger_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void set_enabled(bool enabled);
      void set_lfo_freq(double lfo_freq);
      void set_wet_gamma(double wet_gamma);
};

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_FLANGER_IMPL_H */

