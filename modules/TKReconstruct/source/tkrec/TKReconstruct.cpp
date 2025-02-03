// Interface from Falaise
#include "tkrec/TKReconstruct.h"

// Standard headers
#include <iomanip>

#include <bayeux/datatools/clhep_units.h>
#include <bayeux/geomtools/box.h>

#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>

namespace tkrec {

  DPP_MODULE_REGISTRATION_IMPLEMENT(TKReconstruct, "TKReconstruct")

  struct TKReconstruct::pimpl_type
  {
    TKReconstruct & tkrec; ///< Reference to the father module
    TKgeom geom; ///< Geometry informations
    TKEvent event; ///< Working event to be reconstructed
    std::unique_ptr<TKalgos> palgo; ///< Reconstruction algorithm
    size_t eventCounter = 0;
    pimpl_type(TKReconstruct & tkrec_);
  };

  TKReconstruct::pimpl_type::pimpl_type(TKReconstruct & tkrec_)
    : tkrec(tkrec_)
  {
    tkrec._init_geom_(geom);
    palgo = std::make_unique<TKalgos>(geom);
    palgo->initialize(tkrec._config_.recConfig);
    return;
  }
 
  // static
  const double TKReconstruct::invalid_distance = -1.0;

  const TKReconstruct::config_type & TKReconstruct::config() const
  {
    return _config_;
  }
  
  void TKReconstruct::_set_defaults_()
  {
    _geoManager_ = snemo::service_handle<snemo::geometry_svc>{};
    return;
  }

  void TKReconstruct::_init_geom_(TKgeom & geom_)
  {
    DT_LOG_DEBUG(_config_.verbosity, "Initializing geometry info...");
    const geomtools::mapping & geoMapping = _geoManager_->get_mapping();
    const geomtools::id_mgr & geoIdMgr = _geoManager_->get_id_mgr();
    std::string locator_plugin_name;
    const snemo::geometry::locator_plugin * geoLocator
      = snemo::geometry::getSNemoLocator(*_geoManager_.instance(), locator_plugin_name);
    const snemo::geometry::calo_locator & caloLocator = geoLocator->caloLocator();
    const snemo::geometry::xcalo_locator & xcaloLocator = geoLocator->xcaloLocator();
    const snemo::geometry::gveto_locator & gvetoLocator = geoLocator->gvetoLocator();
    const snemo::geometry::gg_locator & ggLocator = geoLocator->geigerLocator();

    geom_.has_Bi_source = false;
    std::vector<geomtools::geom_id> sourceCalibrationCarrierGids;
    uint32_t sourceCalibrationCarrierType = geomtools::geom_id::INVALID_TYPE; // (1110)
    if (geoIdMgr.has_category_info("source_calibration_carrier")) {
      geom_.has_Bi_source = true;
      sourceCalibrationCarrierType =
	geoIdMgr.get_category_info("source_calibration_carrier").get_type();
      geomtools::geom_id sourceCalibrarionCarrierGidPattern(sourceCalibrationCarrierType,
							    0, // module
							    geomtools::geom_id::ANY_ADDRESS,  // track
							    geomtools::geom_id::ANY_ADDRESS); // position
      geoMapping.compute_matching_geom_id(sourceCalibrarionCarrierGidPattern, sourceCalibrationCarrierGids);
      if (sourceCalibrationCarrierGids.size()) {
	const geomtools::geom_info & srcCalibCarrierGinfo = geoMapping.get_geom_info(sourceCalibrationCarrierGids.front());
	const geomtools::logical_volume & srcCalibCarrierLog = srcCalibCarrierGinfo.get_logical();
	const geomtools::i_shape_3d & srcCalibCarrierShape = srcCalibCarrierLog.get_shape();    
	const geomtools::box & srcCalibCarrierBox = dynamic_cast<const geomtools::box &>(srcCalibCarrierShape);
	double x = srcCalibCarrierBox.get_z();
	double y = srcCalibCarrierBox.get_y();
	double z = srcCalibCarrierBox.get_x();
	DT_LOG_DEBUG(_config_.verbosity, "x = " << x);
	DT_LOG_DEBUG(_config_.verbosity, "y = " << y);
	DT_LOG_DEBUG(_config_.verbosity, "z = " << z);
	geom_.Bi_source_x = x;
	geom_.Bi_source_y = y;
	geom_.Bi_source_z = z;
      }
    }
    
    geom_.tc_radius = ggLocator.cellRadius() / CLHEP::mm;
    DT_LOG_DEBUG(_config_.verbosity, "tc_radius = " << geom_.tc_radius);

    geom_.mw_sizex = caloLocator.blockThickness() / CLHEP::mm; // 194.0 (31.0);
    geom_.mw_sizey = caloLocator.blockWidth() / CLHEP::mm; // 256.0;
    geom_.mw_sizez = caloLocator.blockHeight() / CLHEP::mm; // 256.0;

    geom_.gv_sizex = gvetoLocator.blockHeight() / CLHEP::mm; // 308.0;
    geom_.gv_sizey = gvetoLocator.blockWidth() / CLHEP::mm; // 310.0;
    geom_.gv_sizez = gvetoLocator.blockThickness() / CLHEP::mm; // 150.0;
 
    geom_.xw_sizex = xcaloLocator.blockWidth() / CLHEP::mm; // 200.0;
    geom_.xw_sizey = xcaloLocator.blockThickness() / CLHEP::mm; // 150.0;
    geom_.xw_sizez = xcaloLocator.blockHeight() / CLHEP::mm; // 208.5;

    DT_LOG_DEBUG(_config_.verbosity, "mw_sizex = " << geom_.mw_sizex);
    DT_LOG_DEBUG(_config_.verbosity, "mw_sizey = " << geom_.mw_sizey);
    DT_LOG_DEBUG(_config_.verbosity, "mw_sizez = " << geom_.mw_sizez);
    DT_LOG_DEBUG(_config_.verbosity, "gv_sizex = " << geom_.gv_sizex);
    DT_LOG_DEBUG(_config_.verbosity, "gv_sizey = " << geom_.gv_sizey);
    DT_LOG_DEBUG(_config_.verbosity, "gv_sizez = " << geom_.gv_sizez);
    DT_LOG_DEBUG(_config_.verbosity, "xw_sizex = " << geom_.xw_sizex);
    DT_LOG_DEBUG(_config_.verbosity, "xw_sizey = " << geom_.xw_sizey);
    DT_LOG_DEBUG(_config_.verbosity, "xw_sizez = " << geom_.xw_sizez);
    
    return;
  }

  TKReconstruct::TKReconstruct()
    : dpp::base_module() 
  {
    _set_defaults_();
    return;
  }

  TKReconstruct::~TKReconstruct()
  { 
    if (this->is_initialized())
      {
        this->reset();
      }
    return;
  }

  void TKReconstruct::read_config(const datatools::properties& config_)
  {
    if (config_.has_key("verbosity")) {
      std::string verbosityLabel = config_.fetch_string("verbosity");
      auto parsedVerb = datatools::logger::get_priority(verbosityLabel);
      DT_THROW_IF(parsedVerb == datatools::logger::PRIO_UNDEFINED,
		  std::logic_error,
		  "Undefined verbosity label " << std::quoted(verbosityLabel));
      _config_.verbosity = parsedVerb;
    }

    if (config_.has_key("CD_label")) {
      _config_.CD_label = config_.fetch_string("CD_label");
    }
 
    if (config_.has_key("TCD_label")) {
      _config_.TCD_label = config_.fetch_string("TCD_label");
    }
 
    if (config_.has_key("TTD_label")) {
      _config_.TTD_label = config_.fetch_string("TTD_label");
    }
 
    // Extract properties with prefix 'eventrec.' :
    datatools::properties recParamConfig;
    config_.export_and_rename_starting_with(recParamConfig, "eventrec.", "");
    _config_.recConfig.parse(recParamConfig);
    return;
  }

  void TKReconstruct::initialize(const datatools::properties & config_,
				 datatools::service_manager & services_,
				 dpp::module_handle_dict_type &  /*moduleDict*/
				 ) 
  {
    read_config(config_);
    
    // Geometry manager :
    _geoManager_ = snemo::service_handle<snemo::geometry_svc>{services_};
    // Instantiate the PIMPL working material:
    _work_ = std::make_unique<pimpl_type>(*this);
 
    this->_set_initialized(true);
    return;
  }

  void TKReconstruct::reset() 
  {   
    DT_THROW_IF(!is_initialized(), std::logic_error,
		"Module '" << get_name() << "' is not initialized !");
    this->_set_initialized(false);
    // Destroy the PIMPL working material:
    _work_.reset();
    _set_defaults_();
    return;
  }


  dpp::base_module::process_status TKReconstruct::process(datatools::things & workItem) 
  {
    _work_->eventCounter++;
    DT_LOG_DEBUG(_config_.verbosity, "============ New event #" << _work_->eventCounter);
    populate_working_event(workItem);
    //eventNo = workItem->get_id();
    // TKEvent* event = get_event_data(workItem); 	// event -> TKEvent
    _work_->palgo->process(_work_->event);	
	
    namespace snedm = snemo::datamodel;
	
    // Create or reset TCD bank
    auto & the_tracker_clustering_data
      = ::snedm::getOrAddToEvent<snedm::tracker_clustering_data>(_config_.TCD_label, workItem);
    the_tracker_clustering_data.clear();

    // Fill TKcluster data into TCD bank
    snedm::calibrated_data falaiseCDbank = workItem.get<snedm::calibrated_data>(_config_.CD_label);
    fill_TCD_bank(falaiseCDbank, the_tracker_clustering_data);

    // Create or reset TTD bank
    auto & the_tracker_trajectory_data
      = ::snedm::getOrAddToEvent<snedm::tracker_trajectory_data>(_config_.TTD_label, workItem);
    the_tracker_trajectory_data.clear();

    // Fill TKtrack data into TTD bank
    fill_TTD_bank(the_tracker_clustering_data, the_tracker_trajectory_data);

    // delete event;
    return falaise::processing::status::PROCESS_OK;
  }

  void TKReconstruct::populate_working_event(const datatools::things &workItem)
  {
    _work_->event.reset();
    auto &header = workItem.get<snemo::datamodel::event_header>("EH");
    _work_->event.set_event_ids(header.get_id().get_run_number(),
				header.get_id().get_event_number());

    if(workItem.has(_config_.CD_label))
      {
	DT_LOG_DEBUG(_config_.verbosity, "Has CD bank");
	using namespace snemo::datamodel;

	calibrated_data falaiseCDbank = workItem.get<calibrated_data>(_config_.CD_label);
	DT_LOG_DEBUG(_config_.verbosity, "Nb calo hits = " << falaiseCDbank.calorimeter_hits().size());

	for ( auto &calohit : falaiseCDbank.calorimeter_hits() )
	  {
	    int SWCR[4] = {-1,-1,-1,-1};
	    switch( calohit->get_geom_id().get_type() )
	      {
	      case 1302: 
		SWCR[0] = calohit->get_geom_id().get(1);
		SWCR[2] = calohit->get_geom_id().get(2);
		SWCR[3] = calohit->get_geom_id().get(3);
		break;
	      case 1232:
		SWCR[0] = calohit->get_geom_id().get(1);
		SWCR[1] = calohit->get_geom_id().get(2);
		SWCR[2] = calohit->get_geom_id().get(3);
		SWCR[3] = calohit->get_geom_id().get(4);
		break;
	      case 1252:
		SWCR[0] = calohit->get_geom_id().get(1);
		SWCR[1] = calohit->get_geom_id().get(2);
		SWCR[2] = calohit->get_geom_id().get(3);
		break;
	      }
	    auto OMhitPtr = std::make_shared<TKOMhit>(SWCR);		
	    //OMhitPtr->set_energy( calohit->get_energy() ); 
			
	    _work_->event.add_OM_hit( OMhitPtr );
	  }

	DT_LOG_DEBUG(_config_.verbosity, "Nb tracker hits = " << falaiseCDbank.tracker_hits().size());
	for ( auto &trhit : falaiseCDbank.tracker_hits() )
	  {
	    int SRL[3] = {trhit->get_side(), trhit->get_row(), trhit->get_layer()};
	    auto hit = std::make_shared<TKtrhit>(SRL);
			
	    if(not std::isnan(trhit->get_r()))
	      {
		hit->set_r( trhit->get_r() );        	
		hit->set_sigma_R( /*trhit->get_sigma_r()*/ _config_.recConfig.default_sigma_r );
	      }
	    else
	      {
		hit->set_r( invalid_distance );        	
		hit->set_sigma_R( invalid_distance );
	      }
			
	    if(not std::isnan(trhit->get_z()))
	      {
		hit->set_h( trhit->get_z() );
		hit->set_sigma_Z( trhit->get_sigma_z() );
	      }
	    else
	      {
		hit->set_h( 0.0 );
	      }
	    /*
	      std::cout << "r: 	" << trhit->get_r() << std::endl;
	      std::cout << "sigma_r: 	" << trhit->get_sigma_r() << std::endl;
	      std::cout << "Z: 	" << trhit->get_z() << std::endl;
	      std::cout << "sigma_Z: 	" << trhit->get_sigma_z() << std::endl;
	    */
	    _work_->event.add_tracker_hit(hit);
	  }

      }
    DT_LOG_DEBUG(_config_.verbosity, "Working event has been populated");
    
    return;
  }

  void TKReconstruct::fill_TCD_bank(snemo::datamodel::calibrated_data& falaiseCDbank,
				    snemo::datamodel::tracker_clustering_data& the_tracker_clustering_data) const
  {
    namespace snedm = snemo::datamodel;
 
    auto htcs = datatools::make_handle<snedm::TrackerClusteringSolution>();
    the_tracker_clustering_data.append_solution(htcs, true);
    the_tracker_clustering_data.get_default().set_solution_id(the_tracker_clustering_data.size() - 1);

    snedm::tracker_clustering_solution& clustering_solution
      = the_tracker_clustering_data.get_default();
	
    for(auto i = 0u; i < _work_->event.get_trajectories().size(); i++)
      {
	{
	  snedm::TrackerClusterHdl tch = datatools::make_handle<snedm::tracker_cluster>();
	  clustering_solution.get_clusters().push_back(tch);
	}	  
	snedm::TrackerClusterHdl& cluster_handle
	  = clustering_solution.get_clusters().back();
	cluster_handle->set_cluster_id(i);
	
	ConstTKtrajectoryHdl traj = _work_->event.get_trajectories()[i];
	std::vector<ConstTKtrackHdl> traj_segments = traj->get_segments();
	for(auto j = 0u; j < traj_segments.size(); j++)
	  {
	    auto trajectory_hits = traj_segments[j]->get_associated_tr_hits();
	    for(auto k = 0u; k < trajectory_hits.size(); k++)
	      {
		const TKtrhitHdl & TKhit = trajectory_hits[k];
		int SRL[3] = {TKhit->get_SRL('S'), TKhit->get_SRL('R'), TKhit->get_SRL('L')};
		for (const auto &trhit : falaiseCDbank.tracker_hits() )
		  {
		    if(SRL[0] == trhit->get_side() && SRL[1] == trhit->get_row() && SRL[2] == trhit->get_layer())
		      {
			cluster_handle->hits().push_back(trhit);				
		      }
		  }			
	      }
	  }
	cluster_handle->set_geom_id(geomtools::geom_id(1201, 0, cluster_handle->hits()[0]->get_side()));
      }
	

    /*
      for(int i = 0; i < _work_->event.get_clusters().size(); i ++)
      {
      snedm::TrackerClusterHdl tch(new snedm::tracker_cluster);
      clustering_solution.get_clusters().push_back(tch);
		  
      snedm::TrackerClusterHdl& cluster_handle = clustering_solution.get_clusters().back();
      //cluster_handle->set_cluster_id(clustering_solution.get_clusters().size() - 1);
      cluster_handle->set_cluster_id(i);
		
      TKcluster* cluster = _work_->event.get_clusters()[i];
      for (int j = 0; j < cluster->get_tr_hits().size(); j++)
      {
      TKtrhit* TKhit = cluster->get_tr_hits()[j];
      int SRL[3] = {TKhit->get_SRL('S'), TKhit->get_SRL('R'), TKhit->get_SRL('L')};
      for ( auto &trhit : falaiseCDbank.tracker_hits() )
      {
      if(SRL[0] == trhit->get_side() && SRL[1] == trhit->get_row() && SRL[2] == trhit->get_layer())
      {
      cluster_handle->hits().push_back(trhit);				
      }
      }			
      }		
      cluster_handle->set_geom_id(geomtools::geom_id(1201, 0, cluster_handle->hits()[0]->get_side()));
      }*/

    return;
  }

  void TKReconstruct::fill_TTD_bank(snemo::datamodel::tracker_clustering_data& the_tracker_clustering_data,
				    snemo::datamodel::tracker_trajectory_data& the_tracker_trajectory_data) const
  {	
    namespace snedm = snemo::datamodel;

    // Get cluster solutions:
    const snedm::TrackerClusteringSolutionHdlCollection & cluster_solutions
      = the_tracker_clustering_data.solutions();

    // Loop on all clustering solutions:
    for (const datatools::handle<snedm::tracker_clustering_solution> & a_cluster_solution : cluster_solutions) 
      {
	// Build a new trajectory solution for each clustering solution:
	auto a_trajectory_solution = datatools::make_handle<snedm::tracker_trajectory_solution>();
	the_tracker_trajectory_data.add_solution(a_trajectory_solution);
	a_trajectory_solution->set_solution_id(a_cluster_solution->get_solution_id());
	a_trajectory_solution->set_clustering_solution(a_cluster_solution);
		
	// Get clusters stored in the current tracker solution:
	const snedm::TrackerClusterHdlCollection& clusters = a_cluster_solution->get_clusters();

	const auto & eventTrajectories = _work_->event.get_trajectories();
	for(auto i = 0u; i < eventTrajectories.size(); i++)
	  {
	    const ConstTKtrajectoryHdl & traj = eventTrajectories[i];
			
	    auto& a_cluster = clusters[i]; 
		
	    //snedm::TrackerTrajectoryHdlCollection Trajectories;
						
	    // Create new 'tracker_trajectory' handle:  
	    auto h_trajectory = datatools::make_handle<snedm::tracker_trajectory>();
	    //Trajectories.push_back(h_trajectory);

	    // Create new 'tracker_pattern' handle:
	    snedm::TrajectoryPatternHdl h_pattern;			
	    auto polyline_pattern = new snedm::polyline_trajectory_pattern;
	    h_pattern.reset(polyline_pattern);
			
	    /*
	      auto line_pattern = new snedm::line_trajectory_pattern;
	      h_pattern.reset(line_pattern);
	    */
	    h_trajectory->set_pattern_handle(h_pattern);
			
	    // Set cluster and pattern handle to tracker_trajectory:
	    //auto id = Trajectories.size();
			
	    h_trajectory->set_id(i);
	    h_trajectory->set_cluster_handle(a_cluster);
			
	    h_trajectory->set_geom_id(geomtools::geom_id(1201, 0, traj->get_side()));
			
	    h_trajectory->get_fit_infos().set_best(true);
	    h_trajectory->get_fit_infos().set_algo(snedm::track_fit_algo_type::TRACK_FIT_ALGO_LINE);
			
	    //double chi2 = std::pow(a_fit_solution.chi, 2);
	    //h_trajectory->get_fit_infos().set_chi2(chi2);
	    //h_trajectory->get_fit_infos().set_ndof(a_fit_solution.ndof);
	    			
	    geomtools::polyline_3d& polyline = polyline_pattern->get_path();

	    const auto & trajTrackPoints = traj->get_track_points();
	    for(auto j = 0u; j < trajTrackPoints.size(); j++)
	      {
		const ConstTKpointHdl & point = trajTrackPoints[j];
		geomtools::vector_3d geom_point = {
		  point->get_x(),
		  point->get_y(),
		  point->get_z()}; 
		polyline.add(geom_point);
	      }
	    //line_to_verteces(cluster->get_track(), line_3d);
			
	    /*
	      geomtools::line_3d& line_3d = line_pattern->get_segment();
	      TKpoint* start = traj->get_track_points()[0];
	      TKpoint* end   = traj->get_track_points()[1];	
	      line_3d.set_first(start->get_x() * CLHEP::mm, start->get_y() * CLHEP::mm, start->get_z() * CLHEP::mm);
	      line_3d.set_last(end->get_x() * CLHEP::mm, end->get_y() * CLHEP::mm, end->get_z() * CLHEP::mm);
	    */
			
	    a_trajectory_solution->grab_best_trajectories().insert(i);
	    a_trajectory_solution->grab_trajectories().push_back(h_trajectory);
	
	  }
		
		
	/*
	// Loop over TKclusters
	for (int i = 0; i < _work_->event.get_clusters().size(); i ++)
	{
	TKcluster* cluster = _work_->event.get_clusters()[i];
	if(cluster->get_track() == nullptr) continue;
			
	// using the first hit for TKcluster <-> Falaise cluster identification 
	TKtrhit* TKhit = cluster->get_tr_hits()[0];
	int SRL[3] = {TKhit->get_SRL('S'), TKhit->get_SRL('R'), TKhit->get_SRL('L')};

	// matching Falaise clusters with the TKcluster
	for (const datatools::handle<snedm::tracker_cluster> & a_cluster : clusters) 
	{
	bool same_cluster = false;
	for ( auto &trhit : a_cluster->hits() )
	{
	if(SRL[0] == trhit->get_side() && SRL[1] == trhit->get_row() && SRL[2] == trhit->get_layer())
	{
	same_cluster = true;			
	}
	}
				
	if(!same_cluster) continue;
			
	snedm::TrackerTrajectoryHdlCollection Trajectories;
							
	// Create new 'tracker_trajectory' handle:  
	auto h_trajectory = datatools::make_handle<snedm::tracker_trajectory>();
	Trajectories.push_back(h_trajectory);

	// Create new 'tracker_pattern' handle:
	snemo::datamodel::TrajectoryPatternHdl h_pattern;
	auto line_pattern = new snedm::line_trajectory_pattern;
	h_pattern.reset(line_pattern);
				
	// Set cluster and pattern handle to tracker_trajectory:
	auto id = Trajectories.size();
	h_trajectory->set_id(id);
	h_trajectory->set_cluster_handle(a_cluster);
	h_trajectory->set_pattern_handle(h_pattern);
				
	h_trajectory->set_geom_id(geomtools::geom_id(1201, 0, a_cluster->hits()[0]->get_side()));
				
	h_trajectory->get_fit_infos().set_best(true);
	h_trajectory->get_fit_infos().set_algo(snedm::track_fit_algo_type::TRACK_FIT_ALGO_LINE);
				
	//double chi2 = std::pow(a_fit_solution.chi, 2);
	//h_trajectory->get_fit_infos().set_chi2(chi2);
	//h_trajectory->get_fit_infos().set_ndof(a_fit_solution.ndof);
				
				
	geomtools::line_3d & line_3d = line_pattern->get_segment();
	line_to_verteces(cluster->get_track(), line_3d);
				
	a_trajectory_solution->grab_best_trajectories().insert(id);
	a_trajectory_solution->grab_trajectories().push_back(h_trajectory);
	} 
	}
	*/
      }
	
    return;
  }

  void TKReconstruct::line_to_verteces(const ConstTKtrackHdl & track, geomtools::line_3d & line_)
  {
    line_.set_first(0.0 * CLHEP::mm,
		    track->get_b() * CLHEP::mm,
		    track->get_d() * CLHEP::mm);

    // Implicit length unit: mm
    double x,y,z;
  
    x = 435.0;
    if( track->get_side() == 0 ) 
      {
	x = -x;
      }		
    y = track->get_a()*x + track->get_b();
	
    if( y > 2505.5 )
      {
	x = (2505.5-track->get_b())/track->get_a();
	y = track->get_a()*x + track->get_b();
		
      }
    else if( y < -2505.5 )
      {
	x = (-2505.5-track->get_b())/track->get_a();
	y = track->get_a()*x + track->get_b();
      }
    z = track->get_c()*x + track->get_d();
	
    if( z > 1550.0 )
      {
	x = (1550.0-track->get_d())/track->get_c();
	y = track->get_a()*x + track->get_b();
	z = track->get_c()*x + track->get_d();
      }
    else if( z < -1550.0 )
      {
	x = (-1550.0-track->get_d())/track->get_c();
	y = track->get_a()*x + track->get_b();
	z = track->get_c()*x + track->get_d();
      }
		
    line_.set_last(x * CLHEP::mm, y * CLHEP::mm, z * CLHEP::mm);
    return;	
  }

} //  end of namespace tkrec

