// -*- mode: c++ ; -*-
/* snemo_draw_manager.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-11-20
 * Last modified: 2014-10-01
 *
 * Copyright (C) 2011-2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *
 * Description:
 *  SuperNEMO specific drawer
 *
 * History:
 *
 * 2014-10-01 : Doxygenation
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_SNEMO_DRAW_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_SNEMO_DRAW_MANAGER_H 1

// Standard library:
#include <string>

// This project:
#include <EventBrowser/view/calorimeter_hit_renderer.h>
#include <EventBrowser/view/i_draw_manager.h>
#include <EventBrowser/view/tracker_hit_renderer.h>
#include <EventBrowser/view/visual_track_renderer.h>

// class TObjArray;

namespace snemo {

	namespace visualization {

		namespace io {
			class event_server;
		}

		namespace view {

			/// \brief A dedicated event drawer for SuperNEMO physics
			class snemo_draw_manager : public i_draw_manager {
			public:
				/// Default constructor
				snemo_draw_manager(const io::event_server* server_);

				/// Destructor
				virtual ~snemo_draw_manager();

				/// Getting geometrical ROOT::TObject array
				virtual TObjArray* get_objects();

				/// Getting text ROOT::TObject array
				virtual TObjArray* get_text_objects();

				/// Update event visualization objects
				virtual void update();

				/// Drawing all geometrical scene objects
				virtual void draw();

				/// Drawing all text scene objects
				virtual void draw_text();

				/// Clearing objects
				virtual void clear();

				/// Reset
				virtual void reset();

			protected:
				/// Add 'simulated_data' bank objects
				void _add_simulated_data();

				/// Add 'calibrated_data' bank objects
				void _add_calibrated_data();

				/// Add 'tracker_clustering_data' bank objects
				void _add_tracker_clustering_data();

				/// Add 'tracker_trajectory_data' bank objects
				void _add_tracker_trajectory_data();

				/// Add 'particle_track_data' bank objects
				void _add_particle_track_data();

			private:
				/// Add simulated vertex position
				void _add_simulated_vertex_();

				/// Add simulated sensitive hits
				void _add_simulated_hits_();

			private:
				const io::event_server* _server_;  //!< Pointer to event server
				TObjArray* _objects_;              //!< ROOT array for geometrical TObject
				TObjArray* _text_objects_;         //!< ROOT array for textual TObject

				calorimeter_hit_renderer _calorimeter_hit_renderer_;  //<! Calorimeter hit renderer
				tracker_hit_renderer _tracker_hit_renderer_;          //<! Tracker hit renderer
				visual_track_renderer _visual_track_renderer_;        //<! Visual track renderer
			};

		} // end of namespace view

	} // end of namespace visualization

} // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_SNEMO_DRAW_MANAGER_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
