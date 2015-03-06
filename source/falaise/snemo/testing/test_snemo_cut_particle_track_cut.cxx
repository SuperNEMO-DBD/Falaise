// test_snemo_cut_particle_track_cut.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/cuts/particle_track_cut.h>

int main()
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'particle_track_cut' class." << std::endl;

    // Create a handle of fake vertices :
    datatools::handle<geomtools::blur_spot> hV0;
    hV0.reset(new geomtools::blur_spot(geomtools::blur_spot::dimension_three));
    hV0.grab().grab_auxiliaries().store(snemo::datamodel::particle_track::vertex_type_key(),
                                        snemo::datamodel::particle_track::vertex_on_source_foil_label());
    hV0.get().tree_dump(std::clog, "Foil vertex : ");

    datatools::handle<geomtools::blur_spot> hV1;
    hV1.reset(new geomtools::blur_spot (geomtools::blur_spot::dimension_three));
    hV1.grab().grab_auxiliaries().store(snemo::datamodel::particle_track::vertex_type_key(),
                                        snemo::datamodel::particle_track::vertex_on_main_calorimeter_label());
    hV1.get().tree_dump(std::clog, "Calorimeter vertex : ");

    // Create the particle track :
    snemo::datamodel::particle_track PT0;
    PT0.set_track_id(0);
    PT0.set_charge(snemo::datamodel::particle_track::positive);
    PT0.grab_vertices().push_back(hV0);
    PT0.grab_vertices().push_back(hV1);
    PT0.grab_auxiliaries().store_flag("fake_electron");
    PT0.tree_dump(std::clog, "Particle track : ");

    {
      // Configure the cut:
      datatools::properties setup;
      setup.store_string("cut.name", "test_ptc1");
      setup.store_flag("mode.has_charge");
      setup.store_string("has_charge.type", "negative");

      // Build 'particle_track_cut':
      snemo::cut::particle_track_cut PTC(datatools::logger::PRIO_DEBUG);
      PTC.initialize_standalone(setup);
      PTC.set_user_data(PT0);
      const int status = PTC.process();
      std::cout << "Current particle is ";
      if (status != cuts::SELECTION_ACCEPTED) {
        std::cout << "not ";
      }
      std::cout << "a negative charged particle" << std::endl;
    }
    {
      // Configure the cut:
      datatools::properties setup;
      setup.store_string("cut.name", "test_ptc2");
      setup.store_flag("mode.has_associated_calorimeter_hits");

      // Build 'particle_track_cut':
      snemo::cut::particle_track_cut PTC(datatools::logger::PRIO_DEBUG);
      PTC.initialize_standalone(setup);
      PTC.set_user_data(PT0);
      const int status = PTC.process();
      std::cout << "Current particle has ";
      if (status != cuts::SELECTION_ACCEPTED) {
        std::cout << "no ";
      }
      std::cout << "associated calorimeter hits" << std::endl;
    }
    {
      // Configure the cut:
      datatools::properties setup;
      setup.store_string("cut.name", "test_ptc3");
      setup.store_flag("mode.has_vertex");
      setup.store_string("has_vertex.type", "foil");

      // Build 'particle_track_cut':
      snemo::cut::particle_track_cut PTC(datatools::logger::PRIO_DEBUG);
      PTC.initialize_standalone(setup);
      PTC.set_user_data(PT0);
      const int status = PTC.process();
      std::cout << "Current particle has ";
      if (status != cuts::SELECTION_ACCEPTED) {
        std::cout << "no ";
      }
      std::cout << "foil vertex" << std::endl;
    }
    {
      // Configure the cut:
      datatools::properties setup;
      setup.store_string("cut.name", "test_ptc4");
      setup.store_flag("mode.has_vertex");
      setup.store_string("has_vertex.type", "calo");

      // Build 'particle_track_cut':
      snemo::cut::particle_track_cut PTC(datatools::logger::PRIO_DEBUG);
      PTC.initialize_standalone(setup);
      PTC.set_user_data(PT0);
      const int status = PTC.process();
      std::cout << "Current particle has ";
      if (status != cuts::SELECTION_ACCEPTED) {
        std::cout << "no ";
      }
      std::cout << "calo vertex" << std::endl;
    }

  } catch (std::exception & x) {
    std::cerr << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_snemo_cut_particle_track_cut.cxx
