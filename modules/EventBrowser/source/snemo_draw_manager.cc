/* snemo_draw_manager.cc
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <falaise/snemo/view/browser_tracks.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/snemo_draw_manager.h>

#include <falaise/snemo/detector/detector_manager.h>

#include <falaise/snemo/io/event_server.h>

#include <TObjArray.h>
#include <TPolyMarker3D.h>

namespace snemo {

namespace visualization {

namespace view {

// ctor:
snemo_draw_manager::snemo_draw_manager(const io::event_server* server_) : i_draw_manager() {
  _server_ = server_;

  _objects_ = new TObjArray(1000);
  _text_objects_ = new TObjArray(10);
  _objects_->SetOwner(true);
  _text_objects_->SetOwner(true);

  // Initialize renderers
  _calorimeter_hit_renderer_.initialize(_server_, _objects_, _text_objects_);
  _tracker_hit_renderer_.initialize(_server_, _objects_, _text_objects_);
  _visual_track_renderer_.initialize(_server_, _objects_, _text_objects_);

  return;
}

// dtor:
snemo_draw_manager::~snemo_draw_manager() {
  this->snemo_draw_manager::reset();
  return;
}

TObjArray* snemo_draw_manager::get_objects() { return _objects_; }

TObjArray* snemo_draw_manager::get_text_objects() { return _text_objects_; }

void snemo_draw_manager::update() {
  if (!_server_->is_initialized()) {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event server is not initialized !");
    return;
  }

  // Add 'simulated_data' objects:
  if (_server_->get_event().has(io::SD_LABEL))
    this->_add_simulated_data();
  else {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no simulated data");
  }

  // Add 'calibrated_data' objects:
  if (_server_->get_event().has(io::CD_LABEL))
    this->_add_calibrated_data();
  else {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no calibrated data");
  }

  // Add 'tracker_clustering_data' objects:
  if (_server_->get_event().has(io::TCD_LABEL))
    this->_add_tracker_clustering_data();
  else {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no tracker clustering data");
  }

  // Add 'tracker_trajectory_data' objects:
  if (_server_->get_event().has(io::TTD_LABEL))
    this->_add_tracker_trajectory_data();
  else {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no tracker trajectory data");
  }

  // Add 'tracker_trajectory_data' objects:
  if (_server_->get_event().has(io::PTD_LABEL))
    this->_add_particle_track_data();
  else {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no particle track data");
  }

  return;
}

void snemo_draw_manager::draw() {
  _objects_->Draw();
  //        _text_objects_->Draw();
  return;
}

void snemo_draw_manager::draw_text() {
  _text_objects_->Draw();
  return;
}

void snemo_draw_manager::reset() {
  this->snemo_draw_manager::clear();

  if (_objects_) delete _objects_;
  _objects_ = 0;

  if (_text_objects_) delete _text_objects_;
  _text_objects_ = 0;

  _calorimeter_hit_renderer_.reset();
  _tracker_hit_renderer_.reset();
  _visual_track_renderer_.reset();

  return;
}

void snemo_draw_manager::clear() {
  _calorimeter_hit_renderer_.clear();
  _tracker_hit_renderer_.clear();
  _visual_track_renderer_.clear();

  _objects_->Delete();
  _text_objects_->Delete();
  return;
}

/***************************************************
 *  Filling objects from the 'simulated_data' bank *
 ***************************************************/

void snemo_draw_manager::_add_simulated_data() {
  const options_manager& options_mgr = options_manager::get_instance();

  if (options_mgr.get_option_flag(SHOW_MC_VERTEX)) this->_add_simulated_vertex_();

  if (options_mgr.get_option_flag(SHOW_MC_HITS)) this->_add_simulated_hits_();

  if (options_mgr.get_option_flag(SHOW_MC_TRACKS)) {
    _visual_track_renderer_.push_mc_tracks();
    _visual_track_renderer_.push_mc_legend();
  }

  return;
}

void snemo_draw_manager::_add_simulated_vertex_() {
  const io::event_record& event = _server_->get_event();
  const mctools::simulated_data& sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

  if (!sim_data.has_vertex()) {
    DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                       "Simulated data has no vertex");
    return;
  }

  const geomtools::vector_3d& sim_vertex = sim_data.get_vertex();
  {
    TPolyMarker3D* vertex_3d = base_renderer::make_polymarker(sim_vertex);
    _objects_->Add(vertex_3d);
    vertex_3d->SetMarkerColor(kViolet);
    vertex_3d->SetMarkerStyle(kPlus);
  }
  // Highlight track from primary particles
  const mctools::simulated_data::primary_event_type& pevent = sim_data.get_primary_event();
  const genbb::primary_event::particles_col_type& particles = pevent.get_particles();

  for (genbb::primary_event::particles_col_type::const_iterator ip = particles.begin();
       ip != particles.end(); ++ip) {
    const genbb::primary_particle& a_primary = *ip;
    if (a_primary.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
      TPolyMarker3D* vertex_3d = base_renderer::make_polymarker(sim_vertex);
      _objects_->Add(vertex_3d);
      vertex_3d->SetMarkerColor(kViolet);
      vertex_3d->SetMarkerStyle(kCircle);
      break;
    }
  }
  return;
}

void snemo_draw_manager::_add_simulated_hits_() {
  const options_manager& options_mgr = options_manager::get_instance();
  if (options_mgr.get_option_flag(SHOW_MC_CALORIMETER_HITS)) {
    const std::string& setup_label_name =
        detector::detector_manager::get_instance().get_setup_label_name();
    if (setup_label_name == "snemo::tracker_commissioning") {
      _calorimeter_hit_renderer_.push_simulated_hits("trig");
    } else {
      _calorimeter_hit_renderer_.push_simulated_hits("calo");
      _calorimeter_hit_renderer_.push_simulated_hits("gveto");
      _calorimeter_hit_renderer_.push_simulated_hits("xcalo");
    }
  }

  if (options_mgr.get_option_flag(SHOW_MC_TRACKER_HITS)) {
    _tracker_hit_renderer_.push_simulated_hits("gg");
  }

  return;
}

/****************************************************
 *  Filling objects from the 'calibrated_data' bank *
 ****************************************************/

void snemo_draw_manager::_add_calibrated_data() {
  const options_manager& options_mgr = options_manager::get_instance();
  if (options_mgr.get_option_flag(SHOW_CALIBRATED_HITS)) {
    _calorimeter_hit_renderer_.push_calibrated_hits();
    _tracker_hit_renderer_.push_calibrated_hits();
  }
  return;
}

/************************************************************
 *  Filling objects from the 'tracker_clustering_data' bank *
 ************************************************************/

void snemo_draw_manager::_add_tracker_clustering_data() {
  const options_manager& options_mgr = options_manager::get_instance();
  if (options_mgr.get_option_flag(SHOW_TRACKER_CLUSTERED_HITS)) {
    _tracker_hit_renderer_.push_clustered_hits();
  }

  return;
}

/************************************************************
 *  Filling objects from the 'tracker_trajectory_data' bank *
 ************************************************************/

void snemo_draw_manager::_add_tracker_trajectory_data() {
  const options_manager& options_mgr = options_manager::get_instance();
  if (options_mgr.get_option_flag(SHOW_TRACKER_TRAJECTORIES)) {
    _tracker_hit_renderer_.push_fitted_tracks();
  }

  return;
}

/********************************************************
 *  Filling objects from the 'particle_track_data' bank *
 ********************************************************/

void snemo_draw_manager::_add_particle_track_data() {
  const options_manager& options_mgr = options_manager::get_instance();
  if (options_mgr.get_option_flag(SHOW_PARTICLE_TRACKS)) {
    _visual_track_renderer_.push_reconstructed_tracks();
  }

  return;
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of snemo_draw_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
