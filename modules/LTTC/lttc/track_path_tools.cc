#include <lttc/track_path_tools.hh>

// Standard library:
#include <cmath>
#include <tuple>

// Third party:
// - Bayeux :
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <geomtools/utils.h>
#include <geomtools/ellipse.h>
// - GSL :
#include <gsl/gsl_math.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>

namespace lttc {

  using falaise::geometry::point2;
  using falaise::geometry::vector2;
  using falaise::geometry::point3;
  using falaise::geometry::fitted_point2;
  using falaise::geometry::line2;
 
  size_t track_path_data::size() const
  {
    return vertexes.size();
  }

  const fitted_point2 & track_path_data::first() const
  {
    DT_THROW_IF(vertexes.size() == 0, std::logic_error, "Empty track path!");
    return vertexes.front().node;
  }
  
  const fitted_point2 & track_path_data::last() const
  {
    DT_THROW_IF(vertexes.size() == 0, std::logic_error, "Empty track path!");
    return vertexes.back().node;
  }

  std::tuple<point3, double> track_path_data::first3() const
  {
    return vertex3(0);
  }

  std::tuple<point3, double> track_path_data::last3() const
  {
    return vertex3(vertexes.size() - 1);
  }
  
  std::tuple<point3, double> track_path_data::vertex3(int i_) const
  {
    DT_THROW_IF(i_ < 0 or i_ >= (int) vertexes.size(), std::range_error, "Invalid vertex index!");
    point3 v3(vertexes[i_].node.x, vertexes[i_].node.y, vertexes[i_].z);
    return std::tuple<point3, double>{v3, vertexes[i_].z_err};
  }
 
  double track_path_data::get_z_pvalue() const
  {
    double pv = z_pvalue;
    return pv;
  }
  
  double track_path_data::get_effective_z_pvalue() const
  {
    double pv = get_z_pvalue();
    // int nb_hits     = (int) z_residuals.size();
    // int nb_outliers = (int) z_outliers.size();
    // if ((nb_outliers > 0) and (nb_hits - nb_outliers) >= 3) {
    //   // Give a chance for a better p-value without outliers:
    //   pv = z_pvalue_good;
    // }
    return pv;
  }

  void track_path_data::compute_z_and_error(double sxy_, double & z_, double & z_err_) const
  {
    gsl_fit_linear_est(sxy_, z_c0, z_c1, z_cov00, z_cov01, z_cov11, &z_, &z_err_);
    return;
  }

  void track_path_data::compute_z_statistics()
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_DEBUG(logging, "Entering...");
    size_t nbVertexes = this->vertexes.size();
    DT_THROW_IF(nbVertexes == 0, std::logic_error, "Empty track path!");
    DT_LOG_DEBUG(logging, "==> Track path has " << nbVertexes << " vertices");
    if (nbVertexes < 2) {
      DT_LOG_DEBUG(logging, "==> No SZ linear fit can be perfomed!");
    }
    std::vector<double> vertexCurvPositions;
    std::vector<double> vertexZs;
    std::vector<double> vertexWeights;
    vertexCurvPositions.reserve(nbVertexes);
    vertexZs.reserve(nbVertexes);
    vertexWeights.reserve(nbVertexes);
    double curvPos = 0.0;
    vertexCurvPositions.push_back(curvPos);
    // bool zNoise = false;
    // zNoise = true;
    double z0    = this->vertexes[0].z;
    double zerr0 = this->vertexes[0].z_err;
    // if (zNoise) {
    //   zerr0 += 20.0 * CLHEP::mm;
    //   z0 += (-1 + 2 * drand48()) * zerr0;
    // }
    vertexZs.push_back(z0);
    vertexWeights.push_back(1.0 / gsl_pow_2(zerr0));
    for (size_t i = 1; i < nbVertexes; i++) {
      // double za     = this->vertexes[i-1].z;
      // double za_err = this->vertexes[i-1].z_err;
      double zi    = this->vertexes[i].z;
      double zerri = this->vertexes[i].z_err;
      // if (zNoise) {
      //   zerri += 20.0 * CLHEP::mm;
      //   zi += (-1 + 2 * drand48()) * zerri;
      // }
      double zb     = zi;
      double zb_err = zerri;
      double xa = this->vertexes[i-1].node.x;
      double ya = this->vertexes[i-1].node.y;
      double xb = this->vertexes[i].node.x;
      double yb = this->vertexes[i].node.y;
      double dx = xb - xa;
      double dy = yb - ya;
      double dsxy = std::sqrt(gsl_pow_2(dx) + gsl_pow_2(dy));
      curvPos += dsxy;
      vertexCurvPositions.push_back(curvPos);
      vertexZs.push_back(zb);
      vertexWeights.push_back(1.0 / gsl_pow_2(zb_err));
    }
    if (datatools::logger::is_debug(logging)) {
      std::cerr << "# ===============================\n";
      for (size_t i = 0; i < nbVertexes; i++) {
        std::cerr << vertexCurvPositions[i] << " " << vertexZs[i] << " " << vertexWeights[i] << '\n';
      }
      std::cerr << "# ===============================\n";
    }
    {
      // Linear fit : Z( +/- Zerr) = c0 + c1.s
      DT_LOG_DEBUG(logging, "Performing a linear fit...");
      double c0, c1, cov00, cov01, cov11, chisq;
      size_t ndof = nbVertexes - 2;
      gsl_fit_wlinear(vertexCurvPositions.data(), 1,
                      vertexWeights.data(), 1,
                      vertexZs.data(), 1,
                      nbVertexes,
                      &c0, &c1, &cov00, &cov01, &cov11,
                      &chisq);
      this->z_c0    = c0;
      this->z_c1    = c1;
      this->z_cov00 = cov00;
      this->z_cov01 = cov01;
      this->z_cov11 = cov11;
      this->z_ndof  = ndof;
      this->z_chi2  = chisq;
      this->z_pvalue = gsl_cdf_chisq_Q(this->z_chi2, this->z_ndof);
      for (size_t i = 0; i < nbVertexes; i++) {
        double sxy = vertexCurvPositions[i];
        double z, z_err;
        gsl_fit_linear_est(sxy, c0, c1, cov00, cov01, cov11, &z, &z_err);
        this->vertexes[i].sxy = sxy;
        this->vertexes[i].z = z;
        this->vertexes[i].z_err = z_err;
      }
      DT_LOG_DEBUG(logging, "Reference linear fit results:");
      DT_LOG_DEBUG(logging, "  c0     = " << this->z_c0    <<  "  c1 = " << this->z_c1);
      DT_LOG_DEBUG(logging, "  cov00  = " << this->z_cov00 << "  cov01 = " << this->z_cov01 << "  cov11 = " << this->z_cov11);
      DT_LOG_DEBUG(logging, "  ndof   = " << this->z_ndof  <<  "  chi2 = " << this->z_chi2);
      DT_LOG_DEBUG(logging, "  pvalue = " << this->z_pvalue);
    }
    
    typedef std::tuple<double, // c0
                       double, // c1
                       double, // cov00
                       double, // cov01
                       double, // cov11
                       double, // chisq
                       size_t, // ndof
                       double> // p-value
      fit_result_type;
    std::map<int, fit_result_type> outliers;
    if (nbVertexes > 2 and (this->z_pvalue < 0.05)) {
      // Attempt to improve the SZ fit by trimming some vertex:
      for (size_t iTrim = 0; iTrim < nbVertexes; iTrim++) {
        std::vector<double> vertexCurvPositionsTrim;
        std::vector<double> vertexZsTrim;
        std::vector<double> vertexWeightsTrim;
        vertexCurvPositionsTrim.reserve(nbVertexes - 1);
        vertexZsTrim.reserve(nbVertexes - 1);
        vertexWeightsTrim.reserve(nbVertexes - 1);
        for (size_t i = 0; i < nbVertexes; i++) {
          if (i == iTrim) continue; // Skip this one
          vertexCurvPositionsTrim.push_back(vertexCurvPositions[i]);
          vertexZsTrim.push_back(vertexZs[i]);
          vertexWeightsTrim.push_back(vertexWeights[i]);
        }
        // Linear fit : Z( +/- Zerr) = c0 + c1.s
        DT_LOG_DEBUG(logging, "Performing a linear fit with trimmed vertex #" << iTrim);
        double c0, c1, cov00, cov01, cov11, chisq;
        size_t ndof = vertexCurvPositionsTrim.size() - 2;
        gsl_fit_wlinear(vertexCurvPositionsTrim.data(), 1,
                        vertexWeightsTrim.data(), 1,
                        vertexZsTrim.data(), 1,
                        vertexCurvPositionsTrim.size(),
                        &c0, &c1, &cov00, &cov01, &cov11,
                        &chisq);
        double pValue = gsl_cdf_chisq_Q(chisq, ndof);
        DT_LOG_DEBUG(logging, "Trimmed vertex #" << iTrim << ":  Linear fit results:");
        DT_LOG_DEBUG(logging, "  c0     = " << c0    <<  "  c1 = " << c1);
        DT_LOG_DEBUG(logging, "  cov00  = " << cov00 << "  cov01 = " << cov01 << "  cov11 = " << cov11);
        DT_LOG_DEBUG(logging, "  ndof   = " << ndof  <<  "  chisq = " << chisq);
        // this->z_c0    = c0;
        // this->z_c1    = c1;
        // this->z_cov00 = cov00;
        // this->z_cov01 = cov01;
        // this->z_cov11 = cov11;
        // this->z_ndof  = ndof;
        // this->z_chi2  = chisq;
        // this->z_pvalue = gsl_cdf_chisq_Q(this->z_chi2, this->z_ndof);
        DT_LOG_DEBUG(logging, "  pvalue = " << pValue);
        if (pValue > 0.05) {
          // If the fit result is now acceptable, count this vertex as an outlier and record the fit result:
          fit_result_type fitResult = std::make_tuple(c0, c1, cov00, cov01, cov11, chisq, ndof, pValue);
          outliers[iTrim] = fitResult;
        }
        // for (size_t i = 0; i <vertexCurvPositionsTrim.size(); i++) {
        //   double sxy = vertexCurvPositionsTrim[i];
        //   double z, z_err;
        //   gsl_fit_linear_est(sxy, c0, c1, cov00, cov01, cov11, &z, &z_err);
        //   this->vertexes[i].sxy = sxy;
        //   this->vertexes[i].z = z;
        //   this->vertexes[i].z_err = z_err;
        // }    
      }
    }
    DT_LOG_DEBUG(logging, "  Number of candidate trimmed vertices = " << outliers.size());
    int bestOutlierKey = -1;
    double maxOutlierPvalue = datatools::invalid_real();
    for (auto item : outliers) {
      double pValue = std::get<7>(item.second);
      DT_LOG_DEBUG(logging, "  Trimmed vertex #" << item.first << " => p-value=" << pValue);
      if ((bestOutlierKey == -1) or pValue > maxOutlierPvalue) {
        maxOutlierPvalue = pValue;
        bestOutlierKey = item.first;
      }
    }
    if (bestOutlierKey > -1) {
      const auto & firstOutlier = *outliers.find(bestOutlierKey);
      this->z_c0    = std::get<0>(firstOutlier.second);
      this->z_c1    = std::get<1>(firstOutlier.second);
      this->z_cov00 = std::get<2>(firstOutlier.second);
      this->z_cov01 = std::get<3>(firstOutlier.second);
      this->z_cov11 = std::get<4>(firstOutlier.second);
      this->z_ndof  = std::get<5>(firstOutlier.second);
      this->z_chi2  = std::get<6>(firstOutlier.second);
      this->z_pvalue = std::get<7>(firstOutlier.second);  
      this->z_trimmed.insert(bestOutlierKey);  
    }
    DT_LOG_DEBUG(logging, "Exiting.");
    return;
  }


  void track_path_data::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "Cluster index : " << cluster_index << '\n';
    out_ << indent_ << "|-- " << "Vertexes : " << vertexes.size() << '\n';
    out_ << indent_ << "|-- " << "Z statistics : " << '\n';
    out_ << indent_ << "|   " << "|-- " << "c0      = " << z_c0 << '\n';
    out_ << indent_ << "|   " << "|-- " << "c1      = " << z_c1 << '\n';
    out_ << indent_ << "|   " << "|-- " << "cov00   = " << z_cov00 << '\n';
    out_ << indent_ << "|   " << "|-- " << "cov01   = " << z_cov01 << '\n';
    out_ << indent_ << "|   " << "|-- " << "cov11   = " << z_cov11 << '\n';
    out_ << indent_ << "|   " << "|-- " << "ndof    = " << z_ndof << '\n';
    out_ << indent_ << "|   " << "|-- " << "chi2    = " << z_chi2 << '\n';
    out_ << indent_ << "|   " << "`-- " << "P-value = " << z_pvalue << '\n';
    // out_ << indent_ << "|   " << "|-- " << "# of segments         = " << z_residuals.size() << '\n';
    // out_ << indent_ << "|   " << "|-- " << "# of outlier segments = " << z_outliers.size() << '\n';
    // out_ << indent_ << "|   " << "|-- " << "# of bad segments     = " << z_bads.size() << '\n';
    // out_ << indent_ << "|   " << "|-- " << "Ndof        = " << z_ndof << '\n';
    // out_ << indent_ << "|   " << "|-- " << "Slope       = " << z_slope << '\n';
    // out_ << indent_ << "|   " << "|-- " << "Slope error = " << z_slope_sigma << '\n';
    // out_ << indent_ << "|   " << "|-- " << "Chi2        = " << z_chi2 << '\n';
    // out_ << indent_ << "|   " << "`-- " << "P-value     = " << z_pvalue << '\n';
    // if (z_outliers.size()) {
    //   out_ << indent_ << "|-- " << "Z statistics (only good)" << '\n';
    //   out_ << indent_ << "|   " << "|-- " << "Ndof = " << z_ndof_good << '\n';
    //   out_ << indent_ << "|   " << "|-- " << "Slope = " << z_slope_good << '\n';
    //   out_ << indent_ << "|   " << "|-- " << "Slope error = " << z_slope_sigma_good << '\n';
    //   out_ << indent_ << "|   " << "|-- " << "Chi2 = " << z_chi2_good << '\n';
    //   out_ << indent_ << "|   " << "`-- " << "P-value = " << z_pvalue_good << '\n';
    // }
    out_ << indent_ << "`-- " << "P-value (effective) = " << get_effective_z_pvalue() << '\n';
    // out_ << indent_ << "    " << "`-- " << "end" << '\n';
    return;
  }
 
  void track_path_data::draw(std::ostream & out_, int tag_) const
  {
    for (int i = 0; i < (int) vertexes.size(); i++) {
      vertexes[i].node.draw(out_, 1.0, false, tag_);
    }
    for (int i = 0; i < (int) vertexes.size(); i++) {
      out_ << vertexes[i].node.x << ' ' << vertexes[i].node.y << ' ' << vertexes[i].z + vertexes[i].z_err << ' ' << tag_ << '\n';      
      out_ << vertexes[i].node.x << ' ' << vertexes[i].node.y << ' ' << vertexes[i].z - vertexes[i].z_err << ' ' << tag_ << '\n';
      out_ << '\n';
    }
    for (int i = 0; i < (int) vertexes.size(); i++) {
      out_ << vertexes[i].node.x << ' ' << vertexes[i].node.y << ' ' << vertexes[i].z << ' ' << tag_ << '\n';
    }
    out_ << '\n';
    return;
  }

  track_path_interceptor::track_path_interceptor(mode_type mode_,
                                                 datatools::logger::priority logging_)
    : _mode_(mode_)
    , _logging_(logging_)
  {
    return;
  }
 
  bool track_path_interceptor::find(const track_path_data & tp_a_,
                                    const track_path_data & tp_b_,
                                    track_path_intercept & tpi_)
  {
    tpi_.pattern = TPIP_NONE;
    geomtools::invalidate(tpi_.intercept);
    DT_THROW_IF(tp_a_.size() < 2, std::logic_error, "Not enough vertex along first track path!");
    DT_THROW_IF(tp_b_.size() < 2, std::logic_error, "Not enough vertex along second track path!");
    if (_mode_ == MODE_LINE) {
      _find_linear_(tp_a_, tp_b_, tpi_);
    } // MODE_LINE
    if (_mode_ == MODE_HELIX) {
      _find_helix_(tp_a_, tp_b_, tpi_);
    } // MODE_HELIX

    return tpi_.pattern != TPIP_NONE;
  }
  
  bool track_path_interceptor::_find_helix_(const track_path_data & /* tp_a_ */,
                                            const track_path_data & /* tp_b_ */,
                                            track_path_intercept & tpi_)
  {
    return tpi_.pattern != TPIP_NONE;
  }
   

  
  bool track_path_interceptor::_find_linear_(const track_path_data & tp_a_,
                                             const track_path_data & tp_b_,
                                             track_path_intercept & tpi_)
  {
    DT_LOG_DEBUG(_logging_, "Entering...");
    // From track path A:
    point2 F_a(tp_a_.first().x, tp_a_.first().y);
    point2 L_a(tp_a_.last().x,  tp_a_.last().y);
    line2 line_a = line2::make_from_start_stop(F_a, L_a);
    vector2 FL_a = L_a -  F_a;
      
    // From track path B:
    point2 F_b(tp_b_.first().x, tp_b_.first().y);
    point2 L_b(tp_b_.last().x,  tp_b_.last().y);
    line2 line_b = line2::make_from_start_stop(F_b, L_b);
    vector2 FL_b = L_b -  F_b;
    DT_LOG_DEBUG(_logging_, "A.first = " << F_a);
    DT_LOG_DEBUG(_logging_, "A.last  = " << L_a);
    DT_LOG_DEBUG(_logging_, "B.first = " << F_b);
    DT_LOG_DEBUG(_logging_, "B.last  = " << L_b);
 
    point2 intercept; 
    if (line_a.unique_intersection(line_b, intercept)) {
      DT_LOG_DEBUG(_logging_, "I  = " << intercept);
      uint16_t pattern = 0;
      vector2 FI_a = intercept - F_a;
      vector2 LI_a = intercept - L_a;
      double dist_a = datatools::invalid_real();
      double dist_b = datatools::invalid_real();
      if (FI_a.dot(LI_a) < 0.0) {
        pattern |= TPIB_AM;
        dist_a = FI_a.mag();
      } else {
        if (FI_a.dot(FL_a) <= 0.0) {
          pattern |= TPIB_AF;
          dist_a = -FI_a.mag();
        } else {
          pattern |= TPIB_AL;
          dist_a = (L_a - F_a).mag() + LI_a.mag();
        } 
      }
      vector2 FI_b = intercept - F_b;
      vector2 LI_b = intercept - L_b;
      if (FI_b.dot(LI_b) < 0.0) {
        pattern |= TPIB_BM;
        dist_b = FI_b.mag();
      } else {
        if (FI_b.dot(FL_b) <= 0.0) {
          pattern |= TPIB_BF;
          dist_b = -FI_b.mag();
        } else {
          pattern |= TPIB_BL;
          dist_b= (L_b - F_b).mag() + LI_b.mag();
        } 
      }
      DT_LOG_DEBUG(_logging_, "pattern  = " << pattern);
      tpi_.pattern   = pattern;
      tpi_.intercept = intercept;
      tpi_.s_first = dist_a;
      tpi_.s_second = dist_b;
    } 
    return tpi_.pattern != TPIP_NONE;
  }

} // namespace lttc 
