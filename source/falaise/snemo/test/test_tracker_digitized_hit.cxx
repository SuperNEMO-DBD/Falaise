// test_tracker_digitized_hit.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>

// This project:
#include <snemo/datamodels/tracker_digitized_hit.h>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::tracker_digitized_hit'" << std::endl;

    namespace sdm = snemo::datamodel;

    {
      sdm::tracker_digitized_hit trackerDigiHit;
      trackerDigiHit.set_hit_id(42);
      sncabling::gg_cell_id cellId(0, 7, 52);
      geomtools::geom_id geomId;
      sdm::sncabling_bridge::convert_to_geom_id(cellId, geomId);
      trackerDigiHit.set_geom_id(geomId);    
      {
        // Add a first set of Geiger times:
        sdm::tracker_digitized_hit::gg_times newTimes;

        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R0,
                                  sdm::tracker_digitized_hit::rtd_origin(234, 213134));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R0,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234567));

        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R1,
                                  sdm::tracker_digitized_hit::rtd_origin(237, 213134));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R1,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234733));

        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R2,
                                  sdm::tracker_digitized_hit::rtd_origin(243, 213134));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R2,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234856));

        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R3,
                                  sdm::tracker_digitized_hit::rtd_origin(247, 213134));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R3,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234912));

        newTimes.set_bottom_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(234, 213135));
        newTimes.set_bottom_cathode_time(snfee::data::timestamp(snfee::data::CLOCK_80MHz,1234852));

        newTimes.set_top_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(232, 213134));
        newTimes.set_top_cathode_time(snfee::data::timestamp(snfee::data::CLOCK_80MHz,1234907));

        trackerDigiHit.grab_times().push_back(newTimes);
      }
      
      {
        // Add a second set of Geiger times:
        sdm::tracker_digitized_hit::gg_times newTimes;
        
        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R0,
                                  sdm::tracker_digitized_hit::rtd_origin(434, 213135));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R0,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234542));
        
        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R3,
                                  sdm::tracker_digitized_hit::rtd_origin(447, 213135));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R3,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1234833));
        
        newTimes.set_anode_origin(sdm::tracker_digitized_hit::ANODE_R4,
                                  sdm::tracker_digitized_hit::rtd_origin(441, 213135));
        newTimes.set_anode_time(sdm::tracker_digitized_hit::ANODE_R4,
                                snfee::data::timestamp(snfee::data::CLOCK_80MHz, 1235156));
        
        newTimes.set_top_cathode_origin(sdm::tracker_digitized_hit::rtd_origin(456, 213136));
        newTimes.set_top_cathode_time(snfee::data::timestamp(snfee::data::CLOCK_80MHz,1235307));
        
        trackerDigiHit.grab_times().push_back(newTimes);
      }
      trackerDigiHit.set_cell_id(cellId);    

      std::clog << "A tracker digitized (raw) hit:\n";
      trackerDigiHit.print_tree(std::clog);
      std::clog << "\n";

      {
        datatools::data_writer xout("test_tracker_digitized_hit.xml", datatools::using_multi_archives);
        datatools::data_writer bout("test_tracker_digitized_hit.data", datatools::using_multi_archives);
        xout.store(trackerDigiHit);
        bout.store(trackerDigiHit);
      }

    }
    
    {
      sdm::tracker_digitized_hit trackerDigiHit;
      datatools::data_reader xin("test_tracker_digitized_hit.xml", datatools::using_multi_archives);
      xin.load(trackerDigiHit);
      std::clog << "A tracker digitized (raw) hit (from XML file):\n";
      trackerDigiHit.print_tree(std::clog);
      std::clog << "\n";
    }
    
    {
      sdm::tracker_digitized_hit trackerDigiHit;
      datatools::data_reader bin("test_tracker_digitized_hit.data", datatools::using_multi_archives);
      bin.load(trackerDigiHit);
      std::clog << "A tracker digitized (raw) hit (from binary file):\n";
      trackerDigiHit.print_tree(std::clog);
      std::clog << "\n";
    }
 
    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
