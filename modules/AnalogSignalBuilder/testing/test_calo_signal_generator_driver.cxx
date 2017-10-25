// test_calo_signal_generator_driver.cxx
// Standard libraries :
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
// - Bayeux/mctools:
#include <mctools/signal/signal_shape_builder.h>
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
#include <dpp/output_module.h>
#include <mygsl/parameter_store.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#endif  // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/asb/calo_signal_generator_driver.h>

int main(int argc_, char **argv_) {
  falaise::initialize(argc_, argv_);
  datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::signal::signal_shape_builder'!" << std::endl;

    bool draw = false;

    // Parsing arguments
    int iarg = 1;
    bool is_event_number = false;
    int arg_event_number = -1;

    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw")
        draw = true;
      else if (arg == "-n" || arg == "--number") {
        is_event_number = true;
        arg_event_number = atoi(argv_[++iarg]);
      }
      iarg++;
    }

    std::string SD_filename =
        "/home/goliviero/software/Falaise/Falaise-trunk/modules/AnalogSignalBuilder/testing/data/"
        "Se82_0nubb-source_strips_bulk_SD_10_events.brio";

    // Number of events :
    int event_number = -1;
    if (is_event_number)
      event_number = arg_event_number;
    else
      event_number = 5;

    // Event reader :
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store("logging.priority", "notice");
    reader_config.store("max_record_total", event_number);
    reader_config.store("files.mode", "single");
    reader_config.store("files.single.filename", SD_filename);
    reader.initialize_standalone(reader_config);

    // Event record :
    datatools::things ER;

    // Simulated Data "SD" bank label :
    std::string SD_bank_label = "SD";

    unsigned int psd_count = 0;  // Event counter

    std::string calo_signal_generator_config_filename =
        "@falaise:modules/AnalogSignalBuilder/calo_signal_generator.conf";
    datatools::fetch_path_with_env(calo_signal_generator_config_filename);
    std::string calo_signal_shape_builder_config_filename =
        "@falaise:modules/AnalogSignalBuilder/calo_signal_shape_builder.conf";
    datatools::fetch_path_with_env(calo_signal_shape_builder_config_filename);

    datatools::properties calo_signal_generator_prop;
    datatools::properties calo_signal_shape_builder_prop;

    calo_signal_generator_prop.read_configuration(calo_signal_generator_config_filename);
    calo_signal_shape_builder_prop.read_configuration(calo_signal_shape_builder_config_filename);

    snemo::asb::calo_signal_generator_driver csgd_1;
    csgd_1.initialize(calo_signal_generator_prop);
    // csgd_1.tree_dump(std::clog, "My calo signal generator driver");

    // shape builder 1 : direct configuration (for testing purpose)
    mctools::signal::signal_shape_builder ssb_1;
    ssb_1.set_logging_priority(datatools::logger::PRIO_FATAL);
    ssb_1.set_category("calo");
    ssb_1.add_registered_shape_type_id("mctools::signal::triangle_signal_shape");
    ssb_1.add_registered_shape_type_id("mctools::signal::multi_signal_shape");
    ssb_1.initialize_simple();
    // ssb_1.tree_dump(std::clog, "My signal shape builder 1");

    // shape builder 2 : configuration by file.conf
    mctools::signal::signal_shape_builder ssb_2;
    ssb_2.initialize(calo_signal_shape_builder_prop);
    ssb_2.tree_dump(std::clog, "My signal shape builder 2");

    while (!reader.is_terminated()) {
      reader.process(ER);
      // A plain `mctools::simulated_data' object is stored here :
      if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) {
        // Access to the "SD" bank with a stored `mctools::simulated_data' :
        const mctools::simulated_data &SD = ER.get<mctools::simulated_data>(SD_bank_label);
        mctools::signal::signal_data SSD;

        if (SD.has_step_hits("calo"))  // main calo only for the moment  ||
                                       // SD.has_step_hits("xcalo") || SD.has_step_hits("gveto"))
        {
          // Calo driver : fill the SSD bank from SD
          csgd_1.process(SD, SSD);

          std::clog << "SSD size = " << SSD.get_number_of_signals("calo") << std::endl;

          for (unsigned int isignal = 0; isignal < SSD.get_number_of_signals("calo"); isignal++) {
            mctools::signal::base_signal my_signal = SSD.get_signal("calo", isignal);

            datatools::properties signal_shape_properties;
            my_signal.get_auxiliaries().export_and_rename_starting_with(
                signal_shape_properties, mctools::signal::base_signal::shape_parameter_prefix(),
                "");

            std::string unique_signal_key =
                "Event_" + std::to_string(psd_count) + "_Signal_" + std::to_string(isignal);
            // ssb_1.create_signal_shape(unique_signal_key,
            // "mctools::signal::triangle_signal_shape", signal_shape_properties);
            ssb_2.create_signal_shape(unique_signal_key, "mctools::signal::triangle_signal_shape",
                                      signal_shape_properties);
            my_signal.tree_dump(std::clog, unique_signal_key);
          }

          datatools::temp_file tmp_file;
          tmp_file.set_remove_at_destroy(true);
          tmp_file.create("/tmp", "test_signal_shape_buider_");

          std::set<std::string> fkeys;
          ssb_2.build_list_of_functors(fkeys);
          for (const auto &fkey : fkeys) {
            tmp_file.out() << "#" << fkey << ":\n";
            ssb_2.get_functor(fkey).write_ascii_with_units(
                tmp_file.out(), -10.0 * CLHEP::nanosecond, +100.0 * CLHEP::nanosecond, 1100,
                CLHEP::ns, CLHEP::volt, 16, 16);
            tmp_file.out() << "\n\n" << std::flush;
          }

          if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
            Gnuplot g1;
            g1.cmd("set title 'Test mygsl::signal::base_signal' ");
            g1.cmd("set key out");
            g1.cmd("set grid");
            g1.cmd("set xrange [-20:100]");
            {
              std::ostringstream cmd1;
              cmd1 << "volt=" << CLHEP::volt;
              g1.cmd(cmd1.str());
              std::ostringstream cmd2;
              cmd2 << "nanosecond=" << CLHEP::nanosecond;
              g1.cmd(cmd2.str());
              // std::ostringstream cmd3;
              // cmd3 << "nVs=nanosecond*volt";
              // g1.cmd(cmd3.str());
            }
            g1.cmd("set yrange [-1:+0.1]");
            g1.set_xlabel("time (ns)").set_ylabel("Signal (V)");

            {
              std::size_t fcount = 0;
              std::ostringstream plot_cmd;
              plot_cmd << "plot ";
              for (const auto &fkey : fkeys) {
                if (fcount > 0) {
                  plot_cmd << ',';
                }
                plot_cmd << "  '" << tmp_file.get_filename() << "' "
                         << " index " << fcount << " using (column(1)"
                         << "):(column(2)" << ')' << " title 'Signal shape " << fkey
                         << "' with lines lw 3";
                fcount++;
              }

              // std::ostringstream plot_cmd;
              // plot_cmd << "plot "
              // 	     << "  '" << tmp_file.get_filename() << "' "
              // 	     << " index 0 using (column(1)"
              // 	     << "):(column(2)"<< ')'
              // 	     << " title 'Signal shape 1' with lines lw 6"
              // 	     << ", '" << tmp_file.get_filename() << "' "
              // 	     << " index 1 using (column(1)"
              // 	     << "):(column(2)"<< ')'
              // 	     << " title 'Signal shape 2' with lines lw 2"
              //   ;
              g1.cmd(plot_cmd.str());
              g1.showonscreen();  // window output
              geomtools::gnuplot_drawer::wait_for_key();
              usleep(200);
            }

#endif  // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          }

        }  // end of if has bank label "SD"

        ER.clear();
        psd_count++;
        DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      }
    }  // end of reader is terminated

    DT_LOG_NOTICE(logging, "Reader is terminated");

    std::clog << "The end." << std::endl;

  } catch (std::exception &error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "Unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}
