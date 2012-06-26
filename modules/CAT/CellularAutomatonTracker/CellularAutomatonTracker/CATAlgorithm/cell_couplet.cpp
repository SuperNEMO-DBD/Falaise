#include <CATAlgorithm/cell_couplet.h>

namespace CAT{
  namespace topology{

    using namespace std;
    using namespace mybhep;

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


    void cell_couplet::obtain_tangents_between_circle_and_point(cell c, experimental_point ep){

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


    void cell_couplet::obtain_tangents_between_point_and_circle(experimental_point ep, cell c){

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


    void cell_couplet::obtain_tangents_between_point_and_point(experimental_point epa, experimental_point epb){

      line l(epa, epb, prlevel(), nsigma());

      tangents_.push_back( l );

      return;

    }



  }
}

