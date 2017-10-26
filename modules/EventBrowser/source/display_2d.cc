/* display_2d.cc
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

#include <falaise/snemo/view/display_2d.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/pad_embedded_viewer.h>
#include <falaise/snemo/view/style_manager.h>

#ifdef SNVISUALIZATION_USE_OPENGL
#include <falaise/snemo/view/opengl_embedded_viewer.h>
#endif

#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/io/event_server.h>
#include <falaise/snemo/utils/root_utilities.h>

#include <TCanvas.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGFrame.h>

ClassImp(snemo::visualization::view::display_2d);

    namespace snemo {
  namespace visualization {

  namespace view {

  void display_2d::set_view_type(const view_type vtype_) {
    _2d_viewer_->set_view_type(vtype_);
    return;
  }

  // ctor:
  display_2d::display_2d(TGCompositeFrame* main_, io::event_server* server_, const bool switch_mode)
      : _server_(server_), _2d_drawer_(0), _2d_viewer_(0) {
    const int width = main_->GetWidth();
    const int height = main_->GetHeight();

#ifdef SNVISUALIZATION_USE_OPENGL
    // One can compile snvsiualization using ROOT implementation
    // of OpenGL but then decide not to use it.
    if (style_manager::get_instance().use_opengl()) {
      _2d_viewer_ =
          new opengl_embedded_viewer("2DPlot", main_, width, height, i_embedded_viewer::VIEW_2D);
    } else
#endif

      _2d_viewer_ =
          new pad_embedded_viewer("2DPlot", main_, width, height, i_embedded_viewer::VIEW_2D);

    main_->AddFrame(_2d_viewer_->get_frame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    if (switch_mode) {
      // 2D different views
      TGHButtonGroup* views = new TGHButtonGroup(main_);

      // 2012-06-08 XG: Reminder (see Trac/Ticket #10)
      // front_view == YZ view
      // side_view  == XZ view
      // top view   == XY view
      const unsigned int n_view = 3;
      const std::string view_str[n_view] = {" top view ", " side view ", " front view "};

      for (unsigned int i_view = 0; i_view < n_view; ++i_view) {
        const view_type view_id = static_cast<view_type>(i_view);

        TGRadioButton* view = new TGRadioButton(views, view_str[i_view].c_str(), view_id);

        if (view_id == _2d_viewer_->get_view_type())
          view->SetState(kButtonDown);
        else
          view->SetState(kButtonUp);

        view->Connect("Clicked()", "snemo::visualization::view::display_2d", this,
                      "process_option_buttons()");
      }

      main_->AddFrame(views, new TGLayoutHints(kLHintsCenterX));
    }

    return;
  }

  // dtor:
  display_2d::~display_2d() {
    this->clear();

    if (_2d_viewer_) {
      delete _2d_viewer_;
      _2d_viewer_ = 0;
    }
    return;
  }

  void display_2d::set_drawer(i_draw_manager* draw_manager_) {
    _2d_drawer_ = draw_manager_;
    return;
  }

  void display_2d::clear() {
    _2d_viewer_->clear();
    return;
  }

  void display_2d::reset() {
    _2d_viewer_->reset();
    return;
  }

  void display_2d::update_detector() {
    _2d_viewer_->update_detector();
    return;
  }

  void display_2d::update_scene() {
    _2d_viewer_->update_scene(_2d_drawer_);
    return;
  }

  void display_2d::process_option_buttons() {
    TGButton* button = (TGButton*)gTQSender;
    const view_type view_id = static_cast<view_type>(button->WidgetId());

    set_view_type(view_id);
    return;
  }

  void display_2d::handle_button_signals(const button_signals_type signal_) {
    TCanvas* canvas = (TCanvas*)_2d_viewer_->get_canvas();

    switch (signal_) {
      case PRINT_2D_AS_EPS: {
        std::ostringstream filename;
        filename << style_manager::get_instance().get_save_prefix()
                 << _server_->get_current_event_number();
        switch (_2d_viewer_->get_view_type()) {
          case TOP_VIEW:
            filename << "_top";
            break;
          case SIDE_VIEW:
            filename << "_side";
            break;
          case FRONT_VIEW:
            filename << "_front";
            break;
          default:
            break;
        }
        filename << ".eps";

        if (!utils::root_utilities::save_view_as(canvas, filename.str())) {
          DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                       "Can not save 2D view!");
        }
      } break;

      case PRINT_2D: {
        if (!utils::root_utilities::save_view_as(canvas)) {
          DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                       "Can not save 2D view!");
        }
        break;
      }
      default:
        DT_LOG_ERROR(view::options_manager::get_instance().get_logging_priority(),
                     "Unknown id button");
        break;
    }
  }

  }  // end of namespace view

  }  // end of namespace visualization

}  // end of namespace snemo

// end of display_2d.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
