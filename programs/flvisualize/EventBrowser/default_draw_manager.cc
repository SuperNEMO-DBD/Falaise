/* default_draw_manager.cc
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

#include <EventBrowser/view/browser_tracks.h>
#include <EventBrowser/view/default_draw_manager.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/io/event_server.h>
#include <EventBrowser/log.h>

#include <TObjArray.h>
#include <TPolyMarker3D.h>

namespace snemo {

  namespace visualization {

    namespace view {

      // ctor:
      default_draw_manager::default_draw_manager(const io::event_server* server_) {
	FL_LOG_DEVEL("Entering...");
	_server_ = server_;

	_objects_ = new TObjArray(1000);
	_text_objects_ = new TObjArray(10);
	_objects_->SetOwner(true);
	_text_objects_->SetOwner(true);

	// Initialize renderers
	_calorimeter_hit_renderer_.initialize(_server_, _objects_, _text_objects_);
	_visual_track_renderer_.initialize(_server_, _objects_, _text_objects_);
	FL_LOG_DEVEL("Exiting...");
      }

      // dtor:
      default_draw_manager::~default_draw_manager() { this->default_draw_manager::reset(); }

      TObjArray* default_draw_manager::get_objects() { return _objects_; }

      TObjArray* default_draw_manager::get_text_objects() { return _text_objects_; }

      void default_draw_manager::update() {
	FL_LOG_DEVEL("Entering...");
	if (!_server_->is_initialized()) {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "Event server is not initialized !");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	// Add 'simulated_data' objects:
	if (_server_->get_event().has(io::SD_LABEL)) {
	  this->_add_simulated_data();
	} else {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "Event has no simulated data");
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void default_draw_manager::draw() { _objects_->Draw(); }

      void default_draw_manager::draw_text() { _text_objects_->Draw(); }

      void default_draw_manager::reset() {
	FL_LOG_DEVEL("Entering...");
	this->default_draw_manager::clear();

	delete _objects_;
	_objects_ = nullptr;

	delete _text_objects_;
	_text_objects_ = nullptr;

	_calorimeter_hit_renderer_.reset();
	_visual_track_renderer_.reset();
	FL_LOG_DEVEL("Exiting...");
      }

      void default_draw_manager::clear() {
	FL_LOG_DEVEL("Entering...");
	_calorimeter_hit_renderer_.clear();
	_visual_track_renderer_.clear();
	_objects_->Delete();
	_text_objects_->Delete();
	FL_LOG_DEVEL("Exiting...");
      }

      /***************************************************
       *  Filling objects from the 'simulated_data' bank *
       ***************************************************/

      void default_draw_manager::_add_simulated_data() {
	FL_LOG_DEVEL("Entering...");
	const options_manager& options_mgr = options_manager::get_instance();

	if (options_mgr.get_option_flag(SHOW_MC_VERTEX)) {
	  this->_add_simulated_vertex_();
	}

	if (options_mgr.get_option_flag(SHOW_MC_HITS)) {
	  this->_add_simulated_hits_();
	}

	if (options_mgr.get_option_flag(SHOW_MC_TRACKS)) {
	  _visual_track_renderer_.push_mc_tracks();
	  _visual_track_renderer_.push_mc_legend();
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void default_draw_manager::_add_simulated_vertex_() {
	FL_LOG_DEVEL("Entering...");
	const io::event_record& event = _server_->get_event();
	const auto& sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

	if (!sim_data.has_vertex()) {
	  DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
			     "Simulated data has no vertex");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	const geomtools::vector_3d& sim_vertex = sim_data.get_vertex();
	{
	  TPolyMarker3D* vertex_3d = base_renderer::make_polymarker(sim_vertex);
	  _objects_->Add(vertex_3d);
	  vertex_3d->SetMarkerColor(kViolet);
	  vertex_3d->SetMarkerStyle(kPlus);
	}
	if (sim_data.get_primary_event().get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
	  TPolyMarker3D* vertex_3d = base_renderer::make_polymarker(sim_vertex);
	  _objects_->Add(vertex_3d);
	  vertex_3d->SetMarkerColor(kViolet);
	  vertex_3d->SetMarkerStyle(kCircle);
	}
	FL_LOG_DEVEL("Exiting...");
      }

      void default_draw_manager::_add_simulated_hits_() {
	FL_LOG_DEVEL("Entering...");
	// 2015/07/07 XG: Here we assume that simulated step hit other than
	// visual tracks are calorimeter step hit. For dedicated setup such as
	// SuperNEMO, this method is overloaded for its own purpose making
	// distinction between calorimeter hits and Geiger hits
	const io::event_record& event = _server_->get_event();
	const auto& sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

	// Retrieve all category names except private category such as '__visual.tracks'
	std::vector<std::string> categories;
	sim_data.get_step_hits_categories(categories, mctools::simulated_data::HIT_CATEGORY_TYPE_PUBLIC);

	for (const auto& a_category : categories) {
	  const options_manager& options_mgr = options_manager::get_instance();
	  if (options_mgr.get_option_flag(SHOW_MC_CALORIMETER_HITS)) {
	    _calorimeter_hit_renderer_.push_simulated_hits(a_category);
	  }
	}
	FL_LOG_DEVEL("Exiting...");
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo
