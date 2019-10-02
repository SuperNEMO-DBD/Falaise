/* bipo_draw_manager.cc
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

#include <EventBrowser/view/bipo_draw_manager.h>
#include <EventBrowser/view/calorimeter_hit_renderer.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/visual_track_renderer.h>

#include <EventBrowser/detector/detector_manager.h>

#include <EventBrowser/io/event_server.h>

#include <TObjArray.h>
#include <TPolyMarker3D.h>

namespace snemo {

namespace visualization {

namespace view {

// ctor:
bipo_draw_manager::bipo_draw_manager(const io::event_server* server_)
    : i_draw_manager(),
      _server_(server_),
      _objects_(new TObjArray(1000)),
      _text_objects_(new TObjArray(10)),
      _calorimeter_hit_renderer_(new calorimeter_hit_renderer()),
      _visual_track_renderer_(new visual_track_renderer()) {
  _objects_->SetOwner(true);
  _text_objects_->SetOwner(true);

  // Initialize renderers
  _calorimeter_hit_renderer_->initialize(_server_, _objects_, _text_objects_);
  _visual_track_renderer_->initialize(_server_, _objects_);

  return;
}

// dtor:
bipo_draw_manager::~bipo_draw_manager() {
  this->bipo_draw_manager::reset();
  return;
}

TObjArray* bipo_draw_manager::get_objects() { return _objects_; }

TObjArray* bipo_draw_manager::get_text_objects() { return _text_objects_; }

void bipo_draw_manager::update() {
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

  return;
}

void bipo_draw_manager::draw() {
  _objects_->Draw();
  return;
}

void bipo_draw_manager::draw_text() {
  _text_objects_->Draw();
  return;
}

void bipo_draw_manager::reset() {
  this->bipo_draw_manager::clear();

  if (_objects_) delete _objects_;
  _objects_ = 0;

  if (_text_objects_) delete _text_objects_;
  _text_objects_ = 0;

  _calorimeter_hit_renderer_->reset();
  if (_calorimeter_hit_renderer_) delete _calorimeter_hit_renderer_;
  _calorimeter_hit_renderer_ = 0;

  _visual_track_renderer_->reset();
  if (_visual_track_renderer_) delete _visual_track_renderer_;
  _visual_track_renderer_ = 0;

  return;
}

void bipo_draw_manager::clear() {
  _calorimeter_hit_renderer_->clear();
  _visual_track_renderer_->clear();

  _objects_->Delete();
  _text_objects_->Delete();
  return;
}

/***************************************************
 *  Filling objects from the 'simulated_data' bank *
 ***************************************************/

void bipo_draw_manager::_add_simulated_data() {
  const options_manager& options_mgr = options_manager::get_instance();

  if (options_mgr.get_option_flag(SHOW_MC_VERTEX)) this->_add_simulated_vertex_();

  if (options_mgr.get_option_flag(SHOW_MC_HITS)) this->_add_simulated_hits_();

  if (options_mgr.get_option_flag(SHOW_MC_TRACKS)) this->_add_simulated_tracks_();

  return;
}

void bipo_draw_manager::_add_simulated_vertex_() {
  const io::event_record& event = _server_->get_event();
  const mctools::simulated_data& sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

  if (!sim_data.has_vertex()) {
    DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                       "Simulated data has no vertex");
    return;
  }

  const geomtools::vector_3d& sim_vertex = sim_data.get_vertex();

  TPolyMarker3D* vertex_3d = new TPolyMarker3D();
  _objects_->Add(vertex_3d);
  vertex_3d->SetMarkerColor(kViolet);
  vertex_3d->SetMarkerStyle(kPlus);
  vertex_3d->SetPoint(0, sim_vertex.x(), sim_vertex.y(), sim_vertex.z());

  // TMarker3DBox* vertex_3d_box = new TMarker3DBox ();
  // _objects_->Add (vertex_3d_box);
  // vertex_3d_box->SetLineColor (kViolet);
  // vertex_3d_box->SetPosition (sim_vertex.x (),
  //                             sim_vertex.y (),
  //                             sim_vertex.z ());
  // vertex_3d_box->SetSize (10, 10, 10);

  return;
}

void bipo_draw_manager::_add_simulated_hits_() {
  _calorimeter_hit_renderer_->push_simulated_hits("scin.hit");

  return;
}

void bipo_draw_manager::_add_simulated_tracks_() {
  _visual_track_renderer_->push_mc_tracks("__visu.tracks");
  _visual_track_renderer_->push_mc_legend();
  return;
}

/****************************************************
 *  Filling objects from the 'calibrated_data' bank *
 ****************************************************/

void bipo_draw_manager::_add_calibrated_data() {
  const options_manager& options_mgr = options_manager::get_instance();

  if (options_mgr.get_option_flag(SHOW_CALIBRATED_HITS)) this->_add_calibrated_hits_();

  if (options_mgr.get_option_flag(SHOW_CALIBRATED_INFO)) this->_add_calibrated_info_();

  return;
}

void bipo_draw_manager::_add_calibrated_hits_() {
  _calorimeter_hit_renderer_->push_calibrated_hits();
  return;
}

void bipo_draw_manager::_add_calibrated_info_() {
  _calorimeter_hit_renderer_->push_calibrated_info();
  return;
}

}  // end of namespace view

}  // end of namespace visualization

}  // namespace snemo

// end of bipo_draw_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
