/// \file falaise/snemo/datamodels/clusterized_precalibrated_data.h
/* Author: Emmanuel Chauveau <chauveau@lp2ib.in2p3.fr>
 * Creation date: 2024-01-24
 * Last modified: 2024-03-20
 *
 * Description: SuperNEMO clusterized precalibrated data model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_PRECALIBRATED_DATA_H
#define FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_PRECALIBRATED_DATA_H 1

// Standard library:
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_clear.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/utils.h>

// This project:
#include <falaise/snemo/datamodels/precalibrated_cluster.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO clusterized precalibrated data model
    class clusterized_precalibrated_data
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
      , public datatools::i_clear
    {
    public:
      
      /// Returns the number of clusters
      size_t size() const;

      /// Add a cluster
      void push_back(const precalibrated_cluster_handle& cluster);

      /// Return a non mutable reference to a cluster by index
      const precalibrated_cluster & at(size_t index) const;

      /// Reset the clusters
      virtual void clear() override;

      /// Return the non mutable reference to the collection of clusters
      const precalibrated_cluster_handle_collection & clusters() const;

      /// Return the mutable reference to the collection of clusters
      precalibrated_cluster_handle_collection & clusters();

      /// Return a non mutable reference to the default cluster is any
      const precalibrated_cluster & get_default() const;

      /// Smart print
      ///
      /// Supported options:
      /// \code
      /// boost::property_tree::ptree print_options;
      /// print_options.put("title", "CpCD");
      /// print_options.put("indent", "[debug] ");
      /// print_options.put("list_clusters", true);
      /// print_options.put("list_properties", true);
      /// snemo::datamodel::clusterized_precalibrated_data CpCD;
      /// CpCD.print_tree(std::cout, print_options);
      /// \endcode
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_
                      /**/ = datatools::i_tree_dumpable::empty_options()) const override;

    private:
      
      precalibrated_cluster_handle_collection _clusters_{}; //!< Collection of clusters
      datatools::properties _auxiliaries_{}; //!< Auxiliary properties (unused!)

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::clusterized_precalibrated_data,
                        "snemo::datamodel::clusterized_precalibrated_data")

#endif // FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_PRECALIBRATED_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
