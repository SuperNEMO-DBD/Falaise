/// \file falaise/snemo/cuts/event_header_cut.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-18
 * Last modified : 2015-06-20
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Utility cut.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_CUT_EVENT_HEADER_CUT_H
#define FALAISE_SNEMO_CUT_EVENT_HEADER_CUT_H 1

// Standard library:
#include <iostream>
#include <set>
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/event_id.h>
#include <datatools/object_configuration_description.h>
// - Bayeux/cuts:
#include <cuts/i_cut.h>

namespace datatools {
class service_manager;
class properties;
}  // namespace datatools

namespace snemo {

namespace cut {

/// \brief A cut performed on the event record's 'event header' bank
class event_header_cut : public cuts::i_cut {
 public:
  /// \brief Mode of the cut
  enum mode_type {
    MODE_UNDEFINED = 0,
    MODE_FLAG = 0x1,
    MODE_RUN_NUMBER = 0x2,
    MODE_EVENT_NUMBER = 0x4,
    MODE_LIST_OF_EVENT_IDS = 0x8
  };

  /// Set the 'Event header' bank label/name
  void set_EH_label(const std::string& EH_label_);

  /// Return the 'Event header' bank label/name
  const std::string& get_EH_label() const;

  /// Return the cut mode
  uint32_t get_mode() const;

  /// Check mode FLAG:
  bool is_mode_flag() const;

  /// Check mode RUN_NUMBER:
  bool is_mode_run_number() const;

  /// Check mode EVENT_NUMBER:
  bool is_mode_event_number() const;

  /// Check mode LIST_OF_EVENT_IDS:
  bool is_mode_list_of_event_ids() const;

  void set_flag_name(const std::string& flag_name_);

  const std::string& get_flag_name() const;

  void set_run_number_min(int run_number_min_);

  void set_run_number_max(int run_number_max_);

  void set_event_number_min(int event_number_min_);

  void set_event_number_max(int event_number_max_);

  void list_of_event_ids_dump(std::ostream& out_ = std::clog) const;

  void list_of_event_ids_load(const std::string& filename_);

  /// Constructor
  event_header_cut(datatools::logger::priority logging_priority_ =
                       datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~event_header_cut();

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
  std::string _EH_label_;  //!< Name of the "Event header" bank
  uint32_t _mode_;         //!< Mode of the cut

  std::string
      _flag_name_;  //!< Name of the boolean property in the event header

  int _run_number_min_;
  int _run_number_max_;

  int _event_number_min_;
  int _event_number_max_;

  std::set<datatools::event_id> _list_of_events_ids_;

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(event_header_cut)
};

}  // end of namespace cut

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::cut::event_header_cut the name the registered class in the OCD
// system
DOCD_CLASS_DECLARATION(snemo::cut::event_header_cut)

#endif  // FALAISE_SNEMO_CUT_EVENT_HEADER_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
