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

    bool cluster::has_cell(const cell & c)const{

      if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())
        return true;

      return false;
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


  }
}

// end of cluster.cpp
