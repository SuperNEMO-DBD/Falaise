/// \file falaise/snemo/datamodels/clusterized_precalibrated_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/clusterized_precalibrated_data.h>

namespace snemo {

  namespace datamodel {

    size_t clusterized_precalibrated_data::size() const
    {
      return _clusters_.size();
    }

    void clusterized_precalibrated_data::push_back(const precalibrated_cluster_handle & cluster_)
    {
      DT_THROW_IF(not cluster_, std::logic_error, "Cannot store a null handle !");
      for (const auto & addr : _clusters_) {
        DT_THROW_IF(&(*addr) == &(*cluster_), std::logic_error, "Duplicated clusters is not allowed!");
      }
      _clusters_.push_back(cluster_);
      return;
    }

    const precalibrated_cluster & clusterized_precalibrated_data::at(size_t index) const
    {
      return *(_clusters_.at(index));
    }

    void clusterized_precalibrated_data::clear()
    {
      _clusters_.clear();
      _unclusterized_calorimeter_hits_.clear();
      _unclusterized_tracker_hits_.clear();
      _auxiliaries_.clear();
    }

    const precalibrated_cluster_handle_collection & clusterized_precalibrated_data::clusters() const
    {
      return _clusters_;
    }

    precalibrated_cluster_handle_collection & clusterized_precalibrated_data::clusters()
    {
      return _clusters_;
    }

    const precalibrated_calorimeter_hit_handle_collection & clusterized_precalibrated_data::unclusterized_calorimeter_hits() const
    {
      return _unclusterized_calorimeter_hits_;
    }

    precalibrated_calorimeter_hit_handle_collection & clusterized_precalibrated_data::unclusterized_calorimeter_hits()
    {
      return _unclusterized_calorimeter_hits_;
    }

    const precalibrated_tracker_hit_handle_collection & clusterized_precalibrated_data::unclusterized_tracker_hits() const
    {
      return _unclusterized_tracker_hits_;
    }

    precalibrated_tracker_hit_handle_collection & clusterized_precalibrated_data::unclusterized_tracker_hits()
    {
      return _unclusterized_tracker_hits_;
    }

    void clusterized_precalibrated_data::print_tree(std::ostream & out_,
                                                    const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_clusters_opt = options_.get("list_clusters", false);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Clusters : " << _clusters_.size()
           << std::endl;

      for (size_t i = 0; i < size(); i++) {
	std::ostringstream indent2;
	out_ << popts.indent << skip_tag;
	indent2 << popts.indent << skip_tag;
	if (i == size() - 1) {
	  out_ << last_tag;
	  indent2 << last_skip_tag;
	} else {
	  out_ << tag;
	  indent2 << skip_tag;
	}

	if (list_clusters_opt) {
	  out_ << "Cluster #" << i << " : " << std::endl;
          {
            boost::property_tree::ptree clOpts;
            clOpts.put("indent", indent2.str());
            at(i).print_tree(out_, clOpts);
          }
        }
	else {
	  const precalibrated_cluster & pcd_cluster = at(i);

	  out_ << "Cluster #" << i << " : "
	       << pcd_cluster.calorimeter_hits().size() << " calo hit(s) and "
	       << pcd_cluster.tracker_hits().size() << " tracker hit(s)" << std::endl;
	}
      }
     
      out_ << popts.indent << tag << "Unclusterized : "
	   << _unclusterized_calorimeter_hits_.size() << " calo hit(s) and "
	   << _unclusterized_tracker_hits_.size() << " tracker hit(s)" << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Auxiliary properties : " << _auxiliaries_.size() << std::endl;

      if (list_properties_opt) {
        boost::property_tree::ptree auxOpts;
        auxOpts.put("indent", popts.indent + tags::item(not popts.inherit, true)); 
        _auxiliaries_.print_tree(out_, auxOpts);
      }
      
      return;
    }

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(clusterized_precalibrated_data,
                                                      "snemo::datamodel::clusterized_precalibrated_data")

  } // end of namespace datamodel

} // end of namespace snemo
