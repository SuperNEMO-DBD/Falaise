// falaise/snemo/cuts/calibrated_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/calibrated_data_cut.h>

/// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>
// - Bayeux/geomtools:
#include <geomtools/clhep.h>

// This project :
#include <falaise/snemo/datamodels/calibrated_data.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(calibrated_data_cut, "snemo::cut::calibrated_data_cut");

    void calibrated_data_cut::_set_defaults ()
    {
      _CD_label_ = "";
      _mode_     = MODE_UNDEFINED;
      _flag_name_    = "";
      _hit_category_ = "";
      _hit_category_range_min_ = -1;
      _hit_category_range_max_ = -1;
      _tracker_hit_trait_bits_ = snemo::datamodel::calibrated_tracker_hit::none;
      _tracker_hit_delayed_time_ = 0.0 * CLHEP::microsecond;
      return;
    }

    void calibrated_data_cut::set_CD_label (const std::string & CD_label_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cut '" << get_name () << "' is already initialized ! ");
      _CD_label_ = CD_label_;
      return;
    }

    const std::string & calibrated_data_cut::get_CD_label () const
    {
      return _CD_label_;
    }


    uint32_t calibrated_data_cut::get_mode () const
    {
      return _mode_;
    }

    bool calibrated_data_cut::is_mode_flag () const
    {
      return _mode_ & MODE_FLAG;
    }

    bool calibrated_data_cut::is_mode_range_hit_category () const
    {
      return _mode_ & MODE_RANGE_HIT_CATEGORY;
    }

    bool calibrated_data_cut::is_mode_has_hit_category () const
    {
      return _mode_ & MODE_HAS_HIT_CATEGORY;
    }

    bool calibrated_data_cut::is_mode_tracker_hit_has_traits () const
    {
      return _mode_ & MODE_TRACKER_HIT_HAS_TRAITS;
    }

    bool calibrated_data_cut::is_mode_tracker_hit_is_delayed () const
    {
      return _mode_ & MODE_TRACKER_HIT_IS_DELAYED;
    }

    void calibrated_data_cut::set_flag_name (const std::string & flag_name_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cut '" << get_name () << "' is already initialized ! ");
      _flag_name_ = flag_name_;
      return;
    }

    const std::string & calibrated_data_cut::get_flag_name () const
    {
      return _flag_name_;
    }

    calibrated_data_cut::calibrated_data_cut (datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults ();
      return;
    }

    calibrated_data_cut::~calibrated_data_cut ()
    {
      if (is_initialized()) this->calibrated_data_cut::reset();
      return;
    }

    void calibrated_data_cut::reset ()
    {
      _set_defaults ();
      this->i_cut::_reset ();
      this->i_cut::_set_initialized (false);
      return;
    }

    void calibrated_data_cut::initialize (const datatools::properties & configuration_,
                                          datatools::service_manager  & /* service_manager_ */,
                                          cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cut '" << get_name () << "' is already initialized ! ");

      this->i_cut::_common_initialize (configuration_);

      if (_CD_label_.empty ()) {
        DT_THROW_IF(! configuration_.has_key ("CD_label"), std::logic_error,
                    "Missing 'CD_label' property !");
        set_CD_label (configuration_.fetch_string ("CD_label"));
      }


      if (_mode_ == MODE_UNDEFINED) {
        if (configuration_.has_flag ("mode.flag")) {
          _mode_ |= MODE_FLAG;
        }

        if (configuration_.has_flag ("mode.has_hit_category")) {
          _mode_ |= MODE_HAS_HIT_CATEGORY;
        }

        if (configuration_.has_flag ("mode.range_hit_category")) {
          _mode_ |= MODE_RANGE_HIT_CATEGORY;
        }

        if (configuration_.has_flag ("mode.tracker_hit_has_traits")) {
          _mode_ |= MODE_TRACKER_HIT_HAS_TRAITS;
        }

        if (configuration_.has_flag ("mode.tracker_hit_is_delayed")) {
          _mode_ |= MODE_TRACKER_HIT_IS_DELAYED;
        }

        DT_THROW_IF (_mode_ == MODE_UNDEFINED, std::logic_error,
                     "Missing at least a 'mode.XXX' property ! ");

        // mode FLAG:
        if (is_mode_flag ()) {
          DT_LOG_DEBUG (get_logging_priority (), "Using FLAG mode...");
          DT_THROW_IF (! configuration_.has_key ("flag.name"),
                       std::logic_error,
                       "Missing 'flag.name' property !");
          set_flag_name (configuration_.fetch_string ("flag.name"));
        } // end if is_mode_flag

        // mode HAS_HIT_CATEGORY:
        if (is_mode_has_hit_category ()) {
          DT_LOG_DEBUG (get_logging_priority (), "Using HAS_HIT_CATEGORY mode...");
          DT_THROW_IF (! configuration_.has_key ("has_hit_category.category"),
                       std::logic_error,
                       "Missing 'has_hit_category.category' property !");
          _hit_category_ = configuration_.fetch_string ("has_hit_category.category");

          DT_THROW_IF (_hit_category_ != "tracker" && _hit_category_ != "calorimeter",
                       std::logic_error,
                       "Property '" << _hit_category_ << "' for "
                       << "'has_hit_category.category' is not impemented ! "
                       << "Use either 'tracker' or 'calorimeter' category !");
        } // end if is_mode_has_hit_category


        // mode RANGE_HIT_CATEGORY:
        if (is_mode_range_hit_category ()) {
          DT_LOG_DEBUG (get_logging_priority (), "Using RANGE_HIT_CATEGORY mode...");
          DT_THROW_IF (! configuration_.has_key ("range_hit_category.category"),
                       std::logic_error,
                       "Missing 'range_hit_category.category' property !");
          _hit_category_ = configuration_.fetch_string ("range_hit_category.category");

          DT_THROW_IF (_hit_category_ != "tracker" && _hit_category_ != "calorimeter",
                       std::logic_error,
                       "Property '" << _hit_category_ << "' for "
                       << "'range_hit_category.category' is not impemented ! "
                       << "Use either 'tracker' or 'calorimeter' category !");

          int count = 0;
          if (configuration_.has_key ("range_hit_category.min")) {
            const int nmin = configuration_.fetch_integer ("range_hit_category.min");
            DT_THROW_IF (nmin < 0, std::logic_error, "Invalid min number of hits (" << nmin << ") !");
            _hit_category_range_min_ = nmin;
            count++;
          }
          if (configuration_.has_key ("range_hit_category.max")) {
            const int nmax = configuration_.fetch_integer ("range_hit_category.max");
            DT_THROW_IF (nmax < 0, std::logic_error, "Invalid max number of hits (" << nmax << ") !");
            _hit_category_range_max_ = nmax;
            count++;
          }
          DT_THROW_IF (count == 0,
                       std::logic_error,
                       "Missing 'range_hit_category.min' or 'range_hit_category.max' property !");
          if (count == 2 && _hit_category_range_min_ >= 0 && _hit_category_range_max_ >= 0) {
            DT_THROW_IF (_hit_category_range_min_ > _hit_category_range_max_,
                         std::logic_error,
                         "Invalid 'range_hit_category.min' > 'range_hit_category.max' values !");
          }
        } // end if is_mode_range_hit_category

        // mode TRACKER_HIT_HAS_TRAITS:
        if (is_mode_tracker_hit_has_traits()) {
          DT_LOG_DEBUG (get_logging_priority (), "Using TRACKER_HIT_HAS_TRAITS mode...");
          DT_THROW_IF (! configuration_.has_key ("tracker_hit_has_traits.bits_name"),
                       std::logic_error,
                       "Missing 'tracker_hit_has_traits.bits_name' property !");
          std::vector<std::string> bits_name;
          configuration_.fetch("tracker_hit_has_traits.bits_name", bits_name);
          for (size_t i = 0; i < bits_name.size(); ++i) {
            const std::string & bit_name = bits_name[i];
            if (bit_name == "delayed") {
              _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::delayed;
            } else if (bit_name == "noisy") {
              _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::noisy;
            } else if (bit_name == "missing_bottom_cathode") {
              _tracker_hit_trait_bits_ |= snemo::datamodel::calibrated_tracker_hit::missing_bottom_cathode;
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
        } // end of is_mode_tracker_hit_has_traits

        // mode TRACKER_HIT_IS_DELAYED:
        if (is_mode_tracker_hit_is_delayed()) {
          DT_LOG_DEBUG (get_logging_priority (), "Using TRACKER_HIT_IS_DELAYED mode...");
          if (configuration_.has_key("tracker_hit_is_delayed.delayed_time")) {
            _tracker_hit_delayed_time_ = configuration_.fetch_real("tracker_hit_is_delayed.delayed_time");
            if (! configuration_.has_explicit_unit("tracker_hit_is_delayed.delayed_time")) {
              _tracker_hit_delayed_time_ *= CLHEP::microsecond;
            }
          }
        }// end of is_mode_tracker_hit_is_delayed
      }

      this->i_cut::_set_initialized (true);
      return;
    }


    int calibrated_data_cut::_accept ()
    {
      uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has (_CD_label_))
        {
          DT_LOG_DEBUG (get_logging_priority (), "Event record has no '" << _CD_label_ << "' bank !");
          return cut_returned;
        }

      // Get calibrated data bank
      const snemo::datamodel::calibrated_data & CD = ER.get<snemo::datamodel::calibrated_data> (_CD_label_);

      // Check if the calibrated data has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag ())
        {
          DT_LOG_DEBUG (get_logging_priority (), "Running FLAG mode...");
          const bool check = CD.get_properties ().has_flag (_flag_name_);
          if (! check)
            {
              check_flag = false;
            }
        }

      // Check if the calibrated data has some specific calibrated hits :
      bool check_has_hit_category = true;
      if (is_mode_has_hit_category ()) {
        DT_LOG_DEBUG (get_logging_priority (), "Running HAS_HIT_CATEGORY mode...");
        bool check = true;
        if (_hit_category_ == "tracker") {
          check = CD.has_calibrated_tracker_hits ();
        } else if (_hit_category_ == "calorimeter") {
          check = CD.has_calibrated_calorimeter_hits ();
        }
        if (! check) {
          check_has_hit_category = false;
        }
      }

      // Check if the calibrated data has some specific category of hits :
      bool check_range_hit_category = true;
      if (is_mode_range_hit_category ()) {
        DT_LOG_DEBUG (get_logging_priority (), "Running RANGE_HIT_CATEGORY mode...");
        bool check_has_hit = true;
        if (_hit_category_ == "tracker") {
          check_has_hit = CD.has_calibrated_tracker_hits ();
        } else if (_hit_category_ == "calorimeter") {
          check_has_hit = CD.has_calibrated_calorimeter_hits ();
        } if (!check_has_hit) {
          DT_LOG_DEBUG (get_logging_priority (), "Calibrated data has no calibrated "
                        << _hit_category_ << " hit !");
          return cuts::SELECTION_INAPPLICABLE;
        }

        size_t nhits = 0;
        if (_hit_category_ == "tracker") {
          nhits = CD.calibrated_tracker_hits ().size ();
        } else if (_hit_category_ == "calorimeter") {
          nhits = CD.calibrated_calorimeter_hits ().size ();
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
        if (! check) {
          check_range_hit_category = false;
        }
      }

      // Check if the calibrated tracker hits have some specific trait bits :
      bool check_tracker_hit_has_traits = true;
      if (is_mode_tracker_hit_has_traits()) {
        DT_LOG_DEBUG (get_logging_priority (), "Running TRACKER_HIT_HAS_TRAITS mode...");
        if (!CD.has_calibrated_tracker_hits()) {
          DT_LOG_DEBUG (get_logging_priority (), "Calibrated data has no calibrated tracker hit !");
          return cuts::SELECTION_INAPPLICABLE;
        }

        check_tracker_hit_has_traits = false;
        const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits
          = CD.calibrated_tracker_hits();
        for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::const_iterator
               ihit = hits.begin(); ihit != hits.end(); ++ihit) {
          const snemo::datamodel::calibrated_tracker_hit & a_hit = ihit->get();
          if (a_hit.get_trait_bit(_tracker_hit_trait_bits_)) {
            check_tracker_hit_has_traits = true;
            break;
          }
        }
      }

      // Check if the calibrated tracker hits have delayed cells :
      bool check_tracker_hit_is_delayed = true;
      if (is_mode_tracker_hit_is_delayed()) {
        DT_LOG_DEBUG (get_logging_priority (), "Running TRACKER_HIT_IS_DELAYED mode...");
        if (!CD.has_calibrated_tracker_hits()) {
          DT_LOG_DEBUG (get_logging_priority (), "Calibrated data has no calibrated tracker hit !");
          return cuts::SELECTION_INAPPLICABLE;
        }

        check_tracker_hit_is_delayed = false;
        const snemo::datamodel::calibrated_data::tracker_hit_collection_type & hits
          = CD.calibrated_tracker_hits();
        for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::const_iterator
               ihit = hits.begin(); ihit != hits.end(); ++ihit) {
          const snemo::datamodel::calibrated_tracker_hit & a_hit = ihit->get();
          if (a_hit.is_delayed() && a_hit.get_delayed_time() > _tracker_hit_delayed_time_) {
            check_tracker_hit_is_delayed = true;
            break;
          }
        }
      }

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag               &&
          check_has_hit_category   &&
          check_range_hit_category &&
          check_tracker_hit_has_traits &&
          check_tracker_hit_is_delayed) {
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
