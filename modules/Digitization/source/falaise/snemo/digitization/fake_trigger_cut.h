/// \file falaise/snemo/digitization/fake_trigger_cut.h
/* Author(s)     : Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
 *                 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                 Yves Lemiere <lemiere@lpccaen.in2p3.fr>
 * Creation date : 2016-02-26
 * Last modified : 2014-02-26
 *
 * Copyright (C) 2012-2016 Guillaume Oliviero <goliviero@lpccaen.in2p3.fr>
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
 * Description:
 *
 * Fake trigger cut : A cut which goal is to process 'fake_trigger_algo'
 * to cut SD files from a datatools::things coming from flsimulate
 * The physical selection for a fake trigger is configurable but the
 * default configuration is 1 PMT hit and 3 Geiger cells hits.
 * Both answer can be save in two different files
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DIGITIZATION_FAKE_TRIGGER_CUT_H
#define FALAISE_SNEMO_DIGITIZATION_FAKE_TRIGGER_CUT_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/cuts:
#include <cuts/i_cut.h>

// This plugin (fake_trigger_cut) :
#include <falaise/snemo/digitization/fake_trigger_algo.h>

namespace datatools {
class service_manager;
class properties;
}  // namespace datatools

namespace snemo {

namespace digitization {

class fake_trigger_cut : public cuts::i_cut {
 public:
  /// Constructor
  fake_trigger_cut(datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

  /// Destructor
  virtual ~fake_trigger_cut();

  /// Set the SD bank key
  void set_SD_label(const std::string& SD_label_);

  /// Return the SD bank key
  const std::string& get_SD_label() const;

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
  std::string _SD_label_;  //!< The label of the simulated data bank

  /// Fake Trigger Algo :
  boost::scoped_ptr<snemo::digitization::fake_trigger_algo> _algo_;

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(fake_trigger_cut)
};

}  // namespace digitization

}  // end of namespace snemo

// OCD support::
#include <datatools/ocd_macros.h>

// @arg snemo::digitization::fake_trigger_cut the name the registered class in the OCD system
DOCD_CLASS_DECLARATION(snemo::digitization::fake_trigger_cut)

#endif  // FALAISE_SNEMO_DIGITIZATION_FAKE_TRIGGER_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
