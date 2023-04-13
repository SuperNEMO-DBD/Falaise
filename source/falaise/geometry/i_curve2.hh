#ifndef FALAISE_GEOMETRY_I_CURVE2_HH
#define FALAISE_GEOMETRY_I_CURVE2_HH

// Standard library:
#include <iostream>

// This project:
#include <falaise/geometry/point.hh>

namespace falaise {
  
  namespace geometry {

    /// \brief Abstract interface of a curve in the XY-plane
    struct i_curve2
    {
   
      virtual ~i_curve2() = default;

      /// Return the reference point of the curve (s=0)
      virtual point2 get_reference_point() const
      {
        point2 refPoint;
        compute(0.0, refPoint);
        return refPoint;
      }

      /// Compute the position of a vertex along the curve given its curvilinear position s w/r to a reference point
      virtual void compute(double s_, point2 & vtx_) const = 0;
 
      /// Compute a collection of vertexes vertex along the curve with their associated curvilinear positions w/r to a reference point
      virtual void generate_samples(double s1_, double s2_, polyline2 & samples_, size_t nsamples_ = 100) const = 0;
 
      /// Compute a collection of vertices along the curve with their associated curvilinear positions w/r to a reference point
      void generate_samples_step(double s1_, double s2_, polyline2 & samples_, double tstep_) const
      {
        // std::cerr << "[devel] i_curve2::generate_samples_step: t1_    = " << t1_ << '\n';
        // std::cerr << "[devel] i_curve2::generate_samples_step: t2_    = " << t2_ << '\n';
        // std::cerr << "[devel] i_curve2::generate_samples_step: tstep_ = " << tstep_ << '\n';
        size_t nbSamples = (size_t) ((s2_ - s1_) / tstep_);
        nbSamples++;
        if (nbSamples < 2) {
          nbSamples++;
        }
        // std::cerr << "[devel] i_curve2::generate_samples_step: nbSamples = " << nbSamples << '\n';
        generate_samples(s1_, s2_, samples_, nbSamples);
        return;
      }
   
    };
  
  } // namespace geometry 

} // namespace falaise 

#endif // FALAISE_GEOMETRY_I_CURVE2_HH
