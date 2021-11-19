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

// This project:
#include <lttc/dbscan_clusterizer.hh>
#include <lttc/rectangle.hh>

namespace lttc {

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
    tmin = 0.0;
    tmax = M_PI; 
    rmin =  std::numeric_limits<double>::infinity();
    rmax = -std::numeric_limits<double>::infinity();
    nt = 0;
    nr = 0;
    dt = 0.01;
    dr = 1.0;
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

  
  void lttc_algo::map_type::fill_bin(int it0_, int ir0_, double gauss_threshold_)
  {
    if (gauss_threshold_ <= 0.0) {
      int itr0 = t_r_indexes_to_tr_bin_index(it0_, ir0_);
      if (itr0 >= 0) {
        // if (fill_bin_ctrl::debug) std::cerr << "[debug] lttc_algo::map_type::fill_bin: itr0 = " << itr0 << '\n';    
        bins[itr0] += 1.0;
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
            double g = fgauss(tt, rr);
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
    sorted_bins.assign(bins.size(), 0);
    std::iota(sorted_bins.begin(), sorted_bins.end(), 0);
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size());
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
                                          return (value < 0.0000001);
                                        }
                                        );
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [unsorted]");
    sorted_bins.erase(last_non_zero, sorted_bins.end());
    DT_LOG_DEBUG(logging, "sorted_bins.size = " << sorted_bins.size() << " [final]");
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
      if (bins[i] > 0) {
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

  lttc_algo::map_type::~map_type()
  {
    sorted_bins.clear();
    bins.clear();
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
      point cellCenter = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
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
    DT_LOG_DEBUG(cfg.logging, "Settting step1 map histogram...");
    step1.trmap.nt = cfg.step1_ntbins;
    step1.trmap.nr = cfg.step1_nrbins;
    step1.trmap.dt = trange / step1.trmap.nt;
    step1.trmap.dr = rrange / step1.trmap.nr;
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.nt = " << step1.trmap.nt);
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.nr = " << step1.trmap.nr);
    step1.trmap.bins.assign(step1.trmap.nt * step1.trmap.nr, 0.0);
    DT_LOG_DEBUG(cfg.logging, "  step1.trmap.nbins = " << step1.trmap.bins.size());

    DT_LOG_DEBUG(cfg.logging, "Processing hits...");
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      if (!can_process_hit(ihit)) {
        DT_LOG_DEBUG(cfg.logging, "Hit #" << ihit << " cannot be processed");
        continue;
      }
      DT_LOG_DEBUG(cfg.logging, "Processing hit #" << ihit << "...");
      point cellCenter = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      double x = cellCenter.x();
      double y = cellCenter.y();
      double r = h.drift_radius;
      legendre_transform lt(x, y, r);
      for (int itheta = 0; itheta < (int) step1.trmap.nt; itheta++) {
        // Center of the bin #itheta:
        double theta = step1.trmap.tmin + (itheta + 0.5) * step1.trmap.dt;
        ltr_type ltr = lt.eval(theta);
        // r-concave:
        double r1 = ltr.concave;
        int ir1 = step1.trmap.r_to_index(r1);
        step1.trmap.fill_bin(itheta, ir1);
        // r-convex:
        double r2 = ltr.convex;
        int ir2 = step1.trmap.r_to_index(r2);
        step1.trmap.fill_bin(itheta, ir2);
      }
      
    }
    step1.trmap.apply_threshold(cfg.step1_track_threshold);
    step1.trmap.build_sorted_bins();
    
    cluster_finder clusterFinder(step1.trmap);
    clusterFinder.find(step1.clustering);
    DT_LOG_DEBUG(cfg.logging, "step1.clustering.clusters    = " << step1.clustering.clusters.size() << " clusters");
    DT_LOG_DEBUG(cfg.logging, "step1.clustering.unclustered = " << step1.clustering.unclustered.size());
    
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
    t_min = std::numeric_limits<double>::quiet_NaN(); 
    t_max = std::numeric_limits<double>::quiet_NaN(); 
    r_min = std::numeric_limits<double>::quiet_NaN(); 
    r_step = std::numeric_limits<double>::quiet_NaN();  
    r_max = std::numeric_limits<double>::quiet_NaN(); 
    nbins_over_threshold = 0;
    hits_per_bin_ratio = std::numeric_limits<double>::quiet_NaN(); 
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

  lttc_algo::cluster::~cluster()
  {
    // std::cerr << "*** DEVEL *** cluster #" << id << " with bins size = " << bins.size() << std::endl;
    bins.clear();
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
    if (line_data.a == line_data.a) {
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
    out_ << "#@end-of-cluster=#" << id << '\n';
    return;
  }

  lttc_algo::clustering_data::~clustering_data()
  {
    clusters.clear();
    unclustered.clear();
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

  lttc_algo::cluster_finder::cluster_finder(const map_type & trmap_)
  {
    trmap = &trmap_;
    return;
  }

  lttc_algo::cluster_finder::~cluster_finder()
  {
    trmap = nullptr;
    return;
  }

  void lttc_algo::cluster_finder::find(clustering_data & clustering_)
  {
    dbscan_clusterizer::input_data input;
    dbscan_clusterizer::output_data output;
    input.trmap = trmap;
    double epsilon = 2.5;
    size_t min_pts = 3;
    dbscan_clusterizer dbScan(input, epsilon, min_pts);
    // dbScan.debug = true;
    dbScan.run(output);
    {
      std::ofstream fout("dbscan_output.data");
      fout << "#@dbscan-output" << '\n';
      output.draw(fout, input);
      fout << '\n';
    }

    // Build the list of step-1 unclustered bins from DBSCAN output:
    if (debug) std::cerr << "[debug] lttc_algo::cluster_finder::find: Collecting unclustered bins..." << '\n';
    for (auto isbin : output.unclustered) {
      int ibin = trmap->sorted_bins[isbin];
      // int tBin = -1; 
      // int rBin = -1;
      // trmap->tr_bin_index_to_t_r_indexes(iBin, tBin, rBin);
      clustering_.unclustered.insert(ibin);
    }
    
    // Build the list of step-1 clusters from DBSCAN output:
    if (debug) std::cerr << "[debug] lttc_algo::cluster_finder::find: Collecting clusters..." << '\n';
    for (int ic = 0; ic < (int) output.clusters.size(); ic++) {
      {
        cluster dummy;
        dummy.id = output.clusters[ic].id;
        clustering_.clusters.push_back(dummy);
      }
      if (debug) std::cerr << "[debug] lttc_algo::cluster_finder::find: Add a new cluster..." << '\n';
      cluster & cl = clustering_.clusters.back();
      for (int isbin : output.clusters[ic].sbins) {
        int iBin = trmap->sorted_bins[isbin];
        int tBin = -1; 
        int rBin = -1;
        trmap->tr_bin_index_to_t_r_indexes(iBin, tBin, rBin);
        double hBin = trmap->bins[iBin];
        cl.add_bin(iBin, tBin, rBin, hBin);
      }
      // Number of bins over threshold:
      size_t binsOverThresholdCounter = 0;
      for (const auto & p : cl.bins) {
        // int iBin = p.first;
        double hBin = p.second;
        if (hBin >= cl.threshold_height) { 
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
        int iBin = p.first;
        double hBin = p.second;
        if (hBin < cl.threshold_height) continue;
        int tBin = -1; 
        int rBin = -1;
        trmap->tr_bin_index_to_t_r_indexes(iBin, tBin, rBin);
        // Center of the bin:
        double t = trmap->index_to_t(tBin) + 0.5 * trmap->dt;
        double r = trmap->index_to_r(rBin) + 0.5 * trmap->dr;
        if (t < cl.t_min) cl.t_min = t;
        if (t > cl.t_max) cl.t_max = t;
        if (r < cl.r_min) cl.r_min = r;
        if (r > cl.r_max) cl.r_max = r;
        tsum  += t;
        t2sum += t * t;
        rsum  += r;
        r2sum += r * r;
        cl.hits_per_bin_ratio += hBin;
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
      if (!can_process_hit(ihit)) {
        continue;
      }
      point cellCenter = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
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
      DT_LOG_DEBUG(cfg.logging, "hits_per_bin_ratio=" << cl.hits_per_bin_ratio);
      {
        trc_data dummy;
        step2.trmaps.push_back(dummy);
      }
      trc_data & trc2 = step2.trmaps.back();
      map_type & cltrmap = trc2.trmap;
      cltrmap.id = "step2-" + std::to_string(ic);
      cltrmap.dr = dR;
      cltrmap.dt = cfg.step2_delta_theta; 
      DT_LOG_DEBUG(cfg.logging, "dt = " << cltrmap.dt);
      DT_LOG_DEBUG(cfg.logging, "dr = " << cltrmap.dr);
      // double trange = step1.trmap.index_to_t(cl.itmax) - step1.trmap.index_to_t(cl.itmin) + step1.trmap.dt + 0.5 * cltrmap.dt;
      // double rrange = step1.trmap.index_to_r(cl.irmax) - step1.trmap.index_to_r(cl.irmin) + step1.trmap.dr + 0.5 * cltrmap.dr;
      // cltrmap.nt = (size_t) (trange / cltrmap.dt);
      // cltrmap.nr = (size_t) (rrange / cltrmap.dr);
      // cltrmap.tmin = step1.trmap.index_to_t(cl.itmin);
      // cltrmap.tmax = cltrmap.tmin + cltrmap.nt * cltrmap.dt;
      // cltrmap.rmin = step1.trmap.index_to_r(cl.irmin);
      // cltrmap.rmax = cltrmap.rmin + cltrmap.nr * cltrmap.dr;
      double trange = cl.t_max - cl.t_min + 0.5 * cltrmap.dt;
      double rrange = cl.r_max - cl.r_min + 0.5 * cltrmap.dr;
      cltrmap.nt = (size_t) (trange / cltrmap.dt);
      cltrmap.nr = (size_t) (rrange / cltrmap.dr);
      cltrmap.tmin = cl.t_min - 0.25 * cltrmap.dt;
      cltrmap.tmax = cltrmap.tmin + cltrmap.nt * cltrmap.dt;
      cltrmap.rmin = cl.r_min - 0.25 * cltrmap.dr;
      cltrmap.rmax = cltrmap.rmin + cltrmap.nr * cltrmap.dr;
      DT_LOG_DEBUG(cfg.logging, "nt = " << cltrmap.nt);
      DT_LOG_DEBUG(cfg.logging, "nr = " << cltrmap.nr);
      DT_LOG_DEBUG(cfg.logging, "tmin:tmax = " << cltrmap.tmin << ':' << cltrmap.tmax);
      DT_LOG_DEBUG(cfg.logging, "rmin:rmax = " << cltrmap.rmin << ':' << cltrmap.rmax);
      cltrmap.bins.assign(cltrmap.nt * cltrmap.nr, 0.0);
      DT_LOG_DEBUG(cfg.logging, "cltrmap.bins.size = " << cltrmap.bins.size());
      int hcount = 0;
      DT_LOG_DEBUG(cfg.logging, "Filling histogram for " << indata->hits.size() << " hits...");
      for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
        const auto & h = indata->hits[ihit];
        if (!can_process_hit(ihit)) {
          DT_LOG_DEBUG(cfg.logging, "Do not processing hit #" << ihit << "...");
          continue;
        } else {
          DT_LOG_DEBUG(cfg.logging, "Processing hit #" << ihit << "...");
        }
        // for (const auto & h : indata->hits) {
        // if (cfg.debug) std::cerr << "[debug] lttc_algo::step2_run:   hit #" << hcount << '\n';
        point cellCenter = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
        double x = cellCenter.x();
        double y = cellCenter.y() ;
        double r = h.drift_radius;
        legendre_transform lt(x, y, r);
        for (int itheta = 0; itheta < (int) cltrmap.nt; itheta++) {
          double gt = cfg.step2_gauss_threshold;
          // if (hcount>6) {
          //   gt = 0.0;
          // }
          // if (hcount==5) {
          //   fill_bin_ctrl::debug = true;
          // } else {
          //   fill_bin_ctrl::debug = false;
          // }
          // if (cfg.debug) std::cerr << "[debug] lttc_algo::step2_run:     itheta=" << itheta << " (and nr=" << cltrmap.nr << ")" << '\n';
          // Center of bin #itheta:
          double theta = cltrmap.tmin + (itheta + 0.5) * cltrmap.dt;
          ltr_type ltr = lt.eval(theta);
          // if (cfg.debug) {
          //   std::cerr << "[debug] lttc_algo::step2_run:     theta=" << theta << " ltr.concave=" << ltr.concave << " ltr.convex=" << ltr.convex << "" << '\n';
          // }
          // r-concave:
          double r1 = ltr.concave;
          int ir1 = cltrmap.r_to_index(r1);
          // if (cfg.debug) {
          //   std::cerr << "[debug] lttc_algo::step2_run:     r1=" << r1 << " ir1=" << ir1 << '\n';
          // }
          cltrmap.fill_bin(itheta, ir1, gt);
          // r-convex:
          double r2 = ltr.convex;
          int ir2 = cltrmap.r_to_index(r2);
          // if (cfg.debug) {
          //   std::cerr << "[debug] lttc_algo::step2_run:     r2=" << r2 << " ir2=" << ir2 << '\n';
          // }
          // gt = 0.0;
          cltrmap.fill_bin(itheta, ir2, gt); 
         
        }
        hcount++;
      }
      DT_LOG_DEBUG(cfg.logging, "Filling histogram... Done.");
      cltrmap.apply_threshold(cfg.step2_track_threshold);
      cltrmap.build_sorted_bins();
      DT_LOG_DEBUG(cfg.logging, "Cluster t-r map : ");
      if (datatools::logger::is_debug(cfg.logging)) {
        cltrmap.print(std::cerr, "[debug] lttc_algo::step2_run:   ");
      }
      cluster_finder clusterFinder(cltrmap);
      clusterFinder.find(trc2.clustering);
      nsubclusters += trc2.clustering.clusters.size();
      DT_LOG_DEBUG(cfg.logging, "step2 => clustering.clusters  = " << trc2.clustering.clusters.size() << " sub-clusters");
      DT_LOG_DEBUG(cfg.logging, "step2 => clustering.unclustered = " << trc2.clustering.unclustered.size());
      DT_LOG_DEBUG(cfg.logging, "End of cluster #" << ic);
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
    for (int i1 = 0; i1 < (int) step2.trmaps.size(); i1++) {
      trc_data & trc1 = step2.trmaps[i1];
      const map_type & trmap1 = trc1.trmap;
      clustering_data & cld1 = trc1.clustering;
      for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
        cluster & cl1 = cld1.clusters[icl1];
        cl1.line_data = line::make_from_theta_r(cl1.t_mean, cl1.r_mean);
        trc_ref tref;
        tref.ptrmap = &trmap1;
        tref.pcluster = &cl1;
        step2.clusters.push_back(tref);
      }
    }
    std::sort(step2.clusters.begin(), step2.clusters.end());
    DT_LOG_DEBUG(cfg.logging, "Number of sorted step-2 clusters: " << step2.clusters.size());
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
      if ((i1 + 1) == (int) trmaps.size()) {
        out_ << "`-- ";
      } else {
        out_ << "|-- ";
      }
      out_ << "Clustering region: "
           << cld1.clusters.size() << " cluster(s)"
           << '\n';
      for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
        const cluster & cl1 = cld1.clusters[icl1];
        cl1.print(out_, indent_ + "|   ");
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
      // const tracker_hit & h = indata->hits[ihit];
      for (int icl = 0; icl < (int) step2.clusters.size(); icl++) {
        if (!can_process_hit(ihit)) {
          step3.HC[ihit][icl] = -1;
        } else {
          step3.HC[ihit][icl] = 0;
        }
      }
    }
    
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      if (!can_process_hit(ihit)) {
        continue;
      }
      std::set<int> mcls;
      point cellCenter = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      // double x = cellCenter.x();
      // double y = cellCenter.y() ;
      double r = h.drift_radius;
      double r_err = h.drift_radius_err;
      for (int icl = 0; icl < (int) step2.clusters.size(); icl++) {
        // const auto & trmap = *step2.clusters[icl].ptrmap;
        const auto & cl    = *step2.clusters[icl].pcluster;
        const auto & ln    = cl.line_data;
        double dist = ln.distance(cellCenter);
        double dtest = std::numeric_limits<double>::quiet_NaN();
        double dminus = std::abs(dist - r);
        double dplus  = dist + r;
        dtest = std::min(dminus, dplus);
        bool match = false;
        if (dtest < cfg.step3_nsigma * r_err) {
          match = true;
        }
        if (match) {
          DT_LOG_DEBUG(cfg.logging, "Hit #" << ihit << " @[" << h.side_id << "," << h.layer_id << "," << h.row_id << "] match cluster #" << icl);
          mcls.insert(icl);
          step3.HC[ihit][icl] = 1;
        }
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
  void lttc_algo::print_hc(const int_array2_type & hc_,
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
        if (hc_[ihit][icl] < 0) out_ << '-';
        else out_ << hc_[ihit][icl];
        out_ << "    ";
      }
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
    HC = int_array2_type(boost::extents[1][1]);
    return;
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

  void lttc_algo::update_hit_clustering()
  {
    // const step1_data & step1 = current_loop->step1;
    const step2_data & step2 = current_loop->step2;
    const step3_data & step3 = current_loop->step3;
    auto workingHC = step3.HC;
    size_t nhits = workingHC.shape()[0];
    size_t nclusters = workingHC.shape()[1];
    std::vector<size_t> cluster_stats;
    std::vector<uint32_t> cluster_status;
    cluster_stats.assign(nclusters, 0);
    cluster_status.assign(nclusters, 0);
    for (int icl = 0; icl < (int) nclusters; icl++) {
      for (int ihit = 0; ihit < (int) nhits; ihit++) {
        if (workingHC[ihit][icl] == -1) continue;
        if (workingHC[ihit][icl] > 0) {
          cluster_stats[icl]++;
          for (int icl2 = icl+1; icl2 <  (int) nclusters; icl2++) {
            workingHC[ihit][icl2] = 0;
          }
        }
      }
      DT_LOG_DEBUG(cfg.logging, "cluster #" << icl << " has " << cluster_stats[icl] << " hits");
      if (cluster_stats[icl] < 3) {
        cluster_status[icl] |= hit_clustering_data::CQ_CRIT_STAT;
        DT_LOG_DEBUG(cfg.logging, "cluster #" << icl << " has critical stat");
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
    if (datatools::logger::is_debug(cfg.logging)) {
      print_hc(workingHC, std::cerr, "[devel] Stage 1> ");
      std::cerr << "Wait..." << std::endl;
      std::string resp;
      std::getline(std::cin, resp);
    }
             
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
      new_cluster.line_data = pcluster->line_data;
      for (int ihit = 0; ihit < (int) nhits; ihit++) {
        if (workingHC[ihit][icl] < 0) {
          continue;
        }
        if (workingHC[ihit][icl] > 0) {
          new_cluster.add_hit(ihit, *this);
           outdata->hit_clustering.unclustered_hits.erase(ihit);
          // Tag clusterized hits to be removed : 
          removed_hits[ihit] = new_cluster.id;
        } 
      }
    }

    if (datatools::logger::is_debug(cfg.logging)) {
      print_hc(workingHC, std::cerr, "[debug] Stage 2> ");
      std::cerr << "Wait..." << std::endl;
      std::string resp;
      std::getline(std::cin, resp);
    }
    
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
    for (int ihit :  outdata->hit_clustering.unclustered_hits) {
      int    asso_icl = -1;
      double asso_dist = +std::numeric_limits<double>::infinity();
      const tracker_hit & h = indata->hits[ihit];
      point cellPos = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      for (int icl = 0; icl < (int)  outdata->hit_clustering.clusters.size(); icl++) {
        hit_cluster_data & hcl =  outdata->hit_clustering.clusters[icl];
        const line & cline = hcl.line_data;
        double rdist = cline.distance(cellPos);
        // bool associated = false;
        if (rdist < (h.drift_radius + 3 * h.drift_radius_err))  {
          // The hit is on the cluster's line path:
          int far_hit_1  = hcl.far_hit_1;
          int far_hit_2  = hcl.far_hit_2;
          const tracker_hit & h1 = indata->hits[far_hit_1]; 
          const tracker_hit & h2 = indata->hits[far_hit_2];
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
    return;
  }

  void lttc_algo::terminate_loops()
  {
    associate_unclustered_hits();
    return;
  }
  
  void lttc_algo::hit_cluster_data::add_hit(int ihit_, const lttc_algo & algo_)
  {
    if (hits.size() == 0) {
      far_hit_1 = ihit_;
      far_hit_2 = ihit_;
    } else {
      const tracker_hit & h1 = algo_.indata->hits[far_hit_1]; 
      const tracker_hit & h2 = algo_.indata->hits[far_hit_2];
      const tracker_hit & h  = algo_.indata->hits[ihit_];
      point cellPos1 = algo_.sntracker->cell_position(h1.side_id, h1.layer_id, h1.row_id);
      point cellPos2 = algo_.sntracker->cell_position(h2.side_id, h2.layer_id, h2.row_id);
      point cellPos  = algo_.sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      double d12 = std::hypot(cellPos2.x() - cellPos1.x(), cellPos2.y() - cellPos1.y());
      double d1 = std::hypot(cellPos.x() - cellPos1.x(), cellPos.y() - cellPos1.y());
      double d2 = std::hypot(cellPos.x() - cellPos2.x(), cellPos.y() - cellPos2.y());
      if (d1 > d12 and d1 >= d2) {
        far_hit_2 = ihit_;
      } else if (d2 > d12 and d2 >= d1) {
        far_hit_1 = ihit_;
      }
    }
    hits.insert(ihit_);
    return;
  }

  void lttc_algo::hit_cluster_data::print(std::ostream & out_,
                                         const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "ID : " << id << '\n';
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
    out_ << indent_ << "|-- " << "Far hit 1 : " << far_hit_1 << '\n';
    out_ << indent_ << "|-- " << "Far hit 2 : " << far_hit_2 << '\n';
    out_ << indent_ << "`-- " << "Line data : " << '\n';
    line_data.print(out_, indent_ + "    ");
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
     
  void lttc_algo::hit_clustering_data::draw(std::ostream & out_, const lttc_algo & algo_) const
  {
    double ercell = algo_.sntracker->rcell - 2.0;
    double ercell2 = algo_.sntracker->rcell - 8.0;
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
        point cellPos = algo_.sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() + ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() + ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() - ercell << ' ' << 1+icl << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << 1+icl << '\n';
        out_ << '\n';
        if (ihit == hcl.far_hit_1 || ihit == hcl.far_hit_2) {
          out_ << cellPos.x() - ercell2 << ' ' << cellPos.y() - ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() - ercell2 << ' ' << cellPos.y() + ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() + ercell2 << ' ' << cellPos.y() + ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() + ercell2 << ' ' << cellPos.y() - ercell2 << ' ' << 1+icl << '\n';
          out_ << cellPos.x() - ercell2 << ' ' << cellPos.y() - ercell2 << ' ' << 1+icl << '\n';
          out_ << '\n';
        }
      }
      {
        const auto & ld = hcl.line_data;
        polyline ld_samples;
        double t1 = -500.0;
        double t2 = +500.0;
        ld.generate_samples(t1, t2, ld_samples);
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
    _clear_processing_data_();
    _set_input_data_(indata_);
    outdata = &outdata_;

    ltcAlgo.init_loops();
    int iloops = 0;
    bool go_loops = true;

    while (go_loops) {
      std::string fprefix = "lttc_algo-loop_" + std::to_string(iloops);
      size_t old_nclusters = outdata->hit_clustering.clusters.size();

      {
        std::ofstream fhits(fprefix + "-hits.data");
        for (int ihit = 0; ihit < (int) evt.hits.size(); ihit++) {
          const auto & h = indata->.hits[ihit];
          if (!ltcAlgo.can_process_hit(ihit)) {
            continue;
          }
          fhits << "#@hit\n";
          h.draw(fhits);
        }
        fhits << '\n';
      }
      
      std::clog << "Running loop... \n";
      ltcAlgo.run_loop();
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
     
      {
        std::ofstream fstep1bins(fprefix + "-step1-bins.data");   
        fstep1bins << "#@step1-bins=" << last_loop.step1.trmap.bins.size() << "\n";
        last_loop.step1.trmap.draw_bins(fstep1bins);
        fstep1bins << '\n';
      }

    
/// XXX



      
      if (datatools::logger::is_debug(cfg.logging)) {
        std::clog << "End of loop #" << iloops << std::endl;
        std::string resp;
        std::getline(std::cin, resp);
      }
      iloops++;
      
    }
    terminate_loops();
    
    _build_output_data_();
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
  
} // namespace lttc 
