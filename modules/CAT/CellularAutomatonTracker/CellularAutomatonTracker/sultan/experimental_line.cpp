#include <sultan/experimental_line.h>

namespace SULTAN {
  namespace topology{

    void experimental_line::calculate_dir(){
      experimental_vector ab = epb_ - epa_;
      length_=ab.length();
      dir_ = ab.unit();
    }

    topology::experimental_point experimental_line::position(topology::experimental_double s)const{
      // parameter is arclength from a
      
      return (epa_ + dir_*s).point_from_vector();
      
    }

    double experimental_line::distance_from_cell_measurement(topology::cell c, experimental_double *DR, experimental_double *DH)const{
      //////////////////////////////////////////////////////////////////////////////////////
      //    foot_on_line              radius_of_cell    center_of_cell   radius_of_cell
      //          O                            (                 ep             )
      //          -------------- delta -------------------------->
      //                                       < ----- R ---------
      //
      //////////////////////////////////////////////////////////////////////////////////////

      // returns normalized parameter = 0 on epa, 1 on epb

      double parameter = distance_from_cell_center(c, DR, DH);

      *DR = experimental_fabs(*DR - c.r());
      
      return parameter;
    }
    
    double experimental_line::distance_from_cell_center(topology::cell c, experimental_double *DR, experimental_double *DH)const{
      //////////////////////////////////////////////////////////////////////////////////////
      //    foot_on_line              radius_of_cell    center_of_cell   radius_of_cell
      //          O                            (                 ep             )
      //          -------------- delta -------------------------->
      //
      //////////////////////////////////////////////////////////////////////////////////////

      // returns normalized parameter = 0 on epa, 1 on epb

      topology::experimental_vector ap = c.ep() - epa_;

      topology::experimental_double parameter_along_line=dir_*ap;

      topology::experimental_point foot = position(parameter_along_line);

      topology::experimental_vector delta = c.ep() - foot;

      *DR = delta.hor().length();
      *DH = experimental_fabs(delta.z());
      
      return parameter_along_line.value()/(length_.value());
    }
    
    
  }

}

