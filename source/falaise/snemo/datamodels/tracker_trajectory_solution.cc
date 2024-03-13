// falaise/snemo/datamodels/tracker_trajectory_solution.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// This project:
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

  namespace datamodel {

    // serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory_solution,
                                                      "snemo::datamodel::tracker_trajectory_solution")

    bool tracker_trajectory_solution::has_solution_id() const
    {
      return id_ >= 0;
    }

    int tracker_trajectory_solution::get_solution_id() const
    {
      return id_;
    }

    void tracker_trajectory_solution::set_solution_id(int32_t id)
    {
      if (id >= 0) {
        id_ = id;
      } else {
        invalidate_solution_id();
      }
    }

    void tracker_trajectory_solution::invalidate_solution_id()
    {
      id_ = -1;
    }

    /*** Reference clustering solution ***/

    tracker_clustering_solution& tracker_trajectory_solution::grab_clustering_solution()
    {
      return solutions_.grab();
    }

    void tracker_trajectory_solution::set_clustering_solution(const TrackerClusteringSolutionHdl& s)
    {
      solutions_ = s;
    }

    const tracker_clustering_solution& tracker_trajectory_solution::get_clustering_solution() const
    {
      return solutions_.get();
    }

    bool tracker_trajectory_solution::has_clustering_solution() const
    {
      return solutions_.has_data();
    }

    void tracker_trajectory_solution::invalidate_clustering_solution()
    {
      solutions_.reset();
    }

    bool tracker_trajectory_solution::has_unfitted_clusters() const
    {
      return !unfitted_.empty();
    }

    TrackerClusterHdlCollection& tracker_trajectory_solution::grab_unfitted_clusters()
    {
      return unfitted_;
    }

    const TrackerClusterHdlCollection& tracker_trajectory_solution::get_unfitted_clusters() const
    {
      return unfitted_;
    }

    void tracker_trajectory_solution::invalidate_unfitted_clusters()
    {
      unfitted_.clear();
    }

    datatools::properties& tracker_trajectory_solution::get_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties& tracker_trajectory_solution::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    bool tracker_trajectory_solution::has_trajectories() const
    {
      return !trajectories_.empty();
    }

    TrackerTrajectoryHdlCollection& tracker_trajectory_solution::grab_trajectories()
    {
      return trajectories_;
    }

    const TrackerTrajectoryHdlCollection& tracker_trajectory_solution::get_trajectories() const
    {
      return trajectories_;
    }
    
    bool tracker_trajectory_solution::has_best_trajectories() const
    {
      return best_trajectories_.size() > 0;
    }

    uint32_t tracker_trajectory_solution::number_of_best_trajectories() const
    {
       return best_trajectories_.size();
    }
        
    const std::set<int32_t> &
    tracker_trajectory_solution::get_best_trajectories() const
    {
      return best_trajectories_;
    }

    std::set<int32_t> &
    tracker_trajectory_solution::grab_best_trajectories()
    {
      return best_trajectories_;
    }

    void tracker_trajectory_solution::invalidate_trajectories()
    {
      trajectories_.clear();
    }

    void tracker_trajectory_solution::reset()
    {
      this->clear();
    }

    void tracker_trajectory_solution::clear()
    {
      invalidate_solution_id();
      invalidate_clustering_solution();
      invalidate_trajectories();
      invalidate_unfitted_clusters();
      best_trajectories_.clear();
      _auxiliaries_.clear();
    }

    void tracker_trajectory_solution::print_tree(std::ostream & out_,
						 const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      const std::string & indent = popts.indent;

      // std::string item_tag = indent + datatools::i_tree_dumpable::tags::item();
      // std::string mkip_tag = indent + datatools::i_tree_dumpable::tags::skip_item();
      // std::string last_tag = indent + datatools::i_tree_dumpable::tags::last_item();
      // std::string last_skip_tag = indent + datatools::i_tree_dumpable::tags::last_skip_item();

      out_ << indent << tag << "Solution ID  : " << id_ << std::endl;

      out_ << indent << tag << "Reference clustering solution : ";
      if (has_clustering_solution()) {
        out_ << solutions_->get_solution_id();
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent << tag << "Trajectories : " << trajectories_.size()
          << std::endl;
      for (size_t i = 0; i < trajectories_.size(); i++) {
        const datatools::handle<tracker_trajectory>& htraj = trajectories_.at(i);
        const tracker_trajectory & traj = htraj.get();
        out_ << indent << skip_tag;
        std::ostringstream indent2_oss;
        indent2_oss << indent << skip_tag;
        size_t j = i;
        j++;
        if (j == trajectories_.size()) {
          out_ << last_tag;
          indent2_oss << last_skip_tag;
        } else {
          out_ << tag;
          indent2_oss << skip_tag;
        }
        out_ << "Trajectory #" << i;
        if (best_trajectories_.count(traj.get_id())) {
          out_ << " (best)";
        }
        out_ << " : " << std::endl;
	{
	  boost::property_tree::ptree trajOpts;
	  trajOpts.put("indent", indent2_oss.str());
	  traj.print_tree(out_, trajOpts);
	}
      }

      out_ << indent << tag
          << "Unfitted clusters : " << unfitted_.size() << std::endl;

      out_ << indent << tag
          << "Best trajectories : " << best_trajectories_.size() << std::endl;

      out_ << indent << inherit_tag(popts.inherit) << "Auxiliary properties : ";
      if (_auxiliaries_.empty()) {
        out_ << "<empty>";
      } else {
	out_ << _auxiliaries_.size();
      }
      out_ << std::endl;
      if (list_properties_opt) {
	boost::property_tree::ptree auxOpts;
        auxOpts.put("indent", popts.indent + tags::item(not popts.inherit, true));
        _auxiliaries_.print_tree(out_, auxOpts);
      }
      
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
