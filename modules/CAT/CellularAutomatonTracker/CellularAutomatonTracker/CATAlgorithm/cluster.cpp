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
                         bool a_inherit          ) const {
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
      const bool cluster::Free()const{
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
  }
}

// end of cluster.cpp
