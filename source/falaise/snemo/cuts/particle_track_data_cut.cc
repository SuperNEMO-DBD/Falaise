// falaise/snemo/cuts/particle_track_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/particle_track_data_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/things.h>

// SuperNEMO data models :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/particle_track_data.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(particle_track_data_cut,"snemo::cut::particle_track_data_cut");

    void particle_track_data_cut::_set_defaults()
    {
      _PTD_label_           = "";
      _mode_                = MODE_UNDEFINED;
      _flag_name_           = "";
      _particles_range_min_ = -1;
      _particles_range_max_ = -1;
      _non_associated_calorimeter_hits_range_min_ = -1;
      _non_associated_calorimeter_hits_range_max_ = -1;
      return;
    }

    void particle_track_data_cut::set_PTD_label(const std::string & PTD_label_)
    {
      _PTD_label_ = PTD_label_;
      return;
    }

    const std::string & particle_track_data_cut::get_PTD_label() const
    {
      return _PTD_label_;
    }

    uint32_t particle_track_data_cut::get_mode() const
    {
      return _mode_;
    }

    bool particle_track_data_cut::is_mode_flag() const
    {
      return _mode_ & MODE_FLAG;
    }

    bool particle_track_data_cut::is_mode_has_non_associated_calorimeter_hits() const
    {
      return _mode_ & MODE_HAS_NON_ASSOCIATED_CALORIMETER_HITS;
    }

    bool particle_track_data_cut::is_mode_range_non_associated_calorimeter_hits() const
    {
      return _mode_ & MODE_RANGE_NON_ASSOCIATED_CALORIMETER_HITS;
    }

    bool particle_track_data_cut::is_mode_has_particles() const
    {
      return _mode_ & MODE_HAS_PARTICLES;
    }

    bool particle_track_data_cut::is_mode_range_particles() const
    {
      return _mode_ & MODE_RANGE_PARTICLES;
    }

    void particle_track_data_cut::set_flag_name(const std::string & flag_name_)
    {
      _flag_name_ = flag_name_;
      return;
    }

    const std::string & particle_track_data_cut::get_flag_name() const
    {
      return _flag_name_;
    }

    particle_track_data_cut::particle_track_data_cut(datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults();
      return;
    }

    particle_track_data_cut::~particle_track_data_cut()
    {
      if (is_initialized()) this->particle_track_data_cut::reset();
      return;
    }

    void particle_track_data_cut::reset()
    {
      _set_defaults();
      this->i_cut::_reset();
      this->i_cut::_set_initialized(false);
      return;
    }

    void particle_track_data_cut::initialize(const datatools::properties & configuration_,
                                             datatools::service_manager  & /* service_manager_ */,
                                             cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Cut '" << get_name() << "' is already initialized ! ");

      this->i_cut::_common_initialize(configuration_);

      if (_PTD_label_.empty()) {
        if (configuration_.has_key("PTD_label")) {
          set_PTD_label(configuration_.fetch_string("PTD_label"));
        } else {
          set_PTD_label(snemo::datamodel::data_info::default_particle_track_data_label());
        }
      }

      if (_mode_ == MODE_UNDEFINED) {
        if (configuration_.has_flag("mode.flag")) {
          _mode_ |= MODE_FLAG;
        }

        if (configuration_.has_flag("mode.has_non_associated_calorimeter_hits")) {
          _mode_ |= MODE_HAS_NON_ASSOCIATED_CALORIMETER_HITS;
        }

        if (configuration_.has_flag("mode.range_non_associated_calorimeter_hits")) {
          _mode_ |= MODE_RANGE_NON_ASSOCIATED_CALORIMETER_HITS;
        }

        if (configuration_.has_flag("mode.has_particles")) {
          _mode_ |= MODE_HAS_PARTICLES;
        }

        if (configuration_.has_flag("mode.range_particles")) {
          _mode_ |= MODE_RANGE_PARTICLES;
        }

        DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error,
                    "Missing at least a 'mode.XXX' property !");

        // mode FLAG:
        if (is_mode_flag()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using FLAG mode...");
          DT_THROW_IF(! configuration_.has_key("flag.name"),
                      std::logic_error,
                      "Missing 'flag.name' property !");
          set_flag_name(configuration_.fetch_string("flag.name"));
        } // end if is_mode_flag

        // mode HAS_NON_ASSOCIATED_CALORIMETERS:
        if (is_mode_has_non_associated_calorimeter_hits()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using HAS_NON_ASSOCIATED_CALORIMETER_HITS mode...");
        } // end if is_mode_has_non_associated_calorimeters

          // mode RANGE_NON_ASSOCIATED_CALORIMETERS:
        if (is_mode_range_non_associated_calorimeter_hits()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_NON_ASSOCIATED_CALORIMETER_HITS mode...");
          int count = 0;
          if (configuration_.has_key("range_non_associated_calorimeter_hits.min")) {
            const int nmin = configuration_.fetch_integer("range_non_associated_calorimeter_hits.min");
            DT_THROW_IF(nmin < 0, std::range_error,
                        "Invalid min number of non associated calorimeter hits (" << nmin << ") !");
            _non_associated_calorimeter_hits_range_min_ = nmin;
            count++;
          }
          if (configuration_.has_key("range_non_associated_calorimeter_hits.max")) {
            const int nmax = configuration_.fetch_integer("range_non_associated_calorimeter_hits.max");
            DT_THROW_IF(nmax < 0, std::range_error,
                        "Invalid max number of non associated calorimeter hits (" << nmax << ") !");
            _non_associated_calorimeter_hits_range_max_ = nmax;
            count++;
          }
          DT_THROW_IF(count == 0, std::logic_error,
                      "Missing 'range_non_associated_calorimeters.min' or 'range_non_associated_calorimeter_hits.max' property !");
          if (count == 2 && _non_associated_calorimeter_hits_range_min_ >= 0 &&
              _non_associated_calorimeter_hits_range_max_ >= 0) {
            DT_THROW_IF(_non_associated_calorimeter_hits_range_min_ > _non_associated_calorimeter_hits_range_max_, std::logic_error,
                        "Invalid 'range_non_associated_calorimeter_hits.min' > 'range_non_associated_calorimeter_hits.max' values !");
          }

        } // end if is_mode_range_non_associated_calorimeters

          // mode HAS_PARTICLES:
        if (is_mode_has_particles()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using HAS_PARTICLES mode...");
        } // end if is_mode_has_particles

          // mode RANGE_PARTICLES:
        if (is_mode_range_particles()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_PARTICLES mode...");

          int count = 0;
          if (configuration_.has_key("range_particles.min")) {
            const int nmin = configuration_.fetch_integer("range_particles.min");
            DT_THROW_IF(nmin < 0, std::range_error, "Invalid min number of particles (" << nmin << ") !");
            _particles_range_min_ = nmin;
            count++;
          }
          if (configuration_.has_key("range_particles.max")) {
            const int nmax = configuration_.fetch_integer("range_particles.max");
            DT_THROW_IF(nmax < 0, std::range_error, "Invalid max number of particles (" << nmax << ") !");
            _particles_range_max_ = nmax;
            count++;
          }
          DT_THROW_IF(count == 0, std::logic_error,
                      "Missing 'range_particles.min' or 'range_particles.max' property !");
          if (count == 2 && _particles_range_min_ >= 0 && _particles_range_max_ >= 0) {
            DT_THROW_IF(_particles_range_min_ > _particles_range_max_, std::range_error,
                        "Invalid 'range_particles.min' > 'range_particles.max' values !");
          }
        } // end if is_mode_range_particles
      }

      this->i_cut::_set_initialized(true);
      return;
    }


    int particle_track_data_cut::_accept()
    {
      uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has(_PTD_label_)) {
        DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _PTD_label_ << "' bank !");
        return cut_returned;
      }

      // Get Particle Track Data bank
      const snemo::datamodel::particle_track_data & PTD
        = ER.get<snemo::datamodel::particle_track_data>(_PTD_label_);

      // Check if the tracker clustering data has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running FLAG mode...");
        const bool check = PTD.get_auxiliaries().has_flag(_flag_name_);
        if (! check) {
          check_flag = false;
        }
      }

      // Check if event have an non associated calorimeter hit :
      bool check_non_associated_calorimeter_hits = true;
      if (is_mode_has_non_associated_calorimeter_hits()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running HAS_NON_ASSOCIATED_CALORIMETER_HITS mode...");

        if (!PTD.has_non_associated_calorimeters()) {
          check_non_associated_calorimeter_hits = false;
        }
      }// end mode HAS_NON_ASSOCIATED_CALORIMETER_HITS

      bool check_range_non_associated_calorimeter_hits = true;
      if (is_mode_range_non_associated_calorimeter_hits()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_NON_ASSOCIATED_CALORIMETER_HITS mode...");

        if (!PTD.has_non_associated_calorimeters()) {
          DT_LOG_DEBUG(get_logging_priority(), "The event does not have non associated calorimeter hits !");
          return cuts::SELECTION_INAPPLICABLE;
        }
        const size_t n_unasso_calorimeters = PTD.get_non_associated_calorimeters().size();
        DT_LOG_DEBUG(get_logging_priority(),
                     "Number of non associated calorimeters = " << n_unasso_calorimeters << " (min = "
                     << _non_associated_calorimeter_hits_range_min_ << " , max = " << _non_associated_calorimeter_hits_range_max_ << ")");

        bool check = true;
        if (_non_associated_calorimeter_hits_range_min_ >= 0 && n_unasso_calorimeters < (size_t)_non_associated_calorimeter_hits_range_min_) {
          check = false;
        }
        if (_non_associated_calorimeter_hits_range_max_ >= 0 && n_unasso_calorimeters > (size_t)_non_associated_calorimeter_hits_range_max_) {
          check = false;
        }
        if (! check) {
          DT_LOG_DEBUG(get_logging_priority(), "Event rejected by RANGE_NON_ASSOCIATED_CALORIMETER_HITS cut!");
          check_range_non_associated_calorimeter_hits = false;
        }
      }// end mode RANGE_NON_ASSOCIATED_CALORIMETER_HITS

      // Check if the particle track data has some specific particle trajectory :
      bool check_has_particles = true;
      if (is_mode_has_particles()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running HAS_PARTICLES mode...");
        if (! PTD.has_particles()) {
          check_has_particles = false;
        }
      }

      // Check if the particle track data has some specific category of hits :
      bool check_range_particles = true;
      if (is_mode_range_particles()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_PARTICLES mode...");
        if (! PTD.has_particles()) {
          DT_LOG_DEBUG(get_logging_priority(), "Particle track data has no particle");
          return cuts::SELECTION_INAPPLICABLE;
        }

        const size_t nparticles = PTD.get_number_of_particles();
        DT_LOG_DEBUG(get_logging_priority(), "Number of particle track = " << nparticles << " (min = "
                     << _particles_range_min_ << " , max = " << _particles_range_max_ << ")");
        bool check = true;
        if (_particles_range_min_ >= 0) {
          if (nparticles < (size_t)_particles_range_min_) {
            check = false;
          }
        }
        if (_particles_range_max_ >= 0){
          if (nparticles > (size_t)_particles_range_max_) {
            check = false;
          }
        }
        if (! check) {
          DT_LOG_DEBUG(get_logging_priority(), "Event rejected by RANGE_PARTICLES cut!");
          check_range_particles = false;
        }
      }

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag                                  &&
          check_non_associated_calorimeter_hits       &&
          check_range_non_associated_calorimeter_hits &&
          check_has_particles                         &&
          check_range_particles) {
        cut_returned = cuts::SELECTION_ACCEPTED;
      }

      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::particle_track_data_cut, ocd_)
{
  ocd_.set_class_name("snemo::cut::particle_track_data_cut_cut");
  ocd_.set_class_description("Cut based on criteria applied to the Particle Track data bank stored in the event record");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

  {
    // Description of the 'PTD_label' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("PTD_label")
      .set_terse_description("The name of the Particle Track Data bank")
      .set_traits(datatools::TYPE_STRING)
      .set_default_value_string(snemo::datamodel::data_info::default_particle_track_data_label())
      .add_example("Set the default value::        \n"
                   "                               \n"
                   "  PTD_label : string = \"PTD\" \n"
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
      .add_example("Activate the mode::          \n"
                   "                             \n"
                   "  mode.flag : boolean = true \n"
                   "                             \n"
                   )
      ;
  }

  {
    // Description of the 'mode.has_non_associated_calorimeter_hits' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_non_associated_calorimeter_hits")
      .set_terse_description("Mode requiring the presence of non associated calorimeter hits")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                                         \n"
                   "                                                            \n"
                   "  mode.has_non_associated_calorimeter_hits : boolean = true \n"
                   "                                                            \n"
                   )
      ;
  }

  {
    // Description of the 'mode.range_non_associated_calorimeter_hits' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_non_associated_calorimeter_hits")
      .set_terse_description("Mode with a special requested range of non associated calorimeter hits")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                                           \n"
                   "                                                              \n"
                   "  mode.range_non_associated_calorimeter_hits : boolean = true \n"
                   "                                                              \n"
                   )
      ;
  }

  {
    // Description of the 'mode.has_particles' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_particles")
      .set_terse_description("Mode requiring the presence of particle tracks")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                   \n"
                   "                                      \n"
                   "  mode.has_particles : boolean = true \n"
                   "                                      \n"
                   )
      ;
  }

  {
    // Description of the 'mode.range_particles' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_particles")
      .set_terse_description("Mode with a special requested range of particle tracks")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                     \n"
                   "                                        \n"
                   "  mode.range_particles : boolean = true \n"
                   "                                        \n"
                   )
      ;
  }


  {
    // Description of the 'range_non_associated_calorimeter_hits.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_non_associated_calorimeter_hits.min")
      .set_terse_description("Minimum number of non associated calorimeter hits")
      .set_triggered_by_flag("mode.range_non_associated_calorimeter_hits")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific minimum number of non associated calorimeter hits:: \n"
                   "                                                                   \n"
                   "  range_non_associated_calorimeter_hits.min : integer = 3          \n"
                   "                                                                   \n"
                   )
      ;
  }

  {
    // Description of the 'range_non_associated_calorimeter_hits.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_non_associated_calorimeter_hits.max")
      .set_terse_description("Maximum number of non associated calorimeter hits")
      .set_triggered_by_flag("mode.range_non_associated_calorimeter_hits")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific maximum number of non associated calorimeter hits:: \n"
                   "                                                                   \n"
                   "  range_non_associated_calorimeter_hits.max : integer = 20         \n"
                   "                                                                   \n"
                   )
      ;
  }

  {
    // Description of the 'range_particles.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_particles.min")
      .set_terse_description("Minimum number of particle tracks")
      .set_triggered_by_flag("mode.range_particles")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific minimum number of particle tracks:: \n"
                   "                                                   \n"
                   "  range_particles.min : integer = 3                \n"
                   "                                                   \n"
                   )
      ;
  }

  {
    // Description of the 'range_particles.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_particles.max")
      .set_terse_description("Maximum number of particle tracks")
      .set_triggered_by_flag("mode.range_particles")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific maximum number of particle tracks:: \n"
                   "                                                   \n"
                   "  range_particles.max : integer = 20               \n"
                   "                                                   \n"
                   )
      ;
  }


  // Additional configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the                   \n"
                               "``datatools::properties`` ASCII format::                      \n"
                               "                                                              \n"
                               "                                                              \n"
                               "   PTD_label : string = \"PTD\"                               \n"
                               "   mode.flag : boolean = false                                \n"
                               "   # flag.name : string = \"test2\"                           \n"
                               "   mode.has_non_associated_calorimeter_hits : boolean = false \n"
                               "   mode.has_particles : boolean = true                        \n"
                               "   mode.range_particles : boolean = true                      \n"
                               "   range_particles.min : integer = 0                          \n"
                               "   range_particles.max : integer = 3                          \n"
                               "                                                              \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::cut::particle_track_data_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::particle_track_data_cut, "snemo::cut::particle_track_data_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
