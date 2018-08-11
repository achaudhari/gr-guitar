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


#ifndef INCLUDED_GUITAR_FLANGER_H
#define INCLUDED_GUITAR_FLANGER_H

#include <guitar/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace guitar {

    /*!
     * \brief <+description of block+>
     * \ingroup guitar
     *
     */
    class GUITAR_API flanger : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<flanger> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of guitar::flanger.
       *
       * To avoid accidental use of raw pointers, guitar::flanger's
       * constructor is in a private implementation
       * class. guitar::flanger::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool enabled, double samp_rate, double max_delay, double lfo_freq, double wet_gamma);

      virtual void set_enabled(bool enabled) = 0;
      virtual void set_lfo_freq(double lfo_freq) = 0;
      virtual void set_wet_gamma(double wet_gamma) = 0;
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_FLANGER_H */

