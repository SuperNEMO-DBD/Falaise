/* display_3d.cc
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

#include <falaise/snemo/view/display_3d.h>
#include <falaise/snemo/view/pad_embedded_viewer.h>
#include <falaise/snemo/view/style_manager.h>

#include <falaise/snemo/event_browser_config.h>
#ifdef EVENTBROWSER_USE_OPENGL
#include <falaise/snemo/view/opengl_embedded_viewer.h>
#endif

#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/io/event_server.h>
#include <falaise/snemo/utils/root_utilities.h>

#include <TCanvas.h>
#include <TGComboBox.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGeoManager.h>
#include <TObjArray.h>
#include <TView.h>

#include <iostream>
#include <stdexcept>

// Need trailing ; to satisfy clang-format, but leads to -pedantic error, ignore
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
ClassImp(snemo::visualization::view::display_3d);
#pragma GCC diagnostic pop

namespace snemo {
namespace visualization {

namespace view {

// ctor:
display_3d::display_3d(TGCompositeFrame *main_, io::event_server *server_)
    : _server_(server_), _3d_drawer_(0), _3d_viewer_(0) {
  const int width = main_->GetWidth();
  const int height = main_->GetHeight();

#ifdef SNVISUALIZATION_USE_OPENGL
  // One can compile snvsiualization using ROOT implementation
  // of OpenGL but then decide not to use it.
  if (style_manager::get_instance().use_opengl()) {
    _3d_viewer_ =
        new opengl_embedded_viewer("3DPlot", main_, width, height, i_embedded_viewer::VIEW_3D);
  } else
#endif
    _3d_viewer_ =
        new pad_embedded_viewer("3DPlot", main_, width, height, i_embedded_viewer::VIEW_3D);

  main_->AddFrame(_3d_viewer_->get_frame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  // TGHorizontalFrame* hzoom = new TGHorizontalFrame (main_,
  //                                                   static_cast<int> (0.99*width),
  //                                                   static_cast<int> (0.38*height));

  // main_->AddFrame (hzoom, new TGLayoutHints (kLHintsExpandX, 3, 3, 3, 3));

  // hzoom->AddFrame (new TGLabel (hzoom, "Zoom to: "),
  //                  new TGLayoutHints (kLHintsCenterY, 1, 1, 1, 1));

  // TGComboBox* area_list = new TGComboBox (hzoom, AREA_ZOOM_LIST);
  // hzoom->AddFrame (area_list,
  //                  new TGLayoutHints (kLHintsCenterY|kLHintsLeft, 1, 1, 1, 1));
  // area_list->Resize (static_cast<int> (0.2*width), 20);
  // detList->Associate(theBrowser);

  return;
}

// dtor:
display_3d::~display_3d() {
  this->clear();

  delete _3d_viewer_;
  return;
}

void display_3d::set_drawer(i_draw_manager *draw_manager_) {
  _3d_drawer_ = draw_manager_;
  return;
}

void display_3d::clear() {
  _3d_viewer_->clear();
  return;
}

void display_3d::reset() {
  _3d_viewer_->reset();
  return;
}

void display_3d::update_detector() {
  _3d_viewer_->update_detector();
  return;
}

void display_3d::update_scene() {
  _3d_viewer_->update_scene(_3d_drawer_);
  return;
}

void display_3d::handle_button_signals(const button_signals_type signal_) {
  TCanvas *canvas = (TCanvas *)_3d_viewer_->get_canvas();

  switch (signal_) {
    case VIEW_X3D:
      ((TPad *)(canvas->GetPad(0)))->GetViewer3D("x3d");
      break;

    case VIEW_OGL:
      ((TPad *)(canvas->GetPad(0)))->GetViewer3D("ogl");
      break;

    case PRINT_3D_AS_EPS: {
      std::ostringstream filename;
      filename << style_manager::get_instance().get_save_prefix()
               << _server_->get_current_event_number() << ".eps";

      if (!utils::root_utilities::save_view_as(canvas, filename.str())) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Can't save 3D view!");
      }
    } break;

    case PRINT_3D: {
      if (!utils::root_utilities::save_view_as(canvas)) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Can't save 3D view!");
      }
      break;
    }
    default:
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unknown id button");
      break;
  }
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of display_3d.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
