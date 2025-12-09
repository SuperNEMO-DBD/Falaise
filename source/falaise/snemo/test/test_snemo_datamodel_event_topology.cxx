// test_snemo_datamodel_event_topology.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/datamodels/event_topology_data.h>
#include <falaise/snemo/datamodels/reconstructed_particle.h>

int main(int /* argc_ */, char** /* argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::event_topology'!" << std::endl;

    snemo::datamodel::event_topology_data evTopoData;

    {
      geomtools::blur_spot vtxSpot(geomtools::blur_spot::DIMENSION_THREE);
      vtxSpot.set_position(geomtools::vector_3d(4.4 * CLHEP::mm, 1234.2 * CLHEP::mm, 765.3 * CLHEP::mm));
      vtxSpot.set_x_error(1.4 * CLHEP::mm);
      vtxSpot.set_y_error(24.2 * CLHEP::mm);
      vtxSpot.set_z_error(34.5 * CLHEP::mm);    
      auto vtxHdl = datatools::make_handle<snemo::datamodel::Vertex>();
      vtxHdl->set_hit_id(0);
      vtxHdl->set_geom_id(geomtools::geom_id(666, 0, 0, 0));
      vtxHdl->set_category(snemo::datamodel::VERTEX_CATEGORY_ON_SOURCE_FOIL);
      vtxHdl->set_spot(vtxSpot);
      vtxHdl->set_extrapolation(snemo::datamodel::VERTEX_EXTRAPOLATION_OTHER);
      auto evTopoHdl = datatools::make_handle<snemo::datamodel::event_topology>();
      evTopoHdl->set_origin_timestamp(snemo::datamodel::timestamp(0, 0));
      evTopoHdl->set_origin_vertex(vtxHdl);
      auto partHdl0 = datatools::make_handle<snemo::datamodel::reconstructed_particle>();
      partHdl0->set_electron();
      partHdl0->set_prompt();
      auto partHdl1 = datatools::make_handle<snemo::datamodel::reconstructed_particle>();
      partHdl1->set_gamma();
      partHdl1->set_prompt();
      evTopoHdl->add_particle(partHdl0);
      evTopoHdl->add_particle(partHdl1);
      evTopoHdl->set_likelihood(0.35);
      evTopoData.add_topology(evTopoHdl);
    }
    
    {
     
      geomtools::blur_spot vtxSpot(geomtools::blur_spot::DIMENSION_THREE);
      vtxSpot.set_position(geomtools::vector_3d(4.4 * CLHEP::mm, 1234.2 * CLHEP::mm, 765.3 * CLHEP::mm));
      vtxSpot.set_x_error(1.4 * CLHEP::mm);
      vtxSpot.set_y_error(24.2 * CLHEP::mm);
      vtxSpot.set_z_error(34.5 * CLHEP::mm);
      auto vtxHdl= datatools::make_handle<snemo::datamodel::Vertex>();
      vtxHdl->set_geom_id(geomtools::geom_id(666, 0, 0, 0));
      vtxHdl->set_category(snemo::datamodel::VERTEX_CATEGORY_ON_SOURCE_FOIL);
      vtxHdl->set_spot(vtxSpot);
      vtxHdl->set_extrapolation(snemo::datamodel::VERTEX_EXTRAPOLATION_OTHER);
      auto evTopoHdl = datatools::make_handle<snemo::datamodel::event_topology>();
      evTopoHdl->set_origin_timestamp(snemo::datamodel::timestamp(0, 0));
      evTopoHdl->set_origin_vertex(vtxHdl);
      auto partHdl0 = datatools::make_handle<snemo::datamodel::reconstructed_particle>();
      partHdl0->set_electron();
      partHdl0->set_prompt();
      auto partHdl1 = datatools::make_handle<snemo::datamodel::reconstructed_particle>();
      partHdl1->set_gamma();
      partHdl1->set_prompt();
      auto partHdl2 = datatools::make_handle<snemo::datamodel::reconstructed_particle>();
      partHdl2->set_alpha();
      partHdl2->set_delayed();
      evTopoHdl->add_particle(partHdl0);
      evTopoHdl->add_particle(partHdl1);
      evTopoHdl->add_particle(partHdl2);
      evTopoHdl->set_likelihood(0.15); 
      evTopoData.add_topology(evTopoHdl); 
    }
    
    {
      boost::property_tree::ptree poptions;
      poptions.put("indent", "test> ");
      poptions.put("title", "Event topology data :");
      evTopoData.print_tree(std::clog, poptions);
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
