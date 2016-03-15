#include <CATAlgorithm/cell_couplet.h>

namespace CAT{
  namespace topology{

    using namespace std;
    using namespace mybhep;

    cell_couplet::~cell_couplet()
    {
    }

    cell_couplet::cell_couplet()
      {
        appname_= "cell_couplet: ";
        ca_ = cell();
        cb_ = cell();
        tangents_.clear();
        forward_axis_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                            mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        transverse_axis_ = experimental_vector(mybhep::small_neg,mybhep::small_neg,mybhep::small_neg,
                                               mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
        forward_axis_calculated_ = false;
        transverse_axis_calculated_ = false;
        distance_ = experimental_double();
        distance_hor_ = experimental_double();
        free_ = false;
        begun_ = false;
      }

    cell_couplet::cell_couplet(const cell &ca,const cell &cb,const std::vector<line> &tangents){
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      for(std::vector<line>::const_iterator itang=tangents.begin(); itang!=tangents.end(); ++itang)
        tangents_.push_back(*itang);
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      free_ = false;
      begun_ = false;
    }

    cell_couplet::cell_couplet(const cell &ca, const cell &cb, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      obtain_tangents();
      free_ = false;
      begun_ = false;
    }

    cell_couplet::cell_couplet(const cell &ca, const cell &cb, const std::string & /*just*/, mybhep::prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      free_ = false;
      begun_ = false;
    }

    cell_couplet::cell_couplet(const mybhep::hit &hita, const mybhep::hit &hitb){
      appname_= "cell_couplet: ";
      ca_ = cell(hita);
      cb_ = cell(hitb);
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      obtain_tangents();
      free_ = false;
      begun_ = false;
    }

    //! set cells and tangents
    void cell_couplet::set(const cell &ca, const cell &cb, const std::vector<line> &tangents){
        ca_ = ca;
        cb_ = cb;
        for(std::vector<line>::const_iterator itang=tangents.begin(); itang!=tangents.end(); ++itang)
          tangents_.push_back(*itang);
        set_forward_axis();
        set_transverse_axis();
      }

    void cell_couplet::set_forward_axis(){

      if( forward_axis_calculated_ )
        return;

      forward_axis_.set(ca().ep(), cb().ep());

      distance_ = forward_axis().length();

      distance_hor_ = forward_axis_.hor().length();

      forward_axis_ /= distance_.value();

      forward_axis_calculated_ = true;

      return;
    }

void cell_couplet::dump (std::ostream & a_out,
                       const std::string & a_title,
                       const std::string & a_indent,
                         bool /* a_inherit */) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " ------------------- " << std::endl;
      a_out << indent  << " free: " << free() << " begun: " << begun() << std::endl;
      a_out << indent  << " first cell " << std::endl;
      this->ca().dump(a_out,"",indent + "   ");
      a_out << indent << " second cell " << std::endl;
      this->cb().dump(a_out, "",indent + "   ");
      a_out << indent << " tangents: " << tangents().size() << std::endl;
      for(std::vector<line>::const_iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang)
        itang->dump(a_out,"",indent + "   ");
      a_out << indent << " forward axis " << std::endl;
      this->forward_axis().dump(a_out,"",indent + "   ");
      a_out << indent << " transverse axis " << std::endl;
      this->transverse_axis().dump(a_out,"",indent + "   ");
      a_out << indent  << " ------------------- " << std::endl;

      return;
    }

    void cell_couplet::set_transverse_axis(){

      if( transverse_axis_calculated_ )
        return;

      experimental_vector waxis(0.,1.,0.,0.,0.,0.);

      transverse_axis_ = (forward_axis() ^ waxis).unit();

      transverse_axis_calculated_ = true;

      return;

    }

    void cell_couplet::obtain_tangents(){

      set_forward_axis();
      set_transverse_axis();


      if( ca().small() &&
          cb().small() ){
        experimental_point pa = ca().ep();
        experimental_double newxa = pa.x();
        newxa.set_error(ca().r().error());
        experimental_double newza = pa.z();
        newza.set_error(ca().r().error());
        pa.set_x(newxa);
        pa.set_z(newza);

        experimental_point pb = cb().ep();
        experimental_double newxb = pb.x();
        newxb.set_error(cb().r().error());
        experimental_double newzb = pb.z();
        newzb.set_error(cb().r().error());
        pb.set_x(newxb);
        pb.set_z(newzb);

        obtain_tangents_between_point_and_point(pa, pb);
      }
      else if( !ca().small() &&
               cb().small() ){
        experimental_point p = cb().ep();
        experimental_double newx = p.x();
        newx.set_error(cb().r().error());
        experimental_double newz = p.z();
        newz.set_error(cb().r().error());
        p.set_x(newx);
        p.set_z(newz);
        obtain_tangents_between_circle_and_point(ca(),p);
      }
      else if( ca().small() &&
               !cb().small() ){
        experimental_point p = ca().ep();
        experimental_double newx = p.x();
        newx.set_error(ca().r().error());
        experimental_double newz = p.z();
        newz.set_error(ca().r().error());
        p.set_x(newx);
        p.set_z(newz);
        obtain_tangents_between_point_and_circle(p, cb());
      }
      else
        obtain_tangents_between_circle_and_circle();


      return;

    }

    void cell_couplet::obtain_tangents_between_circle_and_circle(){

      /////////////////////////////////////////////////////////////////
      //
      //   R1, R2 are the std::vectors from center of cell to tangency point
      //   k is the std::vector from center of cell to center of cell
      //   t is the std::vector of the tangent line
      //
      //   because t is tangent:
      //   t R1 = 0
      //   t R2 = 0
      //
      //   so also:
      //   R1 R2 = +- |R1| |R2|  (sign + for parallel tangents,
      //                          sign - for crossed tangents)
      //
      //   in std::vectorial terms:
      //   R1 + t - R2 = k
      //
      //   taking the product with R1 and R2:
      //   |R1|^2 -+ |R1||R2| = |k||R1| cos(a1)
      //   +- |R1||R2| - |R2|^2 = |k||R2| cos(a2)
      //   so:
      //   cos(a1) = (|R1| -+ |R2|)/|k|
      //   cos(a2) = - (|R2| -+ |R1|)/|k|
      //
      //   the radial std::vectors are then:
      //   R1 = |R1| (f cos(a1) +- t sin(a1))  (sign + for top, sign - for bottom solutions)
      //   R2 = |R2| (f cos(a2) +- t sin(a2))
      //
      /////////////////////////////////////////////////////////////////


      int sign_parallel_crossed[2], sign_up_down[2];
      sign_parallel_crossed[0] = 1;  // parallel tangents
      sign_parallel_crossed[1] = -1; // crossed tangents
      sign_up_down[0] = 1;  // upper tangent
      sign_up_down[1] = -1;  // lower tangent


      experimental_double cosa, cosb, sina;
      experimental_point epa, epb;

      // parallel tangents
      for(size_t i=0; i<2; i++){

        cosa = (ca().r() - cb().r()*sign_parallel_crossed[0])/distance_hor();  // parallel tangents
        cosb = cosa*sign_parallel_crossed[0];
        sina = experimental_sin(experimental_acos(cosa))*sign_parallel_crossed[0]*sign_up_down[i];

        epa = ca_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cosa, sign_parallel_crossed[0]*sign_up_down[i], false, 0.);
        epb = cb_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cosb, sign_parallel_crossed[0]*sign_up_down[i], false, 0.);

        set_first_error_in_build_from_cell(sina.value(), sign_parallel_crossed[0], sign_up_down[i], &epa);
        set_second_error_in_build_from_cell(sina.value(), sign_parallel_crossed[0], sign_up_down[i], &epb);

        line l(epa, epb, prlevel(), probmin());

        tangents_.push_back( l );
      }


      if( intersecting() ){
        // tangent or intersecting circles


        experimental_vector pA = ca().ep() + ca().r()*forward_axis();
        experimental_vector pB = cb().ep() - cb().r()*forward_axis();

        experimental_vector average = (pA + pB)/2.;
        // a small offset with a big error
        //experimental_double small_offset(0.1, (ca().r().value() + cb().r().value())/4.);
        experimental_double small_offset(0.1, 0.);

        for(size_t i=0; i<2; i++){

          epa = (average + transverse_axis()*sign_up_down[i]*small_offset).point_from_vector();
          epb = (average - transverse_axis()*sign_up_down[i]*small_offset).point_from_vector();

          line l(epa, epb, prlevel(), probmin());

          tangents_.push_back( l );
        }

      }
      else{
        // crossed tangents
        for(size_t i=0; i<2; i++){

          cosa = (ca().r() - cb().r()*sign_parallel_crossed[1])/distance_hor();  // parallel tangents
          cosb = cosa*sign_parallel_crossed[1];
          sina = experimental_sin(experimental_acos(cosa))*sign_parallel_crossed[1]*sign_up_down[i];

          epa = ca_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cosa, sign_parallel_crossed[1]*sign_up_down[i], false, 0.);
          epb = cb_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cosb, sign_up_down[i], false, 0.);

          set_first_error_in_build_from_cell(sina.value(), sign_parallel_crossed[1], sign_up_down[i], &epa);
          set_second_error_in_build_from_cell(sina.value(), sign_parallel_crossed[1], sign_up_down[i], &epb);

          line l(epa, epb, prlevel(), probmin());

          tangents_.push_back( l );
        }
      }


      return;

    }


    void cell_couplet::obtain_tangents_between_circle_and_point(const cell &c, experimental_point &ep){

      // int sign_parallel_crossed[2];
      int sign_up_down[2];
      sign_up_down[0] = 1;  // upper tangent
      sign_up_down[1] = -1;  // lower tangent

      experimental_double cos, sin;

      cos = c.r()/distance_hor();

      experimental_point epa;

      for(size_t i=0; i<2; i++){

        epa = c.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos, sign_up_down[i], false, 0.);
        sin = experimental_sin(experimental_acos(cos))*sign_up_down[i];
        set_first_error_in_build_from_cell(sin.value(), 1, sign_up_down[i], &epa);

        line l(epa, ep, prlevel(), probmin());

        tangents_.push_back( l );
      }


      return;

    }


    void cell_couplet::obtain_tangents_between_point_and_circle(experimental_point &ep, const cell &c){

      // int sign_parallel_crossed[2];
      int sign_up_down[2];
      sign_up_down[0] = 1;  // upper tangent
      sign_up_down[1] = -1;  // lower tangent

      experimental_double cos, sin;

      cos = - c.r()/distance_hor();

      experimental_point epb;

      for(size_t i=0; i<2; i++){

        epb = c.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos, sign_up_down[i], false, 0.);
        sin = experimental_sin(experimental_acos(cos))*sign_up_down[i];
        set_second_error_in_build_from_cell(sin.value(), 1, sign_up_down[i], &epb);

        line l(ep, epb, prlevel(), probmin());

        tangents_.push_back( l );
      }

      return;

    }


    void cell_couplet::obtain_tangents_between_point_and_point(experimental_point &epa, experimental_point &epb){

      line l(epa, epb, prlevel(), probmin());

      tangents_.push_back( l );

      return;

    }

    void cell_couplet::set_first_error_in_build_from_cell(double sin, int sign_parallel_crossed, int sign_up_down, experimental_point *epa)const{

      // must set manually the error because factors are not independent

      experimental_vector forward = forward_axis_.hor().unit();

      experimental_vector faa = (*epa - ca_.ep())/ca_.r().value() + (forward/distance_hor().value() + transverse_axis()*sign_up_down*((sign_parallel_crossed*cb_.r().value()-ca_.r().value())/(sin*pow(distance_hor().value(),2))))*ca_.r().value();
      experimental_vector fab = (forward*sign_parallel_crossed/(-distance_hor().value()) + transverse_axis()*sign_up_down*((sign_parallel_crossed*ca_.r().value()-cb_.r().value())/(sin*pow(distance_hor().value(),2))))*ca_.r().value();

      double errax = sqrt(pow(ca_.r().error()*faa.x().value(),2) + pow(cb_.r().error()*fab.x().value(),2));
      double erraz = sqrt(pow(ca_.r().error()*faa.z().value(),2) + pow(cb_.r().error()*fab.z().value(),2));
      epa->set_ex(errax);
      epa->set_ez(erraz);

      return;

    }

    void cell_couplet::set_second_error_in_build_from_cell(double sin, int sign_parallel_crossed, int sign_up_down, experimental_point *epb)const{

      // must set manually the error because factors are not independent

      experimental_vector forward = forward_axis_.hor().unit();

      experimental_vector fba = (forward*sign_parallel_crossed/distance_hor().value() - transverse_axis()*sign_up_down*((sign_parallel_crossed*ca_.r().value()-cb_.r().value())/(sin*pow(distance_hor().value(),2))))*cb_.r().value();
      experimental_vector fbb = (*epb - cb_.ep())/cb_.r().value() + (forward/(-distance_hor().value()) - transverse_axis()*sign_up_down*((sign_parallel_crossed*cb_.r().value()-ca_.r().value())/(sin*pow(distance_hor().value(),2))))*cb_.r().value();

      double errbx = sqrt(pow(ca_.r().error()*fba.x().value(),2) + pow(cb_.r().error()*fbb.x().value(),2));
      double errbz = sqrt(pow(ca_.r().error()*fba.z().value(),2) + pow(cb_.r().error()*fbb.z().value(),2));
      epb->set_ex(errbx);
      epb->set_ez(errbz);

      return;

    }

    //! set cells
    void cell_couplet::set(const cell &ca, const cell &cb){
        ca_ = ca;
        cb_ = cb;
        obtain_tangents();
      }


    //! set free level
    void cell_couplet::set_free(bool free){
      free_ = free;
    }

    //! set begun level
    void cell_couplet::set_begun(bool begun){
      begun_ = begun;
    }

    //! set tangents
    void cell_couplet::set_tangents( const std::vector<line> &tangents)
    {
      tangents_ = tangents;
    }

    //! set fwd axis
    void cell_couplet::set_a_forward_axis(const experimental_vector &v)
    {
      forward_axis_ = v;
    }

    //! set trv axis
    void cell_couplet::set_a_transverse_axis(const experimental_vector &v)
    {
      transverse_axis_ = v;
    }

    //! set distance
    void cell_couplet::set_a_distance(const experimental_double &d)
    {
      distance_ = d;
    }

    //! set hor distance
    void cell_couplet::set_a_hor_distance(const experimental_double &d)
    {
      distance_hor_ = d;
    }

    //! get first cell
    const cell& cell_couplet::ca()const
    {
      return ca_;
    }

    //! get second cell
    const cell& cell_couplet::cb()const
    {
      return cb_;
    }

    //! get tangents
    const std::vector<line>& cell_couplet::tangents()const
    {
      return tangents_;
    }

    //! get forward axis
    const experimental_vector& cell_couplet::forward_axis()const
    {
      return forward_axis_;
    }

    //! get transverse axis
    const experimental_vector& cell_couplet::transverse_axis()const
    {
      return transverse_axis_;
    }

    //! get distance
    const experimental_double& cell_couplet::distance()const
    {
      return distance_;
    }

    //! get horizontal distance
    const experimental_double& cell_couplet::distance_hor()const
    {
      return distance_hor_;
    }

    //! get free level
    bool cell_couplet::free()const{
      return free_;
    }

    //! get begun level
    bool cell_couplet::begun()const{
      return begun_;
    }

    size_t cell_couplet::iteration()const{
      for(std::vector<line>::const_iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang)
        if( ! itang->used() )
          return itang - tangents_.begin();

      return tangents().size();
    }


    cell_couplet cell_couplet::invert(){

      //      clock.start(" cell couplet: invert ","cumulative");


      cell_couplet inverted;
      inverted.set(cb(),ca());
      inverted.set_free(free());
      inverted.set_begun(begun());

      std::vector<line> inverted_lines;
      for(std::vector<line>::iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang){
        inverted_lines.push_back(itang->invert());
      }
      inverted.set_tangents( inverted_lines );

      experimental_vector O(0.,0.,0.,0.,0.,0.);
      inverted.set_a_forward_axis(O-forward_axis());
      inverted.set_a_transverse_axis(O-transverse_axis());
      inverted.set_a_distance(-distance());
      inverted.set_a_hor_distance(-distance_hor());

      //      clock.stop(" cell couplet: invert ");

      return inverted;
    }


    void cell_couplet::set_all_used(){
      //      clock.start(" cell couplet: set all used ","cumulative");
      for(std::vector<line>::iterator itang=tangents_.begin(); itang != tangents_.end(); ++itang)
        itang->set_used(true);

      set_begun(true);
      //      clock.stop(" cell couplet: set all used ");
      return;
    }

    bool operator==(const cell_couplet& left,

                           const cell_couplet& right)
    {

      return left.cb().id() == right.cb().id();

    }

    //! are the two circles tangent or intersecting?
    bool cell_couplet::intersecting()const{

      return ca_.intersect(cb_);
    }


  }
}
