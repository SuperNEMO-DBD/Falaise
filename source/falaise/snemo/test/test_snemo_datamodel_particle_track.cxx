// test_snemo_datamodel_particle_track.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/vertex_utils.h>
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

int main(/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'particle_track' class." << std::endl;

    namespace sdm = snemo::datamodel;

    // Create a handle on some trajectory pattern :
    datatools::handle<sdm::base_trajectory_pattern> hLTP0;
    auto* LTP = new sdm::line_trajectory_pattern;
    // access to the internal geometry model :
    LTP->get_segment().set_first(
        geomtools::vector_3d(3. * CLHEP::mm, 5. * CLHEP::mm, 7. * CLHEP::mm));
    LTP->get_segment().set_last(
        geomtools::vector_3d(13. * CLHEP::mm, -5. * CLHEP::mm, 12. * CLHEP::mm));
    hLTP0.reset(LTP);

    datatools::handle<sdm::tracker_trajectory> hTJ0;
    hTJ0.reset(new sdm::tracker_trajectory);
    hTJ0->set_id(0);
    hTJ0->set_pattern_handle(hLTP0);
    hTJ0->grab_auxiliaries().store_flag("test");
    hTJ0->grab_auxiliaries().store("chi2", 0.234);
    hTJ0->tree_dump(std::clog, "Tracker trajectory : ");

    // Create a handle of fake vertices :
    sdm::VertexHdl hV0 = datatools::make_handle<sdm::Vertex>();
    hV0->set_hit_id(0);
    hV0->grab_geom_id().set_type(1102);      // "source_strip" geometry category
    hV0->grab_geom_id().set_address(0, 23);  // module #0, source strip #23
    hV0->set_category(sdm::VERTEX_CATEGORY_ON_SOURCE_FOIL);
    hV0->set_from(sdm::VERTEX_FROM_LAST);
    hV0->set_extrapolation(sdm::VERTEX_EXTRAPOLATION_LINE);
    hV0->set_spot(geomtools::blur_spot(geomtools::blur_spot::dimension_three));
    hV0->get_spot().set_position(geomtools::vector_3d(1.0 * CLHEP::mm,  // Mock position
                                                      2.0 * CLHEP::mm,
                                                      0.0 * CLHEP::mm));
    hV0->get_spot().set_x_error(0.5 * CLHEP::mm);
    hV0->get_spot().set_y_error(0.5 * CLHEP::mm);
    hV0->get_spot().set_z_error(0.5 * CLHEP::mm);
    hV0.get().tree_dump(std::clog, "Foil vertex : ");

    sdm::VertexHdl hV1 = datatools::make_handle<sdm::Vertex>();
    hV1->set_hit_id(1);
    hV1->grab_geom_id().set_type(1302);           // "calorimeter_block" geometry category
    hV1->grab_geom_id().set_address(0, 1, 4, 7);  // module #0, wall #1, column #4, row #7
    hV1->set_category(sdm::VERTEX_CATEGORY_ON_MAIN_CALORIMETER);
    hV1->set_from(sdm::VERTEX_FROM_FIRST);
    hV1->set_extrapolation(sdm::VERTEX_EXTRAPOLATION_HELIX);
    hV1->set_spot(geomtools::blur_spot(geomtools::blur_spot::dimension_three));
    hV1->get_spot().set_position(geomtools::vector_3d(430.0 * CLHEP::mm,  // Mock position
                                                      20.0 * CLHEP::mm,
                                                      30.0 * CLHEP::mm));
    hV1->get_spot().set_x_error(2.5 * CLHEP::mm);
    hV1->get_spot().set_y_error(2.5 * CLHEP::mm);
    hV1->get_spot().set_z_error(2.5 * CLHEP::mm);
    hV1->tree_dump(std::clog, "Calorimeter vertex : ");

    // Create the particle track :
    sdm::particle_track PT0;
    PT0.set_track_id(0);
    PT0.set_trajectory_handle(hTJ0);
    PT0.set_charge(sdm::particle_track::CHARGE_POSITIVE);
    PT0.get_vertices().push_back(hV0);
    PT0.get_vertices().push_back(hV1);
    PT0.grab_auxiliaries().store_flag("fake_electron");
    PT0.tree_dump(std::clog, "Particle track : ");

  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_snemo_datamodel_particle_track.cxx
