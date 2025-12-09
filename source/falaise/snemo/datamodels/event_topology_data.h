/// \file falaise/snemo/datamodels/event_topology_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-12-05
 * Last modified: 2025-12-05
 *
 * Description:  SuperNEMO Event Topology Data model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_DATA_H
#define FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_DATA_H 1

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>

// This project:
#include <falaise/snemo/datamodels/event_topology.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO event topology data model
    // Physical interpretation of a collection of partcile tracks as
    // a specific event topology
    class event_topology_data
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
      , public datatools::i_clear
    {  
    public:

			event_topology_data() = default;

			~event_topology_data() override = default;

			std::size_t number_of_topologies() const;
			
			void add_topology(const EventTopologyHdl & event_topology_hdl_);

			const EventTopologyHdl & get_topology(const int index_);
			
      /// Clear the object
      void clear() override;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ 
											/**/ = datatools::i_tree_dumpable::empty_options()) const override;
 
    private:

      EventTopologyHdlCollection _topologies_; ///< Collection of event topology handles
      
      DATATOOLS_SERIALIZATION_DECLARATION()
      
    };

  } // end of namespace datamodel
  
} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::event_topology_data,
                        "snemo::datamodel::event_topology_data")

#endif // FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
