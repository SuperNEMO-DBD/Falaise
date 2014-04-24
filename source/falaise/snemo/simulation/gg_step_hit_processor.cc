/** \file falaise/snemo/simulation/gg_step_hit_processor.cc */

// Ourselves:
#include <falaise/snemo/simulation/gg_step_hit_processor.h>

// Standard library:
#include <fstream>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/linear_system_solver.h>
// - Bayeux/geomtools:
#include <geomtools/intersection.h>
// - Bayeux/mctools:
#include <mctools/utils.h>

// This project:
#include <falaise/snemo/datamodels/gg_track_utils.h>

namespace snemo {

  namespace simulation {

    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT (gg_step_hit_processor,
                                                       "snemo::simulation::gg_step_hit_processor");

    bool gg_step_hit_processor::accept_external_rng () const
    {
      return true;
    }

    bool gg_step_hit_processor::has_external_rng () const
    {
      return _external_rng_ != 0;
    }

    void gg_step_hit_processor::set_external_rng (mygsl::rng & rng_)
    {
      DT_LOG_NOTICE (get_logging_priority (), "Processor '" << get_name () << "' now uses an external PRNG.");
      _external_rng_ = &rng_;
      return;
    }

    const mygsl::rng & gg_step_hit_processor::get_rng () const
    {
      if (has_external_rng ()) return *_external_rng_;
      return _rng_;
    }

    mygsl::rng & gg_step_hit_processor::grab_rng ()
    {
      if (has_external_rng ()) return *_external_rng_;
      return _rng_;
    }

    void gg_step_hit_processor::_set_defaults ()
    {
      _mapping_    = 0;
      _categories_ = 0;
      _external_rng_ = 0;
      _time_resolution_        = 25. * CLHEP::ns;
      _mean_ionization_energy_ = 50. * CLHEP::eV;
      datatools::invalidate (_fiducial_drift_radius_);
      datatools::invalidate (_fiducial_drift_length_);
      _use_continuous_ionization_         = false;
      _compute_minimum_approach_position_ = false;
      _store_track_infos_                 = false;
      _gg_cell_type_ = geomtools::geom_id::INVALID_TYPE;
      _module_type_  = geomtools::geom_id::INVALID_TYPE;
      _mapping_category_ = "";
      _module_category_ = "module";
      return;
    }

    void gg_step_hit_processor::reset ()
    {
      _set_defaults ();
      return;
    }

    gg_step_hit_processor::~gg_step_hit_processor ()
    {
      if (_gg_cell_type_ != geomtools::geom_id::INVALID_TYPE) {
      }
      if (is_debug ()) {
        DT_LOG_DEBUG (get_logging_priority (), "CT1:");
        _CT1_.tree_dump (std::clog);
        DT_LOG_DEBUG (get_logging_priority (), "CT2:");
        _CT2_.tree_dump (std::clog);
      }
      reset ();
      return;
    }

    gg_step_hit_processor::gg_step_hit_processor () : base_step_hit_processor ()
    {
      _set_defaults ();
      return;
    }

    void gg_step_hit_processor::initialize(const ::datatools::properties & config_,
                                           ::datatools::service_manager & service_mgr_)
    {
      std::cerr << "***** DEVEL: " << "gg_step_hit_processor::initialize: Entering..." << std::endl;
      this->base_step_hit_processor::initialize (config_, service_mgr_);

      // The geometry manager is mandatory for this processor:
      DT_THROW_IF (_geom_manager == 0, std::logic_error, "Missing geometry manager !");

      DT_LOG_DEBUG (get_logging_priority (), "Parsing setup properties...");

      // A sensitive category is mandatory for this processor,
      // i.e. the one defined for the target Geiger drift cell:
      DT_THROW_IF (! config_.has_key ("mapping.category"), std::logic_error,
                   "Missing 'mapping_category' string property !");
      _mapping_category_ = config_.fetch_string ("mapping.category");

      if (config_.has_key ("module.category")) {
        _module_category_ = config_.fetch_string ("module.category");
      }

      // force a continuous ionization along the step hit even for electrons :
      if (config_.has_flag ("use_continuous_ionization")) {
        _use_continuous_ionization_ = true;
      }

      // compute the position of minimum approach to the wire :
      if (config_.has_flag ("compute_minimum_approach_position")) {
        _compute_minimum_approach_position_ = true;
      }

      // store the track auxiliary properties from the original MC hits if any :
      if (config_.has_flag ("store_track_infos")) {
        _store_track_infos_ = true;
      }

      // set the RNG:
      if (! has_external_rng ()) {
        int rng_seed = 12345;
        if (config_.has_key ("rng.seed")) {
          rng_seed = config_.fetch_integer ("rng.seed");
        }
        std::string rng_id = "mt19937";
        if (config_.has_key ("rng.id")) {
          rng_id = config_.fetch_string ("rng.id");
        }
        // initialize the embedded pseudo-random numbers generator:
        _rng_.init (rng_id, rng_seed);
      }

      // set the time resolution of the Geiger cell TDC measurement:
      const double time_unit = CLHEP::ns;
      if (config_.has_key ("time_resolution")) {
        _time_resolution_ = config_.fetch_real ("time_resolution");
        if (! config_.has_explicit_unit ("time_resolution")) {
          _time_resolution_ *= time_unit;
        }
      }

      const double length_unit = CLHEP::mm;
      // set the fiducial drift radius of the Geiger cell:
      if (config_.has_key ("fiducial_drift_radius")) {
        _fiducial_drift_radius_ = config_.fetch_real ("fiducial_drift_radius");
        if (! config_.has_explicit_unit ("fiducial_drift_radius")) {
          _fiducial_drift_radius_ *= length_unit;
        }
      }
      // set the fiducial drift length of the Geiger cell:
      if (config_.has_key ("fiducial_drift_length")) {
        _fiducial_drift_length_ = config_.fetch_real ("fiducial_drift_length");
        if (! config_.has_explicit_unit ("fiducial_drift_length")) {
          _fiducial_drift_length_ *= length_unit;
        }
      }

      // set the mean ionization energy in the tracking gas:
      const double energy_unit = CLHEP::eV;
      if (config_.has_key ("mean_ionization_energy")) {
        _mean_ionization_energy_ = config_.fetch_real ("mean_ionization_energy");
        if (! config_.has_explicit_unit ("mean_ionization_energy")) {
          _mean_ionization_energy_ *= energy_unit;
        }
      }

      // pickup the ID mapping from the geometry manager:
      _mapping_ = &_geom_manager->get_mapping ();

      // check if the sensitive category is known:
      _categories_ = &(_geom_manager->get_id_mgr ().categories_by_name ());
      geomtools::id_mgr::categories_by_name_col_type::const_iterator icat
        = _categories_->find(_mapping_category_);
      DT_THROW_IF (icat == _categories_->end(), std::logic_error,
                   "Cannot find geometry ID category '" << _mapping_category_ << "' string property !");

      // initialize the ID locators for this category of volumes:
      _module_type_ = _categories_->find (_module_category_)->second.get_type ();
      _module_locator_.set_gmap (*_mapping_);
      _module_locator_.set_logging_priority (get_logging_priority ());
      _module_locator_.initialize (_module_type_);
      _gg_cell_type_ = _categories_->find (_mapping_category_)->second.get_type ();
      _gg_cell_locator_.set_gmap (*_mapping_);
      _gg_cell_locator_.set_logging_priority (get_logging_priority ());
      _gg_cell_locator_.initialize (_gg_cell_type_);
      std::cerr << "***** DEVEL: " << "gg_step_hit_processor::initialize: Basic smart locator for setup '"
                << _geom_manager->get_setup_label() << "'" << std::endl;

      // static bool pi = false;
      // if (! pi) {
      //   std::ofstream fpi("/tmp/gg_step_hit_processor.geom_ids.data");
      //   _gg_cell_locator_.print_infos(fpi);
      //   pi = true;
      // }

      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
        DT_LOG_TRACE (get_logging_priority (), "mapping_category = " << _mapping_category_);
        DT_LOG_TRACE (get_logging_priority (), "gg_cell_type     = " << _gg_cell_type_);
        DT_LOG_TRACE (get_logging_priority (), "Module locator:");
        _module_locator_.dump (std::cerr);
        DT_LOG_TRACE (get_logging_priority (), "Geiger locator:");
        _gg_cell_locator_.dump (std::cerr);
      }
      if (_geom_manager->get_setup_label().substr(0, 7) == "XXXsnemo::") {
        //
        // if (_geom_manager->get_setup_label() == "snemo") {

        std::cerr << "***** DEVEL: " << "gg_step_hit_processor::initialize: Fast locator for setup '"
                  << _geom_manager->get_setup_label() << "'" << std::endl;

        // 2012-05-04 FM : to be discarded
        {
          _fast_gg_cell_locator_.set_geo_manager(*_geom_manager);
          const uint32_t module_number = 0;
          DT_LOG_WARNING (get_logging_priority(), "Use default module number " << module_number);
          _fast_gg_cell_locator_.set_module_number(module_number);
          _fast_gg_cell_locator_.initialize();
        }

        // 2012-05-04 FM : now use the following
        {
          const std::list<const geomtools::geom_info *> & module_infos =
            _module_locator_.get_ginfos();
          for (std::list<const geomtools::geom_info *>::const_iterator i
                 = module_infos.begin();
               i !=  module_infos.end();
               i++) {
            const geomtools::geom_info * ginfo = *i;
            const uint32_t module_number = ginfo->get_geom_id().get(0);
            DT_LOG_NOTICE (get_logging_priority(), "Found module #" << module_number);
            {
              snemo::geometry::gg_locator tmp_fggloc;
              _fast_gg_cell_locators_per_module_[module_number] = tmp_fggloc;
            }
            snemo::geometry::gg_locator & fggloc
              = _fast_gg_cell_locators_per_module_[module_number];
            fggloc.set_geo_manager(*_geom_manager);
            fggloc.set_module_number(module_number);
            fggloc.initialize();
          }
        }
      }
      return;
    }


    bool gg_step_hit_processor::match_gg_hit (const mctools::base_step_hit & gg_hit_,
                                              const mctools::base_step_hit & step_hit_) const
    {
      // check the volume geometry ID:
      if (gg_hit_.get_geom_id () != step_hit_.get_geom_id ()) {
        // not in the same drift cell: no match
        return false;
      }

      // Particles are crossing very fastly a drift cell:
      // Ex:
      //   ~100 ps for an electron @ 1 MeV
      //   ~10 ns  for an alpha @ 1 MeV
      //
      // The clock of the tracker electronics is ~80MHz
      // and we expect a time resolution ~15 ns on the anode signal timestamp.
      // It means that we should not be able to discriminate
      // physical hits below this scale, so we consider here that
      // 2 first ionization ion/electron pairs created within
      // a 25 ns time window must be immediately compared to
      // keep only the one with minimal drift distance to the anode.

      // 2011-05-27 FM : nothing to do for now
      // if (gg_hit_.get_auxiliaries ()
      //     .has_flag (snemo::datamodel::gg_track::missing_geiger_hit_key()))
      //   {
      //     return true;
      //   }

      // Check if the step hit time is comparable to
      // the Geiger hit time using a confidence time window
      // @ time resolution:
      const double t1 = gg_hit_.get_time_start() - _time_resolution_;
      const double t2 = gg_hit_.get_time_start() + _time_resolution_;
      const double ta = step_hit_.get_time_start();
      const double tb = step_hit_.get_time_stop();
      /*
       *   The Current Geiger hit:
       *     Creation of a ion/electron pair
       *     with minimal drift distance
       *     to the anode
       *                  |
       *      t1          v          t2
       *  -----[----------:----------]--------------> time
       *            time resolution
       *       <--------------------->
       *
       *   A step hit:
       *     Drift distances from  the
       *     ion/electron pairs created
       *     along this step hit will be
       *     compared to the one in
       *     the current Geiger hit
       *     ==> MATCH
       *           ta     tb              ta'   tb'
       *      -------[----]----------------[----]--->
       *
       *                          Another step hit:
       *                            This ion/electron pair occurs
       *                            too long after the current Geiger hit.
       *                            It consists on a different
       *                            'physical' Geiger hit whom
       *                            ion/electron pairs should
       *                            not be compared to the one in
       *                            the current Geiger hit
       *                            ==> NO MATCH
       *
       */

      if (tb < t1) {
        /* Far before the Geiger hit time => NO MATCH
         *
         *  ta   tb  t1                 t2
         *  -[-+-]----[--------+--------]-------------> time
         *
         */
        return false;
      }
      if (ta > t2) {
        /* Far after the Geiger hit time => NO MATCH
         *
         *    t1                     t2  ta   tb
         *  ---[----------+----------]----[-+-]-------> time
         *
         */
        return false;
      }

      return true;
    }

    void gg_step_hit_processor::process(
      const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
      ::mctools::simulated_data::hit_collection_type & the_plain_gg_hits)
    {
      _process(the_base_step_hits, (mctools::simulated_data::hit_handle_collection_type *) 0, &the_plain_gg_hits);
      return;
    }

    void gg_step_hit_processor::process(
      const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
      ::mctools::simulated_data::hit_handle_collection_type & the_gg_hits)
    {
      _process(the_base_step_hits, &the_gg_hits, (mctools::simulated_data::hit_collection_type *) 0);
      return;
    }

    void gg_step_hit_processor::_process (const mctools::base_step_hit_processor::step_hit_ptr_collection_type & shpc_,
                                          mctools::simulated_data::hit_handle_collection_type * gg_hits_,
                                          mctools::simulated_data::hit_collection_type        * plain_gg_hits_)
    {
      DT_LOG_TRACE (get_logging_priority (), "Entering...");

      // Check the type of output collection (handles or plain hits) :
      bool use_handles = false;
      if (gg_hits_ != 0) {
        use_handles = true;
      } else if (plain_gg_hits_ == 0) {
        DT_THROW_IF (true, std::logic_error, "Missing hit collection type (NULL pointer) !");
      }
      mctools::base_step_hit * current_gg_hit = 0;
      uint32_t gg_hit_count = 0;

      // Traverses the whole list of step hits and computes for each of them
      // the position/time of the ion/electron pair creation closest to the
      // anode wire:
      // Prereservation :
      if (use_handles) {
        gg_hits_->reserve(100);
      } else {
        plain_gg_hits_->reserve(100);
      }

      // Debug stuff:
      size_t missing_hits_count = 0;
      DT_LOG_TRACE (get_logging_priority (), "shpc.size = " << shpc_.size ());

      double locator_tolerance = 0.1 * CLHEP::micrometer;

      for (mctools::base_step_hit_processor::step_hit_ptr_collection_type::const_iterator
             ihit = shpc_.begin (); ihit != shpc_.end (); ++ihit) {
        bool process_this_hit = false;
        mctools::base_step_hit & the_step_hit = const_cast<mctools::base_step_hit &> (*(*ihit));

        if (is_debug ()) _CT1_.start ();
        const double        hit_energy_deposit = the_step_hit.get_energy_deposit ();
        // XXX
        // if (hit_energy_deposit == 0) continue;
        const std::string & hit_particle_name  = the_step_hit.get_particle_name ();
        const double        hit_time_start     = the_step_hit.get_time_start ();
        const double        hit_time_stop      = the_step_hit.get_time_stop ();
        // step hit start/stop in world coordinate frame (WCF):
        const geomtools::vector_3d & world_hit_pos_start  = the_step_hit.get_position_start ();
        const geomtools::vector_3d & world_hit_pos_stop   = the_step_hit.get_position_stop ();
        const geomtools::vector_3d & world_hit_momentum_start = the_step_hit.get_momentum_start ();
        if (! process_this_hit && hit_energy_deposit >= 0.0) {
          process_this_hit = true;
        }
        /*
          if (! process_this_hit && hit_particle_name == "e-")
          {
          process_this_hit = true;
          }
          if (! process_this_hit && hit_particle_name == "alpha")
          {
          process_this_hit = true;
          }
          if (! process_this_hit && hit_particle_name == "e+")
          {
          process_this_hit = true;
          }
        */
        DT_LOG_TRACE (get_logging_priority (), "process_this_hit = " << process_this_hit);
        if (! process_this_hit) {
          continue;
        }

        // locate the hit median position using a 'geom ID' locator:
        const geomtools::vector_3d world_hit_pos_median = 0.5 * (world_hit_pos_start + world_hit_pos_stop);
        geomtools::geom_id gid;
        if (! _fast_gg_cell_locators_per_module_.empty ()) {
          //DT_LOG_WARNING (get_logging_priority (), "Using fast_gg_cell_locator per module...");
          const geomtools::geom_id & module_gid = _module_locator_.get_geom_id (world_hit_pos_median, _module_type_);
          const uint32_t module_number = module_gid.get (0);
          DT_THROW_IF (_fast_gg_cell_locators_per_module_.find (module_number) == _fast_gg_cell_locators_per_module_.end (),
                       std::logic_error,
                       "Cannot find module number '" << module_number << "' from the fast gg cell locator dictionary !");
          // 2012-06-05 FM : add 'find_cell_geom_id' method's returned value check:
          const bool find_success =
            _fast_gg_cell_locators_per_module_[module_number].find_cell_geom_id (world_hit_pos_median, gid);
          if (! find_success) {
            gid.invalidate ();
          }
        } else if (_fast_gg_cell_locator_.is_initialized ()) {
          //DT_LOG_WARNING (get_logging_priority (), "Using fast_gg_cell_locator...");
          // 2012-06-05 FM : add 'find_cell_geom_id' method's returned value check:
          bool find_success =
            _fast_gg_cell_locator_.find_cell_geom_id (world_hit_pos_median, gid, locator_tolerance);
          if (! find_success) {
            gid.invalidate ();
          }
        } else {
          //DT_LOG_WARNING (get_logging_priority (), "Using basic gg_cell_locator...");
          gid = _gg_cell_locator_.get_geom_id (world_hit_pos_median, _gg_cell_type_, locator_tolerance);
          if (! gid.is_valid ()) {
            std::cerr << "***** DEVEL: " << "gg_step_hit_processor::initialize: "
                      << "Invalid GID from basic locator "
                      << std::endl;
          }
        }

        if (! gid.is_valid ()) {
          the_step_hit.tree_dump(std::clog, "Current step hit: ", "WARNING: ");
          DT_LOG_WARNING (get_logging_priority (),
                          "world_hit_pos_median = " << std::setprecision(15)  << world_hit_pos_median / CLHEP::mm << " mm");
          // We do not process such a hit: Should we consider this case as
          // a bug ?
          DT_LOG_WARNING (get_logging_priority (),
                          "We skip this hit for one cannot locate it through the locator attached to the '"
                          << _mapping_category_ << "' ! "
                          << " This is probably due to another mapping category registered in the current '"
                          << _hit_category << "' hit category "
                          << "that may generate its own step hits ! Consider to write your own hit processor able "
                          << "to handle several mapping categories (using several suitable locators) !");
          continue;
        }
        // set the geometry ID of this step hit:
        the_step_hit.set_geom_id (gid);

        // extract geometry information about the drift cell with this ID:
        // 2012-06-04 FM : added for debugging purpose :
        const geomtools::geom_info * ginfo_ptr = 0;
        try {
          const geomtools::geom_info & ginfo = _gg_cell_locator_.get_geom_info (gid);
          ginfo_ptr = &ginfo;
        }
        catch(std::exception & x) {
          DT_LOG_WARNING (get_logging_priority (),
                          "Possible bug !!! Cannot find GID = '" << gid
                          << "' with the GG locator ! exception = " << x.what ());
          DT_LOG_WARNING (get_logging_priority (), "Buggy step:");
          the_step_hit.tree_dump (std::clog);
          throw x;
        }
        const geomtools::geom_info & ginfo = *ginfo_ptr;

        // get the placement of the drift cell in WCF:
        const geomtools::placement & cell_world_plcmt = ginfo.get_world_placement ();

        geomtools::vector_3d cell_hit_pos_start;
        geomtools::vector_3d cell_hit_pos_stop;
        // compute the start/stop step hit position in the cell coordinates frame (DCCF):
        cell_world_plcmt.mother_to_child (world_hit_pos_start, cell_hit_pos_start);
        cell_world_plcmt.mother_to_child (world_hit_pos_stop, cell_hit_pos_stop);

        // default for electrons and positron tracks:
        bool randomize_first_ionisation = true;
        bool store_discrete = true;
        if (hit_particle_name == "alpha") {
          // special processing for alpha tracks:
          randomize_first_ionisation = false;
          store_discrete = false;
        }

        // special behaviour is requested by user :
        if (_use_continuous_ionization_) {
          randomize_first_ionisation = false;
          store_discrete = false;
        }

        // Define the time and position of the (first) ionization
        // nearest to the anodic wire and the impact position
        // on the Geiger avalanche on the wire:
        double               ionization_time_discrete;
        geomtools::vector_3d ionization_world_pos_discrete;
        geomtools::vector_3d ionization_world_momentum_discrete; // 2011-12-08 FM: added
        geomtools::vector_3d avalanche_impact_world_pos_discrete;
        // ... initialized them as invalid:
        datatools::invalidate (ionization_time_discrete);
        geomtools::invalidate (ionization_world_pos_discrete);
        geomtools::invalidate (avalanche_impact_world_pos_discrete);

        double               ionization_time_continuous;
        geomtools::vector_3d ionization_world_pos_continuous;
        geomtools::vector_3d ionization_world_momentum_continuous; // 2011-12-08 FM: added
        geomtools::vector_3d avalanche_impact_world_pos_continuous;
        // ... initialized them as invalid:
        datatools::invalidate (ionization_time_continuous);
        geomtools::invalidate (ionization_world_pos_continuous);
        geomtools::invalidate (avalanche_impact_world_pos_continuous);

        // Minimum approach :
        double minimum_approach_time;
        double minimum_approach_world_distance;
        geomtools::vector_3d minimum_approach_world_pos;
        datatools::invalidate (minimum_approach_time);
        datatools::invalidate (minimum_approach_world_distance);
        geomtools::invalidate (minimum_approach_world_pos);

        /* Now we must compute the position and time of the first ionization process
         * along the step hit that is the closest to the anodic wire
         * in XY plane (DCCF).
         *
         * Active/fiducial drift region:
         *  If a fiducial drift volume is defined (given a fiducial drift radius of the cell
         *  and a fiducial length of the cell), we consider only
         *  ionization within it, otherwise the full cell volume
         *  is considered as active.
         *
         * Two algorithms are available:
         *  (a) for e-/e+ (low ionizing particle in the tracking gas)
         *  (b) for alpha (highly ionizing particle in the tracking gas)
         *
         * Note:
         *   - Computations are done in the current drift cell coordinates frame (DCCF).
         *   - Results are provided in the world coordinates frame (WCF).
         */

        bool algo_discrete = true;
        bool algo_continuous = true;
        if (randomize_first_ionisation) {
          algo_discrete = true;
          algo_continuous = false;
        } else {
          algo_discrete = false;
          algo_continuous = true;
        }
        if (_compute_minimum_approach_position_) {
          algo_continuous = true;
        }

        //bool has_ionization = false;
        if (algo_discrete) {
          /* Algorithm (a)
           *
           * For low ionizing particle (e-/e+), ionization is considered as
           * a discrete process with a small amount of electron/ion pairs
           * created along the step using Poisson statistics. Consequently, we expect
           * fluctuations is the reconstruction of the circle tangential to the track.
           */

          // mean number of first ionizations along the current step hit:
          const double mean_ionizations = hit_energy_deposit / _mean_ionization_energy_;

          // shoot the effective number of first ionizations:
          const size_t number_of_ionizations = grab_rng().poisson(mean_ionizations);

          DT_LOG_TRACE (get_logging_priority (), "Step hit #" << the_step_hit.get_hit_id ());
          DT_LOG_TRACE (get_logging_priority (), "  Particle = " << hit_particle_name);
          DT_LOG_TRACE (get_logging_priority (), "  Hit energy deposit           = " << hit_energy_deposit / CLHEP::keV << " keV");
          DT_LOG_TRACE (get_logging_priority (), "  Mean number of ionizations   = " << mean_ionizations << " ion/electron pairs");
          DT_LOG_TRACE (get_logging_priority (), "  Random number of ionizations = " << number_of_ionizations << " ion/electron pairs");

          // the step hit 3D vector:
          geomtools::vector_3d step_dir = (cell_hit_pos_stop - cell_hit_pos_start);

          // the minimal drift distance for the current step hit:
          double min_drift_distance;
          // ... invalidated first:
          datatools::invalidate (min_drift_distance);

          /* Scheme of the current step hit (XY view in DCCF)
           * and the generation of randomly distributed ion/pair
           * creation vertex along the hit segment:
           *
           *                     step hit segment
           *
           *                            M (minimum approach)
           *           start                          stop
           *      --------+==:=:==:=====*====:==:===>+----> track direction
           *             t1       t     :   / t      t2
           *                            :  /
           *                            : / d: shortest drift distance
           *                            :/
           *                            +
           *                             C (the cell center == origin in DCCF)
           */
          //if (number_of_ionizations > 0) has_ionization = true;

          // loop on ionizations:
          for (size_t ion = 0; ion < number_of_ionizations; ion++) {
            // shoot time and vertex:
            const double ran = grab_rng ().uniform ();
            const double ran_ionization_time = hit_time_start + ran * (hit_time_stop - hit_time_start);
            const geomtools::vector_3d ran_ionization_pos = cell_hit_pos_start + ran * step_dir;

            const double hit_x = ran_ionization_pos.x ();
            const double hit_y = ran_ionization_pos.y ();
            const double hit_z = ran_ionization_pos.z ();

            // compute the drift distance to the anodic wire:
            const double drift_distance = hypot (hit_x, hit_y);

            // compute the impact of the Geiger avalanche on the anodic wire (DCCF):
            geomtools::vector_3d avalanche_impact_cell_pos (0.0, 0.0, hit_z);

            // Check if the ionization occurs in a fiducial cylinder (if requested):

            // In the drift cell XY plane:
            if (datatools::is_valid (_fiducial_drift_radius_)) {
              // not in the fiducial drift X-Y circular region:
              if (drift_distance > _fiducial_drift_radius_) {
                // drop this ion/electron pair which will not
                // produce a Geiger avalanche and thus is sterile:
                continue;
              }
            }

            // Along the drift cell Z axis:
            if (datatools::is_valid (_fiducial_drift_length_)) {
              // not in the fiducial drift Z longitudinal region:
              // drop this ion/electron pair which will not
              // produce a Geiger avalanche and thus is sterile.
              if (std::abs (hit_z) > 0.5 * _fiducial_drift_length_) continue;
            }

            if (! datatools::is_valid (min_drift_distance)) {
              // The first processed ion/electron pair is accepted as
              // the closest to the anodic wire (so far):
              min_drift_distance = drift_distance;
              // record the time of the pair creation:
              ionization_time_discrete = ran_ionization_time;
              // record its position:
              ionization_world_pos_discrete = world_hit_pos_start
                + ran * (world_hit_pos_stop - world_hit_pos_start);
              // 2011-12-08 FM: added
              ionization_world_momentum_discrete = world_hit_momentum_start;
              // compute the impact of the Geiger avalanche
              // on the anodic wire (WCS):
              cell_world_plcmt.child_to_mother (avalanche_impact_cell_pos,
                                                avalanche_impact_world_pos_discrete);
            } else {
              if (drift_distance < min_drift_distance) {
                // This ion/electron pair is closest than the
                // current best candidate:
                min_drift_distance = drift_distance;
                // So we update now the ionization time and position
                // with this new pair:
                ionization_time_discrete = ran_ionization_time;
                ionization_world_pos_discrete = world_hit_pos_start
                  + ran * (world_hit_pos_stop - world_hit_pos_start);
                // 2011-12-08 FM: added
                ionization_world_momentum_discrete = world_hit_momentum_start;
                // compute the impact of the Geiger avalanche
                // on the anodic wire (WCS):
                cell_world_plcmt.child_to_mother (avalanche_impact_cell_pos,
                                                  avalanche_impact_world_pos_discrete);
              }
            }
          } // for

          // at this stage, the space/time coordinates of the best ion/electron
          // pair is available through:
          //   'ionization_time'
          //   'ionization_world_pos'
          //   'avalanche_impact_world_pos'
          // unless no first ionization ion/electron pair was produced for the
          // current step hit.

        } // end of 'if (algo_discrete)'

        if (algo_continuous) {
          /* Algorithm (b)
           *
           * For highly ionizing particle (alpha), ionization is considered as
           * a continuous process with a large amount of electron/ion pairs
           * created along the step:
           */

          // initial boundaries of the step hit (DCCF):
          geomtools::vector_2d s1 (cell_hit_pos_start.x (), cell_hit_pos_start.y ());
          geomtools::vector_2d s2 (cell_hit_pos_stop.x (), cell_hit_pos_stop.y ());

          /* Scheme of a typical step hit (XY view in DCCF):
           *
           *               step hit segment
           *             S1                S2
           *      --------+===============>+--------> track direction
           *            t1 ~~~~~~~~~~~~~~~    t2
           *                 ~~~~~~~~~~~~
           *                   ~~~~~~~~~ <-- the Geiger avalanche
           *                     ~~~~~             region
           *                       ~~
           *                        +
           *                          C (the cell center == origin in DCCF)
           */

          // If fiducial drift Z is defined:
          if (datatools::is_valid (_fiducial_drift_length_)) {
            // approximation (could be refined but at the price of tricky geometrics):
            const double mean_hit_z = 0.5 * (cell_hit_pos_start.z () + cell_hit_pos_stop.z ());
            // not in the fiducial drift Z longitudinal region:
            if (std::abs (mean_hit_z) > 0.5 * _fiducial_drift_length_) {
              // consider this step as sterile
              // => no ionization is generated
              continue; //skip to next step hit
            }
          }

          // If fiducial drift radius is defined (XY plane),
          if (datatools::is_valid (_fiducial_drift_radius_)) {
            /* We recompute the effective boundaries of the step hit.
             * We must find the active part/segment of the
             * step hit segment that belongs to the circular fiducial
             * drift region.
             */

            // compute start=S1 and stop=S2 of the step inside
            // the fiducial drift cylindric volume:
            geomtools::vector_2d effective_s1, effective_s2;
            geomtools::vector_2d cell_center (0., 0.);
            if (geomtools::intersection::find_intersection_segment_disk_2d (s1,
                                                                            s2,
                                                                            cell_center,
                                                                            _fiducial_drift_radius_,
                                                                            effective_s1,
                                                                            effective_s2)) {
              // update the fiducial part of the step hit segment:
              // this remove the part(s) of the step hit that lies
              // outside the drift region:
              s1 = effective_s1;
              s2 = effective_s2;
            } else {
              // step segment (s1,s2) is totally outside the fiducial
              // drift volume (XY plane)
              // => no ionization is generated
              continue; //skip to next step hit
            }
          }

          /* compute the coordinates of the point H along the (S1,S2) axis
           * that is the closest to the anodic wire (cell center C in DCCF):
           */
          const geomtools::vector_2d s1s2 = s2 - s1;
          const geomtools::vector_2d u = s1s2.unit ();
          double a1, b1, c1;
          double a2, b2, c2;
          double xh, yh;
          a1 = u.x ();
          b1 = u.y ();
          c1 = 0.0;
          a2 = u.y ();
          b2 = -u.x ();
          c2 = s1.x () * u.y () - s1.y () * u.x ();
          if (mygsl::linear_system_2x2_solve (a1, b1, c1, a2, b2, c2, xh, yh)) {
            const geomtools::vector_2d h (xh, yh);
            const geomtools::vector_2d s1h = h - s1;

            if (((s1h.dot (u) > 0.0) && (s1h.mag () > s1s2.mag ()))
                || (s1h.dot (u) < 0.0)) {
              // H does not belong to the step hit segment (S1,S2):
              const double d1 = s1.mag ();
              const double d2 = s2.mag ();
              if (d1 < d2) {
                /* Step start is the nearest:
                 *
                 *          H   S1                S2
                 *      ----+----+================+------------>
                 *          :   / t1              t2
                 *          :  /
                 *          : / d1
                 *          :/
                 *        C +
                 */
                ionization_time_continuous = hit_time_start;
                ionization_world_pos_continuous = world_hit_pos_start;
                // 2011-12-08 FM: added
                ionization_world_momentum_continuous = world_hit_momentum_start;
                const geomtools::vector_3d avalanche_impact_cell_pos (0.0,
                                                                      0.0,
                                                                      cell_hit_pos_start.z ());

                cell_world_plcmt.child_to_mother (avalanche_impact_cell_pos,
                                                  avalanche_impact_world_pos_continuous);
              } else {
                /* Step stop is the nearest:
                 *
                 *             S1                S2   H
                 *      --------+================+----+---------->
                 *             t1              t2 \   :
                 *                                 \  :
                 *                              d2  \ :
                 *                                   \:
                 *                                    + C
                 */
                ionization_time_continuous = hit_time_stop;
                ionization_world_pos_continuous = world_hit_pos_stop;
                // 2011-12-08 FM: added
                ionization_world_momentum_continuous = world_hit_momentum_start;
                const geomtools::vector_3d avalanche_impact_cell_pos (0.0,
                                                                      0.0,
                                                                      cell_hit_pos_stop.z ());
                cell_world_plcmt.child_to_mother (avalanche_impact_cell_pos,
                                                  avalanche_impact_world_pos_continuous);
              }
            } else {
              /* H is the nearest:
               *
               *                step prop
               *              <---------->
               *             S1          H      S2
               *      --------+==========+======+-------->
               *             t1          : t    t2
               *                         :
               *                       d :
               *                         :
               *                         + C
               */
              const double step_prop = s1h.mag () / s1s2.mag ();
              // compute the time of nearest ionization:
              ionization_time_continuous = hit_time_start
                + step_prop * (hit_time_stop - hit_time_start);
              // compute the position of nearest ionization in (WCF):
              ionization_world_pos_continuous = world_hit_pos_start
                + step_prop * (world_hit_pos_stop - world_hit_pos_start);
              // 2011-12-08 FM: added
              ionization_world_momentum_continuous = world_hit_momentum_start;
              const double zh = cell_hit_pos_start.z ()
                + step_prop * (cell_hit_pos_stop.z () - cell_hit_pos_start.z ());
              const geomtools::vector_3d avalanche_impact_cell_pos (0.0,
                                                                    0.0,
                                                                    zh);
              cell_world_plcmt.child_to_mother (avalanche_impact_cell_pos,
                                                avalanche_impact_world_pos_continuous);
            }
          }

          if (_compute_minimum_approach_position_) {
            minimum_approach_time = ionization_time_continuous;
            minimum_approach_world_pos = ionization_world_pos_continuous;
            minimum_approach_world_distance
              = (avalanche_impact_world_pos_continuous - ionization_world_pos_continuous).mag ();
          }
        } // end of if (algo_continuous)

        double               ionization_time;
        geomtools::vector_3d ionization_world_pos;
        geomtools::vector_3d ionization_world_momentum; // 2011-12-08 FM: added
        geomtools::vector_3d avalanche_impact_world_pos;
        // ... initialized them as invalid:
        datatools::invalidate (ionization_time);
        geomtools::invalidate (ionization_world_pos);
        geomtools::invalidate (ionization_world_momentum);
        geomtools::invalidate (avalanche_impact_world_pos);

        if (algo_discrete && store_discrete) {
          // restore solution of the discrete ionization algo :
          ionization_time = ionization_time_discrete;
          ionization_world_pos = ionization_world_pos_discrete;
          ionization_world_momentum = ionization_world_momentum_discrete;
          avalanche_impact_world_pos = avalanche_impact_world_pos_discrete;
        } else if (algo_continuous) {
          ionization_time = ionization_time_continuous;
          ionization_world_pos = ionization_world_pos_continuous;
          ionization_world_momentum = ionization_world_momentum_continuous;
          avalanche_impact_world_pos = avalanche_impact_world_pos_continuous;
        }

        // Give up with this step hit if no ion/electron pair
        // was generated somewhere in the drift cell volume:
        bool do_stop_this_hit = false;
        if (! datatools::is_valid (ionization_time) && ! _compute_minimum_approach_position_) {
          do_stop_this_hit = true;
        }

        // skip this hit :
        if (do_stop_this_hit) {
          continue;
        }

        if (is_debug ()) _CT1_.stop ();

        /* Now we try to merge the current step hit
         * within one of the recorded candidate Geiger hits.
         * We first start by searching an existing Geiger hit
         * that matches: same drift cell and compatible times.
         */

        if (is_debug ()) _CT2_.start ();
        // For efficiency we first search a match with
        // the current Geiger hit (if any):
        mctools::base_step_hit * matching_gg = 0;
        if (current_gg_hit != 0) {
          if (match_gg_hit (*current_gg_hit, the_step_hit)) {
            matching_gg = current_gg_hit;
          }
        }
        // else we scan the whole list of gg hits to find a match :
        if (matching_gg == 0) {
          if (use_handles) {
            for (mctools::simulated_data::hit_handle_collection_type::iterator igg
                   = gg_hits_->begin ();
                 igg != gg_hits_->end ();
                 igg++) {
              if (! igg->has_data ()) continue;
              mctools::base_step_hit & matching_hit = igg->grab ();
              if (match_gg_hit (matching_hit, the_step_hit)) {
                // pick up the first matching gg hit :
                matching_gg = &matching_hit;
                break;
              }
            }
          } else {
            for (mctools::simulated_data::hit_collection_type::iterator igg
                   = plain_gg_hits_->begin ();
                 igg != plain_gg_hits_->end ();
                 igg++) {
              mctools::base_step_hit & matching_hit = *igg;
              if (match_gg_hit (matching_hit, the_step_hit)) {
                // pick up the first matching gg hit :
                matching_gg = &matching_hit;
                break;
              }
            }
          }
        }

        if (matching_gg == 0) {
          // If the current step hit does not match any candidate
          // Geiger hit, we insert a new candidate Geiger hit:

          if (use_handles) {
            // pickup a hit from the pool and add it at the end
            // of the collection of hits (handles) :
            add_new_hit (*gg_hits_);
            // get a reference to the GG hit :
            current_gg_hit = &(gg_hits_->back ().grab ());
          } else {
            // add a new hit in the plain collection :
            mctools::base_step_hit dummy;
            plain_gg_hits_->push_back (dummy);
            // get a reference to the last inserted GG hit :
            current_gg_hit = &(plain_gg_hits_->back ());
          }
          // update the attributes of the hit :
          current_gg_hit->set_hit_id (gg_hit_count);
          current_gg_hit->set_geom_id (gid);
          current_gg_hit->set_particle_name (hit_particle_name);
          if (datatools::is_valid (ionization_time)) {
            current_gg_hit->set_time_start (ionization_time);
            current_gg_hit->set_position_start (ionization_world_pos);
            current_gg_hit->set_position_stop (avalanche_impact_world_pos);
            // 2011-12-08 FM: record the momentum at ionization point
            current_gg_hit->set_momentum_start (ionization_world_momentum);
          } else {
            if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
              DT_LOG_TRACE (get_logging_priority (), "Add '" << snemo::datamodel::gg_track::missing_geiger_hit_flag()  << "'!");
              missing_hits_count++;
            }
            current_gg_hit->set_time_start (minimum_approach_time);
            current_gg_hit->grab_auxiliaries ()
              .store_flag (snemo::datamodel::gg_track::missing_geiger_hit_flag());
          }

          if (_compute_minimum_approach_position_
              && datatools::is_valid (minimum_approach_world_distance)) {
            current_gg_hit->grab_auxiliaries ()
              .store (snemo::datamodel::gg_track::minimum_approach_distance_key(),
                      minimum_approach_world_distance);
            std::vector<double> minimum_approach_world_pos_vec;
            minimum_approach_world_pos_vec.reserve (3);
            for (int i = 0; i < 3; i++) {
              minimum_approach_world_pos_vec.push_back (minimum_approach_world_pos[i]);
            }
            current_gg_hit->grab_auxiliaries ()
              .store (snemo::datamodel::gg_track::minimum_approach_position_key(),
                      minimum_approach_world_pos_vec);
          }

          if (_store_track_infos_) {
            if (the_step_hit.has_track_id ()) {
              current_gg_hit->grab_auxiliaries ().store (mctools::track_utils::TRACK_ID_KEY,
                                                         the_step_hit.get_track_id ());
            }
            if (the_step_hit.has_parent_track_id ()) {
              current_gg_hit->grab_auxiliaries ().store (mctools::track_utils::PARENT_TRACK_ID_KEY,
                                                         the_step_hit.get_parent_track_id ());
            }
          }
          // store primary particle information
          const bool is_primary_particle
            = the_step_hit.get_auxiliaries ()
            .has_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
          if (is_primary_particle) {
            current_gg_hit->grab_auxiliaries ()
              .store_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
          }
          //increment the gg id:
          gg_hit_count++;
        } else {
          // If there is a match with a candidate Geiger hit, we first make it
          // the current one:
          current_gg_hit = matching_gg;

          /* Then we check if the Geiger avalanche of the current step hit is
           * shorter than the Geiger avalanche from the current Geiger hit.
           */
          bool update = false;
          if (current_gg_hit->get_auxiliaries ()
              .has_flag(snemo::datamodel::gg_track::missing_geiger_hit_flag())) {
            if (datatools::is_valid (ionization_time)) {
              if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
                DT_LOG_TRACE (get_logging_priority (), "Found ionization ! Remove '" << snemo::datamodel::gg_track::missing_geiger_hit_flag() << "' flag !");
                missing_hits_count--;
              }
              update = true;
              current_gg_hit->grab_auxiliaries ()
                .erase (snemo::datamodel::gg_track::missing_geiger_hit_flag());
            } else {
              // nothing here
            }
          } else {
            // compute the current step hit drift distance:
            const double step_hit_drift_distance
              = (avalanche_impact_world_pos - ionization_world_pos).mag ();

            // compute the current Geiger hit drift distance:
            const double gg_hit_drift_distance
              = (current_gg_hit->get_position_start ()
                 - current_gg_hit->get_position_stop ()).mag ();

            if (step_hit_drift_distance < gg_hit_drift_distance) {
              update = true;
            }
          }
          if (update) {
            // The current step hit is better than the current candidate Geiger hit,
            // so we update the Geiger hit informations:
            current_gg_hit->set_particle_name (hit_particle_name);
            current_gg_hit->set_position_start (ionization_world_pos);
            current_gg_hit->set_position_stop (avalanche_impact_world_pos);
            // 2011-12-08 FM: record the momentum at ionization point
            current_gg_hit->set_momentum_start (ionization_world_momentum);
            current_gg_hit->set_time_start (ionization_time);
            if (_store_track_infos_) {
              if (the_step_hit.has_track_id ()) {
                current_gg_hit->grab_auxiliaries ().update (mctools::track_utils::TRACK_ID_KEY,
                                                            the_step_hit.get_track_id ());
              }
              if (the_step_hit.has_parent_track_id ()) {
                current_gg_hit->grab_auxiliaries ().update (mctools::track_utils::PARENT_TRACK_ID_KEY,
                                                            the_step_hit.get_parent_track_id ());
              }
            }
            // store primary particle information
            const bool is_primary_particle
              = the_step_hit.get_auxiliaries ()
              .has_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
            if (is_primary_particle) {
              current_gg_hit->grab_auxiliaries ()
                .update_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
            }
          }
          if (_compute_minimum_approach_position_
              && datatools::is_valid (minimum_approach_world_distance)) {
            if (! current_gg_hit->get_auxiliaries ().has_key (snemo::datamodel::gg_track::minimum_approach_distance_key())) {
              current_gg_hit->grab_auxiliaries ().store (snemo::datamodel::gg_track::minimum_approach_distance_key(),
                                                         minimum_approach_world_distance);
              std::vector<double> minimum_approach_world_pos_vec;
              minimum_approach_world_pos_vec.reserve (3);
              for (int i = 0; i < 3; i++) {
                minimum_approach_world_pos_vec.push_back (minimum_approach_world_pos[i]);
              }
              current_gg_hit->grab_auxiliaries ().store (snemo::datamodel::gg_track::minimum_approach_position_key(),
                                                         minimum_approach_world_pos_vec);
            } else {
              const double current_minimum_approach_distance
                = current_gg_hit->get_auxiliaries ()
                .fetch_real (snemo::datamodel::gg_track::minimum_approach_distance_key());
              if (minimum_approach_world_distance < current_minimum_approach_distance) {
                current_gg_hit->grab_auxiliaries ()
                  .update (snemo::datamodel::gg_track::minimum_approach_distance_key(),
                           minimum_approach_world_distance);
                std::vector<double> minimum_approach_world_pos_vec;
                minimum_approach_world_pos_vec.reserve (3);
                for (int i = 0; i < 3; i++) {
                  minimum_approach_world_pos_vec.push_back (minimum_approach_world_pos[i]);
                }
                current_gg_hit->grab_auxiliaries ()
                  .update (snemo::datamodel::gg_track::minimum_approach_position_key(),
                           minimum_approach_world_pos_vec);
              }
            }
          }
        }
        if (is_debug ()) _CT2_.stop ();
      }// end of the loop : for (i_step_hit_processor::step_hit_ptr_collection_type::const_iterator ihit...

      // Purge the output collection of gg hits from hits with the 'missing_geiger_hit_flag()' :
      if (_compute_minimum_approach_position_) {
        if (missing_hits_count > 0) {
          DT_LOG_TRACE (get_logging_priority (), "Missing hits count = " << missing_hits_count);
        } else {
          DT_LOG_TRACE (get_logging_priority (), "No count with missing hits left !");
        }

        _purge_gg_hits (gg_hits_, plain_gg_hits_);
      }
      DT_LOG_TRACE (get_logging_priority (), "Exiting.");
      return;
    }

    void gg_step_hit_processor::_purge_gg_hits (mctools::simulated_data::hit_handle_collection_type * gg_hits_,
                                                mctools::simulated_data::hit_collection_type        * plain_gg_hits_)
    {
      // Check the type of output collection (handles or plain hits) :
      bool use_handles = false;
      if (gg_hits_ != 0) {
        use_handles = true;
      } else if (plain_gg_hits_ == 0) {
        DT_THROW_IF (true, std::logic_error, "Missing hit collection type (NULL pointer) !");
      }

      static const std::string KILL_HIT_FLAG = "__kill_hit";
      geomtools::base_hit::has_flag_predicate kill_hit_pred (KILL_HIT_FLAG);
      // wrapper predicate :
      datatools::mother_to_daughter_predicate<geomtools::base_hit, mctools::base_step_hit> kill_hit_pred_M2D (kill_hit_pred);

      if (use_handles)
        {
          for (mctools::simulated_data::hit_handle_collection_type::iterator i = gg_hits_->begin ();
               i != gg_hits_->end ();
               ++i) {
            mctools::simulated_data::hit_handle_type & the_hit_handle = *i;
            if (the_hit_handle.has_data ()) {
              mctools::base_step_hit & the_hit = the_hit_handle.grab ();
              if (the_hit.grab_auxiliaries ()
                  .has_flag (snemo::datamodel::gg_track::missing_geiger_hit_flag())) {
                DT_LOG_TRACE (get_logging_priority (), "Found a handle hit with '" << snemo::datamodel::gg_track::missing_geiger_hit_flag() << "' !");
                the_hit.grab_auxiliaries ().store_flag (KILL_HIT_FLAG);
              }
            }
          }
          // wrapper predicate :
          mctools::simulated_data::hit_handle_type::predicate_type handle_kill_hit_pred (kill_hit_pred_M2D);
          mctools::simulated_data::hit_handle_collection_type::iterator new_end = std::remove_if (gg_hits_->begin (),
                                                                                                  gg_hits_->end (),
                                                                                                  handle_kill_hit_pred);
          if (new_end != gg_hits_->end ()) {
            DT_LOG_TRACE (get_logging_priority (), "Found some handle hits with the KILL_HIT_FLAG ! Kill them !");
            gg_hits_->erase (new_end, gg_hits_->end ());
          }
        } else {
        for (mctools::simulated_data::hit_collection_type::iterator i = plain_gg_hits_->begin ();
             i != plain_gg_hits_->end ();
             ++i) {
          mctools::base_step_hit & the_hit = *i;
          if (the_hit.get_auxiliaries ()
              .has_flag (snemo::datamodel::gg_track::missing_geiger_hit_flag())) {
            DT_LOG_TRACE (get_logging_priority (), "Found a handle hit with '" << snemo::datamodel::gg_track::missing_geiger_hit_flag() << "' !");
            the_hit.grab_auxiliaries ().store_flag (KILL_HIT_FLAG);
          }
        }
        mctools::simulated_data::hit_collection_type::iterator new_end = std::remove_if (plain_gg_hits_->begin (),
                                                                                         plain_gg_hits_->end (),
                                                                                         kill_hit_pred_M2D);
        if (new_end != plain_gg_hits_->end ()) {
          DT_LOG_TRACE (get_logging_priority (), "Found some plain hits with the KILL_HIT_FLAG ! Kill them !");
          plain_gg_hits_->erase (new_end, plain_gg_hits_->end ());
        }
      }

      return;
    }

  } // end of namespace simulation

} // end of namespace snemo
