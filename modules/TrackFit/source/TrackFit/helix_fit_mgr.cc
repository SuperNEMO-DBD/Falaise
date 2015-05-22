/// \file falaise/TrackFit/helix_fit_mgr.cc

// Ourselves:
#include <TrackFit/helix_fit_mgr.h>

// Standard library:
#include <limits>

// Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_deriv.h>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/logger.h>
#include <datatools/temporary_files.h>
// - Bayeux/mygsl:
#include <mygsl/mean.h>
// - Bayeux/geomtools:
#include <geomtools/clhep.h>
#include <geomtools/utils.h>
#include <geomtools/helix_3d.h>
#include <geomtools/rectangle.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>

// This project:
#include <TrackFit/fit_utils.h>
#include <TrackFit/i_drift_time_calibration.h>

namespace TrackFit {

  bool helix_fit_params::has_quality() const
  {
    return quality >= 0;
  }

  int helix_fit_params::get_quality() const
  {
    return quality;
  }

  void helix_fit_params::set_quality(int q_)
  {
    quality = q_ < 0 ? -1 : q_;
    return;
  }

  void helix_fit_params::reset_quality()
  {
    set_quality(-1);
    return;
  }

  bool helix_fit_params::is_valid() const
  {
    return(x0 == x0);
  }

  helix_fit_params::helix_fit_params()
  {
    reset();
    return;
  }

  void helix_fit_params::reset()
  {
    quality    = -1;
    x0         = std::numeric_limits<double>::quiet_NaN();
    y0         = std::numeric_limits<double>::quiet_NaN();
    z0         = std::numeric_limits<double>::quiet_NaN();
    r          = std::numeric_limits<double>::quiet_NaN();
    step       = std::numeric_limits<double>::quiet_NaN();
    start_time = std::numeric_limits<double>::quiet_NaN();
    has_angles = false;
    angle_1    = - M_PI;
    angle_2    = + M_PI;
    return;
  }

  void helix_fit_params::dump(std::ostream      & out_,
                              const std::string & title_,
                              const std::string & indent_) const
  {
    out_ << indent_ << title_ << std::endl;
    out_ << indent_ << "|-- quality    = " << quality << std::endl
         << indent_ << "|-- x0         = " << x0 << std::endl
         << indent_ << "|-- y0         = " << y0 << std::endl
         << indent_ << "|-- z0         = " << z0 << std::endl
         << indent_ << "|-- r          = " << r  << std::endl
         << indent_ << "|-- step       = " << step << std::endl
         << indent_ << "`-- has_angles = " << has_angles << std::endl
         << indent_ << "    |-- theta1 = " << angle_1 << std::endl
         << indent_ << "    `-- theta2 = " << angle_2 << std::endl;
    return;
  }

  /*************************************************/

  const gg_hits_col helix_fit_data::get_hits() const
  {
    return * hits;
  }

  bool helix_fit_data::is_valid() const
  {
    if (hits == 0) return false;
    if (calibration == 0) return false;
    return true;
  }

  void helix_fit_data::reset()
  {
    hits             = 0;
    calibration      = 0;
    start_time       = 0.0;
    using_first      = false;
    using_last       = false;
    using_drift_time = false;
    return;
  }

  helix_fit_data::helix_fit_data()
  {
    reset();
    return;
  }

  void helix_fit_solution::print(std::ostream & out_) const
  {
    out_ << "Solution found:" << std::endl
         << "|-- ok      = " << ok << std::endl
         << "|-- quality = " << quality << std::endl
         << "|-- x0      = " << x0 << " +/- " << err_x0 << std::endl
         << "|-- y0      = " << y0 << " +/- " << err_y0 << std::endl
         << "|-- z0      = " << z0 << " +/- " << err_z0 << std::endl
         << "|-- r       = " << r  << " +/- " << err_r << std::endl
         << "|-- step    = " << step << " +/- " << err_step << std::endl
         << "|-- start_time = " << start_time << std::endl
         << "|-- chi     = " << chi << std::endl
         << "|-- ndof    = " << ndof << std::endl
         << "|-- niter   = " << niter << std::endl
         << "|-- prob(P) = " << probability_p() << std::endl
         << "`-- prob(Q) = " << probability_q()
         << std::endl;
    return;
  }

  helix_fit_solution::helix_fit_solution() : helix_fit_params()
  {
    reset();
    return;
  }

  helix_fit_solution::~helix_fit_solution()
  {
    reset();
    return;
  }

  void helix_fit_solution::reset()
  {
    helix_fit_params::reset();
    ok       = false;
    err_x0   = std::numeric_limits<double>::quiet_NaN();
    err_y0   = std::numeric_limits<double>::quiet_NaN();
    err_z0   = std::numeric_limits<double>::quiet_NaN();
    err_r    = std::numeric_limits<double>::quiet_NaN();
    err_step = std::numeric_limits<double>::quiet_NaN();
    chi      = std::numeric_limits<double>::infinity();
    ndof     = 0;
    niter    = 0;
    return;
  }

  double helix_fit_solution::probability_p() const
  {
    return gsl_cdf_chisq_P(chi * chi, ndof);
  }

  double helix_fit_solution::probability_q() const
  {
    return gsl_cdf_chisq_Q(chi * chi, ndof);
  }

  void helix_fit_mgr::draw_solution(std::ostream & out_) const
  {
    draw_solution(out_, _solution_);
    return;
  }

  void helix_fit_mgr::compute_angles(const gg_hits_col & hits_,
                                     helix_fit_params & traj_,
                                     bool compute_step_,
                                     const uint32_t flags_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    if (flags_ & WARNING) local_priority = datatools::logger::PRIO_WARNING;
    if (flags_ & DEVEL)   local_priority = datatools::logger::PRIO_TRACE;

    const geomtools::vector_3d Oh(traj_.x0, traj_.y0, 0.);
    DT_LOG_TRACE(local_priority, "Oh = " << Oh);

    double mean_x = 0.;
    double mean_y = 0.;
    double mean_z = 0.;
    for (gg_hits_col::const_iterator hit_iter = hits_.begin();
         hit_iter != hits_.end(); ++hit_iter) {
      mean_x += hit_iter->get_x();
      mean_y += hit_iter->get_y();
      mean_z += hit_iter->get_z();
    }
    mean_x /= hits_.size();
    mean_y /= hits_.size();
    mean_z /= hits_.size();

    const geomtools::vector_3d mean_hit(mean_x, mean_y, 0);
    geomtools::vector_3d mean_direction = mean_hit - Oh;
    const double mean_angle = atan2(mean_direction.y(), mean_direction.x());
    DT_LOG_TRACE(local_priority, "mean hit angle = " << mean_angle / M_PI * 180.);;

    geomtools::rotation_3d dir_rot;
    geomtools::create_zyz(dir_rot, mean_angle, 0., 0.);
    geomtools::vector_3d trans_mean_dir   = mean_direction.transform(dir_rot);
    const double trans_mean_angle = atan2(trans_mean_dir.y(), trans_mean_dir.x());
    DT_LOG_TRACE(local_priority, "mean hit angle after zyz rotation = " << trans_mean_angle / M_PI * 180.);

    double min_angle = +2. * M_PI;
    double max_angle = -2. * M_PI;

    mygsl::arithmetic_mean mean_z_l;
    mygsl::arithmetic_mean mean_z_r;
    mygsl::arithmetic_mean mean_theta_l;
    mygsl::arithmetic_mean mean_theta_r;

    for (gg_hits_col::const_iterator hit_iter = hits_.begin();
         hit_iter != hits_.end(); ++hit_iter) {
      const geomtools::vector_3d the_hit_pos(hit_iter->get_x(),
                                             hit_iter->get_y(),
                                             Oh.z() );
      // rotate hit of mean_angle...
      geomtools::vector_3d direction = the_hit_pos - Oh;
      geomtools::vector_3d trans_dir = direction.transform(dir_rot);
      double angle = atan2(trans_dir.y(), trans_dir.x());

      if (compute_step_) {
        if (angle < 0.) {
          mean_theta_l.add(angle);
          mean_z_l.add(hit_iter->get_z());
        } else {
          mean_theta_r.add(angle);
          mean_z_r.add(hit_iter->get_z());
        }
      }

      angle += mean_angle;
      DT_LOG_TRACE(local_priority, "angle = " << angle / M_PI * 180.);

      if (angle < min_angle)
        min_angle = angle;
      if (angle > max_angle)
        max_angle = angle;
    }

    if (compute_step_) {
      const double theta_l = mean_theta_l.get_mean();
      const double z_l     = mean_z_l.get_mean();
      const double theta_r = mean_theta_r.get_mean();
      const double z_r     = mean_z_r.get_mean();
      traj_.step =(z_r - z_l) /(theta_r - theta_l);
      traj_.z0   = z_r - traj_.step *(theta_r + mean_angle);
      traj_.step *= 2. * M_PI;
    }

    const double t = round((traj_.z0 + traj_.step *(mean_angle /(2. * M_PI )) - mean_z) / traj_.step);
    DT_LOG_TRACE(local_priority, "min_angle = " << min_angle / M_PI * 180.);
    DT_LOG_TRACE(local_priority, "max_angle = " << max_angle / M_PI * 180.);
    DT_LOG_TRACE(local_priority, "mean_z = " << mean_z);
    DT_LOG_TRACE(local_priority, "step   = " << traj_.step);
    DT_LOG_TRACE(local_priority, "t = " << t);

    if (traj_.step > 0.) {
      DT_LOG_TRACE(local_priority, "z0(before) = " << traj_.z0 << "(step > 0 )");
      traj_.z0 -= t * traj_.step;
      DT_LOG_TRACE(local_priority, "z0(after) = " << traj_.z0);
      DT_LOG_TRACE(local_priority,  mean_z - traj_.step / 2.
                   << " < z_rec == " << traj_.z0 + traj_.step * mean_angle
                   << " < " << mean_z + traj_.step / 2.);
    } else {
      DT_LOG_TRACE(local_priority, "z0(before) = " << traj_.z0 << "(step > 0 )");
      traj_.z0 += t * traj_.step;
      DT_LOG_TRACE(local_priority, "z0(after) = " << traj_.z0);
      DT_LOG_TRACE(local_priority,  mean_z - traj_.step / 2.
                   << " < z_rec == " << traj_.z0 + traj_.step * mean_angle
                   << " < " << mean_z - traj_.step / 2.);
    }

    traj_.angle_1    = min_angle;
    traj_.angle_2    = max_angle;
    traj_.has_angles = true;

    DT_LOG_TRACE(local_priority, "Computed guess");
    if (local_priority >= datatools::logger::PRIO_TRACE)
      traj_.dump(std::clog);
    return;
  }

  void helix_fit_mgr::draw_solution(std::ostream & out_,
                                    const helix_fit_solution & sol_)
  {
    const geomtools::vector_3d Oh(sol_.x0, sol_.y0, sol_.z0);
    const double r = sol_.r;
    const double step = sol_.step;
    geomtools::helix_3d h3d;
    h3d.set_center(Oh);
    h3d.set_radius(r);
    h3d.set_step  (step);

    double min_angle = -M_PI;
    double max_angle = +M_PI;
    if ( sol_.has_angles ) {
      min_angle = sol_.angle_1;
      max_angle = sol_.angle_2;
    }
    h3d.set_angle1( min_angle );
    h3d.set_angle2( max_angle ); // updated at the end

    // DT_LOG_TRACE(get_logging_priority(), "Reconstructed h3d");
    // if (get_logging_priority() >= datatools::logger::PRIO_TRACE)
    //   h3d.tree_dump(std::clog);

    geomtools::helix_3d::print_xyz(out_, h3d, 0.01, 0);
    return;
  }

  void helix_fit_mgr::draw_temporary_solution(std::ostream & out_) const
  {
    helix_fit_solution sol;
    sol.start_time = _t0_;
    sol.x0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               helix_fit_params::PARAM_INDEX_X0);
    sol.y0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               helix_fit_params::PARAM_INDEX_Y0);
    sol.z0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               helix_fit_params::PARAM_INDEX_Z0);
    sol.r     = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               helix_fit_params::PARAM_INDEX_R);
    sol.step  = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               helix_fit_params::PARAM_INDEX_STEP);
    sol.chi   = gsl_blas_dnrm2(_fit_mf_fdf_solver_->f);
    sol.ndof  = _fit_npoints_ - _fit_npars_;
    sol.niter = _fit_iter_;
    compute_angles(*_hits_, sol);
    draw_solution(out_, sol);

    DT_LOG_TRACE(get_logging_priority(), "Temporary solution: ");
    DT_LOG_TRACE(get_logging_priority(),"   solution.x0    = " << sol.x0);
    DT_LOG_TRACE(get_logging_priority(),"   solution.y0    = " << sol.y0);
    DT_LOG_TRACE(get_logging_priority(),"   solution.z0    = " << sol.z0);
    DT_LOG_TRACE(get_logging_priority(),"   solution.r     = " << sol.r);
    DT_LOG_TRACE(get_logging_priority(),"   solution.step  = " << sol.step);
    DT_LOG_TRACE(get_logging_priority(),"   solution.chi   = " << sol.chi);
    DT_LOG_TRACE(get_logging_priority(),"   solution.ndof  = " << sol.ndof);
    DT_LOG_TRACE(get_logging_priority(),"   solution.niter = " << sol.niter);

    return;
  }

  /*****************************************************************/

  helix_fit_residual_function_param::helix_fit_residual_function_param()
  {
    using_last       = false;
    using_first      = false;
    using_drift_time = false;
    mode             = helix_fit_params::PARAM_INDEX_INVALID;
    residual_type    = RESIDUAL_INVALID;
    first            = false;
    last             = false;
    dtc              = 0;
    start_time       = 0.0 * CLHEP::ns;
    return;
  }

  /*****************************************************************/

  unsigned int helix_fit_mgr::constants::default_fit_max_iter()
  {
    static unsigned int max_iteration = 100;
    return max_iteration;
  }

  double helix_fit_mgr::constants::default_fit_eps()
  {
    static double epsilon = 1.e-3;
    return epsilon;
  }

  unsigned int helix_fit_mgr::constants::min_number_of_hits()
  {
    static unsigned int minimal_nbr_hits = 4;
    return minimal_nbr_hits;
  }

  void helix_fit_mgr::set_logging_priority(datatools::logger::priority priority_)
  {
    _logging_priority_ = priority_;
    return;
  }

  datatools::logger::priority helix_fit_mgr::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool helix_fit_mgr::is_initialized() const
  {
    return _initialized_;
  }

  void helix_fit_mgr::set_initialized(bool initialized_)
  {
    _initialized_ = initialized_;
    return;
  }

  bool helix_fit_mgr::is_debug() const
  {
    return get_logging_priority() >= datatools::logger::PRIO_DEBUG;
  }

  void helix_fit_mgr::set_debug(bool debug_)
  {
    if (debug_) set_logging_priority(datatools::logger::PRIO_DEBUG);
    else        set_logging_priority(datatools::logger::PRIO_WARNING);
    return;
  }

  const helix_fit_solution & helix_fit_mgr::get_solution()
  {
    return _solution_;
  }

  helix_fit_solution & helix_fit_mgr::grab_solution()
  {
    return _solution_;
  }

  void helix_fit_mgr::set_t0(double t0_)
  {
    _t0_ = t0_;
    return;
  }

  void helix_fit_mgr::set_fit_max_iter(size_t fit_max_iter_)
  {
    _fit_max_iter_ = fit_max_iter_;
    return;
  }

  void helix_fit_mgr::set_fit_eps(double eps_)
  {
    _fit_eps_ = eps_;
    return;
  }

  void helix_fit_mgr::set_guess(const helix_fit_params & guess_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");
    _fit_x_init_[helix_fit_params::PARAM_INDEX_X0]   = guess_.x0;
    _fit_x_init_[helix_fit_params::PARAM_INDEX_Y0]   = guess_.y0;
    _fit_x_init_[helix_fit_params::PARAM_INDEX_Z0]   = guess_.z0;
    _fit_x_init_[helix_fit_params::PARAM_INDEX_R]    = guess_.r;
    _fit_x_init_[helix_fit_params::PARAM_INDEX_STEP] = guess_.step;
    DT_LOG_DEBUG(get_logging_priority(), "X0    = "
                 << _fit_x_init_[helix_fit_params::PARAM_INDEX_X0] / CLHEP::mm << " mm");
    DT_LOG_DEBUG(get_logging_priority(), "Y0    = "
                 << _fit_x_init_[helix_fit_params::PARAM_INDEX_Y0] / CLHEP::mm << " mm");
    DT_LOG_DEBUG(get_logging_priority(), "Z0    = "
                 << _fit_x_init_[helix_fit_params::PARAM_INDEX_Z0] / CLHEP::mm << " mm");
    DT_LOG_DEBUG(get_logging_priority(), "R     = "
                 << _fit_x_init_[helix_fit_params::PARAM_INDEX_R] / CLHEP::mm << " mm");
    DT_LOG_DEBUG(get_logging_priority(), "STEP  = "
                 << _fit_x_init_[helix_fit_params::PARAM_INDEX_STEP] /(CLHEP::mm / CLHEP::degree) << " mm/degree");

    DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    return;
  }

  void helix_fit_mgr::_set_defaults_()
  {
    _logging_priority_  = datatools::logger::PRIO_FATAL;

    _fit_npars_         = helix_fit_params::HELIX_FIT_FIXED_START_TIME_NOPARS;
    _fit_npoints_       = 0;
    _fit_mf_fdf_solver_ = 0;
    _fit_covar_         = 0;
    _fit_iter_          = 0;
    _fit_max_iter_      = helix_fit_mgr::constants::default_fit_max_iter();
    _fit_eps_           = helix_fit_mgr::constants::default_fit_eps();
    _fit_status_        = GSL_CONTINUE;

    _hits_        = 0;
    _calibration_ = 0;
    _t0_          = 0.0 * CLHEP::ns;

    _solution_.ok      = false;

    _using_first_      = false;
    _using_last_       = false;
    _using_drift_time_ = false;

    // Debug flags :
    _step_print_status_ = false;
    _step_draw_         = false; // Only with TRACKFIT_TEST_WITH_DRAW_ONLINE == 1
    return;
  }

  void helix_fit_mgr::set_hits(const gg_hits_col & hits_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Object is now locked ! Operation is not allowed !");
    _hits_ = &hits_;
    return;
  }

  bool helix_fit_mgr::has_calibration() const
  {
    return _calibration_ != 0;
  }

  void helix_fit_mgr::set_calibration(const i_drift_time_calibration & calibration_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Object is now locked ! Operation is not allowed !");
    _calibration_ = &calibration_;
    return;
  }

  // ctor:
  helix_fit_mgr::helix_fit_mgr()
  {
    _set_defaults_();
    set_initialized(false);
    return;
  }

  // dtor:
  helix_fit_mgr::~helix_fit_mgr()
  {
    if (is_initialized()) reset();
    return;
  }

  void helix_fit_mgr::initialize(const datatools::properties & config_)
  {
    init(config_);
    return;
  }

  void helix_fit_mgr::init(const datatools::properties & config_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    // 2013-06-03 XG: Sanity checks but you may want to be less strict on that
    // and just warn people.. to be continued...
    DT_THROW_IF (_hits_ == 0, std::logic_error, "No hits !");

    const size_t nhits = _hits_->size();
    DT_THROW_IF (nhits < helix_fit_mgr::constants::min_number_of_hits(),
                 std::logic_error, "Not enough hits !");
    DT_LOG_DEBUG(get_logging_priority(), "nhits=" << nhits);

    _fit_npoints_ = 2 * nhits;
    _fit_npars_ = helix_fit_params::HELIX_FIT_FIXED_START_TIME_NOPARS;
    DT_LOG_DEBUG(get_logging_priority(), "Number of free parameters: " << _fit_npars_);
    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'fit data'...");
    _fit_covar_ = gsl_matrix_alloc(_fit_npars_, _fit_npars_);

    if (config_.has_flag("step_print_status")) {
      _step_print_status_ = true;
    }

    if (config_.has_flag("step_draw")) {
      _step_draw_ = true;
    }

    if (config_.has_flag("using_first")) {
      _using_first_ = true;
    }

    if (config_.has_flag("using_last")) {
      _using_last_ = true;
    }

    if (config_.has_flag("using_drift_time")) {
      _using_drift_time_ = true;
    }
    DT_THROW_IF (_using_drift_time_ && ! has_calibration(), std::logic_error, "Missing drift time calibration !");

    _fit_data_.using_first      = _using_first_;
    _fit_data_.using_last       = _using_last_;
    _fit_data_.using_drift_time = _using_drift_time_;
    _fit_data_.hits             = _hits_;
    _fit_data_.calibration      = _calibration_;
    _fit_data_.start_time       = _t0_;

    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'fdf'...");
    _fit_mf_fdf_function_.f      = &residual_f;
    _fit_mf_fdf_function_.df     = &residual_df;
    _fit_mf_fdf_function_.fdf    = &residual_fdf;
    _fit_mf_fdf_function_.p      = _fit_npars_;
    _fit_mf_fdf_function_.n      = _fit_npoints_;
    _fit_mf_fdf_function_.params = &_fit_data_;

    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'solver'...");
    const gsl_multifit_fdfsolver_type * T = gsl_multifit_fdfsolver_lmder;
    _fit_mf_fdf_solver_ = gsl_multifit_fdfsolver_alloc(T, _fit_npoints_, _fit_npars_);
    DT_THROW_IF (_fit_mf_fdf_solver_ == 0, std::logic_error, "Cannot create solver !");
    const std::string fdsolver_name = gsl_multifit_fdfsolver_name(_fit_mf_fdf_solver_);
    DT_LOG_DEBUG(get_logging_priority(), "Solver name is '" << fdsolver_name << "'");

    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'view'...");
    _fit_vview_ = gsl_vector_view_array(_fit_x_init_, _fit_npars_);

    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'solver' with 'fdf'...");
    gsl_multifit_fdfsolver_set(_fit_mf_fdf_solver_,
                               &_fit_mf_fdf_function_,
                               &_fit_vview_.vector);
    DT_LOG_DEBUG(get_logging_priority(), "'solver' is setup.");

    set_initialized(true);
    DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    return;
  }

  void helix_fit_mgr::reset()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

    if (_fit_mf_fdf_solver_ != 0) {
      const std::string fdsolver_name = gsl_multifit_fdfsolver_name(_fit_mf_fdf_solver_);
      DT_LOG_DEBUG(get_logging_priority(), "Free solver '" << fdsolver_name << "'");
      gsl_multifit_fdfsolver_free(_fit_mf_fdf_solver_);
    }
    if (_fit_covar_ != 0) {
      gsl_matrix_free(_fit_covar_);
    }
    _fit_data_.reset();
    _set_defaults_();
    set_initialized(false);
    return;
  }

  bool helix_fit_mgr::is_using_last() const
  {
    return _using_last_;
  }

  bool helix_fit_mgr::is_using_first() const
  {
    return _using_first_;
  }

  bool helix_fit_mgr::is_using_drift_time() const
  {
    return _using_drift_time_;
  }

  void helix_fit_mgr::print_fit_status(std::ostream & out_) const
  {
    out_ << "TrackFit::helix_fit_mgr::print_fit_status:" << std::endl;
    out_ << "|-- " << "Iterations: " << std::endl;
    out_ << "|   " << "|-- " << "iter:   " << _fit_iter_ << std::endl;
    out_ << "|   " << "|-- " << "status: " << gsl_strerror(_fit_status_) << std::endl;
    out_ << "|   " << "|-- " << "x0= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_X0) << std::endl;
    out_ << "|   " << "|-- " << "y0= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_Y0) << std::endl;
    out_ << "|   " << "|-- " << "z0= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_Z0) << std::endl;
    out_ << "|   " << "|-- " << "r= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_R) << std::endl;
    out_ << "|   " << "|-- " << "step= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_STEP) << std::endl;
    out_ << "|   " << "`-- " << "f= "
         << gsl_blas_dnrm2(_fit_mf_fdf_solver_->f) << std::endl;
    out_ << "`-- " << "Solution: " << "[NOT IMPLEMENTED]" << std::endl;
    return;
  }

  void helix_fit_mgr::at_fit_step_do()
  {
    if (_step_print_status_) {
      DT_LOG_DEBUG(get_logging_priority(), "status = " << gsl_strerror(_fit_status_));
      print_fit_status(std::clog);
    }

    const bool step_draw = _step_draw_;
    // step_draw = true; // DEVEL
    if (step_draw) {
      datatools::temp_file ftmp;
      ftmp.set_remove_at_destroy(true);
      ftmp.create("/tmp", "temp_helix_fit_step_");
      draw_temporary_solution(ftmp.out());
      ftmp.out() << std::endl << std::endl;
      draw_hits(ftmp.out(), *_hits_);
      ftmp.out() << std::endl << std::endl;

      usleep(200.0);
      std::ostringstream title_oss;
      title_oss << "line_fit_mgr::at_fit_step_do : Fit step #" << _fit_iter_ << "(XY-view)";

      Gnuplot g1("lines");
      g1.set_title(title_oss.str());
      std::ostringstream cmd_oss;
      cmd_oss << "set size ratio -1; ";
      cmd_oss << "set grid; ";
      cmd_oss << "plot ";
      cmd_oss << " '" << ftmp.get_filename() << "' index 0 using 1:2 title \"Step solution\" with lines , ";
      cmd_oss << " '" << ftmp.get_filename() << "' index 1 using 1:2 title \"Hits\" with lines ";

      g1.cmd(cmd_oss.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
    }
    return;
  }

  void helix_fit_mgr::fit()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Starting fit...");
    DT_THROW_IF (! is_initialized(), std::logic_error, "Fit manager is not initialized !");

    const double r_crit = 10. * CLHEP::km;
    double r_ref = -std::numeric_limits<double>::infinity();
    size_t count_r_crit = 0;
    const size_t count_r_crit_limit = 10;
    bool under_r_crit_limit = true;

    do {
      DT_LOG_DEBUG(get_logging_priority(), "Fit loop #" << _fit_iter_);
      _fit_iter_++;
      _fit_status_ = gsl_multifit_fdfsolver_iterate(_fit_mf_fdf_solver_);
      DT_LOG_DEBUG(get_logging_priority(), "Iterate status = " << gsl_strerror(_fit_status_));

      // Do not break fit loop for GSL_SUCCESS but also when iteration
      // does not make progress towards solution. Then, redo the fit
      // and hope that the next iteration will be fine. Basically this
      // happens when the initial fitted values are pretty close to
      // the chi2 minimal value.
      if (_fit_status_ != GSL_SUCCESS &&
          _fit_status_ != GSL_ENOPROG) break;
      _fit_status_ = gsl_multifit_test_delta(_fit_mf_fdf_solver_->dx,
                                             _fit_mf_fdf_solver_->x,
                                             _fit_eps_,
                                             _fit_eps_);
      at_fit_step_do();

      const double r = gsl_vector_get(_fit_mf_fdf_solver_->x,
                                      helix_fit_params::PARAM_INDEX_R);
      if (r > r_crit) {
        DT_LOG_DEBUG(get_logging_priority(), "r > r_crit");
        if (r < r_ref) {
          r_ref = -1.;
          count_r_crit = 0;
        } else {
          r_ref = r;
          count_r_crit++;
        }
        if (count_r_crit >= count_r_crit_limit) {
          under_r_crit_limit = false;
          DT_LOG_DEBUG(get_logging_priority(),  "Stop fit at iter = "
                       << _fit_iter_ << " with fit_status = " << _fit_status_);
          break;
        }
      }
      DT_LOG_DEBUG(get_logging_priority(), "Test_delta status = " << gsl_strerror(_fit_status_));
    } while((_fit_status_ == GSL_CONTINUE) &&(_fit_iter_ < _fit_max_iter_));

    if (_fit_status_ <= GSL_SUCCESS && under_r_crit_limit) {
      DT_LOG_DEBUG(get_logging_priority(), "Calling gsl_multifit_covar...");
      gsl_multifit_covar(_fit_mf_fdf_solver_->J, 0.0, _fit_covar_);

      _solution_.ok = true;
      _solution_.start_time = _t0_;
      _solution_.x0   = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_X0);
      _solution_.y0   = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_Y0);
      _solution_.z0   = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_Z0);
      _solution_.r    = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_R);
      _solution_.step = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_STEP);

      _solution_.err_x0 =
        sqrt(gsl_matrix_get(_fit_covar_,
                            helix_fit_params::PARAM_INDEX_X0,
                            helix_fit_params::PARAM_INDEX_X0));
      _solution_.err_y0 =
        sqrt(gsl_matrix_get(_fit_covar_,
                            helix_fit_params::PARAM_INDEX_Y0,
                            helix_fit_params::PARAM_INDEX_Y0));
      _solution_.err_z0 =
        sqrt(gsl_matrix_get(_fit_covar_,
                            helix_fit_params::PARAM_INDEX_Z0,
                            helix_fit_params::PARAM_INDEX_Z0));
      _solution_.err_r  =
        sqrt(gsl_matrix_get(_fit_covar_,
                            helix_fit_params::PARAM_INDEX_R,
                            helix_fit_params::PARAM_INDEX_R));
      _solution_.err_step =
        sqrt(gsl_matrix_get(_fit_covar_,
                            helix_fit_params::PARAM_INDEX_STEP,
                            helix_fit_params::PARAM_INDEX_STEP));

      compute_angles(_fit_data_.get_hits(), _solution_);

      _solution_.chi   = gsl_blas_dnrm2(_fit_mf_fdf_solver_->f);
      _solution_.ndof  = _fit_npoints_ - _fit_npars_;
      _solution_.niter = _fit_iter_;
    } else {
      _solution_.ok = false;
    }

    if (is_debug() && _solution_.ok)
      get_residuals_at_solution();

    return;
  }

  double helix_fit_mgr::residual_function(double x_, void * params_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");
    const helix_fit_residual_function_param * param_ptr =
      static_cast<const helix_fit_residual_function_param *>(params_);
    DT_THROW_IF (param_ptr == 0, std::logic_error, "Invalid cast !");

    const helix_fit_residual_function_param & param = *param_ptr;

    // calibration
    DT_THROW_IF (param.using_drift_time && param.dtc == 0,
                 std::logic_error,
                 "Drift time should be recomputed by some drift-time calibration algo !");
    const i_drift_time_calibration * dtc = param.dtc;
    const bool   using_first      = param.using_first;
    const bool   using_last       = param.using_last;
    const bool   using_drift_time = param.using_drift_time;
    const double start_time       = param.start_time;

    // parameters from the helix:
    double x0   = param.x0;
    double y0   = param.y0;
    double z0   = param.z0;
    double r    = param.r;
    double step = param.step;

    // dynamic parameter:
    const int mode          = param.mode;
    const int residual_type = param.residual_type;
    if (mode == helix_fit_params::PARAM_INDEX_X0)   x0   = x_;
    if (mode == helix_fit_params::PARAM_INDEX_Y0)   y0   = x_;
    if (mode == helix_fit_params::PARAM_INDEX_Z0)   z0   = x_;
    if (mode == helix_fit_params::PARAM_INDEX_R)    r    = x_;
    if (mode == helix_fit_params::PARAM_INDEX_STEP) step = x_;
    DT_LOG_TRACE(local_priority, "x0 = " << x0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "y0 = " << y0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "z0 = " << z0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "r  = " << r / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "step = " << step /(CLHEP::mm / CLHEP::degree)  << " mm/degree");

    // parameters from the hit:
    const bool   last     = param.last;
    const bool   first    = param.first;
    const double xi       = param.xi;
    const double yi       = param.yi;
    const double zi       = param.zi;
    const double sigma_zi = param.szi;
    const double ti       = param.ti;
    const double rmaxi    = param.rmaxi;

    double drift_distance       = param.ri  * CLHEP::mm;
    double sigma_drift_distance = param.dri * CLHEP::mm;

    // 2012-11-15 XG: if a isolated cell is delayed
    // i.e. 'drift_distance' is invalid then force the
    // 'drift_distance' value to rmax and 'sigma_drift_distance' to be
    // large enough : the cell weight is then pretty small
    if (! datatools::is_valid(drift_distance)) {
      drift_distance = param.rmaxi * CLHEP::mm;
      sigma_drift_distance = param.rmaxi * CLHEP::mm;
    }

    // 2012/02/15 XG: maybe here we can check if the geiger hit is
    // delayed and then redo the calibration for such hit using start
    // time value
    if (using_drift_time) {
      const double drift_time = ti - start_time;
      if (! dtc->drift_time_is_valid(drift_time)) {
        DT_LOG_WARNING(local_priority, "Drift_time is out of physics range!");
        DT_LOG_TRACE(local_priority, "ti         = " << ti);
        DT_LOG_TRACE(local_priority, "start_time = " << start_time);
        DT_LOG_TRACE(local_priority, "drift_time = " << drift_time);
      }
      dtc->drift_time_to_radius(drift_time,
                                drift_distance,
                                sigma_drift_distance);

      if (! dtc->radius_is_valid(drift_distance)) {
        DT_LOG_WARNING(local_priority, "Drift_distance is out of physics range!");
      }
    }
    // else
    //   {
    //     drift_time           = 0.;
    //     drift_distance       = 0.0 * CLHEP::mm;
    //     sigma_drift_distance = rmaxi / 2.;
    //   }

    DT_LOG_TRACE(local_priority, "drift_distance= " << drift_distance / CLHEP::mm << " mm");
    DT_LOG_TRACE(local_priority, "sigma_drift_distance= " << sigma_drift_distance / CLHEP::mm << " mm");

    const geomtools::vector_2d Oh(x0, y0);
    const geomtools::vector_2d Oi(xi, yi);
    const geomtools::vector_2d OhOi =(Oi - Oh);
    const geomtools::vector_2d udi = OhOi.unit();
    const geomtools::vector_2d OhPi = r * udi;
    const geomtools::vector_2d OiPi = OhPi - OhOi;

    const double di = OhOi.mag();
    geomtools::vector_2d OiTi = drift_distance * udi;
    geomtools::vector_2d OiCi = rmaxi * udi;
    if (di > r) {
      OiTi *= -1.;
      OiCi *= -1.;
    }
    const geomtools::vector_2d CiPi = OiPi - OiCi;
    const geomtools::vector_2d TiPi = OiPi - OiTi;

    double alpha_i = TiPi.mag();

    // 2012/02/15 XG: comment this part since I'm not sure what it was doing
    // if (! using_drift_time)
    //   {
    //     if (OiPi < rmaxi) // 2012-10-10 FM: this is effectively meaningless
    //       alpha_i = 0.;
    //     else
    //       alpha_i = CiPi.mag();
    //   }
    // else
    {
      if (using_last) {
        if (last){
          if (OiPi.mag() < OiTi.mag()) {
            alpha_i = 0.0;
          }
        }
      }
      if (using_first) {
        if (first){
          if (OiPi.mag() <= OiTi.mag()) {
            alpha_i = 0.0;
          }
        }
      }
    }

    const double sigma_ri = sigma_drift_distance;

    double beta_i = 0.0;
    double zL     = 0.0;
    const double theta_i = atan2(udi.y(), udi.x());
    const double eps_step = 1.e-8;

    if (std::abs(step) > eps_step) {
      const double dkmax =(zi - z0 - 0.5 * step * theta_i / M_PI) / step;
      const int kmax =(int) floor(dkmax);
      const int kminus = kmax;
      const int kplus = kmax + 1;
      DT_LOG_TRACE(local_priority, "dkmax  = " << dkmax);
      DT_LOG_TRACE(local_priority, "kmax   = " << kmax);
      DT_LOG_TRACE(local_priority, "kminus = " << kminus);

      double theta_minus = theta_i + kminus * 2 * M_PI;
      double theta_plus  = theta_minus + 2 * M_PI;

      double zLminus = z0 + step * theta_minus /(2 * M_PI);
      double zLplus = z0 + step * theta_plus /(2 * M_PI);
      if (zLminus > zLplus) {
        DT_LOG_TRACE(local_priority, "Swap z value !");
        std::swap(zLminus, zLplus);
      }
      if (zi < zLminus - 0.001) {
        DT_LOG_FATAL(local_priority, "zi == " << zi);
        DT_LOG_FATAL(local_priority, "|-- zi > zLminus == " << zLminus << "(KMINUS == " << kminus << ")");
        DT_LOG_FATAL(local_priority, "`-- zi < zLplus  == " << zLplus  << "(KPLUS  == " << kplus  << ")");
        DT_THROW_IF (true, std::logic_error, "****** STOP minus *******");
      }
      if (zi > zLplus + 0.001) {
        DT_LOG_FATAL(local_priority, "zi == " << zi);
        DT_LOG_FATAL(local_priority, "|-- zi > zLminus == " << zLminus << "(KMINUS == " << kminus << ")");
        DT_LOG_FATAL(local_priority, "`-- zi < zLplus  == " << zLplus  << "(KPLUS  == " << kplus  << ")");
        DT_THROW_IF (true, std::logic_error, "****** STOP plus *******");
      }
      const double beta_i_plus  = zLplus - zi;
      const double beta_i_minus = zLminus - zi;
      if (std::abs(beta_i_minus) < std::abs(beta_i_plus)) {
        zL = zLminus;
        beta_i = beta_i_minus;
      } else {
        zL = zLplus;
        beta_i = beta_i_plus;
      }
    } else {
      zL = z0;
      beta_i = z0 - zi;
    }

    DT_LOG_TRACE(local_priority, "z0       = " << z0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "zL       = " << zL / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "alpha_i  = " << alpha_i);
    DT_LOG_TRACE(local_priority, "sigma_ri = " << sigma_ri);
    DT_LOG_TRACE(local_priority, "beta_i   = " << beta_i);
    DT_LOG_TRACE(local_priority, "sigma_zi = " << sigma_zi);

    double Ri;
    if (residual_type == helix_fit_residual_function_param::RESIDUAL_ALPHA) {
      Ri  = alpha_i / sigma_ri;
    } else if ( residual_type == helix_fit_residual_function_param::RESIDUAL_BETA) {
      Ri  = beta_i / sigma_zi;
    } else {
      DT_THROW_IF (true, std::logic_error, "Invalid residual type !");
    }

    DT_LOG_TRACE(local_priority, "Exiting.");
    return Ri;
  }

  void helix_fit_mgr::get_residuals_at_solution() const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");

    int                 hit_counter = 0;
    const gg_hits_col * hits        = this->_hits_;

    for (gg_hits_col::const_iterator it_hit = hits->begin();
         it_hit != hits->end(); ++it_hit) {
      const double xi = it_hit->get_x();
      const double yi = it_hit->get_y();
      const double zi = it_hit->get_z();

      double alpha_i, beta_i;
      get_residuals_per_hit(hit_counter, alpha_i, beta_i, true);

      DT_LOG_TRACE(local_priority, "hit #" << hit_counter << "(" << xi << ", " << yi << ", " << zi << "):");
      DT_LOG_TRACE(local_priority, "|-- alpha_res = " << alpha_i);
      DT_LOG_TRACE(local_priority, "`-- beta_res  = " << beta_i);
      hit_counter++;
    }
    DT_LOG_TRACE(local_priority, "Exiting.");
    return;
  }

  void helix_fit_mgr::get_residuals_per_hit(size_t hit_index_,
                                            double & alpha_residual_,
                                            double & beta_residual_,
                                            bool at_solution_) const
  {
    // check hit index validity:
    const gg_hits_col * hits = this->_hits_;
    DT_THROW_IF (hit_index_ >= hits->size(), std::logic_error, "Invalid hit index !");
    helix_fit_residual_function_param param;
    // initialize the line parameters:
    if (! at_solution_) {
      param.x0    = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_X0);
      param.y0    = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_Y0);
      param.z0    = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_Z0);
      param.r     = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_R);
      param.step  = gsl_vector_get(_fit_mf_fdf_solver_->x, helix_fit_params::PARAM_INDEX_STEP);
    } else {
      DT_THROW_IF (! _solution_.ok, std::logic_error, "No available solution !");
      param.x0    = _solution_.x0;
      param.y0    = _solution_.y0;
      param.z0    = _solution_.z0;
      param.r     = _solution_.r;
      param.step  = _solution_.step;
    }
    size_t count = 0;
    gg_hits_col::const_iterator it_hit = hits->end();
    for (it_hit = hits->begin(); it_hit != hits->end(); ++it_hit, count++) {
      if (count == hit_index_) break;
    }
    DT_THROW_IF (it_hit == hits->end(), std::logic_error, "Broken list of hits !");

    param.start_time    = this->_t0_;
    param.dtc           = this->_calibration_;
    param.last          = it_hit->is_last();
    param.first         = it_hit->is_first();
    param.xi            = it_hit->get_x();
    param.yi            = it_hit->get_y();
    param.zi            = it_hit->get_z();
    param.szi           = it_hit->get_sigma_z();
    param.ti            = it_hit->get_t();
    param.ri            = it_hit->get_r();
    param.dri           = it_hit->get_sigma_r();
    param.rmaxi         = it_hit->get_rmax();
    param.mode          = helix_fit_params::PARAM_INDEX_X0;
    param.residual_type = helix_fit_residual_function_param::RESIDUAL_ALPHA;
    alpha_residual_     = residual_function(param.x0, &param);
    param.residual_type = helix_fit_residual_function_param::RESIDUAL_BETA;
    beta_residual_      = residual_function(param.x0, &param);
    return;
  }

  int helix_fit_mgr::residual_f(const gsl_vector * x_,
                                void *             params_,
                                gsl_vector *       f_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");

    // initialize the helix parameters:
    helix_fit_residual_function_param param;
    param.x0    = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_X0);
    param.y0    = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_Y0);
    param.z0    = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_Z0);
    param.r     = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_R);
    param.step  = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_STEP);
    const helix_fit_data * lf_data = static_cast<const helix_fit_data *>(params_);
    param.start_time       = lf_data->start_time;
    param.dtc              = lf_data->calibration;
    param.using_first      = lf_data->using_first;
    param.using_last       = lf_data->using_last;
    param.using_drift_time = lf_data->using_drift_time;

    const gg_hits_col * hits = static_cast<const gg_hits_col *>(lf_data->hits);
    size_t i = 0;
    for (gg_hits_col::const_iterator it_hit = hits->begin(); it_hit != hits->end(); ++it_hit, ++i) {
      DT_LOG_TRACE(local_priority, "hit #" << i);
      // pick up useful values from the hit:
      param.last                   = it_hit->is_last();
      param.first                  = it_hit->is_first();
      param.xi                     = it_hit->get_x();
      param.yi                     = it_hit->get_y();
      param.zi                     = it_hit->get_z();
      param.szi                    = it_hit->get_sigma_z();
      param.ti                     = it_hit->get_t();
      param.ri                     = it_hit->get_r();
      param.dri                    = it_hit->get_sigma_r();
      param.rmaxi                  = it_hit->get_rmax();
      param.mode                   = helix_fit_params::PARAM_INDEX_X0;
      param.residual_type          = helix_fit_residual_function_param::RESIDUAL_ALPHA;
      const double residuali_alpha = residual_function(param.x0, &param);
      param.residual_type          = helix_fit_residual_function_param::RESIDUAL_BETA;
      const double residuali_beta  = residual_function(param.x0, &param);
      DT_LOG_TRACE(local_priority, "residuali_alpha =" << residuali_alpha);
      DT_LOG_TRACE(local_priority, "residuali_beta  =" << residuali_beta);
      gsl_vector_set(f_, i, residuali_alpha);
      gsl_vector_set(f_, i + hits->size(), residuali_beta);
    }
    DT_LOG_TRACE(local_priority, "Exiting.");
    return GSL_SUCCESS;
  }

  int helix_fit_mgr::residual_df(const gsl_vector * x_,
                                 void             * params_,
                                 gsl_matrix       * J_)
  {
    // initialize the helix parameters:
    helix_fit_residual_function_param param;
    param.x0    = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_X0);
    param.y0    = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_Y0);
    param.z0    = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_Z0);
    param.r     = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_R);
    param.step  = gsl_vector_get(x_, helix_fit_params::PARAM_INDEX_STEP);
    const helix_fit_data * lf_data = static_cast<const helix_fit_data *>(params_);
    param.start_time       = lf_data->start_time;
    param.dtc              = lf_data->calibration;
    param.using_first      = lf_data->using_first;
    param.using_last       = lf_data->using_last;
    param.using_drift_time = lf_data->using_drift_time;

    const gg_hits_col   * hits    = static_cast<const gg_hits_col *>(lf_data->hits);

    size_t i = 0;
    for (gg_hits_col::const_iterator it_hit = hits->begin();
         it_hit != hits->end();
         ++it_hit, ++i) {
      // pick up useful values from the hit:
      param.last  = it_hit->is_last();
      param.first = it_hit->is_first();
      param.xi    = it_hit->get_x();
      param.yi    = it_hit->get_y();
      param.zi    = it_hit->get_z();
      param.szi   = it_hit->get_sigma_z();
      param.ti    = it_hit->get_t();
      param.ri    = it_hit->get_r();
      param.dri   = it_hit->get_sigma_r();
      param.rmaxi = it_hit->get_rmax();
      param.mode  = helix_fit_params::PARAM_INDEX_X0;

      gsl_function F;
      double result, abserr;
      F.function = &residual_function;
      F.params   = &param;

      const double h_distance = 0.25 * CLHEP::mm;
      const double h_step     = 0.25 * CLHEP::mm / CLHEP::radian;

      // derivatives for (alpha) residuals:
      {
        param.residual_type = helix_fit_residual_function_param::RESIDUAL_ALPHA;

        param.mode = helix_fit_params::PARAM_INDEX_X0;
        const double hx0 = h_distance;
        gsl_deriv_central(&F, param.x0, hx0, &result, &abserr);
        const double dRi_dx0_alpha = result;

        param.mode = helix_fit_params::PARAM_INDEX_Y0;
        const double hy0 = h_distance;
        gsl_deriv_central(&F, param.y0, hy0, &result, &abserr);
        const double dRi_dy0_alpha = result;

        param.mode = helix_fit_params::PARAM_INDEX_Z0;
        const double hz0 = h_distance;
        gsl_deriv_central(&F, param.z0, hz0, &result, &abserr);
        const double dRi_dz0_alpha = result;

        param.mode = helix_fit_params::PARAM_INDEX_R;
        const double hr = h_distance;
        gsl_deriv_central(&F, param.r, hr, &result, &abserr);
        const double dRi_dr_alpha = result;

        param.mode = helix_fit_params::PARAM_INDEX_STEP;
        const double hstep = h_step;
        gsl_deriv_central(&F, param.step, hstep, &result, &abserr);
        const double dRi_dstep_alpha = result;

        gsl_matrix_set(J_, i, helix_fit_params::PARAM_INDEX_X0  , dRi_dx0_alpha);
        gsl_matrix_set(J_, i, helix_fit_params::PARAM_INDEX_Y0  , dRi_dy0_alpha);
        gsl_matrix_set(J_, i, helix_fit_params::PARAM_INDEX_Z0  , dRi_dz0_alpha);
        gsl_matrix_set(J_, i, helix_fit_params::PARAM_INDEX_R   , dRi_dr_alpha);
        gsl_matrix_set(J_, i, helix_fit_params::PARAM_INDEX_STEP, dRi_dstep_alpha);
      }

      // derivatives for (beta) residuals:
      {
        param.residual_type = helix_fit_residual_function_param::RESIDUAL_BETA;

        param.mode = helix_fit_params::PARAM_INDEX_X0;
        const double hx0 = h_distance;
        gsl_deriv_central(&F, param.x0, hx0, &result, &abserr);
        const double dRi_dx0_beta = result;

        param.mode = helix_fit_params::PARAM_INDEX_Y0;
        const double hy0 = h_distance;
        gsl_deriv_central(&F, param.y0, hy0, &result, &abserr);
        const double dRi_dy0_beta = result;

        param.mode = helix_fit_params::PARAM_INDEX_Z0;
        const double hz0 = h_distance;
        gsl_deriv_central(&F, param.z0, hz0, &result, &abserr);
        const double dRi_dz0_beta = result;

        param.mode = helix_fit_params::PARAM_INDEX_R;
        const double hr = h_distance;
        gsl_deriv_central(&F, param.r, hr, &result, &abserr);
        const double dRi_dr_beta = result;

        param.mode = helix_fit_params::PARAM_INDEX_STEP;
        const double hstep = h_step;
        gsl_deriv_central(&F, param.step, hstep, &result, &abserr);
        const double dRi_dstep_beta = result;

        gsl_matrix_set(J_, i + hits->size(), helix_fit_params::PARAM_INDEX_X0  , dRi_dx0_beta);
        gsl_matrix_set(J_, i + hits->size(), helix_fit_params::PARAM_INDEX_Y0  , dRi_dy0_beta);
        gsl_matrix_set(J_, i + hits->size(), helix_fit_params::PARAM_INDEX_Z0  , dRi_dz0_beta);
        gsl_matrix_set(J_, i + hits->size(), helix_fit_params::PARAM_INDEX_R   , dRi_dr_beta);
        gsl_matrix_set(J_, i + hits->size(), helix_fit_params::PARAM_INDEX_STEP, dRi_dstep_beta);
      }
    }
    return GSL_SUCCESS;
  }

  int helix_fit_mgr::residual_fdf(const gsl_vector * x_ ,
                                  void *             params_ ,
                                  gsl_vector *       f_ ,
                                  gsl_matrix *       J_ )
  {
    residual_f(x_, params_, f_);
    residual_df(x_, params_, J_);
    return GSL_SUCCESS;
  }

  std::string helix_fit_mgr::guess_utils::guess_mode_label(int guess_mode_)
  {
    switch(guess_mode_) {
    case GUESS_MODE_BBB: return "BBB";
    case GUESS_MODE_BBT: return "BBT";
    case GUESS_MODE_BTB: return "BTB";
    case GUESS_MODE_BTT: return "BTT";
    case GUESS_MODE_TBB: return "TBB";
    case GUESS_MODE_TBT: return "TBT";
    case GUESS_MODE_TTB: return "TTB";
    case GUESS_MODE_TTT: return "TTT";
    }
    return "";
  }

  void helix_fit_mgr::guess_utils::_set_defaults()
  {
    _logging_priority_  = datatools::logger::PRIO_FATAL;
    _use_max_radius_    = false;
    _max_radius_factor_ = fit_utils::default_guess_bt_factor();
    _use_guess_trust_   = false;
    _guess_trust_mode_  = fit_utils::GUESS_TRUST_MODE_COUNTER;
    _fit_delayed_clusters_ = false;
    return;
  }

  helix_fit_mgr::guess_utils::guess_utils()
  {
    _set_defaults();
    return;
  }

  void helix_fit_mgr::guess_utils::reset()
  {
    _set_defaults();
    return;
  }

  void helix_fit_mgr::guess_utils::initialize(const datatools::properties & config_)
  {
    datatools::logger::priority p = datatools::logger::PRIO_UNDEFINED;
    p = datatools::logger::extract_logging_configuration(config_,
                                                         datatools::logger::PRIO_FATAL,
                                                         false);
    if (p != datatools::logger::PRIO_UNDEFINED) {
      _logging_priority_ = p;
    }

    if (config_.has_flag("use_max_radius")) {
      _use_max_radius_ = true;
      if (config_.has_key("max_radius_factor")) {
        _max_radius_factor_ = config_.fetch_real("max_radius_factor");
      }
    }

    if (config_.has_flag("fit_delayed_clusters")) {
      _fit_delayed_clusters_ = true;
    }

    if (config_.has_flag("use_guess_trust")) {
      _use_guess_trust_ = true;
      if (config_.has_key("guess_trust_mode")) {
        const std::string gtm = config_.fetch_string("guess_trust_mode");
        if (gtm == "counter") {
          _guess_trust_mode_ = fit_utils::GUESS_TRUST_MODE_COUNTER;
        } else if (gtm == "barycenter") {
          _guess_trust_mode_ = fit_utils::GUESS_TRUST_MODE_BARYCENTER;
        } else {
          DT_THROW_IF (true, std::logic_error, "Invalid guess trust mode '" << gtm << "' !");
        }
      }
    }

    return;
  }


  // COMPUTE GUESS:

  /* ASCII art by frc !!!
   *
   *
   *                o    o  P3   o
   *            O  O         [O]    o
   *        o            .   /   `     O
   *      O      [*]        /      [*]   o
   *      O  .     M1      /      M2   `  O
   *   [o]................+ .............`[o]
   *  P1                  P                  P2
   *                                       O
   *                                       o
   *                                      O
   *
   */
  bool helix_fit_mgr::guess_utils::compute_guess(const gg_hits_col & hits_,
                                                 int guess_mode_,
                                                 helix_fit_params & guess_,
                                                 bool draw_)
  {
    DT_LOG_TRACE(_logging_priority_, "Entering...");

    const int guess_mode = guess_mode_;
    DT_LOG_TRACE(_logging_priority_, "guess_mode = " << guess_mode_label(guess_mode));

    const size_t minimum_number_of_hits = helix_fit_mgr::constants::min_number_of_hits();
    if (hits_.size() < minimum_number_of_hits) {
      DT_LOG_WARNING(_logging_priority_,
                     "Not enough hits(" << hits_.size() << " < min==" << minimum_number_of_hits << ")!");
      return false;
    }

    // Check if the cluster is or not delayed
    bool is_cluster_delayed = true;
    for (gg_hits_col::const_iterator i = hits_.begin();
         i != hits_.end();
         ++i) {
      const gg_hit & a_hit = *i;
      if (! a_hit.get_properties().has_flag(gg_hit::delayed_flag())) {
        is_cluster_delayed = false;
        break;
      }
    }
    if (is_cluster_delayed && !_fit_delayed_clusters_) {
      DT_LOG_WARNING(_logging_priority_, "Cluster is delayed !");
      return false;
    }

    double dist = -std::numeric_limits<double>::infinity();
    gg_hits_col::const_iterator iter_hit_1 = hits_.end();
    gg_hits_col::const_iterator iter_hit_2 = hits_.end();

    // 2012-10-08 FM :
    // GG hits map with infos :
    gg_hit_info_dict_type gg_hit_infos;
    build_hit_info_map(hits_, gg_hit_infos, 2);

    if (_logging_priority_ >= datatools::logger::PRIO_TRACE) {
      for (gg_hit_info_dict_type::const_iterator i = gg_hit_infos.begin();
           i != gg_hit_infos.end();
           i++) {
        DT_LOG_TRACE(_logging_priority_, "Hit #" << i->second.hit->get_id() << " : ");
        i->second.dump(std::clog);
      }
    }

    for (gg_hits_col::const_iterator i = hits_.begin();
         i != hits_.end();
         ++i)  {
      const gg_hit & hit_1 = *i;
      const geomtools::vector_3d pos1(hit_1.get_x(), hit_1.get_y(), hit_1.get_z());

      // Loop on the other hits :
      gg_hits_col::const_iterator j_start = i;
      j_start++;
      for (gg_hits_col::const_iterator j = j_start;
           j != hits_.end();
           ++j) {
        const gg_hit & hit_2 = *j;
        const geomtools::vector_3d pos2(hit_2.get_x(), hit_2.get_y(), hit_2.get_z());

        // Compute distance between two hits :
        const double d12 =(pos2 - pos1).mag();
        if (dist < 0. || d12 > dist) {
          dist = d12;
          iter_hit_1 = i;
          iter_hit_2 = j;
        }
      }
    }

    DT_LOG_TRACE(_logging_priority_, "Hits 1 & 2: ");
    if (_logging_priority_ >= datatools::logger::PRIO_TRACE) {
      iter_hit_1->tree_dump(std::clog);
      iter_hit_2->tree_dump(std::clog);
    }

    const gg_hit & hit_1 = *iter_hit_1;
    const gg_hit & hit_2 = *iter_hit_2;

    const geomtools::vector_3d p_hit1(hit_1.get_x(), hit_1.get_y(), hit_1.get_z());
    const geomtools::vector_3d p_hit2(hit_2.get_x(), hit_2.get_y(), hit_2.get_z());
    const geomtools::vector_3d p_med = 0.5 *(p_hit1 + p_hit2);
    const double z_ref = p_med.z();
    const geomtools::vector_3d u =(p_med - p_hit1).unit();

    geomtools::vector_3d p1(p_hit1);
    geomtools::vector_3d p2(p_hit2);
    p1.setZ(z_ref);
    p2.setZ(z_ref);

    DT_LOG_TRACE(_logging_priority_, "Hit_1.rmax = " << hit_1.get_rmax() / CLHEP::cm << " cm");
    DT_LOG_TRACE(_logging_priority_, "Hit_2.rmax = " << hit_2.get_rmax() / CLHEP::cm << " cm");

    const double extreme_hits_distance_factor  =  2.0;
    const double extreme_hits_minimum_distance =
      extreme_hits_distance_factor *(hit_1.get_rmax() + hit_2.get_rmax());

    const double d1d2 =(p2 - p1).mag();
    if (d1d2 < extreme_hits_minimum_distance) {
      DT_LOG_WARNING(_logging_priority_, "Distance between extreme hits(" << d1d2 / CLHEP::cm
                     << " cm) is not large enough(min = "
                     << extreme_hits_minimum_distance / CLHEP::cm << " cm) !");
      return false;
    }

    const double default_safe_dist_factor =  2.0;
    const double safe_dist_factor         = default_safe_dist_factor;

    size_t count_pos = 0;
    size_t count_neg = 0;
    double dist_far_pos     = std::numeric_limits<double>::infinity();
    double dist_far_neg     = std::numeric_limits<double>::infinity();
    double dist_far_aligned = std::numeric_limits<double>::infinity();
    gg_hits_col::const_iterator iter_hit_far_pos     = hits_.end();
    gg_hits_col::const_iterator iter_hit_far_neg     = hits_.end();
    gg_hits_col::const_iterator iter_hit_far_aligned = hits_.end();

    for (gg_hits_col::const_iterator i = hits_.begin();
         i != hits_.end();
         i++){
      const geomtools::vector_3d hi(i->get_x(), i->get_y(), z_ref);
      const geomtools::vector_3d p1hi = hi - p1;
      const geomtools::vector_3d p2hi = hi - p2;
      const geomtools::vector_3d cross_prod = u.cross(p1hi);
      DT_LOG_TRACE(_logging_priority_, "z_ref= " << z_ref / CLHEP::cm << " cm");
      DT_LOG_TRACE(_logging_priority_, "u= " << u);
      DT_LOG_TRACE(_logging_priority_, "cross_prod= " << cross_prod);

      if ((iter_hit_1 != i) &&(iter_hit_2 != i)) {
        const double d1 = p1hi.mag();
        const double d2 = p2hi.mag();

        bool dist_to_p1_ok = false;
        bool dist_to_p2_ok = false;
        if (d1 > safe_dist_factor * hit_1.get_rmax()) {
          dist_to_p1_ok = true;
        }
        if (d2 > safe_dist_factor * hit_2.get_rmax()) {
          dist_to_p2_ok = true;
        }

        const double dist =(hi - p_med).mag();
        // count hits from one side of the segment(labelled 'pos'):
        if (cross_prod.z() > 0.0) {
          count_pos++;
          if (dist_to_p1_ok && dist_to_p2_ok) {
            if (dist < dist_far_pos) {
              dist_far_pos = dist;
              iter_hit_far_pos = i;
            }
          }
        } else if (cross_prod.z() < 0.0) {
          // count hits from the other side of the segment(labelled 'neg'):

          count_neg++;
          if (dist_to_p1_ok && dist_to_p2_ok) {
            if (dist < dist_far_neg) {
              dist_far_neg = dist;
              iter_hit_far_neg = i;
            }
          }
        } else {
          // gg hit is aligned within the same row
          if (dist_to_p1_ok && dist_to_p2_ok) {
            if (dist < dist_far_aligned) {
              dist_far_aligned = dist;
              iter_hit_far_aligned = i;
            }
          }
        }
      }
    }

    DT_LOG_TRACE(_logging_priority_, "count_pos= " << count_pos);
    DT_LOG_TRACE(_logging_priority_, "count_neg= " << count_neg);

    gg_hits_col::const_iterator iter_hit_far = iter_hit_far_pos;
    // double dist_far = dist_far_pos;
    int sgn = +1;
    if (count_neg == 0 && count_pos == 0) {
      // if count_pos == count_neg == 0 means that all geiger cells are
      // aligned i.e. all have the same y value.
      DT_LOG_TRACE(_logging_priority_, "All geiger cells are aligned !");

      // 2012-11-04 XG: If the cluster hits is delayed and all cells
      // are aligned then r = rmax and there is no way to compute
      // the bisectrix
      if (! datatools::is_valid(iter_hit_far_aligned->get_r())) {
        DT_LOG_WARNING(_logging_priority_,
                       "Current cluster hits seems to be not calibrated and all cells are aligned ! "
                       << guess_mode_label(guess_mode) << " guess can not be computed !");
        return false;
      }
      iter_hit_far = iter_hit_far_aligned;
      // dist_far = dist_far_aligned;
      sgn = 0;
    } else if (count_neg > count_pos){
      iter_hit_far = iter_hit_far_neg;
      // dist_far = dist_far_neg;
      sgn = -1;
    }
    if (iter_hit_far == hits_.end()) {
      DT_LOG_WARNING(_logging_priority_, "Cannot find medium hit !");
      return false;
    }
    DT_LOG_TRACE(_logging_priority_, "Medium hit:");
    if (_logging_priority_ >= datatools::logger::PRIO_TRACE) {
      iter_hit_far->tree_dump(std::clog);
    }

    const gg_hit & hit_3 = *iter_hit_far;
    const geomtools::vector_3d p3(hit_3.get_x(), hit_3.get_y(), z_ref);

    geomtools::vector_3d u2(u);
    u2.setZ(0.);
    u2.unit();
    const geomtools::vector_2d uu2(u2);
    const geomtools::vector_2d vv2 = uu2.orthogonal();
    geomtools::vector_3d v2(vv2.x(), vv2.y(), 0.0);
    const geomtools::vector_3d u2_x_v2 = u2.cross(v2);
    if (sgn != 0 && sgn * u2_x_v2.z() > 0.0) {
      v2 *= -1.0;
    }
    DT_LOG_TRACE(_logging_priority_, "uu2 = " << uu2);
    DT_LOG_TRACE(_logging_priority_, "vv2 = " << vv2);
    DT_LOG_TRACE(_logging_priority_, "v2  = " << v2);
    DT_LOG_TRACE(_logging_priority_, "u2_x_v2 = " << u2_x_v2);
    DT_LOG_TRACE(_logging_priority_, "sgn = " << sgn);

    // Compute intersection 0123 of P1P3 and P2P3 medians :
    double x0_123 = 0.0, y0_123 = 0.0;
    geomtools::vector_3d P13;
    geomtools::vector_3d P23;
    geomtools::vector_3d gw1;
    geomtools::vector_3d gw2;
    if (sgn != 0) {
      P13 = 0.5 *(p1 + p3);
      P23 = 0.5 *(p2 + p3);
      const geomtools::vector_2d uu13((p3 - p1).x(),(p3 - p1).y());
      const geomtools::vector_2d uu23((p3 - p2).x(),(p3 - p2).y());
      const geomtools::vector_2d vv13 = uu13.orthogonal();
      const geomtools::vector_2d vv23 = uu23.orthogonal();
      gw1.set(vv13.x(), vv13.y(), 0.0);
      gw1 = gw1.unit();
      gw2.set(vv23.x(), vv23.y(), 0.0);
      gw2 = gw2.unit();

      // Intersection O123 of(P13,gw1) with(P23,gw2) :
      double a1, a2, b1, b2, c1, c2, det;
      a1 = gw1.y();
      b1 = -gw1.x();
      c1 = gw1.y() * P13.x() -  gw1.x() * P13.y();

      a2 = gw2.y();
      b2 = -gw2.x();
      c2 = gw2.y() * P23.x() -  gw2.x() * P23.y();
      det = a1 * b2 - a2 * b1;

      if (det == 0) {
        DT_LOG_TRACE(_logging_priority_, "Determinant is null, intersection point is at infinity ! Fix it to something small... ");
        det = 1e-5;
      }
      x0_123 =(c1 * b2 - c2 * b1) / det;
      y0_123 =(c2 * a1 - c1 * a2) / det;
    } else {
      // If geiger cells are aligned then use the median point as
      // 'pseudo-intersection' point and shift it by r=100 m. Things
      // are hardcoded here.
      const double center_shift_distance = 100 * CLHEP::m;
      if (p1.x() == p2.x()) {
        x0_123 = center_shift_distance * p_med.x();
        y0_123 = p_med.y();
      } else if (p1.y() == p1.y()) {
        x0_123 = p_med.x();
        y0_123 = center_shift_distance * p_med.y();
      }
    }
    const geomtools::vector_3d O123(x0_123, y0_123, z_ref);
    DT_LOG_TRACE(_logging_priority_, "O123 = " << O123 / CLHEP::mm << " mm");

    // Set radius shift to test guesses :
    double guess_bt_factor = fit_utils::default_guess_bt_factor();
    bool use_rmax = _use_max_radius_;
    if (use_rmax) {
      guess_bt_factor = _max_radius_factor_;
    }
    double r1 = iter_hit_1->get_r();
    double r2 = iter_hit_2->get_r();
    double r3 = iter_hit_far->get_r();
    if (! datatools::is_valid(r1) ||
        ! datatools::is_valid(r2) ||
        ! datatools::is_valid(r3)) {
      DT_LOG_WARNING(_logging_priority_,
                     "Geiger drift distance are not calibrated(delayed cells) ! "
                     "Force the use of rmax is such case !");
      use_rmax = true;
    }
    if (use_rmax) {
      r1 = guess_bt_factor * iter_hit_1->get_rmax();
      r2 = guess_bt_factor * iter_hit_2->get_rmax();
      r3 = guess_bt_factor * iter_hit_far->get_rmax();
    }

    const geomtools::vector_3d shift1 = r1 *(O123 - p1).unit();
    const geomtools::vector_3d shift2 = r2 *(O123 - p2).unit();
    const geomtools::vector_3d shift3 = r3 *(O123 - p3).unit();
    geomtools::vector_3d H1(p1);
    geomtools::vector_3d H2(p2);
    geomtools::vector_3d H3(p3);
    DT_LOG_TRACE(_logging_priority_, "H1 = " << H1 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "H3 = " << H3 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "H2 = " << H2 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "shift1 = " << shift1 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "shift3 = " << shift3 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "shift2 = " << shift2 / CLHEP::mm << " mm");

    const bool use_guess_trust = _use_guess_trust_;
    DT_LOG_TRACE(_logging_priority_, "use_guess_trust = " << use_guess_trust);

    // clarity and debugging purpose
    int computed_guess_trust[3];
    if (use_guess_trust) {
      if (_guess_trust_mode_ == fit_utils::GUESS_TRUST_MODE_COUNTER) {
        computed_guess_trust[0] = fit_utils::compute_guess_trust_counter(hits_, H1, H1 + shift1, H1 - shift1);
        computed_guess_trust[1] = fit_utils::compute_guess_trust_counter(hits_, H2, H2 + shift2, H2 - shift2);
        computed_guess_trust[2] = fit_utils::compute_guess_trust_counter(hits_, H3, H3 + shift3, H3 - shift3);
      }

      if (_guess_trust_mode_ == fit_utils::GUESS_TRUST_MODE_BARYCENTER) {
        computed_guess_trust[0] = fit_utils::compute_guess_trust_barycenter(gg_hit_infos, hit_1, H1, H1 + shift1, H1 - shift1);
        computed_guess_trust[1] = fit_utils::compute_guess_trust_barycenter(gg_hit_infos, hit_2, H2, H2 + shift2, H2 - shift2);
        computed_guess_trust[2] = fit_utils::compute_guess_trust_barycenter(gg_hit_infos, hit_3, H3, H3 + shift3, H3 - shift3);
      }

      if (_logging_priority_ >= datatools::logger::PRIO_TRACE) {
        for (size_t i = 0; i < 3; ++i) {
          std::ostringstream message;
          message << "computed_guess_trust[" << i << "]= " << computed_guess_trust[i] << " ";
          if (computed_guess_trust[i] == fit_utils::INVALID_HYPOTHESIS) message << "invalid";
          if (computed_guess_trust[i] == fit_utils::BOTTOM_HYPOTHESIS)  message << "bottom";
          if (computed_guess_trust[i] == fit_utils::TOP_HYPOTHESIS)     message << "top";
          DT_LOG_TRACE(_logging_priority_, message.str());
        }
      }
    }

    const int gm = guess_mode;
    // Debug only:
    //gm = GUESS_MODE_BTB;
    //gm = -1;
    if (gm == GUESS_MODE_BBB) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[2] == fit_utils::TOP_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "BBB hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_BBB");
      H1 += shift1;
      H3 += shift3;
      H2 += shift2;
    } else if (gm == GUESS_MODE_BBT) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[2] == fit_utils::BOTTOM_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "BBT hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_BBT");
      H1 += shift1;
      H3 += shift3;
      H2 -= shift2;
    } else if (gm == GUESS_MODE_BTT) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[2] == fit_utils::BOTTOM_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "BTT hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_BTT");
      H1 += shift1;
      H3 -= shift3;
      H2 -= shift2;
    } else if (gm == GUESS_MODE_TTT) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[2] == fit_utils::BOTTOM_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "TTT hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_TTT");
      H1 -= shift1;
      H3 -= shift3;
      H2 -= shift2;
    } else if (gm == GUESS_MODE_TTB) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[2] == fit_utils::TOP_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "TTB hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_TTB");
      H1 -= shift1;
      H3 -= shift3;
      H2 += shift2;
    } else if (gm == GUESS_MODE_TBB) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::TOP_HYPOTHESIS    ||
            computed_guess_trust[2] == fit_utils::TOP_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "TBB hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_TBB");
      H1 -= shift1;
      H3 += shift3;
      H2 += shift2;
    } else if (gm == GUESS_MODE_BTB) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::TOP_HYPOTHESIS    ||
            computed_guess_trust[1] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[2] == fit_utils::TOP_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "BTB hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_BTB");
      H1 += shift1;
      H3 -= shift3;
      H2 += shift2;
    } else if (gm == GUESS_MODE_TBT) {
      if (use_guess_trust){
        if (computed_guess_trust[0] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::TOP_HYPOTHESIS    ||
            computed_guess_trust[2] == fit_utils::BOTTOM_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "TBT hypothesis excluded !");
          return false;
        }
      }
      DT_LOG_TRACE(_logging_priority_, "MODE_TBT");
      H1 -= shift1;
      H3 += shift3;
      H2 -= shift2;
    } else {
      DT_LOG_ERROR(_logging_priority_, "Invalid guess mode !");
      return false;
    }

    const std::string mode_label = guess_mode_label(guess_mode);
    DT_LOG_TRACE(_logging_priority_, "Guess mode = '" << mode_label << "'");
    DT_LOG_TRACE(_logging_priority_, "H1[" << mode_label[0] << "] = " << H1 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "H3[" << mode_label[1] << "] = " << H3 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "H2[" << mode_label[2] << "] = " << H2 / CLHEP::mm << " mm");

    // Take care of configuration where H1, H2, H3 are aligned
    const geomtools::vector_3d h2h1 = H2 - H1;
    const geomtools::vector_3d h3h1 = H3 - H1;
    const geomtools::vector_3d cross_prod = h2h1.cross(h3h1);
    DT_LOG_TRACE(_logging_priority_, "cross_prod= " << cross_prod / CLHEP::mm << " mm");

    const double eps = 0.0001;
    if (fabs(cross_prod.z()) < eps) {
      DT_LOG_TRACE(_logging_priority_, "H1, H2, H3 are aligned(i.e. helix with infinite radius) !");
      DT_LOG_TRACE(_logging_priority_, mode_label << " hypothesis excluded !");
      return false;
    }

    const geomtools::vector_3d uuu2 =(H2 - H1).unit();
    const double a =(H2 - H3).mag();
    const double alpha = uuu2.angle(H3 - H1);
    const double radius = 0.5 * a / sin(alpha);
    DT_LOG_TRACE(_logging_priority_, "radius = " << radius / CLHEP::mm << " mm");

    const geomtools::vector_2d ww13((H3-H1).orthogonal().unit().x(),
                                    (H3-H1).orthogonal().unit().y());
    const geomtools::vector_2d ww23((H3-H2).orthogonal().unit().x(),
                                    (H3-H2).orthogonal().unit().y());
    const geomtools::vector_3d M1 = 0.5 *(H1 + H3);
    const geomtools::vector_3d w1(ww13);
    const geomtools::vector_3d M2 = 0.5 *(H2 + H3);
    const geomtools::vector_3d w2(ww23);

    double x0, y0;
    {
      double a1, a2, b1, b2, c1, c2, det;
      a1 = w1.y();
      b1 = -w1.x();
      c1 = w1.y() * M1.x() -  w1.x() * M1.y();
      a2 = w2.y();
      b2 = -w2.x();
      c2 = w2.y() * M2.x() -  w2.x() * M2.y();
      det = a1 * b2 - a2 * b1;
      x0 =(c1 * b2 - c2 * b1) / det;
      y0 =(c2 * a1 - c1 * a2) / det;
    }
    geomtools::vector_3d Oh(x0, y0, z_ref);
    DT_LOG_TRACE(_logging_priority_, "helix center O = " << Oh / CLHEP::mm << " mm");

    guess_.x0         = Oh.x();
    guess_.y0         = Oh.y();
    guess_.r          = radius;
    guess_.step       = 0.;
    guess_.start_time = 0.;

    compute_angles(hits_, guess_, true);

    Oh.setZ( guess_.z0 );
    compute_angles(hits_, guess_);

    geomtools::helix_3d h3d;
    h3d.set_center( Oh             );
    h3d.set_radius( guess_.r       );
    h3d.set_step  ( guess_.step    );
    h3d.set_angle1( guess_.angle_1 );
    h3d.set_angle2( guess_.angle_2 );

    const bool draw = draw_;
    if (draw) {
      const double point_sz = 5.0 * CLHEP::mm;

      datatools::temp_file ftmp0;
      ftmp0.set_remove_at_destroy(true);
      ftmp0.create("/tmp", "trackfit__helix_fit_mgr_guess_");

      Gnuplot g1("lines");

      std::ostringstream title_oss;
      title_oss << "helix_fit_mgr: guess";
      g1.set_title(title_oss.str());

      // DRAW HITS:
      for (TrackFit::gg_hits_col::const_iterator i = hits_.begin();
           i != hits_.end();
           i++) {
        TrackFit::gg_hit hit = *i;
        const double xi = hit.get_x();
        const double yi = hit.get_y();
        const double zi = hit.get_z();
        const double ri = hit.get_r();
        const geomtools::vector_3d pos(xi, yi, zi);
        geomtools::rotation rot;
        geomtools::gnuplot_draw::draw_circle(ftmp0.out(), pos, rot, ri);
        geomtools::rectangle rect(point_sz, point_sz);
        geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), pos, rot, rect);
        //geomtools::gnuplot_draw::draw_circle(ftmp0.out(), pos, rot, 0.1 * CLHEP::mm);
      }

      geomtools::rotation rot;
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p_med, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p_med + 10. * CLHEP::cm * u2, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p_med, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p_med + 10. * CLHEP::cm * v2, true);
      ftmp0.out() << std::endl;
      ftmp0.out() << std::endl;
      // DRAW GUESSED H1, H2, H3:
      geomtools::rectangle rect(point_sz, point_sz);
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), H1, rot, rect);
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), H2, rot, rect);
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), H3, rot, rect);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), H1, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), H3, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), H2, true);
      ftmp0.out() << std::endl;
      ftmp0.out() << std::endl;
      // DRAW GUESSED M1, M2:
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), M1, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), M1 + 10. * CLHEP::cm * w1, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), M2, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), M2 + 10. * CLHEP::cm * w2, true);
      ftmp0.out() << std::endl;
      ftmp0.out() << std::endl;
      // DRAW GUESSES RADIUS SEGMENT:
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), Oh, rot, rect);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), Oh, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), H3, true);
      ftmp0.out() << std::endl;
      /*
        geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), Oh, true);
        geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), Oh + mean_u + 5. * CLHEP::cm, true);
        ftmp0.out() << std::endl;
      */
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), Oh, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), H1, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), Oh, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), H2, true);
      ftmp0.out() << std::endl;
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), O123, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p1, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), O123, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p2, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), O123, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), p3, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), O123, rot, rect);
      geomtools::rectangle rect2(2*point_sz, 2*point_sz);
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), P13, rot, rect2);
      geomtools::gnuplot_draw::draw_rectangle(ftmp0.out(), P23, rot, rect2);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), P13, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), P13 + gw1 * 5 * CLHEP::cm, true);
      ftmp0.out() << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), P23, true);
      geomtools::gnuplot_draw::basic_draw_point(ftmp0.out(), P23 + gw2 * 5 * CLHEP::cm, true);
      ftmp0.out() << std::endl;
      ftmp0.out() << std::endl;
      // DRAW HELIX:
      geomtools::helix_3d::print_xyz(ftmp0.out(), h3d, 0.01, 0);
      ftmp0.out() << std::endl;
      ftmp0.out() << std::endl;
      // end
      ftmp0.out().close();
      usleep(200.0);

      g1.set_grid();

      const bool plot_3d = false;
      if (! plot_3d) {
        g1.cmd("set key on outside right top vertical");
        g1.cmd("set size ratio -1");
        g1.cmd("set grid");
        std::ostringstream title_oss;
        title_oss << "Guess '" << mode_label << "'(XY-view)";
        g1.set_title(title_oss.str());
        g1.set_xlabel("x(mm)").set_ylabel("y(mm)");
        std::ostringstream cmdoss;
        cmdoss << "plot ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 0 using 1:2 title \"Hits\" with lines, ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 1 using 1:2 title \"Working frame\" with lines, ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 2 using 1:2 title \"Guess points\" with lines, ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 3 using 1:2 title \"Special\" with lines, ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 4 using 1:2 title \"Radius\" with lines, ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 5 using 1:2 title \"O123/P1/P2/P3\" with lines, ";
        cmdoss << " '" << ftmp0.get_filename() << "' index 6 using 1:2 title \"Helix\" with lines ";
        g1.cmd(cmdoss.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
      } else {
        std::ostringstream title_oss;
        title_oss << "Guess '" << mode_label << "'(XYZ-view)";
        g1.set_title(title_oss.str());
        g1.set_xlabel("x(mm)").set_ylabel("y(mm)").set_zlabel("z(mm)");
        g1.cmd("set key on outside right top vertical");
        g1.cmd("set view equal xyz");
        g1.cmd("set xyplane at -200");
        std::ostringstream cmdoss3d;
        cmdoss3d << "splot ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 0 using 1:2:3 title \"Hits\" with lines, ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 1 using 1:2:3 title \"Working frame\" with lines, ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 2 using 1:2:3 title \"Guess points\" with lines, ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 3 using 1:2:3 title \"Special\" with lines, ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 4 using 1:2:3 title \"Radius\" with lines, ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 5 using 1:2:3 title \"O123/P1/P2/P3\" with lines, ";
        cmdoss3d << " '" << ftmp0.get_filename() << "' index 6 using 1:2:3 title \"Helix\" with lines ";
        g1.cmd(cmdoss3d.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
      }

      // remove tmp file:
      // unlink(tmp_filename.c_str());
    } // if (draw)

    DT_LOG_TRACE(_logging_priority_, "Guess.x0:  " << guess_.x0 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "Guess.y0:  " << guess_.y0 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "Guess.z0:  " << guess_.z0 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "Guess:");
    if (_logging_priority_ >= datatools::logger::PRIO_TRACE){
      h3d.tree_dump(std::clog);
    }

    return true;
  }

} // end of namespace TrackFit
