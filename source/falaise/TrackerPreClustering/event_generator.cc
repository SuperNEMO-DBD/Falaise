// -*- mode: c++ ; -*-
/// \file falaise/TrackerPreClustering/event_generator.cc

// Ourselves:
#include <TrackerPreClustering/event_generator.h>

// Standard library:
#include <algorithm>

// Third party:
// - Boost:
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>

namespace TrackerPreClustering {

  void event_generator::set_seed(long seed_)
  {
    _seed_ = seed_;
    return;
  }

  void event_generator::set_cell_size(double cell_size_)
  {
    _cell_size_ = cell_size_;
    return;
  }

  void event_generator::set_nb_layers(unsigned int nb_layers_)
  {
    _nb_layers_ = nb_layers_;
    return;
  }

  void event_generator::set_nb_rows(unsigned int nb_rows_)
  {
    _nb_rows_ = nb_rows_;
    return;
  }

  event_generator::event_generator()
  {
    set_seed(0);
    set_nb_layers(9);
    set_nb_rows(113);
    set_cell_size(44.0 * CLHEP::mm);
    return;
  }

  event_generator::event_generator(long seed_)
  {
    set_seed(seed_);
    set_nb_layers(9);
    set_nb_rows(113);
    set_cell_size(44.0 * CLHEP::mm);
    initialize();
    return;
  }

  bool event_generator::is_initialized()
  {
    return _generator_.get() != 0;
  }

  void event_generator::initialize()
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Event generator is already initialized!");
    _generator_.reset(new boost::random::mt19937(_seed_));
    return;
  }

  void event_generator::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Event generator is not initialized!");
    _generator_.reset();
    return;
  }

  int event_generator::build_gid(int side_, int layer_, int row_) const
  {
    int gid = layer_ * 1000 + row_ ;
    if (side_ == 0) gid *= -1;
    return gid;
  }

  void event_generator::register_gid(int gid_, bool delayed_)
  {
    _gids_[gid_] = delayed_;
    return;
  }

  bool event_generator::has_gid(int gid_) const
  {
    return _gids_.find(gid_) != _gids_.end();
  }

  void event_generator::generate_prompt_gg_hits(std::vector<const gg_hit *> & hits_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Event generator is not initialized!");
    const datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");

    boost::random::mt19937 & generator = *_generator_;
    boost::uniform_real<> uni_dist(0, 1);
    boost::variate_generator<boost::random::mt19937&,
                             boost::uniform_real<> > uni(generator, uni_dist);

    boost::random::uniform_int_distribution<> prompt_tracks_dist(1, 3);
    unsigned int nb_prompt_tracks = prompt_tracks_dist(generator);
    DT_LOG_TRACE(local_priority, "nb_prompt_tracks=" << nb_prompt_tracks);
    for (unsigned int itrack = 0; itrack < nb_prompt_tracks; itrack++) {
      boost::random::uniform_int_distribution<> prompt_hits_dist(3, 15);
      unsigned int nb_prompt_hits = prompt_hits_dist(generator);
      int module = 0;
      int side = uni() < 0.5 ? 0 : 1;
      boost::random::uniform_int_distribution<> row0_dist(30, _nb_rows_ - 30);
      int row0 = row0_dist(generator);
      double z0 =(-100. + 200. * uni()) * CLHEP::cm;
      int n0 = 0;
      for (unsigned int ihit = 0; ihit < nb_prompt_hits; ihit++) {
        gg_hit * h = new gg_hit;
        {
          boost::shared_ptr<gg_hit> sp(h);
          _hits_gc_.push_back(sp);
        }
        h->id = hits_.size();
        h->sterile = false;
        h->delayed = false;
        h->module = module;
        h->side = side;
        int attempts = 0;
        while (attempts < 5) {
          h->layer = std::min<int>(ihit, _nb_layers_ - 1);
          boost::random::uniform_int_distribution<> delta_row_dist(-1, +1);
          h->row = row0 + delta_row_dist(generator);
          int gid = build_gid(h->side, h->layer, h->row);
          if (! has_gid(gid)) {
            register_gid(gid, h->delayed);
            break;
          }
          attempts++;
        }
        if (attempts >= 5) break;
        h->sterile = uni() < 0.1 ? true : false;
        h->peripheral = false;
        h->noisy = false;
        h->missing_top_cathode = false;
        h->missing_bottom_cathode = false;
        h->x =  2.0 * CLHEP::cm +(0.5 + h->layer) * _cell_size_;
        if (side == 0) {
          h->x *= -1;
        }
        h->y =(-0.5 * _nb_rows_ + h->row ) * _cell_size_;
        h->z  = z0 + (-1. + 2. * uni()) * CLHEP::cm;
        h->dz = 1.0 * CLHEP::cm;
        h->r  = 0.05 * CLHEP::mm + uni() * 0.5 * _cell_size_;
        h->dr = 0.1 * CLHEP::mm;
        if (h->r > 0.95 * 0.5 * _cell_size_) {
          h->peripheral = true;
        }

        hits_.push_back(h);
        n0++;
      }
      DT_LOG_TRACE(local_priority, "nb_prompt_hits=" << n0);
    }
    DT_LOG_TRACE(local_priority, "N hits=" << _gids_.size());
    DT_LOG_TRACE(local_priority, "Exiting.");
    return;
  }

  void event_generator::generate_delayed_gg_hits(std::vector<const gg_hit *> & hits_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Event generator is not initialized!");
    const datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE(local_priority, "Entering...");

    boost::random::mt19937 & generator = *_generator_;
    boost::uniform_real<> uni_dist(0, 1);
    boost::variate_generator<boost::random::mt19937&,
                             boost::uniform_real<> > uni(generator, uni_dist);

    boost::random::uniform_int_distribution<> delayed_tracks_dist(1, 3);
    unsigned int nb_delayed_tracks = delayed_tracks_dist(generator);
    DT_LOG_TRACE(local_priority, "nb_delayed_tracks=" << nb_delayed_tracks);
    for (unsigned int itrack = 0; itrack < nb_delayed_tracks; itrack++) {
      boost::random::uniform_int_distribution<> delayed_hits_dist(3, 15);
      unsigned int nb_delayed_hits = delayed_hits_dist(generator);
      int module = 0;
      int side = uni() < 0.5 ? 0 : 1;
      boost::random::uniform_int_distribution<> row0_dist(30, _nb_rows_ - 30);
      int row0 = row0_dist(generator);
      double z0 =(-100. + 200. * uni()) * CLHEP::cm;
      double delayed_time =(10. + 4000. * uni()) * CLHEP::microsecond;
      int n0 = 0;
      for (unsigned int ihit = 0; ihit < nb_delayed_hits; ihit++) {
        gg_hit * h = new gg_hit;
        {
          boost::shared_ptr<gg_hit> sp(h);
          _hits_gc_.push_back(sp);
        }
        h->id = hits_.size();
        h->module = module;
        h->side = side;
        int attempts = 0;
        while (attempts < 5) {
          h->layer = std::min<int>(ihit, _nb_layers_ - 1);
          h->layer = std::min<int>(ihit, _nb_layers_ - 1);
          boost::random::uniform_int_distribution<> delta_row_dist(-1, +1);
          h->row = row0 + delta_row_dist(generator);
          int gid = build_gid(h->side, h->layer, h->row);
          if (! has_gid(gid)) {
            register_gid(gid, h->delayed);
            break;
          }
          attempts++;
        }
        if (attempts >= 5) break;
        h->sterile = uni() < 0.1 ? true : false;
        h->delayed = true;
        h->peripheral = false;
        h->noisy = false;
        h->missing_top_cathode = false;
        h->missing_bottom_cathode = false;
        h->x =  2.0 * CLHEP::cm +(0.5 + h->layer) * _cell_size_;
        if (side == 0) {
          h->x *= -1;
        }
        h->y =(-0.5 * _nb_rows_ + h->row ) * _cell_size_;
        h->z  = z0 + (-1. + 2. * uni()) * CLHEP::cm;
        h->dz = 1.0 * CLHEP::cm;
        h->r  = std::numeric_limits<double>::quiet_NaN();
        h->dr = std::numeric_limits<double>::quiet_NaN();
        h->delayed_time = delayed_time + (0.1 + 5. * uni()) * CLHEP::microsecond;
        h->delayed_time_error = 0.0;
        hits_.push_back(h);
        n0++;
      }
      DT_LOG_TRACE(local_priority, "nb_delayed_hits=" << n0);
    }
    DT_LOG_TRACE(local_priority, "N hits=" << _gids_.size());
    DT_LOG_TRACE(local_priority, "Exiting.");
    return;
  }

  void event_generator::shoot_event(std::vector<const gg_hit *> & hits_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Event generator is not initialized!");
    _hits_gc_.clear();
    generate_prompt_gg_hits(hits_);
    generate_delayed_gg_hits(hits_);
    _gids_.clear();
    return;
  }

} // end of namespace TrackerPreClustering
