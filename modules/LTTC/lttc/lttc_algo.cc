// \file lttc_algo.cc

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
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_fit.h>

// Falaise:
#include <falaise/geometry/rectangle.hh>

// This project:
#include <lttc/dbscan_clusterizer.hh>
#include <lttc/missing_hits_finder.hh>

namespace lttc {

  using falaise::geometry::point2;
  using falaise::geometry::point3;
  using falaise::geometry::vector2;
  using falaise::geometry::fitted_point2;
  using falaise::geometry::line2;
  using falaise::geometry::fitted_line2;
  using falaise::geometry::rectangle;
  using falaise::geometry::polyline2;

  // static
  const size_t lttc_algo::DEFAULT_TNBINS;
  
  // static
  const size_t lttc_algo::DEFAULT_RNBINS;

  lttc_algo::lttc_algo(const snemo::processing::detector_description & det_desc_, const config & cfg_)
  {
    detector_desc = &det_desc_;
    sntracker = std::make_unique<tracker>(detector_desc->get_gg_locator());
    cfg = cfg_;
    return;
  }
   
  lttc_algo::~lttc_algo()
  {
    return;
  }

  const  snemo::processing::detector_description & lttc_algo::get_detector_desc() const
  {
    return *detector_desc;
  }
  
  const tracker & lttc_algo::get_sntracker() const
  {
    return *sntracker;
  }
 
  bool lttc_algo::validate_cell(const cell_id & cid_) const
  {
    static const int moduleId = 0;
    if (detector_desc->has_cell_status_service() and snemo::time::is_valid(indata->timestamp)) {
      geomtools::geom_id gid( detector_desc->get_gg_locator().cellGIDType(), moduleId, cid_.side(), cid_.layer(), cid_.row());
      std::uint32_t cellStatus = detector_desc->get_cell_status_service().get_cell_status(gid, indata->timestamp);
      if (snemo::rc::tracker_cell_status::is_dead(cellStatus)) {
        return false;
      }
      if (snemo::rc::tracker_cell_status::is_off(cellStatus)) {
        return false;
      }
      if (snemo::rc::tracker_cell_status::is_no_anode(cellStatus)) {
        return false;
      }
    }
    if (sntracker != nullptr and sntracker->has_tracker_conditions()) {
      if (sntracker->get_tracker_conditions().has_dead_cell(cid_)) {
        return false;
      }
    }
    return true;
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
    const double rcell = detector_desc->get_gg_locator().cellRadius();
    DT_LOG_DEBUG(cfg.logging, "Computing step1 map bounds...");
    for (int ihit = 0; ihit < (int) indata->hits.size(); ihit++) {
      const tracker_hit & h = indata->hits[ihit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      if (not validate_cell(cid)) {
        continue;
      }
      point2 cellCenter(h.x, h.y);
      double x = cellCenter.x();
      double y = cellCenter.y();
      double r1 = std::abs(x) + std::abs(y) + rcell; 
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
      if (! validate_cell(cid)) {
        continue;
      }
      if (!can_process_hit(ihit)) {
        DT_LOG_DEBUG(cfg.logging, "Hit #" << ihit << " cannot be processed");
        continue;
      }
      DT_LOG_DEBUG(cfg.logging, "Processing hit #" << ihit << " at cell " << cid);
      point2 cellCenter(h.x, h.y);
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
        step1.trmap.fill_bin(itheta, ir1, rt_map::no_gauss_kernel, 1.0);
        // double r1Up = ltrUp.concave;
        // int ir1Up = step1.trmap.r_to_index(r1Up);
        // step1.trmap.fill_bin(itheta, ir1Up, rt_map::no_gauss_kernel, 0.5);
        // double r1Down = ltrDown.concave;
        // int ir1Down = step1.trmap.r_to_index(r1Down);
        // step1.trmap.fill_bin(itheta, ir1Down, rt_map::no_gauss_kernel, 0.5);
        
        // r-convex:
        double r2 = ltr.convex;
        int ir2 = step1.trmap.r_to_index(r2);
        step1.trmap.fill_bin(itheta, ir2, rt_map::no_gauss_kernel);
        // double r2Up = ltrUp.convex;
        // int ir2Up = step1.trmap.r_to_index(r2Up);
        // step1.trmap.fill_bin(itheta, ir2Up, rt_map::no_gauss_kernel, 0.5);
        // double r2Down = ltrDown.convex;
        // int ir2Down = step1.trmap.r_to_index(r2Down);
        // step1.trmap.fill_bin(itheta, ir2Down, rt_map::no_gauss_kernel, 0.5);
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
    rectangle c(point2(t_min, r_min), point2(t_max, r_max));
    rectangle oc(point2(other_.t_min, other_.r_min), point2(other_.t_max, other_.r_max));
    if (!c.overlap(oc, 0.5 * other_.t_step, 0.5 * other_.r_step)) return false;
    return true;
  }

  // static   
  bool lttc_algo::cluster_overlap(const cluster & cl1_, const rt_map & trmap1_,
                                  const cluster & cl2_, const rt_map & trmap2_)
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
      point2 c1(t1, r1);
      DT_LOG_DEBUG(logging, "nbins2=" << cl2_.bins.size());
      for (const auto & p2 : cl2_.bins) {
        int ibin2 = p2.first;
        // double hbin2 = p2.second;
        int tbin2 = -1; 
        int rbin2 = -1;
        trmap2_.tr_bin_index_to_t_r_indexes(ibin2, tbin2, rbin2);
        double t2 = trmap2_.index_to_t(tbin2) + 0.5 * trmap2_.dt;
        double r2 = trmap2_.index_to_r(rbin2) + 0.5 * trmap2_.dr;
        rectangle cell2(point2(t2, r2), trmap2_.dt, trmap2_.dr);
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
                                const rt_map & trmap_) const
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
                                        const rt_map & trmap_) const
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
                                        const rt_map & trmap_) const
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

  lttc_algo::cluster_finder::cluster_finder(const rt_map & trmap_, datatools::logger::priority logging_)
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
      if (not validate_cell(cid)) {
        continue;
      }
      if (!can_process_hit(ihit)) {
        continue;
      }
      point2 cellCenter(h.x, h.y);
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
    DT_LOG_DEBUG(cfg.logging, "ncl = " << ncl);
    DT_LOG_DEBUG(cfg.logging, "Max # of clusters = " << cfg.step2_max_nlines);
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
      rt_map & cltrmap = trc2.trmap;
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
        if (not validate_cell(cid)) {
          continue;
        }
        if (!can_process_hit(ihit)) {
          DT_LOG_DEBUG(cfg.logging, "    Do not process hit #" << ihit << "...");
          continue;
        } else {
          // DT_LOG_DEBUG(cfg.logging, "    Processing hit #" << ihit << "...");
        }
        point2 cellCenter(h.x, h.y);
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
        const rt_map & trmap1 = trc1.trmap;
        const clustering_data & cld1 = trc1.clustering;
        for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
          const cluster & cl1 = cld1.clusters[icl1];
          DT_LOG_DEBUG(cfg.logging, "cluster #" << icl1 << " : " << cl1.hits_per_bin_ratio);
          for (int i2 = 0; i2 < (int) step2.trmaps.size(); i2++) {
            if (i1 == i2) continue;
            const trc_data & trc2 = step2.trmaps[i2];
            const rt_map & trmap2 = trc2.trmap;
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
        const rt_map & trmap1 = trc1.trmap;
        clustering_data & cld1 = trc1.clustering;
        for (int icl1 = 0; icl1 < (int) cld1.clusters.size(); icl1++) {
          cluster & cl1 = cld1.clusters[icl1];
          cl1.id = subClusterID;          
          DT_LOG_DEBUG(cfg.logging, "Sub-cluster #" << icl1 << " from region " << i1 << " has ID=" << cl1.id);    
          cl1.line_data = line2::make_from_theta_r(cl1.t_mean, cl1.r_mean);
          // double tPeak = trmap1.index_to_t(cl1.peak_tbin);
          // double rPeak = trmap1.index_to_r(cl1.peak_rbin);
          // cl1.line_data = line2::make_from_theta_r(tPeak, rPeak);
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
      // const rt_map & trmap1 = trc1.trmap;
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
      if (not validate_cell(cid)) {
        continue;
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
      point2 cellCenter(h.x, h.y);
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
        fitted_line2 clusterFitLine;
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
         *      r_dir       |  :\   .   /:
         *     <---------->/.  : `-----' :
         *        drift   / .  :    .    :
         *       radius     .       .
         *          r       .<----->.
         *                  .residual (in chi2 sigma unit)
         */
        fitted_point2 fitLineProj;
        orthogonal_projection_to_fitted_line(clusterFitLine, cellCenter, fitLineProj);
        // P :
        point2 fitLineProjCenter(fitLineProj.x, fitLineProj.y);
        vector2 CP = fitLineProjCenter - cellCenter;
        vector2 radiusDir = CP.unit();
        // T :
        point2 hitTangentPoint = cellCenter + radiusDir * r;
        // vector2 CT = hitTangentPoint - cellCenter;
        // TP :
        double distToLine = (fitLineProjCenter - hitTangentPoint).mag();
        DT_LOG_DEBUG(cfg.logging, "  distToLine = " << distToLine / CLHEP::mm << " mm");
        double nsigma = fitLineProj.nsigma(hitTangentPoint);
        DT_LOG_DEBUG(cfg.logging, "  nsigma = " << nsigma);
        double residual = datatools::invalid_real();
        double chi2 = datatools::invalid_real();
        bool   match = false;
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

    // // >> ZZZ
    // DT_LOG_DEBUG(cfg.logging, "Compute the clusters quality CQ array...");
    // step3.CQ.reserve(step2.clusters.size());
    // for (int iCluster = 0; iCluster < (int) step2.clusters.size(); iCluster++) {
    //   DT_LOG_DEBUG(cfg.logging, "==========> Compute quality for cluster #" << iCluster);
    //   {
    //     cluster_quality_data newCqData;
    //     step3.CQ.push_back(newCqData);
    //   }
    //   cluster_quality_data & cqData = step3.CQ.back();
    //   std::set<int> excluded_hits;
    //   compute_cluster_quality(iCluster, excluded_hits, cqData);
    // } // for iCluster
    // /// << ZZZ
    return;
  }
  
  void lttc_algo::compute_cluster_hits(hit_cluster_data & hc_)
  {
    const step3_data & step3 = current_loop->step3;
    auto workingHC = step3.HC;
    hc_.side = -1;
    hc_.delayed = false;
    hc_.end_hit_0 = -1;
    hc_.end_hit_1 = -1;
    hc_.hits.clear();
    size_t nHits = workingHC.shape()[0];
    for (int iHit = 0; iHit < (int) nHits; iHit++) {
      DT_LOG_DEBUG(cfg.logging, "iHit=" << iHit);
      if (! outdata->hit_clustering.hit_clustering_map.count(iHit)) {
        std::set<int> emptySet;
        outdata->hit_clustering.hit_clustering_map[iHit] = emptySet;
      } 
      if (hc_.excluded_hits.count(iHit)) {
        // Do not take into account a hit marked as excluded by previous reconstruction steps:
        if (outdata->hit_clustering.hit_clustering_map.find(iHit)->second.count(hc_.id)) {
          outdata->hit_clustering.hit_clustering_map.find(iHit)->second.erase(hc_.id);
        }
        continue;
      }
      hit_cluster_association_data & hcadi = workingHC[iHit][hc_.step3_id];
      if (hcadi.association_status == HCAS_NOT_VALID) continue;
      if (hcadi.association_status == HCAS_NOT_ASSOCIABLE) continue;
      if (hcadi.association_status == HCAS_ASSOCIATED) {
        cluster_add_hit(iHit, hc_);
        hc_.hit_associations[iHit] = hcadi;
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
   
  void lttc_algo::cluster_quality_data::print(std::ostream & out_, const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "Nhits : " << nhits << '\n';
    out_ << indent_ << "|-- " << "Chi2 sum : " << sumChi2 << '\n';
    out_ << indent_ << "|-- " << "ndof     : " << ndof << '\n';
    out_ << indent_ << "|-- " << "P-value  : " << pvalue << '\n';
    if (outliers.size()) {
      out_ << indent_ << "|-- " << "Without outliers : " << '\n';
      out_ << indent_ << "|   " << "|-- " << "Chi2 sum : " << noOutliersSumChi2 << " (w/o outliers)" << '\n';
      out_ << indent_ << "|   " << "|-- " << "ndof     : " << noOutliersNdof << " (w/o outliers)" << '\n';
      out_ << indent_ << "|   " << "`-- " << "P-value  : " << noOutliersPvalue << " (w/o outliers)" << '\n';
    }
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
    if (outliers.size()) {
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
    }
    out_ << indent_ << "|-- " << "Missing hits : " << missing_hits.size() << '\n';
    {
      for (int i = 0; i < (int) missing_hits.size(); i++) {
        out_ << indent_ << "|   ";
        if ((i + 1) == (int) missing_hits.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "#" << i << " : " << missing_hits[i] << '\n';
      }
    }
    out_ << indent_ << "`-- " << "Rank : " << rank << '\n';
    return;
  }

  /*
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
      out_ << " rank=" << cq_[icl].rank;
      out_ << '\n';
     }    
    return;
  }
  */
  
  void lttc_algo::step3_data::print(std::ostream & out_, const std::string & indent_) const
  {
    // size_t nhits = HC.shape()[0];
    // size_t nclusters = HC.shape()[1];
    out_ << indent_ << "|-- HC : " << '\n';
    print_hc(HC, out_, indent_ + "|   ");
    // out_ << indent_ << "|-- CQ : " << '\n';
    // print_cq(CQ, out_, indent_ + "|   ");
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

  void lttc_algo::clusters_detect_twins()
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
          // HCD.degenerated_clusters.insert(j);
          // HCD.degenerated_clusters.insert(i);
        }
      }
    }
    
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::clusters_detect_contained()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    
    auto & HCD = outdata->hit_clustering;
    for (int i = 0; i < (int) HCD.clusters.size(); i++) {
      hit_cluster_data & hci = HCD.clusters[i];
      for (int j = i + 1; j < (int) HCD.clusters.size(); j++) {
        hit_cluster_data & hcj = HCD.clusters[j];
        if (hci.twins.count(j)) {
          // Do not consider twin clusters:
          continue;
        }
        if (std::includes(hci.hits.begin(), hci.hits.end(),
                          hcj.hits.begin(), hcj.hits.end())) {
          DT_LOG_DEBUG(cfg.logging, "Cluster #" << i << " includes cluster #" << j);
          hci.superset_of.insert(j);
          hcj.subset_of.insert(i);
        }
      }
    }
    
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::clusters_detect_overlapping()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    auto & HCD = outdata->hit_clustering;
    for (int i = 0; i < (int) HCD.clusters.size(); i++) {
      hit_cluster_data & hci = HCD.clusters[i];
      for (int j = i + 1; j < (int) HCD.clusters.size(); j++) {
        hit_cluster_data & hcj = HCD.clusters[j];
        if (hci.twins.count(j)) {
          // Do not consider twin clusters:
          continue;
        }
        if (hci.superset_of.count(j)) {
          // Do not consider included clusters:
          continue;
        }
        if (hci.subset_of.count(j)) {
          // Do not consider including clusters:
          continue;
        }
        std::set<int> hitIntersection;
        std::set_intersection(hci.hits.begin(), hci.hits.end(),
                              hcj.hits.begin(), hcj.hits.end(),
                              std::inserter(hitIntersection, hitIntersection.begin()));
        size_t intSec = hitIntersection.size();
        if ((intSec > 0) and (intSec < hci.hits.size()) and (intSec < hcj.hits.size()) ) {
          DT_LOG_DEBUG(cfg.logging, "Clusters #" << i << " and #" << j << " overlap");
          hci.overlap_with.insert(j);
          hcj.overlap_with.insert(i);
        }
      }
    }
   
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
  
  void lttc_algo::clusters_detect_curve_degenerated()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");

    auto & HCD = outdata->hit_clustering;
    std::map<int,int> discardedClusters;
    for (int iCluster = 0; iCluster < (int) HCD.clusters.size(); iCluster++) {
      hit_cluster_data & hci = HCD.clusters[iCluster];
      if (hci.flags & hit_cluster_data::FLAG_DISCARDED) {
        continue;
      }
      DT_LOG_DEBUG(cfg.logging, "Searching for curve degeneration for cluster #" << iCluster << "...");
      const fitted_point2 & node0 = hci.hit_associations.find(hci.end_hit_0)->second.node;
      const fitted_point2 & node1 = hci.hit_associations.find(hci.end_hit_1)->second.node;
      DT_LOG_DEBUG(cfg.logging, "  => Node0=" << node0);
      DT_LOG_DEBUG(cfg.logging, "  => Node1=" << node1);
      point2 nodePos0(node0.x, node0.y);
      point2 nodePos1(node1.x, node1.y);
      double r_err0 = indata->hits[hci.end_hit_0].drift_radius_err;
      double r_err1 = indata->hits[hci.end_hit_1].drift_radius_err;
      DT_LOG_DEBUG(cfg.logging, "  =>r_err0=" << r_err0);
      DT_LOG_DEBUG(cfg.logging, "  =>r_err1=" << r_err1);
      for (int iTwinCluster : hci.twins) {
        DT_LOG_DEBUG(cfg.logging, "  Checking twin cluster #" << iTwinCluster << "...");
        hit_cluster_data & twinHci = HCD.clusters[iTwinCluster];
        if (twinHci.flags & hit_cluster_data::FLAG_DISCARDED) {
          continue;
        }
        const fitted_point2 & twinNode0 = twinHci.hit_associations.find(twinHci.end_hit_0)->second.node;
        const fitted_point2 & twinNode1 = twinHci.hit_associations.find(twinHci.end_hit_1)->second.node;
        point2 twinNodePos0(twinNode0.x, twinNode0.y);
        point2 twinNodePos1(twinNode1.x, twinNode1.y);
        DT_LOG_DEBUG(cfg.logging, "     => twinMode0=" << twinNode0);
        DT_LOG_DEBUG(cfg.logging, "     => twinMode1=" << twinNode1);
        double dist0 = (nodePos0 - twinNodePos0).mag();
        double dist1 = (nodePos1 - twinNodePos1).mag();
        DT_LOG_DEBUG(cfg.logging, "     => dist0=" << dist0);
        DT_LOG_DEBUG(cfg.logging, "     => dist1=" << dist1);
        bool match = false;
        if ((dist0 <= cfg.step3_node_match_tolerance)
          and (dist1 <= cfg.step3_node_match_tolerance)) {
          DT_LOG_DEBUG(cfg.logging, "Cluster #" << iCluster << " equal cluster #" <<iTwinCluster);
          match = true;
        }
        // double dist0 = (nodePos0 - twinNodePos0).mag() / r_err0;
        // double dist1 = (nodePos1 - twinNodePos1).mag() / r_err1;
        // DT_LOG_DEBUG(cfg.logging, "     => dist0=" << dist0);
        // DT_LOG_DEBUG(cfg.logging, "     => dist1=" << dist1);
        // if ((dist0 <= cfg.step3_node_match_tolerance)
        //   and (dist1 <= cfg.step3_node_match_tolerance)) {
        //   DT_LOG_DEBUG(cfg.logging, "Cluster #" << iCluster << " equal cluster #" <<iTwinCluster);
        // }
        //  if (node0.equal(twinNode0, cfg.step3_node_match_tolerance)
        //     and node1.equal(twinNode1, cfg.step3_node_match_tolerance)) {
        //   DT_LOG_DEBUG(cfg.logging, "Cluster #" << iCluster << " equal cluster #" <<iTwinCluster);
        // }
        if (match) {
          if (hci.quality.rank > twinHci.quality.rank) {
            hci.flags |= hit_cluster_data::FLAG_DISCARDED;
            discardedClusters[iCluster] = iTwinCluster;
          } else if (twinHci.quality.rank > hci.quality.rank) {
            twinHci.flags |= hit_cluster_data::FLAG_DISCARDED;
            discardedClusters[iTwinCluster] = iCluster;
          } else {
            if (hci.quality.get_effective_pvalue() < twinHci.quality.get_effective_pvalue()) {
              hci.flags |= hit_cluster_data::FLAG_DISCARDED;
              discardedClusters[iCluster] = iTwinCluster;
             } else if (twinHci.quality.get_effective_pvalue() > hci.quality.get_effective_pvalue()) {
              twinHci.flags |= hit_cluster_data::FLAG_DISCARDED;
              discardedClusters[iTwinCluster] = iCluster;
            }
          }
        }
        if (hci.flags & hit_cluster_data::FLAG_DISCARDED) {
          continue;
        }
      }
    }
    DT_LOG_DEBUG(cfg.logging, "Report : ");
    for (auto iCl : discardedClusters) {
      DT_LOG_DEBUG(cfg.logging, " => Discarding cluster #" << iCl.first << " which has been found degenerated with cluster #" << iCl.second);
    }

    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::clusters_detect_relationships()
  {
    for (int i = 0; i < (int) outdata->hit_clustering.clusters.size(); i++) {
      hit_cluster_data & hci = outdata->hit_clustering.clusters[i];
      hci.twins.clear();
      hci.superset_of.clear();
      hci.subset_of.clear();
      hci.overlap_with.clear();
    }
    clusters_detect_twins();
    clusters_detect_curve_degenerated();
    clusters_detect_contained();
    clusters_detect_overlapping();
    return;
  }

  void lttc_algo::compute_cluster_ordered_hits(hit_cluster_data & hc_)
  {
    // XXX
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    hc_.track_ordered_hits.clear();
    hc_.track_ordered_hits.reserve(hc_.hits.size());
    std::set<int> remainingHits = hc_.hits;
    DT_LOG_DEBUG(cfg.logging, "#remainingHits = " << remainingHits.size());
    if (hc_.line_data.is_valid()) {
      DT_LOG_DEBUG(cfg.logging, "Valid line data");
      int iFirstHit = hc_.end_hit_0;
      int iLastHit  = hc_.end_hit_1;
      DT_LOG_DEBUG(cfg.logging, "iFirstHit   = " << iFirstHit);
      DT_LOG_DEBUG(cfg.logging, "iLastHit    = " << iLastHit);
      int iCurrentHit = iFirstHit;
      DT_LOG_DEBUG(cfg.logging, "iCurrentHit = " << iCurrentHit);
      fitted_point2 currentNode = hc_.hit_associations.find(iCurrentHit)->second.node;
      point2 currentVertex(currentNode.x, currentNode.y);
      while (remainingHits.size() > 1) {
        remainingHits.erase(iCurrentHit);
        hc_.track_ordered_hits.push_back(iCurrentHit);
        // Search the closest hit along the fitted line:
        int iNextHit = -1;
        double minDist = std::numeric_limits<double>::infinity();
        for (auto iHit : remainingHits) {
          const fitted_point2 & theNode = hc_.hit_associations.find(iHit)->second.node;
          point2 theVertex(theNode.x, theNode.y);
          double nodeDist = (theVertex - currentVertex).mag();
          if (nodeDist <= minDist) {
            minDist = nodeDist;
            iNextHit = iHit;
          }
        }
        iCurrentHit = iNextHit;
      }
      DT_THROW_IF(remainingHits.count(iLastHit) == 0, std::logic_error,
                  "End hit 1=" << iLastHit << " should be the last in the remaining set!");
      hc_.track_ordered_hits.push_back(iLastHit);    
    } /* else {
      // Not implemented yet
         } 
      */
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  } 
  
  void lttc_algo::clusters_build_ordered_hits()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    auto & HCD = outdata->hit_clustering;
    for (int iCluster = 0; iCluster < (int) HCD.clusters.size(); iCluster++) {
      // Apply to each cluster:
      hit_cluster_data & hci = HCD.clusters[iCluster];
      compute_cluster_ordered_hits(hci);
    } // for iCluster
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
 
  void lttc_algo::compute_cluster_quality_missing_hits(hit_cluster_data & hit_cluster_)
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    // const step3_data & step3 = current_loop->step3;
    std::vector<cluster_missing_hit_data> & missingHits = hit_cluster_.quality.missing_hits;
    missingHits.clear();
    // Line case:
    if (hit_cluster_.line_data.is_valid()) {
      DT_LOG_DEBUG(cfg.logging, "Processing linear cluster ID=" << hit_cluster_.id);
      int end_hit_0 = hit_cluster_.end_hit_0;
      int end_hit_1 = hit_cluster_.end_hit_1;
      const tracker_hit & h1 = indata->hits[end_hit_0];
      const tracker_hit & h2 = indata->hits[end_hit_1];
      cell_id cid1 = h1.get_cell_id();
      cell_id cid2 = h2.get_cell_id();
      DT_LOG_DEBUG(cfg.logging, "  cid1=" << cid1);
      DT_LOG_DEBUG(cfg.logging, "  cid2=" << cid2);
      // point2 cellCenter1 = sntracker->cell_position(cid1);
      // point2 cellCenter2 = sntracker->cell_position(cid2);
      point2 cellCenter1(h1.x, h1.y); // = sntracker->cell_position(cid1);
      point2 cellCenter2(h2.x, h2.y); // = sntracker->cell_position(cid2);
      fitted_point2 proj1;
      DT_LOG_DEBUG(cfg.logging, "  cellCenter1=" << cellCenter1);
      DT_LOG_DEBUG(cfg.logging, "  cellCenter2=" << cellCenter2);
      fitted_point2 proj2;
      orthogonal_projection_to_fitted_line(hit_cluster_.line_data, cellCenter1, proj1);
      orthogonal_projection_to_fitted_line(hit_cluster_.line_data, cellCenter2, proj2);
      point2 nearestPointOnLine1 = proj1.center();
      point2 nearestPointOnLine2 = proj2.center();
      DT_LOG_DEBUG(cfg.logging, "  nearestPointOnLine1=" << nearestPointOnLine1);
      DT_LOG_DEBUG(cfg.logging, "  nearestPointOnLine2=" << nearestPointOnLine2);
      DT_LOG_DEBUG(cfg.logging, "  dist1=" << (nearestPointOnLine1 - cellCenter1).mag());
      DT_LOG_DEBUG(cfg.logging, "  dist2=" << (nearestPointOnLine2 - cellCenter2).mag());      
      missing_hits_finder mhf(*this,
                              indata->hits,
                              hit_cells,
                              hit_cluster_.hits);
      mhf.logging = cfg.logging;
      mhf.find(nearestPointOnLine1, nearestPointOnLine2, missingHits);
      DT_LOG_DEBUG(cfg.logging, "  Found " << missingHits.size() << " missing hits for linear cluster ID=" << hit_cluster_.id);
      /*
      int end_hit_0 = hit_cluster_.end_hit_0;
      int end_hit_1 = hit_cluster_.end_hit_1;
      const fitted_line2 & line_data = hit_cluster_.line_data;
      const tracker_hit_collection & hits = indata->hits;
      const tracker_hit & h1 = hits[end_hit_0];
      const tracker_hit & h2 = hits[end_hit_1];
      cell_id cid1(h1.side_id, h1.layer_id, h1.row_id);
      cell_id cid2(h2.side_id, h2.layer_id, h2.row_id);
      int minLayer = std::min(h1.layer_id, h2.layer_id);
      int maxLayer = std::max(h1.layer_id, h2.layer_id);
      int minRow = std::min(h1.row_id, h2.row_id);
      int maxRow = std::max(h1.row_id, h2.row_id);
      // Check for cells in a fiducial rectangle between the ending hits
      for (int iLayer = minLayer; iLayer <= maxLayer; iLayer++) {
        for (int iRow = minRow; iRow <= maxRow; iRow++) {
          cell_id cid(hit_cluster_.side, iLayer, iRow);
          // Skip the ending hits:
          if (cid == cid1) continue;
          if (cid == cid2) continue;
          point2 cellCenter = sntracker->cell_position(cid);
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
      */
    } else {
      // FUTURE: circle case
      DT_LOG_DEBUG(cfg.logging, "No missing cells can be computed!");
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::clusters_build_track_paths()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");

    auto & HCD = outdata->hit_clustering;
    for (int iCluster = 0; iCluster < (int) HCD.clusters.size(); iCluster++) {
      // Apply to each cluster:
      hit_cluster_data & hci = HCD.clusters[iCluster];
      hci.track_path.cluster_index = iCluster;
      compute_cluster_track_path(hci);
      // hci.build_track_path(hci.track_path, indata->hits);
      hci.track_path.compute_z_statistics();
      // (cfg.step3_nsigma_z, cfg.step3_nsigma_z_outliers);
    }
    
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  } 
 
  void lttc_algo::create_hit_clustering()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    // const step1_data & step1 = current_loop->step1;
    const step2_data & step2 = current_loop->step2;
    const step3_data & step3 = current_loop->step3;
    auto workingHC = step3.HC;
    // auto workingCQ = step3.CQ;
    size_t nhits = workingHC.shape()[0];
    size_t nclusters = workingHC.shape()[1];
    outdata->hit_clustering.hit_clustering_map.clear();
    for (int iHit = 0; iHit < (int) nhits; iHit++) {
      {
        std::set<int> emptySet;
        outdata->hit_clustering.hit_clustering_map[iHit] = emptySet;
      }
     }
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
        }
      }
      DT_LOG_DEBUG(cfg.logging, "cluster #" << icl << " has " << cluster_stats[icl] << " hits");
      if (cluster_stats[icl] < MIN_NUMBER_OF_HITS_IN_CLUSTER) {
        // std::cerr << "******* CRIT STAT CLUSTER ****" << '\n';
        cluster_status[icl] |= CQ_CRIT_STAT;
        DT_LOG_DEBUG(cfg.logging, " -> cluster #" << icl << " has critical stat");
        // Identify for short-arm clusters:
        int iHit1 = -1;
        int iHit2 = -1;
        size_t count = 0;
        for (int ihit = 0; ihit < (int) nhits; ihit++) {
          hit_cluster_association_data & hcadi = workingHC[ihit][icl];
          if (hcadi.association_status == HCAS_ASSOCIATED) {
            if (iHit1 < 0) {
              iHit1 = ihit;
            }
            if (iHit2 < 0) {
              iHit2 = ihit;
            }
            count++;
            if (count == 2) break;
          }
        }
        const tracker_hit & hit1 = indata->hits[iHit1];
        const tracker_hit & hit2 = indata->hits[iHit2];
        cell_id cellId1(hit1.side_id, hit1.layer_id, hit1.row_id);
        cell_id cellId2(hit2.side_id, hit2.layer_id, hit2.row_id);
        int dist12 = cellId1.distance(cellId2);
        bool shortArm = false;
        if (dist12 >= 0 and dist12 <=2) {
          shortArm = true;
        }
        if (shortArm) {
          std::cerr << "******* SHORT ARM CLUSTER ****" << '\n';
          cluster_status[icl] |= CQ_SHORT_ARM;
        }  
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
      std::cerr << "[debug] Clusters quality status:" << '\n';
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
    for (int iCluster = 0; iCluster < (int) nclusters; iCluster++) {
      const cluster * pcluster = step2.clusters[iCluster].pcluster;
      if (cluster_status[iCluster] & CQ_CRIT_STAT) {
        // if (cluster_status[iCluster] & CQ_SHORT_ARM) {
        //   DT_LOG_DEBUG(cfg.logging, "Preserve short-arm cluster #" << iCluster << " with critical stat");
        // } else {
        //   DT_LOG_DEBUG(cfg.logging, "Discard step-3 cluster #" << iCluster << " with critical stat");
        //   continue;
        // }
        DT_LOG_DEBUG(cfg.logging, "Discard cluster #" << iCluster << " with critical stat");
        continue;
      }
      // New cluster ID:
      int clusterId = (int) outdata->hit_clustering.clusters.size();
      DT_LOG_DEBUG(cfg.logging, "Creating cluster #" << clusterId << " from step2 cluster #" << iCluster);
      {
        hit_cluster_data hc;
        outdata->hit_clustering.clusters.push_back(hc);
      }
      hit_cluster_data & new_cluster = outdata->hit_clustering.clusters.back();
      new_cluster.step3_id = iCluster; // Origin cluster from step3 data
      new_cluster.id = clusterId; // ID
      new_cluster.flags = 0;
      if (pcluster->line_data.is_valid()) {
        new_cluster.line_data.theta = pcluster->line_data.get_theta();
        new_cluster.line_data.r = pcluster->line_data.get_r();
        new_cluster.line_data.theta_err = pcluster->t_err;
        new_cluster.line_data.r_err = pcluster->r_err;
      } /* else if (pcluster->circle_data.is_valid()) {
           }
        */
      compute_cluster_hits(new_cluster);
      compute_cluster_quality_fit(new_cluster);
      compute_cluster_quality_missing_hits(new_cluster);
      if (datatools::logger::is_debug(cfg.logging)) {
        std::cerr << "[debug] New step-3 cluster:\n";
        new_cluster.print(std::cerr, "[debug] ");
      }
    }
    DT_LOG_DEBUG(cfg.logging, "# step-3 clusters so far = " << outdata->hit_clustering.clusters.size());
    bool reprocessClusters = true;
    std::set<int> clustersToBeTrimmed;
    size_t loopCounter = 0;
    do {
      clusters_build_ordered_hits();
      clusters_detect_relationships();
      clusters_build_track_paths();
      clusters_build_ranks();      
      clustersToBeTrimmed = clusters_find_n_far_one();
      for (int iCluster = 0; iCluster < (int) outdata->hit_clustering.clusters.size(); iCluster++) {
        hit_cluster_data & hci = outdata->hit_clustering.clusters[iCluster];
        if (hci.track_path.z_trimmed.size()) {
          for (int iTrimmedVtx : hci.track_path.z_trimmed) {
            int iRemovedHit = hci.track_path.vertexes[iTrimmedVtx].hit_index;
            if (clusters_to_be_removed_hits.count(iCluster) == 0) {
              std::set<int> dummy;
              clusters_to_be_removed_hits[iCluster] = dummy;
            }
            clusters_to_be_removed_hits[iCluster].insert(iRemovedHit);
          }
          clustersToBeTrimmed.insert(iCluster);
        }
        hci.track_path.z_trimmed.clear();
      }
      if (clustersToBeTrimmed.size() == 0) {
        reprocessClusters = false;
      } else {
        DT_LOG_DEBUG(cfg.logging, "Need to reprocess clusters because of " << clustersToBeTrimmed.size() << " trimmable cluster(s)");
      }
      if (reprocessClusters) {
        clusters_trim();
        for (int iCluster = 0; iCluster < (int) outdata->hit_clustering.clusters.size(); iCluster++) {
          hit_cluster_data & hci = outdata->hit_clustering.clusters[iCluster];
          compute_cluster_hits(hci);
          compute_cluster_quality_fit(hci);
          compute_cluster_quality_missing_hits(hci);
        }
      }
      loopCounter++;
    } while (reprocessClusters);
    clusters_search_short_arms();
    // Identify unclustered hits:
    for (const auto & item : outdata->hit_clustering.hit_clustering_map) {
      if (item.second.size() == 0) {
        outdata->hit_clustering.unclustered_hits.insert(item.first);
      }
    }
    clusters_search_kinked();
    // associate_unclustered_hits();
     
    DT_LOG_DEBUG(cfg.logging, "Clusters are done : ");
    outdata->hit_clustering.print(std::cerr, "[debug] ");

    // if (datatools::logger::is_debug(cfg.logging)) {
    //   print_hc(workingHC, std::cerr, "[debug] Stage 2> ");
    //   // std::cerr << "Wait..." << std::endl;
    //   // std::string resp;
    //   // std::getline(std::cin, resp);
    // }
    
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  bool lttc_algo::hit_clustering_data::is_unclustered_hit(int ihit_) const
  {
    auto found = hit_clustering_map.find(ihit_);
    if (found == hit_clustering_map.end()) {
      DT_THROW(std::logic_error, "No hit #" << ihit_);
    }
    return found->second.size() == 0;
  }

  void lttc_algo::cluster_quality_data::clear()
  {
    *this = cluster_quality_data();
    return;
  }

  void lttc_algo::compute_cluster_quality_fit(hit_cluster_data & hc_)
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    hit_cluster_data & hitCluster = hc_;
    const std::set<int> & clusterExcludedHits = hitCluster.excluded_hits;
    cluster_quality_data & clusterQuality = hitCluster.quality;
    clusterQuality.clear();
    step3_data & step3 = current_loop->step3;
    for (int iHit = 0; iHit < (int) indata->hits.size(); iHit++) {
      if (clusterExcludedHits.count(iHit)) continue;
      const hit_cluster_association_data & hca = step3.HC[iHit][hitCluster.step3_id];
      if (hca.association_status != HCAS_ASSOCIATED) continue;
      if (! datatools::is_valid(clusterQuality.sumChi2)) {
        clusterQuality.sumChi2 = 0.0;
      }
      if (! datatools::is_valid(clusterQuality.noOutliersSumChi2)) {
        clusterQuality.noOutliersSumChi2 = 0.0;
      }
      clusterQuality.nhits++;
      clusterQuality.ndof++;
      clusterQuality.sumChi2 += hca.chi2;
      clusterQuality.hits.insert(iHit);
      if (std::abs(hca.residual) > cfg.step3_nsigma_outliers) {
        clusterQuality.outliers.insert(iHit);
      } else {
        clusterQuality.noOutliersNdof++;
        clusterQuality.noOutliersSumChi2 += hca.chi2;      
      }
    }
    if (clusterQuality.nhits) { 
      clusterQuality.ndof = clusterQuality.nhits;
      clusterQuality.pvalue = gsl_cdf_chisq_Q(clusterQuality.sumChi2, clusterQuality.ndof);
      clusterQuality.noOutliersPvalue = gsl_cdf_chisq_Q(clusterQuality.noOutliersSumChi2, clusterQuality.noOutliersNdof);
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
 
  void lttc_algo::compute_cluster_quality_rank(hit_cluster_data & hc_)
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    hit_cluster_data & hitCluster = hc_;
    cluster_quality_data & clusterQuality = hitCluster.quality;
    size_t badFlagCounter = 0;
    DT_LOG_DEBUG(cfg.logging, "Effective p-value = " << clusterQuality.get_effective_pvalue());
    DT_LOG_DEBUG(cfg.logging, "Effective # missing hits = " << clusterQuality.get_effective_number_of_missing_hits());
    DT_LOG_DEBUG(cfg.logging, "# of well fitted hits = " << (clusterQuality.hits.size() - clusterQuality.outliers.size()));
    if (clusterQuality.get_effective_pvalue() < cfg.step3_min_pvalue) {
      DT_LOG_DEBUG(cfg.logging, "Bad rank because of a low p-value.");
      badFlagCounter += 2;
    }
    if (clusterQuality.get_effective_number_of_missing_hits() >= cfg.step3_max_missing_hits) {
      DT_LOG_DEBUG(cfg.logging, "Bad rank because of a high number of missing hits : "
                   << clusterQuality.get_effective_number_of_missing_hits());
      badFlagCounter += 2;
    }
    if ((clusterQuality.get_effective_number_of_missing_hits() + 1) == cfg.step3_max_missing_hits) {
      DT_LOG_DEBUG(cfg.logging, "Poor rank because of a significant number of missing hits.");
      badFlagCounter += 1;
    }
    if ((clusterQuality.hits.size() - clusterQuality.outliers.size()) < 4) {
      // Remaining number of non-outliers hits is small:
      DT_LOG_DEBUG(cfg.logging, "Poor rank because of a small number of well fitted hits.");
      badFlagCounter += 1;
    }
    
    // Use SZ-fit informations:
    const track_path_data & trackPath = hitCluster.track_path;
    if (trackPath.get_effective_z_pvalue() < cfg.step3_min_pvalue) {
      DT_LOG_DEBUG(cfg.logging, "Bad rank because of a low Z p-value.");
      badFlagCounter += 2;
    }
    if ((trackPath.vertexes.size() - trackPath.z_trimmed.size()) < 3) {
      DT_LOG_DEBUG(cfg.logging, "Poor rank because of a small number of well Z-fitted hits.");
      badFlagCounter += 1;
    }
      
    clusterQuality.rank = CQR_GOOD;
    if (badFlagCounter == 1) {
      clusterQuality.rank = CQR_POOR;
    } else if (badFlagCounter >= 2 ) {
      clusterQuality.rank = CQR_BAD;
    }  
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
  
  void lttc_algo::clusters_trim()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    for (int iCluster = 0; iCluster < (int) outdata->hit_clustering.clusters.size(); iCluster++) {
      hit_cluster_data & hcl = outdata->hit_clustering.clusters[iCluster];
      if (clusters_to_be_removed_hits.count(iCluster)) {
        const std::set<int> & to_be_removed_hits = clusters_to_be_removed_hits.find(iCluster)->second; 
        DT_LOG_DEBUG(cfg.logging, "Number of removable hits for cluster #" << iCluster << " : "
                     << to_be_removed_hits.size());        
        for (int iHit : to_be_removed_hits) {
          DT_LOG_DEBUG(cfg.logging, "  Removing hit #" << iHit << " from cluster #" << iCluster);        
          hcl.hits.erase(iHit);
          hcl.excluded_hits.insert(iHit);
          hcl.track_path.z_trimmed.erase(iHit);
          outdata->hit_clustering.hit_clustering_map.find(iHit)->second.erase(iCluster);
        }
        clusters_to_be_removed_hits.erase(iCluster);
        DT_LOG_DEBUG(cfg.logging, "   Final #hits =" << hcl.hits.size());        
      }
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  std::set<int> lttc_algo::clusters_find_n_far_one()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    std::set<int> clustersToBeTrimmed;
    const double dcell = detector_desc->get_gg_locator().cellDiameter();
    for (int iCluster = 0; iCluster < (int) outdata->hit_clustering.clusters.size(); iCluster++) {
      hit_cluster_data & hcl = outdata->hit_clustering.clusters[iCluster];
      cluster_quality_data & cqData = hcl.quality;
      size_t nHits = hcl.hits.size();
      if (nHits < 4) continue;
      if (cqData.rank != CQR_BAD) continue;
      DT_LOG_DEBUG(cfg.logging, "Searching N-far-1 pattern for cluster #" << iCluster);
      // Give a second chance to some N-far-1 clusters with bad ranking:
      bool removeFirst = false;
      bool removeLast = false;
      const track_path_data & trackPath = hcl.track_path;
      size_t nbRemovableEndHits = 0;
      {
        const track_path_vertex & firstVertex = trackPath.vertexes[0];
        const track_path_vertex & secondVertex = trackPath.vertexes[1];
        // Distance in the XY plane:
        double distFirst = secondVertex.sxy - firstVertex.sxy;
        if (distFirst > 4 * dcell) {
          removeFirst = true;
        }
        
        // Attempt to remove the first hit if the cell is hit by another cluster: 
        // int firstHit = firstVertex.hit_index;
        
        const track_path_vertex & lastVertex = trackPath.vertexes[nHits-1];
        const track_path_vertex & lastButOneVertex = trackPath.vertexes[nHits-2];
        // Distance in the XY plane:
        double distLast = lastVertex.sxy - lastButOneVertex.sxy;
        if (distLast > 4 * dcell) {
          removeLast = true;
        }
      }
      if (removeFirst) {
        DT_LOG_DEBUG(cfg.logging, "  Remove first hit of cluster #" << iCluster);
        nbRemovableEndHits++;
      }
      if (removeLast) {
        DT_LOG_DEBUG(cfg.logging, "  Remove last hit of cluster #" << iCluster);
        nbRemovableEndHits++;
      }
      std::set<int> to_be_removed_hits;
      // {
      //   // Trimmed hits due to trimmed vertex after SZ-fit:
      //   for (auto trimmedVtxIndex : trackPath.z_trimmed) {
      //     const track_path_vertex & trimmedVertex = trackPath.vertexes[trimmedVtxIndex];
      //     to_be_removed_hits.insert(trimmedVertex.hit_index);
      //   }
      // }
      DT_LOG_DEBUG(cfg.logging, "  Nb of removable end hits = " << nbRemovableEndHits << " after trimming of cluster #" << iCluster);
      if (nbRemovableEndHits > 0) {
        if (removeFirst) {
          DT_LOG_DEBUG(cfg.logging, "  Registering removable first hit #" << hcl.end_hit_0);
          to_be_removed_hits.insert(hcl.end_hit_0);
        }
        if (removeLast) {
          DT_LOG_DEBUG(cfg.logging, "  Registering removable last hit #" << hcl.end_hit_1);
          to_be_removed_hits.insert(hcl.end_hit_1);
        }
      }
      size_t nHitsRemained = nHits - to_be_removed_hits.size();
      DT_LOG_DEBUG(cfg.logging, "  Remaining nb of hits = " << nHitsRemained << " after trimming of cluster #" << iCluster);
      if (nHitsRemained < 3) {
        DT_LOG_DEBUG(cfg.logging, "  Not enough remaining nb of hits after trimming of cluster #" << iCluster << " !");
      } else {
        if (to_be_removed_hits.size()) {
          clusters_to_be_removed_hits[iCluster] = to_be_removed_hits;
          clustersToBeTrimmed.insert(iCluster);
        }
      }
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return clustersToBeTrimmed;
  }

  void lttc_algo::clusters_build_ranks()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    for (int iCluster = 0; iCluster < (int) outdata->hit_clustering.clusters.size(); iCluster++) {
      hit_cluster_data & hcl = outdata->hit_clustering.clusters[iCluster];
      compute_cluster_quality_rank(hcl);
    }
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::clusters_search_short_arms()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    auto & HCD = outdata->hit_clustering;
    std::vector<int> uHits;
    std::copy(HCD.unclustered_hits.begin(), HCD.unclustered_hits.end(),
              std::back_inserter(uHits));
    typedef std::pair<int,int> short_arm_pair_type;
    std::vector<short_arm_pair_type> shortArmPairs;
    DT_LOG_DEBUG(cfg.logging, "Number of unclustered hits : " << uHits.size());
    for (size_t iHit1 = 0; iHit1 < uHits.size(); iHit1++) {
      const tracker_hit & hit1 = indata->hits[iHit1];
      cell_id cellId1(hit1.side_id, hit1.layer_id, hit1.row_id);
      for (size_t iHit2 = iHit1 + 1; iHit2 < uHits.size(); iHit2++) {
        const tracker_hit & hit2 = indata->hits[iHit2];
        cell_id cellId2(hit2.side_id, hit2.layer_id, hit2.row_id);
        int dist12 = cellId1.distance(cellId2);
        bool shortArm = false;
        if (dist12 >= 0 and dist12 <=2) {
          shortArm = true;
        }
        if (shortArm) {
          DT_LOG_DEBUG(cfg.logging, " Found a short arm pair : (" << iHit1 << ',' << iHit1 << ")");
          short_arm_pair_type shortArmPair{iHit1, iHit2};
          shortArmPairs.push_back(shortArmPair);
        }  
      }
    }
    DT_LOG_DEBUG(cfg.logging, "Number of short-arm pairs : " << shortArmPairs.size());
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
  
  void lttc_algo::clusters_search_kinked()
  {
    DT_LOG_DEBUG(cfg.logging, "Entering...");
    auto & HCD = outdata->hit_clustering;
    for (int iCluster = 0; iCluster < (int) HCD.clusters.size(); iCluster++) {
      hit_cluster_data & hci = HCD.clusters[iCluster];
      if (hci.flags & hit_cluster_data::FLAG_DISCARDED) {
        continue;
      }
      if (hci.quality.rank == CQR_BAD) {
        continue;
      }
      DT_LOG_DEBUG(cfg.logging, "Processing cluster #" << iCluster << "...");
      const track_path_data & trackPath = hci.track_path;
      point2 first = trackPath.first().center();
      point2 last  = trackPath.last().center();
      std::tuple<point3, double> first3 = trackPath.first3();
      std::tuple<point3, double> last3  = trackPath.last3();
      cell_id firstCellId;
      sntracker->locate(first, firstCellId); // XXX
      cell_id lastCellId;
      sntracker->locate(last, lastCellId); // XXX
      DT_LOG_DEBUG(cfg.logging, "firstCellId=" << firstCellId);
      DT_LOG_DEBUG(cfg.logging, "lastCellId=" << lastCellId);
      for (int iCluster2 = iCluster + 1; iCluster2 < (int) HCD.clusters.size(); iCluster2++) {
        hit_cluster_data & oHci = HCD.clusters[iCluster2];
        if (oHci.flags & hit_cluster_data::FLAG_DISCARDED) {
          continue;
        }
        if (oHci.quality.rank == CQR_BAD) {
          continue;
        }
        if (hci.twins.count(iCluster2)) {
          continue;
        }
        if (hci.superset_of.count(iCluster2)) {
          continue;
        }
        if (hci.subset_of.count(iCluster2)) {
          continue;
        }
        if (hci.overlap_with.count(iCluster2)) {
          continue;
        }
        DT_LOG_DEBUG(cfg.logging, "  -> checking with cluster #" << iCluster2 << "...");
        const track_path_data & oTrackPath = oHci.track_path;
        point2 oFirst = oTrackPath.first().center();
        point2 oLast  = oTrackPath.last().center();
        std::tuple<point3, double> oFirst3 = oTrackPath.first3();
        std::tuple<point3, double> oLast3  = oTrackPath.last3();
        cell_id oFirstCellId;
        sntracker->locate(oFirst, oFirstCellId);
        cell_id oLastCellId;
        sntracker->locate(oLast, oLastCellId);
        DT_LOG_DEBUG(cfg.logging, "  oFirstCellId =" << oFirstCellId);
        DT_LOG_DEBUG(cfg.logging, "  oLastCellId  =" << oLastCellId);
        track_path_interceptor TPI(track_path_interceptor::MODE_LINE, cfg.logging);
        track_path_intercept clusterIntercept;
        bool success = TPI.find(trackPath, oTrackPath, clusterIntercept);
        if (success) {
          DT_LOG_DEBUG(cfg.logging, "  Found intercept @ " << clusterIntercept.intercept
                       << " with pattern=" << clusterIntercept.pattern);
          DT_LOG_DEBUG(cfg.logging, "   and s_first=" << clusterIntercept.s_first
                       << " s_second=" << clusterIntercept.s_second);
          if ((clusterIntercept.pattern & TPIB_AM) or
              (clusterIntercept.pattern & TPIB_BM)) {
            // Reject intercept if in the middle of one track (A or B)
            // and thus accept only extrapolated vertex from one or the other track's end
            continue;
          }
          if (! sntracker->contains(clusterIntercept.intercept)) {
            DT_LOG_DEBUG(cfg.logging, "  Intercept is not in the tracker!");
            // Intercept is not in the tracker:
            continue;
          }
          bool kinked_ok = true;
          kinked_track_data kinkedTrackData;
          kinkedTrackData.first_cluster  = iCluster;
          kinkedTrackData.second_cluster = iCluster2;
          kinkedTrackData.kink  = clusterIntercept.intercept;
          cell_id interceptId;
          if (sntracker->locate(clusterIntercept.intercept, interceptId)) {
            DT_LOG_DEBUG(cfg.logging, "  Intercept is in cell ID = " << interceptId);
            kinkedTrackData.kink_cell_id = interceptId;
            if (interceptId.side() != hci.side) {
              DT_LOG_DEBUG(cfg.logging, "  Intercept is not on the right side!");
              // Intercept is not in the same side than the cluster, so we consider
              // this cannot be a single track.
              continue;
            }
          } else {
            DT_LOG_DEBUG(cfg.logging, "  Intercept is not in a cell");
            // Not in a cell:
            if (clusterIntercept.intercept.x() < 0.0 and hci.side == 1) {
              DT_LOG_DEBUG(cfg.logging, "  Italy intercept but France cluster");
              // Italy intercept but France cluster:
              continue;
            }
            if (clusterIntercept.intercept.x() > 0.0 and hci.side == 0) {
              DT_LOG_DEBUG(cfg.logging, "  France intercept but Italy cluster");
              // France intercept but Italy cluster:
              continue;
            }
          }

          // Here we compute the number of missing cells along the extrapolated track paths
          // and use it as a rejection criterion:
          std::tuple<point3, double> firstVertex;
          std::tuple<point3, double> secondVertex;
          std::vector<cluster_missing_hit_data> missingHitsA;
          std::vector<cluster_missing_hit_data> missingHitsB;
          if (clusterIntercept.pattern & TPIB_AF) {
            missing_hits_finder mhf(*this,
                                    indata->hits,
                                    hit_cells,
                                    hci.hits);
            mhf.logging = cfg.logging;
            mhf.find(first, clusterIntercept.intercept, missingHitsA);
            DT_LOG_DEBUG(cfg.logging, "  Found " << missingHitsA.size()
                         << " missing hits from first vertex to kink of the linear cluster ID=" << hci.id);
            firstVertex = trackPath.first3();
            kinkedTrackData.first_cluster_from = kinked_track_data::FROM_FIRST;
            kinkedTrackData.first_cluster_vertex = first;
            kinkedTrackData.z_first     = std::get<0>(first3).z();
            kinkedTrackData.z_first_err = std::get<1>(first3);
          } else if (clusterIntercept.pattern & TPIB_AL) {
            missing_hits_finder mhf(*this,
                                    indata->hits,
                                    hit_cells,
                                    hci.hits);
            mhf.logging = cfg.logging;
            mhf.find(last, clusterIntercept.intercept, missingHitsA);
            DT_LOG_DEBUG(cfg.logging, "  Found " << missingHitsA.size()
                         << " missing hits from last vertex to kink of the linear cluster ID=" << hci.id);
            firstVertex = trackPath.last3();
            kinkedTrackData.first_cluster_from = kinked_track_data::FROM_LAST;
            kinkedTrackData.first_cluster_vertex = last;
            kinkedTrackData.z_first     = std::get<0>(last3).z();
            kinkedTrackData.z_first_err = std::get<1>(last3);
          } else {
            DT_LOG_DEBUG(cfg.logging, "  => rejected intercept for track A!");
            kinked_ok = false;
            continue;
          }
          if (clusterIntercept.pattern & TPIB_BF) {
            missing_hits_finder mhf(*this,
                                    indata->hits,
                                    hit_cells,
                                    oHci.hits);
            mhf.logging = cfg.logging;
            mhf.find(oFirst, clusterIntercept.intercept, missingHitsB);
            DT_LOG_DEBUG(cfg.logging, "  Found " << missingHitsB.size()
                         << " missing hits from first vertex to kink of the linear cluster ID=" << oHci.id);
            secondVertex = oTrackPath.first3();
            kinkedTrackData.second_cluster_from = kinked_track_data::FROM_FIRST;
            kinkedTrackData.second_cluster_vertex = oFirst;
            kinkedTrackData.z_second     = std::get<0>(oFirst3).z();
            kinkedTrackData.z_second_err = std::get<1>(oFirst3);
         } else if (clusterIntercept.pattern & TPIB_BL) {
            missing_hits_finder mhf(*this,
                                    indata->hits,
                                    hit_cells,
                                    oHci.hits);
            mhf.logging = cfg.logging;
            mhf.find(oLast, clusterIntercept.intercept, missingHitsB);
            DT_LOG_DEBUG(cfg.logging, "  Found " << missingHitsB.size()
                         << " missing hits from last vertex to kink of the linear cluster ID=" << oHci.id);
            secondVertex = oTrackPath.last3();
            kinkedTrackData.second_cluster_from = kinked_track_data::FROM_FIRST;
            kinkedTrackData.second_cluster_vertex = oLast;
            kinkedTrackData.z_second     = std::get<0>(oLast3).z();
            kinkedTrackData.z_second_err = std::get<1>(oLast3);
          } else {
            DT_LOG_DEBUG(cfg.logging, "  => rejected intercept for track A!");
            kinked_ok = false;
            continue;
          }

          size_t nbTrueMissingHits = 0;
          if (missingHitsA.size()) {
            DT_LOG_DEBUG(cfg.logging, "Missing hits for track A : ");
            std::string indent = "[debug] ";
            for (int i = 0; i < (int) missingHitsA.size(); i++) {
              const cluster_missing_hit_data & cmh = missingHitsA[i];
              if (cmh.why == MCR_NO_TRIGGER) {
                nbTrueMissingHits++;
              }
              if (datatools::logger::is_debug(cfg.logging)) {
                std::cerr << indent;
                if ((i + 1) == (int) missingHitsA.size()) {
                  std::cerr << "`-- ";
                } else {
                  std::cerr << "|-- ";
                }
                std::cerr << "#" << i << " : " << cmh << '\n';
              }
            }
          }
          
          if (missingHitsB.size()) {
            DT_LOG_DEBUG(cfg.logging, "Missing hits for track B : ");
            std::string indent = "[debug] ";
            for (int i = 0; i < (int) missingHitsB.size(); i++) {
              const cluster_missing_hit_data & cmh = missingHitsB[i];
              if (cmh.why == MCR_NO_TRIGGER) {
                nbTrueMissingHits++;
              }
              if (datatools::logger::is_debug(cfg.logging)) {
                std::cerr << indent;
                if ((i + 1) == (int) missingHitsB.size()) {
                  std::cerr << "`-- ";
                } else {
                  std::cerr << "|-- ";
                }
                std::cerr << "#" << i << " : " << cmh << '\n';
              }
            }
          }
         
          if (nbTrueMissingHits > 0) {
            DT_LOG_DEBUG(cfg.logging, "  => rejected kink because too many missing hits!");
            kinked_ok = false;
            continue;       
          }
          
          if (kinked_ok) {
            DT_LOG_DEBUG(cfg.logging, "  Check for Z matching:");
            // Check for Z matching:
            double zFirstAtKink;
            double zFirstAtKinkErr;
            double zSecondAtKink;
            double zSecondAtKinkErr;
            double sFirst = clusterIntercept.s_first;
            trackPath.compute_z_and_error(sFirst, zFirstAtKink, zFirstAtKinkErr);
            double sSecond = clusterIntercept.s_second;
            oTrackPath.compute_z_and_error(sSecond, zSecondAtKink, zSecondAtKinkErr);
            double dataZ[2] = {zFirstAtKink, zSecondAtKink};
            double weightZ[2] = {1. / gsl_pow_2(zFirstAtKinkErr), 1. / gsl_pow_2(zSecondAtKinkErr)};
            double zKink     = gsl_stats_wmean(weightZ, 1, dataZ, 1, 2);
            double zKinkErr  = gsl_stats_wsd_m(weightZ, 1, dataZ, 1, 2, zKink);
            double zKinkChi2 = gsl_stats_wtss_m(weightZ, 1, dataZ, 1, 2, zKink);
            size_t zKinkNdof = 1;
            double zKinkPvalue = gsl_cdf_chisq_Q(zKinkChi2, zKinkNdof);
            DT_LOG_DEBUG(cfg.logging, "  zFirst@kink  = " << zFirstAtKink << " +/- " << zFirstAtKinkErr);
            DT_LOG_DEBUG(cfg.logging, "  zSecond@kink = " << zSecondAtKink << " +/- " << zSecondAtKinkErr);
            DT_LOG_DEBUG(cfg.logging, "  zKink        = " << zKink << " +/- " << zKinkErr);
            DT_LOG_DEBUG(cfg.logging, "  zKinkChi2    = " << zKinkChi2 << " ndof=" << zKinkNdof);
            DT_LOG_DEBUG(cfg.logging, "  zKinkPvalue  = " << zKinkPvalue);
            if (zKinkPvalue < cfg.step3_kink_z_min_pvalue) {
              DT_LOG_DEBUG(cfg.logging, "  => rejected because of unmatching Z at kink!");
              kinked_ok = false;
            } else {
              // kinkedTrackData.z_first      = zFirstAtKink;
              // kinkedTrackData.z_first_err  = zFirstAtKinkErr; 
              // kinkedTrackData.z_second     = zSecondAtKink;
              // kinkedTrackData.z_second_err = zSecondAtKinkErr;
              kinkedTrackData.z            = zKink;
              kinkedTrackData.z_err        = zKinkErr;
              kinkedTrackData.z_pvalue     = zKinkPvalue;
            }
          }

          if (kinked_ok) {
            vector2 FK = kinkedTrackData.kink - kinkedTrackData.first_cluster_vertex;
            vector2 KL = kinkedTrackData.second_cluster_vertex - kinkedTrackData.kink;
            double angle = FK.angle(KL);
            if (angle > cfg.max_kink_xy_angle) {
              DT_LOG_DEBUG(cfg.logging, "  => rejected because of a too large kink angle!");
              kinked_ok = false;
            }
          }
          
          if (kinked_ok) {
            DT_LOG_DEBUG(cfg.logging, "  Found 'kinked' possible merge between cluster #"
                         << iCluster << " and cluster #" << iCluster2);
            kinkedTrackData.id = (int) HCD.kinked_tracks.size();
            HCD.kinked_tracks.push_back(kinkedTrackData);
          } else {
            DT_LOG_DEBUG(cfg.logging, "  No valid intercept was found!");
          }
        } // intercept success
      } // cluster2 loop
    } // cluster loop
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }

  void lttc_algo::enrich_clusters()
  {
    // for (int iCluster = 0; iCluster < (int) hit_clustering.clusters.size(); iCluster++) {
    //   const hit_cluster_data & hcl = hit_clustering.clusters[iCluster];
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
      point2 cellPos = sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
      for (int icl = 0; icl < (int)  outdata->hit_clustering.clusters.size(); icl++) {
        hit_cluster_data & hcl =  outdata->hit_clustering.clusters[icl];
        // const line & cline = hcl.line_data;
        line cline = line2::make_from_theta_r(hcl.line_data.theta, hcl.line_data.r);
        double rdist = cline.distance(cellPos);
        // bool associated = false;
        if (rdist < (h.drift_radius + 3 * h.drift_radius_err))  {
          // The hit is on the cluster's line path:
          int end_hit_0  = hcl.end_hit_0;
          int end_hit_1  = hcl.end_hit_1;
          const tracker_hit & h1 = indata->hits[end_hit_0]; 
          const tracker_hit & h2 = indata->hits[end_hit_1];
          point2 cellPos1 = sntracker->cell_position(h1.side_id, h1.layer_id, h1.row_id);
          point2 cellPos2 = sntracker->cell_position(h2.side_id, h2.layer_id, h2.row_id);
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

  // void lttc_algo::hit_cluster_data::build_track_path(track_path_data & track_path_,
  //                                                    const tracker_hit_collection & hits_) const
  // {
  //   // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Cluster ID=" << this->id);      
  //   // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "  => track ordered hits : " << track_ordered_hits.size());  
  //   track_path_.vertexes.clear();
  //   track_path_.vertexes.reserve(hits.size());
  //   for (size_t i = 0; i < track_ordered_hits.size(); i++) {
  //     track_path_vertex tpv;
  //     tpv.hit_index = track_ordered_hits[i];
  //     tpv.node  = hit_associations.find(track_ordered_hits[i])->second.node;
  //     tpv.z     = hits_[tpv.hit_index].z;
  //     tpv.z_err = hits_[tpv.hit_index].z_err;
  //     track_path_.vertexes.push_back(tpv);
  //   }
  //   return;
  // }

  void lttc_algo::compute_cluster_track_path(hit_cluster_data & hc_)
  {
    track_path_data & track_path = hc_.track_path;
    const tracker_hit_collection & inHits = indata->hits;
    track_path.vertexes.clear();
    track_path.vertexes.reserve(hc_.track_ordered_hits.size());
    for (size_t i = 0; i < hc_.track_ordered_hits.size(); i++) {
      track_path_vertex tpv;
      tpv.hit_index = hc_.track_ordered_hits[i];
      tpv.node  = hc_.hit_associations.find(hc_.track_ordered_hits[i])->second.node;
      tpv.z     = inHits[tpv.hit_index].z;
      tpv.z_err = inHits[tpv.hit_index].z_err;
      track_path.vertexes.push_back(tpv);
    }
    return;
  }

  void lttc_algo::cluster_add_hit(const int ihit_, hit_cluster_data & hc_)
  {
    const tracker_hit & aHit = indata->hits[ihit_];
    if (hc_.hits.size() == 0) {
      hc_.side = aHit.side_id;
      if (aHit.delayed) {
        hc_.delayed = true;
      }
      hc_.end_hit_0 = ihit_;
      hc_.end_hit_1 = ihit_;
    } else {
      DT_THROW_IF(aHit.delayed != hc_.delayed,
                  std::logic_error,
                  "Cannot add hit #" << ihit_ << " with unmatching delayed flag in cluster ID=" << hc_.id << '!');
      DT_THROW_IF(aHit.side_id != hc_.side,
                  std::logic_error,
                  "Cannot add hit #" << ihit_ << " with unmatching side " << aHit.side_id
                  << " in cluster ID=" << hc_.id << " at side " << hc_.side << '!');
      const tracker_hit & h1 = indata->hits[hc_.end_hit_0]; 
      const tracker_hit & h2 = indata->hits[hc_.end_hit_1];
      point2 cellPos1(h1.x, h1.y); // = sntracker->cell_position(h1.side_id, h1.layer_id, h1.row_id);
      point2 cellPos2(h2.x, h2.y); // = sntracker->cell_position(h2.side_id, h2.layer_id, h2.row_id);
      point2 cellPos(aHit.x, aHit.y); // = sntracker->cell_position(aHit.side_id, aHit.layer_id, aHit.row_id);
      double d12 = std::hypot(cellPos2.x() - cellPos1.x(), cellPos2.y() - cellPos1.y());
      double d1 = std::hypot(cellPos.x() - cellPos1.x(), cellPos.y() - cellPos1.y());
      double d2 = std::hypot(cellPos.x() - cellPos2.x(), cellPos.y() - cellPos2.y());
      if (d1 > d12 and d1 >= d2) {
        hc_.end_hit_1 = ihit_;
      } else if (d2 > d12 and d2 >= d1) {
        hc_.end_hit_0 = ihit_;
      }
    }
    hc_.hits.insert(ihit_);
    // Update the hit->cluster map:
    outdata->hit_clustering.hit_clustering_map.find(ihit_)->second.insert(hc_.id);
    return;
  }
  
  bool lttc_algo::hit_cluster_data::is_discarded() const
  {
    return flags & FLAG_DISCARDED;
  }

  bool lttc_algo::hit_cluster_data::has_twins() const
  {
    return twins.size();
  }
  
  bool lttc_algo::hit_cluster_data::is_distinct_of(int icluster_) const
  {
    if (twins.count(icluster_)) return false;
    if (superset_of.count(icluster_)) return false;
    if (subset_of.count(icluster_)) return false;
    if (overlap_with.count(icluster_)) return false;
    return true;
  }

  void lttc_algo::kinked_track_data::print(std::ostream & out_,
                                           const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "ID : " << id << '\n';
    out_ << indent_ << "|-- " << "First cluster  : " << first_cluster << '\n';
    out_ << indent_ << "|   " << "|-- " << "From : " << first_cluster_from  << '\n';
    out_ << indent_ << "|   " << "`-- " << "Vertex : " << first_cluster_vertex  << '\n';
    out_ << indent_ << "|-- " << "Second cluster : " << second_cluster << '\n';
    out_ << indent_ << "|   " << "|-- " << "From : " << second_cluster_from  << '\n';
    out_ << indent_ << "|   " << "`-- " << "Vertex : " << second_cluster_vertex  << '\n';
    out_ << indent_ << "|-- " << "Kink : " << kink << '\n';
    out_ << indent_ << "|   " << "`-- " << "Cell ID : " << kink_cell_id << '\n';
    out_ << indent_ << "|-- " << "Z : " << z << '\n';
    out_ << indent_ << "|-- " << "Z error: " << z_err << '\n';
    out_ << indent_ << "`-- " << "Z p-value: " << z_pvalue << '\n';
    return;
  }
      
  void lttc_algo::kinked_track_data::draw(std::ostream & out_, int tag_) const
  {
    double spotDim  = 2.0 * CLHEP::mm;
    double spotDim2 = 1.5 * CLHEP::mm;
    double spotDim3 = 1.0 * CLHEP::mm;
    
    out_ << "#@kinked_track_data.id=" << id << '\n';
    out_ << (kink.x() + spotDim) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << (kink.y() + spotDim) << ' ' << z << ' ' << tag_ << '\n';
    out_ << (kink.x() - spotDim) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << (kink.y() - spotDim) << ' ' << z << ' ' << tag_ << '\n';
    out_ << (kink.x() + spotDim) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << '\n';
    
    out_ << (kink.x() + spotDim2) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << (kink.y() + spotDim2) << ' ' << z << ' ' << tag_ << '\n';
    out_ << (kink.x() - spotDim2) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << (kink.y() - spotDim2) << ' ' << z << ' ' << tag_ << '\n';
    out_ << (kink.x() + spotDim2) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << '\n';
    
    out_ << (kink.x() + spotDim3) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << (kink.y() + spotDim3) << ' ' << z << ' ' << tag_ << '\n';
    out_ << (kink.x() - spotDim3) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << (kink.y() - spotDim3) << ' ' << z << ' ' << tag_ << '\n';
    out_ << (kink.x() + spotDim3) << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << '\n';
    
    out_ << kink.x() << ' ' << kink.y() << ' ' << (z - z_err) << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << kink.y() << ' ' << (z + z_err) << ' ' << tag_ << '\n';
    out_ << '\n';
    
    out_ << (first_cluster_vertex.x() + spotDim) << ' ' << first_cluster_vertex.y() << ' ' << z_first << ' ' << tag_ << '\n';
    out_ << first_cluster_vertex.x() << ' ' << (first_cluster_vertex.y() + spotDim) << ' ' << z_first << ' ' << tag_ << '\n';
    out_ << (first_cluster_vertex.x() - spotDim) << ' ' << first_cluster_vertex.y() << ' ' << z_first << ' ' << tag_ << '\n';
    out_ << first_cluster_vertex.x() << ' ' << (first_cluster_vertex.y() - spotDim) << ' ' << z_first << ' ' << tag_ << '\n';
    out_ << (first_cluster_vertex.x() + spotDim) << ' ' << first_cluster_vertex.y() << ' ' << z_first << ' ' << tag_ << '\n';
    out_ << '\n';
    
    out_ << (second_cluster_vertex.x() + spotDim) << ' ' << second_cluster_vertex.y() << ' ' << z_second << ' ' << tag_ << '\n';
    out_ << second_cluster_vertex.x() << ' ' << (second_cluster_vertex.y() + spotDim) << ' ' << z_second << ' ' << tag_ << '\n';
    out_ << (second_cluster_vertex.x() - spotDim) << ' ' << second_cluster_vertex.y() << ' ' << z_second << ' ' << tag_ << '\n';
    out_ << second_cluster_vertex.x() << ' ' << (second_cluster_vertex.y() - spotDim) << ' ' << z_second << ' ' << tag_ << '\n';
    out_ << (second_cluster_vertex.x() + spotDim) << ' ' << second_cluster_vertex.y() << ' ' << z_second << ' ' << tag_ << '\n';
    out_ << '\n';
    
    out_ << first_cluster_vertex.x() << ' ' << first_cluster_vertex.y() << ' ' << z_first << ' ' << tag_ << '\n';
    out_ << kink.x() << ' ' << kink.y() << ' ' << z << ' ' << tag_ << '\n';
    out_ << second_cluster_vertex.x() << ' ' << second_cluster_vertex.y() << ' ' << z_second << ' ' << tag_ << '\n';
    out_ << '\n';
    return;
  }
  
  void lttc_algo::hit_cluster_data::print(std::ostream & out_,
                                          const std::string & indent_) const
  {
    out_ << indent_ << "|-- " << "ID : " << id << '\n';
    out_ << indent_ << "|-- " << "Side : " << side << '\n';
    out_ << indent_ << "|-- " << "Delayed : " << std::boolalpha << delayed << '\n';
    out_ << indent_ << "|-- " << "Flags : " << flags << '\n';
    out_ << indent_ << "|   " << "`-- " << "Discarded : " << std::boolalpha << ((flags & FLAG_DISCARDED) == 0x1) << '\n';
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
    
    if (twins.size()) {
      out_ << indent_ << "|-- " << "Twins clusters : " << '\n';
      int count = 0;
      for (int itwin : twins) {
        out_ << indent_ << "|   ";
        if ((count+1) == (int) twins.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Twin cluster #" << itwin << '\n';
        count++;
      }
    }

    if (superset_of.size()) {
      out_ << indent_ << "|-- " << "Superset of : \n";
      int count = 0;
      for (int i : superset_of) {
        out_ << indent_ << "|   ";
        if ((count+1) == (int) superset_of.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Superset of cluster #" << i << '\n';
        count++;
      }
    }

    if (subset_of.size()) {
      out_ << indent_ << "|-- " << "Subset of : \n";
      int count = 0;
      for (int i : subset_of) {
        out_ << indent_ << "|   ";
        if ((count+1) == (int) subset_of.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Subset of cluster #" << i << '\n';
        count++;
      }
    }

    if (overlap_with.size()) {
      out_ << indent_ << "|-- " << "Overlapping : \n";
      int count = 0;
      for (int i : overlap_with) {
        out_ << indent_ << "|   ";
        if ((count+1) == (int) overlap_with.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Overlap with cluster #" << i << '\n';
        count++;
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
    out_ << indent_ << "|-- " << "End hit 0 (first) : " << end_hit_0 << '\n';
    out_ << indent_ << "|-- " << "End hit 1 (last)  : " << end_hit_1 << '\n';
    out_ << indent_ << "|-- " << "Track path  : " << '\n';
    track_path.print(out_, indent_ + "|   ");

    if (excluded_hits.size()) {
      out_ << indent_ << "|-- " << "Excluded hits : \n";
      int count = 0;
      for (int i : excluded_hits) {
        out_ << indent_ << "|   ";
        if ((count+1) == (int) excluded_hits.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Hit #" << i << '\n';
        count++;
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
      int count = 0;
      for (int i : degenerated_clusters) {
        out_ << indent_ << "|   ";
        if ((count+1) == (int) degenerated_clusters.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Degenerated cluster #" << i << '\n';
        count++;
      }
    }

    out_ << indent_ << "|-- " << "Kinked tracks: " << kinked_tracks.size() << '\n';
    {
      int count = 0;
      for (const kinked_track_data & kTrk : kinked_tracks) {
        std::ostringstream itemIndent;
        itemIndent << indent_ << "|   ";
        out_ << indent_ << "|   ";
        if ((count+1) == (int) kinked_tracks.size()) {
          out_ << "`-- ";
          itemIndent << indent_ << "    ";
        } else {
          out_ << "|-- ";
          itemIndent << indent_ << "|   ";
        }
        out_ << "Kinked track #" << count << '\n';
        kTrk.print(out_, itemIndent.str());
        count++;
      }
    }
     
    out_ << indent_ << "`-- " << "Unclustered hits: " << unclustered_hits.size() << '\n';
    {
      int count = 0;
      for (const auto & ihit : unclustered_hits) {
        out_ << indent_ << "    ";
        if ((count + 1) == (int) unclustered_hits.size()) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        out_ << "Hit #" << ihit << '\n' ;      
        count++;
      }
    }
    return;
  }

  void lttc_algo::hit_clustering_data::clear()
  {
    clusters.clear();
    degenerated_clusters.clear();
    kinked_tracks.clear();
    hit_clustering_map.clear();
    unclustered_hits.clear();
    return;
  }
     
  void lttc_algo::hit_clustering_data::draw(std::ostream & out_, const lttc_algo & algo_) const
  {
    double skip1 = 2.0 * CLHEP::mm;
    // double skip2 = 4.0 * CLHEP::mm;
    const double rcell = algo_.detector_desc->get_gg_locator().cellRadius();
    double ercell = rcell - skip1;
    // double ercell2 = rcell - skip2;
    out_ << "#@unclustered_hits=" << unclustered_hits.size() << '\n';
    if (unclustered_hits.size()) {
      size_t count = 0;
      for (const auto & ihit : unclustered_hits) {
        const tracker_hit & h = algo_.indata->hits[ihit];
        point2 cellPos(h.x, h.y); // = algo_.sntracker->cell_position(h.side_id, h.layer_id, h.row_id);
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << h.z << '\n';
        if (count == 0) {
          out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << h.z << '\n';
        }
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() + ercell << ' ' << h.z << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() + ercell << ' ' << h.z << '\n';
        out_ << cellPos.x() + ercell << ' ' << cellPos.y() - ercell << ' ' << h.z << '\n';
        out_ << cellPos.x() - ercell << ' ' << cellPos.y() - ercell << ' ' << h.z << '\n';
        out_ << '\n';
        count++;
      }
    } else {
      out_ << 0.0/0.0 << ' ' << 0.0/0.0 << '\n';
      out_ << '\n';
    }
    out_ << '\n';
    std::set<int> drawnClusters;
    {
      size_t counter = 0;
      for (int icl = 0; icl < (int) clusters.size(); icl++) {
        const hit_cluster_data & hcl = clusters[icl];
        if (hcl.is_discarded()) {
          continue;
        }
        if (hcl.quality.rank == CQR_BAD) {
          continue;
        }
        drawnClusters.insert(icl);
        counter++;
      }
    }
    out_ << "#@nb_hit_clusters=" << drawnClusters.size() << '\n';
    int iskip = 0;
    for (int icl = 0; icl < (int) clusters.size(); icl++) {
      const hit_cluster_data & hcl = clusters[icl];
      if (! drawnClusters.count(icl)) {
        continue;
      }
      out_ << "#@hit_cluster#=" << icl << '\n';
      for (auto ihit : hcl.hits) {
        const tracker_hit & h = algo_.indata->hits[ihit];
        cell_id cid(h.side_id, h.layer_id, h.row_id);
        out_ << "#@=>hit_cell_id#=" << cid << '\n';
        point2 cellPos(h.x, h.y); // = algo_.sntracker->cell_position(cid);
        double dimcell  = algo_.detector_desc->get_gg_locator().cellRadius() - (iskip * 1.0 * CLHEP::mm) - skip1;
        out_ << cellPos.x() - dimcell << ' ' << cellPos.y() - dimcell << ' ' << h.z << ' ' << icl << '\n';
        out_ << cellPos.x() - dimcell << ' ' << cellPos.y() + dimcell << ' ' << h.z << ' ' << icl << '\n';
        out_ << cellPos.x() + dimcell << ' ' << cellPos.y() + dimcell << ' ' << h.z << ' ' << icl << '\n';
        out_ << cellPos.x() + dimcell << ' ' << cellPos.y() - dimcell << ' ' << h.z << ' ' << icl << '\n';
        out_ << cellPos.x() - dimcell << ' ' << cellPos.y() - dimcell << ' ' << h.z << ' ' << icl << '\n';
        out_ << '\n';
        // if (ihit == hcl.end_hit_0 || ihit == hcl.end_hit_1) {
        //   out_ << cellPos.x() - dimcell2 << ' ' << cellPos.y() - dimcell2 << " 0" << ' ' << 1+icl << '\n';
        //   out_ << cellPos.x() - dimcell2 << ' ' << cellPos.y() + dimcell2 << " 0" << ' ' << 1+icl << '\n';
        //   out_ << cellPos.x() + dimcell2 << ' ' << cellPos.y() + dimcell2 << " 0" << ' ' << 1+icl << '\n';
        //   out_ << cellPos.x() + dimcell2 << ' ' << cellPos.y() - dimcell2 << " 0" << ' ' << 1+icl << '\n';
        //   out_ << cellPos.x() - dimcell2 << ' ' << cellPos.y() - dimcell2 << " 0" << ' ' << 1+icl << '\n';
        //   out_ << '\n';
        // }
        const hit_cluster_association_data & hcai = hcl.hit_associations.find(ihit)->second;
        const fitted_point2 & nodei = hcai.node;
        if (nodei.is_valid()) {
          out_ << "#@=>hit_node" << '\n';
          nodei.draw(out_, 1.0, false, h.z, icl);
          // nodei.draw(out_, 2.0, true, h.z, icl);
        }
      }
      // {
      //   const fitted_line2 & ld = hcl.line_data;
      //   line ld2 = line2::make_from_theta_r(ld.theta, ld.r);
      //   polyline2 ld_samples;
      //   double t1 = -1500.0;
      //   double t2 = +1500.0;
      //   ld2.generate_samples(t1, t2, ld_samples);
      //   draw_polyline(out_, ld_samples, 1+icl);
      //   // out_ << '\n';
      // }
      out_ << '\n';     
      iskip++;
    }
    out_ << '\n';     
    out_ << "#@nb_kinked_tracks=" << kinked_tracks.size() << '\n';
    for (size_t ikt = 0; ikt < kinked_tracks.size(); ikt++) {
      out_ << "#@kinked_track-" << ikt << '\n';
      kinked_tracks[ikt].draw(out_, ikt);
    }
    out_ << '\n';
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
    if (cfg.mode == MODE_CIRCLE) {
      DT_LOG_DEBUG(cfg.logging, "MODE_CIRCLE");
      lac.lttc = this;
      lac.prepare();
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
    
    if (cfg.mode == MODE_LINE) {
      DT_LOG_DEBUG(cfg.logging, " ==================== Step-1 =========================");
      step1_run();
      DT_LOG_DEBUG(cfg.logging, " ==================== Step-1 Done ====================");
      
      DT_LOG_DEBUG(cfg.logging, " ==================== Step-2 =========================");
      step2_run();
      DT_LOG_DEBUG(cfg.logging, " ==================== Step-2 Done ====================");
      
      DT_LOG_DEBUG(cfg.logging, " ==================== Step-3 =========================");
      step3_run();
      DT_LOG_DEBUG(cfg.logging, " ==================== Step-3 Done ====================");

      if (datatools::logger::is_debug(cfg.logging)) {
        print_hc(current_loop->step3.HC, std::cerr, "[debug] ");
        // std::cerr << "Wait..." << std::endl;
        // std::string resp;
        // std::getline(std::cin, resp);
      }
      
      create_hit_clustering();
      
      if (datatools::logger::is_debug(cfg.logging)) {
        current_loop->print(std::cerr, "[debug] ");
        std::cerr << "[debug] Hit clustering : \n";
        outdata->hit_clustering.print(std::cerr, "[debug] ");
      }
    }

    if (cfg.mode == MODE_CIRCLE) {
      DT_LOG_DEBUG(cfg.logging, " ==================== Circle Steps ====================");
      lac.do_steps();
      DT_LOG_DEBUG(cfg.logging, " ==================== Circle Done ====================");
    }

    // End of the current loop:
    current_loop = nullptr;
    loop_counter++;
    return;
  }

  void lttc_algo::_prepare_working_data_()
  {
    DT_LOG_DEBUG(cfg.logging, "Preparing working data...");
    outdata->hit_clustering.clear();
    hit_cells.clear();
    clusters_to_be_removed_hits.clear();
    for (int iHit= 0; iHit < (int) indata->hits.size(); iHit++) {
      const tracker_hit & h = indata->hits[iHit];
      cell_id cid(h.side_id, h.layer_id, h.row_id);
      hit_cells[cid] = iHit;
    }  
    removed_hits.clear();
    removed_hits.assign(indata->hits.size(), -1);    
    return;
  }

  void lttc_algo::_build_output_data_()
  {
    DT_LOG_DEBUG(cfg.logging, "Building output data...");
    DT_LOG_DEBUG(cfg.logging, "NOT IMPLEMENTED YET");
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
    indata = &indata_;
    outdata = &outdata_;
    _prepare_working_data_();

    this->init_loops();
    int iloop = 0;
    int max_nloops = 1;
    bool go_loops = true;

    while (go_loops) {
      DT_LOG_DEBUG(cfg.logging, "Entering #loop=" << iloop);
      std::string fprefix = cfg.draw_prefix + "loop_" + std::to_string(iloop) + "-";
      size_t old_nclusters = outdata->hit_clustering.clusters.size();
     
      DT_LOG_DEBUG(cfg.logging, "Running loop...");
      this->run_loop();
      DT_LOG_DEBUG(cfg.logging, "Loop done.");

      size_t new_nclusters = outdata->hit_clustering.clusters.size();
      DT_LOG_DEBUG(cfg.logging, "#clusters=" << new_nclusters);
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
          polyline2 ln_samples;
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
      //  finalHitClustering << "#@number_of_clusters=" << this->outdata->hit_clustering.clusters.size() << '\n';
      this->outdata->hit_clustering.draw(finalHitClustering, *this);
    }
   
    if (cfg.draw) {
      std::ofstream finalTracks(cfg.draw_prefix + "clustering_tracks.data");
      size_t count = 0;
      for (int icl = 0; icl < (int) this->outdata->hit_clustering.clusters.size(); icl++) {
        const hit_cluster_data & hci = this->outdata->hit_clustering.clusters[icl];
        if (hci.is_discarded() or hci.quality.is_bad()) {
          continue;
        }
        const track_path_data & trackPathi = hci.track_path;
        DT_LOG_DEBUG(cfg.logging, "Draw trackPathi of cluster #" << icl << " with " << trackPathi.vertexes.size() << " vertices"); 
        finalTracks << "#@track-path-" << icl << '\n';
        trackPathi.draw(finalTracks, icl);
        finalTracks << '\n';
        count++;
      }
      finalTracks << "#@number_of_tracks=" << count << '\n';
    }
    
    this->_build_output_data_();
    DT_LOG_DEBUG(cfg.logging, "Exiting.");
    return;
  }
  
} // namespace lttc 
