/// \file falaise/snemo/datamodels/tracker_cluster.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_cluster.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_cluster,
                                                      "snemo::datamodel::tracker_cluster")

    bool tracker_cluster::is_delayed() const
    {
      return delayed_;
     }

    bool tracker_cluster::is_prompt() const
    {
      return !is_delayed();
    }

    void tracker_cluster::make_delayed()
    {
      delayed_ = true;
      return;
    }

    void tracker_cluster::make_prompt()
    {
      delayed_ = false;
      return;
    }

    int tracker_cluster::get_cluster_id() const
    {
      return get_hit_id();
    }

    void tracker_cluster::set_cluster_id(int32_t id)
    {
      set_hit_id(id);
    }

    TrackerHitHdlCollection & tracker_cluster::hits()
    {
      return hits_;
    }

    const TrackerHitHdlCollection & tracker_cluster::hits() const
    {
      return hits_;
    }

    void tracker_cluster::clear()
    {
      hits_.clear();
      base_hit::clear();
      return;
    }

    size_t tracker_cluster::size() const
    {
      return hits_.size();
    }

    const calibrated_tracker_hit & tracker_cluster::at(size_t index) const
    {
      return *(hits_.at(index));
    }

    void tracker_cluster::print_tree(std::ostream & out,
                                     const boost::property_tree::ptree & options) const
    {
      base_hit::print_tree(out, base_print_options::force_inheritance(options));
      base_print_options popts;
      popts.configure_from(options);
      const std::string & indent = popts.indent;
      out << indent << tag << "Hits[" << hits_.size() << "]:" << std::endl;
      for (size_t i = 0; i < hits_.size(); ++i) {
        out << indent << skip_tag;
        if (i + 1 == hits_.size()) {
          out << last_tag;
        } else {
          out << tag;
        }
        out << "Hit[" << i << "] : (Id : " << hits_[i]->get_hit_id()
            << ", GID : " << hits_[i]->get_geom_id() << ")" << std::endl;
      }
      out << indent << tag << "Delayed : " << std::boolalpha << delayed_ << std::endl;
      out << indent << inherit_tag(popts.inherit)
          << "Cluster ID  : " << get_cluster_id() << std::endl;
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
