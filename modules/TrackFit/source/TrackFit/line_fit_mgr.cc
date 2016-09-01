/// \file falaise/TrackFit/line_fit_mgr.cc

// Ourselves:
#include <TrackFit/line_fit_mgr.h>

// Standard library:
#include <limits>

// Third party:
// - GSL:
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_version.h>
// - Bayeux/datatools:
#include <bayeux/datatools/ioutils.h>
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/temporary_files.h>
#include <bayeux/datatools/logger.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/clhep.h>
#include <bayeux/geomtools/placement.h>
#include <bayeux/geomtools/line_3d.h>
#include <bayeux/geomtools/rectangle.h>
#include <bayeux/geomtools/gnuplot_draw.h>
#include <bayeux/geomtools/gnuplot_i.h>
#include <bayeux/geomtools/gnuplot_drawer.h>

// This project:
#include <TrackFit/fit_utils.h>
#include <TrackFit/i_drift_time_calibration.h>

namespace TrackFit {

  bool line_fit_params::is_valid() const
  {
    return(z0 == z0);
  }

  line_fit_params::line_fit_params()
  {
    reset();
    return;
  }

  void line_fit_params::reset()
  {
    y0    = datatools::invalid_real();
    z0    = datatools::invalid_real();
    phi   = datatools::invalid_real();
    theta = datatools::invalid_real();
    t0    = datatools::invalid_real();
    return;
  }

  void line_fit_params::draw(std::ostream & out_, double length_) const
  {
    geomtools::vector_3d dir(1.0, 0.0, 0.0);
    dir.setPhi(phi);
    dir.setTheta(theta);
    geomtools::vector_3d A(0.0, y0, z0);
    A += 0.5 * length_ * dir;
    geomtools::vector_3d B(0.0, y0, z0);
    B -= 0.5 * length_ * dir;
    geomtools::gnuplot_draw::basic_draw_point(out_, A);
    geomtools::gnuplot_draw::basic_draw_point(out_, B);
    out_ << std::endl;
    return;
  }

  bool line_fit_data::is_valid() const
  {
    if (hits == 0) return false;
    if (calibration == 0) return false;
    return true;
  }

  void line_fit_data::reset()
  {
    hits             = 0;
    calibration      = 0;
    using_first      = false;
    using_last       = false;
    using_drift_time = false;
    fit_start_time   = false;
    return;
  }

  line_fit_data::line_fit_data()
  {
    reset();
    return;
  }

  void line_fit_solution::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool /*inherit_*/) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << "Solution found:" << std::endl
         << indent << "|-- ok      = " << ok << std::endl
         << indent << "|-- y0      = " << y0 / CLHEP::mm << " +/- " << err_y0 / CLHEP::mm << " mm" << std::endl
         << indent << "|-- z0      = " << z0 / CLHEP::mm << " +/- " << err_z0 / CLHEP::mm << " mm" << std::endl
         << indent << "|-- phi     = " << phi / CLHEP::degree << " +/- " << err_phi / CLHEP::degree << " deg" << std::endl
         << indent << "|-- theta   = " << theta / CLHEP::degree << " +/- " << err_theta / CLHEP::degree << " deg" << std::endl
         << indent << "|-- t0      = " << t0 / CLHEP::ns << " +/- " << err_t0 / CLHEP::ns << " ns" << std::endl
         << indent << "|-- chi     = " << chi << std::endl
         << indent << "|-- ndof    = " << ndof << std::endl
         << indent << "|-- niter   = " << niter << std::endl
         << indent << "|-- prob(P) = " << probability_p() << std::endl
         << indent << "`-- prob(Q) = " << probability_q() << std::endl
         << std::endl;
    return;
  }

  line_fit_solution::line_fit_solution()
  {
    reset();
    return;
  }

  line_fit_solution::~line_fit_solution()
  {
    reset();
    return;
  }

  void line_fit_solution::reset()
  {
    line_fit_params::reset();
    ok        = false;
    err_t0    = datatools::invalid_real();
    err_y0    = datatools::invalid_real();
    err_z0    = datatools::invalid_real();
    err_phi   = datatools::invalid_real();
    err_theta = datatools::invalid_real();
    chi       = std::numeric_limits<double>::infinity();
    ndof      = 0;
    niter     = 0;
    return;
  }

  double line_fit_solution::probability_p() const
  {
    return gsl_cdf_chisq_P(chi * chi, ndof);
  }

  double line_fit_solution::probability_q() const
  {
    return gsl_cdf_chisq_Q(chi * chi, ndof);
  }

  void line_fit_mgr::draw_solution(std::ostream & out_) const
  {
    draw_solution(out_, _solution_);
    return;
  }

  void line_fit_mgr::draw_solution(std::ostream & out_,
                                   const line_fit_solution & sol_) const
  {
    static size_t g_count = 0; // gnuplot trick
    const geomtools::vector_3d ref(0.0, sol_.y0, sol_.z0);
    geomtools::vector_3d dir(1.0, 0.0, 0.0);
    dir.setPhi(sol_.phi);
    dir.setTheta(sol_.theta);
    double xmin = +1.0;
    double xmax = -1.0;

    // compute direction of the line:
    double ex = std::sin(sol_.theta) * std::cos(sol_.phi);
    double ey = std::sin(sol_.theta) * std::sin(sol_.phi);
    double ez = std::cos(sol_.theta);
    double y0 = sol_.y0;
    double z0 = sol_.z0;

    for (gg_hits_col::const_iterator i = _hits_->begin();
         i != _hits_->end();
         i++) {
      geomtools::vector_3d pos(i->get_x(), i->get_y(), i->get_z());
      double x = i->get_x();
      if (xmin > xmax) {
        xmin = x;
        xmax = x;
      } else {
        if (x < xmin) {
          xmin = x;
        }
        if (x > xmax) {
          xmax = x;
        }
      }
    }
    double lambda1 = xmax / dir.x();
    double lambda2 = xmin / dir.x();
    geomtools::vector_3d p1 = ref + lambda1 * dir;
    geomtools::vector_3d p2 = ref + lambda2 * dir;
    geomtools::gnuplot_draw::basic_draw_point(out_, p1);
    if (g_count % 2) {
      geomtools::vector_3d pmid = 0.5 *(p1 + p2);
      geomtools::gnuplot_draw::basic_draw_point(out_, pmid);
    }
    geomtools::gnuplot_draw::basic_draw_point(out_, p2);
    out_ << std::endl;
    g_count++;

    for (gg_hits_col::const_iterator i = _hits_->begin();
         i != _hits_->end();
         i++) {
      bool   last    = i->is_last();
      bool   first   = i->is_first();
      double xi      = i->get_x();
      double yi      = i->get_y();
      double zi      = i->get_z();
      double ti      = i->get_t();
      double rmaxi   = i->get_rmax();
      double Uix     = xi;
      double Uiy     = yi - y0;
      double lambda  =(Uix * ex + Uiy * ey) /(ex * ex + ey * ey);
      geomtools::vector_2d OOd(0.0, y0);
      geomtools::vector_2d OdPi(lambda * ex, lambda * ey);
      geomtools::vector_2d OdOi(Uix, Uiy);
      geomtools::vector_2d OiPi = OdPi - OdOi;
      geomtools::vector_2d udi  = OiPi.unit();

      DT_LOG_TRACE(get_logging_priority(), "udi = " << udi);

      double drift_distance = 1.5 * CLHEP::cm;
      double sigma_drift_distance = 1.0 * CLHEP::mm;
      double drift_time = ti - sol_.t0;
      _calibration_->drift_time_to_radius(drift_time,
                                          drift_distance,
                                          sigma_drift_distance);

      if (! is_using_drift_time()) {
        drift_distance = rmaxi;
        drift_time     = 0.;
      }

      DT_LOG_TRACE(get_logging_priority(), "drift_time = " << drift_time);
      DT_LOG_TRACE(get_logging_priority(), "drift_distance = " << drift_distance);

      geomtools::vector_2d OiCi = rmaxi * udi;
      geomtools::vector_2d CiPi = OiPi - OiCi;
      geomtools::vector_2d OiTi = drift_distance * udi;
      geomtools::vector_2d TiPi = OiPi - OiTi;
      double alpha_i = TiPi.mag();

      if (! is_using_drift_time()) {
        if (OiPi < rmaxi)
          alpha_i = 0.;
        else
          alpha_i = CiPi.mag();
      } else {
        if (is_using_last()) {
          if (last) {
            if (OiPi.mag() <= OiTi.mag()) {
              alpha_i = 0.0;
            }
          }
        }
        if (is_using_first()) {
          if (first) {
            if (OiPi.mag() <= OiTi.mag()) {
              alpha_i = 0.0;
            }
          }
        }
      }
      DT_LOG_TRACE(get_logging_priority(), "alpha_i = " << alpha_i);

      double zPi    = z0 + lambda * ez;
      double beta_i = zPi- zi;
      geomtools::vector_3d OOd_3d(0.0, y0, z0);
      geomtools::vector_3d OOi_3d(xi, yi, zi);
      geomtools::vector_3d OiTi_3d(drift_distance * udi.x(), drift_distance * udi.y(), 0.0);
      DT_LOG_TRACE(get_logging_priority(), "OiTi_3d = " << OiTi_3d);
      geomtools::vector_3d OiPi_3d(OiPi.x(), OiPi.y(), 0.0);
      DT_LOG_TRACE(get_logging_priority(), "OiPi_3d = " << OiPi_3d);
      geomtools::vector_3d PiL_3d(0, 0, beta_i);
      // L is the 3D-position of the hit vertical to Pi(Pi is in the cell x-y plane)
      DT_LOG_TRACE(get_logging_priority(), "PiL_3d = " << PiL_3d);
      geomtools::vector_3d OTi_3d = OOi_3d + OiTi_3d;
      geomtools::vector_3d OPi_3d = OOi_3d + OiPi_3d;
      geomtools::vector_3d OL_3d  = OPi_3d + PiL_3d;
      geomtools::gnuplot_draw::basic_draw_point(out_, OTi_3d);
      if (g_count % 2) {
        geomtools::vector_3d pmid = 0.5 *(OTi_3d + OPi_3d);
        geomtools::gnuplot_draw::basic_draw_point(out_, pmid);
      }
      g_count++;
      geomtools::gnuplot_draw::basic_draw_point(out_, OPi_3d);
      geomtools::gnuplot_draw::basic_draw_point(out_, OL_3d);
      out_ << std::endl;
    }
    return;
  }

  void line_fit_mgr::draw_temporary_solution(std::ostream & out_) const
  {
    line_fit_solution sol;

    if (is_fitting_start_time()) {
      sol.t0 = gsl_vector_get(_fit_mf_fdf_solver_->x,
                              line_fit_params::PARAM_INDEX_T0);
    } else {
      sol.t0 = _t0_;
    }

    sol.z0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               line_fit_params::PARAM_INDEX_Z0);
    sol.y0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               line_fit_params::PARAM_INDEX_Y0);
    sol.phi   = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               line_fit_params::PARAM_INDEX_PHI);
    sol.theta = gsl_vector_get(_fit_mf_fdf_solver_->x,
                               line_fit_params::PARAM_INDEX_THETA);
    sol.chi   = gsl_blas_dnrm2(_fit_mf_fdf_solver_->f);
    sol.ndof  = _fit_npoints_ - _fit_npars_;
    sol.niter = _fit_iter_;

    DT_LOG_TRACE(get_logging_priority(), "Temporary solution: ");
    DT_LOG_TRACE(get_logging_priority(), "   solution.y0    = " << sol.y0);
    DT_LOG_TRACE(get_logging_priority(), "   solution.z0    = " << sol.z0);
    DT_LOG_TRACE(get_logging_priority(), "   solution.phi   = " << sol.phi);
    DT_LOG_TRACE(get_logging_priority(), "   solution.theta = " << sol.theta);
    DT_LOG_TRACE(get_logging_priority(), "   solution.t0    = " << sol.t0);
    DT_LOG_TRACE(get_logging_priority(), "   solution.chi   = " << sol.chi);
    DT_LOG_TRACE(get_logging_priority(), "   solution.ndof  = " << sol.ndof);
    DT_LOG_TRACE(get_logging_priority(), "   solution.niter = " << sol.niter);

    draw_solution(out_, sol);
    return;
  }

  line_fit_residual_function_param::line_fit_residual_function_param()
  {
    using_first      = false;
    using_last       = false;
    using_drift_time = false;
    mode             = line_fit_params::PARAM_INDEX_INVALID;
    residual_type    = RESIDUAL_INVALID;
    first            = false;
    last             = false;
    dtc              = 0;
    fit_start_time   = false;
    t0               = 0.0 * CLHEP::ns;
    return;
  }

  unsigned int line_fit_mgr::constants::default_fit_max_iter()
  {
    static unsigned int max_iteration = 100;
    return max_iteration;
  }

  double line_fit_mgr::constants::default_fit_eps()
  {
    static double epsilon = 1.e-3;
    return epsilon;
  }

  unsigned int line_fit_mgr::constants::min_number_of_hits()
  {
    static unsigned int minimal_nbr_hits = 3;
    return minimal_nbr_hits;
  }

  void line_fit_mgr::set_logging_priority(datatools::logger::priority priority_)
  {
    _logging_priority_ = priority_;
    return;
  }

  datatools::logger::priority line_fit_mgr::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool line_fit_mgr::is_initialized() const
  {
    return _initialized_;
  }

  void line_fit_mgr::_set_initialized(bool initialized_)
  {
    _initialized_ = initialized_;
    return;
  }

  bool line_fit_mgr::is_debug() const
  {
    return get_logging_priority() >= datatools::logger::PRIO_DEBUG;
  }

  void line_fit_mgr::set_debug(bool debug_)
  {
    if (debug_) set_logging_priority(datatools::logger::PRIO_DEBUG);
    else        set_logging_priority(datatools::logger::PRIO_WARNING);
    return;
  }

  const line_fit_solution & line_fit_mgr::get_solution()
  {
    return _solution_;
  }

  line_fit_solution & line_fit_mgr::grab_solution()
  {
    return _solution_;
  }

  void line_fit_mgr::set_t0(double t0_)
  {
    _t0_ = t0_;
    return;
  }

  void line_fit_mgr::set_fit_eps(double eps_)
  {
    _fit_eps_ = eps_;
    return;
  }

  void line_fit_mgr::set_guess(const line_fit_params & guess_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");
    // 2012-11-02 XG: Initialize start time even if it will not be used later
    _fit_x_init_[line_fit_params::PARAM_INDEX_T0]    = guess_.t0;
    _fit_x_init_[line_fit_params::PARAM_INDEX_Y0]    = guess_.y0;
    _fit_x_init_[line_fit_params::PARAM_INDEX_Z0]    = guess_.z0;
    _fit_x_init_[line_fit_params::PARAM_INDEX_PHI]   = guess_.phi;
    _fit_x_init_[line_fit_params::PARAM_INDEX_THETA] = guess_.theta;
    DT_LOG_DEBUG(get_logging_priority(), "T0    = " << _fit_x_init_[line_fit_params::PARAM_INDEX_T0] / CLHEP::ns << " ns");
    DT_LOG_DEBUG(get_logging_priority(), "Y0    = " << _fit_x_init_[line_fit_params::PARAM_INDEX_Y0] / CLHEP::mm << " mm");
    DT_LOG_DEBUG(get_logging_priority(), "Z0    = " << _fit_x_init_[line_fit_params::PARAM_INDEX_Z0] / CLHEP::mm << " mm");
    DT_LOG_DEBUG(get_logging_priority(), "PHI   = " << _fit_x_init_[line_fit_params::PARAM_INDEX_PHI] / CLHEP::degree << " degree");
    DT_LOG_DEBUG(get_logging_priority(), "THETA = " << _fit_x_init_[line_fit_params::PARAM_INDEX_THETA] / CLHEP::degree << " degree");
    DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    return;
  }
  void line_fit_mgr::_set_defaults_()
  {
    _logging_priority_  = datatools::logger::PRIO_FATAL;

    _fit_npars_         = line_fit_params::LINE_FIT_NOPARS;
    _fit_npoints_       = 0;
    _fit_mf_fdf_solver_ = 0;
    _fit_covar_         = 0;
    _fit_iter_          = 0;
    _fit_max_iter_      = line_fit_mgr::constants::default_fit_max_iter();
    _fit_eps_           = line_fit_mgr::constants::default_fit_eps();
    _fit_status_        = GSL_CONTINUE;

    _hits_              = 0;
    _calibration_       = 0;
    _t0_                = 0.0 * CLHEP::ns;

    _solution_.ok       = false;
    _using_first_       = false;
    _using_last_        = false;
    _using_drift_time_  = false;
    _fit_start_time_    = false;

    _step_print_status_ = false;
    _step_draw_         = false;

    return;
  }

  void line_fit_mgr::set_hits(const gg_hits_col & hits_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Object is now locked ! Operation is not allowed !");
    _hits_ = &hits_;
    return;
  }

  bool line_fit_mgr::has_calibration() const
  {
    return _calibration_ != 0;
  }

  void line_fit_mgr::set_calibration(const i_drift_time_calibration & calibration_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Object is now locked ! Operation is not allowed !");
    _calibration_ = &calibration_;
    return;
  }

  // ctor:
  line_fit_mgr::line_fit_mgr(bool /* debug_ */)
  {
    _set_defaults_();
    _set_initialized(false);
    return;
  }

  // dtor:
  line_fit_mgr::~line_fit_mgr()
  {
    if (is_initialized()) reset();
    return;
  }

  void line_fit_mgr::initialize(const datatools::properties & config_)
  {
    init(config_);
    return;
  }

  void line_fit_mgr::init(const datatools::properties & config_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

    DT_THROW_IF(_hits_ == 0, std::logic_error, "No hits !");

    const size_t nhits = _hits_->size();
    DT_THROW_IF(nhits < line_fit_mgr::constants::min_number_of_hits(),
                std::logic_error, "Not enough hits !");
    DT_LOG_DEBUG(get_logging_priority(), "nhits=" << nhits);

    // parse config options:
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

    if (config_.has_flag("fit_start_time")) {
      _fit_start_time_ = true;
    }

    if (config_.has_flag("using_drift_time")) {
      _using_drift_time_ = true;
    }

    DT_THROW_IF(_using_drift_time_ && ! has_calibration(), std::logic_error, "Missing drift time calibration !");

    // Loop over gg hits to find if the cluster is delayed or not : if
    // yes then the '_fit_start_time_' & '_using_drift_time' are
    // enabled by default
    bool is_cluster_delayed = true;
    for (gg_hits_col::const_iterator i = _hits_->begin(); i != _hits_->end(); ++i) {
      const gg_hit & a_hit = *i;
      if (! a_hit.get_properties().has_flag(gg_hit::delayed_flag())) {
        is_cluster_delayed = false;
        break;
      }
    }
    if (is_cluster_delayed) {
      _fit_start_time_   = true;
      _using_drift_time_ = true;
    }

    // Sanity checks
    if (is_fitting_start_time() && ! is_using_drift_time()) {
      DT_LOG_WARNING(get_logging_priority(), "Cluster start time will be fit but the using drift time flag is disable !");
    }

    // Init GSL settings and allocation
    _fit_npoints_ = 2 * nhits;
    _fit_npars_ = line_fit_params::LINE_FIT_NOPARS;
    if (! is_fitting_start_time()) {
      // Only use 4 parameters
      _fit_npars_--;
    }
    DT_LOG_DEBUG(get_logging_priority(), "Number of free parameters: " << _fit_npars_);
    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'fit data'...");
    _fit_covar_ = gsl_matrix_alloc(_fit_npars_, _fit_npars_);

    // init fit params
    _fit_data_.using_first      = _using_first_;
    _fit_data_.using_last       = _using_last_;
    _fit_data_.using_drift_time = _using_drift_time_;
    _fit_data_.fit_start_time   = _fit_start_time_;
    _fit_data_.hits             = _hits_;
    _fit_data_.calibration      = _calibration_;

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
    DT_THROW_IF(_fit_mf_fdf_solver_ == 0, std::logic_error, "Cannot create solver !");
    const std::string fdsolver_name = gsl_multifit_fdfsolver_name(_fit_mf_fdf_solver_);
    DT_LOG_DEBUG(get_logging_priority(), "Solver name is '" << fdsolver_name << "'");

    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'view'...");
    _fit_vview_ = gsl_vector_view_array(_fit_x_init_, _fit_npars_);

    DT_LOG_DEBUG(get_logging_priority(), "Initializing 'solver' with 'fdf'...");
    gsl_multifit_fdfsolver_set(_fit_mf_fdf_solver_,
                               &_fit_mf_fdf_function_,
                               &_fit_vview_.vector);
    DT_LOG_DEBUG(get_logging_priority(), "'solver' is setup.");

    _set_initialized(true);
    DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    return;
  }

  void line_fit_mgr::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

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
    _set_initialized(false);
    return;
  }

  bool line_fit_mgr::is_using_first() const
  {
    return _using_first_;
  }

  bool line_fit_mgr::is_using_last() const
  {
    return _using_last_;
  }

  bool line_fit_mgr::is_using_drift_time() const
  {
    return _using_drift_time_;
  }

  bool line_fit_mgr::is_fitting_start_time() const
  {
    return _fit_start_time_;
  }

  void line_fit_mgr::print_fit_status(std::ostream & out_) const
  {
    out_ << "trackfit::line_fit_mgr::print_fit_status:" << std::endl;
    out_ << "|-- " << "Iterations: " << std::endl;
    out_ << "|   " << "|-- " << "iter:   " << _fit_iter_ << std::endl;
    out_ << "|   " << "|-- " << "status: " << gsl_strerror(_fit_status_) << std::endl;
    if (is_fitting_start_time()) {
      out_ << "|   " << "|-- " << "t0= "
           << gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_T0) << std::endl;
    }
    out_ << "|   " << "|-- " << "z0= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_Z0) << std::endl;
    out_ << "|   " << "|-- " << "y0= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_Y0) << std::endl;
    out_ << "|   " << "|-- " << "phi= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_PHI) << std::endl;
    out_ << "|   " << "|-- " << "theta= "
         << gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_THETA) << std::endl;
    out_ << "|   " << "`-- " << "f= "
         << gsl_blas_dnrm2(_fit_mf_fdf_solver_->f) << std::endl;
    out_ << "`-- " << "Solution: " << "[NOT IMPLEMENTED]" << std::endl;
    return;
  }

  void line_fit_mgr::at_fit_step_do()
  {
    if (_step_print_status_) {
      DT_LOG_DEBUG(get_logging_priority(), "status = " << gsl_strerror(_fit_status_));
      print_fit_status(std::clog);
    }


    const bool step_draw = _step_draw_;
    if (step_draw) {
      datatools::temp_file ftmp;
      ftmp.set_remove_at_destroy(true);
      ftmp.create("/tmp", "temp_trackfit_line_fit_step_");
      draw_temporary_solution(ftmp.out());
      ftmp.out() << std::endl << std::endl;
      draw_hits(ftmp.out(), *_hits_);
      ftmp.out() << std::endl << std::endl;

      usleep(200.0);
      std::ostringstream title_oss;
      title_oss << "TrackFit::line_fit_mgr::at_fit_step_do : Fit step #" << _fit_iter_ << "(XY-view)";


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

  void line_fit_mgr::fit()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Starting fit...");
    DT_THROW_IF(! is_initialized(), std::logic_error, "Fit manager is not initialized !");

    do {
      DT_LOG_DEBUG(get_logging_priority(), "Fit loop #" << _fit_iter_);
      _fit_iter_++;
      _fit_status_ = gsl_multifit_fdfsolver_iterate(_fit_mf_fdf_solver_);
      DT_LOG_DEBUG(get_logging_priority(), "Iterate status = " << gsl_strerror(_fit_status_));

      if (_fit_status_ != GSL_SUCCESS &&
          _fit_status_ != GSL_ENOPROG) break;
      _fit_status_  = gsl_multifit_test_delta(_fit_mf_fdf_solver_->dx,
                                              _fit_mf_fdf_solver_->x,
                                              _fit_eps_,
                                              _fit_eps_);
      at_fit_step_do();

      DT_LOG_DEBUG(get_logging_priority(), "Test_delta status = " << gsl_strerror(_fit_status_));
    } while ((_fit_status_ == GSL_CONTINUE) &&(_fit_iter_ < _fit_max_iter_));

    if (_fit_status_ <= GSL_SUCCESS) {
      DT_LOG_DEBUG(get_logging_priority(), "Calling gsl_multifit_covar...");
#if GSL_MAJOR_VERSION > 1
      gsl_matrix* J = gsl_matrix_alloc(_fit_npoints_, _fit_npars_);
      gsl_multifit_fdfsolver_jac(_fit_mf_fdf_solver_, J);
      gsl_multifit_covar(J, 0.0, _fit_covar_);
      gsl_matrix_free(J);
#else
      gsl_multifit_covar(_fit_mf_fdf_solver_->J, 0.0, _fit_covar_);
#endif

      _solution_.ok    = true;
      _solution_.z0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                                        line_fit_params::PARAM_INDEX_Z0);
      _solution_.y0    = gsl_vector_get(_fit_mf_fdf_solver_->x,
                                        line_fit_params::PARAM_INDEX_Y0);
      _solution_.phi   = gsl_vector_get(_fit_mf_fdf_solver_->x,
                                        line_fit_params::PARAM_INDEX_PHI);
      _solution_.theta = gsl_vector_get(_fit_mf_fdf_solver_->x,
                                        line_fit_params::PARAM_INDEX_THETA);
      if (! is_fitting_start_time()) {
        _solution_.t0     = _t0_;
        _solution_.err_t0 = 0.0 * CLHEP::ns;
      } else {
        _solution_.t0     = gsl_vector_get(_fit_mf_fdf_solver_->x,
                                           line_fit_params::PARAM_INDEX_T0);
        _solution_.err_t0 = std::sqrt(gsl_matrix_get(_fit_covar_,
                                                     line_fit_params::PARAM_INDEX_T0,
                                                     line_fit_params::PARAM_INDEX_T0));
      }

      _solution_.err_z0 =
        std::sqrt(gsl_matrix_get(_fit_covar_,
                                 line_fit_params::PARAM_INDEX_Z0,
                                 line_fit_params::PARAM_INDEX_Z0));
      _solution_.err_y0 =
        std::sqrt(gsl_matrix_get(_fit_covar_,
                                 line_fit_params::PARAM_INDEX_Y0,
                                 line_fit_params::PARAM_INDEX_Y0));
      _solution_.err_phi  =
        std::sqrt(gsl_matrix_get(_fit_covar_,
                                 line_fit_params::PARAM_INDEX_PHI,
                                 line_fit_params::PARAM_INDEX_PHI));
      _solution_.err_theta  =
        std::sqrt(gsl_matrix_get(_fit_covar_,
                                 line_fit_params::PARAM_INDEX_THETA,
                                 line_fit_params::PARAM_INDEX_THETA));
      _solution_.chi   = gsl_blas_dnrm2(_fit_mf_fdf_solver_->f);
      _solution_.ndof  = _fit_npoints_ - _fit_npars_;
      _solution_.niter = _fit_iter_;
    } else {
      _solution_.ok = false;
    }
    return;
  }


  void line_fit_mgr::compute_best_frame(const gg_hits_col & hits_,
                                        gg_hits_col & hits_ref_,
                                        geomtools::placement & pl_,
                                        const uint32_t flags_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    if (flags_ & WARNING) local_priority = datatools::logger::PRIO_WARNING;
    if (flags_ & DEVEL)   local_priority = datatools::logger::PRIO_TRACE;

    DT_LOG_TRACE(local_priority, "Entering...");

    // Clean stuff before doing anything
    hits_ref_.clear();
    hits_ref_.reserve(hits_.size());

    const size_t minimum_number_of_hits = line_fit_mgr::constants::min_number_of_hits();
    if (hits_.size() < minimum_number_of_hits) {
      DT_LOG_WARNING(local_priority,
                     "Not enough hits(" << hits_.size() << " < min==" << minimum_number_of_hits << ")!");
      return;
    }
    gg_hits_col::const_iterator iter_hit_1 = hits_.end();
    gg_hits_col::const_iterator iter_hit_2 = hits_.end();
    // // 2012-10-08 FM :
    // // GG hits map with infos :
    // 2012-11-02 XG :
    // Not used for the time being
    // gg_hit_info_dict_type gg_hit_infos;
    // build_hit_info_map(hits_, gg_hit_infos, 2);

    double dist = -1.0;
    for (gg_hits_col::const_iterator i = hits_.begin(); i != hits_.end(); ++i) {
      const gg_hit & hit_1 = *i;
      const geomtools::vector_3d pos1(hit_1.get_x(), hit_1.get_y(), hit_1.get_z());

      // Loop on the other hits :
      gg_hits_col::const_iterator j_start = i;
      j_start++;
      for (gg_hits_col::const_iterator j = j_start; j != hits_.end(); ++j) {
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

    gg_hit hit1 = *iter_hit_1;
    gg_hit hit2 = *iter_hit_2;
    if (hit1.get_x() < hit2.get_x()) {
      std::swap(hit1, hit2);
    }

    const geomtools::vector_3d phit1(hit1.get_x(), hit1.get_y(), hit1.get_z());
    const geomtools::vector_3d phit2(hit2.get_x(), hit2.get_y(), hit2.get_z());
    const double best_phi =(phit2 - phit1).getPhi();

    pl_.set_translation(0.5 *(phit1 + phit2));
    pl_.set_orientation(best_phi, 0., 0.);

    // Compute the hits' positions in the new frame :
    for (gg_hits_col::const_iterator i = hits_.begin(); i != hits_.end(); ++i) {
      {
        gg_hit dummy_hit;
        hits_ref_.push_back(dummy_hit);
      }
      gg_hit & hit = hits_ref_.back();
      // Copy the hit contents in the new transformed hit :
      hit = *i;
      geomtools::vector_3d pos(i->get_x(), i->get_y(), i->get_z());
      geomtools::vector_3d pos_ref;
      // Compute the hit position in the working reference frame:
      pl_.mother_to_child(geomtools::vector_3d(i->get_x(),
                                               i->get_y(),
                                               i->get_z()),
                          pos_ref);
      hit.set_x(pos_ref.x());
      hit.set_y(pos_ref.y());
      hit.set_z(pos_ref.z());
      hit.set_phi_ref(best_phi);
    }
    DT_LOG_TRACE(local_priority, "Exiting.");
    return;
  }

  double line_fit_mgr::residual_function(double x_, void * params_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_ERROR;
    DT_LOG_TRACE(local_priority, "Entering...");
    const line_fit_residual_function_param * param_ptr =
      static_cast<const line_fit_residual_function_param *>(params_);
    DT_THROW_IF(param_ptr == 0, std::logic_error, "Invalid cast !");

    const line_fit_residual_function_param & param = *param_ptr;

    // calibration
    DT_THROW_IF(param.using_drift_time && param.dtc == 0,
                std::logic_error,
                "Drift time should be recomputed by some drift-time calibration algo !");
    const i_drift_time_calibration * dtc = param.dtc;
    const bool using_first      = param.using_first;
    const bool using_last       = param.using_last;
    const bool using_drift_time = param.using_drift_time;
    const bool fit_start_time = param.fit_start_time;
    DT_THROW_IF(!fit_start_time && param.mode == line_fit_params::PARAM_INDEX_T0,
                std::logic_error,
                "Looking for 't0' parameter while fitting start time is disabled !");

    // parameters from the line:
    double y0    = param.y0;
    double z0    = param.z0;
    double phi   = param.phi;
    double theta = param.theta;
    double t0    = param.t0;

    // dynamic parameter:
    const int mode          = param.mode;
    const int residual_type = param.residual_type;
    if (mode == line_fit_params::PARAM_INDEX_Z0)    z0 = x_;
    if (mode == line_fit_params::PARAM_INDEX_Y0)    y0 = x_;
    if (mode == line_fit_params::PARAM_INDEX_PHI)   phi = x_;
    if (mode == line_fit_params::PARAM_INDEX_THETA) theta = x_;
    if (mode == line_fit_params::PARAM_INDEX_T0)    t0 = x_;
    DT_LOG_TRACE(local_priority, "mode  = " << mode );
    DT_LOG_TRACE(local_priority, "t0    = " << t0 / CLHEP::ns  << " ns");
    DT_LOG_TRACE(local_priority, "z0    = " << z0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "y0    = " << y0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "phi   = " << phi / CLHEP::degree  << " degree");
    DT_LOG_TRACE(local_priority, "theta = " << theta / CLHEP::degree  << " degree");

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

    // 2012-02-15 XG: maybe here we can check if the geiger hit is
    // delayed and then redo the calibration for such hit using start
    // time value
    if (using_drift_time) {
      double drift_time = ti - t0;
      DT_LOG_TRACE(local_priority, "ti         = " << ti / CLHEP::ns << " ns");
      DT_LOG_TRACE(local_priority, "t0         = " << t0 / CLHEP::ns << " ns");
      DT_LOG_TRACE(local_priority, "drift_time = " << drift_time / CLHEP::ns << " ns");
      if (! dtc->drift_time_is_valid(drift_time)) {
        DT_LOG_WARNING(local_priority, "Drift_time is out of physics range!");
        // 2012-11-02 XG: This is a bit harsh !
        drift_time = 0.0 * CLHEP::ns;
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

    // compute direction of the line:
    const double ex = std::sin(theta) * std::cos(phi);
    const double ey = std::sin(theta) * std::sin(phi);
    const double ez = std::cos(theta);

    // vector Ui:
    const double Uix = xi;
    const double Uiy = yi - y0;
    const double lambda =(Uix * ex + Uiy * ey) /(ex * ex + ey * ey);
    DT_LOG_TRACE(local_priority, "ex = " << ex  << " ");
    DT_LOG_TRACE(local_priority, "ey = " << ey  << " ");
    DT_LOG_TRACE(local_priority, "ez = " << ez  << " ");
    DT_LOG_TRACE(local_priority, "lambda = " << lambda  << " ");

    const geomtools::vector_2d OOd(0.0, y0);
    const geomtools::vector_2d OdPi(lambda * ex, lambda * ey);
    const geomtools::vector_2d OdOi(Uix, Uiy);
    const geomtools::vector_2d OiPi = OdPi - OdOi;
    const geomtools::vector_2d udi  = OiPi.unit();

    const geomtools::vector_2d OiCi = rmaxi * udi;
    const geomtools::vector_2d CiPi = OiPi - OiCi;
    const geomtools::vector_2d OiTi = drift_distance * udi;
    const geomtools::vector_2d TiPi = OiPi - OiTi;

    double alpha_i = TiPi.mag();

    // 2012/02/15 XG: comment this part since I'm not sure what it was doing
    // if (! using_drift_time)
    //   {
    //     if (OiPi < rmaxi)
    //       alpha_i = 0.;
    //     else
    //       alpha_i = CiPi.mag();
    //   }
    // else
    {
      if (using_last) {
        if (last) {
          if (OiPi.mag() <= OiTi.mag()) {
            alpha_i = 0.0;
          }
        }
      }

      if (using_first) {
        if (first) {
          if (OiPi.mag() <= OiTi.mag()) {
            alpha_i = 0.0;
          }
        }
      }
    }

    const double sigma_ri = sigma_drift_distance;
    const double zPi      = z0 + lambda * ez;
    const double beta_i   = zi - zPi;
    DT_LOG_TRACE(local_priority, "z0 = " << z0 / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "zPi = " << zPi / CLHEP::mm  << " mm");
    DT_LOG_TRACE(local_priority, "alpha_i = " << alpha_i);
    DT_LOG_TRACE(local_priority, "sigma_ri = " << sigma_ri);
    DT_LOG_TRACE(local_priority, "beta_i = " << beta_i);
    DT_LOG_TRACE(local_priority, "sigma_zi = " << sigma_zi);

    double Ri;
    if (residual_type == line_fit_residual_function_param::RESIDUAL_ALPHA) {
      Ri  = alpha_i / sigma_ri;
    } else if (residual_type == line_fit_residual_function_param::RESIDUAL_BETA) {
      Ri  = beta_i / sigma_zi;
    } else {
      DT_THROW_IF(true, std::logic_error, "Invalid residual type !");
    }
    DT_LOG_TRACE(local_priority, "Exiting.");
    return Ri;
  }

  void line_fit_mgr::get_residuals_per_hit(size_t hit_index_,
                                           double & alpha_residual_,
                                           double & beta_residual_,
                                           bool at_solution_) const
  {
    // check hit index validity:
    const gg_hits_col * hits = this->_hits_;
    DT_THROW_IF(hit_index_ >= hits->size(), std::logic_error, "Invalid hit index !");
    line_fit_residual_function_param param;
    param.using_first      = is_using_first();
    param.using_last       = is_using_last();
    param.using_drift_time = is_using_drift_time();
    param.fit_start_time   = is_fitting_start_time();

    // initialize the line parameters:
    if (! at_solution_) {
      param.z0    = gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_Z0);
      param.y0    = gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_Y0);
      param.phi   = gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_PHI);
      param.theta = gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_THETA);
      if (is_fitting_start_time()) {
        param.t0 = gsl_vector_get(_fit_mf_fdf_solver_->x, line_fit_params::PARAM_INDEX_T0);
      }
    } else {
      DT_THROW_IF(! _solution_.ok, std::logic_error, "No available solution !");
      param.z0    = _solution_.z0;
      param.y0    = _solution_.y0;
      param.phi   = _solution_.phi;
      param.theta = _solution_.theta;
      if (is_fitting_start_time()) {
        param.t0 = _solution_.t0;
      }
    }
    size_t count = 0;
    gg_hits_col::const_iterator it_hit = hits->end();
    for (it_hit = hits->begin(); it_hit != hits->end(); ++it_hit, count++) {
      if (count == hit_index_) {
        break;
      }
    }
    DT_THROW_IF(it_hit == hits->end(), std::logic_error, "Broken list of hits !");

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
    param.mode          = line_fit_params::PARAM_INDEX_Z0;
    param.residual_type = line_fit_residual_function_param::RESIDUAL_ALPHA;
    alpha_residual_     = residual_function(param.z0, &param);
    param.residual_type = line_fit_residual_function_param::RESIDUAL_BETA;
    beta_residual_      = residual_function(param.z0, &param);
    return;
  }

  int line_fit_mgr::residual_f(const gsl_vector * x_,
                               void *             params_,
                               gsl_vector *       f_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");

    // initialize the line parameters:
    line_fit_residual_function_param param;
    param.z0    = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_Z0);
    param.y0    = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_Y0);
    param.phi   = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_PHI);
    param.theta = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_THETA);

    const line_fit_data * lf_data = static_cast<const line_fit_data *>(params_);
    param.dtc              = lf_data->calibration;
    param.using_first      = lf_data->using_first;
    param.using_last       = lf_data->using_last;
    param.using_drift_time = lf_data->using_drift_time;
    param.fit_start_time   = lf_data->fit_start_time;
    if (param.fit_start_time) {
      param.t0 = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_T0);
    }

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
      param.mode                   = line_fit_params::PARAM_INDEX_Z0;
      param.residual_type          = line_fit_residual_function_param::RESIDUAL_ALPHA;
      const double residuali_alpha = residual_function(param.z0, &param);
      param.residual_type          = line_fit_residual_function_param::RESIDUAL_BETA;
      const double residuali_beta  = residual_function(param.z0, &param);
      DT_LOG_TRACE(local_priority, "residuali_alpha =" << residuali_alpha);
      DT_LOG_TRACE(local_priority, "residuali_beta  =" << residuali_beta);
      gsl_vector_set(f_, i, residuali_alpha);
      gsl_vector_set(f_, i + hits->size(), residuali_beta);
    }
    DT_LOG_TRACE(local_priority, "Exiting.");
    return GSL_SUCCESS;
  }

  int line_fit_mgr::residual_df(const gsl_vector * x_,
                                void *             params_,
                                gsl_matrix *       J_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");

    // initialize the line parameters:
    line_fit_residual_function_param param;
    param.z0    = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_Z0);
    param.y0    = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_Y0);
    param.phi   = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_PHI);
    param.theta = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_THETA);

    const line_fit_data * lf_data = static_cast<const line_fit_data *>(params_);
    param.dtc              = lf_data->calibration;
    param.using_first      = lf_data->using_first;
    param.using_last       = lf_data->using_last;
    param.using_drift_time = lf_data->using_drift_time;
    param.fit_start_time   = lf_data->fit_start_time;
    if (param.fit_start_time) {
      param.t0 = gsl_vector_get(x_, line_fit_params::PARAM_INDEX_T0);
    }

    const gg_hits_col * hits = static_cast<const gg_hits_col *>(lf_data->hits);

    size_t i = 0;
    for (gg_hits_col::const_iterator it_hit = hits->begin(); it_hit != hits->end(); ++it_hit, ++i) {
      DT_LOG_TRACE(local_priority, "hit #" << i);
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
      param.mode  = line_fit_params::PARAM_INDEX_Z0;

      gsl_function F;
      double result, abserr;
      F.function = &residual_function;
      F.params   = &param;

      const double h_distance = 0.25 * CLHEP::mm;
      const double h_angle    = M_PI/100 * CLHEP::radian;
      const double h_time     = 0.5 * CLHEP::ns;

      // derivatives for (alpha) residuals:
      {
        param.residual_type = line_fit_residual_function_param::RESIDUAL_ALPHA;

        param.mode = line_fit_params::PARAM_INDEX_PHI;
        const double hphi = h_angle;
        gsl_deriv_central(&F, param.phi, hphi, &result, &abserr);
        const double dRi_dphi_alpha = result;

        param.mode = line_fit_params::PARAM_INDEX_THETA;
        const double htheta = h_angle;
        gsl_deriv_central(&F, param.theta, htheta, &result, &abserr);
        const double dRi_dtheta_alpha = result;

        param.mode = line_fit_params::PARAM_INDEX_Z0;
        const double hz0 = h_distance;
        gsl_deriv_central(&F, param.z0, hz0, &result, &abserr);
        const double dRi_dz0_alpha = result;

        param.mode = line_fit_params::PARAM_INDEX_Y0;
        const double hy0 = h_distance;
        gsl_deriv_central(&F, param.y0, hy0, &result, &abserr);
        const double dRi_dy0_alpha = result;

        gsl_matrix_set(J_, i, line_fit_params::PARAM_INDEX_Z0   , dRi_dz0_alpha);
        gsl_matrix_set(J_, i, line_fit_params::PARAM_INDEX_Y0   , dRi_dy0_alpha);
        gsl_matrix_set(J_, i, line_fit_params::PARAM_INDEX_PHI  , dRi_dphi_alpha);
        gsl_matrix_set(J_, i, line_fit_params::PARAM_INDEX_THETA, dRi_dtheta_alpha);

        if (param.fit_start_time) {
          param.mode = line_fit_params::PARAM_INDEX_T0;
          const double ht0 = h_time;
          gsl_deriv_central(&F, param.t0, ht0, &result, &abserr);
          const double dRi_dt0_alpha = result;
          gsl_matrix_set(J_, i, line_fit_params::PARAM_INDEX_T0, dRi_dt0_alpha);
        }
      }

      // derivatives for (beta) residuals:
      {
        param.residual_type = line_fit_residual_function_param::RESIDUAL_BETA;

        param.mode = line_fit_params::PARAM_INDEX_PHI;
        const double hphi = h_angle;
        gsl_deriv_central(&F, param.phi, hphi, &result, &abserr);
        const double dRi_dphi_beta = result;

        param.mode = line_fit_params::PARAM_INDEX_THETA;
        const double htheta = h_angle;
        gsl_deriv_central(&F, param.theta, htheta, &result, &abserr);
        const double dRi_dtheta_beta = result;

        param.mode = line_fit_params::PARAM_INDEX_Z0;
        const double hz0 = h_distance;
        gsl_deriv_central(&F, param.z0, hz0, &result, &abserr);
        const double dRi_dz0_beta = result;

        param.mode = line_fit_params::PARAM_INDEX_Y0;
        const double hy0 = h_distance;
        gsl_deriv_central(&F, param.y0, hy0, &result, &abserr);
        const double dRi_dy0_beta = result;

        gsl_matrix_set(J_, i + hits->size(), line_fit_params::PARAM_INDEX_Z0   , dRi_dz0_beta);
        gsl_matrix_set(J_, i + hits->size(), line_fit_params::PARAM_INDEX_Y0   , dRi_dy0_beta);
        gsl_matrix_set(J_, i + hits->size(), line_fit_params::PARAM_INDEX_PHI  , dRi_dphi_beta);
        gsl_matrix_set(J_, i + hits->size(), line_fit_params::PARAM_INDEX_THETA, dRi_dtheta_beta);
        if (param.fit_start_time) {
          param.mode = line_fit_params::PARAM_INDEX_T0;
          const double ht0 = h_time;
          gsl_deriv_central(&F, param.t0, ht0, &result, &abserr);
          const double dRi_dt0_beta = result;
          gsl_matrix_set(J_, i + hits->size(), line_fit_params::PARAM_INDEX_T0, dRi_dt0_beta);
        }
      }
    }
    DT_LOG_TRACE(local_priority, "Exiting.");
    return GSL_SUCCESS;
  }

  int line_fit_mgr::residual_fdf(const gsl_vector * x_,
                                 void *             params_,
                                 gsl_vector *       f_,
                                 gsl_matrix *       J_)
  {
    residual_f(x_, params_, f_);
    residual_df(x_, params_, J_);
    return GSL_SUCCESS;
  }


  void line_fit_mgr::convert_solution(const gg_hits_col & hits_ref_,
                                      const line_fit_solution & sol_,
                                      const geomtools::placement & pl_,
                                      geomtools::line_3d & line_)
  {
    const bool draw = false;
    std::ofstream f1;
    if (draw) {
      f1.open("aaa.data");
    }
    //<<<
    gg_hits_col::const_iterator i_neg = hits_ref_.end();
    gg_hits_col::const_iterator i_pos = hits_ref_.end();
    geomtools::vector_3d dir(1.0, 0.0, 0.0);
    dir.setPhi(sol_.phi);
    dir.setTheta(sol_.theta);
    const geomtools::vector_3d Od(0.0, sol_.y0, sol_.z0);

    double dmax_neg = -1.0;
    double dmax_pos = -1.0;
    double dmax_n = -1.0;
    double dmax_p = -1.0;
    const double point_sz = 4.0 * CLHEP::mm;
    for (gg_hits_col::const_iterator i = hits_ref_.begin(); i != hits_ref_.end(); i++) {
      const geomtools::vector_3d Oi(i->get_x(), i->get_y(), i->get_z());
      const geomtools::vector_3d OdOi = Od - Oi;
      if (draw) {
        geomtools::rectangle rect(point_sz, point_sz);
        geomtools::rotation rot;
        geomtools::gnuplot_draw::draw_rectangle(f1, Oi, rot, rect);
      }
      const geomtools::vector_2d Oi_2d(i->get_x(), i->get_y());
      const geomtools::vector_2d Od_2d(0.0, sol_.y0);
      const geomtools::vector_2d OdOi_2d = Oi_2d - Od_2d;
      const double di = OdOi_2d.mag();
      if (OdOi.x() > 0.0) {
        if ((dmax_pos < 0.0) ||(di >= dmax_pos)) {
          dmax_pos = di;
          i_pos = i;
        }
      } else {
        if ((dmax_neg < 0.0) ||(di >= dmax_neg)) {
          dmax_neg = di;
          i_neg = i;
        }
      }
    }
    dmax_p = dmax_pos / sin(sol_.theta) / cos(sol_.phi);
    dmax_n = dmax_neg /  sin(sol_.theta) / cos(sol_.phi);
    geomtools::vector_3d first = Od - dmax_n * dir;
    geomtools::vector_3d last  = Od + dmax_p * dir;
    geomtools::vector_3d Ofirst(i_neg->get_x(), i_neg->get_y(), i_neg->get_z());
    geomtools::vector_3d Olast(i_pos->get_x(), i_pos->get_y(), i_pos->get_z());
    if (draw) {
      f1 << std::endl;
      f1 << "# dmax_neg=" << dmax_neg << std::endl;
      f1 << "# dmax_pos=" << dmax_pos << std::endl;
      geomtools::rotation rot;
      geomtools::vector_3d pos;
      geomtools::gnuplot_draw::basic_draw_segment(f1, first, last);
      f1 << std::endl;
      geomtools::rectangle rect(point_sz, point_sz);
      geomtools::gnuplot_draw::draw_rectangle(f1, first, rot, rect);
      geomtools::gnuplot_draw::draw_rectangle(f1, last, rot, rect);
      geomtools::gnuplot_draw::draw_rectangle(f1, Od, rot, rect);
      f1 << std::endl;
      geomtools::gnuplot_draw::draw_rectangle(f1, Ofirst, rot, rect);
      geomtools::gnuplot_draw::draw_rectangle(f1, Olast, rot, rect);
    }
    line_.set_first(pl_.child_to_mother(first));
    line_.set_last(pl_.child_to_mother(last));
    return;
  }


  /****************************************************************************/

  std::string line_fit_mgr::guess_utils::guess_mode_label(int guess_mode_)
  {
    switch(guess_mode_) {
    case GUESS_MODE_BB: return "BB";
    case GUESS_MODE_BT: return "BT";
    case GUESS_MODE_TB: return "TB";
    case GUESS_MODE_TT: return "TT";
    }
    return "";
  }


  void line_fit_mgr::guess_utils::_set_defaults()
  {
    _logging_priority_  = datatools::logger::PRIO_FATAL;
    _use_max_radius_    = false;
    _max_radius_factor_ = fit_utils::default_guess_bt_factor();
    _use_guess_trust_   = false;
    _guess_trust_mode_  = fit_utils::GUESS_TRUST_MODE_COUNTER;
    _fit_delayed_clusters_ = false;
    return;
  }


  line_fit_mgr::guess_utils::guess_utils()
  {
    _set_defaults();
    return;
  }

  void line_fit_mgr::guess_utils::reset()
  {
    _set_defaults();
    return;
  }

  void line_fit_mgr::guess_utils::initialize(const datatools::properties & config_)
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
        _max_radius_factor_ = config_.fetch_real("guess.max_radius_factor");
      }
    }

    if (config_.has_flag("fit_delayed_clusters")) {
      _fit_delayed_clusters_ = true;
    }

    if (config_.has_flag("use_guess_trust")) {
      _use_guess_trust_ = true;
      if (config_.has_key("guess_trust_mode")) {
        std::string gtm = config_.fetch_string("guess_trust_mode");
        if (gtm == "counter") {
          _guess_trust_mode_ = fit_utils::GUESS_TRUST_MODE_COUNTER;
        } else if (gtm == "barycenter") {
          _guess_trust_mode_ = fit_utils::GUESS_TRUST_MODE_BARYCENTER;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid guess trust mode '" << gtm << "' !");
        }
      }
    }

    return;
  }

  bool line_fit_mgr::guess_utils::compute_guess(const gg_hits_col & hits_,
                                                int guess_mode_,
                                                line_fit_params & guess_)
  {
    DT_LOG_TRACE(_logging_priority_, "Entering...");

    const int guess_mode = guess_mode_;
    DT_LOG_TRACE(_logging_priority_, "guess_mode = " << guess_mode_label(guess_mode));

    const size_t minimum_number_of_hits = line_fit_mgr::constants::min_number_of_hits();
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
    double start_time = std::numeric_limits<double>::infinity();
    gg_hits_col::const_iterator iter_hit_1 = hits_.end();
    gg_hits_col::const_iterator iter_hit_2 = hits_.end();

    for (gg_hits_col::const_iterator i = hits_.begin(); i != hits_.end(); ++i) {
      const geomtools::vector_3d pos1(i->get_x(), i->get_y(), i->get_z());
      gg_hits_col::const_iterator j_start = i;
      j_start++;
      for (gg_hits_col::const_iterator j = j_start;
           j != hits_.end();
           j++) {
        const geomtools::vector_3d pos2(j->get_x(), j->get_y(), j->get_z());
        const double d =(pos2 - pos1).mag();
        if (d > dist) {
          dist = d;
          iter_hit_1 = i;
          iter_hit_2 = j;
        }
      }
      if (start_time > i->get_t()) {
        start_time = i->get_t();
      }
    }
    gg_hit hit1 = *iter_hit_1;
    gg_hit hit2 = *iter_hit_2;
    if (hit2.get_x() < hit1.get_x()) {
      std::swap(hit1, hit2);
    }

    DT_LOG_TRACE(_logging_priority_, "Max cell radius: " << hit1.get_rmax() / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "Hits 1 & 2: ");
    if (_logging_priority_ >= datatools::logger::PRIO_TRACE) {
      hit1.tree_dump(std::clog, "Hit 1:", "[trace]: ");
      hit2.tree_dump(std::clog, "Hit 2:", "[trace]: ");
    }
    geomtools::vector_3d phit1(hit1.get_x(), hit1.get_y(), hit1.get_z());
    geomtools::vector_3d phit2(hit2.get_x(), hit2.get_y(), hit2.get_z());

    const geomtools::vector_2d dd((phit2 - phit1).x(),(phit2 - phit1).y());
    const geomtools::vector_2d dd2 = dd.unit();
    const geomtools::vector_2d shift_dir = dd2.orthogonal();
    DT_LOG_TRACE(_logging_priority_, "dd2=" << dd2);

    double guess_bt_factor = fit_utils::default_guess_bt_factor();
    const bool use_rmax = _use_max_radius_;
    if (use_rmax) {
      guess_bt_factor = _max_radius_factor_;
    }
    const double guess_shift_factor = guess_bt_factor;
    const double max_cell_radius = hit1.get_rmax();
    const double side_shift = guess_shift_factor * max_cell_radius;
    geomtools::vector_3d shift1(shift_dir.x(), shift_dir.y(), 0.0);
    shift1 *= side_shift;
    geomtools::vector_3d shift2(shift_dir.x(), shift_dir.y(), 0.0);
    shift2 *= side_shift;

    // compute guesses
    const bool use_guess_trust = _use_guess_trust_;
    DT_LOG_TRACE(_logging_priority_, "use_guess_trust = " << use_guess_trust);

    // clarity and debugging purpose
    int computed_guess_trust[2];
    if (use_guess_trust) {
      if (_guess_trust_mode_ == fit_utils::GUESS_TRUST_MODE_COUNTER) {
        computed_guess_trust[0] = fit_utils::compute_guess_trust_counter(hits_, phit1, phit1 - shift1, phit1 + shift1);
        computed_guess_trust[1] = fit_utils::compute_guess_trust_counter(hits_, phit2, phit2 - shift2, phit2 + shift2);
      }

      /*
        if (_guess_trust_mode_ == fit_utils::GUESS_TRUST_MODE_BARYCENTER)
        {
        computed_guess_trust[0] = fit_utils::compute_guess_trust_barycenter(gg_hit_infos, hit_1, H1, H1 + shift1, H1 - shift1);
        computed_guess_trust[1] = fit_utils::compute_guess_trust_barycenter(gg_hit_infos, hit_2, H2, H2 + shift2, H2 - shift2);
        }
      */

      if (_logging_priority_ >= datatools::logger::PRIO_TRACE) {
        for (size_t i = 0; i < 2; ++i) {
          std::ostringstream message;
          message << "computed_guess_trust[" << i << "]= " << computed_guess_trust[i] << " ";
          if (computed_guess_trust[i] == fit_utils::INVALID_HYPOTHESIS) message << "invalid";
          if (computed_guess_trust[i] == fit_utils::BOTTOM_HYPOTHESIS)  message << "bottom";
          if (computed_guess_trust[i] == fit_utils::TOP_HYPOTHESIS)     message << "top";
          DT_LOG_TRACE(_logging_priority_, message.str());
        }
      }
    }

    if (guess_mode == GUESS_MODE_BB) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::TOP_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "BB hypothesis excluded !");
          return false;
        }
      }
      phit1 -= shift1;
      phit2 -= shift2;
    } else if (guess_mode == GUESS_MODE_BT) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::TOP_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::BOTTOM_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "BT hypothesis excluded !");
          return false;
        }
      }
      phit1 -= shift1;
      phit2 += shift2;
    } else if (guess_mode == GUESS_MODE_TB) {
      if (use_guess_trust) {
        if (computed_guess_trust[0] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::TOP_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "TB hypothesis excluded !");
          return false;
        }
      }
      phit1 += shift1;
      phit2 -= shift2;
    } else if (guess_mode == GUESS_MODE_TT) {
      if (_use_guess_trust_) {
        if (computed_guess_trust[0] == fit_utils::BOTTOM_HYPOTHESIS ||
            computed_guess_trust[1] == fit_utils::BOTTOM_HYPOTHESIS) {
          DT_LOG_WARNING(_logging_priority_, "TT hypothesis excluded !");
          return false;
        }
      }
      phit1 += shift1;
      phit2 += shift2;
    } else {
      DT_LOG_ERROR(_logging_priority_, "Invalid guess mode !");
      return false;
    }

    const geomtools::vector_3d dir0 =(phit2 - phit1).unit();
    const geomtools::vector_3d phit0 = 0.5 *(phit1 + phit2);
    guess_.y0    = phit0.y();
    guess_.z0    = phit0.z();
    guess_.phi   = dir0.phi();
    guess_.theta = dir0.theta();
    guess_.t0    = start_time;
    DT_LOG_TRACE(_logging_priority_, "Guess.y0:    " << guess_.y0 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "Guess.z0:    " << guess_.z0 / CLHEP::mm << " mm");
    DT_LOG_TRACE(_logging_priority_, "Guess.phi:   " << guess_.phi / CLHEP::degree << " degree");
    DT_LOG_TRACE(_logging_priority_, "Guess.theta: " << guess_.theta / CLHEP::degree << " degree");
    DT_LOG_TRACE(_logging_priority_, "Guess.t0:    " << guess_.t0 / CLHEP::ns << " ns");

    return true;
  }

} // end of namespace TrackFit
