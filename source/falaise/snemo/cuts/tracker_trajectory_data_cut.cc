// falaise/snemo/cuts/tracker_trajectory_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/tracker_trajectory_data_cut.h>

/// Standard library:
#include <stdexcept>

// Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>
// SuperNEMO data models :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(tracker_trajectory_data_cut, "snemo::cut::tracker_trajectory_data_cut");

    void tracker_trajectory_data_cut::_set_defaults()
    {
      _TTD_label_         = "";
      _mode_              = MODE_UNDEFINED;
      _flag_name_         = "";
      _chi2ndf_range_min_ = datatools::invalid_real();
      _chi2ndf_range_max_ = datatools::invalid_real();
      _pvalue_range_min_  = datatools::invalid_real();
      _pvalue_range_max_  = datatools::invalid_real();
      return;
    }

    void tracker_trajectory_data_cut::set_TTD_label(const std::string & TTD_label_)
    {
      _TTD_label_ = TTD_label_;
      return;
    }

    const std::string & tracker_trajectory_data_cut::get_TTD_label() const
    {
      return _TTD_label_;
    }

    uint32_t tracker_trajectory_data_cut::get_mode() const
    {
      return _mode_;
    }

    bool tracker_trajectory_data_cut::is_mode_flag() const
    {
      return _mode_ & MODE_FLAG;
    }

    bool tracker_trajectory_data_cut::is_mode_has_solution() const
    {
      return _mode_ & MODE_HAS_SOLUTION;
    }

    bool tracker_trajectory_data_cut::is_mode_range_chi2ndf() const
    {
      return _mode_ & MODE_RANGE_CHI2NDF;
    }

    bool tracker_trajectory_data_cut::is_mode_range_pvalue() const
    {
      return _mode_ & MODE_RANGE_PVALUE;
    }

    void tracker_trajectory_data_cut::set_flag_name(const std::string & flag_name_)
    {
      _flag_name_ = flag_name_;
      return;
    }

    const std::string & tracker_trajectory_data_cut::get_flag_name() const
    {
      return _flag_name_;
    }

    tracker_trajectory_data_cut::tracker_trajectory_data_cut(datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults ();
      return;
    }

    tracker_trajectory_data_cut::~tracker_trajectory_data_cut()
    {
      if (is_initialized()) this->tracker_trajectory_data_cut::reset();
      return;
    }

    void tracker_trajectory_data_cut::reset ()
    {
      _set_defaults();
      this->i_cut::_reset();
      this->i_cut::_set_initialized(false);
      return;
    }

    void tracker_trajectory_data_cut::initialize(const datatools::properties & configuration_,
                                                 datatools::service_manager  & /* service_manager_ */,
                                                 cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Cut '" << get_name() << "' is already initialized ! ");

      this->i_cut::_common_initialize(configuration_);

      if (_TTD_label_.empty()) {
        if (configuration_.has_key("TTD_label")) {
          set_TTD_label(configuration_.fetch_string("TTD_label"));
        } else {
          set_TTD_label(snemo::datamodel::data_info::default_tracker_trajectory_data_label());
        }
      }

      if (_mode_ == MODE_UNDEFINED) {
        if (configuration_.has_flag("mode.flag")) {
          _mode_ |= MODE_FLAG;
        }

        if (configuration_.has_flag("mode.has_solution")) {
          _mode_ |= MODE_HAS_SOLUTION;
        }

        if (configuration_.has_flag("mode.range_chi2ndf")) {
          _mode_ |= MODE_RANGE_CHI2NDF;
        }

        if (configuration_.has_flag("mode.range_pvalue")) {
          _mode_ |= MODE_RANGE_PVALUE;
        }

        DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error,
                    "Missing at least a 'mode.XXX' property ! ");

        // mode FLAG:
        if (is_mode_flag()) {
          DT_LOG_DEBUG(get_logging_priority (), "Using FLAG mode...");
          DT_THROW_IF(! configuration_.has_key("flag.name"),
                      std::logic_error,
                      "Missing 'flag.name' property !");
          set_flag_name(configuration_.fetch_string("flag.name"));
        } // end if is_mode_flag

        // mode HAS_SOLUTION:
        if (is_mode_has_solution()) {
          DT_LOG_DEBUG(get_logging_priority (), "Using HAS_SOLUTION mode...");
        } // end if is_mode_has_solution

        // mode RANGE_CHI2NDF:
        if (is_mode_range_chi2ndf()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_CHI2NDF mode...");
          int count = 0;
          if (configuration_.has_key("range_chi2ndf.min")) {
            double min = configuration_.fetch_real("range_chi2ndf.min");
            DT_THROW_IF(min < 0, std::range_error,
                        "Invalid min value of chi2/ndf (" << min << ") !");
            _chi2ndf_range_min_ = min;
            count++;
          }
          if (configuration_.has_key("range_chi2ndf.max")) {
            double max = configuration_.fetch_real("range_chi2ndf.max");
            DT_THROW_IF(max < 0, std::range_error,
                        "Invalid max value of chi2/ndf (" << max << ") !");
            _chi2ndf_range_max_ = max;
            count++;
          }
          DT_THROW_IF(count == 0, std::logic_error,
                      "Missing 'range_chi2ndf.min' or 'range_chi2ndf.max' property !");
          if (count == 2 && _chi2ndf_range_min_ >= 0 && _chi2ndf_range_max_ >= 0) {
            DT_THROW_IF(_chi2ndf_range_min_ > _chi2ndf_range_max_, std::logic_error,
                        "Invalid 'range_chi2ndf.min' > 'range_chi2ndf.max' values !");
          }
        } // end if is_mode_range_chi2ndf

        // mode RANGE_PVALUE:
        if (is_mode_range_pvalue()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_PVALUE mode...");
          int count = 0;
          if (configuration_.has_key("range_pvalue.min")) {
            double pmin = configuration_.fetch_real("range_pvalue.min");
            if (! configuration_.has_explicit_unit("range_pvalue.min")) {
              pmin *= CLHEP::perCent;
            }
            DT_THROW_IF(pmin < 0.0*CLHEP::perCent || pmin > 100.0*CLHEP::perCent,
                        std::range_error,
                        "Invalid min value of p-value (" << pmin << ") !");
            _pvalue_range_min_ = pmin;
            count++;
          }
          if (configuration_.has_key("range_pvalue.max")) {
            double pmax = configuration_.fetch_real("range_pvalue.max");
            if (! configuration_.has_explicit_unit("range_pvalue.max")) {
              pmax *= CLHEP::perCent;
            }
            DT_THROW_IF(pmax < 0.0*CLHEP::perCent || pmax > 100.0*CLHEP::perCent,
                        std::range_error,
                        "Invalid max value of p-value (" << pmax << ") !");
            _pvalue_range_max_ = pmax;
            count++;
          }
          DT_THROW_IF(count == 0, std::logic_error,
                      "Missing 'range_pvalue.min' or 'range_pvalue.max' property !");
          if (count == 2 && _pvalue_range_min_ >= 0 && _pvalue_range_max_ >= 0) {
            DT_THROW_IF(_pvalue_range_min_ > _pvalue_range_max_, std::logic_error,
                        "Invalid 'range_pvalue.min' > 'range_pvalue.max' values !");
          }
        } // end if is_mode_range_pvalue
      }

      this->i_cut::_set_initialized (true);
      return;
    }


    int tracker_trajectory_data_cut::_accept ()
    {
      int cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has(_TTD_label_)) {
        DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _TTD_label_ << "' bank !");
        return cut_returned;
      }

      // Get tracker trajectory data bank
      const snemo::datamodel::tracker_trajectory_data & TTD
        = ER.get<snemo::datamodel::tracker_trajectory_data> (_TTD_label_);

      // Check if the tracker trajectory data has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running FLAG mode...");
        const bool check = TTD.get_auxiliaries().has_flag(_flag_name_);
        if (! check) {
          check_flag = false;
        }
      }

      // Check if the tracker trajectory data has solutions :
      bool check_has_solution = true;
      if (is_mode_has_solution()) {
        DT_LOG_DEBUG (get_logging_priority (), "Running HAS_TRAJECTORY mode...");
        // 2012-05-13 XG: Here we only take care of the default solution
        if (! TTD.has_default_solution()) check_has_solution = false;
      }

      // Check if the tracker trajectory data has a range of chi2/ndf values
      bool check_range_chi2ndf = false;
      bool check_range_pvalue  = false;
      if (is_mode_range_chi2ndf() || is_mode_range_pvalue()) {
        // 2012-05-13 XG: Here we only take care of the default solution
        if (! TTD.has_default_solution()) {
          DT_LOG_DEBUG(get_logging_priority(), "Tracker trajectory data has no default solution!");
          return cuts::SELECTION_INAPPLICABLE;
        }
        const snemo::datamodel::tracker_trajectory_solution & a_default_solution
          = TTD.get_default_solution();
        if (! a_default_solution.has_trajectories()) {
          DT_LOG_DEBUG(get_logging_priority(), "Tracker trajectory solution has no trajectories");
          return cuts::SELECTION_INAPPLICABLE;
        }

        const snemo::datamodel::tracker_trajectory_solution::trajectory_col_type &
          the_trajectories = a_default_solution.get_trajectories();
        for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::const_iterator
               itraj = the_trajectories.begin(); itraj != the_trajectories.end(); ++itraj) {
          const snemo::datamodel::tracker_trajectory & a_trajectory = itraj->get();

          const datatools::properties & properties = a_trajectory.get_auxiliaries();
          if (! properties.has_key("chi2") || ! properties.has_key("ndof")) continue;
          const double chi2 = properties.fetch_real("chi2");
          const size_t ndof = properties.fetch_integer("ndof");

          if (is_mode_range_pvalue()) {
            const double pvalue = gsl_cdf_chisq_Q(chi2, ndof)*100*CLHEP::perCent;
            if (datatools::is_valid(_pvalue_range_min_) &&
                datatools::is_valid(_pvalue_range_max_)) {
              if (pvalue > _pvalue_range_min_ && pvalue < _pvalue_range_max_) {
                check_range_pvalue = true;
                break;
              }
            } else if (datatools::is_valid(_pvalue_range_min_)) {
              if (pvalue > _pvalue_range_min_) {
                check_range_pvalue = true;
                break;
              }
            } else if (datatools::is_valid(_pvalue_range_max_)) {
              if (pvalue > _pvalue_range_max_) {
                check_range_pvalue = true;
                break;
              }
            }
          } // end of is_mode_range_pvalue
          if (is_mode_range_chi2ndf()) {
            const double chi2ndf = chi2/(double)ndof;
            if (datatools::is_valid(_chi2ndf_range_min_) &&
                datatools::is_valid(_chi2ndf_range_max_)) {
              if (chi2ndf > _chi2ndf_range_min_ && chi2ndf < _chi2ndf_range_max_) {
                check_range_chi2ndf = true;
                break;
              }
            } else if (datatools::is_valid(_chi2ndf_range_min_)) {
              if (chi2ndf > _chi2ndf_range_min_) {
                check_range_chi2ndf = true;
                break;
              }
            } else if (datatools::is_valid(_chi2ndf_range_max_)) {
              if (chi2ndf > _chi2ndf_range_max_) {
                check_range_chi2ndf = true;
                break;
              }
            }
          }
        }
      }

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag           &&
          check_has_solution   &&
          ((is_mode_range_pvalue() && check_range_pvalue) ||
           (is_mode_range_chi2ndf() && check_range_chi2ndf))) {
        cut_returned = cuts::SELECTION_ACCEPTED;
      }

      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::tracker_trajectory_data_cut, ocd_)
{
  ocd_.set_class_name("snemo::cut::tracker_trajectory_data_cut");
  ocd_.set_class_description("Cut based on criteria applied to the Tracker Trajectory data bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  {
    // Description of the 'TTD_label' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TTD_label")
      .set_terse_description("The name of the Tracker Trajectory Data bank")
      .set_traits(datatools::TYPE_STRING)
      .set_default_value_string(snemo::datamodel::data_info::default_tracker_trajectory_data_label())
      .add_example("Set the default value::        \n"
                   "                               \n"
                   "  TTD_label : string = \"TTD\" \n"
                   "                               \n"
                   )
      ;
  }

  {
    // Description of the 'mode.flag' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.flag")
      .set_terse_description("Mode with a special request flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                              \n"
                   "                                                 \n"
                   "  mode.flag : boolean = true                     \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'mode.has_solution' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_solution")
      .set_terse_description("Mode requiring the presence of default tracker trajectory solution")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                  \n"
                   "                                     \n"
                   "  mode.has_solution : boolean = true \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'mode.range_chi2ndf' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_chi2ndf")
      .set_terse_description("Mode with a special requested ranged of chi2/ndf value of the fit")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                   \n"
                   "                                      \n"
                   "  mode.range_chi2ndf : boolean = true \n"
                   "                                      \n"
                   )
      ;
  }

  {
    // Description of the 'mode.range_pvalue' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_pvalue")
      .set_terse_description("Mode with a special requested ranged of p-value value of the fit")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                  \n"
                   "                                     \n"
                   "  mode.range_pvalue : boolean = true \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'range_chi2ndf.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_chi2ndf.min")
      .set_terse_description("Minimum value of chi2/ndf of the trajectory fit")
      .set_triggered_by_flag("mode.range_chi2ndf")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(false)
      .add_example("Set a specific minimum value of chi2/ndf:: \n"
                   "                                           \n"
                   "  range_chi2ndf.min : real = 0.1           \n"
                   "                                           \n"
                   )
      ;
  }

  {
    // Description of the 'range_chi2ndf.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_chi2ndf.max")
      .set_terse_description("Maximum value of chi2/ndf of the trajectory fit")
      .set_triggered_by_flag("mode.range_chi2ndf")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(false)
      .add_example("Set a specific maximum value of chi2/ndf:: \n"
                   "                                           \n"
                   "  range_chi2ndf.max : real = 2.0           \n"
                   "                                           \n"
                   )
      ;
  }

  {
    // Description of the 'range_pvalue.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_pvalue.min")
      .set_terse_description("Minimum value of p-value of the trajectory fit")
      .set_triggered_by_flag("mode.range_pvalue")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(true)
      .set_unit_label("fraction")
      .set_unit_symbol("%")
      .add_example("Set a specific minimum value of p-value::    \n"
                   "                                             \n"
                   "  range_pvalue.min : real as fraction = 10 % \n"
                   "                                             \n"
                   )
      ;
  }

  {
    // Description of the 'range_pvalue.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_pvalue.max")
      .set_terse_description("Maximum value of p-value of the trajectory fit")
      .set_triggered_by_flag("mode.range_pvalue")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(true)
      .set_unit_label("fraction")
      .set_unit_symbol("%")
      .add_example("Set a specific maximum value of p-value::    \n"
                   "                                             \n"
                   "  range_pvalue.max : real as fraction = 50 % \n"
                   "                                             \n"
                   )
      ;
  }


  // Additional configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "                                                 \n"
                               "   TTD_label : string = \"TTD\"                  \n"
                               "   mode.flag : boolean = false                   \n"
                               "   # flag.name : string = \"test2\"              \n"
                               "   mode.has_solution : boolean = true            \n"
                               "   mode.range_chi2ndf : boolean = false          \n"
                               "   # range_chi2ndf.min : real = 0.1              \n"
                               "   # range_chi2ndf.max : real = 2.0              \n"
                               "   mode.range_pvalue : boolean = true            \n"
                               "   range_pvalue.min : real as fraction = 10 %    \n"
                               "   # range_pvalue.max : real as fraction = 100 % \n"
                               "                                                 \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::cut::tracker_trajectory_data_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::tracker_trajectory_data_cut, "snemo::cut::tracker_trajectory_data_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
