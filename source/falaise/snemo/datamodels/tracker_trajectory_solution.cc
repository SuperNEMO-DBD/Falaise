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
      return _solution_id_ >= 0;
    }

    int tracker_trajectory_solution::get_solution_id() const
    {
      return _solution_id_;
    }

    void tracker_trajectory_solution::set_solution_id(int32_t solution_id_)
    {
      if(solution_id_ >= 0) {
        _solution_id_ = solution_id_;
      } else {
        invalidate_solution_id();
      }
      return;
    }

    void tracker_trajectory_solution::invalidate_solution_id()
    {
      _solution_id_ = -1;
      return;
    }

    /*** Reference clustering solution ***/

    tracker_clustering_solution & tracker_trajectory_solution::grab_clustering_solution()
    {
      return _clustering_solution_.grab();
    }

    void tracker_trajectory_solution::set_clustering_solution
    (const tracker_trajectory_solution::handle_clustering_solution_type & clustering_solution_)
    {
      _clustering_solution_ = clustering_solution_;
      return;
    }

    const tracker_clustering_solution & tracker_trajectory_solution::get_clustering_solution() const
    {
      return _clustering_solution_.get();
    }

    bool tracker_trajectory_solution::has_clustering_solution() const
    {
      return _clustering_solution_.has_data();
    }

    void tracker_trajectory_solution::invalidate_clustering_solution()
    {
      _clustering_solution_.reset();
      return;
    }

    tracker_trajectory_solution::cluster_col_type & tracker_trajectory_solution::grab_unfitted_clusters()
    {
      return _unfitted_clusters_;
    }

    const tracker_trajectory_solution::cluster_col_type & tracker_trajectory_solution::get_unfitted_clusters() const
    {
      return _unfitted_clusters_;
    }

    void tracker_trajectory_solution::invalidate_unfitted_clusters()
    {
      _unfitted_clusters_.clear();
      return;
    }

    datatools::properties & tracker_trajectory_solution::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties & tracker_trajectory_solution::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    tracker_trajectory_solution::trajectory_col_type & tracker_trajectory_solution::grab_trajectories()
    {
      return _trajectories_;
    }

    const tracker_trajectory_solution::trajectory_col_type & tracker_trajectory_solution::get_trajectories() const
    {
      return _trajectories_;
    }

    void tracker_trajectory_solution::invalidate_trajectories()
    {
      _trajectories_.clear();
      return;
    }

    void tracker_trajectory_solution::reset()
    {
      this->clear();
      return;
    }

    void tracker_trajectory_solution::clear()
    {
      invalidate_solution_id();
      invalidate_clustering_solution();
      invalidate_trajectories();
      invalidate_unfitted_clusters();
      _auxiliaries_.clear();
      return;
    }

    tracker_trajectory_solution::tracker_trajectory_solution()
    {
      _solution_id_ = -1;
      return;
    }

    tracker_trajectory_solution::~tracker_trajectory_solution()
    {
      this->reset();
      return;
    }

    void tracker_trajectory_solution::tree_dump(std::ostream & out_,
                                                const std::string & title_,
                                                const std::string & indent_,
                                                bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      if( ! title_.empty() ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Solution ID  : " << _solution_id_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Reference clustering solution : ";
      if(has_clustering_solution()) {
        out_ << _clustering_solution_.get().get_solution_id() ;
      } else {
        out_ << "<No>";
      }
      out_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Trajectories : " << _trajectories_.size() << std::endl;
      for(size_t i = 0; i < _trajectories_.size(); i++) {
        const datatools::handle<tracker_trajectory> & htraj = _trajectories_.at(i);
        const tracker_trajectory & traj = htraj.get();
        //DATATOOLS_UTILS_HANDLE_GET_REF(traj, htraj, tracker_trajectory);
        out_ << indent << datatools::i_tree_dumpable::skip_tag;
        std::ostringstream indent2_oss;
        indent2_oss << indent << datatools::i_tree_dumpable::skip_tag;
        size_t j = i;
        j++;
        if(j == _trajectories_.size()) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent2_oss << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent2_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Trajectory #" << i << " : " << std::endl;
        traj.tree_dump(out_, "", indent2_oss.str());
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Unfitted clusters : " << _unfitted_clusters_.size() << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Auxiliaries : ";
      if(_auxiliaries_.size() == 0) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag(inherit_) ;
        _auxiliaries_.tree_dump(out_, "", indent_oss.str());
      }

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
