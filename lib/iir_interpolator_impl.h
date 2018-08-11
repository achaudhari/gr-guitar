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

#ifndef INCLUDED_GUITAR_IIR_INTERPOLATOR_IMPL_H
#define INCLUDED_GUITAR_IIR_INTERPOLATOR_IMPL_H

#include <guitar/iir_interpolator.h>
#include <gnuradio/filter/iir_filter.h>

namespace gr {
  namespace guitar {

    class iir_interpolator_impl : public iir_interpolator
    {
    private:
      bool d_updated;
      filter::kernel::iir_filter<float,float,double,double> *d_iir;
      std::vector<double> d_new_fftaps;
      std::vector<double> d_new_fbtaps;

    public:
      iir_interpolator_impl(int interpolation, const std::vector<double> &fftaps, const std::vector<double> &fbtaps);
      ~iir_interpolator_impl();

      void set_taps(const std::vector<double> &fftaps, const std::vector<double> &fbtaps);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_IIR_INTERPOLATOR_IMPL_H */

