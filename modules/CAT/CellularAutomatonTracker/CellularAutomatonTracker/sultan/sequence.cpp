/* -*- mode: c++ -*- */
#include <sultan/sequence.h>

#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>
#include <sultan/node.h>
#include <sultan/experimental_helix.h>

#include <sys/time.h> 

namespace SULTAN {
  namespace topology{

    //!Default constructor
    sequence::sequence()
    {
      appname_= "sequence: ";
      nodes_.clear();
      names_.clear();names_.push_back("default");
    }

    //!Default destructor
    sequence::~sequence()
    {
    }

    //! constructor from std::vector of nodes
    sequence::sequence(const std::vector<node> & nodes, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "sequence: ";
      nodes_ = nodes;
      names_.clear();
      names_.push_back("default");
    }

    /*** dump ***/
    void sequence::dump (std::ostream & a_out   ,
                         const std::string & a_title ,
                         const std::string & a_indent ,
                         bool a_inherit          ) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " ------------------- " << std::endl;
      a_out << indent << names_[0] << ". Number of nodes: " << nodes().size() << std::endl;
      for(std::vector<node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode)
        inode->dump(a_out, "",indent + "     ");

      return;
    }

    //! set nodes
    void sequence::set_nodes(const std::vector<node> &nodes){
      nodes_ = nodes;
    }

    //! set name
    void sequence::set_name(const std::string &name){
      names_.clear();
      names_.push_back(name);
    }

    //! set names
    void sequence::set_names(const std::vector<std::string> &names){
      names_.clear();
      names_=names;
    }

    //! add name
    void sequence::add_name(const std::string &name){
      names_.push_back(name);
    }

    //! set helix
    void sequence::set_helix(const  experimental_helix & h){
      helix_ = h;
    }


    //! get nodes
    const std::vector<node> & sequence::nodes()const{
      return nodes_;
    }

    //! get name
    const std::string & sequence::name()const{
      return names_[0];
    }

    //! get names
    const std::vector<std::string> & sequence::names()const{
      return names_;
    }

    //! get sequence
    const experimental_helix & sequence::get_helix()const{
      return helix_;
    }

    bool sequence::has_cell(const cell & c)const{
      if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())

        return true;

      return false;
    }


    sequence sequence::invert(){
      sequence inverted;
      inverted.set_print_level(print_level());
      inverted.set_probmin(probmin());
      inverted.set_helix(helix_);

      std::vector<node> inverted_nodes;
      for(size_t i = 0; i < nodes_.size(); i++){
        topology::node in = nodes_[nodes_.size() - 1 - i];
        inverted_nodes.push_back(in);
      }
      inverted.set_nodes( inverted_nodes );
      inverted.set_names( names() );

      return inverted;

    }

    topology::node sequence::node_of_cell(const topology::cell & c){

      std::vector<node>::iterator fnode = std::find(nodes_.begin(),
                                                    nodes_.end(),
                                                    c);

      if( fnode == nodes_.end()){
        if( print_level() >= mybhep::NORMAL )
          std::clog << " problem: requested cell " << c.id() << " has no node in sequence " << std::endl;;

        topology::node null;
        return null;
      }

      return *fnode;

    }


  }
}

// end
