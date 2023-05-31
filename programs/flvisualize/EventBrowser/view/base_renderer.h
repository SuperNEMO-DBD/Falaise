// -*- mode: c++ ; -*-
/* base_renderer.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2014-10-04
 * Last modified: 2014-10-04
 *
 * Copyright (C) 2014 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *
 *  Rendered interface
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_BASE_RENDERER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_BASE_RENDERER_H 1

// Standard library
#include <set>
#include <string>
#include <vector>
#include <memory>

// Bayeux
// - geomtools
#include <geomtools/utils.h>

#include <EventBrowser/view/utils.h>

namespace geomtools {
	class geom_id;
	class helix_3d;
	class line_3d;
	class i_wires_3d_rendering;
}  // namespace geomtools

// class TObjArray;
class TPolyLine3D;
class TPolyMarker3D;
#include <TObject.h>

namespace snemo {

	namespace visualization {

		namespace io {
			class event_server;
		}

		namespace view {

      class base_renderer {
      public:
        /// Unique set of geomtools::geom_id
        typedef std::set<geomtools::geom_id> geom_id_collection;

        /// Return initialization status
        bool is_initialized() const;

        /// Check if server is referenced
        bool has_server() const;

        /// Check if graphical object is referenced
        bool has_graphical_objects() const;

        /// Check if text object is referenced
				bool has_text_objects() const;

				/// Set event server reference
				void set_server(const io::event_server* server_);

				/// Set graphical ROOT::TObject reference
				void set_graphical_objects(TObjArray* objects_);

				/// Set text ROOT::TObject reference
				void set_text_objects(TObjArray* text_objects_);

				/// Default constructor
				base_renderer();

				/// Destructor
				virtual ~base_renderer();

				/// Initialization
				void initialize(const io::event_server* server_ = 0, TObjArray* objects_ = 0,
												TObjArray* text_objects_ = 0);

				/// Clear
				void clear();

				/// Reset
				void reset();

				/// Visually highlight a given geomtools::geom_id object
				void highlight_geom_id(const geomtools::geom_id& gid_, const size_t color_,
															 const std::string& text_ = "");

				/// Build a marker from a 3D point
				static TPolyMarker3D* make_polymarker(const geomtools::vector_3d& point_,
																							const bool convert_ = false);

				/// Build a polyline from a set of 3D points
				static TPolyLine3D* make_polyline(const geomtools::polyline_type& polyline_,
																					const bool convert_ = false);

				/// Build a polyline from an track
				static TPolyLine3D* make_track(const geomtools::i_wires_3d_rendering& iw3dr_,
																			 const bool convert_ = false);

			protected:
				
				bool _initialized = false;  //!< Initialization flag
				const io::event_server* _server = nullptr;  //!< Event server
				TObjArray* _objects = nullptr;              //!< ROOT graphical objects container
				TObjArray* _text_objects = nullptr;         //!< ROOT text objects container
				geom_id_collection _highlighted_geom_id;  //!< List of geom_id highlighted
				
			};

		} // end of namespace view

	} // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_BASE_RENDERER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
