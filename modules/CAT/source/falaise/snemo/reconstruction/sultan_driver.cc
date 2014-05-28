/** \file falaise/snemo/reconstruction/sultan_driver.cc */

// Ourselves:
#include <falaise/snemo/reconstruction/sultan_driver.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost :
#include <boost/foreach.hpp>
// - Bayeux/datatools :
#include <datatools/properties.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>
// - Bayeux/geomtools :
#include <geomtools/manager.h>

// This project :
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>

namespace snemo {

  namespace reconstruction {

    const std::string sultan_driver::SULTAN_ID = "SULTAN";

    void sultan_driver::set_magfield(double value_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SULTAN driver is already initialized!");
      _magfield_ = value_;
      return;
    }

    double sultan_driver::get_magfield() const
    {
      return _magfield_;
    }

    sultan_driver::sultan_driver() :
      ::snemo::processing::base_tracker_clusterizer(sultan_driver::SULTAN_ID)
    {
      _set_defaults();
      return;
    }

    sultan_driver::~sultan_driver()
    {
      if (is_initialized()) {
        this->sultan_driver::reset();
      }
      return;
    }

    // Initialize the clusterizer through configuration properties
    void sultan_driver::initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "SULTAN driver is already initialized!");

      // Invoke initialization at parent level :
      this->snemo::processing::base_tracker_clusterizer::_initialize(setup_);

      _SULTAN_setup_.SuperNemo = true;
      _SULTAN_setup_.foil_radius = 0.0;

      double default_magfield_unit = CLHEP::tesla;

      // Forcing magnetic field (a temporary trick)
      if (! datatools::is_valid(_magfield_)) {
        if (setup_.has_key("SULTAN.magnetic_field")) {
          double magfield = setup_.fetch_real("SULTAN.magnetic_field");
          if (! setup_.has_explicit_unit("SULTAN.magnetic_field")) {
            magfield *= default_magfield_unit;
          }
          set_magfield(magfield);
        }
      }

      // Verbosity level
      if (setup_.has_key("SULTAN.clusterizer_level")) {
        _SULTAN_setup_.clusterizer_level = setup_.fetch_string("SULTAN.clusterizer_level");
      }

      // Verbosity level
      if (setup_.has_key("SULTAN.sequentiator_level")) {
        _SULTAN_setup_.sequentiator_level = setup_.fetch_string("SULTAN.sequentiator_level");
      }

      // Process calorimeter hits
      if (setup_.has_key("SULTAN.process_calo_hits")) {
        _process_calo_hits_ = setup_.fetch_boolean("SULTAN.process_calo_hits");
      }

      // Maximum processing time in ms
      if (setup_.has_key("SULTAN.max_time")) {
        _SULTAN_setup_.max_time = setup_.fetch_real("SULTAN.max_time");
        if (! setup_.has_explicit_unit("SULTAN.max_time")) {
          _SULTAN_setup_.max_time *= CLHEP::ms;
        }
      }

      // Make an event display?
      if (setup_.has_key("SULTAN.print_event_display")){
        _SULTAN_setup_.print_event_display = setup_.fetch_boolean("SULTAN.print_event_display");
      }

      // Use clocks to time the software ?
      if (setup_.has_key("SULTAN.use_clocks")) {
        _SULTAN_setup_.use_clocks = setup_.fetch_boolean("SULTAN.use_clocks");
      }

      // Use endpoints to clusterize ?
      if (setup_.has_key("SULTAN.use_endpoints")) {
        _SULTAN_setup_.use_endpoints = setup_.fetch_boolean("SULTAN.use_endpoints");
      }

      // Use Legendre transform to clusterize ?
      if (setup_.has_key("SULTAN.use_legendre")) {
        _SULTAN_setup_.use_legendre = setup_.fetch_boolean("SULTAN.use_legendre");
      }

      // Clusterize endpoints with helix model ?
      if (setup_.has_key("SULTAN.clusterize_with_helix_model")) {
        _SULTAN_setup_.clusterize_with_helix_model = setup_.fetch_boolean("SULTAN.clusterize_with_helix_model");
      }

      // Minimum energy of detected electron
      if (setup_.has_key("SULTAN.Emin")) {
        _SULTAN_setup_.Emin = setup_.fetch_real("SULTAN.Emin");
        if (! setup_.has_explicit_unit("SULTAN.Emin")) {
          _SULTAN_setup_.Emin *= CLHEP::MeV;
        }
        _SULTAN_setup_.Emin /= CLHEP::MeV;
      }

      // Maximum energy of detected electron
      if (setup_.has_key("SULTAN.Emax")) {
        _SULTAN_setup_.Emax = setup_.fetch_real("SULTAN.Emax");
        if (! setup_.has_explicit_unit("SULTAN.Emax")) {
          _SULTAN_setup_.Emax *= CLHEP::MeV;
        }
        _SULTAN_setup_.Emax /= CLHEP::MeV;
      }

      // Minimal probability away from the straight line
      if (setup_.has_key("SULTAN.probmin")) {
        _SULTAN_setup_.probmin = setup_.fetch_real("SULTAN.probmin");
      }

      // Number of sigmas
      if (setup_.has_key("SULTAN.nsigma_r")) {
        _SULTAN_setup_.nsigma_r = setup_.fetch_real("SULTAN.nsigma_r");
      }

      if (setup_.has_key("SULTAN.nsigma_z")) {
        _SULTAN_setup_.nsigma_z = setup_.fetch_real("SULTAN.nsigma_z");
      }

      // Number of cells which can be skipped (because the cell did not
      // work) and still the cluster is continuous
      if (setup_.has_key("SULTAN.nofflayers")) {
        _SULTAN_setup_.nofflayers = setup_.fetch_integer("SULTAN.nofflayers");
      }

      DT_THROW_IF (_SULTAN_setup_.nofflayers < 0, std::logic_error,
                   "Invalid number of 'off' layers(" << _SULTAN_setup_.nofflayers << ") !");

      // 1st event to be processed
      if (setup_.has_key("SULTAN.first_event")) {
        _SULTAN_setup_.first_event = setup_.fetch_integer("SULTAN.first_event");
      }

      // Sigma Z factor
      if (setup_.has_key("SULTAN.sigma_z_factor")) {
        _sigma_z_factor_ = setup_.fetch_real("SULTAN.sigma_z_factor");
        DT_THROW_IF (_sigma_z_factor_ <= 0.0 || _sigma_z_factor_ >= 100.0,
                     std::logic_error,
                     "Invalid Sigma Z factor(" << _sigma_z_factor_ << ") !");
      }

      if (setup_.has_key("SULTAN.min_ncells_in_cluster")) {
        _SULTAN_setup_.min_ncells_in_cluster = setup_.fetch_integer("SULTAN.min_ncells_in_cluster");
      }

      if (setup_.has_key("SULTAN.ncells_between_triplet_min")) {
        _SULTAN_setup_.ncells_between_triplet_min = setup_.fetch_integer("SULTAN.ncells_between_triplet_min");
      }

      if (setup_.has_key("SULTAN.ncells_between_triplet_range")) {
        _SULTAN_setup_.ncells_between_triplet_range = setup_.fetch_integer("SULTAN.ncells_between_triplet_range");
      }

      if (setup_.has_key("SULTAN.nsigmas")) {
        _SULTAN_setup_.nsigmas = setup_.fetch_real("SULTAN.nsigmas");
      }

      if (!datatools::is_valid(_magfield_)) {
        set_magfield(0.0025 * CLHEP::tesla);
      }

      // Get the calorimeter locators from a geometry plugin :
      const geomtools::manager & geo_mgr = get_geometry_manager();
      std::string locator_plugin_name;
      if (setup_.has_key("locator_plugin_name")) {
        locator_plugin_name = setup_.fetch_string("locator_plugin_name");
      }
      // If no locator plugin name is set, then search for the first one
      if (locator_plugin_name.empty()) {
        const geomtools::manager::plugins_dict_type & plugins = geo_mgr.get_plugins();
        for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
             ip != plugins.end();
             ip++) {
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
            DT_LOG_DEBUG (get_logging_priority(), "Find locator plugin with name = " << plugin_name);
            locator_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      if (geo_mgr.has_plugin(locator_plugin_name)
          && geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
        DT_LOG_NOTICE (get_logging_priority(), "Found locator plugin named '" << locator_plugin_name << "'");
        const snemo::geometry::locator_plugin & lp
          = geo_mgr.get_plugin<snemo::geometry::locator_plugin> (locator_plugin_name);
        // Set the calo cell locator :
        _calo_locator_ = &(lp.get_calo_locator ());
        _xcalo_locator_ = &(lp.get_xcalo_locator ());
        _gveto_locator_ = &(lp.get_gveto_locator ());
      }
      if (get_logging_priority () >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG (get_logging_priority (), "Calo locator :");
        _calo_locator_->dump (std::clog);
        DT_LOG_DEBUG (get_logging_priority (), "X-calo locator :");
        _xcalo_locator_->dump (std::clog);
        DT_LOG_DEBUG (get_logging_priority (), "G-veto locator :");
        _gveto_locator_->dump (std::clog);
      }

      // Geometry description :
      _SULTAN_setup_.num_blocks = 1;
      _SULTAN_setup_.planes_per_block.clear();
      _SULTAN_setup_.planes_per_block.push_back(_SULTAN_setup_.num_blocks);
      _SULTAN_setup_.planes_per_block.at(0) = get_gg_locator().get_number_of_layers(0);
      _SULTAN_setup_.num_cells_per_plane    = get_gg_locator().get_number_of_rows(0);
      _SULTAN_setup_.cell_distance          = get_gg_locator().get_cell_diameter();

      // Hard-coded values of bfield and chamber size
      _SULTAN_setup_.bfield = _magfield_ / CLHEP::tesla;
      _SULTAN_setup_.xsize  = 0.5 * get_gg_locator().get_number_of_rows(0) * get_gg_locator().get_cell_diameter();
      _SULTAN_setup_.ysize  = 0.5 * get_gg_locator().get_number_of_layers(0) * get_gg_locator().get_cell_diameter();
      _SULTAN_setup_.zsize  = 0.5 * get_gg_locator().get_cell_length();

      // Check the validity of the SULTAN setup data :
      DT_THROW_IF (! _SULTAN_setup_.check(), std::logic_error,
                   "Setup data for the SULTAN machine is not checked : "
                   << _SULTAN_setup_.get_error_message() << " !");

      // Configure and initialize the SULTAN machine :
      SULTAN::clusterizer_configure(_SULTAN_clusterizer_, _SULTAN_setup_);
      SULTAN::sultan_configure(_SULTAN_sultan_, _SULTAN_setup_);

      _SULTAN_clusterizer_.initialize();
      _SULTAN_sultan_.initialize();

      _set_initialized(true);

      return;
    }

    void sultan_driver::_set_defaults()
    {
      _SULTAN_setup_.reset();
      _sigma_z_factor_ = 1.0;
      datatools::invalidate(_magfield_);
      _process_calo_hits_ = true;
      _calo_locator_  = 0;
      _xcalo_locator_ = 0;
      _gveto_locator_ = 0;
      this->base_tracker_clusterizer::_reset();
      return;
    }

    // Reset the Sultan
    void sultan_driver::reset()
    {
      DT_THROW_IF (! is_initialized(), std::logic_error,
                   "SULTAN driver is not initialized !");
      _set_initialized(false);
      _SULTAN_clusterizer_.finalize();
      _SULTAN_sultan_.finalize();
      _set_defaults();
      return;
    }

    // Main clustering method
    int sultan_driver::_process_algo(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                     const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
                                     snemo::datamodel::tracker_clustering_data & clustering_ )
    {

      namespace st = SULTAN::topology;
      namespace sdm = snemo::datamodel;

      // SULTAN input data model :
      _SULTAN_input_.cells.clear();
      if (_SULTAN_input_.cells.capacity() < gg_hits_.size()) {
        _SULTAN_input_.cells.reserve(gg_hits_.size());
      }
      _SULTAN_output_.tracked_data.reset();
      int ihit = 0;

      // Hit accounting :
      std::map<int, sdm::calibrated_data::tracker_hit_handle_type> gg_hits_mapping;

      // GG hit loop :
      BOOST_FOREACH(const sdm::calibrated_data::tracker_hit_handle_type & gg_handle,
                    gg_hits_) {
        // Skip NULL handle :
        if (! gg_handle.has_data()) continue;

        // Get a const reference on the calibrated Geiger hit :
        const sdm::calibrated_tracker_hit & snemo_gg_hit = gg_handle.get();

        // Check the geometry ID as a Geiger cell :
        const snemo::geometry::gg_locator & gg_locator = get_gg_locator();
        const geomtools::geom_id & gg_hit_gid = snemo_gg_hit.get_geom_id();
        DT_THROW_IF (! gg_locator.is_drift_cell_volume(gg_hit_gid),
                     std::logic_error,
                     "Calibrated tracker hit can not be located inside detector !");

        if (!gg_locator.is_drift_cell_volume_in_current_module(gg_hit_gid)) {
          DT_LOG_DEBUG(get_logging_priority(), "Current Geiger cell is not in the module!");
          continue;
        }

        /*            int gg_hit_type = snemo_gg_hit.get_geom_id().get_type();
                      DT_THROW_IF (gg_hit_type != _gg_cell_geom_type_,
                      std::logic_error,
                      "Not the proper type of geometry category for a Geiger hit !");

                      // Extract the module number from its geom ID :
                      int gg_hit_module = snemo_gg_hit.get_geom_id().get(0);
                      if (gg_hit_module != _module_number_)
                      {
                      // Not the target module
                      continue;
                      }
        */
        // Extract the numbering scheme of the cell from its geom ID :
        const int side  = gg_locator.extract_side(gg_hit_gid);
        const int layer = gg_locator.extract_layer(gg_hit_gid);
        const int row   = gg_locator.extract_row(gg_hit_gid);

        // Translate into the SULTAN's numbering scheme :
        // -1 : negative X side; +1 : positive X side
        int block_id = side == 0 ? -1 : +1;
        /* Later number:
         * negative X side: layer from -8, -7, -6, -5, -4, -3, -2, -1, 0
         * positive X side: layer from 0, 1, 2 , 3, 4, 5, 6, 7, 8
         */
        int layer_id = side == 0 ? -layer: layer;
        // Cell number :  0:113 is mapped to -56:+56
        int cell_id  = row -(_SULTAN_setup_.num_cells_per_plane / 2);

        // X-Y position of the anode wire of the hit cell :
        st::experimental_double x;
        st::experimental_double y;

        // Center of the cell set in SULTAN's own reference frame:
        x.set_value(snemo_gg_hit.get_x());
        x.set_error(0.0);
        y.set_value(snemo_gg_hit.get_y());
        y.set_error(0.0);

        // Transverse Geiger drift distance :
        st::experimental_double z;
        // Plasma longitudinal origin along the anode wire :
        z.set_value(snemo_gg_hit.get_z());
        z.set_error(_sigma_z_factor_ * snemo_gg_hit.get_sigma_z());

        // Prompt/delayed trait of the hit :
        bool fast = snemo_gg_hit.is_prompt();

        // Transverse Geiger drift distance :
        double rdrift     = snemo_gg_hit.get_r();
        double rdrift_err = snemo_gg_hit.get_sigma_r();

        // Build the Geiger hit position :
        st::experimental_point gg_hit_position(x,y,z);

        // Add a new hit cell in the SULTAN input data model :
        st::cell & c = _SULTAN_input_.add_cell();
        c.set_type("SN");
        c.set_id(ihit++);
        c.set_probmin(_SULTAN_setup_.probmin);
        c.set_p(gg_hit_position);
        c.set_r(rdrift);
        c.set_er(rdrift_err);
        c.set_layer(layer_id);
        c.set_block(block_id);
        c.set_iid(cell_id);
        c.set_fast(fast);

        // Store mapping info between both data models :
        gg_hits_mapping[c.id()] = gg_handle;

        DT_LOG_DEBUG(get_logging_priority(),
                     "Geiger cell #" << snemo_gg_hit.get_id() << " has been added "
                     << "to SULTAN input data with id number #" << c.id());
      }

      // Take into account calo hits:
      _SULTAN_input_.calo_cells.clear ();
      // Calo hit accounting :
      std::map<int, sdm::calibrated_data::calorimeter_hit_handle_type> calo_hits_mapping;
      if (_process_calo_hits_) {
        if (_SULTAN_input_.calo_cells.capacity() < calo_hits_.size()) {
          _SULTAN_input_.calo_cells.reserve(calo_hits_.size());
        }
        _SULTAN_output_.tracked_data.reset();
        int ihit = 0;

        // CALO hit loop :
        BOOST_FOREACH (const sdm::calibrated_data::calorimeter_hit_handle_type & calo_handle,
                       calo_hits_) {
          // Skip NULL handle :
          if (! calo_handle.has_data()) continue;

          // Get a const reference on the calibrated Calo hit :
          const sdm::calibrated_calorimeter_hit & sncore_calo_hit = calo_handle.get();

          // Get calibrated calo. geom_id
          const geomtools::geom_id & a_calo_hit_gid = sncore_calo_hit.get_geom_id();
          // Extract the numbering scheme of the calo_cell from its geom ID :
          // int side  = sncore_calo_hit.get_geom_id ().get (1);
          // int wall, column, row, type;
          int column = -1;
          double width, height, thickness;
          datatools::invalidate(width);
          datatools::invalidate(height);
          datatools::invalidate(thickness);
          int side        = sncore_calo_hit.get_geom_id().get(1);
          int side_number = (side == 0) ? -1: 1;
          st::experimental_vector norm(0., 0., 0., 0., 0., 0.);
          geomtools::vector_3d block_position;
          // Extract the numbering scheme of the scin block from its geom ID :
          if (_calo_locator_->is_calo_block_in_current_module(a_calo_hit_gid)) {
            _calo_locator_->get_block_position(a_calo_hit_gid, block_position);
            width     = _calo_locator_->get_block_width();
            height    = _calo_locator_->get_block_height();
            thickness = _calo_locator_->get_block_thickness();
            column    = _calo_locator_->extract_column(a_calo_hit_gid);
            norm.set_x(st::experimental_double(-(double) side_number, 0.));
          } else if (_xcalo_locator_->is_calo_block_in_current_module(a_calo_hit_gid)) {
            _xcalo_locator_->get_block_position(a_calo_hit_gid, block_position);
            width     = _xcalo_locator_->get_block_width();
            height    = _xcalo_locator_->get_block_height();
            thickness = _xcalo_locator_->get_block_thickness();
            column    = _xcalo_locator_->extract_column(a_calo_hit_gid);
            norm.set_y(st::experimental_double(-(double) side_number, 0.));
          } else if (_gveto_locator_->is_calo_block_in_current_module(a_calo_hit_gid)) {
            _gveto_locator_->get_block_position(a_calo_hit_gid, block_position);
            width     = _gveto_locator_->get_block_width();
            height    = _gveto_locator_->get_block_height();
            thickness = _gveto_locator_->get_block_thickness();
            column    = _gveto_locator_->extract_column(a_calo_hit_gid);
            norm.set_z(st::experimental_double(-(double) side_number, 0.));
          }

          st::experimental_double energy(sncore_calo_hit.get_energy(),
                                         sncore_calo_hit.get_sigma_energy());
          st::experimental_double time(sncore_calo_hit.get_time(),
                                       sncore_calo_hit.get_sigma_time());
          // size_t id = sncore_calo_hit.get_hit_id ();
          st::experimental_point center(block_position.x(),
                                        block_position.y(),
                                        block_position.z(),
                                        0., 0., 0.);
          st::experimental_vector sizes(width, height, thickness,
                                        0., 0., 0.);
          st::plane pl(center, sizes, norm);
          pl.set_probmin(_SULTAN_setup_.probmin);
          pl.set_type("SuperNEMO");

          // Build the Calo hit position :
          // Add a new hit calo_cell in the SULTAN input data model :
          st::calorimeter_hit & c = _SULTAN_input_.add_calo_cell();
          c.set_pl(pl);
          c.set_e(energy);
          c.set_t(time);
          c.set_probmin(_SULTAN_setup_.probmin);
          c.set_layer(column);
          c.set_id(ihit++);

          // Store mapping info between both data models :
          calo_hits_mapping[c.id()] = calo_handle;

          DT_LOG_DEBUG(get_logging_priority(),
                       "Calo_cell #" << sncore_calo_hit.get_hit_id () << " has been added "
                       << "to SULTAN input data with id number #" << c.id ());
        }
      }

      // Validate the input data :
      if (! _SULTAN_input_.check()) {
        DT_LOG_ERROR(get_logging_priority(), "Invalid SULTAN input data !");
        return 1;
      }

      // Install the input data model within the algorithm object :
      _SULTAN_clusterizer_.set_cells(_SULTAN_input_.cells);

      // Install the input data model within the algorithm object :
      _SULTAN_clusterizer_.set_calorimeter_hits(_SULTAN_input_.calo_cells);

      // Prepare the output data model :
      _SULTAN_clusterizer_.prepare_event(_SULTAN_output_.tracked_data);

      // Run the Sultan algorithm :
      _SULTAN_clusterizer_.clusterize(_SULTAN_output_.tracked_data);
      _SULTAN_sultan_.sequentiate(_SULTAN_output_.tracked_data);

      // Analyse the Sultan output: scenarios made of sequences
      const std::vector<st::scenario> & tss = _SULTAN_output_.tracked_data.get_scenarios();
      DT_LOG_DEBUG(get_logging_priority(), "Number of scenarios = " << tss.size());

      for (std::vector<st::scenario>::const_iterator iscenario = tss.begin();
           iscenario != tss.end();
           ++iscenario){

        // If not default solution, add a new one :
        sdm::tracker_clustering_solution::handle_type htcs(new sdm::tracker_clustering_solution);
        clustering_.add_solution(htcs, true);
        clustering_.grab_default_solution().set_solution_id(clustering_.get_number_of_solutions() - 1);
        sdm::tracker_clustering_solution & clustering_solution = clustering_.grab_default_solution();
        clustering_solution.grab_auxiliaries().update_string("tracker_clusterizer", SULTAN_ID);

        const std::vector<st::sequence> & the_sequences = iscenario->sequences();
        DT_LOG_DEBUG(get_logging_priority(), "Number of sequences = " << the_sequences.size());

        for (std::vector<st::sequence>::const_iterator isequence = the_sequences.begin();
             isequence != the_sequences.end();
             ++isequence) {
          const st::sequence & a_sequence = *isequence;

          // Append a new cluster :
          sdm::tracker_cluster::handle_type tch(new sdm::tracker_cluster);
          clustering_solution.grab_clusters().push_back(tch);
          sdm::tracker_cluster::handle_type & cluster_handle
            = clustering_solution.grab_clusters().back();
          cluster_handle.grab().set_cluster_id(clustering_solution.get_clusters().size() - 1);
          const st::experimental_helix & seq_helix = isequence->get_helix();

          // Adding points
          // from SULTAN algorithm. Since it is a none generic
          // information, this info will be added to
          // calibrated data cells as properties.

          cluster_handle.grab().grab_auxiliaries().update("SULTAN_helix_x0", seq_helix.x0().value());
          cluster_handle.grab().grab_auxiliaries().update("SULTAN_helix_y0", seq_helix.y0().value());
          cluster_handle.grab().grab_auxiliaries().update("SULTAN_helix_z0", seq_helix.z0().value());
          cluster_handle.grab().grab_auxiliaries().update("SULTAN_helix_R",  seq_helix.R().value());
          cluster_handle.grab().grab_auxiliaries().update("SULTAN_helix_H",  seq_helix.H().value());

          // Loop on all hits within the sequence(nodes) :
          const size_t seqsz = a_sequence.nodes().size();

          for (int i = 0; i <(int) seqsz; i++) {
            const st::node & a_node = a_sequence.nodes()[i];
            int hit_id = a_node.c().id();
            cluster_handle.grab().grab_hits().push_back(gg_hits_mapping[hit_id]);
            DT_LOG_DEBUG(get_logging_priority(), "Add tracker hit with id #" << hit_id);

            const double xt    = a_node.ep().x().value();
            const double yt    = a_node.ep().y().value();
            const double zt    = a_node.ep().z().value();
            const double xterr = a_node.ep().x().error();
            const double yterr = a_node.ep().y().error();
            const double zterr = a_node.ep().z().error();

            sdm::calibrated_tracker_hit & the_last_cell = gg_hits_mapping[hit_id].grab();
            the_last_cell.grab_auxiliaries().update("SULTAN_x", xt);
            the_last_cell.grab_auxiliaries().update("SULTAN_y", yt);
            the_last_cell.grab_auxiliaries().update("SULTAN_z", zt);
            the_last_cell.grab_auxiliaries().update("SULTAN_x_error", xterr);
            the_last_cell.grab_auxiliaries().update("SULTAN_y_error", yterr);
            the_last_cell.grab_auxiliaries().update("SULTAN_z_error", zterr);

          } // finish loop in cells in sequence
        } // finish loop on sequences
      } // finish loop on scenarios
      return 0;
    }

  } // end of namespace reconstruction

} // end of namespace snemo


/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::sultan_driver,ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::sultan_driver");
  ocd_.set_class_description("A driver class for the SULTAN algorithm");
  ocd_.set_class_library("Falaise_CAT");
  ocd_.set_class_documentation("This driver manager for the SULTAN clustering algorithm.");

  // Invoke OCD support at parent level :
  ::snemo::processing::base_tracker_clusterizer::ocd_support(ocd_);

  {
    // Description of the 'SULTAN.magnetic_field' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.magnetic_field")
      .set_terse_description("Force the magnetic field value (vertical)")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 25 gauss")
      .set_default_value_real(25 * CLHEP::gauss, "gauss")
      .add_example("Use no magnetic field::                    \n"
                   "                                           \n"
                   "  SULTAN.magnetic_field : real = 0.0 gauss \n"
                   "                                           \n"
                   )
      ;
  }


  {
    // Description of the 'SULTAN.clusterizer_level' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.clusterizer_level")
      .set_terse_description("SULTAN clusterizer's verbosity level")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      // .set_long_description("Default value: \"normal\"")
      .set_default_value_string("normal")
      .add_example("Use normal verbosity::                           \n"
                   "                                                 \n"
                   "  SULTAN.clusterizer_level : string = \"normal\" \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.sequentiator_level' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.sequentiator_level")
      .set_terse_description("SULTAN sequentiator's verbosity level")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      // .set_long_description("Default value: \"normal\"")
      .set_default_value_string("normal")
      .add_example("Use normal verbosity::                           \n"
                   "                                                 \n"
                   "  SULTAN.sequentiator_level : string = \"normal\" \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.max_time' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.max_time")
      .set_terse_description("Maximum processing time")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 5000 ms")
      .set_default_value_real(5000 * CLHEP::ms, "ms")
      .add_example("Use default value::                \n"
                   "                                   \n"
                   "  SULTAN.max_time : real = 5000 ms \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.print_event_display' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.print_event_display")
      .set_terse_description("Activate the SULTAN print event display")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Use normal verbosity::                           \n"
                   "                                                 \n"
                   "  SULTAN.print_event_display : boolean = 0       \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.use_clocks' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.use_clocks")
      .set_terse_description("Activate the SULTAN clocks")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate clocks::                                \n"
                   "                                                 \n"
                   "  SULTAN.use_clocks : boolean = 1                \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.endpoints' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.endpoints")
      .set_terse_description("Use end points to clusterize")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate endpoints::                             \n"
                   "                                                 \n"
                   "  SULTAN.endpoints : boolean = 1                 \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.legendre' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.legendre")
      .set_terse_description("Use Legendre transform to clusterize")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate Legendre transform::                    \n"
                   "                                                 \n"
                   "  SULTAN.legendre : boolean = 1                  \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.clusterize_with_helix_model' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.clusterize_with_helix_model")
      .set_terse_description("Use helix model to clusterize")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .add_example("Activate helix model::                             \n"
                   "                                                   \n"
                   "  SULTAN.clusterize_with_helix_model : boolean = 1 \n"
                   "                                                   \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.Emin' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.Emin")
      .set_terse_description("Minimum energy of detected electron")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 0.2 MeV")
      .set_default_value_real(0.2 * CLHEP::MeV, "MeV")
      .add_example("Use default value::                \n"
                   "                                   \n"
                   "  SULTAN.Emin : real = 0.2 MeV     \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.Emax' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.Emax")
      .set_terse_description("Maximum energy of detected electron")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 7.0 MeV")
      .set_default_value_real(7.0 * CLHEP::MeV, "MeV")
      .add_example("Use default value::                \n"
                   "                                   \n"
                   "  SULTAN.Emax : real = 7.0 MeV     \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.probmin' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.probmin")
      .set_terse_description("Minimal probability away from the straight line")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 0.0")
      .set_default_value_real(0.0)
      .add_example("Use default value::                \n"
                   "                                   \n"
                   "  SULTAN.probmin : real = 0.0      \n"
                   "                                   \n"
                   )
      ;
  }


  {
    // Description of the 'SULTAN.nsigma_r' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.nsigma_r")
      .set_terse_description("Number of sigmas for tolerance on the drift radius")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 5.0")
      .set_default_value_real(5.0)
      .add_example("Use default value::                \n"
                   "                                   \n"
                   "  SULTAN.nsigma_r : real = 5.0     \n"
                   "                                   \n"
                   )
      ;
  }


  {
    // Description of the 'SULTAN.nsigma_z' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.nsigma_z")
      .set_terse_description("Number of sigmas for tolerance on the longitudinal position")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 3.0")
      .set_default_value_real(3.0)
      .add_example("Use default value::                \n"
                   "                                   \n"
                   "  SULTAN.nsigma_z : real = 3.0     \n"
                   "                                   \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.nofflayers' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.nofflayers")
      .set_terse_description("Number of cells which can be skipped (because the cell did not work) and still the cluster is continuous")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      // .set_long_description("Default value: 1")
      .set_default_value_integer(1)
      .add_example("Use default value::               \n"
                   "                                  \n"
                   "  SULTAN.nofflayers : integer = 1 \n"
                   "                                  \n"
                   )
      ;
  }


  {
    // Description of the 'SULTAN.first_event' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.first_event")
      .set_terse_description("First event to be processed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      // .set_long_description("Default value: -1")
      .set_default_value_integer(-1)
      .add_example("Do not specify any first event::     \n"
                   "                                     \n"
                   "  SULTAN.first_event : integer = -1  \n"
                   "                                     \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.sigma_z_factor' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.sigma_z_factor")
      .set_terse_description("Sigma Z factor")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 1.0")
      .set_default_value_real(1.0)
      .add_example("Use the default value::                     \n"
                   "                                            \n"
                   "  SULTAN.sigma_z_factor : real = 1.0 \n"
                   "                                            \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.min_ncells_in_cluster' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.min_ncells_in_cluster")
      .set_terse_description("Minimum number of cells in a cluster")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      // .set_long_description("Default value: -1")
      .set_default_value_integer(0)
      .add_example("Do not specify any min value::                \n"
                   "                                              \n"
                   "  SULTAN.min_ncells_in_cluster : integer = 0  \n"
                   "                                              \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.ncells_between_triplet_min' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.ncells_between_triplet_min")
      .set_terse_description("Minimum distance between cells in a triplet")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      // .set_long_description("Default value: -1")
      .set_default_value_integer(0)
      .add_example("Do not specify any min value::                \n"
                   "                                              \n"
                   "  SULTAN.ncells_between_triplet_min : integer = 0  \n"
                   "                                              \n"
                   )
      ;
  }

  {
    // Description of the 'SULTAN.ncells_between_triplet_range' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.ncells_between_triplet_range")
      .set_terse_description("Range distance between cells in a triplet")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      // .set_long_description("Default value: -1")
      .set_default_value_integer(0)
      .add_example("Do not specify any min value::                \n"
                   "                                              \n"
                   "  SULTAN.ncells_between_triplet_range : integer = 0  \n"
                   "                                              \n"
                   )
      ;
  }

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::sultan_driver, "snemo::reconstruction::sultan_driver")
