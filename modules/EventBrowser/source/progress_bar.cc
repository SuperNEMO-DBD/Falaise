/* progress_bar.cc
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

#include <falaise/snemo/view/progress_bar.h>

#include <TGProgressBar.h>
#include <TGButton.h>
#include <TTimer.h>
#include <TSystem.h>
#include <TApplication.h>

#include <iostream>

ClassImp (snemo::visualization::view::progress_bar);

namespace snemo {

  namespace visualization {

    namespace view {

      // ctor:
      progress_bar::progress_bar (const TGWindow * window_,
                                  const int width_, const int height_,
                                  const std::string & window_name_,
                                  const std::string & label_format_) :
        TGMainFrame (window_, width_, height_),
        _bar_ (0),
        _is_progressing_ (true),
        _is_killing_application_ (false)
      {
        // connect virtual method CloseWindow to close_window
        this->Connect ("CloseWindow ()",
                       "snemo::visualization::view::progress_bar", this, "do_close ()");

        // to avoid double deletions.
        this->DontCallClose ();

        // use hierarchical cleaning
        this->SetCleanup (kDeepCleanup);

        this->ChangeOptions((this->GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

        TGVerticalFrame* main_frame = new TGVerticalFrame (this, 0, 0, 0);

        _bar_ = new TGHProgressBar (main_frame, TGProgressBar::kFancy, width_);
        _bar_->SetBarColor ("lightblue");
        _bar_->ShowPosition (kTRUE, kFALSE, label_format_.c_str ());

        TGTextButton* stop_button = new TGTextButton (main_frame, " Stop process ", 10);
        stop_button->Connect ("Clicked()",
                              "snemo::visualization::view::progress_bar", this, "do_close ()");

        main_frame->AddFrame (_bar_,
                              new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 5,  5, 10));
        main_frame->AddFrame (stop_button,
                              new TGLayoutHints(kLHintsTop | kLHintsCenterX, 10, 10, 10, 10));

        this->AddFrame (main_frame,
                        new TGLayoutHints(kLHintsTop | kLHintsCenterX, 0, 0, 0, 0));

        this->SetWindowName (window_name_.c_str ());

        TGDimension size = this->GetDefaultSize();
        this->Resize(size);

        this->MapSubwindows();
        this->MapWindow();
        return;
      }

      // dtor:
      progress_bar::~progress_bar ()
      {
        return;
      }

      void progress_bar::close_window ()
      {
        // Called when window is closed via the window manager.
        TGMainFrame::CloseWindow();

        if (_is_killing_application_)
          gApplication->Terminate ();
        return;
      }

      void progress_bar::do_close ()
      {
        if (!_is_progressing_)
          close_window ();
        else
          {
            _is_progressing_ = false;
            TTimer::SingleShot(150, "snemo::visualization::view::progress_bar",
                               this, "close_window ()");
          }
        return;
      }

      void progress_bar::kill_application (const bool killed_)
      {
        _is_killing_application_ = killed_;
        return;
      }

      bool progress_bar::increment (const unsigned int i_)
      {
        _bar_->Increment (i_);
        gSystem->ProcessEvents ();

        return _is_progressing_;
      }

      void progress_bar::set_range (const unsigned int min_,
                                    const unsigned int max_)
      {
        _bar_->SetRange (min_, max_);
        return;
      }


    } // end of namespace utils

  } // end of namespace visualization

} // end of namespace snemo

// end of progress_bar.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
