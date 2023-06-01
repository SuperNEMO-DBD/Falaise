/* -*- mode: c++ -*- */
#include <CATAlgorithm/CircleRegression.h>
#if CAT_WITH_DEVEL_ROOT == 1
#include <TROOT.h>
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#endif  // CAT_WITH_DEVEL_ROOT == 1

namespace CAT {
namespace topology {

#if CAT_WITH_DEVEL_ROOT == 1

double CircleRegression::expression_to_be_minimized(const double *par) {
  const Double_t xc = par[0];
  const Double_t yc = par[1];
  const Double_t rad = par[2];

  double expression = 0.;
  for (size_t i = 0; i < xi_.size(); i++) {
    expression += std::abs(pow(xi_[i].value() - xc, 2) + pow(yi_[i].value() - yc, 2) - pow(rad, 2));
    /*
    std::clog << " point " << i << " x " << xi_[i].value() << " par " << xc << " diff " <<
    xi_[i].value() - xc
              << " y " << yi_[i].value() << " par " << yc << " diff " << yi_[i].value() - yc
              << " r " <<  rad << " exp " << pow(xi_[i].value() - xc,2) + pow(yi_[i].value() - yc,2)
    - pow(rad,2) << " cumul " << expression << std::endl;
    */
  }

  //      std::clog << " xc " << par[0] << " yc " << par[1] << " r " << par[2] << " val " <<
  //      expression << std::endl;

  return expression;
}
#endif  // CAT_WITH_DEVEL_ROOT == 1

//! set
void CircleRegression::set(std::vector<experimental_double> &xi,
                           std::vector<experimental_double> &yi) {
  xi_ = xi;
  yi_ = yi;
}

//! set xi
void CircleRegression::set_xi(std::vector<experimental_double> &xi) { xi_ = xi; }

//! set yi
void CircleRegression::set_yi(std::vector<experimental_double> &yi) { yi_ = yi; }

//! set circle
void CircleRegression::set_circle(circle &c) { c_ = c; }

//! get xi
const std::vector<experimental_double> &CircleRegression::xi() const { return xi_; }

//! get yi
const std::vector<experimental_double> &CircleRegression::yi() const { return yi_; }

//! get circle
const circle &CircleRegression::c() const { return c_; }

bool CircleRegression::fit(void) {
  if (xi_.size() != yi_.size()) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << "CAT::CircleRegression::fit: problem: in circle regression, sizes x "
                << xi_.size() << " y " << yi_.size() << std::endl;
    }
    return false;
  }

  experimental_double xc, yc, r;

  // bool method1 = false;
  // method 1: R. Bullock, http://www.dtcenter.org/met/users/docs/write_ups/circle_fit.pdf
  // method 2: D. Umbach, K. N. Jones, http://www.cs.bsu.edu/homepages/kerryj/kjones/circles.pdf

  std::vector<double> ui;
  std::vector<double> vi;
  double Sw = 0.;
  double Swx = 0.;
  double Swy = 0.;
  double Swxx = 0.;
  double Swxy = 0.;
  double Swyy = 0.;
  double Swxxx = 0.;
  double Swxyy = 0.;
  double Swxxy = 0.;
  double Swyyy = 0.;
  // double Swuu = 0.;
  // double Swuv = 0.;
  // double Swvv = 0.;
  // double Swuuu = 0.;
  // double Swuuv = 0.;
  // double Swuvv = 0.;
  // double Swvvv = 0.;

  double delta;
  // double xave, yave;

  // if (method1) {
  //   xave = average(xi_).value();
  //   yave = average(yi_).value();
  //   std::clog << "CAT::CircleRegression::fit: xave=" << xave << " yave=" << yave << std::endl;
  // }

  // calculate parameters for circle
  for (std::vector<experimental_double>::iterator it = xi_.begin(); it != xi_.end(); ++it) {
    double y = yi_[it - xi_.begin()].value();
    double yerr = yi_[it - xi_.begin()].error();
    double w = 1. / (mybhep::square(it->error())) + 1. / (mybhep::square(yerr));
    if (std::isnan(w) || std::isinf(w)) w = 1.;
    Sw += w;

    // if (method1) {
    //   double u = it->value() - xave;
    //   double v = y - yave;
    //   Swuu += w * mybhep::square(u);
    //   Swuv += w * u * v;
    //   Swvv += w * mybhep::square(v);
    //   Swuuu += w * mybhep::cube(u);
    //   Swuuv += w * mybhep::square(u) * v;
    //   Swuvv += w * u * mybhep::square(v);
    //   Swvvv += w * mybhep::cube(v);
    // } else {
      Swx += w * it->value();
      Swy += w * y;
      Swxx += w * mybhep::square(it->value());
      Swxy += w * it->value() * y;
      Swyy += w * mybhep::square(y);
      Swxyy += w * it->value() * mybhep::square(y);
      Swxxy += w * mybhep::square(it->value()) * y;
      Swyyy += w * mybhep::cube(y);
      Swxxx += w * mybhep::cube(it->value());
    // }
  }

  // if (method1) {
  //   delta = Swuu * Swvv - mybhep::square(Swuv);

  //   if (delta == 0.) {
  //     if (print_level() >= mybhep::NORMAL) {
  //       std::clog << "CAT::CircleRegression::fit: problem: in circle regression, delta " << delta
  //                 << " Swuu=" << Swuu << " Swvv=" << Swvv << " Swuv=" << Swuv << std::endl;
  //     }
  //     return false;
  //   }

  //   double uc = (Swuuu + Swuvv) / (2. * delta);
  //   double vc = (Swuuv + Swvvv) / (2. * delta);
  //   double erruc = 0.;
  //   double errvc = 0.;
  //   double alpha = mybhep::square(uc) + mybhep::square(vc) + (Swuu + Swvv) / Sw;
  //   double erralpha = 0.;

  //   std::clog << "CAT::CircleRegression::fit: uc=" << uc << " vc=" << vc << std::endl;

  //   xc.set(uc + xave, erruc);
  //   yc.set(vc + yave, errvc);
  //   r.set(std::sqrt(alpha), erralpha / (2. * std::sqrt(alpha)));
  // } else {
    double A = Sw * Swxx - mybhep::square(Swx);
    double B = Sw * Swxy - Swx * Swy;
    double C = Sw * Swyy - mybhep::square(Swy);
    double D = (Sw * Swxyy - Swx * Swyy + Sw * Swxxx - Swx * Swxx) / 2.;
    double E = (Sw * Swxxy - Swy * Swxx + Sw * Swyyy - Swy * Swyy) / 2.;
    delta = A * C - mybhep::square(B);

    if (std::isnan(A) || std::isinf(A)) {
      if (print_level() > mybhep::NORMAL) {
        std::clog << "CAT::CircleRegression::fit: problem: in circle regression, A=" << A << " Sw="
                  << Sw << " Swxx=" << Swxx << " Swx=" << Swx << std::endl;
      }
      return false;
    }
    if (std::isnan(B) || std::isinf(B)) {
      if (print_level() > mybhep::NORMAL) {
        std::clog << "CAT::CircleRegression::fit: problem: in circle regression, B=" << B << " Sw="
                  << Sw << " Swxy=" << Swxy << " Swx=" << Swx << " Swy=" << Swy << std::endl;
      } 
      return false;
    }
    if (std::isnan(C) || std::isinf(C)) {
      if (print_level() > mybhep::NORMAL) {
        std::clog << "CAT::CircleRegression::fit: problem: in circle regression, C=" << C << " Sw="
                  << Sw << " Swyy=" << Swyy << " Swy=" << Swy << std::endl;
      }
      return false;
    }
    if (std::isnan(D) || std::isinf(D)) {
      if (print_level() > mybhep::NORMAL) {
        std::clog << "CAT::CircleRegression::fit: problem: in circle regression, D=" << D << " Sw="
                  << Sw << " Swxyy=" << Swxyy << " Swx=" << Swx << " Swyy=" << Swyy << " Swxxx="
                  << Swxxx << " Swxx=" << Swxx << std::endl;
      }
      return false;
    }
    if (std::isnan(E) || std::isinf(E)) {
      if (print_level() > mybhep::NORMAL) {
        std::clog << "CAT::CircleRegression::fit: problem: in circle regression, E=" << E << " Sw="
                  << Sw << " Swxxy=" << Swxxy << " Swy=" << Swy << " Swxx=" << Swxx << " Swyyy="
                  << Swyyy << " Swyy=" << Swyy << std::endl;
      }
      return false;
    }

    if (delta == 0.) {
      if (print_level() > mybhep::NORMAL) {
        std::clog << "CAT::CircleRegression::fit: problem: in circle regression, delta=" << delta
                  << " A=" << A << " C=" << C << " B=" << B << std::endl;
      }
      return false;
    }

    double XC = (D * C - B * E) / delta;
    double YC = (A * E - B * D) / delta;
    double rsum = 0.;
    for (std::vector<experimental_double>::iterator it = xi_.begin(); it != xi_.end(); ++it) {
      double u = it->value() - XC;
      double y = yi_[it - xi_.begin()].value();
      double v = y - YC;
      rsum += std::sqrt(mybhep::square(u) + mybhep::square(v));
    }
    rsum /= xi_.size();

    xc.set(XC, 0.);
    yc.set(YC, 0.);
    r.set(rsum, 0.);
    c_ = circle(experimental_point(xc, experimental_double(0., 0.), yc), r, print_level(),
                probmin());

    double mean_error_x = 0.;
    double mean_error_y = 0.;
    double mean_error_r = 0.;
    for (std::vector<experimental_double>::iterator it = xi_.begin(); it != xi_.end(); ++it) {
      topology::experimental_point local_point(*it, experimental_double(0., 0.),
                                               yi_[it - xi_.begin()]);
      topology::experimental_point circle_point = c_.position(local_point);
      topology::experimental_vector distance(local_point, circle_point);
      mean_error_x += pow(distance.x().value(), 2);
      mean_error_y += pow(distance.z().value(), 2);
      mean_error_r += pow(distance.x().value(), 2) + pow(distance.z().value(), 2);
    }

    xc.set_error(std::sqrt(mean_error_x / xi_.size()));
    yc.set_error(std::sqrt(mean_error_y / xi_.size()));
    r.set_error(std::sqrt(mean_error_r / xi_.size()));

    if (print_level() >= mybhep::VVERBOSE) {
      std::clog << "CAT::CircleRegression::fit: fitted circle through " << xi_.size()
                << " points: xc: ";
      xc.dump();
      std::clog << " yc: ";
      yc.dump();
      std::clog << " r: ";
      r.dump();
      std::clog << " " << std::endl;
    }
  //}

  c_ = circle(experimental_point(xc, experimental_double(0., 0.), yc), r, print_level(), probmin());

  return points_in_good_order();
}

#if CAT_WITH_DEVEL_ROOT == 1
bool CircleRegression::minuit_fit(void) {
  if (xi_.size() != yi_.size()) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << "CAT::CircleRegression::minuit_fit: problem: in circle regression, sizes x="
                << xi_.size() << " y=" << yi_.size() << std::endl;
    }
    return false;
  }

  if (xi_.size() <= 2) return false;

  experimental_double xc, yc, r;

  ROOT::Math::Minimizer *min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

  min->SetMaxFunctionCalls(1000000);
  //      min->SetMaxIterations(100000);
  min->SetTolerance(0.001);
  min->SetPrintLevel(0);

  //      CircleRegression cl;
  ROOT::Math::Functor f(&(*this), &CAT::topology::CircleRegression::expression_to_be_minimized, 3);
  double variable[3] = {c_.center().x().value(), c_.center().z().value(), c_.radius().value()};
  double step[3] = {std::abs(variable[0]) / 100., std::abs(variable[1]) / 100.,
                    std::abs(variable[2]) / 100.};

  double initial_value = expression_to_be_minimized(variable);

  min->SetFunction(f);

  // Set the free variables to be minimized!
  min->SetVariable(0, "center_x", variable[0], step[0]);
  min->SetVariable(1, "center_y", variable[1], step[1]);
  min->SetVariable(2, "radius", variable[2], step[2]);

  min->Minimize();

  const double *xs = min->X();
  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << "CAT::CircleRegression::minuit_fit:Minimum: f(" << xs[0] << "," << xs[1] << ", "
              << xs[2] << "): " << expression_to_be_minimized(xs) << std::endl;
  }

  if (std::isnan(xs[0]) || std::isnan(xs[1]) || std::isnan(xs[2])) return false;

  double final_value = expression_to_be_minimized(xs);

  if (final_value > initial_value) {
    if (print_level() >= mybhep::VVERBOSE) {
      std::clog << "CAT::CircleRegression::minuit_fit: the minimization result " << final_value
                << " is worse than the seed " << initial_value << std::endl;
    }
    return false;
  }

  xc.set(xs[0], 0.);
  yc.set(xs[1], 0.);
  r.set(xs[2], 0.);

  c_ = circle(experimental_point(xc, experimental_double(0., 0.), yc), r, print_level(), probmin());

  return points_in_good_order();
}
#endif  // CAT_WITH_DEVEL_ROOT == 1

experimental_point CircleRegression::position(experimental_double &phi) { return c_.position(phi); }

bool CircleRegression::points_in_good_order(void) {
  // check the points are monotonically moving along the circle

  size_t s = xi_.size();
  if (s < 3) return true;

  experimental_double deltaphiAB, deltaphi_product;
  size_t index;

  experimental_double phi_second =
      c_.phi_of_point(experimental_point(xi_[1], experimental_double(0., 0.), yi_[1]));
  experimental_double phi_initial = c_.phi_of_point(
      experimental_point(xi_[0], experimental_double(0., 0.), yi_[0]), phi_second.value());
  experimental_double phi_final_minus_one = c_.phi_of_point(
      experimental_point(xi_[s - 2], experimental_double(0., 0.), yi_[s - 2]), phi_initial.value());
  experimental_double phi_final =
      c_.phi_of_point(experimental_point(xi_.back(), experimental_double(0., 0.), yi_.back()),
                      phi_final_minus_one.value());
  experimental_double deltaphi_overall = phi_final - phi_initial;
  experimental_double phiA = phi_initial;
  experimental_double phiB = phi_initial;

  for (std::vector<experimental_double>::iterator it = xi_.begin(); it != xi_.end(); ++it) {
    index = it - xi_.begin();
    if (index >= 1) {
      phiA = c_.phi_of_point(
          experimental_point(xi_[index - 1], experimental_double(0., 0.), yi_[index - 1]),
          phiA.value());
      phiB = c_.phi_of_point(experimental_point(*it, experimental_double(0., 0.), yi_[index]),
                             phiA.value());
      deltaphiAB = phiB - phiA;
      deltaphi_product = deltaphiAB * deltaphi_overall;
      if (deltaphi_product.value() < -deltaphi_product.error()) {
        if (print_level() >= mybhep::VVERBOSE) {
          std::clog
              << "CAT::CircleRegression::points_in_good_order: points are not in good order: phi[ "
              << index - 1 << "] = " << phiA.value() << ", phi[" << index << "] = " << phiB.value()
              << ", deltaphi_overall = " << deltaphi_overall.value() << std::endl;
        }
        return false;
      }
    }
  }
  return true;
}
}

}  // namespace CAT
