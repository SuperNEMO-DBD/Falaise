// -*- mode: c++ ; -*-
/* progress_bar.h
 * Author(s): Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-12-10
 * Last modified: 2010-12-10
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
 * Display a progress bar window when processing event
 *
 * History:
 *
 */

#ifndef __progress_bar_h
#define __progress_bar_h 1

#include <string>

#include <TGFrame.h>

class TGHProgressBar;

namespace snemo {

  namespace visualization {

    namespace view {

      class progress_bar  : public TGMainFrame
      {

      public:

        progress_bar (const TGWindow *window_,
                      const int width_, const int height_,
                      const std::string & window_name_ = "",
                      const std::string & label_format_ = "");
        virtual ~progress_bar ();

        bool increment (const unsigned int i = 1);
        void set_range (const unsigned int min_ = 0,
                        const unsigned int max_ = 100);

        // slots
        void kill_application (const bool killed_ = true);
        void close_window ();
        void do_close ();

      private:
        TGHProgressBar * _bar_;
        bool             _is_progressing_;
        bool             _is_killing_application_;

        ClassDef (progress_bar, 0);
      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // __progress_bar_h

// end of progress_bar.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
