/* status_bar.cc
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

#include <falaise/snemo/view/status_bar.h>
#include <falaise/snemo/view/event_browser_signals.h>
#include <falaise/snemo/view/options_manager.h>

#include <falaise/snemo/io/event_server.h>

#include <TGFrame.h>
#include <TGComboBox.h>
#include <TGButton.h>

#include <algorithm>

namespace snemo {

  namespace visualization {

    namespace view {

      bool status_bar::is_initialized() const
      {
        return _initialized_;
      }

      // ctor:
      status_bar::status_bar()
      {
        _server_      = 0;
        _initialized_ = false;
        return;
      }

      // dtor:
      status_bar::~status_bar()
      {
        this->reset();
        return;
      }

      void status_bar::set_event_server(io::event_server * server_)
      {
        DT_THROW_IF(is_initialized(),std::logic_error, "Already initialized !");
        _server_ = server_;
        return;
      }

      void status_bar::initialize(TGCompositeFrame * main_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
        this->_at_init_(main_);
        _initialized_ = true;
        return;
      }

      void status_bar::_at_init_(TGCompositeFrame * main_)
      {
        const int width  = main_->GetWidth();
        const int height = main_->GetHeight();

        // a horizontal frame
        TGHorizontalFrame * main_frame = new TGHorizontalFrame(main_, width, int(height*0.1));
        main_->AddFrame(main_frame,
                        new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX, 3, 3, 3, 3));

        // a combo box for the events
        _event_list_ = new TGComboBox(main_frame, STATUS_BAR);
        _event_list_->Resize(int(0.5*width), 20);
        _event_list_->Associate(main_);

        main_frame->AddFrame(_event_list_,
                             new TGLayoutHints(kLHintsTop|kLHintsLeft, 1, 1, 1, 1));

        // navigation buttons
        _button_first_    = new TGPictureButton(main_frame,
                                                gClient->GetPicture("first_t.xpm"), FIRST_EVENT);
        _button_previous_ = new TGPictureButton(main_frame,
                                                gClient->GetPicture("previous_t.xpm"), PREVIOUS_EVENT);
        _button_next_     = new TGPictureButton(main_frame,
                                                gClient->GetPicture("next_t.xpm"), NEXT_EVENT);
        _button_last_     = new TGPictureButton(main_frame,
                                                gClient->GetPicture("last_t.xpm"), LAST_EVENT);

        _button_first_    ->Associate(main_);
        _button_previous_ ->Associate(main_);
        _button_next_     ->Associate(main_);
        _button_last_     ->Associate(main_);

        this->reset();

        TGLayoutHints* layout =
          new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1);

        main_frame->AddFrame(_button_first_, layout);
        main_frame->AddFrame(_button_previous_, layout);
        main_frame->AddFrame(_button_next_, layout);
        main_frame->AddFrame(_button_last_, layout);
        return;
      }

      void status_bar::reset()
      {
        io::event_server & server = *_server_;

        // clear list
        _event_list_->RemoveAll();
        _event_list_->SetEnabled(true);

        if (!server.is_opened()) {
          _event_list_->AddEntry(" +++ NO EVENT LOADED +++ ", 0);
          _event_list_->Select(0);
          _event_list_->SetEnabled(false);
        }

        this->reset_buttons();
        return;
      }

      void status_bar::update(const bool reset_, const bool disable_)
      {
        io::event_server & server = *_server_;

        // Special mode
        if (! server.has_random_data()) {
          const int event_number = server.get_current_event_number();
          std::ostringstream status;
          status << " +++ EVENT " << event_number;
          if (server.has_sequential_data())
            status << " +++ SEQUENTIAL READING ONLY +++ ";
          if (server.has_external_data())
            status << " +++ EXTERNAL PROCESSING +++ ";
          _event_list_->RemoveAll();
          _event_list_->AddEntry(status.str().c_str(), 0);
          _event_list_->Select(0);
          _event_list_->SetEnabled(false);
          return;
        }

        if (reset_) {
          this->reset();
          const io::event_server::event_selection_list_type & event_selection_list
            = server.get_event_selection();
          if (event_selection_list.empty()) {
            _event_list_->AddEntry(" +++ NO EVENT MATCHED SELECTION +++ ", 0);
            _event_list_->Select(0);
          } else {
            for (io::event_server::event_selection_list_type::const_iterator
                   i_selection = event_selection_list.begin();
                 i_selection != event_selection_list.end();
                 ++i_selection) {
              const size_t ievent = *i_selection;
              const size_t total = event_selection_list.size()-1;
              std::ostringstream label;
              label << "event #" << ievent << "/"
                    << total << " ("
                    << (total > 0 ? int(double(ievent)/total * 100) : 100)
                    << "% complete)";
              _event_list_->AddEntry(label.str().c_str(), ievent);
            }
          }
        }

        if (disable_) _event_list_->SetEnabled(false);
        _event_list_->Select(server.get_current_event_number());
        return;
      }

      void status_bar::update_buttons(const button_signals_type signal_)
      {
        this->reset_buttons();

        switch(signal_) {
        case NEXT_EVENT:
          {
            const options_manager & options_mgr = options_manager::get_instance();
            if (options_mgr.is_automatic_event_reading_mode()) {
              _button_next_->SetPicture(gClient->GetPicture("ed_interrupt.png"));
              _button_next_->SetToolTipText("Auto reading mode");
            } else {
              _button_next_->SetPicture(gClient->GetPicture("next_t.xpm"));
              _button_next_->SetToolTipText("Next event");
            }
          }
          break;
        case LAST_EVENT:
          {
            _button_next_->SetEnabled(false);
            _button_last_->SetEnabled(false);
          }
          break;
        case FIRST_EVENT:
          {
            _button_first_   ->SetEnabled(false);
            _button_previous_->SetEnabled(false);
          }
          break;
        default:
          break;
        }

        if (_server_->get_current_event_number() == _server_->get_last_selected_event()) {
          _button_next_->SetEnabled(false);
          _button_last_->SetEnabled(false);
        }

        if (_server_->get_current_event_number() == _server_->get_first_selected_event()) {
          _button_first_   ->SetEnabled(false);
          _button_previous_->SetEnabled(false);
        }

        return;
      }

      void status_bar::reset_buttons()
      {
        _button_first_    ->SetToolTipText("First event");
        _button_previous_ ->SetToolTipText("Previous event");
        _button_next_     ->SetToolTipText("Next event");
        _button_last_     ->SetToolTipText("Last event");

        _button_first_    ->SetEnabled(false);
        _button_previous_ ->SetEnabled(false);
        _button_next_     ->SetEnabled(false);
        _button_last_     ->SetEnabled(false);

        if (!_server_->is_initialized())
          return;

        if (_server_->has_random_data()) {
          _button_first_    ->SetEnabled(true);
          _button_previous_ ->SetEnabled(true);
          _button_next_     ->SetEnabled(true);
          _button_last_     ->SetEnabled(true);
        } else {
          _button_first_    ->SetEnabled(false);
          _button_previous_ ->SetEnabled(false);
          _button_next_     ->SetEnabled(true);
          _button_last_     ->SetEnabled(false);
        }
        return;
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of status_bar.cc

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
