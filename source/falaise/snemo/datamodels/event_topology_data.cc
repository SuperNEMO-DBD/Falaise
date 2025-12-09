// event_topology_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/event_topology_data.h>

namespace snemo {

  namespace datamodel {

    std::size_t event_topology_data::number_of_topologies() const
    {
      return _topologies_.size();
    }
			
    void event_topology_data::add_topology(const EventTopologyHdl & event_topology_hdl_)
    {
      _topologies_.push_back(event_topology_hdl_);
      return;
    }

    const EventTopologyHdl & event_topology_data::get_topology(const int index_)
    {
      return _topologies_[index_];
    }

    // override
    void event_topology_data::clear()
    {
      _topologies_.clear();
      return;
    }

    // override
    void event_topology_data::print_tree(std::ostream & out_,
					 const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_topologies_opt = options_.get("list_topologies", true);
      const std::string & indent = popts.indent;
      if (!popts.title.empty()) {
        out_ << indent << popts.title << std::endl;
      }

      out_ << indent << tag << "Topologies : " << _topologies_.size() << std::endl;
      if (list_topologies_opt) {
	auto count = 0u;
	for (auto i = 0u; i < _topologies_.size(); i++) {
	  std::string tag = tags::item();
	  std::string skip_tag = tags::skip_item();
	  if (i + 1 == _topologies_.size()) {
	    tag = tags::last_item();
	    skip_tag = tags::last_skip_item();
	  }
	  out_ << indent << tags::skip_item() << tag << "Topology #" << i << " : " << std::endl;
	  const auto & topoHdl = _topologies_[i];
	  {
	    boost::property_tree::ptree poptions;
	    poptions.put("indent", indent + tags::skip_item() + skip_tag);
	    topoHdl->print_tree(out_, poptions);
	  }
	  count++;
	}
      }
      
      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(event_topology_data,
                                                      "snemo::datamodel::event_topology_data")

  } // namespace datamodel

} // end of namespace snemo
