// test_snemo_datamodel_particle_track_data.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>

int main(/*int argc_, char ** argv_*/) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'particle_track_data' class." << std::endl;

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
    datatools::handle<geomtools::blur_spot> hV0;
    hV0.reset(new geomtools::blur_spot(geomtools::blur_spot::dimension_three));
    hV0.grab().set_hit_id(0);
    hV0.grab().grab_geom_id().set_type(1102);      // "source_strip" geometry category
    hV0.grab().grab_geom_id().set_address(0, 23);  // module #0, source strip #23
    hV0.grab().set_position(
        geomtools::vector_3d(1.0 * CLHEP::mm, 2.0 * CLHEP::mm, 3.0 * CLHEP::mm));
    hV0.grab().set_x_error(0.5 * CLHEP::mm);
    hV0.grab().set_y_error(0.5 * CLHEP::mm);
    hV0.grab().set_z_error(0.5 * CLHEP::mm);
    hV0.grab().grab_auxiliaries().store(sdm::particle_track::vertex_type_key(),
                                        sdm::particle_track::vertex_on_source_foil_label());
    hV0.get().tree_dump(std::clog, "Foil vertex : ");

    datatools::handle<geomtools::blur_spot> hV1;
    hV1.reset(new geomtools::blur_spot(geomtools::blur_spot::dimension_three));
    hV1.grab().set_hit_id(1);
    hV1.grab().grab_geom_id().set_type(1302);           // "calorimeter_block" geometry category
    hV1.grab().grab_geom_id().set_address(0, 1, 4, 7);  // module #0, wall #1, column #4, row #7
    hV1.grab().set_position(
        geomtools::vector_3d(10.0 * CLHEP::mm, 20.0 * CLHEP::mm, 30.0 * CLHEP::mm));
    hV1.grab().set_x_error(2.5 * CLHEP::mm);
    hV1.grab().set_y_error(2.5 * CLHEP::mm);
    hV1.grab().set_z_error(2.5 * CLHEP::mm);
    hV1.grab().grab_auxiliaries().store(sdm::particle_track::vertex_type_key(),
                                        sdm::particle_track::vertex_on_main_calorimeter_label());
    hV1.get().tree_dump(std::clog, "Calorimeter vertex : ");

    // Create the particle track :
    datatools::handle<sdm::particle_track> hPT0;
    hPT0.reset(new sdm::particle_track);
    hPT0.grab().set_track_id(0);
    hPT0.grab().set_charge(sdm::particle_track::POSITIVE);
    hPT0.grab().set_trajectory_handle(hTJ0);
    hPT0.grab().get_vertices().push_back(hV0);
    hPT0.grab().get_vertices().push_back(hV1);
    hPT0.grab().grab_auxiliaries().store_flag("fake_positron");
    hPT0.get().tree_dump(std::clog, "Particle track : ");

    // Event record :
    snedm::event_record ER;

    // Particle track data bank :
    auto& PTD = ER.add<sdm::particle_track_data>(snedm::labels::particle_track_data());
    PTD.insertParticle(hPT0);
    PTD.tree_dump(std::clog, "Particle track data :");

    // Retrieve electrons if any
    sdm::ParticleHdlCollection electrons = PTD.getParticlesByCharge(sdm::particle_track::NEGATIVE);
    std::clog << "Number of particles = " << PTD.numberOfParticles() << std::endl;
    std::clog << "Number of electrons = " << electrons.size() << std::endl;

    // Adding other particles
    for (size_t i = 0; i < 10; i++) {
      datatools::handle<sdm::particle_track> hPT;
      hPT.reset(new sdm::particle_track);
      hPT->set_track_id(i);
      PTD.insertParticle(hPT);
    }
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
