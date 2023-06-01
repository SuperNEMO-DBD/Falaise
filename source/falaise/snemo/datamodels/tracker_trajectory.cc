// falaise/snemo/datamodels/tracker_trajectory.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_trajectory.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>

namespace snemo {

  namespace datamodel {

    // serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_trajectory,
                                                      "snemo::datamodel::tracker_trajectory")

    bool tracker_trajectory::has_id() const
    {
      return has_hit_id();
    }

    int tracker_trajectory::get_id() const
    {
      return get_hit_id();
    }

    void tracker_trajectory::set_id(int id)
    {
      set_hit_id(id);
    }

    bool tracker_trajectory::has_cluster() const
    {
      return cluster_.has_data();
    }

    void tracker_trajectory::detach_cluster()
    {
      cluster_.reset();
    }

    void tracker_trajectory::set_cluster_handle(const TrackerClusterHdl & cluster)
    {
      cluster_ = cluster;
    }

    tracker_cluster & tracker_trajectory::get_cluster()
    {
      return *cluster_;
    }

    const tracker_cluster & tracker_trajectory::get_cluster() const
    {
      return *cluster_;
    }

    const TrackerClusterHdl & tracker_trajectory::get_cluster_handle() const
    {
      return cluster_;
    }

    bool tracker_trajectory::has_pattern() const
    {
      return pattern_.has_data();
    }

    void tracker_trajectory::set_pattern_handle(const TrajectoryPatternHdl & pattern)
    {
      pattern_ = pattern;
    }

    void tracker_trajectory::detach_pattern()
    {
      pattern_.reset();
    }

    TrajectoryPattern & tracker_trajectory::get_pattern()
    {
      return *pattern_;
    }

    const TrajectoryPattern & tracker_trajectory::get_pattern() const
    {
      return *pattern_;
    }

    track_fit_infos & tracker_trajectory::get_fit_infos()
    {
      return fit_infos_;
    }

    const track_fit_infos & tracker_trajectory::get_fit_infos() const
    {
      return fit_infos_;
    }

    void tracker_trajectory::clear()
    {
      detach_pattern();
      orphans_.clear();
      detach_cluster();
      fit_infos_.reset();
      base_hit::clear();
      return;
    }

    void tracker_trajectory::print_tree(std::ostream & out,
                                        const boost::property_tree::ptree & options) const
    {
      base_hit::print_tree(out, base_print_options::force_inheritance(options));
      datatools::i_tree_dumpable::base_print_options popts;
      popts.configure_from(options);
      const std::string & indent = popts.indent;
      out << indent << tag << "Cluster : "
          << (has_cluster() ? std::to_string(cluster_->get_cluster_id()) : "<none>")
          << std::endl;
      out << indent << tag << "Pattern : "
          << (has_pattern() ? ("'" + pattern_->get_pattern_id() + "'") : "<none>")
          << std::endl;
      if (has_pattern()) {
        pattern_->tree_dump(out, "", indent + tags::skip_item());
      }
      out << indent << inherit_tag(popts.inherit)
          << "Fit infos : " << std::endl;
      fit_infos_.tree_dump(out, "", indent + tags::item(not popts.inherit, true));
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
