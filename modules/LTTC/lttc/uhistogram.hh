#ifndef FALAISE_LTTC_PLUGIN_UHISTOGRAM_HH
#define FALAISE_LTTC_PLUGIN_UHISTOGRAM_HH

// Standard library:
#include <iostream>
#include <map>
#include <limits>

// Third party:
#include <datatools/utils.h>

// This project:

namespace lttc {

  class uhistogram
  {
  public:
    
    uhistogram() = default;
    
    uhistogram(const double xref_, const double xstep_);

    void initialize(const double xref_, const double xstep_);
    
    double xref() const;

    double xstep() const;

    int index(const double value_) const;

    double value(const int index_) const;

    void fill(const double value_, const double weight_ = 1.0);

    double xmin() const;

    double xmax() const;

    double count(const int index_) const;

    void clear();

    void store(std::ostream & out_) const;
    
  private:

    // Configuration:
    double _xref_ = std::numeric_limits<double>::quiet_NaN();
    double _xstep_ = std::numeric_limits<double>::quiet_NaN();
    // Working data:
    double _xmin_ = std::numeric_limits<double>::infinity();
    double _xmax_ = -std::numeric_limits<double>::infinity();
    int _imin_ = 0;
    int _imax_ = -1;
    std::map<int, double> _counts_;
    
  };
  
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_UHISTOGRAM_HH
