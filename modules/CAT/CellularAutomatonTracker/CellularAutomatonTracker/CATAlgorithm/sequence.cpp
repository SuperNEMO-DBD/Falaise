/* -*- mode: c++ -*- */
#include <CATAlgorithm/sequence.h>

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

#include <sys/time.h> 

namespace CAT {
  namespace topology{

    //!Default constructor
    sequence::sequence()
    {
      appname_= "sequence: ";
      nodes_.clear();
      free_ = false;
      names_.clear();names_.push_back("default");
      has_charge_ = false;
      has_helix_charge_ = false;
      has_detailed_charge_ = false;
      has_tangent_length_ = false;
      has_helix_length_ = false;
      has_momentum_ = false;
      has_helix_ = false;
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

      chi2s_all_.clear();
      helix_chi2s_.clear();
      probs_all_.clear();
      helix_ = helix();
      charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      helix_charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      detailed_charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      tangent_length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      helix_length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      momentum_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                      mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
      primary_ = true;
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
      free_ = false;
      names_.clear();
      names_.push_back("default");
      has_charge_ = false;
      has_helix_charge_ = false;
      has_detailed_charge_ = false;
      has_tangent_length_ = false;
      has_helix_length_ = false;
      has_momentum_ = false;
      has_helix_ = false;
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
      chi2s_all_.clear();
      helix_chi2s_.clear();
      probs_all_.clear();
      helix_ = helix();
      charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      helix_charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      detailed_charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      tangent_length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      helix_length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      momentum_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                      mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
      primary_ = true;
    }

    //! constructor from single node
    sequence::sequence(const node &node, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "sequence: ";
      //node.set_free(false);
      nodes_.clear();
      nodes_.push_back(node);
      nodes_.back ().set_free(false);
      free_ = true;
      names_.clear();names_.push_back("default");
      has_charge_ = false;
      has_helix_charge_ = false;
      has_detailed_charge_ = false;
      has_tangent_length_ = false;
      has_helix_length_ = false;
      has_momentum_ = false;
      has_helix_ = false;
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
      chi2s_all_.clear();
      helix_chi2s_.clear();
      probs_all_.clear();
      helix_ = helix();
      charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      helix_charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      detailed_charge_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      tangent_length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      helix_length_ = experimental_double(mybhep::small_neg, mybhep::small_neg);
      momentum_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                      mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
      primary_ = true;
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
      a_out << indent << names_[0] << ". Number of nodes: " << nodes().size() << " free: " << Free() << " chi2 " << chi2() << " helix chi2 " << helix_chi2() << " prob " << Prob() << " helix prob " << helix_Prob() << " momentum " << momentum().length().value() << " primary " << primary() << std::endl;
      for(std::vector<node>::const_iterator inode=nodes_.begin(); inode!=nodes_.end(); ++inode)
        inode->dump(a_out, "",indent + "     ");
      if( has_helix_vertex() ){
        a_out << indent << " helix_vertex type: " << helix_vertex_type() << " helix_vertex: "; helix_vertex_.dump();
	if( helix_vertex_type() == "calo" ) a_out << " icalo " << helix_vertex_id();
      }
      if( has_decay_helix_vertex() ){
        a_out << indent << " decay helix_vertex type " << decay_helix_vertex_type()  << " decay_helix_vertex : "; decay_helix_vertex_.dump();
	if( decay_helix_vertex_type() == "calo" ) a_out << " icalo " << calo_helix_id();
      }
      if( has_tangent_vertex() ){
        a_out << indent << " tangent_vertex type: " << tangent_vertex_type() << " tangent_vertex: "; tangent_vertex_.dump();
	if( tangent_vertex_type() == "calo" ) a_out << " icalo " << tangent_vertex_id();
      }
      if( has_decay_tangent_vertex() ){
        a_out << indent << " decay tangent_vertex type " << decay_tangent_vertex_type() << " decay_tangent_vertex : "; decay_tangent_vertex_.dump();
	if( decay_tangent_vertex_type() == "calo" ) a_out << " icalo " << calo_tangent_id();
      }
      a_out << indent << " ------------------- " << std::endl;

      return;
    }

    //! set nodes
    void sequence::set_nodes(const std::vector<node> &nodes){
      nodes_ = nodes;
    }

    //! set free level
    void sequence::set_free(bool free){
      free_ = free;
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

    //! set helix_vertex
    void sequence::set_helix_vertex(const experimental_point &v, const std::string &type, size_t calo_helix_id){
      has_helix_vertex_ = true;
      helix_vertex_ = v;
      helix_vertex_type_ = type;
      if( type=="calo" )
        helix_vertex_id_ = calo_helix_id;
    }

    //! set decay_helix_vertex
    void sequence::set_decay_helix_vertex(const experimental_point &v, const std::string &type, size_t calo_helix_id){
      has_decay_helix_vertex_ = true;
      decay_helix_vertex_ = v;
      decay_helix_vertex_type_ = type;
      if( type=="calo" )
        calo_helix_id_ = calo_helix_id;
    }

    //! set tangent_vertex
    void sequence::set_tangent_vertex(const experimental_point &v,  const std::string &type, size_t tangent_vertex_id){
      has_tangent_vertex_ = true;
      tangent_vertex_ = v;
      tangent_vertex_type_ = type;
      if( type=="calo" )
        tangent_vertex_id_ = tangent_vertex_id;
    }

    //! set decay_tangent_vertex
    void sequence::set_decay_tangent_vertex(const experimental_point & v, const std::string  &type, size_t calo_tangent_id){
      has_decay_tangent_vertex_ = true;
      decay_tangent_vertex_ = v;
      decay_tangent_vertex_type_ = type;
      if( type=="calo" )
        calo_tangent_id_ = calo_tangent_id;
    }

    //! set chi2 list
    void sequence::set_chi2s(const std::vector<double> & chi2s){
      chi2s_all_ = chi2s;
    }

    //! set helix_chi2 list
    void sequence::set_helix_chi2s(const std::vector<double> &helix_chi2s){
      helix_chi2s_ = helix_chi2s;
    }

    //! set prob list
    void sequence::set_probs(const std::vector<double> &probs){
      probs_all_ = probs;
    }


    //! set charge
    void sequence::set_charge(const experimental_double &charge){
      has_charge_ = true;
      charge_ = charge;
    }

    //! set helix_charge
    void sequence::set_helix_charge(const experimental_double &helix_charge){
      has_helix_charge_ = true;
      helix_charge_ = helix_charge;
    }

    //! set detailed_charge
    void sequence::set_detailed_charge(const experimental_double &detailed_charge){
      has_detailed_charge_ = true;
      detailed_charge_ = detailed_charge;
    }

    //! set momentum
    void sequence::set_momentum(const experimental_vector &mom){
      has_momentum_ = true;
      momentum_ = mom;
    }

    //! set tangent length
    void sequence::set_tangent_length(const experimental_double &mom){
      has_tangent_length_ = true;
      tangent_length_ = mom;
    }

    //! set helix length
    void sequence::set_helix_length(const experimental_double &mom){
      has_helix_length_ = true;
      helix_length_ = mom;
    }

    //! set primary
    void sequence::set_primary(bool primary){
      primary_ = primary;
    }

    //! set helix
    void sequence::set_helix(const  helix & h){
      helix_ = h;
    }


    //! get nodes
    const std::vector<node> & sequence::nodes()const{
      return nodes_;
    }

    //! get free level
    bool sequence::Free()const{
      return free_;
    }

    //! get name
    const std::string & sequence::name()const{
      return names_[0];
    }

    //! get names
    const std::vector<std::string> & sequence::names()const{
      return names_;
    }

    //! get primary
    bool sequence::primary()const{
      return primary_;
    }

    //! get list of all chi2
    const std::vector<double>& sequence::chi2s_all() const
    {
      return chi2s_all_;
    }

    //! get list of prob
    const std::vector<double>& sequence::probs_all() const
    {
      return probs_all_;
    }

    //! get list of chi2 used in the sequence
    std::vector<double> sequence::chi2s()const
    {
      std::vector<double> c;

      for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in)
        c.push_back(in->chi2());

      return c;
    }

    //! get list of helix_chi2
    const std::vector<double> & sequence::helix_chi2s() const
    {

      return helix_chi2s_;
    }

    //! get list of probs used in the sequence
    std::vector<double> sequence::probs()const
    {
      std::vector<double> p;

      for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in)
        p.push_back(in->Prob());

      return p;
    }

    // get chi2 of sequence
    double sequence::chi2()const{
      double chi2 = 0.;
      std::vector<double> cs = chi2s();
      for(std::vector<double>::const_iterator ichi = cs.begin(); ichi != cs.end(); ++ichi){
        chi2 += *ichi;
      }

      return chi2;

    }

    // get helix_chi2 of sequence
    double sequence::helix_chi2()const{
      double helix_chi2 = 0.;
      std::vector<double> cs = helix_chi2s();
      for(std::vector<double>::const_iterator ichi = cs.begin(); ichi != cs.end(); ++ichi){
        helix_chi2 += *ichi;
      }

      return helix_chi2;

    }

    // get prob of sequence
    double sequence::Prob()const{
      return probof(chi2(), ndof());

    }

    // get ndof of sequence
    int32_t sequence::ndof()const{

      int32_t ndof = 0;
      for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in)
        ndof += in->ndof();
      return ndof;

    }

    // get prob of sequence
    double sequence::helix_Prob()const{
      double c = helix_chi2();
      // for each triplet's point, three coordinates
      std::vector<double> cs = helix_chi2s();
      int32_t ndof = 0;
      if( cs.size() > 2 )
        ndof += 3*(cs.size() - 2)*3;

      return probof(c, ndof);

    }

    //! has momentum
    bool sequence::has_momentum()const{
      return has_momentum_;
    }

    //! has helix
    bool sequence::has_helix()const{
      return has_helix_;
    }

    //! has charge
    bool sequence::has_charge()const{
      return has_charge_;
    }

    //! has helix_charge
    bool sequence::has_helix_charge()const{
      return has_helix_charge_;
    }

    //! has detailed_charge
    bool sequence::has_detailed_charge()const{
      return has_detailed_charge_;
    }

    //! has tangent length
    bool sequence::has_tangent_length()const{
      return has_tangent_length_;
    }

    //! has helix length
    bool sequence::has_helix_length()const{
      return has_helix_length_;
    }

    //! has helix_vertex
    bool sequence::has_helix_vertex()const{
      return has_helix_vertex_;
    }

    //! get helix_vertex
    const experimental_point &  sequence::helix_vertex()const{
      return helix_vertex_;
    }

    //! get helix_vertex type
    const std::string & sequence::helix_vertex_type()const{
      return helix_vertex_type_;
    }

    //! get helix_vertex id
    size_t sequence::helix_vertex_id()const{
      return helix_vertex_id_;
    }

    //! has decay_helix_vertex
    bool sequence::has_decay_helix_vertex()const{
      return has_decay_helix_vertex_;
    }

    //! get decay_helix_vertex
    const experimental_point & sequence::decay_helix_vertex()const{
      return decay_helix_vertex_;
    }

    //! get calo id
    size_t sequence::calo_helix_id()const{
      return calo_helix_id_;
    }

    //! get decay helix_vertex type
    const std::string & sequence::decay_helix_vertex_type()const{
      return decay_helix_vertex_type_;
    }

    //! has tangent_vertex
    bool sequence::has_tangent_vertex()const{
      return has_tangent_vertex_;
    }

    //! get tangent_vertex
    const experimental_point & sequence::tangent_vertex()const{
      return tangent_vertex_;
    }

    //! get tangent_vertex type
    const std::string & sequence::tangent_vertex_type()const{
      return tangent_vertex_type_;
    }

    //! get tangent_vertex id
    size_t sequence:: tangent_vertex_id()const{
      return tangent_vertex_id_;
    }

    //! has decay_tangent_vertex
    bool sequence:: has_decay_tangent_vertex()const{
      return has_decay_tangent_vertex_;
    }

    //! get decay_tangent_vertex
    const experimental_point & sequence::decay_tangent_vertex()const{
      return decay_tangent_vertex_;
    }

    //! get calo id
    size_t sequence::calo_tangent_id()const{
      return calo_tangent_id_;
    }

    //! get decay tangent_vertex type
    const std::string & sequence::decay_tangent_vertex_type()const{
      return decay_tangent_vertex_type_;
    }

    //! get charge
    const experimental_double & sequence::charge() const
    {
      return charge_;
    }

    //! get helix_charge
    const experimental_double & sequence::helix_charge() const
    {
      return helix_charge_;
    }

    //! get detailed_charge
    const experimental_double & sequence::detailed_charge() const
    {
      return detailed_charge_;
    }

    //! get momentum
    const experimental_vector & sequence::momentum() const
    {
      return momentum_;
    }

    //! get tangent_length
    const experimental_double & sequence::tangent_length() const
    {
      return tangent_length_;
    }

    //! get helix_length
    const experimental_double & sequence::helix_length() const
    {
      return helix_length_;
    }

    //! get helix
    const helix & sequence::get_helix() const
    {
      return helix_;
    }


    //! get fast
    bool sequence::fast()const
    {
      if( !nodes_.empty())
        return nodes_[0].c().fast();

      return true;
    }


    //! get last node
    node sequence::last_node()const
    {
      size_t s = nodes().size();

      if( s < 1 ){
        std::clog << " problem: request of last node from sequence of size " << s << std::endl;
        topology::node null;
        return null;
      }

      return nodes()[s-1];
    }


    //! get second last node
    node sequence::second_last_node()const
    {
      size_t s = nodes().size();

      if( s < 2 ){
        std::clog << " problem: request of second last node from sequence of size " << s << std::endl;
        topology::node null;
        return null;
      }

      return nodes()[s-2];
    }


    //! get middle node
    node sequence::middle_node()const{
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


    bool sequence::has_cell(const cell & c)const{
      if(std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end())

        return true;

      return false;
    }


    sequence sequence::invert(){
      sequence inverted;
      inverted.set_print_level(print_level());
      inverted.set_probmin(probmin());
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

      if( has_momentum() ){
        experimental_vector newmom(-momentum().x(), -momentum().y(), -momentum().z());
        inverted.set_momentum(newmom);
      }

      if( has_charge() ){
        experimental_double newcharge = - charge();
        inverted.set_charge(newcharge);
      }

      if( has_helix_charge() ){
        experimental_double newcharge = - helix_charge();
        inverted.set_helix_charge(newcharge);
      }

      if( has_detailed_charge() ){
        experimental_double newcharge = - detailed_charge();
        inverted.set_detailed_charge(newcharge);
      }

      if( has_tangent_length() ){
        inverted.set_tangent_length(tangent_length());
      }

      if( has_helix_length() ){
        inverted.set_helix_length(helix_length());
      }

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

    void sequence::set_free_level(){

      set_free(false);
      size_t iccc;
      for(std::vector<node>::iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode)
        {
          inode->set_free(false);

          for(std::vector<cell>::iterator ilink=(*inode).links_.begin(); ilink != (*inode).links_.end(); ++ilink){
            ilink->set_free(false);
            iccc = get_link_index_of_cell(inode - nodes_.begin(), *ilink);
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
    void sequence::last_free_node(size_t *lfn, size_t *link){
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


    bool sequence::contained(const topology::sequence & big)const{
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


    bool sequence::contained_same_size_and_cells(const topology::sequence & big)const{

      if( !contained_same_extreme_quadrants(big) ) return false;

      double thisprob = Prob();
      double bigprob = big.Prob();

      return (thisprob < bigprob);

    }

    bool sequence::contained_same_extreme_quadrants(const topology::sequence & big)const{

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

      return true;

    }


    bool sequence::is_bridge(const topology::sequence & first, const topology::sequence & second)const{

      size_t s = nodes().size();

      if( first.has_cell(nodes()[0].c()) &&
          second.has_cell(nodes()[s-1].c() ) )
        return true;

      if( second.has_cell(nodes()[0].c()) &&
          first.has_cell(nodes()[s-1].c() ) )
        return true;

      return false;

    }



    void sequence::increase_iteration(size_t inode, size_t ilink){
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


    sequence sequence::copy_to_last_free_node(size_t *ilfn, size_t *ilink){

      sequence newsequence;
      newsequence.set_print_level(print_level());
      newsequence.set_probmin(probmin());

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

	  std::vector<cell_couplet>::iterator icc = newsequence.nodes_[i].cc_.begin();
	  while( icc != newsequence.nodes_[i].cc_.end() ){
	    if( i < lfn || (i == lfn && (size_t)(icc - newsequence.nodes_[i].cc_.begin()) < link ) ){
	      icc->set_free( false);
	      icc->set_begun( true);
	      icc->set_all_used();
	    }
	    ++icc;
	    continue;
	  }
	  std::vector<cell_triplet>::iterator iccc = newsequence.nodes_[i].ccc_.begin();
	  while( iccc != newsequence.nodes_[i].ccc_.end() ){
	    if( i < lfn  || (i == lfn && (size_t)(iccc - newsequence.nodes_[i].ccc_.begin()) < link ) ){
	      iccc->set_free( false);
	      iccc->set_begun( true);
	      iccc->set_all_used();
	    }
	    ++iccc;
	    continue;
	  }

	  std::vector<cell>::iterator ilink = newsequence.nodes_[i].links_.begin();
	  while( ilink != newsequence.nodes_[i].links_.end() ){
	    if( ilink - newsequence.nodes_[i].links_.begin() >= newsequence.nodes_[i].links_.size() ){
	      break;
	    }

	    if( i < lfn  || (i == lfn && (size_t)(ilink - newsequence.nodes_[i].links_.begin()) < link ) ){
                ilink->set_free( false);
                ilink->set_begun( true);
              }
	    if( i == lfn && (size_t)(ilink - newsequence.nodes_[i].links_.begin()) > link ){
	      if( print_level() >= mybhep::VVERBOSE )
		std::clog << " removing from node " << newsequence.nodes_[i].c().id()  << "  link " << ilink - newsequence.nodes_[i].links_.begin() << " id " << ilink->id() << " larger than link " << link << " from copied sequence " << std::endl;
		newsequence.nodes_[i].remove_link(ilink - newsequence.nodes_[i].links_.begin());
	    }
	    ++ilink;
	    continue;
	  }
	}

      //      if( nodes()[lfn].cc()[link].begun() )
      increase_iteration(lfn,link);

      if( lfn < 1 )
        newsequence.nodes_[lfn].cc_[link].set_free( true);
      else
        newsequence.nodes_[lfn].ccc_[link].set_free( true);

      if( ( lfn < 1 && !nodes()[lfn].cc()[link].begun() ) ||
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


    void sequence::fill_links_of_node(size_t inode, cluster local_sequence){

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



    bool sequence::pick_new_cell(size_t *ilink, experimental_point *newp, cluster local_cluster){

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

      size_t iccc, iteration;
      std::vector<joint>::iterator ijoint;
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
            }else{

	      iccc = get_link_index_of_cell(s-1, *itlink);
	      
              iteration = nodes_[s-1].ccc_[iccc].iteration();

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

                ijoint = nodes_[s-1].ccc_[iccc].joints_.begin();

                while( ijoint != nodes_[s-1].ccc_[iccc].joints_.end() ){

                  if( nodes_[s-1].ccc_[iccc].ca().id() == itlink->id() ){
                    *ijoint = ijoint->invert();
                  }

                  if( !compatible(& (*ijoint) , *itlink) ){
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
              if( print_level() >= mybhep::VVERBOSE ){
		nodes_[s-1].ccc_[iccc].dump_joint(j);
              }

	      if( s >= 3 ){ // recalculate chi2 of node A
		double chi2_change_A, chi2_change_alpha;
		get_chi2_change_for_changing_end_of_sequence(j.epa(), j.epb(), &chi2_change_A, &chi2_change_alpha);
		if( print_level() >= mybhep::VVERBOSE ){
		  std::clog << " will change for cell A " << nodes_[s-2].c().id() << " chi2 from " << nodes_[s-2].chi2() << " to " << nodes_[s-2].chi2() + chi2_change_A << std::endl; fflush(stdout);
		}

		nodes_[s-2].set_chi2(nodes_[s-2].chi2() + chi2_change_A);
		if( s >= 4 ){ // recalculate chi2 of node alpha
		  if( print_level() >= mybhep::VVERBOSE ){
		    std::clog << " will change for cell alpha " << nodes_[s-3].c().id() << " chi2 from " << nodes_[s-3].chi2() << " to " << nodes_[s-3].chi2() + chi2_change_alpha << std::endl; fflush(stdout);
		  }
		  nodes_[s-3].set_chi2(nodes_[s-3].chi2() + chi2_change_alpha);
		}
	      }

              nodes_[s-2].set_ep(j.epa());
              nodes_[s-1].set_ep(j.epb());
              nodes_[s-1].set_chi2(j.chi2());
              nodes_[s-1].set_ndof(j.ndof());
              *newp = j.epc();
              ok = true;
              break;

            }


          }
      }



      return ok;
    }

    bool sequence::compatible(joint *j, cell cc){
      // sequence: [ ... alpha A B ]    C 
      // C = cc is the proposed new cell

      size_t s = nodes().size();
      if( s < 2 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: checking compatibility for sequence of size " << s << std::endl;
        }
        return false;
      }

      int32_t ndof = 2;  // 1 or 2 kink angles and 0, 1 or 2 separation angles

      topology::experimental_double local_separation_a, local_separation_b;
      double chi2 = 0.;
      double chi2_change_A = 0.;
      double chi2_change_alpha = 0.;
      double chi2_separation_a = 0.;
      double chi2_separation_b = 0.;

      bool use_theta_kink = true;

      topology::experimental_point pa = j->epa();
      topology::experimental_point pb = j->epb();
      topology::cell ca = second_last_node().c();
      topology::cell cb = last_node().c();
      
      if(ca.unknown_vertical() || cb.unknown_vertical() || cc.unknown_vertical()){
	use_theta_kink = false;
	ndof --;
      }
      
      if( !ca.small() && !ca.same_quadrant(second_last_node().ep(), j->epa() ) ){
	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << " connecting cell " << last_node().c().id() << " with this joint is incompatible: points on cell " << ca.id() << " on different quadrants " << std::endl;
	}
	return false;
      }

      if( !cb.small() && !cb.same_quadrant(last_node().ep(), j->epb() ) ){
	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << " connecting cell " << last_node().c().id() << " with this joint is incompatible: points on cell " << cb.id() << " on different quadrants " << std::endl;
	}
	return false;
      }

      if( s > 2 ){
	if( !ca.small() && !ca.intersect(cb) ){
	  pa = ca.angular_average(second_last_node().ep(), j->epa(), &local_separation_a);
	  chi2_separation_a = mybhep::square(local_separation_a.value()/local_separation_a.error());
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " separation chi2 for cell " << ca.id() << " is " << chi2_separation_a << std::endl; fflush(stdout);
	  }
	  ndof ++;
	}
	
	if( !cb.small() && !cb.intersect(ca) && !cb.intersect(cc) ){
	  pb = cb.angular_average(last_node().ep(), j->epb(), &local_separation_b);
	  chi2_separation_b = mybhep::square(local_separation_b.value()/local_separation_b.error());
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " separation chi2 for cell " << cb.id() << " is " << chi2_separation_b << std::endl; fflush(stdout);
	  }
	  ndof ++;
	}
	
	experimental_vector alpha_A = experimental_vector(nodes_[s-3].ep(), second_last_node().ep());
	experimental_vector B_C = experimental_vector(last_node().ep(), j->epc());
	experimental_double phi_kink = alpha_A.kink_phi(B_C);
	if( fabs(phi_kink.value())*180./M_PI > 90. ){
	  if( print_level() >= mybhep::VVERBOSE )
	    std::clog << " incompatible connection: kink between alpha_A and B_C is " << phi_kink.value()*180./M_PI << std::endl;
	  return false;
	}


	if( s >= 3 ){ // we are changing points A and B, affecting the chi2 of connections alpha0-alpha-A and alpha-A-B
	  get_chi2_change_for_changing_end_of_sequence(pa, pb, &chi2_change_A, &chi2_change_alpha);
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " connecting cell " << cb.id() << " to " << cc.id() << " changes chi2 of cell A, i.e. : " << nodes_[s-2].c().id() << " by " << chi2_change_A << std::endl; fflush(stdout);
	    if( s >= 4 )
	      std::clog << " connecting cell " << cb.id() << " to " << cc.id() << " changes chi2 of cell alpha, i.e." << nodes_[s-3].c().id() << " by " << chi2_change_alpha  << std::endl; fflush(stdout);
	  }
	}
	
	
      }
      

      topology::line l1(pa, pb, print_level(), probmin());
      topology::line l2(pb, j->epc(), print_level(), probmin());

      double chi2_just_phi;
      double chi2_kink = l1.chi2(l2, use_theta_kink, &chi2_just_phi);

      chi2 = chi2_separation_a + chi2_separation_b + chi2_kink;

      double local_prob = probof(chi2, ndof);
      double prob_just_phi = probof(chi2_just_phi, 1);
      double net_local_prob;
      double net_chi2 = chi2 + chi2_change_A + chi2_change_alpha;
      if( net_chi2 <= 0 ){
	net_local_prob = 1.;
      } else {
	net_local_prob = probof(net_chi2, ndof);
      }

      j->set_chi2(chi2);
      j->set_ndof(ndof);
      j->set_p(local_prob);
      chi2s_all_.push_back(chi2);
      probs_all_.push_back(local_prob);


      if( net_local_prob > probmin() && prob_just_phi > probmin() ){

        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " connecting cell " << last_node().c().id() << " is compatible with chi2 " << chi2 << " prob " << local_prob << " net prob " << net_local_prob << std::endl; fflush(stdout);
        }

        j->set_epa(pa);
        j->set_epb(pb);

        return true;

      }

      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " connecting cell " << last_node().c().id() << " with this joint is incompatible: ";
        if( s > 2 )
          {
            std::clog << " separation a: " << local_separation_a.value() << " +- " << local_separation_a.error();
            std::clog << " separation b: " << local_separation_b.value() << " +- " << local_separation_b.error()
                      << " total connection chi2: " << chi2 << " prob " << local_prob << std::endl; fflush(stdout);
          }
      }

      return false;
    }


      void sequence::get_chi2_change_for_changing_end_of_sequence(const topology::experimental_point &new_pa, const topology::experimental_point &new_pb, double *delta_chi_A, double *delta_chi_alpha){
      // sequence: [ ... alpha0 alpha A B ]
      size_t s = nodes_.size();
      if( s < 3 ){
        if( print_level() >= mybhep::NORMAL )
          std::clog << " problem: ask prob change for second last node but nnodes is " << s << std::endl;
        return;
      }

      double tolerance = 0.000000001;

      topology::experimental_point palpha = nodes_[s-3].ep();
      topology::experimental_point pa = second_last_node().ep();
      topology::experimental_point pb = last_node().ep();

      topology::line l_alpha_A(palpha, pa, print_level(), probmin());
      topology::line l_A_B(pa, pb, print_level(), probmin());
      bool use_theta_kink_alpha_A_B = !(nodes_[s-3].c().unknown_vertical() || nodes_[s-2].c().unknown_vertical() || nodes_[s-1].c().unknown_vertical() );

      double chi2_just_phi;
      double old_chi2 = l_alpha_A.chi2(l_A_B, use_theta_kink_alpha_A_B, &chi2_just_phi);
      double old_chi2_check = second_last_node().chi2();
      if( old_chi2 > old_chi2_check ){
	if( fabs(old_chi2 - old_chi2_check) > tolerance ){
	  if( print_level() >= mybhep::NORMAL ){
	    std::clog << " problem: cell A " << second_last_node().c().id() << " has old chi2 " << old_chi2_check << " but just kink component is " << old_chi2 << " delta " <<  old_chi2 - old_chi2_check << std::endl;
	  }
	}
	old_chi2 = old_chi2_check;
      }

      topology::line new_l_alpha_A(palpha, new_pa, print_level(), probmin());
      topology::line new_l_A_B(new_pa, new_pb, print_level(), probmin());

      double new_chi2 = new_l_alpha_A.chi2(new_l_A_B, use_theta_kink_alpha_A_B, &chi2_just_phi);
      *delta_chi_A = new_chi2 - old_chi2;
      if( *delta_chi_A + old_chi2 <= 0 ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: node A has old chi2 " << old_chi2 << " new " << new_chi2 << " delta " << *delta_chi_A << std::endl;
	}
	*delta_chi_A = 0.;
      }

      if( s >= 4 ){
	bool use_theta_kink_alpha0_alpha_A = !(nodes_[s-4].c().unknown_vertical() || nodes_[s-3].c().unknown_vertical() || nodes_[s-2].c().unknown_vertical() );
	topology::experimental_point palpha0 = nodes_[s-4].ep();
	topology::line l_alpha0_alpha(palpha0, palpha, print_level(), probmin());
	old_chi2 = l_alpha0_alpha.chi2(l_alpha_A, use_theta_kink_alpha0_alpha_A, &chi2_just_phi);
	old_chi2_check = nodes_[s-3].chi2();
	if( old_chi2 > old_chi2_check ){
	  if( fabs(old_chi2 - old_chi2_check) > tolerance ){
	    if( print_level() >= mybhep::NORMAL ){
	      std::clog << " problem: cell alpha " << nodes_[s-3].c().id() << " has old chi2 " << old_chi2_check << " but just kink component is " << old_chi2 << " delta " <<  old_chi2 - old_chi2_check <<  std::endl;
	    }
	  }
	  old_chi2 = old_chi2_check;
	}

	new_chi2 = l_alpha0_alpha.chi2(new_l_alpha_A, use_theta_kink_alpha0_alpha_A, &chi2_just_phi);
	*delta_chi_alpha = new_chi2 - old_chi2;
	if( old_chi2 + *delta_chi_alpha <= 0. ){
	  if( print_level() >= mybhep::NORMAL )
	    std::clog << " problem: node alpha has old chi2 " << old_chi2 << " new " << new_chi2  << " delta " << *delta_chi_alpha << std::endl;
	  *delta_chi_alpha = 0.;
	}

      }

      return;

    }


    int sequence::get_link_index_of_cell(size_t inode, const cell & link) const {
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

#if 0
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
#else
	
	size_t index;
	if( !nodes()[inode].has_couplet(link,&index) ){
	  if( print_level() >= mybhep::NORMAL )
	    std::clog << " problem: looking for couplet from cell index " << inode << " to " << link.id() << " but it's not there " << std::endl;
	  return 0;
	}

	return index;

#endif
      }


#if 0
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
#else

      size_t index;
      if( !nodes()[inode].has_triplet(link,nodes_[inode-1].c(),&index) ){
	if( print_level() >= mybhep::NORMAL )
	  std::clog << " problem: looking for triplet from cell index " << inode << " id " << nodes()[inode].c().id()  << " to cell id " << link.id() << " but it's not there " << std::endl; fflush(stdout);
	return 0;
      }

      return index;
	
#endif

    }


    bool sequence::calculate_helix(void) {

      bool good_fit=true;

      has_helix_ = true;

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
	std::vector<node>::iterator anode, cnode;
        for(std::vector<node>::iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode){
          if( inode - nodes_.begin() == 0 || nodes_.end() - inode == 1 ) continue;

          anode = inode;
          cnode = inode;
          --anode; ++cnode;
          if( print_level() >= mybhep::VVERBOSE ){
            std::clog << " calculate helix for three points " << anode->c().id() << " , " << inode->c().id() << " , " << cnode->c().id() << std::endl;
          }
          helices.push_back(three_points_helix(anode->ep(), inode->ep(), cnode->ep()));

        }

        helix_ = average(helices);
        helix_.set_print_level(print_level());
        helix_.set_probmin(probmin());

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

        CircleRegression cl(xs, zs, print_level(), probmin());
        if( !cl.fit() ) good_fit = false;
        //      cl.minuit_fit();

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



        LinearRegression l(phis, ys, print_level(), probmin());

#if CAT_WITH_DEVEL_ROOT == 1
        if ( !l.root_fit() ){  // fit with root
#endif // CAT_WITH_DEVEL_ROOT == 1

          // if root does not work or is not used
          // fit best helix with through all points (vertical view)
          l.set_xi(ys);
          l.set_yi(phis);
          l.fit();
          l.invert(); // fit with y as more erroneous variable (phi = phi(y)),
          // then invert the result to have y = y(phi)

#if CAT_WITH_DEVEL_ROOT == 1
         }
#endif // CAT_WITH_DEVEL_ROOT == 1

        if( print_level() >= mybhep::VVERBOSE ){
          l.dump();
        }


        // build helix
        ci.set_center(experimental_point(ci.center().x(), l.y0(), ci.center().z()));
        helix_ = helix(ci, l.tangent(), print_level(), probmin());



        helix_chi2s_ = helix_.chi2s(ps);

        break;
      }
      default:

        if( print_level() >= mybhep::NORMAL ){
          std::clog << " problem: unknonw method "<< method << " to fit a helix " << std::endl;
        }
      }


      return good_fit;
    }


    const experimental_double & sequence::radius()const{
      return helix_.radius();
    }

    const experimental_double & sequence::pitch()const{
      return helix_.pitch();
    }

    const experimental_point & sequence::center()const{
      return helix_.center();
    }

    void sequence::calculate_momentum(double bfield){

      experimental_double mom = experimental_sqrt(experimental_square(radius()) + experimental_square(pitch()))*0.3*bfield;


      momentum_=mom*initial_dir();

      if( !isnan(momentum_.x().value()) &&
          !isnan(momentum_.y().value()) &&
          !isnan(momentum_.z().value()) ){
        has_momentum_ = true;
      }

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " sequence radius " << radius().value() << " pitch " << pitch().value() << " bfield " << bfield
		  << " mom " << mom.value() << " dir (" << initial_dir().x().value() << ", " <<  initial_dir().y().value() << ", " << initial_dir().z().value() << ")" << std::endl;
      }

    }

    void sequence::calculate_charge(void){

      if( nodes().size() < 3 ) return;

      experimental_vector vi(nodes().front().ep(), nodes_[1].ep());
      experimental_vector vf(second_last_node().ep(), last_node().ep());

      double phi1 = vi.phi().value();
      double phi2 = vf.phi().value();
      mybhep::fix_angles(&phi1, &phi2);

      experimental_double deltaphi = vf.phi() - vi.phi();
      deltaphi.set_value(phi2 - phi1);

      if( deltaphi.value() ){
        has_charge_ = true;
        charge_ = deltaphi/fabs(deltaphi.value());
      }
      
      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " calculate tangent charge: initial phi " << phi1*180./acos(-1.) << " final phi " << phi2*180./acos(-1.) << " deltaphi " << deltaphi.value() << " charge " << charge_.value() << " +- " << charge_.error() << std::endl;
      }

      experimental_double first_helix_phi = helix_.phi_of_point(nodes().front().ep());
      experimental_double last_helix_phi = helix_.phi_of_point(last_node().ep());

      double helix_phi1 = first_helix_phi.value();
      double helix_phi2 = last_helix_phi.value();
      mybhep::fix_angles(&helix_phi1, &helix_phi2);

      experimental_double deltahelix_phi = last_helix_phi - first_helix_phi;
      deltahelix_phi.set_value(helix_phi2 - helix_phi1);

      if( deltahelix_phi.value() ){
        has_helix_charge_ = true;
        helix_charge_ = deltahelix_phi/fabs(deltahelix_phi.value());
      }

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " calculate helix charge: initial helix phi " << helix_phi1*180./acos(-1.) << " final phi " << helix_phi2*180./acos(-1.) << " deltaphi " << deltahelix_phi.value() << " charge " << helix_charge_.value() << " +- " << helix_charge_.error() << std::endl;
	std::clog << " calculate detailed charges: " << std::endl;
      }



      std::vector<experimental_double> angles;
      for(size_t i=0; i<nodes().size()-2; i++){
	experimental_vector vi(nodes_[i].ep(), nodes_[i+1].ep());
	experimental_vector vf(nodes_[i+1].ep(), nodes_[i+2].ep());

	double phi1 = vi.phi().value();
	double phi2 = vf.phi().value();
	mybhep::fix_angles(&phi1, &phi2);

	experimental_double deltaphi = vf.phi() - vi.phi();
	deltaphi.set_value(phi2 - phi1);

	if( deltaphi.value() ){
	  has_detailed_charge_ = true;
	  angles.push_back(deltaphi/fabs(deltaphi.value()));
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " triplet: " << i << " q " << angles.back().value() << " +- " << angles.back().error() << std::endl;
	  }
	}
      }
      detailed_charge_=average(angles);
      if( detailed_charge_.value() )
	detailed_charge_/=fabs(detailed_charge_.value());
      else{
	detailed_charge_=weighted_average(angles);
	if( detailed_charge_.value() )
	  detailed_charge_/=fabs(detailed_charge_.value());
	else
	  detailed_charge_.set(0.,0.);
      }

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " detailed charge " << detailed_charge_.value() << " +- " << detailed_charge_.error() << std::endl;
      }

      return;

    }

    bool sequence::intersect_plane_with_tangent_from_end(const plane & pl,
                                                         experimental_point * ep)const{
      // need 2 nodes to build the tangent line
      if( nodes().size() < 2 ) return false;

      experimental_vector direction(second_last_node().ep(), last_node().ep());

      bool result = pl.intersect(last_node().ep(), direction, ep);

      return result;
    }



    bool sequence::intersect_plane_from_end(const plane & pl, experimental_point * ep)const{

      experimental_double _phi = helix_.phi_of_point(last_node().ep());

      bool result = helix_.intersect_plane(pl, ep, _phi, last_node().ep().y().value());

      return result;

    }


    bool sequence::intersect_plane_with_tangent_from_begin(const plane & pl, experimental_point * ep)const{
      // need 2 nodes to build the tangent line
      if( nodes().size() < 2 ) return false;

      experimental_vector direction(nodes_[1].ep(), nodes_[0].ep());
      bool result = pl.intersect(nodes_[0].ep(), direction, ep);

      return result;
    }

    bool sequence::intersect_plane_from_begin(const plane & pl, experimental_point * ep)const{

      if( nodes().size() < 1 ) return false;

      experimental_double _phi = helix_.phi_of_point(nodes_[0].ep());

      bool result = helix_.intersect_plane(pl, ep, _phi, nodes_[0].ep().y().value());

      return result;

    }


    bool sequence::intersect_circle_with_tangent_from_end(const circle & c, experimental_point * ep)const{

      // need 2 nodes to build the tangent line
      if( nodes().size() < 2 ) return false;

      experimental_vector direction(second_last_node().ep(), last_node().ep());
      bool result = helix_.intersect_circle_with_tangent(c, last_node().ep(), direction.hor(), ep);

      return result;
    }

    bool sequence::intersect_circle_from_end(const circle & c, experimental_point * ep)const{

      experimental_double _phi = helix_.phi_of_point(last_node().ep());

      bool result = helix_.intersect_circle(c, ep, _phi);

      return result;

    }

    bool sequence::intersect_circle_from_end_minus_one(const circle & c, experimental_point * ep)const{

      if( nodes().size() < 2 ) return false;

      experimental_double _phi = helix_.phi_of_point(second_last_node().ep());

      bool result = helix_.intersect_circle(c, ep, _phi);

      return result;

    }

    bool sequence::intersect_circle_with_tangent_from_begin(const circle & c, experimental_point * ep)const{

      // need 2 nodes to build the tangent line
      if( nodes().size() < 2 ) return false;

      experimental_vector direction(nodes_[1].ep(), nodes_[0].ep());

      bool result = helix_.intersect_circle_with_tangent(c, nodes_[0].ep(), direction.hor(), ep);



      return result;
    }

    bool sequence::intersect_circle_from_begin(const circle & c, experimental_point * ep)const{

      if( nodes().size() < 1 ) return false;

      experimental_double _phi = helix_.phi_of_point(nodes_[0].ep());

      bool result = helix_.intersect_circle(c, ep, _phi);

      return result;

    }

    bool sequence::intersect_circle_from_begin_minus_one(const circle & c, experimental_point * ep)const{

      if( nodes().size() < 2 ) return false;

      experimental_double _phi = helix_.phi_of_point(nodes_[1].ep());

      bool result = helix_.intersect_circle(c, ep, _phi);

      return result;

    }
  

    bool sequence::intersect_sequence(sequence & seq,
                                      bool invertA, bool invertB, bool acrossGAP,
                                      experimental_point * ep,
                                      double limit_distance, int* with_kink, int cells_to_delete){
      bool result;
      double distanceA, distanceB;
      experimental_double local_distance;
      *with_kink = 0;

      if( print_level() >= mybhep::VVERBOSE )
	std::clog << " ... intersecting sequences, acrossGap " << acrossGAP << std::endl;

      if( !has_helix() ) calculate_helix();
      if( !seq.has_helix() ) seq.calculate_helix();

      if( !acrossGAP ){
	// intersect sequences within block

	circle c = seq.get_helix().get_circle();

	if( invertA ){
	  if( cells_to_delete == 0 || cells_to_delete == 1 ){
	    result=intersect_circle_from_begin(c,ep);
	    local_distance = nodes_[0].ep().distance(*ep);
	  }else if( cells_to_delete == 2 ){
	    result=intersect_circle_from_begin_minus_one(c,ep);
	    local_distance = nodes_[1].ep().distance(*ep);
	  }
	}
	else{
	  if( cells_to_delete == 0 || cells_to_delete == 1 ){
	    result=intersect_circle_from_end(c,ep);
	    local_distance = nodes_.back().ep().distance(*ep);
	  }else if( cells_to_delete == 2 ){
	    result=intersect_circle_from_end_minus_one(c,ep);
	    local_distance = second_last_node().ep().distance(*ep);
	  }
	}
	distanceA = fabs(std::max(local_distance.value()-local_distance.error(), 0.));
	
	if( print_level() >= mybhep::VVERBOSE )
	  std::clog << " ... result of circles intersection: " << result << std::endl;
      
	if(!result){
	  return false;
	}

	if( invertB ){	
	  if( cells_to_delete != 1 )
	    local_distance = seq.nodes().back().ep().distance(*ep);
	  else
	    local_distance = seq.second_last_node().ep().distance(*ep);
	}
	else{
	  if( cells_to_delete != 1 )
	    local_distance = seq.nodes_[0].ep().distance(*ep);
	  else
	    local_distance = seq.nodes_[1].ep().distance(*ep);
	}
	distanceB = fabs(std::max(local_distance.value()-local_distance.error(), 0.));
      
	if( print_level() >= mybhep::VVERBOSE )
	  std::clog << "ep (" << ep->x().value() << ", " << ep->y().value() << ", " << ep->z().value() << ") ... distanceA " << distanceA << " distanceB " << distanceB << " limit " << limit_distance << std::endl;
	
	return (result && distanceA <= limit_distance && distanceB <= limit_distance );
      }

      // intersect sequences across GAP

      experimental_point pA, pB;
      if( invertA ){
	pA = nodes_[0].ep();
      }
      else{
	pA = nodes_.back().ep();
      }
      if( invertB )
	pB = seq.nodes().back().ep();
      else
	pB = seq.nodes_[0].ep();


      experimental_point origin(0.,0.,0.,0.,0.,0.);

      // a circle along the gap of the 1st sequence
      circle cA(origin, pA.radius(), print_level(), probmin());

      // a circle along the gap of the 2nd sequence
      circle cB(origin, pB.radius(), print_level(), probmin());

      bool resultA, resultB, result_tangent_A, result_tangent_B;
      double distance_error_A, distance_error_B;
      experimental_point epA, epB, ep_tangent_A, ep_tangent_B;
      if( invertA ){
	resultB=intersect_circle_from_begin(cB,&epB);
	result_tangent_B= intersect_circle_with_tangent_from_begin(cB, &ep_tangent_B);
      }
      else{
	resultB=intersect_circle_from_end(cB,&epB);
	result_tangent_B= intersect_circle_with_tangent_from_end(cB, &ep_tangent_B);
      }
      local_distance = pB.hor_distance(epB);
      distanceB = local_distance.value();
      distance_error_B = local_distance.error();
      if( result_tangent_B ){
	resultB=true;
	local_distance = pB.hor_distance(ep_tangent_B);
	if( print_level() >= mybhep::VVERBOSE )
	  std::clog << " ... extrapolation through tangent B, pB (" << pB.x().value() << ", " << pB.y().value() << ", " << pB.z().value() << ") distance " << local_distance.value() << " +- " << local_distance.error()  << std::endl;
	distanceB = std::min(local_distance.value(), distanceB);
	distance_error_B = local_distance.error();
      }

      if( invertB ){
	resultA=seq.intersect_circle_from_end(cA,&epA);
	result_tangent_A= seq.intersect_circle_with_tangent_from_end(cA, &ep_tangent_A);
      }
      else{
	resultA=seq.intersect_circle_from_begin(cA,&epA);
	result_tangent_A= seq.intersect_circle_with_tangent_from_begin(cA, &ep_tangent_A);
      }
      local_distance = pA.hor_distance(epA);
      distanceA = fabs(std::max(local_distance.value()-local_distance.error(), 0.));
      if( result_tangent_A ){
	resultA=true;
	local_distance = pA.hor_distance(ep_tangent_A);
	if( print_level() >= mybhep::VVERBOSE )
	  std::clog << " ... extrapolation through tangent A, pA (" << pA.x().value() << ", " << pA.y().value() << ", " << pA.z().value() << ") distance " << local_distance.value() << " +- " << local_distance.error()  << std::endl;
	distanceA = std::min(local_distance.value(), distanceA);
	distance_error_A = local_distance.error();
      }

      double distance, distance_error;
      if( resultA && resultB ){
	if( distanceA < distanceB ){
	  *ep = epA;
	  distance = distanceA;
	  distance_error = distance_error_A;
	  *with_kink=1;
	}
	else{
	  *ep = epB;
	  distance = distanceB;
	  distance_error = distance_error_B;
	  *with_kink=2;
	}
      }else{
	if( resultB ){ // intersection on B circle
	  *ep = epB;
	  distance = distanceB;
	  distance_error = distance_error_B;
	  *with_kink=1;
	}
	if( resultA ){ // intersection on A circle
	  *ep = epA;
	  distance = distanceA;
	  distance_error = distance_error_A;
	  *with_kink=2;
	}
      }

      if( print_level() >= mybhep::VVERBOSE )
	std::clog << " ... resultA " << resultA << " ... resultB " << resultB << " distanceA " << distanceA << " distanceB " << distanceB << " distance " << distance << " +- " << distance_error << " limit " << 2.*limit_distance << std::endl;
      
      return ( (resultA || resultB) && std::max(distance - distance_error, 0.) < 2.*limit_distance );


    }
  

    std::string sequence::family()const{
      size_t i1 = name().find("_");
      size_t i2 = name().find("_",i1+1);
      return name().substr(i1+1,i2-i1-1);
    }

    std::vector<size_t> sequence::families()const{
      std::vector<size_t> fs;
      size_t i1, i2;
      for(std::vector<std::string>::const_iterator iname=names_.begin(); iname!=names_.end(); ++iname){
        i1 = iname->find("_");
        i2 = iname->find("_",i1+1);
        fs.push_back(mybhep::int_from_string(iname->substr(i1+1,i2-i1-1)));
      }
      return fs;
    }

    bool sequence::good_match(const sequence & seq,
                              bool &invertA, bool &invertB,
                              size_t NOffLayers)const{

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


      int layer_distance, cell_number_distance, block_distance;

      double distFF = nodes_[0].ep().distance(seq.nodes_[0].ep()).value();
      double distFL = nodes_[0].ep().distance(seq.nodes().back().ep()).value();
      double distLF = nodes().back().ep().distance(seq.nodes_[0].ep()).value();
      double distLL = nodes().back().ep().distance(seq.nodes().back().ep()).value();

      if( distLF <= distFF && distLF <= distFL && distLF <= distLL ){  // last to first  FL --> FL
        invertA = false;
        invertB = false;

	block_distance = last_node().c().block() - seq.nodes_[0].c().block();

        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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
	cell_number_distance = last_node().c().cell_number() - seq.nodes_[0].c().cell_number();
      }
      else if( distLL <= distFF && distLL <= distFL && distLL <= distLF ){ // last to last  FL -> LF
        invertA = false;
        invertB = true;

	block_distance = last_node().c().block() - seq.last_node().c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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
	cell_number_distance = last_node().c().cell_number() - seq.last_node().c().cell_number();
      }
      else if( distFL <= distFF && distFL <= distLL && distFL <= distLF ){ // first to last  LF -> LF
        invertA = true;
        invertB = true;
	block_distance = nodes_[0].c().block() - seq.last_node().c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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
	cell_number_distance = nodes_[0].c().cell_number() - seq.last_node().c().cell_number();
      }
      else{ // first to first  LF -> FL
        invertA = true;
        invertB = false;
	block_distance = nodes_[0].c().block() - seq.nodes_[0].c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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
	cell_number_distance = nodes_[0].c().cell_number() - seq.nodes_[0].c().cell_number();
      }


      // connection must be between neighboring layers
      if( fabs(layer_distance) > 1 + NOffLayers){
        if( print_level() >= mybhep::VVERBOSE )
          std::clog << " ... forbidden, because layers are far away by " << layer_distance << " planes " << std::endl;
        return false;
      }

      // connection in the same block must be between neighbouring cell numbers
    if( block_distance == 0 && fabs(cell_number_distance) > 1 + NOffLayers){
        if( print_level() >= mybhep::VVERBOSE )
          std::clog << " ... forbidden, because block is the same and cell numbers are far away by " << cell_number_distance << " cells " << std::endl;
        return false;
      }



      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " ... good match, distances: FF " << distFF << " FL " << distFL << " LF " << distLF << " LL " << distLL << " so invertA " << invertA << " invertB " << invertB << " layerdistance " << layer_distance << std::endl;
      }


      return true;
    }


    void sequence::remove_first_node(){
      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " removing first node " << nodes_.begin()->c().id() << std::endl;
      }
      nodes_.erase(nodes_.begin());
    }

    void sequence::remove_last_node(){
      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " removing last node " << nodes_.back().c().id() << std::endl;
      }
      nodes_.pop_back();
    }

    sequence sequence::match(sequence & seq, bool invertA, bool invertB, bool *ok, int with_kink, int cells_to_delete){

      sequence new_first_sequence;
      if( invertA )
        new_first_sequence = this->invert();
      else
        new_first_sequence = *this;

      sequence new_second_sequence;
      if( invertB )
        new_second_sequence = seq.invert();
      else
        new_second_sequence = seq;

      new_first_sequence.add_name(new_second_sequence.name());

      if( cells_to_delete == 1 ||  cells_to_delete == 2 )
	new_second_sequence.remove_first_node();
      if( cells_to_delete == 2 )
	new_first_sequence.remove_last_node();

      size_t s = new_first_sequence.nodes().size();
      size_t index;
      int next_index;
      bool last;

      if( with_kink == 1 ){
	new_first_sequence.last_node().set_is_kink(true);
      }

      for(size_t i = 0; i < new_second_sequence.nodes_.size(); i++){
        index = i;
        next_index = i+1;
        last = false;

        if( i == new_second_sequence.nodes_.size() - 1 )
          last = true;

        topology::node in = new_second_sequence.nodes_[index];

        if( i == 0 ){ // 1st added cell must get a new triplet
          new_first_sequence.nodes_[s-1].links_.push_back(in.c());
          cell_triplet ctA(new_first_sequence.nodes_[s-2].c(), new_first_sequence.nodes_[s-1].c(), in.c());
	  //          new_first_sequence.nodes_[s-1].ccc_.push_back(ctA);
          new_first_sequence.nodes_[s-1].add_triplet(ctA);

	  if( with_kink == 2 )
	    in.set_is_kink(true);
        }

        if( !last ){
          cell_triplet ct(new_first_sequence.last_node().c(), in.c(), new_second_sequence.nodes_[next_index].c());
          std::vector<cell_triplet> ccc;
          ccc.push_back(ct);
          in.set_ccc(ccc);
          std::vector<cell> ll;
          ll.push_back(new_second_sequence.nodes_[next_index].c());
          in.set_links(ll);
        } else {
          in.links_.clear();
          in.ccc_.clear();
        }


        new_first_sequence.nodes_.push_back(in);
      }

      if( new_second_sequence.has_decay_helix_vertex() ){
	if( new_second_sequence.decay_helix_vertex_type() == "foil" || new_second_sequence.decay_helix_vertex_type() == "kink")
	  new_first_sequence.set_decay_helix_vertex( new_second_sequence.decay_helix_vertex(), new_second_sequence.decay_helix_vertex_type() );
	else if( new_second_sequence.decay_helix_vertex_type() == "calo" )
	  new_first_sequence.set_decay_helix_vertex( new_second_sequence.decay_helix_vertex(), new_second_sequence.decay_helix_vertex_type(), new_second_sequence.calo_helix_id() );
      }
      
      if( new_second_sequence.has_decay_tangent_vertex() ){
	if( new_second_sequence.decay_tangent_vertex_type() == "foil" || new_second_sequence.decay_tangent_vertex_type() == "kink")
	  new_first_sequence.set_decay_tangent_vertex( new_second_sequence.decay_tangent_vertex(), new_second_sequence.decay_tangent_vertex_type() );
	else if( new_second_sequence.decay_tangent_vertex_type() == "calo" )
	  new_first_sequence.set_decay_tangent_vertex( new_second_sequence.decay_tangent_vertex(), new_second_sequence.decay_tangent_vertex_type(), new_second_sequence.calo_tangent_id() );
	
      }

      *ok = new_first_sequence.calculate_helix();


      return new_first_sequence;

    }


    void sequence::calculate_length(void) {

      if( nodes_.size() >= 3 ){
	experimental_point fp = nodes_[0].ep();
	experimental_point lp = nodes_.back().ep();
	if( has_helix_vertex() )
	  fp = helix_vertex();
	if( has_decay_helix_vertex() )
	  lp = decay_helix_vertex();
	
	experimental_double deltaphi = helix_.delta_phi(fp, lp);
	helix_length_ = experimental_sqrt(experimental_square(helix_.radius()) + experimental_square(helix_.pitch()))*experimental_fabs(deltaphi);
	has_helix_length_ = true;
      }

      if( nodes_.size() >= 2 ){
	experimental_double tl(0.,0.);
	int nindex=0;
	for(std::vector<node>::const_iterator inode=nodes_.begin(); inode != nodes_.end(); ++inode){
	  nindex = inode - nodes_.begin();
	  if( nindex == 0 ){
	    continue;
	  }
	  tl += inode->ep().distance(nodes_[nindex-1].ep());
	}
	
	if( has_tangent_vertex() ){
	  tl += tangent_vertex().distance(nodes_[0].ep());
	}
	if( has_decay_tangent_vertex() ){
	  tl += nodes_.back().ep().distance(decay_tangent_vertex());
	}

	tangent_length_ = tl;
	has_tangent_length_ = true;
      }

      return;

    }

    bool sequence::good_match_with_kink(const sequence & seq,
                                        bool &invertA, bool &invertB, bool &acrossGAP,
                                        double limit_distance, size_t NOffLayers,
					int &cells_to_delete)const{

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

      

      acrossGAP=false;
      cells_to_delete = 0;
      
      int layer_distance;
      int block_distance;
      int cell_number_distance;
      
      double distFF = nodes_[0].ep().distance(seq.nodes_[0].ep()).value();
      double distFL = nodes_[0].ep().distance(seq.nodes().back().ep()).value();
      double distLF = nodes().back().ep().distance(seq.nodes_[0].ep()).value();
      double distLL = nodes().back().ep().distance(seq.nodes().back().ep()).value();

      if( distLF <= distFF && distLF <= distFL && distLF <= distLL ){  // last to first  FL --> FL
        invertA = false;
        invertB = false;

	block_distance = last_node().c().block() - seq.nodes_[0].c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs(block_distance ) > 1 ){
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

	if( last_node().c().id() == seq.nodes_[0].c().id() ){
	  cells_to_delete = 1;
	  layer_distance = last_node().c().layer() - seq.nodes_[1].c().layer();
	  cell_number_distance = last_node().c().cell_number() - seq.nodes_[1].c().cell_number();
	}else if( second_last_node().c().id() == seq.nodes_[0].c().id() 
		  && last_node().c().id() == seq.nodes_[1].c().id()  ){
	  cells_to_delete = 2;
	  layer_distance = second_last_node().c().layer() - seq.nodes_[1].c().layer();
	  cell_number_distance = second_last_node().c().cell_number() - seq.nodes_[1].c().cell_number();
	}else{
	  layer_distance = last_node().c().layer() - seq.nodes_[0].c().layer();
	  cell_number_distance = last_node().c().cell_number() - seq.nodes_[0].c().cell_number();
	}	

        if( fabs( block_distance ) == 1 )
	  acrossGAP=true;
        else if( distLF > limit_distance*(cells_to_delete + 1)){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, because distance " << distLF << " is larger than limit " << limit_distance*(cells_to_delete + 1) << std::endl;
          return false;
        }

      }
      else if( distLL <= distFF && distLL <= distFL && distLL <= distLF ){ // last to last  FL -> LF
        invertA = false;
        invertB = true;

	block_distance = last_node().c().block() - seq.last_node().c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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

	if( last_node().c().id() == seq.last_node().c().id() ){
	  cells_to_delete = 1;
	  layer_distance = last_node().c().layer() - seq.second_last_node().c().layer();
	  cell_number_distance = last_node().c().cell_number() - seq.second_last_node().c().cell_number();
	}else if( second_last_node().c().id() == seq.last_node().c().id() 
		  && last_node().c().id() == seq.second_last_node().c().id()  ){
	  cells_to_delete = 2;
	  layer_distance = second_last_node().c().layer() - seq.second_last_node().c().layer();
	  cell_number_distance = second_last_node().c().cell_number() - seq.second_last_node().c().cell_number();
	}else{
	  layer_distance = last_node().c().layer() - seq.last_node().c().layer();
	  cell_number_distance = last_node().c().cell_number() - seq.last_node().c().cell_number();
	}

        if( fabs( block_distance ) == 1 )
	  acrossGAP=true;
        else if( distLL > limit_distance*(cells_to_delete + 1)){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, because distance " << distLL << " is larger than limit " << limit_distance*(cells_to_delete + 1) << std::endl;
          return false;
        }


      }
      else if( distFL <= distFF && distFL <= distLL && distFL <= distLF ){ // first to last  LF -> LF

        invertA = true;
        invertB = true;

	block_distance = nodes_[0].c().block() - seq.last_node().c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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

	if( nodes_[0].c().id() == seq.last_node().c().id() ){
	  cells_to_delete = 1;
	  layer_distance = nodes_[0].c().layer() - seq.second_last_node().c().layer();
	  cell_number_distance = nodes_[0].c().cell_number() - seq.second_last_node().c().cell_number();
	}else if( nodes_[1].c().id() == seq.last_node().c().id() 
		  && nodes_[0].c().id() == seq.second_last_node().c().id()  ){
	  cells_to_delete = 2;
	  layer_distance = nodes_[1].c().layer() - seq.second_last_node().c().layer();
	  cell_number_distance = nodes_[1].c().cell_number() - seq.second_last_node().c().cell_number();
	}else{
	  layer_distance = nodes_[0].c().layer() - seq.last_node().c().layer();
	  cell_number_distance = nodes_[0].c().cell_number() - seq.last_node().c().cell_number();
	}

        if( fabs( block_distance ) == 1 )
	  acrossGAP = true;
        else if( distFL > limit_distance*(cells_to_delete + 1)){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, because distance " << distFL << " is larger than limit " << limit_distance*(cells_to_delete + 1) << std::endl;
          return false;
        }

      }
      else{ // first to first  LF -> FL

        invertA = true;
        invertB = false;

	block_distance = nodes_[0].c().block() - seq.nodes_[0].c().block();
        // connection must be between neighboring blocks or within same block
        if( fabs( block_distance ) > 1 ){
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

	if( nodes_[0].c().id() == seq.nodes_[0].c().id() ){
	  cells_to_delete = 1;
	  layer_distance = nodes_[0].c().layer() - seq.nodes_[1].c().layer();
	  cell_number_distance = nodes_[0].c().cell_number() - seq.nodes_[1].c().cell_number();
	}else if( nodes_[1].c().id() == seq.nodes_[0].c().id() 
		  && nodes_[0].c().id() == seq.nodes_[1].c().id()  ){
	  cells_to_delete = 2;
	  layer_distance = nodes_[1].c().layer() - seq.nodes_[1].c().layer();
	  cell_number_distance = nodes_[1].c().cell_number() - seq.nodes_[1].c().cell_number();
	}else{
	  layer_distance = nodes_[0].c().layer() - seq.nodes_[0].c().layer();
	  cell_number_distance = nodes_[0].c().cell_number() - seq.nodes_[0].c().cell_number();
	}

        if( fabs( block_distance ) == 1 )
	  acrossGAP=true;
        else if( distFF > limit_distance*(cells_to_delete + 1)){
          if( print_level() >= mybhep::VVERBOSE )
            std::clog << " ... forbidden, because distance " << distFF << " is larger than limit " << limit_distance*(cells_to_delete + 1) << std::endl;
          return false;
        }

      }


      // connection must be between neighboring layers
      if( fabs(layer_distance) > 1 + NOffLayers){
        if( print_level() >= mybhep::VVERBOSE )
          std::clog << " ... forbidden, because layers are far away by " << layer_distance << " planes " << std::endl;
        return false;
      }


      // connection in the same block must be between neighbouring cell numbers
      if( !acrossGAP && fabs(cell_number_distance) > 1 + NOffLayers){
        if( print_level() >= mybhep::VVERBOSE )
          std::clog << " ... forbidden, because block is the same and cell numbers are far away by " << cell_number_distance << " cells " << std::endl;
        return false;
      }


      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " ... good kink match, distances: FF " << distFF << " FL " << distFL << " LF " << distLF << " LL " << distLL << " so invertA " << invertA << " invertB " << invertB << " across gap " << acrossGAP << " cells_to_delete " << cells_to_delete << std::endl;
      }


      return true;

    }


    bool sequence::same_families(const topology::sequence & s)const{

      std::vector<size_t> fA = families();
      std::vector<size_t> fB = s.families();

      for(std::vector<size_t>::iterator ifA=fA.begin(); ifA!=fA.end(); ++ifA)
        for(std::vector<size_t>::iterator ifB=fB.begin(); ifB!=fB.end(); ++ifB)
          if( *ifA == *ifB )
            return true;

      return false;
    }

    experimental_double sequence::delta_phi(const experimental_point & epa,
                               const experimental_point & epb)const{
      return helix_.delta_phi(epa, epb);
    }


    // is the (true) track all on one side of the foil?
    bool sequence::one_side()const{
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


    experimental_vector sequence::initial_dir()const{
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

    experimental_vector sequence::final_dir()const{
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
    bool sequence::worst_node(topology::node& n)const{

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
    double sequence::phi_kink(size_t inode)const{

      if( nodes_.size() <= 2 ) return 0.;
      if( inode == 0 ) return 0.;
      if( inode >= nodes_.size() - 1 ) return 0.;

      double phi = fabs((joint(nodes_[inode - 1].ep(), nodes_[inode].ep(), nodes_[inode + 1].ep())).kink_phi().value());

      return phi;
    }

    // get node of largest horizontal kink
    bool sequence::largest_kink_node(topology::node& n, double& phi)const{

      if( nodes_.size() <= 2 ) return false;

      int nindex = -1;

      double phimax = mybhep::default_max;
      size_t index;
      double local_phi;
      for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in){
        index = in - nodes_.begin();
        if( index == 0 ) continue;
        if( index == nodes_.size() - 1 ) continue;

        local_phi = phi_kink(index);

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

    bool sequence::helix_out_of_range(double lim){

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

    bool sequence::has_kink(std::vector<size_t> *index) const{
      bool has_kink=false;
      for(std::vector<node>::const_iterator in = nodes_.begin(); in != nodes_.end(); ++in){
	if( in->is_kink() ){
	  has_kink=true;
	  index->push_back(in - nodes_.begin());
	}
      }

      return has_kink;
    }

    bool sequence::has_kink(void) const{
      std::vector<size_t> index;
      return has_kink(&index);
    }

    void sequence::point_of_max_min_radius(experimental_point epa, experimental_point epb, experimental_point *epmax, experimental_point *epmin){
      helix_.point_of_max_min_radius(epa, epb, epmax, epmin);
      return;
    }


  //*************************************************************
    bool sequence::common_vertex_on_foil(const sequence *seqB, double *the_distance)const{
    //*************************************************************

    std::vector<experimental_point> foil_vertex_A, foil_vertex_B;

    if( this->has_helix_vertex() && this->helix_vertex_type() == "foil" )
      foil_vertex_A.push_back(this->helix_vertex());
    if( this->has_tangent_vertex() && this->tangent_vertex_type() == "foil" )
      foil_vertex_A.push_back(this->tangent_vertex());
    if( this->has_decay_helix_vertex() && this->decay_helix_vertex_type() == "foil" )
      foil_vertex_A.push_back(this->decay_helix_vertex());
    if( this->has_decay_tangent_vertex() && this->decay_tangent_vertex_type() == "foil" )
      foil_vertex_A.push_back(this->decay_tangent_vertex());

    if( seqB->has_helix_vertex() && seqB->helix_vertex_type() == "foil" )
      foil_vertex_B.push_back(seqB->helix_vertex());
    if( seqB->has_tangent_vertex() && seqB->tangent_vertex_type() == "foil" )
      foil_vertex_B.push_back(seqB->tangent_vertex());
    if( seqB->has_decay_helix_vertex() && seqB->decay_helix_vertex_type() == "foil" )
      foil_vertex_B.push_back(seqB->decay_helix_vertex());
    if( seqB->has_decay_tangent_vertex() && seqB->decay_tangent_vertex_type() == "foil" )
      foil_vertex_B.push_back(seqB->decay_tangent_vertex());

    bool found = false;
    double distance;
    double min_distance = mybhep::default_min;

    for(std::vector<experimental_point>::const_iterator vA=foil_vertex_A.begin(); vA!=foil_vertex_A.end(); vA++)
      for(std::vector<experimental_point>::const_iterator vB=foil_vertex_B.begin(); vB!=foil_vertex_B.end(); vB++)
	{
	  distance = (vA->distance(*vB)).value();
	  if( distance < min_distance ){
	    min_distance = distance;
	    found = true;
	  }
	}

    if( found ){
      *the_distance = min_distance;
      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " sequences " << this->name() << " and " << seqB->name() << " have vertex on foil with distance " << min_distance << std::endl;
      }
    }

    return found;

  }



  }
}

// end
