/* style_manager.cc
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
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <TStyle.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TEnv.h>

#include <boost/filesystem.hpp>

#include <falaise/resource.h>

#include <falaise/snemo/event_browser_config.h>
#include <falaise/snemo/view/style_manager.h>
#include <falaise/snemo/view/options_manager.h>

#include <falaise/snemo/utils/root_utilities.h>

#include <datatools/utils.h>
#include <datatools/multi_properties.h>

namespace snemo {

  namespace visualization {

    namespace view {

      bool style_manager::is_initialized() const
      {
        return _initialized_;
      }

      bool style_manager::use_opengl() const
      {
        bool use_opengl = false;
#ifdef EVENTBROWSER_USE_OPENGL
        use_opengl = true;
#endif
        return use_opengl;
      }

      void style_manager::set_filename(const std::string & filename_)
      {
        _filename_ = filename_;
        return;
      }

      const std::string & style_manager::get_filename() const
      {
        return _filename_;
      }

      void style_manager::set_setup_label(const std::string & setup_label_)
      {
        _setup_label_ = setup_label_;
        return;
      }

      const std::string & style_manager::get_setup_label() const
      {
        return _setup_label_;
      }

      unsigned int style_manager::get_startup_tab() const
      {
        return _startup_tab_;
      }

      unsigned int style_manager::get_startup_2d_view() const
      {
        return _startup_2d_view_;
      }

      void style_manager::set_background_color(const unsigned int color_)
      {
        _background_color_ = color_;
      }

      unsigned int style_manager::get_background_color() const
      {
        return _background_color_;
      }

      void style_manager::set_mc_line_style(const unsigned int style_)
      {
        _mc_line_style_ = style_;
      }

      unsigned int style_manager::get_mc_line_style() const
      {
        return _mc_line_style_;
      }

      void style_manager::set_mc_line_width(const unsigned int width_)
      {
        _mc_line_width_ = width_;
      }

      unsigned int style_manager::get_mc_line_width() const
      {
        return _mc_line_width_;
      }

      unsigned int style_manager::get_calibrated_data_color() const
      {
        return _calibrated_data_color_;
      }

      bool style_manager::has_color_palette() const
      {
        return !_color_palette_.empty();
      }

      size_t style_manager::get_color(const size_t id_) const
      {
        size_t id = 0;
        if (!has_color_palette() || id_ > _color_palette_.size()) {
          id = 2 + id_; // Skip white & black color
        } else {
          id = _color_palette_[id_];
        }
        return id;
      }

      std::map<std::string, style_manager::volume_properties> &
      style_manager::grab_volumes_properties()
      {
        return _volumes_properties_;
      }

      const std::map<std::string, style_manager::volume_properties> &
      style_manager::get_volumes_properties() const
      {
        return _volumes_properties_;
      }

      bool style_manager::has_volume_properties(const std::string & volume_name_) const
      {
        std::map<std::string, volume_properties>::const_iterator found
          = _volumes_properties_.find(volume_name_);
        if (found == _volumes_properties_.end())
          return false;
        return true;
      }

      unsigned int style_manager::get_volume_color(const std::string & volume_name_) const
      {
        unsigned int volume_color = _default_volume_color_;
        if (has_volume_properties(volume_name_))
          volume_color = _volumes_properties_.at(volume_name_)._color_;
        return volume_color;
      }

      unsigned int style_manager::get_volume_transparency(const std::string & volume_name_) const
      {
        unsigned int volume_transparency = _default_volume_transparency_;
        if (has_volume_properties(volume_name_))
          volume_transparency = _volumes_properties_.at(volume_name_)._transparency_;
        // Take care of pad vs. opengl : if opengl is not used then
        // transparency must be less than 50% otherwise no detector
        // can be seen (the line will be invisible) !
        if (!this->use_opengl())
          return 50;
        else
          return volume_transparency;
      }

      detector::visibility_type style_manager::get_volume_visibility(const std::string & volume_name_) const
      {
        detector::visibility_type volume_visibility = detector::VISIBLE;
        if (has_volume_properties(volume_name_))
          volume_visibility = _volumes_properties_.at(volume_name_)._visibility_;
        return volume_visibility;
      }

      std::map<std::string, style_manager::particle_properties> & style_manager::grab_particles_properties()
      {
        return _particles_properties_;
      }

      const std::map<std::string, style_manager::particle_properties> & style_manager::get_particles_properties() const
      {
        return _particles_properties_;
      }

      bool style_manager::add_particle_properties(const std::string & particle_name_)
      {
        std::map<std::string, std::string>::const_iterator found
          = _particle_name_dict_.find(particle_name_);

        if (found == _particle_name_dict_.end())
          return false;

        _particles_properties_[particle_name_]._color_
          = utils::root_utilities::get_random_color();
        _particles_properties_[particle_name_]._latex_name_ = found->second;
        _particles_properties_[particle_name_]._visibility_ = true;

        return true;
      }

      bool style_manager::has_particle_properties(const std::string & particle_name_) const
      {
        std::map<std::string, particle_properties>::const_iterator found
          = _particles_properties_.find(particle_name_);
        if (found == _particles_properties_.end())
          return false;
        return true;
      }

      unsigned int style_manager::get_particle_color(const std::string & particle_name_) const
      {
        unsigned int particle_color = _undefined_particle_color_;
        if (has_particle_properties(particle_name_))
          particle_color = _particles_properties_.at(particle_name_)._color_;
        return particle_color;
      }

      bool style_manager::get_particle_visibility(const std::string & particle_name_) const
      {
        bool particle_visibility = true;
        if (has_particle_properties(particle_name_))
          particle_visibility = _particles_properties_.at(particle_name_)._visibility_;
        return particle_visibility;
      }

      void style_manager::set_save_directory(const std::string & save_directory_)
      {
        _save_directory_ = save_directory_;
      }

      void style_manager::set_save_extension(const std::string & save_extension_)
      {
        _save_extension_ = save_extension_;
      }

      void style_manager::set_save_prefix(const std::string & save_prefix_)
      {
        _save_prefix_ = save_prefix_;
      }

      const std::string & style_manager::get_save_directory() const
      {
        return _save_directory_;
      }

      const std::string & style_manager::get_save_extension() const
      {
        return _save_extension_;
      }

      const std::string & style_manager::get_save_prefix() const
      {
        return _save_prefix_;
      }

      // ctor:
      style_manager::style_manager()
      {
        this->_set_default_();
        return;
      }

      // dtor:
      style_manager::~style_manager()
      {
        return;
      }

      void style_manager::_build_particle_dictionnary_()
      {
        // build particle dictionnary
        _particle_name_dict_.insert(std::make_pair("gamma"                , "#gamma"));
        _particle_name_dict_.insert(std::make_pair("electron"             , "e^{-}"));
        _particle_name_dict_.insert(std::make_pair("positron"             , "e^{+}"));
        _particle_name_dict_.insert(std::make_pair("proton"               , "p"));
        _particle_name_dict_.insert(std::make_pair("neutron"              , "n"));
        _particle_name_dict_.insert(std::make_pair("alpha"                , "#alpha"));
        _particle_name_dict_.insert(std::make_pair("muon_plus"            , "#mu^{+}"));
        _particle_name_dict_.insert(std::make_pair("muon_minus"           , "#mu^{-}"));
        _particle_name_dict_.insert(std::make_pair("delta_ray_from_alpha" , "#delta_{#alpha}"));
        return;
      }

      void style_manager::_set_default_()
      {
        _initialized_ = false;
        _setup_label_ = "";

        _startup_tab_      = 0;
        _startup_2d_view_  = 0;
        _background_color_ = kBlack;
        _color_palette_.clear();

        _default_volume_color_        = kRed;
        _default_volume_transparency_ = 0;
        _default_volume_visibility_   = detector::VISIBLE;

        _mc_line_style_ = kSolid;
        _mc_line_style_ = 1;

        _calibrated_data_color_ = kGray;

        _undefined_particle_color_ = kGray;

        _save_directory_ = ".";
        _save_extension_ = ".eps";
        _save_prefix_    = "event_";

        _build_particle_dictionnary_();
        return;
      }

      void style_manager::initialize(const std::string & style_filename_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
        this->_at_init_(style_filename_);
        _initialized_ = true;
        return;
      }

      void style_manager::reset()
      {
        DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

        _volumes_properties_.clear();
        _particles_properties_.clear();

        this->_set_default_();

        _initialized_ = false;
        return;
      }

      void style_manager::reload()
      {
        // keep setup label before reseting
        const std::string current_filename = _filename_;
        this->reset();
        this->initialize(current_filename);
        return;
      }

      void style_manager::_at_init_(const std::string & style_filename_)
      {
        this->_set_global_style_();
        this->_read_style_file_(style_filename_);
        return;
      }

      void style_manager::_read_style_file_(const std::string & style_filename_)
      {
        if (style_filename_.empty()) {
          if (_setup_label_.empty()) {
            DT_LOG_ERROR(options_manager::get_instance().get_logging_priority(),
                         "Nor setup label neither filename given");
            return;
          }
          _filename_ = falaise::get_resource_dir() +
            "/modules/EventBrowser/styles/" + _setup_label_ + "_default.sty";
          // Replace double semi colon with underscore
          const size_t start_semi_colon = _filename_.find("::");
          if (start_semi_colon != std::string::npos)
            _filename_.replace(start_semi_colon, 2, "_");
        } else {
          _filename_ = style_filename_;
        }
        datatools::fetch_path_with_env(_filename_);
        if (! boost::filesystem::exists(_filename_)) {
          DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                         "No default style file has been found!");
          return;
        }

        DT_LOG_NOTICE(options_manager::get_instance().get_logging_priority(),
                      "Opened " << _filename_);

        // Get multiproperties:
        datatools::multi_properties configuration("name", "");
        configuration.read(_filename_);
        if (options_manager::get_instance().get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
          configuration.tree_dump(std::clog,
                                  "Style settings for the SuperNEMO event display program",
                                  "DEBUG: ");
        }

        // Browser settings:
        const datatools::properties & browser_config = configuration.get("browser_settings").get_properties();

        if (!browser_config.has_key("setup_label")) {
          DT_LOG_ERROR(options_manager::get_instance().get_logging_priority(),
                       "Setup label key does not exist");
          return;
        }

        if (_setup_label_.empty()) {
          _setup_label_ = browser_config.fetch_string("setup_label");
        } else {
          // sanity check:
          if (_setup_label_ != browser_config.fetch_string("setup_label")) {
            DT_LOG_ERROR(options_manager::get_instance().get_logging_priority(),
                         "Setup labels are differents");
            return;
          }
        }

        if (browser_config.has_key("startup_tab")) {
          _startup_tab_ = browser_config.fetch_integer("startup_tab");
        }

        if (browser_config.has_key("startup_2d_view")) {
          _startup_2d_view_ = browser_config.fetch_integer("startup_2d_view");
        }

        if (browser_config.has_key("background_color")) {
          _background_color_ =
            utils::root_utilities::get_color_value("background_color", browser_config);
        }

        if (browser_config.has_key("color_palette")) {
          std::vector<std::string> colors;
          browser_config.fetch("color_palette", colors);
          for (size_t i = 0; i < colors.size(); ++i) {
            _color_palette_.push_back(utils::root_utilities::get_color_value(colors[i]));
          }
        }

        // Geometry settings:
        const datatools::properties & geometry_config
          = configuration.get("geometry_settings").get_properties();
        this->_set_geometry_settings_(geometry_config);

        // Particle settings:
        const datatools::properties & particle_config
          = configuration.get("particle_settings").get_properties();
        this->_set_particle_settings_(particle_config);

        // Miscellaneous settings:
        const datatools::properties & misc_config
          = configuration.get("miscellaneous").get_properties();
        this->_set_miscellaneous_(misc_config);

        return;
      }

      void style_manager::_set_geometry_settings_(const datatools::properties & config_)
      {
        std::vector<std::string> volume_category_list;
        if (config_.has_key("volume_category_list")) {
          config_.fetch("volume_category_list", volume_category_list);
        }

        if (volume_category_list.empty()) {
          DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
                             "No volume category list found");
        } else {
          for (std::vector<std::string>::const_iterator
                 it_volume = volume_category_list.begin();
               it_volume != volume_category_list.end(); ++it_volume) {
            const std::string & volume_name = *it_volume;

            // first: get visibility
            const std::string volume_visibility = volume_name + ".visibility";
            if (config_.has_key(volume_visibility)) {
              const std::string & visibility_mode
                = config_.fetch_string(volume_visibility);

              detector::visibility_type visibility = detector::VISIBLE;

              if ( visibility_mode == "visible" ) {
                visibility = detector::VISIBLE;
              } else if ( visibility_mode == "invisible") {
                visibility = detector::INVISIBLE;
              } else if ( visibility_mode == "disable" ) {
                visibility = detector::DISABLE;
              } else {
                DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                               "Ignoring unknown visibility option '"
                               << visibility_mode << "'");
              }

              _volumes_properties_[volume_name]._visibility_ = visibility;
            } else {
              // by default, make it visible
              _volumes_properties_[volume_name]._visibility_ = detector::VISIBLE;
            }

            // second: get color
            const std::string volume_color = volume_name + ".color";
            if (config_.has_key(volume_color)) {
              _volumes_properties_[volume_name]._color_ =
                utils::root_utilities::get_color_value(volume_color, config_);
            }

            // third: get transpracency
            const std::string volume_transparency = volume_name + ".transparency";
            if (config_.has_key(volume_transparency)) {
              _volumes_properties_[volume_name]._transparency_ =
                config_.fetch_integer(volume_transparency);
            }
          }
        }

        return;
      }

      void style_manager::_set_particle_settings_(const datatools::properties & config_)
      {
        for (std::map<std::string, std::string>::const_iterator
               i = _particle_name_dict_.begin();
             i != _particle_name_dict_.end(); ++i) {
          const std::string particle_name = i->first;
          std::string key = particle_name + ".color";

          if (!config_.has_key(key))
            continue;

          // Set particle color
          _particles_properties_[particle_name]._color_
            = utils::root_utilities::get_color_value(key, config_);

          // Set particle visibility (default is true)
          key = particle_name + ".visibility";
          if (config_.has_key(key)) {
            _particles_properties_[particle_name]._visibility_
              = config_.has_flag(key);
          } else {
            _particles_properties_[particle_name]._visibility_ = true;
          }

          // Set latex name
          _particles_properties_[particle_name]._latex_name_ = i->second;

        }
        return;
      }

      void style_manager::_set_miscellaneous_(const datatools::properties & config_)
      {
        // MC stuff
        if (config_.has_key("mc_line_style"))
          _mc_line_style_ = utils::root_utilities::get_line_style("mc_line_style", config_);

        if (config_.has_key("mc_line_width"))
          _mc_line_width_ = config_.fetch_integer("mc_line_width");

        if (config_.has_key("calibrated_data_color"))
          _calibrated_data_color_ =
            utils::root_utilities::get_color_value("calibrated_data_color", config_);

        if (config_.has_key("save.directory"))
          _save_directory_ = config_.fetch_string("save.directory");

        if (config_.has_key("save.extension"))
          _save_extension_ = config_.fetch_string("save.extension");

        if (config_.has_key("save.prefix"))
          _save_prefix_ = config_.fetch_string("save.prefix");

        return;
      }

      void style_manager::_set_global_style_() const
      {
        gROOT->SetStyle("Plain");

        gStyle->SetTitleBorderSize(0);
        gStyle->SetTitleX(0.1f);
        gStyle->SetTitleW(0.8f);

        gStyle->SetPalette         (1,0);
        gStyle->SetCanvasBorderMode(0);
        gStyle->SetPadBorderMode   (0);
        gStyle->SetDrawBorder      (0);
        gStyle->SetFrameBorderMode (0);

        gStyle->SetLabelFont (42,"xyz");
        gStyle->SetLabelSize (0.05,"xyz");
        gStyle->SetTitleSize (0.055,"xyz");
        gStyle->SetNdivisions(505,"xyz");

        gStyle->SetTitleOffset(1.1, "Y");
        gStyle->SetTitleOffset(1.15, "X");
        gStyle->SetLabelOffset(0.01, "xy");

        gStyle->SetPadLeftMargin  (0.15);
        gStyle->SetPadRightMargin (0.05);
        gStyle->SetPadTopMargin   (0.05);
        gStyle->SetPadBottomMargin(0.15);

        // GUI style
        gEnv->SetValue("Gui.DefaultFont",  "-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.MenuFont",     "-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.MenuHiFont",   "-*-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.DocFixedFont", "-*-courier-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.DocPropFont",  "-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.IconFont",     "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.StatusFont",   "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1");
        gEnv->SetValue("Gui.BackgroundColor",         "#E0E0E0");
        gEnv->SetValue("Gui.ForegroundColor",         "#4B4A46");
        gEnv->SetValue("Gui.HighLightColor",          "#D0D0D0");
        gEnv->SetValue("Gui.SelectBackgroundColor",   "#4B4A46");
        gEnv->SetValue("Gui.SelectForegroundColor",   "white");
        gEnv->SetValue("Gui.DocumentBackgroundColor", "white");
        gEnv->SetValue("Gui.DocumentForegroundColor", "#4B4A46");
        gEnv->SetValue("Gui.TooltipBackgroundColor",  "#000000");
        gEnv->SetValue("Gui.TooltipForegroundColor",  "#E0E0E0");
        gEnv->SetValue("Gui.FrameBackgroundPixmap", "");
        gEnv->SetValue("Gui.DocumentBackgroundPixmap", "");

        return;
      }

      void style_manager::dump_into_file(const std::string & filename_)
      {
        std::string filename;
        if (filename_.empty()) {
          filename = _filename_;
        } else {
          filename = filename_;
        }

        datatools::fetch_path_with_env(filename);
        std::ofstream fout(filename.c_str());

        const std::string line_delimiter
          = "#########################################################";

        fout << "# -*- mode: conf-unix; -*-" << std::endl;
        fout << std::endl;
        fout << "[name=\"browser_settings\"]" << std::endl;
        fout << std::endl;
        fout << "#@description Setup label." << std::endl;
        fout << "setup_label : string = \"" << _setup_label_ << "\"" << std::endl;
        fout << std::endl;
        fout << "#@description Tab to view at startup." << std::endl;
        fout << "# 0: 3D Display 1: Raw Data 2: Options, 3: Selection" << std::endl;
        fout << "startup_tab : integer = " << _startup_tab_ << std::endl;
        fout << std::endl;
        fout << "# 0: Top view 1: Front view 2: Side view" << std::endl;
        fout << "startup_2d_view : integer = " << _startup_2d_view_ << std::endl;
        fout << std::endl;
        fout << "#@description Background color of displays." << std::endl;
        fout << "background_color : integer[3] = ";
        utils::root_utilities::write_rgb_color(fout, _background_color_);
        fout << std::endl;

        fout << std::endl << line_delimiter << std::endl << std::endl;;
        fout << "[name=\"geometry_settings\"]" << std::endl;
        fout << std::endl;
        fout << "#@description List of volumes to take care." << std::endl;
        std::ostringstream oss;
        oss << "volume_category_list : string[" << _volumes_properties_.size() << "] = ";
        fout << oss.str() << "\\" << std::endl;
        size_t n_characters = 0;
        for (std::map<std::string, volume_properties>::iterator
               it_volume = _volumes_properties_.begin();
             it_volume != _volumes_properties_.end(); ++it_volume) {
          for (size_t i_space = 0; i_space < oss.str().size(); ++i_space)
            fout << " ";
          fout << "\"" << it_volume->first << "\"";

          const size_t index = std::distance(_volumes_properties_.begin(),
                                             it_volume) + 1;
          index == _volumes_properties_.size() ?
            fout << std::endl : fout << " \\" << std::endl;

          // search for largest volume word
          n_characters = std::max(n_characters, it_volume->first.size());
        }

        fout << std::endl;
        fout << "#@description Volume visibility (visible/invisible/disable)" << std::endl;
        for (std::map<std::string, volume_properties>::const_iterator
               it_volume = _volumes_properties_.begin();
             it_volume != _volumes_properties_.end(); ++it_volume) {
          fout << it_volume->first << ".visibility";
          for (size_t i_space = 0; i_space < n_characters - it_volume->first.size(); ++i_space)
            fout << " ";
          fout << " : string = ";
          switch (it_volume->second._visibility_) {
          case detector::VISIBLE:
            fout << "\"visible\"";
            break;
          case detector::INVISIBLE:
            fout << "\"invisible\"";
            break;
          case detector::DISABLE:
            fout << "\"disable\"";
            break;
          default:
            break;
          }
          fout << std::endl;
        }

        fout << std::endl;
        fout << "#@description Volume color" << std::endl;
        for (std::map<std::string, volume_properties>::const_iterator
               it_volume = _volumes_properties_.begin();
             it_volume != _volumes_properties_.end(); ++it_volume) {
          fout << it_volume->first << ".color";
          for (size_t i_space = 0; i_space < n_characters - it_volume->first.size(); ++i_space)
            fout << " ";
          fout << " : integer[3] = ";
          utils::root_utilities::write_rgb_color(fout, it_volume->second._color_);
          fout << std::endl;
        }

        fout << std::endl;
        fout << "#@description Volume transparency (0-100 range)" << std::endl;
        fout << "#note: this is disabled if OpenGL is not used" << std::endl;
        for (std::map<std::string, volume_properties>::const_iterator
               it_volume = _volumes_properties_.begin();
             it_volume != _volumes_properties_.end(); ++it_volume) {
          fout << it_volume->first << ".transparency";
          for (size_t i_space = 0; i_space < n_characters - it_volume->first.size(); ++i_space)
            fout << " ";
          fout << " : integer = " << it_volume->second._transparency_;
          fout << std::endl;
        }

        fout << std::endl << line_delimiter << std::endl << std::endl;;
        fout << "[name=\"particle_settings\"]" << std::endl;
        fout << std::endl;
        fout << "#@description MC particle color" << std::endl;
        n_characters = 0;
        for (std::map<std::string, particle_properties>::const_iterator
               it_particle = _particles_properties_.begin();
             it_particle != _particles_properties_.end(); ++it_particle) {
          n_characters = std::max(n_characters, it_particle->first.size());
        }

        for (std::map<std::string, particle_properties>::const_iterator
               it_particle = _particles_properties_.begin();
             it_particle != _particles_properties_.end(); ++it_particle) {
          fout << it_particle->first << ".color";
          for (size_t i_space = 0; i_space < n_characters - it_particle->first.size(); ++i_space)
            fout << " ";
          fout << " : integer[3] = ";
          utils::root_utilities::write_rgb_color(fout, it_particle->second._color_);
          fout << std::endl;
        }

        fout << std::endl << line_delimiter << std::endl << std::endl;;
        fout << "[name=\"miscellaneous\"]" << std::endl;
        fout << std::endl;
        fout << "#@description MC line style" << std::endl;
        fout << "mc_line_style : integer = " << _mc_line_style_ << std::endl;
        fout << "mc_line_width : integer = " << _mc_line_width_ << std::endl;

        fout << std::endl;
        fout << "#@description Figure save options" << std::endl;
        fout << "save.directory : string = \"" << _save_directory_ << "\"" << std::endl;
        fout << "save.extension : string = \"" << _save_extension_ << "\"" << std::endl;
        fout << "save.prefix    : string = \"" << _save_prefix_ << "\"" << std::endl;

        fout.close();

        DT_LOG_NOTICE(options_manager::get_instance().get_logging_priority(),
                      "Style definitions have been saved in " << filename);
        return;
      }

      void style_manager::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool /*inherit_*/) const
      {
        std::string indent;
        if (!indent_.empty()) {
          indent = indent_;
        }
        if (!title_.empty()) {
          out_ << indent << title_ << std::endl;
        }

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Initialized : " << (_initialized_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Filename    : " << (_filename_.empty() ? "<empty>" : _filename_)
             << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Setup       : " << (_setup_label_.empty() ? "<empty>" : _setup_label_)
             << std::endl;

        // Browser settings
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Browser settings : " << std::endl;

          std::ostringstream oss;
          oss << indent << datatools::i_tree_dumpable::skip_tag;
          out_ << oss.str() << datatools::i_tree_dumpable::tag
               << "Startup tab : " << _startup_tab_ << std::endl;
          out_ << oss.str() << datatools::i_tree_dumpable::last_tag
               << "Back. color : " << _background_color_ << std::endl;
        }

        // Geometry settings
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Geometry settings : "
               << _volumes_properties_.size() << " "
               << "volume" << (_volumes_properties_.size() > 1 ? "s" : "")
               << std::endl;

          for (std::map<std::string, volume_properties>::const_iterator
                 i = _volumes_properties_.begin();
               i != _volumes_properties_.end(); ++i) {
            std::ostringstream oss;
            oss << indent << datatools::i_tree_dumpable::skip_tag;
            out_ << oss.str();

            std::map<std::string, volume_properties>::const_iterator j = i;
            if (++j == _volumes_properties_.end()) {
              out_ << datatools::i_tree_dumpable::last_tag;
              oss << datatools::i_tree_dumpable::last_skip_tag
                  << datatools::i_tree_dumpable::last_skip_tag;
            } else {
              out_ << datatools::i_tree_dumpable::tag;
              oss << datatools::i_tree_dumpable::skip_tag
                  << datatools::i_tree_dumpable::last_skip_tag;
            }

            out_ << "Volume["
                 << std::distance(_volumes_properties_.begin(), i)
                 << "]: '" << i->first << "'" << std::endl;

            const volume_properties & vprop = i->second;
            out_ << oss.str()
                 << datatools::i_tree_dumpable::tag
                 << "Visibility   : ";
            if (vprop._visibility_ == detector::VISIBLE)
              out_ << "visible";
            if (vprop._visibility_ == detector::INVISIBLE)
              out_ << "invisible";
            if (vprop._visibility_ == detector::DISABLE)
              out_ << "disable";
            out_ << std::endl;

            out_ << oss.str()
                 << datatools::i_tree_dumpable::tag
                 << "Color (RGB)  : ";
            utils::root_utilities::write_rgb_color(out_, vprop._color_);
            out_ << std::endl;
            out_ << oss.str()
                 << datatools::i_tree_dumpable::last_tag
                 << "Transparency : " << vprop._transparency_
                 << "%" << std::endl;
          }
        }

        // Particle settings
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Particle settings : "
               << _particles_properties_.size() << " "
               << "particle" << (_particles_properties_.size() > 1 ? "s" : "")
               << std::endl;

          for (std::map<std::string, particle_properties>::const_iterator
                 i = _particles_properties_.begin();
               i != _particles_properties_.end(); ++i) {
            std::ostringstream oss;
            oss << indent << datatools::i_tree_dumpable::skip_tag;
            out_ << oss.str();

            std::map<std::string, particle_properties>::const_iterator j = i;
            if (++j == _particles_properties_.end()) {
              out_ << datatools::i_tree_dumpable::last_tag;
              oss << datatools::i_tree_dumpable::last_skip_tag
                  << datatools::i_tree_dumpable::last_skip_tag;
            } else {
              out_ << datatools::i_tree_dumpable::tag;
              oss << datatools::i_tree_dumpable::skip_tag
                  << datatools::i_tree_dumpable::last_skip_tag;
            }

            out_ << "Particle["
                 << std::distance(_particles_properties_.begin(), i)
                 << "]: '" << i->first << "'" << std::endl;

            const particle_properties & pprop = i->second;
            out_ << oss.str()
                 << datatools::i_tree_dumpable::tag
                 << "Visibility   : "
                 << (pprop._visibility_ ? "Yes" : "No") << std::endl;

            out_ << oss.str()
                 << datatools::i_tree_dumpable::last_tag
                 << "Color (RGB)  : ";
            utils::root_utilities::write_rgb_color(out_, pprop._color_);
            out_ << std::endl;
          }
        }

        // Miscellaneous
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "MC line style : " << _mc_line_style_ << std::endl;
          out_ << indent << datatools::i_tree_dumpable::tag
               << "MC line width : " << _mc_line_width_ << std::endl;

          out_ << indent << datatools::i_tree_dumpable::tag
               << "Save directory : '" << _save_directory_ << "'"
               << std::endl;
          out_ << indent << datatools::i_tree_dumpable::tag
               << "Save extension : '" << _save_extension_ << "'"
               << std::endl;
          out_ << indent << datatools::i_tree_dumpable::last_tag
               << "Save prefix    : '" << _save_prefix_ << "'"
               << std::endl;
        }

        return;
      }
    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of style_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
