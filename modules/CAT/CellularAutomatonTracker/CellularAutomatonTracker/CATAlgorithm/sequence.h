/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ISEQUENCE
#define __CATAlgorithm__ISEQUENCE
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <boost/cstdint.hpp>

#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/helix.h>
#include <CATAlgorithm/plane.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/LinearRegression.h>
#include <CATAlgorithm/CircleRegression.h>



namespace CAT {
  namespace topology{



    class sequence : public tracking_object{

      // a sequence is composed of a list of nodes

    protected:
      std::string appname_;

      // list of all chi2 values found in the construction
      // of the sequence (some of them discarded, kept only for plotting)
      std::vector<double> chi2s_;

      // list of helix helix_chi2 values
      std::vector<double> helix_chi2s_;

      // list of prob values
      std::vector<double> probs_;

      // fitted helix
      helix helix_;

      experimental_double charge_;

      experimental_double momentum_;

      experimental_double length_;

      bool primary_;

    public:

      // list of nodes
      std::vector<node> nodes_;

      // status of sequence
      bool free_;

      // sequence's name(s)
      std::vector<std::string> names_;

      // sequence's vertex: from helix and from tangents
      bool has_helix_vertex_;
      experimental_point helix_vertex_;
      std::string helix_vertex_type_;
      size_t helix_vertex_id_;
      bool has_tangent_vertex_;
      experimental_point tangent_vertex_;
      std::string tangent_vertex_type_;
      size_t tangent_vertex_id_;


      // sequence's decay_vertex
      bool has_decay_helix_vertex_;
      experimental_point decay_helix_vertex_;
      std::string decay_helix_vertex_type_;
      size_t calo_helix_id_;
      bool has_decay_tangent_vertex_;
      experimental_point decay_tangent_vertex_;
      std::string decay_tangent_vertex_type_;
      size_t calo_tangent_id_;

      //!Default constructor
      sequence()
      {
        appname_= "sequence: ";
        nodes_.clear();
        free_ = false;
        names_.clear();names_.push_back("default");
        has_helix_vertex_ = false;
        helix_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                     mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        helix_vertex_type_="default";
        helix_vertex_id_ = mybhep::default_integer;
        has_decay_helix_vertex_ = false;
        decay_helix_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                           mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        decay_helix_vertex_type_="default";
        calo_helix_id_ = mybhep::default_integer;

        has_tangent_vertex_ = false;
        tangent_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                     mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        tangent_vertex_type_="default";
        tangent_vertex_id_ = mybhep::default_integer;
        has_decay_tangent_vertex_ = false;
        decay_tangent_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                           mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        decay_tangent_vertex_type_="default";
        calo_tangent_id_ = mybhep::default_integer;

        chi2s_.clear();
        helix_chi2s_.clear();
        probs_.clear();
        helix_ = helix();
        charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        momentum_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        primary_ = true;
      }

      //!Default destructor
      virtual ~sequence(){};

      //! constructor from std::vector of nodes
      sequence(std::vector<node> nodes, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "sequence: ";
        nodes_ = nodes;
        free_ = false;
        names_.clear();names_.push_back("default");
        has_helix_vertex_ = false;
        helix_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                     mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        helix_vertex_type_="default";
        helix_vertex_id_ = mybhep::default_integer;
        has_decay_helix_vertex_ = false;
        decay_helix_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                           mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        decay_helix_vertex_type_="default";
        calo_helix_id_ = mybhep::default_integer;
        has_tangent_vertex_ = false;
        tangent_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                     mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        tangent_vertex_type_="default";
        tangent_vertex_id_ = mybhep::default_integer;
        has_decay_tangent_vertex_ = false;
        decay_tangent_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                           mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        decay_tangent_vertex_type_="default";
        calo_tangent_id_ = mybhep::default_integer;
        chi2s_.clear();
        helix_chi2s_.clear();
        probs_.clear();
        helix_ = helix();
        charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        momentum_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        primary_ = true;
      }

      //! constructor from single node
      sequence(node &node, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "sequence: ";
        node.set_free(false);
        nodes_.clear();
        nodes_.push_back(node);
        free_ = true;
        names_.clear();names_.push_back("default");
        has_helix_vertex_ = false;
        helix_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                     mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        helix_vertex_type_="default";
        helix_vertex_id_ = mybhep::default_integer;
        has_decay_helix_vertex_ = false;
        decay_helix_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                           mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        decay_helix_vertex_type_="default";
        calo_helix_id_ = mybhep::default_integer;
        has_tangent_vertex_ = false;
        tangent_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                     mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        tangent_vertex_type_="default";
        tangent_vertex_id_ = mybhep::default_integer;
        has_decay_tangent_vertex_ = false;
        decay_tangent_vertex_ = experimental_point(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                           mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        decay_tangent_vertex_type_="default";
        calo_tangent_id_ = mybhep::default_integer;
        chi2s_.clear();
        helix_chi2s_.clear();
        probs_.clear();
        helix_ = helix();
        charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        momentum_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
        primary_ = true;
      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const{
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " ------------------- " << std::endl;
        a_out << indent << names_[0] << ". Number of nodes: " << nodes().size() << " free: " << Free() << " chi2 " << chi2() << " helix chi2 " << helix_chi2() << " prob " << Prob() << " helix prob " << helix_Prob() << " momentum " << momentum().value() << " primary " << primary() << std::endl;
        for(std::vector<node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode)
          inode->dump(a_out, "",indent + "     ");
        if( has_helix_vertex() ){
          a_out << indent << " helix_vertex type: " << helix_vertex_type() << " helix_vertex: "; helix_vertex_.dump();
        }
        if( has_decay_helix_vertex() ){
          a_out << indent << " decay helix_vertex type " << decay_helix_vertex_type() << " calo id: " << calo_helix_id() << " decay_helix_vertex : "; decay_helix_vertex_.dump();
        }
        if( has_tangent_vertex() ){
          a_out << indent << " tangent_vertex type: " << tangent_vertex_type() << " tangent_vertex: "; tangent_vertex_.dump();
        }
        if( has_decay_tangent_vertex() ){
          a_out << indent << " decay tangent_vertex type " << decay_tangent_vertex_type() << " calo id: " << calo_tangent_id() << " decay_tangent_vertex : "; decay_tangent_vertex_.dump();
        }
        a_out << indent << " ------------------- " << std::endl;

        return;
      }

      //! set nodes
      void set_nodes(std::vector<node> nodes){
        nodes_ = nodes;
      }

      //! set free level
      void set_free(bool free){
        free_ = free;
      }

      //! set name
      void set_name(std::string name){
        names_.clear();
        names_.push_back(name);
      }

      //! set names
      void set_names(std::vector<std::string> names){
        names_.clear();
        names_=names;
      }

      //! add name
      void add_name(std::string name){
        names_.push_back(name);
      }

      //! set helix_vertex
      void set_helix_vertex(experimental_point v, std::string type, size_t calo_helix_id = 0){
        has_helix_vertex_ = true;
        helix_vertex_ = v;
        helix_vertex_type_ = type;
        if( type=="calo" )
          calo_helix_id_ = calo_helix_id;
      }

      //! set decay_helix_vertex
      void set_decay_helix_vertex(experimental_point v, std::string type, size_t calo_helix_id = 0){
        has_decay_helix_vertex_ = true;
        decay_helix_vertex_ = v;
        decay_helix_vertex_type_ = type;
        if( type=="calo" )
          calo_helix_id_ = calo_helix_id;
      }

      //! set tangent_vertex
      void set_tangent_vertex(experimental_point v, std::string type, size_t calo_tangent_id = 0){
        has_tangent_vertex_ = true;
        tangent_vertex_ = v;
        tangent_vertex_type_ = type;
        if( type=="calo" )
          calo_tangent_id_ = calo_tangent_id;
      }

      //! set decay_tangent_vertex
      void set_decay_tangent_vertex(experimental_point v, std::string type, size_t calo_tangent_id = 0){
        has_decay_tangent_vertex_ = true;
        decay_tangent_vertex_ = v;
        decay_tangent_vertex_type_ = type;
        if( type=="calo" )
          calo_tangent_id_ = calo_tangent_id;
      }

      //! set chi2 list
      void set_chi2s(std::vector<double> chi2s){
        chi2s_ = chi2s;
      }

      //! set helix_chi2 list
      void set_helix_chi2s(std::vector<double> helix_chi2s){
        helix_chi2s_ = helix_chi2s;
      }

      //! set prob list
      void set_probs(std::vector<double> probs){
        probs_ = probs;
      }


      //! set charge
      void set_charge(experimental_double charge){
        charge_ = charge;
      }

      //! set momentum
      void set_momentum(experimental_double mom){
        momentum_ = mom;
      }

      //! set length
      void set_length(experimental_double mom){
        length_ = mom;
      }

      //! set primary
      void set_primary(bool primary){
        primary_ = primary;
      }

      //! set helix
      void set_helix(helix h){
        helix_ = h;
      }


      //! get nodes
      const std::vector<node> & nodes()const{
        return nodes_;
      }

      //! get free level
      const bool Free()const{
        return free_;
      }

      //! get name
      const std::string & name()const{
        return names_[0];
      }

      //! get names
      const std::vector<std::string> & names()const{
        return names_;
      }

      //! get primary
      const bool primary()const{
        return primary_;
      }

      //! get list of all chi2
      const std::vector<double>& chi2s_all() const
      {
        return chi2s_;
      }

      //! get list of all helix_chi2
      const std::vector<double>& helix_chi2s_all() const
      {
        return helix_chi2s_;
      }

      //! get list of prob
      const std::vector<double>& probs_all() const
      {
        return probs_;
      }

      //! get list of chi2 used in the sequence
      std::vector<double> chi2s()const
      {
        std::vector<double> c;

        for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in)
          c.push_back(in->chi2());

        return c;
      }

      //! get list of helix_chi2
      const std::vector<double> helix_chi2s() const
      {

        return helix_chi2s_;
      }

      //! get list of probs used in the sequence
      std::vector<double> probs()const
      {
        std::vector<double> p;

        for(std::vector<double>::const_iterator iprob =probs_.begin();iprob !=probs_.end(); ++iprob){
          if( probs_[iprob - probs_.begin()] > prob() )
            p.push_back(*iprob);
        }

        return p;
      }

      // get chi2 of sequence
      double chi2()const{
        double chi2 = 0.;
        std::vector<double> cs = chi2s();
        for(std::vector<double>::const_iterator ichi = cs.begin(); ichi != cs.end(); ++ichi){
          chi2 += *ichi;
        }

        return chi2;

      }

      // get helix_chi2 of sequence
      double helix_chi2()const{
        double helix_chi2 = 0.;
        std::vector<double> cs = helix_chi2s();
        for(std::vector<double>::const_iterator ichi = cs.begin(); ichi != cs.end(); ++ichi){
          helix_chi2 += *ichi;
        }

        return helix_chi2;

      }

      // get prob of sequence
      double Prob()const{
        return probof(chi2(), ndof());

      }

      // get ndof of sequence
      int32_t ndof()const{
        // for each triplet, 2 angles and 2 separation angles
        // except for the first triplet (only 2 angles)
        std::vector<double> cs = chi2s();
        int32_t ndof = 0;
        if( cs.size() > 0 )
          ndof += 2;
        if( cs.size() > 1 )
          ndof += (cs.size() - 1)*4;

        return ndof;

      }

      // get prob of sequence
      double helix_Prob()const{
        double c = helix_chi2();
        // for each triplet's point, three coordinates
        std::vector<double> cs = helix_chi2s();
        int32_t ndof = 0;
        if( cs.size() > 2 )
          ndof += 3*(cs.size() - 2)*3;

        return probof(c, ndof);

      }

      //! has helix_vertex
      const bool has_helix_vertex()const{
        return has_helix_vertex_;
      }

      //! get helix_vertex
      const experimental_point helix_vertex()const{
        return helix_vertex_;
      }

      //! get helix_vertex type
      const std::string helix_vertex_type()const{
        return helix_vertex_type_;
      }

      //! get helix_vertex id
      const size_t helix_vertex_id()const{
        return helix_vertex_id_;
      }

      //! has decay_helix_vertex
      const bool has_decay_helix_vertex()const{
        return has_decay_helix_vertex_;
      }

      //! get decay_helix_vertex
      const experimental_point decay_helix_vertex()const{
        return decay_helix_vertex_;
      }

      //! get calo id
      const size_t calo_helix_id()const{
        return calo_helix_id_;
      }

      //! get decay helix_vertex type
      const std::string decay_helix_vertex_type()const{
        return decay_helix_vertex_type_;
      }

      //! has tangent_vertex
      const bool has_tangent_vertex()const{
        return has_tangent_vertex_;
      }

      //! get tangent_vertex
      const experimental_point tangent_vertex()const{
        return tangent_vertex_;
      }

      //! get tangent_vertex type
      const std::string tangent_vertex_type()const{
        return tangent_vertex_type_;
      }

      //! get tangent_vertex id
      const size_t tangent_vertex_id()const{
        return tangent_vertex_id_;
      }

      //! has decay_tangent_vertex
      const bool has_decay_tangent_vertex()const{
        return has_decay_tangent_vertex_;
      }

      //! get decay_tangent_vertex
      const experimental_point decay_tangent_vertex()const{
        return decay_tangent_vertex_;
      }

      //! get calo id
      const size_t calo_tangent_id()const{
        return calo_tangent_id_;
      }

      //! get decay tangent_vertex type
      const std::string decay_tangent_vertex_type()const{
        return decay_tangent_vertex_type_;
      }

      //! get charge
      const experimental_double charge() const
      {
        return charge_;
      }

      //! get momentum
      const experimental_double momentum() const
      {
        return momentum_;
      }

      //! get length
      const experimental_double length() const
      {
        return length_;
      }

      //! get helix
      const helix get_helix() const
      {
        return helix_;
      }


      //! get fast
      bool fast()const
      {
        if( !nodes_.empty())
          return nodes_[0].c().fast();

        return true;
      }


      //! get last node
      node last_node()const{
        size_t s = nodes().size();

        if( s < 1 ){
          std::clog << " problem: request of last node from sequence of size " << s << std::endl;
          topology::node null;
          return null;
        }

        return nodes()[s-1];
      }


      //! get second last node
      node second_last_node()const{
        size_t s = nodes().size();

        if( s < 2 ){
          std::clog << " problem: request of second last node from sequence of size " << s << std::endl;
          topology::node null;
          return null;
        }

        return nodes()[s-2];
      }


      //! get middle node
      node middle_node()const{
        size_t s = nodes().size();

        if( s < 1 ){
          std::clog << " problem: request of middle node from sequence of size " << s << std::endl;
          topology::node null;
          return null;
        }

        size_t smid = 0;
        if(s % 2 == 0) // It's even
          {
            smid = (size_t)((double)s/2. + 0.5);
          }
        else  // It's odd
          {
            smid = (size_t)((double)(s - 1.)/2. + 0.5);
          }

        return nodes()[smid];
      }



    public:

      bool has_cell(cell c)const{
        if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())

          return true;

        return false;
      }


      sequence invert(){
        sequence inverted;
        inverted.set_print_level(print_level());
        inverted.set_nsigma(nsigma());
        inverted.set_free(Free());
        inverted.set_helix(helix_.invert());

        std::vector<node> inverted_nodes;
        for(size_t i = 0; i < nodes_.size(); i++){
          topology::node in = nodes_[nodes_.size() - 1 - i];
          if( i == nodes_.size() - 1 ){  // first point which becomes last point
            std::vector<cell_couplet> cc;
            std::vector<cell_triplet> ccc;
            std::vector<cell>  links;
            in.set_cc(cc);
            in.set_ccc(ccc);
            in.set_links(links);
          }
          else if( i == 0 ){ // last point which becomes first point
            std::vector<cell_couplet> cc;
            std::vector<cell_triplet> ccc;
            std::vector<cell>  links;
            in.set_cc(cc);
            in.set_ccc(ccc);
            in.set_links(links);
          }
          else{
            std::vector<cell_couplet> cc;
            std::vector<cell_triplet> ccc;
            std::vector<cell>  links;
            in.set_cc(cc);
            in.set_ccc(ccc);
            in.set_links(links);
          }


          inverted_nodes.push_back(in);
        }
        inverted.set_nodes( inverted_nodes );
	inverted.set_names( names() );

        if( has_decay_helix_vertex() ){
          if( decay_helix_vertex_type() == "foil" || decay_helix_vertex_type() == "kink")
            inverted.set_helix_vertex( decay_helix_vertex(), decay_helix_vertex_type() );
          else if( decay_helix_vertex_type() == "calo" )
            inverted.set_helix_vertex( decay_helix_vertex(), decay_helix_vertex_type(), calo_helix_id() );
        }
        if( has_helix_vertex() ){
          if( helix_vertex_type() == "foil" || helix_vertex_type() == "kink")
            inverted.set_decay_helix_vertex( helix_vertex(), helix_vertex_type() );
          else if( helix_vertex_type() == "calo" )
            inverted.set_decay_helix_vertex( helix_vertex(), helix_vertex_type(), helix_vertex_id() );
        }

        if( has_decay_tangent_vertex() ){
          if( decay_tangent_vertex_type() == "foil" || decay_tangent_vertex_type() == "kink")
            inverted.set_tangent_vertex( decay_tangent_vertex(), decay_tangent_vertex_type() );
          else if( decay_tangent_vertex_type() == "calo" )
            inverted.set_tangent_vertex( decay_tangent_vertex(), decay_tangent_vertex_type(), calo_tangent_id() );
        }
        if( has_tangent_vertex() ){
          if( tangent_vertex_type() == "foil" || tangent_vertex_type() == "kink")
            inverted.set_decay_tangent_vertex( tangent_vertex(), tangent_vertex_type() );
          else if( tangent_vertex_type() == "calo" )
            inverted.set_decay_tangent_vertex( tangent_vertex(), tangent_vertex_type(), tangent_vertex_id() );
        }
        inverted.set_chi2s(chi2s());
        inverted.set_helix_chi2s(helix_chi2s());
        inverted.set_probs(probs());
        return inverted;

      }

      void set_free_level(){

        set_free(false);
        for(std::vector<node>::iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode)
          {
            inode->set_free(false);

            for(std::vector<cell>::iterator ilink=(*inode).links_.begin(); ilink != (*inode).links_.end(); ++ilink){
              ilink->set_free(false);
              size_t iccc = get_link_index_of_cell(inode - nodes_.begin(), *ilink);
              if( inode - nodes_.begin() < 1 ){

                if( has_cell(*ilink)){
                  ilink->set_begun(true);
                  continue;
                }

                for(std::vector<line>::const_iterator itang = (*inode).cc_[iccc].tangents_.begin(); itang != (*inode).cc_[iccc].tangents_.end(); ++itang){
                  if( ! itang->used() ){
                    ilink->set_free(true);
                    inode->set_free(true);
                    set_free( true);
                    if( print_level() >= mybhep::VVERBOSE ){
                      std::clog << " sequence is free, for cell " << inode->c().id() << " is free, for link to cell " << ilink->id() << " is free, for tangent " << itang - (*inode).cc_[iccc].tangents_.begin() << " is unused " << std::endl;
                    }
                  }
                }
              }else{
                if( has_cell((*inode).ccc_[iccc].ca()) &&
                    has_cell((*inode).ccc_[iccc].cc()) ){
                  ilink->set_begun(true);
                }

                for(std::vector<joint>::const_iterator ijoint = (*inode).ccc_[iccc].joints_.begin(); ijoint != (*inode).ccc_[iccc].joints_.end();++ijoint){
                  if( ! ijoint->used() ){
                    ilink->set_free(true);
                    inode->set_free(true);
                    set_free( true);
                    if( print_level() >= mybhep::VVERBOSE ){
                      std::clog << " sequence is free, for cell " << (*inode).c().id() << " is free, for triplet with cells " << (*inode).ccc_[iccc].ca().id() << "<->" << (*inode).ccc_[iccc].cc().id() << " is free, for joint " << ijoint - (*inode).ccc_[iccc].joints_.begin() << " is unused " << std::endl;
                    }
                  }
                }
              }
            }
          }

        return;

      }


      //! get last free node
      void last_free_node(size_t *lfn, size_t *link){
        size_t ilfn = 0;
        size_t ilink = 0;

        bool found = false;
        for(std::vector<node>::iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode)
          if( inode->free() )
            {
              ilfn = inode - nodes_.begin();
              found = false;
              for(std::vector<cell>::iterator itlink = (*inode).links_.begin(); itlink != (*inode).links_.end(); ++itlink)
                if( itlink->free() ){
                  ilink = itlink - (*inode).links_.begin();
                  found = true;
                  break;
                }
            }

        if( !found ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: sequence has no last free node. free status is " << Free() << std::endl;
          *lfn = 0;
          *link = 0;
          return;
        }

        ilink = get_link_index_of_cell(ilfn, nodes()[ilfn].links_[ilink]);

        *lfn = ilfn;
        *link = ilink;

        return;
      }


      topology::node node_of_cell(topology::cell c){

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


      bool contained( topology::sequence big)const{
        // true if the sequence is shorter than BIG and each node is also in BIG
        // false otherwise

        if( nodes().size() > big.nodes().size() )
          return false;


        for(std::vector<node>::const_iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode)
          if( !big.has_cell( inode->c() ) )
            return false;

        if( nodes().size() == big.nodes().size() )
          return contained_same_size_and_cells(big);

        return true;

      }


      bool contained_same_size_and_cells( topology::sequence big)const{

        size_t s = nodes().size();

        if( nodes().front().c().id() == big.nodes().front().c().id() ){
          if( !nodes_[0].c_.same_quadrant( nodes().front().ep(), big.nodes().front().ep() ) )
            return false;
          if( !nodes_[s-1].c_.same_quadrant( nodes().back().ep(), big.nodes().back().ep() ) )
            return false;
        }
        else if( nodes().front().c().id() == big.nodes().back().c().id() ){
          if( !nodes_[0].c_.same_quadrant( nodes().front().ep(), big.nodes().back().ep() ) )
            return false;
          if( !nodes_[s-1].c_.same_quadrant( nodes().back().ep(), big.nodes().front().ep() ) )
            return false;
        }
        else{
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: sequences with same size and cells have different beginnings or endings: 1st sequence from " << nodes().front().c().id() << " to " << nodes().back().c().id() << ", 2nd from " << big.nodes().front().c().id()  << " to " << big.nodes().back().c().id()   << std::endl;
          }
        }

        double thisprob = Prob();
        double bigprob = big.Prob();

        return (thisprob < bigprob);

      }


      bool is_bridge( topology::sequence first, topology::sequence second)const{

        size_t s = nodes().size();

        if( first.has_cell(nodes()[0].c()) &&
            second.has_cell(nodes()[s-1].c() ) )
          return true;

        if( second.has_cell(nodes()[0].c()) &&
            first.has_cell(nodes()[s-1].c() ) )
          return true;

        return false;

      }



      void increase_iteration(size_t inode, size_t ilink){
        if( inode >= nodes().size() ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: increasing iteration of node " << inode << " but size is " << nodes().size() << std::endl;
          return;
        }

        if( inode < 1 ){
          if( ilink >= nodes()[inode].cc().size() ){
            if( print_level() >= mybhep::NORMAL )
              std::clog << " problem: increasing iteration of link " << ilink << " but couplet size is " << nodes()[inode].cc().size() << std::endl;
            return;
          }

          for(std::vector<line>::iterator itang = nodes_[inode].cc_[ilink].tangents_.begin(); itang != nodes_[inode].cc_[ilink].tangents_.end(); ++itang){
            if( ! itang->used() ){
              itang->set_used(true);
              return;
            }
          }
          nodes_[inode].cc_[ilink].set_free(false);
          return;
        }

        if( ilink >= nodes()[inode].ccc().size() ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: increasing iteration of link " << ilink << " but triplet size is " << nodes()[inode].ccc().size() << std::endl;
          return;
        }

        for(std::vector<joint>::iterator ijoint = nodes_[inode].ccc_[ilink].joints_.begin(); ijoint != nodes_[inode].ccc_[ilink].joints_.end(); ++ijoint){
          if( ! ijoint->used() ){
            ijoint->set_used(true);
            return;
          }
        }

        nodes_[inode].ccc_[ilink].set_free(false);
        return;
      }


      sequence copy_to_last_free_node(size_t *ilfn, size_t *ilink){

        sequence newsequence;
        newsequence.set_print_level(print_level());
        newsequence.set_nsigma(nsigma());

        size_t lfn , link;
        last_free_node(&lfn, &link);
        *ilfn = lfn;
        *ilink = link;

        size_t linkid;
        if( lfn < 1 )
          linkid = nodes_[lfn].cc()[link].cb().id();
        else{
          linkid = nodes_[lfn].ccc()[link].ca().id();
          if( nodes()[lfn-1].c().id() == linkid )
            linkid = nodes_[lfn].ccc()[link].cc().id();
        }

        if( print_level() >= mybhep::VVERBOSE )
          std::clog << " ilfn: " << lfn << " id: " << nodes_[lfn].c().id()
               << " ilink " << link << " id: " << linkid << std::endl;


        for(size_t i=0; i<lfn+1; i++)
          {
            newsequence.nodes_.push_back( nodes()[i] );
            newsequence.nodes_[i].set_free( false );
            for(std::vector<cell_couplet>::iterator icc = newsequence.nodes_[i].cc_.begin(); icc != newsequence.nodes_[i].cc_.end(); ++icc)
              {
                if( i < lfn || (i == lfn && (size_t)(icc - newsequence.nodes_[i].cc_.begin()) < link ) ){
                  icc->set_free( false);
                  icc->set_begun( true);
                  icc->set_all_used();
                }
              }
            for(std::vector<cell_triplet>::iterator iccc = newsequence.nodes_[i].ccc_.begin(); iccc != newsequence.nodes_[i].ccc_.end(); ++iccc)
              {
                if( i < lfn  || (i == lfn && (size_t)(iccc - newsequence.nodes_[i].ccc_.begin()) < link ) ){
                  iccc->set_free( false);
                  iccc->set_begun( true);
                  iccc->set_all_used();
                }
              }
            for(std::vector<cell>::iterator ilink = newsequence.nodes_[i].links_.begin();  ilink != newsequence.nodes_[i].links_.end(); ++ilink)
              {
                if( i < lfn  || (i == lfn && (size_t)(ilink - newsequence.nodes_[i].links_.begin()) < link ) ){
                  ilink->set_free( false);
                  ilink->set_begun( true);
                }
              }
          }


        //      if( nodes()[lfn].cc()[link].begun() )
        increase_iteration(lfn,link);

        if( lfn < 1 )
          newsequence.nodes_[lfn].cc_[link].set_free( true);
        else
          newsequence.nodes_[lfn].ccc_[link].set_free( true);

        if( lfn == 0 ||
            ( lfn < 1 && !nodes()[lfn].cc()[link].begun() ) ||
            ( lfn >= 1 && !nodes()[lfn].ccc()[link].begun() ) )
          {
            if( lfn == 0 || (lfn < 1 && !nodes()[lfn].cc()[link].begun() ) ){
              nodes_[lfn].cc_[link].set_begun( true);
              nodes_[lfn].cc_[link].set_free( false);
            }
            else{
              nodes_[lfn].ccc_[link].set_begun( true);
              nodes_[lfn].ccc_[link].set_free( false);
            }
          }
        else{
          if( lfn < 1 && nodes_[lfn].cc_[link].iteration() + 1 == nodes()[lfn].cc()[link].tangents().size() )
            {
              nodes_[lfn].cc_[link].set_free( false);
              //      nodes()[lfn].cc_[link].iteration = 0;
            }
          else if(lfn >= 1 && nodes_[lfn].ccc_[link].iteration() + 1 == nodes()[lfn].ccc()[link].joints().size()){
            nodes_[lfn].ccc_[link].set_free( false);
          }
        }

        set_free_level();
        newsequence.set_free_level();

        return newsequence;

      }


      void fill_links_of_node(size_t inode, cluster local_sequence){

        if( print_level() >= mybhep::VVERBOSE )
          std::clog << " filling links for cell " << nodes()[inode].c().id() << std::endl;

        if( inode >= nodes().size() ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: asking for links for node with index " << inode << " but nodes size is " << nodes().size() << std::endl;
          return;
        }

        if( inode == 0 ){
          // the first node is a helix_vertex (there are no triplets)
          // so it shall propagate through a couplet

          std::vector<bool> shall_one_have_triplet;
          shall_one_have_triplet.assign (nodes()[0].cc().size(), false);
          // 2012-03-26 FM: remove C style
          //bool* shall_one_have_triplet = (bool*)malloc(sizeof(bool)*nodes()[0].cc().size());

          bool shall_have_triplet = false;
          for(std::vector<cell_couplet>::iterator icc=nodes_[0].cc_.begin(); icc != nodes_[0].cc_.end(); ++icc){
            shall_one_have_triplet[icc - nodes_[0].cc_.begin()] = false;
            node n = local_sequence.node_of_cell(icc->cb());

            if( n.has_triplet(nodes_[0].c()) ){
              shall_have_triplet = true;
              shall_one_have_triplet[icc - nodes_[0].cc_.begin()] = true;
            }
          }


          for(std::vector<cell_couplet>::iterator icc=nodes_[0].cc_.begin(); icc != nodes_[0].cc_.end(); ++icc){
            if( has_cell(icc->cb() ) )
              continue;
            if( (!shall_have_triplet ) ||
                ( shall_have_triplet && shall_one_have_triplet[icc - nodes_[0].cc_.begin()] ) )
              nodes_[0].links_.push_back(icc->cb());
          }

          // 2012-03-26 FM: remove C style
          //free(shall_one_have_triplet);
          return;
        }


        for(std::vector<cell_triplet>::iterator iccc=nodes_[inode].ccc_.begin(); iccc != nodes_[inode].ccc_.end(); ++iccc){

          // the new candidate triplet must include the previous node
          if( iccc->ca().id() == nodes_[inode - 1].c().id() && !has_cell(iccc->cc())){
            nodes_[inode].links_.push_back(iccc->cc());
          }
          else if( iccc->cc().id() == nodes_[inode - 1].c().id() && !has_cell(iccc->ca()) ){
            nodes_[inode].links_.push_back(iccc->ca());
          }

        }

        return;


      }



      bool pick_new_cell(size_t *ilink, experimental_point *newp, cluster local_cluster){

        size_t s = nodes().size();

        bool ok = false;

        if( nodes_[s-1].links().empty() )
          fill_links_of_node(s-1, local_cluster);

        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " possible links: ";
          for(std::vector<cell>::iterator ilink=nodes_[s-1].links_.begin(); ilink != nodes_[s-1].links_.end(); ++ilink){
            std::clog << " " << ilink->id();
          }
          std::clog << " " << std::endl;
        }


        for(std::vector<cell>::iterator itlink = nodes_[s-1].links_.begin(); itlink != nodes_[s-1].links_.end(); ++itlink){
          if( itlink->free() || ! itlink->begun() )
            {
              *ilink = (size_t)(itlink - nodes_[s-1].links_.begin());
              itlink->set_begun(true);
              itlink->set_free(false);

              if( s == 1 ){
                ok = true;
                if( print_level() >= mybhep::VVERBOSE )
                  std::clog << " new cell is " << itlink->id() << std::endl;
                break;
              }
              else{

                size_t iccc = get_link_index_of_cell(s-1, *itlink);

                size_t iteration = nodes_[s-1].ccc_[iccc].iteration();

                if( iteration >= nodes_[s-1].ccc_[iccc].joints().size() ){
                  if( print_level() >= mybhep::NORMAL ){
                    std::clog << "problem: iteration" << iteration << " joints size " << nodes_[s-1].ccc_[iccc].joints_.size() << "... stop evolving" << std::endl;
                  }

                  itlink->set_free(false);
                  return false;
                }

                if( iteration == 0 ){
                  // first time this connection is tried
                  // so get rid of spurious joints

                  if( print_level() >= mybhep::VVERBOSE ){
                    std::clog << " initially there are " << nodes_[s-1].ccc_[iccc].joints_.size() << " possible joints to go from cell " << nodes_[s-1].c().id() << " to " << itlink->id()  << std::endl;
                  }

                  std::vector<joint>::iterator ijoint = nodes_[s-1].ccc_[iccc].joints_.begin();

                  while( ijoint != nodes_[s-1].ccc_[iccc].joints_.end() ){

                    if( nodes_[s-1].ccc_[iccc].ca().id() == itlink->id() ){
                      *ijoint = ijoint->invert();
                    }

                    if( !compatible(& (*ijoint) , *itlink ) ){
                      if( print_level() >= mybhep::VVERBOSE ){
                        std::clog << " erase joint " << ijoint - nodes_[s-1].ccc_[iccc].joints_.begin() << std::endl;
                      }
                      nodes_[s-1].ccc_[iccc].joints_.erase(ijoint);
                    }
                    else
                      ++ijoint;

                  }

                  if( print_level() >= mybhep::VVERBOSE ){
                    std::clog << " after cleaning there are " << nodes_[s-1].ccc_[iccc].joints_.size() << " possible joints to go from cell " << nodes_[s-1].c().id() << " to " << itlink->id()  << std::endl;
                  }

                }

                if( nodes_[s-1].ccc_[iccc].joints_.empty() ){
                  if( print_level() >= mybhep::VVERBOSE ){
                    std::clog << " no joints to connect cell " << nodes_[s-1].c().id() << " to new cell " << itlink->id()
                         << std::endl;
                  }
                  continue;
                }


                iteration = nodes_[s-1].ccc_[iccc].iteration();

                if( print_level() >= mybhep::VVERBOSE ){
                  std::clog << " connecting cell " << nodes_[s-1].c().id() << " to new cell " << itlink->id()
                       << " with iteration " << iteration << " of " << nodes_[s-1].ccc_[iccc].joints_.size() <<  std::endl;
                }

                if( iteration + 1 == nodes_[s-1].ccc_[iccc].joints_.size() )
                  itlink->set_free( false);
                else
                  {
                    itlink->set_free( true);
                    nodes_[s-1].set_free( true);
                    set_free( true);
                  }

                increase_iteration(s-1,iccc);

                topology::joint j = nodes_[s-1].ccc_[iccc].joints_[iteration];

                nodes_[s-2].set_ep(j.epa());
                nodes_[s-1].set_ep(j.epb());
                nodes_[s-1].set_chi2(j.chi2());
                *newp = j.epc();
                ok = true;
                break;

              }


            }
        }




        return ok;
      }

      bool compatible(joint *j, cell cc){

        size_t s = nodes().size();
        if( s < 2 ){
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: checking compatibility for sequence of size " << s << std::endl;
          }
          return false;
        }

        int32_t ndof = 2;  // 2 kink angles and 0, or 2 separation angles

        topology::experimental_double local_separation_a, local_separation_b;
        double chi2 = 0.;

        bool use_theta_kink = true;

        topology::experimental_point pa = j->epa();
        topology::experimental_point pb = j->epb();
        if( s >= 2 ){
          topology::cell ca = second_last_node().c();
          topology::cell cb = last_node().c();

          if(ca.unknown_vertical() || cb.unknown_vertical() || cc.unknown_vertical()){
            use_theta_kink = false;
            ndof --;
          }

          if( s > 2 ){
            if( !ca.small() && !ca.intersect(cb) ){
              pa = ca.angular_average(second_last_node().ep(), j->epa(), &local_separation_a);
              chi2 += mybhep::square(local_separation_a.value()/local_separation_a.error());
              ndof ++;
            }

            if( !cb.small() && !cb.intersect(ca) && !cb.intersect(cc) ){
              pb = cb.angular_average(last_node().ep(), j->epb(), &local_separation_b);
              chi2 += mybhep::square(local_separation_b.value()/local_separation_b.error());
              ndof ++;
            }
          }

        }


        topology::line l1(pa, pb);
        topology::line l2(pb, j->epc());

        chi2 += l1.chi2(l2, use_theta_kink);
        j->set_chi2(chi2);
        chi2s_.push_back(chi2);
        double local_prob = probof(chi2, ndof);
        probs_.push_back(local_prob);

        if( local_prob > prob() ){

          if( print_level() >= mybhep::VVERBOSE ){
            std::clog << " connecting cell " << last_node().c().id() << " is compatible with chi2 " << chi2 << " prob " << local_prob << std::endl; fflush(stdout);
          }


          j->set_epa(pa);
          j->set_epb(pb);

          return true;

        }

        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " connecting cell " << last_node().c().id() << " is incompatible: ";
          if( s > 2 )
            std::clog << " separation a: " << local_separation_a.value() << " +- " << local_separation_a.error();
          std::clog << " separation b: " << local_separation_b.value() << " +- " << local_separation_b.error()
               << " total connection chi2: " << chi2 << " prob " << local_prob << std::endl; fflush(stdout);
        }

        return false;

      }


      int get_link_index_of_cell(size_t inode, cell link) const {
        // inode = 0 if it is the first node, 1 if it is the second node...
        // link = a cell to which the node inode links with a couplet or triplet
        // returns an index, such that cc[index] or ccc[index] is the needed couplet or triplet


        if( inode >= nodes().size() ){
          if( print_level() >= mybhep::NORMAL )
            std::clog << " problem: getting link index for node index " << inode << " but size is " << nodes().size() << std::endl;
          return 0;
        }



        if( fabs( nodes()[inode].c().block() - link.block()) == 1 ){
          // the connection happens through a gap
          return -1;
        }


        cell null;

        if( inode < 1 ){

          std::vector<cell_couplet> ccv = nodes()[inode].cc();
          std::vector<cell_couplet>::iterator fcouplet = std::find(ccv.begin(),
                                                              ccv.end(),
                                                              cell_couplet(null, link));

          if( fcouplet == ccv.end()){
            if( print_level() >= mybhep::NORMAL )
              std::clog << " problem: looking for couplet from cell index " << inode << " to " << link.id() << " but it's not there " << std::endl;

            return 0;
          }

          return fcouplet - ccv.begin();


        }

        std::vector<cell_triplet> cccv = nodes()[inode].ccc();
        std::vector<cell_triplet>::iterator ftriplet = std::find(cccv.begin(),
                                                            cccv.end(),
                                                            cell_triplet(link,null,nodes_[inode-1].c()));

        if( ftriplet == cccv.end()){
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: looking for triplet from cell index " << inode << " id " << nodes()[inode].c().id()  << " to cell id " << link.id() << " but it's not there " << std::endl; fflush(stdout);
          }
          return 0;
        }


        return ftriplet - cccv.begin();

      }


      void calculate_helix(void) {

        helix_chi2s_.clear();

        int method = 3;
        // method 1:
        // fit a helix throgh every three consecutive points
        // then average
        //
        // method 2:
        // fit a helix throgh 1st, middle and last point
        //
        // method 3:
        // fit best circle, then best helix, through all points
        //


        switch(method){

        case 1: {

          std::vector<helix> helices;

          for(std::vector<node>::iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode){
            if( inode - nodes_.begin() == 0 || nodes_.end() - inode == 1 ) continue;

            std::vector<node>::iterator anode = inode;
            std::vector<node>::iterator cnode = inode;
            --anode; ++cnode;
            if( print_level() >= mybhep::VVERBOSE ){
              std::clog << " calculate helix for three points " << anode->c().id() << " , " << inode->c().id() << " , " << cnode->c().id() << std::endl;
            }
            helices.push_back(three_points_helix(anode->ep(), inode->ep(), cnode->ep()));

          }

          helix_ = average(helices);
          helix_.set_print_level(print_level());
          helix_.set_nsigma(nsigma());

          experimental_point _center = helix_.center();
          experimental_double Yc = middle_node().ep().y() - helix_.pitch()*experimental_vector(_center, middle_node().ep()).phi();
          _center.set_y(Yc);
          helix_.set_center(_center);

          break;
        }
        case 2:{

          circle ci = three_points_circle(nodes().front().ep(), middle_node().ep(), nodes().back().ep());

          break;
        }
        case 3:{

          // fit best circle through all points (horizontal view)

          std::vector<experimental_point> ps;
          std::vector<experimental_double> phis;
          std::vector<experimental_double> xs;
          std::vector<experimental_double> ys;
          std::vector<experimental_double> zs;

          for(std::vector<node>::iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode){
            ps.push_back(inode->ep());
            xs.push_back(inode->ep().x());
            ys.push_back(inode->ep().y());
            zs.push_back(inode->ep().z());
          }

          CircleRegression cl(xs, zs, print_level(), nsigma());
          cl.fit();

          if( print_level() >= mybhep::VVERBOSE ){
            cl.dump();
          }


          circle ci = cl.c();

          bool first = true;
          experimental_double offset(0.,0.);;

	  experimental_double p(0.,0.);
	  double phi_ref = 0.;
          for(std::vector<node>::iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode){
	    phi_ref = p.value();
            p = ci.phi_of_point(inode->ep(), phi_ref);
            if( !phis.empty() && first){
              if( fabs(p.value() - phis.back().value()) > M_PI ){

                first = false;
                if( p.value() > phis.back().value() )
                  offset.set_value(-2.*M_PI);
                else
                  offset.set_value(2.*M_PI);
              }
            }


            phis.push_back(p + offset);
          }

          // fit best helix with through all points (vertical view)
          LinearRegression l(ys, phis, print_level(), nsigma());
          l.fit();
          l.invert(); // fit with y as more erroneous variable (phi = phi(y)),
          // then invert the result to have y = y(phi)


          if( print_level() >= mybhep::VVERBOSE ){
            l.dump();
          }


          // build helix
          ci.set_center(experimental_point(ci.center().x(), l.y0(), ci.center().z()));
          helix_ = helix(ci, l.tangent(), print_level(), nsigma());

	

          helix_chi2s_ = helix_.chi2s(ps);


          break;
        }
        default:
	  
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: unknonw method "<< method << " to fit a helix " << std::endl;
          }
        }
	
	
        return;
      }


      experimental_double radius(){
        return helix_.radius();
      }

      experimental_double pitch(){
        return helix_.pitch();
      }

      experimental_point center(){
        return helix_.center();
      }

      void calculate_momentum(double bfield){
        momentum_ = experimental_sqrt(experimental_square(radius()) + experimental_square(pitch()))*0.3*bfield;
      }

      void calculate_charge(void){

        if( nodes().size() < 3 ) return;

        experimental_vector vi(nodes().front().ep(), nodes_[1].ep());
        experimental_vector vf(second_last_node().ep(), last_node().ep());

        double phi1 = vi.phi().value();
        double phi2 = vf.phi().value();
        mybhep::fix_angles(&phi1, &phi2);

        experimental_double deltaphi = vf.phi() - vi.phi();
        deltaphi.set_value(phi2 - phi1);

        charge_ = deltaphi/fabs(deltaphi.value());
      }

      bool intersect_plane_with_tangent_from_end(plane pl, experimental_point * ep)const{
	// need 2 nodes to build the tangent line
        if( nodes().size() < 2 ) return false;

        experimental_vector direction(second_last_node().ep(), last_node().ep());
	bool result = pl.intersect(last_node().ep(), direction, ep);

        return result;
      }



      bool intersect_plane_from_end(plane pl, experimental_point * ep)const{

        experimental_double _phi = helix_.phi_of_point(last_node().ep());

        bool result = helix_.intersect_plane(pl, ep, _phi);

        return result;

      }


      bool intersect_plane_with_tangent_from_begin(plane pl, experimental_point * ep)const{
	// need 2 nodes to build the tangent line
        if( nodes().size() < 2 ) return false;

        experimental_vector direction(nodes_[1].ep(), nodes_[0].ep());
	bool result = pl.intersect(nodes_[0].ep(), direction, ep);

        return result;
      }

      bool intersect_plane_from_begin(plane pl, experimental_point * ep)const{

        if( nodes().size() < 1 ) return false;

        experimental_double _phi = helix_.phi_of_point(nodes_[0].ep());

        bool result = helix_.intersect_plane(pl, ep, _phi);

        return result;

      }


      bool intersect_circle_with_tangent_from_end(circle c, experimental_point * ep)const{

	// need 2 nodes to build the tangent line
        if( nodes().size() < 2 ) return false;
	
        experimental_vector direction(second_last_node().ep(), last_node().ep());
	bool result = helix_.intersect_circle_with_tangent(c, last_node().ep(), direction, ep);

        return result;
      }

      bool intersect_circle_from_end(circle c, experimental_point * ep)const{

        experimental_double _phi = helix_.phi_of_point(last_node().ep());

        bool result = helix_.intersect_circle(c, ep, _phi);

        return result;

      }

      bool intersect_circle_with_tangent_from_begin(circle c, experimental_point * ep)const{

	// need 2 nodes to build the tangent line
        if( nodes().size() < 2 ) return false;
	
        experimental_vector direction(nodes_[1].ep(), nodes_[0].ep());
	bool result = helix_.intersect_circle_with_tangent(c, nodes_[0].ep(), direction, ep);

        return result;
      }

      bool intersect_circle_from_begin(circle c, experimental_point * ep)const{

        if( nodes().size() < 1 ) return false;

        experimental_double _phi = helix_.phi_of_point(nodes_[0].ep());

        bool result = helix_.intersect_circle(c, ep, _phi);

        return result;

      }


      bool intersect_sequence(sequence seq, bool invertA, bool invertB, experimental_point * ep, double limit_distance){
	circle c = seq.get_helix().get_circle();
	bool result;
        double distanceA, distanceB;
	if( invertA ){
	  result=intersect_circle_from_end(c,ep);
	  distanceA = nodes_[0].ep().distance(*ep).value();
	}
	else{
	  result=intersect_circle_from_begin(c,ep);
	  distanceA = nodes_.back().ep().distance(*ep).value();
	}

	if(!result) return false;

	if( invertB )
	  distanceB = seq.nodes().back().ep().distance(*ep).value();
	else
	  distanceB = seq.nodes_[0].ep().distance(*ep).value();


	return (result && distanceA <= limit_distance && distanceB <= limit_distance );

      }


      std::string family()const{
        size_t i1 = name().find("_");
        size_t i2 = name().find("_",i1+1);
        return name().substr(i1+1,i2-i1-1);
      }

      std::vector<size_t> families()const{
        std::vector<size_t> fs;
        for(std::vector<std::string>::const_iterator iname=names_.begin(); iname!=names_.end(); ++iname){
          size_t i1 = iname->find("_");
          size_t i2 = iname->find("_",i1+1);
          fs.push_back(mybhep::int_from_string(iname->substr(i1+1,i2-i1-1)));
        }
        return fs;
      }

      bool good_match(sequence seq, bool &invertA, bool &invertB, size_t NOffLayers)const{

        if( !seq.fast() ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, delayed track " << std::endl;
          return false;
        }


        if( seq.nodes().size() < 1 ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, hits size is " << seq.nodes().size() << std::endl;
          return false;
        }


        //no connection if on opposite sides of the foil
        if( nodes_[0].c().block() * seq.nodes_[0].c().block() < 0 ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, opposite side of foil " << std::endl;
          return false;
        }


        if( family() == seq.family() ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, belongs to same family " << family() << std::endl;
          return false;
        }


        int layer_distance;

        double distFF = nodes_[0].ep().distance(seq.nodes_[0].ep()).value();
        double distFL = nodes_[0].ep().distance(seq.nodes().back().ep()).value();
        double distLF = nodes().back().ep().distance(seq.nodes_[0].ep()).value();
        double distLL = nodes().back().ep().distance(seq.nodes().back().ep()).value();

        if( distLF <= distFF && distLF <= distFL && distLF <= distLL ){  // last to first  FL --> FL
          invertA = false;
          invertB = false;

          // connection must be between neighboring blocks or within same block
          if( fabs( last_node().c().block() - seq.nodes_[0].c().block()) > 1 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_decay_helix_vertex() && decay_helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has decay helix_vertex on calo " << std::endl;
            return false;
          }
          if( seq.has_helix_vertex() && seq.helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has helix_vertex on calo " << std::endl;
            return false;
          }
          layer_distance = last_node().c().layer() - seq.nodes_[0].c().layer();
        }
        else if( distLL <= distFF && distLL <= distFL && distLL <= distLF ){ // last to last  FL -> LF
          invertA = false;
          invertB = true;

          // connection must be between neighboring blocks or within same block
          if( fabs( last_node().c().block() - seq.last_node().c().block()) > 1 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_decay_helix_vertex() && decay_helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has decay helix_vertex on calo " << std::endl;
            return false;
          }
          if( seq.has_decay_helix_vertex() && seq.decay_helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has decay helix_vertex on calo " << std::endl;
            return false;
          }
          layer_distance = last_node().c().layer() - seq.last_node().c().layer();

        }
        else if( distFL <= distFF && distFL <= distLL && distFL <= distLF ){ // first to last  LF -> LF
          invertA = true;
          invertB = true;
          // connection must be between neighboring blocks or within same block
          if( fabs( nodes_[0].c().block() - seq.last_node().c().block()) > 1 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_helix_vertex() && helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has helix_vertex on calo " << std::endl;
            return false;
          }
          if( seq.has_decay_helix_vertex() && seq.decay_helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has decay helix_vertex on calo " << std::endl;
            return false;
          }
          layer_distance = nodes_[0].c().layer() - seq.last_node().c().layer();

        }
        else{ // first to first  LF -> FL
          invertA = true;
          invertB = false;
          // connection must be between neighboring blocks or within same block
          if( fabs( nodes_[0].c().block() - seq.nodes_[0].c().block()) > 1 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_helix_vertex() && helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has helix_vertex on calo " << std::endl;
            return false;
          }
          if( seq.has_helix_vertex() && seq.helix_vertex_type() == "calo" ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has helix_vertex on calo " << std::endl;
            return false;
          }
          layer_distance = nodes_[0].c().layer() - seq.nodes_[0].c().layer();

        }


        // connection must be between neighboring layers
        if( fabs(layer_distance) > 1 + NOffLayers){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, because layers are far away by " << layer_distance << " planes " << std::endl;
          return false;
        }


        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " ... good match, distances: FF " << distFF << " FL " << distFL << " LF " << distLF << " LL " << distLL << " so invertA " << invertA << " invertB " << invertB << " layerdistance " << layer_distance << std::endl;
        }


        return true;

      }



      sequence match(sequence seq, bool invertA, bool invertB){

        sequence news;
        if( invertA )
          news = this->invert();
        else
          news = *this;

        news.add_name(seq.name());

        size_t s = news.nodes().size();

        for(size_t i = 0; i < seq.nodes_.size(); i++){
          size_t index = i;
          int next_index = i+1;
          bool last = false;

          if( i == seq.nodes_.size() - 1 )
            last = true;

          if( invertB ){
            index = seq.nodes_.size() - 1 - i;
            next_index = index - 1;
          }
          topology::node in = seq.nodes_[index];

          if( i == 0 ){ // 1st added cell must get a new triplet
            news.nodes_[s-1].links_.push_back(in.c());
            cell_triplet ctA(news.nodes_[s-2].c(), news.nodes_[s-1].c(), in.c());
            news.nodes_[s-1].ccc_.push_back(ctA);
          }

          if( !last ){
            cell_triplet ct(news.last_node().c(), in.c(), seq.nodes_[next_index].c());
            std::vector<cell_triplet> ccc;
            ccc.push_back(ct);
            in.set_ccc(ccc);
            std::vector<cell> ll;
            ll.push_back(seq.nodes_[next_index].c());
            in.set_links(ll);
          } else {
            in.links_.clear();
            in.ccc_.clear();
          }


          news.nodes_.push_back(in);
        }

        if( !invertB ){
          if( seq.has_decay_helix_vertex() ){
            if( seq.decay_helix_vertex_type() == "foil" || seq.decay_helix_vertex_type() == "kink")
              news.set_decay_helix_vertex( seq.decay_helix_vertex(), seq.decay_helix_vertex_type() );
            else if( seq.decay_helix_vertex_type() == "calo" )
              news.set_decay_helix_vertex( seq.decay_helix_vertex(), seq.decay_helix_vertex_type(), seq.calo_helix_id() );
	  }

          if( seq.has_decay_tangent_vertex() ){
            if( seq.decay_tangent_vertex_type() == "foil" || seq.decay_tangent_vertex_type() == "kink")
              news.set_decay_tangent_vertex( seq.decay_tangent_vertex(), seq.decay_tangent_vertex_type() );
            else if( seq.decay_tangent_vertex_type() == "calo" )
              news.set_decay_tangent_vertex( seq.decay_tangent_vertex(), seq.decay_tangent_vertex_type(), seq.calo_tangent_id() );

          }
        }
        else{
          if( seq.has_helix_vertex() ){
            if( seq.helix_vertex_type() == "foil" || seq.helix_vertex_type() == "kink" )
              news.set_decay_helix_vertex( seq.helix_vertex(), seq.helix_vertex_type() );
            else if( seq.helix_vertex_type() == "calo" )
              news.set_decay_helix_vertex( seq.helix_vertex(), seq.helix_vertex_type(), seq.helix_vertex_id() );
	  }

          if( seq.has_tangent_vertex() ){
            if( seq.tangent_vertex_type() == "foil" || seq.tangent_vertex_type() == "kink" )
              news.set_decay_tangent_vertex( seq.tangent_vertex(), seq.tangent_vertex_type() );
            else if( seq.tangent_vertex_type() == "calo" )
              news.set_decay_tangent_vertex( seq.tangent_vertex(), seq.tangent_vertex_type(), seq.tangent_vertex_id() );


          }
        }


        news.calculate_helix();

        return news;

      }


      bool good_match_with_kink(sequence seq, bool &invertA, bool &invertB, double limit_distance)const{

        if( !seq.fast() ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, delayed track " << std::endl;
          return false;
        }


        if( nodes_.size() < 2 || seq.nodes().size() < 2 ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, hits sizes are " << nodes_.size() << " and " << seq.nodes().size() << std::endl;
          return false;
        }


        //no connection if on opposite sides of the foil
        if( nodes_[0].c().block() * seq.nodes_[0].c().block() < 0 ){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, opposite side of foil " << std::endl;
          return false;
        }





        double distFF = nodes_[0].ep().distance(seq.nodes_[0].ep()).value();
        double distFL = nodes_[0].ep().distance(seq.nodes().back().ep()).value();
        double distLF = nodes().back().ep().distance(seq.nodes_[0].ep()).value();
        double distLL = nodes().back().ep().distance(seq.nodes().back().ep()).value();

        if( distLF <= distFF && distLF <= distFL && distLF <= distLL ){  // last to first  FL --> FL
          invertA = false;
          invertB = false;

	  if( distLF > limit_distance){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because distance " << distLF << " is larger than limit " << limit_distance << std::endl;
            return false;
	  }

          // connection must be within same block
          if( fabs( last_node().c().block() - seq.nodes_[0].c().block()) > 0 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_decay_helix_vertex() && (decay_helix_vertex_type() == "calo" || decay_helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has decay helix_vertex on calo or foil " << std::endl;
            return false;
          }
          if( seq.has_helix_vertex() && (seq.helix_vertex_type() == "calo" || seq.helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has helix_vertex on calo or foil " << std::endl;
            return false;
          }


        }
        else if( distLL <= distFF && distLL <= distFL && distLL <= distLF ){ // last to last  FL -> LF
          invertA = false;
          invertB = true;

	  if( distLL > limit_distance){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because distance " << distLL << " is larger than limit " << limit_distance << std::endl;
            return false;
	  }

          // connection must be within same block
          if( fabs( last_node().c().block() - seq.last_node().c().block()) > 0 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_decay_helix_vertex() && (decay_helix_vertex_type() == "calo" || decay_helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has decay helix_vertex on calo or foil " << std::endl;
            return false;
          }
          if( seq.has_decay_helix_vertex() && (seq.decay_helix_vertex_type() == "calo" || seq.decay_helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has decay helix_vertex on calo or foil " << std::endl;
            return false;
          }

        }
        else if( distFL <= distFF && distFL <= distLL && distFL <= distLF ){ // first to last  LF -> LF
	  if( distFL > limit_distance){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because distance " << distFL << " is larger than limit " << limit_distance << std::endl;
            return false;
	  }

          invertA = true;
          invertB = true;
          // connection must be within same block
          if( fabs( nodes_[0].c().block() - seq.last_node().c().block()) > 0 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_helix_vertex() && (helix_vertex_type() == "calo" || helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has helix_vertex on calo or foil " << std::endl;
            return false;
          }
          if( seq.has_decay_helix_vertex() && (seq.decay_helix_vertex_type() == "calo" || seq.decay_helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has decay helix_vertex on calo or foil " << std::endl;
            return false;
          }

        }
        else{ // first to first  LF -> FL
	  if( distFF > limit_distance){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because distance " << distFF << " is larger than limit " << limit_distance << std::endl;
            return false;
	  }

          invertA = true;
          invertB = false;
          // connection must be within same block
          if( fabs( nodes_[0].c().block() - seq.nodes_[0].c().block()) > 0 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because blocks are far away " << std::endl;
            return false;
          }

          if( has_helix_vertex() && (helix_vertex_type() == "calo" || helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 1st track already has helix_vertex on calo or foil " << std::endl;
            return false;
          }
          if( seq.has_helix_vertex() && (seq.helix_vertex_type() == "calo" || seq.helix_vertex_type() == "foil") ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " ... forbidden, because 2nd track already has helix_vertex on calo or foil " << std::endl;
            return false;
          }

        }




        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " ... good kink match, distances: FF " << distFF << " FL " << distFL << " LF " << distLF << " LL " << distLL << " so invertA " << invertA << " invertB " << invertB << std::endl;
        }


        return true;

      }


      bool same_families(topology::sequence s)const{

        std::vector<size_t> fA = families();
        std::vector<size_t> fB = s.families();

        for(std::vector<size_t>::iterator ifA=fA.begin(); ifA!=fA.end(); ++ifA)
          for(std::vector<size_t>::iterator ifB=fB.begin(); ifB!=fB.end(); ++ifB)
            if( *ifA == *ifB )
              return true;

        return false;
      }

      double delta_phi(experimental_point epa, experimental_point epb)const{
        return helix_.delta_phi(epa, epb);
      }


      // is the (true) track all on one side of the foil?
      bool one_side()const{
        int old_block = 0;
        for(std::vector<node>::const_iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode){
          if( inode - nodes_.begin() == 0 ){
            old_block = inode->c().block();
            continue;
          }

          if( inode->c().block()*old_block < 0 ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " old block: " << old_block << " new block: " << inode->c().block() << " the track is not all on one side " << std::endl;
            return false;
          }
          old_block = inode->c().block();
        }

        return true;

      }


      experimental_vector initial_dir()const{
        if( has_helix_vertex() ){
          if( nodes_.size() < 1 ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: asking for initial dir of sequence with " << nodes_.size() << " nodes " << std::endl;
              return experimental_vector();
            }
          }
          return experimental_vector(helix_vertex_, nodes_[0].ep()).unit();
        }
        if( nodes_.size() < 2 ){
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: asking for initial dir of sequence with " << nodes_.size() << " nodes " << std::endl;
            return experimental_vector();
          }
        }

        return experimental_vector(nodes_[0].ep(), nodes_[1].ep()).unit();

      }

      experimental_vector final_dir()const{
        if( has_decay_helix_vertex() ){
          if( nodes_.size() < 1 ){
            if( print_level() >= mybhep::NORMAL ){
              std::clog << " problem: asking for final dir of sequence with " << nodes_.size() << " nodes " << std::endl;
              return experimental_vector();
            }
          }
          return experimental_vector(nodes_.back().ep(), decay_helix_vertex_).unit();
        }
        if( nodes_.size() < 2 ){
          if( print_level() >= mybhep::NORMAL ){
            std::clog << " problem: asking for final dir of sequence with " << nodes_.size() << " nodes " << std::endl;
            return experimental_vector();
          }
        }

        size_t s = nodes_.size();
        return experimental_vector(nodes_[s-2].ep(), nodes_.back().ep()).unit();

      }


      //! get node of worst chi2
      bool worst_node(topology::node& n)const{

        if( nodes_.size() <= 2 ) return false;

        int index = -1;

        double chi2max = mybhep::default_max;
        for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in){
          if( in->chi2() > chi2max ){
            chi2max = in->chi2();
            index = in - nodes_.begin();
          }
        }

        if( index >= 0 ){
          n = nodes_[index];
          return  true;
        }

        return false;
      }


      // get node of largest horizontal kink
      bool largest_kink_node(topology::node& n, double& phi)const{

        if( nodes_.size() <= 2 ) return false;

        int nindex = -1;

        double phimax = mybhep::default_max;
        for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in){
          size_t index = in - nodes_.begin();
          if( index == 0 ) continue;
          if( index == nodes_.size() - 1 ) continue;

          double local_phi = fabs((joint(nodes_[index - 1].ep(), nodes_[index].ep(), nodes_[index + 1].ep())).kink_phi().value());

          if( local_phi > phimax ){
            phimax = local_phi;
            nindex = index;
          }
        }

        if( nindex >= 0 ){
          n = nodes_[nindex];
          phi = phimax;
          return  true;
        }

        return false;
      }

      bool helix_out_of_range(double lim){

	experimental_double phi(0.,0.);
	double phi_ref =0.;
	experimental_point p;
        for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in){
	  phi_ref = phi.value();
	  phi=helix_.phi_of_point(in->ep(),phi_ref);
	  p=helix_.position(phi);

	  if( p.y().value() < - lim ||
	      p.y().value() > lim ){
	    std::clog << " problem: helix out of range " << std::endl;
	    std::clog << " phi_ref " << phi_ref << " helix_position: ";
	    p.dump();
	    std::clog << " from point ";
	    in->ep().dump();
	    return true;
	  }
	}

	return true;

      }

    };

  }


}

#endif
