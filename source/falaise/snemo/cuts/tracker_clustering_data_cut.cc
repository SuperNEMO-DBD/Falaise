// -*- mode: c++ ; -*-
/** \file falaise/snemo/cuts/tracker_clustering_data_cut.cc
 */

// Ourselves
#include <falaise/snemo/cuts/tracker_clustering_data_cut.h>

/// Standard library
#include <stdexcept>

// Third party
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>
// SuperNEMO data models :
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(tracker_clustering_data_cut, "snemo::cut::tracker_clustering_data_cut");

    void tracker_clustering_data_cut::_set_defaults ()
    {
      _TCD_label_         = "";
      _mode_              = MODE_UNDEFINED;
      _flag_name_         = "";
      _cluster_range_min_ = -1;
      _cluster_range_max_ = -1;
      return;
    }

    void tracker_clustering_data_cut::set_TCD_label (const std::string & TCD_label_)
    {
      _TCD_label_ = TCD_label_;
      return;
    }

    const std::string & tracker_clustering_data_cut::get_TCD_label () const
    {
      return _TCD_label_;
    }

    uint32_t tracker_clustering_data_cut::get_mode () const
    {
      return _mode_;
    }

    bool tracker_clustering_data_cut::is_mode_flag () const
    {
      return _mode_ & MODE_FLAG;
    }

    bool tracker_clustering_data_cut::is_mode_has_cluster () const
    {
      return _mode_ & MODE_HAS_CLUSTER;
    }

    bool tracker_clustering_data_cut::is_mode_range_cluster () const
    {
      return _mode_ & MODE_RANGE_CLUSTER;
    }

    void tracker_clustering_data_cut::set_flag_name (const std::string & flag_name_)
    {
      _flag_name_ = flag_name_;
      return;
    }

    const std::string & tracker_clustering_data_cut::get_flag_name () const
    {
      return _flag_name_;
    }

    tracker_clustering_data_cut::tracker_clustering_data_cut (datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults ();
      return;
    }

    tracker_clustering_data_cut::~tracker_clustering_data_cut ()
    {
      if (is_initialized()) this->tracker_clustering_data_cut::reset();
      return;
    }

    void tracker_clustering_data_cut::reset ()
    {
      _set_defaults ();
      this->i_cut::_reset ();
      this->i_cut::_set_initialized (false);
      return;
    }

    void tracker_clustering_data_cut::initialize (const datatools::properties & configuration_,
                                                  datatools::service_manager  & /* service_manager_ */,
                                                  cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cut '" << get_name () << "' is already initialized ! ");

      this->i_cut::_common_initialize (configuration_);

      if (_TCD_label_.empty ())
        {
          DT_THROW_IF (! configuration_.has_key ("TCD_label"), std::logic_error,
                       "Missing 'TCD_label' property !");
          set_TCD_label (configuration_.fetch_string ("TCD_label"));
        }

      if (_mode_ == MODE_UNDEFINED)
        {
          if (configuration_.has_flag ("mode.flag"))
            {
              _mode_ |= MODE_FLAG;
            }

          if (configuration_.has_flag ("mode.has_cluster"))
            {
              _mode_ |= MODE_HAS_CLUSTER;
            }

          if (configuration_.has_flag ("mode.range_cluster"))
            {
              _mode_ |= MODE_RANGE_CLUSTER;
            }

          DT_THROW_IF (_mode_ == MODE_UNDEFINED, std::logic_error,
                       "Missing at least a 'mode.XXX' property ! ");

          // mode FLAG:
          if (is_mode_flag ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using FLAG mode...");
              DT_THROW_IF (! configuration_.has_key ("flag.name"),
                           std::logic_error,
                           "Missing 'flag.name' property !");
              set_flag_name (configuration_.fetch_string ("flag.name"));
            } // end if is_mode_flag

          // mode HAS_CLUSTER:
          if (is_mode_has_cluster ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using HAS_CLUSTER mode...");
            } // end if is_mode_has_cluster

          // mode RANGE_CLUSTER:
          if (is_mode_range_cluster ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using RANGE_CLUSTER mode...");
              int count = 0;
              if (configuration_.has_key ("range_cluster.min"))
                {
                  int nmin = configuration_.fetch_integer ("range_cluster.min");
                  DT_THROW_IF (nmin < 0, std::range_error,
                               "Invalid min number of clusters (" << nmin << ") !");
                  _cluster_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key ("range_cluster.max"))
                {
                  int nmax = configuration_.fetch_integer ("range_cluster.max");
                  DT_THROW_IF (nmax < 0, std::range_error,
                               "Invalid max number of clusters (" << nmax << ") !");
                  _cluster_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF (count == 0, std::logic_error,
                           "Missing 'range_cluster.min' or 'range_cluster.max' property !");
              if (count == 2 && _cluster_range_min_ >= 0 && _cluster_range_max_ >= 0)
                {
                  DT_THROW_IF (_cluster_range_min_ > _cluster_range_max_, std::logic_error,
                               "Invalid 'range_cluster.min' > 'range_cluster.max' values !");
                }
            } // end if is_mode_range_cluster
        }

      this->i_cut::_set_initialized (true);
      return;
    }


    int tracker_clustering_data_cut::_accept ()
    {
      int cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has (_TCD_label_))
        {
          DT_LOG_DEBUG (get_logging_priority (), "Event record has no '" << _TCD_label_ << "' bank !");
          return cut_returned;
        }

      // Get tracker clustering data bank
      const snemo::datamodel::tracker_clustering_data & TCD
        = ER.get<snemo::datamodel::tracker_clustering_data> (_TCD_label_);

      // Check if the tracker clustering data has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running FLAG mode...");
          const bool check = TCD.get_auxiliaries ().has_flag (_flag_name_);
          if (! check)
            {
              check_flag = false;
            }
        }

      // Check if the tracker clustering data has clusters :
      bool check_has_cluster = true;
      if (is_mode_has_cluster ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running HAS_CLUSTER mode...");
          // 2012-05-13 XG: Here we only take care of the default solution
          if (!TCD.has_default_solution ()) check_has_cluster = false;
          else check_has_cluster = ! TCD.get_default_solution ().get_clusters ().empty ();
        }

      // Check if the tracker clustering data has a range of clusters :
      bool check_range_cluster = true;
      if (is_mode_range_cluster ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running RANGE_CLUSTER mode...");
          // 2012-05-13 XG: Here we only take care of the default solution
          bool check_has_cluster = true;
          if (!TCD.has_default_solution ()) check_has_cluster = false;
          else check_has_cluster = ! TCD.get_default_solution ().get_clusters ().empty ();
          if (!check_has_cluster)
            {
              DT_LOG_DEBUG (get_logging_priority (), "Tracker clustering data has no clusters");
              return cuts::SELECTION_INAPPLICABLE;
            }

          const size_t nclusters
            = TCD.get_default_solution ().get_clusters ().size ();
          DT_LOG_DEBUG (get_logging_priority (), "Number of isolated cluster= " << nclusters << " "
                        << "cluster_min= " << _cluster_range_min_ << " "
                        << "cluster_max= " << _cluster_range_max_);

          if (_cluster_range_min_ >= 0)
            {
              if (nclusters < (size_t)_cluster_range_min_)
                {
                  check_range_cluster = false;
                }
            }
          if (_cluster_range_max_ >= 0)
            {
              if (nclusters > (size_t)_cluster_range_max_)
                {
                  check_range_cluster = false;
                }
            }
        }

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag        &&
          check_has_cluster &&
          check_range_cluster)
        {
          cut_returned = cuts::SELECTION_ACCEPTED;
        }

      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

// end of tracker_clustering_data_cut.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
