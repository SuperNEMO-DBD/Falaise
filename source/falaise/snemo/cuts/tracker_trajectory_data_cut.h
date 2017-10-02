/// \file falaise/snemo/cuts/tracker_trajectory_data_cut.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-11-27
 * Last modified : 2014-03-20
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
 *   Basic tracker trajectory data cut.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_CUT_TRACKER_TRAJECTORY_DATA_CUT_H
#define FALAISE_SNEMO_CUT_TRACKER_TRAJECTORY_DATA_CUT_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
// - Bayeux/cuts:
#include <cuts/i_cut.h>

namespace snemo {

namespace cut {

/// \brief A cut performed on the event record's 'tracker trajectory data' bank
class tracker_trajectory_data_cut : public cuts::i_cut {
 public:
  /// \brief Mode of the cut
  enum mode_type {
    MODE_UNDEFINED = 0,
    MODE_FLAG = datatools::bit_mask::bit00,
    MODE_HAS_SOLUTION = datatools::bit_mask::bit01,
    MODE_RANGE_SOLUTION = datatools::bit_mask::bit02,
    MODE_RANGE_CHI2NDF = datatools::bit_mask::bit04,
    MODE_RANGE_PVALUE = datatools::bit_mask::bit05
  };

  /// Set the 'Tracker trajectory' bank label/name
  void set_TTD_label(const std::string& TTD_label_);

  /// Return the 'Tracker trajectory' bank label/name
  const std::string& get_TTD_label() const;

  /// Return the cut mode
  uint32_t get_mode() const;

  /// Check mode FLAG:
  bool is_mode_flag() const;

  /// Check mode HAS_SOLUTION:
  bool is_mode_has_solution() const;

  /// Check mode RANGE_SOLUTION:
  bool is_mode_range_solution() const;

  /// Check mode RANGE_CHI2NDF:
  bool is_mode_range_chi2ndf() const;

  /// Check mode RANGE_PVALUE:
  bool is_mode_range_pvalue() const;

  void set_flag_name(const std::string& flag_name_);

  const std::string& get_flag_name() const;

  /// Constructor
  tracker_trajectory_data_cut(
      datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~tracker_trajectory_data_cut();

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
  std::string _TTD_label_;     //!< Name of the "tracker trajectory data" bank
  uint32_t _mode_;             //!< Mode of the cut
  std::string _flag_name_;     //!< Name of the boolean property in the tracker trajectory data
  int _solution_range_min_;    //!< Minimal number of cluster
  int _solution_range_max_;    //!< Maximal number of cluster
  double _chi2ndf_range_min_;  //!< Minimal value of chi2/ndf
  double _chi2ndf_range_max_;  //!< Maximal value of chi2/ndf
  double _pvalue_range_min_;   //!< Minimal value of p-value
  double _pvalue_range_max_;   //!< Maximal value of p-value

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(tracker_trajectory_data_cut)
};

}  // end of namespace cut

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::cut::tracker_trajectory_data_cut the name the registered class in the OCD system
DOCD_CLASS_DECLARATION(snemo::cut::tracker_trajectory_data_cut)

#endif  // FALAISE_SNEMO_CUT_TRACKER_TRAJECTORY_DATA_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
