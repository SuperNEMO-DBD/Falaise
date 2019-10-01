// -*- mode: c++ ; -*-
/* display_options.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-07-10
 * Last modified: 2010-07-10
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
 *   Options for 2D/3D display
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_OPTIONS_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_OPTIONS_H 1

#include <map>
#include <string>

#include <Rtypes.h>

class TGCompositeFrame;
class TGColorSelect;
class TGNumberEntryField;
class TGCheckButton;
class TGGroupFrame;
class TGTextEntry;
class TGLineStyleComboBox;
class TGLineWidthComboBox;

namespace snemo {

namespace visualization {

namespace view {

class event_browser;

class display_options {
 public:
  struct volume_widgets {
    TGGroupFrame* _tg_frame_;
    TGColorSelect* _tg_color_;
    TGNumberEntryField* _tg_transparency_;
    TGCheckButton* _tg_visibility_;
  };

  struct particle_widgets {
    TGColorSelect* _tg_color_;
    TGCheckButton* _tg_visibility_;
  };

 public:
  display_options();
  virtual ~display_options();

  void initialize(TGCompositeFrame* main_);

  bool is_initialized() const;

  void update();
  void clear();
  void reset();

  void process_volume_settings();
  void process_particle_settings();
  void process_button_misc_settings();
  void process_combo_misc_settings();
  void process_auto_reading();

  void reload_style_settings();
  void save_style_settings();

  void do_slider(const int delay_);

  void test_add_frame();

 private:
  void _at_init_(TGCompositeFrame* main_);
  void _at_construct_();

  void _build_volume_buttons_();
  void _build_particle_buttons_();
  void _build_misc_buttons_();

  void _set_button_values_();

 private:
  bool _initialized_;

  TGCompositeFrame* _main_;
  event_browser* _browser_;

  std::map<unsigned int, std::string> _button_dictionnary_;
  std::map<std::string, volume_widgets> _volumes_widgets_;
  std::map<std::string, particle_widgets> _particles_widgets_;

  TGColorSelect* _background_color_;
  TGLineWidthComboBox* _line_width_;
  TGLineStyleComboBox* _line_style_;
  TGTextEntry* _show_delay_;

  // No I/O so ClassDefVersionID = 0
  ClassDef(display_options, 0);
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_OPTIONS_H

// end of display_options.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
