// -*- mode: c++ ; -*-
/* visual_track_renderer.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2011-11-04
 * Last modified: 2011-11-04
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
 *  Dedicated MC track renderer
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_VISUAL_TRACK_RENDERER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_VISUAL_TRACK_RENDERER_H 1

// Standard library:
#include <string>

// This project
#include <falaise/snemo/view/base_renderer.h>

namespace snemo {

  namespace visualization {

    namespace view {

      /// \brief A ROOT renderer dedicated to track visualization such as
      /// Monte-Carlo particle path or reconstructed particle track.
      class visual_track_renderer : public base_renderer
      {
      public:

        /// Default constructor
        visual_track_renderer();

        /// Destructor
        virtual ~visual_track_renderer();

        /// Build Monte-Carlo tracks
        void push_mc_tracks();

        /// Build Monte-Carlo legend
        void push_mc_legend();

        /// Build reconstructed tracks
        void push_reconstructed_tracks();
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_VISUAL_TRACK_RENDERER_H

// end of visual_track_renderer.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
