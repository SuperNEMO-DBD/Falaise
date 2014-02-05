// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/tracker_clustering_solution.cc */

// Ourselves
#include <falaise/snemo/datamodels/tracker_clustering_solution.h>

namespace snemo {

  namespace datamodel {

    bool tracker_clustering_solution::has_solution_id() const
    {
      return _solution_id_ >= 0;
    }

    int tracker_clustering_solution::get_solution_id() const
    {
      return _solution_id_;
    }

    void tracker_clustering_solution::set_solution_id(int32_t solution_id_)
    {
      if(solution_id_ >= 0) {
        _solution_id_ = solution_id_;
      } else {
        invalidate_solution_id();
      }
      return;
    }

    void tracker_clustering_solution::invalidate_solution_id()
    {
      _solution_id_ = -1;
      return;
    }

    datatools::properties & tracker_clustering_solution::grab_auxiliaries()
    {
      return _auxiliaries_;
    }

    const datatools::properties & tracker_clustering_solution::get_auxiliaries() const
    {
      return _auxiliaries_;
    }

    tracker_clustering_solution::hit_collection_type &
    tracker_clustering_solution::grab_unclustered_hits()
    {
      return _unclustered_hits_;
    }

    const tracker_clustering_solution::hit_collection_type &
    tracker_clustering_solution::get_unclustered_hits() const
    {
      return _unclustered_hits_;
    }

    tracker_clustering_solution::cluster_col_type &
    tracker_clustering_solution::grab_clusters()
    {
      return _clusters_;
    }

    const tracker_clustering_solution::cluster_col_type &
    tracker_clustering_solution::get_clusters() const
    {
      return _clusters_;
    }

    void tracker_clustering_solution::reset()
    {
      this->clear();
      return;
    }

    void tracker_clustering_solution::clear()
    {
      reset_hit_belonging();
      _clusters_.clear();
      _unclustered_hits_.clear();
      invalidate_solution_id();
      _auxiliaries_.clear();
      return;
    }

    tracker_clustering_solution::tracker_clustering_solution()
    {
      _solution_id_ = -1;
      return;
    }

    tracker_clustering_solution::~tracker_clustering_solution()
    {
      return;
    }

    void tracker_clustering_solution::tree_dump(std::ostream & out_,
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
           << "Clusters     : " << _clusters_.size() << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Unclustered hits : " << _unclustered_hits_.size() << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Hits belonging : " << _hit_belonging_.size() << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Auxiliaries : ";
      if(_auxiliaries_.size() == 0)  {
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

    bool tracker_clustering_solution::hit_belongs_to_cluster(const calibrated_tracker_hit & hit_,
                                                              const tracker_cluster & cluster_) const
    {
      return hit_belongs_to_cluster(hit_.get_hit_id(), cluster_.get_cluster_id());
    }

    bool tracker_clustering_solution::hit_is_clustered(const calibrated_tracker_hit & hit_) const
    {
      return hit_is_clustered(hit_.get_hit_id());
    }

    bool tracker_clustering_solution::hit_is_clustered(int32_t hit_id_) const
    {
      if(_hit_belonging_.size() > 0) {
        hit_belonging_col_type::const_iterator found = _hit_belonging_.find(hit_id_);
        return found != _hit_belonging_.end() &&  found->second.size() > 0;
      }
      for(hit_collection_type::const_iterator ihit = _unclustered_hits_.begin();
           ihit != _unclustered_hits_.end();
           ihit++) {
        if(! ihit->has_data()) continue;
        if(ihit->get().get_hit_id() == hit_id_) return false;
      }
      return true;
    }

    bool tracker_clustering_solution::hit_belongs_to_several_clusters(const calibrated_tracker_hit & hit_) const
    {
      return hit_belongs_to_several_clusters(hit_.get_hit_id());
    }

    bool tracker_clustering_solution::hit_belongs_to_several_clusters(int32_t hit_id_) const
    {
      // If available, use the' hit_belonging' collection :
      if(_hit_belonging_.size() > 0) {
        hit_belonging_col_type::const_iterator found = _hit_belonging_.find(hit_id_);
        return found != _hit_belonging_.end() &&  found->second.size() > 1;
      }
      // Shortcut :
      if(_clusters_.size() < 2) return false;
      // Traverse all clusters to count belongings for this hit :
      int cluster_counter = 0;
      for(cluster_col_type::const_iterator icluster = _clusters_.begin();
           icluster != _clusters_.end();
           icluster++) {
        const cluster_handle_type & the_cluster_handle = *icluster;
        if(! the_cluster_handle.has_data()) continue;
        const tracker_cluster & the_cluster = the_cluster_handle.get();
        const calibrated_tracker_hit::collection_type & the_hits = the_cluster.get_hits();
        for(calibrated_tracker_hit::collection_type::const_iterator ihit =
               the_hits.begin();
             ihit != the_hits.end();
             ihit++) {
          const calibrated_tracker_hit::handle_type & the_hit_handle = *ihit;
          if(! the_hit_handle.has_data()) continue;
          const calibrated_tracker_hit & the_hit = the_hit_handle.get();
          if( the_hit.get_hit_id() == hit_id_) {
            cluster_counter++;
            break;
          }
        }
      }
      return cluster_counter > 1;
    }

    bool tracker_clustering_solution::hit_belongs_to_cluster(int32_t hit_id_, int32_t cluster_id_) const
    {
      // If available, use the' hit_belonging' collection :
      if(_hit_belonging_.size() > 0) {
        hit_belonging_col_type::const_iterator found_iter = _hit_belonging_.find(hit_id_);
        if(found_iter == _hit_belonging_.end()) {
          return false;
        }
        const cluster_col_type & hosting_clusters = found_iter->second;
        for(cluster_col_type::const_iterator icluster
               = hosting_clusters.begin();
             icluster != hosting_clusters.end();
             icluster++) {
          const cluster_handle_type & the_cluster_handle = *icluster;
          if(! the_cluster_handle.has_data()) {
            continue;
          }
          const tracker_cluster & the_cluster = the_cluster_handle.get();
          if(the_cluster.get_cluster_id() == cluster_id_) {
            return true;
          }
        }
      } else {
        // Shortcut :
        if(_clusters_.size() < 1) return false;
        // Traverse all clusters to detect belongings for this hit :
        for(cluster_col_type::const_iterator icluster = _clusters_.begin();
             icluster != _clusters_.end();
             icluster++) {
          const cluster_handle_type & the_cluster_handle = *icluster;
          if(! the_cluster_handle.has_data()) {
            continue;
          }
          const tracker_cluster & the_cluster = the_cluster_handle.get();
          if(the_cluster.get_cluster_id() != cluster_id_) {
            continue;
          }
          const calibrated_tracker_hit::collection_type & the_hits = the_cluster.get_hits();
          // Traverse all hits in the current cluster :
          for(calibrated_tracker_hit::collection_type::const_iterator ihit =
                 the_hits.begin();
               ihit != the_hits.end();
               ihit++)  {
            const calibrated_tracker_hit::handle_type & the_hit_handle = *ihit;
            if(! the_hit_handle.has_data()) {
              continue;
            }
            const calibrated_tracker_hit & the_hit = the_hit_handle.get();
            if( the_hit.get_hit_id() == hit_id_) {
              return true;
            }
          }
        }
      }
      return false;
    }

    const tracker_clustering_solution::hit_belonging_col_type &
    tracker_clustering_solution::get_hit_belonging() const
    {
      return _hit_belonging_;
    }

    void tracker_clustering_solution::reset_hit_belonging()
    {
      _hit_belonging_.clear();
      return;
    }

    bool tracker_clustering_solution::has_hit_belonging() const
    {
      return _hit_belonging_.size() > 0;
    }

    // static
    void tracker_clustering_solution::compute_hit_belonging_from_solution(const tracker_clustering_solution & tcs_,
                                                                           hit_belonging_col_type &hbc_)
    {
      hbc_.clear();
      for(cluster_col_type::const_iterator icluster
             = tcs_._clusters_.begin();
           icluster != tcs_._clusters_.end();
           icluster++) {
        const cluster_handle_type & the_cluster_handle = *icluster;
        if(! the_cluster_handle.has_data()) continue;
        const tracker_cluster & the_cluster = the_cluster_handle.get();
        const calibrated_tracker_hit::collection_type & the_hits = the_cluster.get_hits();
        for(calibrated_tracker_hit::collection_type::const_iterator ihit =
               the_hits.begin();
             ihit != the_hits.end();
             ihit++) {
          const calibrated_tracker_hit::handle_type & the_hit_handle = *ihit;
          if(! the_hit_handle.has_data()) continue;
          const calibrated_tracker_hit & the_hit = the_hit_handle.get();
          int32_t hit_id = the_hit.get_hit_id();
          if(hbc_.find(hit_id) == hbc_.end()) {
            {
              cluster_col_type dummy_col;
              hbc_[hit_id] = dummy_col;
            }
            hbc_[hit_id].push_back(the_cluster_handle);
          }
        }

      }
      return;
    }

    void tracker_clustering_solution::compute_hit_belonging()
    {
      compute_hit_belonging_from_solution(*this, _hit_belonging_);
      return;
    }

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_solution,
                                                      "snemo::datamodel::tracker_clustering_solution")

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/tracker_clustering_solution.cc
