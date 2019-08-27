// -*- mode: c++ ; -*-
/* i_volume.h
 * Author (s) :   Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-30
 * Last modified: 2014-07-11
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *   Abstract class for geometry volume
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_DETECTOR_I_VOLUME_H
#define FALAISE_SNEMO_VISUALIZATION_DETECTOR_I_VOLUME_H 1

#include <EventBrowser/detector/volume_model.h>
#include <datatools/i_tree_dump.h>

namespace geomtools {
class geom_info;
}

namespace snemo {

namespace visualization {

namespace detector {

/// \brief A generic interface to volume object
class i_volume : public datatools::i_tree_dumpable, public volume_model {
 public:
  /// Get a mutable pointer to volume
  virtual void* grab_volume() = 0;

  /// Get a non-mutable pointer to volume
  virtual const void* get_volume() const = 0;

  /// Virtual method to initialize the volume properties
  virtual void initialize(const geomtools::geom_info& ginfo_) = 0;

  /// Virtual method to refresh volume properties
  virtual void update() = 0;

  /// Virtual method to clear the volume properties
  virtual void clear() = 0;

  /// Virtual method to reset the volume properties
  virtual void reset() = 0;

  /// Virtual method to highlight the volume
  virtual void highlight(const size_t color_) = 0;

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const = 0;

  /// Default dump
  virtual void dump() const = 0;

  /// Default constructor
  i_volume();

  /// Destructor
  virtual ~i_volume();
};

}  // end of namespace detector

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_DETECTOR_I_VOLUME_H

// end of i_volume.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
