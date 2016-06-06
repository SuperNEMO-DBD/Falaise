// -*- mode: c++ ; -*-
/* tracker_hit_renderer.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2011-11-03
 * Last modified: 2011-11-03
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
 *
 * Description:
 *
 *  Dedicated tracker hit renderer
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_TRACKER_HIT_RENDERER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_TRACKER_HIT_RENDERER_H 1

// Standard library
#include <string>
#include <set>

// This project
#include <falaise/snemo/io/data_model.h>
#include <falaise/snemo/view/base_renderer.h>

namespace snemo {

  namespace visualization {

    namespace view {

      /// \brief A dedicated renderer for tracker hits
      class tracker_hit_renderer : public base_renderer
      {
      public:

        /// Default constructor
        tracker_hit_renderer();

        /// Destructor
        virtual ~tracker_hit_renderer();

        /// Add objects from simulated hits
        void push_simulated_hits(const std::string & hit_category_ = "");

        /// Add objects from calibrated hits
        void push_calibrated_hits();

        /// Add objects from clustered hits
        void push_clustered_hits();

        /// Add objects from fitted hits
        void push_fitted_tracks();

      protected:

        /// Special method to show calibrated geiger hit
        void _make_calibrated_geiger_hit(const snemo::datamodel::calibrated_tracker_hit & hit_,
                                         const bool show_cluster = false);
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_TRACKER_HIT_RENDERER_H

// end of tracker_hit_renderer.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
