// falaise/snemo/cuts/particle_track_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/particle_track_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>

// SuperNEMO data models :
#include <falaise/snemo/datamodels/particle_track.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(particle_track_cut, "snemo::cut::particle_track_cut");

    void particle_track_cut::_set_defaults()
    {
      _mode_ = MODE_UNDEFINED;
      _flag_name_ = "";
      _calorimeter_hits_range_category_ = "";
      _calorimeter_hits_range_min_ = -1;
      _calorimeter_hits_range_max_ = -1;
      return;
    }

    uint32_t particle_track_cut::get_mode() const
    {
      return _mode_;
    }

    bool particle_track_cut::is_mode_flag() const
    {
      return _mode_ & MODE_FLAG;
    }

    bool particle_track_cut::is_mode_has_associated_calorimeter_hits() const
    {
      return _mode_ & MODE_HAS_ASSOCIATED_CALORIMETER_HITS;
    }

    bool particle_track_cut::is_mode_range_associated_calorimeter_hits() const
    {
      return _mode_ & MODE_RANGE_ASSOCIATED_CALORIMETER_HITS;
    }

    bool particle_track_cut::is_mode_has_vertex() const
    {
      return _mode_ & MODE_HAS_VERTEX;
    }

    bool particle_track_cut::is_mode_has_charge() const
    {
      return _mode_ & MODE_HAS_CHARGE;
    }

    bool particle_track_cut::is_mode_has_delayed_cluster() const
    {
      return _mode_ & MODE_HAS_DELAYED_CLUSTER;
    }

    particle_track_cut::particle_track_cut(datatools::logger::priority logger_priority_)
      : cuts::i_cut(logger_priority_)
    {
      _set_defaults();
      return;
    }

    particle_track_cut::~particle_track_cut()
    {
      if (is_initialized()) this->particle_track_cut::reset();
      return;
    }

    void particle_track_cut::reset()
    {
      _set_defaults();
      this->i_cut::_reset();
      this->i_cut::_set_initialized(false);
      return;
    }

    void particle_track_cut::initialize(const datatools::properties & configuration_,
                                        datatools::service_manager  & /* service_manager_ */,
                                        cuts::cut_handle_dict_type  & /* cut_dict_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Cut '" << get_name() << "' is already initialized ! ");

      this->i_cut::_common_initialize(configuration_);

      if (_mode_ == MODE_UNDEFINED) {
        if (configuration_.has_flag("mode.flag")) {
          _mode_ |= MODE_FLAG;
        }
        if (configuration_.has_flag("mode.has_charge")) {
          _mode_ |= MODE_HAS_CHARGE;
        }
        if (configuration_.has_flag("mode.has_associated_calorimeter_hits")) {
          _mode_ |= MODE_HAS_ASSOCIATED_CALORIMETER_HITS;
        }
        if (configuration_.has_flag("mode.range_associated_calorimeter_hits")) {
          _mode_ |= MODE_RANGE_ASSOCIATED_CALORIMETER_HITS;
        }
        if (configuration_.has_flag("mode.has_vertex")) {
          _mode_ |= MODE_HAS_VERTEX;
        }
        if (configuration_.has_flag("mode.has_delayed_cluster")) {
          _mode_ |= MODE_HAS_DELAYED_CLUSTER;
        }
        DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error,
                    "Missing at least a 'mode.XXX' property !");

        // mode FLAG:
        if (is_mode_flag()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using FLAG mode...");
          DT_THROW_IF(! configuration_.has_key("flag.name"),
                      std::logic_error,
                      "Missing 'flag.name' property !");
          _flag_name_ = configuration_.fetch_string("flag.name");
        } // end if is_mode_flag

        // mode HAS_CHARGE:
        if (is_mode_has_charge()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using HAS_CHARGE mode...");
          DT_THROW_IF(! configuration_.has_key("has_charge.type"), std::logic_error,
                      "Missing 'has_charge.type' property !");
          _charge_type_ = configuration_.fetch_string("has_charge.type");
          DT_THROW_IF(_charge_type_ != "negative"  &&
                      _charge_type_ != "positive"  &&
                      _charge_type_ != "undefined" &&
                      _charge_type_ != "neutral",
                      std::logic_error, "Invalid charge type label !");
        } // end if is_mode_has_charge

        // mode HAS_ASSOCIATED_CALORIMETERS:
        if (is_mode_has_associated_calorimeter_hits()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using HAS_ASSOCIATED_CALORIMETER_HITS mode...");
        } // end if is_mode_has_associated_calorimeters

        // mode PARTICLE_RANGE_ASSOCIATED_CALORIMETERS:
        if (is_mode_range_associated_calorimeter_hits()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_ASSOCIATED_CALORIMETER_HITS mode...");
          if (configuration_.has_key("range_associated_calorimeter_hits.category")) {
            _calorimeter_hits_range_category_ = configuration_.fetch_string("range_associated_calorimeter_hits.category");
            DT_THROW_IF(_calorimeter_hits_range_category_ != "calo"  &&
                        _calorimeter_hits_range_category_ != "xcalo" &&
                        _calorimeter_hits_range_category_ != "gveto",
                        std::logic_error, "Invalid calorimeter category label !");
          }
          size_t count = 0;
          if (configuration_.has_key("range_associated_calorimeter_hits.min")) {
            const int nmin = configuration_.fetch_integer("range_associated_calorimeter_hits.min");
            DT_THROW_IF(nmin < 0, std::range_error,
                        "Invalid min number of associated calorimeter hits (" << nmin << ") !");
            _calorimeter_hits_range_min_ = nmin;
            count++;
          }
          if (configuration_.has_key("range_associated_calorimeter_hits.max")) {
            const int nmax = configuration_.fetch_integer("range_associated_calorimeter_hits.max");
            DT_THROW_IF(nmax < 0, std::range_error,
                        "Invalid max number of associated calorimeter hits (" << nmax << ") !");
            _calorimeter_hits_range_max_ = nmax;
            count++;
          }
          DT_THROW_IF(count == 0, std::logic_error,
                      "Missing 'range_associated_calorimeters.min' or 'range_associated_calorimeter_hits.max' property !");
          if (count == 2 && _calorimeter_hits_range_min_ >= 0 && _calorimeter_hits_range_max_ >= 0) {
            DT_THROW_IF(_calorimeter_hits_range_min_ > _calorimeter_hits_range_max_, std::logic_error,
                        "Invalid 'range_associated_calorimeter_hits.min' > 'range_associated_calorimeter_hits.max' values !");
          }
        } // end if is_mode_range_associated_calorimeters

        // mode HAS_VERTEX:
        if (is_mode_has_vertex()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using HAS_VERTEX mode...");
          DT_THROW_IF(! configuration_.has_key("has_vertex.type"), std::logic_error,
                      "Missing 'has_vertex.type' property !");
          _vertex_type_ = configuration_.fetch_string("has_vertex.type");
          DT_THROW_IF(_vertex_type_ != "wire"  &&
                      _vertex_type_ != "foil"  &&
                      _vertex_type_ != "calo"  &&
                      _vertex_type_ != "xcalo" &&
                      _vertex_type_ != "gveto",
                      std::logic_error, "Invalid vertex type label !");
        } // end if is_mode_has_vertex

        // mode HAS_DELAYED_CLUSTER:
        if (is_mode_has_delayed_cluster()) {
          DT_LOG_DEBUG(get_logging_priority(), "Using HAS_DELAYED_CLUSTER mode...");
        } // end if is_mode_has_delayed_cluster
     }

      this->i_cut::_set_initialized(true);
      return;
    }


    int particle_track_cut::_accept()
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");
      uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get particle track
      const snemo::datamodel::particle_track & a_particle = get_user_data<snemo::datamodel::particle_track>();

      // Check if the calibrated data has a property flag with a specific name :
      bool check_flag = true;
      if (is_mode_flag()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running FLAG mode...");
        const bool check = a_particle.get_auxiliaries().has_flag(_flag_name_);
        if (! check) {
          check_flag = false;
        }
      }

      // Check the charge of the particle track :
      bool check_charge = true;
      if (is_mode_has_charge()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running HAS_CHARGE mode...");
        const int8_t a_charge = a_particle.get_charge();
        if (a_charge == snemo::datamodel::particle_track::negative &&
            _charge_type_ != "negative") {
          check_charge = false;
        } else if (a_charge == snemo::datamodel::particle_track::positive &&
                   _charge_type_ != "positive") {
          check_charge = false;
        } else if (a_charge == snemo::datamodel::particle_track::undefined &&
                   _charge_type_ != "undefined") {
          check_charge = false;
        } else if (a_charge == snemo::datamodel::particle_track::neutral &&
                   _charge_type_ != "neutral") {
          check_charge = false;
        }
      }// end mode CHARGE

      // Check if particle track has an associated calorimeter hit :
      bool check_calorimeter_association = true;
      if (is_mode_has_associated_calorimeter_hits()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running HAS_ASSOCIATED_CALORIMETER_HITS mode...");
        if (! a_particle.has_associated_calorimeter_hits()) {
          DT_LOG_DEBUG(get_logging_priority(), "A particle has no associated calorimeter hit!");
          check_calorimeter_association = false;
        }
      }// end mode HAS_ASSOCIATED_CALORIMETER_HITS

      bool check_range_calorimeter_hits = true;
      if (is_mode_range_associated_calorimeter_hits()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_ASSOCIATED_CALORIMETER_HITS mode...");
        if (! a_particle.has_associated_calorimeter_hits()) {
          DT_LOG_DEBUG(get_logging_priority(), "A particle has no associated calorimeter hits !");
          return cuts::SELECTION_INAPPLICABLE;
        }

        const snemo::datamodel::calibrated_calorimeter_hit::collection_type & the_calorimeters
          = a_particle.get_associated_calorimeter_hits();
        size_t ncalorimeters = 0;
        if (_calorimeter_hits_range_category_.empty()) {
          // Count every calorimeter hits
          ncalorimeters = the_calorimeters.size();
          DT_LOG_DEBUG(get_logging_priority(),
                       "Number of associated calorimeters = " << ncalorimeters << " (min = "
                       << _calorimeter_hits_range_min_ << " , max = " << _calorimeter_hits_range_max_ << ")");
        } else {
          // Look for special calorimeter category
          for (snemo::datamodel::calibrated_calorimeter_hit::collection_type::const_iterator
                 icalo = the_calorimeters.begin();
               icalo != the_calorimeters.end(); ++icalo) {
            const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = icalo->get();
            const datatools::properties & aux = a_calo_hit.get_auxiliaries();
            if (aux.has_key("category") &&
                aux.fetch_string("category") == _calorimeter_hits_range_category_) {
              ncalorimeters++;
            }
          }
        }
        bool check = true;
        if (_calorimeter_hits_range_min_ >= 0 && ncalorimeters < (size_t)_calorimeter_hits_range_min_) {
          check = false;
        }
        if (_calorimeter_hits_range_max_ >= 0 && ncalorimeters > (size_t)_calorimeter_hits_range_max_) {
          check = false;
        }
        if (! check) {
          DT_LOG_DEBUG(get_logging_priority(), "Event rejected by PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS cut!");
          check_range_calorimeter_hits = false;
        }
      }// end mode RANGE_ASSOCIATED_CALORIMETER_HITS

      // Check if particle track has a vertex on foil :
      bool check_vertex = true;
      if (is_mode_has_vertex()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running HAS_VERTEX mode...");
        if (!a_particle.has_vertices()) {
          DT_LOG_DEBUG(get_logging_priority(), "A particle has no vertices !");
          check_vertex = false;
        } else {
          bool has_vertex = false;
          const snemo::datamodel::particle_track::vertex_collection_type & the_vertices
            = a_particle.get_vertices();
          for (snemo::datamodel::particle_track::vertex_collection_type::const_iterator
                 ivertex = the_vertices.begin();
               ivertex != the_vertices.end(); ++ivertex) {
            const geomtools::blur_spot & a_vertex = ivertex->get();
            const snemo::datamodel::particle_track::vertex_type vtype
              = snemo::datamodel::particle_track::label_to_vertex_type(_vertex_type_);
            if (snemo::datamodel::particle_track::vertex_is(a_vertex, vtype)) {
              has_vertex = true;
              break;
            }
          }
          if (! has_vertex) {
            DT_LOG_DEBUG(get_logging_priority(), "A particle has no vertex on the '" << _vertex_type_ << "' !");
            check_vertex = false;
          }
        }
      }// end mode HAS_VERTEX_ON_FOIL

      // Check if particle track has a delayed cluster :
      bool check_delayed_cluster = true;
      if (is_mode_has_delayed_cluster()) {
        DT_LOG_DEBUG(get_logging_priority(), "Running HAS_DELAYED_CLUSTER mode...");
        if (! a_particle.has_trajectory()) {
          DT_LOG_TRACE(get_logging_priority(), "No trajectory associated to particle track !");
          check_delayed_cluster = false;
        } else {
          const snemo::datamodel::tracker_trajectory & a_trajectory = a_particle.get_trajectory();
          if (! a_trajectory.has_cluster()) {
            DT_LOG_TRACE(get_logging_priority(), "Tracker trajectory has no associated cluster !");
            check_delayed_cluster = false;
          } else {
            const snemo::datamodel::tracker_cluster & a_cluster = a_trajectory.get_cluster();
            if (a_cluster.is_prompt()) {
              DT_LOG_TRACE(get_logging_priority(), "Tracker cluster is not delayed !");
              check_delayed_cluster = false;
            }
          }
        }
        if (check_delayed_cluster) {
          DT_LOG_TRACE(get_logging_priority(), "Particle track has delayed cluster !");
        }
      }// end mode HAS_DELAYED_CLUSTER

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_flag                    &&
          check_charge                  &&
          check_calorimeter_association &&
          check_range_calorimeter_hits  &&
          check_vertex                  &&
          check_delayed_cluster) {
          cut_returned = cuts::SELECTION_ACCEPTED;
        }
      return cut_returned;
    }

  }  // end of namespace cut

}  // end of namespace snemo

DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::cut::particle_track_cut, ocd_)
{
  ocd_.set_class_name("snemo::cut::particle_track_cut");
  ocd_.set_class_description("Cut based on criteria applied to a particle track object stored in the particle track data bank");
  ocd_.set_class_library("falaise");
  // ocd_.set_class_documentation("");

  cuts::i_cut::common_ocd(ocd_);

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
    // Description of the 'mode.has_charge' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_charge")
      .set_terse_description("Mode with a special requested charge")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested charge mode:: \n"
                   "                                     \n"
                   "  mode.has_charge : boolean = true   \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'mode.has_vertex' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_vertex")
      .set_terse_description("Mode with a special requested vertex")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested vertex mode:: \n"
                   "                                     \n"
                   "  mode.has_vertex : boolean = true   \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'mode.has_delayed_cluster' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_delayed_cluster")
      .set_terse_description("Mode with a special request for delayed cluster")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested delayed cluster mode:: \n"
                   "                                              \n"
                   "  mode.has_delayed_cluster : boolean = true   \n"
                   "                                              \n"
                   )
      ;
  }

  {
    // Description of the 'mode.has_associated_calorimeter_hits' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.has_associated_calorimeter_hits")
      .set_terse_description("Mode with a special request for associated calorimeter hits")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the requested associated calorimeter hits mode:: \n"
                   "                                                          \n"
                   "  mode.has_associated_calorimeter_hits : boolean = true   \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    // Description of the 'mode.range_associated_calorimeter_hits' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode.range_associated_calorimeter_hits")
      .set_terse_description("Mode with a special requested ranged calorimeter hit category")
      .set_traits(datatools::TYPE_BOOLEAN)
      .add_example("Activate the mode::                                       \n"
                   "                                                          \n"
                   "  mode.range_associated_calorimeter_hits : boolean = true \n"
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
    // Description of the 'has_charge.type' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("has_charge.type")
      .set_terse_description("Type of the requested charge")
      .set_triggered_by_flag("mode.has_charge")
      .set_traits(datatools::TYPE_STRING)
      .set_long_description("Supported values are: \n"
                            "                      \n"
                            " * ``negative``       \n"
                            " * ``positive``       \n"
                            " * ``undefined``      \n"
                            " * ``neutral``        \n"
                            "                      \n"
                            )
      .add_example("Set a specific charge type to be selected:: \n"
                   "                                            \n"
                   "  has_charge.type : string = \"positive\"   \n"
                   "                                            \n"
                   )
      ;
  }

  {
    // Description of the 'has_vertex.type' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("has_vertex.type")
      .set_terse_description("Type of the requested vertex")
      .set_triggered_by_flag("mode.has_vertex")
      .set_traits(datatools::TYPE_STRING)
      .set_long_description("Supported values are: \n"
                            "             \n"
                            " * ``wire``  \n"
                            " * ``foil``  \n"
                            " * ``calo``  \n"
                            " * ``xcalo`` \n"
                            " * ``gveto`` \n"
                            "             \n"
                            )
      .add_example("Set a specific vertex type to be selected:: \n"
                   "                                            \n"
                   "  has_vertex.type : string = \"gveto\"      \n"
                   "                                            \n"
                   )
      ;
  }

  {
    // Description of the 'range_associated_calorimeter_hits.category' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_associated_calorimeter_hits.category")
      .set_terse_description("Category of the requested calorimeter hits")
      .set_triggered_by_flag("mode.range_associated_calorimeter_hits")
      .set_traits(datatools::TYPE_STRING)
      .set_long_description("Supported values are: \n"
                            "             \n"
                            " * ``calo``  \n"
                            " * ``xcalo`` \n"
                            " * ``gveto`` \n"
                            "             \n"
                            "In case no value is set, all associated calorimeter hits are taken into account\n"
                            )
      .add_example("Set a specific calorimeter category to be selected::              \n"
                   "                                                                  \n"
                   "  range_associated_calorimeter_hits.category : string = \"gveto\" \n"
                   "                                                                  \n"
                   )
      ;
  }

  {
    // Description of the 'range_associated_calorimeter_hits.min' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_associated_calorimeter_hits.min")
      .set_terse_description("Minimum number of hits of the requested ranged hit category")
      .set_triggered_by_flag("mode.range_associated_calorimeter_hits")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific minimum number of hits::               \n"
                   "                                                      \n"
                   "  range_associated_calorimeter_hits.min : integer = 1 \n"
                   "                                                      \n"
                   )
      ;
  }

  {
    // Description of the 'range_associated_calorimeter_hits.max' configuration property :
    datatools::configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("range_associated_calorimeter_hits.max")
      .set_terse_description("Maximum number of hits of the requested ranged hit category")
      .set_triggered_by_flag("mode.range_associated_calorimeter_hits")
      .set_traits(datatools::TYPE_INTEGER)
      .add_example("Set a specific maximum number of hits::               \n"
                   "                                                      \n"
                   "  range_associated_calorimeter_hits.max : integer = 2 \n"
                   "                                                      \n"
                   )
      ;
  }

  // Additional configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the                        \n"
                               "``datatools::properties`` ASCII format::                           \n"
                               "                                                                   \n"
                               "   mode.flag : boolean = false                                     \n"
                               "   # flag.name : string = \"high_energy\"                          \n"
                               "   mode.has_charge : boolean = true                                \n"
                               "   has_charge.type : string = \"positive\"                         \n"
                               "   mode.has_vertex : boolean = true                                \n"
                               "   has_vertex.type : string = \"wire\"                             \n"
                               "   mode.range_associated_calorimeter_hits : boolean = true         \n"
                               "   range_associated_calorimeter_hits.category : string = \"xcalo\" \n"
                               "   range_associated_calorimeter_hits.min : integer = 1             \n"
                               "   range_associated_calorimeter_hits.max : integer = 2             \n"
                               "                                                                   \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::cut::simulated_data_cut' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::cut::particle_track_cut, "snemo::cut::particle_track_cut")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
