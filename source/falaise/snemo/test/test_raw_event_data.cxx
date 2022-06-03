// test_raw_event_data.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>

// This project:
#include <snemo/datamodels/raw_event_data.h>
#include <snemo/datamodels/raw_event_data-serial.h>

int main(/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::raw_event_data'!" << std::endl;
    
    namespace sdm = snemo::datamodel;

    {
      sdm::raw_event_data rawEvent;
      rawEvent.set_run_id(1);
      rawEvent.set_event_id(42);
      rawEvent.set_reference_time(snfee::data::timestamp(snfee::data::CLOCK_40MHz, 666));
      rawEvent.add_origin_trigger_id(234);
      rawEvent.add_origin_trigger_id(235);
      rawEvent.add_origin_trigger_id(236);
      
      // Add calo digi hits:
      {
        sdm::calo_digitized_hit & caloDigiHit = rawEvent.add_calo_hit(); 
        sncabling::om_id omId(sncabling::OM_MAIN, 0, 9, 4);
        geomtools::geom_id geomId;
        sdm::sncabling_bridge::convert_to_geom_id(omId, geomId);
        caloDigiHit.set_geom_id(geomId);
        caloDigiHit.set_reference_time(snfee::data::timestamp(snfee::data::CLOCK_160MHz, 1234567));
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
        caloDigiHit.set_origin(sdm::calo_digitized_hit::rtd_origin(234, 213223));
        caloDigiHit.set_om_id(omId);
      }
      
      {
        sdm::calo_digitized_hit & caloDigiHit = rawEvent.add_calo_hit(); 
        sncabling::om_id omId(sncabling::OM_MAIN, 1, 3, 7);
        geomtools::geom_id geomId;
        sdm::sncabling_bridge::convert_to_geom_id(omId, geomId);
        caloDigiHit.set_geom_id(geomId);
        caloDigiHit.set_reference_time(snfee::data::timestamp(snfee::data::CLOCK_160MHz, 1234568));
        std::vector<int16_t> & wf = caloDigiHit.grab_waveform();
        wf.assign(1024, 704);
        for (int i = 110; i < 235; i++) {
          wf[i] = 685;
        }
        caloDigiHit.set_low_threshold_only(true);
        caloDigiHit.set_fcr(432);
        caloDigiHit.set_fwmeas_baseline(7041);
        caloDigiHit.set_fwmeas_charge(54321);
        caloDigiHit.set_origin(sdm::calo_digitized_hit::rtd_origin(234, 213223));
        caloDigiHit.set_om_id(omId);
      }
      
      // Add tracker digi hits:
      {
        sdm::tracker_digitized_hit & trackerDigiHit = rawEvent.add_tracker_hit(); 
        sncabling::gg_cell_id cellId(0, 7, 52);
        geomtools::geom_id geomId;
        sdm::sncabling_bridge::convert_to_geom_id(cellId, geomId);
        trackerDigiHit.set_geom_id(geomId);    
        {
          // Add a set of Geiger times:
          sdm::tracker_digitized_hit::gg_times newTimes;
          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R0,
                                    sdm::tracker_digitized_hit::rtd_origin(234, 213134));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R0,
                                  snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234567));
          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R1,
                                    sdm::tracker_digitized_hit::rtd_origin(235, 213133));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R1,
                                  snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234733));
          newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R3,
                                    sdm::tracker_digitized_hit::rtd_origin(236, 213134));
          newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R3,
                                  snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234912));
          newTimes.set_bottom_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(234, 213135));
          newTimes.set_bottom_cathode_time(snfee::data::timestamp(snfee::data::CLOCK_80MHz,1234852));
          newTimes.set_top_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(236, 213134));
          newTimes.set_top_cathode_time(snfee::data::timestamp(snfee::data::CLOCK_80MHz,1234907));
          trackerDigiHit.grab_times().push_back(newTimes);
        }
        trackerDigiHit.set_cell_id(cellId);    
        rawEvent.grab_auxiliaries().store_flag("mock");
        rawEvent.grab_auxiliaries().store("type", "simulated");
        rawEvent.grab_auxiliaries().store("the_answer", 42);
      }
      
      {
        boost::property_tree::ptree options;
        options.put("title", "Raw event data: ");
        rawEvent.print_tree(std::clog, options);
      }
      
      {
        datatools::data_writer xout("test_raw_event_data.xml",
                                    datatools::using_multi_archives);
        xout.store(rawEvent);
      }
    }
    
    {
      sdm::raw_event_data rawEvent;
      datatools::data_reader xin("test_raw_event_data.xml",
                                 datatools::using_multi_archives);
      xin.load(rawEvent);
      std::clog << "A raw event (from XML file):\n";
      rawEvent.print_tree(std::clog);
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

