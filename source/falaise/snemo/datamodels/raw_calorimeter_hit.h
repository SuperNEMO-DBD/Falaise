// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/raw_calorimeter_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-01-30
 * Last modified: 2014-01-31
 *
 * Description: The SuperNEMO raw calorimeter hit
 */

#ifndef FALAISE_SNEMO_DATAMODEL_RAW_CALORIMETER_HIT_H
#define FALAISE_SNEMO_DATAMODEL_RAW_CALORIMETER_HIT_H 1

// Standard library:
#include <bitset>
#include <vector>

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that represents a SuperNEMO raw calorimeter hit
    /*
     *   + 4095
     *   :
     *   :
     *   :                       <------------------------- Posttrig ------------------------->
     *   :                       |                                                            |
     *   :      0                |                                                           1023
     *   :      |                |               ::: :                                        |
     *   :      ----:--:-:-::--::|::::::::::::::::::::::::::--:--:::--:----:::-:----:--::------
     *   :                      :|:::::::::::::       :: :
     *   :                       +:::::::::
     *   :                       ::::::::
     *   :                        :::::
     *   :                        ::::
     *   :                         ::
     *   :                         ::
     *   :
     *   :
     *   :
     *   :
     *   :
     *   :
     *   + 0
     *
     */
    class raw_calorimeter_hit : public geomtools::base_hit
    {
    public:

      /// \brief Sampling frequencies (NemoDocDB-doc-2635-v1)
      enum sampling_frequency_type {
        SAMPLING_FREQUENCY_UNDEF   = 0,   //! Undefined sampling frequency [000]
        SAMPLING_FREQUENCY_640MHZ  = 0x1, //! Sampling frequency is 640 MHz [001]
        SAMPLING_FREQUENCY_1280MHZ = 0x2, //! Sampling frequency is 1280 MHz  [010]
        SAMPLING_FREQUENCY_1920MHZ = 0x3, //! Sampling frequency is 1920 MHz [011]
        SAMPLING_FREQUENCY_2560MHZ = 0x4, //! Sampling frequency is 2560 MHz [100]
        SAMPLING_FREQUENCY_3200MHZ = 0x5, //! Sampling frequency is 3200 MHz [101]
        SAMPLING_FREQUENCY_MASK    = 0x7  //! Bit mask for sampling frequency [111]
      };

      /// \brief Bit masks to describe data
      /**
       * Bit contents:  [UUUU UUUU UUUT DCBW]
       *
       */
      enum data_description_mask {
        DATA_DESC_UNDEFINED            = 0,     //!< The undefined data description
        DATA_DESC_SIG_HASWAVEFORM      = 0x1,   //!< The raw data stores the waveform [W]
        DATA_DESC_AUX_HASBASELINE      = 0x2,   //!< The raw data stores the computed baseline [B]
        DATA_DESC_AUX_HASPEAKCHARGE    = 0x4,   //!< The raw data stores the computed peack charge [C]
        DATA_DESC_AUX_HASPEAKDATA      = 0x8,   //!< The raw data stores other values related to the peak [D]
        DATA_DESC_AUX_HASTIMECFDINTERP = 0x10,  //!< The raw data stores the interpolated CFD timestamp [T]
        DATA_DESC_UNUSED               = 0xFFC0 //!< Unused bits [U]
      };

      /// \brief Bit masks to describe the channel address
      enum address_mask {
        ADDRESS_MODULE_ID  = 0xF800, //!< 5 bits: Module ID  = [0:19]
        ADDRESS_CRATE_ID   = 0x600,  //!< 2 bits: Crate ID   = [0:2]
        ADDRESS_BOARD_ID   = 0x1F0,  //!< 5 bits: Board ID   = [0:20]
        ADDRESS_CHANNEL_ID = 0xF     //!< 4 bits: Channel ID = [0:15]
      };

      /// \brief Bit masks do decribe the time informations
      /**
       * Bit contents:  [UUUU UUUU UUUU UUUU UUUU UUUU UUUU UUUU
       *                 UUUU UUUU UUUU UUUU UPPP PPPP PPPP PFFF]
       *
       */
      enum time_info_mask {
        TIME_INFO_UNDEFINED            = 0,      //!< The undefined time information
        TIME_INFO_SAMPLING_FREQUENCY   = 0x3,    //!< The mask for the sampling frequency [F]
        TIME_INFO_POSTTRIG             = 0xFFF8  //!< The mask for the posttrig value [P]
      };

      // Special values for TDC samples:
      static const uint16_t TDC_MIN_VALUE          = 0x0;    //!< Minimal TDC value (0)    [0000 0000 0000 0000]
      static const uint16_t TDC_MAX_VALUE          = 0x3FF;  //!< Minimal TDC value (1023) [0000 0011 1111 1111]
      static const uint16_t TDC_UNDEFINED          = 0xFFF;  //!< Undefined TDC value      [0000 1111 1111 1111]
      static const uint16_t TDC_MAX_SAMPLING_DEPTH = 1024;   //!< Maximum number of samples with the SAMLONG chip

      // Special values for ADC samples:
      static const uint16_t ADC_MIN_VALUE          = 0x0;    //!< Minimal ADC value (0)    [0000 0000 0000 0000]
      static const uint16_t ADC_MAX_VALUE          = 0xFFF;  //!< Maximal ADC value (4095) [0000 1111 1111 1111]
      static const uint16_t ADC_OVERFLOW           = 0x1000; //!< Overflow ADC value       [0001 0000 0000 0000]
      static const uint16_t ADC_UNDERFLOW          = 0x2000; //!< Underflow ADC value      [0010 0000 0000 0000]
      static const uint16_t ADC_UNDEFINED          = 0xFFFF; //!< Undefined ADC value      [1111 1111 1111 1111]

      /// Collection of waveform 16-bit samples
      typedef std::vector<uint16_t> waveform_type;

      /// Check if the hit contains valid data
      bool is_valid() const;

      /// Set the trigger ID
      void set_trigger_id(uint16_t);

      /// Return the trigger ID
      uint16_t get_trigger_id() const;

      /// Set the channel address
      void set_channel_address(uint16_t);

      /// Return the channel address
      uint16_t get_channel_address() const;

      /// Return the module ID
      uint32_t get_module_id() const;

      /// Return the crate ID
      uint32_t get_crate_id() const;

      /// Return the board ID
      uint32_t get_board_id() const;

      /// Return the channel ID
      uint32_t get_channel_id() const;

      /// Set the time information
      void set_time_info(uint64_t);

      /// Return the time information
      uint64_t get_time_info() const;

      /// Return the sampling frequency
      sampling_frequency_type get_sampling_frequency() const;

      /// Return the posttrig value
      uint16_t get_posttrig() const;

      /// Set the data description
      void set_data_description(uint16_t);

      /// Return the time_info
      uint16_t get_data_description() const;

      /// Check if waveform is available
      bool has_waveform() const;

      /// Check if baseline is available
      bool has_aux_baseline() const;

      /// Check if peak charge is available
      bool has_aux_peak_charge() const;

      /// Check if other peak data are available
      bool has_aux_peak_data() const;

      /// Check if peak CFD time (interpolation) is available
      bool has_aux_cfd_interpolated_time() const;

      /// Set the waveform first sample
      void set_waveform_first_sample(uint16_t);

      /// Return the waveform first sample
      uint16_t get_waveform_first_sample() const;

      /// Set the waveform last sample
      void set_waveform_last_sample(uint16_t);

      /// Return the waveform last sample
      uint16_t get_waveform_last_sample() const;

      /// Return the non mutable waveform collection of samples
      const waveform_type & get_waveform() const;

      /// Return the mutable waveform collection of samples
      waveform_type & grab_waveform();

      /// Set the baseline value
      void set_aux_baseline(uint16_t);

      /// Return the baseline value
      uint16_t get_aux_baseline() const;

      /// Set the peak charge value
      void set_aux_peak_charge(uint16_t);

      /// Return the peak charge value
      uint16_t get_aux_peak_charge() const;

      /// Set the peak amplitude value
      void set_aux_peak_amplitude(uint16_t);

      /// Return the peak amplitude value
      uint16_t get_aux_peak_amplitude() const;

      /// Set the peak amplitude value
      void set_aux_peak_time_max_amplitude(uint16_t);

      /// Return the peak time at max amplitude value
      uint16_t get_aux_peak_time_max_amplitude() const;

      /// Set the peak underflow tot value
      void set_aux_underflow_tot(uint16_t);

      /// Return the underflow tot value
      uint16_t get_aux_underflow_tot() const;

      /// Set the time CFD interpolation value
      void set_aux_time_cfd_interp(uint16_t);

      /// Return the time CFD interpolation value
      uint16_t get_aux_time_cfd_interp() const;

      /// Constructor
      raw_calorimeter_hit();

      /// Destructor
      virtual ~raw_calorimeter_hit();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit          = false) const;

    protected:

      /// Set default value for attributes
      void _set_defaults();

    private:

      uint16_t _trigger_id_;            //!< Trigger ID
      uint16_t _channel_address_;       //!< Encoded address of the optical module
      uint64_t _time_info_;             //!< Time informations
      uint16_t _data_description_;      //!< Description of the available data
      uint16_t _waveform_first_sample_; //!< Index of the first sample
      uint16_t _waveform_last_sample_;  //!< Index of the last sample
      waveform_type _waveform_;         //!< Waveform
      uint16_t _aux_baseline_;          //!< Pedestal value from the baseline before the peak region
      uint16_t _aux_peak_charge_;       //!< Peak integrated charge
      uint16_t _aux_peak_amplitude_;    //!< Peak amplitude
      uint16_t _aux_peak_time_max_amplitude_; //!< Peak time at max amplitude
      uint16_t _aux_underflow_tot_;     //!< Underflow signal time-over-threshold
      uint16_t _aux_time_cfd_interp_;   //!< CFD interpolation

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_RAW_CALORIMETER_HIT_H
