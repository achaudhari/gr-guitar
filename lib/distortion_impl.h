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

#ifndef INCLUDED_GUITAR_DISTORTION_IMPL_H
#define INCLUDED_GUITAR_DISTORTION_IMPL_H

#include <guitar/distortion.h>
#include <map>

namespace gr {
  namespace guitar {

    class distortion_impl : public distortion
    {
     private:
      bool d_enabled;
      double d_boost;
      double d_wet_gamma;

      float(*d_dist_func)(const float&);

     public:
      distortion_impl(bool enabled, std::string dist_func, double boost, double wet_gamma);
      ~distortion_impl();

      void set_enabled(bool enabled);
      void set_dist_func(std::string dist_func);
      void set_boost(double boost);
      void set_wet_gamma(double wet_gamma);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_DISTORTION_IMPL_H */

