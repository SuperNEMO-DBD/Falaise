/// \file falaise/snemo/reconstruction/trackfit_driver.cc

// Ourselves:
#include <snemo/reconstruction/trackfit_driver.h>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/processing/geiger_regime.h>

// TrackFit library
#include <TrackFit/fit_utils.h>
#include <TrackFit/helix_fit_mgr.h>
#include <TrackFit/i_drift_time_calibration.h>
#include <TrackFit/line_fit_mgr.h>

namespace snemo {

namespace reconstruction {

/// SuperNEMO drift time calibration
snemo_drift_time_calibration::snemo_drift_time_calibration() {
  _gg_regime_.reset(new snemo::processing::geiger_regime);
  // Initialize by using default parameters
  datatools::properties dummy;
  _gg_regime_.get()->initialize(dummy);
  return;
}

snemo_drift_time_calibration::~snemo_drift_time_calibration() {
  _gg_regime_.reset();
  return;
}

double snemo_drift_time_calibration::get_max_cell_radius() const {
  return _gg_regime_.get()->get_rdiag();
}

double snemo_drift_time_calibration::get_sensitive_cell_radius() const {
  return _gg_regime_.get()->get_rdiag();
}

void snemo_drift_time_calibration::drift_time_to_radius(double time_, double& radius_,
                                                        double& sigma_radius_) const {
  _gg_regime_.get()->calibrate_drift_radius_from_drift_time(time_, radius_, sigma_radius_);
  return;
}

void snemo_drift_time_calibration::radius_to_drift_time(double /* radius_ */, double& /* time_ */,
                                                        double& /* sigma_time_ */) const {
  DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Not implemented");
  return;
}

const std::string& trackfit_driver::trackfit_id() {
  static const std::string _id("trackfit");
  return _id;
}

const std::string& trackfit_driver::get_drift_time_calibration_label() const {
  return _drift_time_calibration_label_;
}

void trackfit_driver::set_drift_time_calibration_label(const std::string& label_) {
  _drift_time_calibration_label_ = label_;
  return;
}

void trackfit_driver::set_use_line_fit(const bool use_line_fit_) {
  _use_line_fit_ = use_line_fit_;
  return;
}

bool trackfit_driver::use_line_fit() const { return _use_line_fit_; }

void trackfit_driver::set_use_helix_fit(const bool use_helix_fit_) {
  _use_helix_fit_ = use_helix_fit_;
  return;
}

bool trackfit_driver::use_helix_fit() const { return _use_helix_fit_; }

void trackfit_driver::set_line_only_guesses(const std::vector<std::string>& only_guesses_) {
  for (size_t i = 0; i < TrackFit::line_fit_mgr::guess_utils::NUMBER_OF_GUESS; ++i) {
    const std::string key = TrackFit::line_fit_mgr::guess_utils::guess_mode_label(i);

    std::vector<std::string>::const_iterator found =
        std::find(only_guesses_.begin(), only_guesses_.end(), key);
    if (found != only_guesses_.end()) {
      _line_guess_dict_[key] = i;
    }
  }
  return;
}

void trackfit_driver::set_helix_only_guesses(const std::vector<std::string>& only_guesses_) {
  for (size_t i = 0; i < TrackFit::helix_fit_mgr::guess_utils::NUMBER_OF_GUESS; ++i) {
    const std::string key = TrackFit::helix_fit_mgr::guess_utils::guess_mode_label(i);

    std::vector<std::string>::const_iterator found =
        std::find(only_guesses_.begin(), only_guesses_.end(), key);
    if (found != only_guesses_.end()) {
      _helix_guess_dict_[key] = i;
    }
  }
  return;
}

const geomtools::placement& trackfit_driver::get_working_referential() const {
  return *_working_referential_;
}

geomtools::placement& trackfit_driver::grab_working_referential() { return *_working_referential_; }

const TrackFit::gg_hits_col& trackfit_driver::get_hits_referential() const {
  return _gg_hits_referential_;
}

TrackFit::gg_hits_col& trackfit_driver::grab_hits_referential() { return _gg_hits_referential_; }

void trackfit_driver::_set_defaults_() {
  _trackfit_flag_ = 0;
  _drift_time_calibration_label_.clear();
  _dtc_.reset();

  _use_line_fit_ = false;
  _line_guess_driver_.reset();
  _line_guess_dict_.clear();
  _line_fit_setup_.reset();
  _gg_hits_referential_.clear();
  _working_referential_ = 0;

  _use_helix_fit_ = false;
  _helix_guess_driver_.reset();
  _helix_guess_dict_.clear();
  _helix_fit_setup_.reset();
  return;
}

// Constructor
trackfit_driver::trackfit_driver()
    : snemo::processing::base_tracker_fitter(trackfit_driver::trackfit_id()) {
  _trackfit_flag_ = 0;
  _working_referential_ = 0;
  _set_defaults_();
  return;
}

// Destructor
trackfit_driver::~trackfit_driver() {
  if (is_initialized()) {
    reset();
  }
  return;
}

// Initialize the fitter through configuration properties
void trackfit_driver::initialize(const datatools::properties& setup_) {
  DT_THROW_IF(is_initialized(), std::logic_error,
              "Driver '" << get_id() << "' is already initialized !");

  // Invoke initialization at parent level :
  this->snemo::processing::base_tracker_fitter::_initialize(setup_);

  if (_drift_time_calibration_label_.empty()) {
    if (setup_.has_key("drift_time_calibration_label")) {
      const std::string dtcl = setup_.fetch_string("drift_time_calibration_label");
      set_drift_time_calibration_label(dtcl);
    }
  }
  if (_drift_time_calibration_label_.empty()) {
    set_drift_time_calibration_label("snemo");
  }

  std::vector<std::string> fitting_models;
  if (setup_.has_key("fitting_models")) {
    setup_.fetch("fitting_models", fitting_models);
  }
  // Default fitting models:
  if (fitting_models.size() == 0) {
    fitting_models.push_back("line");
    fitting_models.push_back("helix");
  }
  for (std::vector<std::string>::const_iterator i = fitting_models.begin();
       i != fitting_models.end(); ++i) {
    const std::string& a_model = *i;

    if (a_model == "line") {
      set_use_line_fit(true);
      if (setup_.has_key("line.only_guess")) {
        std::vector<std::string> only_guess;
        setup_.fetch("line.only_guess", only_guess);
        set_line_only_guesses(only_guess);
      }
    } else if (a_model == "helix") {
      set_use_helix_fit(true);
      if (setup_.has_key("helix.only_guess")) {
        std::vector<std::string> only_guess;
        setup_.fetch("helix.only_guess", only_guess);
        set_helix_only_guesses(only_guess);
      }
    } else {
      DT_THROW_IF(true, std::logic_error, "No '" << a_model << "' fit model is available !");
    }
  }

  if (use_line_fit()) {
    // Setup the line guess algo :
    datatools::properties lg_setup;
    setup_.export_and_rename_starting_with(lg_setup, "line.guess.", "");
    _line_guess_driver_.initialize(lg_setup);

    // Extract the setup of the line fit algo :
    setup_.export_and_rename_starting_with(_line_fit_setup_, "line.fit.", "");
  }

  if (use_helix_fit()) {
    // Setup the helix guess algo :
    datatools::properties hg_setup;
    setup_.export_and_rename_starting_with(hg_setup, "helix.guess.", "");
    _helix_guess_driver_.initialize(hg_setup);

    // Extract the setup of the helix fit algo :
    setup_.export_and_rename_starting_with(_helix_fit_setup_, "helix.fit.", "");
  }

  _install_drift_time_calibration_driver_();

  _set_initialized(true);
  return;
}

void trackfit_driver::_install_drift_time_calibration_driver_() {
  if (!_drift_time_calibration_label_.empty()) {
    if (_drift_time_calibration_label_ == "default") {
      // define a drift_time calibration rule:
      _dtc_.reset(new TrackFit::default_drift_time_calibration);
    } else if (_drift_time_calibration_label_ == "snemo") {
      _dtc_.reset(new snemo_drift_time_calibration);
    } else {
      DT_THROW_IF(true, std::logic_error,
                  "DTC label '" << _drift_time_calibration_label_ << "' is not implemented !");
    }
  }
  return;
}

// Reset the fitter
void trackfit_driver::reset() {
  this->base_tracker_fitter::_reset();

  _dtc_.reset();
  _drift_time_calibration_label_.clear();

  _line_fit_setup_.clear();
  _helix_fit_setup_.clear();

  _helix_guess_driver_.reset();
  _line_guess_driver_.reset();

  _line_guess_dict_.clear();
  _helix_guess_dict_.clear();

  if (_working_referential_) {
    _working_referential_->reset();
    delete _working_referential_;
  }

  _gg_hits_referential_.clear();

  _set_defaults_();

  return;
}

// Main fitting method
int trackfit_driver::_process_algo(const snemo::datamodel::tracker_clustering_data& clustering_,
                                   snemo::datamodel::tracker_trajectory_data& trajectory_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  // Retrieve geiger cell diameter from gg_locator (to be used
  // by trackfit algorithm)
  const double gg_cell_diameter = get_gg_locator().get_cell_diameter() / CLHEP::mm;
  DT_LOG_DEBUG(get_logging_priority(), "Geiger cell diameter = " << gg_cell_diameter << " mm");

  // Get cluster solutions:
  const snemo::datamodel::tracker_clustering_data::solution_col_type& cluster_solutions =
      clustering_.get_solutions();

  for (snemo::datamodel::tracker_clustering_data::solution_col_type::const_iterator isolution =
           cluster_solutions.begin();
       isolution != cluster_solutions.end(); ++isolution) {
    // Get current tracker solution:
    const snemo::datamodel::tracker_clustering_solution& a_cluster_solution = isolution->get();
    DT_LOG_DEBUG(get_logging_priority(), "Tracker solution: ");
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
      a_cluster_solution.tree_dump(std::clog, "", "[debug]: ");
    }

    // Create a new 'tracker_trajectory_solution':
    snemo::datamodel::tracker_trajectory_solution::handle_type htt_solution(
        new snemo::datamodel::tracker_trajectory_solution);
    trajectory_.add_solution(htt_solution);
    snemo::datamodel::tracker_trajectory_solution& a_trajectory_solution = htt_solution.grab();
    a_trajectory_solution.set_solution_id(a_cluster_solution.get_solution_id());
    a_trajectory_solution.set_clustering_solution(*isolution);

    // Get clusters stored in the current tracker solution:
    const snemo::datamodel::tracker_clustering_solution::cluster_col_type& clusters =
        a_cluster_solution.get_clusters();

    for (snemo::datamodel::tracker_clustering_solution::cluster_col_type::const_iterator icluster =
             clusters.begin();
         icluster != clusters.end(); ++icluster) {
      // Get current tracker cluster:
      const snemo::datamodel::tracker_cluster& a_cluster = icluster->get();
      DT_LOG_DEBUG(get_logging_priority(), "Tracker cluster: ");
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        a_cluster.tree_dump(std::clog, "", "[debug]: ");
      }

      // Get tracker hits stored in the current tracker cluster:
      const snemo::datamodel::calibrated_tracker_hit::collection_type& hits = a_cluster.get_hits();

      // Home made Geiger hit model for 'trackfit':
      TrackFit::gg_hits_col gg_hits;
      for (snemo::datamodel::calibrated_tracker_hit::collection_type::const_iterator igg =
               hits.begin();
           igg != hits.end(); ++igg) {
        const snemo::datamodel::calibrated_tracker_hit& a_gg_hit = igg->get();

        // Fill TrackFit::gg_hits_col
        {
          TrackFit::gg_hit hit;
          gg_hits.push_back(hit);
        }
        TrackFit::gg_hit& hit = gg_hits.back();

        hit.set_x(a_gg_hit.get_x());
        hit.set_y(a_gg_hit.get_y());
        hit.set_z(a_gg_hit.get_z());
        hit.set_sigma_z(a_gg_hit.get_sigma_z());
        hit.set_r(a_gg_hit.get_r());
        hit.set_sigma_r(a_gg_hit.get_sigma_r());
        hit.set_rmax(gg_cell_diameter / 2.0);

        // 2012-06-05 XG: if particle is delayed then set the
        // delayed time in order to recalibrate it and thus extract
        // the drift distance. Everything is done inside the fitting
        // procedure using a dedicated time calibrator.
        //
        // 2012-11-03 XG: Flag the delayed hit to fit also the start
        // time
        if (a_gg_hit.has_delayed_time()) {
          DT_LOG_DEBUG(get_logging_priority(),
                       "A delayed geiger hit is added to the cell collection !");
          hit.set_t(a_gg_hit.get_delayed_time());
          hit.grab_properties().store_flag(TrackFit::gg_hit::delayed_flag());
        } else {
          hit.set_t(a_gg_hit.get_anode_time());
        }
      }

      // Helix fit solutions:
      std::list<TrackFit::helix_fit_solution> helix_solutions;
      if (use_helix_fit()) this->do_helix_fit(gg_hits, helix_solutions);

      bool helix_fit_succeed = false;
      for (std::list<TrackFit::helix_fit_solution>::const_iterator ihs = helix_solutions.begin();
           ihs != helix_solutions.end(); ++ihs) {
        const TrackFit::helix_fit_solution& a_fit_solution = *ihs;

        if (!a_fit_solution.ok) continue;
        helix_fit_succeed = true;

        // Create new 'tracker_trajectory' handle:
        snemo::datamodel::tracker_trajectory::handle_type h_trajectory(
            new snemo::datamodel::tracker_trajectory);
        a_trajectory_solution.grab_trajectories().push_back(h_trajectory);

        // 2012/05/11 XG : this work if all cells are clusterized on
        // the same side. If clusterizer algorithms puts together
        // cells from the two sides then, geom_id should invalidated
        // or tagged differently Set trajectory geom_id using the
        // first geiger hit of the associated cluster
        get_geometry_manager().get_id_mgr().make_id("tracker_submodule",
                                                    h_trajectory.grab().grab_geom_id());
        get_geometry_manager().get_id_mgr().extract(hits.front().get().get_geom_id(),
                                                    h_trajectory.grab().grab_geom_id());

        // Create new 'tracker_pattern' handle:
        snemo::datamodel::tracker_trajectory::handle_pattern h_pattern;
        snemo::datamodel::helix_trajectory_pattern* htp =
            new snemo::datamodel::helix_trajectory_pattern;
        h_pattern.reset(htp);

        // Set cluster and pattern handle to tracker_trajectory:
        h_trajectory.grab().set_trajectory_id(a_trajectory_solution.get_trajectories().size());
        h_trajectory.grab().set_cluster_handle(*icluster);
        h_trajectory.grab().set_pattern_handle(h_pattern);
        h_trajectory.grab().grab_auxiliaries().store_real("chi2", pow(a_fit_solution.chi, 2));
        h_trajectory.grab().grab_auxiliaries().store_integer("ndof", a_fit_solution.ndof);
        h_trajectory.grab().grab_auxiliaries().store_string(
            "guess", a_fit_solution.auxiliaries.fetch_string("guess"));

        const geomtools::vector_3d center(a_fit_solution.x0, a_fit_solution.y0, a_fit_solution.z0);
        htp->get_helix().set_center(center);
        htp->get_helix().set_radius(a_fit_solution.r);
        htp->get_helix().set_step(a_fit_solution.step);
        htp->get_helix().set_angle1(a_fit_solution.angle_1);
        htp->get_helix().set_angle2(a_fit_solution.angle_2);
      }

      // Line fit solutions:
      std::list<TrackFit::line_fit_solution> line_solutions;
      if (use_line_fit()) this->do_line_fit(gg_hits, line_solutions);

      bool line_fit_succeed = false;
      for (std::list<TrackFit::line_fit_solution>::const_iterator ils = line_solutions.begin();
           ils != line_solutions.end(); ++ils) {
        const TrackFit::line_fit_solution& a_fit_solution = *ils;

        if (!a_fit_solution.ok) continue;
        line_fit_succeed = true;

        // Create new 'tracker_trajectory' handle:
        snemo::datamodel::tracker_trajectory::handle_type h_trajectory(
            new snemo::datamodel::tracker_trajectory);
        a_trajectory_solution.grab_trajectories().push_back(h_trajectory);

        // Set trajectory geom_id using the first geiger
        // hit of the associated cluster
        get_geometry_manager().get_id_mgr().make_id("tracker_submodule",
                                                    h_trajectory.grab().grab_geom_id());
        get_geometry_manager().get_id_mgr().extract(hits.front().get().get_geom_id(),
                                                    h_trajectory.grab().grab_geom_id());

        // Create new 'tracker_pattern' handle:
        snemo::datamodel::tracker_trajectory::handle_pattern h_pattern;
        snemo::datamodel::line_trajectory_pattern* ltp =
            new snemo::datamodel::line_trajectory_pattern;
        h_pattern.reset(ltp);

        // Set cluster and pattern handle to tracker_trajectory:
        h_trajectory.grab().set_trajectory_id(a_trajectory_solution.get_trajectories().size());
        h_trajectory.grab().set_cluster_handle(*icluster);
        h_trajectory.grab().set_pattern_handle(h_pattern);
        h_trajectory.grab().grab_auxiliaries().store_real("chi2", pow(a_fit_solution.chi, 2));
        h_trajectory.grab().grab_auxiliaries().store_integer("ndof", a_fit_solution.ndof);
        h_trajectory.grab().grab_auxiliaries().store_string(
            "guess", a_fit_solution.auxiliaries.fetch_string("guess"));
        if (a_fit_solution.t0 > 0.0 * CLHEP::ns) {
          h_trajectory.grab().grab_auxiliaries().store_real("t0", a_fit_solution.t0);
        }

        // compute the trajectory segment in the g.r.f(lab) frame:
        geomtools::line_3d& l3d = ltp->get_segment();
        TrackFit::line_fit_mgr::convert_solution(this->get_hits_referential(), a_fit_solution,
                                                 this->get_working_referential(), l3d);
      }

      if (!helix_fit_succeed && !line_fit_succeed) {
        DT_LOG_DEBUG(get_logging_priority(), "Helix/Line fits both fail !");
        snemo::datamodel::tracker_trajectory_solution::cluster_col_type& cct =
            a_trajectory_solution.grab_unfitted_clusters();
        cct.push_back(*icluster);
      }
    }  // end of 'tracker_cluster'
  }    // end of 'tracker_solution'

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return 0;
}

void trackfit_driver::do_helix_fit(const TrackFit::gg_hits_col& gg_hits_,
                                   std::list<TrackFit::helix_fit_solution>& solutions_) {
  // Helix fit parameters initialization:
  const size_t max_guess = TrackFit::helix_fit_mgr::guess_utils::NUMBER_OF_GUESS;
  helix_guess_dict_type guesses;

  // Compute different 'helix' guesses:
  _compute_helix_guesses_(gg_hits_, guesses, max_guess);

  // Compute 'helix' fit solutions:
  _compute_helix_fit_solutions_(gg_hits_, guesses, solutions_);

  return;
}

void trackfit_driver::do_line_fit(const TrackFit::gg_hits_col& gg_hits_,
                                  std::list<TrackFit::line_fit_solution>& solutions_) {
  // Line fit parameters initialization:
  const size_t max_guess = TrackFit::line_fit_mgr::guess_utils::NUMBER_OF_GUESS;
  line_guess_dict_type guesses;

  // compute the hits within a working reference frame(w.r.f.) for the fit:
  // - in the working frame, the candidate track is garanted to be
  //   roughly aligned along the x'x axis.
  // - the fit is performed in this w.r.f. and possible solution of the fit
  //   have to be computed in the global reference frame(g.r.f.) at the end
  //   using the 'line_fit_mgr::convert_solution' method

  // Initialize working frame placement
  _working_referential_ = new geomtools::placement();

  TrackFit::line_fit_mgr::compute_best_frame(gg_hits_, _gg_hits_referential_,
                                             grab_working_referential(), _trackfit_flag_);

  // Compute different 'line' guesses:
  _compute_line_guesses_(_gg_hits_referential_, guesses, max_guess);

  // Compute 'line' fit solutions:
  _compute_line_fit_solutions_(_gg_hits_referential_, guesses, solutions_);

  return;
}

void trackfit_driver::_compute_helix_guesses_(const TrackFit::gg_hits_col& gg_hits_,
                                              helix_guess_dict_type& guesses_,
                                              const size_t max_guess_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  for (size_t iguess = 0; iguess < max_guess_; ++iguess) {
    const std::string guess_label = TrackFit::helix_fit_mgr::guess_utils::guess_mode_label(iguess);

    if (!_helix_guess_dict_.empty()) {
      if (_helix_guess_dict_.find(guess_label) == _helix_guess_dict_.end()) {
        continue;
      }
    }

    DT_LOG_DEBUG(get_logging_priority(), "Compute guess '" << guess_label << "'");

    // // Add use_guess_trust flag
    // if (use_guess_trust())
    //   {
    //     _trackfit_flag_ |= TrackFit::USE_GUESS_TRUST;
    //   }
    // if (get_guess_trust_mode() == TrackFit::fit_utils::GUESS_TRUST_MODE_COUNTER)
    //   {
    //     _trackfit_flag_ |= TrackFit::GUESS_TRUST_COUNTER;
    //   }
    // else if (get_guess_trust_mode() == TrackFit::fit_utils::GUESS_TRUST_MODE_BARYCENTER)
    //   {
    //     _trackfit_flag_ |= TrackFit::GUESS_TRUST_BARYCENTER;
    //   }

    TrackFit::helix_fit_params hf_params;
    bool guess_ok = _helix_guess_driver_.compute_guess(gg_hits_, iguess, hf_params, false);
    if (!guess_ok) {
      DT_LOG_DEBUG(get_logging_priority(), "Cannot compute guess '" << guess_label << "'");
      continue;
    }

    // Create new guess
    guesses_.insert(make_pair(guess_label, hf_params));
  }

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

void trackfit_driver::_compute_line_guesses_(const TrackFit::gg_hits_col& gg_hits_,
                                             line_guess_dict_type& guesses_,
                                             const size_t max_guess_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");

  for (size_t iguess = 0; iguess < max_guess_; ++iguess) {
    const std::string guess_label = TrackFit::line_fit_mgr::guess_utils::guess_mode_label(iguess);

    if (!_line_guess_dict_.empty()) {
      if (_line_guess_dict_.find(guess_label) == _line_guess_dict_.end()) {
        continue;
      }
    }

    DT_LOG_DEBUG(get_logging_priority(), "Compute guess '" << guess_label << "'");

    // datatools::utils::properties config;
    TrackFit::line_fit_params lf_params;

    bool guess_ok = _line_guess_driver_.compute_guess(gg_hits_, iguess, lf_params);

    if (!guess_ok) {
      DT_LOG_DEBUG(get_logging_priority(), "Cannot compute guess '" << guess_label << "'");
      continue;
    }

    // Create new guess
    guesses_.insert(make_pair(guess_label, lf_params));
  }

  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

void trackfit_driver::_compute_helix_fit_solutions_(
    const TrackFit::gg_hits_col& gg_hits_, const helix_guess_dict_type& guesses_,
    std::list<TrackFit::helix_fit_solution>& solutions_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");
  DT_LOG_INFORMATION(get_logging_priority(), "Perform the fit...");
  for (helix_guess_dict_type::const_iterator iguess = guesses_.begin(); iguess != guesses_.end();
       ++iguess) {
    DT_LOG_INFORMATION(get_logging_priority(), "Starting fit for guess '" << iguess->first << "'");
    TrackFit::helix_fit_mgr hfm;
    hfm.set_logging_priority(get_logging_priority());
    hfm.set_hits(gg_hits_);
    if (_dtc_.get() != 0) {
      hfm.set_calibration(*_dtc_.get());
    }
    hfm.set_t0(0.0 * CLHEP::ns);
    const double eps = 1.0e-2;
    hfm.set_fit_eps(eps);
    hfm.set_guess(iguess->second);
    hfm.initialize(_helix_fit_setup_);
    hfm.fit();

    if (hfm.get_solution().ok) {
      TrackFit::helix_fit_solution& the_solution = hfm.grab_solution();

      // Store initial guess as properties:
      the_solution.auxiliaries.store_string("guess", iguess->first);

      if (get_logging_priority() >= datatools::logger::PRIO_INFORMATION) {
        DT_LOG_INFORMATION(get_logging_priority(), "Solution in working frame has been found after "
                                                       << the_solution.niter << " iterations:");
        the_solution.tree_dump(std::clog, "", "[information]: ");

        DT_LOG_INFORMATION(get_logging_priority(), "Fit residuals:");
        for (size_t i = 0; i < gg_hits_.size(); ++i) {
          if (i == gg_hits_.size() - 1) {
            std::clog << "[information]: " << datatools::i_tree_dumpable::last_tag;
          } else {
            std::clog << "[information]: " << datatools::i_tree_dumpable::tag;
          }
          std::clog << "Hit #" << std::setw(2) << i << " : ";
          double alpha_residual, beta_residual;
          hfm.get_residuals_per_hit(i, alpha_residual, beta_residual, true);
          std::clog << " Rai = " << alpha_residual;
          std::clog << " Rbi = " << beta_residual;
          std::clog << std::endl;
        }
      }
      solutions_.push_back(the_solution);
    } else {
      DT_LOG_INFORMATION(get_logging_priority(),
                         "No solution has been found for guess '" << iguess->first << "' !");
    }
    hfm.reset();
  }
  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

void trackfit_driver::_compute_line_fit_solutions_(
    const TrackFit::gg_hits_col& gg_hits_, const line_guess_dict_type& guesses_,
    std::list<TrackFit::line_fit_solution>& solutions_) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");
  DT_LOG_INFORMATION(get_logging_priority(), "Perform the fit...");
  for (line_guess_dict_type::const_iterator iguess = guesses_.begin(); iguess != guesses_.end();
       ++iguess) {
    DT_LOG_INFORMATION(get_logging_priority(), "Starting fit for guess '" << iguess->first << "'");
    TrackFit::line_fit_mgr lfm;
    lfm.set_logging_priority(get_logging_priority());
    if (_dtc_.get() != 0) {
      lfm.set_calibration(*_dtc_.get());
    }
    lfm.set_hits(gg_hits_);
    lfm.set_t0(0.0 * CLHEP::ns);
    const double eps = 1.0e-2;
    lfm.set_fit_eps(eps);
    lfm.set_guess(iguess->second);
    lfm.initialize(_line_fit_setup_);
    lfm.fit();

    if (lfm.get_solution().ok) {
      TrackFit::line_fit_solution& the_solution = lfm.grab_solution();

      // Store initial guess as properties:
      the_solution.auxiliaries.store_string("guess", iguess->first);

      if (get_logging_priority() >= datatools::logger::PRIO_INFORMATION) {
        DT_LOG_INFORMATION(get_logging_priority(), "Solution in working frame has been found after "
                                                       << the_solution.niter << " iterations:");
        the_solution.tree_dump(std::clog, "", "[information]: ");

        DT_LOG_INFORMATION(get_logging_priority(), "Fit residuals:");
        for (size_t i = 0; i < gg_hits_.size(); ++i) {
          if (i == gg_hits_.size() - 1) {
            std::clog << "[information]: " << datatools::i_tree_dumpable::last_tag;
          } else {
            std::clog << "[information]: " << datatools::i_tree_dumpable::tag;
          }
          std::clog << "Hit #" << std::setw(2) << i << " : ";
          double alpha_residual, beta_residual;
          lfm.get_residuals_per_hit(i, alpha_residual, beta_residual, true);
          std::clog << " Rai = " << alpha_residual;
          std::clog << " Rbi = " << beta_residual;
          std::clog << std::endl;
        }
      }
      solutions_.push_back(the_solution);
    } else {
      DT_LOG_INFORMATION(get_logging_priority(),
                         "No solution has been found for guess '" << iguess->first << "' !");
    }
    lfm.reset();
  }
  DT_LOG_TRACE(get_logging_priority(), "Exiting.");
  return;
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::trackfit_driver, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::trackfit_driver");
  ocd_.set_class_description("A driver class for the TrackFit algorithm");
  ocd_.set_class_library("Falaise_TrackFit");
  ocd_.set_class_documentation(
      "The driver manager for the TrackFit fitting algorithms\n"
      "is able to perform line and/or helix fit given some   \n"
      "collections of clusters of tracker calibrated hits    \n"
      "(Geiger mode).");

  ::snemo::processing::base_tracker_fitter::ocd_support(ocd_, "");

  {
    // Description of the 'drift_time_calibration_label' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("drift_time_calibration_label")
        .set_terse_description("Label of the drift time calibration algorithm")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "This parameter is only useful when one wants to perform  \n"
            "the fit with on-the-fly calibration of the drift radius. \n")
        .set_default_value_string("snemo")
        .add_example(
            "Use the SuperNEMO basic calibration::               \n"
            "                                                    \n"
            "  drift_time_calibration_label : string = \"snemo\" \n"
            "                                                    \n");
  }

  {
    // Description of the 'fitting_models' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("fitting_models")
        .set_terse_description("Names of the trajectory models to be fitted")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_mandatory(false)
        .set_long_description(
            "Values are collected from the following list: \n"
            " \"line\", \"helix\".                         \n")
        .add_example(
            "Use all possible fits::                            \n"
            "                                                   \n"
            "  fitting_models : string[2] = \"line\" \"helix\"  \n"
            "                                                   \n");
  }

  {
    // Description of the 'line.only_guess' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("line.only_guess")
        .set_terse_description("Labels of the guess to be used for the linear fit")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_mandatory(false)
        .set_long_description(
            "Values are collected from the following list: \n"
            "  \"BB\", \"BT\", \"TB\", \"TT\".             \n")
        .add_example(
            "Use all the possible guesses::              \n"
            "                                            \n"
            "  line.only_guess : string[4]= \\           \n"
            "     \"BB\" \"BT\" \"TB\" \"TT\"            \n"
            "                                            \n");
  }

  {
    // Description of the 'helix.only_guess' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("helix.only_guess")
        .set_terse_description("Labels of the guess to be used for the helix fit")
        .set_traits(datatools::TYPE_STRING, datatools::configuration_property_description::ARRAY)
        .set_mandatory(false)
        .set_long_description(
            "Values are collected from the following list: \n"
            "  \"BBB\", \"BBT\", \"BTB\", \"BTT\",         \n"
            "  \"TBB\", \"TBT\", \"TTB\", \"TTT\".         \n")
        .add_example(
            "Use all the possible guesses::                \n"
            "                                              \n"
            "  line.only_guess : string[8]= \\             \n"
            "    \"BBB\" \"BBT\" \"BTB\" \"BTT\" \\        \n"
            "    \"TBB\" \"TBT\" \"TTB\" \"TTT\"           \n"
            "                                              \n");
  }

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::trackfit_driver,
                               "snemo::reconstruction::trackfit_driver")
