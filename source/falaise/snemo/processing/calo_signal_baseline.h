/// \file falaise/snemo/processing/calo_signal_baseline.h


#ifndef FALAISE_SNEMO_PROCESSING_CALO_SIGNAL_BASELINE_H
#define FALAISE_SNEMO_PROCESSING_CALO_SIGNAL_BASELINE_H 1

#include <vector>
#include <cmath>
#include <cstdint>
#include <limits>
#include <iostream>

#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/logger.h>

namespace snemo {

  namespace processing {
 
    /// \brief Calorimeter signal baseline measurement
    struct calo_signal_baseline_measurement
    {
    
      enum quality_flags {
	quality_poor = 0x1
      };
      
      std::uint16_t quality = 0;
      double value = std::numeric_limits<double>::quiet_NaN(); ///< explicit volts
      double sigma = std::numeric_limits<double>::quiet_NaN(); ///< explicit volts
      double start_time = std::numeric_limits<double>::quiet_NaN(); ///< explicit ns
      double duration = std::numeric_limits<double>::quiet_NaN(); ///< explicit ns
      std::uint16_t nb_samples = 0;
      // double noise
      
      friend std::ostream & operator<<(std::ostream & out_,
				       const calo_signal_baseline_measurement &);
    };


    typedef std::vector<std::int16_t> calo_signal_waveform_type;
    
    /// \brief Calorimeter signal baseline calculator
    struct calo_signal_baseline_calculator
    {
      calo_signal_baseline_calculator() = default;
      
      calo_signal_baseline_measurement
      operator()(const calo_signal_waveform_type & udd_calo_waveform_) const;

    public:

      datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
     
    private:

      double _calo_adczero_ = 0.0; // 2048.0;
      double _calo_adc2volt_ = (2.5/4096.0) * CLHEP::volt;
      double _calo_tdc2ns_ = 0.390625 * CLHEP::ns;
      double _min_width_time_ = 37.5 * CLHEP::ns;
      std::uint16_t _block_size_ = 16;
      double _nsigmas_ = 5.0;
      
    };
    
  }

}

#endif // FALAISE_SNEMO_PROCESSING_CALO_SIGNAL_BASELINE_H 1

