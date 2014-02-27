// -*- mode: c++ ; -*-
/** \file falaise/TrackFit/drawing.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-15
 * Last modified: 2014-02-08
 *
 * Copyright 2012-2014 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * License:
 *
 * Description:
 *
 *   Drawing utilities (for gnuplot)
 *
 * History:
 *
 */

#ifndef FALAISE_TRACKFIT_DRAWING_H
#define FALAISE_TRACKFIT_DRAWING_H 1

// Standard library:
#include <sstream>

// This project:
#include <TrackFit/gg_hit.h>

namespace geomtools {
  class placement;
}

namespace TrackFit {

  class drawing
  {
  public:

    /// Check debug flag
    bool is_debug () const;

    /// Set debug flag
    void set_debug (bool);

    /// Default constructor
    drawing ();

    /// Destructor
    virtual ~drawing ();

    /// Draw a Geiger hit
    void draw (std::ostream & out_,
               const geomtools::placement & p_,
               const gg_hit & hit_) const;

    /// Draw a collection of Geiger hits
    void draw (std::ostream & out_,
               const geomtools::placement & p_,
               const gg_hits_col & hits_) const;
  private:

    bool _debug_; /// Debug flag

  };

} // end of namespace TrackFit

#endif // FALAISE_TRACKFIT_DRAWING_H
