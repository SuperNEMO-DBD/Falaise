// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/tracker_cluster.cc */

// Ourselves
#include <falaise/snemo/datamodels/tracker_cluster.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    //DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_cluster, "snemo::core::model::tracker_cluster")
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_cluster,
                                                      "snemo::datamodel::tracker_cluster")

    // static
    const std::string & tracker_cluster::delayed_cluster_flag()
    {
      static const std::string _flag("delayed");
      return _flag;
    }

    bool tracker_cluster::is_delayed() const
    {
      return get_auxiliaries().has_flag(tracker_cluster::delayed_cluster_flag());
    }

    bool tracker_cluster::is_prompt() const
    {
      return ! is_delayed();
    }

    void tracker_cluster::make_delayed()
    {
      grab_auxiliaries().update_flag(tracker_cluster::delayed_cluster_flag());
      return;
    }

    void tracker_cluster::make_prompt()
    {
      grab_auxiliaries().unset_flag(tracker_cluster::delayed_cluster_flag());
      return;
    }

    bool tracker_cluster::has_cluster_id() const
    {
      return has_hit_id();
    }

    int tracker_cluster::get_cluster_id() const
    {
      return get_hit_id();
    }

    void tracker_cluster::set_cluster_id(int32_t cluster_id_)
    {
      set_hit_id(cluster_id_);
      return;
    }

    void tracker_cluster::invalidate_cluster_id()
    {
      invalidate_hit_id();
      return;
    }

    calibrated_tracker_hit::collection_type & tracker_cluster::grab_hits()
    {
      return _hits_;
    }

    const calibrated_tracker_hit::collection_type & tracker_cluster::get_hits() const
    {
      return _hits_;
    }

    void tracker_cluster::reset()
    {
      this->clear();
      return;
    }

    void tracker_cluster::clear()
    {
      _hits_.clear();
      base_hit::clear();
      return;
    }

    tracker_cluster::tracker_cluster()
    {
      return;
    }

    tracker_cluster::~tracker_cluster()
    {
      this->reset();
      return;
    }

    void tracker_cluster::tree_dump(std::ostream      & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
    {
      std::string indent;
      if(! indent_.empty()) {
        indent = indent_;
      }
      base_hit::tree_dump(out_, title_, indent, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Hits        : " << _hits_.size() << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Cluster ID  : " << get_cluster_id() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/tracker_cluster.cc
