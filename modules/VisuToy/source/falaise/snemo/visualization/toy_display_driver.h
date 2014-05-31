/// \file falaise/snemo/visualization/toy_display_driver.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2014-04-24
 *
 * Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   A simple event visualization driver.
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_TOY_DISPLAY_DRIVER_H
#define FALAISE_SNEMO_VISUALIZATION_TOY_DISPLAY_DRIVER_H 1

// Standard library:
#include <list>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/geomtools:
#include <geomtools/placement.h>
#include <geomtools/smart_id_locator.h>
// - Falaise :
#include <falaise/snemo/geometry/gg_locator.h>

class Gnuplot;

namespace geomtools {
  class mapping;
}

namespace datatools {
  class things;
  class temp_file;
}

namespace snemo {

  namespace geometry {
    class manager;
  }

  namespace visualization {

    struct toy_display_driver
    {

    public:

      enum display_code_type {
        DISPLAY_OK        =  0,
        DISPLAY_INHIBITED = -1,
        DISPLAY_ERROR     =  1
      };

      static const std::string & snemo_tc_label();

      static const std::string & snemo_demo_label();

      void set_default_view_parameters();

      void set_geom_manager(const geomtools::manager & geom_manager_);

      void set_module_number(int module_number_);

      void set_interactive(bool interactive_);

      bool is_interactive() const;

      bool is_batch() const;

      bool is_inhibited() const;

      void set_inhibited(bool);

      toy_display_driver(datatools::logger::priority priority_ = datatools::logger::PRIO_WARNING);

      toy_display_driver(geomtools::manager & geom_manager_,
                             int module_number_,
                             datatools::logger::priority priority_ = datatools::logger::PRIO_WARNING);

      ~toy_display_driver();

      bool is_initialized() const;

      void initialize();

      void reset();

      display_code_type display(datatools::things & event_record_);

      void prepare_draw_module_();

      void generate_data_files_();

      void build_draw_command_();

      void display_();

      void reset_display_geom_params_();

      int tui();

      void print_menu(std::ostream & out_ = std::clog) const;

      int interpret_command_line();

      void cmd_dump(datatools::things & event_record_);

      void print_status(std::ostream & out_ = std::clog) const;

      void set_current_event_id(const std::string & event_id_);

      void purge_temp_files(unsigned int n_ = 0);

    protected:

      void _set_defaults();

      void _ui_init();

      void _ui_terminate();

    public :

      datatools::logger::priority logging_priority;

      bool        initialized_;
      bool        inhibited_;
      bool        interactive_;
      std::string interactive_mode_;
      const geomtools::manager * gmgr;
      const geomtools::mapping * mapping;
      uint32_t module_type;
      double   default_range_x;
      double   default_range_y;
      double   default_range_z;
      double   zoom_range_xyz;
      int                  module_number_;
      geomtools::placement module_placement_;
      snemo::geometry::gg_locator module_gg_locator_;

      // Only in 'snemo::demonstrator' setup :
      uint32_t calorimeter_block_type;
      geomtools::smart_id_locator calo_block_locator;

      uint32_t tracker_volume_type;
      geomtools::smart_id_locator tracker_volume_locator;

      // Only in 'snemo::demonstrator' setup :
      uint32_t xcalo_block_type;
      geomtools::smart_id_locator xcalo_block_locator;

      // Only in 'snemo::demonstrator' setup :
      uint32_t gveto_block_type;
      geomtools::smart_id_locator gveto_block_locator;

      uint32_t drift_cell_type;
      geomtools::smart_id_locator drift_cell_locator;

      // Only in 'snemo::demonstrator' setup :
      uint32_t source_pad_type;
      geomtools::smart_id_locator source_pad_locator;

      // Only in 'snemo::tracker_commissioning' setup :
      uint32_t tc_mu_trigger_block_type;
      geomtools::smart_id_locator tc_mu_trigger_block_locator;

      boost::scoped_ptr<datatools::temp_file> temp_geom_file;
      boost::scoped_ptr<datatools::temp_file> temp_draw_file;
      datatools::things    * current_event_record_;

      datatools::things & get_current_event_record_();
      const datatools::things & get_current_event_record_() const;

      std::map<std::string,int> geom_data;
      std::map<std::string,int> data;
      std::string plot_command_;
      std::string last_plot_command_;
      boost::scoped_ptr<Gnuplot> gnuplot_driver_;

      Gnuplot & get_gnuplot_driver();

      std::string current_event_id;
      std::string view_mode;
      std::string print_file;
      bool wxt;
      bool display_key;
      bool display_title;

      bool display_Geo;
      bool display_Geo_Src;
      bool display_Geo_Calo;
      bool display_Geo_Xcalo;
      bool display_Geo_Gveto;

      bool display_SD;
      bool display_SD_hits;
      bool display_SD_hits_boxes;
      bool display_SD_gg_MAP;
      bool display_SD_visu_track;
      bool display_SD_visu_track_points;
      bool display_SD_vertex;

      bool display_CD;
      bool display_CD_tracker_prompt;
      bool display_CD_tracker_delayed;
      bool display_CD_tracker_noisy;

      bool display_TCD;
      bool display_TCD_def_clusters;
      bool display_TCD_def_unclustered_hits;

      bool display_TJD;
      bool display_TJD_def_trajectories;
      bool display_TJD_def_orphans_hits;

      double zoom_factor_2d;
      double zoom_center_x;
      double zoom_center_y;
      double zoom_center_z;
      double range_x;
      double range_y;
      double range_z;

      std::string ui_prompt;
      std::string ui_history_filename;
      std::string ui_command_line_;
      std::string ui_last_hist;
      std::list<std::string> tmp_filenames;

    };

  }  // end of namespace visualization

}  // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_TOY_DISPLAY_DRIVER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
