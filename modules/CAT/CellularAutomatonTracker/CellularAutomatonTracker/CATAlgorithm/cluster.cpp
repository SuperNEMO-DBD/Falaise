/* -*- mode: c++ -*- */
#include <CATAlgorithm/cluster.h>

namespace CAT{
  namespace topology{

    //!Default constructor
    cluster::cluster()
    {
      appname_= "cluster: ";
      nodes_.clear();
      free_ = false;
    }

    //!Default destructor
    cluster::~cluster(){};

    //! constructor from std::vector of nodes
    cluster::cluster(const std::vector<node> &nodes, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cluster: ";
      nodes_ = nodes;
      free_ = false;
    }

    //! constructor from single node
    cluster::cluster(node &a_node, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cluster: ";
      a_node.set_free(false);
      nodes_.clear();
      nodes_.push_back(a_node);
      free_ = true;
    }

    /*** dump ***/
    void cluster::dump (std::ostream & a_out ,
                        const std::string & a_title  ,
                        const std::string & a_indent,
                        bool /* a_inherit */        ) const {
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " ------------------- " << std::endl;
      a_out << indent << " number of nodes: " << nodes().size() << " free: " << Free() << std::endl;
      for(std::vector<node>::const_iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode)
        inode->dump(a_out, "",indent + "     ");
      a_out << indent << " ------------------- " << std::endl;

      return;
    }

    //! set nodes
    void cluster::set_nodes(const std::vector<node> &nodes){
      nodes_ = nodes;
    }

    //! set free level
    void cluster::set_free(bool free){
      free_ = free;
    }

    //! get nodes
    const std::vector<node> & cluster::nodes()const{
      return nodes_;
    }

    //! get free level
    bool cluster::Free()const{
      return free_;
    }

    bool cluster::has_cell(const cell & c)const{

      if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())
        return true;

      return false;
    }


    cluster cluster::invert(){
      cluster inverted;
      inverted.set_print_level(print_level());
      inverted.set_probmin(probmin());
      inverted.set_free(Free());
      std::vector<node> inverted_nodes;
      for(std::vector<node>::iterator inode = nodes_.end(); inode != nodes_.begin(); --inode){
        inverted_nodes.push_back(*inode);
      }
      inverted.set_nodes( inverted_nodes );
      return inverted;

    }

    topology::node cluster::node_of_cell(const topology::cell & c){

      std::vector<node>::iterator fnode = std::find(nodes_.begin(),
                                                    nodes_.end(),
                                                    c);

      if( fnode == nodes_.end()){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: requested cell " << c.id() << " has no node in cluster. cluster nodes are: " << std::endl;

          for(std::vector<node>::iterator in = nodes_.begin(); in != nodes_.end(); ++in){
            std::clog << " " << in->c().id();
          }

          std::clog << " " << std::endl;
        }

        topology::node null;
        return null;
      }

      return *fnode;

    }

    bool cluster::start_ambiguity(size_t i){
      // node i starts an ambiguity if:
      // - it has 2 joints, and
      // - the connection to the next node is not through a gap, and
      // - it is the 2nd node, or the previous node has 1 joint, or the previous node comes here through a block


      if( i >= nodes_.size() - 1){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: start ambiguity: i " << i << " size " << nodes_.size() << std::endl;
	  return false;
        }
      }

      if( nodes_[i].ccc().size() == 0 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: start ambiguity: i " << i << " triplet size " << nodes_[i].ccc().size() << std::endl;
	  return false;
        }
      }

      std::vector<topology::joint> joints = nodes_[i].ccc()[0].joints();

      if( joints.size() <= 1 ) return false; // node has 2 joints

      if( i == 0 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: start ambiguity: i " << i << std::endl;
	  return false;
        }
      }


      if( i < nodes_.size() - 1 &&
	  nodes_[i].c().block() != nodes_[i+1].c().block() ) return false; // node does not connect through gap


      if( i == 1 ){
        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::start_ambiguity: node " << nodes_[i].c().id() << " of index " << i 
		    << " and " << joints.size() << " joints starts amibgous piece " << std::endl;
        }
	return true; // second node
      }

      if( i > 0 &&
	  nodes_[i].c().block() != nodes_[i-1].c().block() ){
        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::start_ambiguity: node " << nodes_[i].c().id() << " of index " << i << ", block " << nodes_[i].c().block()
		    << " and " << joints.size() << " joints starts amibgous piece connecting from node " << nodes_[i-1].c().id() << " of block " <<  nodes_[i-1].c().block() << std::endl;
        }
	return true; // previous node connects through gap
      }

      if( nodes_[i-1].ccc().size() == 0 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: start ambiguity: i-1 " << i-1 << std::endl;
	  return false;
        }
      }

      std::vector<topology::joint> prev_joints = nodes_[i-1].ccc()[0].joints();

      if( prev_joints.size() == 1 ){
        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::start_ambiguity: node " << nodes_[i].c().id() << " of index " << i 
		    << " and " << joints.size() << " joints starts amibgous piece after node " << nodes_[i-1].c().id() << " of " << prev_joints.size() << " joints " << std::endl;
        }
	return true; // prev node has 1 joint
      }

      return false;

    }

    bool cluster::end_ambiguity(size_t i){
      // node i ends an ambiguity if:
      // - {it has 2 joints, and
      // - it is the last-but-one node, or the next node has 1 joint,}
      // - or
      // - the connection to the next node is through a gap

      if( i >= nodes_.size() -1 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: end ambiguity: i " << i << " size " << nodes_.size() << std::endl;
	  return false;
        }
      }

      if( nodes_[i].ccc().size() == 0 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: end ambiguity: i " << i << " triplet size " << nodes_[i].ccc().size() << std::endl;
	  return false;
        }
      }

      if( i < nodes_.size() - 1 &&
	  nodes_[i].c().block() != nodes_[i+1].c().block() ){
        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::end_ambiguity: node " << nodes_[i].c().id() << " of index " << i << ", block " << nodes_[i].c().block()
		    << " ends amibgous piece connecting to node " << nodes_[i+1].c().id() << " of block " <<  nodes_[i+1].c().block() << std::endl;
        }
	return true; // node connects through gap
      }

      std::vector<topology::joint> joints = nodes_[i].ccc()[0].joints();

      if( joints.size() <= 1 ) return false; // node has 2 joints

      if( i == nodes_.size() - 2 ){
        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::end_ambiguity: node " << nodes_[i].c().id() << " of index " << i 
		    << " and " << joints.size() << " joints ends amibgous piece " << std::endl;
        }
	return true; // last-but-one node
      }

      if( nodes_[i+1].ccc().size() == 0 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: end ambiguity: i+1 " << i+1 << std::endl;
	  return false;
        }
      }

      std::vector<topology::joint> next_joints = nodes_[i+1].ccc()[0].joints();

      if( next_joints.size() == 1 ){
        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::end_ambiguity: node " << nodes_[i].c().id() << " of index " << i 
		    << " and " << joints.size() << " joints ends amibgous piece before node " << nodes_[i+1].c().id() << " of " << next_joints.size() << " joints " << std::endl;
        }
	return true; // next node has 1 joint
      }

      return false;


    }

    void cluster::solve_ambiguities(std::vector< std::vector<topology::broken_line> > * sets_of_bl_alternatives){


      std::vector<long int> joint_indexes;
      std::vector<long int> best_joint_indexes;
      long int io_value;
      std::vector<node>::iterator first_node, last_node;

      for(first_node = nodes_.begin()+1; first_node != nodes_.end()-1; ++first_node){

	////////////////////////////////////////////////////////////
	////// find starting and ending point of ambiguous piece
	////////////////////////////////////////////////////////////
	if( ! start_ambiguity(first_node - nodes_.begin()) ) continue;

	topology::cluster ambiguous_piece;

	for(last_node = first_node; last_node != nodes_.end()-1; ++last_node){
	  ambiguous_piece.nodes_.push_back(*last_node);
	  if( end_ambiguity(last_node - nodes_.begin()) ){
	    break;
	  }
	}

        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::solve_ambiguities: ambiguous piece: ( ";
	  for( std::vector<node>::const_iterator knode = ambiguous_piece.nodes_.begin(); knode != ambiguous_piece.nodes_.end(); ++knode )
	    std::clog << knode->c().id() << " ";
	  std::clog << ")" << std::endl;
        }

	for( std::vector<node>::const_iterator knode = ambiguous_piece.nodes_.begin(); knode != ambiguous_piece.nodes_.end(); ++knode ){
	  if( knode->ccc().size() == 0 ){
	    if( print_level() >= mybhep::NORMAL ){
	      std::clog << " problem: solve ambiguitues: node " << knode->c().id() << " has no triplets " << std::endl;
	      return ;
	    }
	  }

	  if( knode->ccc()[0].joints().size() < 2 ){
	    if( print_level() >= mybhep::NORMAL ){
	      std::clog << " problem: solve ambiguitues: node " << knode->c().id() << " has " << knode->ccc()[0].joints().size() << " joints " << std::endl;
	      return ;
	    }
	  }
	}
      

	std::vector<topology::broken_line> bls = solve_ambiguities_with_ends(first_node - nodes_.begin(), last_node - nodes_.begin());
	sets_of_bl_alternatives->push_back(bls);

        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::solve_ambiguities: the ambiguous piece: ( ";
	  for( std::vector<node>::const_iterator knode = ambiguous_piece.nodes_.begin(); knode != ambiguous_piece.nodes_.end(); ++knode )
	    std::clog << knode->c().id() << " ";
	  std::clog << ") has given rise to " << bls.size() << " broken lines " << std::endl;
        }

      
#if 0
	////////////////////////////////////////////////////////////
	/// assign possible choices for joints in ambiguous piece
	////////////////////////////////////////////////////////////
	size_t nnodes = ambiguous_piece.nodes().size();
	long double noptions = powl(2, nnodes);

        if( print_level() >= mybhep::VERBOSE ){
          std::clog << " CAT::cluster::solve_ambiguities: nnodes " << nnodes << " noptions " << noptions  << std::endl;
        }
	

	double maxprob = mybhep::default_max;
	// example: nnodes = 3, noptions = 8
	for(long long int io=0; io<noptions; io++){ // example: io = 0, 1, 2, 3, 4, 5, 6, 7

	  joint_indexes.clear();
	  
	  for(size_t in=0; in<nnodes; in++){

	    io_value = (io/(size_t)(pow(2,in)))%2; 
	    // example: in = 0, io_value = 0, 1, 2, 3, 4, 5, 6, 7
	    //          in = 1, io_value = 0, 0, 1, 1, 2, 2, 3, 4
	    //          in = 2, io_value = 0, 0, 0, 0, 1, 1, 1, 1

	    joint_indexes.push_back( io_value );
	    
	  }

	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities: explore option " << io << " of " << noptions << " joints: [ ";
	    for(std::vector<long int>::const_iterator ii = joint_indexes.begin(); ii!=joint_indexes.end(); ++ii)
	      std::clog << *ii << " ";
	    std::clog << "]" << std::endl;
	  }


	  ////////////////////////////////////////////////////////////
	  ////// form broken line with assign joints and get chi2
	  ////////////////////////////////////////////////////////////
	  topology::broken_line bl;
	  bl.eps_.push_back(ambiguous_piece.nodes()[0].ccc()[0].joints()[joint_indexes[0]].epa());
	  for(size_t in=0; in<nnodes; in++){

	    if( ambiguous_piece.nodes()[in].ccc().size() == 0 ){
	      if( print_level() >= mybhep::NORMAL ){
		std::clog << " problem: solve ambiguities: in " << in << " triplet size " << ambiguous_piece.nodes()[in].ccc().size() << std::endl;
		return;
	      }
	    }

	    std::vector<topology::joint> joints = ambiguous_piece.nodes()[in].ccc()[0].joints();

	    if( joint_indexes[in] >= joints.size() ){
	      if( print_level() >= mybhep::NORMAL ){
		std::clog << " problem: solve ambiguities: joint index " << joint_indexes[in] << " joints size " << joints.size() << std::endl;
		return;
	      }
	    }

	    experimental_point ep = joints[joint_indexes[in]].epb();

	    bl.eps_.push_back(ep);
	  }

	  bl.eps_.push_back(ambiguous_piece.nodes().back().ccc()[0].joints()[joint_indexes[nnodes-1]].epc());
	  bl.calculate_chi2();

	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities: chi2 " << bl.chi2() << " ndof " << bl.ndof() << " prob " << bl.p() << std::endl;
	  }

	  if( bl.p() > maxprob ){
	    maxprob = bl.p();
	    best_joint_indexes = joint_indexes;
	  }


	}


	for(std::vector<topology::node>::iterator inode = first_node; inode != last_node+1; ++inode){

	  size_t chosen_joint = best_joint_indexes[inode - first_node];
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities: set node " << inode->c().id() << " to joint " << chosen_joint << std::endl;
	  }
	  std::vector<topology::joint> chosen_joints;
	  chosen_joints.push_back(inode->ccc()[0].joints()[chosen_joint]);
	  inode->ccc_[0].set_joints(chosen_joints);

	}
#endif
      

      }





      return;
    }


    std::vector<topology::broken_line> cluster::solve_ambiguities_with_ends__1_node(size_t ifirst, size_t ilast, bool first_ambiguous_is_after_gap, bool first_ambiguous_is_second, bool last_ambiguous_is_begore_gap, bool last_ambiguous_is_last_but_one){

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;

      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is right after a gap:   gap - A
      if( first_ambiguous_is_after_gap ){

	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last:  gap - A - N |
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: gap - A - N | (should be decided by matching) " <<  std::endl;
	  }
	  
	  for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	    topology::broken_line bl;
	    bl.set_ifirst(ifirst);
	    bl.set_ilast(ifirst+1);
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	    topology::experimental_point pN = nodes_[ifirst].ccc()[0].joints()[joint_index].epc();
	    bl.eps_.push_back(pA);
	    bl.eps_.push_back(pN);
	    bls.push_back(bl);
	  }
	  return bls;
	}

	// gap - A - B  ...
	if( last_ambiguous_is_begore_gap ){ // gap - A - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: gap - A - gap (should be decided by matching) " <<  std::endl;
	  }
	  for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	    topology::broken_line bl;
	    bl.set_ifirst(ifirst);
	    bl.set_ilast(ifirst);
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	    bl.eps_.push_back(pA);
	    bls.push_back(bl);
	  }
	  return bls;
	}
	
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: gap - A - b ... with b singular (should be decided by matching) " <<  std::endl;
	}
	for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	  topology::broken_line bl;
	  bl.set_ifirst(ifirst);
	  bl.set_ilast(ifirst);
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	  bl.eps_.push_back(pA);
	  bls.push_back(bl);
	}
	return bls;

      }
      
      
      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is second node:   0 - A
      if( first_ambiguous_is_second ){

	if( last_ambiguous_is_begore_gap ){
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | 0 - A - gap (should be decided by matching) " <<  std::endl;
	  }
	  for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	    topology::broken_line bl;
	    bl.set_ifirst(ifirst-1);
	    bl.set_ilast(ifirst);
	    topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index].epa();
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	    bl.eps_.push_back(p0);
	    bl.eps_.push_back(pA);
	    bls.push_back(bl);
	  }
	  return bls;
	}
	
	if( !last_ambiguous_is_last_but_one ){ // node after last ambiguous is not the last: 0 - A - b - ...
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | 0 - A - b ... (should be decided by matching) " <<  std::endl;
	  }
	  for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	    topology::broken_line bl;
	    bl.set_ifirst(ifirst-1);
	    bl.set_ilast(ifirst);
	    topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index].epa();
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	    bl.eps_.push_back(p0);
	    bl.eps_.push_back(pA);
	    bls.push_back(bl);
	  }
	  return bls;
	}
	
	// 0 - A - N
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | 0 - A - N | (should be decided by matching)" <<  std::endl;
	}
	for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	  topology::broken_line bl;
	  bl.set_ifirst(ifirst-1);
	  bl.set_ilast(ifirst+1);
	  topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index].epa();
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	  topology::experimental_point pN = nodes_[ifirst].ccc()[0].joints()[joint_index].epc();
	  bl.eps_.push_back(p0);
	  bl.eps_.push_back(pA);
	  bl.eps_.push_back(pN);
	  bls.push_back(bl);
	}
	return bls;
      }
      
      
      if( last_ambiguous_is_last_but_one ){
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | ... a - A - N (should be decided by matching) " <<  std::endl;
	}
	for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	  topology::broken_line bl;
	  bl.set_ifirst(ifirst);
	  bl.set_ilast(ifirst+1);
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	  topology::experimental_point pN = nodes_[ifirst].ccc()[0].joints()[joint_index].epc();
	  bl.eps_.push_back(pA);
	  bl.eps_.push_back(pN);
	  bls.push_back(bl);
	}
	return bls;
      }

      if( last_ambiguous_is_begore_gap ){ // node after last ambiguous is not the last: ... a - A - b - ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | ... a - A - gap (should be decided by matching) " <<  std::endl;
	}
	for( size_t joint_index = 0; joint_index <= 1; ++joint_index){
	  topology::broken_line bl;
	  bl.set_ifirst(ifirst);
	  bl.set_ilast(ifirst);
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index].epb();
	  bl.eps_.push_back(pA);
	  bls.push_back(bl);
	}
	return bls;
      }

      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends:  ... a - A - b ... ; optimize A " <<  std::endl;
      }
      topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
      topology::experimental_point pb = nodes_[ilast+1].ccc()[0].joints()[0].epb();
      topology::broken_line best_bl;
      best_bl.set_ifirst(ifirst);
      best_bl.set_ilast(ifirst);
      topology::experimental_point pAbest;
      for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	topology::broken_line bl;
	topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	bl.eps_.push_back(pa);
	bl.eps_.push_back(pA);
	bl.eps_.push_back(pb);
	bl.calculate_chi2();
	if( bl.p() > maxprob ){
	  maxprob = bl.p();
	  pAbest = pA;
	}
      }
      best_bl.eps_.push_back(pAbest);
      bls.push_back(best_bl);
      return bls;

      
    }


    std::vector<topology::broken_line> cluster::solve_ambiguities_with_ends__2_nodes(size_t ifirst, size_t ilast, bool first_ambiguous_is_after_gap, bool first_ambiguous_is_second, bool last_ambiguous_is_begore_gap, bool last_ambiguous_is_last_but_one){

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;


      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is right after a gap:   gap - A - B
      if( first_ambiguous_is_after_gap ){

	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last:  gap - A - B - N |
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: gap - A - B - N | (should be decided by matching)" <<  std::endl;
	  }
	  
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      bl.set_ifirst(ifirst);
	      bl.set_ilast(ilast+1);
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	      topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_B].epc();
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pN);
	      bls.push_back(bl);
	    }
	  }
	  return bls;
	  
	}

	if( last_ambiguous_is_begore_gap ){ // gap - A - B - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: gap - A - B - gap (should be decided by matching) " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      bl.set_ifirst(ifirst);
	      bl.set_ilast(ilast);
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bls.push_back(bl);
	    }
	  }
	  return bls;
	}
	
	// gap - A - B - c - ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: gap - A - B - c ... with c singular : optimize B " <<  std::endl;
	}
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::experimental_point pc = nodes_[ilast+1].ccc()[0].joints()[0].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pBbest;
	  for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	    topology::broken_line bl;
	    topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	    bl.eps_.push_back(pA);
	    bl.eps_.push_back(pB);
	    bl.eps_.push_back(pc);
	    bl.calculate_chi2();
	    if( bl.p() > maxprob ){
	      maxprob = bl.p();
	      pBbest = pB;
	    }
	  }
	  best_bl.eps_.push_back(pA);
	  best_bl.eps_.push_back(pBbest);
	  bls.push_back(best_bl);
	}
	return bls;
      }
      
      
      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is second node:   0 - A - B
      if( first_ambiguous_is_second ){
	
	if( last_ambiguous_is_begore_gap ){ // 0 - A - B - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | 0 - A - B - gap (should be decided by matching) " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      bl.set_ifirst(ifirst-1);
	      bl.set_ilast(ilast);
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	      bl.eps_.push_back(p0);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bls.push_back(bl);
	    }
	  }
	  return bls;
	}
	
	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last: 0 - A - B - N
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: | 0 - A - B - N | (should be decided by matching) " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      bl.set_ifirst(ifirst-1);
	      bl.set_ilast(ilast+1);
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	      topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_B].epc();
	      bl.eps_.push_back(p0);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pN);
	      bls.push_back(bl);
	    }
	  }
	  return bls;
	}
	
	// 0 - A - B - c - ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: 0 - A - B - c optimize B " <<  std::endl;
	}
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst-1);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pBbest;
	  for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	    topology::broken_line bl;
	    topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	    topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_B].epc();
	    bl.eps_.push_back(p0);
	    bl.eps_.push_back(pA);
	    bl.eps_.push_back(pB);
	    bl.eps_.push_back(pN);
	    bl.calculate_chi2();
	    if( bl.p() > maxprob ){
	      maxprob = bl.p();
	      pBbest = pB;
	    }
	  }
	  best_bl.eps_.push_back(p0);
	  best_bl.eps_.push_back(pA);
	  best_bl.eps_.push_back(pBbest);
	  bls.push_back(best_bl);
	}
	return bls;
      }      
      


      if( last_ambiguous_is_last_but_one ){ // a - A - B - N
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: cannot solve ambiguity: ... a - A - B - N | optimize A " <<  std::endl;
	}
	topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	  topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	  topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_B].epc();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast+1);
	  topology::experimental_point pAbest;
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    topology::broken_line bl;
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	    bl.eps_.push_back(pa);
	    bl.eps_.push_back(pA);
	    bl.eps_.push_back(pB);
	    bl.eps_.push_back(pN);
	    bl.calculate_chi2();
	    if( bl.p() > maxprob ){
	      maxprob = bl.p();
	      pAbest = pA;
	    }
	  }
	  best_bl.eps_.push_back(pAbest);
	  best_bl.eps_.push_back(pB);
	  best_bl.eps_.push_back(pN);
	  bls.push_back(best_bl);
	}
	return bls;
      }

      if( last_ambiguous_is_begore_gap ){ // a - A - B - gap
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: ... a - A - B - gap ; optimize A " <<  std::endl;
	}
	topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	  topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pAbest;
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    topology::broken_line bl;
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	    bl.eps_.push_back(pa);
	    bl.eps_.push_back(pA);
	    bl.eps_.push_back(pB);
	    bl.calculate_chi2();
	    if( bl.p() > maxprob ){
	      maxprob = bl.p();
	      pAbest = pA;
	    }
	  }
	  best_bl.eps_.push_back(pAbest);
	  best_bl.eps_.push_back(pB);
	  bls.push_back(best_bl);
	}
	return bls;
      }



      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends:  ... a - A - B - b ... ; optimize A, B " <<  std::endl;
      }
      topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
      topology::experimental_point pb = nodes_[ilast+1].ccc()[0].joints()[0].epb();
      topology::broken_line best_bl;
      best_bl.set_ifirst(ifirst);
      best_bl.set_ilast(ilast);
      topology::experimental_point pAbest;
      topology::experimental_point pBbest;
      for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	  topology::broken_line bl;
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
	  bl.eps_.push_back(pa);
	  bl.eps_.push_back(pA);
	  bl.eps_.push_back(pB);
	  bl.eps_.push_back(pb);
	  bl.calculate_chi2();
	  if( bl.p() > maxprob ){
	    maxprob = bl.p();
	    pAbest = pA;
	    pBbest = pB;
	  }
	}
      }
      best_bl.eps_.push_back(pAbest);
      best_bl.eps_.push_back(pBbest);
      bls.push_back(best_bl);
      return bls;


    }


    std::vector<topology::broken_line> cluster::solve_ambiguities_with_ends__3_nodes(size_t ifirst, size_t ilast, bool first_ambiguous_is_after_gap, bool first_ambiguous_is_second, bool last_ambiguous_is_begore_gap, bool last_ambiguous_is_last_but_one){

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;


      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is right after a gap:   gap - A - B - C
      if( first_ambiguous_is_after_gap ){
	  
	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last:  gap - A - B - C - N
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: configuration: gap - A - B - C - N |  optimize B " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_C].epc();
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst);
	      best_bl.set_ilast(ilast+1);
	      topology::experimental_point pBbest;
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pN);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest= pB;
		}
	      }
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      best_bl.eps_.push_back(pN);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	}


	if( last_ambiguous_is_begore_gap ){ // gap - A - B - C - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: gap - A - B - C - gap : optimize B " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pBbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst);
	      best_bl.set_ilast(ilast);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ilast].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest = pB;
		}
	      }
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	}
	  

	// gap - A - B - C - d ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: gap - A - B - C - d ... with d singular; optimize B and C " <<  std::endl;
	}
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::experimental_point pd = nodes_[ilast+1].ccc()[0].joints()[0].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pBbest;
	  topology::experimental_point pCbest;
	  for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::broken_line bl;
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.eps_.push_back(pd);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pBbest = pB;
		pCbest = pC;
	      }
	    }
	  }
	  best_bl.eps_.push_back(pA);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pCbest);
	  bls.push_back(best_bl);
	}
	return bls;
      }
      
      
      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is second node:   0 - A - B - C
      if( first_ambiguous_is_second ){

	if( last_ambiguous_is_begore_gap ){ // 0 - A - B - C - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: | 0 - A - B - C - gap : optimize B " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pBbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst-1);
	      best_bl.set_ilast(ilast);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(p0);
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest = pB;
		}
	      }
	      best_bl.eps_.push_back(p0);
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	}
	

	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last: 0 - A - B - C - N
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends:  | 0 - A - B - C - N | (optimize B )" <<  std::endl;
	  }

	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_C].epc();
	      topology::experimental_point pBbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst-1);
	      best_bl.set_ilast(ilast+1);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(p0);
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pN);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest = pB;
		}
	      }
	      best_bl.eps_.push_back(p0);
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      best_bl.eps_.push_back(pN);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;

	}

	
	// 0 - A - B - C - d - ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: 0 - A - B - C - d ... with d singular; optimize B, C " <<  std::endl;
	}

	topology::experimental_point pd = nodes_[ilast+1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst-1);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pBbest;
	  topology::experimental_point pCbest;
	  for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::broken_line bl;
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	      bl.eps_.push_back(p0);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.eps_.push_back(pd);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pBbest = pB;
		pCbest = pC;
	      }
	    }
	  }
	  best_bl.eps_.push_back(p0);
	  best_bl.eps_.push_back(pA);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pCbest);
	  bls.push_back(best_bl);
	}
	return bls;
      }


      
      // ... a - A - B - C
      if( last_ambiguous_is_begore_gap ){ // .. - a - A - B - C - gap

	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: ... - a - A - B - C - gap; optimize A, B " <<  std::endl;
	}

	topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	  topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	  topology::experimental_point pAbest;
	  topology::experimental_point pBbest;
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast);
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      bl.eps_.push_back(pa);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pAbest = pA;
		pBbest = pB;
	      }
	    }
	  }
	  best_bl.eps_.push_back(pAbest);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pC);
	  bls.push_back(best_bl);
	}
	return bls;
      }



      if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last: ... a - A - B - C - N |
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends:  ... a - A - B - C - N | (optimize A, B )" <<  std::endl;
	}

	topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	  topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	  topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_C].epc();
	  topology::experimental_point pAbest;
	  topology::experimental_point pBbest;
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast+1);
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      bl.eps_.push_back(pa);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.eps_.push_back(pN);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pAbest = pA;
		pBbest = pB;
	      }
	    }
	  }
	  best_bl.eps_.push_back(pAbest);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pC);
	  best_bl.eps_.push_back(pN);
	  bls.push_back(best_bl);
	}
	return bls;
	
      }

	
      // ... a - A - B - C - d - ...
      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends: ... a - A - B - C - d ... ; optimize A, B, C " <<  std::endl;
      }

      topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
      topology::experimental_point pd = nodes_[ilast+1].ccc()[0].joints()[0].epb();
      topology::experimental_point pAbest;
      topology::experimental_point pBbest;
      topology::experimental_point pCbest;
      topology::broken_line best_bl;
      best_bl.set_ifirst(ifirst);
      best_bl.set_ilast(ilast);
      for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	  for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	    topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	    topology::experimental_point pC = nodes_[ilast].ccc()[0].joints()[joint_index_C].epb();
	    topology::broken_line bl;
	    bl.eps_.push_back(pa);
	    bl.eps_.push_back(pA);
	    bl.eps_.push_back(pB);
	    bl.eps_.push_back(pC);
	    bl.eps_.push_back(pd);
	    bl.calculate_chi2();
	    if( bl.p() > maxprob ){
	      maxprob = bl.p();
	      pAbest = pA;
	      pBbest = pB;
	      pCbest = pC;
	    }
	  }
	}
      }
      best_bl.eps_.push_back(pAbest);
      best_bl.eps_.push_back(pBbest);
      best_bl.eps_.push_back(pCbest);
      bls.push_back(best_bl);
      return bls;


    }


    std::vector<topology::broken_line> cluster::solve_ambiguities_with_ends__4_nodes(size_t ifirst, size_t ilast, bool first_ambiguous_is_after_gap, bool first_ambiguous_is_second, bool last_ambiguous_is_begore_gap, bool last_ambiguous_is_last_but_one){

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;


      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is right after a gap:   gap - A - B - C - D
      if( first_ambiguous_is_after_gap ){
	  
	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last:  gap - A - B - C - D - N
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: configuration: gap - A - B - C - D - N |  optimize B, C " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_D].epc();
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst);
	      best_bl.set_ilast(ilast+1);
	      topology::experimental_point pBbest;
	      topology::experimental_point pCbest;
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		  topology::broken_line bl;
		  topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		  topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		  bl.eps_.push_back(pA);
		  bl.eps_.push_back(pB);
		  bl.eps_.push_back(pC);
		  bl.eps_.push_back(pD);
		  bl.eps_.push_back(pN);
		  bl.calculate_chi2();
		  if( bl.p() > maxprob ){
		    maxprob = bl.p();
		    pBbest= pB;
		    pCbest= pC;
		  }
		}
	      }
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pCbest);
	      best_bl.eps_.push_back(pD);
	      best_bl.eps_.push_back(pN);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	}


	if( last_ambiguous_is_begore_gap ){ // gap - A - B - C - D - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: gap - A - B - C - gap : optimize B, C " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::experimental_point pBbest;
	      topology::experimental_point pCbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst);
	      best_bl.set_ilast(ilast);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		  topology::broken_line bl;
		  topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		  topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		  bl.eps_.push_back(pA);
		  bl.eps_.push_back(pB);
		  bl.eps_.push_back(pC);
		  bl.eps_.push_back(pD);
		  bl.calculate_chi2();
		  if( bl.p() > maxprob ){
		    maxprob = bl.p();
		    pBbest = pB;
		    pCbest = pC;
		  }
		}
	      }
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pCbest);
	      best_bl.eps_.push_back(pD);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	}
	  

	// gap - A - B - C - D - e ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: gap - A - B - C - D - e ... with d singular; optimize B, C, D " <<  std::endl;
	}
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::experimental_point pe = nodes_[ilast+1].ccc()[0].joints()[0].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pBbest;
	  topology::experimental_point pCbest;
	  topology::experimental_point pDbest;
	  for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pD);
		bl.eps_.push_back(pe);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest = pB;
		  pCbest = pC;
		  pDbest = pD;
		}
	      }
	    }
	  }
	  best_bl.eps_.push_back(pA);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pCbest);
	  best_bl.eps_.push_back(pDbest);
	  bls.push_back(best_bl);
	}
	return bls;
      }
      
      
      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is second node:   0 - A - B - C - D
      if( first_ambiguous_is_second ){

	if( last_ambiguous_is_begore_gap ){ // 0 - A - B - C - D - gap
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends: | 0 - A - B - C - D - gap : optimize B, C " <<  std::endl;
	  }
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::experimental_point pBbest;
	      topology::experimental_point pCbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst-1);
	      best_bl.set_ilast(ilast);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		  topology::broken_line bl;
		  topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		  topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		  bl.eps_.push_back(p0);
		  bl.eps_.push_back(pA);
		  bl.eps_.push_back(pB);
		  bl.eps_.push_back(pC);
		  bl.eps_.push_back(pD);
		  bl.calculate_chi2();
		  if( bl.p() > maxprob ){
		    maxprob = bl.p();
		    pBbest = pB;
		    pCbest = pC;
		  }
		}
	      }
	      best_bl.eps_.push_back(p0);
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pCbest);
	      best_bl.eps_.push_back(pD);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	}
	

	if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last: 0 - A - B - C - D - N
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << " CAT::cluster::solve_ambiguities_with_ends:  | 0 - A - B - C - D - N | (optimize B, C)" <<  std::endl;
	  }

	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_D].epc();
	      topology::experimental_point pBbest;
	      topology::experimental_point pCbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst-1);
	      best_bl.set_ilast(ilast+1);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		  topology::broken_line bl;
		  topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		  topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		  bl.eps_.push_back(p0);
		  bl.eps_.push_back(pA);
		  bl.eps_.push_back(pB);
		  bl.eps_.push_back(pC);
		  bl.eps_.push_back(pD);
		  bl.eps_.push_back(pN);
		  bl.calculate_chi2();
		  if( bl.p() > maxprob ){
		    maxprob = bl.p();
		    pBbest = pB;
		    pCbest = pC;
		  }
		}
	      }
	      best_bl.eps_.push_back(p0);
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pCbest);
	      best_bl.eps_.push_back(pD);
	      best_bl.eps_.push_back(pN);
	      bls.push_back(best_bl);
	    }
	  }
	  return bls;
	  
	}

	
	// 0 - A - B - C - D - e - ...
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: 0 - A - B - C - D - e ... with d singular; optimize B, C, D " <<  std::endl;
	}

	topology::experimental_point pe = nodes_[ilast+1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	  topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst-1);
	  best_bl.set_ilast(ilast);
	  topology::experimental_point pBbest;
	  topology::experimental_point pCbest;
	  topology::experimental_point pDbest;
	  for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::broken_line bl;
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      bl.eps_.push_back(p0);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.eps_.push_back(pD);
	      bl.eps_.push_back(pe);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pBbest = pB;
		pCbest = pC;
		pDbest = pD;
	      }
	      }
	    }
	  }
	  best_bl.eps_.push_back(p0);
	  best_bl.eps_.push_back(pA);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pCbest);
	  best_bl.eps_.push_back(pDbest);
	  bls.push_back(best_bl);
	}
	return bls;
      }


      
      // ... a - A - B - C - D
      if( last_ambiguous_is_begore_gap ){ // .. - a - A - B - C - D - gap

	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: ... - a - A - B - C - D - gap; optimize A, B, C " <<  std::endl;
	}

	topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	  topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	  topology::experimental_point pAbest;
	  topology::experimental_point pBbest;
	  topology::experimental_point pCbest;
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast);
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		topology::broken_line bl;
		topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		bl.eps_.push_back(pa);
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pD);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pAbest = pA;
		  pBbest = pB;
		  pCbest = pC;
		}
	      }
	    }
	  }
	  best_bl.eps_.push_back(pAbest);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pCbest);
	  best_bl.eps_.push_back(pD);
	  bls.push_back(best_bl);
	}
	return bls;
      }



      if( last_ambiguous_is_last_but_one ){ // node after last ambiguous is the last: ... a - A - B - C - D - N |
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends:  ... a - A - B - C - D - N | (optimize A, B, C)" <<  std::endl;
	}

	topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
	for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	  topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	  topology::experimental_point pN = nodes_[ilast].ccc()[0].joints()[joint_index_D].epc();
	  topology::experimental_point pAbest;
	  topology::experimental_point pBbest;
	  topology::experimental_point pCbest;
	  topology::broken_line best_bl;
	  best_bl.set_ifirst(ifirst);
	  best_bl.set_ilast(ilast+1);
	  for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		topology::broken_line bl;
		topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
		bl.eps_.push_back(pa);
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pD);
		bl.eps_.push_back(pN);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pAbest = pA;
		  pBbest = pB;
		  pCbest = pC;
		}
	      }
	    }
	  }
	  best_bl.eps_.push_back(pAbest);
	  best_bl.eps_.push_back(pBbest);
	  best_bl.eps_.push_back(pCbest);
	  best_bl.eps_.push_back(pD);
	  best_bl.eps_.push_back(pN);
	  bls.push_back(best_bl);
	}
	return bls;
	
      }

	
      // ... a - A - B - C - D - e - ...
      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends: ... a - A - B - C - D - e ... ; optimize A, B, C, D " <<  std::endl;
      }

      topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
      topology::experimental_point pe = nodes_[ilast+1].ccc()[0].joints()[0].epb();
      topology::experimental_point pAbest;
      topology::experimental_point pBbest;
      topology::experimental_point pCbest;
      topology::experimental_point pDbest;
      topology::broken_line best_bl;
      best_bl.set_ifirst(ifirst);
      best_bl.set_ilast(ilast);
      for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	  for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::broken_line bl;
	      bl.eps_.push_back(pa);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.eps_.push_back(pD);
	      bl.eps_.push_back(pe);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pAbest = pA;
		pBbest = pB;
		pCbest = pC;
		pDbest = pD;
	      }
	    }
	  }
	}
      }
      best_bl.eps_.push_back(pAbest);
      best_bl.eps_.push_back(pBbest);
      best_bl.eps_.push_back(pCbest);
      best_bl.eps_.push_back(pDbest);
      bls.push_back(best_bl);
      return bls;


    }


    void cluster::solve_ambiguities_with_ends__more_than_4_nodes(topology::broken_line ACD[2][2][2], size_t ifirst, size_t ilast, bool first_ambiguous_is_after_gap, bool first_ambiguous_is_second){

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;

      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is right after a gap:   gap - A - B - C - D
      if( first_ambiguous_is_after_gap ){
	  
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends__more_than_4_nodes: configuration: gap - A - B - C - D |  optimize B " <<  std::endl;
	}
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst);
	      best_bl.set_ilast(ilast);
	      topology::experimental_point pBbest;
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pD);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest= pB;
		}
	      }
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      best_bl.eps_.push_back(pD);
	      ACD[joint_index_A][joint_index_C][joint_index_D] = best_bl;
	    }
	  }
	}
	return;
      }

      ////////////////////////////////////////////////////////////
      // ... if first ambiguous node is second node:   0 - A - B - C - D
      else if( first_ambiguous_is_second ){

	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends__more_than_4_nodes: | 0 - A - B - C - D : optimize B " <<  std::endl;
	}
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point p0 = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epa();
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::experimental_point pBbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst-1);
	      best_bl.set_ilast(ilast);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(p0);
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pD);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest = pB;
		}
	      }
	      best_bl.eps_.push_back(p0);
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      best_bl.eps_.push_back(pD);
	      ACD[joint_index_A][joint_index_C][joint_index_D] = best_bl;
	    }
	  }
	}
	return;
      }
      

      // ... a - A - B - C - D
      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends__more_than_4_nodes: ... - a - A - B - C - D ; optimize B " <<  std::endl;
      }
      
      topology::experimental_point pa = nodes_[ifirst-1].ccc()[0].joints()[0].epb();
      for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	  for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	    topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	    topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
	    topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	    topology::experimental_point pBbest;
	    topology::broken_line best_bl;
	    best_bl.set_ifirst(ifirst);
	    best_bl.set_ilast(ilast);
	    for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
	      topology::broken_line bl;
	      topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
	      bl.eps_.push_back(pa);
	      bl.eps_.push_back(pA);
	      bl.eps_.push_back(pB);
	      bl.eps_.push_back(pC);
	      bl.eps_.push_back(pD);
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		pBbest = pB;
	      }
	    }
	    best_bl.eps_.push_back(pA);
	    best_bl.eps_.push_back(pBbest);
	    best_bl.eps_.push_back(pC);
	    best_bl.eps_.push_back(pD);
	    ACD[joint_index_A][joint_index_C][joint_index_D] = best_bl;
	  }
	}
      }

      return;

    }


    void cluster::solve_ambiguities_with_ends__more_than_4_nodes(topology::broken_line aACD[2][2][2][2], size_t ifirst, size_t ilast){

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;

      // ... AN - A - B - C - D
      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends__more_than_4_nodes: ... - AN - A - B - C - D ; optimize B " <<  std::endl;
      }
      
      for( size_t joint_index_AN = 0; joint_index_AN <= 1; ++joint_index_AN){
	topology::experimental_point ante = nodes_[ifirst-1].ccc()[0].joints()[joint_index_AN].epb();

	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	    for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	      topology::experimental_point pA = nodes_[ifirst].ccc()[0].joints()[joint_index_A].epb();
	      topology::experimental_point pC = nodes_[ilast-1].ccc()[0].joints()[joint_index_C].epb();
	      topology::experimental_point pD = nodes_[ilast].ccc()[0].joints()[joint_index_D].epb();
	      topology::experimental_point pBbest;
	      topology::broken_line best_bl;
	      best_bl.set_ifirst(ifirst);
	      best_bl.set_ilast(ilast);
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl;
		topology::experimental_point pB = nodes_[ifirst+1].ccc()[0].joints()[joint_index_B].epb();
		bl.eps_.push_back(ante);
		bl.eps_.push_back(pA);
		bl.eps_.push_back(pB);
		bl.eps_.push_back(pC);
		bl.eps_.push_back(pD);
		bl.calculate_chi2();
		if( bl.p() > maxprob ){
		  maxprob = bl.p();
		  pBbest = pB;
		}
	      }
	      best_bl.eps_.push_back(pA);
	      best_bl.eps_.push_back(pBbest);
	      best_bl.eps_.push_back(pC);
	      best_bl.eps_.push_back(pD);
	      aACD[joint_index_AN][joint_index_A][joint_index_C][joint_index_D] = best_bl;
	    }
	  }
	}
      }

      return;

    }

    void cluster::merge__more_than_4_nodes(topology::broken_line ACD[2][2][2], topology::broken_line aACD[2][2][2][2]){

      topology::broken_line old_ACD[2][2][2];
      for( size_t a = 0; a <= 1; ++a)
	for( size_t b = 0; b <= 1; ++b)
	  for( size_t c = 0; c <= 1; ++c)
	    old_ACD[a][b][c] = ACD[a][b][c];
      
      for( size_t joint_index_AN = 0; joint_index_AN <= 1; ++joint_index_AN){
	for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	  for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	    
	    topology::broken_line best_bl, best_bl1, best_bl2;
	    size_t bestA, bestB;
	    double min_chi2 = mybhep::default_min;
	    
	    for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	      for( size_t joint_index_B = 0; joint_index_B <= 1; ++joint_index_B){
		topology::broken_line bl1 = old_ACD[joint_index_AN][joint_index_A][joint_index_B];
		topology::broken_line bl2 = aACD[joint_index_A][joint_index_B][joint_index_C][joint_index_D];
		bl1.calculate_chi2();
		bl2.calculate_chi2();
		double total_chi = bl1.chi2() + bl2.chi2();
		if( total_chi < min_chi2 ){
		  min_chi2 = total_chi;
		  bestA = joint_index_A;
		  bestB = joint_index_B;
		}
	      }
	    }
	    
	    best_bl1 = old_ACD[joint_index_AN][bestA][bestB];
	    best_bl2 = aACD[bestA][bestB][joint_index_C][joint_index_D];
	    best_bl = best_bl1;
	    best_bl.set_ilast(best_bl2.ilast());
	    best_bl.eps_.push_back(best_bl2.eps_[1]);
	    best_bl.eps_.push_back(best_bl2.eps_[2]);
	    best_bl.eps_.push_back(best_bl2.eps_[3]);
	    ACD[joint_index_AN][joint_index_C][joint_index_D] = best_bl;
	  }
	}
      }

      return;

    }

    std::vector<topology::broken_line> cluster::finish__more_than_4_nodes(topology::broken_line ACD[2][2][2], size_t ifirst, size_t ipivot, size_t n_residuals){

      size_t ilast = ipivot + n_residuals - 1;
      
      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::finish__more_than_4_nodes: ifirst " << ifirst << " ipivot " << ipivot << " ilast " << ilast << " n_residuals " << n_residuals << std::endl;
      }

      std::vector<topology::broken_line> bls;

      double maxprob = mybhep::default_max;
      double min_chi2 = mybhep::default_min;

      if( n_residuals == 0 ){
	for( size_t joint_index_A = 0; joint_index_A <= 1; ++joint_index_A){
	  for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	    topology::broken_line best_bl;
	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      topology::broken_line bl = ACD[joint_index_A][joint_index_C][joint_index_D];
	      bl.calculate_chi2();
	      if( bl.p() > maxprob ){
		maxprob = bl.p();
		best_bl = bl;
	      }
	    }
	    bls.push_back(best_bl);
	  }
	}
	return bls;
      }


      if( n_residuals == 1 ){
	for( size_t joint_index_AN = 0; joint_index_AN <= 1; ++joint_index_AN){
	  for( size_t joint_index_Z = 0; joint_index_Z <= 1; ++joint_index_Z){

	    topology::experimental_point pZ = nodes_[ilast].ccc()[0].joints()[joint_index_Z].epb();
	    topology::broken_line best_bl;

	    for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
	      for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
	    
		topology::broken_line bl1 = ACD[joint_index_AN][joint_index_C][joint_index_D];
		bl1.eps_.push_back(pZ);
		bl1.calculate_chi2();
		double total_chi = bl1.chi2();
		if( total_chi < min_chi2 ){
		  min_chi2 = total_chi;
		  best_bl = bl1;
		}
	      }
	    }
	    best_bl.set_ilast(ilast);
	    bls.push_back(best_bl);
	  }
	}
	return bls;
      }


      if( n_residuals == 2 ){
	for( size_t joint_index_AN = 0; joint_index_AN <= 1; ++joint_index_AN){
	  for( size_t joint_index_Z = 0; joint_index_Z <= 1; ++joint_index_Z){
	    topology::experimental_point pZ = nodes_[ilast].ccc()[0].joints()[joint_index_Z].epb();
	    topology::broken_line best_bl;
	    
	    for( size_t joint_index_Y = 0; joint_index_Y <= 1; ++joint_index_Y){
	      topology::experimental_point pY = nodes_[ilast-1].ccc()[0].joints()[joint_index_Y].epb();
	      
	      for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
		  
		  topology::broken_line bl1 = ACD[joint_index_AN][joint_index_C][joint_index_D];
		  bl1.eps_.push_back(pY);
		  bl1.eps_.push_back(pZ);
		  bl1.calculate_chi2();
		  double total_chi = bl1.chi2();
		  if( total_chi < min_chi2 ){
		    min_chi2 = total_chi;
		    best_bl = bl1;
		  }
		}
	      }
	    }
	    best_bl.set_ilast(ilast);
	    bls.push_back(best_bl);
	  }
	}
	return bls;
      }


      if( n_residuals == 3 ){
	for( size_t joint_index_AN = 0; joint_index_AN <= 1; ++joint_index_AN){
	  for( size_t joint_index_Z = 0; joint_index_Z <= 1; ++joint_index_Z){
	    topology::experimental_point pZ = nodes_[ilast].ccc()[0].joints()[joint_index_Z].epb();
	    topology::broken_line best_bl;
	    
	    for( size_t joint_index_X = 0; joint_index_X <= 1; ++joint_index_X){
	      topology::experimental_point pX = nodes_[ilast-2].ccc()[0].joints()[joint_index_X].epb();
	      for( size_t joint_index_Y = 0; joint_index_Y <= 1; ++joint_index_Y){
		topology::experimental_point pY = nodes_[ilast-1].ccc()[0].joints()[joint_index_Y].epb();
	      
		for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		  for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
		    
		    topology::broken_line bl1 = ACD[joint_index_AN][joint_index_C][joint_index_D];
		    bl1.eps_.push_back(pX);
		    bl1.eps_.push_back(pY);
		    bl1.eps_.push_back(pZ);
		    bl1.calculate_chi2();
		    double total_chi = bl1.chi2();
		    if( total_chi < min_chi2 ){
		      min_chi2 = total_chi;
		      best_bl = bl1;
		    }
		  }
		}
	      }
	    }
	    best_bl.set_ilast(ilast);
	    bls.push_back(best_bl);
	  }
	}
	return bls;
      }


      for( size_t joint_index_AN = 0; joint_index_AN <= 1; ++joint_index_AN){
	for( size_t joint_index_Z = 0; joint_index_Z <= 1; ++joint_index_Z){
	  topology::experimental_point pZ = nodes_[ilast].ccc()[0].joints()[joint_index_Z].epb();
	  topology::broken_line best_bl;
	    
	  for( size_t joint_index_W = 0; joint_index_W <= 1; ++joint_index_W){
	    topology::experimental_point pW = nodes_[ilast-3].ccc()[0].joints()[joint_index_W].epb();
	    for( size_t joint_index_X = 0; joint_index_X <= 1; ++joint_index_X){
	      topology::experimental_point pX = nodes_[ilast-2].ccc()[0].joints()[joint_index_X].epb();
	      for( size_t joint_index_Y = 0; joint_index_Y <= 1; ++joint_index_Y){
		topology::experimental_point pY = nodes_[ilast-1].ccc()[0].joints()[joint_index_Y].epb();
		
		for( size_t joint_index_C = 0; joint_index_C <= 1; ++joint_index_C){
		  for( size_t joint_index_D = 0; joint_index_D <= 1; ++joint_index_D){
		    
		    topology::broken_line bl1 = ACD[joint_index_AN][joint_index_C][joint_index_D];
		    bl1.eps_.push_back(pW);
		    bl1.eps_.push_back(pX);
		    bl1.eps_.push_back(pY);
		    bl1.eps_.push_back(pZ);
		    bl1.calculate_chi2();
		    double total_chi = bl1.chi2();
		    if( total_chi < min_chi2 ){
		      min_chi2 = total_chi;
		      best_bl = bl1;
		    }
		  }
		}
	      }
	    }
	  }
	  best_bl.set_ilast(ilast);
	  bls.push_back(best_bl);
	}
      }
      return bls;


    }

    std::vector<topology::broken_line> cluster::solve_ambiguities_with_ends(size_t ifirst, size_t ilast){
      
      ////////////////////////////////////////////////////////////
      /// solve ambiguities
      ////////////////////////////////////////////////////////////

      std::vector<topology::broken_line> bls;

      
      if( ifirst < 1 || ifirst + 2 > nodes_.size() ){
	if( print_level() >= mybhep::NORMAL ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: problem: first ambiguous node " << ifirst << " size " << nodes_.size() << std::endl;
	}
	return bls;
      }

      if( ilast < 1 || ilast + 2 > nodes_.size() ){
	if( print_level() >= mybhep::NORMAL ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: problem: last ambiguous node " << ilast << " size " << nodes_.size() << std::endl;
	}
	return bls;
      }

      size_t n_ambiguous_nodes = ilast - ifirst + 1;
 
      //// first ambiguous node can be:
      ///  - right after gap
      ///  - 2nd node
      ///  - node with previous singular

      bool first_ambiguous_is_after_gap = (nodes_[ifirst - 1].c().block() != nodes_[ifirst].c().block());
      bool first_ambiguous_is_second = (ifirst == 1);


      //// last ambiguous node can be:
      ///  - right before gap
      ///  - last-but-one node
      ///  - node with next singular

      bool last_ambiguous_is_begore_gap = (nodes_[ilast].c().block() != nodes_[ilast+1].c().block());
      bool last_ambiguous_is_last_but_one = (ilast + 2 == nodes_.size());

      if( print_level() >= mybhep::VERBOSE ){
	std::clog << " CAT::cluster::solve_ambiguities_with_ends: first ambiguous node " << ifirst << " after_gap: " << first_ambiguous_is_after_gap << " is_second: " << first_ambiguous_is_second << " last ambiguous node " << ilast << " before_gap: " << last_ambiguous_is_begore_gap << " is_last_but_one: " << last_ambiguous_is_last_but_one << " n of ambiguous nodes " << n_ambiguous_nodes << std::endl;
      }



      ////////////////////////////////////////////////////////////
      // 1 ambiguous node
      ////////////////////////////////////////////////////////////
      if( n_ambiguous_nodes == 1 ){
	
	bls = solve_ambiguities_with_ends__1_node(ifirst, ilast, first_ambiguous_is_after_gap, first_ambiguous_is_second, last_ambiguous_is_begore_gap, last_ambiguous_is_last_but_one);
	// gap - A - N|        :  2 solutions (pA, pN)
	// gap - A - gap       :  2 solutions (pA)
	// gap - A - b - ...   :  2 solutions (pA)
	// 0 - A - gap         :  2 solutions (p0, pA)
	// 0 - A - b - ...     :  2 solutions (p0, pA)
	// 0 - A - N|          :  2 solutions (p0, pA, pN)
	// ... a - A - N|      :  2 solutions (pA, pN)
	// ... a - A - gap     :  2 solutions (pA)
	// ... a - A - b - ... :  1 solution (pAbest)

	return bls;

      }


      ////////////////////////////////////////////////////////////
      // 2 ambiguous nodes
      ////////////////////////////////////////////////////////////
      if( n_ambiguous_nodes == 2 ){


	bls = solve_ambiguities_with_ends__2_nodes(ifirst, ilast, first_ambiguous_is_after_gap, first_ambiguous_is_second, last_ambiguous_is_begore_gap, last_ambiguous_is_last_but_one);

	// gap - A - B - N|        :  4 solutions (pA, pB, pN)
	// gap - A - B- gap        :  4 solutions (pA, pB)
	// gap - A - B - c - ...   :  2 solutions (pA, pBbest)
	// 0 - A - B - gap         :  4 solutions (p0, pA, pB)
	// 0 - A - B - N|          :  4 solutions (p0, pA, pB, pN)
	// 0 - A - B - c - ...     :  2 solutions (p0, pA, pBbest)
	// ... a - A - B - N|      :  2 solutions (pAbest, pB, pN)
	// ... a - A - B - gap     :  2 solutions (pAbest, pB)
	// ... a - A - B - b - ... :  1 solution (pAbest, pBbest)

	return bls;

      }
      


      ////////////////////////////////////////////////////////////
      // 3 ambiguous nodes
      ////////////////////////////////////////////////////////////
      if( n_ambiguous_nodes == 3 ){

	bls = solve_ambiguities_with_ends__3_nodes(ifirst, ilast, first_ambiguous_is_after_gap, first_ambiguous_is_second, last_ambiguous_is_begore_gap, last_ambiguous_is_last_but_one);

	// gap - A - B - C - N|        :  4 solutions (pA, pBbest, pC, pN)
	// gap - A - B - C - gap       :  4 solutions (pA, pBbest, pC)
	// gap - A - B - C - d - ...   :  2 solutions (pA, pBbest, pCbest)
	// 0 - A - B - C - gap         :  4 solutions (p0, pA, pBbest, pC)
	// 0 - A - B - C - N|          :  4 solutions (p0, pA, pBbest, pC, pN)
	// 0 - A - B - C - d - ...     :  2 solutions (p0, pA, pBbest, pCbest)
	// ... a - A - B - C - gap     :  2 solutions (pAbest, pBbest, pC)
	// ... a - A - B - C - N|      :  2 solutions (pAbest, pBbest, pC, pN)
	// ... a - A - B - C - d - ... :  1 solution (pAbest, pBbest, pCbest)

	return bls;

      }

     
      ////////////////////////////////////////////////////////////
      // 4 ambiguous nodes
      ////////////////////////////////////////////////////////////
      if( n_ambiguous_nodes == 4 ){

	bls = solve_ambiguities_with_ends__4_nodes(ifirst, ilast, first_ambiguous_is_after_gap, first_ambiguous_is_second, last_ambiguous_is_begore_gap, last_ambiguous_is_last_but_one);

	// gap - A - B - C - D - N|        :  4 solutions (pA, pBbest, pCbest, pD, pN)
	// gap - A - B - C - D - gap       :  4 solutions (pA, pBbest, pCbest, pD)
	// gap - A - B - C - D - e - ...   :  2 solutions (pA, pBbest, pCbest, pDbest)
	// 0 - A - B - C - D - gap         :  4 solutions (p0, pA, pBbest, pCbest, pD)
	// 0 - A - B - C - D - N|          :  4 solutions (p0, pA, pBbest, pCbest, pD, pN)
	// 0 - A - B - C - D - e - ...     :  2 solutions (p0, pA, pBbest, pCbest, pDbest)
	// ... a - A - B - C - D - gap     :  2 solutions (pAbest, pBbest, pCbest, pD)
	// ... a - A - B - C - D - N|      :  2 solutions (pAbest, pBbest, pCbest, pD, pN)
	// ... a - A - B - C - D - e - ... :  1 solution (pAbest, pBbest, pCbest, pDbest)

	return bls;

      }

     
      ////////////////////////////////////////////////////////////
      // more than 4 ambiguous node
      ////////////////////////////////////////////////////////////

      size_t n_residuals = n_ambiguous_nodes;
      size_t new_ifirst = ifirst;
      size_t new_ilast = ilast;
      topology::broken_line ACD[2][2][2];
      topology::broken_line aACD[2][2][2][2];
      bool first = true;

      while( n_residuals > 4 ){
	new_ilast = new_ifirst + 3;

	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: new first ambiguous node " << new_ifirst << " after_gap: " << first_ambiguous_is_after_gap << " is_second: " << first_ambiguous_is_second << " new last ambiguous node " << new_ilast << " n of ambiguous nodes " << n_ambiguous_nodes << " n_residuals " << n_residuals << std::endl;
	}

	if( first ){
	  first = false;
	  solve_ambiguities_with_ends__more_than_4_nodes(ACD, new_ifirst, new_ilast, first_ambiguous_is_after_gap, first_ambiguous_is_second);
	  // gap - A - B - C - D         :  8 solutions (pA, pBbest, pC, pD)
	  // 0 - A - B - C - D           :  8 solutions (p0, pA, pBbest, pC, pD)
	  // ... a - A - B - C - D       :  8 solutions (pA, pBbest, pC, pD)

	}else{
	  solve_ambiguities_with_ends__more_than_4_nodes(aACD, new_ifirst, new_ilast);
	  // ... a - A - B - C - D       :  16 solutions (pA, pBbest, pC, pD)

	  merge__more_than_4_nodes(ACD, aACD);
	}

	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << " CAT::cluster::solve_ambiguities_with_ends: broken_line ACD[0][0][0] has " << ACD[0][0][0].eps().size() << " points " << std::endl;
	}

	new_ifirst = new_ilast;
	n_residuals -= 3;

      }

      bls = finish__more_than_4_nodes(ACD, ifirst, new_ifirst, n_residuals);
      // n_residuals = 0, 1, 2, 3, 4;   4 solutions (pA, pB, ..., pY, pZ)

      return bls;


    }


  }
}

// end of cluster.cpp
