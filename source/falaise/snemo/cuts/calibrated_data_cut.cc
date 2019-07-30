// falaise/snemo/cuts/calibrated_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/calibrated_data_cut.h>

/// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/clhep.h>

// This project :
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

namespace cut {

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(calibrated_data_cut, "snemo::cut::calibrated_data_cut")

void calibrated_data_cut::_set_defaults() {
  _CD_label_ = "";
  _mode_ = MODE_UNDEFINED;
  _flag_name_ = "";
  _hit_category_ = "";
  _hit_category_range_min_ = -1;
  _hit_category_range_max_ = -1;
  _tracker_hit_trait_bits_ = snemo::datamodel::calibrated_tracker_hit::none;
  _tracker_hit_delay_time_ = 0.0 * CLHEP::microsecond;
}

void calibrated_data_cut::set_CD_label(const std::string& CD_label_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cut '" << get_name() << "' is already initialized ! ");
  _CD_label_ = CD_label_;
}

const std::string& calibrated_data_cut::get_CD_label() const { return _CD_label_; }

uint32_t calibrated_data_cut::get_mode() const { return _mode_; }

bool calibrated_data_cut::is_mode_flag() const { return (_mode_ & MODE_FLAG) != 0u; }

bool calibrated_data_cut::is_mode_range_hit_category() const {
  return (_mode_ & MODE_RANGE_HIT_CATEGORY) != 0u;
}

bool calibrated_data_cut::is_mode_has_hit_category() const {
  return (_mode_ & MODE_HAS_HIT_CATEGORY) != 0u;
}

bool calibrated_data_cut::is_mode_tracker_hit_has_traits() const {
  return (_mode_ & MODE_TRACKER_HIT_HAS_TRAITS) != 0u;
}

bool calibrated_data_cut::is_mode_tracker_hit_is_delayed() const {
  return (_mode_ & MODE_TRACKER_HIT_IS_DELAYED) != 0u;
}

void calibrated_data_cut::set_flag_name(const std::string& flag_name_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cut '" << get_name() << "' is already initialized ! ");
  _flag_name_ = flag_name_;
}

const std::string& calibrated_data_cut::get_flag_name() const { return _flag_name_; }

calibrated_data_cut::calibrated_data_cut(datatools::logger::priority logger_priority_)
    : cuts::i_cut(logger_priority_) {
  _set_defaults();
}

calibrated_data_cut::~calibrated_data_cut() {
  if (is_initialized()) {
    this->calibrated_data_cut::reset();
  }
}

void calibrated_data_cut::reset() {
  _set_defaults();
  this->i_cut::_reset();
  this->i_cut::_set_initialized(false);
}

void calibrated_data_cut::initialize(const datatools::properties& configuration_,
                                     datatools::service_manager& /* service_manager_ */,
                                     cuts::cut_handle_dict_type& /* cut_dict_ */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cut '" << get_name() << "' is already initialized ! ");

  this->i_cut::_common_initialize(configuration_);

  if (_CD_label_.empty()) {
    if (configuration_.has_key("CD_label")) {
      set_CD_label(configuration_.fetch_string("CD_label"));
    } else {
      set_CD_label(snemo::datamodel::data_info::default_calibrated_data_label());
    }
  }

  if (_mode_ == MODE_UNDEFINED) {
    if (configuration_.has_flag("mode.flag")) {
      _mode_ |= MODE_FLAG;
    }

    if (configuration_.has_flag("mode.has_hit_category")) {
      _mode_ |= MODE_HAS_HIT_CATEGORY;
    }

    if (configuration_.has_flag("mode.range_hit_category")) {
      _mode_ |= MODE_RANGE_HIT_CATEGORY;
    }

    if (configuration_.has_flag("mode.tracker_hit_has_traits")) {
      _mode_ |= MODE_TRACKER_HIT_HAS_TRAITS;
    }

    if (configuration_.has_flag("mode.tracker_hit_is_delayed")) {
      _mode_ |= MODE_TRACKER_HIT_IS_DELAYED;
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

    // mode HAS_HIT_CATEGORY:
    if (is_mode_has_hit_category()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using HAS_HIT_CATEGORY mode...");
      DT_THROW_IF(!configuration_.has_key("has_hit_category.category"), std::logic_error,
                  "Missing 'has_hit_category.category' property !");
      _hit_category_ = configuration_.fetch_string("has_hit_category.category");

      DT_THROW_IF(_hit_category_ != "tracker" && _hit_category_ != "calorimeter", std::logic_error,
                  "Property '" << _hit_category_ << "' for "
                               << "'has_hit_category.category' is not impemented ! "
                               << "Use either 'tracker' or 'calorimeter' category !");
    }  // end if is_mode_has_hit_category

    // mode RANGE_HIT_CATEGORY:
    if (is_mode_range_hit_category()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_HIT_CATEGORY mode...");
      DT_THROW_IF(!configuration_.has_key("range_hit_category.category"), std::logic_error,
                  "Missing 'range_hit_category.category' property !");
      _hit_category_ = configuration_.fetch_string("range_hit_category.category");

      DT_THROW_IF(_hit_category_ != "tracker" && _hit_category_ != "calorimeter", std::logic_error,
                  "Property '" << _hit_category_ << "' for "
                               << "'range_hit_category.category' is not impemented ! "
                               << "Use either 'tracker' or 'calorimeter' category !");

      int count = 0;
      if (configuration_.has_key("range_hit_category.min")) {
        const int nmin = configuration_.fetch_integer("range_hit_category.min");
        DT_THROW_IF(nmin < 0, std::logic_error, "Invalid min number of hits (" << nmin << ") !");
        _hit_category_range_min_ = nmin;
        count++;
      }
      if (configuration_.has_key("range_hit_category.max")) {
        const int nmax = configuration_.fetch_integer("range_hit_category.max");
        DT_THROW_IF(nmax < 0, std::logic_error, "Invalid max number of hits (" << nmax << ") !");
        _hit_category_range_max_ = nmax;
        count++;
      }
      DT_THROW_IF(count == 0, std::logic_error,
                  "Missing 'range_hit_category.min' or 'range_hit_category.max' property !");
      if (count == 2 && _hit_category_range_min_ >= 0 && _hit_category_range_max_ >= 0) {
        DT_THROW_IF(_hit_category_range_min_ > _hit_category_range_max_, std::logic_error,
                    "Invalid 'range_hit_category.min' > 'range_hit_category.max' values !");
      }
    }  // end if is_mode_range_hit_category

    // mode TRACKER_HIT_HAS_TRAITS:
    if (is_mode_tracker_hit_has_traits()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using TRACKER_HIT_HAS_TRAITS mode...");
      DT_THROW_IF(!configuration_.has_key("tracker_hit_has_traits.bits_name"), std::logic_error,
                  "Missing 'tracker_hit_has_traits.bits_name' property !");
      std::vector<std::string> bits_name;
      configuration_.fetch("tracker_hit_has_traits.bits_name", bits_name);
      for (const auto& bit_name : bits_name) {
        if (bit_name == "delayed") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::delayed;
        } else if (bit_name == "noisy") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::noisy;
        } else if (bit_name == "missing_bottom_cathode") {
          _tracker_hit_trait_bits_ |=
              snemo::datamodel::calibrated_tracker_hit::missing_bottom_cathode;
        } else if (bit_name == "missing_top_cathode") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::missing_top_cathode;
        } else if (bit_name == "peripheral") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::peripheral;
        } else if (bit_name == "xy") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::xy;
        } else if (bit_name == "sterile") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::sterile;
        } else if (bit_name == "fake") {
          _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::fake;
        } else {
          DT_THROW_IF(true, std::logic_error, "Tracker bit trait '" << bit_name << "' is unkown !");
        }
      }
    }  // end of is_mode_tracker_hit_has_traits

    // mode TRACKER_HIT_IS_DELAYED:
    if (is_mode_tracker_hit_is_delayed()) {
      DT_LOG_DEBUG(get_logging_priority(), "Using TRACKER_HIT_IS_DELAYED mode...");
      if (configuration_.has_key("tracker_hit_is_delayed.delay_time")) {
        _tracker_hit_delay_time_ = configuration_.fetch_real_with_explicit_dimension(
            "tracker_hit_is_delayed.delay_time", "time");
      }
    }  // end of is_mode_tracker_hit_is_delayed
  }

  this->i_cut::_set_initialized(true);
}

int calibrated_data_cut::_accept() {
  uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

  // Get event record
  const auto& ER = get_user_data<datatools::things>();

  if (!ER.has(_CD_label_)) {
    DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _CD_label_ << "' bank !");
    return cut_returned;
  }

  // Get calibrated data bank
  const auto& CD = ER.get<snemo::datamodel::calibrated_data>(_CD_label_);

  // Check if the calibrated data has a property flag with a specific name :
  bool check_flag = true;
  if (is_mode_flag()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running FLAG mode...");
    const bool check = CD.get_properties().has_flag(_flag_name_);
    if (!check) {
      check_flag = false;
    }
  }

  // Check if the calibrated data has some specific calibrated hits :
  bool check_has_hit_category = true;
  if (is_mode_has_hit_category()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running HAS_HIT_CATEGORY mode...");
    bool check = true;
    if (_hit_category_ == "tracker") {
      check = CD.has_calibrated_tracker_hits();
    } else if (_hit_category_ == "calorimeter") {
      check = CD.has_calibrated_calorimeter_hits();
    }
    if (!check) {
      check_has_hit_category = false;
    }
  }

  // Check if the calibrated data has some specific category of hits :
  bool check_range_hit_category = true;
  if (is_mode_range_hit_category()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_HIT_CATEGORY mode...");
    bool check_has_hit = true;
    if (_hit_category_ == "tracker") {
      check_has_hit = CD.has_calibrated_tracker_hits();
    } else if (_hit_category_ == "calorimeter") {
      check_has_hit = CD.has_calibrated_calorimeter_hits();
    }
    if (!check_has_hit) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Calibrated data has no calibrated " << _hit_category_ << " hit !");
      return cuts::SELECTION_INAPPLICABLE;
    }

    size_t nhits = 0;
    if (_hit_category_ == "tracker") {
      nhits = CD.calibrated_tracker_hits().size();
    } else if (_hit_category_ == "calorimeter") {
      nhits = CD.calibrated_calorimeter_hits().size();
    }
    bool check = true;
    if (_hit_category_range_min_ >= 0) {
      if (nhits < (size_t)_hit_category_range_min_) {
        check = false;
      }
    }
    if (_hit_category_range_max_ >= 0) {
      if (nhits > (size_t)_hit_category_range_max_) {
        check = false;
      }
    }
    if (!check) {
      check_range_hit_category = false;
    }
  }

  // Check if the calibrated tracker hits have some specific trait bits :
  bool check_tracker_hit_has_traits = true;
  if (is_mode_tracker_hit_has_traits()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running TRACKER_HIT_HAS_TRAITS mode...");
    if (!CD.has_calibrated_tracker_hits()) {
      DT_LOG_DEBUG(get_logging_priority(), "Calibrated data has no calibrated tracker hit !");
      return cuts::SELECTION_INAPPLICABLE;
    }

    check_tracker_hit_has_traits = false;
    const snemo::datamodel::calibrated_data::tracker_hit_collection_type& hits =
        CD.calibrated_tracker_hits();
    for (const auto& hit : hits) {
      const snemo::datamodel::calibrated_tracker_hit& a_hit = hit.get();
      if (a_hit.get_trait_bit(_tracker_hit_trait_bits_)) {
        check_tracker_hit_has_traits = true;
        break;
      }
    }
  }

  // Check if the calibrated tracker hits have delayed cells :
  bool check_tracker_hit_is_delayed = true;
  if (is_mode_tracker_hit_is_delayed()) {
    DT_LOG_DEBUG(get_logging_priority(), "Running TRACKER_HIT_IS_DELAYED mode...");
    if (!CD.has_calibrated_tracker_hits()) {
      DT_LOG_DEBUG(get_logging_priority(), "Calibrated data has no calibrated tracker hit !");
      return cuts::SELECTION_INAPPLICABLE;
    }

    check_tracker_hit_is_delayed = false;
    const snemo::datamodel::calibrated_data::tracker_hit_collection_type& hits =
        CD.calibrated_tracker_hits();
    for (const auto& hit : hits) {
      const snemo::datamodel::calibrated_tracker_hit& a_hit = hit.get();
      if (a_hit.is_delayed() && a_hit.get_delayed_time() > _tracker_hit_delay_time_) {
        check_tracker_hit_is_delayed = true;
        break;
      }
    }
  }

  cut_returned = cuts::SELECTION_REJECTED;
  if (check_flag && check_has_hit_category && check_range_hit_category &&
      check_tracker_hit_has_traits && check_tracker_hit_is_delayed) {
    cut_returned = cuts::SELECTION_ACCEPTED;
  }

  return cut_returned;
}

}  // end of namespace cut

}  // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::calibrated_data_cut, ocd_) {
  ocd_.set_class_name("snemo::cut::calibrated_data_cut");
  ocd_.set_class_description(
      "Cut based on criteria applied to the calibrated data bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
        .set_terse_description("The name of the Calibrated Data bank")
        .set_traits(datatools::TYPE_STRING)
        .set_default_value_string(snemo::datamodel::data_info::default_calibrated_data_label())
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  CD_label : string = \"CD\"                     \n"
            "                                                 \n");
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
    // Description of the 'mode.has_hit_category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_hit_category")
        .set_terse_description("Mode with a special requested hit category")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                              \n"
            "                                                 \n"
            "  mode.has_hit_category : boolean = true         \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.range_hit_category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_hit_category")
        .set_terse_description("Mode with a special requested ranged hit category")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                          \n"
            "                                             \n"
            "  mode.range_hit_category : boolean = true   \n"
            "                                             \n");
  }

  {
    // Description of the 'mode.tracker_hit_has_traits' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.tracker_hit_has_traits")
        .set_terse_description("Mode with a special traits associated to tracker hits")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                              \n"
            "                                                 \n"
            "  mode.tracker_hit_has_traits : boolean = true   \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.tracker_hit_is_delayed' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.tracker_hit_is_delayed")
        .set_terse_description("Mode with selection of delayed tracker hits")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the mode::                              \n"
            "                                                 \n"
            "  mode.tracker_hit_is_delayed : boolean = true   \n"
            "                                                 \n");
  }

  {
    // Description of the 'flag.name' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("flag.name")
        .set_terse_description("Name of the requested flag")
        .set_triggered_by_flag("mode.flag")
        .set_traits(datatools::TYPE_STRING)
        .add_example(
            "Set a specific requested flag name::                      \n"
            "                                                          \n"
            "  flag.name : string = \"high_energy\"                    \n"
            "                                                          \n");
  }

  {
    // Description of the 'has_hit_category.category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("has_hit_category.category")
        .set_terse_description("Name of the requested hit category")
        .set_triggered_by_flag("mode.has_hit_category")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description(
            "Supported values are: \n"
            "                      \n"
            " * ``tracker``        \n"
            " * ``calorimeter``    \n"
            "                      \n")
        .add_example(
            "Set a specific hit category::                             \n"
            "                                                          \n"
            "  has_hit_category.category : string = \"tracker\"        \n"
            "                                                          \n");
  }

  {
    // Description of the 'range_hit_category.category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_hit_category.category")
        .set_terse_description("Name of the requested ranged hit category")
        .set_triggered_by_flag("mode.range_hit_category")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description(
            "Supported values are: \n"
            "                      \n"
            " * ``tracker``        \n"
            " * ``calorimeter``    \n"
            "                      \n")
        .add_example(
            "Set a specific hit category to be ranged::                \n"
            "                                                          \n"
            "  range_hit_category.category : string = \"tracker\"      \n"
            "                                                          \n");
  }

  {
    // Description of the 'range_hit_category.min' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_hit_category.min")
        .set_terse_description("Minimum number of hits of the requested ranged hit category")
        .set_triggered_by_flag("mode.range_hit_category")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific minimum number of hits::                   \n"
            "                                                          \n"
            "  range_hit_category.min : integer = 3                    \n"
            "                                                          \n");
  }

  {
    // Description of the 'range_hit_category.max' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_hit_category.max")
        .set_terse_description("Maximum number of hits of the requested ranged hit category")
        .set_triggered_by_flag("mode.range_hit_category")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific maximum number of hits::                   \n"
            "                                                          \n"
            "  range_hit_category.max : integer = 20                   \n"
            "                                                          \n");
  }

  {
    // Description of the 'tracker_hit_has_traits.bits_name' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("tracker_hit_has_traits.bits_name")
        .set_terse_description("Name of the requested trakcer traits")
        .set_triggered_by_flag("mode.tracker_hit_has_traits")
        .set_long_description(
            "Supported values are: \n"
            "                      \n"
            " * ``delayed``        \n"
            " * ``noisy``          \n"
            " * ``missing_bottom_cathode`` \n"
            " * ``missing_top_cathode``    \n"
            " * ``peripheral``     \n"
            " * ``xy``             \n"
            " * ``sterile``        \n"
            " * ``fake``           \n"
            "                      \n")
        .add_example(
            "Set the name of a specific tracker traits::                 \n"
            "                                                            \n"
            "  tracker_hit_has_traits.bits_name : string = \"peripheral\"\n"
            "                                                            \n");
  }

  {
    // Description of the 'tracker_hit_has_traits.bits_name' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.tracker_hit_is_delayed.delay_time")
        .set_terse_description("Tracker hit delayed time threshold")
        .set_triggered_by_flag("mode.tracker_hit_is_delayed")
        .set_traits(datatools::TYPE_STRING)
        .add_example(
            "Set the tracker hit delayed time threshold::                     \n"
            "                                                                 \n"
            "  mode.tracker_hit_is_delayed.delay_time : real as time = 10 us  \n"
            "                                                                 \n");
  }

  // Additional configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the           \n"
      "``datatools::properties`` ASCII format::              \n"
      "                                                      \n"
      "   CD_label : string = \"CD\"                         \n"
      "   mode.flag : boolean = false                        \n"
      "   # flag.name : string = \"test2\"                   \n"
      "   mode.has_hit_category : boolean = false            \n"
      "   # has_hit_category.category : string = \"tracker\" \n"
      "   mode.range_hit_category : boolean = true           \n"
      "   range_hit_category.category : string = \"tracker\" \n"
      "   range_hit_category.min : integer = 5               \n"
      "   range_hit_category.max : integer = 20              \n"
      "                                                      \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::cut::calibrated_data_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::calibrated_data_cut, "snemo::cut::calibrated_data_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
