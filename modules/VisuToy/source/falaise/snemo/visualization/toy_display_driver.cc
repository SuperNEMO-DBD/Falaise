/// \file falaise/snemo/visualization/toy_display_driver.cc

// Ourselves:
#include <falaise/snemo/visualization/toy_display_driver.h>

// Standard library:
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
// - GSL:
#include <gsl/gsl_cdf.h>
// - Bayeux/datatools:
#include <datatools/things.h>
#include <datatools/temporary_files.h>
#include <datatools/version_id.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/color.h>
#include <geomtools/line_3d.h>
#include <geomtools/helix_3d.h>
#include <geomtools/polyline_3d.h>
// - Bayeux/mctools:
#include <mctools/utils.h>
#include <mctools/simulated_data.h>
// - Falaise :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/gg_track_utils.h>
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>

// This project:
#include <falaise/snemo/visualization/visu_toy_config.h>

#if FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
#include <readline/readline.h>
#include <readline/history.h>
#endif // FALAISEVISUTOYPLUGIN_WITH_READLINE == 1

namespace snemo {

  namespace visualization {

    // static
    const std::string & toy_display_driver::snemo_tc_label()
    {
      static const std::string _label("snemo::tracker_commissioning");
      return _label;
    }

    // static
    const std::string & toy_display_driver::snemo_demo_label()
    {
      static const std::string _label("snemo::demonstrator");
      return _label;
    }

    bool toy_display_driver::is_inhibited() const
    {
      return inhibited_;
    }

    void toy_display_driver::set_inhibited(bool i_)
    {
      inhibited_ = i_;
      return;
    }

    bool toy_display_driver::is_initialized() const
    {
      return initialized_;
    }

    void toy_display_driver::set_interactive(bool isInteractive)
    {
      interactive_ = isInteractive;
    }

    bool toy_display_driver::is_interactive() const
    {
      return interactive_;
    }

    bool toy_display_driver::is_batch() const
    {
      return ! is_interactive();
    }

    void toy_display_driver::set_module_number(int module_number)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
      module_number_ = module_number;
      return;
    }

    void toy_display_driver::set_geom_manager(const geomtools::manager & geom_manager_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
      gmgr = &geom_manager_;
      return;
    }

    void toy_display_driver::set_current_event_id(const std::string & event_id_)
    {
      current_event_id = event_id_;
      return;
    }

    void toy_display_driver::set_default_view_parameters()
    {
      view_mode = "xy";
      default_range_x = 2000 * CLHEP::mm;
      default_range_y = 6000 * CLHEP::mm;
      default_range_z = 5000 * CLHEP::mm;
      zoom_range_xyz  = 4000 * CLHEP::mm;
      wxt                          = false;
      display_key                  = true;
      display_title                = true;
      display_Geo                  = true;
      display_Geo_Src              = true;
      display_Geo_Calo             = true;
      display_Geo_Xcalo            = true;
      display_Geo_Gveto            = true;
      display_SD                   = true;
      display_SD_hits              = true;
      display_SD_hits_boxes        = true;
      display_SD_gg_MAP            = false;
      display_SD_visu_track        = false;
      display_SD_visu_track_points = false;
      display_SD_vertex            = true;
      display_CD                   = true;
      display_CD_tracker_prompt    = true;
      display_CD_tracker_delayed   = true;
      display_CD_tracker_noisy     = true;
      display_TCD                      = true;
      display_TCD_def_clusters         = true;
      display_TCD_def_unclustered_hits = true;
      display_TTD                      = true;
      display_TTD_def_trajectories     = true;
      display_TTD_def_orphans_hits     = true;
      display_PTD                      = true;
      reset_display_geom_params_();

      return;
    }

    void toy_display_driver::initialize()
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
      DT_LOG_DEBUG(logging_priority, "Entering...");
      mapping = &(gmgr->get_mapping());

      const geomtools::id_mgr::categories_by_name_col_type & categories
        = gmgr->get_id_mgr().categories_by_name();
      DT_LOG_DEBUG(logging_priority, "categories ok...");

      module_type = categories.find("module")->second.get_type();
      DT_LOG_DEBUG(logging_priority, "module_type=" << module_type);

      const std::string setup_label = gmgr->get_setup_label();

      DT_LOG_NOTICE(logging_priority, "setup_label = '" << setup_label << "'");

      datatools::version_id geom_mgr_setup_vid;
      gmgr->fetch_setup_version_id(geom_mgr_setup_vid);

      if (setup_label == snemo_tc_label()) {
        view_mode       = "yz";
        default_range_x = 5000 * CLHEP::mm;
        default_range_y = 6000 * CLHEP::mm;
        default_range_z = 2000 * CLHEP::mm;
        zoom_range_xyz  = 4000 * CLHEP::mm;
      }

      if (setup_label == snemo_demo_label()) {
        // Calorimeter blocks:
        calorimeter_block_type = categories.find("calorimeter_block")->second.get_type();
        DT_LOG_DEBUG(logging_priority, "calorimeter_block_type=" << calorimeter_block_type);
        calo_block_locator.set_gmap(*mapping);
        calo_block_locator.initialize(calorimeter_block_type);
        DT_LOG_DEBUG(logging_priority, "calorimeter blocks locator is initialized.");

        // X-calorimeter blocks:
        xcalo_block_type = categories.find("xcalo_block")->second.get_type();
        DT_LOG_DEBUG(logging_priority, "xcalo_block_type=" << xcalo_block_type);
        xcalo_block_locator.set_gmap(*mapping);
        xcalo_block_locator.initialize(xcalo_block_type);
        DT_LOG_DEBUG(logging_priority, "xcalo blocks locator is initialized.");

        // Gamma-veto blocks:
        gveto_block_type = categories.find("gveto_block")->second.get_type();
        DT_LOG_DEBUG(logging_priority, "gveto_block_type=" << gveto_block_type);
        gveto_block_locator.set_gmap(*mapping);
        gveto_block_locator.initialize(gveto_block_type);
        DT_LOG_DEBUG(logging_priority, "gveto blocks locator is initialized.");
      }

      if (setup_label == snemo_demo_label() || setup_label == snemo_tc_label() ) {
        {
          // Tracker volume:
          tracker_volume_type = categories.find("tracker_volume")->second.get_type();
          DT_LOG_DEBUG(logging_priority, "tracker_volume_type=" << tracker_volume_type);
          tracker_volume_locator.set_gmap(*mapping);
          tracker_volume_locator.initialize(tracker_volume_type);
          DT_LOG_DEBUG(logging_priority, "tracker volumes locator is initialized.");
        }

        {
          // Drift cells:
          drift_cell_type = categories.find("drift_cell_core")->second.get_type();
          DT_LOG_DEBUG(logging_priority, "drift_cell_type=" << drift_cell_type);
          drift_cell_locator.set_gmap(*mapping);
          drift_cell_locator.initialize(drift_cell_type);
          DT_LOG_DEBUG(logging_priority, "drift cells locator is initialized.");
        }
      }

      if (setup_label == snemo_demo_label()) {
        std::string source_cat = "source_strip";
        if (geom_mgr_setup_vid.has_major() && geom_mgr_setup_vid.get_major() == 1) {
          source_cat = "source_pad";
        }
        // Source pads:
        source_pad_type = categories.find(source_cat)->second.get_type();
        DT_LOG_DEBUG(logging_priority, "source_pad_type=" << source_pad_type);
        source_pad_locator.set_gmap(*mapping);
        source_pad_locator.initialize(source_pad_type);
        DT_LOG_DEBUG(logging_priority, "source pads locator is initialized.");
      }

      if (setup_label == snemo_tc_label()) {
        DT_LOG_DEBUG(logging_priority, "Initializing muon_trigger block stuff...");
        // muon_trigger block:
        tc_mu_trigger_block_type = categories.find("mu_trigger")->second.get_type();
        DT_LOG_DEBUG(logging_priority, "tc_mu_trigger_block_type=" << tc_mu_trigger_block_type);
        tc_mu_trigger_block_locator.set_gmap(*mapping);
        tc_mu_trigger_block_locator.initialize(tc_mu_trigger_block_type);
        DT_LOG_DEBUG(logging_priority, "mu_trigger blocks locator is initialized.");
        if (logging_priority >= datatools::logger::PRIO_DEBUG) {
          tc_mu_trigger_block_locator.dump(std::clog);
        }
      }

      DT_LOG_DEBUG(logging_priority, "Preparing draw module...");

      module_number_ = 0;
      prepare_draw_module_();

      _ui_init();

      initialized_ = true;
      return;
    }

    void toy_display_driver::reset()
    {
      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");
      _ui_terminate();

      if (gnuplot_driver_) {
        if (! wxt) {
          get_gnuplot_driver().cmd("set terminal x11 close; ");
        } else {
          get_gnuplot_driver().cmd("set terminal wxt close; ");
        }
        usleep(1000);
        gnuplot_driver_.reset();
      }

      if (temp_draw_file) {
        temp_draw_file.reset();
      }

      if (temp_geom_file != 0) {
        temp_geom_file.reset();
      }

      // purge_temp_files(0);

      if (calo_block_locator.is_initialized()) calo_block_locator.reset();
      if (xcalo_block_locator.is_initialized()) xcalo_block_locator.reset();
      if (gveto_block_locator.is_initialized()) gveto_block_locator.reset();
      if (drift_cell_locator.is_initialized()) drift_cell_locator.reset();
      if (source_pad_locator.is_initialized()) source_pad_locator.reset();
      if (tracker_volume_locator.is_initialized()) tracker_volume_locator.reset();
      if (tc_mu_trigger_block_locator.is_initialized()) tc_mu_trigger_block_locator.reset();

      module_placement_.invalidate();
      module_number_ = -1;
      mapping = 0;
      gmgr = 0;

      initialized_ = false;
      return;
    }

    void toy_display_driver::_set_defaults()
    {
      inhibited_ = false;
      interactive_ = false;
      interactive_mode_ = "tui";
      logging_priority = datatools::logger::PRIO_FATAL; //priority_;
      gmgr = 0;
      mapping = 0;
      current_event_record_ = 0;
      module_number_ = -1;
      module_type              = geomtools::geom_id::INVALID_TYPE;
      calorimeter_block_type   = geomtools::geom_id::INVALID_TYPE;
      tracker_volume_type      = geomtools::geom_id::INVALID_TYPE;
      gveto_block_type         = geomtools::geom_id::INVALID_TYPE;
      xcalo_block_type         = geomtools::geom_id::INVALID_TYPE;
      drift_cell_type          = geomtools::geom_id::INVALID_TYPE;
      source_pad_type          = geomtools::geom_id::INVALID_TYPE;
      tc_mu_trigger_block_type = geomtools::geom_id::INVALID_TYPE;
      set_default_view_parameters();
      ui_prompt = "flvisutoy> ";
      ui_history_filename = "~/.flvisutoy.history";
      datatools::fetch_path_with_env(ui_history_filename);
      return;
    }


    void toy_display_driver::_ui_init()
    {
#if FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      using_history(); // use readline library
      if (boost::filesystem::exists(ui_history_filename)) {
        int error = read_history(ui_history_filename.c_str());
        if (error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Cannot read history file '" << ui_history_filename << "'!");
        }
      }
#endif // FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      return;
    }

    void toy_display_driver::_ui_terminate()
    {
#if FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Saving history file '" << ui_history_filename << "'...");
      int error = write_history(ui_history_filename.c_str());
      if (error) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Cannot write history file '" << ui_history_filename << "' !");
      }
      history_truncate_file(ui_history_filename.c_str(), 200);
      clear_history();
#endif // FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      return;
    }

    toy_display_driver::toy_display_driver(datatools::logger::priority priority_)
    {
      initialized_ = false;
      gmgr = 0;
      mapping = 0;
      current_event_record_ = 0;
      _set_defaults();
      logging_priority = priority_;
      return;
    }

    toy_display_driver::toy_display_driver(geomtools::manager & geom_manager_,
                                           int module_number_,
                                           datatools::logger::priority priority_)
    {
      initialized_ = false;
      gmgr = 0;
      mapping = 0;
      current_event_record_ = 0;
      _set_defaults();
      logging_priority = priority_;
      set_geom_manager(geom_manager_);
      set_module_number(module_number_);
      initialize();
      return;
    }

    toy_display_driver::~toy_display_driver()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void toy_display_driver::prepare_draw_module_()
    {
      DT_LOG_DEBUG(logging_priority, "Entering...");
      // 2012-11-11 FM : Cannot use /tmp/${USER} yet, needs ensure the directory exists (create it on the fly with Boost):
      //std::string tmp_geom_file_dir = "/tmp/${USER}";
      const std::string tmp_geom_file_dir = "/tmp";
      const std::string tmp_geom_file_prefix = "dsd_g.";
      temp_geom_file.reset(new datatools::temp_file(tmp_geom_file_dir,
                                                    tmp_geom_file_prefix));
      temp_geom_file->set_remove_at_destroy(true);
      temp_geom_file->out().precision(15);

      const std::string setup_label = gmgr->get_setup_label();

      const uint32_t module_number = module_number_;

      int index = 0;
      geom_data.clear();

      {
        temp_geom_file->out() << "#draw module:" << std::endl;
        // Pickup geometry informations about the SuperNEMO module #0:
        geomtools::geom_id module_id(module_type, module_number);
        DT_THROW_IF (! mapping->validate_id(module_id), std::logic_error,
                    "No module with ID " << module_id << " !");
        const geomtools::geom_info & module_geom_info = mapping->get_geom_info(module_id);
        const geomtools::placement & mp = module_geom_info.get_world_placement();
        const geomtools::logical_volume & mlog = module_geom_info.get_logical();
        const geomtools::i_shape_3d & mshape   = mlog.get_shape();
        DT_LOG_DEBUG(logging_priority, "Module geometry info: " << module_geom_info);
        // Draw module #0 volume:
        module_placement_ = mp;
        // geomtools::placement display_mp;
        // module_placement_.relocate(mp, display_mp);
        // display_mp.tree_dump();
        geomtools::gnuplot_draw::draw(temp_geom_file->out(), mp, mshape);
        temp_geom_file->out() << std::endl << std::endl;
        temp_geom_file->out() << "##############################################" << std::endl;
        geom_data["Geom.module"] = index++;
      }

      {
        temp_geom_file->out() << "#draw tracker volumes:" << std::endl;
        for (std::list<const geomtools::geom_info *>::const_iterator i
               = tracker_volume_locator.get_ginfos().begin();
             i != tracker_volume_locator.get_ginfos().end();
             i++) {
          const geomtools::geom_info & gi = **i;
          const geomtools::placement & a_placement = gi.get_world_placement();
          const geomtools::logical_volume & a_log = gi.get_logical();
          const geomtools::i_shape_3d & a_shape = a_log.get_shape();
          DT_LOG_DEBUG(logging_priority, "Tracker volume geometry info: " << gi);
          geomtools::gnuplot_draw::draw(temp_geom_file->out(), a_placement, a_shape);
        }
        temp_geom_file->out() << std::endl << std::endl;
        temp_geom_file->out() << "##############################################" << std::endl;
        geom_data["Geom.tracker_volume"] = index++;
      }

      if (setup_label == snemo_demo_label()) {
        temp_geom_file->out() << "#draw source pads:" << std::endl;
        for (std::list<const geomtools::geom_info *>::const_iterator i
               = source_pad_locator.get_ginfos().begin();
             i != source_pad_locator.get_ginfos().end();
             i++) {
          const geomtools::geom_info & gi = **i;
          // Pickup geometry informations about a specific source pad:
          //const geomtools::geom_info & a_geom_info
          //  = mapping->get_geom_info(gi.get_id());
          const geomtools::placement & a_placement = gi.get_world_placement();
          const geomtools::logical_volume & a_log = gi.get_logical();
          const geomtools::i_shape_3d & a_shape = a_log.get_shape();
          DT_LOG_DEBUG(logging_priority, "Source pad geometry info: " << gi);
          geomtools::gnuplot_draw::draw(temp_geom_file->out(), a_placement, a_shape);
        }
        temp_geom_file->out() << std::endl << std::endl;
        temp_geom_file->out() << "##############################################" << std::endl;
        geom_data["Geom.source"] = index++;
      }

      if (setup_label == snemo_demo_label()) {
        temp_geom_file->out() << "#draw calorimeter blocks:" << std::endl;
        for (std::list<const geomtools::geom_info *>::const_iterator i
               = calo_block_locator.get_ginfos().begin();
             i != calo_block_locator.get_ginfos().end();
             i++) {
          const geomtools::geom_info & gi = **i;
          // Pickup geometry informations about a specific calo block:
          //const geomtools::geom_info & a_geom_info
          //  = mapping->get_geom_info(gi.get_id());
          const geomtools::placement & a_placement = gi.get_world_placement();
          const geomtools::logical_volume & a_log = gi.get_logical();
          const geomtools::i_shape_3d & a_shape = a_log.get_shape();
          DT_LOG_DEBUG(logging_priority, "Calorimeter block geometry info: " << gi);
          geomtools::gnuplot_draw::draw(temp_geom_file->out(), a_placement, a_shape);
        }
        temp_geom_file->out() << std::endl << std::endl;
        temp_geom_file->out() << "##############################################" << std::endl;
        geom_data["Geom.calo_blocks"] = index++;
      }

      if (setup_label == snemo_demo_label()) {
        temp_geom_file->out() << "#draw X-calorimeter blocks:" << std::endl;
        for (std::list<const geomtools::geom_info *>::const_iterator i
               = xcalo_block_locator.get_ginfos().begin();
             i != xcalo_block_locator.get_ginfos().end();
             i++) {
          const geomtools::geom_info & gi = **i;
          // Pickup geometry informations about a specific calo block:
          //const geomtools::geom_info & a_geom_info
          //  = mapping->get_geom_info(gi.get_id());
          const geomtools::placement & a_placement = gi.get_world_placement();
          const geomtools::logical_volume & a_log = gi.get_logical();
          const geomtools::i_shape_3d & a_shape = a_log.get_shape();
          DT_LOG_DEBUG(logging_priority, "X-calorimeter block geometry info: " << gi);
          geomtools::gnuplot_draw::draw(temp_geom_file->out(), a_placement, a_shape);
        }
        temp_geom_file->out() << std::endl << std::endl;
        temp_geom_file->out() << "##############################################" << std::endl;
        geom_data["Geom.xcalo_blocks"] = index++;
      }

      if (setup_label == snemo_demo_label()) {
          temp_geom_file->out() << "#draw Gamma-veto blocks:" << std::endl;
          for (std::list<const geomtools::geom_info *>::const_iterator i
                 = gveto_block_locator.get_ginfos().begin();
               i != gveto_block_locator.get_ginfos().end();
               i++) {
            const geomtools::geom_info & gi = **i;
            // Pickup geometry informations about a specific calo block:
            //const geomtools::geom_info & a_geom_info
            //  = mapping->get_geom_info(gi.get_id());
            const geomtools::placement & a_placement = gi.get_world_placement();
            const geomtools::logical_volume & a_log = gi.get_logical();
            const geomtools::i_shape_3d & a_shape = a_log.get_shape();
            DT_LOG_DEBUG(logging_priority, "Gamma-veto block geometry info: " << gi);
            geomtools::gnuplot_draw::draw(temp_geom_file->out(), a_placement, a_shape);
          }
          temp_geom_file->out() << std::endl << std::endl;
          temp_geom_file->out() << "##############################################" << std::endl;
          geom_data["Geom.gveto_blocks"] = index++;
        }

      if (setup_label == snemo_tc_label()) {
        DT_LOG_DEBUG(logging_priority, "# of muon-trigger blocks ="
                      << tc_mu_trigger_block_locator.get_ginfos().size());

        temp_geom_file->out() << "#draw muon-trigger blocks:" << std::endl;
        for (std::list<const geomtools::geom_info *>::const_iterator i
               = tc_mu_trigger_block_locator.get_ginfos().begin();
             i != tc_mu_trigger_block_locator.get_ginfos().end();
             i++) {
          const geomtools::geom_info & gi = **i;
          const geomtools::placement & a_placement = gi.get_world_placement();
          const geomtools::logical_volume & a_log = gi.get_logical();
          const geomtools::i_shape_3d & a_shape = a_log.get_shape();
          DT_LOG_DEBUG(logging_priority, "muon-trigger block geometry info: " << gi);
          geomtools::gnuplot_draw::draw(temp_geom_file->out(), a_placement, a_shape);
        }
        temp_geom_file->out() << std::endl << std::endl;
        temp_geom_file->out() << "##############################################" << std::endl;
        geom_data["Geom.mu_trigger_blocks"] = index++;
      }

      temp_geom_file->out().close();
      DT_LOG_DEBUG(logging_priority, "Exiting.");
      return;
    }

    datatools::things & toy_display_driver::get_current_event_record_()
    {
      return *current_event_record_;
    }

    const datatools::things & toy_display_driver::get_current_event_record_() const
    {
      return *current_event_record_;
    }

    void toy_display_driver::purge_temp_files(unsigned int n_)
    {
      bool purge_all = false;
      if (n_ == 0) {
        purge_all = true;
      }
      unsigned int count = 0;
      if (tmp_filenames.size() < n_ + 20) return;
      for (std::list<std::string>::iterator i = tmp_filenames.begin();
           i != tmp_filenames.end();
           i++) {
        const std::string & tmp_filename = *i;
        //if (purge_all || tmp_filename[0] != '!') {
        bool purge_it = false;
        bool delete_it = false;
        if (boost::filesystem::exists(tmp_filename)) {
          if (purge_all) {
            delete_it = true;
          }
          std::cerr << "DEVEL: Removing temporary file : '" << tmp_filename << "'" << std::endl;
          if (delete_it) {
            boost::filesystem::remove(tmp_filename);
          }
          purge_it = true;
        }
        if (purge_it) {
          // Remove the filename:
          std::list<std::string>::iterator j = i;
          i = tmp_filenames.erase(j);
        }
        //}
        if (! purge_all) {
          if (count >= n_) {
            std::cerr << "DEVEL: Purged " << count << " temporary files..." << std::endl;
            break;
          }
        }
        count++;
      }
      std::cerr << "DEVEL: List of temporary filenames : " << tmp_filenames.size() << std::endl;
      for (std::list<std::string>::const_iterator i = tmp_filenames.begin();
           i != tmp_filenames.end();
           i++) {
        const std::string & tmp_filename = *i;
        std::cerr << "DEVEL:   Filename = '" << tmp_filename << "'" << std::endl;
      }
      return;
    }

    void toy_display_driver::generate_SD_data_(std::ostream & out_, int & index_)
    {
      // Working value:
      int index = index_;
      const std::string setup_label = gmgr->get_setup_label();
      // Simulated data :
      const std::string & SD_label = snemo::datamodel::data_info::default_simulated_data_label();
      if (get_current_event_record_().has(SD_label)) {
        DT_LOG_DEBUG(logging_priority, "has SIMULATED_DATA_LABEL !");
        DT_THROW_IF (! get_current_event_record_().is_a<mctools::simulated_data>(SD_label),
                     std::logic_error,
                     "Data model has an item named '" << SD_label
                     << "' but which is not a 'simulated_data' instance !");
        mctools::simulated_data & the_sd = get_current_event_record_().grab<mctools::simulated_data>(SD_label);
        DT_LOG_DEBUG(logging_priority, "SIMULATED_DATA grabbed !");

        // Draw the vertex:
        {
          DT_LOG_DEBUG(logging_priority, "draw the vertex !");
          const geomtools::vector_3d & vertex = the_sd.get_vertex();
          if (geomtools::is_valid(vertex)) {
            out_ << "# 'SD.vertex' @ index " << index << ": " << std::endl;
            geomtools::gnuplot_draw::basic_draw_point(out_, vertex);
            data["SD.vertex"] = index++;
            out_ << std::endl << std::endl;
            out_ << "##############################################" << std::endl;
          }
        }

        std::map<geomtools::geom_id, bool> scin_hits;

        // Draw the calorimeter hits:
        if (setup_label == snemo_demo_label()) {
          DT_LOG_DEBUG(logging_priority, "draw the calorimeter simulated hits !");
          const std::string hit_category = "calo";
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            const mctools::simulated_data::hit_handle_collection_type & BSHC
              = the_sd.get_step_hits(hit_category);
            size_t count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++) {
              // check the handle :
              if (! i->has_data()) continue;
              // get a reference to the hit :
              const mctools::base_step_hit & BSH = i->get();
              geomtools::geom_id gid = BSH.get_geom_id();
              if (gid.is_type(calorimeter_block_type) && gid.get_depth() == 5) {
                if (gid.is_any(4)) {
                  gid.set(4, 1);
                }
              }
              const geomtools::vector_3d & pstart = BSH.get_position_start();
              const geomtools::vector_3d & pstop  = BSH.get_position_stop();
              const double dx = std::abs(0.5 *(pstart.x() - pstop.x()));
              const double dy = std::abs(0.5 *(pstart.y() - pstop.y()));
              const double dz = std::abs(0.5 *(pstart.z() - pstop.z()));
              if (count == 0) {
                out_ << "# 'SD." << hit_category << "' step hits @ index " << index << ": " << std::endl;
              }
              geomtools::box cluster_box;
              cluster_box.set_half(dx, dy, dz);
              geomtools::vector_3d pos = 0.5 *(pstart + pstop);
              geomtools::rotation_3d rot;
              geomtools::gnuplot_draw::draw_box(out_, pos, rot, cluster_box);
              scin_hits[gid] = true;
              count++;
            }
            if (count) {
              data["SD." + hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
        }

        // draw the X-calo simulated hits:
        if (setup_label == snemo_demo_label()) {
          DT_LOG_DEBUG(logging_priority, "draw the X-calo simulated hits !");
          const std::string hit_category = "xcalo";
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            const mctools::simulated_data::hit_handle_collection_type & BSHC
              = the_sd.get_step_hits(hit_category);
            size_t count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++) {
              if (! i->has_data()) continue;
              const mctools::base_step_hit & BSH = i->get();
              const geomtools::geom_id & gid = BSH.get_geom_id();

              const geomtools::vector_3d & pstart = BSH.get_position_start();
              const geomtools::vector_3d & pstop  = BSH.get_position_stop();
              const double dx = std::abs(0.5 *(pstart.x() - pstop.x()));
              const double dy = std::abs(0.5 *(pstart.y() - pstop.y()));
              const double dz = std::abs(0.5 *(pstart.z() - pstop.z()));
              if (count == 0) {
                out_ << "# 'SD." << hit_category << "' step hits @ index " << index << ": " << std::endl;
              }
              geomtools::box cluster_box;
              cluster_box.set_half(dx, dy, dz);
              geomtools::vector_3d pos = 0.5 *(pstart + pstop);
              geomtools::rotation_3d rot;
              geomtools::gnuplot_draw::draw_box(out_, pos, rot, cluster_box);
              scin_hits[gid] = true;
              count++;
            }
            if (count) {
              data["SD." + hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
        }

        // draw the Gamma-veto simulated hits:
        if (setup_label == snemo_demo_label()) {
          DT_LOG_DEBUG(logging_priority, "draw the gveto simulated hits !");
          const std::string hit_category = "gveto";
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            const mctools::simulated_data::hit_handle_collection_type & BSHC
              = the_sd.get_step_hits(hit_category);
            size_t count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++) {
              if (! i->has_data()) continue;
              const mctools::base_step_hit & BSH = i->get();
              const geomtools::geom_id & gid = BSH.get_geom_id();

              const geomtools::vector_3d & pstart = BSH.get_position_start();
              const geomtools::vector_3d & pstop  = BSH.get_position_stop();
              const double dx = std::abs(0.5 *(pstart.x() - pstop.x()));
              const double dy = std::abs(0.5 *(pstart.y() - pstop.y()));
              const double dz = std::abs(0.5 *(pstart.z() - pstop.z()));
              if (count == 0) {
                out_ << "# 'SD." << hit_category << "' step hits @ index " << index << ": " << std::endl;
              }
              geomtools::box cluster_box;
              cluster_box.set_half(dx, dy, dz);
              geomtools::vector_3d pos = 0.5 *(pstart + pstop);
              geomtools::rotation_3d rot;
              geomtools::gnuplot_draw::draw_box(out_, pos, rot, cluster_box);
              scin_hits[gid] = true;
              count++;
            }
            if (count) {
              data["SD." + hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
        }

        // draw the muon-trigger simulated hits:
        if (setup_label == snemo_tc_label()) {
          DT_LOG_DEBUG(logging_priority, "draw the muon-trigger simulated hits !");
          const std::string hit_category = "trig";
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            DT_LOG_DEBUG(logging_priority, "there are some muon-trigger simulated hits !");
            out_ << "# 'SD." << hit_category << "' step hits @ index " << index << ": " << std::endl;
            const mctools::simulated_data::hit_handle_collection_type & BSHC
              = the_sd.get_step_hits(hit_category);
            size_t count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++) {
              if (! i->has_data()) continue;
              const mctools::base_step_hit & BSH = i->get();
              const geomtools::geom_id & gid = BSH.get_geom_id();

              const geomtools::vector_3d & pstart = BSH.get_position_start();
              const geomtools::vector_3d & pstop  = BSH.get_position_stop();
              const double dx = std::abs(0.5 *(pstart.x() - pstop.x()));
              const double dy = std::abs(0.5 *(pstart.y() - pstop.y()));
              const double dz = std::abs(0.5 *(pstart.z() - pstop.z()));
              if (count == 0) {
                out_ << "# '" << hit_category << "' step hits @ index " << index << ": " << std::endl;
              }
              geomtools::box cluster_box;
              cluster_box.set_half(dx, dy, dz);
              geomtools::vector_3d pos = 0.5 *(pstart + pstop);
              geomtools::rotation_3d rot;
              geomtools::gnuplot_draw::draw_box(out_, pos, rot, cluster_box);
              scin_hits[gid] = true;
              count++;
            }
            if (count) {
              data["SD." + hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
          DT_LOG_DEBUG(logging_priority, "'snemo::tracker_commissioning' is done !");
        }

        if (scin_hits.size() > 0) {
          DT_LOG_DEBUG(logging_priority, "scin_hits.size = " << scin_hits.size());
          int count = 0;
          for (std::map<geomtools::geom_id, bool>::const_iterator i = scin_hits.begin();
               i != scin_hits.end();
               i++) {
            const geomtools::geom_id & gid = i->first;
            geomtools::geom_id sb_gid = gid;
            if (sb_gid.is_type(calorimeter_block_type) && sb_gid.get_depth() == 5) {
              if (sb_gid.is_any(4)) {
                sb_gid.set(4, 1);
              }
            }
            //std::cerr << "********* DEVEL: " << "scin_hit with GID = " << sb_gid << std::endl;
            const geomtools::geom_info & ginfo = mapping->get_geom_info(sb_gid);
            const geomtools::placement & world_plcmt = ginfo.get_world_placement();
            const geomtools::logical_volume & log = ginfo.get_logical();
            const geomtools::i_shape_3d & shape = log.get_shape();
            if (count == 0) {
              out_ << "# 'SD.scin_hit' @ index " << index << ": " << std::endl;
            }
            if (shape.has_user_draw()) {
              void * user_draw_void_function = shape.get_user_draw();
              geomtools::gnuplot_draw::draw_user_function_type user_draw_function
                = reinterpret_cast<geomtools::gnuplot_draw::draw_user_function_type>(user_draw_void_function);
             (*user_draw_function)(out_,
                                     world_plcmt.get_translation(),
                                     world_plcmt.get_rotation(),
                                     shape,
                                     0);
              out_ << "# test 1 " << std::endl;
              //std::cerr << "********* DEVEL: " << "user_draw_void_function for shape at = " << sb_gid << std::endl;
            } else {
              geomtools::gnuplot_draw::draw(out_, world_plcmt, shape);
              //std::cerr << "********* DEVEL: " << "gnuplot_draw::draw for shape '" << shape.get_shape_name() << "' at = " << sb_gid << std::endl;
              //out_ << "# test 2 " << std::endl;
            }
            count++;
          }
          if (count) {
            data["SD.scin_hit"] = index++;
            out_ << std::endl << std::endl;
            out_ << "##############################################" << std::endl;
          }
          DT_LOG_DEBUG(logging_priority, "'SD.scin_hit' is done !");
        }

        std::map<geomtools::geom_id, geomtools::vector_3d> gg_hits;

        // Draw the tracker simulated hits:
        {
          DT_LOG_DEBUG(logging_priority, "draw the tracker simulated hits !");
          const std::string hit_category = "gg";
          size_t count = 0;
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            DT_LOG_DEBUG(logging_priority, "there are some the tracker '" << hit_category << "' simulated hits !");
            out_ << "# '" << hit_category << "' step hits @ index " << index << ": " << std::endl;
            const mctools::simulated_data::hit_handle_collection_type & BSHC = the_sd.get_step_hits(hit_category);
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++, count++) {
              if (! i->has_data()) continue;
              const mctools::base_step_hit & BSH = i->get();
              DT_LOG_DEBUG(logging_priority, "Hit #" << (i - BSHC.begin()));

              // Draw the Geiger avalanche path:
              geomtools::gnuplot_draw::basic_draw_point(out_, BSH.get_position_start());
              geomtools::gnuplot_draw::basic_draw_point(out_, BSH.get_position_stop());
              out_ << std::endl;

              DT_LOG_DEBUG(logging_priority, "Hit #" << (i - BSHC.begin()) << " avalanche path done.");

              // Draw circle tangential to the track:
              const geomtools::geom_id & drift_cell_gid = BSH.get_geom_id();
              DT_LOG_DEBUG(logging_priority, "Hit #" << (i - BSHC.begin()) << " drift_cell_gid=" << drift_cell_gid);
              const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
              DT_LOG_DEBUG(logging_priority, "Hit #" << (i - BSHC.begin()) << " drift_cell_ginfo=" << drift_cell_ginfo);
              const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
              const geomtools::i_shape_3d & drift_cell_shape = drift_cell_ginfo.get_logical().get_shape();
              DT_LOG_DEBUG(logging_priority, "Hit #" << (i - BSHC.begin()) << " drift_cell_shape=" << drift_cell_shape.get_shape_name());
              // const geomtools::box & drift_cell_box = dynamic_cast<const geomtools::box &>(drift_cell_shape);
              // DT_LOG_DEBUG(logging_priority, "Hit #" << (i - BSHC.begin()) << " drift_cell_box fetched.");

              // const double w = drift_cell_box.get_x();
              // const double d = drift_cell_box.get_y();
              const double drift_radius =(BSH.get_position_start() - BSH.get_position_stop()).mag();
              geomtools::gnuplot_draw::draw_circle(out_,
                                                    BSH.get_position_stop(),
                                                    drift_cell_world_plcmt.get_rotation(),
                                                    drift_radius);
              gg_hits[drift_cell_gid] = BSH.get_position_stop();

              // geomtools::box draw_box(w, d, 1.0 * CLHEP::cm);
              // geomtools::gnuplot_draw::draw_box(out_,
              //                                    BSH.get_position_stop(),
              //                                    drift_cell_world_plcmt.get_rotation(),
              //                                    draw_box);
            }
            if (BSHC.size()) {
              data["SD." + hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
          DT_LOG_DEBUG(logging_priority, "'tracker' hits are done !");
        }

        // std::cerr << "DEVEL: ********** SD.gg_hit size = " << gg_hits.size() << std::endl;
        if (gg_hits.size() > 0) {
          DT_LOG_DEBUG(logging_priority, "gg_hits.size = " << gg_hits.size());
          int count = 0;
          for (std::map<geomtools::geom_id, geomtools::vector_3d>::const_iterator i = gg_hits.begin();
               i != gg_hits.end();
               i++) {
            const geomtools::geom_id & drift_cell_gid = i->first;
            const geomtools::vector_3d & geiger_hit_pos = i->second;
            const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
            const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
            const geomtools::i_shape_3d & drift_cell_shape = drift_cell_ginfo.get_logical().get_shape();
            const geomtools::box & drift_cell_box = dynamic_cast<const geomtools::box &>(drift_cell_shape);
            const double w = drift_cell_box.get_x();
            const double d = drift_cell_box.get_y();
            if (! geomtools::is_valid(geiger_hit_pos)) {
              continue;
            }
            if (count == 0) {
              out_ << "# 'SD.gg_hit' @ index " << index << ": " << std::endl;
            }
            geomtools::box cell_box(w, d, 2.0 * CLHEP::cm);
            geomtools::gnuplot_draw::draw_box(out_,
                                              geiger_hit_pos,
                                              drift_cell_world_plcmt.get_rotation(),
                                              cell_box);
            count++;
          }
          if (count) {
            data["SD.gg_hit"] = index++;
            out_ << std::endl << std::endl;
            out_ << "##############################################" << std::endl;
          }
          DT_LOG_DEBUG(logging_priority, "'SD.gg_hit' is done !");
        }

        // Draw the maximal approach position(MAP) if any is stored in the gg.hits:
        {
          const std::string hit_category = "gg";
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            const mctools::simulated_data::hit_handle_collection_type & BSHC = the_sd.get_step_hits(hit_category);
            size_t count = 0;
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++) {
              if (! i->has_data()) continue;
              const mctools::base_step_hit & BSH = i->get();

              if (BSH.get_auxiliaries().has_key(snemo::datamodel::gg_track::minimum_approach_position_key())) {
                std::vector<double> vMAP;
                BSH.get_auxiliaries().fetch(snemo::datamodel::gg_track::minimum_approach_position_key(),
                                              vMAP);
                geomtools::vector_3d the_MAP(vMAP[0], vMAP[1], vMAP[2]);
                //std::cerr << "********* DEVEL: MAP=" << the_MAP << std::endl;
                if (count == 0) {
                  out_ << "# 'SD." << hit_category << ".MAP' @ index " << index << ": " << std::endl;
                }
                // draw the maximum approach position(MAP) of the true track associated to this gg hit :
                geomtools::gnuplot_draw::basic_draw_point(out_, the_MAP);
                count++;
              } else {
                continue;
              }
            }
            if (count) {
              std::ostringstream data_label;
              data_label << "SD." + hit_category + ".MAP";
              data[data_label.str()] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
        }

        // draw the visu hits:
        {
          const std::string hit_category = "__visu.tracks";
          if (the_sd.has_step_hits(hit_category) && the_sd.get_step_hits(hit_category).size()) {
            std::map<std::string, std::ostringstream *> VP_streams;
            std::map<std::string, size_t>               VP_counters;
            {
              VP_streams["e-"] = new std::ostringstream;
              VP_streams["e-"]->precision(15);
              VP_counters["e-"] = 0;
            }
            {
              VP_streams["e+"] = new std::ostringstream;
              VP_streams["e+"]->precision(15);
              VP_counters["e+"] = 0;
            }
            {
              VP_streams["alpha"] = new std::ostringstream;
              VP_streams["alpha"]->precision(15);
              VP_counters["alpha"] = 0;
            }
            {
              VP_streams["delta_ray_from_alpha"] = new std::ostringstream;
              VP_streams["delta_ray_from_alpha"]->precision(15);
              VP_counters["delta_ray_from_alpha"] = 0;
            }
            {
              VP_streams["gamma"] = new std::ostringstream;
              VP_streams["gamma"]->precision(15);
              VP_counters["gamma"] = 0;
            }
            {
              VP_streams["neutron"] = new std::ostringstream;
              VP_streams["neutron"]->precision(15);
              VP_counters["neutron"] = 0;
            }
            {
              VP_streams["mu-"] = new std::ostringstream;
              VP_streams["mu-"]->precision(15);
              VP_counters["mu-"] = 0;
            }
            {
              VP_streams["mu+"] = new std::ostringstream;
              VP_streams["mu+"]->precision(15);
              VP_counters["mu+"] = 0;
            }
            {
              VP_streams["__other__"] = new std::ostringstream;
              VP_streams["__other__"]->precision(15);
              VP_counters["__other__"] = 0;
            }
            const mctools::simulated_data::hit_handle_collection_type & BSHC = the_sd.get_step_hits(hit_category);
            for (mctools::simulated_data::hit_handle_collection_type::const_iterator i
                   = BSHC.begin();
                 i != BSHC.end();
                 i++) {
              if (! i->has_data()) continue;
              const mctools::base_step_hit & BSH = i->get();
              std::string particle_name = BSH.get_particle_name();
              bool is_delta_ray_from_alpha = BSH.get_auxiliaries().has_flag(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
              if (is_delta_ray_from_alpha) {
                particle_name = "delta_ray_from_alpha";
              }
              // search the stream associated to this kind of particles:
              std::map<std::string, std::ostringstream *>::iterator found = VP_streams.find(particle_name);
              std::ostringstream * pstream = 0;
              std::string effective_particle_name;
              if (found != VP_streams.end()) {
                // if some VP_streams exists for this kind of particle:
                pstream = found->second;
                VP_counters[particle_name]++;
                effective_particle_name = particle_name;
              } else {
                // if not found, use stream for "other" particles:
                pstream = VP_streams["__other__"];
                VP_counters["__other__"]++;
                effective_particle_name = "__other__";
              }
              // draw the segment for this step:
              geomtools::gnuplot_draw::basic_draw_point(*pstream, BSH.get_position_start());
              if (VP_counters[effective_particle_name] == 1) {
                /* a workaround for special nasty 3D grid rendering within gnuplot:
                 * adding this median point along the step breaks the grid rendering.
                 */
                geomtools::vector_3d mid_step
                  = 0.5 *(BSH.get_position_start() + BSH.get_position_stop());
                geomtools::gnuplot_draw::basic_draw_point(*pstream, mid_step);
              }
              geomtools::gnuplot_draw::basic_draw_point(*pstream, BSH.get_position_stop());
              *pstream << std::endl;
            }

            for (std::map<std::string, std::ostringstream *>::iterator i = VP_streams.begin();
                 i != VP_streams.end();
                 i++) {
              const std::string & pname = i->first;
              if (VP_counters[pname] > 0) {
                out_ << "# 'SD." << hit_category
                                       << "' step hits(" << VP_counters[pname]
                                       << ") for " << pname << " particles @ index " << index << ": " << std::endl;
                out_ << i->second->str();
                out_ << std::endl;
                std::ostringstream label;
                label << "SD." << hit_category << "." << pname;
                data[label.str()] = index++;
              }
              delete i->second;
            }
            VP_streams.clear();
            VP_counters.clear();
          }
        }
      }

      index_ = index;
      return;
    }

    void toy_display_driver::generate_CD_data_(std::ostream & out_, int & index_)
    {
      // Working value:
      int index = index_;
      const std::string setup_label = gmgr->get_setup_label();

      const std::string & CD_label = snemo::datamodel::data_info::default_calibrated_data_label();
      if (get_current_event_record_().has(CD_label)) {
        DT_LOG_DEBUG(logging_priority, "has CALIBRATED_DATA_LABEL !");
        DT_THROW_IF (! get_current_event_record_().is_a<snemo::datamodel::calibrated_data>(CD_label),
                     std::logic_error,
                     "Data model has an item named '" << CD_label
                     << "' but which is not a 'simulated_data' instance !");
        snemo::datamodel::calibrated_data & the_cd = get_current_event_record_().grab<snemo::datamodel::calibrated_data>(CD_label);

        // draw the calibrated tracker hits:
        {
          snemo::datamodel::calibrated_data::tracker_hit_collection_type & CTHC = the_cd.calibrated_tracker_hits();
          if (CTHC.size()) {
            // Normal hits :
            {
              const std::string hit_category = "CD.tracker";
              size_t hit_count = 0;
              for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::const_iterator i
                     = CTHC.begin();
                   i != CTHC.end();
                   i++) {
                const snemo::datamodel::calibrated_data::tracker_hit_handle_type & the_handle = *i;
                if (! the_handle.has_data()) continue;
                const snemo::datamodel::calibrated_tracker_hit & CTH = the_handle.get();

                const geomtools::geom_id   & drift_cell_gid = CTH.get_geom_id();
                const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
                const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
                bool do_it = true;
                if (CTH.is_noisy() || CTH.is_delayed()) {
                  do_it = false;
                }
                if (! do_it) {
                  continue;
                }
                if (hit_count == 0) {
                  out_ << "# '" << hit_category << "' hits @ index " << index << ": " << std::endl;
                }
                // draw the longitudinal error :
                {
                  geomtools::vector_3d pos1 = drift_cell_world_plcmt.get_translation();
                  geomtools::vector_3d pos2 = drift_cell_world_plcmt.get_translation();
                  pos1.setZ(CTH.get_z() - CTH.get_sigma_z());
                  pos2.setZ(CTH.get_z() + CTH.get_sigma_z());
                  geomtools::gnuplot_draw::basic_draw_point(out_, pos1);
                  geomtools::gnuplot_draw::basic_draw_point(out_, pos2);
                  out_ << std::endl;
                }

                // draw circles tangential to the track:
                {
                  const geomtools::geom_id & drift_cell_gid = CTH.get_geom_id();
                  const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
                  const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
                  geomtools::vector_3d pos = drift_cell_world_plcmt.get_translation();
                  pos.setZ(CTH.get_z());
                  geomtools::gnuplot_draw::draw_circle(out_,
                                                        pos,
                                                        drift_cell_world_plcmt.get_rotation(),
                                                        CTH.get_r() - CTH.get_sigma_r());
                  geomtools::gnuplot_draw::draw_circle(out_,
                                                        pos,
                                                        drift_cell_world_plcmt.get_rotation(),
                                                        CTH.get_r() + CTH.get_sigma_r());
                }
                ++hit_count;
              }
              if (hit_count > 0) {
                data[hit_category] = index++;
                out_ << std::endl << std::endl;
                out_ << "##############################################" << std::endl;
              }
            } // End of normal hits.

              // Delayed hits :
            {
              const std::string hit_category = "CD.tracker_delayed";
              size_t hit_count = 0;
              for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::const_iterator i
                     = CTHC.begin();
                   i != CTHC.end();
                   i++) {
                const snemo::datamodel::calibrated_data::tracker_hit_handle_type & the_handle = *i;
                if (! the_handle.has_data()) continue;
                const snemo::datamodel::calibrated_tracker_hit & CTH = the_handle.get();

                const geomtools::geom_id   & drift_cell_gid = CTH.get_geom_id();
                const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
                const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
                bool do_it = true;
                if (! CTH.is_delayed()) {
                  do_it = false;
                }
                //DEVEL: if (drand48() > 0.5) do_it = true;
                if (! do_it) {
                  continue;
                }
                if (hit_count == 0) {
                  out_ << "# '" << hit_category << "' hits @ index " << index << ": " << std::endl;
                }
                // draw the longitudinal error :
                {
                  geomtools::vector_3d pos1 = drift_cell_world_plcmt.get_translation();
                  geomtools::vector_3d pos2 = drift_cell_world_plcmt.get_translation();
                  pos1.setZ(CTH.get_z() - CTH.get_sigma_z());
                  pos2.setZ(CTH.get_z() + CTH.get_sigma_z());
                  geomtools::gnuplot_draw::basic_draw_point(out_, pos1);
                  geomtools::gnuplot_draw::basic_draw_point(out_, pos2);
                  out_ << std::endl;
                }

                // draw a circle:
                {
                  const geomtools::geom_id & drift_cell_gid = CTH.get_geom_id();
                  const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
                  const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
                  geomtools::vector_3d pos = drift_cell_world_plcmt.get_translation();
                  pos.setZ(CTH.get_z());
                  const double rmax = 22. * CLHEP::mm;
                  geomtools::gnuplot_draw::draw_circle(out_,
                                                        pos,
                                                        drift_cell_world_plcmt.get_rotation(),
                                                        rmax);
                }
                ++hit_count;
              }
              if (hit_count > 0) {
                data[hit_category] = index++;
                out_ << std::endl << std::endl;
                out_ << "##############################################" << std::endl;
              }
            } // End of delayed hits

              // Noisy hits :
            {
              const std::string hit_category = "CD.tracker_noisy";
              size_t hit_count = 0;
              for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::const_iterator i
                     = CTHC.begin();
                   i != CTHC.end();
                   i++) {
                const snemo::datamodel::calibrated_data::tracker_hit_handle_type & the_handle = *i;
                if (! the_handle.has_data()) continue;
                const snemo::datamodel::calibrated_tracker_hit & CTH = the_handle.get();

                const geomtools::geom_id   & drift_cell_gid = CTH.get_geom_id();
                const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
                const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
                bool do_it = true;
                if (! CTH.is_noisy()) {
                  do_it = false;
                }
                if (! do_it) {
                  continue;
                }
                if (hit_count == 0) {
                  out_ << "# '" << hit_category << "' hits @ index " << index << ": " << std::endl;
                }
                // draw the longitudinal error :
                {
                  geomtools::vector_3d pos1 = drift_cell_world_plcmt.get_translation();
                  geomtools::vector_3d pos2 = drift_cell_world_plcmt.get_translation();
                  pos1.setZ(CTH.get_z() - CTH.get_sigma_z());
                  pos2.setZ(CTH.get_z() + CTH.get_sigma_z());
                  geomtools::gnuplot_draw::basic_draw_point(out_, pos1);
                  geomtools::gnuplot_draw::basic_draw_point(out_, pos2);
                  out_ << std::endl;
                }

                // draw a square:
                {
                  const geomtools::geom_id & drift_cell_gid = CTH.get_geom_id();
                  const geomtools::geom_info & drift_cell_ginfo = drift_cell_locator.get_geom_info(drift_cell_gid);
                  const geomtools::placement & drift_cell_world_plcmt = drift_cell_ginfo.get_world_placement();
                  geomtools::vector_3d pos = drift_cell_world_plcmt.get_translation();
                  pos.setZ(CTH.get_z());
                  const double rmax = 22. * CLHEP::mm;
                  geomtools::gnuplot_draw::draw_rectangle(out_,
                                                           pos,
                                                           drift_cell_world_plcmt.get_rotation(),
                                                           2 * rmax, 2 * rmax);
                }
                ++hit_count;
              }
              if (hit_count > 0) {
                data[hit_category] = index++;
                out_ << std::endl << std::endl;
                out_ << "##############################################" << std::endl;
              }
            } // End of noisy hits

          }
        }

        // Draw the calibrated calorimeter hits:
        if (setup_label == snemo_demo_label()) {
          snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & CCHC = the_cd.calibrated_calorimeter_hits();
          if (CCHC.size()) {
            const std::string hit_category = "CD.calorimeter";
            size_t hit_count = 0;
            for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator i
                   = CCHC.begin();
                 i != CCHC.end();
                 i++) {
              const snemo::datamodel::calibrated_data::calorimeter_hit_handle_type & the_handle = *i;
              if (! the_handle.has_data()) continue;
              const snemo::datamodel::calibrated_calorimeter_hit & CCH = the_handle.get();

              geomtools::geom_id scin_block_gid = CCH.get_geom_id();
              const geomtools::geom_info * scin_block_ginfo = 0;
              if (scin_block_gid.is_type(calorimeter_block_type)) {
                if (scin_block_gid.get_depth() == 5) {
                  if (scin_block_gid.is_any(4)) {
                    scin_block_gid.set(4, 1);
                  }
                }
                scin_block_ginfo = &(calo_block_locator.get_geom_info(scin_block_gid));
              } else if (scin_block_gid.get_type() == xcalo_block_type) {
                scin_block_ginfo = &(xcalo_block_locator.get_geom_info(scin_block_gid));
              } else if (scin_block_gid.get_type() == gveto_block_type) {
                scin_block_ginfo = &(gveto_block_locator.get_geom_info(scin_block_gid));
              } else {
                DT_LOG_WARNING(logging_priority, "Cannot identify calorimeter hit GID = '"
                                <<  scin_block_gid << "' !");
                continue;
              }
              const geomtools::placement & scin_block_world_plcmt = scin_block_ginfo->get_world_placement();
              const geomtools::logical_volume & scin_block_log = scin_block_ginfo->get_logical();
              const geomtools::i_shape_3d & scin_block_shape = scin_block_log.get_shape();
              if (hit_count == 0) {
                out_ << "# '" << hit_category << "' hits @ index " << index << ": " << std::endl;
              }
              if (scin_block_shape.has_user_draw()) {
                void * user_draw_void_function = scin_block_shape.get_user_draw();
                geomtools::gnuplot_draw::draw_user_function_type user_draw_function
                  = reinterpret_cast<geomtools::gnuplot_draw::draw_user_function_type>(user_draw_void_function);
               (*user_draw_function)(out_,
                                       scin_block_world_plcmt.get_translation(),
                                       scin_block_world_plcmt.get_rotation(),
                                       scin_block_shape,
                                       0);
              } else {
                // DT_LOG_WARNING(logging_priority, "Cannot draw calorimeter block @ GID = '"
                //                 <<  scin_block_gid << "' !");
                geomtools::gnuplot_draw::draw(out_, scin_block_world_plcmt, scin_block_shape);
              }
              ++hit_count;
            }
            if (hit_count > 0) {
              data[hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
        }

        // draw the calibrated muon trigger hits:
        if (setup_label == snemo_tc_label()) {
          snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & CCHC = the_cd.calibrated_calorimeter_hits();
          if (CCHC.size()) {
            const std::string hit_category = "CD.muon_trigger";
            size_t hit_count = 0;
            for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator i
                   = CCHC.begin();
                 i != CCHC.end();
                 i++) {
              const snemo::datamodel::calibrated_data::calorimeter_hit_handle_type & the_handle = *i;
              if (! the_handle.has_data()) continue;
              const snemo::datamodel::calibrated_calorimeter_hit & CCH = the_handle.get();

              const geomtools::geom_id   & scin_block_gid = CCH.get_geom_id();
              const geomtools::geom_info * scin_block_ginfo = 0;
              if (scin_block_gid.get_type() == tc_mu_trigger_block_type) {
                scin_block_ginfo = &(tc_mu_trigger_block_locator.get_geom_info(scin_block_gid));
              } else {
                DT_LOG_WARNING(logging_priority, "Cannot identify muon trigger hit GID = '"
                                <<  scin_block_gid << "' !");
                continue;
              }
              const geomtools::placement & scin_block_world_plcmt = scin_block_ginfo->get_world_placement();
              const geomtools::logical_volume & scin_block_log = scin_block_ginfo->get_logical();
              const geomtools::i_shape_3d & scin_block_shape = scin_block_log.get_shape();
              if (hit_count == 0) {
                out_ << "# '" << hit_category << "' hits @ index " << index << ": " << std::endl;
              }
              if (scin_block_shape.has_user_draw()) {
                void * user_draw_void_function = scin_block_shape.get_user_draw();
                geomtools::gnuplot_draw::draw_user_function_type user_draw_function
                  = reinterpret_cast<geomtools::gnuplot_draw::draw_user_function_type>(user_draw_void_function);
               (*user_draw_function)(out_,
                                       scin_block_world_plcmt.get_translation(),
                                       scin_block_world_plcmt.get_rotation(),
                                       scin_block_shape,
                                       0);
              } else {
                // DT_LOG_WARNING(logging_priority, "Cannot draw muon trigger block @ GID = '"
                //                 <<  scin_block_gid << "' !");
                geomtools::gnuplot_draw::draw(out_, scin_block_world_plcmt, scin_block_shape);
              }
              ++hit_count;
            }
            if (hit_count > 0) {
              data[hit_category] = index++;
              out_ << std::endl << std::endl;
              out_ << "##############################################" << std::endl;
            }
          }
        }

      }

      index_ = index;
      return;
    }

    void toy_display_driver::generate_TCD_data_(std::ostream & out_, int & index_)
    {
      // Working value:
      int index = index_;
      const std::string setup_label = gmgr->get_setup_label();
      double cell_diameter = 44.0 * CLHEP::mm;
      const std::string & TCD_label = snemo::datamodel::data_info::default_tracker_clustering_data_label();
      if (get_current_event_record_().has(TCD_label))  {
        DT_LOG_DEBUG(logging_priority, "has TRACKER_CLUSTERING_DATA_LABEL !");
        DT_THROW_IF (! get_current_event_record_().is_a<snemo::datamodel::tracker_clustering_data>(TCD_label),
                     std::logic_error,
                     "Data model has an item named '" << TCD_label
                     << "' but which is not a 'tracker_clustering_data' instance !");
        snemo::datamodel::tracker_clustering_data & the_tcd
          = get_current_event_record_().grab<snemo::datamodel::tracker_clustering_data>(TCD_label);
        if (the_tcd.has_default_solution()) {
          // Color context:
          geomtools::color::context & CC = geomtools::gnuplot_draw::color_context();
          const snemo::datamodel::tracker_clustering_solution & the_tcs
            = the_tcd.get_default_solution();
          size_t cluster_count = 0;
          for (int i = 0; i < (int) the_tcs.get_clusters().size(); ++i) {
            const snemo::datamodel::tracker_cluster & the_cluster
              = the_tcs.get_clusters()[i].get();
            const snemo::datamodel::calibrated_tracker_hit::collection_type & clhits
              = the_cluster.get_hits();
            //int test1 = geomtools::color::COLOR_TEST;
            //geomtools::color::code_type code = geomtools::color:: COLOR_MAGENTA;
            //geomtools::color::COLOR_MAGENTA;

            if (i == 0) CC.set_color_code(geomtools::color::COLOR_MAGENTA);
            if (i == 1) CC.set_color_code(geomtools::color::COLOR_GREEN);
            if (i == 2) CC.set_color_code(geomtools::color::COLOR_BLUE);
            if (i == 3) CC.set_color_code(geomtools::color::COLOR_YELLOW);
            if (i == 4) CC.set_color_code(geomtools::color::COLOR_ORANGE);
            if (i >= 5) CC.set_color_code(geomtools::color::COLOR_RED);

            // Produce tracker cluster rendering data for each hit in the current cluster:
            for (int j = 0; j < (int) clhits.size(); j++) {
              const snemo::datamodel::calibrated_data::tracker_hit_handle_type & hclhit = clhits[j];
              const snemo::datamodel::calibrated_tracker_hit & clhit = hclhit.get();
              double x  = clhit.get_x();
              double y  = clhit.get_y();
              double z  = clhit.get_z();
              double ez = clhit.get_sigma_z();
              if (cluster_count == 0 && j == 0) {
                out_ << "# '" << "TCD.default.clusters" << "' @ index " << index << ": " << std::endl;
              }
              geomtools::vector_3d hit_pos(x, y, z);
              geomtools::rotation_3d identity;
              geomtools::gnuplot_draw::draw_box(out_,
                                                hit_pos,
                                                identity,
                                                cell_diameter,
                                                cell_diameter,
                                                2 * ez);
            }
            ++cluster_count;
          }
          if (cluster_count > 0) {
            data["TCD.default.clusters"] = index++;
            out_ << std::endl << std::endl;
            out_ << "##############################################" << std::endl;
          }

          // Unclustered hits:
          const snemo::datamodel::calibrated_tracker_hit::collection_type & the_uhits
            = the_tcs.get_unclustered_hits();
          for (int i = 0; i < (int) the_uhits.size(); ++i) {
            const snemo::datamodel::calibrated_tracker_hit & uclhit = the_uhits[i].get();
            // Produce tracker unclustered hits rendering data
              double x  = uclhit.get_x();
              double y  = uclhit.get_y();
              double z  = uclhit.get_z();
              double ez = uclhit.get_sigma_z();
              if (i == 0) {
                out_ << "# '" << "TCD.default.unclustered_hits" << "' @ index " << index << ": " << std::endl;
              }
              geomtools::vector_3d hit_pos(x, y, z);
              geomtools::rotation_3d identity;
              geomtools::gnuplot_draw::draw_box(out_,
                                                hit_pos,
                                                identity,
                                                cell_diameter,
                                                cell_diameter,
                                                2 * ez);
          }

          if (the_uhits.size()) {
            data["TCD.default.unclustered_hits"] = index++;
            out_ << std::endl << std::endl;
          }

        } // default solution

      } // has TCD data

      index_ = index;
      return;
    }

    void toy_display_driver::generate_TTD_data_(std::ostream & out_, int & index_)
    {
      // Working value:
      int index = index_;
      const std::string setup_label = gmgr->get_setup_label();

      const std::string & TTD_label = snemo::datamodel::data_info::default_tracker_trajectory_data_label();
      if (get_current_event_record_().has(TTD_label))  {
        DT_LOG_DEBUG(logging_priority, "has TRACKER_TRAJECTORY_DATA_LABEL !");
        DT_THROW_IF (! get_current_event_record_().is_a<snemo::datamodel::tracker_trajectory_data>(TTD_label),
                     std::logic_error,
                     "Data model has an item named '" << TTD_label
                     << "' but which is not a 'tracker_trajectory_data' instance !");
        snemo::datamodel::tracker_trajectory_data & the_ttd
          = get_current_event_record_().grab<snemo::datamodel::tracker_trajectory_data>(TTD_label);
        if (the_ttd.has_default_solution()) {
          // Color context:
          geomtools::color::context & CC = geomtools::gnuplot_draw::color_context();
          const snemo::datamodel::tracker_trajectory_solution & the_tts
            = the_ttd.get_default_solution();
          size_t trajectory_count = 0;
          for (int i = 0; i < (int) the_tts.get_trajectories().size(); ++i) {
            const snemo::datamodel::tracker_trajectory & traj = the_tts.get_trajectories().at(i).get();
            double chi2;
            datatools::invalidate(chi2);
            if (traj.get_auxiliaries().has_key("chi2")) {
              chi2 = traj.get_auxiliaries().fetch_real("chi2");
            }
            int    ndof = -1;
            if (traj.get_auxiliaries().has_key("ndof")) {
              ndof = traj.get_auxiliaries().fetch_integer("ndof");
            }
            double Pvalue;
            datatools::invalidate(Pvalue);
            if (ndof >= 0 && datatools::is_valid(chi2)) {
              Pvalue = 1.0 - gsl_cdf_chisq_P(chi2, ndof);
            }
            const snemo::datamodel::base_trajectory_pattern & pattern = traj.get_pattern();
            const std::string & pattern_id = pattern.get_pattern_id();
            // std::cerr << "DEVEL: ********** Pattern ID='" << pattern_id << "' : chi2=" << chi2 << " ndof=" << ndof << " ==> P-value=" << Pvalue << std::endl;
            double prob_high_threshold = 0.05;
            double prob_low_threshold  = 0.025;
            bool draw_it = true;
            if (datatools::is_valid(Pvalue) && Pvalue < prob_low_threshold) {
              draw_it = false;
            }
            if (draw_it) {
              if (pattern_id == snemo::datamodel::line_trajectory_pattern::pattern_id()) {
                const snemo::datamodel::line_trajectory_pattern & line_pattern
                  = dynamic_cast<const snemo::datamodel::line_trajectory_pattern &>(pattern);
                const geomtools::line_3d & line = line_pattern.get_segment();
                ++trajectory_count;
                CC.set_color_code(geomtools::color::COLOR_YELLOW);
                if (datatools::is_valid(Pvalue) && Pvalue > prob_high_threshold) {
                  CC.set_color_code(geomtools::color::COLOR_MAGENTA);
                }
                geomtools::gnuplot_draw::draw_line(out_, line, true);
              } else if (pattern_id == snemo::datamodel::helix_trajectory_pattern::pattern_id()) {
                const snemo::datamodel::helix_trajectory_pattern & helix_pattern
                  = dynamic_cast<const snemo::datamodel::helix_trajectory_pattern &>(pattern);
                const geomtools::helix_3d & helix = helix_pattern.get_helix();
                ++trajectory_count;
                CC.set_color_code(geomtools::color::COLOR_ORANGE);
                if (datatools::is_valid(Pvalue) && Pvalue > prob_high_threshold) {
                  CC.set_color_code(geomtools::color::COLOR_BLUE);
                }
                geomtools::gnuplot_draw::draw_helix(out_, helix, true);
                // } else if (pattern_id == snemo::datamodel::polyline_trajectory_pattern::pattern_id()) {
                //   const snemo::datamodel::polyline_trajectory_pattern & polyline_pattern
                //      = dynamic_cast<const snemo::datamodel::polyline_trajectory_pattern &>(pattern);
                //   CC.set_color_code(geomtools::color::COLOR_GREEN);
                //   const geomtools::polyline_3d & polyline = polyline_pattern.get_path();
                //   //
              } else {
                // pattern is not supported
              }
            }
          } // trajectory for loop
          if (trajectory_count > 0) {
            data["TTD.default.trajectories"] = index++;
            out_ << std::endl << std::endl;
            out_ << "##############################################" << std::endl;
          }

        } // default solution

      } // has TTD data

      index_ = index;
      return;
    }

    void toy_display_driver::generate_PTD_data_(std::ostream & /* out_ */, int & index_)
    {
      // Working value:
      int index = index_;
      const std::string setup_label = gmgr->get_setup_label();

      // Not implemented yet

      index_ = index;
      return;
    }

    void toy_display_driver::generate_data_files_()
    {
      DT_LOG_DEBUG(logging_priority, "Entering...");

      if (temp_draw_file) {
        temp_draw_file.reset();
      }
      // 2012-11-11 FM: not uses yet
      //std::string tmp_draw_file_dir = "/tmp/${USER}";
      std::string tmp_draw_file_dir = "/tmp";
      std::string tmp_draw_file_prefix = "dsd_h.";
      temp_draw_file.reset(new datatools::temp_file(tmp_draw_file_dir,
                                                    tmp_draw_file_prefix));
      temp_draw_file->set_remove_at_destroy(true);
      // if (interactive_) {
      //   temp_draw_file->set_remove_at_destroy(true);
      // } else {
      //   temp_draw_file->set_remove_at_destroy(false);
      //   // // std::ostringstream oss;
      //   // // oss << "!" << temp_draw_file->get_filename();
      //   // // tmp_filenames.push_back(oss.str());
      //   // tmp_filenames.push_back(temp_draw_file->get_filename());
      // }
      temp_draw_file->out().precision(15);

      const std::string setup_label = gmgr->get_setup_label();
      DT_LOG_DEBUG(logging_priority, "setup_label = '" << setup_label << "'");

      int index = 0;
      data.clear();

      // Simulated data :
      generate_SD_data_(temp_draw_file->out(), index);

      // Calibrated data :
      generate_CD_data_(temp_draw_file->out(), index);

      // Tracker clustering data :
      generate_TCD_data_(temp_draw_file->out(), index);

      // Tracker trajectory data :
      generate_TTD_data_(temp_draw_file->out(), index);

      // Particle track data :
      generate_PTD_data_(temp_draw_file->out(), index);

      temp_draw_file->out() << "# end" << std::endl;

      DT_LOG_DEBUG(logging_priority, "Exiting.");
      return;
    }

    toy_display_driver::display_code_type
    toy_display_driver::display(datatools::things & a_event_record)
    {
      DT_LOG_DEBUG(logging_priority, "Entering...");
      if (is_inhibited()) {
        DT_LOG_DEBUG(logging_priority, "Exiting.");
        return DISPLAY_INHIBITED;
      }

      // purge_temp_files(3);

      try {
        DT_THROW_IF ( ! is_initialized(), std::logic_error, "Not initialized !");

        DT_LOG_DEBUG(logging_priority, "Set the current event record...");
        current_event_record_ = &a_event_record;

        DT_LOG_DEBUG(logging_priority, "Generating plot data file...");
        generate_data_files_();

        DT_LOG_DEBUG(logging_priority, "Building draw command...");
        build_draw_command_();

        DT_LOG_DEBUG(logging_priority, "Displaying event...");
        display_();
        if (is_interactive()) {
          if (interactive_mode_ == "tui") {
            do {
              DT_LOG_DEBUG(logging_priority, "Run TUI interface :");
              int status = tui();
              if (status != 0) {
                break;
              }
              DT_LOG_DEBUG(logging_priority, "plot_command_ = '" << plot_command_  << "'");
              //std::cerr << "DEVEL: ********** toy_display_driver::draw: " << "plot_command_ = '" << plot_command_ << "'" << std::endl;
              if (plot_command_.empty()) {
                build_draw_command_();
              }
              display_();
            } while(true);
          } else {
            DT_LOG_WARNING(logging_priority, "Unsupported interactive mode '"
                            << interactive_mode_ << "'! ");
          }
        } else {
          // std::list<std::string>::iterator i_current_filename = tmp_filenames.begin();
          // std::string token = *i_current_filename;
          // if (token[0] == '!') {
          //   std::cerr << "DEVEL: Untag temp filename '" << token << "'" << std::endl;
          //   *i_current_filename = token.substr(1);
          // }
        }
      }
      catch(std::exception & error) {
        DT_LOG_ERROR(logging_priority, "Display error: " << error.what());
        return DISPLAY_ERROR;
      }

      DT_LOG_DEBUG(logging_priority, "Exiting.");
      return DISPLAY_OK;
    }

    void toy_display_driver::reset_display_geom_params_()
    {
      zoom_factor_2d = 1.0;
      zoom_center_x = 0.0;
      zoom_center_y = 0.0;
      zoom_center_z = 0.0;
      range_x = default_range_x;
      range_y = default_range_y;
      range_z = default_range_z;
      return;
    }


    void toy_display_driver::build_draw_command_()
    {
      DT_LOG_DEBUG(logging_priority, "Entering...");

      // Reset current plot command :
      plot_command_ = "";

      // GNUPLOT commands :
      std::string plot_cmd = "plot";
      std::string uopt = "using 1:2";
      std::string ropt;

      print_file.clear();

      if (std::abs(zoom_factor_2d - 1.0) > 0.1) {
        range_x = zoom_range_xyz;
        range_y = zoom_range_xyz;
        range_z = zoom_range_xyz;
      }
      if (view_mode == "3d") {
        plot_cmd = "splot";
        std::ostringstream uopt_oss;
        uopt_oss << "using(($1)):(($2)):(($3))";
        uopt = uopt_oss.str();
      } else {
        ropt = "[*:*][*:*]";
        plot_cmd = "plot";
        std::ostringstream ropt_oss;
        std::ostringstream uopt_oss;
        uopt_oss << "using ";
        if (view_mode == "xy") {
          ropt_oss << '[' << (zoom_center_x - zoom_factor_2d * 0.5 * range_x)
                   << ':' << (zoom_center_x + zoom_factor_2d * 0.5 * range_x)
                   << ']'
                   << '[' << (zoom_center_y - zoom_factor_2d * 0.5 * range_y)
                   << ':' << (zoom_center_y + zoom_factor_2d * 0.5 * range_y)
                   << ']';
          uopt_oss << "(($1))"
                   << ':'
                   << "(($2))" ;
        } else if (view_mode == "yz")  {
          ropt_oss << '[' << (zoom_center_y - zoom_factor_2d * 0.5 * range_y)
                   << ':' << (zoom_center_y + zoom_factor_2d * 0.5 * range_y)
                   << ']'
                   << '[' << (zoom_center_z - zoom_factor_2d * 0.5 * range_z)
                   << ':' << (zoom_center_z + zoom_factor_2d * 0.5 * range_z)
                   << ']';
          uopt_oss << "(($2))"
                   << ':'
                   << "(($3))" ;
        } else if (view_mode == "xz") {
          ropt_oss << '[' << (zoom_center_x - zoom_factor_2d * 0.5 * range_x)
                   << ':' << (zoom_center_x + zoom_factor_2d * 0.5 * range_x)
                   << ']'
                   << '[' << (zoom_center_z - zoom_factor_2d * 0.5 * range_z)
                   << ':' << (zoom_center_z + zoom_factor_2d * 0.5 * range_z)
                   << ']';
          uopt_oss << "(($1))"
                   << ':'
                   << "(($3))" ;
        }
        ropt = ropt_oss.str();
        uopt = uopt_oss.str();
      }
      std::ostringstream cmdstr;
      bool use_legend = display_key;
      if (view_mode != "3d") {
        cmdstr << "set size ratio -1;  set grid; set clip two; ";
      } else {
        use_legend = false;
        cmdstr << "set size ratio -1;  set view equal xyz; set clip two; ";
        cmdstr << "set xyplane at -2000 ; ";
      }
      if (use_legend) {
        cmdstr << "set key on outside right top vertical width 1 height 1 box;"; // set key graph 1.1, graph 1.1; ";
      } else {
        cmdstr << "set key off;";
      }
      cmdstr << plot_cmd << " " << ropt << " ";

      int count = 0;
      if (display_Geo) {

        if (geom_data.find("Geom.module") != geom_data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                 << geom_data.find("Geom.module")->second << " "
                 << uopt
                 << " notitle with lines lt 0";
          count++;
        }

        if (geom_data.find("Geom.tracker_volume") != geom_data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                 << geom_data.find("Geom.tracker_volume")->second << " "
                 << uopt
                 << " notitle with lines lt 9";
          count++;
        }

        if (display_Geo_Src) {
          if (geom_data.find("Geom.source") != geom_data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                   << geom_data.find("Geom.source")->second << " "
                   << uopt
                   << " notitle with lines lt 2";
            count++;
          }
        }

        if (display_Geo_Calo) {
          if (geom_data.find("Geom.calo_blocks") != geom_data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                   << geom_data.find("Geom.calo_blocks")->second << " "
                   << uopt
                   << " notitle with lines lt 5";
            count++;
          }
        }

        if (display_Geo_Xcalo) {
          if (geom_data.find("Geom.xcalo_blocks") != geom_data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                   << geom_data.find("Geom.xcalo_blocks")->second << " "
                   << uopt
                   << " notitle with lines lt 5";
            count++;
          }
        }

        if (display_Geo_Gveto) {
          if (geom_data.find("Geom.gveto_blocks") != geom_data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                   << geom_data.find("Geom.gveto_blocks")->second << " "
                   << uopt
                   << " notitle with lines lt 5";
            count++;
          }
        }

        if (geom_data.find("Geom.mu_trigger_blocks") != geom_data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'"   << temp_geom_file->get_filename() << "' index "
                 << geom_data.find("Geom.mu_trigger_blocks")->second << " "
                 << uopt
                 << " notitle with lines lt 5";
          count++;
        }

      }

      // plot simulated data :
      if (display_SD) {
        if (display_SD_vertex) {
          if (data.find("SD.vertex") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.vertex")->second << " " << uopt
                   << " title \"Vertex(SD)\"" << ' ' << "with point lt 6 pt 4 ps 1";
            count++;
          }
        } // if (display_SD_vertex)
        if (display_SD_visu_track) {
          bool with_points = display_SD_visu_track_points;
          if (data.find("SD.__visu.tracks.gamma") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.gamma")->second << " "
                   << uopt << " title \"gamma true tracks(SD)\" with lines lt 2";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.gamma")->second << " "
                     << uopt << " notitle with point lt 2 pt 6 ps 0.25";
            }
            count++;
          }
          if (data.find("SD.__visu.tracks.alpha") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.alpha")->second << " "
                   << uopt << " title \"alpha true tracks\" with lines lt 4 lw 2";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.alpha")->second << " "
                     << uopt << " notitle with point lt 4 pt 6 ps 0.25";
            }
            count++;
          }
          if (data.find("SD.__visu.tracks.delta_ray_from_alpha") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.delta_ray_from_alpha")->second << " "
                   << uopt << " title \"alpha delta ray true tracks(SD)\" with lines lt 4 lw 1";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.delta_ray_from_alpha")->second << " "
                     << uopt << " notitle with point lt 4 pt 6 ps 0.25";
            }
            count++;
          }
          if (data.find("SD.__visu.tracks.e-") != data.end()) {
            DT_LOG_TRACE(logging_priority, "Hits: SD.__visu.tracks.e-");
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.e-")->second << " "
                   << uopt << " title \"e- true tracks(SD)\" with lines lt 3";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.e-")->second << " "
                     << uopt << " notitle with point lt 3 pt 6 ps 0.25";
            }
            count++;
          }
          if (data.find("SD.__visu.tracks.e+") != data.end()) {
            DT_LOG_TRACE(logging_priority, "Hits: SD.__visu.tracks.e+");
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.e+")->second << " "
                   << uopt << " title \"e+ true tracks(SD)\" with lines lt 1";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.e+")->second << " "
                     << uopt << " notitle with point lt 1 pt 6 ps 0.25";
            }
            count++;
          }
          if (data.find("SD.__visu.tracks.neutron") != data.end()) {
            DT_LOG_TRACE(logging_priority, "Hits: SD.__visu.tracks.neutron");
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.neutron")->second << " "
                   << uopt << " title \"neutron true tracks(SD)\" with lines lt 1";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.neutron")->second << " "
                     << uopt << " notitle with point lt 2 pt 6 ps 0.25";
            }
            count++;
          }
          if (data.find("SD.__visu.tracks.mu+") != data.end()) {
            DT_LOG_TRACE(logging_priority, "Hits: SD.__visu.tracks.mu+");
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.mu+")->second << " "
                   << uopt << " title \"mu+ true tracks(SD)\" with lines lt 1";
            if (with_points)
              {
                cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                       << data.find("SD.__visu.tracks.mu+")->second << " "
                       << uopt << " notitle with point lt 1 pt 6 ps 0.25";
              }
            count++;
          }
          if (data.find("SD.__visu.tracks.mu-") != data.end()) {
            DT_LOG_TRACE(logging_priority, "Hits: SD.__visu.tracks.mu-");
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.mu-")->second << " "
                   << uopt << " title \"mu- true tracks(SD)\" with lines lt 1";
            if (with_points)
              {
                cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                       << data.find("SD.__visu.tracks.mu-")->second << " "
                       << uopt << " notitle with point lt 3 pt 6 ps 0.25";
              }
            count++;
          }
          if (data.find("SD.__visu.tracks.__other__") != data.end()) {
            DT_LOG_TRACE(logging_priority, "Hits: SD.__visu.tracks.__other__");
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.__visu.tracks.__other__")->second << " "
                   << uopt << " title \"Other true tracks(SD)\" with lines lt 8";
            if (with_points) {
              cmdstr << ", '" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.__visu.tracks.__other__")->second << " "
                     << uopt << " notitle with point lt 8 pt 6 ps 0.25";
            }
            count++;
          }
        } // if (display_SD_visu_track)

        if (display_SD_hits) {
          if (data.find("SD.calo") != data.end()) {
            //cmdstr << ", '" << temp_draw_file->get_filename()  << "' index " << data.find("calorimeter")->second << " "
            //   << uopt << " title \"calorimeter hits\" with point lt 1 ps 0.5 pt 6";
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.calo")->second << " "
                   << uopt << " notitle with lines lt 1 lw 2";
            count++;
          }

          if (data.find("SD.xcalo") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.xcalo")->second << " "
                   << uopt << " notitle with lines lt 1 lw 2";
            count++;
          }

          if (data.find("SD.gveto") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.gveto")->second << " "
                   << uopt << " notitle with lines lt 1 lw 2";
            count++;
          }

          if (data.find("SD.trig") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.trig")->second << " "
                   << uopt << " notitle with lines lt 1 lw 2";
            count++;
          }

          if (display_SD_hits_boxes) {
            if (data.find("SD.scin_hit") != data.end()) {
              if (count) cmdstr << ", ";
              cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.scin_hit")->second << " "
                     << uopt << " notitle with lines lt 4 lw 1";
              count++;
            }
          }

          if (data.find("SD.gg") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.gg")->second << " "
                   << uopt << " title \"Geiger hits (SD)\" with lines lt 8 lw 1";
            count++;
          }

          if (display_SD_hits_boxes) {
            if (data.find("SD.gg_hit") != data.end()) {
              if (count) cmdstr << ", ";
              cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                     << data.find("SD.gg_hit")->second << " "
                     << uopt << " notitle with lines lt 8 lw 1";
              count++;
            }
          }

        } // if (display_SD_hits)

        if (display_SD_gg_MAP) {
          if (data.find("SD.gg.MAP") != data.end()) {
            if (count) cmdstr << ", ";
            cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                   << data.find("SD.gg.MAP")->second << " "
                   << uopt << " title \"Geiger MAP (SD)\" with point lt 4 pt 4 ps 0.25";
            count++;
          }
        } // if (display_SD_gg_MAP)
      }

      if (display_CD) {
        // plot calibrated data :
        if (display_CD_tracker_prompt && data.find("CD.tracker") != data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("CD.tracker")->second << " "
                 << uopt << " title \"Tracker hits(CD)\" with lines lt 1 lw 1";
          count++;
        }

        if (display_CD_tracker_delayed && data.find("CD.tracker_delayed") != data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("CD.tracker_delayed")->second << " "
                 << uopt << " title \"Tracker delayed hits (CD)\" with lines lt 4 lw 1";
          count++;
        }

        if (display_CD_tracker_noisy && data.find("CD.tracker_noisy") != data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("CD.tracker_noisy")->second << " "
                 << uopt << " title \"Tracker noisy hits (CD)\" with lines lt 7 lw 1";
          count++;
        }

        if (data.find("CD.calorimeter") != data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("CD.calorimeter")->second << " "
                 << uopt << " title \"Calorimeter hits (CD)\" with lines lt 3 lw 1";
          count++;
        }

        if (data.find("CD.muon_trigger") != data.end()) {
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("CD.muon_trigger")->second << " "
                 << uopt << " title \"Trigger hits (CD)\" with lines lt 3 lw 1";
          count++;
        }
      } // if (display_CD)

      if (display_TCD) {
        // Plot tracker clustering data :
        // Clusters:
        if (display_TCD_def_clusters && data.find("TCD.default.clusters") != data.end()) {
          std::string uopt2 = uopt + ":4";
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("TCD.default.clusters")->second << " "
                 << uopt2 << " title \"Tracker clusters (TCD)\" with lines linecolor variable lw 1";
          count++;
        }
        // Unclustered hits:
        if (display_TCD_def_unclustered_hits && data.find("TCD.default.unclustered_hits") != data.end()) {
          std::string uopt2 = uopt + "";
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("TCD.default.unclustered_hits")->second << " "
                 << uopt2 << " title \"Tracker uncluster hits (TCD)\" with lines lt -1 lw 1";
          count++;
        }

      } // if (display_TCD)

      if (display_TTD) {
        // Plot tracker trajectory data :
        // Trajectories:
        if (display_TTD_def_trajectories && data.find("TTD.default.trajectories") != data.end()) {
          std::string uopt2 = uopt + ":4";
          // std::string uopt2 = uopt;
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("TTD.default.trajectories")->second << " "
                 << uopt2 << " title \"Tracker trajectories (TTD)\" with lines linecolor variable lw 1";
          count++;
        }
        // Orphans hits:
        if (display_TTD_def_orphans_hits && data.find("TTD.default.orphans_hits") != data.end()) {
          std::string uopt2 = uopt + "";
          if (count) cmdstr << ", ";
          cmdstr << "'" << temp_draw_file->get_filename()  << "' index "
                 << data.find("TTD.default.orphans_hits")->second << " "
                 << uopt2 << " title \"Tracker orphans hits (TTD)\" with lines lt -1 lw 1";
          count++;
        }

      } // if (display_TTD)

      if (display_PTD) {
      } // if (display_PTD)


      // if (!interactive_) {
      //   cmdstr << "; "
      //          << " ! rm -f " << temp_geom_file->get_filename() << " ; ";
      // }

      // memorize this plot command:
      plot_command_ = cmdstr.str();

      DT_LOG_DEBUG(logging_priority, "Gnuplot command = '" << plot_command_ << "'");
      return;
    }

    int toy_display_driver::tui()
    {
      std::string line;
#if FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      char * readline_line = 0;
      readline_line = readline(ui_prompt.c_str()); // use readline library
      if (readline_line != 0) {
        line = readline_line; // use readline library
        if (! line.empty()) {
          if (ui_last_hist != line) {
            add_history(readline_line); // use readline/history library
            ui_last_hist = line;
          }
        }
        free(readline_line);
        readline_line = 0;
      }
#else // FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      std::cerr << ui_prompt << std::flush;
      std::getline(std::cin, line);
      if (! std::cin || std::cin.eof()) {
        return 1;
      }
#endif // FALAISEVISUTOYPLUGIN_WITH_READLINE == 1
      if (line.empty()) {
        return 0;
      }
      ui_command_line_ = line;
      int status = interpret_command_line();
      if (status != 0) {
        return status;
      }
      return 0;
    }

    int toy_display_driver::interpret_command_line()
    {
      last_plot_command_ = plot_command_;
      plot_command_.clear();
      print_file.clear();

      std::istringstream iss(ui_command_line_);
      std::string cmd;
      iss >> cmd;
      if (cmd.empty()) {
        return 0;
      } else if (cmd == "q" || cmd == "quit") {
        return 1;
      } else if (cmd == "menu" || cmd == "help") {
        print_menu();
      } else if (cmd == "print") {
        std::string filename;
        iss >> filename;
        if (! filename.empty()) {
          print_file = filename;
          plot_command_ = last_plot_command_;
        } else {
          std::cerr << "Missing filename for command '" << cmd << "' !" << std::endl;
        }
      } else if (cmd == "3d") {
        view_mode = cmd;
      } else if (cmd == "xy") {
        view_mode = cmd;
      } else if (cmd == "yz") {
        view_mode = cmd;
      } else if (cmd == "xz") {
        view_mode = cmd;
      } else if (cmd == "reset") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          reset_display_geom_params_();
        }
      } else if (cmd == "nozoom") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          zoom_factor_2d = 1.0;
        }
      } else if (cmd == "zoom") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          std::string arg;
          iss >> arg;
          if (! arg.empty()) {
            if (arg == "+") {
              zoom_factor_2d /= 2.0;
            } else if (arg == "-") {
              zoom_factor_2d *= 2.0;
            } else if (arg == "++") {
              zoom_factor_2d /= 10.0;
            } else if (arg == "--") {
              zoom_factor_2d *= 10.0;
            } else {
              std::istringstream iss(arg);
              double zf;
              iss >> zf;
              if (! iss) {
                std::cerr << "Invalid format for zoom factor '" << arg << "' for command '" << cmd << "' !" << std::endl;
              } else {
                if (zf <= 0.0) {
                  std::cerr << "Invalid value for zoom factor '" << arg << "' for command '" << cmd << "' !" << std::endl;
                } else {
                  zoom_factor_2d =(1. / zf);
                }
              }
            }
          }
        }
      } else if (cmd == "nomove") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          zoom_center_x = 0.0;
          zoom_center_y = 0.0;
          zoom_center_z = 0.0;
        }
      } else if (cmd == "move") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          double unit = CLHEP::mm;
          double a, b;
          iss >> a >> b;
          if (! iss) {
            std::cerr << "Invalid argument for command '" << cmd << "' !" << std::endl;
          } else {
            std::string unit_str;
            iss >> unit_str;
            if (! unit_str.empty()) {
              unit = datatools::units::get_length_unit_from(unit_str);
            }
          }
          a *= unit;
          b *= unit;
          if (view_mode == "xy") {
            zoom_center_x = a;
            zoom_center_y = b;
          }
          if (view_mode == "yz") {
            zoom_center_y = a;
            zoom_center_z = b;
          }
          if (view_mode == "xz") {
            zoom_center_x = a;
            zoom_center_z = b;
          }
        }
      } else if (cmd == "skip") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          double unit = CLHEP::mm;
          double a, b;
          iss >> a >> b;
          if (! iss) {
            std::cerr << "Invalid argument for command '" << cmd << "' !" << std::endl;
          } else {
            std::string unit_str;
            iss >> unit_str;
            if (! unit_str.empty()) {
              unit = datatools::units::get_length_unit_from(unit_str);
            }
          }
          a *= unit;
          b *= unit;
          if (view_mode == "xy") {
            zoom_center_x += a;
            zoom_center_y += b;
          }
          if (view_mode == "yz") {
            zoom_center_y += a;
            zoom_center_z += b;
          }
          if (view_mode == "xz") {
            zoom_center_x += a;
            zoom_center_z += b;
          }
        }
      } else if (cmd == ">" || cmd == "right") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          double unit = CLHEP::mm;
          double a = 100.0;
          a *= unit;
          if (view_mode == "xy") {
            zoom_center_x += a;
          }
          if (view_mode == "yz") {
            zoom_center_y += a;
          }
          if (view_mode == "xz") {
            zoom_center_x += a;
          }
        }
      } else if (cmd == "<" || cmd == "left") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          double unit = CLHEP::mm;
          double a = 100.0;
          a *= unit;
          if (view_mode == "xy") {
            zoom_center_x -= a;
          }
          if (view_mode == "yz") {
            zoom_center_y -= a;
          }
          if (view_mode == "xz") {
            zoom_center_x -= a;
          }
        }
      } else if (cmd == "^" || cmd == "up") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          double unit = CLHEP::mm;
          double a = 100.0;
          a *= unit;
          if (view_mode == "xy") {
            zoom_center_y += a;
          }
          if (view_mode == "yz") {
            zoom_center_z += a;
          }
          if (view_mode == "xz") {
            zoom_center_z += a;
          }
        }
      } else if (cmd == "v" || cmd == "down") {
        if (view_mode == "3d") {
          std::cerr << "Invalid command '" << cmd << "' in 3D mode !" << std::endl;
        } else {
          double unit = CLHEP::mm;
          double a = 100.0;
          a *= unit;
          if (view_mode == "xy") {
            zoom_center_y -= a;
          }
          if (view_mode == "yz") {
            zoom_center_z -= a;
          }
          if (view_mode == "xz") {
            zoom_center_z -= a;
          }
        }
      } else if (cmd == "Geo") {
        display_Geo = ! display_Geo;
      } else if (cmd == "Geo.Src") {
        display_Geo_Src = ! display_Geo_Src;
      } else if (cmd == "Geo.Xcalo") {
        display_Geo_Xcalo = ! display_Geo_Xcalo;
      } else if (cmd == "Geo.Calo") {
        display_Geo_Calo = ! display_Geo_Calo;
      } else if (cmd == "Geo.Gveto") {
        display_Geo_Gveto = ! display_Geo_Gveto;
      } else if (cmd == "SD") {
        display_SD = ! display_SD;
      } else if (cmd == "SD.Hits") {
        display_SD_hits = ! display_SD_hits;
      } else if (cmd == "SD.Hits.Boxes") {
        display_SD_hits_boxes = ! display_SD_hits_boxes;
      } else if (cmd == "SD.Hits.Geiger.MAP") {
        display_SD_gg_MAP = ! display_SD_gg_MAP;
      } else if (cmd == "SD.Tracks") {
        display_SD_visu_track = ! display_SD_visu_track;
      } else if (cmd == "SD.Tracks.Points") {
        display_SD_visu_track_points = ! display_SD_visu_track_points;
      } else if (cmd == "SD.Vertex") {
        display_SD_vertex = ! display_SD_vertex;
      } else if (cmd == "CD") {
        display_CD = ! display_CD;
      } else if (cmd == "TCD") {
        display_TCD = ! display_TCD;
      } else if (cmd == "TCD.Def.Clusters") {
        display_TCD_def_clusters = ! display_TCD_def_clusters;
      } else if (cmd == "TCD.Def.Unclustered") {
        display_TCD_def_unclustered_hits = ! display_TCD_def_unclustered_hits;
      } else if (cmd == "TTD") {
        display_TTD = ! display_TTD;
      } else if (cmd == "TTD.Def.Trajectories") {
        display_TTD_def_trajectories = ! display_TTD_def_trajectories;
      } else if (cmd == "TTD.Def.Orphans") {
        display_TTD_def_orphans_hits = ! display_TTD_def_orphans_hits;
      } else if (cmd == "PTD") {
        display_PTD = ! display_PTD;
      } else if (cmd == "key") {
        display_key = ! display_key;
      } else if (cmd == "title") {
        display_title = ! display_title;
      } else if (cmd == "debug") {
        logging_priority = datatools::logger::PRIO_DEBUG;
      } else if (cmd == "nodebug") {
        logging_priority = datatools::logger::PRIO_WARNING;
      } else if (cmd == "dump") {
        cmd_dump(get_current_event_record_());
      } else if (cmd == "display") {
        // nothing special
      } else if (cmd == "status") {
        print_status();
      } else if (cmd == "wxt") {
        if (! wxt) {
          get_gnuplot_driver().cmd("set terminal x11 close; ");
        } else {
          get_gnuplot_driver().cmd("set terminal wxt close; ");
        }
        if (is_interactive()) {
          get_gnuplot_driver().showonscreen(); // window output
          usleep(1000);
        }
        wxt = ! wxt;
      } else if (cmd == "abort") {
        set_inhibited(true);
        return 1;
      } else {
        std::cerr << "Ignoring command: " << cmd << std::endl;
      }
      DT_LOG_DEBUG(logging_priority, "Exiting.");
      return 0;
    }

    Gnuplot & toy_display_driver::get_gnuplot_driver()
    {
      if (! gnuplot_driver_) {
        gnuplot_driver_.reset(new Gnuplot("lines"));
      }
      return *gnuplot_driver_;
    }

    void toy_display_driver::display_()
    {
      bool print_it = false;
      if (! print_file.empty()) {
        print_it = true;
      }
      if (display_title) {
        std::ostringstream title_ss;
        title_ss << "SuperNEMO event display (Gnuplot toy version)";
        if (! current_event_id.empty()) {
          title_ss << " -- " << current_event_id;
        }
        get_gnuplot_driver().set_title(title_ss.str());
      } else {
        get_gnuplot_driver().cmd("unset title");
      }
      std::ostringstream full_cmd;
      if (view_mode == "xy") {
        get_gnuplot_driver().set_xlabel("x(mm)");
        get_gnuplot_driver().set_ylabel("y(mm)");
      } else if (view_mode == "yz") {
        get_gnuplot_driver().set_xlabel("y(mm)");
        get_gnuplot_driver().set_ylabel("z(mm)");
      } else if (view_mode == "xz") {
        get_gnuplot_driver().set_xlabel("x(mm)");
        get_gnuplot_driver().set_ylabel("z(mm)");
      } else {
        // 3D
        get_gnuplot_driver().set_xlabel("x(mm)");
        get_gnuplot_driver().set_ylabel("y(mm)");
        get_gnuplot_driver().set_zlabel("z(mm)");
      }
      if (print_it) {
        datatools::fetch_path_with_env(print_file);
        std::ostringstream print_cmd;
        print_cmd << "set terminal ";
        std::string extension = boost::filesystem::extension(print_file);
        std::clog << "Extension is '" << extension << "'" << std::endl;
        if (extension == ".fig") {
          print_cmd << "fig color landscape big metric solid ";
        } else if (extension == ".eps") {
          print_cmd << "postscript eps color";
        } else if (extension == ".jpeg" || extension == ".jpg") {
          print_cmd << "jpeg interlace medium enhanced";
        } else if (extension == ".png") {
          print_cmd << "png interlace medium enhanced";
        }
        print_cmd << "; ";
        print_cmd << "set output '" << print_file << "' ;";
        DT_LOG_DEBUG(logging_priority, "Print command = '" << print_cmd.str() << "' ");
        full_cmd << print_cmd.str();
      } else  {
        if (wxt) {
          full_cmd << "set terminal wxt "
                   << " size 800,600 "
                   << " font \"Arial,10\" "
                   << " enhanced "
                   << " title \"SuperNEMO Toy Display\" "
                   << " solid "
                   << " persist "
                   << " ; ";
        } else {
          full_cmd << "set terminal x11 "
                   << " title \"SuperNEMO Toy Display\" "
                   << " enhanced "
                   << " font \"arial,10\" "
                   << " linewidth 1 "
                   << " persist "
                   << " size 800,600 "
                   << " ; ";
        }
      }
      DT_LOG_DEBUG(logging_priority, "Plot command = '" << plot_command_ << "'");
      full_cmd << plot_command_ << "; ";
      if (print_it) {
        std::ostringstream print_cmd;
        print_cmd << "set output; ";
        // if (wxt)
        //   {
        //  print_cmd << "set terminal wxt; ";
        //   }
        // else
        //   {
        //  print_cmd << "set terminal x11; ";
        //   }
        get_gnuplot_driver().cmd(print_cmd.str());
        full_cmd << print_cmd.str();
      }
      DT_LOG_DEBUG(logging_priority, "Full command = '" << full_cmd.str() << "'");
      get_gnuplot_driver().cmd(full_cmd.str());

      if (! print_it) {
        if (is_interactive()) {
          get_gnuplot_driver().showonscreen(); // window output
          //geomtools::gnuplot_drawer::wait_for_key();
          usleep(200);
        }
      }
      print_file.clear();
      return;
    }

    void toy_display_driver::print_menu(std::ostream & out_) const
    {
      if (logging_priority >= datatools::logger::PRIO_DEBUG) {
        out_ << "View mode        = " << view_mode << std::endl;
        out_ << "Zoom factor(2D) = " << zoom_factor_2d << std::endl;
      }

      out_ << std::endl << "Allowed commands :" << std::endl;
      out_ << "  [Enter]                 do nothing" << std::endl;
      out_ << "  quit                    quit this event" << std::endl;
      out_ << "  3d                      use 3D display mode" << std::endl;
      out_ << "  xy                      use XY 2D display mode" << std::endl;
      out_ << "  yz                      use YZ 2D display mode" << std::endl;
      out_ << "  xz                      use XZ 2D display mode" << std::endl;
      out_ << "  Geo                     toggle the display of the geometry setup" << std::endl;
      out_ << "    Geo.Src               toggle the display of the source foils" << std::endl;
      out_ << "    Geo.Calo              toggle the display of the main calo blocks" << std::endl;
      out_ << "    Geo.Xcalo             toggle the display of the X-calo blocks" << std::endl;
      out_ << "    Geo.Gveto             toggle the display of the gamma-veto blocks" << std::endl;
      out_ << "  SD                      toggle the display of simulated data" << std::endl;
      out_ << "    SD.Vertex             toggle the display of simulated vertex" << std::endl;
      out_ << "    SD.Hits               toggle the display of true hits" << std::endl;
      out_ << "      SD.Hits.Boxes       toggle the display of bounding boxes around true hits " << std::endl;
      out_ << "      SD.Hits.Geiger.MAP  toggle the display of MAP for Geiger true hits" << std::endl;
      out_ << "    SD.Tracks             toggle the display of true tracks" << std::endl;
      out_ << "      SD.Tracks.Points    toggle the display of true tracks with points" << std::endl;
      out_ << "  CD                      toggle the display of calibrated data" << std::endl;
      out_ << "  TCD                     toggle the display of tracker clustering data" << std::endl;
      out_ << "    TCD.Def.Clusters      toggle the display of the clusters(default solution)" << std::endl;
      out_ << "    TCD.Def.Unclustered   toggle the display of the unclustered hits(default solution)" << std::endl;
      out_ << "  TTD                     toggle the display of tracker trajectory data" << std::endl;
      out_ << "    TTD.Def.Trajectories  toggle the display of the trajectories(default solution)" << std::endl;
      out_ << "    TTD.Def.Orphans       toggle the display of the orphan hits(default solution)" << std::endl;
      out_ << "  PTD                     toggle the display of particle track data" << std::endl;
      out_ << "  2D modes only :" << std::endl;
      out_ << "    zoom +                zoom in by a relative factor 2" << std::endl;
      out_ << "    zoom ++               zoom in by a relative factor 10" << std::endl;
      out_ << "    zoom -                zoom out by a relative factor 2" << std::endl;
      out_ << "    zoom --               zoom out by a relative factor 10" << std::endl;
      out_ << "    zoom <value>          zoom at zooming factor <value>" << std::endl;
      out_ << "    nozoom                no zooming factor" << std::endl;
      out_ << "    move <x> <y> [unit]   move at position <x> <y>(XY view)" << std::endl;
      out_ << "    move <y> <z> [unit]   move at position <y> <z>(YZ view)" << std::endl;
      out_ << "    move <x> <z> [unit]   move at position <x> <z>(XZ view)" << std::endl;
      out_ << "    nomove                no move " << std::endl;
      out_ << "    skip <dx> <dz> [unit] skip position by <dx> <dy>(XY view)" << std::endl;
      out_ << "    skip <dy> <dz> [unit] skip position by <dy> <dz>(YZ view)" << std::endl;
      out_ << "    skip <dx> <dz> [unit] skip position by <dx> <dz>(XZ view)" << std::endl;
      out_ << "    reset                 reset zoom and move to default(nozoom/nomove)" << std::endl;
      out_ << "  key                     toggle the display of the legend" << std::endl;
      out_ << "  title                   toggle the display of the main title" << std::endl;
      out_ << "  display                 display the event" << std::endl;
      out_ << "  dump                    print the data structure" << std::endl;
      out_ << "  status                  print the status of the display processor" << std::endl;
      out_ << "  wxt                     toggle wxt/X11 terminals" << std::endl;
      out_ << "  print <file>            produce an image" << std::endl;
      out_ << "  abort                   terminate the display" << std::endl;
      return;
    }

    void toy_display_driver::cmd_dump(datatools::things & a_data)
    {
      namespace sdm = snemo::datamodel;
      const sdm::event_header             * eh_ptr  = 0;
      const mctools::simulated_data       * sd_ptr  = 0;
      const sdm::calibrated_data          * cd_ptr  = 0;
      const sdm::tracker_clustering_data  * tcd_ptr = 0;
      const sdm::tracker_trajectory_data  * ttd_ptr = 0;
      const sdm::particle_track_data      * ptd_ptr = 0;

      if (a_data.has(sdm::data_info::default_event_header_label())) {
        eh_ptr = &(a_data.get<sdm::event_header>(sdm::data_info::default_event_header_label()));
      }

      if (a_data.has(sdm::data_info::default_simulated_data_label())) {
        sd_ptr = &(a_data.get<mctools::simulated_data>(sdm::data_info::default_simulated_data_label()));
      }

      if (a_data.has(sdm::data_info::default_calibrated_data_label())) {
        cd_ptr = &(a_data.get<sdm::calibrated_data>(sdm::data_info::default_calibrated_data_label()));
      }

      if (a_data.has(sdm::data_info::default_tracker_clustering_data_label())) {
        tcd_ptr = &(a_data.get<sdm::tracker_clustering_data>(sdm::data_info::default_tracker_clustering_data_label()));
      }

      if (a_data.has(sdm::data_info::default_tracker_trajectory_data_label())) {
        ttd_ptr = &(a_data.get<sdm::tracker_trajectory_data>(sdm::data_info::default_tracker_trajectory_data_label()));
      }

      if (a_data.has(sdm::data_info::default_particle_track_data_label())) {
        ptd_ptr = &(a_data.get<sdm::particle_track_data>(sdm::data_info::default_particle_track_data_label()));
      }

      if (eh_ptr != 0) {
        const sdm::event_header & eh = *eh_ptr;
        std::clog << "EH : " << "Event header :" << std::endl;
        eh.tree_dump(std::clog, "", "EH : ");
      } else {
        std::clog << "EH : " << "No event header." << std::endl;
      }

      if (sd_ptr != 0) {
        const mctools::simulated_data & sd = *sd_ptr;
        std::clog << "SD : " << "Simulated data : " << std::endl;
        sd.tree_dump(std::clog, "", "SD : ");
      } else {
        std::clog << "SD : " << "No simulated data." << std::endl;
      }

      if (cd_ptr != 0) {
        const sdm::calibrated_data & cd = *cd_ptr;
        std::clog << "CD : " << "Calibrated data : " << std::endl;
        cd.tree_dump(std::clog, "", "CD : ");
      } else {
        std::clog << "CD : " << "No calibrated data." << std::endl;
      }

      if (tcd_ptr != 0) {
        const sdm::tracker_clustering_data & tcd = *tcd_ptr;
        std::clog << "TCD: " << "Tracker clustering data : " << std::endl;
        tcd.tree_dump(std::clog, "", "TCD: ");
      } else {
        std::clog << "TCD: " << "No tracker clustering data." << std::endl;
      }

      if (ttd_ptr != 0) {
        const sdm::tracker_trajectory_data & ttd = *ttd_ptr;
        std::clog << "TTD: " << "Tracker trajectory data : " << std::endl;
        ttd.tree_dump(std::clog, "", "TTD: ");
      } else {
        std::clog << "TTD: " << "No tracker trajectory data." << std::endl;
      }

      if (ptd_ptr != 0) {
        const sdm::particle_track_data & ptd = *ptd_ptr;
        std::clog << "PTD: " << "Particle track data : " << std::endl;
        ptd.tree_dump(std::clog, "", "PTD: ");
      } else {
        std::clog << "PTD: " << "No particle track data." << std::endl;
      }


      return;
    }

    void toy_display_driver::print_status(std::ostream & out_) const
    {
      out_ << "Toy display driver status: " << std::endl;
      out_ << "|-- View mode                         : " << view_mode << std::endl;
      out_ << "|-- Wxt terminal                      : " << wxt << std::endl;
      out_ << "|-- Display the geometry              : " << display_Geo << std::endl;
      out_ << "|   |-- Source foils                  : " << display_Geo_Src << std::endl;
      out_ << "|   |-- Calo blocks                   : " << display_Geo_Calo << std::endl;
      out_ << "|   |-- X-calo blocks                 : " << display_Geo_Xcalo << std::endl;
      out_ << "|   `-- gamma veto blocks             : " << display_Geo_Gveto << std::endl;
      out_ << "|-- Display simulated data            : " << display_SD << std::endl;
      out_ << "|   |-- True hits                     : " << display_SD_hits << std::endl;
      out_ << "|   |-- Bounding boxes                : " << display_SD_hits_boxes << std::endl;
      out_ << "|   |-- Geiger hits MAP               : " << display_SD_gg_MAP << std::endl;
      out_ << "|   |-- Visu tracks                   : " << display_SD_visu_track << std::endl;
      out_ << "|   |   `-- with points               : " << display_SD_visu_track_points << std::endl;
      out_ << "|   `-- Vertex                        : " << display_SD_vertex << std::endl;
      out_ << "|-- Display calibrated data           : " << display_CD << std::endl;
      out_ << "|-- Display tracker clustering data   : " << display_TCD << std::endl;
      out_ << "|-- Display tracker trajectory data   : " << display_TTD << std::endl;
      out_ << "|-- Display particle track data       : " << display_PTD << std::endl;
      out_ << "|-- Last plot command : " << std::endl;
      std::vector<std::string> lines;

      boost::split(lines, last_plot_command_,boost::is_any_of(",;"));
      for (int line = 0; line < (int) lines.size(); line++) {
        out_ << "|   " << lines[line] << " " << std::endl;
      }
      out_ << "`-- End." << std::endl;

      return;
    }

  } // end of namespace visualization

} // end of namespace snemo
