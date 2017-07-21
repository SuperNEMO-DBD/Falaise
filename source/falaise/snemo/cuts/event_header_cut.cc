// falaise/snemo/cuts/event_header_cut.cc

// Ourselves:
#include "falaise/snemo/cuts/event_header_cut.h"

// Standard library:
#include <stdexcept>
#include <sstream>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>

// This project :
#include "falaise/snemo/datamodels/event_header.h"

namespace  {
// Until datatools::event_id set functions are fixed:
//   https://github.com/BxCppDev/Bayeux/issues/9
// provide a workaround for parsing event_ids from strings.
// Import implementation of event_id's operator>> as a factory function
// Retain behaviour of state of input stream begin modified to indicate
// general parse error, default (invalid) event_id otherwise.
datatools::event_id make_event_id(std::istream& iput) {
  int r, e;
  char c = 0;
  datatools::event_id invalidId;

  if (!(iput >> r)) return invalidId;

  if (!(iput >> c)) return invalidId;

  if (c != datatools::event_id::IO_FORMAT_SEP) {
    iput.setstate (std::ios_base::failbit);
    return invalidId;
  }

  if (!(iput >> e)) return invalidId;

  return datatools::event_id(r, e);
}
} // namespace


namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(event_header_cut, "snemo::cut::event_header_cut")

    void event_header_cut::_set_defaults ()
    {
      _EH_label_         = "";
      _mode_             = MODE_UNDEFINED;
      _flag_name_        = "";
      _run_number_min_   = -1;
      _run_number_max_   = -1;
      _event_number_min_ = -1;
      _event_number_max_ = -1;
    }

    void event_header_cut::set_EH_label (const std::string & EH_label_)
    {
      _EH_label_ = EH_label_;
    }

    const std::string & event_header_cut::get_EH_label () const
    {
      return _EH_label_;
    }

    uint32_t event_header_cut::get_mode () const
    {
      return _mode_;
    }

    bool event_header_cut::is_mode_flag () const
    {
      return _mode_ & MODE_FLAG;
    }

    bool event_header_cut::is_mode_run_number () const
    {
      return _mode_ & MODE_RUN_NUMBER;
    }

    bool event_header_cut::is_mode_event_number () const
    {
      return _mode_ & MODE_EVENT_NUMBER;
    }

    bool event_header_cut::is_mode_list_of_event_ids () const
    {
      return _mode_ & MODE_LIST_OF_EVENT_IDS;
    }

    void event_header_cut::set_flag_name (const std::string & flag_name_)
    {
      _flag_name_ = flag_name_;
    }

    const std::string & event_header_cut::get_flag_name () const
    {
      return _flag_name_;
    }

    void event_header_cut::set_run_number_min (int run_number_min_)
    {
      _run_number_min_ = run_number_min_ >= 0 ? run_number_min_ : -1;
    }

    void event_header_cut::set_run_number_max (int run_number_max_)
    {
      _run_number_max_ = run_number_max_ >= 0 ? run_number_max_ : -1;
    }

    void event_header_cut::set_event_number_min (int event_number_min_)
    {
      _event_number_min_ = event_number_min_ >= 0 ? event_number_min_ : -1;
    }

    void event_header_cut::set_event_number_max (int event_number_max_)
    {
      _event_number_max_ = event_number_max_ >= 0 ? event_number_max_ : -1;
    }

    void event_header_cut::list_of_event_ids_dump (std::ostream & out_) const
    {
      for (auto& i : _list_of_events_ids_) {
        out_ << i << std::endl;
      }
    }

    void event_header_cut::list_of_event_ids_load (const std::string & filename_)
    {
      std::string filename = filename_;
      datatools::fetch_path_with_env (filename);
      std::ifstream ifs (filename.c_str ());
      DT_THROW_IF (! ifs, std::logic_error, "Cannot open file '" << filename << "' (list of event IDs) !");
      while (ifs)
        {
          std::string line;
          std::getline (ifs, line);
          DT_THROW_IF (! ifs, std::logic_error,
                       "I/O error while reading file '" << filename << "' (list of event IDs) !");
          if (! line.empty ())
            {
              std::istringstream iss (line);
              std::string word;
              iss >> word;
              // skip blank and comment lines:
              if (word.length () > 0 && word[0] != '#')
                {
                  std::istringstream iss2 (word);
                  iss2 >> std::ws;
                  datatools::event_id the_event_id = make_event_id(iss2);
                  DT_THROW_IF (! iss2, std::logic_error,
                               "Invalid format for event ID ('" << word << "')  while reading file '"
                               << filename << "' (list of event IDs) !");
                  if (the_event_id.is_valid ())
                    {
                      _list_of_events_ids_.insert (the_event_id);
                    }
                  else
                    {
                      DT_LOG_WARNING (get_logging_priority (), "Invalid value for event ID ('" << word << "')  while reading file '"
                                      << filename << "' (list of event IDs) !");
                    }
                }
            }
          ifs >> std::ws;
          if (ifs.eof ()) break;
        }

      _mode_ |= MODE_LIST_OF_EVENT_IDS;
    }

    event_header_cut::event_header_cut (datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults ();
    }

    event_header_cut::~event_header_cut ()
    {
      if (is_initialized()) this->event_header_cut::reset();
    }

    void event_header_cut::reset ()
    {
      _set_defaults ();
      this->i_cut::_reset ();
      this->i_cut::_set_initialized (false);
    }

    void event_header_cut::initialize (const datatools::properties & configuration_,
                                       datatools::service_manager  & /* service_manager_ */,
                                       cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cut '" << get_name () << "' is already initialized ! ");

      this->i_cut::_common_initialize (configuration_);

      if (_EH_label_.empty ())
        {
          DT_THROW_IF (! configuration_.has_key ("EH_label"), std::logic_error,
                       "Missing 'EH_label' property !");
          set_EH_label (configuration_.fetch_string ("EH_label"));
        }

      if (_mode_ == MODE_UNDEFINED)
        {
          if (configuration_.has_flag ("mode.flag"))
            {
              _mode_ |= MODE_FLAG;
            }
          if (configuration_.has_flag ("mode.run_number"))
            {
              _mode_ |= MODE_RUN_NUMBER;
            }
          if (configuration_.has_flag ("mode.event_number"))
            {
              _mode_ |= MODE_EVENT_NUMBER;
            }
          if (configuration_.has_flag ("mode.list_of_event_ids"))
            {
              _mode_ |= MODE_LIST_OF_EVENT_IDS;
            }

          DT_THROW_IF (_mode_ == MODE_UNDEFINED, std::logic_error,
                       "Missing at least a 'mode.XXX' property ! ");

          if (is_mode_flag ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using FLAG mode...");
              DT_THROW_IF (! configuration_.has_key ("flag.name"),
                           std::logic_error,
                           "Missing 'flag.name' property !");
              set_flag_name (configuration_.fetch_string ("flag.name"));
            } // end if is_mode_flag

          if (is_mode_run_number ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using RUN_NUMBER mode...");
              int count = 0;
              if (configuration_.has_key ("run_number.min"))
                {
                  const int rn = configuration_.fetch_integer ("run_number.min");
                  DT_THROW_IF (rn < 0, std::logic_error, "Invalid min run number (" << rn << ") !");
                  set_run_number_min (rn);
                  count++;
                }
              if (configuration_.has_key ("run_number.max"))
                {
                  const int rn = configuration_.fetch_integer ("run_number.max");
                  DT_THROW_IF (rn < 0, std::logic_error, "Invalid max run number (" << rn << ") !");
                  set_run_number_max (rn);
                  count++;
                }
              DT_THROW_IF (count == 0, std::logic_error, "Missing 'run_number.min' or 'run_number.max' property !");
              if (count == 2 && _run_number_min_ >= 0 && _run_number_max_ >= 0)
                {
                  DT_THROW_IF (_run_number_min_ > _run_number_max_,
                               std::logic_error,
                               "Invalid 'run_number.min' > 'run_number.max' values !");
                }
            } // end if is_mode_run_number

          if (is_mode_event_number ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using EVENT_NUMBER mode...");
              int count = 0;
              if (configuration_.has_key ("event_number.min"))
                {
                  const int rn = configuration_.fetch_integer ("event_number.min");
                  DT_THROW_IF (rn < 0, std::logic_error, "Invalid min event number (" << rn << ") !");
                  set_event_number_min (rn);
                  count++;
                }
              if (configuration_.has_key ("event_number.max"))
                {
                  const int rn = configuration_.fetch_integer ("event_number.max");
                  DT_THROW_IF (rn < 0, std::logic_error, "Invalid max event number (" << rn << ") !");
                  set_event_number_max (rn);
                  count++;
                }
              DT_THROW_IF (count == 0,
                           std::logic_error,
                           "Missing 'event_number.min' or 'event_number.max' property !");
              if (count == 2 && _event_number_min_ >= 0 && _event_number_max_ >= 0)
                {
                  DT_THROW_IF (_event_number_min_ > _event_number_max_,
                               std::logic_error,
                               "Invalid 'event_number.min' > 'event_number.max' values !");
                }
            } // end if is_mode_event_number

          if (is_mode_list_of_event_ids ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using LIST_OF_EVENT_IDS mode...");
              int count = 0;
              if (configuration_.has_key ("list_of_event_ids.ids"))
                {
                  std::vector<std::string> str_ids;
                  configuration_.fetch ("list_of_event_ids.ids", str_ids);
                  for (const std::string& str : str_ids)
                    {
                      std::istringstream id_iss (str);
                      datatools::event_id the_event_id = make_event_id(id_iss);
                      DT_THROW_IF (! id_iss, std::logic_error,
                                   "Invalid format for event ID ('" << str << "') !");
                      if (the_event_id.is_valid())
                        {
                          _list_of_events_ids_.insert (the_event_id);
                        }
                      else
                        {
                          DT_LOG_WARNING (get_logging_priority (), "Invalid value for event ID ('" << str << "') in list_of_event_ids.ids");
                        }
                    }
                  count++;
                }
              if (configuration_.has_key ("list_of_event_ids.file"))
                {
                  const std::string filelist = configuration_.fetch_string ("list_of_event_ids.file");
                  list_of_event_ids_load (filelist);
                  count++;
                }
              DT_THROW_IF (count == 0, std::logic_error,
                           "Missing 'list_of_event_ids.ids' or 'list_of_event_ids.file' property !");
              DT_LOG_DEBUG (get_logging_priority (), "Dumping list of event id:");
              if (is_debug ())
                {
                  list_of_event_ids_dump (std::clog);
                }
            } // end if is_mode_list_of_event_ids
        }

      this->i_cut::_set_initialized (true);
    }

    int event_header_cut::_accept()
    {
      uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has (_EH_label_))
        {
          DT_LOG_DEBUG (get_logging_priority (), "Event record has no '" << _EH_label_ << "' bank !");
          return cut_returned;
        }

      // Get event header bank
      const snemo::datamodel::event_header & EH = ER.get<snemo::datamodel::event_header> (_EH_label_);

      // Check if the event header has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag ()) {
        DT_LOG_DEBUG (get_logging_priority (), "Running FLAG mode...");
        const bool check = EH.get_properties ().has_flag (_flag_name_);
        if (! check)
          {
            check_flag = false;
          }
      }

      // Check if the event ID has a run number within some interval :
      bool check_run_number = true;
      if (is_mode_run_number ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running RUN_NUMBER mode...");
          if (! EH.get_id ().is_valid ())
            {
              return cut_returned;
            }
          const int rn = EH.get_id ().get_run_number ();
          bool check = true;
          if (_run_number_min_ >= 0)
            {
              if (rn < _run_number_min_) check = false;
            }
          if (_run_number_max_ >= 0)
            {
              if (rn > _run_number_max_) check = false;
            }
          if (! check)
            {
              check_run_number = false;
            }
          if (check_run_number)
            {
              DT_LOG_DEBUG (get_logging_priority (),
                            "RUN_NUMBER mode : (" << _run_number_min_ << " <= "
                            << rn << " <= " << _run_number_max_ << ") ACCEPTED !");
            }
          else
            {
              DT_LOG_DEBUG (get_logging_priority (),
                            "RUN_NUMBER mode : " << rn << " is NOT in [" << _run_number_min_ << ";"
                            << _run_number_max_ << "] REJECTED !");
            }
        }

      // Check if the event ID has a event number within some interval :
      bool check_event_number = true;
      if (is_mode_event_number ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running EVENT_NUMBER mode...");
          if (! EH.get_id ().is_valid ())
            {
              return cut_returned;
            }
          const int en = EH.get_id ().get_event_number ();
          bool check = true;
          if (_event_number_min_ >= 0)
            {
              if (en < _event_number_min_) check = false;
            }
          if (_event_number_max_ >= 0)
            {
              if (en > _event_number_max_) check = false;
            }
          if (! check)
            {
              check_event_number = false;
            }
          if (check_event_number)
            {
              DT_LOG_DEBUG (get_logging_priority (),
                            "EVENT_NUMBER mode : (" << _event_number_min_ << " <= "
                            << en << " <= " << _event_number_max_ << ") ACCEPTED !");
            }
          else
            {
              DT_LOG_DEBUG (get_logging_priority (),
                            "EVENT_NUMBER mode : " << en << " is NOT in [" << _event_number_min_ << ";"
                            << _event_number_max_ << "] REJECTED !");
            }
        }

      // Check if the event ID belongs to a given list :
      bool check_list_of_events = true;
      if (is_mode_list_of_event_ids ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running LIST_OF_EVENT_IDS mode...");
          if (! EH.get_id ().is_valid ())
            {
              return cut_returned;
            }
          std::set<datatools::event_id>::const_iterator found =
            _list_of_events_ids_.find (EH.get_id ());
          bool check = true;
          if (found == _list_of_events_ids_.end ())
            {
              check = false;
            }
          if (! check)
            {
              check_list_of_events = false;
            }
          if (check_list_of_events)
            {
              DT_LOG_DEBUG (get_logging_priority (),
                            "LIST_OF_EVENT_IDS mode : EID=" << EH.get_id () << " belongs to the list! ACCEPTED !");
            }
          else
            {
              DT_LOG_DEBUG (get_logging_priority (),
                            "LIST_OF_EVENT_IDS mode : EID=" << EH.get_id () << " does NOT belongs to the list! REJECTED !");
            }
        }

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag && check_run_number && check_event_number && check_list_of_events)
        {
          cut_returned = cuts::SELECTION_ACCEPTED;
        }

      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

// OCD support:

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::event_header_cut, ocd_)
{
  ocd_.set_class_name("snemo::cut::event_header_cut");
  ocd_.set_class_description("Cut based on criteria applied to the event header bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  {
    // Description of the 'EH_label' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("EH_label")
      .set_terse_description("The name of the Event Header bank")
      .set_traits(datatools::TYPE_STRING)
      .set_default_value_string("EH")
      .add_example("Set the default value::                          \n"
                   "                                                 \n"
                   "  EH_label : string = \"EH\"                     \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'mode.flag' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.flag")
      .set_terse_description("Mode with a special request flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested flag mode::               \n"
                   "                                                 \n"
                   "  mode.flag : boolean = true                     \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'mode.run_number' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.run_number")
      .set_terse_description("Mode requesting some specific run numbers")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested run number selection mode:: \n"
                   "                                                 \n"
                   "  mode.run_number : boolean = true               \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'mode.event_number' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.event_number")
      .set_terse_description("Mode requesting some specific event numbers")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested event number selection mode:: \n"
                   "                                                     \n"
                   "  mode.event_number : boolean = true                 \n"
                   "                                                     \n"
                   )
      ;
  }

  {
    // Description of the 'mode.list_of_event_ids' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.list_of_event_ids")
      .set_terse_description("Mode requesting a specific list of event identifiers")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested list of event IDs selection mode:: \n"
                   "                                                          \n"
                   "  mode.list_of_event_ids : boolean = true                 \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'flag.name' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("flag.name")
      .set_terse_description("Name of the requested flag")
      .set_triggered_by_flag("mode.flag")
      .set_traits(datatools::TYPE_STRING)
      .add_example("Set a specific requested flag name::                      \n"
                   "                                                          \n"
                   "  flag.name : string = \"high_energy\"                    \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'run_number.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("run_number.min")
      .set_terse_description("Minimum run number")
      .set_triggered_by_flag("mode.run_number")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific minimum run number::                       \n"
                   "                                                          \n"
                   "  run_number.min : integer = 25                           \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'run_number.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("run_number.max")
      .set_terse_description("Maximum run number")
      .set_triggered_by_flag("mode.run_number")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific maximum run number:: \n"
                   "                                                          \n"
                   "  run_number.max : integer = 34                           \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'event_number.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("event_number.min")
      .set_terse_description("Minimum event number")
      .set_traits(datatools::TYPE_INTEGER)
      .set_triggered_by_flag("mode.event_number")
      .add_example("Set a specific minimum event number:: \n"
                   "                                                          \n"
                   "  event_number.min : integer = 0                          \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'event_number.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("event_number.max")
      .set_terse_description("Maximum event number")
      .set_traits(datatools::TYPE_INTEGER)
      .set_triggered_by_flag("mode.event_number")
      .add_example("Set a specific maximum event number:: \n"
                   "                                                          \n"
                   "  event_number.max : integer = 1000                       \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'list_of_event_ids.ids' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("list_of_event_ids.ids")
      .set_terse_description("Explicit list of event IDs")
      .set_triggered_by_flag("mode.list_of_event_ids")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .add_example("Set a specific list of event IDs:: \n"
                   "                                                          \n"
                   "  list_of_event_ids.ids : string[5] = \\                  \n"
                   "     \"234_0\" \\                                         \n"
                   "     \"234_1\" \\                                         \n"
                   "     \"234_5\" \\                                         \n"
                   "     \"241_2\" \\                                         \n"
                   "     \"241_3\"                                            \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'list_of_event_ids.file' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("list_of_event_ids.file")
      .set_terse_description("Set the name of the file containing an explicit list of event IDs")
      .set_triggered_by_flag("mode.list_of_event_ids")
      .set_traits(datatools::TYPE_STRING)
      .add_example("Set a specific file of event IDs:: \n"
                   "                                                          \n"
                   "  list_of_event_ids.file : string = \"my_list.lis\"       \n"
                   "                                                          \n"
                   )
      ;
  }

  // Additional configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the     \n"
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
                               "                                                \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::cut::event_header_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::event_header_cut, "snemo::cut::event_header_cut")



/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
