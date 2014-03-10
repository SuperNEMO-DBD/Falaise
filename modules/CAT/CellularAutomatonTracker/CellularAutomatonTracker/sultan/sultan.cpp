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

    m.message("\n SULTAN:sultan::initialize: Beginning algorithm sultan \n",mybhep::VERBOSE); fflush(stdout);

    //    clock.start(" sultan: initialize ");

    read_properties();

    nevent = 0;
    initial_events = 0;
    skipped_events = 0;
    experimental_legendre_vector = new topology::experimental_legendre_vector(level, probmin);
    experimental_legendre_vector->set_nsigmas(nsigmas);
    std::vector<topology::node> nodes;
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

    clock.start(" sultan: finalize ");

    if( print_event_display ){
      root_file_->Close();
      delete root_file_;
    }


    m.message("\nSULTAN:sultan::finalize: Ending algorithm sultan \n ",mybhep::NORMAL); fflush(stdout);

    m.message("SULTAN:sultan::finalize: Initial events: ", initial_events, mybhep::NORMAL);
    m.message("SULTAN:sultan::finalize: Skipped events: ", skipped_events, "(", 100.*skipped_events/initial_events, "\%)", mybhep::NORMAL);

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

    m.message("SULTAN::sultan::read_properties: SULTAN sultan parameters",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: verbosity print level", level, mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: max_time",max_time,"ms",mybhep::NORMAL);
    m.message("SULTAN::sultan::read_properties: print_event_display",print_event_display,mybhep::NORMAL);
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
  bool sultan::sequentiate(topology::tracked_data & tracked_data_) {
    //*************************************************************

    clock.start(" sultan: sequentiate ","cumulative");
    clock.start(" sultan: sequentiation ","restart"); // use this one to check late

    event_number ++;
    m.message("SULTAN::sultan::sequentiate:  preparing event", event_number, mybhep::VERBOSE);

    if( event_number < first_event_number ){
      m.message("SULTAN::sultan::sequentiate: local_tracking: skip event", event_number, " first event is "
                , first_event_number,  mybhep::VERBOSE);
      return true;
    }

    vector<topology::cluster> & the_clusters = tracked_data_.get_clusters ();

    calos_ = tracked_data_.get_calos ();

    if (the_clusters.empty ()) return true;

    sequences_.clear();
    scenarios_.clear();

    tracked_data_.scenarios_.clear();

    m.message("SULTAN::sultan::sequentiate: prepare to reduce ", the_clusters.size(), " clusters of cells to output clusters with ", calos_.size(), " calos ", mybhep::VERBOSE);

    for (vector<topology::cluster>::iterator icluster = the_clusters.begin(); icluster != the_clusters.end(); ++icluster){
      topology::cluster & a_cluster = *icluster;

      m.message("SULTAN::sultan::sequentiate: prepare to reduce cluster ", icluster - the_clusters.begin(), " of ", the_clusters.size(), " having", icluster->nodes_.size(), " cells ", mybhep::VERBOSE);

      // look for cluster with largest number of triplet-neighbours
      sequentiate_cluster_with_experimental_vector(a_cluster, icluster - the_clusters.begin());

      //keep track in smarter way of which cluster has the largest number of triplet-neighbours
      //sequentiate_cluster_with_experimental_vector_2(a_cluster, icluster - the_clusters.begin());

      // look for cluster with largest number of cell-neighbours
      //sequentiate_cluster_with_experimental_vector_3(a_cluster, icluster - the_clusters.begin());

      // put helices in clusters, assign each cell to the cluster with the largest n of cells
      //sequentiate_cluster_with_experimental_vector_4(a_cluster, icluster - the_clusters.begin());

      // form triplets with 2 cells on foil or calo
      //sequentiate_cluster_with_experimental_vector_5(a_cluster, icluster - the_clusters.begin());

    }

    if (late())
      {
        skipped_events ++;
        return false;
      }


    make_scenarios(tracked_data_);

    m.message("SULTAN::sultan::sequentiate:  SULTAN has created ", scenarios_.size(), " scenarios and ", sequences_.size(), " sequences for this event ", mybhep::VERBOSE);

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
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: cell " << inode->c().id() << " ["; inode->c().ep().dump(); std::clog << "] r "; inode->c().r().dump(); std::clog << std::endl;
        }

      active = false;
      drmin = mybhep::plus_infinity;
      dhmin = mybhep::plus_infinity;
      for(std::vector<topology::experimental_helix>::const_iterator ihel=helices->begin(); ihel!=helices->end(); ++ihel){
        //ihel->distance_from_cell_measurement(inode->c(), &dr, &dh);
        ihel->distance_from_cell_center(inode->c(), &dr, &dh);
        if (level >= mybhep::VVERBOSE){
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: distance of helix " << ihel - helices->begin() << " from cell " << inode->c().id() << " dr (" << dr.value() << " +- " << dr.error() << ") dh (" << dh.value() << " +- " << dh.error() << ")" << std::endl;
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
            std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is near (" << p.x().value() << ", " << p.y().value() << ", " << p.z().value() << ")" << std::endl;
          }
        // angle of cell center wrt circle center
        angle = atan2(inode->c().ep().y().value() - b->y0().value(), inode->c().ep().x().value() - b->x0().value());
        inode->set_circle_phi(angle);
        inode->set_ep(p);
        assigned_nodes.push_back(*inode);
      }else{
        if (level >= mybhep::VVERBOSE){
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is not near " << std::endl;
        }
        if( std::find(leftover_nodes_copy.begin(), leftover_nodes_copy.end(), *inode) != leftover_nodes_copy.end() )
          leftover_nodes.push_back(*inode);
      }
    }

    bool ok = check_continous_cells(assigned_nodes, leftover_nodes, b);

    m.message("SULTAN::sultan::assign_nodes_based_on_experimental_helix: associated ", assigned_nodes.size(), " nodes to this helix out of ", nodes.size(), " so ", leftover_nodes.size(), " remain unassigned - initially there were ", leftover_nodes_copy.size(),  " -, continous ", ok, mybhep::VERBOSE);

    clock.stop(" sultan: assign_nodes_based_on_experimental_helix ");

    return ok &&
      (leftover_nodes.size() < leftover_nodes_copy.size()) &&
      (assigned_nodes.size() ) ;

  }


  //*************************************************************
  bool sultan::assign_nodes_based_on_experimental_helix(std::vector<topology::node> nodes, std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix * b, std::vector<size_t> *neighbouring_cells){
  //*************************************************************

    clock.start(" sultan: assign_nodes_based_on_experimental_helix ", "cumulative");

    topology::experimental_double dr, dh;
    vector<topology::node> leftover_nodes_copy=leftover_nodes;
    assigned_nodes.clear();
    leftover_nodes.clear();
    topology::experimental_point p;
    // size_t best_helix_index;
    double angle;

    bool active = false;
    //double drmin = mybhep::plus_infinity;
    //double dhmin = mybhep::plus_infinity;

    for (vector<topology::node>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode){
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
        assigned_nodes.push_back(*inode);
      }else{
        if (level >= mybhep::VVERBOSE){
          std::clog << "SULTAN::sultan::assign_nodes_based_on_experimental_helix: is not near " << std::endl;
        }
        if( std::find(leftover_nodes_copy.begin(), leftover_nodes_copy.end(), *inode) != leftover_nodes_copy.end() )
          leftover_nodes.push_back(*inode);
      }

    }

    bool ok = check_continous_cells(assigned_nodes, leftover_nodes, b);

    m.message("SULTAN::sultan::assign_nodes_based_on_experimental_helix: associated ", assigned_nodes.size(), " nodes to this helix out of ", nodes.size(), " so ", leftover_nodes.size(), " remain unassigned - initially there were ", leftover_nodes_copy.size(),  " -, continous ", ok, mybhep::VERBOSE);

    clock.stop(" sultan: assign_nodes_based_on_experimental_helix ");

    return ok &&
      (leftover_nodes.size() < leftover_nodes_copy.size()) &&
      (assigned_nodes.size() ) ;

  }


  //*************************************************************
  bool sultan::check_continous_cells(std::vector<topology::node> &assigned_nodes, std::vector<topology::node> &leftover_nodes, topology::experimental_helix *b){
  //*************************************************************
    // - assigned_nodes = input
    // - order them according to parameter along model
    // - count n of breaks
    // - if n > 1, keep the longest piece in assigned_nodes and put the others in leftover_nodes

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

    m.message("SULTAN::sultan::check_continous_cells:  the ", assigned_nodes.size(), " assigned nodes have ", first_cell_of_piece.size(), " breaks, continous: ", ok, mybhep::VERBOSE);

    if( !ok ){
      int length;
      vector<size_t> length_of_piece;
      for(size_t i=1; i<first_cell_of_piece.size(); i++){
        length = first_cell_of_piece[i] - first_cell_of_piece[i-1];
        if( length < 0 ) length += assigned_nodes.size();
        m.message("SULTAN::sultan::check_continous_cells:  piece ", i - 1, " from ", first_cell_of_piece[i-1], " to ", first_cell_of_piece[i], " length ", length, mybhep::VERBOSE);
        length_of_piece.push_back(length);
      }
      length = first_cell_of_piece.front() - first_cell_of_piece.back();
      if( length < 0 ) length += assigned_nodes.size();
      m.message("SULTAN::sultan::check_continous_cells:  piece ", first_cell_of_piece.size()-1, " from ", first_cell_of_piece.back(), " to ", first_cell_of_piece.front(), " length ", length, mybhep::VERBOSE);
      length_of_piece.push_back(length);
      std::vector<size_t>::const_iterator ml = max_element(length_of_piece.begin(), length_of_piece.end());
      if( *ml >= 3 ){
        ok = true;
        size_t longest_piece_first = first_cell_of_piece[ml - length_of_piece.begin()];
        size_t longest_piece_last;
        if( ml - length_of_piece.begin() + 1 < (int) length_of_piece.size() )
          longest_piece_last = first_cell_of_piece[ml - length_of_piece.begin()+1];
        else
          longest_piece_last = first_cell_of_piece.front();
        m.message("SULTAN::sultan::check_continous_cells:  longest piece from ", longest_piece_first, " to ", longest_piece_last, " with length ", *ml, mybhep::VERBOSE);
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
        m.message("SULTAN::sultan::check_continous_cells:  the longest piece is too short ", mybhep::VERBOSE);
        assigned_nodes.clear();
      }
      if (level >= mybhep::VERBOSE){
	std::clog << "SULTAN::sultan::check_continous_cells:  return piece of length " << assigned_nodes.size() << ": ( ";
	for(std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode)
	  std::clog << inode->c().id() << " ";
	std::clog << ")" << std::endl;
      }
    }


    for(std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode)
      b->add_id(inode->c().id());

    clock.stop(" sultan: continous ");

    return ok;
  }

  //*************************************************************
  bool sultan::check_continous_cells(topology::cluster * assigned_nodes_cluster){
  //*************************************************************
    // - assigned_nodes = input
    // - order them according to parameter along model
    // - count n of breaks
    // - if n > 1, keep the longest piece in assigned_nodes

    clock.start(" sultan: continous ", "cumulative");

    std::vector<size_t> the_first_cell_of_piece, length_of_piece;
    std::vector<topology::node> the_assigned_nodes;

    bool ok = break_cluster_into_continous_parts(assigned_nodes_cluster, &the_first_cell_of_piece, &length_of_piece, &the_assigned_nodes);

    assigned_nodes_cluster->set_nodes(the_assigned_nodes);

    clock.stop(" sultan: continous ");

    return ok;
  }


  //*************************************************************
  std::vector<size_t> sultan::first_cells(std::vector<topology::node> assigned_nodes){
  //*************************************************************
    // input nodes = (a1, ..., an)
    // - returns a vector (a b ... c) (a vector of indexes)
    // where the nodes "(nodes[a], nodes[b-1])" form continuous strip of cells
    // if needed it's the nodes "(nodes[c], an) + (a1, nodes[a])" that form a continous strip
    // - if there is only one input node = (a), returns (0)
    // - if all input cells are near, returns (0)

    std::vector<size_t> first_cell_of_piece;
    
    if( assigned_nodes.size() == 1 ){
      first_cell_of_piece.push_back(0);
    }
    else{ // loop over cells
      size_t next_index;
      for(std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode){
	
	next_index = inode - assigned_nodes.begin() + 1;
	if( next_index == assigned_nodes.size() )
	  next_index = 0;
	
	if( inode->c().near_level(assigned_nodes.at(next_index).c(), nofflayers, cell_distance ) == 0 ){
	  first_cell_of_piece.push_back(next_index);
	}
	
      }
    }

    if( !first_cell_of_piece.size() ){ // there are >1 nodes but no breaks
      first_cell_of_piece.push_back(0);      
    }

    return first_cell_of_piece;

  }

  //*************************************************************
  bool sultan::break_cluster_into_continous_parts(topology::cluster * assigned_nodes_cluster, std::vector<size_t> * the_first_cell_of_piece, std::vector<size_t> * the_length_of_piece, std::vector<topology::node> * the_assigned_nodes){
  //*************************************************************
    // - assigned_nodes = input
    // - order them according to parameter along model
    // - count n of breaks
    // - if n > 1, keep the longest piece in assigned_nodes

    clock.start(" sultan: break ", "cumulative");

    std::vector<topology::node> assigned_nodes = assigned_nodes_cluster->nodes();

    // order node based on their angle wrt helix center
    std::sort(assigned_nodes.begin(),assigned_nodes.end(),topology::node::circle_order);

    vector<size_t> first_cell_of_piece = first_cells(assigned_nodes);

    bool ok = (first_cell_of_piece.size() <= 1 );

    m.message("SULTAN::sultan::break_cluster_into_continous_parts:  the ", assigned_nodes.size(), " assigned nodes have ", first_cell_of_piece.size(), " breaks, continous: ", ok, mybhep::VERBOSE);
    
    std::vector<size_t> length_of_piece;

    if( first_cell_of_piece.size() <= 1 ){ // there is only one piece
      length_of_piece.push_back(assigned_nodes.size());
    }
    else{ // there are at least 2 pieces
      int length;
      for(size_t i=1; i<first_cell_of_piece.size(); i++){
        length = first_cell_of_piece[i] - first_cell_of_piece[i-1];
        if( length < 0 ) length += assigned_nodes.size();
        m.message("SULTAN::sultan::break_cluster_into_continous_parts:  piece ", i - 1, " from ", first_cell_of_piece[i-1], " to ", first_cell_of_piece[i], " length ", length, mybhep::VVERBOSE);
        length_of_piece.push_back(length);
      }
      length = first_cell_of_piece.front() - first_cell_of_piece.back();
      if( length < 0 ) length += assigned_nodes.size();
      m.message("SULTAN::sultan::break_cluster_into_continous_parts:  piece ", first_cell_of_piece.size()-1, " from ", first_cell_of_piece.back(), " to ", first_cell_of_piece.front(), " length ", length, mybhep::VVERBOSE);
      length_of_piece.push_back(length);
      std::vector<size_t>::const_iterator ml = max_element(length_of_piece.begin(), length_of_piece.end());
      if( *ml >= 3 ){
        ok = true;
        size_t longest_piece_first = first_cell_of_piece[ml - length_of_piece.begin()];
        size_t longest_piece_last;
        if( ml - length_of_piece.begin() + 1 < (int) length_of_piece.size() )
          longest_piece_last = first_cell_of_piece[ml - length_of_piece.begin()+1];
        else
          longest_piece_last = first_cell_of_piece.front();
        m.message("SULTAN::sultan::break_cluster_into_continous_parts:  longest piece from ", longest_piece_first, " to ", longest_piece_last, " with length ", *ml, mybhep::VVERBOSE);
        if( longest_piece_first < longest_piece_last ){
          std::vector<topology::node> newnodes(assigned_nodes.begin()+longest_piece_first, assigned_nodes.begin()+longest_piece_last);
          assigned_nodes = newnodes;
        }else{
          std::vector<topology::node> newnodes(assigned_nodes.begin()+longest_piece_first, assigned_nodes.end());
          newnodes.insert(newnodes.end(),assigned_nodes.begin(), assigned_nodes.begin()+longest_piece_last);
          assigned_nodes = newnodes;
        }
      }else{
        m.message("SULTAN::sultan::break_cluster_into_continous_parts:  the longest piece is too short ", mybhep::VVERBOSE);
        assigned_nodes.clear();
      }
      if (level >= mybhep::VVERBOSE){
	std::clog << "SULTAN::sultan::break_cluster_into_continous_parts:  return piece of length " << assigned_nodes.size() << ": ( ";
	for(std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode)
	  std::clog << inode->c().id() << " ";
	std::clog << ")" << std::endl;
      }
    }


    * the_first_cell_of_piece = first_cell_of_piece;
    * the_length_of_piece = length_of_piece;
    * the_assigned_nodes = assigned_nodes;

    clock.stop(" sultan: break ");

    return ok;
  }


  //*************************************************************
  bool sultan::form_triplets_from_cells(const std::vector<topology::node> & nodes){
  //*************************************************************

    clock.start(" sultan: form_triplets_from_cells ", "cumulative");

    reset_triplets();

    size_t nnodes = nodes.size();

    m.message("SULTAN::sultan::form_triplets_from_cells: calculate triples for ", nnodes, " nodes, minimum ", min_ncells_in_cluster, mybhep::VVERBOSE);

    if( nnodes < min_ncells_in_cluster ) {
      clock.stop(" sultan: form_triplets_from_cells ");
      return false;
    }


    topology::cell_triplet *ccc;
    // size_t ncells;
    // size_t ncells_max = 0;
    topology::experimental_helix best_helix;

    std::vector<topology::node> assigned_nodes, assigned_nodes_best;
    double distance12, distance23, distance13;
    double dmin1, dmin2;

    for(std::vector<topology::node>::const_iterator inode=nodes.begin(); inode != nodes.end()-2; ++inode){
      for(std::vector<topology::node>::const_iterator jnode=inode+1; jnode != nodes.end()-1; ++jnode){

        if( jnode == inode ) continue;

        for(std::vector<topology::node>::const_iterator knode=jnode+1; knode != nodes.end(); ++knode){

          if( knode == inode ) continue;
          if( knode == jnode ) continue;

          distance12 = (inode->c().ep().hor_distance(jnode->c().ep())).value();
          distance23 = (jnode->c().ep().hor_distance(knode->c().ep())).value();
          distance13 = (inode->c().ep().hor_distance(knode->c().ep())).value();

          dmin1 = distance12;
          if( distance12 > distance23 )
            dmin1 = distance23;

          dmin2 = distance12;
          if( distance12 > distance13 )
            dmin2 = distance13;

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

    clock.stop(" sultan: form_triplets_from_cells ");

    m.message("SULTAN::sultan::form_triplets_from_cells: sultan: the ", nnodes, " cells have been combined into ", triplets_.size(), " triplets ", mybhep::VERBOSE);

    return true;

  }


  //*************************************************************
  bool sultan::form_helices_from_triplets(std::vector<topology::experimental_helix> *the_helices, size_t icluster){
  //*************************************************************

    clock.start(" sultan: form_helices_from_triplets ", "cumulative");

    m.message("SULTAN::sultan::form_helices_from_triplets:  calculate helices for ", triplets_.size(), " triplets ", mybhep::VVERBOSE);

    if( triplets_.size() == 0 ) {
      clock.stop(" sultan: form_helices_from_triplets ");
      return false;
    }

    the_helices->clear();

    // size_t ncells;
    // size_t ncells_max = 0;
    topology::experimental_helix best_helix;

    std::vector<topology::node> assigned_nodes, assigned_nodes_best;

    std::vector<topology::experimental_helix> helices;


    for(std::vector<topology::cell_triplet>::iterator ccc = triplets_.begin(); ccc!= triplets_.end(); ++ccc){

      ccc->calculate_helices(Rmin, Rmax, nsigmas);
      helices = ccc->helices();

      the_helices->insert(the_helices->end(),helices.begin(),helices.end());

      m.message("SULTAN::sultan::form_helices_from_triplets:  adding " , helices.size() , " helices, total " , the_helices->size() , mybhep::VVERBOSE);

    }

    clock.stop(" sultan: form_helices_from_triplets ");

    if( print_event_display && event_number < 10 ){
      clock.start(" sultan : form_helices_from_triplets : print_event_display ", "cumulative");

      // double weight = 0.0;

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

        double x0_value = 0., x0_error = 0.;
        double y0_value = 0., y0_error = 0.;
        double z0_value = 0., z0_error = 0.;
        double R_value = 0., R_error = 0.;
        double H_value = 0., H_error = 0.;

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

          double weight = 1./(pow(x0_error,2) +
                       pow(y0_error,2) +
                       pow(z0_error,2) +
                       pow(R_error,2) +
                       pow(H_error,2)
                       );
          weight += 0.0; // trick to avoid warn

          root_tree->Fill();

          //std::clog << " helix " << ihel - the_helices->begin() << " x0 " << x0_value << " +- " << x0_error     << " y0 " << y0_value << " +- " << y0_error     << " z0 " << z0_value << " +- " << z0_error     << " R " << R_value << " +- " << R_error        << " H " << H_value << " +- " << H_error << " weight " << weight <<  std::endl;

        }

        root_file_->cd();
        root_tree->Write();
        delete root_tree;
      }
      clock.stop(" sultan : form_helices_from_triplets : print_event_display ");
    }

    m.message("SULTAN::sultan::form_helices_from_triplets:  sultan: the", triplets_.size(), " triplets have given rise to ", the_helices->size(), " helices ", mybhep::VVERBOSE);

    return true;

  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector ","cumulative");

    experimental_legendre_vector->reset();

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  sequentiate cluster with " << cluster_.nodes_.size() << " nodes " << std::endl; fflush(stdout);
      }

    if( cluster_.nodes_.size() < 3 ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector ");
      return;
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

    while( form_triplets_from_cells(leftover_nodes) && form_helices_from_triplets(&the_helices, icluster) ){

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: clean ","cumulative");

      assigned_nodes.clear();
      experimental_legendre_vector->reset();
      neighbours.clear();

      if (level >= mybhep::VERBOSE){
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
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
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
        clock.stop(" sultan: sequentiate_cluster_with_experimental_vector: helix loop: max ");
        break;
      }


      if (level >= mybhep::VERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  best helix: " ; b.dump();
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  associated " << neighbours.size() << " helices " << std::endl;
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
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  finished track [", sequences_.size()-1, "] with ", s->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
        delete s;
        delete center;
      }else{
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
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
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2: sequentiate cluster with " << cluster_.nodes_.size() << " nodes, leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
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

    while( form_triplets_from_cells(leftover_nodes) && form_helices_from_triplets(&the_helices, icluster) ){

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_2: helix loop: clean ","cumulative");

      assigned_nodes.clear();
      experimental_legendre_vector->reset();

      if (level >= mybhep::VERBOSE){
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  there are " << leftover_nodes.size() << " leftover nodes and " << assigned_nodes.size() << " assigned nodes, the leg vector has " << experimental_legendre_vector->helices().size() << " helices and " << the_helices.size() << " have been prepared " << std::endl; fflush(stdout);
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
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
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
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  finished track [", sequences_.size()-1, "] with ", s->nodes_.size(), " nodes", mybhep::VERBOSE); fflush(stdout);
        delete s;
        delete center;
      }else{
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_2:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
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
  void sultan::sequentiate_cluster_with_experimental_vector_3(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3 ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3 ");
      return;
    }

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  sequentiate cluster with " << cluster_.nodes_.size() << " nodes, leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

    topology::experimental_helix b;
    vector<topology::node> assigned_nodes, leftover_nodes;
    topology::sequence *s;
    topology::experimental_point *center;
    topology::experimental_double radius;
    topology::experimental_double pitch;
    std::vector<size_t> neighbouring_cells;
    std::vector<topology::experimental_helix> the_helices;

    leftover_nodes = cluster_.nodes_;

    while( form_triplets_from_cells(leftover_nodes) && form_helices_from_triplets(&the_helices, icluster) ){

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: clean ","cumulative");

      assigned_nodes.clear();
      experimental_legendre_vector->reset();
      neighbouring_cells.clear();

      if (level >= mybhep::VERBOSE){
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  leg vector has " << experimental_legendre_vector->helices().size() << " helices " << std::endl; fflush(stdout);
      }

      if( !the_helices.size() ){
        clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: clean ");
        break;
      }
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: clean ");


      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: add helix ","cumulative");

      for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
        experimental_legendre_vector->add_helix(*hh);
      }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: add helix ");

      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: max ","cumulative");

      b = experimental_legendre_vector->max(&neighbouring_cells);

      if( !b.ids().size() ){
        m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  could not make a track ", mybhep::VERBOSE); fflush(stdout);
        clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: max ");
        break;
      }


      if (level >= mybhep::VERBOSE)
        {
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  best helix: " ; b.dump();
          std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_3:  associated " << neighbouring_cells.size() << " cells to this cluster " << std::endl;
        }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: max ");
      clock.start(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: assign ","cumulative");

      bool ok = assign_nodes_based_on_experimental_helix(cluster_.nodes_, assigned_nodes, leftover_nodes, &b, &neighbouring_cells);

      if( ok ){
        s = new topology::sequence(assigned_nodes, level, probmin);
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
        clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: assign ");
        break;
      }

      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3: helix loop: assign ");

      if( print_event_display ) break;

    }


    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_3 ");

    return;
  }


  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_4(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4 ","cumulative");

    experimental_legendre_vector->reset();

    if( cluster_.nodes_.size() < 3 ){
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

    form_triplets_from_cells(cluster_.nodes_);
    form_helices_from_triplets(&the_helices, icluster);

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: clean ","cumulative");

    experimental_legendre_vector->reset();

    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: clean ");

    if( !the_helices.size() ){
      clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4 ");
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: could not make a track ", mybhep::VERBOSE); fflush(stdout);
      return;
    }

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: add helix to clusters ","cumulative");

    // bool force_neighbours_to_have_different_ids = false;
    for(std::vector<topology::experimental_helix>::const_iterator hh = the_helices.begin(); hh!=the_helices.end(); ++hh){
      experimental_legendre_vector->add_helix_to_clusters(*hh);
    }

    if (level >= mybhep::VERBOSE)
      {
        std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_4: sultan: the " <<  the_helices.size() << " helices have been combined into " << experimental_legendre_vector->clusters().size() << " clusters of helices " << std::endl;
      }

    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: add helix to clusters ");

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: find clusters ","cumulative");

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

    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: find clusters ");
    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: assign ","cumulative");

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

    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4: helix loop: assign ");

    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_4 ");

    return;
  }


  //*************************************************************
  bool sultan::check_if_cell_is_near_calo(topology::cell c){
  //*************************************************************

    std::string cview;
    if( c.is_near_calo() ) cview="x";
    else if( c.is_near_xcalo() ) cview="y";
    else if( c.is_near_gveto() ) cview="z";
    else
      return false;

    topology::experimental_vector dist;
    topology::experimental_point cp;
    topology::experimental_point p = c.ep();
    topology::experimental_double distance;

    p.set_x(topology::experimental_double(p.x().value(), cell_distance));
    p.set_y(topology::experimental_double(p.y().value(), cell_distance));

    // loop on calo hits
    for(std::vector<topology::calorimeter_hit>::const_iterator icalo=calos_.begin(); icalo<calos_.end(); ++icalo){

      if( strcmp(icalo->pl().view().c_str(), cview.c_str()) != 0 ) continue;

      cp = icalo->pl().face();
      cp.set_x(topology::experimental_double(cp.x().value(), icalo->pl().sizes().x().value()));
      cp.set_y(topology::experimental_double(cp.y().value(), icalo->pl().sizes().y().value()));
      cp.set_z(topology::experimental_double(cp.z().value(), icalo->pl().sizes().z().value()));
      dist = cp - p;
      distance = dist.length();

      if( distance.is_less_than__optimist(topology::experimental_double(0.,0.), nsigmas) ) return true;

    }

    return false;

  }

  //*************************************************************
  std::vector<topology::cluster> sultan::get_clusters_of_cells_to_be_used_as_end_points(){
  //*************************************************************

    const std::vector<topology::node> & input_nodes = leftover_cluster_->nodes();
    m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: looking for clusters of endpoints in a cluster of ", input_nodes.size(), " cells, with ", calos_.size(), " calo hits", mybhep::VVERBOSE);

    std::vector<topology::cluster> clusters;

    bool node_has_been_added_to_cluster;

    bool on_foil, on_calo, on_xcalo, on_gveto, on_calo_hit;

    // loop on all input nodes
    for(std::vector<topology::node>::const_iterator inode=input_nodes.begin(); inode<input_nodes.end(); ++inode){

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
	  if( jnode->c().near_level(inode->c(), nofflayers, cell_distance) == 3 ){  // side-by-side cells
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


    m.message("SULTAN::sultan::get_clusters_of_cells_to_be_used_as_end_points: " , clusters.size() , " clusters of endpoints have been found in a cluster of ", input_nodes.size(), " cells " , mybhep::VERBOSE);
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

    return clusters;

  }

  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_5_on_endpoints(topology::cluster & cluster_, size_t icluster, bool * cluster_is_finished, std::vector<topology::cluster> * cs_given_endpoints, std::vector<topology::node>::const_iterator inode , std::vector<topology::node>::const_iterator jnode  ) {
    //*************************************************************
    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_endpoints: ", mybhep::VVERBOSE);

    //assigned_cluster_->nodes_.clear();
    //*leftover_cluster_ = cluster_;

    *cs_given_endpoints = get_clusters_from(cluster_, *inode, *jnode, icluster, cluster_is_finished);
    
    if (level >= mybhep::VVERBOSE){
      std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_endpoints:  endpoints : " << inode->c().id() << " - " << jnode->c().id() << " --> " << cs_given_endpoints->size() << " clusters of cells containing: (" ;
      for( std::vector<topology::cluster>::const_iterator pclu=cs_given_endpoints->begin(); pclu!=cs_given_endpoints->end(); ++pclu){
	std::clog << pclu->nodes().size() << " ";
      }
      
      std::clog << ") cells " << std::endl;
    }
    *cs_given_endpoints = clean_up(*cs_given_endpoints);
    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_endpoints:  after cleaning ", cs_given_endpoints->size(), " clusters remain with endpoints : ", inode->c().id(), " - " , jnode->c().id(), " cluster_is_finished: ", *cluster_is_finished, mybhep::VVERBOSE);

    return;

  }

  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_5_on_clusters_of_endpoints(topology::cluster & cluster_, size_t icluster, bool * cluster_is_finished,  const std::vector<topology::node> &inodes,  const std::vector<topology::node> &jnodes, std::vector<topology::cluster> * cs ) {
    //*************************************************************
    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_clusters_of_endpoints:  2nd cluster of endpints has " , jnodes.size() , " nodes " , mybhep::VVERBOSE);
    
    std::vector<topology::cluster> cs_given_endpoints;
    std::vector<topology::cluster> cs_given_clusters_of_endpoints;
    
    for( std::vector<topology::node>::const_iterator inode = inodes.begin(); inode != inodes.end(); ++inode ){
      
      if( * cluster_is_finished ) break;
      
      for( std::vector<topology::node>::const_iterator jnode = jnodes.begin(); jnode != jnodes.end(); ++jnode ){
	
	if( * cluster_is_finished ) break;
	
	sequentiate_cluster_with_experimental_vector_5_on_endpoints(cluster_, icluster, cluster_is_finished, &cs_given_endpoints, inode, jnode);

	cs_given_clusters_of_endpoints.insert(cs_given_clusters_of_endpoints.end(),cs_given_endpoints.begin(),cs_given_endpoints.end());

	if( * cluster_is_finished ){
	  break;
	}
      }
    }

    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_clusters_of_endpoints:  " , cs_given_clusters_of_endpoints.size() , " clusters of cells have been done between 2 clusters of endpoints " , mybhep::VERBOSE);
    cs_given_clusters_of_endpoints = clean_up(cs_given_clusters_of_endpoints);
    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_clusters_of_endpoints:  after cleaning ", cs_given_clusters_of_endpoints.size(), " clusters remain between 2 clusters of endpoints", mybhep::VVERBOSE);
    cs->insert(cs->end(),cs_given_clusters_of_endpoints.begin(),cs_given_clusters_of_endpoints.end());
    
    return;

  }

  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_5_on_vector_of_clusters_of_endpoints(topology::cluster & cluster_, size_t icluster, std::vector<topology::cluster> * cs, std::vector<topology::cluster> clusters_of_endpoints, bool * cluster_is_finished ) {
    //*************************************************************


    // loop on 1st cluster of endpoints
    for(std::vector<topology::cluster>::const_iterator iclu = clusters_of_endpoints.begin(); iclu != clusters_of_endpoints.end() - 1; ++iclu){
      
      const std::vector<topology::node> &inodes = iclu->nodes();
      
      if( * cluster_is_finished ){
	break;
      }
      
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5_on_vector_of_clusters_of_endpoints:  1st cluster of endpints has " , inodes.size() , " nodes " , mybhep::VVERBOSE);
      
      // loop on 2nd cluster of endpoints
      for(std::vector<topology::cluster>::const_iterator jclu = iclu+1; jclu != clusters_of_endpoints.end(); ++jclu){
	
	if( * cluster_is_finished ){
	  break;
	}
	
	const std::vector<topology::node> &jnodes = jclu->nodes();
	
	sequentiate_cluster_with_experimental_vector_5_on_clusters_of_endpoints( cluster_, icluster, cluster_is_finished, inodes, jnodes, cs);

      }
    }

    return;

  }

  //*************************************************************
  void sultan::sequentiate_cluster_with_experimental_vector_5(topology::cluster & cluster_, size_t icluster) {
    //*************************************************************

    clock.start(" sultan: sequentiate_cluster_with_experimental_vector_5 ","cumulative");

    *leftover_cluster_ = cluster_;
    assigned_cluster_->nodes_.clear();
    std::vector<topology::sequence> sequences;
    std::vector<topology::cluster> cs;

    bool there_are_nodes_to_clusterize=true;
    while( there_are_nodes_to_clusterize ){

      sequences.clear();
      cs.clear();

      bool cluster_is_finished = false;
      std::vector<topology::cluster> clusters_of_endpoints = get_clusters_of_cells_to_be_used_as_end_points();

      if( clusters_of_endpoints.size() < 2 ) {
	m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5: cannot sequentiate because there are ", clusters_of_endpoints.size(), " clusters of endpoints " , mybhep::VVERBOSE);
	break;
      }

      sequentiate_cluster_with_experimental_vector_5_on_vector_of_clusters_of_endpoints(cluster_, icluster, & cs, clusters_of_endpoints, &cluster_is_finished);

      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  " , cs.size() , " clusters of cells have been done between all", clusters_of_endpoints.size(), " clusters of endpoints " , mybhep::VERBOSE);

      if( leftover_cluster_->nodes().size() + assigned_cluster_->nodes().size() != cluster_.nodes().size() ){
	m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  problem: leftover nodes: ", leftover_cluster_->nodes().size(), " assigned nodes: ", assigned_cluster_->nodes().size(), " initial nodes: ",  cluster_.nodes().size(), mybhep::NORMAL); fflush(stdout);

      }
    
      // loop on clusters
      for(std::vector<topology::cluster>::const_iterator iclu = cs.begin(); iclu != cs.end(); ++iclu){
	create_sequence_from_cluster(&sequences, iclu->nodes());
      }
      
    
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  the ", cluster_.nodes().size(), " cells have been reduced to ", sequences.size(), " candidate sequences", mybhep::VERBOSE); fflush(stdout);

      
      sequences = clean_up(sequences);
      
      
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  after clean_up, ", sequences.size(), "sequences remain", mybhep::VERBOSE); fflush(stdout);
      
      if (level >= mybhep::VERBOSE)
	{
	  for(std::vector<topology::sequence>::const_iterator is=sequences.begin(); is!=sequences.end(); ++is){
	    std::clog << "SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  sequence [" << is - sequences.begin() << "] has " << is->nodes_.size() << " nodes " ;
	    const std::vector<topology::node> nodes = is->nodes();
	    for( std::vector<topology::node>::const_iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	      std::clog << inode->c().id() << " " ;
	    std::clog << " " << std::endl;
	  }
	}
    
      sequences_.insert(sequences_.end(),sequences.begin(),sequences.end());;

      there_are_nodes_to_clusterize = !cluster_is_finished &&
	sequences.size() &&
	(leftover_cluster_->nodes().size() < cluster_.nodes().size()) &&
	(assigned_cluster_->nodes().size() ) ;

      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  status: sequences size: ", sequences.size(), " leftover:", leftover_cluster_->nodes().size(), " assigned:", assigned_cluster_->nodes().size(), " cluster_is_finished:", cluster_is_finished, " there_are_nodes_to_clusterize:", there_are_nodes_to_clusterize, mybhep::VERBOSE); fflush(stdout);

    }


    m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  made ", sequences_.size(), " sequences, leftover hits = ", leftover_cluster_->nodes().size(), mybhep::VERBOSE); fflush(stdout);

    // separate unclustered hits into clusters based solely on nearness
    if( leftover_cluster_->nodes().size() ){

      std::vector<size_t> the_first_cell_of_piece, length_of_piece;
      std::vector<topology::node> the_assigned_nodes;
      std::vector<topology::node> assigned_nodes = leftover_cluster_->nodes();
      std::sort(assigned_nodes.begin(),assigned_nodes.end(),topology::node::circle_order);
      bool ok = break_cluster_into_continous_parts(leftover_cluster_, &the_first_cell_of_piece, &length_of_piece, &the_assigned_nodes);
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  leftover hits = ", 
		leftover_cluster_->nodes().size(), " broken into ", length_of_piece.size(), " pieces with ", the_first_cell_of_piece.size(), " first cells", mybhep::VERBOSE); fflush(stdout);
      size_t minimum_length=2;
      
      sequences.clear();
      for(std::vector<size_t>::const_iterator il=length_of_piece.begin();il!=length_of_piece.end();++il){
	size_t index= il - length_of_piece.begin();
	size_t first=the_first_cell_of_piece[index];
	size_t last=the_first_cell_of_piece[0];
	if( index + 1 < length_of_piece.size() )
	  last = the_first_cell_of_piece[index + 1];
	m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  piece ", index, " L ", *il, " first ", first, "last", last, " minimum length ", minimum_length, mybhep::VERBOSE); fflush(stdout);
	if( *il < minimum_length ) continue;
	std::vector<topology::node> the_nodes;
	if( first < last ){
	  for( std::vector<topology::node>::const_iterator inode=assigned_nodes.begin() + first; inode!=assigned_nodes.begin() + last; ++inode){
	    the_nodes.push_back(*inode);
	  }
	}else if( first == last ){ // there is only one piece
	  for( std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.end(); ++inode){
	    the_nodes.push_back(*inode);
	  }
	}else{
	  for( std::vector<topology::node>::const_iterator inode=assigned_nodes.begin() + last; inode!=assigned_nodes.end(); ++inode){
	    the_nodes.push_back(*inode);
	  }
	  for( std::vector<topology::node>::const_iterator inode=assigned_nodes.begin(); inode!=assigned_nodes.begin() + first; ++inode){
	    the_nodes.push_back(*inode);
	  }
	  
	}
	
	create_sequence_from_cluster(&sequences, the_nodes);
      }
 
      m.message("SULTAN::sultan::sequentiate_cluster_with_experimental_vector_5:  adding ", sequences.size(), " unclusterized clusters", mybhep::VERBOSE); fflush(stdout);
      sequences_.insert(sequences_.end(),sequences.begin(),sequences.end());
      
   }
  
 
    clock.stop(" sultan: sequentiate_cluster_with_experimental_vector_5 ");

    return;
  }


  //*************************************************************
  void sultan::create_sequence_from_cluster(std::vector<topology::sequence> * sequences, const std::vector<topology::node> & nodes){
  //*************************************************************

      if( nodes.size() ){

	topology::sequence s(nodes, level, probmin);
	make_name(s);
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


  //*************************************************************
  topology::cluster sultan::get_cluster_from(topology::cell_triplet t, topology::experimental_helix helix, std::vector<topology::node> full_nodes){
  //*************************************************************
    // make a cluster with the nodes intercepted by helix
    m.message("SULTAN::sultan::get_cluster_from: , make a cluster with the nodes intercepted by helix through ", t.ca().id()," - ", t.cb().id(), " - ",t.cc().id(), mybhep::VVERBOSE);

    topology::cluster c;
    topology::experimental_double DR, DH;
    //topology::experimental_double DR_limit(cell_distance,0.);
    topology::experimental_double DR_limit(0.,0.);
    topology::experimental_double DH_limit(0.,0.);
    topology::experimental_point p_circle;
    double angle, angle_a, angle_b;

    helix.get_phi_of_point(t.ca().ep(), &p_circle, &angle_a);
    helix.get_phi_of_point(t.cc().ep(), &p_circle, &angle_b);
    if( angle_a > angle_b ){ // swap, ensure angle_a < angle_b
      angle=angle_a;
      angle_a = angle_b;
      angle_b= angle;
    }
      
    if( level >= mybhep::VERBOSE ){
      std::clog << "SULTAN::sultan::get_cluster_from: ...: [cell, distance_hor, distance_vert, chosen] : ";
    }

    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= leftover_cluster_->nodes_.begin(); inode!=leftover_cluster_->nodes_.end(); ++inode){
	
      //helix.distance_from_cell_center(inode->c(), &DR, &DH);
      helix.distance_from_cell_measurement(inode->c(), &DR, &DH);
      

      bool chosen = (bool)(DR.is_less_than__optimist(DR_limit, nsigma_r) && DH.is_less_than__optimist(DH_limit, nsigma_z));

      if( level >= mybhep::VVERBOSE ){
	std::clog << "SULTAN::sultan::get_cluster_from: ..., cell " << inode->c().id() << " has distance hor "; DR.dump(); std::clog << ",  vert "; DH.dump(); std::clog << " from helix, chosen " << chosen << std::endl;
      }

      if( level >= mybhep::VERBOSE ){
	std::clog << "[" << inode->c().id() << ", "; DR.dump(); std::clog << ",  "; DH.dump(); std::clog << " , " << chosen << "] ";
      }

      if( !chosen ) continue;
      
      // cell is close to helix
      
      helix.get_phi_of_point(inode->c().ep(), &p_circle, &angle);

      // ensure cell is between limits
      if( angle < angle_a ){ 
	if( level >= mybhep::VERBOSE ){
	  std::clog << "[" << inode->c().id() << " before a] ";
	}
	continue;
      }
      if( angle > angle_b ){
	if( level >= mybhep::VERBOSE ){
	  std::clog << "[" << inode->c().id() << " after b] ";
	}
	continue;
      }

      m.message("SULTAN::sultan::get_cluster_from: ... ..., cell " , inode->c().id() , " is intercepted " , mybhep::VVERBOSE);

      // add circle position
      inode->set_circle_phi(angle);
      inode->set_ep(p_circle);

      c.nodes_.push_back(*inode);
      
    }

    if( level >= mybhep::VERBOSE ){
      std::clog << " " << std::endl;
    }

    m.message("SULTAN::sultan::get_cluster_from: , a cluster for the helix has been made with ", c.nodes().size(), " nodes", mybhep::VVERBOSE);

    if( c.is_good() ){
      
      // add to cluster from full nodes
      c = add_cells_to_helix_cluster_from(c, full_nodes, t, helix);
      
      check_continous_cells(&c);
    }



    return c;


  }

  //*************************************************************
  topology::cluster sultan::add_cells_to_helix_cluster_from(topology::cluster cluster, std::vector<topology::node> full_nodes, topology::cell_triplet t, topology::experimental_helix helix){
  //*************************************************************
    // make a cluster with the nodes intercepted by helix
    m.message("SULTAN::sultan::add_cells_to_helix_cluster_from: , make a cluster with the nodes intercepted by helix ", mybhep::VVERBOSE);

    topology::cluster c = cluster;
    topology::experimental_double DR, DH;
    //topology::experimental_double DR_limit(cell_distance,0.);
    topology::experimental_double DR_limit(0.,0.);
    topology::experimental_double DH_limit(0.,0.);
    topology::experimental_point p_circle;
    double angle, angle_a, angle_b;

    helix.get_phi_of_point(t.ca().ep(), &p_circle, &angle_a);
    helix.get_phi_of_point(t.cc().ep(), &p_circle, &angle_b);
    if( angle_a > angle_b ){ // swap, ensure angle_a < angle_b
      angle=angle_a;
      angle_a = angle_b;
      angle_b= angle;
    }

    const std::vector<topology::node> nodes_to_be_skipped = leftover_cluster_->nodes();

    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= full_nodes.begin(); inode!=full_nodes.end(); ++inode){

      bool node_already_in_cluster = false;

      for(std::vector<topology::node>::const_iterator jnode= nodes_to_be_skipped.begin(); jnode!=nodes_to_be_skipped.end(); ++jnode){
	if( inode->c().id() == jnode->c().id() ){
	  node_already_in_cluster = true;
	  break;
	}
      }
      
      if( node_already_in_cluster ) continue;

	
      //helix.distance_from_cell_center(inode->c(), &DR, &DH);
      helix.distance_from_cell_measurement(inode->c(), &DR, &DH);
      
      m.message("SULTAN::sultan::add_cells_to_helix_cluster_from: ..., cell " , inode->c().id() , " has distance DR " , DR.value() , " +- " , DR.error() , " DH " , DH.value() , " +- " , DH.error() , " from helix " , mybhep::VVERBOSE);
      
      if( !DR.is_less_than__optimist(DR_limit, nsigma_r) ) continue;
      
      if( !DH.is_less_than__optimist(DH_limit, nsigma_z) ) continue;
      
      // cell is close to helix
      m.message("SULTAN::sultan::add_cells_to_helix_cluster_from: ... ..., cell " , inode->c().id() , " is intercepted " , mybhep::VVERBOSE);
      
      // add circle position
      helix.get_phi_of_point(inode->c().ep(), &p_circle, &angle);

      // ensure cell is between limits
      if( angle < angle_a ) continue;
      if( angle > angle_b ) continue;

      inode->set_circle_phi(angle);
      inode->set_ep(p_circle);

      c.nodes_.push_back(*inode);
      
    }

    m.message("SULTAN::sultan::add_cells_to_helix_cluster_from: , a cluster for the helix has been made with ", c.nodes().size(), " nodes", mybhep::VVERBOSE);

    return c;

  }


  //*************************************************************
  topology::cluster sultan::get_line_cluster_from(topology::cluster full_cluster, topology::node a_node, topology::node b_node){
  //*************************************************************
    // get cluster with cells intercepted by line ab
    std::vector<topology::node> nodes = leftover_cluster_->nodes();
    m.message("SULTAN::sultan::get_line_cluster_from: get cluster with cells intercepted by line ab ", a_node.c().id(), "-", b_node.c().id(), " in ", nodes.size(), " cells ", mybhep::VVERBOSE);

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

    topology::cluster c;
    a_node.set_circle_phi(0);
    c.nodes_.push_back(a_node);

    //topology::experimental_double dist_hor_limit(cell_distance,0.);
    topology::experimental_double dist_hor_limit(0.,0.);
    topology::experimental_double dist_vert_limit(0.,0.);

    topology::experimental_double DR, DH;
    double normalized_parameter_along_line;

    if( level >= mybhep::VERBOSE ){
      std::clog << "SULTAN::sultan::get_line_cluster_from: ...: [cell, distance_hor, distance_vert, chosen] : ";
    }

    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= nodes.begin(); inode!=nodes.end(); ++inode){

      if( inode->c().id() == a_node.c().id() ) continue;
      if( inode->c().id() == b_node.c().id() ) continue;

      normalized_parameter_along_line = line.distance_from_cell_center(inode->c(), &DR, &DH);

      // ensure parameter is between limits
      if( normalized_parameter_along_line < 0 ){
	if( level >= mybhep::VERBOSE ){
	  std::clog << "[" << inode->c().id() << " before a] ";
	}
	continue;
      }
      if( normalized_parameter_along_line > 1 ){
	if( level >= mybhep::VERBOSE ){
	  std::clog << "[" << inode->c().id() << " after b] ";
	}
	continue;
      }

      bool chosen = (bool)(DR.is_less_than__optimist(dist_hor_limit, nsigma_r) && DH.is_less_than__optimist(dist_vert_limit, nsigma_z));

      if( level >= mybhep::VERBOSE ){
	std::clog << "[" << inode->c().id() << ", "; DR.dump(); std::clog << ",  "; DH.dump(); std::clog << " , " << chosen << "] ";
      }

      
      if( !chosen ) continue;
      
      // cell is close to line
      
      // add circle position: in case of line it is parameter along line
      inode->set_circle_phi(normalized_parameter_along_line);

      c.nodes_.push_back(*inode);
      
    }

    if( level >= mybhep::VERBOSE ){
      std::clog << " " << std::endl;
    }

    b_node.set_circle_phi(1);
    c.nodes_.push_back(b_node);

    m.message("SULTAN::sultan::get_line_cluster_from: the line ",a_node.c().id(), " - ", b_node.c().id(), " intercepts " , c.nodes_.size() , " nodes " , mybhep::VVERBOSE);

    if( c.is_good() ){
      
      c = add_cells_to_line_cluster_from(c, full_cluster, a_node, b_node);
      
      check_continous_cells(&c);

    }

    return c;


  }


  //*************************************************************
  topology::cluster sultan::add_cells_to_line_cluster_from(topology::cluster cluster, topology::cluster full_cluster, topology::node a_node, topology::node b_node){
  //*************************************************************
    // add to line (obtained from cluster) cells from full_cluster
    m.message("SULTAN::sultan::add_cells_to_line_cluster_from: get cluster with cells intercepted by line ab " , mybhep::VVERBOSE);

    topology::experimental_point a = a_node.c().ep();
    a.set_x(topology::experimental_double(a.x().value(), a_node.c().r().value()));
    a.set_y(topology::experimental_double(a.y().value(), a_node.c().r().value()));
    topology::experimental_point b = b_node.c().ep();
    b.set_x(topology::experimental_double(b.x().value(), b_node.c().r().value()));
    b.set_y(topology::experimental_double(b.y().value(), b_node.c().r().value()));

    topology::cluster c = cluster;

    topology::experimental_vector ab = b - a;
    // unit direction from a to b
    topology::experimental_vector dir = ab.unit();

    //topology::experimental_double dist_hor_limit(cell_distance,0.);
    topology::experimental_double dist_hor_limit(0.,0.);
    topology::experimental_double dist_vert_limit(0.,0.);

    std::vector<topology::node> full_nodes = full_cluster.nodes();
    std::vector<topology::node> cnodes = leftover_cluster_->nodes();

    topology::experimental_point p;
    topology::experimental_vector ap, dist;
    topology::experimental_double dist_hor, dist_vert;
    topology::experimental_double parameter_along_line;
    double normalized_parameter_along_line;

    // loop on cells in the cluster
    for(std::vector<topology::node>::iterator inode= full_nodes.begin(); inode!=full_nodes.end(); ++inode){

      if( inode->c().id() == a_node.c().id() ) continue;
      if( inode->c().id() == b_node.c().id() ) continue;

      bool node_already_in_cluster = false;

      for(std::vector<topology::node>::iterator jnode= cnodes.begin(); jnode!=cnodes.end(); ++jnode){
	if( inode->c().id() == jnode->c().id() ){
	  node_already_in_cluster = true;
	  break;
	}
      }

      if( node_already_in_cluster ) continue;

      p = inode->c().ep();
      p.set_x(topology::experimental_double(p.x().value(), inode->c().r().value()));
      p.set_y(topology::experimental_double(p.y().value(), inode->c().r().value()));
      
      // vector from a to p
      ap = p - a;

      parameter_along_line=dir*ap;
      // 0 for p = a
      // 1 for p = b
      normalized_parameter_along_line = parameter_along_line.value()/ab.length().value();

      // ensure parameter is between limits
      if( normalized_parameter_along_line < 0 ) continue;
      if( normalized_parameter_along_line > 1 ) continue;

      dist = dir*parameter_along_line - ap;
      dist_hor = dist.hor().length();
      dist_vert = experimental_fabs(dist.z());

      m.message("SULTAN::sultan::add_cells_to_line_cluster_from: ..., cell " , inode->c().id() , " has distance hor " , dist_hor.value() , " +- " , dist_hor.error() , " vert " , dist_vert.value() , " +- " , dist_vert.error() , " from line " , mybhep::VVERBOSE);
      
      if( !dist_hor.is_less_than__optimist(dist_hor_limit, nsigma_r) ) continue;
      
      if( !dist_vert.is_less_than__optimist(dist_vert_limit, nsigma_z) ) continue;
      
      // cell is close to line
      m.message("SULTAN::sultan::add_cells_to_line_cluster_from: ... ..., cell " , inode->c().id() , " is intercepted " , mybhep::VVERBOSE);
      
      // add circle position: in case of line it is parameter along line
      inode->set_circle_phi(normalized_parameter_along_line);

      c.nodes_.push_back(*inode);
      
    }

    m.message("SULTAN::sultan::add_cells_to_line_cluster_from: the line ",a_node.c().id(), " - ", b_node.c().id(), " intercepts " , c.nodes_.size() , " nodes " , mybhep::VVERBOSE);

    return c;


  }


  //*************************************************************
  std::vector<topology::cluster> sultan::get_helix_clusters_from(std::vector<topology::node> full_nodes, topology::node a, topology::node b, size_t icluster, bool *cluster_is_finished){
  //*************************************************************
    // get all clusters based on helices built on triplets (a, X, b), with X in cluster
    m.message("SULTAN::sultan::get_helix_clusters_from: get all clusters based on helices built on triplets (a, X, b), with X in cluster of", leftover_cluster_->nodes().size(), " cells " , mybhep::VVERBOSE);
    std::vector<topology::cluster> cs;
    std::vector<topology::experimental_helix> helices;

    const std::vector<topology::node> leftover_nodes = leftover_cluster_->nodes();


    // loop on cells in the cluster of leftover nodes for form a triplet (a, X, b)
    for(std::vector<topology::node>::const_iterator inode= leftover_nodes.begin(); inode!=leftover_nodes.end(); ++inode){

      m.message("SULTAN::sultan::get_helix_clusters_from:  ... build helices for triplet ( " , a.c().id() , ", " , inode->c().id() , ", " , b.c().id() , ") using node ", inode - leftover_nodes.begin(), " of ", leftover_nodes.size(), mybhep::VVERBOSE);

      if( inode->c().id() == a.c().id() ) continue;
      if( inode->c().id() == b.c().id() ) continue;

      triplets_.clear();
      topology::cell_triplet t(a.c(), inode->c(), b.c(), level, probmin);
      triplets_.push_back(t);
      helices.clear();
      
      m.message("SULTAN::sultan::get_helix_clusters_from:  ... build helices for triplet ( " , a.c().id() , ", " , inode->c().id() , ", " , b.c().id() , ")" , mybhep::VVERBOSE);


      if( !form_helices_from_triplets(&helices, icluster) ) continue;

      m.message("SULTAN::sultan::get_helix_clusters_from:  ..., " , helices.size() , " helices produced " , mybhep::VVERBOSE);
          
      if( level >= mybhep::VERBOSE ){
	std::clog << " triplet (" << a.c().id() << ", " << inode->c().id() << ", " << b.c().id() << ") gives " << helices.size() << " helices " << std::endl;
      }

      // choose 1 best cluster from triplet (a, X, b)
      size_t nmax=0;
      topology::cluster cmax;

      // loop on helices
      for(std::vector<topology::experimental_helix>::const_iterator ihelix= helices.begin(); ihelix!=helices.end(); ++ihelix){

	// form cluster from leftover nodes
	topology::cluster c = get_cluster_from(t, *ihelix, full_nodes);

	if( c.is_good() ){
	  m.message("SULTAN::sultan::get_helix_clusters_from:  ... ... helix " , ihelix - helices.begin() , " makes a good cluster of " , c.nodes().size(), " cells, nmax ", nmax , mybhep::VERBOSE);
	  
	  const std::vector<topology::node> & cnodes = c.nodes();
	  if( cnodes.size() > nmax ){
	    nmax = cnodes.size();
	    cmax = c;
	    
	    if( cnodes.size() == full_nodes.size() ){
	      m.message("SULTAN::sultan::get_helix_clusters_from:  all", cnodes.size(), "cells of cluster have been assigned as a helix ", mybhep::VERBOSE);
	      *cluster_is_finished = true;
	      assign_nodes_of_cluster(c);
	      cs.push_back(c);
	      return cs;
	    }
	    
	  }
	}

      } // finish loop on helices from (a, X, b)

      m.message("SULTAN::sultan::get_helix_clusters_from:  the best cluster has ", nmax, " nodes ", mybhep::VVERBOSE);
      if( nmax > 0 ){ // 1 best cluster was found

	assign_nodes_of_cluster(cmax);
	cs.push_back(cmax);

	if( assigned_cluster_->nodes().size() == full_nodes.size() ){
	  m.message("SULTAN::sultan::get_helix_clusters_from:  all", assigned_cluster_->nodes().size(), "cells of cluster have been assigned as a helix ", mybhep::VERBOSE);
	  *cluster_is_finished = true;
	  return cs;
	}
	
      }

    } // finish loop on cell X in (a, X, b)

    m.message("SULTAN::sultan::get_helix_clusters_from:  helices built on triplets (a, X, b), with X in cluster, have produced " , cs.size() , " clusters " , mybhep::VVERBOSE);

    return cs;
    


  }


  //*************************************************************
  void sultan::assign_nodes_of_cluster(topology::cluster c){
  //*************************************************************

    const std::vector<topology::node> & cnodes = c.nodes();
    assigned_cluster_->add_nodes(cnodes);
    leftover_cluster_->remove_nodes(cnodes);
  }

  //*************************************************************
  std::vector<topology::cluster> sultan::get_clusters_from(topology::cluster full_cluster, topology::node a, topology::node b, size_t icluster, bool * cluster_is_finished){
  //*************************************************************

    // get all clusters with endpoints a and b
    m.message("SULTAN::sultan::get_clusters_from:  get all clusters with endpoints " , a.c().id() , "->" , b.c().id() , " from cluster with ", full_cluster.nodes().size(), " cells ", mybhep::VERBOSE);
    std::vector<topology::cluster> cs;

    topology::cluster c = get_line_cluster_from(full_cluster, a, b);
    bool clusterized_as_line=false;
    if( c.is_good() ){
      assign_nodes_of_cluster(c);
      cs.push_back(c);
      clusterized_as_line=true;
    }

    if (level >= mybhep::VERBOSE){
      if( clusterized_as_line ){
	const std::vector<topology::node> & cnodes = c.nodes();
	std::clog << "SULTAN::sultan::get_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " clusterized as line with " << cnodes.size() << " cells ( ";
	for( std::vector<topology::node>::const_iterator inode=cnodes.begin(); inode!=cnodes.end(); ++inode){
	  std::clog <<inode->c().id() << " ";
	}
	std::clog <<")" << std::endl;
      }else{
	std::clog << "SULTAN::sultan::get_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " not clusterized as line " << std::endl;
      }
    }

    if( assigned_cluster_->nodes().size() == full_cluster.nodes().size() ){
      m.message("SULTAN::sultan::get_clusters_from: all", assigned_cluster_->nodes().size(), "cells of cluster have been assigned as a line ", mybhep::VERBOSE);
      *cluster_is_finished = true;
      return cs;
    }


    std::vector<topology::cluster> csh = get_helix_clusters_from(full_cluster.nodes(), a, b, icluster, cluster_is_finished);
  
    if (level >= mybhep::VERBOSE){
      if( csh.size() ){
	for( std::vector<topology::cluster>::const_iterator iclu=csh.begin(); iclu!=csh.end(); ++iclu){
	  const std::vector<topology::node> & cnodes = iclu->nodes();
	  std::clog << "SULTAN::sultan::get_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " clusterized as helix with " << cnodes.size() << " cells ( ";
	  for( std::vector<topology::node>::const_iterator inode=cnodes.begin(); inode!=cnodes.end(); ++inode){
	    std::clog <<inode->c().id() << " ";
	  }
	  std::clog <<")" << std::endl;
	}
      }else{
	std::clog << "SULTAN::sultan::get_clusters_from:  " << a.c().id() << " -> " << b.c().id() << " not clusterized as helix " << std::endl;
      }
    }

    m.message("SULTAN::sultan::get_clusters_from: gotten " , csh.size() , " helix clusters with endpoints a " , a.c().id() , " and b " , b.c().id() , mybhep::VVERBOSE);

    // loop on helix clusters
    for(std::vector<topology::cluster>::const_iterator iclu= csh.begin(); iclu!=csh.end(); ++iclu){
      if( iclu->is_good() )
	cs.push_back(*iclu);
    }


    m.message("SULTAN::sultan::get_clusters_from: gotten " , cs.size() , " good clusters with endpoints a " , a.c().id() , " and b " , b.c().id() , mybhep::VVERBOSE);

    return cs;


  }





} // end of namespace SULTAN
