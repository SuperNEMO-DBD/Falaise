/// \file falaise/snemo/cuts/calibrated_data_cut.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-11-24
 * Last modified : 2014-01-30
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
 *
 * Description:
 *
 *   Calibrated data cut.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_CUT_CALIBRATED_DATA_CUT_H
#define FALAISE_SNEMO_CUT_CALIBRATED_DATA_CUT_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
// - Bayeux/cuts:
#include <cuts/i_cut.h>

namespace datatools {
class service_manager;
class properties;
}  // namespace datatools

namespace snemo {

namespace cut {

/// \brief A cut performed on the event record's 'calibrated data' bank
class calibrated_data_cut : public cuts::i_cut {
 public:
  /// \brief Type of cut on calibrated data model
  enum mode_type {
    MODE_UNDEFINED = 0,
    MODE_FLAG = datatools::bit_mask::bit00,
    MODE_HAS_HIT_CATEGORY = datatools::bit_mask::bit01,
    MODE_RANGE_HIT_CATEGORY = datatools::bit_mask::bit02,
    MODE_TRACKER_HIT_HAS_TRAITS = datatools::bit_mask::bit03,
    MODE_TRACKER_HIT_IS_DELAYED = datatools::bit_mask::bit04
  };

  /// Set the calibrated data bank label/name
  void set_CD_label(const std::string& CD_label_);

  /// Return the calibrated data bank label/name
  const std::string& get_CD_label() const;

  /// Return the cut mode
  uint32_t get_mode() const;

  /// Check cut mode MODE_FLAG :
  bool is_mode_flag() const;

  /// Check cut mode MODE_HAS_HIT_CATEGORY:
  bool is_mode_has_hit_category() const;

  /// Check cut mode MODE_RANGE_HIT_CATEGORY:
  bool is_mode_range_hit_category() const;

  /// Check cut mode MODE_TRACKER_HIT_HAS_TRAITS:
  bool is_mode_tracker_hit_has_traits() const;

  /// Check cut mode MODE_TRACKER_HIT_IS_DELAYED:
  bool is_mode_tracker_hit_is_delayed() const;

  /// Set the name of cut mode MODE_FLAG
  void set_flag_name(const std::string& flag_name_);

  /// Return the name of cut mode MODE_FLAG
  const std::string& get_flag_name() const;

  /// Constructor
  calibrated_data_cut(
      datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~calibrated_data_cut();

  /// Initilization
  virtual void initialize(const datatools::properties& configuration_,
                          datatools::service_manager& service_manager_,
                          cuts::cut_handle_dict_type& cut_dict_);

  /// Reset
  virtual void reset();

 protected:
  /// Default values
  void _set_defaults();

  /// Selection
  virtual int _accept();

 private:
  std::string _CD_label_;           //!< Name of the "Calibrated data" bank
  uint32_t _mode_;                  //!< Mode of the cut
  std::string _flag_name_;          //!< Name of the boolean property in the calibrated data
  std::string _hit_category_;       //!< Name of the hit category to be checked
  int _hit_category_range_min_;     //!< Minimal number of hits in a category
  int _hit_category_range_max_;     //!< Maximal number of hits in a category
  int _tracker_hit_trait_bits_;     //!< Tracker hit traits bits
  double _tracker_hit_delay_time_;  //!< Tracker hit delayed time
  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(calibrated_data_cut)
};

}  // end of namespace cut

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::cut::calibrated_data_cut the name the registered class in the OCD system
DOCD_CLASS_DECLARATION(snemo::cut::calibrated_data_cut)

#endif  // FALAISE_SNEMO_CUT_CALIBRATED_DATA_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
