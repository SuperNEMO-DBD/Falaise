// falaise/snemo/cuts/simulated_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/simulated_data_cut.h>

/// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(simulated_data_cut, "snemo::cut::simulated_data_cut");

    void simulated_data_cut::_set_defaults ()
    {
      _SD_label_ = "";
      _mode_     = MODE_UNDEFINED;
      _flag_name_    = "";
      _hit_category_ = "";
      _hit_category_range_min_ = -1;
      _hit_category_range_max_ = -1;
      _hit_property_logic_ = "";
      return;
    }

    void simulated_data_cut::set_SD_label (const std::string & SD_label_)
    {
      _SD_label_ = SD_label_;
      return;
    }

    const std::string & simulated_data_cut::get_SD_label () const
    {
      return _SD_label_;
    }


    uint32_t simulated_data_cut::get_mode () const
    {
      return _mode_;
    }

    bool simulated_data_cut::is_mode_flag () const
    {
      return _mode_ & MODE_FLAG;
    }

    bool simulated_data_cut::is_mode_range_hit_category () const
    {
      return _mode_ & MODE_RANGE_HIT_CATEGORY;
    }

    bool simulated_data_cut::is_mode_has_hit_category () const
    {
      return _mode_ & MODE_HAS_HIT_CATEGORY;
    }

    bool simulated_data_cut::is_mode_has_hit_property () const
    {
      return _mode_ & MODE_HAS_HIT_PROPERTY;
    }

    void simulated_data_cut::set_flag_name (const std::string & flag_name_)
    {
      _flag_name_ = flag_name_;
      return;
    }

    const std::string & simulated_data_cut::get_flag_name () const
    {
      return _flag_name_;
    }

    simulated_data_cut::simulated_data_cut (datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults ();
      return;
    }

    simulated_data_cut::~simulated_data_cut ()
    {
      if (is_initialized()) this->simulated_data_cut::reset();
      return;
    }

    void simulated_data_cut::reset ()
    {
      _set_defaults ();
      this->i_cut::_reset ();
      this->i_cut::_set_initialized (false);
      return;
    }

    void simulated_data_cut::initialize (const datatools::properties & configuration_,
                                         datatools::service_manager  & /* service_manager_ */,
                                         cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cut '" << get_name () << "' is already initialized ! ");

      this->i_cut::_common_initialize (configuration_);

      if (_SD_label_.empty ())
        {
          DT_THROW_IF (! configuration_.has_key ("SD_label"), std::logic_error,
                       "Missing 'SD_label' property !");
          set_SD_label (configuration_.fetch_string ("SD_label"));
        }

      if (_mode_ == MODE_UNDEFINED)
        {
          if (configuration_.has_flag ("mode.flag"))
            {
              _mode_ |= MODE_FLAG;
            }

          if (configuration_.has_flag ("mode.has_hit_category"))
            {
              _mode_ |= MODE_HAS_HIT_CATEGORY;
            }

          if (configuration_.has_flag ("mode.range_hit_category"))
            {
              _mode_ |= MODE_RANGE_HIT_CATEGORY;
            }

          if (configuration_.has_flag ("mode.has_hit_property"))
            {
              _mode_ |= MODE_HAS_HIT_PROPERTY;
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

          // mode HAS_HIT_CATEGORY:
          if (is_mode_has_hit_category ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using HAS_HIT_CATEGORY mode...");
              DT_THROW_IF (! configuration_.has_key ("has_hit_category.category"),
                           std::logic_error,
                           "Missing 'has_hit_category.category' property !");
              _hit_category_ = configuration_.fetch_string ("has_hit_category.category");
            } // end if is_mode_has_hit_category

          // mode RANGE_HIT_CATEGORY:
          if (is_mode_range_hit_category ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using RANGE_HIT_CATEGORY mode...");
              DT_THROW_IF (! configuration_.has_key ("range_hit_category.category"),
                           std::logic_error,
                           "Missing 'range_hit_category.category' property !");
              _hit_category_ = configuration_.fetch_string ("range_hit_category.category");

              int count = 0;
              if (configuration_.has_key ("range_hit_category.min"))
                {
                  const int nmin = configuration_.fetch_integer ("range_hit_category.min");
                  DT_THROW_IF (nmin < 0, std::logic_error,
                               "Invalid min number of hits (" << nmin << ") !");
                  _hit_category_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key ("range_hit_category.max"))
                {
                  const int nmax = configuration_.fetch_integer ("range_hit_category.max");
                  DT_THROW_IF (nmax < 0, std::logic_error,
                               "Invalid max number of hits (" << nmax << ") !");
                  _hit_category_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF (count == 0, std::logic_error,
                           "Missing 'range_hit_category.min' or 'range_hit_category.max' property !");
              if (count == 2 && _hit_category_range_min_ >= 0 && _hit_category_range_max_ >= 0)
                {
                  DT_THROW_IF (_hit_category_range_min_ > _hit_category_range_max_,
                               std::logic_error,
                               "Invalid 'range_hit_category.min' > 'range_hit_category.max' values !");
                }
            } // end if is_mode_range_hit_category

          if (is_mode_has_hit_property ())
            {
              DT_LOG_DEBUG (get_logging_priority (), "Using HAS_HIT_PROPERTY mode...");
              DT_THROW_IF (! configuration_.has_key ("has_hit_property.category"),
                           std::logic_error,
                           "Missing 'has_hit_property.category' property !");
              _hit_category_ = configuration_.fetch_string ("has_hit_property.category");

              DT_THROW_IF (! configuration_.has_key ("has_hit_property.keys"),
                           std::logic_error,
                           "Missing 'has_hit_property.keys' property !");
              std::vector<std::string> keys;
              configuration_.fetch ("has_hit_property.keys", keys);

              // Look for the logical relation between properties
              if (keys.size () > 1)
                {
                  DT_THROW_IF (! configuration_.has_key ("has_hit_property.logic"),
                               std::logic_error,
                               "Missing 'has_hit_property.logic' property !");
                  _hit_property_logic_ = configuration_.fetch_string ("has_hit_property.logic");
                  DT_THROW_IF (_hit_property_logic_ != "and" && _hit_property_logic_ != "or",
                               std::logic_error,
                               "Logic operation '" << _hit_property_logic_ << "' is unkown or not supported!");
                }

              for (std::vector<std::string>::const_iterator ikey = keys.begin ();
                   ikey != keys.end (); ++ikey)
                {
                  const std::string check_key = "has_hit_property." + *ikey + ".values";
                  DT_THROW_IF (! configuration_.has_key (check_key), std::logic_error,
                               "Missing '" << check_key << "' property !");
                  // 2012-11-25 XG: It is maybe better to store things in an
                  // 'unordered' way (thus store property_keys in a
                  // independent list) to let the user define the best way to
                  // look for property keys. For example, it is more efficient
                  // to look for a given 'creator_process' and then
                  // 'creator_category' since there can be a lot of hits
                  // fulfilling 'creator_category' condition and only few for
                  // 'creator_process'.
                  configuration_.fetch (check_key, _hit_property_values_[*ikey]);
                }
            } // end if is_mode_hit_has_property
        }

      this->i_cut::_set_initialized (true);
      return;
    }


    int simulated_data_cut::_accept ()
    {
      int cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has (_SD_label_))
        {
          DT_LOG_DEBUG (get_logging_priority (), "Event record has no '" << _SD_label_ << "' bank !");
          return cut_returned;
        }

      // Get simulated data bank
      const mctools::simulated_data & SD = ER.get<mctools::simulated_data> (_SD_label_);

      // Check if the simulated data has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running FLAG mode...");
          const bool check = SD.get_properties ().has_flag (_flag_name_);
          if (! check)
            {
              check_flag = false;
            }
        }

      // Check if the simulated data has some specific category of hits :
      bool check_has_hit_category = true;
      if (is_mode_has_hit_category ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running HAS_HIT_CATEGORY mode...");
          const bool check = SD.has_step_hits (_hit_category_);
          if (! check)
            {
              check_has_hit_category = false;
            }
        }

      // Check if the simulated data has some specific category of hits :
      bool check_range_hit_category = true;
      if (is_mode_range_hit_category ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running RANGE_HIT_CATEGORY mode...");
          if (!SD.has_step_hits (_hit_category_))
            {
              DT_LOG_DEBUG (get_logging_priority (), "Simulated data has no step hit from '"
                            << _hit_category_ << "' category");
              return cuts::SELECTION_INAPPLICABLE;
            }
          const size_t nhits = SD.get_number_of_step_hits (_hit_category_);
          bool check = true;
          if (_hit_category_range_min_ >= 0)
            {
              if (nhits < (size_t)_hit_category_range_min_)
                {
                  check = false;
                }
            }
          if (_hit_category_range_max_ >= 0)
            {
              if (nhits > (size_t)_hit_category_range_max_)
                {
                  check = false;
                }
            }
          if (! check)
            {
              check_range_hit_category = false;
            }
        }

      // Check if the simulated step hits have some property value :
      bool check_has_hit_property = true;
      if (is_mode_has_hit_property ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running HAS_HIT_PROPERTY mode...");
          if (!SD.has_step_hits (_hit_category_))
            {
              DT_LOG_DEBUG (get_logging_priority (), "Simulated data has no step hit from '"
                            << _hit_category_ << "' category");
              return cuts::SELECTION_INAPPLICABLE;
            }
          const mctools::simulated_data::hit_handle_collection_type & the_step_hits
            = SD.get_step_hits (_hit_category_);

          // Iterators
          mctools::simulated_data::hit_handle_collection_type::const_iterator
            istart = the_step_hits.begin ();
          mctools::simulated_data::hit_handle_collection_type::const_iterator
            istop = the_step_hits.end ();
          property_values_dict_type::const_iterator iprop = _hit_property_values_.begin ();

          while (iprop != _hit_property_values_.end ())
            {
              const std::string & a_key = iprop->first;
              const std::vector<std::string> & the_values = iprop->second;

              geomtools::base_hit::has_string_property_predicate str_pred (a_key, the_values);
              // wrapper predicates :
              datatools::mother_to_daughter_predicate<geomtools::base_hit, mctools::base_step_hit> pred (str_pred);
              datatools::handle_predicate<mctools::base_step_hit> pred_via_handle (pred);

              // Update iterator position
              mctools::simulated_data::hit_handle_collection_type::const_iterator
                ifound = std::find_if (istart, istop, pred_via_handle);
              if (ifound == the_step_hits.end ())
                {
                  check_has_hit_property = false;
                  if (_hit_property_logic_ == "and") break;
                  // Go to the next property (OR mode)
                  istart = the_step_hits.begin ();
                  istop  = the_step_hits.end ();
                  iprop++;
                }
              else if (ifound == istop)
                {
                  // This hit does not fulfill requirement : restart to first
                  // property but at different iterator position
                  istart = ++ifound;
                  istop  = the_step_hits.end ();
                  iprop = _hit_property_values_.begin ();
                }
              else
                {
                  // Found one hit : check next property for this hit
                  check_has_hit_property = true;
                  if (_hit_property_logic_ == "or") break;
                  istart = ifound;
                  istop  = ++ifound;
                  iprop++;
                }
            }
        }

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag               &&
          check_has_hit_category   &&
          check_range_hit_category &&
          check_has_hit_property)
        {
          cut_returned = cuts::SELECTION_ACCEPTED;
        }

      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
