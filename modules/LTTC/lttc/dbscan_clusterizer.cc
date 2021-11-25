#include <lttc/dbscan_clusterizer.hh>

// Standard library:
// #include <cmath>
// #include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>

namespace lttc {

  dbscan_clusterizer::dbscan_clusterizer(const input_data & input_,
                                         const config & cfg_)
  {
    input = &input_;
    cfg = cfg_;
    noisy.assign(input->trmap->sorted_bins.size(), false);
    sbins2clusters.assign(input->trmap->sorted_bins.size(), -1);
    nclusters = 0;
    for (int sbin_index = 0; sbin_index < (int) input->trmap->sorted_bins.size(); sbin_index++) {
      bins2sbins[input->trmap->sorted_bins[sbin_index]] = sbin_index;
    }
    DT_LOG_DEBUG(cfg.logging, "Reverse map bins2sbin size=" << bins2sbins.size());
    for (auto p : bins2sbins) {
      DT_LOG_DEBUG(cfg.logging, "  idxBin=" << p.first << " -> " << p.second);
    }
    DT_LOG_DEBUG(cfg.logging, "  ");
    return;
  }

  bool dbscan_clusterizer::is_visited(int sbin_index_) const
  {
    return sbins2clusters[sbin_index_] >= 0 or noisy[sbin_index_];
  }

  bool dbscan_clusterizer::is_unvisited(int sbin_index_) const
  {
    return ! is_visited(sbin_index_);
  }

  void dbscan_clusterizer::epsilon_neighbourhood(int sbin_index_,
                                                 std::set<int> & pts_) const
  {
    pts_.clear();
    double eps = cfg.epsilon;
    // eps = 4.0;
    DT_LOG_DEBUG(cfg.logging, "eps=" << eps);
    DT_LOG_DEBUG(cfg.logging, "sbin_index =" << sbin_index_);
    DT_THROW_IF((sbin_index_ < 0) || (sbin_index_ >= (int) input->trmap->sorted_bins.size()),
                std::range_error,
                "Invalid sbin index!");
    int binIndex = input->trmap->sorted_bins[sbin_index_];
    DT_LOG_DEBUG(cfg.logging, "binIndex =" << binIndex);
    int iBin = -1; 
    int jBin = -1;
    if (!input->trmap->tr_bin_index_to_t_r_indexes(binIndex, iBin, jBin)) {
      return;
    }
    DT_LOG_DEBUG(cfg.logging, "iBin =" << iBin);
    DT_LOG_DEBUG(cfg.logging, "jBin =" << jBin);
    double t = input->trmap->index_to_t(iBin);
    double r = input->trmap->index_to_r(jBin);
    DT_LOG_DEBUG(cfg.logging, "t =" << t);
    DT_LOG_DEBUG(cfg.logging, "r =" << r);
    int max_dist = (int) (eps + 1);
    DT_LOG_DEBUG(cfg.logging, "max_dist =" << max_dist);
    //
    //   :           :           :
    //   :<---eps--->:<---eps--->:
    //   :           :           :
    //  -3  -2  -1   0   1   2   3
    // 
    //  [-] [-] [ ] [ ] [ ] [ ] [ ]  3 ...
    //                                  ^
    //  [-] [+] [ ] [ ] [ ] [ ] [ ]  2  |
    //                                 eps
    //  [-] [+] [+] [ ] [ ] [ ] [ ]  1  |
    //                                  v
    //  [+] [+] [+] [x] [+] [+] [ ]  0 ...
    //                                  ^
    //  [ ] [+] [+] [+] [+] [+] [-] -1  |
    //                                 eps
    //  [-] [+] [+] [+] [+] [ ] [ ] -2  |
    //                                  v
    //  [-] [-] [ ] [ ] [ ] [ ] [ ] -3 ...
    //
    //
    for (int i = iBin - max_dist; i <= iBin + max_dist; i++) {
      DT_LOG_DEBUG(cfg.logging, "check i=" << i);
      int di = i-iBin;
      if ((i < 0) || (i >= (int) input->trmap->nt)) {
        DT_LOG_DEBUG(cfg.logging, "i=" << i << " is out of map");
        continue;
      }
      for (int j = jBin - max_dist; j <= jBin + max_dist; j++) {
        int dj = j-jBin;
        DT_LOG_DEBUG(cfg.logging, "check j=" << j);
        if ((j < 0) || (j >= (int) input->trmap->nr)) {
          DT_LOG_DEBUG(cfg.logging, "j=" << j << " is out of map");
          continue;
        }
        double dist_ij = std::sqrt(di * di + dj * dj);
        DT_LOG_DEBUG(cfg.logging, "dist_ij=" << dist_ij);
        if (dist_ij > eps) {
          DT_LOG_DEBUG(cfg.logging, "(i,j)=(" << i << ',' << j << ") is too far");
          continue;
        }
        int neighbourIndex = input->trmap->t_r_indexes_to_tr_bin_index(i, j);
        auto found = bins2sbins.find(neighbourIndex);
        if (found == bins2sbins.end()) {
          DT_LOG_DEBUG(cfg.logging, "Not found in valid sorted hits");
          continue;
        }
        int neighbourSindex = found->second;
        DT_LOG_DEBUG(cfg.logging, "insert neighbour @ sindex=" << neighbourSindex);
        pts_.insert(neighbourSindex);
      }
    }
    return;
  }

  void dbscan_clusterizer::expand_cluster(int sbin_index_,
                                          const std::set<int> & pts_)
  {
    int curClusterId = nclusters - 1;
    sbins2clusters[sbin_index_] = curClusterId;
    noisy[sbin_index_] = false;
    std::set<int> S(pts_);
    S.erase(sbin_index_);    
    while (S.size()) {
      int Q = *S.begin();
      if (noisy[Q]) {
        sbins2clusters[Q] = curClusterId;
        noisy[Q] = false;
      }
      if (is_visited(Q)) {
        S.erase(Q);
        continue;
      }
      sbins2clusters[Q] = curClusterId;
      noisy[Q] = false;
      std::set<int> N;
      epsilon_neighbourhood(Q, N);
      if (N.size() >= cfg.min_pts) {
        for (auto pt : N) {
          S.insert(pt);
        }
      }
      S.erase(Q);
    }
    return;
  }

  void dbscan_clusterizer::run(output_data & output_)
  {
    nclusters = 0;
    for (int sbin_index = 0; sbin_index < (int) input->trmap->sorted_bins.size(); sbin_index++) {
      if (is_visited(sbin_index)) continue;
      std::set<int> N;
      epsilon_neighbourhood(sbin_index, N);
      if (datatools::logger::is_debug(cfg.logging)) {
        DT_LOG_DEBUG(cfg.logging, "neighbour_sbins.size()=" << N.size());
        std::string fname = "dbscan-" + std::to_string(sbin_index) + ".data";
        std::ofstream fout(fname.c_str());
        fout << "#@dbscan_clusterizer::run: " << '\n';
        draw_step1(fout, sbin_index, N); 
      }
      if (N.size() < cfg.min_pts) {
        noisy[sbin_index] = true;
        continue;
      } 
      nclusters++;
      expand_cluster(sbin_index, N);
    }
    // Build output:
    output_.clusters.clear();
    output_.unclustered.clear();
    for (int icluster = 0; icluster < (int) nclusters ; icluster++) {
      cluster c;
      c.id = icluster;
      output_.clusters.push_back(c);
    }
    for (int isbin = 0; isbin < (int) sbins2clusters.size(); isbin++) {
      int clusterId = sbins2clusters[isbin];
      if (clusterId == -1) {
        output_.unclustered.insert(isbin);
      } else {
        output_.clusters[clusterId].sbins.insert(isbin);
        int ibin = input->trmap->sorted_bins[isbin];
        double height = input->trmap->bins[ibin];
        if (height > output_.clusters[clusterId].max_height) {
          output_.clusters[clusterId].max_height = height;
          output_.clusters[clusterId].max_sbin = isbin;
        }
      }
    }
    if (datatools::logger::is_debug(cfg.logging)) {
      output_.print(std::cerr, *input);
    }
    return;
  }

  void dbscan_clusterizer::output_data::print(std::ostream & out_,
                                              const input_data & input_) const
  {
    out_ << "DBSCAN clusterizer output:\n";
    out_ << "|-- #unclustered sbins = " << unclustered.size() << '\n';
    size_t nUnclusteredSbins = 0;
    for (auto isbin : unclustered) {
      std::string tag = "|-- ";
      if (nUnclusteredSbins + 1 == unclustered.size()) tag = "`-- ";
      int ibin = input_.trmap->sorted_bins[isbin];
      out_ << "|   " << tag << "unclustered #" << isbin
           << " mapped to ibin=" << ibin
           << " with height=" << input_.trmap->bins[ibin]
           << '\n';
      nUnclusteredSbins++;
    }
    
    out_ << "|-- #clusters = " << clusters.size() << '\n';
    size_t ntotClusteredSbins = 0;
    for (int ic = 0; ic < (int) clusters.size(); ic++) {
      std::string tag = "|-- ";
      if ((ic + 1) == (int) clusters.size()) tag = "`-- ";
      ntotClusteredSbins += clusters[ic].sbins.size();
      out_ << "|   " << tag << "cluster #" << ic << " : #sbins=" << clusters[ic].sbins.size()
           << " max_sbin=" << clusters[ic].max_sbin << " with max_height=" << clusters[ic].max_height
           << " and to peak bin=" << input_.trmap->sorted_bins[clusters[ic].max_sbin] << '\n';
    }
    out_ << "`-- " << "#sbins (total) = " << (ntotClusteredSbins + unclustered.size()) << '\n';
    return;
  }

  void dbscan_clusterizer::draw_step1(std::ostream & out_,
                                      int sbin_index_,
                                      const std::set<int> & pts_) const
  {
    for (auto sbinIndex : pts_) {
      int binIndex = input->trmap->sorted_bins[sbinIndex];
      int iBin = -1; 
      int jBin = -1;
      if (!input->trmap->tr_bin_index_to_t_r_indexes(binIndex, iBin, jBin)) {
        return;
      }
      double t = input->trmap->index_to_t(iBin);
      double r = input->trmap->index_to_r(jBin);
      int    tag = 0;
      double height = input->trmap->bins[binIndex];
      if (sbin_index_ == sbinIndex) tag = 1;
      out_ << t - 0.5 * input->trmap->dt << ' ' << r - 0.5 * input->trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t - 0.5 * input->trmap->dt << ' ' << r + 0.5 * input->trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t + 0.5 * input->trmap->dt << ' ' << r + 0.5 * input->trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t + 0.5 * input->trmap->dt << ' ' << r - 0.5 * input->trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t - 0.5 * input->trmap->dt << ' ' << r - 0.5 * input->trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << '\n';
    }
    return;
  }

  void dbscan_clusterizer::output_data::draw(std::ostream & out_, const input_data & input_) const
  {
    out_ << "#@unclustered-sbins=" << unclustered.size() << '\n';
    for (auto sbinIndex : unclustered) {
      int binIndex = input_.trmap->sorted_bins[sbinIndex];
      int iBin = -1; 
      int jBin = -1;
      input_.trmap->tr_bin_index_to_t_r_indexes(binIndex, iBin, jBin);
      double t = input_.trmap->index_to_t(iBin);
      double r = input_.trmap->index_to_r(jBin);
      double height = input_.trmap->bins[binIndex];
      int tag = -1;
      out_ << t - 0.5 * input_.trmap->dt << ' ' << r - 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t - 0.5 * input_.trmap->dt << ' ' << r + 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t + 0.5 * input_.trmap->dt << ' ' << r + 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t + 0.5 * input_.trmap->dt << ' ' << r - 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << t - 0.5 * input_.trmap->dt << ' ' << r - 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
      out_ << '\n';
    }
    out_ << "#@end-of-unclustered-sbins" << '\n';
    out_ << '\n';

    for (int ic = 0; ic < (int) clusters.size(); ic++) {
      out_ << "#@cluster-" << ic << '\n';
      for (auto sbinIndex : clusters[ic].sbins) {
        int binIndex = input_.trmap->sorted_bins[sbinIndex];
        int iBin = -1; 
        int jBin = -1;
        input_.trmap->tr_bin_index_to_t_r_indexes(binIndex, iBin, jBin);
        double t = input_.trmap->index_to_t(iBin);
        double r = input_.trmap->index_to_r(jBin);
        double height = input_.trmap->bins[binIndex];
        int tag = ic;
        out_ << t - 0.5 * input_.trmap->dt << ' ' << r - 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
        out_ << t - 0.5 * input_.trmap->dt << ' ' << r + 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
        out_ << t + 0.5 * input_.trmap->dt << ' ' << r + 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
        out_ << t + 0.5 * input_.trmap->dt << ' ' << r - 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
        out_ << t - 0.5 * input_.trmap->dt << ' ' << r - 0.5 * input_.trmap->dr << ' ' << height << ' ' << tag << '\n';
        out_ << '\n';
      }
      out_ << "#@end-of-cluster-" << ic << '\n';
      out_ << '\n';
    }
  
    return;
  }
 
} // namespace lttc 
