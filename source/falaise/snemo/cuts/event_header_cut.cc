// falaise/snemo/cuts/event_header_cut.cc

// Ourselves:
#include "falaise/snemo/cuts/event_header_cut.h"

// Standard library:
#include <fstream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>

// This project :
#include "falaise/property_set.h"
#include "falaise/snemo/datamodels/event_header.h"

namespace snemo {

namespace cut {

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(event_header_cut, "snemo::cut::event_header_cut")

event_header_cut::event_header_cut(datatools::logger::priority lp) : cuts::i_cut(lp) {
  _set_defaults();
}

event_header_cut::~event_header_cut() {
  // Because f-you i_cut
  reset();
}

void event_header_cut::reset() {
  _set_defaults();
  this->i_cut::_reset();
  this->i_cut::_set_initialized(false);
}

void event_header_cut::initialize(const datatools::properties& dps,
                                  datatools::service_manager& /* unused */,
                                  cuts::cut_handle_dict_type& /* unused */) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Cut '" << get_name() << "' is already initialized ! ");
  this->i_cut::_common_initialize(dps);

  falaise::property_set ps{dps};

  eventHeaderTag_ = ps.get<std::string>("EH_label");

  if (dps.has_flag("mode.flag")) {
    cutMode_ |= mode_t::FLAG;
    flagLabel_ = ps.get<std::string>("flag.name");
  }

  if (dps.has_flag("mode.run_number")) {
    cutMode_ |= mode_t::RUN_NUMBER;
    setMinRunNumber(ps.get<int>("run_number.min"));
    setMaxRunNumber(ps.get<int>("run_number.max"));
  }

  if (dps.has_flag("mode.event_number")) {
    cutMode_ |= mode_t::EVENT_NUMBER;
    setMinEventNumber(ps.get<int>("event_number.min"));
    setMaxEventNumber(ps.get<int>("event_number.max"));
  }

  if (dps.has_flag("mode.list_of_event_ids")) {
    cutMode_ |= mode_t::EVENT_ID_LIST;

    if (ps.has_key("list_of_event_ids.ids")) {
      auto str_ids = ps.get<std::vector<std::string>>("list_of_event_ids.ids");
      for (const auto& str_id : str_ids) {
        datatools::event_id the_event_id;
        the_event_id.from_string(str_id);
        eventIDs_.insert(the_event_id);
      }
    } else if (dps.has_key("list_of_event_ids.file")) {
      auto filelist = ps.get<std::string>("list_of_event_ids.file");
      loadEventIDList(filelist);
    } else {
      DT_THROW(std::logic_error,
               "Missing 'list_of_event_ids.ids' or 'list_of_event_ids.file' property !");
    }
  }

  DT_THROW_IF(cutMode_ == mode_t::UNDEFINED, std::logic_error,
              "Missing at least a 'mode.XXX' property ! ");

  this->i_cut::_set_initialized(true);
}

void event_header_cut::setEventHeaderTag(const std::string& tag) { eventHeaderTag_ = tag; }

const std::string& event_header_cut::getEventHeaderTag() const { return eventHeaderTag_; }

uint32_t event_header_cut::cutMode() const { return cutMode_; }

bool event_header_cut::cutsOnFlag() const { return (cutMode_ & mode_t::FLAG) != 0u; }

bool event_header_cut::cutsOnRunNumber() const { return (cutMode_ & mode_t::RUN_NUMBER) != 0u; }

bool event_header_cut::cutsOnEventNumber() const { return (cutMode_ & mode_t::EVENT_NUMBER) != 0u; }

bool event_header_cut::cutsOnEventIDs() const { return (cutMode_ & mode_t::EVENT_ID_LIST) != 0u; }

void event_header_cut::setFlagLabel(const std::string& label) { flagLabel_ = label; }

const std::string& event_header_cut::getFlagLabel() const { return flagLabel_; }

void event_header_cut::setMinRunNumber(int min) { minRunNumber_ = min >= 0 ? min : -1; }

void event_header_cut::setMaxRunNumber(int max) { maxRunNumber_ = max >= 0 ? max : -1; }

void event_header_cut::setMinEventNumber(int min) { minEventNumber_ = min >= 0 ? min : -1; }

void event_header_cut::setMaxEventNumber(int max) { maxEventNumber_ = max >= 0 ? max : -1; }

void event_header_cut::loadEventIDList(const std::string& fname) {
  std::string filename = fname;
  datatools::fetch_path_with_env(filename);
  std::ifstream ifs(filename.c_str());
  DT_THROW_IF(!ifs, std::logic_error,
              "Cannot open file '" << filename << "' (list of event IDs) !");
  while (ifs) {
    std::string line;
    std::getline(ifs, line);
    DT_THROW_IF(!ifs, std::logic_error,
                "I/O error while reading file '" << filename << "' (list of event IDs) !");
    if (!line.empty()) {
      std::istringstream iss(line);
      std::string word;
      iss >> word;
      // skip blank and comment lines:
      if (word.length() > 0 && word[0] != '#') {
        datatools::event_id the_event_id;
        the_event_id.from_string(word);
        if (the_event_id.is_valid()) {
          eventIDs_.insert(the_event_id);
        } else {
          DT_THROW(std::logic_error, "Invalid value for event ID ('"
                                         << word << "')  while reading file '" << filename
                                         << "' (list of event IDs) !");
        }
      }
    }
    ifs >> std::ws;
    if (ifs.eof()) {
      break;
    }
  }

  cutMode_ |= mode_t::EVENT_ID_LIST;
}

void event_header_cut::_set_defaults() {
  eventHeaderTag_ = "";
  cutMode_ = mode_t::UNDEFINED;
  flagLabel_ = "";
  minRunNumber_ = -1;
  maxRunNumber_ = -1;
  minEventNumber_ = -1;
  maxEventNumber_ = -1;
}

int event_header_cut::_accept() {
  uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

  // Get event record
  const auto& ER = get_user_data<datatools::things>();

  if (!ER.has(eventHeaderTag_)) {
    return cut_returned;
  }

  // Get event header bank
  const auto& EH = ER.get<snemo::datamodel::event_header>(eventHeaderTag_);

  // Check if the event header has a property flag with a specific name :
  bool check_flag = true;
  if (cutsOnFlag()) {
    check_flag = EH.get_properties().has_flag(flagLabel_);
  }

  // Check if the event ID has a run number within some interval :
  bool check_run_number = true;
  if (cutsOnRunNumber()) {
    if (!EH.get_id().is_valid()) {
      return cut_returned;
    }
    const int rn = EH.get_id().get_run_number();
    if (minRunNumber_ >= 0 && rn < minRunNumber_) {
      check_run_number = false;
    }
    if (maxRunNumber_ >= 0 && rn > maxRunNumber_) {
      check_run_number = false;
    }
  }

  // Check if the event ID has a event number within some interval :
  bool check_event_number = true;
  if (cutsOnEventNumber()) {
    if (!EH.get_id().is_valid()) {
      return cut_returned;
    }
    const int en = EH.get_id().get_event_number();
    if (minEventNumber_ >= 0 && en < minEventNumber_) {
      check_event_number = false;
    }
    if (maxEventNumber_ >= 0 && en > maxEventNumber_) {
      check_event_number = false;
    }
  }

  // Check if the event ID belongs to a given list :
  bool check_list_of_events = true;
  if (cutsOnEventIDs()) {
    if (!EH.get_id().is_valid()) {
      return cut_returned;
    }
    auto found = eventIDs_.find(EH.get_id());
    check_list_of_events = (found != eventIDs_.end());
  }

  cut_returned = cuts::SELECTION_REJECTED;
  if (check_flag && check_run_number && check_event_number && check_list_of_events) {
    cut_returned = cuts::SELECTION_ACCEPTED;
  }

  return cut_returned;
}

}  // namespace cut
}  // namespace snemo

// OCD support:

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::event_header_cut, ocd_) {
  ocd_.set_class_name("snemo::cut::event_header_cut");
  ocd_.set_class_description(
      "Cut based on criteria applied to the event header bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  {
    // Description of the 'EH_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("EH_label")
        .set_terse_description("The name of the Event Header bank")
        .set_traits(datatools::TYPE_STRING)
        .set_default_value_string("EH")
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  EH_label : string = \"EH\"                     \n"
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
    // Description of the 'mode.run_number' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.run_number")
        .set_terse_description("Mode requesting some specific run numbers")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the requested run number selection mode:: \n"
            "                                                 \n"
            "  mode.run_number : boolean = true               \n"
            "                                                 \n");
  }

  {
    // Description of the 'mode.event_number' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.event_number")
        .set_terse_description("Mode requesting some specific event numbers")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the requested event number selection mode:: \n"
            "                                                     \n"
            "  mode.event_number : boolean = true                 \n"
            "                                                     \n");
  }

  {
    // Description of the 'mode.list_of_event_ids' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.list_of_event_ids")
        .set_terse_description("Mode requesting a specific list of event identifiers")
        .set_traits(datatools::TYPE_BOOLEAN)
        .add_example(
            "Activate the requested list of event IDs selection mode:: \n"
            "                                                          \n"
            "  mode.list_of_event_ids : boolean = true                 \n"
            "                                                          \n");
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
    // Description of the 'run_number.min' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("run_number.min")
        .set_terse_description("Minimum run number")
        .set_triggered_by_flag("mode.run_number")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific minimum run number::                       \n"
            "                                                          \n"
            "  run_number.min : integer = 25                           \n"
            "                                                          \n");
  }

  {
    // Description of the 'run_number.max' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("run_number.max")
        .set_terse_description("Maximum run number")
        .set_triggered_by_flag("mode.run_number")
        .set_traits(datatools::TYPE_INTEGER)
        .add_example(
            "Set a specific maximum run number:: \n"
            "                                                          \n"
            "  run_number.max : integer = 34                           \n"
            "                                                          \n");
  }

  {
    // Description of the 'event_number.min' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("event_number.min")
        .set_terse_description("Minimum event number")
        .set_traits(datatools::TYPE_INTEGER)
        .set_triggered_by_flag("mode.event_number")
        .add_example(
            "Set a specific minimum event number:: \n"
            "                                                          \n"
            "  event_number.min : integer = 0                          \n"
            "                                                          \n");
  }

  {
    // Description of the 'event_number.max' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("event_number.max")
        .set_terse_description("Maximum event number")
        .set_traits(datatools::TYPE_INTEGER)
        .set_triggered_by_flag("mode.event_number")
        .add_example(
            "Set a specific maximum event number:: \n"
            "                                                          \n"
            "  event_number.max : integer = 1000                       \n"
            "                                                          \n");
  }

  {
    // Description of the 'list_of_event_ids.ids' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("list_of_event_ids.ids")
        .set_terse_description("Explicit list of event IDs")
        .set_triggered_by_flag("mode.list_of_event_ids")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .add_example(
            "Set a specific list of event IDs:: \n"
            "                                                          \n"
            "  list_of_event_ids.ids : string[5] = \\                  \n"
            "     \"234_0\" \\                                         \n"
            "     \"234_1\" \\                                         \n"
            "     \"234_5\" \\                                         \n"
            "     \"241_2\" \\                                         \n"
            "     \"241_3\"                                            \n"
            "                                                          \n");
  }

  {
    // Description of the 'list_of_event_ids.file' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("list_of_event_ids.file")
        .set_terse_description("Set the name of the file containing an explicit list of event IDs")
        .set_triggered_by_flag("mode.list_of_event_ids")
        .set_traits(datatools::TYPE_STRING)
        .add_example(
            "Set a specific file of event IDs:: \n"
            "                                                          \n"
            "  list_of_event_ids.file : string = \"my_list.lis\"       \n"
            "                                                          \n");
  }

  // Additional configuration hints :
  ocd_.set_configuration_hints(
      "Here is a full configuration example in the     \n"
      "``datatools::properties`` ASCII format::        \n"
      "                                                \n"
      "   EH_label : string = \"EH\"                   \n"
      "   mode.flag : boolean = false                  \n"
      "   # flag.name : string = \"high_energy\"       \n"
      "   mode.run_number : boolean = false            \n"
      "   # run_number.min : integer = 0               \n"
      "   # run_number.max : integer = 1000            \n"
      "   mode.event_number : boolean = true           \n"
      "   event_number.min : integer = 0               \n"
      "   event_number.max : integer = 100000          \n"
      "   mode.list_of_event_ids : boolean = false     \n"
      "   # list_of_event_ids.ids : string[5] = \\     \n"
      "   #  \"234_0\" \\                              \n"
      "   #  \"234_1\" \\                              \n"
      "   #  \"234_5\" \\                              \n"
      "   #  \"241_2\" \\                              \n"
      "   #  \"241_3\"                                 \n"
      "   # list_of_event_ids.file: string as path = \\\n"
      "   #    \"my_selection.lis\"                    \n"
      "                                                \n");

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snemo::cut::event_header_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::event_header_cut, "snemo::cut::event_header_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
