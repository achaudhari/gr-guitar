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


#ifndef INCLUDED_GUITAR_REVERB_H
#define INCLUDED_GUITAR_REVERB_H

#include <guitar/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace guitar {

    /*!
     * \brief <+description of block+>
     * \ingroup guitar
     *
     */
    class GUITAR_API reverb : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<reverb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of guitar::reverb.
       *
       * To avoid accidental use of raw pointers, guitar::reverb's
       * constructor is in a private implementation
       * class. guitar::reverb::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool enabled, double samp_rate,
          const std::string& comb_coeff_mode, const std::string& allpass_coeff_mode,
          double wet_gamma);
    };

  } // namespace guitar
} // namespace gr

#endif /* INCLUDED_GUITAR_REVERB_H */

