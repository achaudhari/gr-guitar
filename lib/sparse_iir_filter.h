/* -*- c++ -*- */
/*
 * Copyright 2018 Ashish Chaudhari.
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SPARSE_IIR_FILTER_H
#define INCLUDED_SPARSE_IIR_FILTER_H

#include <boost/circular_buffer.hpp>

namespace gr {
  namespace guitar {

  /*!
   * \brief class template for single pole IIR filter
   */
  template<class o_type, class i_type, class tap_type>
  class sparse_iir_filter
  {
  public:
    /*!
     * \brief construct new sparse IIR with given taps
     *
     * computes y(n) = x(n)*ff_first + x(n-D)*ff_last + y(n-D)*fb_last
     */
    sparse_iir_filter(size_t num_taps,
      tap_type ff_first = 1.0, tap_type ff_last = 0.0, tap_type fb_last = 0.0):
        d_prev_input(num_taps - 1), d_prev_output(num_taps - 1),
        d_ff_first(ff_first), d_ff_last(ff_last), d_fb_last(fb_last)
    {
      reset();
    }

    /*!
     * \brief compute a single output value.
     * \returns the filtered input value.
     */
    o_type filter(const i_type input)
    {
      // Accumulate partial sums
      tap_type output = d_fb_last * d_prev_output.front();
      output += d_ff_first * static_cast<tap_type>(input);
      output += d_ff_last * static_cast<tap_type>(d_prev_input.front());
      // Update delay lines
      d_prev_input.push_back(input);
      d_prev_output.push_back(output);
      // Output value
      return static_cast<o_type>(output);
    }

    /*!
     * \brief install \p alpha as the current taps.
     */
    void set_taps(tap_type ff_first = 1.0, tap_type ff_last = 0.0, tap_type fb_last = 0.0)
    {
      d_ff_first = ff_first;
      d_ff_last  = ff_last;
      d_fb_last  = fb_last;
    }

    //! reset state to zero
    void reset()
    {
      for (size_t i = 0; i < d_prev_input.capacity(); i++) {
        d_prev_input.push_back(tap_type(0.0));
        d_prev_output.push_back(tap_type(0.0));
      }
    }

  protected:
    boost::circular_buffer<i_type>   d_prev_input;
    boost::circular_buffer<tap_type> d_prev_output;
    tap_type  d_ff_first, d_ff_last, d_fb_last;
  };

  } /* namespace guitar */
} /* namespace gr */

#endif /* INCLUDED_SPARSE_IIR_FILTER_H */
