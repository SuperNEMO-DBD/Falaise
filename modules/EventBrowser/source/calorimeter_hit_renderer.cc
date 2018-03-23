/* calorimeter_hit_renderer.cc
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
#include <falaise/snemo/view/calorimeter_hit_renderer.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/style_manager.h>

#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/detector/i_root_volume.h>

#include <falaise/snemo/io/event_server.h>

#include <falaise/snemo/utils/root_utilities.h>

#include <geomtools/id_mgr.h>

#include <TColor.h>
#include <TMarker3DBox.h>
#include <TObjArray.h>
#include <TPolyMarker3D.h>

namespace snemo {

namespace visualization {

namespace view {

// ctor:
calorimeter_hit_renderer::calorimeter_hit_renderer() : base_renderer() { return; }

// dtor:
calorimeter_hit_renderer::~calorimeter_hit_renderer() { return; }

void calorimeter_hit_renderer::push_simulated_hits(const std::string& hit_category_) {
  const io::event_record& event = _server->get_event();
  const mctools::simulated_data& sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

  if (!sim_data.has_step_hits(hit_category_)) {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no '" << hit_category_ << "' calorimeter hits");
    return;
  }

  const mctools::simulated_data::hit_handle_collection_type& hit_collection =
      sim_data.get_step_hits(hit_category_);
  if (hit_collection.empty()) {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "No simulated calorimeter hits");
    return;
  }

  for (mctools::simulated_data::hit_handle_collection_type::const_iterator it_hit =
           hit_collection.begin();
       it_hit != hit_collection.end(); ++it_hit) {
    const mctools::base_step_hit& a_hit = it_hit->get();

    const geomtools::vector_3d& pstart = a_hit.get_position_start();
    const geomtools::vector_3d& pstop = a_hit.get_position_stop();

    const double dx = std::abs(0.5 * (pstart.x() - pstop.x()));
    const double dy = std::abs(0.5 * (pstart.y() - pstop.y()));
    const double dz = std::abs(0.5 * (pstart.z() - pstop.z()));

    const geomtools::vector_3d pos = 0.5 * (pstart + pstop);

    TMarker3DBox* step_3d = new TMarker3DBox;
    _objects->Add(step_3d);
    step_3d->SetPosition(pos.x(), pos.y(), pos.z());
    step_3d->SetSize(dx, dy, dz);
    step_3d->SetLineColor(kRed);

    // // Store this value into cluster properties:
    // io::step_hit_type * mutable_hit = const_cast<io::step_hit_type*>(&(a_hit));
    // datatools::utils::properties & hit_properties = mutable_hit->grab_auxiliaries();
    // const long pixel = TColor::Number2Pixel(kRed);
    // const string hex_str = TColor::PixelAsHexString(pixel);
    // hit_properties.update(browser_tracks::COLOR_FLAG, hex_str);

    // Retrieve line width from properties if 'hit' is highlighted:
    size_t line_width = style_manager::get_instance().get_mc_line_width();
    if (a_hit.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
      line_width = 3;
      TPolyMarker3D* mark1 = new TPolyMarker3D;
      _objects->Add(mark1);
      mark1->SetMarkerColor(kRed);
      mark1->SetMarkerStyle(kCircle);
      mark1->SetPoint(0, pstart.x(), pstart.y(), pstart.z());
      TPolyMarker3D* mark2 = new TPolyMarker3D;
      _objects->Add(mark2);
      mark2->SetMarkerColor(kRed);
      mark2->SetMarkerStyle(kCircle);
      mark2->SetPoint(0, pstop.x(), pstop.y(), pstop.z());
    }
    // hit_properties.update(browser_tracks::HIGHLIGHT_FLAG, false);
    step_3d->SetLineWidth(line_width);

    this->highlight_geom_id(a_hit.get_geom_id(), kRed);
  }  // end of step collection

  return;
}

void calorimeter_hit_renderer::push_calibrated_hits() {
  const io::event_record& event = _server->get_event();
  const snemo::datamodel::calibrated_data& calib_data =
      event.get<snemo::datamodel::calibrated_data>(io::CD_LABEL);

  if (!calib_data.has_calibrated_calorimeter_hits()) {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "Event has no calibrated calorimeter hits");
    return;
  }

  const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type& cc_collection =
      calib_data.calibrated_calorimeter_hits();

  if (cc_collection.empty()) {
    DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                 "No calibrated calorimeter hits");
    return;
  }

  for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator it_hit =
           cc_collection.begin();
       it_hit != cc_collection.end(); ++it_hit) {
    const snemo::datamodel::calibrated_calorimeter_hit& a_hit = it_hit->get();

    this->highlight_geom_id(a_hit.get_geom_id(),
                            style_manager::get_instance().get_calibrated_data_color());

    if (options_manager::get_instance().get_option_flag(SHOW_CALIBRATED_INFO)) {
      const double energy = a_hit.get_energy();
      const double sigma_e = a_hit.get_sigma_energy();
      const double time = a_hit.get_time();
      const double sigma_t = a_hit.get_sigma_time();

      // Save z position inside text and then parse it
      std::ostringstream oss;
      oss.precision(2);
      oss << std::fixed << "#splitline{E = ";
      utils::root_utilities::get_prettified_energy(oss, energy, sigma_e, true);
      oss << "}{t  = ";
      utils::root_utilities::get_prettified_time(oss, time, sigma_t, true);
      oss << "}";
      this->highlight_geom_id(a_hit.get_geom_id(),
                              style_manager::get_instance().get_calibrated_data_color(), oss.str());
    }
  }
  return;
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of calorimeter_hit_renderer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
