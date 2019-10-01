/* opengl_embedded_viewer.cc
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

#include <EventBrowser/view/i_draw_manager.h>
#include <EventBrowser/view/opengl_embedded_viewer.h>
#include <EventBrowser/view/opengl_event_handler.h>
#include <EventBrowser/view/opengl_scene.h>

#include <EventBrowser/detector/detector_manager.h>

#include <TCanvas.h>
#include <TGFrame.h>
#include <TGLScenePad.h>
#include <TGeoManager.h>
#include <THashList.h>
#include <TMath.h>

namespace snemo {

namespace visualization {

namespace view {

// ctor:
opengl_embedded_viewer::opengl_embedded_viewer(const std::string& name_, const TGFrame* frame_,
                                               const unsigned int width_,
                                               const unsigned int height_,
                                               const view_dim_type view_dim_)
    : i_embedded_viewer(view_dim_),
      TGLEmbeddedViewer(frame_, 0, 0, 0),
      _opengl_global_scene_(0),
      _opengl_event_scene_(0),
      _opengl_event_handler_(0) {
  this->_set_default_();
  this->_set_cameras_();

  // Instantiate OpenGL scene:
  // - global scene corresponds to the geometry part i.e. detector
  // - event scene is related to event display, so
  //   cleaning is done every new event
  _opengl_global_scene_ = new opengl_scene();
  _opengl_event_scene_ = new opengl_scene();

  this->AddScene(_opengl_global_scene_->get_scene_pad());
  this->AddScene(_opengl_event_scene_->get_scene_pad());

  // Set event handler
  _opengl_event_handler_ = new opengl_event_handler(this);
  this->SetEventHandler(_opengl_event_handler_);
  return;
}

// dtor:
opengl_embedded_viewer::~opengl_embedded_viewer() {
  this->clear();

  delete _opengl_event_scene_;
  delete _opengl_global_scene_;
  delete _opengl_event_handler_;

  return;
}

void opengl_embedded_viewer::clear() {
  this->RemoveAllScenes();
  return;
}

void opengl_embedded_viewer::reset() {
  // this->ResetCurrentCamera ();

  if (_view_dim_type == VIEW_3D) {
    this->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
    return;
  }

  switch (_view_type_) {
    case TOP_VIEW:
      this->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
      break;
    case FRONT_VIEW: {
      this->SetOrthoCamera(TGLViewer::kCameraOrthoXOZ, 0.78, 0, 0, 0.0, 0.0);
      this->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
    } break;
    case SIDE_VIEW: {
      this->SetOrthoCamera(TGLViewer::kCameraOrthoXOZ, 0.58, 0, 0, 0.0, TMath::Pi() / 2.);
      this->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
    } break;
    default:
      break;
  }

  return;
}

void opengl_embedded_viewer::set_view_type(const view_type view_type_) {
  _view_type_ = view_type_;
  this->reset();
  return;
}

view_type opengl_embedded_viewer::get_view_type() const { return _view_type_; }

void opengl_embedded_viewer::update_detector() {
  detector::detector_manager& detector_mgr = detector::detector_manager::get_instance();
  TGeoVolume* world_volume = detector_mgr.get_world_volume();

  _opengl_global_scene_->clean();
  _opengl_global_scene_->add(world_volume);

  return;
}

void opengl_embedded_viewer::update_scene(i_draw_manager* drawer_) {
  _opengl_event_scene_->clean();
  _opengl_event_scene_->add(drawer_->get_objects());

  // // Cancel any pending redraw timer.
  // fRedrawTimer->Stop();

  // _opengl_global_scene_->get_scene_pad ()->PadPaintFromViewer (this);
  // _opengl_event_scene_->get_scene_pad ()->PadPaintFromViewer (this);

  // MergeSceneBBoxes (fOverallBoundingBox);
  // fReferencePos.Set(fOverallBoundingBox.Center());

  // if (_view_dim_type == VIEW_3D)
  //   {
  //     fPerspectiveCameraXOY.Setup(fOverallBoundingBox, true);
  //     this->SetPerspectiveCamera (TGLViewer::kCameraPerspXOY, 30, 0, 0,
  //                                 -TMath::Pi ()/4., -TMath::Pi ()/4.);
  //     this->SetCurrentCamera (TGLViewer::kCameraPerspXOY);
  //   }

  // //        this->reset ();
  // //        PostSceneBuildSetup(true);

  // // if (redraw)
  // RequestDraw(// TGLRnrCtx::kLODHigh
  //             );

  this->UpdateScene(/*true*/);
  this->PostSceneBuildSetup(true);
  // this->reset ();
  return;
}

TCanvas* opengl_embedded_viewer::get_canvas() { return 0; }

TGFrame* opengl_embedded_viewer::get_frame() { return this->GetFrame(); }

void opengl_embedded_viewer::_set_default_() {
  _view_type_ = TOP_VIEW;

  //        this->SetLOD (TGLRnrCtx::kLODHigh);
  this->SetResetCamerasOnUpdate(false);
  this->SetStyle(TGLRnrCtx::kWireFrame);

  this->SetLineScale(1.0);

  this->UseLightColorSet();
  this->ColorSet().Background().SetColor(kBlack);

  // Reset by ResetCurrentCameras
  // this->SetPerspectiveCamera (TGLViewer::kCameraPerspXOY, 30, 0, 0,
  //                             -TMath::Pi ()/4., -TMath::Pi ()/4.);
  // this->SetOrthoCamera(TGLViewer::kCameraOrthoZOY, 0.78, 0,
  //                      0, TMath::Pi ()/2., TMath::Pi ()/2.);

  // double ref_position[] = {0.0, 0.0, 0.0};
  // this->SetGuideState (TGLUtil::kAxesOrigin, false, false, ref_position);
  // this->SetPerspectiveCamera (TGLViewer::kCameraPerspXOY, 30, 0, 0,
  //                             -TMath::Pi ()/4., -TMath::Pi ()/4.);
}

void opengl_embedded_viewer::_set_cameras_() {
  const double zoom = 0.78;
  const double dolly = 0.0;
  double* center = 0;
  this->SetOrthoCamera(TGLViewer::kCameraOrthoXOZ, zoom, dolly, center, 0.0, TMath::Pi() / 2.);
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of opengl_embedded_viewer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
