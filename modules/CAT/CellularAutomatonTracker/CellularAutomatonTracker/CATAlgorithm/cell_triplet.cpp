/* -*- mode: c++ -*- */

#include <CATAlgorithm/cell_triplet.h>

namespace CAT{
  namespace topology{

    using namespace std;
    using namespace mybhep;
 
    //!Default constructor     
    cell_triplet::cell_triplet()
    {
      appname_= "cell_triplet: ";
      free_ = false;
      begun_ = false;
    }

    //!Default destructor
    cell_triplet::~cell_triplet(){};

    //! constructor
    cell_triplet::cell_triplet(cell_couplet &cca, cell_couplet &ccb){
      appname_= "cell_triplet: ";
      free_ = false;
      begun_ = false;
    }

    //! constructor
    cell_triplet::cell_triplet(const cell &ca, const cell &cb, const cell &cc, prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cell_triplet: ";
      ca_ = ca;
      cb_ = cb;
      cc_ = cc;
      free_ = false;
      begun_ = false;
      chi2s_.clear();
      probs_.clear();
    }

    /*** dump ***/
    void cell_triplet::dump (std::ostream & a_out,
                             const std::string & a_title,
                             const std::string & a_indent,
                             bool a_inherit) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " -------------- " << std::endl;
      a_out << indent  << " free: " << free() << " begun: " << begun()  << std::endl;
      a_out << indent  << " first cell " << std::endl;
      ca().dump(a_out,"", indent + "   ");
      a_out << indent << " second cell " << std::endl;
      cb().dump(a_out, "",indent + "   ");
      a_out << indent << " third cell " << std::endl;
      cc().dump(a_out, "",indent + "   ");
      for(std::vector<joint>::const_iterator ijoint=joints_.begin(); ijoint!=joints_.end(); ++ijoint)
        ijoint->dump(a_out,"",indent + "   ");
      a_out << indent  << " -------------- " << std::endl;
 
      return;
    }

    /*** dump ***/
    void cell_triplet::dump_joint (joint j,
				   std::ostream & a_out,
				   const std::string & a_title,
				   const std::string & a_indent,
				   bool a_inherit) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " -------------- " << std::endl;
      a_out << indent << " first point " << std::endl;
      this->ca().dump_point(j.epa());
      a_out << indent << " second point " << std::endl;
      this->cb().dump_point(j.epb());
      a_out << indent << " third point " << std::endl;
      this->cc().dump_point(j.epc());
      a_out << indent << " -------------- " << std::endl;

      return;
    }
    
    //! set cells
    void cell_triplet::set(const cell_couplet &cca, const cell_couplet &ccb){
      cb_ = cca.ca();
      ca_ = cca.cb();
      cc_ = ccb.cb();
      if( cca.ca().id() != ccb.ca().id() ){
        std::clog << " problem: trying to form a triplet of cell with cells " << cca.ca().id() << " " 
             << cca.cb().id() << " " 
             << ccb.ca().id() << " " 
             << ccb.cb().id() << std::endl;
      }
    }

    //! set cells
    void cell_triplet::set(const cell &ca, const cell &cb, const cell &cc){
      ca_ = ca;
      cb_ = cb;
      cc_ = cc;
    }


    //! set free level
    void cell_triplet::set_free(bool free){
      free_ = free;
    }

    //! set begun level
    void cell_triplet::set_begun(bool begun){
      begun_ = begun;
    }

    //! set joints
    void cell_triplet::set_joints(const std::vector<joint> & joints){
      joints_ = joints;
    }

    //! set chi2 list
    void cell_triplet::set_chi2s(const std::vector<double> & chi2s){
      chi2s_ = chi2s;
    }

    //! set prob list
    void cell_triplet::set_probs(const std::vector<double> & probs){
      probs_ = probs;
    }

    //! get first cell couplet
    cell_couplet cell_triplet::cca()
    {
      cell_couplet cc1(cb_, ca_, print_level(), probmin());
      return cc1;
    }      

    //! get second cell couplet
    cell_couplet cell_triplet::ccb()
    {
      cell_couplet cc2(cb_, cc_, print_level(), probmin());
      return cc2;
    }      

    //! get joints
    const std::vector<joint>& cell_triplet::joints() const
    {
      return joints_;
    }

    //! get first cell
    const cell& cell_triplet::ca()const
    {
      return ca_;
    }      

    //! get second cell
    const cell& cell_triplet::cb()const
    {
      return cb_;
    }      

    //! get third cell
    const cell& cell_triplet::cc()const
    {
      return cc_;
    }      

    //! get list of chi2
    const std::vector<double>& cell_triplet::chi2s() const
    {
      return chi2s_;
    }      

    //! get list of prob
    const std::vector<double>& cell_triplet::probs() const
    {
      return probs_;
    }      


    //! get free level
    bool cell_triplet::free()const{
      return free_;
    }

    //! get begun level
    bool cell_triplet::begun()const{
      return begun_;
    }

    void cell_triplet::calculate_joints(double Ratio, double separation_limit, double phi_limit, double theta_limit){

      if( print_level() > mybhep::VERBOSE ){
        std::clog << appname_ << " calculate joints for cells: " << ca_.id() << " " << cb_.id() << " " << cc_.id() << std::endl;
      }

      joints_.clear();
      std::vector<line> t1 = cca().tangents(); // note: this tangent goes from cell B to cell A
      std::vector<line> t2 = ccb().tangents(); // this goes from B to C
      bool intersect_ab = ca_.intersect(cb_);
      bool intersect_bc = cb_.intersect(cc_);
      bool intersect_ca = cc_.intersect(ca_);


      if( print_level() > mybhep::VERBOSE ){
        std::clog << appname_ << " angles of tangents " << ca_.id() << " -> " << cb_.id() << " :" << std::endl;
	for(std::vector<line>::iterator i1=t1.begin(); i1!=t1.end(); ++i1){
	  std::clog << i1 - t1.begin() << ":  phi "; ca_.dump_point_phi(i1->epb()); std::clog << " -> "; cb_.dump_point_phi(i1->epa()); std::clog << " " << std::endl;
	}
        std::clog << appname_ << " angles of tangents " << cb_.id() << " -> " << cc_.id() << " :" << std::endl;
	for(std::vector<line>::iterator i2=t2.begin(); i2!=t2.end(); ++i2){
	  std::clog << i2 - t2.begin() << ":  phi ";  cb_.dump_point_phi(i2->epa()); std::clog << " -> " ; cc_.dump_point_phi(i2->epb()); std::clog << " " << std::endl;
	}
	if( ca_.small() ) std::clog << " cell " << ca_.id() << " is small " << std::endl;
	if( cb_.small() ) std::clog << " cell " << cb_.id() << " is small " << std::endl;
	if( cc_.small() ) std::clog << " cell " << cc_.id() << " is small " << std::endl;
	if( intersect_ab ) std::clog << " cells " << ca_.id() << " and " << cb_.id() << " intersect " << std::endl;
	if( intersect_bc ) std::clog << " cells " << cb_.id() << " and " << cc_.id() << " intersect " << std::endl;
	if( intersect_ca ) std::clog << " cells " << cc_.id() << " and " << ca_.id() << " intersect " << std::endl;
      }


      bool use_ownerror = true;

      experimental_double local_separation;
      bool shall_include_separation;
      experimental_vector a0, a, b0, b, c0, c, d0, d;
      double psc, psc2, psc3, psc4;
      int32_t ndof;
      experimental_point p;
      experimental_double newxa, newza;
      double chi2, local_prob;
      bool ok, use_theta_kink;
      experimental_double phi_kink, theta_kink;

      for(std::vector<line>::iterator i1=t1.begin(); i1!=t1.end(); ++i1){

        for(std::vector<line>::iterator i2=t2.begin(); i2!=t2.end(); ++i2){

          if( print_level() > mybhep::VERBOSE ){
            std::clog << " tangents " << i1 - t1.begin() << " and " << i2 - t2.begin() << std::endl;
          }

          shall_include_separation = true;
	  a0 = i1->forward_axis();
	  a = a0.hor();
	  d0 = i2->forward_axis();
	  d = d0.hor();

	  // keep only the connections that don't invert foward sense
	  psc3 = a.kink_phi(d).value()*180./M_PI;
	  
	  if( fabs(psc3) < 60. ){
	    if( print_level() > mybhep::VERBOSE ){
	      std::clog << " rejected because direction is reversed: psc = " << psc3 << std::endl;
	    }
	    continue;
	  }


          if( cb_.small() ){
            if( print_level() > mybhep::VERBOSE ){
              std::clog << " no separation: middle cells is small ";
            }
            shall_include_separation = false;
          }
          else if( intersect_ab ){
            b0 = cca().forward_axis();
            b = b0.hor();
	    psc = a.kink_phi(b).value()*180./M_PI;

            if( fabs(psc - 90.) < 30. ||  fabs(psc + 90.) < 30.  ||  fabs(psc - 270.) < 30.  ){ // connection along the intersection
	      /*
              if( print_level() > mybhep::VERBOSE ){
                std::clog << " no separation: connect along intersection AB ";
              }
              shall_include_separation = false;
	      */
              // keep only the connection with consistent ordering of cells
              c0 = ccb().forward_axis();
              c = c0.hor();
	      psc2 = b.kink_phi(c).value()*180./M_PI;
              if( fabs(psc2) < 60. ){
                if( print_level() > mybhep::VERBOSE ){
                  std::clog << " rejected because first 2 cells intersect and the ordering is wrong: psc = " << psc2 << std::endl;
                }
                continue;
              }
              
            } 
          }
          else if( intersect_bc ){
            b0 = ccb().forward_axis();
            b = b0.hor();
	    psc = d.kink_phi(b).value()*180./M_PI;
            if( fabs(psc - 90.) < 30. ||  fabs(psc + 90.) < 30.  ||  fabs(psc - 270.) < 30.  ){ // connection along the intersection
	      /*
              if( print_level() > mybhep::VERBOSE ){
                std::clog << " no separation: connect along intersection BC ";
              }
              shall_include_separation = false;
	      */
              // keep only the connection with consistent ordering of cells
              c0 = cca().forward_axis();
              c = c0.hor();
	      psc2 = b.kink_phi(c).value()*180./M_PI;
              if( fabs(psc2) < 60. ){
                if( print_level() > mybhep::VERBOSE ){
                  std::clog << " rejected because last 2 cells intersect and the ordering is wrong: psc = " << psc2 << std::endl;
                }
                continue;
              }

            }
	    
          }
	          
          ndof = 2;  // 2 kink angles, 0 or 1 one separation angle
          if( shall_include_separation )
            ndof ++;

          if( cb_.small() ){
            p = cb_.ep();
            newxa = p.x();
            newxa.set_error(cb_.r().error());
            newza = p.z();
            newza.set_error(cb_.r().error());
            p.set_x(newxa);
            p.set_z(newza);
          }
          else{
            p = cb_.angular_average(i1->epa(), i2->epa(), &local_separation);
          }

          line newt1(i1->epb(), p, print_level(), get_probmin());
          line newt2(p, i2->epb(), print_level(), get_probmin());

          if( print_level() > mybhep::VERBOSE ){
            std::clog << " p1: phi = "; ca_.dump_point(i1->epb()); std::clog << " " << std::endl;
            std::clog << " p2 average point: "; cb_.dump_point(p); std::clog << " " << std::endl;
            std::clog << " p3: "; cc_.dump_point(i2->epb()); std::clog << " " << std::endl;
            std::clog << "    separation: "; (local_separation*180/M_PI).dump(); std::clog << " " << std::endl;
          }

	  chi2 = 0.;
	  ok = false;

          if( !use_ownerror ){
            if( fabs(local_separation.value()) <= separation_limit ){

              phi_kink = newt1.kink_phi(newt2);
              theta_kink = newt1.kink_theta(newt2);
              if( print_level() > mybhep::VERBOSE ){
                std::clog << " phi 1st tangent: "; (newt1.phi()*180./M_PI).dump();
                std::clog << " phi 2nd tangent: "; (newt2.phi()*180./M_PI).dump();
                std::clog << "    phi_kink " << phi_kink.value()*180/M_PI << " +- " << phi_kink.error()*180/M_PI << std::endl;
                std::clog << " theta 1st tangent: "; (newt1.theta()*180./M_PI).dump();
                std::clog << " theta 2nd tangent: "; (newt2.theta()*180./M_PI).dump();
                std::clog << "    theta_kink " << theta_kink.value()*180/M_PI << " +- " << theta_kink.error()*180/M_PI << std::endl;
              }
              
              if(  (fabs(phi_kink.value()) <= phi_limit ) && (fabs((newt1.kink_theta(newt2)).value()) <= theta_limit ) )
                ok = true;
            }
          }
          else{
            use_theta_kink = !(ca_.unknown_vertical() || cb_.unknown_vertical() || cc_.unknown_vertical());
            if( !use_theta_kink ) ndof --;

            chi2 = newt1.chi2(newt2, use_theta_kink);

            if( shall_include_separation )
              chi2 += square(local_separation.value()/local_separation.error());

            chi2s_.push_back(chi2);
            local_prob = probof(chi2, ndof);
            probs_.push_back(local_prob);
            if( local_prob > probmin() )
              ok = true;

            if( print_level() > mybhep::VERBOSE ){
              std::clog << "    chi2 " << chi2 << " prob " << probof(chi2, ndof) << " accepted: " << ok << std::endl;
            }

          }

          if( ok ){
            joint j(newt1.epa(),p,newt2.epb(), print_level(), get_probmin());
            j.set_chi2(chi2);
            joints_.push_back(j);
          }
        }  
        
      }


      joints_ = refine(joints_, Ratio);

      return;

    }

    std::vector<joint> cell_triplet::refine(const std::vector<joint> & joints, double Ratio)
    {
      std::vector<joint> _joints;

      if( print_level() > mybhep::VERBOSE ){
        std::clog << " refining " << joints.size() << " joints " << std::endl;
      }
      experimental_double delta_phi;
      bool found;
      for(std::vector<joint>::const_iterator ijoint=joints.begin(); ijoint != joints.end(); ++ijoint){
        found = false;
        for(std::vector<joint>::const_iterator jjoint=joints.begin(); jjoint != joints.end(); ++jjoint){
          if( jjoint == ijoint ) continue;
          
          if( ca_.same_quadrant(ijoint->epa(), jjoint->epa() ) &&
              !cb_.same_quadrant(ijoint->epb(), jjoint->epb() ) &&
              cc_.same_quadrant(ijoint->epc(), jjoint->epc() ) ){
            delta_phi = experimental_fabs(ijoint->kink_phi()) - experimental_fabs(jjoint->kink_phi());
            if( delta_phi.value() > delta_phi.error() ){  // if ijoint has larger kink than jjoint
              if( print_level() > mybhep::VERBOSE ){
                std::clog << " ... removing joint " << ijoint - joints.begin()  << " because joint " << jjoint - joints.begin()  <<  " has the same initial and final quadrant, a different middle quadrant and better horizontal kink: delta phi = " << delta_phi.value() << std::endl;
              }
              found = true;
              break;
            }
          }
        }
        
        if( !found )
          _joints.push_back(*ijoint);
        
      }
      
      // order joints in order of increasing chi2 (best joint comes first)
      std::sort(_joints.begin(), _joints.end());

      // only keep 2 best joints
      if( _joints.size() > 2 ){
        _joints.erase(_joints.begin() + 2, _joints.end());
      }

      bool intersect_ab = ca_.intersect(cb_);
      bool intersect_bc = cb_.intersect(cc_);

      // delete 2nd best joint if chi2 ratio is larger than set value
      // unless joints go to different quadrants and
      // unless AB and BC both intersect; in that case force keeping 2 best joints independently of ratio
      if( _joints.size() == 2 &&
	  ca_.same_quadrant(_joints[0].epa(), _joints[1].epa()) &&
	  cc_.same_quadrant(_joints[0].epc(), _joints[1].epc()) &&
	  !(intersect_ab && intersect_bc) )
        if( _joints[1].chi2() / _joints[0].chi2() > Ratio ){
          if( print_level() > mybhep::VERBOSE ){
            std::clog << " erase 2nd best joint ( chi2 = " << _joints[1].chi2() << ") in favour of 1st best joint ( chi2 = " << _joints[0].chi2() << ") limit ratio is: " << Ratio << std::endl;
          }
          _joints.erase(_joints.begin() + 1);
        }
      
      if( print_level() > mybhep::VERBOSE ){
        std::clog << " after refining there are " << _joints.size() << " joints " << std::endl;
	for(std::vector<joint>::const_iterator ij=_joints.begin(); ij!=_joints.end(); ++ij){
	  std::clog << " joint " << ij - _joints.begin() << " : "; dump_joint(*ij);
	}
      }

      return _joints;
    }

    size_t cell_triplet::iteration()const{
      for(std::vector<joint>::const_iterator i=joints_.begin(); i!=joints_.end(); ++i)
        if( ! i->used() )
          return (size_t)(i - joints_.begin());

      return joints().size();
    }

    cell_triplet cell_triplet::invert(){
      cell_triplet inverted;
      inverted.set(ccb(),cca());
      inverted.set_free(free());
      inverted.set_begun(begun());
      inverted.set_chi2s(chi2s());
      inverted.set_probs(probs());

      std::vector<joint> inverted_joints;
      for(std::vector<joint>::iterator i=joints_.begin(); i!=joints_.end(); ++i){
        inverted_joints.push_back(i->invert());
      }
      inverted.set_joints( inverted_joints );
      return inverted;
    }


    void cell_triplet::set_all_used(){
      for(std::vector<joint>::iterator i=joints_.begin(); i!=joints_.end(); ++i)
        i->set_used(true);
      set_begun(true);
      return;
    }

    bool operator==(const cell_triplet& left,
                    const cell_triplet& right)
    {

      return ((left.ca().id() == right.ca().id()) && (left.cc().id() == right.cc().id())) ||
        ((left.ca().id() == right.cc().id()) && (left.cc().id() == right.ca().id()));

    }


    bool cell_triplet::same_last_cell(cell c)const{
      return ((this->ca().id() == c.id()) ||
              (this->cc().id() == c.id()) );
      
    }
 
     
  }

}
