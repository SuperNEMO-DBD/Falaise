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
    cell_triplet::cell_triplet(cell_couplet cca, cell_couplet ccb){
      appname_= "cell_triplet: ";
      free_ = false;
      begun_ = false;
    }

    //! constructor
    cell_triplet::cell_triplet(cell ca, cell cb, cell cc, prlevel level, double nsigma){
      set_print_level(level);
      set_nsigma(nsigma);
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
      string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << endl;
        }

      a_out << indent << appname_ << " -------------- " << endl;
      a_out << indent  << " free: " << free() << " begun: " << begun()  << endl;
      a_out << indent  << " first cell " << endl;
      ca().dump(a_out,"", indent + "   ");
      a_out << indent << " second cell " << endl;
      cb().dump(a_out, "",indent + "   ");
      a_out << indent << " third cell " << endl;
      cc().dump(a_out, "",indent + "   ");
      for(vector<joint>::const_iterator ijoint=joints_.begin(); ijoint!=joints_.end(); ++ijoint)
        ijoint->dump(a_out,"",indent + "   ");
      a_out << indent  << " -------------- " << endl;
 
      return;
    }
    
    //! set cells
    void cell_triplet::set(cell_couplet cca, cell_couplet ccb){
      cb_ = cca.ca();
      ca_ = cca.cb();
      cc_ = ccb.cb();
      if( cca.ca().id() != ccb.ca().id() ){
        clog << " problem: trying to form a triplet of cell with cells " << cca.ca().id() << " " 
             << cca.cb().id() << " " 
             << ccb.ca().id() << " " 
             << ccb.cb().id() << endl;
      }
    }

    //! set cells
    void cell_triplet::set(cell ca, cell cb, cell cc){
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
      cell_couplet cc1(cb_, ca_, print_level(), nsigma());
      return cc1;
    }      

    //! get second cell couplet
    cell_couplet cell_triplet::ccb()
    {
      cell_couplet cc2(cb_, cc_, print_level(), nsigma());
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
        clog << appname_ << " calculate joints for cells: " << ca_.id() << " " << cb_.id() << " " << cc_.id() << endl;
      }

      joints_.clear();
      std::vector<line> t1 = cca().tangents(); // note: this tangent goes from cell B to cell A
      std::vector<line> t2 = ccb().tangents();
      if( print_level() > mybhep::VERBOSE ){
        clog << appname_ << " angles of tangents " << ca_.id() << " -> " << cb_.id() << " :" << endl;
	for(vector<line>::iterator i1=t1.begin(); i1!=t1.end(); ++i1){
	  clog << i1 - t1.begin() << ":  phi "; ca_.dump_point_phi(i1->epb()); clog << " -> "; cb_.dump_point_phi(i1->epa()); clog << " " << endl;
	}
        clog << appname_ << " angles of tangents " << cb_.id() << " -> " << cc_.id() << " :" << endl;
	for(vector<line>::iterator i2=t2.begin(); i2!=t2.end(); ++i2){
	  clog << i2 - t2.begin() << ":  phi ";  cb_.dump_point_phi(i2->epa()); clog << " -> " ; cc_.dump_point_phi(i2->epb()); clog << " " << endl;
	}
	if( ca_.small() ) clog << " cell " << ca_.id() << " is small " << endl;
	if( cb_.small() ) clog << " cell " << cb_.id() << " is small " << endl;
	if( cc_.small() ) clog << " cell " << cc_.id() << " is small " << endl;
	if( ca_.intersect(cb_) ) clog << " cells " << ca_.id() << " and " << cb_.id() << " intersect " << endl;
	if( cb_.intersect(cc_) ) clog << " cells " << cb_.id() << " and " << cc_.id() << " intersect " << endl;
	if( cc_.intersect(ca_) ) clog << " cells " << cc_.id() << " and " << ca_.id() << " intersect " << endl;
      }


      bool use_ownerror = true;

      experimental_double local_separation;
      for(vector<line>::iterator i1=t1.begin(); i1!=t1.end(); ++i1){

        for(vector<line>::iterator i2=t2.begin(); i2!=t2.end(); ++i2){

          if( print_level() > mybhep::VERBOSE ){
            clog << " tangents " << i1 - t1.begin() << " and " << i2 - t2.begin() << endl;
          }

          bool shall_include_separation = true;
          if( cb_.small() ){
            if( print_level() > mybhep::VERBOSE ){
              clog << " no separation: middle cells is small ";
            }
            shall_include_separation = false;
          }
          else if( cb_.intersect(ca_) ){
            experimental_vector a0 = i1->forward_axis();
            experimental_vector a = a0.hor();
            experimental_vector b0 = cca().forward_axis();
            experimental_vector b = b0.hor();
            double psc = (a*b).value();

            if( fabs(psc) < 0.5 ){ // connection along the intersection
              if( print_level() > mybhep::VERBOSE ){
                clog << " no separation: connect along intersection AB ";
              }
              shall_include_separation = false;

              // keep only the connection with consistent ordering of cells
              experimental_vector c0 = ccb().forward_axis();
              experimental_vector c = c0.hor();
              double psc2 = (b*c).value();
              if( psc2 > 0.1 ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because first 2 cells intersect and the ordering is wrong: psc = " << psc2 << endl;
                }
                continue;
              }
              
              // keep only the connection that doesn't invert foward sense
              experimental_vector d0 = i2->forward_axis();
              experimental_vector d = d0.hor();
              double psc3 = (a*d).value();
              if( psc3 > 0. ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because direction is reversed: psc = " << psc3 << endl;
                }
                continue;
              }
            } 
          }
          else if( cb_.intersect(cc_) ){
            experimental_vector a0 = i2->forward_axis();
            experimental_vector a = a0.hor();
            experimental_vector b0 = ccb().forward_axis();
            experimental_vector b = b0.hor();
            double psc = (a*b).value();
            if( fabs(psc) < 0.5 ){ // connection along the intersection
              if( print_level() > mybhep::VERBOSE ){
                clog << " no separation: connect along intersection BC ";
              }
              shall_include_separation = false;
              // keep only the connection with consistent ordering of cells
              experimental_vector c0 = cca().forward_axis();
              experimental_vector c = c0.hor();
              double psc2 = (b*c).value();
              if( psc2 > 0.1 ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because last 2 cells intersect and the ordering is wrong: psc = " << psc2 << endl;
                }
                continue;
              }

              // keep only the connection that doesn't invert foward sense
              experimental_vector d0 = i1->forward_axis();
              experimental_vector d = d0.hor();
              double psc3 = (a*d).value();
              if( psc3 > 0. ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because direction is reversed: psc = " << psc3 << endl;
                }
                continue;
              }

            }

          }
          
          int32_t ndof = 2;  // 2 kink angles, 0 or 1 one separation angle
          if( shall_include_separation )
            ndof ++;

          experimental_point p;
          if( cb_.small() ){
            p = cb_.ep();
            experimental_double newxa = p.x();
            newxa.set_error(cb_.r().error());
            experimental_double newza = p.z();
            newza.set_error(cb_.r().error());
            p.set_x(newxa);
            p.set_z(newza);
          }
          else{
            p = cb_.angular_average(i1->epa(), i2->epa(), &local_separation);
          }

          line newt1(i1->epb(), p, print_level(), get_nsigma());
          line newt2(p, i2->epb(), print_level(), get_nsigma());

          if( print_level() > mybhep::VERBOSE ){
            clog << " p1: phi = "; ca_.dump_point(i1->epb()); clog << " " << endl;
            clog << " p2 average point: "; cb_.dump_point(p); clog << " " << endl;
            clog << " p3: "; cc_.dump_point(i2->epb()); clog << " " << endl;
            clog << "    separation: "; (local_separation*180/M_PI).dump(); clog << " " << endl;
          }

          double chi2 = 0.;
          bool ok = false;

          if( !use_ownerror ){
            if( fabs(local_separation.value()) <= separation_limit ){

              experimental_double phi_kink = newt1.kink_phi(newt2);
              experimental_double theta_kink = newt1.kink_theta(newt2);
              
              if( print_level() > mybhep::VERBOSE ){
                clog << " phi 1st tangent: "; (newt1.phi()*180./M_PI).dump();
                clog << " phi 2nd tangent: "; (newt2.phi()*180./M_PI).dump();
                clog << "    phi_kink " << phi_kink.value()*180/M_PI << " +- " << phi_kink.error()*180/M_PI << endl;
                clog << " theta 1st tangent: "; (newt1.theta()*180./M_PI).dump();
                clog << " theta 2nd tangent: "; (newt2.theta()*180./M_PI).dump();
                clog << "    theta_kink " << theta_kink.value()*180/M_PI << " +- " << theta_kink.error()*180/M_PI << endl;
              }
              
              if(  (fabs(phi_kink.value()) <= phi_limit ) && (fabs((newt1.kink_theta(newt2)).value()) <= theta_limit ) )
                ok = true;
            }
          }
          else{
            bool use_theta_kink = !(ca_.unknown_vertical() || cb_.unknown_vertical() || cc_.unknown_vertical());
            if( !use_theta_kink ) ndof --;

            chi2 = newt1.chi2(newt2, use_theta_kink);

            if( shall_include_separation )
              chi2 += square(local_separation.value()/local_separation.error());

            chi2s_.push_back(chi2);
            double local_prob = probof(chi2, ndof);
            probs_.push_back(local_prob);
            if( local_prob > prob() )
              ok = true;

            if( print_level() > mybhep::VERBOSE ){
              clog << "    chi2 " << chi2 << " prob " << probof(chi2, ndof) << " accepted: " << ok << endl;
            }

          }

          if( ok ){
            joint j(newt1.epa(),p,newt2.epb(), print_level(), get_nsigma());
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
        clog << " refining " << joints.size() << " joints " << endl;
      }

      for(std::vector<joint>::const_iterator ijoint=joints.begin(); ijoint != joints.end(); ++ijoint){
        bool found = false;
        for(std::vector<joint>::const_iterator jjoint=joints.begin(); jjoint != joints.end(); ++jjoint){
          if( jjoint == ijoint ) continue;
          
          if( ca_.same_quadrant(ijoint->epa(), jjoint->epa() ) &&
              !cb_.same_quadrant(ijoint->epb(), jjoint->epb() ) &&
              cc_.same_quadrant(ijoint->epc(), jjoint->epc() ) ){
            experimental_double delta_phi = experimental_fabs(ijoint->kink_phi()) - experimental_fabs(jjoint->kink_phi());
            if( delta_phi.value() > delta_phi.error() ){  // if ijoint has larger kink than jjoint
              if( print_level() > mybhep::VERBOSE ){
                clog << " ... removing joint " << ijoint - joints.begin()  << " because joint " << jjoint - joints.begin()  <<  " has the same initial and final quadrant, a different middle quadrant and better horizontal kink: delta phi = " << delta_phi.value() << endl;
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


      if( _joints.size() == 2 )
        if( _joints[1].chi2() / _joints[0].chi2() > Ratio ){
          if( print_level() > mybhep::VERBOSE ){
            clog << " erase 2nd best joint ( chi2 = " << _joints[1].chi2() << ") in favour of 1st best joint ( chi2 = " << _joints[0].chi2() << ") limit ratio is: " << Ratio << endl;
          }
          _joints.erase(_joints.begin() + 1);
        }
      
      if( print_level() > mybhep::VERBOSE ){
        clog << " after refining there are " << _joints.size() << " joints " << endl;
      }

      return _joints;
    }

    size_t cell_triplet::iteration()const{
      for(vector<joint>::const_iterator i=joints_.begin(); i!=joints_.end(); ++i)
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
      for(vector<joint>::iterator i=joints_.begin(); i!=joints_.end(); ++i){
        inverted_joints.push_back(i->invert());
      }
      inverted.set_joints( inverted_joints );
      return inverted;
    }


    void cell_triplet::set_all_used(){
      for(vector<joint>::iterator i=joints_.begin(); i!=joints_.end(); ++i)
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
