// -*- mode: c++ ; -*-
/* pad_embedded_viewer.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-12-12
 * Last modified: 2010-12-12
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
 *   Default PAD embedded view
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_PAD_EMBEDDED_VIEWER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_PAD_EMBEDDED_VIEWER_H 1

#include <string>
#include <map>

#include <falaise/snemo/view/i_embedded_viewer.h>
#include <falaise/snemo/view/view_models.h>

#include <TRootEmbeddedCanvas.h>
#include <KeySymbols.h>
#include <TVector3.h>

class TGFrame;
class TCanvas;
class TObjArray;

namespace snemo {

  namespace visualization {

    namespace view {

      class i_draw_manager;

      class pad_embedded_viewer : public i_embedded_viewer,
                                  public TRootEmbeddedCanvas
      {

      public:

        pad_embedded_viewer  (const std::string & name_, const TGFrame * frame_,
                              const unsigned int width_ = 0, const unsigned int height_ = 0,
                              const view_dim_type view_dim_ = VIEW_3D);
        virtual ~pad_embedded_viewer ();

        virtual void clear ();
        virtual void reset ();

        virtual void set_view_type      (const view_type view_type_);
        virtual view_type get_view_type () const;

        virtual void update_detector ();
        virtual void update_scene    (i_draw_manager * drawer_);

        virtual TCanvas * get_canvas ();
        virtual TGFrame * get_frame  ();

        virtual void search_for_boundaries ();

        virtual void optimize_range (const TVector3 & min_bound_,
                                     const TVector3 & max_bound_);

        // ROOT Interface
        virtual bool HandleContainerMotion      (Event_t* event_) { return _handle_container_motion (event_); }
        virtual bool HandleContainerKey         (Event_t* event_) { return _handle_container_key (event_); }
        virtual bool HandleContainerButton      (Event_t* event_) { return _handle_container_button (event_); }
        virtual bool HandleContainerDoubleClick (Event_t* event_) { return _handle_container_double_click (event_); }

      protected:

        virtual bool _handle_container_motion       (Event_t* event_);
        virtual bool _handle_container_key          (Event_t* event_);
        virtual bool _handle_container_button       (Event_t* event_);
        virtual bool _handle_container_double_click (Event_t* event_);
        virtual bool _handle_mouse_wheel            (Event_t* event_);

        virtual bool _handle_input (const int event_, const int x_, const int y_);

        virtual bool _zoom             (const int x_, const int y_);
        virtual bool _unzoom           (const int x_, const int y_);
        virtual bool _zoom_to_position (const int x_, const int y_);

        virtual bool _move   (const EKeySym key_pressed_);
        virtual bool _rotate (const EKeySym key_pressed_);

      private:

        pad_embedded_viewer             ();                            // not implemented
        pad_embedded_viewer             (const pad_embedded_viewer &); // not implemented
        pad_embedded_viewer & operator= (const pad_embedded_viewer &); // not implemented

        void _set_pad_style_() const;

        const TVector3 & _get_minimal_roi_bound_ () const;

        const TVector3 & _get_maximal_roi_bound_ () const;

        TVector3 & _grab_minimal_roi_bound_ ();

        TVector3 & _grab_maximal_roi_bound_ ();

        void _set_scale_factors_ ();

        void _scale_text_ ();

        void _convert_pixel_to_pad_coordinates_ (const int xpixel_,
                                                 const int ypixel_,
                                                 double & xpad_,
                                                 double & ypad_);

        void _convert_pixel_to_world_coordinates_ (const int xpixel_,
                                                   const int ypixel_,
                                                   double & xworld_,
                                                   double & yworld_);

        void _convert_world_to_pad_coordinates_ (const double xworld_,
                                                 const double yworld_,
                                                 const double zworld,
                                                 double & xpad_,
                                                 double & ypad_);

      private:

        TObjArray * _objects_;
        TObjArray * _text_objects_;

        view_type   _view_type_;

        double      _zoom_factor_;
        int         _zoom_index_;

        // For 2D purpose
        bool                        _scaling_;
        std::map<view_type, double> _scale_factors_;
        std::map<view_type, double> _fudge_factors_;

        // ROI boundaries
        bool _search_for_boundaries_done_;
        TVector3 _min_roi_bound_;
        TVector3 _max_roi_bound_;

      };

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_PAD_EMBEDDED_VIEWER_H

// end of pad_embedded_viewer.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
