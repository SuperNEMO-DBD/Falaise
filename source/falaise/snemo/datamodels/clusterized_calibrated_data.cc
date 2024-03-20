/// \file falaise/snemo/datamodels/clusterized_calibrated_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/clusterized_calibrated_data.h>

namespace snemo {

  namespace datamodel {

    size_t clusterized_calibrated_data::size() const {
      return _clusters_.size();
    }

    void clusterized_calibrated_data::push_back(const calibrated_cluster_handle& cluster) {
      DT_THROW_IF(!cluster, std::logic_error, "Cannot store a null handle !");
      for (const auto& addr : _clusters_) {
	DT_THROW_IF(&(*addr) == &(*cluster), std::logic_error, "Duplicated clusters is not allowed!");
      }
      _clusters_.push_back(cluster);
    }

    const calibrated_cluster& clusterized_calibrated_data::at(size_t index) const {
      return *(_clusters_.at(index));
    }

    void clusterized_calibrated_data::clear() {
      _clusters_.clear();
      _auxiliaries_.clear();
    }

    const calibrated_cluster_handle_collection& clusterized_calibrated_data::clusters() const {
      return _clusters_;
    }

    calibrated_cluster_handle_collection& clusterized_calibrated_data::clusters() {
      return _clusters_;
    }

    void clusterized_calibrated_data::tree_dump(std::ostream& out, const std::string& title,
						const std::string& indent, bool /*inherit_*/) const {

      if (!title.empty()) {
	out << indent << title << std::endl;
      }

      out << indent << datatools::i_tree_dumpable::tag << "Clusters[" << _clusters_.size() << "]: "
	  << std::endl;

      for (size_t i = 0; i < size(); i++) {
	std::ostringstream indent2;
	out << indent << datatools::i_tree_dumpable::skip_tag;
	indent2 << indent << datatools::i_tree_dumpable::skip_tag;
	if (i == size() - 1) {
	  out << datatools::i_tree_dumpable::last_tag;
	  indent2 << datatools::i_tree_dumpable::last_skip_tag;
	} else {
	  out << datatools::i_tree_dumpable::tag;
	  indent2 << datatools::i_tree_dumpable::skip_tag;
	}
	out << "Cluster #" << i << " : " << std::endl;
	at(i).tree_dump(out, "", indent2.str());
      }

      // out << indent << datatools::i_tree_dumpable::tag
      //     << "Default cluster : " << (default_ ? "Yes" : "No") << std::endl;
    }

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(clusterized_calibrated_data,
						      "snemo::datamodel::clusterized_calibrated_data")

  }  // end of namespace datamodel

}  // end of namespace snemo
