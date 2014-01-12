#include "sultan.h"
#include <vector>
#include <mybhep/system_of_units.h>
#include <sys/time.h>
#include <math.h>

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
    _moduleNR.clear ();
    event_number=-1;
    nevent = 0;
    initial_events = 0;
    skipped_events = 0;
    run_time = std::numeric_limits<double>::quiet_NaN ();
    return;
  }



  //*************************************************************
  bool sultan::initialize( void ){
    //*************************************************************

    m.message("\n Beginning algorithm sultan \n",mybhep::VERBOSE); fflush(stdout);

    //    clock.start(" sultan: initialize ");

    read_properties();

    nevent = 0;
    initial_events = 0;
    skipped_events = 0;
    experimental_legendre_vector = new topology::experimental_legendre_vector(level, probmin);
    experimental_legendre_vector->set_nsigmas(nsigmas);

    if( print_event_display )
      root_file_ = new TFile("a.root","RECREATE");
    

    //    clock.stop(" sultan: initialize ");

    return true;
  }



  //*************************************************************
  bool sultan::finalize() {
    //*************************************************************

    clock.start(" sultan: finalize ");

    if( print_event_display ){
      root_file_->Close();
      delete root_file_;
    }
    

    m.message("\n Ending algorithm sultan \n ",mybhep::NORMAL); fflush(stdout);

    m.message("Initial events: ", initial_events, mybhep::NORMAL);
    m.message("Skipped events: ", skipped_events, "(", 100.*skipped_events/initial_events, "\%)", mybhep::NORMAL);    

    clock.stop(" sultan: finalize ");

    print_clocks();
    experimental_legendre_vector->print_clocks();

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

    m.message("",mybhep::NORMAL);
    m.message("SULTAN sultan parameters",mybhep::NORMAL);
    m.message("verbosity print level", level, mybhep::NORMAL);
    m.message("max_time",max_time,"ms",mybhep::NORMAL);
    m.message("print_event_display",print_event_display,mybhep::NORMAL);
    m.message("probmin", probmin, mybhep::NORMAL);
    m.message("nsigma_r", nsigma_r, mybhep::NORMAL);
    m.message("nsigma_z", nsigma_z, mybhep::NORMAL);
    m.message("nsigmas",nsigmas,mybhep::NORMAL);
    m.message("nofflayers",nofflayers,mybhep::NORMAL);
    m.message("first event number", first_event_number, mybhep::NORMAL);
    m.message("cell_distance",cell_distance,"mm",mybhep::NORMAL);
    m.message("bfield",bfield,"T",mybhep::NORMAL);
    m.message("Emin",Emin,"MeV",mybhep::NORMAL);
    m.message("Emax",Emax,"MeV",mybhep::NORMAL);
    m.message("xsize is",xsize,"mm",mybhep::NORMAL);
    m.message("ysize is",ysize,"mm",mybhep::NORMAL);
    m.message("zsize is",zsize,"mm",mybhep::NORMAL);
    m.message("foil radius: ",foil_radius,"mm",mybhep::NORMAL);
    m.message(" min_ncells_in_cluster ", min_ncells_in_cluster, mybhep::NORMAL);
    m.message(" ncells_between_triplet_min ", ncells_between_triplet_min, mybhep::NORMAL);
    m.message(" ncells_between_triplet_range ", ncells_between_triplet_range, mybhep::NORMAL);
    m.message(" ncells to skip ", ncells_to_skip, " dist limit ", dist_limit_inf, " - ", dist_limit_sup, " Rmin ", Rmin, " Rmax ", Rmax, mybhep::NORMAL);

    //    clock.stop(" sultan: read dst properties ");

    return;
  }


  //*************************************************************
  bool sultan::sequentiate(topology::tracked_data & tracked_data_) {
    //*************************************************************

    clock.start(" sultan: sequentiate ","cumulative");
    clock.start(" sultan: sequentiation ","restart"); // use this one to check late

    event_number ++;
    m.message(" local_tracking: preparing event", event_number, mybhep::VERBOSE);

    if( event_number < first_event_number ){
      m.message(" local_tracking: skip event", event_number, " first event is "
		, first_event_number,  mybhep::VERBOSE);
      return true;
    }

    vector<topology::cluster> & the_clusters = tracked_data_.get_clusters ();

    if (the_clusters.empty ()) return true;

    sequences_.clear();
    scenarios_.clear();

    tracked_data_.scenarios_.clear();

    for (vector<topology::cluster>::iterator icluster = the_clusters.begin(); icluster != the_clusters.end(); ++icluster){
      topology::cluster & a_cluster = *icluster;
      sequentiate_cluster_with_experimental_vector(a_cluster, icluster - the_clusters.begin());
      //sequentiate_cluster_with_experimental_vector_2(a_cluster, icluster - the_clusters.begin());
    }

    if (late())
      {
	skipped_events ++;
	return false;
      }
    

    make_scenarios(tracked_data_);

    m.message(" SULTAN has created ", scenarios_.size(), " scenarios and ", sequences_.size(), " sequences ", mybhep::VERBOSE);

    clock.stop(" sultan: sequentiate ");

    return true;
  }

  //*************************************************************
  bool sultan::assign_nodes_based_on_experimental_helix(std::vector<topology::node> nodes, std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix * b, std::vector<topology::experimental_helix> *helices){
  //*************************************************************

    clock.start(" sultan: assign_nodes_based_on_experimental_helix ", "cumulative");

    topology::experimental_double dr, dh;
    vector<topology::node> leftover_nodes_copy=leftover_nodes;
    assigned_nodes.clear();
    leftover_nodes.clear();
    topology::experimental_point p;
    size_t best_helix_index;
    double angle;
    
    bool active = false;
    double drmin = mybhep::plus_infinity;
    double dhmin = mybhep::plus_infinity;

    for (vector<topology::node>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode){
      if (level >= mybhep::VVERBOSE)
	{
	  std::clog << " cell " << inode->c().id() << " ["; inode->c().ep().dump(); std::clog << "] r "; inode->c().r().dump(); std::clog << std::endl;
	}
      
      active = false;
      drmin = mybhep::plus_infinity;
      dhmin = mybhep::plus_infinity;
      for(std::vector<topology::experimental_helix>::const_iterator ihel=helices->begin(); ihel!=helices->end(); ++ihel){
	//ihel->distance_from_cell_measurement(inode->c(), &dr, &dh);
	ihel->distance_from_cell_center(inode->c(), &dr, &dh);
	if (level >= mybhep::VVERBOSE){
	  std::clog << " distance of helix " << ihel - helices->begin() << " from cell " << inode->c().id() << " dr (" << dr.value() << " +- " << dr.error() << ") dh (" << dh.value() << " +- " << dh.error() << ")" << std::endl;
	}
	if( fabs(dr.value()) < cell_distance/2. + nsigma_r*inode->c().r().error() && fabs(dh.value()) < nsigma_z*inode->c().ep().z().error() ){
	  active = true;
	  if( fabs(dr.value()) < drmin && fabs(dh.value()) < dhmin ){
	    drmin = fabs(dr.value());
	    dhmin = fabs(dh.value());
	    best_helix_index = ihel - helices->begin();
	  }
	}
      }
      
      if( active ){
	p = helices->at(best_helix_index).position(inode->c().ep());
	if (level >= mybhep::VVERBOSE)
	  {
	    std::clog << " is near (" << p.x().value() << ", " << p.y().value() << ", " << p.z().value() << ")" << std::endl;
	  }
	// angle of cell center wrt circle center
	angle = atan2(inode->c().ep().y().value() - b->y0().value(), inode->c().ep().x().value() - b->x0().value());
	inode->set_circle_phi(angle);
	inode->set_ep(p);
	assigned_nodes.push_back(*inode);
      }else{
	if (level >= mybhep::VVERBOSE){
	  std::clog << " is not near " << std::endl;
	}
	if( std::find(leftover_nodes_copy.begin(), leftover_nodes_copy.end(), *inode) != leftover_nodes_copy.end() )
	  leftover_nodes.push_back(*inode);
      }
    }
    
    bool ok = check_continous_cells(assigned_nodes, leftover_nodes, b);
    
    m.message(" associated ", assigned_nodes.size(), " nodes to this helix out of ", nodes.size(), " so ", leftover_nodes.size(), " remain unassigned - initially there were ", leftover_nodes_copy.size(),  " -, continous ", ok, mybhep::VERBOSE);

    clock.stop(" sultan: assign_nodes_based_on_experimental_helix ");

    return ok && 
      (leftover_nodes.size() < leftover_nodes_copy.size()) && 
      (assigned_nodes.size() ) ;
    
  }


  //*************************************************************
  bool sultan::check_continous_cells(std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix *b){
  //*************************************************************

    clock.start(" sultan: continous ", "cumulative");
    
    size_t next_index;
    
    // order node based on their angle wrt helix center
    std::sort(assigned_nodes.begin(),assigned_nodes.end(),topology::node::circle_order);
    
    vector<size_t> first_cell_of_piece;
    
    // loop over cells
    for(std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode){
      
      next_index = inode - assigned_nodes.begin() + 1;
      if( next_index == assigned_nodes.size() )
	next_index = 0;
      
      if( inode->c().near_level(assigned_nodes.at(next_index).c(), nofflayers, cell_distance ) == 0 ){
	first_cell_of_piece.push_back(next_index);
      }
      
    }
    
    bool ok = (first_cell_of_piece.size() <= 1 );

    m.message(" the ", assigned_nodes.size(), " assigned nodes have ", first_cell_of_piece.size(), " breaks, continous: ", ok, mybhep::VERBOSE);
    
    if( !ok ){
      int length;
      vector<size_t> length_of_piece;
      for(size_t i=1; i<first_cell_of_piece.size(); i++){
	length = first_cell_of_piece[i] - first_cell_of_piece[i-1];
	if( length < 0 ) length += assigned_nodes.size();
	m.message(" piece ", i - 1, " from ", first_cell_of_piece[i-1], " to ", first_cell_of_piece[i], " length ", length, mybhep::VERBOSE);
	length_of_piece.push_back(length);
      }
      length = first_cell_of_piece.front() - first_cell_of_piece.back();
      if( length < 0 ) length += assigned_nodes.size();
      m.message(" piece ", first_cell_of_piece.size()-1, " from ", first_cell_of_piece.back(), " to ", first_cell_of_piece.front(), " length ", length, mybhep::VERBOSE);
      length_of_piece.push_back(length);
      std::vector<size_t>::const_iterator ml = max_element(length_of_piece.begin(), length_of_piece.end());
      if( *ml >= 3 ){
	ok = true;
	size_t longest_piece_first = first_cell_of_piece[ml - length_of_piece.begin()];
	size_t longest_piece_last;
	if( ml - length_of_piece.begin() + 1 < length_of_piece.size() )
	  longest_piece_last = first_cell_of_piece[ml - length_of_piece.begin()+1];
	else
	  longest_piece_last = first_cell_of_piece.front();
	m.message(" longest piece from ", longest_piece_first, " to ", longest_piece_last, " with length ", *ml, mybhep::VERBOSE);
	if( longest_piece_first < longest_piece_last ){
	  std::vector<topology::node> newnodes(assigned_nodes.begin()+longest_piece_first, assigned_nodes.begin()+longest_piece_last);
	  leftover_nodes.insert(leftover_nodes.end(),assigned_nodes.begin()+longest_piece_last,assigned_nodes.end());
	  leftover_nodes.insert(leftover_nodes.end(),assigned_nodes.begin(),assigned_nodes.begin()+longest_piece_first);
	  assigned_nodes = newnodes;
	}else{
	  std::vector<topology::node> newnodes(assigned_nodes.begin()+longest_piece_first, assigned_nodes.end());
	  newnodes.insert(newnodes.end(),assigned_nodes.begin(), assigned_nodes.begin()+longest_piece_last);
	  leftover_nodes.insert(leftover_nodes.end(),assigned_nodes.begin()+longest_piece_last,assigned_nodes.begin()+longest_piece_first);
	  assigned_nodes = newnodes;
	}
      }else{
	m.message(" the longest piece is too short ", mybhep::VERBOSE);
	assigned_nodes.clear();
      }
      m.message(" return piece of length ", assigned_nodes.size(), mybhep::VERBOSE);
    }


    for(std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode)
      b->add_id(inode->c().id());
    
    clock.stop(" sultan: continous ");

    return ok;
  }

  //*************************************************************
  bool sultan::calculate_helices(std::vector<topology::node> nodes, std::vector<topology::experimental_helix> *the_helices, size_t icluster){
  //*************************************************************
      
    clock.start(" sultan: calculate_helices ", "cumulative");

    size_t nnodes = nodes.size();
    
    m.message(" calculate helices for ", nnodes, " nodes, minimum ", min_ncells_in_cluster, mybhep::VERBOSE);

    if( nnodes < min_ncells_in_cluster ) {
      clock.stop(" sultan: calculate_helices ");
      return false;
    }

    the_helices->clear();
    
    topology::cell_triplet *ccc;
    size_t ncells;
    size_t ncells_max = 0;
    topology::experimental_helix best_helix;
    
    std::vector<topology::node> assigned_nodes, assigned_nodes_best;
    double distance12, distance23, distance13;

    std::vector<topology::experimental_helix> helices;
    
    for(std::vector<topology::node>::const_iterator inode=nodes.begin(); inode != nodes.end()-2; ++inode){
      for(std::vector<topology::node>::const_iterator jnode=inode+1; jnode != nodes.end()-1; ++jnode){
	distance12 = (inode->c().ep().hor_distance(jnode->c().ep())).value();
	m.message(" (triplet ", inode->c().id() , ", " , jnode->c().id() , ") d12 " , distance12, mybhep::VVERBOSE);
	if( distance12 < dist_limit_inf || distance12 > dist_limit_sup ) continue;
	for(std::vector<topology::node>::const_iterator knode=jnode+1; knode != nodes.end(); ++knode){
	  
	  if( knode == jnode ) continue;

	  distance23 = (jnode->c().ep().hor_distance(knode->c().ep())).value();
	  m.message(" (triplet " , inode->c().id() , ", " , jnode->c().id() , ", " , knode->c().id() , ") d23 " , distance23 , mybhep::VVERBOSE);
	  if( distance23 < dist_limit_inf || distance23 > dist_limit_sup ) continue;
	  distance13 = (inode->c().ep().hor_distance(knode->c().ep())).value();
	  m.message(" (triplet " , inode->c().id() , ", " , jnode->c().id() , ", " , knode->c().id() , ") d13 " , distance13 , mybhep::VVERBOSE);
	  if( distance13 < dist_limit_inf ) continue;
	  
	  ccc = new topology::cell_triplet(inode->c(), jnode->c(), knode->c(), level);
	  ccc->calculate_helices(Rmin, Rmax, nsigmas);
	  helices = ccc->helices();
	  
	  the_helices->insert(the_helices->end(),helices.begin(),helices.end());

	  delete ccc;

	  m.message(" adding " , helices.size() , " helices, total " , the_helices->size() , mybhep::VVERBOSE);
	  
	}
      }
    }
    
    
    clock.stop(" sultan: calculate_helices ");

    if( print_event_display && event_number < 10 ){
      clock.start(" sultan : calculate_helices : print_event_display ", "cumulative");

      if( the_helices->size() ){

	TString tree_name=Form("event_%d_cluster_%d", event_number, icluster);
	TTree *root_tree = new TTree(tree_name,tree_name);
	
	TString x0_name=Form("x0_event_%d_cluster_%d", event_number, icluster);
	TString y0_name=Form("y0_event_%d_cluster_%d", event_number, icluster);
	TString z0_name=Form("z0_event_%d_cluster_%d", event_number, icluster);
	TString R_name=Form("R_event_%d_cluster_%d", event_number, icluster);
	TString H_name=Form("H_event_%d_cluster_%d", event_number, icluster);
	
	TString x0_weight_name=Form("x0_weight_event_%d_cluster_%d", event_number, icluster);
	TString y0_weight_name=Form("y0_weight_event_%d_cluster_%d", event_number, icluster);
	TString z0_weight_name=Form("z0_weight_event_%d_cluster_%d", event_number, icluster);
	TString R_weight_name=Form("R_weight_event_%d_cluster_%d", event_number, icluster);
	TString H_weight_name=Form("H_weight_event_%d_cluster_%d", event_number, icluster);
	
	double x0_value = 0., x0_weight = 0.;
	double y0_value = 0., y0_weight = 0.;
	double z0_value = 0., z0_weight = 0.;
	double R_value = 0., R_weight = 0.;
	double H_value = 0., H_weight = 0.;
	
	root_tree->Branch(x0_name,&x0_value);
	root_tree->Branch(y0_name,&y0_value);
	root_tree->Branch(z0_name,&z0_value);
	root_tree->Branch(R_name,&R_value);
	root_tree->Branch(H_name,&H_value);
	
	root_tree->Branch(x0_weight_name,&x0_weight);
	root_tree->Branch(y0_weight_name,&y0_weight);
	root_tree->Branch(z0_weight_name,&z0_weight);
	root_tree->Branch(R_weight_name,&R_weight);
	root_tree->Branch(H_weight_name,&H_weight);
	
	for(std::vector<topology::experimental_helix>::const_iterator ihel=the_helices->begin();
	    ihel != the_helices->end(); ++ihel){
	  x0_value = ihel->x0().value();
	  y0_value = ihel->y0().value();
	  z0_value = ihel->z0().value();
	  R_value = ihel->R().value();
	  H_value = ihel->H().value();
	  
	  x0_weight = pow(ihel->x0().error(),-2);
	  y0_weight = pow(ihel->y0().error(),-2);
	  z0_weight = pow(ihel->z0().error(),-2);
	  R_weight = pow(ihel->R().error(),-2);
	  H_weight = pow(ihel->H().error(),-2);
	  
	  root_tree->Fill();
	  
	}
	
	root_file_->cd();
	root_tree->Write();
	delete root_tree;
      }
      clock.stop(" sultan : calculate_helices : print_event_display ");
    }

    return true;
    
  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector ");
      return;
    }

    if (level >= mybhep::VERBOSE)
      {
	std::clog << " sequentiate cluster with " << cluster_.nodes_.size() << " nodes, leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

    topology::experimental_helix b;
    vector<topology::node> assigned_nodes, leftover_nodes;
    topology::sequence *s;
    topology::experimental_point *center;
    topology::experimental_double radius;
    topology::experimental_double pitch;
    std::vector<topology::experimental_helix> neighbours;
    std::vector<topology::experimental_helix> the_helices;

    leftover_nodes = cluster_.nodes_;

    while( calculate_helices(leftover_nodes, &the_helices, icluster) ){

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ","cumulative");

      assigned_nodes.clear();
      experimental_legendre_vector->reset();
      neighbours.clear();

      if (level >= mybhep::VERBOSE){
	std::clog << " leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }
      
      if( !the_helices.size() ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ");
	break;
      }
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ");


      clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: add helix ","cumulative");

      for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
	experimental_legendre_vector->add_helix(*hh);
      }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: add helix ");

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ","cumulative");

      b = experimental_legendre_vector->max(&neighbours);

      if( !b.ids().size() ){
      	m.message(" could not make a track ", mybhep::VERBOSE); fflush(stdout);
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ");
	break;
      }


      if (level >= mybhep::VERBOSE)
	{
	  std::clog << " best helix: " ; b.dump();
	  std::clog << " associated " << neighbours.size() << " helices " << std::endl;
	}
      
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ");
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: assign ","cumulative");

      bool ok = assign_nodes_based_on_experimental_helix(cluster_.nodes_, assigned_nodes, leftover_nodes, &b, &neighbours);
      
      if( ok ){
	s = new topology::sequence(assigned_nodes, level, probmin);
	make_name(*s);
	center = new topology::experimental_point(b.x0(),b.y0(),b.z0());
	radius.set(b.R());
	pitch.set(b.H());
	s->set_helix(b);
	sequences_.push_back(*s);
	m.message(" finished track [", sequences_.size()-1, "] with ", s->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
	delete s;
	delete center;
      }else{
	m.message(" could not make a track ", mybhep::VERBOSE); fflush(stdout);
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: assign ");
	break;
      }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: assign ");

      if( print_event_display ) break;
      
    }


    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector ");

    return;
  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_2(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2 ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2 ");
      return;
    }

    if (level >= mybhep::VERBOSE)
      {
	std::clog << " sequentiate cluster with " << cluster_.nodes_.size() << " nodes, leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

    topology::experimental_helix b;
    vector<topology::node> assigned_nodes, leftover_nodes;
    topology::sequence *s;
    topology::experimental_point *center;
    topology::experimental_double radius;
    topology::experimental_double pitch;
    std::vector<topology::experimental_helix> neighbours;
    std::vector<topology::experimental_helix> the_helices;

    leftover_nodes = cluster_.nodes_;

    topology::cluster_of_experimental_helices best_cluster;
    bool found;

    while( calculate_helices(leftover_nodes, &the_helices, icluster) ){

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ","cumulative");

      assigned_nodes.clear();
      experimental_legendre_vector->reset();

      if (level >= mybhep::VERBOSE){
	std::clog << " there are " << leftover_nodes.size() << " leftover nodes and " << assigned_nodes.size() << " assigned nodes, the leg vector has " << experimental_legendre_vector->helices().size() << " helices and " << the_helices.size() << " have been prepared " << std::endl; fflush(stdout);
      }
      
      if( !the_helices.size() ){
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ");
	break;
      }
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ");


      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: add helix to clusters ","cumulative");

      for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
	experimental_legendre_vector->add_helix_to_clusters(*hh);

	if( experimental_legendre_vector->max_cluster(&b, &found).helices().size() >= the_helices.size()/2. )
	  break;

      }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: add helix to clusters ");

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: max ","cumulative");

      found = false;
      best_cluster = experimental_legendre_vector->max_cluster(&b, &found);
      
      if( !found ){
      	m.message(" could not make a track ", mybhep::VERBOSE); fflush(stdout);
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
	  std::clog << " best helix with " << neighbours.size() << " neighbours is: " ; b.dump();
	}
      
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: max ");
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: assign ","cumulative");

      bool ok = assign_nodes_based_on_experimental_helix(cluster_.nodes_, assigned_nodes, leftover_nodes, &b, &neighbours);
      
      if( ok ){
	s = new topology::sequence(assigned_nodes, level, probmin);
	make_name(*s);
	center = new topology::experimental_point(b.x0(),b.y0(),b.z0());
	radius.set(b.R());
	pitch.set(b.H());
	s->set_helix(b);
	sequences_.push_back(*s);
	m.message(" finished track [", sequences_.size()-1, "] with ", s->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
	delete s;
	delete center;
      }else{
	m.message(" could not make a track ", mybhep::VERBOSE); fflush(stdout);
	clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: assign ");
	break;
      }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: assign ");
	
      if( print_event_display ) break;

    }


    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_2 ");

    return;
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


} // end of namespace SULTAN
