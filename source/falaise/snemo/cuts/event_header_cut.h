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
  struct mode_t {
  enum mode_t_enum_ {
    UNDEFINED = 0,
    FLAG = 0x1,
    RUN_NUMBER = 0x2,
    EVENT_NUMBER = 0x4,
    EVENT_ID_LIST = 0x8
  };
  };

  /// Constructor
  event_header_cut(datatools::logger::priority lp = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~event_header_cut();

  /// Initilization
  virtual void initialize(const datatools::properties& dps,
                          datatools::service_manager& services,
                          cuts::cut_handle_dict_type& cuts);

  /// Reset
  virtual void reset();

  /// Set the 'Event header' bank label/name
  void setEventHeaderTag(const std::string& tag);

  /// Return the 'Event header' bank label/name
  const std::string& getEventHeaderTag() const;

  /// Return the cut mode
  uint32_t cutMode() const;

  /// Check mode FLAG:
  bool cutsOnFlag() const;

  /// Check mode RUN_NUMBER:
  bool cutsOnRunNumber() const;

  /// Check mode EVENT_NUMBER:
  bool cutsOnEventNumber() const;

  /// Check mode LIST_OF_EVENT_IDS:
  bool cutsOnEventIDs() const;

  void setFlagLabel(const std::string& label);

  const std::string& getFlagLabel() const;

  void setMinRunNumber(int min);

  void setMaxRunNumber(int max);

  void setMinEventNumber(int min);

  void setMaxEventNumber(int max);

  void loadEventIDList(const std::string& fname);

 protected:
  /// Default values
  void _set_defaults();

  /// Selection
  virtual int _accept();

 private:
  std::string eventHeaderTag_;  //!< Name of the "Event header" bank
  uint32_t cutMode_;            //!< Mode of the cut

  std::string flagLabel_;  //!< Name of the boolean property in the event header

  // datatools::integer_range runRange_;
  int minRunNumber_;
  int maxRunNumber_;

  int minEventNumber_;
  int maxEventNumber_;

  std::set<datatools::event_id> eventIDs_;

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(event_header_cut)
};

}  // end of namespace cut

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::cut::event_header_cut the name the registered class in the OCD system
DOCD_CLASS_DECLARATION(snemo::cut::event_header_cut)

#endif  // FALAISE_SNEMO_CUT_EVENT_HEADER_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
