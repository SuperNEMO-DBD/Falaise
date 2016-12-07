/// \file falaise/snemo/datamodels/sim_readout_digi_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated digitized readout data model
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_READOUT_DIGI_DATA_H
#define FALAISE_SNEMO_DATAMODELS_SIM_READOUT_DIGI_DATA_H

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
// #include <falaise/snemo/digitization/readout_structures.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about the digitized readout data
    class sim_readout_digi_data :
      public datatools::i_serializable,
      public datatools::i_tree_dumpable
    {

    public:

      /// Constructor
      sim_readout_digi_data();

      /// Destructor
      virtual ~sim_readout_digi_data();

      /// Check validity
      bool is_valid() const;

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit               = false) const;

    private:

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_SIM_READOUT_DIGI_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
