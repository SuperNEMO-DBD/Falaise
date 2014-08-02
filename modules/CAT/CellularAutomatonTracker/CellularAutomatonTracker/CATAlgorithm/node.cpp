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
        is_kink_ = false;
        chi2_ = 0.;
        ndof_=0;
        circle_phi_ = mybhep::small_neg;
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
        is_kink_ = false;
        chi2_ = 0.;
        ndof_=0;
        setup_cc_maps();
        setup_ccc_maps();
        circle_phi_ = mybhep::small_neg;
      }

      //! constructor
      node::node(const cell &c, prlevel level, double probmin){
        set_print_level(level);
        set_probmin(probmin);
        appname_= "node: ";
        c_ = c;
        free_ = false;
        is_kink_ = false;
        chi2_ = 0.;
        ndof_=0;
        circle_phi_ = mybhep::small_neg;
      }

      //! constructor from bhep true hit
      node::node(const mybhep::hit &truehit, size_t id, bool SuperNemo, prlevel level, double probmin){
        set_print_level(level);
        set_probmin(probmin);
        appname_= "node: ";
        chi2_ = 0.;
        ndof_=0;
        std::vector<double> cellpos;
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
          cell tmp_cell (center, radius, id, fast, probmin, level);
          c_ = tmp_cell;
        }
        int block, plane, iid, n3id;
        if( SuperNemo )
          {
            c_.set_type("SN");
            std::string value = truehit.fetch_property("CELL"); // GG_CELL_block_plane_id

            sscanf(value.c_str(),"GG_CELL_%d_%d_%d",&block,&plane,&iid);
            plane --;
            if( block < 0 ) plane *= -1;
            n3id = 0;

          }
        else
          {
            c_.set_type("N3");
            std::string value = truehit.fetch_property("BLK");  // BLK = sector_io_layer
            // sector = petal of the detector
            // io = 1 if hit is between foil and external calorimeter
            //     0 if hit is between foil and internal calorimeter
            // layer = 0-8

            int io;
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

            std::string val = truehit.fetch_property("CELL");  // cell number
            sscanf(val.c_str(),"%d",&n3id);

          }

        c_.set_layer(plane);
        c_.set_n3id(n3id);
        c_.set_block(block);
        c_.set_iid(iid);

        free_ = false;
        is_kink_ = false;
        {
          experimental_point tmp_ep(truehit.x().x(), truehit.x().y(), truehit.x().z(),
                                    0., 0., 0.);
          ep_ = tmp_ep;
        }
        circle_phi_ = mybhep::small_neg;
        return;


      }

      /*** dump ***/
      void node::dump (ostream & a_out,
                       const std::string & a_title,
                       const std::string & a_indent,
                       bool /*a_inherit*/) const{
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " --------------------- " << std::endl;
        a_out << indent  << " main cell " << " free " << free() << " chi2 " << chi2() << " ndof " << ndof() << std::endl;
        this->c().dump(a_out,"",indent + "   ");
        a_out << indent << " fitted point: "; ep().dump();
        a_out << indent << " cell couplets: " << cc().size() << std::endl;
        for(std::vector<cell_couplet>::const_iterator icc=cc_.begin(); icc!=cc_.end(); ++icc)
          icc->dump(a_out, "",indent + "     ");
        a_out << indent << " cell triplets: " << ccc().size() << std::endl;
        for(std::vector<cell_triplet>::const_iterator iccc=ccc_.begin(); iccc!=ccc_.end(); ++iccc)
          iccc->dump(a_out, "",indent + "     ");
        a_out << indent  << " --------------------- " << std::endl;

        return;
      }

    void node::setup_cc_maps(){
      cc_index_.clear();
      for(std::vector<cell_couplet>::const_iterator icc=cc_.begin(); icc!=cc_.end(); ++icc){
        cc_index_[icc->cb().id()] = icc-cc_.begin();
      }
    }

    void node::setup_ccc_maps(){
      ccc_ca_index_.clear();
      ccc_cc_index_.clear();
      for(std::vector<cell_triplet>::const_iterator iccc=ccc_.begin(); iccc!=ccc_.end(); ++iccc){
        ccc_ca_index_[iccc->cb().id()] = iccc-ccc_.begin();
        ccc_cc_index_[iccc->cc().id()] = iccc-ccc_.begin();
      }
    }


      //! set cells
      void node::set(const cell &c, const std::vector<cell_couplet> &cc, const std::vector<cell_triplet> & ccc){
        c_ = c;
        cc_ = cc;
        ccc_ = ccc;
        setup_cc_maps();
        setup_ccc_maps();
      }

      //! set main cell
      void node::set_c(const cell& c){
        c_ = c;
      }

      //! set cell couplets
      void node::set_cc(const std::vector<cell_couplet> &cc){
        cc_ = cc;
        setup_cc_maps();
        setup_ccc_maps();
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

      //! set is_kink
      void node::set_is_kink(bool is_kink){
        is_kink_ = is_kink;
      }

      //! set chi2
      void node::set_chi2(double chi2){
        chi2_ = chi2;
      }

      //! set ndof
      void node::set_ndof(int32_t ndof){
        ndof_ = ndof;
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

      //! get is_kink
      bool node::is_kink()const{
        return is_kink_;
      }

      //! get chi2
      double node::chi2()const{
        return chi2_;
      }

      //! get ndof
      int32_t node::ndof()const{
        return ndof_;
      }

      //! get prob
      double node::Prob()const{
        return probof(chi2(),ndof());
      }

      //! get fitted experimental_point
      const experimental_point& node::ep()const
      {
        return ep_;
      }

      void node::calculate_triplets(double Ratio,
                              double separation_limit, double phi_limit, double theta_limit){
        if( cc_.size() < 2 ) return;
        for(std::vector<cell_couplet>::const_iterator icc=cc_.begin(); icc!=cc_.end(); ++icc){
          cell c1 = icc->cb();
          for(std::vector<cell_couplet>::const_iterator jcc=cc_.begin() + (size_t)(icc - cc_.begin()); jcc!=cc_.end(); ++jcc){
            cell c2 = jcc->cb();
            if( c1.id() == c2.id() ) continue;
            cell_triplet ccc(c1,c_,c2, print_level(), probmin());
            if( print_level() >= mybhep::VVERBOSE ){
              std::clog << appname_ << " calculate triplets for three cells: " << ccc.ca().id() << "  " << ccc.cb().id() << "  " << ccc.cc().id() << std::endl;
            }
            ccc.calculate_joints(Ratio, separation_limit, phi_limit, theta_limit);
            if( ccc.joints().size() > 0 ){
              if( print_level() >= mybhep::VVERBOSE ){
                std::clog << appname_ << " adding joints " << std::endl;
                for(std::vector<joint>::iterator ijoint = ccc.joints_.begin(); ijoint != ccc.joints_.end(); ++ ijoint )
                  std::clog << " joint " << ijoint - ccc.joints_.begin() << " phia: " << experimental_vector(ccc.ca().ep(), ijoint->epa()).phi().value()*180./M_PI
                       << " phib: " << experimental_vector(ccc.cb().ep(), ijoint->epb()).phi().value()*180./M_PI
                       << " phic: " << experimental_vector(ccc.cc().ep(), ijoint->epc()).phi().value()*180./M_PI << " chi2 " << ijoint->chi2() << std::endl;
              }
              add_triplet(ccc);
            }
          }
        }
      }

    void node::calculate_triplets_after_sultan(double Ratio){
      // always get 1 triplet with 1 or 2 joints
      if( cc_.size() != 2 ){
	return;
      }

      cell c1 = cc_[0].cb();
      cell c2 = cc_[1].cb();
      if( c1.id() == c2.id() ){
	if( print_level() >= mybhep::NORMAL ){
	  std::clog << appname_ << " problem: calculate triplets after sultan: cc id1 " << c1.id() << " id2 " << c2.id() << std::endl;
	}
	return;
      }

      cell_triplet ccc(c1,c_,c2, print_level(), probmin());
      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << appname_ << " calculate joints for three cells: " << ccc.ca().id() << "  " << ccc.cb().id() << "  " << ccc.cc().id() << std::endl;
      }
      ccc.calculate_joints_after_sultan(Ratio);
      if( ccc.joints().size() > 0 ){
	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << appname_ << " adding joints " << std::endl;
	  for(std::vector<joint>::iterator ijoint = ccc.joints_.begin(); ijoint != ccc.joints_.end(); ++ ijoint )
	    std::clog << " joint " << ijoint - ccc.joints_.begin() << " phia: " << experimental_vector(ccc.ca().ep(), ijoint->epa()).phi().value()*180./M_PI
		      << " phib: " << experimental_vector(ccc.cb().ep(), ijoint->epb()).phi().value()*180./M_PI
		      << " phic: " << experimental_vector(ccc.cc().ep(), ijoint->epc()).phi().value()*180./M_PI << " chi2 " << ijoint->chi2() << std::endl;
	}
	add_triplet(ccc);
      }
    }

    void node::add_triplet(const cell_triplet &ccc){
      ccc_.push_back(ccc);
      ccc_ca_index_[ccc.ca().id()] = ccc_.size() - 1;
      ccc_cc_index_[ccc.cc().id()] = ccc_.size() - 1;
      return;
    }

    void node::remove_couplet(size_t index){
      cc_.erase(cc_.begin() + index);
      setup_cc_maps();
      return;
    }

    void node::remove_triplet(size_t index){
      ccc_.erase(ccc_.begin() + index);
      setup_ccc_maps();
      return;
    }

    void node::remove_link(size_t index){
      links_.erase(links_.begin() + index);
      return;
    }

      node node::invert(){
        node inverted;
        inverted.set_print_level(print_level());
        inverted.set_probmin(probmin());
        inverted.set_c(c());
        inverted.set_cc(cc());
        inverted.set_ccc(ccc());
        inverted.set_free(free());
        inverted.set_chi2(chi2());
        inverted.set_ndof(ndof());
        inverted.set_ep(ep());
        return inverted;

      }

      std::string node::topological_type() const{

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

    //! grab cc index map
    std::map<size_t,size_t> node::cc_index()const{
      return cc_index_;
    }

    //! grab ccc cb index map
    std::map<size_t,size_t> node::ccc_ca_index()const{
      return ccc_ca_index_;
    }

    //! grab ccc cc index map
    std::map<size_t,size_t> node::ccc_cc_index()const{
      return ccc_cc_index_;
    }


      bool node::has_couplet(const cell & a, cell_couplet* ct)const {

#if 0
        cell null;
        std::vector<cell_couplet>::const_iterator fcouplet = std::find(cc_.begin(), cc_.end(), cell_couplet(null, a));

        if( fcouplet != cc().end() ){
          *ct = *fcouplet;
          return true;
        }

        return false;
#else

        if( !cc_index_.count(a.id()) ) return false;
        size_t index=cc_index()[a.id()];
        if( index >= cc_.size() ){
          std::clog << " problem: cc index " << index << " for cell a of id " << a.id() << " is larger than cc size " << cc_.size() << endl;
          dump();
          return false;
        }
        *ct = cc_.at(index);
        return true;
#endif

      }

      bool node::has_couplet(const cell& a, size_t* index)const{

#if 0
        cell null;
        std::vector<cell_couplet>::const_iterator fcouplet = std::find(cc_.begin(),
                                                            cc_.end(),
                                                            cell_couplet(null, a, "just"));

        if( fcouplet != cc().end()){
          *index = fcouplet - cc_.begin();
          return true;
        }
        return false;
#else

        if( !cc_index_.count(a.id()) ) return false;
        *index= cc_index()[a.id()];
        if( *index >= cc_.size() ){
          std::clog << " problem: cc index " << *index << " for cell of id " << a.id() << " is larger than cc size " << cc_.size() << endl;
          dump();
          return false;
        }
        return true;
#endif

      }

      bool node::has_couplet(size_t idd, size_t* index)const{

#if 0
        cell null;
        null.set_id(idd);
        return has_couplet(null, index);
#else

        if( !cc_index_.count(idd) ) return false;
        *index= cc_index()[idd];
        if( *index >= cc_.size() ){
          std::clog << " problem: cc index " << *index << " for id " << idd << " is larger than cc size " << cc_.size() << endl;
          dump();
          return false;
        }
        return true;

#endif

      }

    bool node::has_triplet(const cell &a, const cell &c, size_t *index)const{

#if 1
      cell null;
      std::vector<cell_triplet>::const_iterator ftriplet = std::find(ccc().begin(), ccc().end(),cell_triplet(a,null,c) );

      if( ftriplet != ccc().end() ){
        *index = ftriplet - ccc().begin();
        return true;
      }
      return false;
#else

        if( !ccc_ca_index_.count(a.id()) ) return false;
        if( !ccc_cc_index_.count(c.id()) ) return false;
        size_t indexa=ccc_ca_index()[a.id()];
        if( indexa >= ccc_ca_index_.size() ){
          std::clog << " problem: ccc ca index " << indexa << " is larger than ccc size " << ccc_.size() << endl;
          return false;
        }
        size_t indexc=ccc_cc_index()[c.id()];
        if( indexc >= ccc_cc_index_.size() ){
          std::clog << " problem: ccc cc index " << indexc << " is larger than ccc size " << ccc_.size() << endl;
          return false;
        }
        if( indexa != indexc ) return false;
        *index = indexa;
        return true;
#endif

      }

      bool node::has_triplet(const cell &a, const cell &c)const{

#if 1
        cell null;
        if( std::find(ccc().begin(), ccc().end(),cell_triplet(a,null,c) ) != ccc().end() )
          return true;
        return false;
#else

        if( !ccc_ca_index_.count(a.id()) ) return false;
        if( !ccc_cc_index_.count(c.id()) ) return false;
        size_t indexa=ccc_ca_index()[a.id()];
        if( indexa >= ccc_ca_index_.size() ){
          std::clog << " problem: ccc ca index " << indexa << " is larger than ccc size " << ccc_.size() << endl;
          return false;
        }
        size_t indexc=ccc_cc_index()[c.id()];
        if( indexc >= ccc_cc_index_.size() ){
          std::clog << " problem: ccc cc index " << indexc << " is larger than ccc size " << ccc_.size() << endl;
          return false;
        }
        if( indexa != indexc ) return false;
        return true;
#endif

      }

      bool node::has_triplet(const cell &a)const{

#if 1
        for(std::vector<cell_triplet>::const_iterator iccc=ccc_.begin(); iccc!=ccc_.end(); ++iccc){
          size_t ida = iccc->ca().id();
          size_t idc = iccc->cc().id();
          if( ( ida == a.id() || idc == a.id() ) ){
            return true;
          }
        }

        return false;
#else

        return( ccc_ca_index_.count(a.id()) || ccc_cc_index_.count(a.id()) );

#endif


      }


      bool operator==(const node& left,
                      const node& right)
      {

        return left.c().id() == right.c().id();

      }

  }
}
