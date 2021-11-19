#include <lttc/hit_simulator.hh>

// This project:

namespace lttc {

  hit_simulator::hit_simulator(const tracker & sntracker_, const config & cfg_)
  {
    sntracker = &sntracker_;
    cfg = cfg_;
    return;
  }
   
  void hit_simulator::set_trk_conds(const tracker_conditions & trk_conds_)
  {
    trk_conds = &trk_conds_;
    return;
  }

  void hit_simulator::reset_trk_conds()
  {
    trk_conds = nullptr;
    return;
  }

  void hit_simulator::generate_noisy_hits(std::default_random_engine & generator_,
                                          tracker_hit_collection & hits_) const
  {
    // Add noisy hits:
    if (cfg.add_noisy_hits) {
      DT_LOG_DEBUG(cfg.logging, "nb_noisy_hits=" << cfg.nb_noisy_hits);
      std::uniform_int_distribution<int> side_distribution(0, sntracker->nsides - 1);
      std::uniform_int_distribution<int> layer_distribution(0, sntracker->nlayers - 1);
      std::uniform_int_distribution<int> row_distribution(0, sntracker->nrows - 1);
      double drift_radius_err = 2.0 * cfg.drift_radius_err;
      std::normal_distribution<double> rdistribution(18.0, drift_radius_err);
      for (int inh = 0; inh < (int) cfg.nb_noisy_hits; inh++) {
        int iside = side_distribution(generator_);
        int ilayer = layer_distribution(generator_);
        int irow = row_distribution(generator_);
        double z = 0.0;
        double zErr = 0.0;
        bool delayed = false;
        tracker_hit h(hits_.size(), iside, ilayer, irow, rdistribution(generator_), drift_radius_err, z, zErr, delayed);
        if (trk_conds != nullptr) {
          if (trk_conds->has_dead_cell(cell_id(iside, ilayer, irow))) {
            continue;
          }
        }
        DT_LOG_DEBUG(cfg.logging, "Add a new hit @(" << ilayer << ',' << irow << ")");
        hits_.add_hit(h);
      }
    }
  }
  
  void hit_simulator::generate_hits(std::default_random_engine & generator_,
                                    const track & trk_,
                                    tracker_hit_collection & hits_) const
  {
    int lastId = hits_.max_hit_id();
    for (const auto & p : trk_.pl) {
      int iside;
      int ilayer;
      int irow;
      if (sntracker->locate(p, iside, ilayer, irow)) {
        DT_LOG_DEBUG(cfg.logging, "ilayer=" << ilayer);
        DT_LOG_DEBUG(cfg.logging, "irow=" << irow);
        point cellCenter = sntracker->cell_position(iside, ilayer, irow);
        DT_LOG_DEBUG(cfg.logging, "cellCenter=" << cellCenter);
        double dist = (p - cellCenter).mag();
        DT_LOG_DEBUG(cfg.logging, "dist=" << dist);
        if (dist >= sntracker->rcell) {
          continue;
        }
        double drift_radius_err = cfg.drift_radius_err;
        std::normal_distribution<double> distribution(dist, drift_radius_err);
        double drift_radius = distribution(generator_);
        DT_LOG_DEBUG(cfg.logging, "drift_radius=" << drift_radius);
        double z = 0.0;
        double zErr = 0.0;
        bool delayed = false;
        int hitId = lastId + 1;
        tracker_hit h(hitId, iside, ilayer, irow, drift_radius, drift_radius_err, z, zErr, delayed);
        if (trk_conds != nullptr) {
          if (trk_conds->has_dead_cell(cell_id(iside, ilayer, irow))) {
            continue;
          }
        }
        DT_LOG_DEBUG(cfg.logging, "Attempt to add the hit #" << h.id << " @cell=" << h.get_cell_id());
        bool success = hits_.add_hit(h);
        if (success) {
          DT_LOG_DEBUG(cfg.logging, "==> Hit collection size is now : " << hits_.size());
          lastId = hitId;
        }
        DT_LOG_DEBUG(cfg.logging, "==> Hit collection size is now : " << hits_.size());
      }
    }
    
    DT_LOG_DEBUG(cfg.logging, "#hits=" << hits_.size());
    return;
  }
  
} // namespace lttc 
