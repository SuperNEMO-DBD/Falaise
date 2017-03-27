/* options_manager.cc
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

// Ourselves
#include <falaise/snemo/view/options_manager.h>

// Standard libraries
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party
// Bayeux/datatools:
#include <datatools/exception.h>
// Falaise:
#include <falaise/resource.h>

namespace snemo {

  namespace visualization {

    namespace view {

      datatools::logger::priority options_manager::get_logging_priority() const
      {
        return _logging_priority_;
      }

      void options_manager::set_logging_priority(datatools::logger::priority p_)
      {
        _logging_priority_ = p_;
        return;
      }

      options_manager::options_manager()
      {
        this->set_defaults();
        return;
      }

      options_manager::~options_manager()
      {
        reset();
        return;
      }

      void options_manager::reset()
      {
        this->set_defaults();
        return;
      }

      void options_manager::set_defaults()
      {
        _logging_priority_ = datatools::logger::PRIO_WARNING;

        _scaling_factor_                 = 0.75;
        _preload_                        = false;

        _automatic_event_reading_        = false;
        _automatic_event_reading_delay_  = 1;

        _detector_config_file_           = "";
        _style_config_file_              = "";
        _cut_config_file_                = "";

        _2d_display_on_left_             = true;

        _options_dictionnary_.clear();
        _input_files_.clear();
        _libraries_.clear();

        set_default_options();

        return;
      }

      void options_manager::set_default_options()
      {
        _options_dictionnary_[FULL_2D_VIEW]                  = false;
        _options_dictionnary_[FOCUS_ROI]                     = false;
        _options_dictionnary_[SHOW_GG_CIRCLE]                = true;
        _options_dictionnary_[SHOW_GG_TIME_GRADIENT]         = true;
        _options_dictionnary_[SHOW_MC_VERTEX]                = true;
        _options_dictionnary_[SHOW_MC_TRACKS]                = true;
        _options_dictionnary_[SHOW_MC_HITS]                  = true;
        _options_dictionnary_[SHOW_MC_CALORIMETER_HITS]      = true;
        _options_dictionnary_[SHOW_MC_TRACKER_HITS]          = true;
        _options_dictionnary_[SHOW_CALIBRATED_HITS]          = true;
        _options_dictionnary_[SHOW_CALIBRATED_INFO]          = true;
        _options_dictionnary_[SHOW_TRACKER_CLUSTERED_HITS]   = true;
        _options_dictionnary_[SHOW_TRACKER_CLUSTERED_BOX]    = false;
        _options_dictionnary_[SHOW_TRACKER_CLUSTERED_CIRCLE] = true;
        _options_dictionnary_[SHOW_TRACKER_TRAJECTORIES]     = true;
        _options_dictionnary_[SHOW_RECALIBRATED_TRACKER_HITS]= true;
        _options_dictionnary_[SHOW_PARTICLE_TRACKS]          = true;
        _options_dictionnary_[DUMP_INTO_TOOLTIP]             = true;
        _options_dictionnary_[DUMP_INTO_TERMINAL]            = false;
        _options_dictionnary_[DUMP_INTO_WINDOW]              = false;
        return;
      }

      void options_manager::define_browser_options(boost::program_options::options_description & browser_options_,
                                                   uint32_t flags_)
      {
        // Shortcut for Boost/program_options namespace :
        // See: http://www.boost.org/doc/libs/1_46_1/doc/html/program_options.html
        namespace po = boost::program_options;
        po::options_description_easy_init easy_init = browser_options_.add_options();
        bool parse_logging         = true;
        bool parse_load_dll        = true;
        bool parse_detector_config_file = true;
        // Inhibition of the parsinf for specific options:
        if (flags_ & browser_opt_no_logging) parse_logging = false;
        if (flags_ & browser_opt_no_dll_load) parse_load_dll = false;
        if (flags_ & browser_opt_no_detector_config) parse_detector_config_file = false;

        if (parse_logging) {
          easy_init("logging-priority,P",
                    po::value<std::string>()
                    ->default_value("notice")
                    ->value_name("level"),
                    "set the logging priority threshold");
        }

        easy_init("scale,S",
                  po::value<double>(&_scaling_factor_)
                  ->value_name("value"),
                  "scale factor for computer screen (height/width)");

        easy_init("auto-reading-delay,a",
                  po::value<unsigned int>()
                  ->value_name("value"),
                  "automatic event reading delay in seconds");

        if (parse_detector_config_file) {
          easy_init("detector-config-file",
                    po::value<std::string>(&_detector_config_file_)
                    ->value_name("file"),
                    "set the path to the detector configuration file");
        }

        easy_init("style-config-file",
                  po::value<std::string>(&_style_config_file_)
                  ->value_name("file"),
                  "set the path to the style configuration file");

        easy_init("cut-config-file",
                  po::value<std::string>(&_cut_config_file_)
                  ->value_name("file"),
                  "set the path to the cut configuration file");

        easy_init("preload",
                  po::value<bool>(&_preload_)
                  ->zero_tokens()
                  ->default_value(false),
                  "enable the load in memory of Boost archive files (working only with pure 'bxg4_production' output)");

        easy_init("input-data-files,I",
                  po::value<std::vector<std::string> >(&_input_files_)
                  ->value_name("file"),
                  "set an input data file(s)");

        if (parse_load_dll) {
          easy_init("load-dll,l",
                    po::value<std::vector<std::string> >(&_libraries_)
                    ->value_name("name"),
                    "set a DLL to be loaded.");
        }

        return;
      }

      void options_manager::define_view_options(boost::program_options::options_description & viewer_options_,
                                                uint32_t /* flags_ */)
      {
        namespace po = boost::program_options;

        viewer_options_.add_options()
          ("2d-display",
           po::value<std::string>()
           ->default_value("left")
           ->value_name("position"),
           "set 2D display position")

          ("full-2d-view",
           po::value<bool>(&_options_dictionnary_[FULL_2D_VIEW])
           ->zero_tokens(),
           "add a new tab with top/front/side 2D view in one frame")

          ("focus-on-roi",
           po::value<bool>(&_options_dictionnary_[FOCUS_ROI])
           ->zero_tokens(),
           "focus views on the 'region-of-interest'")

          ("show-simulated-vertex",
           po::value<bool>(&_options_dictionnary_[SHOW_MC_VERTEX])
           ->value_name("flag"),
           "show simulated vertex")

          ("show-simulated-tracks",
           po::value<bool>(&_options_dictionnary_[SHOW_MC_TRACKS])
           ->value_name("flag"),
           "show simulated tracks")

          ("show-simulated-hits",
           po::value<bool>(&_options_dictionnary_[SHOW_MC_HITS])
           ->value_name("flag"),
           "show simulated hits")

          ("show-calibrated-hits",
           po::value<bool>(&_options_dictionnary_[SHOW_CALIBRATED_HITS])
           ->value_name("flag"),
           "show calibrated hits")

          ("show-calibrated-info",
           po::value<bool>(&_options_dictionnary_[SHOW_CALIBRATED_INFO])
           ->value_name("flag"),
           "show calibrated info")

          ("show-tracker-clustered-hits",
           po::value<bool>(&_options_dictionnary_[SHOW_TRACKER_CLUSTERED_HITS])
           ->value_name("flag"),
           "show tracker clustered hits")

          ("show-tracker-trajectories",
           po::value<bool>(&_options_dictionnary_[SHOW_TRACKER_TRAJECTORIES])
           ->value_name("flag"),
           "show tracker trajectories")

          ("show-particle-tracks",
           po::value<bool>(&_options_dictionnary_[SHOW_PARTICLE_TRACKS])
           ->value_name("flag"),
           "show particle tracks")

          ; // end of 'view options' description
        return;
      }

      // parse command line
      bool options_manager::parse_command_line(int argc_, char** argv_)
      {
        // Shortcut for Boost/program_options namespace :
        // See: http://www.boost.org/doc/libs/1_46_1/doc/html/program_options.html
        namespace po = boost::program_options;

        // Describe command line switches (-X, --XXX...) :
        po::options_description general_opts("Allowed options");

        general_opts.add_options()
          ("help,h", "produce help message")

          ("logging-priority,P",
           po::value<std::string>()
           ->value_name("level")
           ->default_value("notice"),
           "set the logging priority threshold")

          ("scale,s",
           po::value<double>(&_scaling_factor_)
           ->value_name("value"),
           "scale factor for computer screen (height/width)")

          ("auto-reading-delay,a",
           po::value<unsigned int>()
           ->value_name("value"),
           "automatic event reading delay in seconds")

          ("detector-config-file",
           po::value<std::string>(&_detector_config_file_)
           ->value_name("file"),
           "set the path to the detector configuration file")

          ("style-config-file",
           po::value<std::string>(&_style_config_file_)
           ->value_name("file"),
           "set the path to the style configuration file")

          ("cut-config-file",
           po::value<std::string>(&_cut_config_file_)
           ->value_name("file"),
           "set the path to the cut configuration file")

          ("preload",
           po::value<bool>(&_preload_)
           ->zero_tokens()
           ->default_value(false),
           "enable the load in memory of Boost archive files (working only with pure 'bxg4_production' output)")

          ("input-files,i",
           po::value<std::vector<std::string> >(&_input_files_)
           ->value_name("file"),
           "set an input file(s)")

          ("load-dll,l",
           po::value<std::vector<std::string> >(&_libraries_)
           ->value_name("name"),
           "set a DLL to be loaded.")

          ; // end of 'options' description

        // Describe command line switches (-X, --XXX...) :
        po::options_description view_opts("View options (arg is either true or false)");

        view_opts.add_options()
          ("2d-display",
           po::value<std::string>()
           ->default_value("left")
           ->value_name("side"),
           "set 2D display position")

          ("full-2d-view",
           po::value<bool>(&_options_dictionnary_[FULL_2D_VIEW])
           ->zero_tokens(),
           "add a new tab with top/front/side 2D view in one frame")

          ("focus-on-roi",
           po::value<bool>(&_options_dictionnary_[FOCUS_ROI])
           ->zero_tokens(),
           "focus views on the 'region-of-interest'")

          ("show-simulated-vertex",
           po::value<bool>(&_options_dictionnary_[SHOW_MC_VERTEX])
           ->value_name("flag"),
           "show simulated vertex")

          ("show-simulated-tracks",
           po::value<bool>(&_options_dictionnary_[SHOW_MC_TRACKS])
           ->value_name("flag"),
           "show simulated tracks")

          ("show-simulated-hits",
           po::value<bool>(&_options_dictionnary_[SHOW_MC_HITS])
           ->value_name("flag"),
           "show simulated hits")

          ("show-calibrated-hits",
           po::value<bool>(&_options_dictionnary_[SHOW_CALIBRATED_HITS])
           ->value_name("flag"),
           "show calibrated hits")

          ("show-calibrated-info",
           po::value<bool>(&_options_dictionnary_[SHOW_CALIBRATED_INFO])
           ->value_name("flag"),
           "show calibrated info")

          ("show-tracker-clustered-hits",
           po::value<bool>(&_options_dictionnary_[SHOW_TRACKER_CLUSTERED_HITS])
           ->value_name("flag"),
           "show tracker clustered hits")

          ("show-tracker-trajectories",
           po::value<bool>(&_options_dictionnary_[SHOW_TRACKER_TRAJECTORIES])
           ->value_name("flag"),
           "show tracker trajectories")

          ("show-particle-tracks",
           po::value<bool>(&_options_dictionnary_[SHOW_PARTICLE_TRACKS])
           ->value_name("flag"),
           "show particle tracks")

          ; // end of 'view options' description

        // Collection option descriptions into one
        po::options_description all_opts;
        all_opts.add(general_opts).add(view_opts);

        // Describe command line arguments :
        po::positional_options_description args;
        args.add("input-files", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc_, argv_)
                  .options(all_opts)
                  .allow_unregistered()
                  .positional(args)
                  .run(), vm);
        po::notify(vm);

        /*** begin of opts/args parsing ***/

        // Fetch the opts/args :
        if (vm.count("help")) {
          std::cout << "flvisualize -- ";
          std::cout << "A generic SuperNEMO event browser program" << std::endl;
          std::cout << std::endl;
          std::cout << "Usage : " << std::endl;
          std::cout << std::endl;
          std::cout << "  flvisualize [OPTIONS] [ARGUMENTS] "
                    << std::endl;
          std::cout << std::endl;
          std::cout << general_opts << std::endl;
          std::cout << std::endl;
          std::cout << view_opts << std::endl;
          std::cout << std::endl;
          this->print_examples(std::cout,
                               "flvisualize",
                               "Examples : ");
          std::cout << std::endl;
          return false;
        }

        if (vm.count("auto-reading-delay")) {
          _automatic_event_reading_ = true;
          if (_automatic_event_reading_delay_ == 0)
            _automatic_event_reading_delay_ = 1;
        }

        if (vm.count("logging-priority")) {
          const std::string logging_label = vm["logging-priority"].as<std::string>();
          _logging_priority_ = datatools::logger::get_priority(logging_label);
          DT_THROW_IF(_logging_priority_ == datatools::logger::PRIO_UNDEFINED,
                      std::logic_error,
                      "Invalid logging priority label '" << logging_label << "' !");
        }

        if (vm.count("2d-display")) {
          const std::string position = vm["2d-display"].as<std::string>();
          if (position == "left") {
            _2d_display_on_left_ = true;
          } else if (position == "right") {
            _2d_display_on_left_ = false;
          } else {
            DT_THROW_IF(true, std::logic_error, "2D display value must be 'left' or 'right'");
          }
        }

        /*** end of opts/args parsing ***/
        return true;
      }

      int options_manager::apply_options(const boost::program_options::variables_map & vm_)
      {

        // Fetch the opts/args :
        if (vm_.count("help")) {
          // std::cout << "flvisualize -- ";
          // std::cout << "A generic SuperNEMO event browser program" << std::endl;
          // std::cout << std::endl;
          // std::cout << "Usage : " << std::endl;
          // std::cout << std::endl;
          // std::cout << "  flvisualize [OPTIONS] [ARGUMENTS] "
          //           << std::endl;
          // std::cout << std::endl;
          // std::cout << general_opts << std::endl;
          // std::cout << std::endl;
          // std::cout << view_opts << std::endl;
          // std::cout << std::endl;
          // this->print_examples(std::cout,
          //                      "flvisualize",
          //                      "Examples : ");
          // std::cout << std::endl;
          return -1;
        }

        if (vm_.count("auto-reading-delay")) {
          _automatic_event_reading_ = true;
          if (_automatic_event_reading_delay_ == 0)
            _automatic_event_reading_delay_ = 1;
        }

        if (vm_.count("logging-priority")) {
          const std::string logging_label = vm_["logging-priority"].as<std::string>();
          _logging_priority_ = datatools::logger::get_priority(logging_label);
          DT_THROW_IF(_logging_priority_ == datatools::logger::PRIO_UNDEFINED,
                      std::logic_error,
                      "Invalid logging priority label '" << logging_label << "' !");
        }

        if (vm_.count("2d-display")) {
          const std::string position = vm_["2d-display"].as<std::string>();
          if (position == "left") {
            _2d_display_on_left_ = true;
          } else if (position == "right") {
            _2d_display_on_left_ = false;
          } else {
            DT_THROW_IF(true, std::logic_error, "2D display value must be 'left' or 'right'");
          }
        }

        return 0;
      }

      void options_manager::print_examples(std::ostream      & out_,
                                           const std::string & name_,
                                           const std::string & title_) const
      {
        if (!title_.empty()) {
          out_ << title_ << std::endl;
        }
        out_ << std::endl;
        out_ << " 1) Using default configuration :" << std::endl;
        out_ << "    " << name_ << " \\" << std::endl;
        out_ << "      --input-file <simulation/reconstruction file>";
        out_ << std::endl;
        out_ << " 2) Auto reading mode:" << std::endl;
        out_ << "    " << name_ << " \\" << std::endl;
        out_ << "      --verbose \\" << std::endl;
        out_ << "      --auto-reading-delay 2 \\" << std::endl;
        out_ << "      --input-file <simulation/reconstruction file>";
        out_ << std::endl;
        out_ << " 3) Using special experimental setup with variant profile:" << std::endl;
        out_ << "    " << name_ << " \\" << std::endl;
        out_ << "      --verbose \\" << std::endl;
        out_ << "      --experiment-setup \"urn:snemo:demonstrator:setup:1.0\" \\" << std::endl;
        out_ << "      --variant-profile \"variant.profile\" \\" << std::endl;
        out_ << "      --input-file <simulation/reconstruction file>";
        out_ << std::endl;
        out_ << " 4) Visualizing BiPo3 detector :" << std::endl;
        out_ << "    " << name_ << " \\" << std::endl;
        out_ << "      --detector-config-file "
             << "$(flquery --resourcedir)"
          /* << falaise::get_resource_dir()*/
             << "/config/bipo3/geometry/2.0/manager.conf \\" << std::endl;
        out_ << "      --style-config-file "
             << "$(flquery --resourcedir)"
          /* << falaise::get_resource_dir() */
             << "/modules/EventBrowser/styles/bipo3_default.sty" << std::endl;
        out_ << std::endl;
        out_ << std::endl;
        out_ << " See README for other running examples" << std::endl;
        out_ << std::endl;

        return;
      }

      void options_manager::set_input_files(const std::vector<std::string> & input_files_)
      {
        _input_files_ = input_files_;
        return;
      }

      void options_manager::add_input_file(const std::string & input_file_)
      {
        _input_files_.push_back(input_file_);
        return;
      }

      const std::vector<std::string> & options_manager::get_input_files() const
      {
        return _input_files_;
      }

      double options_manager::get_scaling_factor() const
      {
        return _scaling_factor_;
      }

      bool options_manager::is_preload_required() const
      {
        return _preload_;
      }

      bool options_manager::is_automatic_event_reading_mode() const
      {
        return _automatic_event_reading_;
      }

      double options_manager::get_automatic_event_reading_delay() const
      {
        return _automatic_event_reading_delay_;
      }

      void options_manager::set_automatic_event_reading_mode(const bool test_)
      {
        _automatic_event_reading_ = test_;
        return;
      }

      void options_manager::set_automatic_event_reading_delay(const unsigned int delay_)
      {
        _automatic_event_reading_delay_ = delay_;
        return;
      }

      const std::string & options_manager::get_style_config_file() const
      {
        return _style_config_file_;
      }

      void options_manager::set_style_config_file(const std::string & config_file_)
      {
        _style_config_file_ = config_file_;
        return;
      }

      const std::string & options_manager::get_cut_config_file() const
      {
        return _cut_config_file_;
      }

      void options_manager::set_cut_config_file(const std::string & config_file_)
      {
        _cut_config_file_ = config_file_;
        return;
      }

      const std::string & options_manager::get_detector_config_file() const
      {
        return _detector_config_file_;
      }

      void options_manager::set_detector_config_file(const std::string & config_file_)
      {
        _detector_config_file_ = config_file_;
        return;
      }

      bool options_manager::is_2d_display_on_left() const
      {
        return _2d_display_on_left_;
      }

      void options_manager::set_2d_display_on_left(const bool display_left_)
      {
        _2d_display_on_left_ = display_left_;
      }

      const std::map<button_signals_type, bool> & options_manager::get_options_dictionnary() const
      {
        return _options_dictionnary_;
      }

      std::map<button_signals_type, bool> & options_manager::grab_options_dictionnary()
      {
        return _options_dictionnary_;
      }

      bool options_manager::get_option_flag(const button_signals_type signal_) const
      {
        std::map<button_signals_type, bool>::const_iterator found
          = _options_dictionnary_.find(signal_);

        if (found == _options_dictionnary_.end())
          return false;

        return _options_dictionnary_.at(signal_);
      }

      void options_manager::set_option_flag(const button_signals_type signal_,
                                            const bool flag_)
      {
        _options_dictionnary_[signal_] = flag_;
      }

      const std::vector<std::string> & options_manager::get_libraries() const
      {
        return _libraries_;
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo

// end of options_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
