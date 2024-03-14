// -*- mode: c++ ; -*-
// falaise/snemo/datamodels/tracker_trajectory_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

namespace snemo {

  namespace datamodel {

    bool tracker_trajectory_data::has_solutions() const { return get_number_of_solutions() > 0; }

    size_t tracker_trajectory_data::get_number_of_solutions() const { return solutions_.size(); }

    const tracker_trajectory_solution& tracker_trajectory_data::get_solution(size_t index) const {
      return solutions_.at(index).get();
    }

    void tracker_trajectory_data::add_solution(const TrackerTrajectorySolutionHdl& solution,
					       bool is_default) {
      DT_THROW_IF(!solution, std::logic_error, "Cannot store a null handle !");

      for (const auto& addr : solutions_) {
	DT_THROW_IF(&*addr == &*solution, std::logic_error, "Duplicated solutions are not allowed!");
      }
      solutions_.push_back(solution);
      if (is_default || solutions_.size() == 1) {
	default_ = solution;
      }
    }

    void tracker_trajectory_data::invalidate_solutions() { solutions_.clear(); }

    bool tracker_trajectory_data::has_default_solution() const { return default_.has_data(); }

    const tracker_trajectory_solution& tracker_trajectory_data::get_default_solution() const {
      DT_THROW_IF(!has_default_solution(), std::logic_error, "No default solution available");
      return *default_;
    }

    /// Set the default trajectory solution
    void tracker_trajectory_data::set_default_solution(size_t index) {
      default_ = solutions_.at(index);
    }

    void tracker_trajectory_data::invalidate_default_solution() { default_.reset(); }

    TrackerTrajectorySolutionHdlCollection& tracker_trajectory_data::get_solutions() {
      return solutions_;
    }

    const TrackerTrajectorySolutionHdlCollection& tracker_trajectory_data::get_solutions() const {
      return solutions_;
    }

    void tracker_trajectory_data::reset() { this->clear(); }

    void tracker_trajectory_data::clear() {
      invalidate_solutions();
      invalidate_default_solution();
      _auxiliaries_.clear();
    }

    void tracker_trajectory_data::print_tree(std::ostream & out_,
					     const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_solutions_opt = options_.get("list_solutions", false);
      if (not popts.title.empty()) {
	out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag
	   << "Solutions : " << solutions_.size()
	   << std::endl;
      if (list_solutions_opt and solutions_.size()) {
	auto solCounter = 0u;
	for (const auto & sol : solutions_) {
	  out_ << popts.indent << skip_tag;
	  std::ostringstream indent_oss;
	  indent_oss << popts.indent << skip_tag;  
	  if ((solCounter + 1) == solutions_.size()) {
	    out_ << last_tag;
	    indent_oss << last_skip_tag;
	  } else {
	    out_ << tag;
	    indent_oss << skip_tag;
	  }
	  out_ << "Solution #" << solCounter << '\n';
	  {
	    boost::property_tree::ptree solOpts;
	    solOpts.put("indent", indent_oss.str()); 
	    sol->print_tree(out_, solOpts);
	  }
	  solCounter++;
	}
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
	   << "Default solution : " << (default_ ? "" : "none") << std::endl;
      if (default_) {
	std::ostringstream indent_oss;
	indent_oss << popts.indent;
        {
	  boost::property_tree::ptree solOpts;
	  solOpts.put("indent", popts.indent + tags::item(not popts.inherit, true)); 
	  default_->print_tree(out_, solOpts);
	}
      }
      
      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory_data,
						      "snemo::datamodel::tracker_trajectory_data")

  } // end of namespace datamodel

} // end of namespace snemo
