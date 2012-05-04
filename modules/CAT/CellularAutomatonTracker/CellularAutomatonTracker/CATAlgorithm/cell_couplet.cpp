#include <CATAlgorithm/cell_couplet.h>

namespace CAT{
  namespace topology{

    cell_couplet::cell_couplet()
    {
      appname_= "cell_couplet: ";
      forward_axis_ = experimental_vector(small_neg,small_neg,small_neg,
                                          small_neg, small_neg, small_neg);
      transverse_axis_ = experimental_vector(small_neg,small_neg,small_neg,
                                             small_neg, small_neg, small_neg);
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      free_ = false;
      begun_ = false;
      return;
    }

    //!Default destructor
    cell_couplet::~cell_couplet()
    {
      return;
    }

    //! constructor
    cell_couplet::cell_couplet(const cell &ca,const cell &cb,const std::vector<line> &tangents){
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      for(vector<line>::const_iterator itang=tangents.begin(); itang!=tangents.end(); ++itang)
        {
          tangents_.push_back(*itang);
        }
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      set_forward_axis();
      set_transverse_axis();
      free_ = false;
      begun_ = false;
      return;
    }

    cell_couplet::cell_couplet(const cell &ca, const cell &cb, prlevel level, double nsigma){
      set_print_level(level);
      set_nsigma(nsigma);
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

    //! constructor
    cell_couplet::cell_couplet(const cell &ca, const cell &cb, const string &just, prlevel level, double nsigma){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "cell_couplet: ";
      ca_ = ca;
      cb_ = cb;
      forward_axis_calculated_ = false;
      transverse_axis_calculated_ = false;
      free_ = false;
      begun_ = false;
    }

    //! constructor from bhep hit
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
      //   R1, R2 are the vectors from center of cell to tangency point
      //   k is the vector from center of cell to center of cell
      //   t is the vector of the tangent line
      //
      //   because t is tangent:
      //   t R1 = 0
      //   t R2 = 0
      //
      //   so also:
      //   R1 R2 = +- |R1| |R2|  (sign + for parallel tangents,
      //                          sign - for crossed tangents)
      //
      //   in vectorial terms:
      //   R1 + t - R2 = k
      //
      //   taking the product with R1 and R2:
      //   |R1|^2 -+ |R1||R2| = |k||R1| cos(a1)
      //   +- |R1||R2| - |R2|^2 = |k||R2| cos(a2)
      //   so:
      //   cos(a1) = (|R1| -+ |R2|)/|k|
      //   cos(a2) = - (|R2| -+ |R1|)/|k|
      //
      //   the radial vectors are then:
      //   R1 = |R1| (f cos(a1) +- t sin(a1))  (sign + for top, sign - for bottom solutions)
      //   R2 = |R2| (f cos(a2) +- t sin(a2))
      //
      /////////////////////////////////////////////////////////////////


      experimental_double cos[2];

      cos[0] = (ca().r() - cb().r())/distance_hor();
      cos[1] = (ca().r() + cb().r())/distance_hor();

      experimental_point epa;
      experimental_point epb;

      int sign[2];
      sign[0] = 1;
      sign[1] = -1;


      // parallel tangents
      for(size_t i=0; i<2; i++){

        epa = ca_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos[0], sign[i]);
        epb = cb_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos[0], sign[i]);

        line l(epa, epb, prlevel(), nsigma());
    
        tangents_.push_back( l );
      }


      if( intersecting() ){
        // tangent or intersecting circles

        experimental_double avx = (ca().ep().x() + cb().ep().x())/2.;
        experimental_double avy = (ca().ep().y() + cb().ep().y())/2.;
        experimental_double avz = (ca().ep().z() + cb().ep().z())/2.;
        experimental_point average(avx, avy, avz);
        // a small offset with a big error
        experimental_double small_offset(0.1, (ca().r().value() + cb().r().value())/4.);
        for(size_t i=0; i<2; i++){
      
          epa = (experimental_vector(average) + transverse_axis()*sign[i]*small_offset).point_from_vector();
          epb = (experimental_vector(average) - transverse_axis()*sign[i]*small_offset).point_from_vector();

          line l(epa, epb, prlevel(), nsigma());
      
          tangents_.push_back( l );
        }

      }
      else{
        // crossed tangents
        for(size_t i=0; i<2; i++){
      
          epa = ca_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos[1], sign[i]);
          epb = cb_.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), -cos[1], -sign[i]);
      
          line l(epa, epb, prlevel(), nsigma());
      
          tangents_.push_back( l );
        }
      }
      

      return;

    }


    void cell_couplet::obtain_tangents_between_circle_and_point(const cell & c, const experimental_point & ep){

      experimental_double cos;

      cos = c.r()/distance_hor();

      experimental_point epa;

      int sign[2];
      sign[0] = 1;
      sign[1] = -1;


      for(size_t i=0; i<2; i++){

        epa = c.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos, sign[i]);

        line l(epa, ep, prlevel(), nsigma());
    
        tangents_.push_back( l );
      }


      return;

    }


    void cell_couplet::obtain_tangents_between_point_and_circle(const experimental_point & ep, const cell & c){

      experimental_double cos = - c.r()/distance_hor();

      experimental_point epb;

      int sign[2];
      sign[0] = 1;
      sign[1] = -1;


      for(size_t i=0; i<2; i++){

        epb = c.build_from_cell(forward_axis_.hor().unit(), transverse_axis(), cos, sign[i]);

        line l(ep, epb, prlevel(), nsigma());
    
        tangents_.push_back( l );
      }


      return;

    }

    /*** dump ***/
    void cell_couplet::dump (ostream & a_out,
                             const string & a_title,
                             const string & a_indent,
                             bool a_inherit ) const{
      string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << endl;
        }

      a_out << indent << appname_ << " ------------------- " << endl;
      a_out << indent  << " free: " << free() << " begun: " << begun() << endl;
      a_out << indent  << " first cell " << endl;
      this->ca().dump(a_out,"",indent + "   ");
      a_out << indent << " second cell " << endl;
      this->cb().dump(a_out, "",indent + "   ");
      a_out << indent << " tangents: " << tangents().size() << endl;
      for(vector<line>::const_iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang)
        itang->dump(a_out,"",indent + "   ");
      a_out << indent << " forward axis " << endl;
      this->forward_axis().dump(a_out,"",indent + "   ");
      a_out << indent << " transverse axis " << endl;
      this->transverse_axis().dump(a_out,"",indent + "   ");
      a_out << indent  << " ------------------- " << endl;
 
      return;
    }

    //! set cells and tangents
    void cell_couplet::set(const cell &ca, const cell &cb, const std::vector<line> &tangents){
      ca_ = ca;
      cb_ = cb;
      for(vector<line>::const_iterator itang=tangents.begin(); itang!=tangents.end(); ++itang)
        tangents_.push_back(*itang);
      set_forward_axis();
      set_transverse_axis();
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
    const bool cell_couplet::free()const{
      return free_;
    }

    //! get begun level
    const bool cell_couplet::begun()const{
      return begun_;
    }

    size_t cell_couplet::iteration()const
    {
      for(vector<line>::const_iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang)
        if( ! itang->used() )
          return itang - tangents_.begin();

      return tangents().size();
    }

    void cell_couplet::obtain_tangents_between_point_and_point(const experimental_point & epa, 
                                                               const experimental_point & epb){

      line l(epa, epb, prlevel(), nsigma());
    
      tangents_.push_back( l );

      return;

    }

    cell_couplet cell_couplet::invert(){

      //      clock.start(" cell couplet: invert ","cumulative");


      cell_couplet inverted;
      inverted.set(cb(),ca());
      inverted.set_free(free());
      inverted.set_begun(begun());

      std::vector<line> inverted_lines;
      for(vector<line>::iterator itang=tangents_.begin(); itang!=tangents_.end(); ++itang){
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

    bool cell_couplet::intersecting()const{

      return ca_.intersect(cb_);
    }


    bool operator==(const cell_couplet& left,
                    const cell_couplet& right)
    {
      return left.cb().id() == right.cb().id();
    }

  }
}

