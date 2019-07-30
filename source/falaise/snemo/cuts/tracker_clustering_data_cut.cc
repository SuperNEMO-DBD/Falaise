// falaise/snemo/cuts/tracker_clustering_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/tracker_clustering_data_cut.h>

/// Standard library:
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>
// SuperNEMO data models :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

namespace cut {

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(tracker_clustering_data_cut, "snemo::cut::tracker_clustering_data_cut")

void tracker_clustering_data_cut::_set_defaults() {
  _TCD_label_ = "";
  _mode_ = MODE_UNDEFINED;
  _flag_name_ = "";
  _cluster_range_min_ = -1;
  _cluster_range_max_ = -1;
  _unclustered_range_min_ = -1;
  _unclustered_range_max_ = -1;
}

void tracker_clustering_data_cut::set_TCD_label(const std::string& TCD_label_) {
  _TCD_label_ = TCD_label_;
}

const std::string& tracker_clustering_data_cut::get_TCD_label() const { return _TCD_label_; }

uint32_t tracker_clustering_data_cut::get_mode() const { return _mode_; }

bool tracker_clustering_data_cut::is_mode_flag() const { return (_mode_ & MODE_FLAG) != 0u; }

bool tracker_clustering_data_cut::is_mode_has_cluster() const {
  return (_mode_ & MODE_HAS_CLUSTER) != 0u;
}

bool tracker_clustering_data_cut::is_mode_range_cluster() const {
  return (_mode_ & MODE_RANGE_CLUSTER) != 0u;
}

bool tracker_clustering_data_cut::is_mode_has_unclustered_hits() const {
  return (_mode_ & MODE_HAS_UNCLUSTERED_HITS) != 0u;
}

bool tracker_clustering_data_cut::is_mode_range_unclustered_hits() const {
  return (_mode_ & MODE_RANGE_UNCLUSTERED_HITS) != 0u;
}

void tracker_clustering_data_cut::set_flag_name(const std::string& flag_name_) {
  _flag_name_ = flag_name_;
}

const std::string& tracker_clustering_data_cut::get_flag_name() const { return _flag_name_; }

tracker_clustering_data_cut::tracker_clustering_data_cut(
    datatools::logger::priority logger_priority_)
    : cuts::i_cut(logger_priority_) {
  _set_defaults();
}

tracker_clustering_data_cut::~tracker_clustering_data_cut() {
  if (is_initialized()) {
    this->tracker_clustering_data_cut::reset();
  }
}

void tracker_clustering_data_cut::reset() {
  _set_defaults();
  this->i_cut::_reset();
  this->i_cut::_set_initialized(false);
}

void tracker_clustering_data_cut::initialize(const datatools::properties& configuration_,
                                             datatools::service_manager& /* service_manager_ */,
                                             cuts::cut_handle_dict_type& /* cut_dict_ */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cut '" << get_name() << "' is already initialized ! ");

  this->i_cut::_common_initialize(configuration_);

  if (_TCD_label_.empty()) {
    if (configuration_.has_key("TCD_label")) {
      set_TCD_label(configuration_.fetch_string("TCD_label"));
    } else {
      set_TCD_label(snemo::datamodel::data_info::default_tracker_clustering_data_label());
    }
  }

  if (_mode_ == MODE_UNDEFINED) {
    if (configuration_.has_flag("mode.flag")) {
      _mode_ |= MODE_FLAG;
    }

    if (configuration_.has_flag("mode.has_cluster")) {
      _mode_ |= MODE_HAS_CLUSTER;
    }

    if (configuration_.has_flag("mode.range_cluster")) {
      _mode_ |= MODE_RANGE_CLUSTER;
    }

    if (configuration_.has_flag("mode.has_unclustered_hits")) {
      _mode_ |= MODE_HAS_UNCLUSTERED_HITS;
    }

    if (configuration_.has_flag("mode.range_unclustered_hits")) {
      _mode_ |= MODE_RANGE_UNCLUSTERED_HITS;
    }

    DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error,
                "Missing at least a 'mode.XXX' property ! ");

    // mode FLAG:
    if (is_mode_flag()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using FLAG mode...");
      DT_THROW_IF(!configuration_.has_key("flag.name"), std::logic_error,
                  "Missing 'flag.name' property !");
      set_flag_name(configuration_.fetch_string("flag.name"));
    }  // end if is_mode_flag

    // mode HAS_CLUSTER:
    if (is_mode_has_cluster()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using HAS_CLUSTER mode...");
    }  // end if is_mode_has_cluster

    // mode RANGE_CLUSTER:
    if (is_mode_range_cluster()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_CLUSTER mode...");
      int count = 0;
      if (configuration_.has_key("range_cluster.min")) {
        int nmin = configuration_.fetch_integer("range_cluster.min");
        DT_THROW_IF(nmin < 0, std::range_error,
                    "Invalid min number of clusters (" << nmin << ") !");
        _cluster_range_min_ = nmin;
        count++;
      }
      if (configuration_.has_key("range_cluster.max")) {
        int nmax = configuration_.fetch_integer("range_cluster.max");
        DT_THROW_IF(nmax < 0, std::range_error,
                    "Invalid max number of clusters (" << nmax << ") !");
        _cluster_range_max_ = nmax;
        count++;
      }
      DT_THROW_IF(count == 0, std::logic_error,
                  "Missing 'range_cluster.min' or 'range_cluster.max' property !");
      if (count == 2 && _cluster_range_min_ >= 0 && _cluster_range_max_ >= 0) {
        DT_THROW_IF(_cluster_range_min_ > _cluster_range_max_, std::logic_error,
                    "Invalid 'range_cluster.min' > 'range_cluster.max' values !");
      }
    }  // end if is_mode_range_cluster

    // mode HAS_UNCLUSTERED_HITS:
    if (is_mode_has_unclustered_hits()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using HAS_UNCLUSTERED_HITS mode...");
    }  // end if is_mode_has_unclustered_hits

    // mode RANGE_UNCLUSTERED_HITS:
    if (is_mode_range_unclustered_hits()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_UNCLUSTERED_HITS mode...");
      int count = 0;
      if (configuration_.has_key("range_unclustered_hits.min")) {
        int nmin = configuration_.fetch_integer("range_unclustered_hits.min");
        DT_THROW_IF(nmin < 0, std::range_error,
                    "Invalid min number of unclustered hits (" << nmin << ") !");
        _unclustered_range_min_ = nmin;
        count++;
      }
      if (configuration_.has_key("range_unclustered_hits.max")) {
        int nmax = configuration_.fetch_integer("range_unclustered_hits.max");
        DT_THROW_IF(nmax < 0, std::range_error,
                    "Invalid max number of unclustered hits (" << nmax << ") !");
        _unclustered_range_max_ = nmax;
        count++;
      }
      DT_THROW_IF(
          count == 0, std::logic_error,
          "Missing 'range_unclustered_hits.min' or 'range_unclustered_hits.max' property !");
      if (count == 2 && _unclustered_range_min_ >= 0 && _unclustered_range_max_ >= 0) {
        DT_THROW_IF(_unclustered_range_min_ > _unclustered_range_max_, std::logic_error,
                    "Invalid 'range_unclustered_hits.min' > 'range_unclustered_hits.max' values !");
      }
    }  // end if is_mode_range_unclustered_hits
  }

  this->i_cut::_set_initialized(true);
}

int tracker_clustering_data_cut::_accept() {
  int cut_returned = cuts::SELECTION_INAPPLICABLE;

  // Get event record
  const datatools::things& ER = get_user_data<datatools::things>();

  if (!ER.has(_TCD_label_)) {
    DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _TCD_label_ << "' bank !");
    return cut_returned;
  }

  // Get tracker clustering data bank
  const snemo::datamodel::tracker_clustering_data& TCD =
      ER.get<snemo::datamodel::tracker_clustering_data>(_TCD_label_);

  // Check if the tracker clustering data has a property flag with a specific name :
  bool check_flag = false;
  if (is_mode_flag()) {
    DT_LOG_ERROR(get_logging_priority(), "FLAG mode no longer supported");
  }

  // Check if the tracker clustering data has clusters :
  bool check_has_cluster = true;
  if (is_mode_has_cluster()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running HAS_CLUSTER mode...");
    // 2012-05-13 XG: Here we only take care of the default solution
    if (!TCD.has_default_solution()) {
      check_has_cluster = false;
    } else {
      check_has_cluster = !TCD.get_default_solution().get_clusters().empty();
    }
  }

  // Check if the tracker clustering data has a range of clusters :
  bool check_range_cluster = true;
  if (is_mode_range_cluster()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_CLUSTER mode...");
    // 2012-05-13 XG: Here we only take care of the default solution
    bool check_has_cluster_2 = true;
    if (!TCD.has_default_solution()) {
      check_has_cluster_2 = false;
    } else {
      check_has_cluster_2 = !TCD.get_default_solution().get_clusters().empty();
    }
    if (!check_has_cluster_2) {
      DT_LOG_DEBUG(get_logging_priority(), "Tracker clustering data has no clusters");
      return cuts::SELECTION_INAPPLICABLE;
    }

    const size_t nclusters = TCD.get_default_solution().get_clusters().size();
    DT_LOG_DEBUG(get_logging_priority(),
                 "Number of isolated cluster= " << nclusters << " "
                                                << "cluster_min= " << _cluster_range_min_ << " "
                                                << "cluster_max= " << _cluster_range_max_);

    if (_cluster_range_min_ >= 0) {
      if (nclusters < (size_t)_cluster_range_min_) {
        check_range_cluster = false;
      }
    }
    if (_cluster_range_max_ >= 0) {
      if (nclusters > (size_t)_cluster_range_max_) {
        check_range_cluster = false;
      }
    }
  }

  // Check if the tarcker clustering data has unclustered hits
  bool check_has_unclustered_hits = true;
  if (is_mode_has_unclustered_hits()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running HAS_UNCLUSTERED_HITS mode...");
    // 2012-05-13 XG: Here we only take care of the default solution
    if (!TCD.has_default_solution()) {
      check_has_unclustered_hits = false;
    } else {
      check_has_unclustered_hits = !TCD.get_default_solution().get_unclustered_hits().empty();
    }
  }

  // Check if the tracker clustering data has a range of unclustered hits :
  bool check_range_unclustered_hits = true;
  if (is_mode_range_unclustered_hits()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_UNCLUSTERED_HITS mode...");
    // 2012-05-13 XG: Here we only take care of the default solution
    bool check_has_unclustered_hits_2 = true;
    if (!TCD.has_default_solution()) {
      check_has_unclustered_hits_2 = false;
    } else {
      check_has_unclustered_hits_2 = !TCD.get_default_solution().get_unclustered_hits().empty();
    }
    if (!check_has_unclustered_hits_2) {
      DT_LOG_DEBUG(get_logging_priority(), "Tracker clustering data has no unclustered hits");
      return cuts::SELECTION_INAPPLICABLE;
    }

    const size_t nunclustered_hits = TCD.get_default_solution().get_unclustered_hits().size();
    DT_LOG_DEBUG(get_logging_priority(),
                 "Number of unclustered cluster= "
                     << nunclustered_hits << " "
                     << "unclustered_hits_min= " << _unclustered_range_min_ << " "
                     << "unclustered_hits_max= " << _unclustered_range_max_);

    if (_unclustered_range_min_ >= 0) {
      if (nunclustered_hits < (size_t)_unclustered_range_min_) {
        check_range_unclustered_hits = false;
      }
    }
    if (_unclustered_range_max_ >= 0) {
      if (nunclustered_hits > (size_t)_unclustered_range_max_) {
        check_range_unclustered_hits = false;
      }
    }
  }

  cut_returned = cuts::SELECTION_REJECTED;
  if (check_flag && check_has_cluster && check_range_cluster && check_has_unclustered_hits &&
      check_range_unclustered_hits) {
    cut_returned = cuts::SELECTION_ACCEPTED;
  }

  return cut_returned;
}

}  // end of namespace cut

}  // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::tracker_clustering_data_cut, ocd_) {
  ocd_.set_class_name("snemo::cut::tracker_clustering_data_cut");
  ocd_.set_class_description(
      "Cut based on criteria applied to the Tracker Clustering data bank stored in the event "
      "record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  {
    // Description of the 'TCD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TCD_label")
        .set_terse_description("The name of the Tracker Clustering Data bank")
        .set_traits(datatools::TYPE_STRING)
        .set_default_value_string(
            snemo::datamodel::data_info::default_tracker_clustering_data_label())
        .add_example(
            "Set the default value::        \n"
            "                               \n"
            "  TCD_label : string = \"TCD\" \n"
            "                               \n");
  }

  {
    // Description of the 'mode.flag' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.flag")
        .set_terse_description("Mode with a special request flag")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                              \n"
            "                                                 \n"
            "  mode.flag : boolean = true                     \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.has_cluster' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_cluster")
        .set_terse_description("Mode requiring the presence of cluster")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                 \n"
            "                                    \n"
            "  mode.has_cluster : boolean = true \n"
            "                                    \n");
  }

  {
    // Description of the 'mode.range_cluster' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_cluster")
        .set_terse_description("Mode with a special requested ranged cluster")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                   \n"
            "                                      \n"
            "  mode.range_cluster : boolean = true \n"
            "                                      \n");
  }

  {
    // Description of the 'mode.has_unclustered_hits' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_unclustered_hits")
        .set_terse_description("Mode requiring the presence of unclustered hits")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                          \n"
            "                                             \n"
            "  mode.has_unclustered_hits : boolean = true \n"
            "                                             \n");
  }

  {
    // Description of the 'mode.range_unclustered_hits' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_unclustered_hits")
        .set_terse_description("Mode with a special requested range of unclustered hits")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                            \n"
            "                                               \n"
            "  mode.range_unclustered_hits : boolean = true \n"
            "                                               \n");
  }

  {
    // Description of the 'range_cluster.min' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_cluster.min")
        .set_terse_description("Minimum number of clusters of the requested ranged cluster")
        .set_triggered_by_flag("mode.range_cluster")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific minimum number of clusters:: \n"
            "                                            \n"
            "  range_cluster.min : integer = 3           \n"
            "                                            \n");
  }

  {
    // Description of the 'range_cluster.max' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_cluster.max")
        .set_terse_description("Maximum number of clusters of the requested ranged cluster")
        .set_triggered_by_flag("mode.range_cluster")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific maximum number of clusters:: \n"
            "                                            \n"
            "  range_cluster.max : integer = 20          \n"
            "                                            \n");
  }

  {
    // Description of the 'range_unclustered_hits.min' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_unclustered_hits.min")
        .set_terse_description("Minimum number of unclustered hits")
        .set_triggered_by_flag("mode.range_unclustered_hits")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific minimum number of unclustered hits:: \n"
            "                                                    \n"
            "  range_unclustered_hits.min : integer = 3          \n"
            "                                                    \n");
  }

  {
    // Description of the 'range_unclustered_hits.max' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_unclustered_hits.max")
        .set_terse_description("Maximum number of unclustered hits")
        .set_triggered_by_flag("mode.range_unclustered_hits")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific maximum number of unclustered hits:: \n"
            "                                                    \n"
            "  range_unclustered_hits.max : integer = 20         \n"
            "                                                    \n");
  }

  // Additional configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the           \n"
      "``datatools::properties`` ASCII format::              \n"
      "                                                      \n"
      "                                                      \n"
      "   TCD_label : string = \"TCD\"                       \n"
      "   mode.flag : boolean = false                        \n"
      "   # flag.name : string = \"test2\"                   \n"
      "   mode.has_cluster : boolean = false                 \n"
      "   mode.range_cluster : boolean = true                \n"
      "   range_cluster.min : integer = 1                    \n"
      "   range_cluster.max : integer = 2                    \n"
      "   mode.has_unclustered_hits : boolean = true         \n"
      "   mode.range_unclustered_hits : boolean = true       \n"
      "   range_unclustered_hits.min : integer = 0           \n"
      "   range_unclustered_hits.max : integer = 3           \n"
      "                                                      \n");

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::cut::tracker_clustering_data_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::tracker_clustering_data_cut,
                               "snemo::cut::tracker_clustering_data_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
