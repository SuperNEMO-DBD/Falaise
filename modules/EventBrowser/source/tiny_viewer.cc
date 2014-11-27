/* tiny_viewer.cc
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

#include <datatools/exception.h>

#include <falaise/snemo/view/tiny_viewer.h>
#include <falaise/snemo/view/style_manager.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/i_draw_manager.h>

#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/utils/root_utilities.h>

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

      void tiny_viewer::set_external_canvas (TCanvas * canvas_)
      {
        _canvas_ = canvas_;
        return;
      }

      // ctor:
      tiny_viewer::tiny_viewer () :
        i_embedded_viewer ()
      {
        _canvas_       = 0;
        _objects_      = 0;
        _text_objects_ = 0;

        const int vtype = style_manager::get_instance ().get_startup_2d_view ();
        _view_type_ = static_cast<view_type>(vtype);

        _zoom_factor_ = 2.0;
        _zoom_index_  = 0;

        _min_roi_bound_.SetXYZ (+std::numeric_limits<double>::infinity (),
                                +std::numeric_limits<double>::infinity (),
                                +std::numeric_limits<double>::infinity ());
        _max_roi_bound_.SetXYZ (-std::numeric_limits<double>::infinity (),
                                -std::numeric_limits<double>::infinity (),
                                -std::numeric_limits<double>::infinity ());

        return;
      }

      tiny_viewer::tiny_viewer (const std::string & name_,
                                const unsigned int width_,
                                const unsigned int height_,
                                const view_dim_type view_dim_) :
        i_embedded_viewer (view_dim_)
      {
        _canvas_ = new TCanvas (name_.c_str (), name_.c_str (), width_, height_);

        _text_objects_ = 0;
        _objects_      = 0;

        const int vtype = style_manager::get_instance ().get_startup_2d_view ();
        _view_type_ = static_cast<view_type>(vtype);

        _zoom_factor_ = 2.0;
        _zoom_index_  = 0;

        _min_roi_bound_.SetXYZ (+std::numeric_limits<double>::infinity (),
                                +std::numeric_limits<double>::infinity (),
                                +std::numeric_limits<double>::infinity ());
        _max_roi_bound_.SetXYZ (-std::numeric_limits<double>::infinity (),
                                -std::numeric_limits<double>::infinity (),
                                -std::numeric_limits<double>::infinity ());
        return;
      }

      // dtor:
      tiny_viewer::~tiny_viewer ()
      {
        return;
      }

      void tiny_viewer::clear ()
      {
        this->get_canvas ()->SetEditable (true);
        this->get_canvas ()->Clear ();

        return;
      }

      void tiny_viewer::reset ()
      {
        _zoom_index_ = 0;

        // For 2D view, shapes are deformed by ROOT: cubes projection
        // does not give a square! ROOT seems to optimize the size of
        // objects in a canvas in order to get them as big as possible
        // Here we calculate the scale factor given the dimensions of
        // the detector and the size of the embedded canvas
        if (_view_dim_type == VIEW_2D)
          this->_set_scale_factors_ ();

        TCanvas * canvas = this->get_canvas ();

        switch (_view_dim_type)
          {
          case VIEW_2D:
            {
              canvas->GetView ()->SetParallel ();

              if (_view_type_ == TOP_VIEW)   canvas->GetView ()->TopView ();
              if (_view_type_ == FRONT_VIEW) canvas->GetView ()->Front ();
              if (_view_type_ == SIDE_VIEW)  canvas->GetView ()->RotateView (180.0, 90.0);
              // TView::Side () shows the other side
              if (options_manager::get_instance ().get_option_flag (FOCUS_ROI) && _objects_)
                {
                  optimize_range (_get_minimal_roi_bound_ (),
                                  _get_maximal_roi_bound_ ());
                }
              else
                {
                  const double scale_factor = _scale_factors_.at (_view_type_);
                  const double fudge_factor = _fudge_factors_.at (_view_type_);
                  canvas->Range (-scale_factor/fudge_factor, -1./fudge_factor,
                                 scale_factor/fudge_factor, 1./fudge_factor);
                }
            }
            break;
          case VIEW_3D:
            {
              canvas->GetView ()->SetPerspective ();
              canvas->Range (-1, -1, 1, 1);
            }
            break;
          default:
            break;
          }

        canvas->SetEditable (false);
        canvas->Resize ();
        this->_scale_text_ ();
        canvas->Modified ();
        canvas->Update ();

        return;
      }

      void tiny_viewer::set_view_type (const view_type view_type_)
      {
        _view_type_ = view_type_;
        this->reset ();
        return;
      }

      view_type tiny_viewer::get_view_type () const
      {
        return _view_type_;
      }

      void tiny_viewer::update_detector ()
      {
        this->clear ();

        TCanvas * canvas = this->get_canvas ();

        canvas->cd ();
        canvas->SetEditable (true);
        canvas->SetFillColor (style_manager::get_instance ().get_background_color ());

        detector::detector_manager & detector_mgr = detector::detector_manager::get_instance ();

        if (detector_mgr.is_initialized ())
          {
            TGeoVolume * world_volume = detector_mgr.grab_world_volume ();
            world_volume->Draw ();
            this->reset ();
          }

        canvas->SetEditable (false);
        canvas->Modified ();
        canvas->Update ();

        return;
      }

      void tiny_viewer::update_scene (i_draw_manager * drawer_)
      {
        TCanvas * canvas = this->get_canvas ();

        canvas->cd ();
        canvas->SetEditable (true);
        canvas->SetFillColor (style_manager::get_instance ().get_background_color ());

        if (drawer_)
          {
            // if (_view_dim_type == VIEW_3D)
            //   {
            //     drawer_->draw_legend ();
            //   }

            if (_view_dim_type == VIEW_2D)
              {
                // Special use of font for TPad: since text size is static,
                // one has to rescale the text size with respect to zoom
                // factor used. This is done by _scale_text_ method.
                _text_objects_ = drawer_->get_text_objects ();
                this->_scale_text_ ();
                drawer_->draw_text ();

                if (options_manager::get_instance ().get_option_flag (FOCUS_ROI))
                  {
                    // Retrieve pointer to saved objects to look for the
                    // boundaries of the ROI inside this drawn objects
                    _objects_ = drawer_->get_objects ();

                    _min_roi_bound_.SetXYZ (+std::numeric_limits<double>::infinity (),
                                            +std::numeric_limits<double>::infinity (),
                                            +std::numeric_limits<double>::infinity ());
                    _max_roi_bound_.SetXYZ (-std::numeric_limits<double>::infinity (),
                                            -std::numeric_limits<double>::infinity (),
                                            -std::numeric_limits<double>::infinity ());
                    search_for_boundaries ();
                  }
              }

            // Draw every objects
            drawer_->draw ();
          }

        canvas->SetEditable (false);
        canvas->Modified ();
        canvas->Update ();
        return;
      }

      TCanvas * tiny_viewer::get_canvas ()
      {
        DT_THROW_IF (!_canvas_, std::logic_error, "No canvas has be instantiated!");
        return _canvas_;
      }

      TGFrame * tiny_viewer::get_frame ()
      {
        return 0;
      }

      void tiny_viewer::search_for_boundaries ()
      {
        if (!_objects_) return;

        TVector3 & min_roi_bound = _grab_minimal_roi_bound_ ();
        TVector3 & max_roi_bound = _grab_maximal_roi_bound_ ();

        {
          TObjArrayIter iter (_objects_);
          while (iter.Next ())
            {
              const TObject * a_object = *iter;

              if (a_object->IsA () == TPolyLine3D::Class ())
                {
                  const TPolyLine3D * pl3d = dynamic_cast<const TPolyLine3D *>(a_object);

                  for (int i = 0; i < pl3d->Size (); ++i)
                    {
                      const double x = pl3d->GetP ()[3*i];
                      const double y = pl3d->GetP ()[3*i+1];
                      const double z = pl3d->GetP ()[3*i+2];

                      min_roi_bound.SetXYZ (std::min (min_roi_bound.x (), x),
                                            std::min (min_roi_bound.y (), y),
                                            std::min (min_roi_bound.z (), z));

                      max_roi_bound.SetXYZ (std::max (max_roi_bound.x (), x),
                                            std::max (max_roi_bound.y (), y),
                                            std::max (max_roi_bound.z (), z));
                    }
                }

              if (a_object->IsA () == TPolyMarker3D::Class ())
                {
                  const TPolyMarker3D * pm3d = dynamic_cast<const TPolyMarker3D *>(a_object);

                  for (int i = 0; i < pm3d->Size (); ++i)
                    {
                      const double x = pm3d->GetP ()[3*i];
                      const double y = pm3d->GetP ()[3*i+1];
                      const double z = pm3d->GetP ()[3*i+2];

                      min_roi_bound.SetXYZ (std::min (min_roi_bound.x (), x),
                                            std::min (min_roi_bound.y (), y),
                                            std::min (min_roi_bound.z (), z));

                      max_roi_bound.SetXYZ (std::max (max_roi_bound.x (), x),
                                            std::max (max_roi_bound.y (), y),
                                            std::max (max_roi_bound.z (), z));
                    }
                }

              if (a_object->IsA () == TMarker3DBox::Class ())
                {
                  const TMarker3DBox * m3d = dynamic_cast<const TMarker3DBox *>(a_object);
                  float x, y, z;
                  m3d->GetPosition (x, y, z);
                  float dx, dy, dz;
                  m3d->GetSize (dx, dy, dz);

                  min_roi_bound.SetXYZ (std::min (min_roi_bound.x (), (double)x - dx),
                                        std::min (min_roi_bound.y (), (double)y - dy),
                                        std::min (min_roi_bound.z (), (double)z - dz));

                  max_roi_bound.SetXYZ (std::max (max_roi_bound.x (), (double)x + dx),
                                        std::max (max_roi_bound.y (), (double)y + dy),
                                        std::max (max_roi_bound.z (), (double)z + dz));
                }
            }
        }

        return;
      }

      void tiny_viewer::optimize_range (const TVector3 & min_bound_,
                                        const TVector3 & max_bound_)
      {
        datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
        DT_LOG_TRACE (local_priority, "Entering...");
        DT_LOG_TRACE (local_priority, "Min bound (x,y,z)=("
                      << min_bound_.x () << ","
                      << min_bound_.y () << ","
                      << min_bound_.z () << ")");
        DT_LOG_TRACE (local_priority, "Max bound (x,y,z)=("
                      << max_bound_.x () << ","
                      << max_bound_.y () << ","
                      << max_bound_.z () << ")");

        // Adapt pad range to show the most essential part of the
        // event
        double xmin, xmax, ymin, ymax;
        _convert_world_to_pad_coordinates_ (min_bound_.x (),
                                            min_bound_.y (),
                                            min_bound_.z (),
                                            xmin, ymin);
        _convert_world_to_pad_coordinates_ (max_bound_.x (),
                                            max_bound_.y (),
                                            max_bound_.z (),
                                            xmax, ymax);

        TCanvas * canvas = this->get_canvas ();

        switch (_view_dim_type)
          {
          case VIEW_2D:
            {
              const double dw =
                static_cast<double> (canvas->GetWw  ()) /
                static_cast<double> (canvas->GetWh ());

              const double sf = _scale_factors_.at (_view_type_) / dw;

              double new_xmax = std::max (xmax, xmin) / sf;
              double new_ymax = std::max (ymax, ymin);
              double new_xmin = std::min (xmax, xmin) / sf;
              double new_ymin = std::min (ymax, ymin);

              xmax = new_xmax;
              xmin = new_xmin;
              ymax = new_ymax;
              ymin = new_ymin;

              const double dwpad = fabs (ymax - ymin) / fabs (xmax - xmin);
              if (dwpad*dw > 1.0)
                {
                  xmin = new_xmin - (fabs (ymin - ymax) * dw - fabs (new_xmax - new_xmin))/2.0;
                  xmax = new_xmax + (fabs (ymin - ymax) * dw - fabs (new_xmax - new_xmin))/2.0;
                }
              else
                {
                  ymin = new_ymin - (fabs (xmin - xmax) / dw  - fabs (new_ymax - new_ymin))/2.0;
                  ymax = new_ymax + (fabs (xmin - xmax) / dw  - fabs (new_ymax - new_ymin))/2.0;
                }

              xmin *= sf;
              xmax *= sf;

              // Sanity check
              if ((xmin >= xmax) || (ymin >= ymax))
                {
                  return;
                }

              //              canvas->Range (xmin, ymin, xmax, ymax);

              // Unzoom one time to be a bit larger
              const double xcenter = (xmax + xmin) / 2.0;
              const double ycenter = (ymax + ymin) / 2.0;

              double zoom_factor = 0.0;
              if (!_text_objects_->GetEntriesFast ())
                {
                  zoom_factor = 2.0;
                }
              else
                {
                  // const double ratio = fabs (xcenter - xmin) / fabs (ycenter - ymin);
                  // if (ratio > 1.0) zoom_factor = 1.0 + 1/ratio;
                  // else             zoom_factor = 1.0 + ratio;
                  if (_view_type_ == TOP_VIEW)        zoom_factor = 3.0;
                  else if (_view_type_ == FRONT_VIEW) zoom_factor = 4.0;
                  else if (_view_type_ == SIDE_VIEW)  zoom_factor = 2.0;
                }

              const double xdown = xcenter - zoom_factor * fabs (xcenter - xmin);
              const double ydown = ycenter - zoom_factor * fabs (ycenter - ymin);
              const double xup   = xcenter + zoom_factor * fabs (xcenter - xmax);
              const double yup   = ycenter + zoom_factor * fabs (ycenter - ymax);

              canvas->Range (xdown, ydown, xup, yup);
            }
          }

        canvas->Resize ();
        this->_scale_text_ ();
        canvas->Modified ();
        canvas->Update ();

        DT_LOG_TRACE (local_priority, "Exiting.");
        return;
      }

      const TVector3 & tiny_viewer::_get_minimal_roi_bound_ () const
      {
        return _min_roi_bound_;
      }

      const TVector3 & tiny_viewer::_get_maximal_roi_bound_ () const
      {
        return _max_roi_bound_;
      }

      TVector3 & tiny_viewer::_grab_minimal_roi_bound_ ()
      {
        return _min_roi_bound_;
      }

      TVector3 & tiny_viewer::_grab_maximal_roi_bound_ ()
      {
        return _max_roi_bound_;
      }

      void tiny_viewer::_convert_pixel_to_pad_coordinates_ (const int xpixel_,
                                                            const int ypixel_,
                                                            double & xpad_,
                                                            double & ypad_)
      {
        // TCanvas class provides a PixeltoXY method which seems to
        // work fine (_convert_pixel_to_world_coordinates_ method uses
        // it without apparent problems) except when (un)zooming is
        // performed after a window resize. Here we build our own
        // conversion method taking into account frame size
        TCanvas * canvas = this->get_canvas ();

        double xdown, ydown, xup, yup;
        canvas->GetRange (xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        const double dx = xup - xdown;
        const double dy = yup - ydown;

        const double width  = canvas->GetWw ();
        const double height = canvas->GetWh ();

        xpad_ = xpixel_ * dx / width + xdown;
        ypad_ = (height-ypixel_) * dy / height + ydown;

        return;
      }

      void tiny_viewer::_convert_pixel_to_world_coordinates_ (const int xpixel_,
                                                              const int ypixel_,
                                                              double & xworld_,
                                                              double & yworld_)
      {
        if (_view_dim_type == VIEW_3D)
          {
            DT_LOG_WARNING (options_manager::get_instance ().get_logging_priority (),
                            "This method only makes sense in 2D view!");
            return;
          }

        TCanvas * canvas = this->get_canvas ();

        const double xpad = canvas->PixeltoX (xpixel_);
        const double ypad = canvas->PixeltoY (ypixel_);

        const double pn[3] = {xpad, ypad, 0.0};
        double pw[3];
        canvas->GetView ()->NDCtoWC (pn, pw);

        if (_view_type_ == TOP_VIEW)
          {
            xworld_ = pw[0];
            yworld_ = pw[1];
          }
        else if (_view_type_ == FRONT_VIEW)
          {
            xworld_ = pw[0];
            yworld_ = pw[2];
          }
        else if (_view_type_ == SIDE_VIEW)
          {
            xworld_ = pw[1];
            yworld_ = pw[2];
          }

        return;
      }

      void tiny_viewer::_convert_world_to_pad_coordinates_ (const double xworld_,
                                                            const double yworld_,
                                                            const double zworld,
                                                            double & xpad_,
                                                            double & ypad_)
      {
        if (_view_dim_type == VIEW_3D)
          {
            DT_LOG_WARNING (options_manager::get_instance ().get_logging_priority (),
                            "This method only makes sense in 2D view!");
            return;
          }

        TCanvas * canvas = this->get_canvas ();

        const double pw[3] = {xworld_, yworld_, zworld};
        double pn[3];
        canvas->GetView ()->WCtoNDC (pw, pn);

        xpad_ = pn[0];
        ypad_ = pn[1];

        return;
      }

      void tiny_viewer::_set_scale_factors_ ()
      {
        // Get scale factor for each view : use to rescale 2D View
        // first scale wrt to the whole detector size
        const double * size_view = this->get_canvas ()->GetView ()->GetRmin ();

        _scale_factors_[TOP_VIEW]   = size_view[1]/size_view[0]; //width/length;
        _scale_factors_[FRONT_VIEW] = size_view[2]/size_view[0]; //height/length;
        _scale_factors_[SIDE_VIEW]  = size_view[2]/size_view[1]; //height/width;

        TCanvas * canvas = this->get_canvas ();

        const double dw =
          static_cast<double> (canvas->GetWw  ()) /
          static_cast<double> (canvas->GetWh ());

        // then, scale wrt to the frame size
        _scale_factors_[TOP_VIEW]   *= dw;
        _scale_factors_[FRONT_VIEW] *= dw;
        _scale_factors_[SIDE_VIEW]  *= dw;

        const detector::detector_manager & detector_mgr
          = detector::detector_manager::get_instance ();

        if (detector_mgr.get_setup_label () == detector::detector_manager::SNEMO)
          {
            // fudge factors for SuperNEMO detector
            _fudge_factors_[TOP_VIEW]   = 1.6;
            _fudge_factors_[FRONT_VIEW] = 1.6;
            _fudge_factors_[SIDE_VIEW]  = 0.83;
          }
        else
          {
            _fudge_factors_[TOP_VIEW]   = 1.0;
            _fudge_factors_[FRONT_VIEW] = 1.0;
            _fudge_factors_[SIDE_VIEW]  = 1.0;
          }
        return;
      }

      void tiny_viewer::_scale_text_ ()
      {
        datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
        DT_LOG_TRACE (local_priority, "Entering...");
        if (!_text_objects_)
          return;
        DT_LOG_TRACE (local_priority, "text objects address " << _text_objects_);

        TIter iter (_text_objects_);
        utils::root_utilities::TLatex3D * itext;
        while ((itext = (utils::root_utilities::TLatex3D *)iter.Next ()))
          {
            DT_LOG_TRACE (local_priority, "text address " << itext
                          << "world position (x, y, z) = ("
                          << itext->GetX () << ", " << itext->GetY () << ", " << itext->GetZ () << ")");

            // If same geom_id has text (FindObject compare geom_id
            // coordinates) then TLatex "splitline" function is used
            // to show both information
            utils::root_utilities::TLatex3D * found
              = (utils::root_utilities::TLatex3D*) _text_objects_->FindObject (itext);

            if (found)
              {
                const std::string str1 = found->GetText ();
                const std::string str2 = itext->GetText ();
                std::ostringstream oss;
                oss << "#splitline"
                    << "{" << str1 << "}"
                    << "{" << str2 << "}";

                // Something here is not clear for me
                itext = found;
                _text_objects_->Remove (itext);

                // Update TLatex text
                itext->SetText (oss.str ());
              }

            const double xworld = itext->GetX ();
            const double yworld = itext->GetY ();
            const double zworld = itext->GetZ ();
            double xpad, ypad;
            this->_convert_world_to_pad_coordinates_ (xworld, yworld, zworld,
                                                      xpad, ypad);
            DT_LOG_TRACE (local_priority, "pad coordinates (x, y) = ("
                          << xpad << ", " << ypad << ")");

            itext->SetPadCoordinates (xpad, ypad);

            // By default, centered text horizontally and vertically
            itext->SetTextAlign (22);

            // Use Helvetica font i.e. 4 with the higher precision
            // available i.e. 3 : the text size is then not
            // proportionnal to pad size but expresses in pixel value
            itext->SetTextFont (43);

            // Slightly increase text size value wrt zoom value
            double pixel_size = 13;
            _zoom_index_ >= 0 ?
              pixel_size *= (1.0 + _zoom_index_/10.0) :
              pixel_size *= 0.0;
            itext->SetTextSize (pixel_size);

            const detector::detector_manager & detector_mgr
              = detector::detector_manager::get_instance ();
            if (detector_mgr.get_setup_label () == detector::detector_manager::SNEMO)
              {
                // Hard coded but specific code to SuperNEMO
                if (_view_type_ != SIDE_VIEW &&
                    _zoom_index_ <= 2)
                  {
                    const double xshift = 0.15;
                    if (xpad > 0.0)
                      {
                        itext->SetTextAlign (12);
                        itext->SetPadCoordinates (xpad+xshift, ypad);
                      }
                    else
                      {
                        itext->SetTextAlign (32);
                        itext->SetPadCoordinates (xpad-xshift, ypad);
                      }
                  }
              }
            else if (detector_mgr.get_setup_label () == detector::detector_manager::BIPO1 ||
                     detector_mgr.get_setup_label () == detector::detector_manager::BIPO3)
              {
                if (_zoom_index_ <= 2)
                  {
                    const double yshift = 0.10;
                    if (_view_type_ == TOP_VIEW)
                      {
                        if (zworld > 0.0) itext->SetPadCoordinates (xpad, ypad+yshift);
                        else              itext->SetPadCoordinates (xpad, ypad-yshift);
                      }
                    else
                      {
                        if (ypad > 0.0) itext->SetPadCoordinates (xpad, ypad+yshift);
                        else            itext->SetPadCoordinates (xpad, ypad-yshift);
                      }
                  }
              }
          }
        return;
      }

      bool tiny_viewer::_zoom (const int x_, const int y_)
      {
        _zoom_index_++;

        TCanvas * canvas = this->get_canvas ();

        double xdown, ydown, xup, yup;
        canvas->GetRange (xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        double x, y;
        this->_convert_pixel_to_pad_coordinates_ (x_, y_, x, y);

        xdown = x - 1/_zoom_factor_ * fabs (x - xdown);
        ydown = y - 1/_zoom_factor_ * fabs (y - ydown);
        xup   = x + 1/_zoom_factor_ * fabs (x - xup);
        yup   = y + 1/_zoom_factor_ * fabs (y - yup);

        canvas->Range (xdown, ydown, xup, yup);
        canvas->Resize ();
        this->_scale_text_ ();
        canvas->Modified ();
        canvas->Update ();

        return true;
      }

      bool tiny_viewer::_unzoom (const int x_, const int y_)
      {
        _zoom_index_--;

        TCanvas * canvas = this->get_canvas ();

        double xdown, ydown, xup, yup;
        canvas->GetRange (xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        double x, y;
        this->_convert_pixel_to_pad_coordinates_ (x_, y_, x, y);

        xdown = x - _zoom_factor_ * fabs (x - xdown);
        ydown = y - _zoom_factor_ * fabs (y - ydown);
        xup   = x + _zoom_factor_ * fabs (x - xup);
        yup   = y + _zoom_factor_ * fabs (y - yup);

        canvas->Range (xdown, ydown, xup, yup);
        canvas->Resize ();
        this->_scale_text_ ();
        canvas->Modified ();
        canvas->Update ();

        return true;
      }

      bool tiny_viewer::_zoom_to_position (const int x_, const int y_)
      {
        // When zooming to position the value of zoom index can not be
        // retrieved since zoom is proportionnal to pad size. Then a
        // fixed value is likely enough. Need to be optimize...
        _zoom_index_ = 5;//static_cast<size_t> (10 / _zoom_factor_);

        TCanvas * canvas = this->get_canvas ();

        double xdown, ydown, xup, yup;
        canvas->GetRange (xdown, ydown, xup, yup);

        // Convert mouse coordinate into pad values
        double x, y;
        this->_convert_pixel_to_pad_coordinates_ (x_, y_, x, y);

        // Make a 'box' around the double-clicked point which size is
        // 10% the total size of canvas
        xdown = x - 0.1 * fabs (x - xdown);
        ydown = y - 0.1 * fabs (y - ydown);
        xup   = x + 0.1 * fabs (x - xup);
        yup   = y + 0.1 * fabs (y - yup);

        canvas->Range (xdown, ydown, xup, yup);
        canvas->Resize ();
        this->_scale_text_ ();
        canvas->Modified ();
        canvas->Update ();

        return true;
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of tiny_viewer.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
