#include <lttc/rt_map.hh>

// Standard library:
#include <cmath>
#include <numeric> // std::iota
#include <algorithm> // std::sort

// This project:
#include <lttc/legendre_transform_tools.hh>

namespace lttc {
  
  void rt_map::clear()
  {
    id.clear();
    sorted_bins.clear();
    bins.clear();
    tmin = 0.0 * CLHEP::radian;
    tmax = M_PI * CLHEP::radian; 
    rmin =  std::numeric_limits<double>::infinity();
    rmax = -std::numeric_limits<double>::infinity();
    nt = 0;
    nr = 0;
    dt = 0.01 * CLHEP::radian;
    dr = 1.0 * CLHEP::mm;
    return;
  }
 
  int rt_map::r_to_index(double r_) const
  {
    if (r_ < rmin) return -1;
    if (r_ >= rmax) return -1;
    return (int) ((r_ - rmin) / dr);
  }
  
  int rt_map::t_to_index(double t_) const
  {
    if (t_ < tmin) return -1;
    if (t_ >= tmax) return -1;
    return (int) ((t_ - tmin) / dt);
  }

  int rt_map::t_r_indexes_to_tr_bin_index(int t_index_, int r_index_) const
  {
    if (t_index_ < 0) return -1;
    if (t_index_ >= (int) nt) return -1;
    if (r_index_ < 0) return -1;
    if (r_index_ >= (int) nr) return -1;
    int ibin = r_index_ * nt + t_index_;
    return ibin;
  }

  bool rt_map::tr_bin_index_to_t_r_indexes(int bin_index_,
                                           int & t_index_, int & r_index_) const
  {
    t_index_ = -1;
    r_index_ = -1;
    if ((bin_index_ < 0) || (bin_index_ >= (int) bins.size())) {
      return false;
    }
    t_index_ = bin_index_ % nt;
    r_index_ = bin_index_ / nt;
    return (t_index_ > -1) && (r_index_ > -1);
  }

  datatools::logger::priority rt_map::fill_bin_ctrl::logging = datatools::logger::PRIO_FATAL;
  
  void rt_map::fill_bin(int it0_, int ir0_, double gauss_threshold_, double weight_)
  {
    double weight = weight_;
    if (gauss_threshold_ <= 0.0) {
      int itr0 = t_r_indexes_to_tr_bin_index(it0_, ir0_);
      if (itr0 >= 0) {
        // if (fill_bin_ctrl::debug) std::cerr << "[debug] rt_map::fill_bin: itr0 = " << itr0 << '\n';    
        bins[itr0] += weight;
      }
    } else {
      if (it0_ < 0) return;
      if (it0_ >= (int) nt) return;
      if (ir0_ < 0) return;
      if (ir0_ >= (int) nr) return;
      double Dt = dt * std::sqrt(-2 * std::log(gauss_threshold_));
      double Dr = dr * std::sqrt(-2 * std::log(gauss_threshold_));
      int Di_t  = (int) (Dt / dt);
      int Di_r  = (int) (Dr / dr);
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "it0  = " << it0_);    
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "ir0  = " << ir0_);    
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "Di_t = " << Di_t);    
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "Di_r = " << Di_r);
      int itmin = it0_ - Di_t;
      if (itmin < 0) itmin = 0;
      int itmax = it0_ + Di_t;
      if (itmax >= (int) nt) itmax = nt - 1;
      int irmin = ir0_ - Di_r;
      if (irmin < 0) irmin = 0;
      int irmax = ir0_ + Di_r;
      if (irmax >= (int) nr) irmax = nr - 1;
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "itmin = " << itmin);    
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "itmax = " << itmax);    
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "irmin = " << irmin);    
      DT_LOG_DEBUG(fill_bin_ctrl::logging, "irmax = " << irmax);
      double t0 = index_to_t(it0_);
      double r0 = index_to_r(ir0_);
      lt_gauss_func fgauss(t0, dt, r0, dr);
      for (int it = itmin; it <= itmax; it++) {
        double tt = index_to_t(it);
        for (int ir = irmin; ir <= irmax; ir++) {
          double rr = index_to_r(ir);
          int itr = t_r_indexes_to_tr_bin_index(it, ir);
          if (itr >= 0) {
            double g = weight * fgauss(tt, rr);
            DT_LOG_DEBUG(fill_bin_ctrl::logging,
                         "it=" << it << "/" << itmax << " ir=" << ir << "/" << irmax
                         << " => itr=" << itr << " => g=" << g << " [" << (g >= gauss_threshold_ ? "yes" : "no") << "]");
            if (g >= gauss_threshold_) {
              bins[itr] += g;
            }
          }
        } 
      }
    }
    return;
  }

  void rt_map::build_sorted_bins()
  {
    DT_LOG_DEBUG(logging, "bins.size = " << bins.size());
    // Default fill the array of sorted bins: 
    sorted_bins.assign(bins.size(), 0);
    std::iota(sorted_bins.begin(), sorted_bins.end(), 0);
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [unsorted]");
    std::sort(sorted_bins.begin(),
              sorted_bins.end(),
              [&](const int & ibin1, const std::size_t & ibin2)
              {
                return bins[ibin1] > bins[ibin2];
              }
              );
    auto last_non_zero = std::remove_if(sorted_bins.begin(),
                                        sorted_bins.end(),
                                        [&](int index_) {
                                          double value = bins[index_];
                                          // if (debug) std::cerr << "[debug] rt_map::build_sorted_bins[lambda2]: index=" << index_ << " -> value = " << value << '\n';
                                          return (value < 1e-7);
                                        }
                                        );
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [sorted]");
    sorted_bins.erase(last_non_zero, sorted_bins.end());
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [final=sorted & unzeroed]");
    // std::cerr << "[debug] rt_map::build_sorted_bins: sorted_bins.size = " << sorted_bins.size() << " [final]" << '\n'; 
    // for (int i = 0; i < sorted_bins.size(); i++) {
    //   std::cerr << "[debug] rt_map::build_sorted_bins: sorted_bins[" << i << "] = " << sorted_bins[i]
    //             << " with value=" << bins[sorted_bins[i]] << '\n';
    // }           
    //  }
    return;
  }
  
  double rt_map::index_to_t(int it_) const
  {
    return tmin + it_ * dt;
  }
  
  double rt_map::index_to_r(int ir_) const
  {
    return rmin + ir_ * dr;
  }

  void rt_map::apply_threshold(double t_)
  {
    bool local_debug = false;
    size_t nbins_non_null = 0;
    size_t nbins_under_threshold = 0;
    for (int i = 0; i < (int) bins.size(); i++) {
      if (bins[i] > 0.0) {
        nbins_non_null++;
        if (bins[i] < t_) {
          bins[i] = 0.0;
          nbins_under_threshold++;
        }
      }
    }
    local_debug = true;
    if (local_debug) {
      std::cerr << "[debug] rt_map::apply_threshold: #bins                 = " << bins.size() << '\n';
      std::cerr << "[debug] rt_map::apply_threshold: #bins non_null        = " << nbins_non_null << '\n';
      std::cerr << "[debug] rt_map::apply_threshold: #bins under threshold = " << nbins_under_threshold << '\n';
      std::cerr << "[debug] rt_map::apply_threshold: #bins over threshold  = " << (nbins_non_null - nbins_under_threshold) << '\n';
    }
    return;
  }

  void rt_map::draw_bins(std::ostream & out_) const
  {
    for (int it = 0; it < (int) nt; it++) {
      out_ << "#@theta=" << index_to_t(it) << '\n';
      for (int ir = 0; ir < (int) nr; ir++) {
        int ibin = t_r_indexes_to_tr_bin_index(it, ir);
        double val = bins[ibin];
        out_ << index_to_t(it) + 0.5 * dt << ' ' << index_to_r(ir) + 0.5 * dr << ' ' << val << '\n';
      }
      out_ << '\n';
    }
    return;
  }

  void rt_map::draw_sorted_bins(std::ostream & out_, double threshold_) const
  {
    bool local_debug = false;
    for (int i = 0; i < (int) sorted_bins.size(); i++) {
      int tr_bin = sorted_bins[i];
      int t_index = -1;
      int r_index = -1;
      tr_bin_index_to_t_r_indexes(tr_bin, t_index, r_index);
      double t = index_to_t(t_index);
      double r = index_to_r(r_index);
      if (local_debug) {
        std::cerr << "[debug] rt_map::draw_sorted_bins: t_index = " << t_index << '\n';
        std::cerr << "[debug] rt_map::draw_sorted_bins: r_index = " << r_index << '\n';
        // std::cerr << "[debug] rt_map::draw_sorted_bins: t = " << t << '\n';
        // std::cerr << "[debug] rt_map::draw_sorted_bins: r = " << r << '\n';
        std::cerr << "[debug] rt_map::draw_sorted_bins: value   = " << bins[tr_bin] << '\n';
      }
      double val = bins[tr_bin];
      if (val >= threshold_) {
        out_ << t + 0.5 * dt << ' ' << r + 0.5 * dr << ' ' << val << '\n';
      }
    }
    return;
  }

  void rt_map::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "id    = '" << id << "'" << '\n';
    out_ << indent_ << "|-- " << "tmin  = " << tmin << " [rad]" << '\n';
    out_ << indent_ << "|-- " << "tmax  = " << tmax << " [rad]" << '\n';
    out_ << indent_ << "|-- " << "rmin  = " << rmin << " [mm]" << '\n';
    out_ << indent_ << "|-- " << "rmax  = " << rmax << " [mm]" << '\n';
    out_ << indent_ << "|-- " << "nt    = " << nt << " [theta bins]" << '\n';
    out_ << indent_ << "|-- " << "nr    = " << nr << " [r bins]" << '\n';
    out_ << indent_ << "|-- " << "dt    = " << dt << " [rad]" << '\n';
    out_ << indent_ << "|-- " << "dr    = " << dr << " [mm]" << '\n';
    out_ << indent_ << "|-- " << "nbins = " << nt * nr << " [bins]" << '\n';
    out_ << indent_ << "|-- " << "bins.size()        = " << bins.size() << " [bins]" << '\n';
    out_ << indent_ << "`-- " << "sorted_bins.size() = " << sorted_bins.size() << " [bins]" << '\n';
    if (sorted_bins.size()) {
      out_ << indent_ << "    " << "|-- " << "first sorted bin @" << sorted_bins.front() << " with " << bins[sorted_bins.front()] << " hits" << '\n';
      out_ << indent_ << "    " << "`-- " << "last  sorted bin @" << sorted_bins.back()  << " with " << bins[sorted_bins.back()] << " hits" << '\n';
    }
    return;
  }


}
