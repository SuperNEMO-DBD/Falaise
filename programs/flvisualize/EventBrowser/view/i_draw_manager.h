// -*- mode: c++ ; -*-
/* i_draw_manager.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-11-20
 * Last modified: 2010-11-20
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
 *  interface class to implement drawing base methods
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_I_DRAW_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_I_DRAW_MANAGER_H 1

#include <EventBrowser/view/utils.h>

// class TObjArray;

namespace snemo {

	namespace visualization {

		namespace view {

			class i_draw_manager {
			public:
				i_draw_manager();
				virtual ~i_draw_manager();

				virtual TObjArray* get_objects() = 0;
				virtual TObjArray* get_text_objects() = 0;

				virtual void update() = 0;
				virtual void draw() = 0;
				virtual void draw_text() = 0;
				virtual void clear() = 0;
				virtual void reset() = 0;
			};

		} // end of namespace view

	} // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_I_DRAW_MANAGER_H
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
