// -*- mode: c++ ; -*-
/* opengl_embedded_viewer.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-12-12
 * Last modified: 2011-03-23
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
 *   OpenGL embedded view
 *   The OpenGL "scene" is built using a pad
 *   which is linked to embedded viewer just like
 *   it is for TRootEmbeddedCanvas class.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_EMBEDDED_VIEWER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_EMBEDDED_VIEWER_H 1

#include <string>

#include <falaise/snemo/view/i_embedded_viewer.h>

#include <TGLEmbeddedViewer.h>

class TGFrame;

namespace snemo {

namespace visualization {

namespace view {

class opengl_scene;
class opengl_event_handler;
class i_draw_manager;

class opengl_embedded_viewer : public i_embedded_viewer, public TGLEmbeddedViewer {
 public:
  opengl_embedded_viewer(const std::string &name_, const TGFrame *frame_,
                         const unsigned int width_ = 0, const unsigned int height_ = 0,
                         const view_dim_type view_dim_ = VIEW_3D);
  virtual ~opengl_embedded_viewer();

  virtual void clear();
  virtual void reset();

  virtual void set_view_type(const view_type view_type_);
  virtual view_type get_view_type() const;

  virtual void update_detector();
  virtual void update_scene(i_draw_manager *drawer_);

  virtual TCanvas *get_canvas();
  virtual TGFrame *get_frame();

 private:
  void _set_default_();
  void _set_cameras_();

 private:
  opengl_scene *_opengl_global_scene_;
  opengl_scene *_opengl_event_scene_;
  opengl_event_handler *_opengl_event_handler_;

  view_type _view_type_;

  opengl_embedded_viewer();                                           // not implemented
  opengl_embedded_viewer(const opengl_embedded_viewer &);             // not implemented
  opengl_embedded_viewer &operator=(const opengl_embedded_viewer &);  // not implemented
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_OPENGL_EMBEDDED_VIEWER_H

// end of opengl_embedded_viewer.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
