/* base_renderer.cc
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

#include <EventBrowser/view/base_renderer.h>
#include <EventBrowser/view/browser_tracks.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/style_manager.h>

#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/detector/i_root_volume.h>

#include <EventBrowser/io/event_server.h>

#include <EventBrowser/utils/root_utilities.h>

#include <geomtools/helix_3d.h>
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/id_mgr.h>
#include <geomtools/line_3d.h>

#include <TObjArray.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>

namespace snemo {

namespace visualization {

namespace view {

bool base_renderer::is_initialized() const { return _initialized; }

bool base_renderer::has_server() const { return _server != 0; }

void base_renderer::set_server(const io::event_server* server_) {
  _server = server_;
  return;
}

bool base_renderer::has_graphical_objects() const { return _objects != 0; }

void base_renderer::set_graphical_objects(TObjArray* objects_) {
  _objects = objects_;
  return;
}

bool base_renderer::has_text_objects() const { return _text_objects != 0; }

void base_renderer::set_text_objects(TObjArray* text_objects_) {
  _text_objects = text_objects_;
  return;
}

// ctor:
base_renderer::base_renderer() {
  _initialized = false;
  _server = 0;
  _objects = 0;
  _text_objects = 0;
  return;
}

// dtor:
base_renderer::~base_renderer() {
  this->reset();
  return;
}

void base_renderer::initialize(const io::event_server* server_, TObjArray* objects_,
                               TObjArray* text_objects_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  this->set_server(server_);
  this->set_graphical_objects(objects_);
  this->set_text_objects(text_objects_);
  _initialized = true;
  return;
}

void base_renderer::clear() {
  for (geom_id_collection::const_iterator gid = _highlighted_geom_id.begin();
       gid != _highlighted_geom_id.end(); ++gid) {
    detector::detector_manager& detector_mgr = detector::detector_manager::get_instance();
    detector::i_volume* volume_hit = detector_mgr.grab_volume(*gid);
    volume_hit->clear();
  }
  _highlighted_geom_id.clear();
  return;
}

void base_renderer::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
  this->clear();
  _initialized = false;
  return;
}

void base_renderer::highlight_geom_id(const geomtools::geom_id& gid_, const size_t color_,
                                      const std::string& text_) {
  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
               "Geom id '" << gid_ << "' is hit");

  // Highlight calorimeter block
  detector::detector_manager& detector_mgr = detector::detector_manager::get_instance();

  // Get matching list of geom_id (using 'any_adress' concept)
  std::vector<geomtools::geom_id> gid_list;
  detector_mgr.get_matching_ids(gid_, gid_list);

  if (gid_list.empty()) {
    DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                   "No calorimeter volume with geom_id '"
                       << gid_ << "' has been found ! Check either geometry file or style file "
                       << "in case this detector part is disable");
    return;
  }

  if (text_.empty()) {
    for (std::vector<geomtools::geom_id>::iterator igid = gid_list.begin(); igid != gid_list.end();
         ++igid) {
      detector::i_root_volume* volume_hit =
          dynamic_cast<detector::i_root_volume*>(detector_mgr.grab_volume(*igid));
      volume_hit->highlight(color_);
      _highlighted_geom_id.insert(*igid);
    }
  } else {
    // Here we get the first element :
    detector::i_root_volume* volume_hit =
        dynamic_cast<detector::i_root_volume*>(detector_mgr.grab_volume(gid_list.front()));
    utils::root_utilities::TLatex3D* new_text_obj = new utils::root_utilities::TLatex3D;
    _text_objects->Add(new_text_obj);
    const geomtools::vector_3d& position = volume_hit->get_placement().get_translation();
    new_text_obj->SetX(position.x());
    new_text_obj->SetY(position.y());
    new_text_obj->SetZ(position.z());
    new_text_obj->SetText(text_);
    new_text_obj->SetTextColor(color_);
  }
  return;
}

TPolyMarker3D* base_renderer::make_polymarker(const geomtools::vector_3d& point_,
                                              const bool convert_) {
  TPolyMarker3D* marker = new TPolyMarker3D;
  geomtools::vector_3d new_point = point_;
  if (convert_) {
    detector::detector_manager::get_instance().compute_world_coordinates(point_, new_point);
  }
  marker->SetPoint(0, new_point.x(), new_point.y(), new_point.z());
  return marker;
}

TPolyLine3D* base_renderer::make_polyline(const geomtools::polyline_type& polyline_,
                                          const bool convert_) {
  TPolyLine3D* polyline = new TPolyLine3D;
  size_t idx = 0;
  for (geomtools::polyline_type::const_iterator i = polyline_.begin(); i != polyline_.end(); ++i) {
    const geomtools::vector_3d& a_point = *i;
    geomtools::vector_3d new_point = a_point;
    if (convert_) {
      detector::detector_manager::get_instance().compute_world_coordinates(a_point, new_point);
    }
    polyline->SetPoint(idx++, new_point.x(), new_point.y(), new_point.z());
  }
  return polyline;
}

TPolyLine3D* base_renderer::make_track(const geomtools::i_wires_3d_rendering& iw3dr_,
                                       const bool convert_) {
  geomtools::wires_type wires;
  iw3dr_.generate_wires_self(wires);
  DT_THROW_IF(wires.size() > 1, std::logic_error, "Track must be defined by only one polyline !");
  return make_polyline(wires.back(), convert_);
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of base_renderer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
