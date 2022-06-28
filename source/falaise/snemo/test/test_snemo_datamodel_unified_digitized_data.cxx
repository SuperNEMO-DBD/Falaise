// test_unified_digitized_data.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>

// This project:
#include <falaise/snemo/datamodels/unified_digitized_data.h>

int main(/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::unified_digitized_data'!" << std::endl;

    namespace sdm = snemo::datamodel;

    {
      // sdm::unified_digitized_data & unifiedEvent;
      sdm::unified_digitized_data unifiedEvent;
      unifiedEvent.set_run_id(1);
      unifiedEvent.set_event_id(42);
      unifiedEvent.set_reference_timestamp(666);
      unifiedEvent.add_origin_trigger_id(234);
      unifiedEvent.add_origin_trigger_id(235);
      unifiedEvent.add_origin_trigger_id(236);

      // Add calo digi hits:
      {
        sdm::calorimeter_digitized_hit & caloDigiHit = unifiedEvent.add_calorimeter_hit();
        geomtools::geom_id geomId(1302, 0, 9, 4);
        caloDigiHit.set_geom_id(geomId);
        caloDigiHit.set_timestamp(1234567);
        std::vector<int16_t> & wf = caloDigiHit.grab_waveform();
        wf.assign(1024, 700);
        for (int i = 120; i < 200; i++) {
          wf[i] = 650;
        }
        caloDigiHit.set_low_threshold_only(false);
        caloDigiHit.set_high_threshold(true);
        caloDigiHit.set_fcr(132);
        caloDigiHit.set_fwmeas_baseline(234);
        caloDigiHit.set_fwmeas_charge(54321);
        caloDigiHit.set_origin(sdm::calorimeter_digitized_hit::rtd_origin(234, 213223));
      }

      {
        sdm::calorimeter_digitized_hit & caloDigiHit = unifiedEvent.add_calorimeter_hit();
        geomtools::geom_id geomId(1302, 1, 3, 7);;
        caloDigiHit.set_geom_id(geomId);
        caloDigiHit.set_timestamp(1234568);
        std::vector<int16_t> & wf = caloDigiHit.grab_waveform();
        wf.assign(1024, 704);
        for (int i = 110; i < 235; i++) {
          wf[i] = 685;
        }
        caloDigiHit.set_low_threshold_only(true);
        caloDigiHit.set_fcr(432);
        caloDigiHit.set_fwmeas_baseline(7041);
        caloDigiHit.set_fwmeas_charge(54321);
        caloDigiHit.set_origin(sdm::calorimeter_digitized_hit::rtd_origin(234, 213223));
      }

      // Add tracker digi hits:
      {
        sdm::tracker_digitized_hit & trackerDigiHit = unifiedEvent.add_tracker_hit();
        geomtools::geom_id geomId(0, 7, 52);;
        trackerDigiHit.set_geom_id(geomId);
        {
          // Add a set of Geiger times:
          sdm::tracker_digitized_hit::gg_times newTimes;
          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R0,
                                    sdm::tracker_digitized_hit::rtd_origin(234, 213134));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R0, 1234567);

          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R1,
                                    sdm::tracker_digitized_hit::rtd_origin(237, 213134));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R1, 1234733);

          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R2,
                                    sdm::tracker_digitized_hit::rtd_origin(243, 213134));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R2, 1234856);

          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R3,
                                    sdm::tracker_digitized_hit::rtd_origin(247, 213134));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R3, 1234912);

          newTimes.set_bottom_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(234, 213135));
          newTimes.set_bottom_cathode_time(1234852);

          newTimes.set_top_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(232, 213134));
          newTimes.set_top_cathode_time(1234907);

          trackerDigiHit.grab_times().push_back(newTimes);
        }
        unifiedEvent.grab_auxiliaries().store_flag("mock");
        unifiedEvent.grab_auxiliaries().store("type", "simulated");
        unifiedEvent.grab_auxiliaries().store("the_answer", 42);
      }

      {
        boost::property_tree::ptree options;
        options.put("title", "Unified digitized event data: ");
        unifiedEvent.print_tree(std::clog, options);
      }

      {
        datatools::data_writer xout("test_unified_digitized_data.xml",
                                    datatools::using_multi_archives);
        xout.store(unifiedEvent);
      }
    }

    {
      sdm::unified_digitized_data unifiedEvent;
      datatools::data_reader xin("test_unified_digitized_data.xml",
                                 datatools::using_multi_archives);
      xin.load(unifiedEvent);
      std::clog << "A unified digitized event (from XML file):\n";
      unifiedEvent.print_tree(std::clog);
      std::clog << "\n";
    }

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
