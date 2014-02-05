// -*- mode: c++ ; -*-
/** \file falaise/snemo/cuts/event_header_cut.cc
 */

// Ourselves:
#include <falaise/snemo/cuts/event_header_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>

// This project :
#include <falaise/snemo/datamodels/event_header.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(event_header_cut, "snemo::cut::event_header_cut");

    void event_header_cut::_set_defaults ()
    {
      _EH_label_         = "";
      _mode_             = MODE_UNDEFINED;
      _flag_name_        = "";
      _run_number_min_   = -1;
      _run_number_max_   = -1;
      _event_number_min_ = -1;
      _event_number_max_ = -1;
      return;
    }

    void event_header_cut::set_EH_label (const std::string & EH_label_)
    {
      _EH_label_ = EH_label_;
      return;
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
      return;
    }

    const std::string & event_header_cut::get_flag_name () const
    {
      return _flag_name_;
    }

    void event_header_cut::set_run_number_min (int run_number_min_)
    {
      _run_number_min_ = run_number_min_ >= 0 ? run_number_min_ : -1;
      return;
    }

    void event_header_cut::set_run_number_max (int run_number_max_)
    {
      _run_number_max_ = run_number_max_ >= 0 ? run_number_max_ : -1;
      return;
    }

    void event_header_cut::set_event_number_min (int event_number_min_)
    {
      _event_number_min_ = event_number_min_ >= 0 ? event_number_min_ : -1;
      return;
    }

    void event_header_cut::set_event_number_max (int event_number_max_)
    {
      _event_number_max_ = event_number_max_ >= 0 ? event_number_max_ : -1;
      return;
    }

    void event_header_cut::list_of_event_ids_dump (std::ostream & out_) const
    {
      for (std::set<datatools::event_id>::const_iterator
             i = _list_of_events_ids_.begin ();
           i != _list_of_events_ids_.end ();
           i++)
        {
          out_ << *i << std::endl;
        }
      return;
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
                  datatools::event_id the_event_id;
                  iss2 >> std::ws >> the_event_id;
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
      return;
    }

    event_header_cut::event_header_cut (datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults ();
      return;
    }

    event_header_cut::~event_header_cut ()
    {
      if (is_initialized()) this->event_header_cut::reset();
      return;
    }

    void event_header_cut::reset ()
    {
      _set_defaults ();
      this->i_cut::_reset ();
      this->i_cut::_set_initialized (false);
      return;
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
                  for (size_t i = 0; i < str_ids.size (); i++)
                    {
                      std::istringstream id_iss (str_ids[i]);
                      datatools::event_id the_event_id;
                      id_iss >> the_event_id;
                      DT_THROW_IF (! id_iss, std::logic_error,
                                   "Invalid format for event ID ('" << str_ids[i] << "') !");
                      _list_of_events_ids_.insert (the_event_id);
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
                           "Missing 'list_of_event_ids.ids' or 'list_of_event_ids.file_list' property !");
              DT_LOG_DEBUG (get_logging_priority (), "Dumping list of event id:");
              if (is_debug ())
                {
                  list_of_event_ids_dump (std::clog);
                }
            } // end if is_mode_list_of_event_ids
        }

      this->i_cut::_set_initialized (true);
      return;
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
      if (is_mode_event_number ())
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

// end of falaise/snemo/cuts/event_header_cut.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
