/// \file falaise/snemo/datamodels/tracker_clustering_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

  namespace datamodel {

    bool tracker_clustering_data::empty() const { return solutions_.empty(); }

    size_t tracker_clustering_data::size() const { return solutions_.size(); }

    const tracker_clustering_solution & tracker_clustering_data::at(size_t index) const
    {
      return *(solutions_.at(index));
    }

    void tracker_clustering_data::append_solution(const TrackerClusteringSolutionHdl& solution,
						  bool isDefault)
    {
      DT_THROW_IF(!solution, std::logic_error, "Cannot store a null handle !");
      for (const auto& addr : solutions_) {
	DT_THROW_IF(&(*addr) == &(*solution), std::logic_error, "Duplicated solutions is not allowed!");
      }
      solutions_.push_back(solution);
      if (isDefault || solutions_.size() == 1) {
	default_ = solution;
      }
    }

    bool tracker_clustering_data::has_default() const
    {
      return default_;
    }

    tracker_clustering_solution& tracker_clustering_data::get_default()
    {
      DT_THROW_IF(!has_default(), std::logic_error, "No default solution is available !");
      return *default_;
    }

    const tracker_clustering_solution & tracker_clustering_data::get_default() const
    {
      DT_THROW_IF(!has_default(), std::logic_error, "No default solution is available !");
      return *default_;
    }

    void tracker_clustering_data::set_default(size_t index_)
    {
      default_ = solutions_.at(index_);
    }

    TrackerClusteringSolutionHdlCollection & tracker_clustering_data::solutions()
    {
      return solutions_;
    }

    const TrackerClusteringSolutionHdlCollection & tracker_clustering_data::solutions() const
    {
      return solutions_;
    }

    void tracker_clustering_data::clear()
    {
      solutions_.clear();
      default_ = TrackerClusteringSolutionHdl{};
      _auxiliaries_.clear();
      return;
    }

    const datatools::properties & tracker_clustering_data::auxiliaries() const
    {
      return _auxiliaries_;
    }

    datatools::properties & tracker_clustering_data::auxiliaries()
    {
      return _auxiliaries_;
    }

    void tracker_clustering_data::print_tree(std::ostream & out_,
					     const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_solutions_opt = options_.get("list_solutions", false);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
	out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Solutions : " << solutions_.size() << std::endl;
      if (list_solutions_opt) {
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
	  out_ << "Solution #" << i << " : " << std::endl;
	  at(i).tree_dump(out_, "", indent2.str());
	}
      }

      out_ << popts.indent << tag
	   << "Default solution : " << (default_ ? "yes" : "no") << std::endl;
      
      out_ << popts.indent << inherit_tag(popts.inherit) << "Auxiliary properties : ";
      out_ << _auxiliaries_.size();
      out_ << std::endl;
      if (list_properties_opt) {
        boost::property_tree::ptree auxOpts;
        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << inherit_skip_tag(popts.inherit);
        auxOpts.put("indent", indent_oss.str());
        _auxiliaries_.print_tree(out_, auxOpts);
      }

      return;
    }

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_data,
						      "snemo::datamodel::tracker_clustering_data")

  } // end of namespace datamodel

} // end of namespace snemo
