/* display_options.cc
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

// Ourselves:
#include <falaise/snemo/view/display_options.h>

// Standard library:
#include <algorithm>
#include <list>
#include <sstream>
#include <stdexcept>

// This plugin:
#include <falaise/snemo/detector/detector_manager.h>
#include <falaise/snemo/view/event_browser.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/style_manager.h>

// Third party:
// - ROOT:
#include <TColor.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGColorSelect.h>
#include <TGComboBox.h>
#include <TGFileDialog.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGSlider.h>
#include <TGTextEntry.h>
#include <TSystem.h>
#include <TTimer.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
// - Falaise
#include <falaise/resource.h>

ClassImp(snemo::visualization::view::display_options);

    namespace snemo {
  namespace visualization {

  namespace view {

  bool display_options::is_initialized() const { return _initialized_; }

  // ctor:
  display_options::display_options() {
    _initialized_ = false;
    _browser_ = 0;
    _main_ = 0;
    return;
  }

  // dtor:
  display_options::~display_options() {
    this->reset();
    return;
  }

  void display_options::initialize(TGCompositeFrame* main_) {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    this->_at_init_(main_);
    _initialized_ = true;
    return;
  }

  void display_options::_at_init_(TGCompositeFrame* main_) {
    // Keep track of main frame in order to regenerate it for
    // different detector setup
    _main_ = main_;
    _main_->SetCleanup(kDeepCleanup);

    // event browser frame : this sucks but this is needed to
    // connect button with action. In order to instantiate
    // properly the number of parent, one has to count how many
    // frames have been instantiated until here
    TGCompositeFrame* parent = (TGCompositeFrame*)_main_->GetParent()
                                   ->GetParent()
                                   ->GetParent()
                                   ->GetParent()
                                   ->GetParent()
                                   ->GetParent();

    _browser_ = dynamic_cast<event_browser*>(parent);
    DT_THROW_IF(!_browser_, std::logic_error, "Event_browser can't be cast from frame!");

    this->_at_construct_();
    return;
  }

  void display_options::_at_construct_() {
    // Volume and particles properties
    this->_build_volume_buttons_();
    this->_build_particle_buttons_();
    this->_build_misc_buttons_();
    this->_set_button_values_();

    // Add automatic event reading
    TGGroupFrame* group = new TGGroupFrame(_main_, "Automatic event reading", kHorizontalFrame);
    _main_->AddFrame(group, new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3));

    TGCheckButton* auto_read = new TGCheckButton(group, "Enable..", AUTO_READ_ENABLE);
    group->AddFrame(auto_read, new TGLayoutHints(kLHintsLeft, 1, 25, 2, 2));

    const options_manager& options_mgr = options_manager::get_instance();
    options_mgr.is_automatic_event_reading_mode() ? auto_read->SetState(kButtonDown)
                                                  : auto_read->SetState(kButtonUp);
    auto_read->Connect("Clicked ()", "snemo::visualization::view::display_options", this,
                       "process_auto_reading ()");

    group->AddFrame(new TGLabel(group, "Interval [sec]:"),
                    new TGLayoutHints(kLHintsLeft, 5, 1, 2, 2));

    TGHSlider* slider = new TGHSlider(group, 200, kSlider1 | kScaleBoth, AUTO_READ_DELAY);
    slider->Connect("PositionChanged (Int_t)", "snemo::visualization::view::display_options", this,
                    "do_slider (const int)");
    slider->SetRange(1, 10);
    slider->SetPosition(static_cast<int>(options_mgr.get_automatic_event_reading_delay()));
    group->AddFrame(slider, new TGLayoutHints(kLHintsLeft, 5, 1, 2, 2));

    std::ostringstream oss;
    oss << options_mgr.get_automatic_event_reading_delay();
    _show_delay_ = new TGTextEntry(group, oss.str().c_str());
    _show_delay_->Resize(50, _show_delay_->GetDefaultHeight());
    _show_delay_->SetState(false);
    _show_delay_->SetFrameDrawn(true);
    _show_delay_->SetAlignment(kTextCenterX);
    group->AddFrame(_show_delay_, new TGLayoutHints(kFixedWidth, 2, 1, 2, 2));

    // Add 2 buttons to save and reload style file: there will be
    // always here, so no clearing will be required
    TGHorizontalFrame* process_style_buttons = new TGHorizontalFrame(_main_);
    _main_->AddFrame(process_style_buttons,
                     new TGLayoutHints(kLHintsBottom | kLHintsRight, 10, 10, 10, 10));
    TGTextButton* reload_style_file =
        new TGTextButton(process_style_buttons, "Reload style file", RELOAD_STYLE_FILE);
    reload_style_file->SetToolTipText("reload current version of style file");
    reload_style_file->Connect("Clicked ()", "snemo::visualization::view::display_options", this,
                               "reload_style_settings ()");
    process_style_buttons->AddFrame(reload_style_file,
                                    new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));

    TGTextButton* save_style_into_file =
        new TGTextButton(process_style_buttons, "Save style file as...", SAVE_STYLE_FILE);
    save_style_into_file->SetToolTipText("save current settings into style file");
    save_style_into_file->Connect("Clicked ()", "snemo::visualization::view::display_options", this,
                                  "save_style_settings ()");
    process_style_buttons->AddFrame(save_style_into_file,
                                    new TGLayoutHints(kLHintsNoHints, 2, 2, 2, 2));
    return;
  }

  void display_options::_build_volume_buttons_() {
    // Retrieve volume properties
    const style_manager& style_mgr = style_manager::get_instance();
    const std::map<std::string, style_manager::volume_properties>& volumes =
        style_mgr.get_volumes_properties();

    // Layout for positionning frame
    TGLayoutHints* layout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3);
    TGGroupFrame* volume_group = new TGGroupFrame(_main_, "Volume Settings", kVerticalFrame);
    volume_group->SetTitlePos(TGGroupFrame::kLeft);
    _main_->AddFrame(volume_group, layout);

    unsigned int id_button = _button_dictionnary_.size();
    for (std::map<std::string, style_manager::volume_properties>::const_iterator it_volume =
             volumes.begin();
         it_volume != volumes.end(); ++it_volume) {
      const std::string& volume_name = it_volume->first;

      // Volume name without _
      std::string volume_group_name = volume_name;
      std::replace(volume_group_name.begin(), volume_group_name.end(), '_', ' ');
      if (volume_group_name.find(".category") != std::string::npos) {
        volume_group_name.replace(volume_group_name.find(".category"), volume_group_name.length(),
                                  "");
      }

      // Define group frame
      TGCompositeFrame* cframe =
          new TGCompositeFrame(volume_group, 200, 1, kHorizontalFrame | kFixedWidth);
      volume_group->AddFrame(cframe);
      // _volumes_widgets_[volume_name]._tg_frame_ = volume_group;

      // Add visibility widget and check visibility state
      TGCheckButton* visibility_check =
          new TGCheckButton(cframe, volume_group_name.c_str(), id_button);
      visibility_check->Connect("Clicked()", "snemo::visualization::view::display_options", this,
                                "process_volume_settings ()");
      cframe->AddFrame(visibility_check,
                       new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
      _volumes_widgets_[volume_name]._tg_visibility_ = visibility_check;
      _button_dictionnary_[id_button++] = volume_name;

      // Add color selector
      TGColorSelect* color_selector = new TGColorSelect(
          cframe, TColor::Number2Pixel(style_mgr.get_volume_color(volume_name)), id_button);
      color_selector->Connect("ColorSelected (Pixel_t)",
                              "snemo::visualization::view::display_options", this,
                              "process_volume_settings ()");
      cframe->AddFrame(color_selector);
      // , new TGLayoutHints (kLHintsTop | kLHintsExpandX,
      //                      2, 2, 2, 2));

      _volumes_widgets_[volume_name]._tg_color_ = color_selector;
      _button_dictionnary_[id_button++] = volume_name;

      // // Add transparency slider
      // TGNumberEntryField* transparency_entry
      //   = new TGNumberEntryField (cframe, id_button,
      //                             style_mgr.get_volume_transparency (volume_name),
      //                             TGNumberFormat::kNESInteger,
      //                             TGNumberFormat::kNEANonNegative);
      // transparency_entry->SetLimits (TGNumberFormat::kNELLimitMinMax, 0, 100);
      // transparency_entry->Connect ("TextChanged (char*)",
      //                              "snemo::visualization::view::display_options",
      //                              this, "process_volume_settings ()");
      // cframe->AddFrame (transparency_entry);
      // // , new TGLayoutHints (kLHintsTop |
      // //                      kLHintsLeft, 2, 2, 2, 2));
      // _volumes_widgets_[volume_name]._tg_transparency_ = transparency_entry;
      // _button_dictionnary_[id_button++] = volume_name;
    }

    return;
  }

  void display_options::_build_particle_buttons_() {
    // Retrieve volume properties
    const style_manager& style_mgr = style_manager::get_instance();
    const std::map<std::string, style_manager::particle_properties>& particles =
        style_mgr.get_particles_properties();

    // Layout for positionning frame
    TGLayoutHints* layout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3);
    TGGroupFrame* particle_group = new TGGroupFrame(_main_, "Simulation Settings", kVerticalFrame);
    particle_group->SetTitlePos(TGGroupFrame::kLeft);
    _main_->AddFrame(particle_group, layout);

    unsigned int id_button = _button_dictionnary_.size();
    for (std::map<std::string, style_manager::particle_properties>::const_iterator it_particle =
             particles.begin();
         it_particle != particles.end(); ++it_particle) {
      const std::string& particle_name = it_particle->first;

      // Define group frame
      TGCompositeFrame* cframe =
          new TGCompositeFrame(particle_group, 200, 1, kHorizontalFrame | kFixedWidth);
      particle_group->AddFrame(cframe);
      //_particles_widgets_[volume_name]._tg_frame_ = particle_group;

      // Add visibility widget and check visibility state
      TGCheckButton* visibility_check = new TGCheckButton(cframe, particle_name.c_str(), id_button);
      visibility_check->Connect("Clicked()", "snemo::visualization::view::display_options", this,
                                "process_particle_settings ()");
      cframe->AddFrame(visibility_check,
                       new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
      _particles_widgets_[particle_name]._tg_visibility_ = visibility_check;
      _button_dictionnary_[id_button++] = particle_name;

      // Add color selector
      TGColorSelect* color_selector = new TGColorSelect(
          cframe, TColor::Number2Pixel(style_mgr.get_particle_color(particle_name)), id_button);
      color_selector->Connect("ColorSelected (Pixel_t)",
                              "snemo::visualization::view::display_options", this,
                              "process_particle_settings ()");
      cframe->AddFrame(color_selector);  //, new TGLayoutHints (kLHintsTop // | kLHintsExpandX
                                         //                    ,
                                         //                    2, 2, 2, 2));

      _particles_widgets_[particle_name]._tg_color_ = color_selector;
      _button_dictionnary_[id_button++] = particle_name;
    }

    return;
  }

  void display_options::_build_misc_buttons_() {
    // Retrieve misc properties
    const style_manager& style_mgr = style_manager::get_instance();

    // Layout for positionning frame
    TGLayoutHints* layout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 3, 3, 3);
    TGGroupFrame* misc_group = new TGGroupFrame(_main_, "Miscellaneous", kVerticalFrame);
    misc_group->SetTitlePos(TGGroupFrame::kLeft);
    _main_->AddFrame(misc_group, layout);

    {
      // Add color selector
      TGCompositeFrame* cframe =
          new TGCompositeFrame(misc_group, 200, 1, kHorizontalFrame | kFixedWidth);
      cframe->AddFrame(new TGLabel(cframe, "background color  "),
                       new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 2, 2));
      misc_group->AddFrame(cframe);

      _background_color_ = new TGColorSelect(
          cframe, TColor::Number2Pixel(style_mgr.get_background_color()), COLOR_BACKGROUND);
      _background_color_->Connect("ColorSelected (Pixel_t)",
                                  "snemo::visualization::view::display_options", this,
                                  "process_button_misc_settings ()");
      cframe->AddFrame(_background_color_);
    }

    {
      // Add line style selector
      TGCompositeFrame* cframe =
          new TGCompositeFrame(misc_group, 200, 1, kHorizontalFrame | kFixedWidth);
      cframe->AddFrame(new TGLabel(cframe, "line style  "),
                       new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 2, 2));
      misc_group->AddFrame(cframe);
      _line_style_ = new TGLineStyleComboBox(cframe, LINE_STYLE);
      _line_style_->Resize(90, 22);
      _line_style_->Connect("Selected (int)", "snemo::visualization::view::display_options", this,
                            "process_combo_misc_settings ()");
      cframe->AddFrame(_line_style_, new TGLayoutHints(kLHintsRight, 1, 1, 2, 2));
    }

    {
      // Add line width selector
      TGCompositeFrame* cframe =
          new TGCompositeFrame(misc_group, 200, 1, kHorizontalFrame | kFixedWidth);
      cframe->AddFrame(new TGLabel(cframe, "line width  "),
                       new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 1, 1, 2, 2));
      misc_group->AddFrame(cframe);
      _line_width_ = new TGLineWidthComboBox(cframe, LINE_WIDTH);
      _line_width_->Resize(90, 22);
      _line_width_->Connect("Selected(Int_t)", "snemo::visualization::view::display_options", this,
                            "process_combo_misc_settings ()");
      cframe->AddFrame(_line_width_, new TGLayoutHints(kLHintsRight, 1, 1, 2, 2));
    }

    return;
  }

  void display_options::_set_button_values_() {
    const style_manager& style_mgr = style_manager::get_instance();

    // Volume settings
    for (std::map<std::string, volume_widgets>::iterator it_widget = _volumes_widgets_.begin();
         it_widget != _volumes_widgets_.end(); ++it_widget) {
      const std::string& volume_name = it_widget->first;

      TGCheckButton* visibility_check = it_widget->second._tg_visibility_;
      switch (style_mgr.get_volume_visibility(volume_name)) {
        case detector::VISIBLE:
          visibility_check->SetState(kButtonDown);
          break;
        case detector::INVISIBLE:
          visibility_check->SetState(kButtonUp);
          break;
        case detector::DISABLE:
          visibility_check->SetDisabledAndSelected(false);
          visibility_check->SetToolTipText("Volume disabled in style file");
          break;
        default:
          break;
      }

      // Disable special shape volume (too hard to get back later)
      if (detector::detector_manager::get_instance().is_special_volume(volume_name)) {
        visibility_check->SetDisabledAndSelected(true);
        visibility_check->SetToolTipText("Volume disabled due to special shape");
      }

      TGColorSelect* color_selector = it_widget->second._tg_color_;
      const unsigned int color = style_mgr.get_volume_color(volume_name);
      color_selector->SetColor(TColor::Number2Pixel(color), false);

      // TGNumberEntryField* transparency_entry = it_widget->second._tg_transparency_;
      // const unsigned int value = style_mgr.get_volume_transparency (volume_name);
      // transparency_entry->SetNumber (value);
    }

    // MC particle settings
    for (std::map<std::string, particle_widgets>::iterator it_widget = _particles_widgets_.begin();
         it_widget != _particles_widgets_.end(); ++it_widget) {
      const std::string particle_name = it_widget->first;

      TGCheckButton* visibility_check = it_widget->second._tg_visibility_;
      style_mgr.get_particle_visibility(particle_name) ? visibility_check->SetState(kButtonDown)
                                                       : visibility_check->SetState(kButtonUp);

      TGColorSelect* color_selector = it_widget->second._tg_color_;
      const unsigned int color = style_mgr.get_particle_color(particle_name);
      color_selector->SetColor(TColor::Number2Pixel(color), false);
    }

    // Misc options
    const unsigned int color = style_mgr.get_background_color();
    _background_color_->SetColor(TColor::Number2Pixel(color), false);
    const unsigned int line_style = style_mgr.get_mc_line_style();
    _line_style_->Select(line_style, false);
    const unsigned int line_width = style_mgr.get_mc_line_width();
    _line_width_->Select(line_width, false);

    return;
  }

  void display_options::update() { return; }

  void display_options::clear() {
    // for (map<string, volume_widgets>::iterator
    //        it_widget = _volumes_widgets_.begin ();
    //      it_widget != _volumes_widgets_.end (); ++it_widget)
    //   {
    //     it_widget->second._tg_frame_->DestroyWindow ();
    //     it_widget->second._tg_frame_->Cleanup ();
    //     _main_->RemoveFrame (it_widget->second._tg_frame_);

    //     // delete it_widget->second._tg_color_;
    //     // delete it_widget->second._tg_transparency_;
    //     // delete it_widget->second._tg_visibility_;
    //     delete it_widget->second._tg_frame_;
    //   }

    _volumes_widgets_.clear();
    _button_dictionnary_.clear();

    return;
  }

  void display_options::reset() {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");
    this->clear();
    _initialized_ = false;
    return;
  }

  void display_options::process_volume_settings() {
    // This is a bit brute because one change cause the loading of
    // all buttons. Each action can be split into one given method
    TGButton* button = static_cast<TGButton*>(gTQSender);
    unsigned int id = button->WidgetId();

    // Retrieve style manager to change volume properties
    style_manager& style_mgr = style_manager::get_instance();
    std::map<std::string, style_manager::volume_properties>& volumes =
        style_mgr.grab_volumes_properties();

    // Get corresponding volume name wrt to button id. If id has
    // not been stored then signal comes from somewhere else. It
    // is just a safety check
    if (_button_dictionnary_.find(id) == _button_dictionnary_.end()) return;

    const std::string& volume_name = _button_dictionnary_.at(id);

    // Set visibility
    _volumes_widgets_[volume_name]._tg_visibility_->IsDown()
        ? volumes[volume_name]._visibility_ = detector::VISIBLE
        : volumes[volume_name]._visibility_ = detector::INVISIBLE;

    // Set color: convert Pixel_t value to integer
    unsigned int color = TColor::GetColor(_volumes_widgets_[volume_name]._tg_color_->GetColor());
    volumes[volume_name]._color_ = color;

    // // Set transparency
    // unsigned int transparency
    //   = _volumes_widgets_[volume_name]._tg_transparency_->GetNumber ();
    // volumes[volume_name]._transparency_ = transparency;

    // Update detector and event browser
    detector::detector_manager::get_instance().update();
    const bool reset_view = false;
    _browser_->update_browser(reset_view);

    return;
  }

  void display_options::process_particle_settings() {
    // This is a bit brute because one change cause the loading of
    // all buttons. Each action can be split into one given method
    TGButton* button = static_cast<TGButton*>(gTQSender);
    unsigned int id = button->WidgetId();

    // Retrieve style manager to change volume properties
    style_manager& style_mgr = style_manager::get_instance();
    std::map<std::string, style_manager::particle_properties>& particles =
        style_mgr.grab_particles_properties();

    // Get corresponding volume name wrt to button id. If id has
    // not been stored then signal comes from somewhere else. It
    // is just a safety check
    if (_button_dictionnary_.find(id) == _button_dictionnary_.end()) return;

    const std::string& particle_name = _button_dictionnary_.at(id);

    // Set visibility
    _particles_widgets_[particle_name]._tg_visibility_->IsDown()
        ? particles[particle_name]._visibility_ = true
        : particles[particle_name]._visibility_ = false;

    // Set color: convert Pixel_t value to integer
    unsigned int color =
        TColor::GetColor(_particles_widgets_[particle_name]._tg_color_->GetColor());
    particles[particle_name]._color_ = color;

    // Update event browser
    const bool reset_view = false;
    _browser_->update_browser(reset_view);

    return;
  }

  void display_options::process_button_misc_settings() {
    // This is a bit brute because one change cause the loading of
    // all buttons. Each action can be split into one given method
    TGButton* button = static_cast<TGButton*>(gTQSender);
    unsigned int id = button->WidgetId();

    // Retrieve style manager to change misc properties
    style_manager& style_mgr = style_manager::get_instance();

    switch (id) {
      case COLOR_BACKGROUND: {
        unsigned int color = TColor::GetColor(_background_color_->GetColor());
        style_mgr.set_background_color(color);
        break;
      }
    }

    // Update event browser
    const bool reset_view = false;
    _browser_->update_browser(reset_view);

    return;
  }

  void display_options::process_combo_misc_settings() {
    TGComboBox* combo = static_cast<TGComboBox*>(gTQSender);
    unsigned int id = combo->WidgetId();

    // Retrieve style manager to change misc properties
    style_manager& style_mgr = style_manager::get_instance();

    switch (id) {
      case LINE_WIDTH: {
        unsigned int width = _line_width_->GetSelected();
        style_mgr.set_mc_line_width(width);
        break;
      }
      case LINE_STYLE: {
        unsigned int style = _line_style_->GetSelected();
        style_mgr.set_mc_line_style(style);
        break;
      }
    }

    // Update event browser
    const bool reset_view = false;
    _browser_->update_browser(reset_view);

    return;
  }

  void display_options::process_auto_reading() {
    TGButton* button = static_cast<TGButton*>(gTQSender);
    unsigned int id = button->WidgetId();

    if (id == AUTO_READ_ENABLE) {
      options_manager& options_mgr = options_manager::get_instance();
      options_mgr.set_automatic_event_reading_mode(button->IsDown());

      if (options_mgr.is_automatic_event_reading_mode()) {
        const unsigned int nbr_seconds =
            (unsigned int)options_mgr.get_automatic_event_reading_delay() * 1000;
        TTimer::SingleShot(nbr_seconds, "snemo::visualization::view::event_browser", _browser_,
                           "change_event ()");
      }
    }

    return;
  }

  void display_options::reload_style_settings() {
    // Reload previous style settings
    style_manager::get_instance().reload();

    // Set button values
    this->_set_button_values_();

    // Update detector and event browser
    detector::detector_manager::get_instance().update();
    const bool reset_view = false;
    _browser_->update_browser(reset_view);

    return;
  }

  void display_options::save_style_settings() {
    const size_t nbr_file_format = 4;
    const char* save_as_types[nbr_file_format] = {"Style File", "*.sty", 0, 0};

    std::list<std::string> file_format;
    for (size_t i = 0; i < nbr_file_format / 2 - 1; ++i) {
      std::string dummy_one;
      dummy_one.assign(save_as_types[i * 2 + 1]);
      file_format.push_back(dummy_one);
    }

    std::list<std::string>::iterator it =
        find(file_format.begin(), file_format.end(), std::string("*.sty"));

    int typeidx = distance(file_format.begin(), it) * 2;
    bool overwr = false;

    const std::string default_dir = falaise::get_resource_dir() + "/modules/EventBrowser/styles";
    TGFileInfo file_info;
    file_info.fFileTypes = save_as_types;
    file_info.fIniDir = StrDup(default_dir.c_str());
    file_info.fFileTypeIdx = typeidx;
    file_info.fOverwrite = overwr;

    new TGFileDialog(gClient->GetRoot(), 0, kFDSave, &file_info);
    if (!file_info.fFilename) return;

    std::string file_name = file_info.fFilename;
    std::string file_type = file_info.fFileTypes[file_info.fFileTypeIdx + 1];
    std::string directory = file_info.fIniDir;

    // remove asterisk
    file_type.erase(0, 1);

    // if extension is missing to filename
    if (!TString(file_name).EndsWith(file_type.c_str())) {
      file_name += file_type;
    }

    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, file_name << " saved");

    style_manager::get_instance().dump_into_file(file_name);
    return;
  }

  void display_options::do_slider(const int delay_) {
    options_manager& options_mgr = options_manager::get_instance();
    options_mgr.set_automatic_event_reading_delay(delay_);
    std::ostringstream oss;
    oss << delay_;
    _show_delay_->SetText(oss.str().c_str());
  }

  void display_options::test_add_frame() {
    // this->clear ();

    // cout << "cocuou" << endl;
    // this->_build_volume_buttons_ ();
    // cout << "caca" << endl;

    // _main_->UnmapWindow ();
    // _main_->RemoveAll ();

    TGGroupFrame* volume_group = new TGGroupFrame(_main_, "toto", kHorizontalFrame);
    volume_group->SetTitlePos(TGGroupFrame::kLeft);
    _main_->AddFrame(volume_group);

    TGCheckButton* test = new TGCheckButton(volume_group, "test toto", 10000);
    volume_group->AddFrame(test, new TGLayoutHints(kLHintsTop & kLHintsLeft, 2, 2, 2, 2));

    volume_group->MapWindow();
    // _main_->MapWindow ();
    // _browser_->MapSubwindows ();
    // _browser_->MapWindow ();
  }

  }  // end of namespace view

  }  // end of namespace visualization

}  // end of namespace snemo

// end of display_options.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
