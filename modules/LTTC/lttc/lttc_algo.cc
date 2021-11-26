/// \file lttc_algo.cc

// Ourselves:
#include <lttc/lttc_algo.hh>

// Standard library:
// #include <cmath>
#include <algorithm>
#include <limits>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iomanip>

// Third party:
#include <datatools/utils.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_cdf.h>

// This project:
#include <lttc/dbscan_clusterizer.hh>
#include <lttc/rectangle.hh>

namespace lttc {

  // static
  const size_t lttc_algo::DEFAULT_TNBINS;
  // static
  const size_t lttc_algo::DEFAULT_RNBINS;

  lttc_algo::lttc_algo(const tracker & sntracker_,
                       const config & cfg_)
  {
    sntracker = &sntracker_;
    cfg = cfg_;
    return;
  }
  
  lttc_algo::~lttc_algo()
  {
    return;
  }

  void lttc_algo::map_type::clear()
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
 
  int lttc_algo::map_type::r_to_index(double r_) const
  {
    if (r_ < rmin) return -1;
    if (r_ >= rmax) return -1;
    return (int) ((r_ - rmin) / dr);
  }
  
  int lttc_algo::map_type::t_to_index(double t_) const
  {
    if (t_ < tmin) return -1;
    if (t_ >= tmax) return -1;
    return (int) ((t_ - tmin) / dt);
  }

  int lttc_algo::map_type::t_r_indexes_to_tr_bin_index(int t_index_, int r_index_) const
  {
    if (t_index_ < 0) return -1;
    if (t_index_ >= (int) nt) return -1;
    if (r_index_ < 0) return -1;
    if (r_index_ >= (int) nr) return -1;
    int ibin = r_index_ * nt + t_index_;
    return ibin;
  }

  bool lttc_algo::map_type::tr_bin_index_to_t_r_indexes(int bin_index_,
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

  
  datatools::logger::priority lttc_algo::map_type::fill_bin_ctrl::logging =  datatools::logger::PRIO_FATAL;

  
  void lttc_algo::map_type::fill_bin(int it0_, int ir0_, double gauss_threshold_, double weight_)
  {
    double weight = weight_;
    if (gauss_threshold_ <= 0.0) {
      int itr0 = t_r_indexes_to_tr_bin_index(it0_, ir0_);
      if (itr0 >= 0) {
        // if (fill_bin_ctrl::debug) std::cerr << "[debug] lttc_algo::map_type::fill_bin: itr0 = " << itr0 << '\n';    
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

  void lttc_algo::map_type::build_sorted_bins()
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
                                          // if (debug) std::cerr << "[debug] lttc_algo::map_type::build_sorted_bins[lambda2]: index=" << index_ << " -> value = " << value << '\n';
                                          return (value < 1e-7);
                                        }
                                        );
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [sorted]");
    sorted_bins.erase(last_non_zero, sorted_bins.end());
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [final=sorted & unzeroed]");
    // std::cerr << "[debug] lttc_algo::map_type::build_sorted_bins: sorted_bins.size = " << sorted_bins.size() << " [final]" << '\n'; 
    // for (int i = 0; i < sorted_bins.size(); i++) {
    //   std::cerr << "[debug] lttc_algo::map_type::build_sorted_bins: sorted_bins[" << i << "] = " << sorted_bins[i]
    //             << " with value=" << bins[sorted_bins[i]] << '\n';
    // }           
    //  }
    return;
  }
  
  double lttc_algo::map_type::index_to_t(int it_) const
  {
    return tmin + it_ * dt;
  }
  
  double lttc_algo::map_type::index_to_r(int ir_) const
  {
    return rmin + ir_ * dr;
  }

  void lttc_algo::map_type::apply_threshold(double t_)
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
      std::cerr << "[debug] lttc_algo::map_type::apply_threshold: #bins                 = " << bins.size() << '\n';
      std::cerr << "[debug] lttc_algo::map_type::apply_threshold: #bins non_null        = " << nbins_non_null << '\n';
      std::cerr << "[debug] lttc_algo::map_type::apply_threshold: #bins under threshold = " << nbins_under_threshold << '\n';
      std::cerr << "[debug] lttc_algo::map_type::apply_threshold: #bins over threshold  = " << (nbins_non_null - nbins_under_threshold) << '\n';
    }
    return;
  }

  void lttc_algo::map_type::draw_bins(std::ostream & out_) const
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

  void lttc_algo::map_type::draw_sorted_bins(std::ostream & out_, double threshold_) const
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
        std::cerr << "[debug] lttc_algo::map_type::draw_sorted_bins: t_index = " << t_index << '\n';
        std::cerr << "[debug] lttc_algo::map_type::draw_sorted_bins: r_index = " << r_index << '\n';
        // std::cerr << "[debug] lttc_algo::map_type::draw_sorted_bins: t = " << t << '\n';
        // std::cerr << "[debug] lttc_algo::map_type::draw_sorted_bins: r = " << r << '\n';
        std::cerr << "[debug] lttc_algo::map_type::draw_sorted_bins: value   = " << bins[tr_bin] << '\n';
      }
      double val = bins[tr_bin];
      if (val >= threshold_) {
        out_ << t + 0.5 * dt << ' ' << r + 0.5 * dr << ' ' << val << '\n';
      }
    }
    return;
  }

  void lttc_algo::map_type::print(std::ostream & out_, const std::string & indent_) const
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
      out_ << indent_ << "    " << "`-- " << "last  sorted bin @" << sorted_bins.back() << " with  " << bins[sorted_bins.back()] << " hits" << '\n';
    }
    return;
  }

  void lttc_algo::step1_run()
  {
    step1_data & step1 = current_loop->step1;
    step1.trmap.logging = cfg.logging;
    step1.trmap.id   = "step1";
    step1.trmap.tmin = 0.0;
    step1.trmap.tmax = M_PI;
    step1.trmap.rmin =  std::numeric_limits<double>::infinity();
    step1.trmap.rmax = -std::numeric_limits<double>::infinity();
    DT_LOG_DEBUG(cfg.logging, "Computing step1 map bounds...");
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      if (sntracker->has_tracker_conditions()) {
        // This hit is invalidated because the hit cell is considered dead:
        if (sntracker->get_tracker_conditions().has_dead_cell(cid)) {
          continue;
        }
      }
      point cellCenter = sntracker->cell_position(cid);
      double x = cellCenter.x();
      double y = cellCenter.y();
      double r1 = std::abs(x) + std::abs(y) + sntracker->rcell; 
      double r2 = -r1;
      if (step1.trmap.rmax < r1) step1.trmap.rmax = r1;
      if (step1.trmap.rmin > r2) step1.trmap.rmin = r2;
    }
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.rmin = " << step1.trmap.rmin);
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.rmax = " << step1.trmap.rmax);
    double trange = step1.trmap.tmax - step1.trmap.tmin;
    double rrange = step1.trmap.rmax - step1.trmap.rmin;
    DT_LOG_DEBUG(cfg.logging, "  trange = " << trange);
    DT_LOG_DEBUG(cfg.logging, "  rrange = " << rrange);
    DT_LOG_DEBUG(cfg.logging, "Setting step1 map histogram...");
    step1.trmap.nt = cfg.step1_ntbins;
    step1.trmap.nr = cfg.step1_nrbins;
    step1.trmap.dt = trange / step1.trmap.nt;
    step1.trmap.dr = rrange / step1.trmap.nr;
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.nt = " << step1.trmap.nt);
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.nr = " << step1.trmap.nr);
    // Zeroes the map:
    step1.trmap.bins.assign(step1.trmap.nt * step1.trmap.nr, 0.0);
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.nbins = " << step1.trmap.bins.size());
    DT_LOG_DEBUG(cfg.logging, "Processing hits...");
    std::unique_ptr< std::ofstream> pfstep1bins;
    if (datatools::logger::is_debug(cfg.logging)) {
      pfstep1bins.reset(new std::ofstream(cfg.draw_prefix + "hits-lt.data"));
    }
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      if (pfstep1bins) {
        *pfstep1bins << "#@hit-lt#" << ihit << "@" << cid << '\n';
      }
      if (sntracker->has_tracker_conditions()) {
        // This hit is invalidated because the hit cell is considered dead:
        if (sntracker->get_tracker_conditions().has_dead_cell(cid)) {
          continue;
        }
      }
      if (!can_process_hit(ihit)) {
        DT_LOG_DEBUG(cfg.logging, "Hit #" << ihit << " cannot be processed");
        continue;
      }
      DT_LOG_DEBUG(cfg.logging, "Processing hit #" << ihit << " at cell " << cid);
      point cellCenter = sntracker->cell_position(cid);
      double x = cellCenter.x();
      double y = cellCenter.y();
      double r = h.drift_radius;
      // double errR = h.drift_radius;
      legendre_transform lt(x, y, r);
      // legendre_transform ltUp(x, y, r + errR);
      // legendre_transform ltDown(x, y, std::max(0.25 * errR, r - errR));
      for (int itheta = 0; itheta < (int) step1.trmap.nt; itheta++) {
        // Center of the bin #itheta:
        double theta = step1.trmap.tmin + (itheta + 0.5) * step1.trmap.dt;
        ltr_type ltr = lt.eval(theta);
        // ltr_type ltrUp = ltUp.eval(theta);
        // ltr_type ltrDown = ltDown.eval(theta);
        // r-concave:
        double r1 = ltr.concave;
        int ir1 = step1.trmap.r_to_index(r1);
        step1.trmap.fill_bin(itheta, ir1, map_type::no_gauss_kernel, 1.0);
        // double r1Up = ltrUp.concave;
        // int ir1Up = step1.trmap.r_to_index(r1Up);
        // step1.trmap.fill_bin(itheta, ir1Up, map_type::no_gauss_kernel, 0.5);
        // double r1Down = ltrDown.concave;
        // int ir1Down = step1.trmap.r_to_index(r1Down);
        // step1.trmap.fill_bin(itheta, ir1Down, map_type::no_gauss_kernel, 0.5);
        
        // r-convex:
        double r2 = ltr.convex;
        int ir2 = step1.trmap.r_to_index(r2);
        step1.trmap.fill_bin(itheta, ir2, map_type::no_gauss_kernel);
        // double r2Up = ltrUp.convex;
        // int ir2Up = step1.trmap.r_to_index(r2Up);
        // step1.trmap.fill_bin(itheta, ir2Up, map_type::no_gauss_kernel, 0.5);
        // double r2Down = ltrDown.convex;
        // int ir2Down = step1.trmap.r_to_index(r2Down);
        // step1.trmap.fill_bin(itheta, ir2Down, map_type::no_gauss_kernel, 0.5);
        if (pfstep1bins) {
          *pfstep1bins << theta << ' ' << r1 << ' ' << r2 << ' ' << ihit << '\n';
        }
      }
      if (pfstep1bins) {
        *pfstep1bins << '\n';
      }
    }
    step1.trmap.apply_threshold(cfg.step1_track_threshold);
    step1.trmap.build_sorted_bins();   
    cluster_finder clusterFinder(step1.trmap);
    clusterFinder.find(step1.clustering);
    DT_LOG_DEBUG(cfg.logging, "step1.clustering.clusters    = " << step1.clustering.clusters.size() << " clusters");
    DT_LOG_DEBUG(cfg.logging, "step1.clustering.unclustered = " << step1.clustering.unclustered.size());
    if (datatools::logger::is_debug(cfg.logging)) {
      step1.clustering.print(std::cerr, "[debug] ",step1.trmap);
    }
    return;
  }

  int lttc_algo::compute_neighbour_distance(int ifrom_, int jfrom_, int i_, int j_)
  {
    if ((ifrom_ == -1) || (jfrom_ == -1) || (i_ == -1) || (j_ == -1) ) {
      return std::numeric_limits<int>::max();
    }
    return std::abs(ifrom_ - i_) + std::abs(jfrom_ - j_);
  }

  void lttc_algo::cluster::clear()
  {
    id = -1;
    peak_bin_index = -1;
    peak_height = std::numeric_limits<double>::quiet_NaN();
    peak_tbin = -1;
    peak_rbin = -1;
    threshold_height = std::numeric_limits<double>::quiet_NaN();
    itmin = -1;
    itmax = -1;
    irmin = -1;
    irmax = -1;
    t_step = std::numeric_limits<double>::quiet_NaN();  
    t_min  = std::numeric_limits<double>::quiet_NaN(); 
    t_max  = std::numeric_limits<double>::quiet_NaN(); 
    r_min  = std::numeric_limits<double>::quiet_NaN(); 
    r_step = std::numeric_limits<double>::quiet_NaN();  
    r_max  = std::numeric_limits<double>::quiet_NaN(); 
    nbins_over_threshold = 0;
    hits_per_bin_ratio   = std::numeric_limits<double>::quiet_NaN(); 
    t_mean = std::numeric_limits<double>::quiet_NaN(); 
    r_mean = std::numeric_limits<double>::quiet_NaN(); 
    t_err  = std::numeric_limits<double>::quiet_NaN(); 
    r_err  = std::numeric_limits<double>::quiet_NaN(); 
    bins.clear();
    line_data.clear();
    return;
  }

  bool lttc_algo::cluster::has_bin(int ibin_) const
  {
    return bins.count(ibin_);
  }

  void lttc_algo::cluster::add_bin(int ibin_, int itbin_, int irbin_, double height_)
  {
    if (bins.size() == 0) {
      // First bin in this cluster:
      peak_bin_index = ibin_;
      peak_tbin = itbin_;
      peak_rbin = irbin_;
      peak_height = height_;
      threshold_height = 0.7 * peak_height;
      itmin = peak_tbin;
      itmax = peak_tbin;
      irmin = peak_rbin;
      irmax = peak_rbin;
      bins[ibin_] = height_;
    } else {
      if (height_ >= peak_height) {
        peak_bin_index = ibin_;
        peak_tbin = itbin_;
        peak_rbin = irbin_;
        peak_height = height_;
        threshold_height = 0.7 * peak_height;
      }
      if (itbin_ < itmin) itmin = itbin_;
      if (itbin_ > itmax) itmax = itbin_;
      if (irbin_ < irmin) irmin = irbin_;
      if (irbin_ > irmax) irmax = irbin_;
      bins[ibin_] = height_;
    }
    return;
  }
  
  size_t lttc_algo::cluster::size() const
  {
    return bins.size();
  }

  bool lttc_algo::cluster::empty() const
  {
    return bins.size() == 0;
  }

  bool lttc_algo::cluster::overlap(const cluster & other_) const
  {
    rectangle c(point(t_min, r_min), point(t_max, r_max));
    rectangle oc(point(other_.t_min, other_.r_min), point(other_.t_max, other_.r_max));
    if (!c.overlap(oc, 0.5 * other_.t_step, 0.5 * other_.r_step)) return false;
    return true;
  }

  // static   
  bool lttc_algo::cluster_overlap(const cluster & cl1_, const map_type & trmap1_,
                                  const cluster & cl2_, const map_type & trmap2_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    if (&cl1_ == &cl2_) return false;
    if (!cl1_.overlap(cl2_)) return false;
    size_t overlapping_nbins = 0;
    size_t nbins1 = cl1_.bins.size();
    DT_LOG_DEBUG(logging, "nbins1=" << nbins1);
    for (const auto & p1 : cl1_.bins) {
      bool overlapping_bin = false;
      int ibin1 = p1.first;
      // double hbin1 = p1.second;
      int tbin1 = -1; 
      int rbin1 = -1;
      trmap1_.tr_bin_index_to_t_r_indexes(ibin1, tbin1, rbin1);
      double t1 = trmap1_.index_to_t(tbin1) + 0.5 * trmap1_.dt;
      double r1 = trmap1_.index_to_r(rbin1) + 0.5 * trmap1_.dr;
      point  c1(t1, r1);
      DT_LOG_DEBUG(logging, "nbins2=" << cl2_.bins.size());
      for (const auto & p2 : cl2_.bins) {
        int ibin2 = p2.first;
        // double hbin2 = p2.second;
        int tbin2 = -1; 
        int rbin2 = -1;
        trmap2_.tr_bin_index_to_t_r_indexes(ibin2, tbin2, rbin2);
        double t2 = trmap2_.index_to_t(tbin2) + 0.5 * trmap2_.dt;
        double r2 = trmap2_.index_to_r(rbin2) + 0.5 * trmap2_.dr;
        rectangle cell2(point(t2, r2), trmap2_.dt, trmap2_.dr);
        if (cell2.contains(c1, 0.0)) {
          overlapping_bin = true;
        }
        if (overlapping_bin) {
          overlapping_nbins++;
          // break;
        }
      }
    } 
    return (overlapping_nbins * 1.0 / nbins1) > 0.7;
  }

  void lttc_algo::cluster::print(std::ostream & out_,
                                 const std::string & indent_) const
  {
    out_ << indent_ << "|-- id = " << id << '\n';
    out_ << indent_ << "|-- peak_bin_index = " << peak_bin_index << '\n';
    out_ << indent_ << "|-- peak_tbin = " << peak_tbin << '\n';
    out_ << indent_ << "|-- peak_rbin = " << peak_rbin << '\n';
    out_ << indent_ << "|-- peak_height = " << peak_height << '\n';
    out_ << indent_ << "|-- threshold_height = " << threshold_height << '\n';
    out_ << indent_ << "|-- [itmin:itmax] = [" << itmin << ':' << itmax << ']' << '\n';
    out_ << indent_ << "|-- [irmin:irmax] = [" << irmin << ':' << irmax << ']' << '\n';
    out_ << indent_ << "|-- nbins_over_threshold = " << nbins_over_threshold << '\n';
    out_ << indent_ << "|-- hits_per_bin_ratio = " << hits_per_bin_ratio << '\n';
    out_ << indent_ << "|-- t_min  = " << t_min << '\n';
    out_ << indent_ << "|-- t_max  = " << t_max << '\n';
    out_ << indent_ << "|-- t_step = " << t_step << '\n';
    out_ << indent_ << "|-- r_min  = " << r_min << '\n';
    out_ << indent_ << "|-- r_max  = " << r_max << '\n';
    out_ << indent_ << "|-- r_step = " << r_step << '\n';
    out_ << indent_ << "|-- t_mean +/- t_err = " << t_mean << " +/- " << t_err << '\n';
    out_ << indent_ << "|-- r_mean +/- r_err = " << r_mean << " +/- " << r_err << '\n';
    out_ << indent_ << "|-- #bins = " << bins.size() << '\n';
    out_ << indent_ << "`-- Line data: " << '\n';
    if (line_data.is_valid()) {
      line_data.print(out_, indent_ + "    ");
    }
    return;
  }

  void lttc_algo::cluster::draw(std::ostream & out_,
                               const map_type & trmap_) const
  {
    double hdt = 0.5 * trmap_.dt;
    double hdr = 0.5 * trmap_.dr;
    double dt = trmap_.dt;
    double dr = trmap_.dr;
    out_ << "#@cluster=#" << id << '\n';
    int tag = id;
    for (const auto & p : bins) {
      int    ibin  = p.first;
      // double hbin  = p.second;
      int    itbin = -1; 
      int    irbin = -1;
      trmap_.tr_bin_index_to_t_r_indexes(ibin, itbin, irbin);
      // Center of the bin:
      double t = trmap_.index_to_t(itbin) + hdt;
      double r = trmap_.index_to_r(irbin) + hdr;
      out_ << t - hdt << ' ' << r - hdr << ' ' << tag << '\n';
      out_ << t - hdt << ' ' << r + hdr << ' ' << tag << '\n';
      out_ << t + hdt << ' ' << r + hdr << ' ' << tag << '\n';
      out_ << t + hdt << ' ' << r - hdr << ' ' << tag << '\n';
      out_ << t - hdt << ' ' << r - hdr << ' ' << tag << '\n';
      out_ << '\n';
    }
    out_ << trmap_.index_to_t(itmin) << ' '
         << trmap_.index_to_r(irmin) << ' ' << tag << '\n';
    out_ << trmap_.index_to_t(itmin) << ' '
         << trmap_.index_to_r(irmax) + dr << ' ' << tag << '\n';
    out_ << trmap_.index_to_t(itmax) + dt << ' '
         << trmap_.index_to_r(irmax) + dr << ' ' << tag << '\n';
    out_ << trmap_.index_to_t(itmax) + dt << ' '
         << trmap_.index_to_r(irmin) << ' ' << tag << '\n';
    out_ << trmap_.index_to_t(itmin) << ' '
         << trmap_.index_to_r(irmin) << ' ' << tag << '\n';
    out_ << '\n';
    double teps = 0.15 * dt;
    double reps = 0.15 * dr;
    out_ << t_min + teps << ' '
         << r_min + reps << ' ' << tag << '\n';
    out_ << t_max - teps<< ' '
         << r_min + reps << ' ' << tag << '\n';
    out_ << t_max - teps<< ' '
         << r_max - reps << ' ' << tag << '\n';
    out_ << t_min + teps<< ' '
         << r_max - reps << ' ' << tag << '\n';
    out_ << t_min + teps << ' '
         << r_min + reps << ' ' << tag << '\n';
    out_ << '\n';
    teps = 0.075 * dt;
    reps = 0.075 * dr;
    out_ << t_min + teps << ' '
         << r_min + reps << ' ' << tag << '\n';
    out_ << t_max - teps<< ' '
         << r_min + reps << ' ' << tag << '\n';
    out_ << t_max - teps<< ' '
         << r_max - reps << ' ' << tag << '\n';
    out_ << t_min + teps<< ' '
         << r_max - reps << ' ' << tag << '\n';
    out_ << t_min + teps << ' '
         << r_min + reps << ' ' << tag << '\n';
    out_ << '\n';
    out_ << "#@end-of-cluster=#" << id << '\n';
    return;
  }

  void lttc_algo::clustering_data::draw(std::ostream & out_,
                                       const map_type & trmap_) const
  {
    double dt = trmap_.dt;
    double dr = trmap_.dr;
    int tag = -1;
    out_ << "#@unclustered" << '\n';
    for (auto ibin : unclustered) {
      int    itbin = -1; 
      int    irbin = -1;
      trmap_.tr_bin_index_to_t_r_indexes(ibin, itbin, irbin);
      double t = trmap_.index_to_t(itbin);
      double r = trmap_.index_to_r(irbin);
      out_ << t << ' ' << r << ' ' << tag << '\n';
      out_ << t << ' ' << r + dr << ' ' << tag << '\n';
      out_ << t + dt << ' ' << r + dr << ' ' << tag << '\n';
      out_ << t + dt << ' ' << r << ' ' << tag << '\n';
      out_ << t << ' ' << r << ' ' << tag << '\n';
      out_ << '\n';
    }
    if (unclustered.size() == 0) {
      out_ << 0.0 / 0.0 << ' ' << 0.0 / 0.0 << ' ' << tag << '\n';
      out_ << '\n';
    }
    out_ << "#@end-of-unclustered" << '\n';
    out_ << '\n';
    for (int ic = 0; ic < (int) clusters.size(); ic++) {
      clusters[ic].draw(out_, trmap_);
      out_ << '\n';
    }
    return;
  }

  void lttc_algo::clustering_data::clear()
  {
    clusters.clear();
    unclustered.clear();
    return;
  }

  void lttc_algo::clustering_data::print(std::ostream & out_,
                                        const std::string & indent_,
                                        const map_type & trmap_) const
  {
    // out_ << "|-- #ignored bins = " << unclustered.size() << '\n';
    
    out_ << indent_ << "|-- #unclustered bins = " << unclustered.size() << '\n';
    size_t nUnclusteredBins = 0;
    for (auto ibin : unclustered) {
      std::string tag = "|-- ";
      if (nUnclusteredBins + 1 == unclustered.size()) tag = "`-- ";
      out_ << indent_ << "|   " << tag << "unclustered #" << ibin
           << " with height=" << trmap_.bins[ibin]
           << '\n';
      nUnclusteredBins++;
    }
    
    out_ << indent_ << "|-- #clusters = " << clusters.size() << '\n';
    size_t ntotClusteredBins = 0;
    for (int ic = 0; ic < (int) clusters.size(); ic++) {
      std::ostringstream indent2;
      indent2 << indent_ << "|   ";
      std::string tag = "|-- ";
      std::string tag2 = "|   ";
      if (ic + 1 == (int) clusters.size()) {
        tag = "`-- ";
        tag2 = "    ";
      }
      ntotClusteredBins += clusters[ic].bins.size();
      out_ << indent_ << "|   " << tag << "cluster #" << ic << " : \n";
      indent2 << tag2;
      clusters[ic].print(out_, indent2.str());
      // #sbins=" << clusters[ic].sbins.size()
      // << " max_sbin=" << clusters[ic].max_sbin << " with max_height=" << clusters[ic].max_height << '\n';
    }
    out_ << indent_ << "`-- " << "#bins (total) = " << (ntotClusteredBins + unclustered.size()) << '\n';
   return;
  }

  lttc_algo::cluster_finder::cluster_finder(const map_type & trmap_, datatools::logger::priority logging_)
  {
    logging = logging_;
    trmap = &trmap_;
    return;
  }

  void lttc_algo::cluster_finder::find(clustering_data & clustering_)
  {
    dbscan_clusterizer::input_data inputDbScan;
    inputDbScan.trmap = trmap;
    dbscan_clusterizer::output_data outputDbScan;
    dbscan_clusterizer::config      cfgDbScan;
    cfgDbScan.logging = datatools::logger::PRIO_FATAL;
    if (datatools::logger::is_debug(logging)) cfgDbScan.logging = datatools::logger::PRIO_DEBUG;
    cfgDbScan.epsilon = 2.5;
    cfgDbScan.epsilon = 1.5;
    cfgDbScan.min_pts = 3;
    dbscan_clusterizer dbScan(inputDbScan, cfgDbScan);
    // dbScan.logging = datatools::logger::PRIO_DEBUG;
    dbScan.run(outputDbScan);
    {
      std::ofstream fout("dbscan_output.data");
      fout << "#@dbscan-output" << '\n';
      outputDbScan.draw(fout, inputDbScan);
      fout << '\n';
    }
    // Build the list of step-1 unclustered bins from DBSCAN output:
    DT_LOG_DEBUG(logging, "Collecting unclustered bins...");
    for (auto isbin : outputDbScan.unclustered) {
      int ibin = trmap->sorted_bins[isbin];
      // int tBin = -1; 
      // int rBin = -1;
      // trmap->tr_bin_index_to_t_r_indexes(iBin, tBin, rBin);
      clustering_.unclustered.insert(ibin);
    }
    
    // Build the list of step-1 clusters from DBSCAN output:
    DT_LOG_DEBUG(logging, "Collecting clusters...");
    for (int ic = 0; ic < (int) outputDbScan.clusters.size(); ic++) {
      DT_LOG_DEBUG(logging, "Add a new cluster...");
      {
        cluster newCluster;
        newCluster.id = outputDbScan.clusters[ic].id;
        clustering_.clusters.push_back(newCluster);
      }
      cluster & cl = clustering_.clusters.back();
      for (int isbin : outputDbScan.clusters[ic].sbins) {
        int iMapBin = trmap->sorted_bins[isbin];
        int thetaBin = -1; 
        int rBin = -1;
        trmap->tr_bin_index_to_t_r_indexes(iMapBin, thetaBin, rBin);
        double heightBin = trmap->bins[iMapBin];
        cl.add_bin(iMapBin, thetaBin, rBin, heightBin); 
      }
      // Number of bins over threshold:
      size_t binsOverThresholdCounter = 0;
      for (const auto & p : cl.bins) {
        // int iBin = p.first;
        double heightBin = p.second;
        if (heightBin >= cl.threshold_height) { 
          binsOverThresholdCounter++;
        }
      }
      cl.nbins_over_threshold = binsOverThresholdCounter;
      // Compute statistics associated to the cluster:
      cl.hits_per_bin_ratio = 0.0;
      cl.t_mean = std::numeric_limits<double>::quiet_NaN();
      cl.r_mean = std::numeric_limits<double>::quiet_NaN();
      cl.t_min =  std::numeric_limits<double>::infinity();
      cl.t_max = -std::numeric_limits<double>::infinity();
      cl.r_min =  std::numeric_limits<double>::infinity();
      cl.r_max = -std::numeric_limits<double>::infinity();
      cl.t_step = trmap->dt;
      cl.r_step = trmap->dr;
      double tsum = 0.0;
      double rsum = 0.0;
      double t2sum = 0.0;
      double r2sum = 0.0;
      for (const auto & p : cl.bins) {
        int iMapBin = p.first;
        double heightBin = p.second;
        if (heightBin < cl.threshold_height) continue;
        int thetaBin = -1; 
        int rBin = -1;
        trmap->tr_bin_index_to_t_r_indexes(iMapBin, thetaBin, rBin);
        // Center of the bin:
        double t = trmap->index_to_t(thetaBin) + 0.5 * trmap->dt;
        double r = trmap->index_to_r(rBin) + 0.5 * trmap->dr;
        if (t < cl.t_min) cl.t_min = t;
        if (t > cl.t_max) cl.t_max = t;
        if (r < cl.r_min) cl.r_min = r;
        if (r > cl.r_max) cl.r_max = r;
        tsum  += t;
        t2sum += t * t;
        rsum  += r;
        r2sum += r * r;
        cl.hits_per_bin_ratio += heightBin;
      }
      cl.t_min -= 0.5 * trmap->dt;
      cl.t_max += 0.5 * trmap->dt;
      cl.r_min -= 0.5 * trmap->dr;
      cl.r_max += 0.5 * trmap->dr;
      cl.hits_per_bin_ratio /= binsOverThresholdCounter;
      cl.t_mean = tsum / binsOverThresholdCounter;
      cl.r_mean = rsum / binsOverThresholdCounter;
      t2sum /= binsOverThresholdCounter;
      r2sum /= binsOverThresholdCounter;
      cl.t_err = std::sqrt(t2sum - cl.t_mean * cl.t_mean);
      if (cl.t_err < 0.5 * trmap->dt) cl.t_err = 0.5 * trmap->dt;
      cl.r_err = std::sqrt(r2sum - cl.r_mean * cl.r_mean);
      if (cl.r_err < 0.5 * trmap->dr) cl.r_err = 0.5 * trmap->dr;
      /*
      double ct = std::cos(cl.t_mean);
      double st = std::sin(cl.t_mean);
      cl.line.xt = cl.r_mean * ct;
      cl.line.yt = cl.r_mean * st;
      cl.line.xt_err =
        std::sqrt((cl.r_err * ct) * (cl.r_err * ct)
                  + (cl.r_mean * st * cl.t_err) * (cl.r_mean * st * cl.t_err));
      cl.line.yt_err =
        std::sqrt((cl.r_err * st) * (cl.r_err * st)
                  + (cl.r_mean * ct * cl.t_err) * (cl.r_mean * ct * cl.t_err));;

      double st_err = std::abs(ct) * cl.t_err;
      if (std::abs(st) < 0.5 * st_err) {
        cl.line.px = 0.0;
        cl.line.py = 1.0;
      } else {
        double p = ct / st;
        double p_err = cl.t_err / (st * st);
        double pn = std::sqrt(1 + p * p);
        cl.line.px = 1.0 / pn;
        cl.line.py = p / pn;
      }
      */
    }
    std::sort(clustering_.clusters.begin(),
              clustering_.clusters.end(),
              compare_by_ratio);
    
    return;
  }
 
  bool lttc_algo::compare_by_ratio(const cluster & c1_, const cluster & c2_)
  {
    if (c1_.hits_per_bin_ratio > c2_.hits_per_bin_ratio) return true;
    return false;
  }
 
  void lttc_algo::step2_run()
  {
    const step1_data & step1 = current_loop->step1;
    step2_data & step2 = current_loop->step2;
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    double dR = 0.0;
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const auto & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      if (sntracker->has_tracker_conditions()) {
        // This hit is invalidated because the hit cell is considered dead:
        if (sntracker->get_tracker_conditions().has_dead_cell(cid)) {
          continue;
        }
      }
      if (!can_process_hit(ihit)) {
        continue;
      }
      point cellCenter = sntracker->cell_position(cid);
      double R_err = h.drift_radius_err;
      DT_LOG_DEBUG(cfg.logging, "R_err = " << R_err);
      if (R_err > dR) {
        dR = R_err;
      }
    }
    DT_LOG_DEBUG(cfg.logging, "dR = " << dR);
    if (dR < cfg.step2_delta_r) dR = cfg.step2_delta_r;
    DT_LOG_DEBUG(cfg.logging, "dR = " << dR << " (safe)");
    size_t ncl = step1.clustering.clusters.size();
    if (ncl > cfg.step2_max_nlines) ncl = cfg.step2_max_nlines;
    DT_LOG_DEBUG(cfg.logging, "ncl = " << ncl);
    {
      step2.trmaps.reserve(ncl);
    }
    DT_LOG_DEBUG(cfg.logging, "Processing the first " << ncl << " clusters...");
    std::size_t nsubclusters = 0;
    for (int ic = 0; ic < (int) ncl; ic++) {
      const cluster & cl = step1.clustering.clusters[ic];
      DT_LOG_DEBUG(cfg.logging, "Processing cluster #" << ic << " with " << cl.bins.size() << " bins");
      DT_LOG_DEBUG(cfg.logging, "  hits_per_bin_ratio=" << cl.hits_per_bin_ratio);
      {
        trc_data dummy;
        step2.trmaps.push_back(dummy);
      }
      trc_data & trc2 = step2.trmaps.back();
      map_type & cltrmap = trc2.trmap;
      cltrmap.id = "step2-" + std::to_string(ic);
      cltrmap.dr = dR;
      cltrmap.dt = cfg.step2_delta_theta; 
      DT_LOG_DEBUG(cfg.logging, "  dt = " << cltrmap.dt);
      DT_LOG_DEBUG(cfg.logging, "  dr = " << cltrmap.dr);
      double trange = cl.t_max - cl.t_min + 0.5 * cltrmap.dt;
      double rrange = cl.r_max - cl.r_min + 0.5 * cltrmap.dr;
      cltrmap.nt = (size_t) (trange / cltrmap.dt);
      cltrmap.nr = (size_t) (rrange / cltrmap.dr);
      cltrmap.tmin = cl.t_min - 0.25 * cltrmap.dt;
      cltrmap.tmax = cltrmap.tmin + cltrmap.nt * cltrmap.dt;
      cltrmap.rmin = cl.r_min - 0.25 * cltrmap.dr;
      cltrmap.rmax = cltrmap.rmin + cltrmap.nr * cltrmap.dr;
      DT_LOG_DEBUG(cfg.logging, "  nt = " << cltrmap.nt);
      DT_LOG_DEBUG(cfg.logging, "  nr = " << cltrmap.nr);
      DT_LOG_DEBUG(cfg.logging, "  tmin:tmax = " << cltrmap.tmin << ':' << cltrmap.tmax);
      DT_LOG_DEBUG(cfg.logging, "  rmin:rmax = " << cltrmap.rmin << ':' << cltrmap.rmax);
      cltrmap.bins.assign(cltrmap.nt * cltrmap.nr, 0.0);
      DT_LOG_DEBUG(cfg.logging, "  cltrmap.bins.size = " << cltrmap.bins.size());
      int hcount = 0;
      DT_LOG_DEBUG(cfg.logging, "  Filling histogram for " << indata->hits.size() << " hits...");
      for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
        const auto & h = indata->hits[ihit];
        cell_id cid(h.side_id, h.layer_id, h.row_id);
        if (sntracker->has_tracker_conditions()) {
          // This hit is invalidated because the hit cell is considered dead:
          if (sntracker->get_tracker_conditions().has_dead_cell(cid)) {
            DT_LOG_DEBUG(cfg.logging, "    Dead cell hit #" << ihit << "...");
            continue;
          }
        }
        if (!can_process_hit(ihit)) {
          DT_LOG_DEBUG(cfg.logging, "    Do not processing hit #" << ihit << "...");
          continue;
        } else {
          DT_LOG_DEBUG(cfg.logging, "    Processing hit #" << ihit << "...");
        }
        point cellCenter = sntracker->cell_position(cid);
        double x = cellCenter.x();
        double y = cellCenter.y() ;
        double r = h.drift_radius;
        legendre_transform lt(x, y, r);
        for (int itheta = 0; itheta < (int) cltrmap.nt; itheta++) {
          double gt = cfg.step2_gauss_threshold;
          // Center of bin #itheta:
          double theta = cltrmap.tmin + (itheta + 0.5) * cltrmap.dt;
          ltr_type ltr = lt.eval(theta);
          // r-concave:
          double r1 = ltr.concave;
          int ir1 = cltrmap.r_to_index(r1);
          cltrmap.fill_bin(itheta, ir1, gt);
          // r-convex:
          double r2 = ltr.convex;
          int ir2 = cltrmap.r_to_index(r2);
          cltrmap.fill_bin(itheta, ir2, gt);        
        }
        hcount++;
      }
      DT_LOG_DEBUG(cfg.logging, "  Filling histogram... Done.");
      cltrmap.apply_threshold(cfg.step2_track_threshold);
      cltrmap.build_sorted_bins();
      DT_LOG_DEBUG(cfg.logging, "  Cluster t-r map : ");
      if (datatools::logger::is_debug(cfg.logging)) {
        cltrmap.print(std::cerr, "[debug]    ");
      }
      cluster_finder clusterFinder(cltrmap);
      clusterFinder.find(trc2.clustering);
      nsubclusters += trc2.clustering.clusters.size();
      DT_LOG_DEBUG(cfg.logging, "  step2 => clustering.clusters  = " << trc2.clustering.clusters.size() << " sub-clusters");
      DT_LOG_DEBUG(cfg.logging, "  step2 => clustering.unclustered = " << trc2.clustering.unclustered.size());
      DT_LOG_DEBUG(cfg.logging, "  End of cluster #" << ic);
    }
    DT_LOG_DEBUG(cfg.logging, "The first " << ncl << " clusters have been processed.");
    DT_LOG_DEBUG(cfg.logging, "" << nsubclusters << " sub-clusters have been found.");

    bool check_overlap = false;
    if (check_overlap) {
      DT_LOG_DEBUG(cfg.logging, "Checking cluster overlapping...");
      for (int i1 = 0; i1 < (int) step2.trmaps.size(); i1++) {
        DT_LOG_DEBUG(cfg.logging, "trmap #" << i1);
        const trc_data & trc1 = step2.trmaps[i1];
        const map_type & trmap1 = trc1.trmap;
        const clustering_data & cld1 = trc1.clustering;
        for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
          const cluster & cl1 = cld1.clusters[icl1];
          DT_LOG_DEBUG(cfg.logging, "cluster #" << icl1 << " : " << cl1.hits_per_bin_ratio);
          for (int i2 = 0; i2 < (int) step2.trmaps.size(); i2++) {
            if (i1 == i2) continue;
            const trc_data & trc2 = step2.trmaps[i2];
            const map_type & trmap2 = trc2.trmap;
            const clustering_data & cld2 = trc2.clustering;
            for (int icl2 = 0; icl2 < (int) cld2.clusters.size(); icl2++) {
              const cluster & cl2 = cld2.clusters[icl2];
              bool cl_over = cluster_overlap(cl1, trmap1, cl2, trmap2);
              if (cl_over) {
                DT_LOG_DEBUG(cfg.logging, "Cluster #" << icl1 << " from histo #" << i1
                             << " overlaps with cluster #" << icl1 << " from histo #" << i2);
              } 
            }     
          }
        } 
      }
    }
      
    // Collect all clusters from all clustering regions/histograms:
    {
      // And also recompute a general step-2 cluster ID then build the line parameters;
      int subClusterID=0;
      for (int i1 = 0; i1 < (int) step2.trmaps.size(); i1++) {
        trc_data & trc1 = step2.trmaps[i1];
        const map_type & trmap1 = trc1.trmap;
        clustering_data & cld1 = trc1.clustering;
        for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
          cluster & cl1 = cld1.clusters[icl1];
          cl1.id = subClusterID;          
          DT_LOG_DEBUG(cfg.logging, "Sub-cluster #" << icl1 << " from region " << i1 << " has ID=" << cl1.id);    
          cl1.line_data = line::make_from_theta_r(cl1.t_mean, cl1.r_mean);
          // double tPeak = trmap1.index_to_t(cl1.peak_tbin);
          // double rPeak = trmap1.index_to_r(cl1.peak_rbin);
          // cl1.line_data = line::make_from_theta_r(tPeak, rPeak);
          trc_ref tref;
          tref.ptrmap = &trmap1;
          tref.pcluster = &cl1;
          step2.clusters.push_back(tref);
          subClusterID++;
        }
      }
    }
    // Sort clusters per 'hits/bin ratio':
    std::sort(step2.clusters.begin(), step2.clusters.end());
    DT_LOG_DEBUG(cfg.logging, "Number of sorted step-2 clusters: " << step2.clusters.size());
    for (int i = 0; i < (int) step2.clusters.size(); i++) {
      const trc_ref & trcRef = step2.clusters[i];
      const cluster & cl = *trcRef.pcluster;
      if (datatools::logger::is_debug(cfg.logging)) {
        DT_LOG_DEBUG(cfg.logging, "Step-2 cluster #" << i << " : ");
        cl.print(std::cerr, "[debug] ");
      } 
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  bool lttc_algo::trc_ref::operator<(const trc_ref & tr_) const
  {
    return pcluster->hits_per_bin_ratio > tr_.pcluster->hits_per_bin_ratio;
  }
 
  void lttc_algo::step2_data::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- Clustering regions : " << trmaps.size() << '\n';
    for (int i1 = 0; i1 < (int) trmaps.size(); i1++) {
      const trc_data & trc1 = trmaps[i1];
      // const map_type & trmap1 = trc1.trmap;
      const clustering_data & cld1 = trc1.clustering;
      out_ << indent_ << "|   ";
      bool last_region = false;
      if ((i1 + 1) == (int) trmaps.size()) {
        out_ << "`-- ";
        last_region = true;
      } else {
        out_ << "|-- ";
      }
      out_ << "Clustering region: "
           << cld1.clusters.size() << " cluster(s)"
           << '\n';
      for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
        const cluster & cl1 = cld1.clusters[icl1];
        out_ << indent_ << "|   ";
        if (last_region) out_ << "    ";
        else out_ << "|   ";
        if ((icl1 + 1) == (int) cld1.clusters.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Cluster[" << icl1 << "] with ID=" << cl1.id << " : " << std::endl;
        // cl1.print(out_, indent_ + "|   ");
      }
    }
    out_ << indent_ << "`-- Total clusters : " << clusters.size() << '\n';
    for (int i1 = 0; i1 < (int) clusters.size(); i1++) {
      const cluster & cl1 = *clusters[i1].pcluster;
      out_ << indent_ << "    ";
      std::ostringstream indents;
      indents << indent_ << "    ";
      if ((i1 + 1) == (int) clusters.size()) {
        out_ << "`-- ";
        indents << "    ";
      } else {
        out_ << "|-- ";
        indents << "|   ";
      }
      out_ << "Cluster #" << i1 << " : "; //ratio=" << cl1.hits_per_bin_ratio;
      out_ << '\n';
      cl1.print(out_, indents.str()); //, *clusters[i1].ptrmap);
    }
    return;
  }

  void lttc_algo::step3_run()
  {
    // const step1_data & step1 = current_loop->step1;
    const step2_data & step2 = current_loop->step2;
    step3_data & step3 = current_loop->step3;
    step3.HC.resize(boost::extents[indata->hits.size()][step2.clusters.size()]);
    DT_LOG_DEBUG(cfg.logging, "#nhits=" << indata->hits.size());
    DT_LOG_DEBUG(cfg.logging, "#nclusters=" << step2.clusters.size());
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      bool dead_cell = false;
      bool can_process = true;
      if (sntracker->has_tracker_conditions()) {
        // This hit is invalidated because the hit cell is considered dead:
        if (sntracker->get_tracker_conditions().has_dead_cell(cid)) {
          dead_cell = true;
        }
      }
      if (!can_process_hit(ihit)) {
        can_process = false;
      }
      for (int icl = 0; icl < (int) step2.clusters.size(); icl++) {
        if (dead_cell) {
          step3.HC[ihit][icl].association_status = HCAS_NOT_VALID;
        } else {
          if (!can_process) {
            step3.HC[ihit][icl].association_status = HCAS_NOT_ASSOCIABLE;
          } else {
            step3.HC[ihit][icl].association_status = HCAS_NOT_ASSOCIATED;
          }
        }
      }
    }
    
    DT_LOG_DEBUG(cfg.logging, "Compute the hit-to-clusters HC association table...");
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      DT_LOG_DEBUG(cfg.logging, "===== ihit #" << ihit << " =====");
      const tracker_hit & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      // C :
      point cellCenter = sntracker->cell_position(cid);
      double r = h.drift_radius;
      double rErr = h.drift_radius_err;
      DT_LOG_DEBUG(cfg.logging, "r    = " << r / CLHEP::mm << " mm");
      DT_LOG_DEBUG(cfg.logging, "rErr = " << rErr / CLHEP::mm << " mm");
      for (int icl = 0; icl < (int) step2.clusters.size(); icl++) {
        if (step3.HC[ihit][icl].association_status != HCAS_NOT_ASSOCIATED) {
          continue;
        }
        DT_LOG_DEBUG(cfg.logging, "=> Check with cluster #" << icl);
        // const auto & trmap = *step2.clusters[icl].ptrmap;
        const auto & thisCluster = *step2.clusters[icl].pcluster;
        const auto & clusterLine = thisCluster.line_data;
        fitted_line clusterFitLine;
        clusterFitLine.theta = clusterLine.get_theta();
        clusterFitLine.r = clusterLine.get_r();
        clusterFitLine.theta_err = thisCluster.t_err;
        clusterFitLine.r_err = thisCluster.r_err;
        /*                         
         *                       fitproj :
         *    cell       \     :  _____  :<--fitline
         *   center        \   : /     \ : 
         *     C            |T :/   P   \:
         *  ---+---->-------+---:---+----:----
         *      r-dir       |  :\   .   /:
         *     <---------->/.  : `-----' :
         *        drift   / .  :    .    :
         *       radius     .       .
         *                  .<----->.
         *                  .residual (in chi2 sigma unit)
         */
        fitted_point fitLineProj;
        orthogonal_projection_to_fitted_line(clusterFitLine, cellCenter, fitLineProj);
        // P :
        point fitLineProjCenter(fitLineProj.x, fitLineProj.y);
        vector2 CP = fitLineProjCenter - cellCenter;
        vector2 radiusDir = CP.unit();
        // T :
        point hitTangentPoint = cellCenter + radiusDir * r;
        // vector2 CT = hitTangentPoint - cellCenter;
        double distToLine = (fitLineProjCenter - hitTangentPoint).mag();
        double nsigma = datatools::invalid_real();
        if (fitLineProj.inside(hitTangentPoint, 5.0)) {
          nsigma = 4.5;
          if (fitLineProj.inside(hitTangentPoint, 4.0)) {
            nsigma = 3.5;
            if (fitLineProj.inside(hitTangentPoint, 3.0)) {
              nsigma = 2.5;
              if (fitLineProj.inside(hitTangentPoint, 2.0)) {
                nsigma = 1.5;
                if (fitLineProj.inside(hitTangentPoint, 1.0)) {
                  nsigma = 0.5;
                }
              }
            }
          }
        }
        DT_LOG_DEBUG(cfg.logging, "  distToLine = " << distToLine / CLHEP::mm << " mm");
        DT_LOG_DEBUG(cfg.logging, "  nsigma = " << nsigma);
        double residual = datatools::invalid_real();
        double chi2 = datatools::invalid_real();
        bool match = false;
        if (datatools::is_valid(nsigma)) {
          residual = nsigma;
          if (CP < r) residual *= -1.0;
          chi2 = gsl_pow_2(residual);
          DT_LOG_DEBUG(cfg.logging, "  residual = " << residual);
          DT_LOG_DEBUG(cfg.logging, "  chi2     = " << chi2);
          DT_LOG_DEBUG(cfg.logging, "  checking hit/cluster match with cfg.step3_nsigma=" << cfg.step3_nsigma);
          if (std::abs(residual) <= cfg.step3_nsigma) {
            match = true;
          }
        }
        step3.HC[ihit][icl].distance = distToLine;
        step3.HC[ihit][icl].residual = residual;
        step3.HC[ihit][icl].chi2     = chi2;
        if (match) {
          DT_LOG_DEBUG(cfg.logging, "  -> hit #" << ihit << " @[" << h.side_id << "," << h.layer_id << "," << h.row_id << "] match cluster #" << icl);
          step3.HC[ihit][icl].association_status = HCAS_ASSOCIATED;
          step3.HC[ihit][icl].node = fitLineProj;
        }
      }
    }
    
    DT_LOG_DEBUG(cfg.logging, "Compute the clusters quality CQ array...");
    step3.CQ.reserve(step2.clusters.size());
    for (int icl = 0; icl < (int) step2.clusters.size(); icl++) {
      {
        cluster_quality_data newCqData;
        step3.CQ.push_back(newCqData);
      }
      cluster_quality_data & cqData = step3.CQ.back();
      cqData.nhits = 0;
      for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
        const hit_cluster_association_data & hca = step3.HC[ihit][icl];
        if (hca.association_status == HCAS_ASSOCIATED) {
          if (! datatools::is_valid(cqData.sumChi2)) {
            cqData.sumChi2 = 0.0;
          }
          if (! datatools::is_valid(cqData.noOutliersSumChi2)) {
            cqData.noOutliersSumChi2 = 0.0;
          }
          cqData.nhits++;
          cqData.ndof++;
          cqData.sumChi2 += hca.chi2;
          cqData.hits.insert(ihit);
          if (std::abs(hca.residual) > cfg.step3_nsigma_outliers) {
            cqData.outliers.insert(ihit);
          } else {
            cqData.noOutliersNdof++;
            cqData.noOutliersSumChi2 += hca.chi2;      
          }
        }
      }
      if (cqData.nhits) { 
        cqData.ndof = cqData.nhits;
        cqData.pvalue = gsl_cdf_chisq_Q(cqData.sumChi2, cqData.ndof);
        cqData.noOutliersPvalue = gsl_cdf_chisq_Q(cqData.noOutliersSumChi2, cqData.noOutliersNdof);
      }
   }
    return;
  }

  bool lttc_algo::can_process_hit(int ihit_) const
  {
    return removed_hits[ihit_] == -1;
  }
  
  void lttc_algo::trc_data::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- t-r 2D-histogram:" << '\n';
    trmap.print(out_, indent_ + "|   ");
    out_ << indent_ << "`-- Clustering:" << '\n';
    clustering.print(out_, indent_ + "    ", trmap);
    return;
  }

  void lttc_algo::trc_data::clear()
  {
    trmap.clear();
    clustering.clear();    
    return;
  }

  // static
  void lttc_algo::print_hc(const hit_cluster_association_array_type & hc_,
                          std::ostream & out_,
                          const std::string & indent_)
  {
    size_t nhits = hc_.shape()[0];
    size_t nclusters = hc_.shape()[1];
    out_ << indent_ << "Clusters : ";
    for (int icl = 0; icl < (int) nclusters; icl++) {
      out_ << std::setfill('0') << std::setw(4);
      out_ << std::right << icl << " ";     
    }
    out_ << '\n';
    for (int ihit = 0; ihit < (int) nhits; ihit++) {
      out_ << indent_;
      out_ << "Hit-";
      out_ << std::setfill('0') << std::setw(4);
      out_ << std::right << ihit << " : ";
      for (int icl = 0; icl < (int) nclusters; icl++) {
        if (hc_[ihit][icl].association_status == HCAS_NOT_VALID) out_ << '!';
        else if (hc_[ihit][icl].association_status == HCAS_NOT_ASSOCIABLE) out_ << '-';
        else if (hc_[ihit][icl].association_status == HCAS_NOT_ASSOCIATED) out_ << '0';
        else out_ << "1";
        out_ << "    ";
      }
      out_ << '\n';
    }
    return;
  }

  // static
  std::string lttc_algo::to_string(missing_cell_reason_type why_)
  {
    switch(why_) {
    case MCR_NO_TRIGGER : return std::string("no-trigger");
    case MCR_DEAD_CELL  : return std::string("dead-cell");
    case MCR_BUSY_CELL  : return std::string("busy-cell");
    case MCR_OTHER_TRACK : return std::string("other-track");
    default:
      break;
    }
    return std::string("undefined");
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const lttc_algo::cluster_missing_hit_data & cmh_)
  {
    out_ << "{cell-id=" << cmh_.cid << ",why=" << lttc_algo::to_string(cmh_.why) << '}';
    return out_;
  }
  
  void lttc_algo::cluster_quality_data::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "Nhits : " << nhits << '\n';
    out_ << indent_ << "|-- " << "Chi2 sum : " << sumChi2 << '\n';
    out_ << indent_ << "|-- " << "ndof     : " << ndof << '\n';
    out_ << indent_ << "|-- " << "P-value  : " << pvalue << '\n';
    out_ << indent_ << "|-- " << "Chi2 sum : " << noOutliersSumChi2 << " (no outliers)" << '\n';
    out_ << indent_ << "|-- " << "ndof     : " << noOutliersNdof << " (no outliers)" << '\n';
    out_ << indent_ << "|-- " << "P-value  : " << noOutliersPvalue << " (no outliers)" << '\n';
    out_ << indent_ << "|-- " << "Hits     : " << hits.size() << '\n';
    out_ << indent_ << "|   " << "`-- ";
    out_ << '{';
    {
      int hitCounter=0;
      for (auto ihit: hits) {
        out_ << ihit;
        hitCounter++;
        if (hitCounter != (int) hits.size()) out_ << ", ";
      }
    }
    out_ << '}';
    out_ << '\n';
    out_ << indent_ << "|-- " << "Outliers : " << outliers.size() << '\n';
    out_ << indent_ << "|   " << "`-- ";
    out_ << '{';
    {
      int outlierCounter=0;
      for (auto outlier: outliers) {
        out_ << outlier;
        outlierCounter++;
        if (outlierCounter != (int) outliers.size()) out_ << ", ";
      }
    }
    out_ << '}';
    out_ << '\n';
    out_ << indent_ << "`-- " << "Missing hits : " << missing_hits.size() << '\n';
    {
      for (int i = 0; i < (int) missing_hits.size(); i++) {
        out_ << indent_ << "    ";
        if ((i + 1) == (int) missing_hits.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "#" << i << " : " << missing_hits[i] << '\n';
      }
    }
    return;
  }

  // static
  void lttc_algo::print_cq(const cluster_quality_array_type & cq_,
                           std::ostream & out_,
                           const std::string & indent_)
  {
    size_t nclusters = cq_.size();
    for (int icl = 0; icl < (int) nclusters; icl++) {
      out_ << indent_;
      if ((icl + 1) == (int) nclusters) {
        out_ << "`-- ";
      } else {
        out_ << "|-- ";
      }
      out_ << "Cluster ";
      out_ << std::setfill('0') << std::setw(4);
      out_ << std::right << icl << std::setfill(' ');
      out_ << " : Chi2=" << std::setw(8) << std::setprecision(5) << cq_[icl].sumChi2
           << " ndof=" << cq_[icl].ndof
           << " pvalue=" << cq_[icl].pvalue
           << " outliers[" << cq_[icl].outliers.size() << "]={";
      int outlierCounter=0;
      for (auto outlier: cq_[icl].outliers) {
        out_ << outlier;
        outlierCounter++;
        if (outlierCounter != (int) cq_[icl].outliers.size()) out_ << ", ";
      }
      out_ << '}';
      out_ << '\n';
     }    
    return;
  }
  
  void lttc_algo::step3_data::print(std::ostream & out_, const std::string & indent_) const
  {
    // size_t nhits = HC.shape()[0];
    // size_t nclusters = HC.shape()[1];
    out_ << indent_ << "|-- HC : " << '\n';
    print_hc(HC, out_, indent_ + "|   ");
    out_ << indent_ << "|-- CQ : " << '\n';
    print_cq(CQ, out_, indent_ + "|   ");
    out_ << indent_ << "`-- end" << '\n';

    /*
    out_ << indent_ << "`-- Cluster-hit matches : " << ch_matches.size() << '\n';
    for (int ih = 0; ih < (int) ch_matches.size(); ih++) {
      const std::set<int> & mcls = ch_matches[ih];
      out_ << indent_ << "    ";
      std::ostringstream indents;
      indents << indent_ << "    ";
      if ((ih + 1) == (int) ch_matches.size()) {
        out_ << "`-- ";
        indents << "    ";
      } else {
        out_ << "|-- ";
        indents << "|   ";
      }
      out_ << "Cluster #" << ih << " : match with " << mcls.size() << " hits(s)" << '\n'; 
    }
    */
    
    return;
  }

  void lttc_algo::step2_data::clear()
  {
    trmaps.clear();
    clusters.clear();    
    return;
  }

  void lttc_algo::step3_data::clear()
  {
    HC = hit_cluster_association_array_type(boost::extents[1][1]);
    return;
  }

  // static
  std::string lttc_algo::to_string(hit_cluster_association_status_type hcas_)
  {
    switch(hcas_) {
    case HCAS_NOT_ASSOCIABLE  : return std::string("not-associable");
    case HCAS_NOT_ASSOCIATED  : return std::string("not-associated");
    case HCAS_ASSOCIATED : return std::string("associated");
    default: break;
    }
    return std::string("invalid");
  }
 
  // friend
  std::ostream & operator<<(std::ostream & out_, const lttc_algo::hit_cluster_association_data & hca_)
  {
    out_ << "{status=" << lttc_algo::to_string(hca_.association_status)
         << ",distance=" << hca_.distance / CLHEP::mm << " mm"
         << ",residual=" << hca_.residual
         << ",chi2=" << hca_.chi2
         << ",node=";
    if (datatools::is_valid(hca_.node.x)) {
      out_ << "yes";
    } else {
      out_ << "no";
    }
    out_ << '}';
    return out_;
  }

  void lttc_algo::loop_data::clear()
  {
    loop_id = -1;
    step1.clear();
    step2.clear();
    step3.clear();
    return;
  }
 
  void lttc_algo::loop_data::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- Loop ID : " << loop_id << "\n";
    out_ << indent_ << "|-- Step-1 : \n";
    step1.print(out_, indent_ + "|   ");
    out_ << indent_ << "|-- Step-2 : \n";
    step2.print(out_, indent_ + "|   ");
    out_ << indent_ << "|-- Step-3 : \n";
    step3.print(out_, indent_ + "|   ");
    out_ << indent_ << "`-- End.\n";
   return;
  }

  void lttc_algo::detect_degenerated_clusters()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    
    auto & HCD = outdata->hit_clustering;
    for (int i = 0; i < (int) HCD.clusters.size(); i++) {
      hit_cluster_data & hci = HCD.clusters[i];
      for (int j = i + 1; j < (int) HCD.clusters.size(); j++) {
        hit_cluster_data & hcj = HCD.clusters[j];
        if (hci.hits == hcj.hits) {
          DT_LOG_DEBUG(cfg.logging, "Clusters #" << i << " and #" << j << " are degenerated");
          hci.twins.insert(j);
          hcj.twins.insert(i);
          HCD.degenerated_clusters.insert(j);
          HCD.degenerated_clusters.insert(i);
        }
      }
    }
    
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::build_cluster_ordered_hits()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    auto & HCD = outdata->hit_clustering;
    for (int icl = 0; icl < (int) HCD.clusters.size(); icl++) {
      // Apply to each cluster:
      hit_cluster_data & hci = HCD.clusters[icl];
      hci.track_ordered_hits.clear();
      hci.track_ordered_hits.reserve(hci.hits.size());
      std::set<int> remainingHits = hci.hits;
      int iFirstHit = hci.end_hit_1;
      int iLastHit  = hci.end_hit_2;
      int iCurrentHit = iFirstHit;
      fitted_point currentNode = hci.hit_associations.find(iCurrentHit)->second.node;
      point currentVertex(currentNode.x, currentNode.y);
      while (remainingHits.size() > 1) {
        remainingHits.erase(iCurrentHit);
        hci.track_ordered_hits.push_back(iCurrentHit);
        // Search the closest hit along the fitted line
        int iNextHit = -1;
        double minDist = std::numeric_limits<double>::infinity();
        for (auto iHit : remainingHits) {
          const fitted_point & theNode = hci.hit_associations.find(iHit)->second.node;
          point theVertex(theNode.x, theNode.y);
          double nodeDist = (theVertex - currentVertex).mag();
          if (nodeDist < minDist) {
            minDist = nodeDist;
            iNextHit = iHit;
          }
        }
        iCurrentHit = iNextHit;
      }
      DT_THROW_IF(remainingHits.count(iLastHit) == 0, std::logic_error,
                  "End hit #2=" << iLastHit << " should be the last in the remaining set!");
      hci.track_ordered_hits.push_back(iLastHit);
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
 
  void lttc_algo::compute_cluster_missing_hits(hit_cluster_data & hit_cluster_)
  {
    // const step1_data & step1 = current_loop->step1;
    // const step2_data & step2 = current_loop->step2;
    const step3_data & step3 = current_loop->step3;
    auto workingHC = step3.HC;
    auto workingCQ = step3.CQ;
    std::vector<cluster_missing_hit_data> & missingHits = hit_cluster_.quality.missing_hits;
    missingHits.clear();

    // Line case:
    if (hit_cluster_.line_data.is_valid()) {
      int end_hit_1 = hit_cluster_.end_hit_1;
      int end_hit_2 = hit_cluster_.end_hit_2;
      const fitted_line & line_data = hit_cluster_.line_data;
      const tracker_hit_collection & hits = indata->hits;
      const tracker_hit & h1 = hits[end_hit_1];
      const tracker_hit & h2 = hits[end_hit_2];
      cell_id cid1(h1.side_id, h1.layer_id, h1.row_id);
      cell_id cid2(h2.side_id, h2.layer_id, h2.row_id);
      int minLayer = std::min(h1.layer_id, h2.layer_id);
      int maxLayer = std::max(h1.layer_id, h2.layer_id);
      int minRow = std::min(h1.row_id, h2.row_id);
      int maxRow = std::max(h1.row_id, h2.row_id);
      // point cellCenter1 = sntracker->cell_position(cid1);
      // point cellCenter2 = sntracker->cell_position(cid2);
      // const hit_cluster_association_data & hClAssoc1 = workingHC[end_hit_1][icl_];
      // const hit_cluster_association_data & hClAssoc2 = workingHC[end_hit_2][icl_];
      // const fitted_point & vertex1 = hClAssoc1.node;
      // const fitted_point & vertex2 = hClAssoc2.node;

      // Check for cells in a fiducial rectangle between the ending hits
      for (int iLayer = minLayer; iLayer <= maxLayer; iLayer++) {
        for (int iRow = minRow; iRow <= maxRow; iRow++) {
          cell_id cid(hit_cluster_.side, iLayer, iRow);
          // Skip the ending hits:
          if (cid == cid1) continue;
          if (cid == cid2) continue;
          point cellCenter = sntracker->cell_position(cid);
          double expectedDist = datatools::invalid_real();
          double expectedDistErr = datatools::invalid_real();
          distance_to_fitted_line(line_data, cellCenter, expectedDist, expectedDistErr);
          if (expectedDist > sntracker->rcell) continue;
          // This is a candidate cell:
          if (sntracker->has_tracker_conditions()) {
            const tracker_conditions & trkConds = sntracker->get_tracker_conditions();
            if (trkConds.has_dead_cell(cid)) {
              cluster_missing_hit_data cmh;
              cmh.cid = cid;
              cmh.why = MCR_DEAD_CELL;
              missingHits.push_back(cmh);
            } else if (hit_cells.count(cid) > 0) {
              int iHitWithThisCell = hit_cells.find(cid)->second;
              const tracker_hit & hitWithThisCell = hits[iHitWithThisCell];
              double hitWithThisCellDriftRadius = hitWithThisCell.drift_radius;
              double hitWithThisCellDriftRadiusErr = hitWithThisCell.drift_radius_err;
              if (hit_cluster_.hits.count(iHitWithThisCell) == 0) {
                cluster_missing_hit_data cmh;
                cmh.cid = cid;
                if ((expectedDist + expectedDistErr) > (hitWithThisCellDriftRadius - hitWithThisCellDriftRadiusErr)) {
                  // The missing hit is due to another hit with a shorter drift radius:
                  cmh.why = MCR_OTHER_TRACK;
                } else {
                  // The missing hit is due to the lack of a Geiger avalanche (no trigger):
                  cmh.why = MCR_NO_TRIGGER;
                }
                missingHits.push_back(cmh);        
              }
            } else if (hit_cells.count(cid) == 0) {
              cluster_missing_hit_data cmh;
              cmh.cid = cid;
              cmh.why = MCR_NO_TRIGGER;
              missingHits.push_back(cmh);          
            }
          }     
        }
      }
    } else {
      // FUTURE: circle case
      DT_LOG_DEBUG(cfg.logging, "No missing cells can be computed!");
    }
    return;
  }

  void lttc_algo::update_hit_clustering()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    // const step1_data & step1 = current_loop->step1;
    const step2_data & step2 = current_loop->step2;
    const step3_data & step3 = current_loop->step3;
    auto workingHC = step3.HC;
    auto workingCQ = step3.CQ;
    size_t nhits = workingHC.shape()[0];
    size_t nclusters = workingHC.shape()[1];
    std::vector<size_t> cluster_stats;
    std::vector<uint32_t> cluster_status;
    cluster_stats.assign(nclusters, 0);
    cluster_status.assign(nclusters, 0);
    for (int icl = 0; icl < (int) nclusters; icl++) {
      for (int ihit = 0; ihit < (int) nhits; ihit++) {
        hit_cluster_association_data & hcadi = workingHC[ihit][icl];
        if (hcadi.association_status == HCAS_NOT_VALID) continue;
        if (hcadi.association_status == HCAS_NOT_ASSOCIABLE) continue;
        if (hcadi.association_status == HCAS_ASSOCIATED) {
          cluster_stats[icl]++;
          // XXX do not do that ! let hits associated to several clusters
          // for (int icl2 = icl+1; icl2 <  (int) nclusters; icl2++) {
          //   workingHC[ihit][icl2].association_status = HCAS_NOT_ASSOCIATED;
          // }
        }
      }
      DT_LOG_DEBUG(cfg.logging, "cluster #" << icl << " has " << cluster_stats[icl] << " hits");
      if (cluster_stats[icl] < 3) {
        cluster_status[icl] |= hit_clustering_data::CQ_CRIT_STAT;
        DT_LOG_DEBUG(cfg.logging, " -> cluster #" << icl << " has critical stat");
      } else {
        // double hpbr = step2.clusters[icl].pcluster->hits_per_bin_ratio;
        // double nhlow = hpbr - std::sqrt(hpbr);
        // if (cluster_stats[icl] < nhlow) {
        //   cluster_status[icl] |= hit_clustering_data::CQ_LOW_STAT;
        //   std::cerr << "[debug] " << "cluster #" << icl << " has low stat" << '\n';
        // }
      }
    }

    if (datatools::logger::is_debug(cfg.logging)) {
      std::cerr << "[debug] Clusters status:" << '\n';
      for (int icl = 0; icl < (int) nclusters; icl++) {
        std::cerr << "[debug] Cluster #" << icl << " : status=" << cluster_status[icl] << '\n';
      }
      std::cerr << '\n';
    }
    // if (datatools::logger::is_debug(cfg.logging)) {
    //   print_hc(workingHC, std::cerr, "[devel] Stage 1> ");
    //   std::cerr << "Wait..." << std::endl;
    //   std::string resp;
    //   std::getline(std::cin, resp);
    // }
             
    // Create hit clusters:
    DT_LOG_DEBUG(cfg.logging, "Creating hit clusters...");
    for (int icl = 0; icl < (int) nclusters; icl++) {
      const cluster * pcluster = step2.clusters[icl].pcluster;
      if (cluster_status[icl] & hit_clustering_data::CQ_CRIT_STAT) {
        DT_LOG_DEBUG(cfg.logging, "" << "cluster #" << icl << " has critical stat");
        continue;
      }
      {
        hit_cluster_data hc;
        outdata->hit_clustering.clusters.push_back(hc);
      }
      hit_cluster_data & new_cluster = outdata->hit_clustering.clusters.back();
      new_cluster.id = outdata->hit_clustering.clusters.size() - 1;
      new_cluster.flags = 0;
      new_cluster.quality = workingCQ[icl];
      new_cluster.line_data.theta = pcluster->line_data.get_theta();
      new_cluster.line_data.r = pcluster->line_data.get_r();
      new_cluster.line_data.theta_err = pcluster->t_err;
      new_cluster.line_data.r_err = pcluster->r_err;
      for (int ihit = 0; ihit < (int) nhits; ihit++) {
        hit_cluster_association_data & hcadi = workingHC[ihit][icl];
        if (hcadi.association_status == HCAS_NOT_VALID) continue;
        if (hcadi.association_status == HCAS_NOT_ASSOCIABLE) continue;
        if (hcadi.association_status == HCAS_ASSOCIATED) {
          new_cluster.add_hit(ihit, *this);
          new_cluster.hit_associations[ihit] = hcadi;
          outdata->hit_clustering.unclustered_hits.erase(ihit);
          // XXX questionable
          // Tag clusterized hits to be removed : 
          // removed_hits[ihit] = new_cluster.id;
        } 
      }
      compute_cluster_missing_hits(new_cluster);
    }
    detect_degenerated_clusters();
    associate_unclustered_hits();
    build_cluster_ordered_hits();
    
    DT_LOG_DEBUG(cfg.logging, "Clusters are done : ");
    outdata->hit_clustering.print(std::cerr, "[debug] ");

    // if (datatools::logger::is_debug(cfg.logging)) {
    //   print_hc(workingHC, std::cerr, "[debug] Stage 2> ");
    //   std::cerr << "Wait..." << std::endl;
    //   std::string resp;
    //   std::getline(std::cin, resp);
    // }
    
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::enrich_clusters()
  {
    // for (int icl = 0; icl < (int) hit_clustering.clusters.size(); icl++) {
    //   const hit_cluster_data & hcl = hit_clustering.clusters[icl];
    //   std::set<int> hits = hcl.hits;
    //   
    // }   
    return;
  }

  void lttc_algo::associate_unclustered_hits()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    /*
    for (int ihit :  outdata->hit_clustering.unclustered_hits) {
      int    asso_icl = -1;
      double asso_dist = +std::numeric_limits<double>::infinity();
      const tracker_hit & h = indata->hits[ihit];
      point cellPos = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      for (int icl = 0; icl < (int)  outdata->hit_clustering.clusters.size(); icl++) {
        hit_cluster_data & hcl =  outdata->hit_clustering.clusters[icl];
        // const line & cline = hcl.line_data;
        line cline = line::make_from_theta_r(hcl.line_data.theta, hcl.line_data.r);
        double rdist = cline.distance(cellPos);
        // bool associated = false;
        if (rdist < (h.drift_radius + 3 * h.drift_radius_err))  {
          // The hit is on the cluster's line path:
          int end_hit_1  = hcl.end_hit_1;
          int end_hit_2  = hcl.end_hit_2;
          const tracker_hit & h1 = indata->hits[end_hit_1]; 
          const tracker_hit & h2 = indata->hits[end_hit_2];
          point cellPos1 = sntracker->cell_position(h1.side_id, h1.layer_id, h1.row_id);
          point cellPos2 = sntracker->cell_position(h2.side_id, h2.layer_id, h2.row_id);
          double d1 = std::hypot(cellPos.x() - cellPos1.x(), cellPos.y() - cellPos1.y());
          double d2 = std::hypot(cellPos.x() - cellPos2.x(), cellPos.y() - cellPos2.y());
          double dist = std::min(d1, d2);
          if (dist < 7. * sntracker->rcell and dist < asso_dist) {
            // The hit is near one of the extreme hits:
            asso_icl = icl;
            asso_dist = dist;
          }
        }
      }
      if (asso_icl != -1) {
        DT_LOG_DEBUG(cfg.logging, "associate hit #" << ihit << " to cluster " << asso_icl);
        outdata->hit_clustering.clusters[asso_icl].add_hit(ihit, *this);
      }
    }
    */
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::terminate_loops()
  {
    return;
  }
  
  double lttc_algo::cluster_quality_data::get_effective_pvalue() const
  {
    double pv = pvalue;
    int nb_hits     = (int) hits.size();
    int nb_outliers = (int) outliers.size();
    if (nb_outliers and (nb_hits - nb_outliers) >= 3) {
      // Give a chance for a better p-value without outliers:
      pv = noOutliersPvalue;
    }
    return pv;
  }

  unsigned int lttc_algo::cluster_quality_data::get_effective_number_of_missing_hits() const
  {
    unsigned int enomh = 0;
    for (int i = 0; i < (int) missing_hits.size(); i++) {
      if (missing_hits[i].why == MCR_NO_TRIGGER) {
        // Count only true missing hit (other than a good reason)
        enomh++;
      }
    }
    return enomh;
  }

  void lttc_algo::hit_cluster_data::build_track_path(track_path_data & track_path_) const
  {
    DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Cluster ID=" << this->id);      
    DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "  => track ordered hits" << track_ordered_hits.size());  
    track_path_.vertexes.clear();
    track_path_.vertexes.reserve(hits.size());
    for (int i = 0; i < (int) track_ordered_hits.size(); i++) {
      track_path_vertex tpv;
      tpv.hit = track_ordered_hits[i];
      tpv.node = hit_associations.find(track_ordered_hits[i])->second.node;
      track_path_.vertexes.push_back(tpv);
    }
    return;
  }

  void lttc_algo::track_path_data::draw(std::ostream & out_, int tag_) const
  {
    for (int i = 0; i < (int) vertexes.size(); i++) {
      vertexes[i].node.draw(out_, 1.0, false, tag_);
    }
    for (int i = 0; i < (int) vertexes.size(); i++) {
      out_ << vertexes[i].node.x << ' ' << vertexes[i].node.y << ' ' << tag_ << '\n';
    }
    out_ << '\n';
    return;
  }

  // fitted_point lttc_algo::hit_cluster_data::get_vertex_1() const
  // {
  //   fitted_point fp;
  //   if (end_hit_1 >= 0) {
  //     const hit_cluster_association_data & hca = hit_associations.find(end_hit_1)->second;
  //   }
  //   return fp;
  // }
  
  // fitted_point lttc_algo::hit_cluster_data::get_vertex_2() const
  // {
  // }
    
  void lttc_algo::hit_cluster_data::add_hit(int ihit_, const lttc_algo & algo_)
  {
    const tracker_hit & h = algo_.indata->hits[ihit_];
    if (hits.size() == 0) {
      side = h.side_id;
      if (h.delayed) {
        delayed = true;
      }
      end_hit_1 = ihit_;
      end_hit_2 = ihit_;
    } else {
      DT_THROW_IF(h.delayed != delayed,
                  std::logic_error,
                  "Cannot add hit #" << ihit_ << " with unmatching delayed flag in cluster ID=" << id << '!');
      DT_THROW_IF(h.side_id != side,
                  std::logic_error,
                  "Cannot add hit #" << ihit_ << " with unmatching side " << h.side_id << " in cluster ID=" << id << " at side " << side << '!');
      const tracker_hit & h1 = algo_.indata->hits[end_hit_1]; 
      const tracker_hit & h2 = algo_.indata->hits[end_hit_2];
      point cellPos1 = algo_.sntracker->cell_position(h1.side_id, h1.layer_id, h1.row_id);
      point cellPos2 = algo_.sntracker->cell_position(h2.side_id, h2.layer_id, h2.row_id);
      point cellPos  = algo_.sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      double d12 = std::hypot(cellPos2.x() - cellPos1.x(), cellPos2.y() - cellPos1.y());
      double d1 = std::hypot(cellPos.x() - cellPos1.x(), cellPos.y() - cellPos1.y());
      double d2 = std::hypot(cellPos.x() - cellPos2.x(), cellPos.y() - cellPos2.y());
      if (d1 > d12 and d1 >= d2) {
        end_hit_2 = ihit_;
      } else if (d2 > d12 and d2 >= d1) {
        end_hit_1 = ihit_;
      }
    }
    hits.insert(ihit_);
    return;
  }

  void lttc_algo::hit_cluster_data::print(std::ostream & out_,
                                          const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "ID : " << id << '\n';
    out_ << indent_ << "|-- " << "Side : " << side << '\n';
    out_ << indent_ << "|-- " << "Delayed : " << std::boolalpha << delayed << '\n';
    out_ << indent_ << "|-- " << "Flags : " << flags << '\n';
    out_ << indent_ << "|-- " << "Hits : " << hits.size();
    if (hits.size()) {
      out_ << " {";
      int hcount=0;
      for (auto ihit : hits) {
        out_ << ihit;
        if ((hcount + 1) == (int) hits.size()) out_ << '}';
        else out_ << ',';
        hcount++;
      }
    }
    out_ << '\n';
    out_ << indent_ << "|-- " << "Quality : \n";
    quality.print(out_, indent_ + "|   ");
    
    out_ << indent_ << "|-- " << "Hit associations : " << hit_associations.size() << '\n';
    {
      int haCount = 0;
      for (const auto & ha : hit_associations) {
        out_ << indent_ << "|   ";
        if ((haCount+1) == (int) hit_associations.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Hit #" << ha.first << " : " << ha.second << '\n';
        haCount++;
      }
    }
    
    out_ << indent_ << "|-- " << "End hit 1 : " << end_hit_1 << '\n';
    out_ << indent_ << "|-- " << "End hit 2 : " << end_hit_2 << '\n';
    out_ << indent_ << "|-- " << "Twins : " << twins.size() << '\n';
    {
      int twinCount = 0;
      for (int itwin : twins) {
        out_ << indent_ << "|   ";
        if ((twinCount+1) == (int) twins.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Twin cluster #" << itwin << '\n';
        twinCount++;
      }
    }

    out_ << indent_ << "|-- " << "Track ordered hits : " << track_ordered_hits.size() << '\n';
    {
      int tohCount = 0;
      for (int itoh : track_ordered_hits) {
        out_ << indent_ << "|   ";
        if ((tohCount + 1) == (int) track_ordered_hits.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Ordered hit #" << itoh << '\n';
        tohCount++;
      }
    }
 
    if (line_data.is_valid()) {
      out_ << indent_ << "`-- " << "Line data : " << '\n';
      line_data.print(out_, indent_ + "    ");
    } else {
      out_ << indent_ << "`-- " << "No track fit pattern" << '\n';
    }
    return;
  }
     
  void lttc_algo::hit_clustering_data::print(std::ostream & out_,
                                            const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "Clusters of hits: " << clusters.size() << '\n';
    for (int icl = 0; icl < (int) clusters.size(); icl++) {
      const hit_cluster_data & hcl = clusters[icl];
      std::ostringstream sindent2;
      sindent2 << indent_ << "|   ";
      out_ << indent_ << "|   ";
      if ((icl + 1) == (int) clusters.size()) {
        out_ << "`-- ";
        sindent2 << "    ";
      } else {
        out_ << "|-- ";
        sindent2 << "|   ";
      }
      out_ << "Cluster of hit #" << icl << '\n' ;      
      hcl.print(out_, sindent2.str());
    }

    out_ << indent_ << "|-- " << "Degenerated clusters: " << degenerated_clusters.size() << '\n';
    {
      int degCount = 0;
      for (int ideg : degenerated_clusters) {
        out_ << indent_ << "|   ";
        if ((degCount+1) == (int) degenerated_clusters.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Degenerated cluster #" << ideg << '\n';
        degCount++;
      }
    }
     
    out_ << indent_ << "`-- " << "Unclustered hits: " << unclustered_hits.size() << '\n';
    {
      int hcount = 0;
      for (const auto & ihit : unclustered_hits) {
        out_ << indent_ << "    ";
        if ((hcount + 1) == (int) unclustered_hits.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Hit #" << ihit << '\n' ;      
        hcount++;
      }
    }
    return;
  }

  void lttc_algo::hit_clustering_data::clear()
  {
    clusters.clear();
    unclustered_hits.clear();
    return;
  }
     
  void lttc_algo::hit_clustering_data::draw(std::ostream & out_, const lttc_algo & algo_) const
  {
    double ercell = algo_.sntracker->rcell - 2.0 * CLHEP::mm;
    double ercell2 = algo_.sntracker->rcell - 8.0 * CLHEP::mm;
    out_ << "#@unclustered_hits=" << unclustered_hits.size() << '\n';
    if (unclustered_hits.size()) {
      for (const auto & ihit : unclustered_hits) {
        const tracker_hit & h = algo_.indata->hits[ihit];
        point cellPos = algo_.sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() + ercell << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() + ercell << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() - ercell << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << '\n';
        out_ << '\n';     
      }
    } else {
      out_ << 0.0/0.0 << ' ' << 0.0/0.0 << '\n';
    }
    out_ << '\n';
    out_ << "#@nb_hit_clusters=" << clusters.size() << '\n';
    for (int icl = 0; icl < (int) clusters.size(); icl++) {
      const hit_cluster_data & hcl = clusters[icl];
      out_ << "#@hit_cluster#=" << icl << '\n';
      for (auto ihit : hcl.hits) {
        const tracker_hit & h = algo_.indata->hits[ihit];
        cell_id cid(h.side_id, h.layer_id, h.row_id);
        out_ << "#@=>hit_cell_id#=" << cid << '\n';
        point cellPos = algo_.sntracker->cell_position(cid);
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() + ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() + ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() - ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << 1+icl << '\n';
        out_ << '\n';
        if (ihit == hcl.end_hit_1 || ihit == hcl.end_hit_2) {
          out_ << cellPos.x() - ercell2 << ' ' << cellPos.y() - ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() - ercell2 << ' ' << cellPos.y() + ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() + ercell2 << ' ' << cellPos.y() + ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() + ercell2 << ' ' << cellPos.y() - ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() - ercell2 << ' ' << cellPos.y() - ercell2 << ' ' << 1+icl << '\n';
          out_ << '\n';
        }
        const hit_cluster_association_data & hcai = hcl.hit_associations.find(ihit)->second;
        const fitted_point & nodei = hcai.node;
        if (nodei.is_valid()) {
          out_ << "#@=>hit_node" << '\n';
          nodei.draw(out_, 1.0, false, 1+icl);
          nodei.draw(out_, 2.0, true, 1+icl);
        }
      }
      {
        const fitted_line & ld = hcl.line_data;
        line ld2 = line::make_from_theta_r(ld.theta, ld.r);
        polyline ld_samples;
        double t1 = -1500.0;
        double t2 = +1500.0;
        ld2.generate_samples(t1, t2, ld_samples);
        draw_polyline(out_, ld_samples, 1+icl);
        // out_ << '\n';
      }
      out_ << '\n';     
    }
    return;
  }
    
  void lttc_algo::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "Removed hits : " << '\n';
    for (int ihit = 0; ihit < (int) removed_hits.size(); ihit++) {
      if ((ihit + 1) == (int) removed_hits.size()) {
        out_ << "`-- ";
      } else {
        out_ << "|-- ";
      }
      out_ << "Hit #" << ihit << " : " << removed_hits[ihit] << '\n';
    }
    out_ << indent_ << "|-- " << "Loop counter : " << loop_counter << '\n';
    out_ << indent_ << "`-- " << "Hit clustering : ";
    out_ << outdata->hit_clustering.clusters.size() << " cluster(s)";
    out_ << '\n';
    outdata->hit_clustering.print(out_, indent_ + "    ");
    return;
  }

  void lttc_algo::init_loops()
  {
    // All hits marked as unclustered:
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
       outdata->hit_clustering.unclustered_hits.insert(ihit);
    }
    return;
  }

  void lttc_algo::run_loop()
  {
    // Initialize a new loop:
    {
      loop_data new_loop;
      new_loop.loop_id = loop_counter;
      loops.push_back(new_loop);
    }
    current_loop = &loops.back();

    DT_LOG_DEBUG(cfg.logging, " ==================== Step-1 ====================");
    step1_run();
    DT_LOG_DEBUG(cfg.logging, " ==================== Step-1 Done ====================");
 
    DT_LOG_DEBUG(cfg.logging, " ==================== Step-2 ====================");
    step2_run();
    DT_LOG_DEBUG(cfg.logging, " ==================== Step-2 Done ====================");

    DT_LOG_DEBUG(cfg.logging, " ==================== Step-3 ====================");
    step3_run();
    DT_LOG_DEBUG(cfg.logging, " ==================== Step-3 Done ====================");
 
    update_hit_clustering();
    
    if (datatools::logger::is_debug(cfg.logging)) {
      current_loop->print(std::cerr, "[debug] ");
      std::cerr << "[debug] Hit clustering : \n";
      outdata->hit_clustering.print(std::cerr, "[debug] ");
    }
    
    // End of the current loop:
    current_loop = nullptr;
    loop_counter++;
    return;
  }

  void lttc_algo::_set_input_data_(const input_data & indata_)
  {
    DT_LOG_DEBUG(cfg.logging, "Set input data...");
    indata = &indata_;
    hit_cells.clear();
    for (int ihit= 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      hit_cells[cid] = ihit;
    }
    removed_hits.clear();
    removed_hits.assign(indata->hits.size(), -1);
    return;
  }

  void lttc_algo::_build_output_data_()
  {
    DT_LOG_DEBUG(cfg.logging, "Building output data...");
    return;
  }

  void lttc_algo::_clear_processing_data_()
  {
    DT_LOG_DEBUG(cfg.logging, "Clear internal processing data...");
    loop_counter = 0;
    current_loop = nullptr;
    loops.clear();
    return;
  }
 
  void lttc_algo::process(const input_data & indata_, output_data & outdata_)
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    this->_clear_processing_data_();
    this->_set_input_data_(indata_);
    outdata = &outdata_;

    this->init_loops();
    int iloop = 0;
    int max_nloops = 1;
    bool go_loops = true;

    while (go_loops) {
      std::string fprefix = cfg.draw_prefix + "loop_" + std::to_string(iloop) + "-";
      size_t old_nclusters = outdata->hit_clustering.clusters.size();
     
      std::clog << "Running loop... \n";
      this->run_loop();
      std::clog << "Done.\n";

      size_t new_nclusters = outdata->hit_clustering.clusters.size();
      if ((new_nclusters == old_nclusters) or (outdata->hit_clustering.unclustered_hits.size() < 3)) {
        go_loops = false;
      }

      const loop_data & last_loop = this->loops.back();
      bool print_it = false;

      if (print_it) {
        std::clog << "Step-1: Map:\n";
        last_loop.step1.trmap.print(std::clog);
      }

      bool do_loop_draw = false;
      if (cfg.draw) {
        do_loop_draw = true;
        if (iloop < cfg.loop_draw_nmin) {
          do_loop_draw = false;
        } else if (cfg.loop_draw_nmax >= 0) {
          if (iloop > cfg.loop_draw_nmax) {
            do_loop_draw = false;
          }
        }
      }
      
      if (do_loop_draw) {
        std::ofstream fstep1bins(fprefix + "step1-bins.data");   
        fstep1bins << "#@step1-bins=" << last_loop.step1.trmap.bins.size() << "\n";
        last_loop.step1.trmap.draw_bins(fstep1bins);
        fstep1bins << '\n';
      }

      if (print_it) {
        for (int iSbin = 0; iSbin < (int) last_loop.step1.trmap.sorted_bins.size(); iSbin++) {
          int idxBin = last_loop.step1.trmap.sorted_bins[iSbin];
          int iBin, jBin;
          last_loop.step1.trmap.tr_bin_index_to_t_r_indexes(idxBin, iBin, jBin);
          std::clog << "Sorted bin #" << iSbin << " => bin #" << idxBin
                    << " @(" << iBin << ',' << jBin << ") =>height="
                    << last_loop.step1.trmap.bins[idxBin] << std::endl;
          if (iSbin > 10) {
            std::clog << "..." << '\n';
            break;
          }
        }
      }
  
      if (print_it) {
        std::clog << "\nStep-1 : Clustering data:\n";
        last_loop.step1.clustering.print(std::clog, "", last_loop.step1.trmap);
        std::clog << '\n';
      }
  
      if (do_loop_draw) {
        std::ofstream fstep1sbins(fprefix + "step1-sorted-bins.data");   
        fstep1sbins << "#@step1-sorted-bins=" << last_loop.step1.trmap.sorted_bins.size() << "\n";
        last_loop.step1.trmap.draw_sorted_bins(fstep1sbins, 0.0);
        fstep1sbins << '\n';

        std::ofstream fstep1clustering(fprefix + "step1-clustering.data");
        last_loop.step1.clustering.draw(fstep1clustering, last_loop.step1.trmap);
        fstep1clustering << '\n';

        std::ofstream fstep2sbins(fprefix + "step2-sorted-bins.data");   
        for (int i = 0; i < (int) last_loop.step2.trmaps.size(); i++) {
          const auto & trmap = last_loop.step2.trmaps[i].trmap;
          fstep2sbins << "#@step2-sorted-bins-cluster[" << i << "]="
                      << trmap.sorted_bins.size() << "\n";
          trmap.draw_sorted_bins(fstep2sbins, 0.0);
          fstep2sbins << '\n';
        }

        std::ofstream fstep2clustering(fprefix + "step2-clustering.data");   
        for (int i = 0; i < (int) last_loop.step2.trmaps.size(); i++) {
          const auto & trmap = last_loop.step2.trmaps[i].trmap;
          const auto & clg = last_loop.step2.trmaps[i].clustering;
          clg.draw(fstep2clustering, trmap);
          fstep2clustering << '\n';
        }

        std::ofstream fstep2lines(fprefix + "step2-lines.data");
        for (int i = 0; i < (int) last_loop.step2.clusters.size(); i++) {
          // const auto & trmap = *last_loop.step2.clusters[i].ptrmap;
          const auto & cl    = *last_loop.step2.clusters[i].pcluster;
          const auto & ln    = cl.line_data;
          polyline ln_samples;
          ln.generate_samples(-500.0, +500.0, ln_samples);
          fstep2lines << "#@step2-lines" << '\n';
          draw_polyline(fstep2lines, ln_samples);
          fstep2lines << '\n';
          fstep2lines << '\n';
        }     
      }

      std::clog << "Last loop #" << iloop << " : " << '\n';
      last_loop.print(std::clog);
      std::clog << '\n';
      std::clog << "Current hit clustering : \n";
      this->outdata->hit_clustering.print(std::cerr, "");
      
      // if (datatools::logger::is_debug(cfg.logging)) {
      //   std::clog << "End of loop #" << iloop << std::endl;
      //   std::clog << "Hit [Enter]" << std::endl;
      //   std::string resp;
      //   std::getline(std::cin, resp);
      // }
      iloop++;
      if (iloop == max_nloops) {
        break;
      }
    }
    this->terminate_loops();
   
    if (cfg.draw) {
      std::ofstream finalHitClustering(cfg.draw_prefix + "hit_clustering.data");
      this->outdata->hit_clustering.draw(finalHitClustering, *this);
    }
   
    if (cfg.draw) {
      std::ofstream finalTracks(cfg.draw_prefix + "clustering_tracks.data");
      for (int icl = 0; icl < (int) this->outdata->hit_clustering.clusters.size(); icl++) {
        const hit_cluster_data & hci = this->outdata->hit_clustering.clusters[icl];
        track_path_data trackPathi;
        hci.build_track_path(trackPathi);
        DT_LOG_DEBUG(cfg.logging, "trackPathi #" << trackPathi.vertexes.size()); 
        trackPathi.draw(finalTracks, icl);
        finalTracks << '\n';
      }
    }
    
    this->_build_output_data_();
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
  
} // namespace lttc 
