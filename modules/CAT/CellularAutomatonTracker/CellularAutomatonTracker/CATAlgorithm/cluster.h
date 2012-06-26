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


    class cluster : public tracking_object{

      // a cluster is composed of a list of nodes

    protected:
      std::string appname_;

    public:

      // list of nodes
      std::vector<node> nodes_;

      // status of cluster
      bool free_;

      //!Default constructor
      cluster()
      {
        appname_= "cluster: ";
        nodes_.clear();
        free_ = false;
      }

      //!Default destructor
      virtual ~cluster(){};

      //! constructor from std::vector of nodes
      cluster(const std::vector<node> &nodes, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "cluster: ";
        nodes_ = nodes;
        free_ = false;
      }

      //! constructor from single node
      cluster(node &a_node, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "cluster: ";
        a_node.set_free(false);
        nodes_.clear();
        nodes_.push_back(a_node);
        free_ = true;
      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const {
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
        for(std::vector<node>::iterator inode = nodes_.end(); inode != nodes_.begin(); --inode){
          inverted_nodes.push_back(*inode);
        }
        inverted.set_nodes( inverted_nodes );
        return inverted;

      }

      topology::node node_of_cell(const topology::cell & c){

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


    };
  }
}

#endif
