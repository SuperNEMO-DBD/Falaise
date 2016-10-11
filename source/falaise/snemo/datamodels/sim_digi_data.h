/// \file falaise/snemo/datamodels/sim_digi_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated digitized data model
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_H
#define FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_H

// Standard library:
#include <string>
#include <vector>
#include <iostream>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/sim_trigger_digi_data.h>
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about the digitized information
    /// (trigger and hits) associated to a simulated event.
    class sim_digi_data :
      public datatools::i_serializable,
      public datatools::i_tree_dumpable
    {

    public:

      /// The collection of calorimeter digi hits
      typedef std::vector<sim_calo_digi_hit>    calo_digi_hit_col;

      /// The collection of tracker digi hits
      typedef std::vector<sim_tracker_digi_hit> tracker_digi_hit_col;

      /// Constructor
      sim_digi_data();

      /// Destructor
      virtual ~sim_digi_data();

      /// Check validity
      bool is_valid() const;

      /// Reset
      void reset();

      /// Return the const trigger data structure
      const sim_trigger_digi_data & get_trigger_digi_data() const;

      /// Return the mutable trigger data structure
      sim_trigger_digi_data & grab_trigger_digi_data();

      /// Return the const collection of calorimeter digi hits
      const calo_digi_hit_col & get_calo_digi_hits() const;

      /// Return the mutable collection of calorimeter digi hits
      calo_digi_hit_col & grab_calo_digi_hits();

      /// Return the const collection of tracker digi hits
      const tracker_digi_hit_col & get_tracker_digi_hits() const;

      /// Return the mutable collection of tracker digi hits
      tracker_digi_hit_col & grab_tracker_digi_hits();

      /// Return the const container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return the mutable container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit               = false) const;

    private:

      datatools::properties   _auxiliaries_;       //!< Auxiliary properties
      sim_trigger_digi_data   _trigger_digi_data_; //!< Trigger data
      calo_digi_hit_col       _calo_digi_hits_;    //!< Simulated calorimeter digitized hits
      tracker_digi_hit_col    _tracker_digi_hits_; //!< Simulated tracker digitized hits

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::sim_digi_data,
                        "snemo::datamodel::sim_digi_data")

#endif // FALAISE_SNEMO_DATAMODELS_SIM_DIGI_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
