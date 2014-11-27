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

#include <falaise/snemo/view/base_renderer.h>
#include <falaise/snemo/view/browser_tracks.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/style_manager.h>

#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/detector/i_root_volume.h>

#include <falaise/snemo/io/event_server.h>

#include <falaise/snemo/utils/root_utilities.h>

#include <geomtools/id_mgr.h>
#include <geomtools/helix_3d.h>
#include <geomtools/line_3d.h>

#include <TObjArray.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>

namespace snemo {

  namespace visualization {

    namespace view {

      bool base_renderer::is_initialized() const
      {
        return _initialized;
      }

      bool base_renderer::has_server() const
      {
        return _server != 0;
      }

      void base_renderer::set_server(const io::event_server * server_)
      {
        _server = server_;
        return;
      }

      bool base_renderer::has_graphical_objects() const
      {
        return _objects != 0;
      }

      void base_renderer::set_graphical_objects(TObjArray * objects_)
      {
        _objects = objects_;
        return;
      }

      bool base_renderer::has_text_objects() const
      {
        return _text_objects != 0;
      }

      void base_renderer::set_text_objects(TObjArray * text_objects_)
      {
        _text_objects = text_objects_;
        return;
      }

      // ctor:
      base_renderer::base_renderer()
      {
        _initialized  = false;
        _server       = 0;
        _objects      = 0;
        _text_objects = 0;
        return;
      }

      // dtor:
      base_renderer::~base_renderer()
      {
        this->reset();
        return;
      }

      void base_renderer::initialize(const io::event_server * server_,
                                     TObjArray              * objects_,
                                     TObjArray              * text_objects_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
        this->set_server(server_);
        this->set_graphical_objects(objects_);
        this->set_text_objects(text_objects_);
        _initialized = true;
        return;
      }

      void base_renderer::clear()
      {
        for (geom_id_collection::const_iterator gid = _highlighted_geom_id.begin();
             gid != _highlighted_geom_id.end(); ++gid) {
          detector::detector_manager & detector_mgr = detector::detector_manager::get_instance();
          detector::i_volume* volume_hit = detector_mgr.grab_volume(*gid);
          volume_hit->clear();
        }
        _highlighted_geom_id.clear();
        return;
      }

      void base_renderer::reset()
      {
        DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
        this->clear();
        _initialized = false;
        return;
      }

      void base_renderer::highlight_geom_id(const geomtools::geom_id & gid_,
                                            const size_t color_,
                                            const std::string & text_)
      {
        DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
                     "Geom id '" << gid_ << "' is hit");

        // Highlight calorimeter block
        detector::detector_manager & detector_mgr = detector::detector_manager::get_instance();

        // Get matching list of geom_id (using 'any_adress' concept)
        std::vector<geomtools::geom_id> gid_list;
        detector_mgr.get_matching_ids(gid_, gid_list);

        if (gid_list.empty()) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "No calorimeter volume with geom_id '" << gid_
                         << "' has been found ! Check either geometry file or style file "
                         << "in case this detector part is disable");
          return;
        }

        if (text_.empty()) {
          for (std::vector<geomtools::geom_id>::iterator igid = gid_list.begin();
               igid != gid_list.end(); ++igid) {
            detector::i_root_volume* volume_hit
              = dynamic_cast<detector::i_root_volume*>(detector_mgr.grab_volume(*igid));
            volume_hit->highlight(color_);
            _highlighted_geom_id.insert(*igid);
          }
        } else {
          // Here we get the first element :
          detector::i_root_volume * volume_hit
            = dynamic_cast<detector::i_root_volume *>(detector_mgr.grab_volume(gid_list.front()));
          utils::root_utilities::TLatex3D * new_text_obj = new utils::root_utilities::TLatex3D;
          _text_objects->Add(new_text_obj);
          new_text_obj->SetTextColor(color_);
          const geomtools::vector_3d & position = volume_hit->get_placement().get_translation();
          new_text_obj->SetX(position.x());
          new_text_obj->SetY(position.y());
          new_text_obj->SetZ(position.z());
          new_text_obj->SetText(text_);
        }
        return;
      }

      TPolyMarker3D * base_renderer::make_vertex(const geomtools::vector_3d & point_)
      {
        TPolyMarker3D * marker = new TPolyMarker3D;
        marker->SetPoint(0, point_.x(), point_.y(), point_.z());
        return marker;
      }

      TPolyLine3D * base_renderer::make_polyline(const std::vector<geomtools::vector_3d> & points_)
      {
        TPolyLine3D * polyline = new TPolyLine3D;
        for (size_t i =0; i < points_.size(); ++i) {
          const geomtools::vector_3d & a_point = points_[i];
          polyline->SetPoint(i, a_point.x(), a_point.y(), a_point.z());
        }
        return polyline;
      }

      TPolyLine3D * base_renderer::make_line_track(const geomtools::line_3d & line_)
      {
        std::vector<geomtools::vector_3d> points;
        points.push_back(line_.get_first());
        points.push_back(line_.get_last());
        return make_polyline(points);
      }

      TPolyLine3D * base_renderer::make_helix_track(const geomtools::helix_3d & helix_)
      {
        // From print_xyz method of geomtools::helix_3d class:
        double delta_t = 1. / 360.; // default
        const double step_angle = 0.01;
        if (step_angle > 0.0) delta_t = geomtools::helix_3d::angle_to_t(step_angle);

        double t_skip = 0.0;
        double t_min = helix_.get_t1() - t_skip;
        double t_max = helix_.get_t2() + t_skip;
        double t = t_min;
        bool stop = false;
        std::vector<geomtools::vector_3d> points;
        do {
          const geomtools::vector_3d & v = helix_.get_point(t);
          points.push_back(v);

          if (stop) break;
          t += delta_t;
          if (t > t_max) {
            t = t_max;
            stop = true;
          }
        } while (true);

        return make_polyline(points);
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of base_renderer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
