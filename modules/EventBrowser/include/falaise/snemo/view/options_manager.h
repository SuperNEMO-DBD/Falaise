// -*- mode: c++ ; -*-
/* options_manager.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-20
 * Last modified: 2010-06-20
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
 *   Options manager
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_VIEW_OPTIONS_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_VIEW_OPTIONS_MANAGER_H 1

#include <falaise/snemo/utils/singleton.h>
#include <falaise/snemo/view/event_browser_signals.h>

#include <datatools/logger.h>

#include <vector>
#include <string>
#include <map>

namespace snemo {

  namespace visualization {

    namespace view {

      class options_manager : public utils::singleton<options_manager>
      {
      public:

        /// Get the logging priority threshold
        datatools::logger::priority get_logging_priority () const;

        /// Set the logging priority threshold
        void set_logging_priority (datatools::logger::priority priority_);

        void reset ();

        void set_defaults ();

        void set_default_options ();

        void print_examples (std::ostream      & out_,
                             const std::string & name_,
                             const std::string & title_) const;

        bool parse_command_line (int argc_, char** argv_);

        /// Setting the input file list
        void set_input_files(const std::vector<std::string> & input_files_);

        const std::vector<std::string> & get_input_files () const;

        bool is_preload_required () const;

        bool is_automatic_event_reading_mode   () const;

        double get_automatic_event_reading_delay () const;

        void set_automatic_event_reading_mode    (const bool test_ = true);

        void set_automatic_event_reading_delay   (const unsigned int delay_ = 1);

        // Detector & style config files
        const std::string & get_detector_config_file () const;

        void set_detector_config_file (const std::string & config_file_);

        const std::string & get_style_config_file () const;

        void set_style_config_file (const std::string & config_file_);

        const std::string & get_cut_config_file () const;

        void set_cut_config_file (const std::string & config_file_);

        // View options
        double get_scaling_factor () const;

        bool is_2d_display_on_left () const;

        void set_2d_display_on_left (const bool display_left_ = true);

        // Event record options
        const std::map<button_signals_type, bool> & get_options_dictionnary  () const;

        std::map<button_signals_type, bool> & grab_options_dictionnary ();

        bool get_option_flag (const button_signals_type signal_) const;

        void set_option_flag (const button_signals_type signal_,
                              const bool flag_);

        // Retrieve 'library loader' config:
        const std::vector<std::string> & get_libraries () const;

      private:

        datatools::logger::priority _logging_priority_;

        bool         _preload_;

        double       _scaling_factor_;

        bool         _automatic_event_reading_;
        unsigned int _automatic_event_reading_delay_;

        std::string  _detector_config_file_;
        std::string  _style_config_file_;
        std::string  _cut_config_file_;

        bool _2d_display_on_left_;

        std::map<button_signals_type, bool> _options_dictionnary_;

        std::vector<std::string> _input_files_;

        std::vector<std::string> _libraries_;

        friend class utils::singleton<options_manager>;

        options_manager  ();
        ~options_manager ();
        options_manager            (const options_manager&);
        options_manager& operator= (const options_manager&);

      };


    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_VIEW_DISPLAY_3D_H

// end of display_3d.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
