#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_fit.h>

struct data {
  size_t n;
  double * e;
  double * q;
};

int expb_f(const gsl_vector * params_, void * data_, gsl_vector * f_)
{
  size_t n = ((struct data *)data_)->n;
  double * e = ((struct data *)data_)->e;
  double * q = ((struct data *)data_)->q;
  double a = gsl_vector_get(params_, 0);
  double b = gsl_vector_get(params_, 1);
  for (auto i = 0u; i < n; i++) {
    // Model Qi = (Ei - b) / a
    double qi = (e[i] - b) / a;
    gsl_vector_set(f_, i, qi - q[i]);
  }
  return GSL_SUCCESS;
}

int expb_df (const gsl_vector * params_, void * data_, gsl_matrix * J_)
{
  size_t n = ((struct data *)data_)->n;  
  double * e = ((struct data *)data_)->e;
  double a = gsl_vector_get(params_, 0);
  for (auto i = 0u; i < n; i++) {
   // Jacobian matrix J(i,j) = dfi / dxj
   // where fi = (Qi - yi)/sigma[i],
   // Yi = A * exp(-lambda * t_i) + b 
   // and the xj are the parameters (A,lambda,b)
   gsl_matrix_set(J_, i, 0, -e[i] / a / a);
   gsl_matrix_set(J_, i, 1, -1.0 / a);
  }
  return GSL_SUCCESS;
}

void callback(const size_t iter_, void * params_,
	      const gsl_multifit_nlinear_workspace * ws_)
{
  gsl_vector * f = gsl_multifit_nlinear_residual(ws_);
  gsl_vector * x = gsl_multifit_nlinear_position(ws_);
  double rcond;
  /* compute reciprocal condition number of J(x) */
  gsl_multifit_nlinear_rcond(&rcond, ws_);
  fprintf(stderr,
	  "iter %2zu: a = %.4f, b = %.4f, cond(J)= %8.4f, |f(x)|=%.4f\n",
	  iter_,
	  gsl_vector_get(x, 0),
	  gsl_vector_get(x, 1),
	  1.0 / rcond,
	  gsl_blas_dnrm2(f));
  return;
}

int main(void)
{
  static const size_t NDATA = 2u;
  // static const size_t NDATA = 3u;
  static const size_t NPARAMS = 2u;

  // Input data to be fitted:
  std::vector<double> Es{ 0.5 , 1.0 };
  std::vector<double> Qs{ 2.6 , 5.1 };
  // std::vector<double> errQs{ 0.1 , 0.15 };
  std::vector<double> errQs{ 0.05 , 0.07 };
  // std::vector<double> Es{ 0.5 , 0.6, 1.0 };
  // std::vector<double> Qs{ 2.6 , 3.2, 5.1 };
  // std::vector<double> errQs{ 0.1 , 0.13, 0.15 };

  const gsl_multifit_nlinear_type * T = gsl_multifit_nlinear_trust;
  gsl_multifit_nlinear_parameters fdf_params =
    gsl_multifit_nlinear_default_parameters();
  gsl_matrix *covar = gsl_matrix_alloc(NPARAMS, NPARAMS);
  double x[NDATA], y[NDATA], weights[NDATA];
  struct data d = { NDATA, x, y };
  double params_init[NPARAMS] = { 0.3, 0.005 }; /* starting values */
  gsl_vector_view params = gsl_vector_view_array(params_init, NPARAMS);
  gsl_vector_view wts = gsl_vector_view_array(weights, NDATA);

  // Define the function to be minimized
  gsl_multifit_nlinear_fdf fdf;
  fdf.f = expb_f;
  fdf.df = expb_df;
  /* set to NULL for finite-difference Jacobian */
  fdf.fvv = nullptr;
  // not using geodesic acceleration 
  fdf.n = NDATA;
  fdf.p = NPARAMS;
  fdf.params = &d;

  // This is the data to be fitted: 
  for (auto i = 0u; i < NDATA; i++) {
    double xi = Es[i];
    double yi = Qs[i];
    double si = errQs[i];
    x[i] = xi;
    y[i] = yi;
    weights[i] = 1.0 / (si * si);
    fprintf(stderr, "data: %g %g %g %g\n", x[i], y[i], si, weights[i]);
  };

  /* allocate workspace with default parameters */
  gsl_multifit_nlinear_workspace * ws
    = gsl_multifit_nlinear_alloc (T, &fdf_params, NDATA, NPARAMS);
  /* initialize solver with starting point and weights */
  gsl_multifit_nlinear_winit(&params.vector, &wts.vector, &fdf, ws);
  /* compute initial cost function */
  gsl_vector * f = gsl_multifit_nlinear_residual(ws);
  double chisq0;
  gsl_blas_ddot(f, f, &chisq0);

  /* solve the system with a maximum of 100 iterations */
  int status, info;
  const double xtol = 1e-8;
  const double gtol = 1e-8;
  const double ftol = 0.0;
  status = gsl_multifit_nlinear_driver(100,
				       xtol,
				       gtol,
				       ftol,
				       callback, nullptr,
				       &info, ws);
  // compute covariance of best fit parameters 
  gsl_matrix * J = gsl_multifit_nlinear_jac(ws);
  gsl_multifit_nlinear_covar(J, 0.0, covar);
  // compute final cost 
  double chisq;
  gsl_blas_ddot(f, f, &chisq);
  
#define FIT(i) gsl_vector_get(ws->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))
  
  fprintf(stderr, "summary from method '%s/%s'\n",
	  gsl_multifit_nlinear_name(ws),
	  gsl_multifit_nlinear_trs_name(ws));
  fprintf(stderr, "number of iterations: %zu\n",
	  gsl_multifit_nlinear_niter(ws));
  fprintf(stderr, "function evaluations: %zu\n", fdf.nevalf);
  fprintf(stderr, "Jacobian evaluations: %zu\n", fdf.nevaldf);
  fprintf(stderr, "reason for stopping: %s\n",
	  (info == 1) ? "small step size" : "small gradient");
  fprintf(stderr, "initial |f(x)| = %f\n", sqrt(chisq0));
  fprintf(stderr, "final   |f(x)| = %f\n", sqrt(chisq));
  {
    auto dof = NDATA - NPARAMS;
    fprintf(stderr, "dof = %lu\n", dof);
    if (dof > 0) {
      double c = GSL_MAX_DBL(1, sqrt(chisq / dof));
      fprintf(stderr, "chisq/dof = %g\n", chisq / dof);
      fprintf (stderr, "a = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
      fprintf (stderr, "b = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
      fprintf (stderr, "Var_a = %.5f\n", gsl_matrix_get(covar, 0, 0));
      fprintf (stderr, "Var_b = %.5f\n", gsl_matrix_get(covar, 1, 1));
      fprintf (stderr, "Cov_ab = %.5f\n", gsl_matrix_get(covar, 0, 1));
      fprintf (stderr, "Cov_ba = %.5f\n", gsl_matrix_get(covar, 1, 0));
    } else {
      fprintf (stderr, "a = %.5f\n", FIT(0));
      fprintf (stderr, "b = %.5f\n", FIT(1));
      fprintf (stderr, "Var_a = %.5f\n", gsl_matrix_get(covar, 0, 0));
      fprintf (stderr, "Var_b = %.5f\n", gsl_matrix_get(covar, 1, 1));
      fprintf (stderr, "Cov_ab = %.5f\n", gsl_matrix_get(covar, 0, 1));
      fprintf (stderr, "Cov_ba = %.5f\n", gsl_matrix_get(covar, 1, 0));
    }
  }
  fprintf (stderr, "status = %s\n", gsl_strerror(status));

 
  double c0 = gsl_vector_get(ws->x, 1);
  double c1 = gsl_vector_get(ws->x, 0);
  double cov00 = gsl_matrix_get(covar, 1, 1);
  double cov01 = gsl_matrix_get(covar, 0, 1);
  double cov11 = gsl_matrix_get(covar, 0, 0);
 
  gsl_multifit_nlinear_free(ws);
  gsl_matrix_free(covar);

  double Qstep = 0.25;
  for (double Q = 0.05; Q < 6.0 + 1e-3 * Qstep; Q += Qstep) {
    double E, Eerr;
    gsl_fit_linear_est(Q, c0, c1, cov00, cov01, cov11, &E, &Eerr);
    std::cout << Q << ' ' << E << ' ' << Eerr << '\n';
  }
  std::cout << "\n\n";
  
  return 0;
}
