// falaise/snemo/cuts/simulated_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/simulated_data_cut.h>

/// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>
// - Bayeux/mctools:
#include <bayeux/mctools/simulated_data.h>

// This project :
#include <falaise/property_set.h>
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

namespace cut {

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(simulated_data_cut, "snemo::cut::simulated_data_cut")

simulated_data_cut::simulated_data_cut(datatools::logger::priority lp) : cuts::i_cut(lp) {
  _set_defaults();
}

simulated_data_cut::~simulated_data_cut() {
  if (is_initialized()) {
    this->simulated_data_cut::reset();
  }
}

void simulated_data_cut::reset() {
  _set_defaults();
  this->i_cut::_reset();
  this->i_cut::_set_initialized(false);
}

void simulated_data_cut::initialize(const datatools::properties& configuration_,
                                    datatools::service_manager& /* service_manager_ */,
                                    cuts::cut_handle_dict_type& /* cut_dict_ */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cut '" << get_name() << "' is already initialized ! ");

  this->i_cut::_common_initialize(configuration_);

  falaise::property_set ps{configuration_};

  SDTag_ =
      ps.get<std::string>("SD_label", snemo::datamodel::data_info::default_simulated_data_label());

  if (ps.has_key("mode.flag")) {
    cutMode_ |= mode_t::FLAG;
    flagLabel_ = ps.get<std::string>("flag.name");
  }

  if (ps.has_key("mode.has_hit_category")) {
    cutMode_ |= mode_t::HAS_HIT_CATEGORY;
    hitCategory_ = ps.get<std::string>("has_hit_category.category");
  }

  if (configuration_.has_flag("mode.range_hit_category")) {
    cutMode_ |= mode_t::RANGE_HIT_CATEGORY;
    hitCategory_ = ps.get<std::string>("range_hit_category.category");
    minHitCount_ = ps.get<int>("range_hit_category.min");
    DT_THROW_IF(minHitCount_ < 0, std::out_of_range, "Min hit count < 0")
    maxHitCount_ = ps.get<int>("range_hit_category.max");
    DT_THROW_IF(maxHitCount_ < 0, std::out_of_range, "Max hit count < 0")
  }

  if (configuration_.has_flag("mode.has_hit_property")) {
    cutMode_ |= mode_t::HAS_HIT_PROPERTY;
    hitCategory_ = ps.get<std::string>("has_hit_property.category");
    hitPropertyLogic_ = ps.get<std::string>("has_hit_property.logic");
    DT_THROW_IF(hitPropertyLogic_ != "and" && hitPropertyLogic_ != "or", std::logic_error,
                "Logic operation '" << hitPropertyLogic_ << "' is unkown or not supported!");

    auto keys = ps.get<std::vector<std::string>>("has_hit_property.keys");
    for (const std::string& k : keys) {
      const std::string check_key = "has_hit_property." + k + ".values";
      hitPropertyMap_[k] = ps.get<std::vector<std::string>>(check_key);
    }
  }

  DT_THROW_IF(cutMode_ == mode_t::UNDEFINED, std::logic_error,
              "Missing at least a 'mode.XXX' property ! ");

  this->i_cut::_set_initialized(true);
}


void simulated_data_cut::setSDTag(const std::string& tag) { SDTag_ = tag; }

const std::string& simulated_data_cut::getSDTag() const { return SDTag_; }

uint32_t simulated_data_cut::getCutMode() const { return cutMode_; }

bool simulated_data_cut::cutsOnFlag() const { return (cutMode_ & mode_t::FLAG) != 0u; }

bool simulated_data_cut::cutsOnHitCount() const {
  return (cutMode_ & mode_t::RANGE_HIT_CATEGORY) != 0u;
}

bool simulated_data_cut::cutsOnHitCategory() const {
  return (cutMode_ & mode_t::HAS_HIT_CATEGORY) != 0u;
}

bool simulated_data_cut::cutsOnHitProperty() const {
  return (cutMode_ & mode_t::HAS_HIT_PROPERTY) != 0u;
}

void simulated_data_cut::setFlagLabel(const std::string& label) { flagLabel_ = label; }

const std::string& simulated_data_cut::getFlagLabel() const { return flagLabel_; }

void simulated_data_cut::_set_defaults() {
  SDTag_ = "";
  cutMode_ = mode_t::UNDEFINED;
  flagLabel_ = "";
  hitCategory_ = "";
  minHitCount_ = -1;
  maxHitCount_ = -1;
  hitPropertyLogic_ = "";
}

int simulated_data_cut::_accept() {
  int cut_returned = cuts::SELECTION_INAPPLICABLE;

  // Get event record
  const auto& ER = get_user_data<datatools::things>();

  if (!ER.has(SDTag_)) {
    return cut_returned;
  }

  // Get simulated data bank
  const auto& SD = ER.get<mctools::simulated_data>(SDTag_);

  // Check if the simulated data has a property flag with a specific name :
  bool check_flag = true;
  if (cutsOnFlag()) {
    check_flag = SD.get_properties().has_flag(flagLabel_);
  }

  // Check if the simulated data has some specific category of hits :
  bool check_has_hit_category = true;
  if (cutsOnHitCategory()) {
    check_has_hit_category = SD.has_step_hits(hitCategory_);
  }

  // Check if the simulated data has some specific category of hits :
  bool check_range_hit_category = true;
  if (cutsOnHitCount()) {
    if (!SD.has_step_hits(hitCategory_)) {
      return cuts::SELECTION_INAPPLICABLE;
    }
    const size_t nhits = SD.get_number_of_step_hits(hitCategory_);
    bool check = true;
    if (minHitCount_ >= 0) {
      if (nhits < (size_t)minHitCount_) {
        check = false;
      }
    }
    if (maxHitCount_ >= 0) {
      if (nhits > (size_t)maxHitCount_) {
        check = false;
      }
    }
    if (!check) {
      check_range_hit_category = false;
    }
  }

  // Check if the simulated step hits have some property value :
  bool check_has_hit_property = true;
  if (cutsOnHitProperty()) {
    if (!SD.has_step_hits(hitCategory_)) {
      return cuts::SELECTION_INAPPLICABLE;
    }
    const mctools::simulated_data::hit_handle_collection_type& the_step_hits =
        SD.get_step_hits(hitCategory_);

    // Iterators
    auto istart = the_step_hits.begin();
    auto istop = the_step_hits.end();
    auto iprop = hitPropertyMap_.begin();

    while (iprop != hitPropertyMap_.end()) {
      const std::string& a_key = iprop->first;
      const std::vector<std::string>& the_values = iprop->second;
      geomtools::base_hit::has_string_property_predicate str_pred(a_key, the_values);
      datatools::mother_to_daughter_predicate<geomtools::base_hit, mctools::base_step_hit> pred(
          str_pred);
      datatools::handle_predicate<mctools::base_step_hit> pred_via_handle(pred);

      // Update iterator position
      auto ifound = std::find_if(istart, istop, pred_via_handle);
      if (ifound == the_step_hits.end()) {
        check_has_hit_property = false;
        if (hitPropertyLogic_ == "and") {
          break;
        }
        // Go to the next property (OR mode)
        istart = the_step_hits.begin();
        istop = the_step_hits.end();
        iprop++;
      } else if (ifound == istop) {
        // This hit does not fulfill requirement : restart to first
        // property but at different iterator position
        istart = ++ifound;
        istop = the_step_hits.end();
        iprop = hitPropertyMap_.begin();
      } else {
        // Found one hit : check next property for this hit
        check_has_hit_property = true;
        if (hitPropertyLogic_ == "or") {
          break;
        }
        istart = ifound;
        istop = ++ifound;
        iprop++;
      }
    }
  }

  cut_returned = cuts::SELECTION_REJECTED;
  if (check_flag && check_has_hit_category && check_range_hit_category && check_has_hit_property) {
    cut_returned = cuts::SELECTION_ACCEPTED;
  }

  return cut_returned;
}

}  // namespace cut

}  // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::simulated_data_cut, ocd_) {
  ocd_.set_class_name("snemo::cut::simulated_data_cut");
  ocd_.set_class_description(
      "Cut based on criteria applied to the simulated data bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SD_label")
        .set_terse_description("The name of the Simulated Data bank")
        .set_traits(datatools::TYPE_STRING)
        .set_default_value_string(snemo::datamodel::data_info::default_simulated_data_label())
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  SD_label : string = \"SD\"                     \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.flag' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.flag")
        .set_terse_description("Mode with a special request flag")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the requested flag mode::               \n"
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
            "Activate the requested hit category mode::       \n"
            "                                                 \n"
            "  mode.has_hit_category : boolean = true         \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.range_hit_category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_hit_category")
        .set_terse_description("Mode with a special requested hit category to be ranged")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the requested ranged hit category mode::\n"
            "                                                 \n"
            "  mode.range_hit_category : boolean = true       \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.has_hit_property' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_hit_property")
        .set_terse_description("Mode with a special requested hit property")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the requested hit property mode::       \n"
            "                                                 \n"
            "  mode.has_hit_property : boolean = true         \n"
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
        .add_example(
            "Set a specific hit category::                             \n"
            "                                                          \n"
            "  has_hit_category.category : string = \"calo\"           \n"
            "                                                          \n");
  }

  {
    // Description of the 'range_hit_category.category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_hit_category.category")
        .set_terse_description("Name of the requested ranged hit category")
        .set_triggered_by_flag("mode.range_hit_category")
        .set_traits(datatools::TYPE_STRING)
        .add_example(
            "Set a specific hit category to be ranged::                \n"
            "                                                          \n"
            "  range_hit_category.category : string = \"gg\"           \n"
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
    // Description of the 'has_hit_property.category' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("has_hit_property.category")
        .set_terse_description("Name of the hit category to search for a specific property")
        .set_triggered_by_flag("mode.has_hit_property")
        .set_traits(datatools::TYPE_STRING)
        .add_example(
            "Set a specific hit category to be investigated::          \n"
            "                                                          \n"
            "  has_hit_property.category : string = \"gg\"             \n"
            "                                                          \n");
  }

  {
    // Description of the 'has_hit_property.keys' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("has_hit_property.keys")
        .set_terse_description("Names of property keys to be tested in a specific hit category")
        .set_triggered_by_flag("mode.has_hit_property")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .add_example(
            "Set the lists of keys to be tested::                      \n"
            "                                                          \n"
            "  has_hit_property.keys : string[2] = \\                  \n"
            "     \"noisy\" \\                                         \n"
            "     \"bad\"                                              \n"
            "                                                          \n");
  }

  {
    // Description of the 'has_hit_property.${has_hit_property.keys}.values' configuration property
    // :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("has_hit_property.${has_hit_property.keys}.values")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .add_example(
            "Set the lists of values to be associated to tested keys::   \n"
            "                                                            \n"
            "  has_hit_property.keys : string[2] = \\                    \n"
            "     \"creator_process\" \\                                 \n"
            "     \"g4_volume\"                                          \n"
            "  has_hit_property.creator_process.values : string[2] = \\  \n"
            "     \"brems\" \\                                           \n"
            "     \"compton\"                                            \n"
            "  has_hit_property.g4_volume.values : string[2] = \\        \n"
            "     \"drift_cell_core.log\" \\                             \n"
            "     \"tracker_chamber.log\"                                \n"
            "                                                            \n");
  }

  // Additional configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the     \n"
      "``datatools::properties`` ASCII format::        \n"
      "                                                \n"
      "   SD_label : string = \"SD\"                   \n"
      "   mode.flag : boolean = false                  \n"
      "   # flag.name : string = \"high_energy\"       \n"
      "   mode.has_hit_category : boolean = false      \n"
      "   # has_hit_category.category : string = \"gg\"\n"
      "   mode.range_hit_category : boolean = true     \n"
      "   range_hit_category.category : string = \"gg\"\n"
      "   range_hit_category.min : integer = 5         \n"
      "   range_hit_category.max : integer = 20        \n"
      "   mode.has_hit_property : boolean = true       \n"
      "   has_hit_property.category : string = \"gg\"  \n"
      "   has_hit_property.logic : string = \"or\"     \n"
      "   has_hit_property.keys : string[2] = \\                   \n"
      "     \"creator_process\" \\                                 \n"
      "     \"g4_volume\"                                          \n"
      "   has_hit_property.creator_process.values : string[1] = \\ \n"
      "         \"brems\"                                          \n"
      "   has_hit_property.g4_volume.values : string[2] = \\       \n"
      "         \"tracking_chamber.log\"  \"drift_cell.log\"       \n"
      "                                                            \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::cut::simulated_data_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::simulated_data_cut, "snemo::cut::simulated_data_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
