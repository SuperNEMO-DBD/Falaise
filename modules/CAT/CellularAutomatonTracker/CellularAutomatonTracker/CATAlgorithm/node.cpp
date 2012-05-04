/* -*- mode: c++ -*- */

#include <CATAlgorithm/node.h>

namespace CAT {
  namespace topology{

    using namespace std;
    using namespace mybhep;

      //!Default constructor     
    node::node()
      {
        appname_= "node: ";
        free_ = false;
        chi2_ = 0.;
      }

      //!Default destructor
      node::~node()
      {
        return;
      }

      //! constructor
      node::node(const cell & c, const std::vector<cell_couplet> & cc, const std::vector<cell_triplet> & ccc){
        appname_= "node: ";
        c_ = c;
        cc_ = cc;
        ccc_ = ccc;
        free_ = false;
        chi2_ = 0.;
      }

      //! constructor
      node::node(const cell &c, prlevel level, double nsigma){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "node: ";
        c_ = c;
        free_ = false;
        chi2_ = 0.;
      }

      //! constructor from bhep true hit
      node::node(const mybhep::hit &truehit, size_t id, bool SuperNemo, prlevel level, double nsigma){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "node: ";
        chi2_ = 0.;
        vector<double> cellpos;
        mybhep::vector_from_string(truehit.fetch_property("CELL_POS"), cellpos);
        double rpos = mybhep::float_from_string(truehit.fetch_property("DIST"));
        experimental_point center(cellpos[0], cellpos[1], cellpos[2],
                                  0., 0., 0.);
        experimental_double radius(rpos, 0.);

        bool fast;
        if( truehit.find_property("SLOW"))
          fast = false;
        else
          fast = true;
        {
          cell tmp_cell (center, radius, id, fast, nsigma, level);
          c_ = tmp_cell;
        }
        int block, plane, iid, n3id;
        if( SuperNemo )
          {
            c_.set_type("SN");
            string value = truehit.fetch_property("CELL"); // GG_CELL_block_plane_id
          
            sscanf(value.c_str(),"GG_CELL_%d_%d_%d",&block,&plane,&iid);
            plane --;
            if( block < 0 ) plane *= -1;
            n3id = 0;
          
          }
        else
          {
            c_.set_type("N3");
            string value = truehit.fetch_property("BLK");  // BLK = sector_io_layer
            // sector = petal of the detector
            // io = 1 if hit is between foil and external calorimeter
            //     0 if hit is between foil and internal calorimeter
            // layer = 0-8
      
            size_t io;
            sscanf(value.c_str(),"%d_%d_%d",&block,&io,&iid);

            plane = iid;

            //translate layer into block number
            if (plane<4)
              block = 1;  // 0, 1, 2, 3
            else if(plane >=4 && plane <6)
              block = 2; // 4, 5
            else
              block = 3;  // 6, 7, 8

            if( io == 0 ){
              plane *= -1;
              block *= -1;
            }
            // block = 1, 2, 3 or -1, -2, -3
            // layer = 0, 1, ..., 8 or 0, -1, ..., -8

            string val = truehit.fetch_property("CELL");  // cell number
            sscanf(val.c_str(),"%d",&n3id);

          }

        c_.set_layer(plane);
        c_.set_n3id(n3id);
        c_.set_block(block);
        c_.set_iid(iid);

        free_ = false;
        {
          experimental_point tmp_ep(truehit.x().x(), truehit.x().y(), truehit.x().z(),
                                    0., 0., 0.); 
          ep_ = tmp_ep;
        }
        return;


      }

      /*** dump ***/
      void node::dump (ostream & a_out,
                       const string & a_title,
                       const string & a_indent,
                       bool a_inherit) const{
        string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << endl;
          }

        a_out << indent << appname_ << " --------------------- " << endl;
        a_out << indent  << " main cell " << " free " << free() << " chi2 " << chi2() << endl;
        this->c().dump(a_out,"",indent + "   ");
        a_out << indent << " fitted point: "; ep().dump();
        a_out << indent << " cell couplets: " << cc().size() << endl;
        for(vector<cell_couplet>::const_iterator icc=cc_.begin(); icc!=cc_.end(); ++icc)
          icc->dump(a_out, "",indent + "     ");
        a_out << indent << " cell triplets: " << ccc().size() << endl;
        for(vector<cell_triplet>::const_iterator iccc=ccc_.begin(); iccc!=ccc_.end(); ++iccc)
          iccc->dump(a_out, "",indent + "     ");
        a_out << indent  << " --------------------- " << endl;
 
        return;
      }
    
      //! set cells
      void node::set(const cell &c, const std::vector<cell_couplet> &cc, const std::vector<cell_triplet> & ccc){
        c_ = c;
        cc_ = cc;
        ccc_ = ccc;
      }

      //! set main cell
      void node::set_c(const cell& c){
        c_ = c;
      }

      //! set cell couplets
      void node::set_cc(const std::vector<cell_couplet> &cc){
        cc_ = cc;
      }

      //! set cell triplets
      void node::set_ccc(const std::vector<cell_triplet>  &ccc){
        ccc_ = ccc;
      }

      //! set links
      void node::set_links(const std::vector<cell>  &links){
        links_ = links;
      }

      //! set free level
      void node::set_free(bool free){
        free_ = free;
      }

      //! set chi2
      void node::set_chi2(double chi2){
        chi2_ = chi2;
      }

      //! set fitted experimental_point
      void node::set_ep( const experimental_point &ep )
      {
        ep_ = ep;
      }

      //! get main cell
      const cell& node::c()const
      {
        return c_;
      }      

      //! get cell couplets
      const std::vector<cell_couplet> &node::cc()const{
        return cc_;
      }

      //! get cell triplets
      const std::vector<cell_triplet> &node::ccc()const{
        return ccc_;
      }

      //! get links
      const std::vector<cell> &node::links()const{
        return links_;
      }

      //! get free level
      bool node::free()const{
        return free_;
      }

      //! get chi2
      double node::chi2()const{
        return chi2_;
      }

      //! get fitted experimental_point
      const experimental_point& node::ep()const
      {
        return ep_;
      }

      void node::calculate_triplets(double Ratio, 
                              double separation_limit, double phi_limit, double theta_limit){
        if( cc_.size() < 2 ) return;
        for(vector<cell_couplet>::const_iterator icc=cc_.begin(); icc!=cc_.end(); ++icc){
          cell c1 = icc->cb();
          for(vector<cell_couplet>::const_iterator jcc=cc_.begin() + (size_t)(icc - cc_.begin()); jcc!=cc_.end(); ++jcc){
            cell c2 = jcc->cb();
            if( c1.id() == c2.id() ) continue;
            cell_triplet ccc(c1,c_,c2, print_level(), nsigma());
            if( print_level() >= mybhep::VVERBOSE ){
              clog << appname_ << " calculate triplets for three cells: " << ccc.ca().id() << "  " << ccc.cb().id() << "  " << ccc.cc().id() << endl;
            }
            ccc.calculate_joints(Ratio, separation_limit, phi_limit, theta_limit);
            if( ccc.joints().size() > 0 ){
              if( print_level() >= mybhep::VVERBOSE ){
                clog << appname_ << " adding joints " << endl;
                for(std::vector<joint>::iterator ijoint = ccc.joints_.begin(); ijoint != ccc.joints_.end(); ++ ijoint )
                  clog << " joint " << ijoint - ccc.joints_.begin() << " phia: " << experimental_vector(ccc.ca().ep(), ijoint->epa()).phi().value()*180./M_PI
                       << " phib: " << experimental_vector(ccc.cb().ep(), ijoint->epb()).phi().value()*180./M_PI
                       << " phic: " << experimental_vector(ccc.cc().ep(), ijoint->epc()).phi().value()*180./M_PI << " chi2 " << ijoint->chi2() << endl;
              }
              ccc_.push_back(ccc);
            }
          }
        }
      }


      node node::invert(){
        node inverted;
        inverted.set_print_level(print_level());
        inverted.set_nsigma(nsigma());
        inverted.set_c(c());
        inverted.set_cc(cc());
        inverted.set_ccc(ccc());
        inverted.set_free(free());
        inverted.set_chi2(chi2());
        inverted.set_ep(ep());
        return inverted;

      }

      string node::topological_type() const{

        if( cc().empty() ) // no cell couplets
          return "ISOLATED";

        if( ccc().empty() ){ // no cell triplets
          if( cc().size() == 1 )
            return "VERTEX";
          return "MULTI_VERTEX";
        }

        if( ccc().size() == 1 ) // 1 cell triplet
          return "BRIDGE";

        return "OTHER";
      }


      bool node::has_couplet(const cell & a, cell_couplet* ct)const {

        cell null;
        vector<cell_couplet>::const_iterator fcouplet = std::find(cc_.begin(), cc_.end(), cell_couplet(null, a));

        if( fcouplet != cc().end() ){
          *ct = *fcouplet;
          return true;
        }

        return false;
      }

      bool node::has_couplet(const cell& a, size_t* index)const{

        cell null;
        vector<cell_couplet>::const_iterator fcouplet = std::find(cc_.begin(),
                                                            cc_.end(),
                                                            cell_couplet(null, a, "just"));

        if( fcouplet != cc().end()){
          *index = fcouplet - cc_.begin();
          return true;
        }

        return false;

      }

      bool node::has_couplet(size_t idd, size_t* index)const{

        cell null;
        null.set_id(idd);

        return has_couplet(null, index);

      }

      bool node::has_triplet(const cell &a, const cell &c)const{

        cell null;
        if( std::find(ccc().begin(), ccc().end(),cell_triplet(a,null,c) ) != ccc().end() )
          return true;

        return false;
      }

      bool node::has_triplet(const cell &a)const{

        for(vector<cell_triplet>::const_iterator iccc=ccc_.begin(); iccc!=ccc_.end(); ++iccc){
          size_t ida = iccc->ca().id();
          size_t idc = iccc->cc().id();
          if( ( ida == a.id() || idc == a.id() ) ){
            return true;
          }
        }
      
        return false;
      }


      bool operator==(const node& left,
                      const node& right)
      {

        return left.c().id() == right.c().id();

      }

  }
}
