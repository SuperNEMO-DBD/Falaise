/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICLUSTER
#define __CATAlgorithm__ICLUSTER
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>


namespace CAT{
  namespace topology{

    using namespace std;
    using namespace mybhep;

    class cluster : public tracking_object{

      // a cluster is composed of a list of nodes

    protected:
      string appname_;

    public:   

      // list of nodes
      std::vector<node> nodes_;

      // status of cluster
      bool free_;

      //!Default constructor     
      cluster()
      {
        appname_= "cluster: ";
        free_ = false;
      }

      //!Default destructor
      virtual ~cluster(){};

      //! constructor from vector of nodes
      cluster(const std::vector<node> &nodes, prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "cluster: ";
        nodes_ = nodes;
        free_ = false;
      }

      //! constructor from single node
      cluster(node &a_node, prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "cluster: ";
        a_node.set_free(false);
        nodes_.push_back(a_node);
        free_ = true;
      }
  
      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false) const {
        string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << endl;
          }

        a_out << indent << appname_ << " ------------------- " << endl;
        a_out << indent << " number of nodes: " << nodes().size() << " free: " << Free() << endl;
        for(vector<node>::const_iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode)
          inode->dump(a_out, "",indent + "     ");
        a_out << indent << " ------------------- " << endl;
 
        return;
      }
    
      //! set nodes
      void set_nodes(const std::vector<node> &nodes){
        nodes_ = nodes;
      }

      //! set free level
      void set_free(bool free){
        free_ = free;
      }

      //! get nodes
      const std::vector<node> & nodes()const{
        return nodes_;
      }

      //! get free level
      const bool Free()const{
        return free_;
      }


    public:
    
      bool has_cell(const cell & c)const{

        if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())
          return true;

        return false;
      }


      cluster invert(){
        cluster inverted;
        inverted.set_print_level(print_level());
        inverted.set_nsigma(nsigma());
        inverted.set_free(Free());
        std::vector<node> inverted_nodes;
        for(vector<node>::iterator inode = nodes_.end(); inode != nodes_.begin(); --inode){
          inverted_nodes.push_back(*inode);
        }
        inverted.set_nodes( inverted_nodes );
        return inverted;

      }

      topology::node node_of_cell(const topology::cell & c){
      
        vector<node>::iterator fnode = std::find(nodes_.begin(),
                                                 nodes_.end(),
                                                 c);

        if( fnode == nodes_.end()){
          if( print_level() >= mybhep::NORMAL ){
            clog << " problem: requested cell " << c.id() << " has no node in cluster. cluster nodes are: " << endl;
          
            for(std::vector<node>::iterator in = nodes_.begin(); in != nodes_.end(); ++in){
              clog << " " << in->c().id();
            }
          
            clog << " " << endl;
          }

          topology::node null;
          return null;
        }

        return *fnode;

      }

  
    };
  }
}

#endif
