/// \file falaise/snemo/datamodels/clusterized_calibrated_data.h
/* Author: Emmanuel Chauveau <chauveau@lp2ib.in2p3.fr>
 * Creation date: 2024-01-19
 * Last modified: 2024-01-19
 *
 * Description:  SuperNEMO clusterized calibrated data model
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_CALIBRATED_DATA_H
#define FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_CALIBRATED_DATA_H 1

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
#include <falaise/snemo/datamodels/calibrated_cluster.h>

namespace snemo {

namespace datamodel {

/// \brief SuperNEMO clusterized calibrated data model

class clusterized_calibrated_data : public datatools::i_serializable,
																		public datatools::i_tree_dumpable,
																		public datatools::i_clear {
 public:
  /// Returns the number of clusters
  size_t size() const;

  /// Add a cluster
  void push_back(const calibrated_cluster_handle& cluster);

  /// Return a non mutable reference to a cluster by index
  const calibrated_cluster & at(size_t index) const;

  /// Reset the clusters
  virtual void clear() override;

  /// Return the non mutable reference to the collection of clusters
  const calibrated_cluster_handle_collection& clusters() const;

  /// Return the mutable reference to the collection of clusters
  calibrated_cluster_handle_collection& clusters();

  /// Return a non mutable reference to the default cluster is any
  const calibrated_cluster& get_default() const;

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog, const std::string& title = "",
                         const std::string& indent = "", bool is_last = false) const override;

 private:
  calibrated_cluster_handle_collection _clusters_{};  //!< Collection of clusters

  datatools::properties _auxiliaries_{};  // unused, kept for backward serialization compatibility

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::clusterized_calibrated_data,
                        "snemo::datamodel::clusterized_calibrated_data")

#endif  // FALAISE_SNEMO_DATAMODELS_CLUSTERIZED_CALIBRATED_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
