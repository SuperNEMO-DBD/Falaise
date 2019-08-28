/** \file falaise/snemo/cuts/simulated_data_cut.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Simulated data cut.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_CUT_SIMULATED_DATA_CUT_H
#define FALAISE_SNEMO_CUT_SIMULATED_DATA_CUT_H 1

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

class simulated_data_cut : public cuts::i_cut {
 public:
  /// \brief The cut mode
  struct mode_t {
    enum mode_t_enum_ {
      UNDEFINED = 0,
      FLAG = datatools::bit_mask::bit00,
      HAS_HIT_CATEGORY = datatools::bit_mask::bit01,    // simulated_data::has_step_hits
      RANGE_HIT_CATEGORY = datatools::bit_mask::bit02,  // simulated_data::get_number_of_step_hits
      HAS_HIT_PROPERTY = datatools::bit_mask::bit03     //
    };
  };

  /// Constructor
  simulated_data_cut(datatools::logger::priority lp = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~simulated_data_cut();

  /// Initilization
  virtual void initialize(const datatools::properties& dps,
                          datatools::service_manager& services,
                          cuts::cut_handle_dict_type& cuts);

  /// Reset
  virtual void reset();

  /// Set the SD bank key
  void setSDTag(const std::string& tag);

  /// Return the SD bank key
  const std::string& getSDTag() const;

  /// Return the cut mode
  uint32_t getCutMode() const;

  /// Check mode MODE_FLAG:
  bool cutsOnFlag() const;

  /// Check mode MODE_HAS_HIT_CATEGORY:
  bool cutsOnHitCategory() const;

  /// Check mode MODE_RANGE_HIT_CATEGORY:
  bool cutsOnHitCount() const;

  /// Check mode MODE_HAS_HIT_PROPERTY:
  bool cutsOnHitProperty() const;

  /// Set the name of cut mode MODE_FLAG
  void setFlagLabel(const std::string& label);

  /// Return the name of cut mode MODE_FLAG
  const std::string& getFlagLabel() const;

 protected:
  /// Default values
  void _set_defaults();

  /// Selection
  virtual int _accept();

 private:
  std::string SDTag_;  //!< Name of the "Simulated data" bank
  uint32_t cutMode_;   //!< Mode of the cut

  std::string flagLabel_;  //!< Name of the boolean property in the simulated data

  std::string hitCategory_;  //!< Name of the hit category to be checked
  int minHitCount_;          //!< Minimal number of hits in a category
  int maxHitCount_;          //!< Maximal number of hits in a category

  std::string hitPropertyLogic_;  //!< Logic operation between property selection
  std::map<std::string, std::vector<std::string> >
      hitPropertyMap_;  //!< Values of the 'step_hit' property to look for

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(simulated_data_cut)
};

}  // end of namespace cut

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::cut::simulated_data_cut the name the registered class in the OCD system
DOCD_CLASS_DECLARATION(snemo::cut::simulated_data_cut)

#endif  // FALAISE_SNEMO_CUT_SIMULATED_DATA_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
