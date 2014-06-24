/* -*- mode: c++ -*- */
#include <sultan/cluster.h>

namespace SULTAN{
  namespace topology{

    //!Default constructor
    cluster::cluster()
    {
      appname_= "cluster: ";
      nodes_.clear();
    }

    //!Default destructor
    cluster::~cluster(){};

    //! constructor from std::vector of nodes
    cluster::cluster(const std::vector<node> &nodes, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cluster: ";
      nodes_ = nodes;
    }


    /*** dump ***/
    void cluster::dump (std::ostream & a_out ,
                        const std::string & a_title  ,
                        const std::string & a_indent,
                        bool /*a_inherit*/          ) const {
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " ------------------- " << std::endl;
      a_out << indent << " number of nodes: " << nodes().size() << std::endl;
      for(std::vector<node>::const_iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode)
        inode->dump(a_out, "",indent + "     ");

      return;
    }

    //! set nodes
    void cluster::set_nodes(const std::vector<node> &nodes){
      nodes_ = nodes;
    }

    //! get nodes
    const std::vector<node> & cluster::nodes()const{
      return nodes_;
    }

    //! add nodes
    void cluster::add_nodes(const std::vector<node> &nodes){
      for(std::vector<topology::node>::const_iterator inode=nodes.begin(); inode!=nodes.end();++inode){
	bool already_there=false;
	for(std::vector<topology::node>::const_iterator jnode=nodes_.begin(); jnode!=nodes_.end();++jnode){
	  if( inode->c().id() == jnode->c().id() ){
	    already_there=true;
	    break;
	  }
	}

	if( !already_there ){
	  nodes_.push_back(*inode);
	}
      }
    }

    //! remove node
    void cluster::remove_node_with_id(size_t id){
      for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end();++inode){
	if( inode->c().id() == id ){
	  nodes_.erase(nodes_.begin() + (inode - nodes_.begin()));
	  return;
	}
      }
      return;
    }

    //! remove nodes
    void cluster::remove_nodes(const std::vector<node> &nodes){
      for(std::vector<topology::node>::const_iterator inode=nodes.begin(); inode!=nodes.end(); ++inode)
	remove_node_with_id(inode->c().id());
    }

    //! set helix
    void cluster::set_helix(const  experimental_helix & h){
      helix_ = h;
    }

    //! get sequence
    const experimental_helix & cluster::get_helix()const{
      return helix_;
    }

    bool cluster::has_cell(const cell & c)const{

      if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())
        return true;

      return false;
    }


    void cluster::circle_order(){

      std::sort(nodes_.begin(),nodes_.end(),topology::node::circle_order);

      return;
    }



    topology::node cluster::node_of_cell(const topology::cell & c){

      std::vector<node>::iterator fnode = std::find(nodes_.begin(),
                                                    nodes_.end(),
                                                    c);

      if( fnode == nodes_.end()){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::cluster::node_of_cell: problem: requested cell " << c.id() << " has no node in cluster. cluster nodes are: " << std::endl;

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

    bool cluster::is_good()const{
      return (this->nodes().size() >= 4);
    }

    bool cluster::is_contained_in(const cluster & s)const{

      for(std::vector<node>::const_iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode ){
        if( !s.has_cell(inode->c() ) )
          return false;
      }

      return true;
    }

    bool cluster::contains(const cluster & s)const{
      
      const std::vector<node> & nodes = s.nodes();
      for(std::vector<node>::const_iterator inode = nodes.begin(); inode != nodes.end(); ++inode ){
        if( !this->has_cell(inode->c() ) )
          return false;
      }

      return true;
    }


    topology::cluster cluster::get_cluster_with_first_last(size_t first, size_t last){

      // take the nodes A .... B of the cluster
      // isolate the portion F .. L:
      //  A ... (F ... L) ... B
      // return (F ... L) as a cluster

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " SULTAN::cluster::get_cluster_with_first_last: first " << first << " last " << last << std::endl; fflush(stdout);
      }      

      std::vector<topology::node> the_nodes;
      if( first < last ){ // A ... (F ... L) ... B
	the_nodes.insert(the_nodes.end(), nodes_.begin() + first, nodes_.begin() + last);
      }else if( first == last ){ // there is only one piece A ... )F( ... B
	the_nodes.insert(the_nodes.end(), nodes_.begin() + first, nodes_.end());
	the_nodes.insert(the_nodes.end(), nodes_.begin(), nodes_.begin() + first); 
     }else{ // A ... L) ... (F ... B
	the_nodes.insert(the_nodes.end(), nodes_.begin() + first, nodes_.end());
	the_nodes.insert(the_nodes.end(), nodes_.begin(), nodes_.begin() + last);
      }
      
      topology::cluster c;
      c.set_nodes(the_nodes);

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " SULTAN::cluster::get_cluster_with_first_last: picked " << the_nodes.size() << " nodes " << std::endl;
      }      

      return c;
    }

    topology::cluster cluster::remove_cluster_with_first_last(size_t first, size_t last){
      // take the nodes A .... B of the cluster
      // isolate the portion F .. L:
      //  A ... (F ... L) ... B
      // remove (F ... L) from A ... B and return it as a cluster

      topology::cluster c = get_cluster_with_first_last(first,last);
      remove_nodes(c.nodes());
      
      return c;
      
    }

    
    size_t cluster::get_next_index(size_t index){
      // get index of following cell

      if( index + 1 > nodes_.size() ){
	std::clog << "SULTAN::cluster::next_index: problem: index " << index << " size " << nodes_.size() << std::endl;
	return 0;
      }

      size_t next_index = index + 1;
      if( next_index == nodes_.size() ) next_index = 0;

      return next_index;


    }



    topology::cluster cluster::longest_piece(){

      topology::cluster c;

      if( !length_of_piece_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::cluster::longest_piece: problem: requested logenst piece whene pieces size is " << length_of_piece_.size() << std::endl;
	}
	return c;
      }

      if( first_cell_of_piece_.size() != length_of_piece_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::cluster::longest_piece: problem: requested logenst piece whene pieces size is " << length_of_piece_.size() << " and first cell of pieces size " << first_cell_of_piece_.size() << std::endl;
	}
	return c;
      }

      // get longest piece
      std::vector<size_t>::const_iterator ml = max_element(length_of_piece_.begin(), length_of_piece_.end());

      // get indexes of first and last cell of longest piece
      size_t longest_piece_first = first_cell_of_piece_[ml - length_of_piece_.begin()];
      size_t longest_piece_last;
      if( ml - length_of_piece_.begin() + 1 < (int) length_of_piece_.size() )
	longest_piece_last = first_cell_of_piece_[ml - length_of_piece_.begin()+1];
      else
	longest_piece_last = first_cell_of_piece_.front();

      c = get_cluster_with_first_last(longest_piece_first, longest_piece_last);
	
      if( print_level() >= mybhep::VERBOSE ){
	std::clog << "SULTAN::cluster::longest_piece:  longest piece from "<< nodes_[longest_piece_first].c().id() << " to "<< nodes_[longest_piece_last].c().id() << " with length "<< *ml << " (" ;
	for(std::vector<topology::node>::const_iterator inode=c.nodes_.begin(); inode!=c.nodes_.end(); ++inode)
	  std::clog << " " << inode->c().id();
	std::clog << " )" << std::endl;
      }

      return c;
    }


    void cluster::break_into_continous_pieces(size_t nofflayers, double cell_distance){
      // if cluster = (a)
      //    first_cell=(0)
      //    length=1
      // else cluster = (a ... (a1 ... b1) (a2 ... b2) (a3 ... b3) ...  b )
      //    first_cell=(a1 a2 a3 ...)
      //    length=(b1 - a1  b2 - a2  ...)
      // if cluster = (a ... b) is all near
      //    first_cell=(a)
      //    length=(b - a)


      first_cell_of_piece_.clear();
      length_of_piece_.clear();

      if( nodes_.size() < 1 ){
	return;
      }

      // order node based on their angle wrt helix center
      circle_order();

      if (print_level() >= mybhep::VERBOSE){
	std::clog << "SULTAN::cluster::break_into_continous_pieces: for given " << nodes_.size() << " hits: (";
	for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode)
	  std::clog << inode->c().id() << " ";
	std::clog << ") " << std::endl;
      }
      
      //////////////////////////////////////
      //  get first cells of each piece  ///
      //////////////////////////////////////
      if( nodes_.size() == 1 ){
	first_cell_of_piece_.push_back(0);
      }
      else{ // loop over cells
	size_t next_index;
	for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	  
	  // get index of following cell
	  next_index = get_next_index(inode - nodes_.begin());
	  
	  if( inode->c().near_level(nodes_.at(next_index).c(), nofflayers, cell_distance ) == 0 ){
	    if( print_level() >= mybhep::VVERBOSE ){
	      std::clog << "SULTAN::cluster::break_into_continous_pieces:  cells " << inode->c().id() << " and " << nodes_.at(next_index).c().id() << " are far " << std::endl;
	    }
	    first_cell_of_piece_.push_back(next_index);
	  }
	  
	}
      }
      
      if( !first_cell_of_piece_.size() ){ // there are >1 nodes but no breaks
	first_cell_of_piece_.push_back(0);      
      }


      //////////////////////////////////////
      //    get length of each piece     ///
      //////////////////////////////////////
      int the_length;
      if( first_cell_of_piece_.size() <= 1 ){ // there is only one piece
	length_of_piece_.push_back(nodes_.size());
      }else{
	// loop on all 1st cells of pieces (excluded the 1st)
	for(size_t i=1; i<first_cell_of_piece_.size(); i++){

	  // length (= n of cells) of this piece
	  the_length = first_cell_of_piece_[i] - first_cell_of_piece_[i-1];
	  if( the_length < 0 ) the_length += nodes_.size();
	  if( print_level() >= mybhep::VERBOSE ){
	    std::clog << "SULTAN::cluster::break_into_continous_pieces:  piece " << i - 1 << " from " << first_cell_of_piece_[i-1] << " to " << first_cell_of_piece_[i] << " length " << the_length << std::endl;
	  }
	  length_of_piece_.push_back(the_length);
	}

	// add length of the 1st piece
	// front .... back
	the_length = first_cell_of_piece_.front() - first_cell_of_piece_.back();
	if( the_length < 0 ) the_length += nodes_.size();
	if( print_level() >= mybhep::VERBOSE ){
	  std::clog << "SULTAN::cluster::break_into_continous_pieces:  piece "<< first_cell_of_piece_.size()-1<< " from "<< first_cell_of_piece_.back()<< " to "<< first_cell_of_piece_.front()<< " length "<< the_length << std::endl;
	}
	length_of_piece_.push_back(the_length);
      }

      return;
    }


    void cluster::self_order(topology::node * n = 0){
      // if n = 0
      // calculate self circle of cluster
      // and order cells based on that
      // otherwise use n as endpoint


      if (print_level() >= mybhep::VVERBOSE){
	std::clog << "SULTAN::cluster::self_order cluster of " << nodes_.size() << " cells (";
	for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode)
	  std::clog << inode->c().id() << " ";
	std::clog << ") without endpoint?: " << (bool)(n == 0) << std::endl;
      }

      double angle;
      if( nodes_.size() <= 3 ){
	for(std::vector<topology::node>::iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	  angle = (double)(inode - nodes_.begin())/(double)nodes_.size();
	  inode->set_circle_phi(angle);
	}
	return;
      }
      

      if( n == 0 ){ // calculate self circle

	double x0 = 0.;
	double y0 = 0.;
	for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	  x0 += inode->c().ep().x().value();
	  y0 += inode->c().ep().y().value();
	}
	x0 /= nodes_.size();
	y0 /= nodes_.size();
	
	for(std::vector<topology::node>::iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	  angle = atan2(inode->c().ep().y().value() - y0, inode->c().ep().x().value() - x0);
	  inode->set_circle_phi(angle);
	}
      }else{ // use n as endpoint

	double dist2max = 0.;
	double dist2;
	for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	  dist2 = pow(inode->c().ep().x().value() - n->c().ep().x().value(),2) + pow(inode->c().ep().y().value() - n->c().ep().y().value(),2);
	  if( dist2 > dist2max ) dist2max = dist2;
	}
	
	for(std::vector<topology::node>::iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	  dist2 = pow(inode->c().ep().x().value() - n->c().ep().x().value(),2) + pow(inode->c().ep().y().value() - n->c().ep().y().value(),2);
	  angle = dist2/dist2max;
	  inode->set_circle_phi(angle);
	}

      }

      circle_order();

      if (print_level() >= mybhep::VVERBOSE){
	std::clog << "SULTAN::cluster::self_order: ordered cluster of " << nodes_.size() << " cells (";
	for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode)
	  std::clog << inode->c().id() << " [" << inode->circle_phi() << "] ";
	std::clog << ") " << std::endl;
      }


      return;
    }


    bool cluster::is_continous(){
      return (length_of_piece_.size() <= 1);

    }

    size_t cluster::max_length_of_piece(){
      
      if( !length_of_piece_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::cluster::max_length_of_piece: problem: requested logenst piece whene pieces size is " << length_of_piece_.size() << std::endl;
	}
	return 0;
      }
      
      if( first_cell_of_piece_.size() != length_of_piece_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "SULTAN::cluster::max_length_of_piece: problem: requested logenst piece whene pieces size is " << length_of_piece_.size() << " and first cell of pieces size " << first_cell_of_piece_.size() << std::endl;
	}
	return 0;
      }
      
      // get longest piece
      std::vector<size_t>::const_iterator ml = max_element(length_of_piece_.begin(), length_of_piece_.end());
      
      return *ml;
      
    }


    void cluster::recalculate(size_t n_iterations){

      for(size_t iter=0; iter<n_iterations; iter++){
	recalculate_R();
	recalculate_x0();
	recalculate_y0();
      }

    }

    void cluster::recalculate_x0(){

      double error = helix_.x0().error();
      std::vector<double> x0s, local_x0s;
      double local_x0, dy, delta;

      for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	dy = helix_.y0().value() - inode->c().ep().y().value();
	delta = pow(helix_.R().value() + inode->c().r().value(),2) - pow(dy,2);
	if( delta >= 0. ){
	  local_x0s.push_back( inode->c().ep().x().value() + sqrt(delta) );
	  local_x0s.push_back( inode->c().ep().x().value() - sqrt(delta) );
	}
	delta = pow(helix_.R().value() - inode->c().r().value(),2) - pow(dy,2);
	if( delta >= 0. ){
	  local_x0s.push_back( inode->c().ep().x().value() + sqrt(delta) );
	  local_x0s.push_back( inode->c().ep().x().value() - sqrt(delta) );
	}
	if( local_x0s.size() ){
	  local_x0 = local_x0s[0];
	  for(size_t j=1; j<local_x0s.size(); j++)
	    if( fabs(local_x0s[j] - helix_.x0().value() ) < fabs( local_x0 - helix_.x0().value() ) )
	      local_x0 = local_x0s[j];
	  x0s.push_back(local_x0);
	}
      }

      if( x0s.size() ){
	double average_x0 = average(x0s);
	helix_.set_x0(experimental_double(average_x0,error));
      }

    }


    void cluster::recalculate_y0(){

      double error = helix_.y0().error();
      std::vector<double> y0s, local_y0s;
      double local_y0, dx, delta;

      for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
	dx = helix_.x0().value() - inode->c().ep().x().value();
	delta = pow(helix_.R().value() + inode->c().r().value(),2) - pow(dx,2);
	if( delta >= 0. ){
	  local_y0s.push_back( inode->c().ep().x().value() + sqrt(delta) );
	  local_y0s.push_back( inode->c().ep().x().value() - sqrt(delta) );
	}
	delta = pow(helix_.R().value() - inode->c().r().value(),2) - pow(dx,2);
	if( delta >= 0. ){
	  local_y0s.push_back( inode->c().ep().x().value() + sqrt(delta) );
	  local_y0s.push_back( inode->c().ep().x().value() - sqrt(delta) );
	}
	if( local_y0s.size() ){
	  local_y0 = local_y0s[0];
	  for(size_t j=1; j<local_y0s.size(); j++)
	    if( fabs(local_y0s[j] - helix_.y0().value() ) < fabs( local_y0 - helix_.y0().value() ) )
	      local_y0 = local_y0s[j];
	  y0s.push_back(local_y0);
	}
      }

      if( y0s.size() ){
	double average_y0 = average(y0s);
	helix_.set_y0(experimental_double(average_y0,error));
      }

    }


    void cluster::recalculate_z0(){

    }

    void cluster::recalculate_R(){

      //double error = helix_.R().error();
      std::vector<topology::experimental_double> Rs;
      experimental_double local_R1, local_R2, local_R;

      for(std::vector<topology::node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode){
#if 1
	local_R1 = experimental_sqrt(experimental_square(helix_.x0() - inode->c().ep().x()) +
				     experimental_square(helix_.y0() - inode->c().ep().y()) 
				     ) + inode->c().r();
	local_R2 = experimental_fabs(experimental_sqrt(experimental_square(helix_.x0() - inode->c().ep().x()) +
						       experimental_square(helix_.y0() - inode->c().ep().y()) 
						       ) - inode->c().r());
	if( fabs(local_R1.value() - helix_.R().value() ) < fabs(local_R2.value() - helix_.R().value() ) )
	  local_R = local_R1;
	else
	  local_R = local_R2;
#else
	local_R = experimental_sqrt(experimental_square(helix_.x0() - inode->c().ep().x()) +
				    experimental_square(helix_.y0() - inode->c().ep().y()) );
#endif
	Rs.push_back(local_R);

      }

      experimental_double average_R = weighted_average(Rs);

      helix_.set_R(average_R);

    }

    void cluster::recalculate_H(){


    }


  }
}

// end of cluster.cpp
