#include <lttc/tangent_circle_3x1.hh>

// Third party:
#include <datatools/logger.h>
#include <datatools/clhep_units.h>

namespace lttc {
  
  namespace gergonne {

    // https://fr.wikipedia.org/wiki/Cercle_circonscrit_%C3%A0_un_triangle
    circle make_circle_from_three_points(const point & p1, const point & p2, const point & p3, double eps_, bool debug_)
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      if (debug_) logging = datatools::logger::PRIO_DEBUG;
      double eps2 = eps_ * eps_;
      circle c;
      c.invalidate();
      double x1 = p1.x;
      double y1 = p1.y;
      double x2 = p2.x;
      double y2 = p2.y;
      double x3 = p3.x;
      double y3 = p3.y;
      double D = 2 * (x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2);
      DT_LOG_DEBUG(logging, "D = " << D);
      if (std::abs(D) > eps2) {
        double X1 = x1 * x1 + y1 * y1;
        double X2 = x2 * x2 + y2 * y2;
        double X3 = x3 * x3 + y3 * y3;
        double DX =  (X1 * y2 + X2 * y3 + X3 * y1 - X1 * y3 - X2 * y1 - X3 * y2);
        double DY = -(X1 * x2 + X2 * x3 + X3 * x1 - X1 * x3 - X2 * x1 - X3 * x2);
        DX /= D;
        DY /= D;
        double R = std::hypot(DX - x1, DY - y1);
        c.p.x = DX;
        c.p.y = DY;
        c.r   = R;
      } else {
        DT_LOG_DEBUG(logging, "D = 0 : " << "Cannot build the circle!");
      }
      return c;
    }
   
    solver1::solver1(const gergonne::circle ** circles_,
                     int i_,
                     double a1_, double b1_,
                     double a2_, double b2_,
                     double eps_,
                     bool debug_
                     ) {
      debug = debug_;
      circles = circles_;
      i = i_;
      a1 = a1_;
      b1 = b1_;
      a2 = a2_;
      b2 = b2_;
      eps = eps_;
      eps2 = eps * eps;
      eps3 = eps2 * eps;
      if (debug) {
        std::cerr << "[debug] epsilon=" << eps << '\n';
      }
      return;
    }

    void solver1::solve(double r1_, double r2_, double r3_,
                        std::vector<result_type> & results_) const
    {
      double r1 = r1_;
      double r2 = r2_;
      double r3 = r3_;
      double r13 = r3 - r1;
      double r23 = r3 - r2;
      double d1 = a1*a1 + b1*b1 - r13 * r13;
      double d2 = a2*a2 + b2*b2 - r23 * r23;
      double c1 = -r3 * r13;
      double c2 = -r3 * r23;
      double alpha = a1 * d2 - a2 * d1;
      double beta  = b1 * d2 - b2 * d1;
      double gamma = c1 * d2 - c2 * d1;
      if (debug) {
        std::cerr << "[debug] r1 = " << r1 << '\n';
        std::cerr << "[debug] r2 = " << r2 << '\n';
        std::cerr << "[debug] r3 = " << r3 << '\n';
        std::cerr << "[debug] r13 = " << r13 << '\n';
        std::cerr << "[debug] r23 = " << r23 << '\n';
        std::cerr << "[debug] d1 = " << d1 << '\n';
        std::cerr << "[debug] d2 = " << d2 << '\n';
        std::cerr << "[debug] c1 = " << c1 << '\n';
        std::cerr << "[debug] c2 = " << c2 << '\n';
        std::cerr << "[debug] alpha = " << alpha << '\n';
        std::cerr << "[debug] beta  = " << beta << '\n';
        std::cerr << "[debug] gamma = " << gamma << '\n';
      }
      if (std::abs(alpha) < eps) {
        double y1 = -gamma / beta;
        double y2 = y1;
        double x1 = std::sqrt(r3*r3-y1*y1);
        double x2 = -x1;
        point p;
        p.x = x1 + circles[i]->p.x;
        p.y = y1 + circles[i]->p.y;
        result_type res;
        res.i = i;
        res.p = p;
        results_.push_back(res);
        p.x = x2 + circles[i]->p.x;
        p.y = y2 + circles[i]->p.y;
        res.p = p;
        results_.push_back(res);
        if (debug) {
          std::cerr << "[debug] alpha = 0" << '\n';
          std::cerr << "[debug]   x1,y1 = " << x1 << "," << y1 << '\n';
          std::cerr << "[debug]   x2,y2 = " << x2 << "," << y2 << '\n';
        }
      } else if (std::abs(beta) < eps3) {
        double x1 = -gamma / alpha;
        double x2 = x1;
        double y1 = std::sqrt(r3*r3-x1*x1);
        double y2 = -y1;
        point p;
        p.x = x1 + circles[i]->p.x;
        p.y = y1 + circles[i]->p.y;
        result_type res;
        res.i = i;
        res.p = p;
        results_.push_back(res);
        p.x = x2 + circles[i]->p.x;
        p.y = y2 + circles[i]->p.y;
        res.p = p;
        results_.push_back(res);
        if (debug) {
          std::cerr << "[debug] beta = 0" << '\n';
          std::cerr << "[debug]   x1,y1 = " << x1 << "," << y1 << '\n';
          std::cerr << "[debug]   x2,y2 = " << x2 << "," << y2 << '\n';
        }
      } else if ((std::abs(r3 - r1) < eps) and
                 (std::abs(r3 - r2) < eps)) {
        double x1 = beta * r3 / std::sqrt(alpha*alpha+beta*beta);
        double x2 = -x1;
        double y1 = -alpha * x1 / beta;
        double y2 = -alpha * x2 / beta;
        point p;
        p.x = x1 + circles[i]->p.x;
        p.y = y1 + circles[i]->p.y;
        result_type res;
        res.i = i;
        res.p = p;
        results_.push_back(res);
        p.x = x2 + circles[i]->p.x;
        p.y = y2 + circles[i]->p.y;
        res.p = p;
        results_.push_back(res);
        if (debug) {
          std::cerr << "[debug] r3=r1 and r3=r2" << '\n';
          std::cerr << "[debug]   x1,y1 = " << x1 << "," << y1 << '\n';
          std::cerr << "[debug]   x2,y2 = " << x2 << "," << y2 << '\n';
        }
      } else {
        double A = alpha * alpha + beta * beta;
        double B = 2*alpha*gamma;
        double C = gamma*gamma - beta*beta*r3*r3;
        double D = B*B - 4*A*C;
        if (debug) {
          std::cerr << "[debug] D=" << D << " ; epsilon=" << eps << '\n';
        }
        if (std::abs(D) < eps) {
          double x1 = -B / (A+A);
          double y1 = - (alpha * x1 + gamma) / beta;
          point p;
          p.x = x1 + circles[i]->p.x;
          p.y = y1 + circles[i]->p.y;
          result_type res;
          res.i = i;
          res.p = p;
          results_.push_back(res);
          if (debug) {
            std::cerr << "[debug] D=0" << '\n';
            std::cerr << "[debug]   x1,y1 = " << x1 << "," << y1 << '\n';
          }
        } else if (D > 0.0) {
          double x1 = (-B + std::sqrt(D)) / (A+A);
          double y1 = - (alpha * x1 + gamma) / beta;
          double x2 = (-B - std::sqrt(D)) / (A+A);
          double y2 = - (alpha * x2 + gamma) / beta;
          point p;
          p.x = x1 + circles[i]->p.x;
          p.y = y1 + circles[i]->p.y;
          result_type res;
          res.i = i;
          res.p = p;
          results_.push_back(res);
          p.x = x2 + circles[i]->p.x;
          p.y = y2 + circles[i]->p.y;
          res.p = p;
          results_.push_back(res);
          if (debug) {
            std::cerr << "[debug] D>0" << '\n';
            std::cerr << "[debug]   x1,y1 = " << x1 << "," << y1 << '\n';
            std::cerr << "[debug]   x2,y2 = " << x2 << "," << y2 << '\n';
          }
        }
      }
      return;
    }
 
  } // namespace gergonne
  
  tangent_circle_3x1::tangent_circle_3x1(const config_type & cfg_)
  {
    config = cfg_;
    circles[0] = nullptr;
    circles[1] = nullptr;
    circles[2] = nullptr;
    return;
  }

  void tangent_circle_3x1::solve(const contact_config & cfg_,
                                 std::vector<gergonne::result_type> & results_)
  {
    double rho[3];
    rho[0] = circles[0]->r;
    rho[1] = circles[1]->r;
    rho[2] = circles[2]->r;
    if (not cfg_.flags[0]) rho[0] *= -1;
    if (not cfg_.flags[1]) rho[1] *= -1;
    if (not cfg_.flags[2]) rho[2] *= -1;
    for (int i = 0; i < 3; ++i) { 
      if (config.debug) {
        std::cerr << "[debug] ====> Processing circle #" << i << " from the triplet..." << '\n';
      }
      double r3 = rho[i];
      double a1 = circles[(i+1)%3]->p.x - circles[i]->p.x;
      double b1 = circles[(i+1)%3]->p.y - circles[i]->p.y;
      double r1 = rho[(i+1)%3];
      double a2 = circles[(i+2)%3]->p.x - circles[i]->p.x;
      double b2 = circles[(i+2)%3]->p.y - circles[i]->p.y;
      double r2 = rho[(i+2)%3];
      if (config.debug) {
        std::cerr << "[debug] a1 = " << a1 << '\n';
        std::cerr << "[debug] b1 = " << b1 << '\n';
        std::cerr << "[debug] a2 = " << a2 << '\n';
        std::cerr << "[debug] b2 = " << b2 << '\n';
        std::cerr << "[debug] r1 = " << r1 << '\n';
        std::cerr << "[debug] r2 = " << r2 << '\n';
        std::cerr << "[debug] r3 = " << r3 << '\n';
      }
      gergonne::solver1 fs(circles, i, a1, b1, a2, b2, config.epsilon, config.debug);
      fs.solve(r1, r2, r3, results_);
    }
    return;
  }
  
  void tangent_circle_3x1::process(const input_data & idata_, output_data & odata_)
  {
    odata_.clear();
    circles[0] = idata_.circles[0];
    circles[1] = idata_.circles[1];
    circles[2] = idata_.circles[2];  
    for (const auto & cfg : contact_configs()) {
      std::vector<gergonne::result_type> vresults;
      this->solve(cfg, vresults);
      if (config.debug) {
        std::cerr << "[debug] solve: #results = " << ' ' << vresults.size() << '\n';
      }
      std::vector<gergonne::point> pts[3];
      for (auto v : vresults) {
        pts[v.i].push_back(v.p);
      }
      for (auto v : vresults) {
        if (config.debug) {

          std::cerr << "[debug]" << " candidate contact on circle #" << v.i << " : " << v.p.x << ' ' << v.p.y << '\n';
        }
      }
      std::vector<gergonne::triplet> triplets;
      triplets.reserve(8);
      for (auto p0 : pts[0]) {
        for (auto p1 : pts[1]) {
          for (auto p2 : pts[2]) {
            gergonne::triplet tr;
            tr.indexes[0] = idata_.hit_indexes[0];
            tr.indexes[1] = idata_.hit_indexes[1];
            tr.indexes[2] = idata_.hit_indexes[2];
            tr.p[0] = p0;
            tr.p[1] = p1;
            tr.p[2] = p2;
            triplets.push_back(tr);
          }
        }
      }
      for (auto tr : triplets) {
        if (config.debug) {
          std::cerr << "[debug]" 
                    << " triplet[0 : (" << tr.p[0].x << ',' << tr.p[0].y << ')'
                    << ", 1 : (" << tr.p[1].x << ',' << tr.p[1].y << ')'
                    << ", 2 : (" << tr.p[2].x << ',' << tr.p[2].y << ')'
                    << "]\n";
        }
        bool makeDebug = config.debug;
        double makeEps = config.epsilon;
        gergonne::circle c = make_circle_from_three_points(tr.p[0], tr.p[1], tr.p[2], makeEps, makeDebug);
        if (config.debug) {
          std::cerr << "[debug]" 
                    << " Built circle : {x=" << c.p.x << ",y=" << c.p.y << ",r=" << c.r << ')' << '\n';
        }
        bool good = true;
        if (not c.tangent(*idata_.circles[0], config.epsilon)) good = false;
        if (not c.tangent(*idata_.circles[1], config.epsilon)) good = false;
        if (not c.tangent(*idata_.circles[2], config.epsilon)) good = false;
        if (good) {
          circle_data cData;
          cData.hit_index[0] = idata_.hit_indexes[0];
          cData.hit_index[1] = idata_.hit_indexes[1];
          cData.hit_index[2] = idata_.hit_indexes[2];
          cData.contacts[0] = tr.p[0];
          cData.contacts[1] = tr.p[1];
          cData.contacts[2] = tr.p[2];
          cData.tangent_circle = c;
          if (config.debug) {
            cData.print(std::cerr);
          }
          odata_.circles.push_back(cData);
        } else {
          // std::cerr << "[debug]" << " Rejected circle\n";
        }
      }
    }
    return;
  }

  void tangent_circle_3x1::circle_data::print(std::ostream & out_) const
  {
    out_ << "Circle data:\n";
    out_ << "|-- Hits: [" << hit_index[0] << ',' << hit_index[1] << ',' << hit_index[2] << ']' << '\n'; 
    out_ << "|-- Contacts: [" << contacts[0] << ',' << contacts[1] << ',' << contacts[2] << ']' << '\n'; 
    out_ << "`-- Tangent circle: [p=" << tangent_circle.p << ",r=" << tangent_circle.r << ']' << '\n';
    return;
  }
 
} // namespace lttc 
