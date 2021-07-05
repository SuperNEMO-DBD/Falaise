/*=================================================================
/
/ SuperNemo - Lazarus Module to resurrect hits in the Tracker
/
/ Paolo Franchini 2020 - p.franchini@imperial.ac.uk
/ https://github.com/pfranchini/SuperNEMO-LazarusModule
/
===================================================================*/

#include "LazarusModule.h"

// output file
dpp::output_module simOutput;

// event output
datatools::things workItem;
dpp::base_module::process_status status;
snemo::datamodel::calibrated_data *new_calibrated_data;
snemo::datamodel::calibrated_data calData;

// metadata
FLSimulate::FLSimulateArgs flSimParameters;

// various variables
std::string filename_output = "test-output.brio";
std::string dead_cells_list;
std::string method = "ALL";
int N_dead_cells;
int dead_cells[2500][3];  // Define a matrix (side, layer, column) of dead cells 
Long64_t hits = 0;
double wz = 0.0; // [mm] halfway of the cells
double wz_err = 1500.0; // [mm] uncertainty

// Tracker dimensions
const double d = 44.0; // [mm] sense wire distance
const double offsetx = 53.0; // [mm] for wire plane 0 closest to foil
const double offsety = -2464.0; // [mm] for bottom wire row with the origin sitting at the centre of the demonstrator


DPP_MODULE_REGISTRATION_IMPLEMENT(LazarusModule,"LazarusModule");

//=================================================================

LazarusModule::LazarusModule() : dpp::base_module() {}


LazarusModule::~LazarusModule() {
  //if (is_initialized())
  this->reset();
}


falaise::exit_code do_metadata( const FLSimulate::FLSimulateArgs& flSimParameters, datatools::multi_properties& flSimMetadata) {

  falaise::exit_code code = falaise::EXIT_OK;

  // System section:
  datatools::properties& system_props = flSimMetadata.add_section("flsimulate", "flsimulate::section");
  system_props.set_description("flsimulate basic system informations");
  system_props.store_string("bayeux.version", bayeux::version::get_version(),
                            "Bayeux version");
  system_props.store_string("falaise.version", falaise::version::get_version(),
                            "Falaise version");
  system_props.store_string(
			    "application", "flsimulate",
			    "The simulation application used to produce Monte Carlo data");
  system_props.store_string("application.version",
                            falaise::version::get_version(),
                            "The version of the simulation application");
  system_props.store_string("userProfile", flSimParameters.userProfile,
                            "User profile");
  system_props.store_integer("numberOfEvents", flSimParameters.numberOfEvents,
                             "Number of simulated events");
  system_props.store_boolean("doSimulation", flSimParameters.doSimulation,
                             "Activate simulation");
  system_props.store_boolean("doDigitization", flSimParameters.doDigitization,
                             "Activate digitization");
  if (!flSimParameters.experimentalSetupUrn.empty())
    system_props.store_string("experimentalSetupUrn",
                              flSimParameters.experimentalSetupUrn,
                              "Experimental setup URN");
  system_props.store_boolean("embeddedMetadata",
                             flSimParameters.embeddedMetadata,
                             "Metadata embedding flag");
  boost::posix_time::ptime start_run_timestamp = boost::posix_time::second_clock::universal_time();
  system_props.store_string( "timestamp", boost::posix_time::to_iso_string(start_run_timestamp), "Run start timestamp");
  if (flSimParameters.doSimulation) {
    // Simulation section:
    datatools::properties& simulation_props = flSimMetadata.add_section("flsimulate.simulation", "flsimulate::section");
    simulation_props.set_description("Simulation setup parameters");
    if (!flSimParameters.simulationSetupUrn.empty()) {
      simulation_props.store_string("simulationSetupUrn",
                                    flSimParameters.simulationSetupUrn,
                                    "Simulation setup URN");
    } else if (!flSimParameters.simulationManagerParams.manager_config_filename
	       .empty()) {
      simulation_props.store_path(
				  "simulationManagerConfig",
				  flSimParameters.simulationManagerParams.manager_config_filename,
				  "Simulation manager configuration file");
    }
  }

  if (flSimParameters.doDigitization) {
    // Digitization section:
    datatools::properties& digitization_props = flSimMetadata.add_section(
									  "flsimulate.digitization", "flsimulate::section");
    digitization_props.set_description("Digitization setup parameters");

    // Not implemented yet.
  }

  // Variants section:
  datatools::properties& variants_props = flSimMetadata.add_section(
								    "flsimulate.variantService", "flsimulate::section");
  variants_props.set_description("Variant setup");

  if (!flSimParameters.variantConfigUrn.empty()) {
    variants_props.store_string("configUrn", flSimParameters.variantConfigUrn,
                                "Variants setup configuration URN");
  } else if (!flSimParameters.variantSubsystemParams.config_filename.empty()) {
    variants_props.store_path(
			      "config", flSimParameters.variantSubsystemParams.config_filename,
			      "Variants setup configuration path");
  }

  if (!flSimParameters.variantProfileUrn.empty()) {
    variants_props.store_string("profileUrn", flSimParameters.variantProfileUrn,
                                "Variants profile URN");
  } else if (!flSimParameters.variantSubsystemParams.profile_load.empty()) {
    variants_props.store_path(
			      "profile", flSimParameters.variantSubsystemParams.profile_load,
			      "Variants profile path");
  }

  if (flSimParameters.variantSubsystemParams.settings.size()) {
    variants_props.store("settings",
                         flSimParameters.variantSubsystemParams.settings,
                         "Variants settings");
  }

  // Services section:
  datatools::properties& services_props =
    flSimMetadata.add_section("flsimulate.services", "flsimulate::section");
  services_props.set_description("Services configuration");

  if (!flSimParameters.servicesSubsystemConfigUrn.empty()) {
    services_props.store_string("configUrn",
                                flSimParameters.servicesSubsystemConfigUrn,
                                "Services setup configuration URN");
  } else if (!flSimParameters.servicesSubsystemConfig.empty()) {
    services_props.store_path("config", flSimParameters.servicesSubsystemConfig,
                              "Services setup configuration path");
  }

  return code;
}


void dead_cells_service(int dc[2500][3]) {

  // Create a vector of `dead_cells` with dimension `N_dead_cells`
  try {
    std::ifstream infile(dead_cells_list);   // File for list of dead cells
    Long64_t i=0;
    int side, layer, column;
    while (infile >> side >> layer >> column){
      dead_cells[i][0]=side;
      dead_cells[i][1]=layer;
      dead_cells[i][2]=column;
      i++;
    }
    N_dead_cells=i;
    std::cout << "Read " << N_dead_cells << " dead cells" << std::endl;
  }
  catch (std::logic_error& e) {
    std::cerr << "Problem in the list of dead cells " << e.what() << std::endl;
  };
  
};

void write_hit(Long64_t index){
  
  // Write a particular hit on the list of dead cells into the output calibrated data

  double wx, wy; // x,y global coordinate in the detector
  double rad = 11.0; // [mm] half radius of a cell
  double rad_err = 11.0;  // [mm] uncertainty
  int id; // counter for the hits in each event

  // create the calibrated tracker hit to build:
  snemo::datamodel::calibrated_data::tracker_hit_handle_type new_hit_handle(new snemo::datamodel::calibrated_tracker_hit);
  snemo::datamodel::calibrated_tracker_hit& new_calibrated_tracker_hit = new_hit_handle.grab();
  
  if (dead_cells[index][0]) { // Italian side
    wx = dead_cells[index][1]*d+offsetx; 
    wy = dead_cells[index][2]*d+offsety;
  }
  else {                      // French side
    wx = -(dead_cells[index][1]*d+offsetx); 
    wy = dead_cells[index][2]*d+offsety;
  }                                                                                                   
  
  const geomtools::geom_id& gid = geomtools::geom_id(1204, 0, dead_cells[index][0], dead_cells[index][1], dead_cells[index][2]); // 1204 is the type of a geiger cell core
  
  if (!calData.calibrated_tracker_hits().empty()) // get the last hit_id from the existing hits
    id = calData.calibrated_tracker_hits().back()->get_hit_id()+index+1;
  
  new_calibrated_tracker_hit.set_r(rad);
  new_calibrated_tracker_hit.set_sigma_r(rad_err);
  new_calibrated_tracker_hit.set_z(wz);
  new_calibrated_tracker_hit.set_sigma_z(wz_err);
  new_calibrated_tracker_hit.set_xy(wx, wy);
  new_calibrated_tracker_hit.set_delayed(false);
  new_calibrated_tracker_hit.set_geom_id(gid);
  new_calibrated_tracker_hit.set_hit_id(id);      
  
  // write hit
  new_calibrated_data->calibrated_tracker_hits().push_back(new_hit_handle);
  hits++;
 
}

void LazarusModule::initialize(const datatools::properties& myConfig,
			    datatools::service_manager& flServices,
			    dpp::module_handle_dict_type& /*moduleDict*/){

  // Extract the filename_out key from the supplied config, if the key exists.
  // datatools::properties throws an exception if the key isn't in the config, so catch this if thrown
  try {
    myConfig.fetch("filename_out", filename_output);
  }
  catch (std::logic_error& e) {
    std::cerr << "Problem in the output file " << e.what() << std::endl;
  };

  try {
    myConfig.fetch("dead_cells", dead_cells_list);
  } 
  catch (std::logic_error& e) {
    std::cerr << "Error in configuration files: file with list of dead cells not present " << e.what() << std::endl;
  };

  try {
    myConfig.fetch("method", method);
  }
  catch (std::logic_error& e) {
    std::cerr << "Error in configuration files: method not defined " << e.what() << std::endl;
  };

  
  std::cout << "Lazarus module initialized..." << std::endl;
  std::cout << "Method selected: " << method << std::endl;
  std::cout << "Output file: " << filename_output << std::endl;
  
  flSimParameters = FLSimulate::FLSimulateArgs::makeDefault();

  datatools::service_manager services("DeadCells", "SuperNEMO Dead Cells");
  datatools::properties services_config;
  services_config.store("name", "DeadCells");
  services_config.store("description","SuperNEMO Demonstrator Dead Cells module");
  std::vector<std::string> scf = {"@falaise:snemo/demonstrator/geometry/" "GeometryService.conf"};
  services_config.store("services.configuration_files", scf);
  services.initialize(services_config);

  // Output metadata management:
  datatools::multi_properties flSimMetadata("name", "type", "Metadata associated to a flsimulate run");
  do_metadata(flSimParameters, flSimMetadata);  

  // Metadata management:
  simOutput.set_name("DeadCellsOutput");
  simOutput.set_single_output_file(filename_output);
  datatools::multi_properties& metadataStore =  simOutput.grab_metadata_store();
  metadataStore = flSimMetadata;
  simOutput.initialize_simple();   

  // set up CD data bank
  new_calibrated_data = &(workItem.add<snemo::datamodel::calibrated_data>("CD"));

  // Read the dead cells from a file into the vector:
  std::cout << "Reading dead cells from " << dead_cells_list << "..." << std::endl;
  dead_cells_service(dead_cells);
  
  // print list of dead cells   
  //for (Long64_t i=0; i<N_dead_cells; i++)                                                                                                                             
  //  std::cout << dead_cells[i][0] << "-" << dead_cells[i][1] << "-" << dead_cells[i][2] << std::endl;

  this->_set_initialized(true);
  
}

//! [LazarusModule::Process]
dpp::base_module::process_status LazarusModule::process(datatools::things& event) {
  
  // grab Calibrated Data bank
  try {

    // To Read                    
    //const mctools::simulated_data& CD = event.get<mctools::simulated_data>("CD");
    calData = event.get<snemo::datamodel::calibrated_data>("CD");

    // clone hits to the new calibrated data
    new_calibrated_data->calibrated_tracker_hits() = calData.calibrated_tracker_hits();

    // loops in the dead cells vector
    for (Long64_t k=0; k<N_dead_cells; k++){
      
      bool is_present=false;
      // check if the hit is already present before adding it so loops over new tracker hits for this event
      for (const auto& trackerHitHdl : new_calibrated_data->calibrated_tracker_hits()) {
	if ( (trackerHitHdl->get_side()==dead_cells[k][0])&&(trackerHitHdl->get_layer()==dead_cells[k][1])&&(trackerHitHdl->get_row()==dead_cells[k][2]) ){    
	  is_present=true;
	  break;
	}
      } 	
      
      if (is_present)  // the dead cell is already present in the event's hits
	break;
	  
      // ======== Select which method to use: ========

      // ==== Method 0: NONE: ====
      if (method=="NONE") {
	// Do absolute nothing
      }
	
      // ==== Method A: Resuscitate ALL the dead cells in each event: ====
      else if (method=="ALL"){
	wz = 0.0; // [mm] halfway of the cells
	wz_err = 1500.0; // [mm] uncertainty
	write_hit(k);
      }
      
      // ==== Method B: Resuscitate ONLY cells NEAR a hit: ====
      else if (method=="NEAR") {
	// Try to if this dead cell has any nearest neighbour

	int layer_0 = dead_cells[k][1];
	int row_0 = dead_cells[k][2];

	for (const auto& oldTrackerHit : calData.calibrated_tracker_hits()) {

	  int layer = oldTrackerHit->get_layer();
	  int row = oldTrackerHit->get_row();

	  if (oldTrackerHit->get_side()==dead_cells[k][0]) // same side 
	    // check if is in an adiacent cell looking at the distance
	    if ( (sqrt(pow(layer-layer_0,2) + pow(row-row_0,2))==1) || (sqrt(pow(layer-layer_0,2) + pow(row-row_0,2))==sqrt(2)) ){
	      //std::cout << layer_0 << "," << row_0 << " - " << layer << "," << row << std::endl;
	      wz = oldTrackerHit->get_z(); // use the same longitudinal position of the adiacent cell
	      wz_err = oldTrackerHit->get_sigma_z(); // use the same error on the longitudinal position of the adiacent cell
	      write_hit(k);
	      break;
	    }
	}
      } // Method B
      
      // ==== Method C: Resuscitate ONLY cells BETWEEN two hits: ====
      else if (method=="BETWEEN") {
	// Try to if this dead cell has any nearest neighbour

	int layer_0 = dead_cells[k][1];
	int row_0 = dead_cells[k][2];
	int layer_1 = 999;
	int row_1 = 999;

	for (const auto& oldTrackerHit : calData.calibrated_tracker_hits()) {

	  int layer = oldTrackerHit->get_layer();
	  int row = oldTrackerHit->get_row();

	  if (oldTrackerHit->get_side()==dead_cells[k][0]) // same side 
	    // check if is in an adiacent cell looking at the distance
	    if ( (sqrt(pow(layer-layer_0,2) + pow(row-row_0,2))==1) || (sqrt(pow(layer-layer_0,2) + pow(row-row_0,2))==sqrt(2)) ){
	      if ((layer_1==999) && (row_1==999)){
		layer_1 = layer;
		row_1 = row;
		wz = oldTrackerHit->get_z(); // get the longitudinal position from the first adiacent cell
		wz_err = oldTrackerHit->get_sigma_z(); // get the error from the first adiacent cell
	      }
	      else {
		// second hit adiacent to the dead cell
		int layer_2 = layer;
		int row_2 = row;
		
		// the two hits should not be close to each other (the dead cell should be in the middle)
		if (sqrt(pow(layer_1-layer_2,2) + pow(row_1-row_2,2))>sqrt(2)) {
		  //std::cout << layer_0 << "," << row_0 << ": " << layer_1 << "," << row_1 << " - " << layer_2 << "," << row_2 <<std::endl;
		  wz = (wz + oldTrackerHit->get_z())/2; // use the average longitudinal position of the two adiacent cells 
		  wz_err = sqrt(pow(wz_err,2) + pow(oldTrackerHit->get_sigma_z(),2)); // use the error from the longitudinal position erro of the two adiacent cells 
		  write_hit(k);
		  break;
		}
	      }
	    }
	}
      } // Method C
    } // loops in the dead cells vector 
    
    // Calorimeter data kept in any case
    new_calibrated_data->calibrated_calorimeter_hits() = calData.calibrated_calorimeter_hits();
    
    // write workItem in the NEW brio file
    dpp::base_module::process_status status = simOutput.process(workItem);
    if (status != dpp::base_module::PROCESS_OK)
      std::cerr << "Output module failed" << std::endl;
    
    // clear for next event
    new_calibrated_data->reset();


  }
  
  catch (std::logic_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return dpp::base_module::PROCESS_INVALID;
  }// end try on CD bank
  
  // returns a status, see ref dpp::processing_status_flags_type
  return dpp::base_module::PROCESS_OK;
}


//! [LazarusModule::reset]
void LazarusModule::reset() {

  // Print stats
  std::cout << "Total number of resuscitated hits: " << hits << std::endl;

  std::cout << "END" << std::endl;

  this->_set_initialized(false);

}

