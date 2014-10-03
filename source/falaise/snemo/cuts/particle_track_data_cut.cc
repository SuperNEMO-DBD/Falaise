// falaise/snemo/cuts/particle_track_data_cut.cc

// Ourselves:
#include <falaise/snemo/cuts/particle_track_data_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>

// SuperNEMO data models :
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>

namespace snemo {

  namespace cut {

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(particle_track_data_cut,"snemo::cut::particle_track_data_cut");

    void particle_track_data_cut::_set_defaults()
    {
      _PTD_label_              = "";
      _mode_                   = MODE_UNDEFINED;
      _particle_charge_type_   = "";
      _particles_range_min_    = -1;
      _particles_range_max_    = -1;
      _calorimeter_hits_range_min_ = -1;
      _calorimeter_hits_range_max_ = -1;
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

    bool particle_track_data_cut::is_mode_particle_charge() const
    {
      return _mode_ & MODE_PARTICLE_CHARGE;
    }

    bool particle_track_data_cut::is_mode_particle_has_associated_calorimeter_hits() const
    {
      return _mode_ & MODE_PARTICLE_HAS_ASSOCIATED_CALORIMETER_HITS;
    }

    bool particle_track_data_cut::is_mode_particle_range_associated_calorimeter_hits() const
    {
      return _mode_ & MODE_PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS;
    }

    bool particle_track_data_cut::is_mode_particle_has_vertex_on_foil() const
    {
      return _mode_ & MODE_PARTICLE_HAS_VERTEX_ON_FOIL;
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

      if (_PTD_label_.empty())
        {
          DT_THROW_IF(! configuration_.has_key("PTD_label"), std::logic_error,
                      "Missing 'PTD_label' property !");
          set_PTD_label(configuration_.fetch_string("PTD_label"));
        }

      if (_mode_ == MODE_UNDEFINED)
        {
          if (configuration_.has_flag("mode.has_non_associated_calorimeter_hits"))
            {
              _mode_ |= MODE_HAS_NON_ASSOCIATED_CALORIMETER_HITS;
            }

          if (configuration_.has_flag("mode.range_non_associated_calorimeter_hits"))
            {
              _mode_ |= MODE_RANGE_NON_ASSOCIATED_CALORIMETER_HITS;
            }

          if (configuration_.has_flag("mode.has_particles"))
            {
              _mode_ |= MODE_HAS_PARTICLES;
            }

          if (configuration_.has_flag("mode.range_particles"))
            {
              _mode_ |= MODE_RANGE_PARTICLES;
            }

          if (configuration_.has_flag("mode.particle_has_charge"))
            {
              _mode_ |= MODE_PARTICLE_CHARGE;
            }

          if (configuration_.has_flag("mode.particle_has_associated_calorimeter_hits"))
            {
              _mode_ |= MODE_PARTICLE_HAS_ASSOCIATED_CALORIMETER_HITS;
            }

          if (configuration_.has_flag("mode.particle_range_associated_calorimeter_hits"))
            {
              _mode_ |= MODE_PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS;
            }

          if (configuration_.has_flag("mode.particle_has_vertex_on_foil"))
            {
              _mode_ |= MODE_PARTICLE_HAS_VERTEX_ON_FOIL;
            }

          DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error,
                      "Missing at least a 'mode.XXX' property !");

          // mode HAS_NON_ASSOCIATED_CALORIMETERS:
          if (is_mode_has_non_associated_calorimeter_hits())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using HAS_NON_ASSOCIATED_CALORIMETER_HITS mode...");
            } // end if is_mode_has_non_associated_calorimeters

          // mode RANGE_NON_ASSOCIATED_CALORIMETERS:
          if (is_mode_range_non_associated_calorimeter_hits())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_NON_ASSOCIATED_CALORIMETER_HITS mode...");
              int count = 0;
              if (configuration_.has_key("range_non_associated_calorimeter_hits.min"))
                {
                  const int nmin = configuration_.fetch_integer("range_non_associated_calorimeter_hits.min");
                  DT_THROW_IF(nmin < 0, std::range_error,
                              "Invalid min number of non associated calorimeter hits (" << nmin << ") !");
                  _non_associated_calorimeter_hits_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key("range_non_associated_calorimeter_hits.max"))
                {
                  const int nmax = configuration_.fetch_integer("range_non_associated_calorimeter_hits.max");
                  DT_THROW_IF(nmax < 0, std::range_error,
                              "Invalid max number of non associated calorimeter hits (" << nmax << ") !");
                  _non_associated_calorimeter_hits_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF(count == 0, std::logic_error,
                          "Missing 'range_non_associated_calorimeters.min' or 'range_non_associated_calorimeter_hits.max' property !");
              if (count == 2 && _non_associated_calorimeter_hits_range_min_ >= 0 && _non_associated_calorimeter_hits_range_max_ >= 0)
                {
                  DT_THROW_IF(_non_associated_calorimeter_hits_range_min_ > _non_associated_calorimeter_hits_range_max_, std::logic_error,
                              "Invalid 'range_non_associated_calorimeter_hits.min' > 'range_non_associated_calorimeter_hits.max' values !");
                }

            } // end if is_mode_range_non_associated_calorimeters

          // mode HAS_PARTICLES:
          if (is_mode_has_particles())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using HAS_PARTICLES mode...");
            } // end if is_mode_has_particles

          // mode RANGE_PARTICLES:
          if (is_mode_range_particles())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using RANGE_PARTICLES mode...");

              int count = 0;
              if (configuration_.has_key("range_particles.min"))
                {
                  const int nmin = configuration_.fetch_integer("range_particles.min");
                  DT_THROW_IF(nmin < 0, std::range_error, "Invalid min number of particles (" << nmin << ") !");
                  _particles_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key("range_particles.max"))
                {
                  const int nmax = configuration_.fetch_integer("range_particles.max");
                  DT_THROW_IF(nmax < 0, std::range_error, "Invalid max number of particles (" << nmax << ") !");
                  _particles_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF(count == 0, std::logic_error,
                          "Missing 'range_particles.min' or 'range_particles.max' property !");
              if (count == 2 && _particles_range_min_ >= 0 && _particles_range_max_ >= 0)
                {
                  DT_THROW_IF(_particles_range_min_ > _particles_range_max_, std::range_error,
                              "Invalid 'range_particles.min' > 'range_particles.max' values !");
                }
            } // end if is_mode_range_particles

          // mode PARTICLE_CHARGE:
          if (is_mode_particle_charge())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using PARTICLE_CHARGE mode...");
              DT_THROW_IF(! configuration_.has_key("particle_has_charge.type"), std::logic_error,
                          "Missing 'has_particle_charge.type' property !");
              _particle_charge_type_ = configuration_.fetch_string("particle_has_charge.type");
              DT_THROW_IF (_particle_charge_type_ != "negative" && _particle_charge_type_ != "positive" && _particle_charge_type_ != "undefined",
                           std::logic_error,
                           "Invalid charge type label ! Must be either 'negative', 'positive' or 'undefined'.");
            } // end if is_mode_particle_charge

          // mode PARTICLE_HAS_ASSOCIATED_CALORIMETERS:
          if (is_mode_particle_has_associated_calorimeter_hits())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using PARTICLE_HAS_ASSOCIATED_CALORIMETER_HITS mode...");
            } // end if is_mode_particle_has_associated_calorimeters

          // mode PARTICLE_RANGE_ASSOCIATED_CALORIMETERS:
          if (is_mode_particle_range_associated_calorimeter_hits())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS mode...");
              int count = 0;
              if (configuration_.has_key("particle_range_associated_calorimeter_hits.min"))
                {
                  const int nmin = configuration_.fetch_integer("particle_range_associated_calorimeter_hits.min");
                  DT_THROW_IF(nmin < 0, std::range_error,
                              "Invalid min number of associated calorimeter hits (" << nmin << ") !");
                  _calorimeter_hits_range_min_ = nmin;
                  count++;
                }
              if (configuration_.has_key("particle_range_associated_calorimeter_hits.max"))
                {
                  const int nmax = configuration_.fetch_integer("particle_range_associated_calorimeter_hits.max");
                  DT_THROW_IF(nmax < 0, std::range_error,
                              "Invalid max number of associated calorimeter hits (" << nmax << ") !");
                  _calorimeter_hits_range_max_ = nmax;
                  count++;
                }
              DT_THROW_IF(count == 0, std::logic_error,
                          "Missing 'particle_range_associated_calorimeters.min' or 'particle_range_associated_calorimeter_hits.max' property !");
              if (count == 2 && _calorimeter_hits_range_min_ >= 0 && _calorimeter_hits_range_max_ >= 0)
                {
                  DT_THROW_IF(_calorimeter_hits_range_min_ > _calorimeter_hits_range_max_, std::logic_error,
                              "Invalid 'particle_range_associated_calorimeter_hits.min' > 'particle_range_associated_calorimeter_hits.max' values !");
                }
            } // end if is_mode_particle_range_associated_calorimeters

          // mode PARTICLE_HAS_VERTEX_ON_FOIL:
          if (is_mode_particle_has_vertex_on_foil())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Using PARTICLE_HAS_VERTEX_ON_FOIL mode...");
            } // end if is_mode_particle_has_vertex_on_foil
        }

      this->i_cut::_set_initialized(true);
      return;
    }


    int particle_track_data_cut::_accept()
    {
      uint32_t cut_returned = cuts::SELECTION_INAPPLICABLE;

      // Get event record
      const datatools::things & ER = get_user_data<datatools::things>();

      if (! ER.has(_PTD_label_))
        {
          DT_LOG_DEBUG(get_logging_priority(), "Event record has no '" << _PTD_label_ << "' bank !");
          return cut_returned;
        }

      // Get Particle Track Data bank
      const snemo::datamodel::particle_track_data & PTD
        = ER.get<snemo::datamodel::particle_track_data>(_PTD_label_);

      // Check if event have an non associated calorimeter hit :
      bool check_non_associated_calorimeter_hits = true;
      if (is_mode_has_non_associated_calorimeter_hits())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running HAS_NON_ASSOCIATED_CALORIMETER_HITS mode...");

          if (!PTD.has_non_associated_calorimeters()) {
            check_non_associated_calorimeter_hits = false;
          }
        }// end mode HAS_NON_ASSOCIATED_CALORIMETER_HITS

      bool check_range_non_associated_calorimeter_hits = true;
      if (is_mode_range_non_associated_calorimeter_hits())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_NON_ASSOCIATED_CALORIMETER_HITS mode...");

          if (!PTD.has_non_associated_calorimeters())
            {
              DT_LOG_DEBUG(get_logging_priority(), "The event does not have non associated calorimeter hits !");
              return cuts::SELECTION_INAPPLICABLE;
            }
          const size_t n_unasso_calorimeters = PTD.get_non_associated_calorimeters().size();
          DT_LOG_DEBUG(get_logging_priority(),
                       "Number of non associated calorimeters = " << n_unasso_calorimeters << " (min = "
                       << _non_associated_calorimeter_hits_range_min_ << " , max = " << _non_associated_calorimeter_hits_range_max_ << ")");

              bool check = true;
              if (_non_associated_calorimeter_hits_range_min_ >= 0 && n_unasso_calorimeters < (size_t)_non_associated_calorimeter_hits_range_min_)
                {
                  check = false;
                }
              if (_non_associated_calorimeter_hits_range_max_ >= 0 && n_unasso_calorimeters > (size_t)_non_associated_calorimeter_hits_range_max_)
                {
                  check = false;
                }
              if (! check)
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Event rejected by RANGE_NON_ASSOCIATED_CALORIMETER_HITS cut!");
                  check_range_non_associated_calorimeter_hits = false;
                }
        }// end mode RANGE_NON_ASSOCIATED_CALORIMETER_HITS

      // Check if the particle track data has some specific particle trajectory :
      bool check_has_particles = true;
      if (is_mode_has_particles())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running HAS_PARTICLES mode...");
          if (!PTD.has_particles())
            {
              check_has_particles = false;
            }
        }

      // Check if the particle track data has some specific category of hits :
      bool check_range_particles = true;
      if (is_mode_range_particles())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running RANGE_PARTICLES mode...");
          if (!PTD.has_particles())
            {
              DT_LOG_DEBUG(get_logging_priority(), "Particle track data has no particle");
              return cuts::SELECTION_INAPPLICABLE;
            }

          const size_t nparticles = PTD.get_number_of_particles();
          DT_LOG_DEBUG(get_logging_priority(), "Number of particle track = " << nparticles << " (min = "
                       << _particles_range_min_ << " , max = " << _particles_range_max_ << ")");
          bool check = true;
          if (_particles_range_min_ >= 0)
            {
              if (nparticles < (size_t)_particles_range_min_)
                {
                  check = false;
                }
            }
          if (_particles_range_max_ >= 0)
            {
              if (nparticles > (size_t)_particles_range_max_)
                {
                  check = false;
                }
            }
          if (! check)
            {
              DT_LOG_DEBUG(get_logging_priority(), "Event rejected by RANGE_PARTICLES cut!");
              check_range_particles = false;
            }
        }

      // Check if the charge of the particle track :
      bool check_particle_charge = true;
      if (is_mode_particle_charge())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running PARTICLE_CHARGE mode...");
          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const int8_t a_charge = iparticle->get().get_charge();
              if (a_charge == snemo::datamodel::particle_track::negative && _particle_charge_type_ != "negative")
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Found negative particle! Reject event!");
                  check_particle_charge = false;
                  break;
                }
              else if (a_charge == snemo::datamodel::particle_track::positive && _particle_charge_type_ != "positive")
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Found positive particle! Reject event!");
                  check_particle_charge = false;
                  break;
                }
              else if (a_charge == snemo::datamodel::particle_track::undefined && _particle_charge_type_ != "undefined")
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Found undefined particle! Reject event!");
                  check_particle_charge = false;
                  break;
                }
            }
        }// end mode PARTICLE_CHARGE

      // Check if each particle tracks have an associated calorimeter hit :
      bool check_particle_association = true;
      if (is_mode_particle_has_associated_calorimeter_hits())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running PARTICLE_HAS_ASSOCIATED_CALORIMETER_HITS mode...");
          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const snemo::datamodel::particle_track & a_particle = iparticle->get();

              if (!a_particle.has_associated_calorimeter_hits())
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no associated calorimeter hit!");
                  check_particle_association = false;
                  break;
                }
            }
        }// end mode PARTICLE_HAS_ASSOCIATED_CALORIMETER_HITS

      bool check_particle_range_calorimeter_hits = true;
      if (is_mode_particle_range_associated_calorimeter_hits())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS mode...");

          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const snemo::datamodel::particle_track & a_particle = iparticle->get();

              if (!a_particle.has_associated_calorimeter_hits())
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no associated calorimeter hits !");
                  return cuts::SELECTION_INAPPLICABLE;
                }

              const snemo::datamodel::calibrated_calorimeter_hit::collection_type & the_calorimeters
                = a_particle.get_associated_calorimeter_hits();
              const size_t ncalorimeters = the_calorimeters.size();
              DT_LOG_DEBUG(get_logging_priority(),
                           "Number of associated calorimeters = " << ncalorimeters << " (min = "
                           << _calorimeter_hits_range_min_ << " , max = " << _calorimeter_hits_range_max_ << ")");

              bool check = true;
              if (_calorimeter_hits_range_min_ >= 0 && ncalorimeters < (size_t)_calorimeter_hits_range_min_)
                {
                  check = false;
                }
              if (_calorimeter_hits_range_max_ >= 0 && ncalorimeters > (size_t)_calorimeter_hits_range_max_)
                {
                  check = false;
                }
              if (! check)
                {
                  DT_LOG_DEBUG(get_logging_priority(), "Event rejected by PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS cut!");
                  check_particle_range_calorimeter_hits = false;
                }
            }
        }// end mode PARTICLE_RANGE_ASSOCIATED_CALORIMETER_HITS

      // Check if each particle tracks have a vertex on foil :
      bool check_particle_vertex_on_foil = true;
      if (is_mode_particle_has_vertex_on_foil())
        {
          DT_LOG_DEBUG(get_logging_priority(), "Running PARTICLE_HAS_VERTEX_ON_FOIL mode...");
          const snemo::datamodel::particle_track_data::particle_collection_type & the_particles
            = PTD.get_particles();

          if (the_particles.empty())
            {
              return cuts::SELECTION_INAPPLICABLE;
            }
          for (snemo::datamodel::particle_track_data::particle_collection_type::const_iterator
                 iparticle = the_particles.begin();
               iparticle != the_particles.end(); ++iparticle)
            {
              const snemo::datamodel::particle_track & a_particle = iparticle->get();

              if (!a_particle.has_vertices())
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no vertices !");
                  check_particle_vertex_on_foil = false;
                  break;
                }

              bool has_vertex = false;
              const snemo::datamodel::particle_track::vertex_collection_type & the_vertices
                = a_particle.get_vertices();
              for (snemo::datamodel::particle_track::vertex_collection_type::const_iterator
                     ivertex = the_vertices.begin();
                   ivertex != the_vertices.end(); ++ivertex)
                {
                  const geomtools::blur_spot & a_vertex = ivertex->get();
                  if (snemo::datamodel::particle_track::vertex_is_on_source_foil(a_vertex))
                    {
                      has_vertex = true;
                    }
                }

              if (!has_vertex)
                {
                  DT_LOG_DEBUG(get_logging_priority(), "A particle has no vertex on the foil !");
                  check_particle_vertex_on_foil = false;
                  break;
                }
            }
        }// end mode HAS_VERTEX_ON_FOIL

      cut_returned = cuts::SELECTION_REJECTED;
      if (check_non_associated_calorimeter_hits       &&
          check_range_non_associated_calorimeter_hits &&
          check_particle_charge                       &&
          check_has_particles                         &&
          check_range_particles                       &&
          check_particle_association                  &&
          check_particle_range_calorimeter_hits       &&
          check_particle_vertex_on_foil)
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
