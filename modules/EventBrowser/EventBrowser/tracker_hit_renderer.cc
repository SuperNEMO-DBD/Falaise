/* tracker_hit_renderer.cc
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

// Third party:
// - ROOT:
#include <TColor.h>
#include <TMarker3DBox.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TRotation.h>

// - Bayeux/geomtools:
#include <bayeux/geomtools/manager.h>

// - Falaise:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/processing/geiger_regime.h>

// This project:
#include <EventBrowser/io/data_model.h>
#include <EventBrowser/view/browser_tracks.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/style_manager.h>
#include <EventBrowser/view/tracker_hit_renderer.h>

#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/detector/i_root_volume.h>

#include <EventBrowser/io/event_server.h>

namespace snemo {

namespace visualization {

namespace view {

// ctor:
tracker_hit_renderer::tracker_hit_renderer() = default;

// dtor:
tracker_hit_renderer::~tracker_hit_renderer() = default;

void tracker_hit_renderer::push_simulated_hits(const std::string &hit_category_) {
  const io::event_record &event = _server->get_event();
  const auto &sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

  if (!sim_data.has_step_hits(hit_category_)) {
    DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                       "Event has no '" << hit_category_ << "' tracker hits");
    return;
  }

  const mctools::simulated_data::hit_handle_collection_type &hit_collection =
      sim_data.get_step_hits(hit_category_);

  if (hit_collection.empty()) {
    DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(), "No tracker hits");
    return;
  }

  // time gradient color
  double hit_start_time = hit_collection.front().get().get_time_start();
  double hit_stop_time = hit_collection.front().get().get_time_start();

  const bool geiger_with_gradient =
      options_manager::get_instance().get_option_flag(SHOW_GG_TIME_GRADIENT);
  if (geiger_with_gradient) {
    for (const auto &it_hit : hit_collection) {
      hit_start_time = std::min(it_hit.get().get_time_start(), hit_start_time);
      hit_stop_time = std::max(it_hit.get().get_time_start(), hit_stop_time);
    }
  }

  for (const auto &it_hit : hit_collection) {
    const mctools::base_step_hit &a_step = it_hit.get();

    // draw the Geiger avalanche path:
    auto *gg_path = new TPolyLine3D;
    _objects->Add(gg_path);
    gg_path->SetPoint(0, a_step.get_position_start().x(), a_step.get_position_start().y(),
                      a_step.get_position_start().z());
    gg_path->SetPoint(1, a_step.get_position_stop().x(), a_step.get_position_stop().y(),
                      a_step.get_position_stop().z());

    const auto time_percent =
        (size_t)((TColor::GetNumberOfColors() - 1) * (a_step.get_time_start() - hit_start_time) /
                 (hit_stop_time - hit_start_time));
    const size_t color = geiger_with_gradient ? TColor::GetColorPalette(time_percent) : kSpring;
    gg_path->SetLineColor(color);

    // Store this value into cluster properties:
    auto *mutable_hit = const_cast<mctools::base_step_hit *>(&(a_step));
    datatools::properties &hit_properties = mutable_hit->grab_auxiliaries();
    const long pixel = TColor::Number2Pixel(color);
    const std::string hex_str = TColor::PixelAsHexString(pixel);
    hit_properties.update(browser_tracks::COLOR_FLAG, hex_str);

    // Retrieve line width from properties if 'hit' is highlighted:
    size_t line_width = style_manager::get_instance().get_mc_line_width();
    if (hit_properties.has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
      line_width = 3;
    }
    // hit_properties.update(browser_tracks::HIGHLIGHT_FLAG, false);
    gg_path->SetLineWidth(line_width);

    // draw circle tangential to the track:
    if (options_manager::get_instance().get_option_flag(SHOW_GG_CIRCLE)) {
      const size_t n_point = 100;
      TRotation dr;
      int cell_axis = 'z';

      const detector::detector_manager &detector_mgr = detector::detector_manager::get_instance();
      const std::string &setup_label = detector_mgr.get_setup_label_name();
      if (setup_label == "snemo::tracker_commissioning") {
        cell_axis = 'x';
      }

      if (cell_axis == 'z') {
        dr.RotateZ(2 * TMath::Pi() / (double)n_point);
      } else if (cell_axis == 'x') {
        dr.RotateX(2 * TMath::Pi() / (double)n_point);
      } else {
        DT_THROW_IF(true, std::logic_error, "Unsupported cell axis !");
      }

      geomtools::polyline_type points;
      if (cell_axis == 'z') {
        TVector3 current_pos((a_step.get_position_start() - a_step.get_position_stop()).x(),
                             (a_step.get_position_start() - a_step.get_position_stop()).y(),
                             a_step.get_position_stop().z());
        for (size_t i_point = 0; i_point <= n_point; ++i_point) {
          current_pos *= dr;
          points.push_back(geomtools::vector_3d(current_pos.x() + a_step.get_position_stop().x(),
                                                current_pos.y() + a_step.get_position_stop().y(),
                                                current_pos.z()));
        }
      } else if (cell_axis == 'x') {
        TVector3 current_pos(a_step.get_position_stop().x(),
                             (a_step.get_position_start() - a_step.get_position_stop()).y(),
                             (a_step.get_position_start() - a_step.get_position_stop()).z());

        for (size_t i_point = 0; i_point <= n_point; ++i_point) {
          current_pos *= dr;
          points.push_back(geomtools::vector_3d(current_pos.x(),
                                                current_pos.y() + a_step.get_position_stop().y(),
                                                current_pos.z() + a_step.get_position_stop().z()));
        }
      }

      TPolyLine3D *gg_drift = base_renderer::make_polyline(points);
      _objects->Add(gg_drift);
      gg_drift->SetLineColor(color);
      gg_drift->SetLineWidth(line_width);
    }  // end of "show geiger drift circle" condition
  }    // end of step collection
}

void tracker_hit_renderer::push_calibrated_hits() {
  const io::event_record &event = _server->get_event();
  const auto &calib_data = event.get<snemo::datamodel::calibrated_data>(io::CD_LABEL);

  const snemo::datamodel::TrackerHitHdlCollection &ct_collection = calib_data.tracker_hits();

  if (ct_collection.empty()) {
    DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                       "No calibrated tracker hits");
    return;
  }

  for (const auto &it_hit : ct_collection) {
    const snemo::datamodel::calibrated_tracker_hit &a_hit = it_hit.get();
    tracker_hit_renderer::_make_calibrated_geiger_hit(a_hit, false);
  }
}

void tracker_hit_renderer::push_clustered_hits() {
  const io::event_record &event = _server->get_event();
  const auto &tracker_clustered_data =
      event.get<snemo::datamodel::tracker_clustering_data>(io::TCD_LABEL);

  for (const auto &cluster_solution : tracker_clustered_data.solutions()) {
    // Get current tracker solution:
    const snemo::datamodel::tracker_clustering_solution &a_solution = cluster_solution.get();

    // Check solution properties:
    if (a_solution.get_auxiliaries().has_key(browser_tracks::CHECKED_FLAG) &&
        !a_solution.get_auxiliaries().has_flag(browser_tracks::CHECKED_FLAG)) {
      continue;
    }

    // Get clusters stored in the current tracker solution:
    const snemo::datamodel::TrackerClusterHdlCollection &clusters = a_solution.get_clusters();

    for (auto icluster = clusters.begin(); icluster != clusters.end(); ++icluster) {
      // Get current tracker cluster:
      const snemo::datamodel::tracker_cluster &a_cluster = icluster->get();

      // Check cluster properties:
      if (a_cluster.get_auxiliaries().has_key(browser_tracks::CHECKED_FLAG) &&
          !a_cluster.get_auxiliaries().has_flag(browser_tracks::CHECKED_FLAG)) {
        continue;
      }

      // Determine cluster color
      const size_t cluster_color =
          style_manager::get_instance().get_color(std::distance(clusters.begin(), icluster));

      // Store this value into cluster properties:
      auto *mutable_cluster = const_cast<snemo::datamodel::tracker_cluster *>(&(a_cluster));
      datatools::properties &cluster_properties = mutable_cluster->grab_auxiliaries();
      const long pixel = TColor::Number2Pixel(cluster_color);
      const std::string hex_str = TColor::PixelAsHexString(pixel);
      cluster_properties.update(browser_tracks::COLOR_FLAG, hex_str);

      // Get tracker hits stored in the current tracker cluster:
      const snemo::datamodel::TrackerHitHdlCollection &hits = a_cluster.hits();

      // Make a gradient color starting from color_solution:
      for (const auto &hit : hits) {
        const snemo::datamodel::calibrated_tracker_hit &a_gg_hit = hit.get();

        // Retrieve a mutable reference to calibrated_tracker_hit:
        auto *mutable_hit = const_cast<snemo::datamodel::calibrated_tracker_hit *>(&(a_gg_hit));
        datatools::properties &gg_properties = mutable_hit->grab_auxiliaries();

        // Store current color to be used by calibrated_tracker_hit renderer:
        const long ppixel = TColor::Number2Pixel(cluster_color);
        const std::string hhex_str = TColor::PixelAsHexString(ppixel);
        gg_properties.update(browser_tracks::COLOR_FLAG, hhex_str);

        const options_manager &options_mgr = options_manager::get_instance();
        if (options_mgr.get_option_flag(SHOW_TRACKER_CLUSTERED_BOX)) {
          const double x = a_gg_hit.get_x();
          const double y = a_gg_hit.get_y();
          const double z = a_gg_hit.get_z();
          const double dz = a_gg_hit.get_sigma_z();
          const double r = 22.0 / CLHEP::mm;

          auto *hit_3d = new TMarker3DBox;
          _objects->Add(hit_3d);
          hit_3d->SetPosition(x, y, z);
          hit_3d->SetSize(r, r, dz);
          hit_3d->SetLineColor(cluster_color);
          // Retrieve line width from properties if 'hit' is highlighted:
          size_t line_width = 1;
          if (gg_properties.has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
            line_width = 3;
          }
          // gg_properties.update(browser_tracks::HIGHLIGHT_FLAG, false);
          hit_3d->SetLineWidth(line_width);
        } else if (options_mgr.get_option_flag(SHOW_TRACKER_CLUSTERED_CIRCLE)) {
          tracker_hit_renderer::_make_calibrated_geiger_hit(a_gg_hit, true);
        }
      }  // end of gg hits
    }    // end of cluster loop
  }      // end of solution loop
}

void tracker_hit_renderer::push_fitted_tracks() {
  const io::event_record &event = _server->get_event();
  const auto &tracker_trajectory_data =
      event.get<snemo::datamodel::tracker_trajectory_data>(io::TTD_LABEL);

  const snemo::datamodel::TrackerTrajectorySolutionHdlCollection &trajectory_solutions =
      tracker_trajectory_data.get_solutions();
  for (const auto &isolution : trajectory_solutions) {
    // Get current tracker trajectory solution:
    const snemo::datamodel::tracker_trajectory_solution &a_solution = isolution.get();

// DONT couple data model to view!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    // Check solution properties:
    if (a_solution.get_auxiliaries().has_key(browser_tracks::CHECKED_FLAG) &&
        !a_solution.get_auxiliaries().has_flag(browser_tracks::CHECKED_FLAG)) {
      continue;
    }
#pragma GCC diagnostic pop

    // Get trajectories stored in the current tracker trajectory solution:
    const snemo::datamodel::TrackerTrajectoryHdlCollection &trajectories =
        a_solution.get_trajectories();

    for (const auto &itrajectory : trajectories) {
      // Get current tracker trajectory:
      const snemo::datamodel::tracker_trajectory &a_trajectory = itrajectory.get();

      // Get trajectory properties:
      const datatools::properties &traj_properties = a_trajectory.get_auxiliaries();

      // Check if trajectory has to be shown or not:
      if (traj_properties.has_key(browser_tracks::CHECKED_FLAG)) {
        if (!traj_properties.has_flag(browser_tracks::CHECKED_FLAG)) {
          continue;
        }
      } else {
        if (!traj_properties.has_flag("default")) {
          continue;
        }
      }

      // Retrieve trajectory visual rendering:
      const snemo::datamodel::base_trajectory_pattern &a_pattern = a_trajectory.get_pattern();
      const auto &iw3dr =
          dynamic_cast<const geomtools::i_wires_3d_rendering &>(a_pattern.get_shape());
      TPolyLine3D *track = base_renderer::make_track(iw3dr);
      _objects->Add(track);

      // Determine trajectory color by getting cluster color:
      int trajectory_color = 0;
      if (a_trajectory.has_cluster()) {
        const snemo::datamodel::tracker_cluster &a_cluster = a_trajectory.get_cluster();
        const datatools::properties &prop = a_cluster.get_auxiliaries();
        if (prop.has_key(browser_tracks::COLOR_FLAG)) {
          const std::string hex_str = prop.fetch_string(browser_tracks::COLOR_FLAG);
          trajectory_color = TColor::GetColor(hex_str.c_str());
        }
      }
      track->SetLineColor(trajectory_color);

      // Retrieve line width from properties if 'track' is highlighted:
      size_t line_width = 1;
      if (traj_properties.has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
        line_width = 3;
      }
      track->SetLineWidth(line_width);

      // For delayed tracks such as alpha track, show the recalibrated
      // tracker hit
      if (traj_properties.has_key("t0") &&
          options_manager::get_instance().get_option_flag(SHOW_RECALIBRATED_TRACKER_HITS)) {
        const double t0 = traj_properties.fetch_real("t0");
        snemo::processing::geiger_regime a_gg_regime;
        if (a_trajectory.has_cluster()) {
          const snemo::datamodel::tracker_cluster &a_cluster = a_trajectory.get_cluster();
          // Get tracker hits stored in the current tracker cluster:
          const snemo::datamodel::TrackerHitHdlCollection &hits = a_cluster.hits();

          for (const auto &igg : hits) {
            snemo::datamodel::calibrated_tracker_hit a_gg_hit_copy = igg.get();
            // Recalibrate drift radius given fitted t0
            double new_r = datatools::invalid_real();
            double new_sigma_r = datatools::invalid_real();
            double new_anode_time = datatools::invalid_real();
            if (a_gg_hit_copy.is_delayed()) {
              new_anode_time = a_gg_hit_copy.get_delayed_time() - t0;
            } else {
              new_anode_time = a_gg_hit_copy.get_anode_time() - t0;
            }
            if (new_anode_time < 0.0 * CLHEP::ns) {
              new_anode_time = 0.0 * CLHEP::ns;
            }
            a_gg_regime.calibrateRadiusFromTime(new_anode_time, new_r, new_sigma_r);
            a_gg_hit_copy.set_r(new_r);
            a_gg_hit_copy.set_sigma_r(new_sigma_r);
            a_gg_hit_copy.set_delayed(false);
            tracker_hit_renderer::_make_calibrated_geiger_hit(a_gg_hit_copy, true);
          }
        }
      }

    }  // end of trajectory loop
  }    // end of solution loop
}

void tracker_hit_renderer::_make_calibrated_geiger_hit(
    const snemo::datamodel::calibrated_tracker_hit &hit_, const bool show_cluster) {
  // Compute the position of the anode impact in the drift cell coordinates reference frame:
  const detector::detector_manager &detector_mgr = detector::detector_manager::get_instance();

  geomtools::vector_3d cell_module_pos(hit_.get_x(), hit_.get_y(), hit_.get_z());
  geomtools::vector_3d cell_world_pos;
  detector_mgr.compute_world_coordinates(cell_module_pos, cell_world_pos);

  // Get (x, y) position of triggered cell
  const double x = cell_world_pos.x();
  const double y = cell_world_pos.y();

  // Add error in z coordinate
  const double z = cell_world_pos.z();
  const double sigma_z = hit_.get_sigma_z();

  // Get hit auxiliaries
  const datatools::properties &aux = hit_.get_auxiliaries();

  // Retrieve line width from properties if 'hit' is highlighted:
  size_t line_width = style_manager::get_instance().get_mc_line_width();
  if (aux.has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
    line_width = 3;
  }

  int color = style_manager::get_instance().get_calibrated_data_color();
  if (show_cluster && aux.has_key(browser_tracks::COLOR_FLAG)) {
    std::string hex_str;
    aux.fetch(browser_tracks::COLOR_FLAG, hex_str);
    const options_manager &options_mgr = options_manager::get_instance();
    if (options_mgr.get_option_flag(SHOW_TRACKER_CLUSTERED_HITS) &&
        options_mgr.get_option_flag(SHOW_TRACKER_CLUSTERED_CIRCLE)) {
      color = TColor::GetColor(hex_str.c_str());
    }
  }
  auto *gg_dz = new TPolyLine3D;
  _objects->Add(gg_dz);
  gg_dz->SetLineColor(color);
  gg_dz->SetLineWidth(line_width);

  int cell_axis = 'z';
  const std::string &setup_label = detector_mgr.get_setup_label_name();
  if (setup_label == "snemo::tracker_commissioning") {
    cell_axis = 'x';
  }
  DT_THROW_IF(cell_axis != 'z' && cell_axis != 'x', std::logic_error, "Unsupported cell axis !");

  if (cell_axis == 'z') {
    gg_dz->SetPoint(0, x, y, z - sigma_z);
    gg_dz->SetPoint(1, x, y, z + sigma_z);
  } else if (cell_axis == 'x') {
    gg_dz->SetPoint(0, x - sigma_z, y, z);
    gg_dz->SetPoint(1, x + sigma_z, y, z);
  }

  if (hit_.is_delayed()) {
    const double r = 22.0 / CLHEP::mm;  // hit_.get_r();
    geomtools::polyline_type points;
    points.push_back(geomtools::vector_3d(x + r, y + r, z));
    points.push_back(geomtools::vector_3d(x + r, y - r, z));
    points.push_back(geomtools::vector_3d(x - r, y - r, z));
    points.push_back(geomtools::vector_3d(x - r, y + r, z));
    points.push_back(geomtools::vector_3d(x + r, y + r, z));

    TPolyLine3D *gg_drift_square = base_renderer::make_polyline(points);
    _objects->Add(gg_drift_square);
    gg_drift_square->SetLineColor(color);
    gg_drift_square->SetLineWidth(line_width);

  } else {
    // add calibrated drift value:  r-dr; r+dr
    const size_t n_point = 100;
    TRotation dr;
    if (cell_axis == 'z') {
      dr.RotateZ(2 * TMath::Pi() / (double)n_point);
    } else if (cell_axis == 'x') {
      dr.RotateX(2 * TMath::Pi() / (double)n_point);
    }

    // get calibrated info
    const double r = hit_.get_r();
    const double sigma_r = hit_.get_sigma_r();

    geomtools::polyline_type rmins;
    geomtools::polyline_type rmaxs;
    if (cell_axis == 'z') {
      TVector3 r_min(r - sigma_r, 0, z);
      TVector3 r_max(r + sigma_r, 0, z);

      for (size_t i_point = 0; i_point <= n_point; ++i_point) {
        r_min *= dr;
        r_max *= dr;
        rmins.push_back(geomtools::vector_3d(r_min.x() + x, r_min.y() + y, r_min.z()));
        rmaxs.push_back(geomtools::vector_3d(r_max.x() + x, r_max.y() + y, r_max.z()));
      }
    } else if (cell_axis == 'x') {
      TVector3 r_min(x, r - sigma_r, 0);
      TVector3 r_max(x, r + sigma_r, 0);

      for (size_t i_point = 0; i_point <= n_point; ++i_point) {
        r_min *= dr;
        r_max *= dr;
        rmins.push_back(geomtools::vector_3d(r_min.x(), r_min.y() + y, r_min.z() + z));
        rmaxs.push_back(geomtools::vector_3d(r_max.x(), r_max.y() + y, r_max.z() + z));
      }
    }
    TPolyLine3D *gg_drift_min = base_renderer::make_polyline(rmins);
    _objects->Add(gg_drift_min);
    gg_drift_min->SetLineColor(color);
    gg_drift_min->SetLineWidth(line_width);

    TPolyLine3D *gg_drift_max = base_renderer::make_polyline(rmaxs);
    _objects->Add(gg_drift_max);
    gg_drift_max->SetLineColor(color);
    gg_drift_max->SetLineWidth(line_width);
  }
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of tracker_hit_renderer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
