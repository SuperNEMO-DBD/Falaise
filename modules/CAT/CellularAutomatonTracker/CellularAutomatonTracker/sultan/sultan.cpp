#include "sultan/sultan.h"

#include <vector>
#include <cmath>
#include <sstream>
#include <sys/time.h>

#include <mybhep/system_of_units.h>

namespace SULTAN {

  using namespace std;
  using namespace mybhep;

  //************************************************************
  // Default constructor :
  sultan::sultan(void){
    //*************************************************************
    _set_defaults ();
    return;
  }


  //*************************************************************
  sultan::~sultan() {
    //*************************************************************

  }

  void sultan::_set_defaults ()
  {
    bfield = std::numeric_limits<double>::quiet_NaN ();
    nsigmas = std::numeric_limits<double>::quiet_NaN ();
    level = mybhep::NORMAL;
    m = mybhep::messenger(level);
    cell_distance  = std::numeric_limits<double>::quiet_NaN ();
    xsize = ysize = zsize = std::numeric_limits<double>::quiet_NaN ();
    inner_radius = outer_radius= foil_radius = std::numeric_limits<double>::quiet_NaN ();
    Emin = std::numeric_limits<double>::quiet_NaN ();
    Emax = std::numeric_limits<double>::quiet_NaN ();
    probmin = std::numeric_limits<double>::quiet_NaN ();
    nsigma_r = std::numeric_limits<double>::quiet_NaN ();
    nsigma_z = std::numeric_limits<double>::quiet_NaN ();
    nofflayers = 0;
    first_event_number = -1;
    min_ncells_in_cluster=0;
    ncells_between_triplet_min=0;
    ncells_between_triplet_range=0;
    SuperNemoChannel = false;
    max_time = std::numeric_limits<double>::quiet_NaN ();
    print_event_display = false;
    use_clocks = false;
    use_endpoints = true;
    use_legendre = false;
    clusterize_with_helix_model = false;
    _moduleNR.clear ();
    event_number=-1;
    nevent = 0;
    skipped_events = 0;
    run_time = std::numeric_limits<double>::quiet_NaN ();
    return;
  }



  //*************************************************************
  bool sultan::initialize( void ){
    //*************************************************************

    m.message("\n SULTAN:sultan::initialize: Beginning algorithm sultan \n",mybhep::VERBOSE); fflush(stdout);

    //    clock.start(" sultan: initialize ");

    read_properties();

    nevent = 0;
    event_number=-1;
    skipped_events = 0;
    experimental_legendre_vector = new topology::experimental_legendre_vector(level, probmin);
    experimental_legendre_vector->set_nsigmas(nsigmas);
    std::vector<topology::node> nodes;
    full_cluster_ = new topology::cluster(nodes, level, probmin);
    leftover_cluster_ = new topology::cluster(nodes, level, probmin);
    assigned_cluster_ = new topology::cluster(nodes, level, probmin);

    if( print_event_display )
      root_file_ = new TFile("a.root","RECREATE");


    //    clock.stop(" sultan: initialize ");

    return true;
  }



  //*************************************************************
  bool sultan::finalize() {
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: finalize ");

    if( print_event_display ){
      root_file_->Close();
      delete root_file_;
    }


    m.message("\nSULTAN:sultan::finalize: Ending algorithm sultan \n ",mybhep::NORMAL); fflush(stdout);

    m.message("SULTAN:sultan::finalize: Input events: ", event_number, mybhep::NORMAL);
    m.message("SULTAN:sultan::finalize: Skipped events: ", skipped_events, "(", 100.*skipped_events/event_number, "\%)", mybhep::NORMAL);
    //if( use_clocks ){
    double sequentiation_time = clock.read(" sultan: sequentiate ");
    m.message("SULTAN:sultan::finalize: sequentiation time =: ", sequentiation_time, " ms = " , sequentiation_time/event_number, " ms/event ", mybhep::NORMAL);
    //}

    if( use_clocks )
      clock.stop(" sultan: finalize ");

    if( use_clocks ){
      print_clocks();
      experimental_legendre_vector->print_clocks();
    }

    return true;
  }

  //*************************************************************
  void sultan::print_clocks() {
    //*************************************************************

    clock.dump();

    return;
  }

  //*************************************************************
  void sultan::read_properties(void) {
    //*************************************************************

    if(SuperNemoChannel)
      {
        m.message("SULTAN::sultan::read_properties: SuperNemo kind of data",mybhep::NORMAL);
      }
    else
      {
        m.message("SULTAN::sultan::read_properties: Nemo-3 kind of data",mybhep::NORMAL);
        bfield = 0.0025;
      }

    double ncells_to_skip = (double)ncells_between_triplet_min;

    dist_limit_inf = ncells_to_skip*cell_distance;
    dist_limit_sup = (ncells_to_skip+(double)ncells_between_triplet_range)*cell_distance;

    double emass = 0.511;
    double pmin = sqrt(pow(Emin + emass,2) - pow(emass,2));
    double pmax = sqrt(pow(Emax + emass,2) - pow(emass,2));
    Rmin = pmin/(0.3*bfield);
    Rmax = pmax/(0.3*bfield);

    m.message("SULTAN::sultan::read_properties: SULTAN sultan parameters",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: verbosity print level", level, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: max_time",max_time,"ms",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: print_event_display",print_event_display,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: use_clocks",use_clocks,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: use_endpoints",use_endpoints,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: use_legendre",use_legendre,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: clusterize_with_helix_model",clusterize_with_helix_model,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: probmin", probmin, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: nsigma_r", nsigma_r, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: nsigma_z", nsigma_z, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: nsigmas",nsigmas,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: nofflayers",nofflayers,mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: first event number", first_event_number, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: cell_distance",cell_distance,"mm",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: bfield",bfield,"T",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: Emin",Emin,"MeV",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: Emax",Emax,"MeV",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: xsize is",xsize,"mm",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: ysize is",ysize,"mm",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: zsize is",zsize,"mm",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: foil radius: ",foil_radius,"mm",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties:  min_ncells_in_cluster ", min_ncells_in_cluster, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties:  ncells_between_triplet_min ", ncells_between_triplet_min, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties:  ncells_between_triplet_range ", ncells_between_triplet_range, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties:  ncells to skip ", ncells_to_skip, " dist limit ", dist_limit_inf, " - ", dist_limit_sup, " Rmin ", Rmin, " Rmax ", Rmax, mybhep::NORMAL);

    //    clock.stop(" sultan: read dst properties ");

    return;
  }


  //*************************************************************
  void sultan::make_sequences_from_clusters(){
  //*************************************************************

    // turn clusters to sequences
    for(std::vector<topology::cluster>::const_iterator iclu = made_clusters_.begin(); iclu != made_clusters_.end(); ++iclu){
      create_sequence_from_cluster(&sequences_, *iclu);
    }
    
    return;
  }

  //*************************************************************
  void sultan::reduce_clusters() {
    //*************************************************************

    for (vector<topology::cluster>::iterator icluster = clusters_.begin(); icluster != clusters_.end(); ++icluster){

      // initialize clusters
      *full_cluster_ = *icluster;
      *leftover_cluster_ = *full_cluster_;
      status();
      m.message("SULTAN::sultan::reduce_clusters: prepare to reduce cluster ", icluster - clusters_.begin(), " of ", clusters_.size(), " having", icluster->nodes_.size(), "gg cells ", mybhep::VERBOSE);

      if( use_endpoints ){
	// form clusters between endpoints (foil, calo hits)
	reduce_cluster_based_on_endpoints(icluster - clusters_.begin());
      }

      if( use_legendre ){
	// look for cluster with largest number of triplet-neighbours
	sequentiate_cluster_with_experimental_vector(icluster - clusters_.begin());
	
	//keep track in smarter way of which cluster has the largest number of triplet-neighbours
	//sequentiate_cluster_with_experimental_vector_2(a_cluster, icluster - clusters_.begin());
	
	// look for cluster with largest number of cell-neighbours
	//sequentiate_cluster_with_experimental_vector_3(a_cluster, icluster - clusters_.begin());
	
	// put helices in clusters, assign each cell to the cluster with the largest n of cells
	//sequentiate_cluster_with_experimental_vector_4(a_cluster, icluster - clusters_.begin());
      }

    }

  

    if( use_endpoints ){
      size_t n_iterations = 10;
      std::vector<topology::experimental_helix> the_helices;
      std::vector<topology::experimental_helix> neighbours;
      m.message("SULTAN::sultan::reduce_clusters: assign helices to ", made_clusters_.size(), " clusters ", mybhep::VERBOSE);
      for(std::vector<topology::cluster>::iterator iclu = made_clusters_.begin(); iclu != made_clusters_.end(); ++iclu){
	*leftover_cluster_ = *iclu;
	if( leftover_cluster_->nodes_.size() < min_ncells_in_cluster ) continue;
	experimental_legendre_vector->reset();
	neighbours.clear();
	//form_triplets_from_cells_with_endpoints();
	form_triplets_from_cells();
	form_helices_from_triplets(&the_helices, iclu - made_clusters_.begin());
	if( !the_helices.size() ) continue;
	for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
	  experimental_legendre_vector->add_helix(*hh);
	}
	m.message("SULTAN::sultan::reduce_clusters: cluster ", iclu - made_clusters_.begin(), " has ", leftover_cluster_->nodes_.size(), " nodes, ", triplets_.size(), " triplets, ", the_helices.size(), " helices ", mybhep::VERBOSE);
	if (level >= mybhep::VERBOSE){
	  for(std::vector<topology::cell_triplet>::const_iterator tr = triplets_.begin(); tr!=triplets_.end(); ++tr){
	    tr->print_ids();
	  }	  
	  std::clog << " " << std::endl;
	}	
	//experimental_legendre_vector->reset_helices_errors();
	//experimental_legendre_vector->calculate_metric();
	topology::experimental_helix b = experimental_legendre_vector->max(&neighbours);
	//topology::experimental_helix b = experimental_legendre_vector->max_with_ids();
	//topology::experimental_helix b = experimental_legendre_vector->max_with_metric();
	//b = experimental_legendre_vector->gaussian_max(n_iterations, b);
	iclu->set_helix(b);
	//iclu->recalculate_R();
	//iclu->recalculate(n_iterations);
	iclu->set_cluster_type("helix");
      }
    }

    // turn clusters to sequences
    m.message("SULTAN::sultan::reduce_clusters: make sequences", mybhep::VERBOSE);
    make_sequences_from_clusters();
    sequences_ = clean_up(sequences_);
    status();


  }

  //*************************************************************
  void sultan::reset(){
    //*************************************************************

    made_clusters_.clear();
    sequences_.clear();
    scenarios_.clear();
    full_cluster_->nodes_.clear();
    leftover_cluster_->nodes_.clear();
    triplets_.clear();

  }

  //*************************************************************
  bool sultan::sequentiate(topology::tracked_data & tracked_data_) {
    //*************************************************************
    // main method

    // start clocks
    //if( use_clocks ){
    clock.start(" sultan: sequentiate ","cumulative");
    //}
    clock.start(" sultan: sequentiation ","restart"); // use this one to check late

    // count events
    event_number ++;
    m.message("SULTAN::sultan::sequentiate:  preparing event", event_number, mybhep::VERBOSE);
    if( event_number < first_event_number ){
      m.message("SULTAN::sultan::sequentiate: local_tracking: skip event", event_number, " first event is "
                , first_event_number,  mybhep::VERBOSE);
      //if( use_clocks )
      clock.stop(" sultan: sequentiate ");
      return true;
    }

    // setup input data
    reset();
    tracked_data_.scenarios_.clear();
    clusters_ = tracked_data_.get_clusters (); // clusters from clusterizer
    for( std::vector<topology::cluster>::iterator iclu=clusters_.begin(); iclu!=clusters_.end(); ++iclu){
      iclu->set_probmin(probmin);
      iclu->set_print_level(level);
    }
    if (clusters_.empty ()){
      //if( use_clocks )
      clock.stop(" sultan: sequentiate ");
      return true;
    }
    cells_ = tracked_data_.get_cells();
    calos_ = tracked_data_.get_calos ();


    // reduce input clusters to output clusters
    m.message("SULTAN::sultan::sequentiate: prepare to reduce ", clusters_.size(), " clusters of cells to output clusters with ", calos_.size(), " calos ", mybhep::VERBOSE);
    reduce_clusters();


    // quit if it's too late
    if (late())
      {
        skipped_events ++;
	//if( use_clocks )
	clock.stop(" sultan: sequentiate ");
        return false;
      }


    // turn sequences into a coherent scenario
    make_scenarios(tracked_data_);
    m.message("SULTAN::sultan::sequentiate:  SULTAN has created ", scenarios_.size(), " scenarios and ", sequences_.size(), " sequences for this event ", mybhep::VERBOSE);


    // stop clock
    //if( use_clocks )
    clock.stop(" sultan: sequentiate ");

    return true;
  }

  //*************************************************************
  bool sultan::assign_nodes_based_on_experimental_helix(topology::experimental_helix * b, std::vector<topology::experimental_helix> *helices){
  //*************************************************************

    if( use_clocks )
      clock.start(" sultan: assign_nodes_based_on_experimental_helix ", "cumulative");

    topology::experimental_double dr, dh;
    topology::cluster assigned_cluster;
    assigned_cluster.set_print_level(level);
    assigned_cluster.set_probmin(probmin);
    topology::experimental_point p;
    size_t best_helix_index;
    double angle;

    bool active = false;
    double drmin = mybhep::plus_infinity;
    double dhmin = mybhep::plus_infinity;

    // loop on cells in full cluster
    for (vector<topology::node>::iterator inode = full_cluster_->nodes_.begin(); inode != full_cluster_->nodes_.end(); ++inode){
      if (level >= mybhep::VVERBOSE)
        {
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: cell " << inode->c().id() << " ["; inode->c().ep().dump(); std::clog << "] r "; inode->c().r().dump(); std::clog << std::endl;
        }

      active = false;
      drmin = mybhep::plus_infinity;
      dhmin = mybhep::plus_infinity;
      // loop on helices
      // see if an helix activates inode
      for(std::vector<topology::experimental_helix>::const_iterator ihel=helices->begin(); ihel!=helices->end(); ++ihel){

        ihel->distance_from_cell_measurement(inode->c(), &dr, &dh);
        //ihel->distance_from_cell_center(inode->c(), &dr, &dh);

        if (level >= mybhep::VVERBOSE){
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: distance of helix " << ihel - helices->begin() << " from cell " << inode->c().id() << " dr (" << dr.value() << " +- " << dr.error() << ") dh (" << dh.value() << " +- " << dh.error() << ")" << std::endl;
        }

	if( dr.is_zero__optimist( nsigma_r) && dh.is_zero__optimist( nsigma_z) ){
	  active = true;
          if( fabs(dr.value()) < drmin && fabs(dh.value()) < dhmin ){
            drmin = fabs(dr.value());
            dhmin = fabs(dh.value());
            best_helix_index = ihel - helices->begin();
          }
        }
      } // finish loop on helices



      if( active ){ // the inode is activated by at least 1 helix

        p = helices->at(best_helix_index).position(inode->c().ep());
        if (level >= mybhep::VVERBOSE)
          {
            std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is near (" << p.x().value() << ", " << p.y().value() << ", " << p.z().value() << ")" << std::endl;
          }
        // angle of cell center wrt circle center
        angle = atan2(inode->c().ep().y().value() - b->y0().value(), inode->c().ep().x().value() - b->x0().value());
        inode->set_circle_phi(angle);
        inode->set_ep(p);
	assigned_cluster.nodes_.push_back(*inode);
      }else{
        if (level >= mybhep::VVERBOSE){
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is not near " << std::endl;
        }
      }
    } // finish loop on cells in full_cluster


    bool ok = check_continous_cells(&assigned_cluster, b) && assigned_cluster.nodes_.size();

    if( ok ){
      assigned_cluster.set_cluster_type("helix");
      assigned_cluster.set_helix(*b);
      // add cluster to list of clusters
      made_clusters_.push_back(assigned_cluster);
      leftover_cluster_->remove_nodes(assigned_cluster.nodes());
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  finished cluster [", made_clusters_.size()-1, "] with ", assigned_cluster.nodes_.size(), " nodes, so ", leftover_cluster_->nodes_.size(), " remain unassigned", mybhep::VERBOSE);
    }


    if( use_clocks )
      clock.stop(" sultan: assign_nodes_based_on_experimental_helix ");

    return ok;

  }


  //*************************************************************
  bool sultan::assign_nodes_based_on_experimental_helix(topology::experimental_helix * b, std::vector<size_t> *neighbouring_cells){
  //*************************************************************

    if( use_clocks )
      clock.start(" sultan: assign_nodes_based_on_experimental_helix ", "cumulative");

    topology::experimental_double dr, dh;
    vector<topology::node> leftover_nodes_copy=leftover_cluster_->nodes_;
    assigned_cluster_->nodes_.clear();
    leftover_cluster_->nodes_.clear();
    topology::experimental_point p;
    // size_t best_helix_index;
    topology::cluster assigned_cluster;
    double angle;

    bool active = false;
    //double drmin = mybhep::plus_infinity;
    //double dhmin = mybhep::plus_infinity;

    for (vector<topology::node>::iterator inode = full_cluster_->nodes_.begin(); inode != full_cluster_->nodes_.end(); ++inode){
      if (level >= mybhep::VVERBOSE)
        {
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: cell " << inode->c().id() << " ["; inode->c().ep().dump(); std::clog << "] r "; inode->c().r().dump(); std::clog << std::endl;
        }

      active = false;

      if( std::find(neighbouring_cells->begin(), neighbouring_cells->end(), inode->c().id()) != neighbouring_cells->end() )
        active = true;


      if( active ){
        p = b->position(inode->c().ep());
        if (level >= mybhep::VVERBOSE)
          {
            std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is near (" << p.x().value() << ", " << p.y().value() << ", " << p.z().value() << ")" << std::endl;
          }
        // angle of cell center wrt circle center
        angle = atan2(inode->c().ep().y().value() - b->y0().value(), inode->c().ep().x().value() - b->x0().value());
        inode->set_circle_phi(angle);
        inode->set_ep(p);
        assigned_cluster_->nodes_.push_back(*inode);
      }else{
        if (level >= mybhep::VVERBOSE){
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is not near " << std::endl;
        }
        if( std::find(leftover_nodes_copy.begin(), leftover_nodes_copy.end(), *inode) != leftover_nodes_copy.end() )
          leftover_cluster_->nodes_.push_back(*inode);
      }

    }

    bool ok = check_continous_cells(&assigned_cluster, b);

    m.message("SULTAN::sultan::assign_nodes_based_on_experimental_helix: associated ", assigned_cluster_->nodes_.size(), " nodes to this helix out of ", full_cluster_->nodes_.size(), " so ", leftover_cluster_->nodes_.size(), " remain unassigned - initially there were ", leftover_nodes_copy.size(),  " -, continous ", ok, mybhep::VERBOSE);

    if( use_clocks )
      clock.stop(" sultan: assign_nodes_based_on_experimental_helix ");

    return ok &&
      (leftover_cluster_->nodes_.size() < leftover_nodes_copy.size()) &&
      (assigned_cluster_->nodes_.size() ) ;

  }


  //*************************************************************
  bool sultan::check_continous_cells(topology::cluster * given_cluster, topology::experimental_helix *b){
  //*************************************************************
    // - take given cluster as input
    // - if is it continous, it stays as is
    // - otherwise it becomes its longest continous piece
    // - if it is continous and good, return true

    if( use_clocks )
      clock.start(" sultan: continous ", "cumulative");

    int min_length = 3;

    // get lengths of each piece
    size_t longest_piece_first, longest_piece_last;
    vector<size_t> length_of_piece;
    size_t maxlength;
    std::vector<size_t> the_first_cell_of_piece;
    
    given_cluster->break_into_continous_pieces(nofflayers, cell_distance);

    bool ok = given_cluster->is_continous();

    if( !ok ){
      // get lengths of each piece

      maxlength = given_cluster->max_length_of_piece();

      if( maxlength >= min_length ){
        ok = true;

	*given_cluster =  given_cluster->longest_piece();
	if (level >= mybhep::VERBOSE){
	  std::clog << "SULTAN::sultan::check_continous_cells:  longest piece has length " << given_cluster->nodes_.size() << ": ( ";
	  for(std::vector<topology::node>::const_iterator inode=given_cluster->nodes_.begin(); inode!=given_cluster->nodes_.end(); ++inode)
	    std::clog << inode->c().id() << " ";
	  std::clog << ")" << std::endl;
	}
      }
	
      if( given_cluster->nodes_.size() != maxlength ){
	m.message("SULTAN::sultan::check_continous_cells:  problem: assigned cluster size", given_cluster->nodes_.size(), " length ", maxlength, mybhep::NORMAL);
      }
      
    }

    if( ok ) ok = given_cluster->is_good();
    
    if( ok )
      for(std::vector<topology::node>::const_iterator inode=given_cluster->nodes_.begin(); inode!=given_cluster->nodes_.end(); ++inode)
	b->add_id(inode->c().id());
    
    if( use_clocks )
      clock.stop(" sultan: continous ");
    
    return ok;
  }
  

  //*************************************************************
  bool sultan::get_longest_piece(topology::cluster * given_cluster, topology::node a, topology::node b, topology::cluster * longest_piece){
  //*************************************************************

    if( use_clocks )
      clock.start(" sultan: get_longest_piece ", "cumulative");

    int min_length = 2;

    given_cluster->set_probmin(probmin);
    given_cluster->set_print_level(level);
    topology::cluster cluster_copy = *given_cluster;

    cluster_copy.break_into_continous_pieces( nofflayers, cell_distance);
    *longest_piece = cluster_copy.longest_piece();

    if( longest_piece->nodes_.size() >= min_length &&
	( (longest_piece->nodes_.front().c().id() == a.c().id() && longest_piece->nodes_.back().c().id() == b.c().id()) ||
	  (longest_piece->nodes_.back().c().id() == a.c().id() && longest_piece->nodes_.front().c().id() == b.c().id()) ) ){
      if( use_clocks )
	clock.stop(" sultan: get_longest_piece ");
      return true;
    }


    if( use_clocks )
      clock.stop(" sultan: get_longest_piece ");
    return false;
  }



  //*************************************************************
  bool sultan::form_triplets_from_cells(){
  //*************************************************************
    // combine leftover_cluster nodes
    // to produce triplets (A, B, C) such that
    // the distances A-B and B-C are in specified range


    if( use_clocks )
      clock.start(" sultan: form_triplets_from_cells ", "cumulative");

    reset_triplets();

    m.message("SULTAN::sultan::form_triplets_from_cells: calculate triples for ", leftover_cluster_->nodes_.size(), " nodes, minimum ", min_ncells_in_cluster, mybhep::VVERBOSE);

    if( leftover_cluster_->nodes_.size() < min_ncells_in_cluster ) {
      // not enough cells to form a cluster
      if( use_clocks )
	clock.stop(" sultan: form_triplets_from_cells ");
      return false;
    }


    topology::cell_triplet *ccc;
    topology::experimental_helix best_helix;

    std::vector<topology::node> assigned_nodes_best;
    double distance12, distance23, distance13;
    double dmin1, dmin2;

    for(std::vector<topology::node>::const_iterator inode=leftover_cluster_->nodes_.begin(); inode != leftover_cluster_->nodes_.end()-2; ++inode){
      for(std::vector<topology::node>::const_iterator jnode=inode+1; jnode != leftover_cluster_->nodes_.end()-1; ++jnode){

        if( jnode == inode ) continue;

	distance12 = (inode->c().ep().hor_distance(jnode->c().ep())).value();

        for(std::vector<topology::node>::const_iterator knode=jnode+1; knode != leftover_cluster_->nodes_.end(); ++knode){

          if( knode == inode ) continue;
          if( knode == jnode ) continue;

          distance23 = (jnode->c().ep().hor_distance(knode->c().ep())).value();
          distance13 = (inode->c().ep().hor_distance(knode->c().ep())).value();

          dmin1 = std::min( distance12,  distance13 );
          dmin2 = std::min( distance12,  distance23 );
	  if( dmin1 == dmin2 )
	    dmin2 = std::min( distance13,  distance23 );

          m.message(" (triplet " , inode->c().id() , ", " , jnode->c().id() , ", " , knode->c().id() , ") dmin1 " , dmin1, " dmin2 ", dmin2 , mybhep::VVERBOSE);

          if( dmin1 < dist_limit_inf || dmin1 > dist_limit_sup ) continue;
          if( dmin2 < dist_limit_inf || dmin2 > dist_limit_sup ) continue;

          ccc = new topology::cell_triplet(inode->c(), jnode->c(), knode->c(), level);
          triplets_.push_back(*ccc);

          delete ccc;

          m.message(" adding triplet, total " , triplets_.size() , mybhep::VVERBOSE);

        }
      }
    }

    if( use_clocks )
      clock.stop(" sultan: form_triplets_from_cells ");

    m.message("SULTAN::sultan::form_triplets_from_cells: sultan: the ", leftover_cluster_->nodes_.size(), " cells have been combined into ", triplets_.size(), " triplets ", mybhep::VERBOSE);

    return true;

  }


  //*************************************************************
  bool sultan::form_triplets_from_cells_with_endpoints(){
  //*************************************************************
    // combine leftover_cluster nodes
    // to produce triplets (A, B, C) with A and C fixed


    if( use_clocks )
      clock.start(" sultan: form_triplets_from_cells_with_endpoints ", "cumulative");

    reset_triplets();

    m.message("SULTAN::sultan::form_triplets_from_cells_with_endpoints: calculate triples for ", leftover_cluster_->nodes_.size(), " nodes, minimum ", min_ncells_in_cluster, mybhep::VVERBOSE);

    if( leftover_cluster_->nodes_.size() < min_ncells_in_cluster ) {
      // not enough cells to form a cluster
      if( use_clocks )
	clock.stop(" sultan: form_triplets_from_cells_with_endpoints ");
      return false;
    }


    topology::cell_triplet *ccc;

    const topology::cell A = leftover_cluster_->nodes_.begin()->c();
    const topology::cell C = leftover_cluster_->nodes_.back().c();

    if( A.id() == C.id() ){
      if( use_clocks )
	clock.stop(" sultan: form_triplets_from_cells_with_endpoints ");
      return false;
    }

    for(std::vector<topology::node>::const_iterator jnode=leftover_cluster_->nodes_.begin(); jnode != leftover_cluster_->nodes_.end(); ++jnode){
      
      if( jnode->c().id() == A.id() ) continue;
      if( jnode->c().id() == C.id() ) continue;
      
      ccc = new topology::cell_triplet(A, jnode->c(), C, level);
      triplets_.push_back(*ccc);
      
      delete ccc;
      
      m.message(" adding triplet, total " , triplets_.size() , mybhep::VVERBOSE);
      
    }

    if( use_clocks )
      clock.stop(" sultan: form_triplets_from_cells_with_endpoints ");

    m.message("SULTAN::sultan::form_triplets_from_cells_with_endpoints: sultan: the ", leftover_cluster_->nodes_.size(), " cells have been combined into ", triplets_.size(), " triplets ", mybhep::VERBOSE);

    return true;

  }


  //*************************************************************
  bool sultan::form_helices_from_triplets(std::vector<topology::experimental_helix> *the_helices, size_t icluster){
  //*************************************************************

    if( use_clocks )
      clock.start(" sultan: form_helices_from_triplets ", "cumulative");

    m.message("SULTAN::sultan::form_helices_from_triplets:  calculate helices for ", triplets_.size(), " triplets ", mybhep::VVERBOSE);

    the_helices->clear();

    if( triplets_.size() == 0 ) {
      if( use_clocks )
	clock.stop(" sultan: form_helices_from_triplets ");
      return false;
    }

    // size_t ncells;
    // size_t ncells_max = 0;
    topology::experimental_helix best_helix;

    std::vector<topology::node> assigned_nodes_best;

    std::vector<topology::experimental_helix> helices;


    for(std::vector<topology::cell_triplet>::iterator ccc = triplets_.begin(); ccc!= triplets_.end(); ++ccc){

      ccc->calculate_helices(Rmin, Rmax, nsigmas);
      helices = ccc->helices();

      the_helices->insert(the_helices->end(),helices.begin(),helices.end());

      m.message("SULTAN::sultan::form_helices_from_triplets:  adding " , helices.size() , " helices, total " , the_helices->size() , mybhep::VVERBOSE);

    }

    if( use_clocks )
      clock.stop(" sultan: form_helices_from_triplets ");

    if( print_event_display && event_number < 10 ){
      if( use_clocks )
	clock.start(" sultan: form_helices_from_triplets : print_event_display ", "cumulative");

      root_file_->cd();

      if( the_helices->size() ){

        std::ostringstream title_ss;
        title_ss << "event_" << event_number << "_cluster_" << icluster;
        TString tree_name = title_ss.str().c_str();
        TTree *root_tree = new TTree(tree_name,tree_name);

        TString x0_name=Form("x0_"+tree_name, event_number, icluster);
        TString y0_name=Form("y0_"+tree_name, event_number, icluster);
        TString z0_name=Form("z0_"+tree_name, event_number, icluster);
        TString R_name=Form("R_"+tree_name, event_number, icluster);
        TString H_name=Form("H_"+tree_name, event_number, icluster);

        TString x0_error_name=Form("x0_error_"+tree_name, event_number, icluster);
        TString y0_error_name=Form("y0_error_"+tree_name, event_number, icluster);
        TString z0_error_name=Form("z0_error_"+tree_name, event_number, icluster);
        TString R_error_name=Form("R_error_"+tree_name, event_number, icluster);
        TString H_error_name=Form("H_error_"+tree_name, event_number, icluster);

        double x0_value = 0.; 
	double x0_error = 0.;
        double y0_value = 0.; 
	double y0_error = 0.;
        double z0_value = 0.; 
	double z0_error = 0.;
        double R_value = 0.; 
	double R_error = 0.;
        double H_value = 0.; 
	double H_error = 0.;
	double weight;

        root_tree->Branch(x0_name,&x0_value);
        root_tree->Branch(y0_name,&y0_value);
        root_tree->Branch(z0_name,&z0_value);
        root_tree->Branch(R_name,&R_value);
        root_tree->Branch(H_name,&H_value);

        root_tree->Branch(x0_error_name,&x0_error);
        root_tree->Branch(y0_error_name,&y0_error);
        root_tree->Branch(z0_error_name,&z0_error);
        root_tree->Branch(R_error_name,&R_error);
        root_tree->Branch(H_error_name,&H_error);

        //std::clog << " tree_name " << tree_name.Data() << " helices " << the_helices->size() << std::endl;

        for(std::vector<topology::experimental_helix>::const_iterator ihel=the_helices->begin();
            ihel != the_helices->end(); ++ihel){
          x0_value = ihel->x0().value();
          y0_value = ihel->y0().value();
          z0_value = ihel->z0().value();
          R_value = ihel->R().value();
          H_value = ihel->H().value();

          x0_error = ihel->x0().error();
          y0_error = ihel->y0().error();
          z0_error = ihel->z0().error();
          R_error = ihel->R().error();
          H_error = ihel->H().error();

          weight = 1./(pow(x0_error,2) +
                       pow(y0_error,2) +
                       pow(z0_error,2) +
                       pow(R_error,2) +
                       pow(H_error,2)
                       );
          //weight += 0.0; // trick to avoid warn

          root_tree->Fill();

          //std::clog << " helix " << ihel - the_helices->begin() << " x0 " << x0_value << " +- " << x0_error     << " y0 " << y0_value << " +- " << y0_error     << " z0 " << z0_value << " +- " << z0_error     << " R " << R_value << " +- " << R_error        << " H " << H_value << " +- " << H_error << " weight " << weight <<  std::endl;

        }

        root_tree->Write();
        delete root_tree;
      }
      if( use_clocks )
	clock.stop(" sultan: form_helices_from_triplets : print_event_display ");
    }

    m.message("SULTAN::sultan::form_helices_from_triplets:  sultan: the", triplets_.size(), " triplets have given rise to ", the_helices->size(), " helices ", mybhep::VVERBOSE);

    return true;

  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector( size_t icluster) {
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector ","cumulative");

    // reset
    experimental_legendre_vector->reset();

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  sequentiate cluster with " << full_cluster_->nodes_.size() << " nodes " << std::endl; fflush(stdout);
      }

    // need at least 3 nodes
    if( full_cluster_->nodes_.size() < 3 ){
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector ");
      return;
    }

    topology::experimental_helix b;
    topology::sequence *s;
    std::vector<topology::experimental_helix> neighbours;
    std::vector<topology::experimental_helix> the_helices;
    std::vector<topology::cluster> newly_made_clusters;

    size_t n_of_leftover_nodes;

    // while loop: keep trying as long as 
    // one can form triplets and helices out of them
    while( form_triplets_from_cells() && form_helices_from_triplets(&the_helices, icluster) ){

      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ","cumulative");

      // reset
      experimental_legendre_vector->reset();
      neighbours.clear();
      n_of_leftover_nodes = leftover_cluster_->nodes_.size();

      if (level >= mybhep::VERBOSE){
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

      // quit if no helices were built
      if( !the_helices.size() ){
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ");
        break;
      }
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ");


      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: add helix ","cumulative");


      // add all helices to legendre_vector
      for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
        experimental_legendre_vector->add_helix(*hh);
      }

      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: add helix ");

      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ","cumulative");
      // get the best helix in the vector
      b = experimental_legendre_vector->max(&neighbours);

      if( !b.ids().size() ){
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ");
        break;
      }


      if (level >= mybhep::VERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  best helix: " ; b.dump();
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  associated " << neighbours.size() << " helices " << std::endl;
        }

      if( use_clocks ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ");
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: assign ","cumulative");
      }

      // assign all nodes you can to this helix
      bool ok = assign_nodes_based_on_experimental_helix(&b, &neighbours);

      if( !ok ){
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: assign ");
        break;
      }

      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: assign ");
      
      if( print_event_display ) break;
      

      if( leftover_cluster_->nodes_.size() == n_of_leftover_nodes ){
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector: leftover nodes have not been reduced; break ", mybhep::VERBOSE); fflush(stdout);
	break;
      }

    }



    if( use_clocks )
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector ");

    return;
  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_2(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2 ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2 ");
      return;
    }

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2: sequentiate cluster with " << cluster_.nodes_.size() << " nodes, leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

    topology::experimental_helix b;
    vector<topology::node> leftover_nodes;
    topology::sequence *s;
    topology::experimental_point *center;
    topology::experimental_double radius;
    topology::experimental_double pitch;
    std::vector<topology::experimental_helix> neighbours;
    std::vector<topology::experimental_helix> the_helices;

    leftover_nodes = cluster_.nodes_;

    topology::cluster_of_experimental_helices best_cluster;
    bool found;

    while( form_triplets_from_cells() && form_helices_from_triplets(&the_helices, icluster) ){

      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ","cumulative");

      assigned_cluster_->nodes_.clear();
      experimental_legendre_vector->reset();

      if (level >= mybhep::VERBOSE){
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  there are " << leftover_nodes.size() << " leftover nodes and " << assigned_cluster_->nodes_.size() << " assigned nodes, the leg vector has " << experimental_legendre_vector->helices().size() << " helices and " << the_helices.size() << " have been prepared " << std::endl; fflush(stdout);
      }

      if( !the_helices.size() ){
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ");
        break;
      }
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ");


      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: add helix to clusters ","cumulative");

      for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
        experimental_legendre_vector->add_helix_to_clusters(*hh);

        if( experimental_legendre_vector->max_cluster(&b, &found).helices().size() >= the_helices.size()/2. )
          break;

      }

      if( use_clocks ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: add helix to clusters ");
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: max ","cumulative");
      }

      found = false;
      best_cluster = experimental_legendre_vector->max_cluster(&b, &found);

      if( !found ){
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: max ");
        break;
      }

      if (late())
        {
          return;
        }

      neighbours = best_cluster.helices();

      if (level >= mybhep::VERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  best helix with " << neighbours.size() << " neighbours is: " ; b.dump();
        }

      if( use_clocks ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: max ");
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: assign ","cumulative");
      }

      bool ok = assign_nodes_based_on_experimental_helix(&b, &neighbours);

      if( ok ){
        s = new topology::sequence(assigned_cluster_->nodes_, level, probmin);
        make_name(*s);
        center = new topology::experimental_point(b.x0(),b.y0(),b.z0());
        radius.set(b.R());
        pitch.set(b.H());
        s->set_helix(b);
        sequences_.push_back(*s);
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  finished track [", sequences_.size()-1, "] with ", s->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
        delete s;
        delete center;
      }else{
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: assign ");
        break;
      }

      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: assign ");

      if( print_event_display ) break;

    }


    if( use_clocks )
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2 ");

    return;
  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_3(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3 ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3 ");
      return;
    }

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  sequentiate cluster with " << cluster_.nodes_.size() << " nodes, leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

    topology::experimental_helix b;
    vector<topology::node> leftover_nodes;
    topology::sequence *s;
    topology::experimental_point *center;
    topology::experimental_double radius;
    topology::experimental_double pitch;
    std::vector<size_t> neighbouring_cells;
    std::vector<topology::experimental_helix> the_helices;

    leftover_nodes = cluster_.nodes_;

    while( form_triplets_from_cells() && form_helices_from_triplets(&the_helices, icluster) ){

      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: clean ","cumulative");

      assigned_cluster_->nodes_.clear();
      experimental_legendre_vector->reset();
      neighbouring_cells.clear();

      if (level >= mybhep::VERBOSE){
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

      if( !the_helices.size() ){
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: clean ");
        break;
      }
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: clean ");


      if( use_clocks )
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: add helix ","cumulative");

      for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
        experimental_legendre_vector->add_helix(*hh);
      }

      if( use_clocks ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: add helix ");
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: max ","cumulative");
      }

      b = experimental_legendre_vector->max(&neighbouring_cells);

      if( !b.ids().size() ){
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: max ");
        break;
      }


      if (level >= mybhep::VERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  best helix: " ; b.dump();
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  associated " << neighbouring_cells.size() << " cells to this cluster " << std::endl;
        }

      if( use_clocks ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: max ");
	clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: assign ","cumulative");
      }

      bool ok = assign_nodes_based_on_experimental_helix(&b, &neighbouring_cells);

      if( ok ){
        s = new topology::sequence(assigned_cluster_->nodes_, level, probmin);
        make_name(*s);
        center = new topology::experimental_point(b.x0(),b.y0(),b.z0());
        radius.set(b.R());
        pitch.set(b.H());
        s->set_helix(b);
        sequences_.push_back(*s);
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  finished track [", sequences_.size()-1, "] with ", s->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
        delete s;
        delete center;
      }else{
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
	if( use_clocks )
	  clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: assign ");
        break;
      }

      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: assign ");

      if( print_event_display ) break;

    }


    if( use_clocks )
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3 ");

    return;
  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_4(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4 ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
      if( use_clocks )
	clock.stop("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4:  sultan: sequentiate_cluster_with_experimental_vector_4 ");
      return;
    }

    topology::experimental_helix b;
    topology::sequence *s;
    topology::experimental_point *center;
    topology::experimental_double radius;
    topology::experimental_double pitch;
    std::vector<topology::experimental_helix> neighbours;
    std::vector<topology::experimental_helix> the_helices;

    std::vector<topology::cluster_of_experimental_helices> best_cluster;
    // bool found;
    topology::experimental_point p;
    double angle;

    //form_triplets_from_cells(cluster_.nodes_);
    form_triplets_from_cells();
    form_helices_from_triplets(&the_helices, icluster);

    if( use_clocks )
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: clean ","cumulative");

    experimental_legendre_vector->reset();

    if( use_clocks )
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: clean ");

    if( !the_helices.size() ){
      if( use_clocks )
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4 ");
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: could not make a track ", mybhep::VERBOSE); fflush(stdout);
      return;
    }

    if( use_clocks )
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: add helix to clusters ","cumulative");

    // bool force_neighbours_to_have_different_ids = false;
    for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
      experimental_legendre_vector->add_helix_to_clusters(*hh);
    }

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: sultan: the " <<  the_helices.size() << " helices have been combined into " << experimental_legendre_vector->clusters().size() << " clusters of helices " << std::endl;
      }

    if( use_clocks ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: add helix to clusters ");
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: find clusters ","cumulative");
    }

    bool assigned_node;
    std::vector<topology::node> unclustered_hits;
    for(std::vector<topology::node>::const_iterator in = cluster_.nodes_.begin(); in!=cluster_.nodes_.end(); ++in){
      assigned_node = experimental_legendre_vector->assign_cell(in->c().id());
      if (level >= mybhep::VVERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: cell " << in->c().id() << " assigned " << assigned_node << " to cluster " << std::endl;
        }
      if( !assigned_node )
        unclustered_hits.push_back(*in);
    }

    if( use_clocks ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: find clusters ");
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: assign ","cumulative");
    }

    std::vector<size_t> ids;
    std::vector<topology::node> nodes;
    std::vector<topology::cluster_of_experimental_helices> best_clusters = experimental_legendre_vector->clusters();

    std::vector<topology::sequence> sequences;
    for(std::vector<topology::cluster_of_experimental_helices>::const_iterator ic=best_clusters.begin(); ic!=best_clusters.end(); ++ic){
      nodes.clear();
      ids = ic->assigned_ids();

      for(std::vector<size_t>::const_iterator id=ids.begin(); id!=ids.end(); ++id){
        for(std::vector<topology::node>::const_iterator inode = cluster_.nodes_.begin(); inode!=cluster_.nodes_.end(); ++inode){
          if( *id == inode->c().id() ){
            nodes.push_back(*inode);
            break;
          }
        }
      }

      if (level >= mybhep::VVERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: cluster " << ic - best_clusters.begin() << " has " << ic->ids().size() << " different cells and " << ids.size() << " assigned cells and " << nodes.size() << " assigned nodes " << std::endl;
        }

      if( nodes.size() ){

        for(std::vector<topology::node>::iterator inode = nodes.begin(); inode!=nodes.end(); ++inode){
          p = b.position(inode->c().ep());
          // angle of cell center wrt circle center
          angle = atan2(inode->c().ep().y().value() - b.y0().value(), inode->c().ep().x().value() - b.x0().value());
          inode->set_circle_phi(angle);
          inode->set_ep(p);
        }

        s = new topology::sequence(nodes, level, probmin);
        make_name(*s);
        b = ic->average_helix();
        center = new topology::experimental_point(b.x0(),b.y0(),b.z0());
        radius.set(b.R());
        pitch.set(b.H());
        s->set_helix(b);
        sequences.push_back(*s);
        delete s;
        delete center;
      }
    }

#if 0
    if( unclustered_hits.size() ){
      s = new topology::sequence(unclustered_hits, level, probmin);
      make_name(*s);
      sequences.push_back(*s);
      //m.message(" finished track [", sequences.size()-1, "] with ", s->nodes_.size(), " unclustered hits ", mybhep::VERBOSE); fflush(stdout);
      delete s;
    }
#endif

    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: sultan: the ", experimental_legendre_vector->clusters().size(), " clusters of helices have been reduced to ", sequences.size(), " candidate sequences; ", unclustered_hits.size(), " hits remain unclustered ", mybhep::VERBOSE); fflush(stdout);

    sequences = clean_up(sequences);


    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: sultan: after clean_up, ", sequences.size(), "sequences remain", mybhep::VERBOSE); fflush(stdout);

    if (level >= mybhep::VERBOSE)
      {
        for(std::vector<topology::sequence>::const_iterator is=sequences.begin(); is!=sequences.end(); ++is)
          m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: sequence [", is - sequences.begin(), "] has ", is->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
      }

    sequences_.insert(sequences_.end(),sequences.begin(),sequences.end());;

    if( use_clocks ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: assign ");
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4 ");
    }

    return;
  }


  //*************************************************************
  bool sultan::check_if_cell_is_near_calo(topology::cell c){
  //*************************************************************

    std::string cview;
    if( c.is_near_calo() ) cview="x";
    else if( c.is_near_xcalo() ) cview="y";
    else if( c.is_near_gveto() ) cview="z";
    else{
      m.message("SULTAN::sultan::check_if_cell_is_near_calo: problem: cell is not near any calo", mybhep::NORMAL);
      return false;
    }

    m.message("SULTAN::sultan::check_if_cell_is_near_calo: check if cell", c.id(), " with view ", cview, " is near one of ", calos_.size(), " calos", mybhep::VVERBOSE);

    topology::experimental_vector dist;
    topology::experimental_point cp;
    topology::experimental_point p = c.ep();
    topology::experimental_double distance;

    p.set_x(topology::experimental_double(p.x().value(), cell_distance));
    p.set_y(topology::experimental_double(p.y().value(), cell_distance));

    // increase the error on calo sizes to allow for diagonal connections
    double factor=2.;

    // loop on calo hits
    for(std::vector<topology::calorimeter_hit>::const_iterator icalo=calos_.begin(); icalo<calos_.end(); ++icalo){

      m.message("SULTAN::sultan::check_if_cell_is_near_calo: calorimeter ", icalo - calos_.begin(), " with view ", icalo->pl().view(), mybhep::VVERBOSE);

      if( strcmp(icalo->pl().view().c_str(), cview.c_str()) != 0 ) continue;

      cp = icalo->pl().face();
      cp.set_x(topology::experimental_double(cp.x().value(), factor*icalo->pl().sizes().x().value()));
      cp.set_y(topology::experimental_double(cp.y().value(), factor*icalo->pl().sizes().y().value()));
      cp.set_z(topology::experimental_double(cp.z().value(), factor*icalo->pl().sizes().z().value()));
      dist = cp - p;
      distance = dist.length();

      m.message("SULTAN::sultan::check_if_cell_is_near_calo: cell - calo distance", distance.value(), " +- ", distance.error(), mybhep::VVERBOSE);

      if( distance.is_zero__optimist(nsigmas) ) return true;

    }

    return false;

  }

  //*************************************************************
  std::vector<topology::cluster> sultan::get_clusters_of_cells_to_be_used_as_end_points(){
  //*************************************************************

    m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: looking for clusters of endpoints in a cluster of ", leftover_cluster_->nodes_.size(), " cells, with ", calos_.size(), " calo hits", mybhep::VVERBOSE);

    if( use_clocks )
      clock.start(" sultan: get_clusters_of_cells_to_be_used_as_end_points ","cumulative");

    std::vector<topology::cluster> clusters;

    bool node_has_been_added_to_cluster;

    bool on_foil, on_calo, on_xcalo, on_gveto, on_calo_hit;

    // loop on all leftover nodes
    for(std::vector<topology::node>::const_iterator inode=leftover_cluster_->nodes_.begin(); inode<leftover_cluster_->nodes_.end(); ++inode){

      on_foil=inode->c_.is_near_foil();
      on_calo=inode->c_.is_near_calo();
      on_xcalo=inode->c_.is_near_xcalo();
      on_gveto=inode->c_.is_near_gveto();

      // check if node "inode" can be used as an end point (is it on the foil? on the calo wall? on the xcalo wall? it is always on the gveto!)
      m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: cell " , inode->c_.id() , " is: near foil " , on_foil
		, " near calo " , on_calo
		, " near xcalo " , on_xcalo , mybhep::VVERBOSE);

      // hit must be near some boundary (except gveto: all cells are close to gveto)
      if( !on_foil &&
	  !on_calo &&
	  !on_xcalo )
	continue;

      // hit on a calo must be near a calo hit
      if( !on_foil ){
	on_calo_hit = check_if_cell_is_near_calo(inode->c_);
	m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: cell " , inode->c().id() , " is on calo " , on_calo, " on xcalo ", on_xcalo, " on calo hit ", on_calo_hit , mybhep::VVERBOSE);
	if( !on_calo_hit ) 
	  continue;
      }

      // inode can be used as an end point

      node_has_been_added_to_cluster = false;

      // loop on clusters already prepared, see if "inode" should be added to an existing cluster or form a new one
      for( std::vector<topology::cluster>::iterator iclu= clusters.begin(); iclu!=clusters.end(); ++iclu){
	if( node_has_been_added_to_cluster ) break; // go back to loop on input nodes
	const std::vector<topology::node> & cluster_nodes = iclu->nodes();
	for( std::vector<topology::node>::const_iterator jnode=cluster_nodes.begin(); jnode!=cluster_nodes.end(); ++jnode){
	  // side-by-side cells
	  bool side_by_side_cells = (jnode->c().near_level(inode->c(), nofflayers, cell_distance) == 3);
	  if( side_by_side_cells ){  
	    topology::experimental_double vertical_dist = jnode->c().ep().z() - inode->c().ep().z();
	    if( !vertical_dist.is_zero__optimist(nsigma_z) ) continue;
	    // "inode" gets added to the same cluster as jnode
	    iclu->nodes_.push_back(*inode);
	    node_has_been_added_to_cluster = true;
	    m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: cell " , inode->c().id() , " is added to existing cluster becaues it's near cell " , jnode->c().is_near_foil()  , mybhep::VVERBOSE);
	    break; // go back to loop on clusters
	  }
	}
      }

      if( !node_has_been_added_to_cluster ) {
	// form a new cluster with "inode"
	m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: cell " , inode->c().id() , " is added as new cluster " , mybhep::VVERBOSE);
	topology::cluster cluster;
	cluster.nodes_.push_back(*inode);
	clusters.push_back(cluster);
      }

    } // finish loop in input nodes


    m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: " , clusters.size() , " clusters of endpoints have been found in a cluster of ", leftover_cluster_->nodes_.size(), " cells " , mybhep::VERBOSE);
    if (level >= mybhep::VERBOSE){
      for( std::vector<topology::cluster>::iterator iclu= clusters.begin(); iclu!=clusters.end(); ++iclu){
	std::clog <<"SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: cluster of endpoints [" << iclu - clusters.begin() << "] ";
	const std::vector<topology::node> & cluster_nodes = iclu->nodes();
	for( std::vector<topology::node>::const_iterator jnode=cluster_nodes.begin(); jnode!=cluster_nodes.end(); ++jnode){
	  std::clog <<jnode->c().id() << " ";
	}
	if( cluster_nodes.size() ){
	  topology::cell fc = cluster_nodes[0].c();
	  if( fc.is_near_foil() ) std::clog << " near foil ";
	  if( fc.is_near_calo() ) std::clog << " near calo ";
	  if( fc.is_near_xcalo() ) std::clog << " near xcalo ";
	}
	std::clog <<" " << std::endl;
      }
    }


    if( use_clocks )
      clock.stop(" sultan: get_clusters_of_cells_to_be_used_as_end_points ");

    return clusters;

  }

  //*************************************************************
  void sultan::reduce_cluster__with_2_endpoints(size_t icluster, bool * cluster_is_finished, std::vector<topology::cluster> * cs_given_endpoints, std::vector<topology::node>::const_iterator inode , std::vector<topology::node>::const_iterator jnode  ) {
    //*************************************************************
    m.message("SULTAN::sultan::reduce_cluster__with_2_endpoints: ", mybhep::VVERBOSE);
    // put in cs_given_endpoints all clusters between inode and jnode

    if( use_clocks )
      clock.start(" sultan: reduce_cluster__with_2_endpoints ","cumulative");

    *cs_given_endpoints = get_clusters_from(*inode, *jnode, icluster, cluster_is_finished);
    
    if (level >= mybhep::VERBOSE){
      std::clog << "SULTAN::sultan::reduce_cluster__with_2_endpoints:  endpoints : " << inode->c().id() << " -> " << jnode->c().id() << " contribute " << cs_given_endpoints->size() << " clusters of cells containing: (" ;
      for( std::vector<topology::cluster>::const_iterator pclu=cs_given_endpoints->begin(); pclu!=cs_given_endpoints->end(); ++pclu){
	std::clog << pclu->nodes().size() << " ";
      }
      
      std::clog << ") cells " << std::endl;
    }
    *cs_given_endpoints = clean_up(*cs_given_endpoints);
    m.message("SULTAN::sultan::reduce_cluster__with_2_endpoints:  after cleaning ", cs_given_endpoints->size(), " clusters remain with endpoints : ", inode->c().id(), " - " , jnode->c().id(), " cluster_is_finished: ", *cluster_is_finished, mybhep::VERBOSE);

    if( use_clocks )
      clock.stop(" sultan: reduce_cluster__with_2_endpoints ");

    return;

  }

  //*************************************************************
  void sultan::reduce_cluster__with_2_clusters_of_endpoints(size_t icluster, bool * cluster_is_finished,  const std::vector<topology::node> &inodes,  const std::vector<topology::node> &jnodes, std::vector<topology::cluster> * cs ) {
    //*************************************************************
    m.message("SULTAN::sultan::reduce_cluster__with_2_clusters_of_endpoints:  2nd cluster of endpoints has " , jnodes.size() , " nodes " , mybhep::VVERBOSE);
    // we expect at most 1 cluster between 2 clusters of endpoints

    if( use_clocks )
      clock.start(" sultan: reduce_cluster__with_2_clusters_of_endpoints ","cumulative");
    
    std::vector<topology::cluster> cs_given_endpoints;
    std::vector<topology::cluster> cs_given_clusters_of_endpoints;
    
    for( std::vector<topology::node>::const_iterator inode = inodes.begin(); inode != inodes.end(); ++inode ){
      
      if( * cluster_is_finished ) break;
      
      for( std::vector<topology::node>::const_iterator jnode = jnodes.begin(); jnode != jnodes.end(); ++jnode ){
	
	if( * cluster_is_finished ) break;
	
	reduce_cluster__with_2_endpoints(icluster, cluster_is_finished, &cs_given_endpoints, inode, jnode);

	cs_given_clusters_of_endpoints.insert(cs_given_clusters_of_endpoints.end(),cs_given_endpoints.begin(),cs_given_endpoints.end());

	m.message("SULTAN::sultan::reduce_cluster__with_2_clusters_of_endpoints: ", inode->c().id(), "->", jnode->c().id(), " contributes ", cs_given_endpoints.size(), " clusters, total = ",  cs_given_clusters_of_endpoints.size() , mybhep::VERBOSE);

	if( * cluster_is_finished ){
	  break;
	}
      }
    }

    m.message("SULTAN::sultan::reduce_cluster__with_2_clusters_of_endpoints:  " , cs_given_clusters_of_endpoints.size() , " clusters of cells have been done between 2 clusters of endpoints " , mybhep::VERBOSE);

    if( cs_given_clusters_of_endpoints.size() ) {
      cs_given_clusters_of_endpoints = clean_up(cs_given_clusters_of_endpoints);
      topology::cluster best_cluster = get_best_cluster_from(cs_given_clusters_of_endpoints);
      cs->push_back(best_cluster);
      m.message("SULTAN::sultan::reduce_cluster__with_2_clusters_of_endpoints:  after cleaning ", cs_given_clusters_of_endpoints.size(), " clusters remain between 2 clusters of endpoints, chosen a best, total = ", cs->size(), mybhep::VERBOSE);
    }

    if( use_clocks )
      clock.stop(" sultan: reduce_cluster__with_2_clusters_of_endpoints ");

    return;

  }

  //*************************************************************
  void sultan::reduce_cluster__with_vector_of_clusters_of_endpoints(size_t icluster, std::vector<topology::cluster> * cs, std::vector<topology::cluster> clusters_of_endpoints, bool * cluster_is_finished ) {
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: reduce_cluster__with_vector_of_clusters_of_endpoints ","cumulative");

    // loop on 1st cluster of endpoints
    for(std::vector<topology::cluster>::const_iterator iclu = clusters_of_endpoints.begin(); iclu != clusters_of_endpoints.end() - 1; ++iclu){
      
      const std::vector<topology::node> &inodes = iclu->nodes();
      
      if( * cluster_is_finished ){
	break;
      }
      
      m.message("SULTAN::sultan::reduce_cluster__with_vector_of_clusters_of_endpoints:  1st cluster of endpoints has " , inodes.size() , " nodes " , mybhep::VVERBOSE);
      
      // loop on 2nd cluster of endpoints
      for(std::vector<topology::cluster>::const_iterator jclu = iclu+1; jclu != clusters_of_endpoints.end(); ++jclu){
	
	if( * cluster_is_finished ){
	  break;
	}
	
	const std::vector<topology::node> &jnodes = jclu->nodes();
	
	// we expect only 1 cluster between 2 clusters of endpoints
	reduce_cluster__with_2_clusters_of_endpoints(icluster, cluster_is_finished, inodes, jnodes, cs);

      }
    }

    if( use_clocks )
      clock.stop(" sultan: reduce_cluster__with_vector_of_clusters_of_endpoints ");

    return;

  }

  //*************************************************************
  std::vector<topology::cluster> sultan::make_unclustered_hits(topology::node * n = 0) {
    //*************************************************************

    size_t minimum_length=2;

    std::vector<topology::cluster> cs;
    // separate unclustered hits into clusters based solely on nearness
    if( leftover_cluster_->nodes_.size() ){
      

      if (level >= mybhep::VVERBOSE){
	std::clog << "SULTAN::sultan::make_unclustered_hits: make unclustered hits for " << leftover_cluster_->nodes_.size() << " hits: (";
	for(std::vector<topology::node>::const_iterator inode=leftover_cluster_->nodes_.begin(); inode != leftover_cluster_->nodes_.end(); ++inode)
	  std::clog << inode->c().id() << " " ;
	std::clog << ") without endpoint ? " << (bool)(n == 0) << std::endl;
      }

      
      leftover_cluster_->self_order(n);
      leftover_cluster_->break_into_continous_pieces(nofflayers, cell_distance);
      vector<size_t> length_of_piece = leftover_cluster_->length_of_piece();
      std::vector<size_t> the_first_cell_of_piece = leftover_cluster_->first_cell_of_piece();
      
      m.message("SULTAN::sultan::make_unclustered_hits:  leftover hits = ", 
		leftover_cluster_->nodes_.size(), " broken into ", length_of_piece.size(), " pieces with ", the_first_cell_of_piece.size(), " first cells ", mybhep::VERBOSE); fflush(stdout);
      
      
      // for each continuous piece, make a cluster
      for(std::vector<size_t>::const_iterator il=length_of_piece.begin();il!=length_of_piece.end();++il){

	size_t index= il - length_of_piece.begin();

	size_t first=the_first_cell_of_piece[index];

	size_t last=the_first_cell_of_piece[0];
	if( index + 1 < length_of_piece.size() )
	  last = the_first_cell_of_piece[index + 1];

	m.message("SULTAN::sultan::make_unclustered_hits:  piece ", index, " L ", *il, " first ", first, "last", last, " minimum length ", minimum_length, mybhep::VVERBOSE); fflush(stdout);

	if( *il < minimum_length ) continue;

	topology::cluster c = leftover_cluster_->get_cluster_with_first_last(first,last);
	c.set_cluster_type("neighbouring_cells");
	cs.push_back(c);
	
      }
    }

    m.message("SULTAN::sultan::make_unclustered_hits:  adding ", cs.size(), " unclusterized clusters", mybhep::VERBOSE); fflush(stdout);
      
    return cs;
  }


  //*************************************************************
  void sultan::reduce_cluster_based_on_endpoints(size_t icluster) {
    //*************************************************************
    // input: a cluster of neighbouring cells
    // output: a vector of clusters

    // start clock
    if( use_clocks )
      clock.start(" sultan: reduce_cluster_based_on_endpoints ","cumulative");

    // define variables
    std::vector<topology::cluster> cs, newly_made_clusters;
    bool there_are_nodes_to_clusterize=true;
    int iteration = -1;
    bool cluster_is_finished = false;
    std::vector<topology::cluster> clusters_of_endpoints;

    size_t n_of_leftover_cells;

    // while loop: continue to reduce while there are nodes to clusterize
    while( there_are_nodes_to_clusterize ){

      // count iterations
      iteration++;
      m.message("SULTAN::sultan::reduce_cluster_based_on_endpoints: iteration ", iteration, " sequentiate cluster of ", leftover_cluster_->nodes().size(), " leftover gg cells " , mybhep::VERBOSE);
      status();

      // setup
      cs.clear();
      cluster_is_finished = false;
      n_of_leftover_cells = leftover_cluster_->nodes().size();


      // get the clusters of endpoints for this input cluster of neighbouring cells
      clusters_of_endpoints = get_clusters_of_cells_to_be_used_as_end_points();
      if( clusters_of_endpoints.size() < 2 ) {
	m.message("SULTAN::sultan::reduce_cluster_based_on_endpoints: cannot sequentiate because there are ", clusters_of_endpoints.size(), " clusters of endpoints " , mybhep::VERBOSE);
	break;
      }


      // make clusters between the endpoints
      reduce_cluster__with_vector_of_clusters_of_endpoints(icluster, & cs, clusters_of_endpoints, &cluster_is_finished);
      m.message("SULTAN::sultan::reduce_cluster_based_on_endpoints:  " , cs.size() , " clusters of cells have been done between all", clusters_of_endpoints.size(), " clusters of endpoints " , mybhep::VERBOSE);
      status();

      // add clusters just made to the vector of made clusters
      m.message("SULTAN::sultan::reduce_cluster_based_on_endpoints:  the ", full_cluster_->nodes().size(), " cells have been reduced to ", cs.size(), " clusters, to be added to ", newly_made_clusters.size(), " newly made clusters so far ", mybhep::VERBOSE); fflush(stdout);
      cs = clean_up(cs);
      //assign_nodes_of_clusters(cs);
      newly_made_clusters.insert(newly_made_clusters.end(), cs.begin(), cs.end());
      newly_made_clusters = clean_up(newly_made_clusters);
      m.message("SULTAN::sultan::reduce_cluster_based_on_endpoints:  after clean_up, ", newly_made_clusters.size(), "newly made clusters remain", mybhep::VERBOSE); fflush(stdout);
      status();
      

      if (level >= mybhep::VERBOSE)
	{
	  for(std::vector<topology::cluster>::const_iterator is=newly_made_clusters.begin(); is!=newly_made_clusters.end(); ++is){
	    std::clog << "SULTAN::sultan::reduce_cluster_based_on_endpoints: newly_made cluster [" << is - newly_made_clusters.begin() << "] has " << is->nodes_.size() << " nodes " ;
	    const std::vector<topology::node> nodes = is->nodes();
	    for( std::vector<topology::node>::const_iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	      std::clog << inode->c().id() << " " ;
	    std::clog << " " << std::endl;
	  }
	}
    

      // clusterize again if:
      // the cluster is not finished
      // AND some clusters have been obtained in this round
      // AND the leftover cells have decreased
      there_are_nodes_to_clusterize = !cluster_is_finished &&
	cs.size() &&
	(leftover_cluster_->nodes().size() < n_of_leftover_cells) ;


      m.message("SULTAN::sultan::reduce_cluster_based_on_endpoints:  status: newly made size: ", newly_made_clusters.size(), " leftover:", leftover_cluster_->nodes().size(), " cluster_is_finished:", cluster_is_finished, " there_are_nodes_to_clusterize:", there_are_nodes_to_clusterize, mybhep::VERBOSE); fflush(stdout);

    }

    status();

    // make clusters with unclustered hits
    std::vector<topology::cluster> unclustered_clusters;
    if( clusters_of_endpoints.size() && clusters_of_endpoints[0].nodes().size() ) { // at least 1 end point was found
      topology::node n = clusters_of_endpoints[0].nodes()[0];
      unclustered_clusters = make_unclustered_hits(&n);      
    }else{ 
      unclustered_clusters = make_unclustered_hits();
    }
    newly_made_clusters.insert(newly_made_clusters.end(), unclustered_clusters.begin(), unclustered_clusters.end());

    // add clusters to list of clusters
    made_clusters_.insert(made_clusters_.end(), newly_made_clusters.begin(), newly_made_clusters.end());

    status();

    if( use_clocks )
      clock.stop(" sultan: reduce_cluster_based_on_endpoints ");

    return;
  }


  //*************************************************************
  void sultan::assign_nodes_of_clusters(std::vector<topology::cluster> clusters){
    //*************************************************************

    for( std::vector<topology::cluster>::const_iterator iclu=clusters.begin(); iclu!=clusters.end(); ++iclu){
      assign_nodes_of_cluster(*iclu);
    }

    
  }

  //*************************************************************
  void sultan::create_sequence_from_cluster(std::vector<topology::sequence> * sequences, const topology::cluster & c){
  //*************************************************************

    const std::vector<topology::node> & nodes = c.nodes();
    if( nodes.size() ){
      
      topology::sequence s(nodes, level, probmin);
      make_name(s);
      if( !strcmp((c.get_cluster_type()).c_str(), "helix") ){
	s.set_helix(c.get_helix());
      }
      sequences->push_back(s);
    }

    
    return;

  }

  //*************************************************************
  std::vector<topology::sequence> sultan::clean_up(std::vector<topology::sequence> seqs){
  //*************************************************************

    if( seqs.size() <= 1 ) return seqs;

    std::vector<topology::sequence> cleaned_sequences;

    for(std::vector<topology::sequence>::const_iterator is = seqs.begin(); is!=seqs.end(); ++is){

      bool this_sequence_is_clean = true;

      for(std::vector<topology::sequence>::const_iterator js = seqs.begin(); js!=seqs.end(); ++js){

        if( is == js ) continue;

        if( is->is_contained_in(*js) ){
          if( !is->contains(*js) ){
            this_sequence_is_clean = false;
            break;
          }else{ // is and js have the same cells
                 // keep only the first one
            if( is > js ) this_sequence_is_clean = false;

          }
        }
      }

      if( this_sequence_is_clean ) cleaned_sequences.push_back(*is);


    }


    return cleaned_sequences;
  }


  //*************************************************************
  std::vector<topology::cluster> sultan::clean_up(std::vector<topology::cluster> cs){
  //*************************************************************

    if( cs.size() <= 1 ) return cs;

    std::vector<topology::cluster> cleaned_clusters;

    for(std::vector<topology::cluster>::const_iterator is = cs.begin(); is!=cs.end(); ++is){

      bool this_cluster_is_clean = true;

      for(std::vector<topology::cluster>::const_iterator js = cs.begin(); js!=cs.end(); ++js){

        if( is == js ) continue;

        if( is->is_contained_in(*js) ){
          if( !is->contains(*js) ){
            this_cluster_is_clean = false;
            break;
          }else{ // is and js have the same cells
                 // keep only the first one
            if( is > js ) this_cluster_is_clean = false;

          }
        }
      }

      if( this_cluster_is_clean ) cleaned_clusters.push_back(*is);


    }


    return cleaned_clusters;
  }


  //*************************************************************
  void sultan::make_name(topology::sequence & sequence_) {
    //*************************************************************

    std::string number = mybhep::to_string(sequences_.size());
    std::string name = "track_"+number;
    sequence_.set_name(name);

    return;

  }

  //*************************************************************
  bool sultan::late(void){
    //*************************************************************

    if( clock.read(" sultan: sequentiation ") >= max_time ){
      m.message(" execution time " , clock.read(" sultan: sequentiation ") , " ms  greater than max_time" , max_time , " quitting! ", mybhep::NORMAL);
      return true;
    }

    return false;

  }

  //*************************************************************
  void sultan::print_sequences() const {
    //*************************************************************

    m.message(" sequence matrix is", mybhep::NORMAL); fflush(stdout);
    for(vector<topology::sequence>::const_iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      {
        std::clog << " [" << iseq - sequences_.begin() << "]";
        print_a_sequence(*iseq);
      }

    return;

  }


  //*************************************************************
  void sultan::print_a_sequence(const topology::sequence & sequence) const {
    //*************************************************************

    std::clog << sequence.name();

    for(vector<topology::node>::const_iterator inode = sequence.nodes_.begin();
        inode != sequence.nodes_.end(); ++inode)
      {
        std::clog << " " << (int)inode->c().id();fflush(stdout);

        topology::experimental_vector v(inode->c().ep(),inode->ep());

        if( level >= mybhep::VVERBOSE ){
          std::clog << "[" << v.x().value() << ", " << v.z().value() << "]";fflush(stdout);

        }
      }

    std::clog << " " << std::endl;fflush(stdout);

    return;

  }

  //*************************************************************
  bool sultan::make_scenarios(topology::tracked_data &td){
    //*************************************************************

    if( use_clocks )
      clock.start(" sultan: make scenarios ", "cumulative");

    if( sequences_.size() ){
      topology::scenario sc;
      for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){

        sc.level_ = level;
        sc.set_probmin(probmin);
        sc.sequences_.push_back(*iseq);

      }

      scenarios_.push_back(sc);

      td.scenarios_.push_back(sc);
    }

    if( use_clocks )
      clock.stop(" sultan: make scenarios ");

    return true;


  }

  //*************************************************************
  void sultan::print_scenarios() const {
    //*************************************************************

    std::clog << "Printing scenarios " << scenarios_.size() << std::endl; fflush(stdout);

    for(vector<topology::scenario>::const_iterator isc=scenarios_.begin(); isc!=scenarios_.end(); ++isc)
      {
        std::clog << " scenario " << isc - scenarios_.begin() << std::endl;
        print_a_scenario(*isc);
      }

    return;

  }


  //*************************************************************
  void sultan::print_a_scenario(const topology::scenario & scenario) const {
    //*************************************************************

    std::clog << "Print associated sequences: " << std::endl;
    for (vector<topology::sequence>::const_iterator iseq = scenario.sequences_.begin(); iseq != scenario.sequences_.end(); ++iseq)
      {
        print_a_sequence(*iseq);
      }

    return;
  }


  //*************************************************************
  topology::cluster sultan::get_helix_cluster_from(topology::cell_triplet t, topology::experimental_helix helix){
  //*************************************************************
    // make a cluster with the nodes intercepted by helix
    if( use_clocks )
      clock.start(" sultan: get_helix_cluster_from ","cumulative");
    m.message("SULTAN::sultan::get_helix_cluster_from: , make a cluster with the nodes intercepted by helix through ", t.ca().id()," - ", t.cb().id(), " - ",t.cc().id(), mybhep::VVERBOSE);


    topology::cluster c;
    topology::experimental_double DR, DH;
      
    if( level >= mybhep::VVERBOSE ){
      std::clog << "SULTAN::sultan::get_helix_cluster_from: ...: [cell, distance_hor, distance_vert, chosen] : ";
    }

    bool chosen;

    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= leftover_cluster_->nodes_.begin(); inode!=leftover_cluster_->nodes_.end(); ++inode){
	
      chosen = helix_is_near_cell(t, helix, &DR, &DH, &(*inode));

      if( level >= mybhep::VVERBOSE ){
	std::clog << "[" << inode->c().id() << ", "; DR.dump(); std::clog << ",  "; DH.dump(); std::clog << " , " << chosen << "] ";
      }

      if( !chosen ) continue;
      
      // cell is close to helix

      c.nodes_.push_back(*inode);
      
    }

    if( level >= mybhep::VVERBOSE ){
      std::clog << " " << std::endl;
    }

    m.message("SULTAN::sultan::get_helix_cluster_from: , a cluster for the helix has been made with ", c.nodes().size(), " nodes", mybhep::VVERBOSE);

    if( c.is_good() ){
      
      // add to cluster from full nodes
      c = add_cells_to_helix_cluster_from(c, t, helix);

      topology::cluster longest_piece;

      if( get_longest_piece(&c, t.ca(), t.cc(), &longest_piece) ){
	c = longest_piece;
      }else{
	c.nodes_.clear();
      }
    }else{
      c.nodes_.clear();
    }



    if( use_clocks )
      clock.stop(" sultan: get_helix_cluster_from ");
    return c;


  }

  //*************************************************************
  topology::cluster sultan::add_cells_to_helix_cluster_from(topology::cluster cluster, topology::cell_triplet t, topology::experimental_helix helix){
  //*************************************************************
    // make a cluster with the nodes intercepted by helix
    if( use_clocks )
      clock.start(" sultan: add_cells_to_helix_cluster_from ","cumulative");
    m.message("SULTAN::sultan::add_cells_to_helix_cluster_from: , make a cluster with the nodes intercepted by helix ", mybhep::VVERBOSE);


    topology::cluster c = cluster;
    topology::experimental_double DR, DH;

    bool chosen;

    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= full_cluster_->nodes_.begin(); inode!=full_cluster_->nodes_.end(); ++inode){

      bool node_already_in_cluster = false;

      for(std::vector<topology::node>::const_iterator jnode= leftover_cluster_->nodes_.begin(); jnode!=leftover_cluster_->nodes_.end(); ++jnode){
	if( inode->c().id() == jnode->c().id() ){
	  node_already_in_cluster = true;
	  break;
	}
      }
      
      if( node_already_in_cluster ) continue;

      chosen = helix_is_near_cell(t, helix, &DR, &DH, &(*inode));
	
      if( !chosen ) continue;

      c.nodes_.push_back(*inode);
      
    }

    m.message("SULTAN::sultan::add_cells_to_helix_cluster_from: , a cluster for the helix has been made with ", c.nodes().size(), " nodes", mybhep::VVERBOSE);

    if( use_clocks )
      clock.stop(" sultan: add_cells_to_helix_cluster_from ");
    return c;

  }


  //*************************************************************
  bool sultan::line_is_near_cell(topology::experimental_line line, topology::experimental_double * DR, topology::experimental_double * DH, topology::node * node){
  //*************************************************************

    bool chosen = false;

    double normalized_parameter_along_line = line.distance_from_cell_center(node->c(), DR, DH);

    // ensure parameter is between limits
    if( normalized_parameter_along_line < 0 ){
      if( level >= mybhep::VVERBOSE ){
	std::clog << "[" << node->c().id() << " before a] ";
      }
      return chosen;
    }
    if( normalized_parameter_along_line > 1 ){
      if( level >= mybhep::VVERBOSE ){
	std::clog << "[" << node->c().id() << " after b] ";
      }
      return chosen;
    }

    chosen = (bool)(DR->is_zero__optimist(nsigma_r) && DH->is_zero__optimist(nsigma_z));

    if( level >= mybhep::VVERBOSE ){
      std::clog << "[" << node->c().id() << ", "; DR->dump(); std::clog << ",  "; DH->dump(); std::clog << " , " << chosen << "] ";
    }

    if( chosen ){
      // add circle position: in case of line it is parameter along line
      node->set_circle_phi(normalized_parameter_along_line);
    }

    return chosen;

  }


  //*************************************************************
  bool sultan::helix_is_near_cell(topology::cell_triplet t, topology::experimental_helix helix, topology::experimental_double * DR, topology::experimental_double * DH, topology::node * node){
  //*************************************************************

    if( use_clocks )
      clock.start(" sultan: helix_is_near_cell ","cumulative");

    bool chosen = false;

    topology::experimental_point p_circle;

    //helix.distance_from_cell_center(inode->c(), &DR, &DH);
    helix.distance_from_cell_measurement(node->c(), DR, DH);
      
    m.message("SULTAN::sultan::helix_is_near_cell: ..., cell " , node->c().id() , " has distance DR " , DR->value() , " +- " , DR->error() , " DH " , DH->value() , " +- " , DH->error() , " from helix " , mybhep::VVERBOSE);
      
    if( !DR->is_zero__optimist( nsigma_r) ){
      if( use_clocks )
	clock.stop(" sultan: helix_is_near_cell ");
      return chosen;
    }
    if( !DH->is_zero__optimist( nsigma_z) ){
      if( use_clocks )
	clock.stop(" sultan: helix_is_near_cell ");
      return chosen;
    }


    double angle, angle_a, angle_b;

    helix.get_phi_of_point(t.ca().ep(), &p_circle, &angle_a);
    helix.get_phi_of_point(t.cc().ep(), &p_circle, &angle_b);
    if( angle_a > angle_b ){ // swap, ensure angle_a < angle_b
      angle=angle_a;
      angle_a = angle_b;
      angle_b= angle;
    }

    helix.get_phi_of_point(node->c().ep(), &p_circle, &angle);

    if( angle < angle_a || angle > angle_b ){
      if( use_clocks )
	clock.stop(" sultan: helix_is_near_cell ");
      return chosen;
    }

    chosen=true;

    // cell is close to helix
    m.message("SULTAN::sultan::helix_is_near_cell: ... ..., cell " , node->c().id() , " is intercepted, angle ", angle , mybhep::VVERBOSE);
      
    // add circle position: in case of helix it is parameter along helix
    node->set_circle_phi(angle);
    node->set_ep(p_circle);

    if( use_clocks )
      clock.stop(" sultan: helix_is_near_cell ");

    return chosen;

  }




  //*************************************************************
  topology::cluster sultan::get_line_cluster_from(topology::node a_node, topology::node b_node){
  //*************************************************************
    // get cluster with cells intercepted by line ab
    m.message("SULTAN::sultan::get_line_cluster_from: get cluster with cells intercepted by line ab ", a_node.c().id(), "-", b_node.c().id(), " in ", leftover_cluster_->nodes_.size(), " leftover cells ", mybhep::VVERBOSE);

    if( use_clocks )
      clock.start(" sultan: get_line_cluster_from ","cumulative");

    /////////////////////////////////
    ///   built thick line a->b   ///
    /////////////////////////////////
    topology::experimental_point a = a_node.c().ep();
    double cell_center_error = cell_distance/2.;
    //a.set_x(topology::experimental_double(a.x().value(), a_node.c().r().value()));
    //a.set_y(topology::experimental_double(a.y().value(), a_node.c().r().value()));
    a.set_x(topology::experimental_double(a.x().value(), cell_center_error));
    a.set_y(topology::experimental_double(a.y().value(), cell_center_error));
    topology::experimental_point b = b_node.c().ep();
    //b.set_x(topology::experimental_double(b.x().value(), b_node.c().r().value()));
    //b.set_y(topology::experimental_double(b.y().value(), b_node.c().r().value()));
    b.set_x(topology::experimental_double(b.x().value(), cell_center_error));
    b.set_y(topology::experimental_double(b.y().value(), cell_center_error));

    topology::experimental_line line(a, b, level, probmin);


    /////////////////////////////////////
    ///   clusterize from line a->b   ///
    /////////////////////////////////////
    
    topology::cluster c;
    a_node.set_circle_phi(0);
    c.nodes_.push_back(a_node);

    topology::experimental_double DR, DH;

    if( level >= mybhep::VVERBOSE ){
      std::clog << "SULTAN::sultan::get_line_cluster_from: ...: [cell, distance_hor, distance_vert, chosen] : ";
    }

    bool chosen;
    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= leftover_cluster_->nodes_.begin(); inode!=leftover_cluster_->nodes_.end(); ++inode){

      if( inode->c().id() == a_node.c().id() ) continue;
      if( inode->c().id() == b_node.c().id() ) continue;

      chosen = line_is_near_cell(line, &DR, &DH, &(*inode));

      if( !chosen ) continue;
      
      // cell is close to line

      c.nodes_.push_back(*inode);
      
    }

    if( level >= mybhep::VVERBOSE ){
      std::clog << " " << std::endl;
    }

    b_node.set_circle_phi(1);
    c.nodes_.push_back(b_node);

    m.message("SULTAN::sultan::get_line_cluster_from: the line ",a_node.c().id(), " - ", b_node.c().id(), " intercepts " , c.nodes_.size() , " nodes " , mybhep::VVERBOSE);

    if( c.is_good() ){
      
      c = add_cells_to_line_cluster_from(line, a_node.c().id(), b_node.c().id(), c);
      
      topology::cluster longest_piece;
      if( get_longest_piece(&c, a_node, b_node, &longest_piece) ){
	c = longest_piece;
      }else{
	c.nodes_.clear();
      }
    }else{
      c.nodes_.clear();
    }

    if( use_clocks )
      clock.stop(" sultan: get_line_cluster_from ");
    return c;


  }

  //*************************************************************
  topology::cluster sultan::add_cells_to_line_cluster_from(topology::experimental_line line, size_t ida, size_t idb, topology::cluster cluster){
  //*************************************************************
    // add to line (obtained from cluster) cells from full_cluster
    if( use_clocks )
      clock.start(" sultan: add_cells_to_line_cluster_from ","cumulative");
    m.message("SULTAN::sultan::add_cells_to_line_cluster_from: get cluster with cells intercepted by line ab " , mybhep::VVERBOSE);

    topology::cluster c = cluster;

    topology::experimental_double dist_hor, dist_vert;
    bool chosen;

    // loop on cells in the full cluster
    for(std::vector<topology::node>::iterator inode= full_cluster_->nodes_.begin(); inode!=full_cluster_->nodes_.end(); ++inode){

      if( inode->c().id() == ida ) continue;
      if( inode->c().id() == idb ) continue;

      bool node_already_in_cluster = false;

      for(std::vector<topology::node>::iterator jnode= cluster.nodes_.begin(); jnode!=cluster.nodes_.end(); ++jnode){
	if( inode->c().id() == jnode->c().id() ){
	  node_already_in_cluster = true;
	  break;
	}
      }

      if( node_already_in_cluster ) continue;

      chosen = line_is_near_cell(line, &dist_hor, &dist_vert, &(*inode));

      if( !chosen ) continue;

      // cell is close to line
      m.message("SULTAN::sultan::add_cells_to_line_cluster_from: ... ..., cell " , inode->c().id() , " is intercepted " , mybhep::VVERBOSE);
      
      c.nodes_.push_back(*inode);
      
    }

    m.message("SULTAN::sultan::add_cells_to_line_cluster_from: the line intercepts " , c.nodes_.size() , " nodes " , mybhep::VVERBOSE);

    if( use_clocks )
      clock.stop(" sultan: add_cells_to_line_cluster_from ");
    return c;


  }


  //*************************************************************
   topology::cluster sultan::get_best_cluster_from(std::vector<topology::cluster> cs){
  //*************************************************************
     
     // choose 1 best cluster from cs
     size_t nmax=0;
     topology::cluster cmax;
     
     // loop on clusters
     for(std::vector<topology::cluster>::const_iterator iclu = cs.begin(); iclu != cs.end(); ++iclu){
       if( iclu->nodes_.size() > nmax ){
	 nmax = iclu->nodes_.size();
	 cmax = *iclu;
	 cmax.set_probmin(iclu->probmin());
	 cmax.set_print_level(iclu->print_level());
	 cmax.set_cluster_type(iclu->get_cluster_type());
       }
     }
     
     m.message("SULTAN::sultan::get_best_cluster_from:  the best cluster of ", cs.size(),  " has ", nmax, " nodes ", mybhep::VVERBOSE);

     return cmax;
   }


  //*************************************************************
  void sultan::get_line_clusters_from(topology::node a, topology::node b, size_t icluster, bool *cluster_is_finished, std::vector<topology::cluster> * cs){
  //*************************************************************
    // get all clusters based on line (a, b), with X in leftover cluster
    // all clusters returned are "good"

    if( use_clocks )
      clock.start(" sultan: get_line_clusters_from ","cumulative");

    m.message("SULTAN::sultan::get_line_clusters_from: get all clusters based on line (a, b), with X in cluster of", leftover_cluster_->nodes().size(), " cells " , mybhep::VVERBOSE);

    topology::cluster c = get_line_cluster_from(a, b);
    if( !c.is_good() ){
      if (level >= mybhep::VERBOSE){
	std::clog << "SULTAN::sultan::get_line_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " not clusterized as line " << std::endl;
      }
      if( use_clocks )
	clock.stop(" sultan: get_line_clusters_from ");
      return;
    }

    c.set_cluster_type("straight_line");
    assign_nodes_of_cluster(c);
    cs->push_back(c);

    if (level >= mybhep::VERBOSE){
      const std::vector<topology::node> & cnodes = c.nodes();
      std::clog << "SULTAN::sultan::get_line_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " clusterized as line with " << cnodes.size() << " cells ( ";
      for( std::vector<topology::node>::const_iterator inode=cnodes.begin(); inode!=cnodes.end(); ++inode){
	std::clog <<inode->c().id() << " ";
      }
      std::clog <<")" << std::endl;
    }

    if( c.nodes().size() == full_cluster_->nodes().size() ){
      m.message("SULTAN::sultan::get_line_clusters_from: all", c.nodes().size(), "cells of cluster have been assigned as a line ", mybhep::VERBOSE);
      *cluster_is_finished = true;
    }

    if( use_clocks )
      clock.stop(" sultan: get_line_clusters_from ");
    return;

  }

  //*************************************************************
  void sultan::get_helix_clusters_from(topology::node a, topology::node b, size_t icluster, bool *cluster_is_finished, std::vector<topology::cluster> * cs){
  //*************************************************************
    // get all clusters based on helices built on triplets (a, X, b), with X in leftover cluster
    // all clusters returned are "good"

    if( use_clocks )
      clock.start(" sultan: get_helix_clusters_from ","cumulative");

    m.message("SULTAN::sultan::get_helix_clusters_from: get all clusters based on helices built on triplets (a, X, b), with X in cluster of", leftover_cluster_->nodes().size(), " cells " , mybhep::VVERBOSE);

    std::vector<topology::experimental_helix> helices;


    // loop on cells in the cluster of leftover nodes for form a triplet (a, X, b)
    double distance12, distance23;
    std::vector<topology::node>::iterator inode= leftover_cluster_->nodes_.begin();
    while( inode != leftover_cluster_->nodes_.end()){

      if( inode - leftover_cluster_->nodes_.begin() + 1 > leftover_cluster_->nodes_.size() ) break;

      m.message("SULTAN::sultan::get_helix_clusters_from:  ... build helices for triplet ( " , a.c().id() , ", " , inode->c().id() , ", " , b.c().id() , ") using node ", inode - leftover_cluster_->nodes_.begin(), " of ", leftover_cluster_->nodes_.size(), mybhep::VVERBOSE);

      if( inode->c().id() == a.c().id() ){
	++inode;
	continue;
      }
      if( inode->c().id() == b.c().id() ){
	++inode;
	continue;
      }

      distance12 = (a.c().ep().hor_distance(inode->c().ep())).value();
      distance23 = (inode->c().ep().hor_distance(b.c().ep())).value();

      m.message(" (triplet " , a.c().id() , ", " , inode->c().id() , ", " , b.c().id() , ") distance12 " , distance12, " distance23 ", distance23 , mybhep::VVERBOSE);

      if( distance12 < dist_limit_inf ){
	++inode;
	continue;
      }
      if( distance23 < dist_limit_inf ){
	++inode;
	continue;
      }

      triplets_.clear();
      topology::cell_triplet t(a.c(), inode->c(), b.c(), level, probmin);
      triplets_.push_back(t);
      helices.clear();
      
      m.message("SULTAN::sultan::get_helix_clusters_from:  ... build helices for triplet ( " , a.c().id() , ", " , inode->c().id() , ", " , b.c().id() , ")" , mybhep::VVERBOSE);


      if( !form_helices_from_triplets(&helices, icluster) ){
	++inode;
	continue;
      }

      m.message("SULTAN::sultan::get_helix_clusters_from:  ..., " , helices.size() , " helices produced " , mybhep::VVERBOSE);
          
      if( level >= mybhep::VVERBOSE ){
	if( helices.size() )
	  std::clog << " triplet (" << a.c().id() << ", " << inode->c().id() << ", " << b.c().id() << ") gives " << helices.size() << " helices " << std::endl;
      }


      // clusters from helix
      std::vector<topology::cluster> csh;

      // loop on helices
      for(std::vector<topology::experimental_helix>::const_iterator ihelix= helices.begin(); ihelix!=helices.end(); ++ihelix){

	// form cluster from leftover nodes
	topology::cluster c = get_helix_cluster_from(t, *ihelix);

	if( !c.is_good() ) continue;

	if (level >= mybhep::VERBOSE){
	  
	  std::vector<topology::node> the_nodes = c.nodes();
	  std::clog << "SULTAN::sultan::get_helix_clusters_from:  helix " << ihelix - helices.begin() << " for triplet ( " << a.c().id() << ", " << inode->c().id() << ", " << b.c().id() << ") makes a good cluster of " << c.nodes().size()<< " cells, cluster: ( ";
	  for(std::vector<topology::node>::const_iterator jnode=the_nodes.begin(); jnode!=the_nodes.end(); ++jnode)
	    std::clog << jnode->c().id() << " ";
	  std::clog << ")" << std::endl;
	}
	
	c.set_cluster_type("helix");

	if( c.nodes().size() == full_cluster_->nodes_.size() ){
	  m.message("SULTAN::sultan::get_helix_clusters_from:  all", c.nodes().size(), "cells of cluster have been assigned as helix ", mybhep::VERBOSE);
	  *cluster_is_finished = true;
	  assign_nodes_of_cluster(c);
	  cs->push_back(c);
	  if( use_clocks )
	    clock.stop(" sultan: get_helix_clusters_from ");
	  return;
	}else{
	  csh.push_back(c);
	}

      } // finish loop on helices from (a, X, b)

      topology::cluster cmax = get_best_cluster_from(csh);
      size_t nmax=cmax.nodes().size();

      m.message("SULTAN::sultan::get_helix_clusters_from:  the best cluster has ", nmax, " nodes ", mybhep::VVERBOSE);
      if( nmax > 0 && cmax.is_good() ){ // 1 best cluster was found in (a, X, b)

	cmax.set_cluster_type("helix");
	assign_nodes_of_cluster(cmax); // this will remove the assigned nodes from leftover
	                               // reducing Y range for future searches in triplets (a, Y, b)
	cs->push_back(cmax);
	m.message("SULTAN::sultan::get_helix_clusters_from: cells assigned as helix ", mybhep::VERBOSE);
	
	if( cmax.nodes().size() == full_cluster_->nodes_.size() ){
	  m.message("SULTAN::sultan::get_helix_clusters_from:  all", cmax.nodes().size(), "cells of cluster have been assigned as helix ", mybhep::VERBOSE);
	  *cluster_is_finished = true;
	  if( use_clocks )
	    clock.stop(" sultan: get_helix_clusters_from ");
	  return;
	}
	
      }
     
      ++inode;
    } // finish loop on cell X in (a, X, b)


    if( use_clocks )
      clock.stop(" sultan: get_helix_clusters_from ");
    return;
    
  }


  //*************************************************************
  void sultan::assign_nodes_of_cluster(topology::cluster c){
  //*************************************************************

    leftover_cluster_->remove_nodes(c.nodes());

  }

  //*************************************************************
  std::vector<topology::cluster> sultan::get_clusters_from(topology::node a, topology::node b, size_t icluster, bool * cluster_is_finished){
  //*************************************************************
    // get all clusters with endpoints a and b

    if( use_clocks )
      clock.start(" sultan: get_clusters_from ","cumulative");

    if (level >= mybhep::VERBOSE){
      bool on_foil, on_calo, on_xcalo, on_gveto, on_calo_hit;
      on_foil=a.c().is_near_foil();
      on_calo=a.c().is_near_calo();
      on_xcalo=a.c().is_near_xcalo();
      on_gveto=a.c().is_near_gveto();

      std::clog << "SULTAN::sultan::get_clusters_from:  get all clusters with endpoints " << a.c().id();
      if( on_foil )  std::clog << "(foil)";
      if( on_calo )  std::clog << "(calo)";
      if( on_xcalo )  std::clog << "(xcalo)";

      on_foil=b.c().is_near_foil();
      on_calo=b.c().is_near_calo();
      on_xcalo=b.c().is_near_xcalo();
      on_gveto=b.c().is_near_gveto();

      std::clog << " - > " << b.c().id();
      if( on_foil )  std::clog << "(foil)";
      if( on_calo )  std::clog << "(calo)";
      if( on_xcalo )  std::clog << "(xcalo)";

      std::clog << " from full cluster with " << full_cluster_->nodes().size() << " gg cells " << std::endl;
    }

    std::vector<topology::cluster> cs;


    get_line_clusters_from(a, b, icluster, cluster_is_finished, &cs);
    status();

    if( !clusterize_with_helix_model ){
      if( use_clocks )
	clock.stop(" sultan: get_clusters_from ");
      return cs;
    }

    if( *cluster_is_finished ){ 
      if( use_clocks )
	clock.stop(" sultan: get_clusters_from ");
      return cs;
    }


    get_helix_clusters_from(a, b, icluster, cluster_is_finished, &cs);
    status();
  
    m.message("SULTAN::sultan::get_clusters_from: gotten " , cs.size() , " good clusters with endpoints a " , a.c().id() , " and b " , b.c().id() , mybhep::VVERBOSE);
    if (level >= mybhep::VERBOSE){
      for( std::vector<topology::cluster>::const_iterator iclu=cs.begin(); iclu!=cs.end(); ++iclu){
	const std::vector<topology::node> & cnodes = iclu->nodes();
	std::clog << "SULTAN::sultan::get_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " clusterized as " << iclu->get_cluster_type() << " with " << cnodes.size() << " cells ( ";
	for( std::vector<topology::node>::const_iterator inode=cnodes.begin(); inode!=cnodes.end(); ++inode){
	  std::clog <<inode->c().id() << " ";
	}
	std::clog <<")" << std::endl;
      }
    }
    
    if( use_clocks )
      clock.stop(" sultan: get_clusters_from ");
    return cs;
    

  }

  //*************************************************************
  void sultan::status(){
  //*************************************************************

    if (level >= mybhep::VERBOSE){
      std::clog << "SULTAN::sultan::status: clusters of neighbouring cells: " << clusters_.size() << ", scenarios returned by sultan: " << scenarios_.size() << ", sequences: " << sequences_.size() << ", cluster: " << made_clusters_.size() << ", gg cells: " << cells_.size() << ", calo hits: " << calos_.size() << ", cluster under study: " << full_cluster_->nodes_.size() << ", triplets under study: " << triplets_.size() << ", leftover cluster under study: " << leftover_cluster_->nodes_.size() <<  std::endl;
    }

    return;
  }




} // end of namespace SULTAN
