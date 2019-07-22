// -*- mode: c++ ; -*-
/* style_manager.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-13
 * Last modified: 2010-06-13
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
 *   style definition
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_STYLE_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_STYLE_MANAGER_H 1

#include <map>
#include <string>

// Interfaces
#include <datatools/i_tree_dump.h>
#include <EventBrowser/utils/singleton.h>

#include <EventBrowser/detector/volume_model.h>

namespace datatools {
class properties;
}

namespace snemo {

namespace visualization {

namespace view {

/// \brief A class that manages all the style configuration (color, line
/// style...)
class style_manager : public utils::singleton<style_manager>, public datatools::i_tree_dumpable {
 public:
  /// Structure that defines detector volume properties
  struct volume_properties {
    detector::visibility_type _visibility_;
    size_t _color_;
    size_t _transparency_;
  };

  /// Structure that defines particle properties
  struct particle_properties {
    bool _visibility_;
    size_t _color_;
    std::string _latex_name_;
  };

  /// Return initialization status
  bool is_initialized() const;

  /// Use OpenGL rendering
  bool use_opengl() const;

  /// Set detector setup label
  void set_setup_label(const std::string& setup_label_);

  /// Get detector setup label
  const std::string& get_setup_label() const;

  /// Set style file location
  void set_filename(const std::string& filename_);

  /// Get style file location
  const std::string& get_filename() const;

  /// Get startup tab for event browser
  size_t get_startup_tab() const;

  /// Get startup tab for 2D tab component
  size_t get_startup_2d_view() const;

  /// Set background color
  void set_background_color(const size_t color_);

  /// Get background color
  size_t get_background_color() const;

  /// Set Monte-Carlo line style (solid, dashed)
  void set_mc_line_style(const size_t style_);

  /// Get Monte-Carlo line style
  size_t get_mc_line_style() const;

  /// Set Monte-Carlo line width
  void set_mc_line_width(const size_t width_);

  /// Get Monte-Carlo line width
  size_t get_mc_line_width() const;

  /// Get color for calibrated info
  size_t get_calibrated_data_color() const;

  /// Check color palette availability
  bool has_color_palette() const;

  /// Return color id from color palette
  size_t get_color(const size_t id_) const;

  /// Typedef referecing volume properties
  typedef std::map<std::string, volume_properties> volume_properties_dict_type;

  /// Get a mutable reference to volume properties
  volume_properties_dict_type& grab_volumes_properties();

  /// Get a non-mutable reference to volume properties
  const volume_properties_dict_type& get_volumes_properties() const;

  /// Check volume properties availability for a given volume name
  bool has_volume_properties(const std::string& volume_name_) const;

  /// Get volume color for a given volume name
  size_t get_volume_color(const std::string& volume_name_) const;

  /// Get volume transparency for a a given volume name
  size_t get_volume_transparency(const std::string& volume_name_) const;

  /// Get volume visibility for a given volume name
  detector::visibility_type get_volume_visibility(const std::string& volume_name_) const;

  /// Typedef referecing particle properties
  typedef std::map<std::string, particle_properties> particle_properties_dict_type;

  /// Get a mutable reference to particle properties
  particle_properties_dict_type& grab_particles_properties();

  /// Get a non-mutable reference to particle properties
  const particle_properties_dict_type& get_particles_properties() const;

  /// Add particle properties to a given particle
  bool add_particle_properties(const std::string& particle_name_);

  /// Check particle properties availability for a given particle name
  bool has_particle_properties(const std::string& particle_name_) const;

  /// Get particle color for a given particle name
  size_t get_particle_color(const std::string& particle_name_) const;

  /// Get particle visbility for a given particle name
  bool get_particle_visibility(const std::string& particle_name_) const;

  /// Set location to save 2D/3D snapshots
  void set_save_directory(const std::string& save_directory_);

  /// Set the default file extension for 2D/3D snapshots
  void set_save_extension(const std::string& save_extension_);

  /// Set the prefix name for 2D/3D snapshots
  void set_save_prefix(const std::string& save_prefix_);

  /// Return location where to save 2D/3D snapshots
  const std::string& get_save_directory() const;

  /// Return file extension of 2D/3D snapshots
  const std::string& get_save_extension() const;

  /// Return prefix name of 2D/3D snapshots
  const std::string& get_save_prefix() const;

  /// Initialization method
  void initialize(const std::string& style_filename_ = "");

  /// Reset
  void reset();

  /// Reload style configuration
  void reload();

  /// Dump style configuration into file
  void dump_into_file(const std::string& filename_ = "");

  /// Smart dump
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

 private:
  /// Forbid default constructor
  style_manager();

  /// Forbid destructor (done within singleton object)
  ~style_manager();

  /// Non copyable constructor
  style_manager(const style_manager&);

  /// Non-copyable assignation
  style_manager& operator=(const style_manager&);

  /// Make the class singleton friend
  friend class utils::singleton<style_manager>;

  /// Main initialization process
  void _at_init_(const std::string& style_filename_);

  /// Default value of data member
  void _set_default_();

  /// Build particle dictionnary
  void _build_particle_dictionnary_();

  /// Read user specific options
  void _read_style_file_(const std::string& style_filename_);

  /// Set GUI style
  void _set_gui_style_() const;

  /// Dedicated function for geometry settings section
  void _set_geometry_settings_(const datatools::properties& config_);

  /// Dedicated function for particle settings section
  void _set_particle_settings_(const datatools::properties& config_);

  /// Dedicated function for misc. settings section
  void _set_miscellaneous_(const datatools::properties& config_);

 private:
  bool _initialized_;  //!< Initialization flag

  std::string _filename_;     //!< Style filename location
  std::string _setup_label_;  //!< Dectetor setup label

  size_t _startup_tab_;
  size_t _startup_2d_view_;
  size_t _background_color_;

  // volume stuff
  size_t _default_volume_color_;
  size_t _default_volume_transparency_;
  detector::visibility_type _default_volume_visibility_;
  volume_properties_dict_type _volumes_properties_;

  // MC stuff
  size_t _mc_line_style_;
  size_t _mc_line_width_;
  size_t _undefined_particle_color_;
  particle_properties_dict_type _particles_properties_;
  std::map<std::string, std::string> _particle_name_dict_;

  // Calibrated stuff
  size_t _calibrated_data_color_;

  // Save options
  std::string _save_directory_;
  std::string _save_extension_;
  std::string _save_prefix_;

  std::vector<size_t> _color_palette_;
};

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_VISUALIZATION_VIEW_STYLE_MANAGER_H

// end of style_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
