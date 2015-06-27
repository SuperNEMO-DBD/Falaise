/* pad_embedded_viewer.cc
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

#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>

#include <falaise/snemo/view/pad_embedded_viewer.h>
#include <falaise/snemo/view/style_manager.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/i_draw_manager.h>

#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/utils/root_utilities.h>

#include <datatools/exception.h>

#include <TCanvas.h>
#include <TGeoVolume.h>
#include <TView.h>
#include <TObjArray.h>

#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>
#include <TMarker3DBox.h>

namespace snemo {

  namespace visualization {

    namespace view {

      // ctor:
      pad_embedded_viewer::pad_embedded_viewer(const std::string & name_,
                                               const TGFrame * frame_,
                                               const unsigned int width_,
                                               const unsigned int height_,
                                               const view_dim_type view_dim_) :
        i_embedded_viewer(view_dim_),
        TRootEmbeddedCanvas(name_.c_str(), frame_, width_, height_)
      {
        _objects_      = 0;
        _text_objects_ = 0;

        const int vtype = style_manager::get_instance().get_startup_2d_view();
        _view_type_ = static_cast<view_type>(vtype);

        _zoom_factor_ = 2.0;
        _zoom_index_  = 0;

        _search_for_boundaries_done_ = false;
        _min_roi_bound_.SetXYZ(+std::numeric_limits<double>::infinity(),
                               +std::numeric_limits<double>::infinity(),
                               +std::numeric_limits<double>::infinity());
        _max_roi_bound_.SetXYZ(-std::numeric_limits<double>::infinity(),
                               -std::numeric_limits<double>::infinity(),
                               -std::numeric_limits<double>::infinity());

        return;
      }

      // dtor:
      pad_embedded_viewer::~pad_embedded_viewer()
      {
        return;
      }

      void pad_embedded_viewer::clear()
      {
        this->get_canvas()->SetEditable(true);
        this->get_canvas()->Clear();
        return;
      }

      void pad_embedded_viewer::reset()
      {
        _zoom_index_ = 0;

        // For 2D view, shapes are deformed by ROOT: cubes projection
        // does not give a square! ROOT seems to optimize the size of
        // objects in a canvas in order to get them as big as possible
        // Here we calculate the scale factor given the dimensions of
        // the detector and the size of the embedded canvas
        if (_view_dim_type == VIEW_2D)
          this->_set_scale_factors_();

        TCanvas * canvas = this->get_canvas();

        switch (_view_dim_type) {
        case VIEW_2D:
          {
            canvas->GetView()->SetParallel();

            if (_view_type_ == TOP_VIEW)   canvas->GetView()->TopView();
            if (_view_type_ == FRONT_VIEW) canvas->GetView()->Front();
            if (_view_type_ == SIDE_VIEW)  canvas->GetView()->RotateView(180.0, 90.0);
            // TView::Side() shows the other side
            if (options_manager::get_instance().get_option_flag(FOCUS_ROI) && _objects_) {
              optimize_range(_get_minimal_roi_bound_(),
                             _get_maximal_roi_bound_());
              _search_for_boundaries_done_ = false;
            } else {
              const double scale_factor = _scale_factors_.at(_view_type_);
              const double fudge_factor = _fudge_factors_.at(_view_type_);

              canvas->Range(-scale_factor/fudge_factor, -1./fudge_factor,
                            scale_factor/fudge_factor, 1./fudge_factor);
            }
          }
          break;
        case VIEW_3D:
          {
            canvas->GetView()->SetPerspective();
            canvas->Range(-1, -1, 1, 1);
          }
          break;
        default:
          break;
        }

        canvas->SetEditable(false);
        canvas->Resize();
        this->_scale_text_();
        canvas->Modified();
        canvas->Update();

        return;
      }

      void pad_embedded_viewer::set_view_type(const view_type view_type_)
      {
        _view_type_ = view_type_;
        this->reset();
        return;
      }

      view_type pad_embedded_viewer::get_view_type() const
      {
        return _view_type_;
      }

      void pad_embedded_viewer::update_detector()
      {
        this->clear();

        TCanvas * canvas = this->get_canvas();

        canvas->cd();
        canvas->SetEditable(true);
        canvas->SetFillColor(style_manager::get_instance().get_background_color());

        detector::detector_manager::get_instance().draw();

        this->reset();

        canvas->SetEditable(false);
        canvas->Modified();
        canvas->Update();

        return;
      }

      void pad_embedded_viewer::update_scene(i_draw_manager * drawer_)
      {
        TCanvas * canvas = this->get_canvas();

        canvas->cd();
        canvas->SetEditable(true);
        canvas->SetFillColor(style_manager::get_instance().get_background_color());

        if (drawer_) {
          // if (_view_dim_type == VIEW_3D)
          //   {
          //     drawer_->draw_legend();
          //   }

          // Special use of font for TPad: since text size is static,
          // one has to rescale the text size with respect to zoom
          // factor used. This is done by update_font method.
          if (_view_dim_type == VIEW_2D) {
            _text_objects_ = drawer_->get_text_objects();
            this->_scale_text_();
            drawer_->draw_text();

            if (options_manager::get_instance().get_option_flag(FOCUS_ROI) &&
                !_search_for_boundaries_done_) {
              // Retrieve pointer to saved objects to look for the
              // boundaries of the ROI inside this drawn objects
              _objects_ = drawer_->get_objects();

              _min_roi_bound_.SetXYZ(+std::numeric_limits<double>::infinity(),
                                     +std::numeric_limits<double>::infinity(),
                                     +std::numeric_limits<double>::infinity());
              _max_roi_bound_.SetXYZ(-std::numeric_limits<double>::infinity(),
                                     -std::numeric_limits<double>::infinity(),
                                     -std::numeric_limits<double>::infinity());
              search_for_boundaries();
              optimize_range(_get_minimal_roi_bound_(),
                             _get_maximal_roi_bound_());
            }
          }

          // Draw every objects
          drawer_->draw();
        }

        canvas->SetEditable(false);
        canvas->Modified();
        canvas->Update();
        return;
      }

      TCanvas * pad_embedded_viewer::get_canvas()
      {
        TCanvas * canvas = this->GetCanvas();
        DT_THROW_IF(!canvas, std::logic_error, "No canvas has be instantiated!");
        return canvas;
      }

      TGFrame * pad_embedded_viewer::get_frame()
      {
        return this;
      }

      void pad_embedded_viewer::search_for_boundaries()
      {
        if (!_objects_) return;

        TVector3 & min_roi_bound = _grab_minimal_roi_bound_();
        TVector3 & max_roi_bound = _grab_maximal_roi_bound_();

        TObjArrayIter iter(_objects_);
        while (iter.Next()) {
          const TObject * a_object = *iter;

          if (a_object->IsA() == TPolyLine3D::Class()) {
            const TPolyLine3D * pl3d = dynamic_cast<const TPolyLine3D *>(a_object);

            for (int i = 0; i < pl3d->Size(); ++i) {
              const double x = pl3d->GetP()[3*i];
              const double y = pl3d->GetP()[3*i+1];
              const double z = pl3d->GetP()[3*i+2];

              min_roi_bound.SetXYZ(std::min(min_roi_bound.x(), x),
                                   std::min(min_roi_bound.y(), y),
                                   std::min(min_roi_bound.z(), z));

              max_roi_bound.SetXYZ(std::max(max_roi_bound.x(), x),
                                   std::max(max_roi_bound.y(), y),
                                   std::max(max_roi_bound.z(), z));
            }
          }

          if (a_object->IsA() == TPolyMarker3D::Class()) {
            const TPolyMarker3D * pm3d = dynamic_cast<const TPolyMarker3D *>(a_object);

            for (int i = 0; i < pm3d->Size(); ++i) {
              const double x = pm3d->GetP()[3*i];
              const double y = pm3d->GetP()[3*i+1];
              const double z = pm3d->GetP()[3*i+2];

              min_roi_bound.SetXYZ(std::min(min_roi_bound.x(), x),
                                   std::min(min_roi_bound.y(), y),
                                   std::min(min_roi_bound.z(), z));

              max_roi_bound.SetXYZ(std::max(max_roi_bound.x(), x),
                                   std::max(max_roi_bound.y(), y),
                                   std::max(max_roi_bound.z(), z));
            }
          }

          if (a_object->IsA() == TMarker3DBox::Class()) {
            const TMarker3DBox * m3d = dynamic_cast<const TMarker3DBox *>(a_object);
            float x, y, z;
            m3d->GetPosition(x, y, z);
            float dx, dy, dz;
            m3d->GetSize(dx, dy, dz);

            min_roi_bound.SetXYZ(std::min(min_roi_bound.x(), (double)x - dx),
                                 std::min(min_roi_bound.y(), (double)y - dy),
                                 std::min(min_roi_bound.z(), (double)z - dz));

            max_roi_bound.SetXYZ(std::max(max_roi_bound.x(), (double)x + dx),
                                 std::max(max_roi_bound.y(), (double)y + dy),
                                 std::max(max_roi_bound.z(), (double)z + dz));
          }
        }

        _search_for_boundaries_done_ = true;
        return;
      }

      void pad_embedded_viewer::optimize_range(const TVector3 & min_bound_,
                                               const TVector3 & max_bound_)
      {
        datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
        DT_LOG_TRACE(local_priority, "Entering...");
        DT_LOG_TRACE(local_priority, "Min bound (x,y,z)=("
                     << min_bound_.x() << ","
                     << min_bound_.y() << ","
                     << min_bound_.z() << ")");
        DT_LOG_TRACE(local_priority, "Max bound (x,y,z)=("
                     << max_bound_.x() << ","
                     << max_bound_.y() << ","
                     << max_bound_.z() << ")");

        // Adapt pad range to show the most essential part of the
        // event
        double xmin, xmax, ymin, ymax;
        _convert_world_to_pad_coordinates_(min_bound_.x(),
                                           min_bound_.y(),
                                           min_bound_.z(),
                                           xmin, ymin);
        _convert_world_to_pad_coordinates_(max_bound_.x(),
                                           max_bound_.y(),
                                           max_bound_.z(),
                                           xmax, ymax);

        TCanvas * canvas = this->get_canvas();

        switch (_view_dim_type) {
        case VIEW_2D:
          {
            const double dw =
              static_cast<double>(this->GetWidth()) /
              static_cast<double>(this->GetHeight());

            const double sf = _scale_factors_.at(_view_type_) / dw;

            double new_xmax = std::max(xmax, xmin) / sf;
            double new_ymax = std::max(ymax, ymin);
            double new_xmin = std::min(xmax, xmin) / sf;
            double new_ymin = std::min(ymax, ymin);

            xmax = new_xmax;
            xmin = new_xmin;
            ymax = new_ymax;
            ymin = new_ymin;

            const double dwpad = std::fabs(ymax - ymin) / std::fabs(xmax - xmin);
            if (dwpad*dw > 1.0) {
              xmin = new_xmin - (std::fabs(ymin - ymax) * dw - std::fabs(new_xmax - new_xmin))/2.0;
              xmax = new_xmax + (std::fabs(ymin - ymax) * dw - std::fabs(new_xmax - new_xmin))/2.0;
            } else {
              ymin = new_ymin - (std::fabs(xmin - xmax) / dw  - std::fabs(new_ymax - new_ymin))/2.0;
              ymax = new_ymax + (std::fabs(xmin - xmax) / dw  - std::fabs(new_ymax - new_ymin))/2.0;
            }

            xmin *= sf;
            xmax *= sf;

            // Sanity check
            if ((xmin >= xmax) || (ymin >= ymax)) return;

            //              canvas->Range(xmin, ymin, xmax, ymax);

            // Unzoom one time to be a bit larger
            const double xcenter = (xmax + xmin) / 2.0;
            const double ycenter = (ymax + ymin) / 2.0;

            double zoom_factor = 0.0;
            if (!_text_objects_->GetEntriesFast()) {
              zoom_factor = 2.0;
            } else {
              if (_view_type_ == TOP_VIEW)        zoom_factor = 3.0;
              else if (_view_type_ == FRONT_VIEW) zoom_factor = 4.0;
              else if (_view_type_ == SIDE_VIEW)  zoom_factor = 2.0;
            }

            const double xdown = xcenter - zoom_factor * std::fabs(xcenter - xmin);
            const double ydown = ycenter - zoom_factor * std::fabs(ycenter - ymin);
            const double xup   = xcenter + zoom_factor * std::fabs(xcenter - xmax);
            const double yup   = ycenter + zoom_factor * std::fabs(ycenter - ymax);

            canvas->Range(xdown, ydown, xup, yup);
          }
        default:
          break;
        }

        canvas->Resize();
        this->_scale_text_();
        canvas->Modified();
        canvas->Update();

        DT_LOG_TRACE(local_priority, "Exiting.");
        return;
      }

      const TVector3 & pad_embedded_viewer::_get_minimal_roi_bound_() const
      {
        return _min_roi_bound_;
      }

      const TVector3 & pad_embedded_viewer::_get_maximal_roi_bound_() const
      {
        return _max_roi_bound_;
      }

      TVector3 & pad_embedded_viewer::_grab_minimal_roi_bound_()
      {
        return _min_roi_bound_;
      }

      TVector3 & pad_embedded_viewer::_grab_maximal_roi_bound_()
      {
        return _max_roi_bound_;
      }

      void pad_embedded_viewer::_convert_pixel_to_pad_coordinates_(const int xpixel_,
                                                                   const int ypixel_,
                                                                   double & xpad_,
                                                                   double & ypad_)
      {
        // TCanvas class provides a PixeltoXY method which seems to
        // work fine (_convert_pixel_to_world_coordinates_ method uses
        // it without apparent problems) except when (un)zooming is
        // performed after a window resize. Here we build our own
        // conversion method taking into account frame size
        TCanvas* canvas = this->get_canvas();

        double xdown, ydown, xup, yup;
        canvas->GetRange(xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        const double dx = xup - xdown;
        const double dy = yup - ydown;

        const double width  = this->GetWidth();
        const double height = this->GetHeight();

        xpad_ = xpixel_ * dx / width + xdown;
        ypad_ = (height-ypixel_) * dy / height + ydown;

        return;
      }

      void pad_embedded_viewer::_convert_pixel_to_world_coordinates_(const int xpixel_,
                                                                     const int ypixel_,
                                                                     double & xworld_,
                                                                     double & yworld_)
      {
        if (_view_dim_type == VIEW_3D) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "This method only makes sense in 2D view!");
          return;
        }

        TCanvas* canvas = this->get_canvas();

        const double xpad = canvas->PixeltoX(xpixel_);
        const double ypad = canvas->PixeltoY(ypixel_);

        const double pn[3] = {xpad, ypad, 0.0};
        double pw[3];
        canvas->GetView()->NDCtoWC(pn, pw);

        if (_view_type_ == TOP_VIEW) {
          xworld_ = pw[0];
          yworld_ = pw[1];
        } else if (_view_type_ == FRONT_VIEW) {
          xworld_ = pw[0];
          yworld_ = pw[2];
        } else if (_view_type_ == SIDE_VIEW) {
          xworld_ = pw[1];
          yworld_ = pw[2];
        }

        return;
      }

      void pad_embedded_viewer::_convert_world_to_pad_coordinates_(const double xworld_,
                                                                   const double yworld_,
                                                                   const double zworld,
                                                                   double & xpad_,
                                                                   double & ypad_)
      {
        if (_view_dim_type == VIEW_3D) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "This method only makes sense in 2D view!");
          return;
        }

        TCanvas* canvas = this->get_canvas();

        const double pw[3] = {xworld_, yworld_, zworld};
        double pn[3];
        canvas->GetView()->WCtoNDC(pw, pn);

        xpad_ = pn[0];
        ypad_ = pn[1];

        return;
      }

      void pad_embedded_viewer::_set_scale_factors_()
      {
        // Get scale factor for each view : use to rescale 2D View
        // first scale wrt to the whole detector size
        const double *size_view = this->get_canvas()->GetView()->GetRmin();

        _scale_factors_[TOP_VIEW]   = size_view[1]/size_view[0]; //width/length;
        _scale_factors_[FRONT_VIEW] = size_view[2]/size_view[0]; //height/length;
        _scale_factors_[SIDE_VIEW]  = size_view[2]/size_view[1]; //height/width;

        const double dw =
          static_cast<double>(this->GetWidth ()) /
          static_cast<double>(this->GetHeight());

        // then, scale wrt to the frame size
        _scale_factors_[TOP_VIEW]   *= dw;
        _scale_factors_[FRONT_VIEW] *= dw;
        _scale_factors_[SIDE_VIEW]  *= dw;

        const detector::detector_manager & detector_mgr
          = detector::detector_manager::get_instance();

        if (detector_mgr.get_setup_label() == detector::detector_manager::SNEMO ||
            detector_mgr.get_setup_label() == detector::detector_manager::SNEMO_DEMONSTRATOR) {
          // fudge factors for SuperNEMO detector
          _fudge_factors_[TOP_VIEW]   = 1.6;
          _fudge_factors_[FRONT_VIEW] = 1.6;
          _fudge_factors_[SIDE_VIEW]  = 0.83;
        } else {
          _fudge_factors_[TOP_VIEW]   = 1.0;
          _fudge_factors_[FRONT_VIEW] = 1.0;
          _fudge_factors_[SIDE_VIEW]  = 1.0;
        }
        return;
      }

      void pad_embedded_viewer::_scale_text_()
      {
        datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
        if (!_text_objects_) return;

        DT_LOG_TRACE(local_priority, "text objects address " << _text_objects_);

        TIter iter(_text_objects_);
        utils::root_utilities::TLatex3D * itext;
        while ((itext = (utils::root_utilities::TLatex3D *)iter.Next())) {
          DT_LOG_TRACE(local_priority, "text address " << itext
                       << "world position (x, y, z) = ("
                       << itext->GetX() << ", "
                       << itext->GetY() << ", "
                       << itext->GetZ() << ")");

          // If same geom_id has text (FindObject compare geom_id
          // coordinates) then TLatex "splitline" function is used
          // to show both information
          utils::root_utilities::TLatex3D * found
            = (utils::root_utilities::TLatex3D*) _text_objects_->FindObject(itext);

          if (found) {
            std::string str1 = found->GetText();
            std::string str2 = itext->GetText();
            std::ostringstream oss;
            oss << "#splitline"
                << "{" << str1 << "}"
                << "{" << str2 << "}";

            // Something here is not clear for me
            itext = found;
            _text_objects_->Remove(itext);

            // Update TLatex text
            itext->SetText(oss.str());
          }

          const double xworld = itext->GetX();
          const double yworld = itext->GetY();
          const double zworld = itext->GetZ();
          double xpad, ypad;
          this->_convert_world_to_pad_coordinates_(xworld, yworld, zworld,
                                                   xpad, ypad);

          DT_LOG_TRACE(local_priority, "pad coordinates (x, y) = ("
                       << xpad << ", " << ypad << ")");

          itext->SetPadCoordinates(xpad, ypad);

          // By default, centered text horizontally and vertically
          itext->SetTextAlign(22);

          // Use Helvetica font i.e. 4 with the higher precision
          // available i.e. 3 : the text size is then not
          // proportionnal to pad size but expresses in pixel value
          itext->SetTextFont(43);

          // Slightly increase text size value wrt zoom value
          double pixel_size = 13;
          _zoom_index_ >= 0 ?
            pixel_size *= (1.0 + _zoom_index_/10.0) :
            pixel_size *= 0.0;
          itext->SetTextSize(pixel_size);

          const detector::detector_manager & detector_mgr
            = detector::detector_manager::get_instance();
          if (detector_mgr.get_setup_label() == detector::detector_manager::SNEMO ||
              detector_mgr.get_setup_label() == detector::detector_manager::SNEMO_DEMONSTRATOR) {
            // Hard coded but specific code to SuperNEMO
            if (_view_type_ != SIDE_VIEW && _zoom_index_ <= 2) {
              const double xshift = 0.15;
              if (xpad > 0.0) {
                itext->SetTextAlign(12);
                itext->SetPadCoordinates(xpad+xshift, ypad);
              } else {
                itext->SetTextAlign(32);
                itext->SetPadCoordinates(xpad-xshift, ypad);
              }
            }
          } else if (detector_mgr.get_setup_label() == detector::detector_manager::BIPO1 ||
                     detector_mgr.get_setup_label() == detector::detector_manager::BIPO3) {
            if (_zoom_index_ <= 2) {
              const double yshift = 0.10;
              if (_view_type_ == TOP_VIEW) {
                if (zworld > 0.0) {
                  itext->SetPadCoordinates(xpad, ypad+yshift);
                } else {
                  itext->SetPadCoordinates(xpad, ypad-yshift);
                }
              } else {
                if (ypad > 0.0) {
                  itext->SetPadCoordinates(xpad, ypad+yshift);
                } else {
                  itext->SetPadCoordinates(xpad, ypad-yshift);
                }
              }
            }
          }
        }
        return;
      }

      bool pad_embedded_viewer::_handle_container_key(Event_t* event_)
      {
        char tmp[2];
        unsigned int keysym;

        gVirtualX->LookupString(event_, tmp, sizeof(tmp), keysym);

        if (event_->fType != kGKeyPress) return false;

        // Zoom factor must be between 1 and +inf !
        if (event_->fState & kKeyControlMask) {
          // Smooth zoom
          _zoom_factor_ = 1.1;
        } else {
          // Rough zoom
          _zoom_factor_ = 2.0;
        }

        EKeySym key_pressed = (EKeySym) keysym;

        // +/- key perform zoom from the center of the view which is
        // different to what mouse wheel do by zooming arround the
        // mouse cursor position
        if (event_->fState & kKeyShiftMask) {
          switch (key_pressed) {
          case kKey_Down:
          case kKey_Up:
          case kKey_Left:
          case kKey_Right:
            return this->_rotate(key_pressed);
            break;
            // french laptop
          case kKey_Plus:
            return this->_zoom(this->GetWidth()/2,
                               this->GetHeight()/2);
            break;
          default:
            break;
          }
        } else {
          switch (key_pressed) {
          case kKey_Plus:
            return this->_zoom(this->GetWidth()/2,
                               this->GetHeight()/2);
            break;
          case kKey_Minus:
            return this->_unzoom(this->GetWidth()/2,
                                 this->GetHeight()/2);
            break;
          case kKey_Down:
          case kKey_Up:
          case kKey_Left:
          case kKey_Right:
            return this->_move(key_pressed);
            break;
          default:
            break;
          }
        }

        return true;
      }

      bool pad_embedded_viewer::_handle_container_motion(Event_t * event_)
      {
        DT_LOG_TRACE(datatools::logger::PRIO_WARNING,
                     event_->fCode << " " << event_->fType << " "
                     << event_->fX << " " << event_->fY);

        const int x = event_->fX;
        const int y = event_->fY;

        switch (fButton) {
        case 0:
          {
            this->_handle_input(kMouseMotion, x, y);
            break;
          }
        case kButton1:
          {
            // Left click
            this->_handle_input(kButton1Motion, x, y);
            break;
          }
        case kButton2:
          {
            // Middle click
            this->_handle_input(kButton2Motion, x, y);
            break;
          }
          // case kButton3:
          //   {
          //     // Right click
          //     this->_handle_input(kButton3Motion, x, y);
          //     break;
          //   }
        }
        return true;
      }

      bool pad_embedded_viewer::_handle_container_button(Event_t* event_)
      {
        DT_LOG_TRACE(datatools::logger::PRIO_WARNING,
                     event_->fCode << " " << event_->fType << " "
                     << event_->fX << " " << event_->fY);

        const int button = event_->fCode;
        const int type   = event_->fType;
        const int x = event_->fX;
        const int y = event_->fY;

        // Handle mouse-wheel events first.
        if (button > kButton3)
          return this->_handle_mouse_wheel(event_);

        // kButton1 : left   click
        // kButton2 : middle click
        // kButton3 : right  click
        if (type == kButtonPress) {
          fButton = button;
          if (button == kButton1) {
            // if (event->fState & kKeyShiftMask)
            //   canvas->HandleInput(EEventType(7), x, y);
            // else
            this->_handle_input(kButton1Down, x, y);
          } else if (button == kButton2) {
            this->_handle_input(kButton2Down, x, y);
          } else if (button == kButton3) {
            this->_handle_input(kButton3Down, x, y);
          }
        } else if (type == kButtonRelease) {
          if (button == kButton1) {
            this->_handle_input(kButton1Up, x, y);
          } else if (button == kButton2) {
            this->_handle_input(kButton2Up, x, y);
          } else if (button == kButton3) {
            this->_handle_input(kButton3Up, x, y);
          }

          // if (button == kButton4)
          //   fCanvas->HandleInput(EEventType(5), x, y);//hack
          // if (button == kButton5)
          //   fCanvas->HandleInput(EEventType(6), x, y);//hack

          fButton = 0;
        }

        // return true;

        return TRootEmbeddedCanvas::HandleContainerButton(event_);
      }

      bool pad_embedded_viewer::_handle_container_double_click(Event_t* event_)
      {
        const int button = event_->fCode;

        // Discard event from mouse wheel
        if (button > kButton3)
          return true;

        const int x = event_->fX;
        const int y = event_->fY;

        if (button == kButton1)
          return this->_zoom_to_position(x, y);
        if (button == kButton2)
          this->reset();

        return true;
      }

      bool pad_embedded_viewer::_handle_mouse_wheel(Event_t* event_)
      {
        const int x = event_->fX;
        const int y = event_->fY;

        // Zoom factor must be between 1 and +inf !
        if (event_->fState & kKeyControlMask) {
          // Smooth zoom
          _zoom_factor_ = 1.1;
        } else {
          // Rough zoom
          _zoom_factor_ = 2.0;
        }

        // From TGLEventHandler.cxx
        if (event_->fType == kButtonRelease) {
          switch (event_->fCode) {
          case kButton5: // Zoom out
            return this->_unzoom(x, y);
            break;

          case kButton4: // Zoom in
            return this->_zoom(x, y);
            break;

          }
        }
        return true;
      }

      bool pad_embedded_viewer::_zoom(const int x_, const int y_)
      {
        _zoom_index_++;

        TCanvas* canvas = this->get_canvas();

        double xdown, ydown, xup, yup;
        canvas->GetRange(xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        double x, y;
        this->_convert_pixel_to_pad_coordinates_(x_, y_, x, y);

        xdown = x - 1/_zoom_factor_ * std::fabs(x - xdown);
        ydown = y - 1/_zoom_factor_ * std::fabs(y - ydown);
        xup   = x + 1/_zoom_factor_ * std::fabs(x - xup);
        yup   = y + 1/_zoom_factor_ * std::fabs(y - yup);

        canvas->Range(xdown, ydown, xup, yup);
        canvas->Resize();
        this->_scale_text_();
        canvas->Modified();
        canvas->Update();

        return true;
      }

      bool pad_embedded_viewer::_unzoom(const int x_, const int y_)
      {
        _zoom_index_--;

        TCanvas* canvas = this->get_canvas();

        double xdown, ydown, xup, yup;
        canvas->GetRange(xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        double x, y;
        this->_convert_pixel_to_pad_coordinates_(x_, y_, x, y);

        xdown = x - _zoom_factor_ * std::fabs(x - xdown);
        ydown = y - _zoom_factor_ * std::fabs(y - ydown);
        xup   = x + _zoom_factor_ * std::fabs(x - xup);
        yup   = y + _zoom_factor_ * std::fabs(y - yup);

        canvas->Range(xdown, ydown, xup, yup);
        canvas->Resize();
        this->_scale_text_();
        canvas->Modified();
        canvas->Update();

        return true;
      }

      bool pad_embedded_viewer::_zoom_to_position(const int x_, const int y_)
      {
        // When zooming to position the value of zoom index can not be
        // retrieved since zoom is proportionnal to pad size. Then a
        // fixed value is likely enough. Need to be optimize...
        _zoom_index_ = 5;//static_cast<size_t> (10 / _zoom_factor_);

        TCanvas* canvas = this->get_canvas();

        double xdown, ydown, xup, yup;
        canvas->GetRange(xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        double x, y;
        this->_convert_pixel_to_pad_coordinates_(x_, y_, x, y);

        // Make a 'box' around the double-clicked point which size is
        // 10% the total size of canvas
        xdown = x - 0.1 * std::fabs(x - xdown);
        ydown = y - 0.1 * std::fabs(y - ydown);
        xup   = x + 0.1 * std::fabs(x - xup);
        yup   = y + 0.1 * std::fabs(y - yup);

        canvas->Range(xdown, ydown, xup, yup);
        canvas->Resize();
        this->_scale_text_();
        canvas->Modified();
        canvas->Update();

        return true;
      }

      bool pad_embedded_viewer::_move(const EKeySym key_pressed_)
      {
        TCanvas* canvas = this->get_canvas();

        double xdown, ydown, xup, yup;
        canvas->GetRange(xdown, ydown, xup, yup);

        // Adaptive zoom step
        const double deltaZoomX = fabs(xdown - xup) / 10.;
        const double deltaZoomY = fabs(ydown - yup) / 10.;
        if (fabs(xdown - xup) < deltaZoomX ||
            fabs(ydown - yup) < deltaZoomY)
          return false;

        switch (key_pressed_) {
        case kKey_Down:
          ydown -= deltaZoomY;
          yup   -= deltaZoomY;
          break;
        case kKey_Up:
          ydown += deltaZoomY;
          yup   += deltaZoomY;
          break;
        case kKey_Left:
          xdown -= deltaZoomX;
          xup   -= deltaZoomX;
          break;
        case kKey_Right:
          xdown += deltaZoomX;
          xup   += deltaZoomX;
          break;
        default:
          break;
        }

        canvas->Range(xdown, ydown, xup, yup);
        canvas->Resize();
        canvas->Modified();
        canvas->Update();

        return true;
      }

      bool pad_embedded_viewer::_rotate(const EKeySym key_pressed_)
      {
        TCanvas* canvas = this->get_canvas();

        if (_view_dim_type == VIEW_2D) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "Rotation can not be done in 2D view");
          return false;
        }

        TView* view = canvas->GetView();

        if (!view) return false;

        const double dangle = 10.; // degree

        double longitude = view->GetLongitude();
        double latitude  = view->GetLatitude();

        switch (key_pressed_)
          {
          case kKey_Down:
            latitude -= dangle;
            break;
          case kKey_Up:
            latitude += dangle;
            break;
          case kKey_Left:
            longitude += dangle;
            break;
          case kKey_Right:
            longitude -= dangle;
            break;
          default:
            break;
          }

        view->RotateView(longitude, latitude, 0);

        canvas->Modified();
        canvas->Update();

        return true;
      }


      bool pad_embedded_viewer::_handle_input(const int event_, const int x_, const int y_)
      {
        // Hack TView3D::ExecuteRotateView method to show
        // interactively the rotation of volumes without 'box' outline
        // Define also translation using Range method of TCanvas

        static double xrange, yrange, xmin, ymin;
        static double xdown, ydown, xup, yup;
        static double longitude1, latitude1, longitude2, latitude2;
        static double newlatitude, newlongitude, oldlatitude, oldlongitude;

        double dlatitude, dlongitude, x, y;
        double psideg;
        int irep;

        TCanvas* canvas = this->get_canvas();

        // All coordinates transformation are from absolute to relative
        canvas->AbsCoordinates(true);

        TView* view = (TView*)canvas->GetView();

        switch (event_) {

        case kMouseMotion:
          break;

          //*********************
          // Perform translation
          //*********************

        case kButton1Down:
          {
            // _convert_pixel_to_pad_coordinate_ can not be used
            // here since *this* pointer is screwing everything
            canvas->SetCursor(kMove);
            canvas->GetRange(xdown, ydown, xup, yup);

            const double dx = xup - xdown;
            const double dy = yup - ydown;

            const double width  = this->GetWidth();
            const double height = this->GetHeight();

            xmin = x_ * dx / width;
            ymin = (height - y_) * dy / height;

            break;
          }

        case kButton1Motion:
          {
            canvas->SetCursor(kMove);

            const double dx = xup - xdown;
            const double dy = yup - ydown;

            const double width  = this->GetWidth();
            const double height = this->GetHeight();

            x = x_ * dx / width;
            y = (height-y_) * dy / height;

            canvas->Range(+xdown - (x - xmin), +ydown - (y - ymin),
                          +xup   - (x - xmin), +yup   - (y - ymin));
            canvas->Resize();
            canvas->Modified();
            canvas->Update();
            break;
          }

        case kButton1Up:
          {
            canvas->SetCursor(kMove);
            canvas->Modified();
            canvas->Update();
            break;
          }

          //******************
          // Perform rotation
          //******************
        case kButton2Down:
          {
            if (_view_dim_type != VIEW_3D) return false;
            canvas->SetCursor(kRotate);
            // remember position
            xmin   = canvas->GetX1();
            ymin   = canvas->GetY1();
            xrange = canvas->GetX2() - xmin;
            yrange = canvas->GetY2() - ymin;
            x      = canvas->PixeltoX(x_);
            y      = canvas->PixeltoY(y_);
            longitude1 = 180*(x-xmin)/xrange;
            latitude1  =  90*(y-ymin)/yrange;

            newlongitude = oldlongitude = -90 - canvas->GetPhi();
            newlatitude  = oldlatitude  =  90 - canvas->GetTheta();
            psideg       = view->GetPsi();

            break;
          }

        case kButton2Motion:
          {
            if (_view_dim_type != VIEW_3D) return false;
            canvas->SetCursor(kRotate);
            x = canvas->PixeltoX(x_);
            y = canvas->PixeltoY(y_);
            longitude2 = 180*(x-xmin)/xrange;
            latitude2  =  90*(y-ymin)/yrange;
            dlongitude   = longitude2   - longitude1;
            dlatitude    = latitude2    - latitude1;
            newlatitude  = oldlatitude  + dlatitude;
            newlongitude = oldlongitude - dlongitude;
            psideg       = view->GetPsi();
            view->SetView(newlongitude, newlatitude, psideg, irep);
            canvas->SetPhi(-90-newlongitude);
            canvas->SetTheta(90-newlatitude);
            // Interactively view the rotation
            canvas->Modified();
            canvas->Update();
            break;
          }

        case kButton2Up:
          {
            if (_view_dim_type != VIEW_3D) return false;
            // Recompute new view matrix and redraw
            psideg = view->GetPsi();
            view->SetView(newlongitude, newlatitude, psideg, irep);
            canvas->SetPhi(-90-newlongitude);
            canvas->SetTheta(90-newlatitude);
            canvas->Modified();
            canvas->Update();
            break;
          }

          //********************
          // Popup Notification
          //********************
        case kButton3Down:
          {
            double xworld, yworld;
            this->_convert_pixel_to_world_coordinates_(x_, y_, xworld, yworld);
            // // popup context menu
            // pad = Pick(px, py, prevSelObj);
            // if (!pad) return;

            // if (!fDoubleBuffer) FeedbackMode(kFALSE);

            // if (fContextMenu && !fSelected->TestBit(kNoContextMenu) &&
            //     !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu))
            //canvas->GetContextMenu()->Popup(x_, y_, canvas->GetSelected());
            //              fContextMenu->Popup(x_, y_, new , this, pad);
            break;
          }

        }

        // set back to default transformation mode
        canvas->AbsCoordinates(false);
        return true;
      }


    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of pad_embedded_viewer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
