// -*- mode: c++ ; -*-
/// \file  falaise/snemo/datamodels/calorimeter_digitized_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 *                Guillaume Oliviero <goliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-13
 *
 * Description:
 *
 *   SuperNEMO calorimeter digitized hit in the Raw Event Data (RED) bank
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_H
#define FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_H 1

// Standard Library:
#include <vector>

// Third party:
// - Bayeux:
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/bit_mask.h>
#include <datatools/utils.h>
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/timestamp.h>
#include <falaise/snemo/datamodels/red_utils.h>

namespace snemo {

  namespace datamodel {

    /// \brief Calorimeter raw digitized hit
    class calorimeter_digitized_hit
      : public geomtools::base_hit
    {
    public:

      /// \brief RTD origin (calorimeter_hit_record instance) of the calorimeter hit (for backtracing)
      class rtd_origin
        : public datatools::i_tree_dumpable
      {
      public:

        rtd_origin() = default;
        virtual ~rtd_origin() = default;
        rtd_origin(int32_t hit_number_,
                   int32_t trigger_id_);
        int32_t get_hit_number() const;
        int32_t get_trigger_id() const;
        bool is_valid() const;
        void invalidate();
        void print_tree(std::ostream & out_ = std::clog,
                        const boost::property_tree::ptree & options_ = empty_options()) const override;

      private:

        int32_t _hit_number_ = INVALID_HIT_ID;     ///< Calo hit record number
        int32_t _trigger_id_ = INVALID_TRIGGER_ID; ///< Trigger ID

        BOOST_SERIALIZATION_BASIC_DECLARATION()

      };

      calorimeter_digitized_hit() = default;
      virtual ~calorimeter_digitized_hit() = default;

      /// Return the timestamp associated to the hit
      const int64_t & get_timestamp() const;

      /// Set the timestamp associated to the hit
      void set_timestamp(const int64_t & timestamp_);

      /// Return the waveform associated to the hit
      const std::vector<int16_t> & get_waveform() const;

      /// Return the mutable waveform associated to the hit
      std::vector<int16_t> & grab_waveform();

      /// Set the waveform associated to the hit
      void set_waveform(std::vector<int16_t> & wf_);

      /// Check if the hit has a waveform
      bool has_waveform() const;

      /// Check if the internal data of the hit are valid
      bool is_valid() const override;

      /// Invalidate the internal data of hit
      void invalidate() override;

      // \brief Bit flags of various digitization features
      enum store_digiflags_mask_type
        {
         // We start at the fourth bit because the first ones are used by the parent geomtools::base_hit class
         STORE_DIGIFLAGS_WAVEFORM               = datatools::bit_mask::bit03,
         STORE_DIGIFLAGS_FWMEAS_BASELINE        = datatools::bit_mask::bit04,
         STORE_DIGIFLAGS_FWMEAS_PEAK_AMPLITUDE  = datatools::bit_mask::bit05,
         STORE_DIGIFLAGS_FWMEAS_PEAK_CELL       = datatools::bit_mask::bit06,
         STORE_DIGIFLAGS_FWMEAS_CHARGE          = datatools::bit_mask::bit07,
         STORE_DIGIFLAGS_FWMEAS_RISING_CELL     = datatools::bit_mask::bit08,
         STORE_DIGIFLAGS_FWMEAS_FALLING_CELL    = datatools::bit_mask::bit09,
         STORE_DIGIFLAGS_ORIGIN                 = datatools::bit_mask::bit10,
        };

      /// \brief Special firmware features: measurements on waveform/peak candidate
      struct fw_measurements_type
      {
        int16_t baseline = 0;
        int16_t peak_amplitude = 0;
        int16_t peak_cell = 0;
        int32_t charge = 0;
        int32_t rising_cell = 0;
        int32_t falling_cell = 0;
      };

      /// Check the status of the low threshold only boolean
      bool is_low_threshold_only() const;\

      /// Set the low threshold only boolean
      void set_low_threshold_only(bool);

      /// Check the status of the high threshold boolean
      bool is_high_threshold() const;

      /// Set the high threshold boolean
      void set_high_threshold(bool);

      static const uint16_t INVALID_FCR = 0xFFFF;

      /// Set the First Cell Read (FCR)
      void set_fcr(uint16_t);

      /// Return the First Cell Read (FCR)
      uint16_t get_fcr() const;

      /// Set the Low Threshold counter
      void set_lt_trigger_counter(uint16_t);

      /// Return the Low Threshold counter
      uint16_t get_lt_trigger_counter() const;

      /// Set the Low Threshold time counter
      void set_lt_time_counter(uint32_t);

      /// Return the Low Threshold time counter
      uint32_t get_lt_time_counter() const;

      /// Check if the hit has a firmware measurement baseline
      bool has_fwmeas_baseline() const;

      /// Set the firmware measurement baseline
      void set_fwmeas_baseline(int16_t);

      /// Reset the firmware measurement baseline
      void reset_fwmeas_baseline();

      /// Return the firmware measurement baseline
      int16_t get_fwmeas_baseline() const;

      /// Check if the hit has a firmware measurement peak amplitude
      bool has_fwmeas_peak_amplitude() const;

      /// Set the firmware measurement peak amplitude
      void set_fwmeas_peak_amplitude(int16_t);

      /// Reset the firmware measurement peak amplitude
      void reset_fwmeas_peak_amplitude();

      /// Return the firmware measurement peak amplitude
      int16_t get_fwmeas_peak_amplitude() const;

      /// Check if the hit has a firmware measurement peak cell
      bool has_fwmeas_peak_cell() const;

      /// Set the firmware measurement peak cell
      void set_fwmeas_peak_cell(int16_t);

      /// Reset the firmware measurement peak cell
      void reset_fwmeas_peak_cell();

      /// Return the firmware measurement peak cell
      int16_t get_fwmeas_peak_cell() const;

      /// Check if the hit has a firmware measurement charge
      bool has_fwmeas_charge() const;

      /// Set the firmware measurement charge
      void set_fwmeas_charge(int32_t);

      /// Reset the firmware measurement charge
      void reset_fwmeas_charge();

      /// Return the firmware measurement charge
      int32_t get_fwmeas_charge() const;

      /// Check if the hit has a firmware measurement rising cell
      bool has_fwmeas_rising_cell() const;

      /// Set the firmware measurement rising cell
      void set_fwmeas_rising_cell(int32_t);

      /// Reset the firmware measurement rising cell
      void reset_fwmeas_rising_cell();

      /// Return the firmware measurement rising cell
      int32_t get_fwmeas_rising_cell() const;

      /// Check if the hit has a firmware measurement falling cell
      bool has_fwmeas_falling_cell() const;

      /// Set the firmware measurement falling cell
      void set_fwmeas_falling_cell(int32_t);

      /// Reset the firmware measurement falling cell
      void reset_fwmeas_falling_cell();

      /// Return the firmware measurement falling cell
      int32_t get_fwmeas_falling_cell() const;

      /// Check if the hit has a RTD origin
      bool has_origin() const;

      /// Set the RTD oigin
      void set_origin(const rtd_origin &);

      /// Return the RTD oigin
      const rtd_origin & get_origin() const;

      /// Reset the RTD origin
      void reset_origin();

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::calorimeter_digitized_hit caloDigiHit
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Calo Digi Hit:");
      /// poptions.put("indent", ">>> ");
      /// caloDigiHit.print_tree(std::clog, poptions);
      /// \endcode
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      // Mandatory attributes:
      int64_t              _timestamp_;         ///< Timestamp of the hit (40MHz main clock)
      bool                 _low_threshold_only_ = false; ///< Flag for low threshold only
      bool                 _high_threshold_ = false; ///< Flag for high threshold
      uint16_t             _fcr_ = INVALID_FCR;  ///< First cell read (10 bits)
      uint16_t             _lt_trigger_counter_ = 0; ///< useful for off-line monitoring
      uint32_t             _lt_time_counter_ = 0;  ///< useful for off-line monitoring
      std::vector<int16_t> _waveform_; ///< Digitized signal sampling array

      // Possibly not available attributes:
      fw_measurements_type _fwmeas_;   ///< Firmware measurements
      rtd_origin           _origin_;   ///< RTD origin of the hit
      // sncabling::om_id     _om_id_;    ///< Identifier of the optical module

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

    /// Handle of calibrated calorimeter hit
    // typedef datatools::handle<calibrated_calorimeter_hit> handle_type;
    /// Collection of handles of calibrated calorimeter hit
    // typedef std::vector<handle_type> collection_type;
    using CalorimeterDigiHit = calorimeter_digitized_hit;
    using CalorimeterDigiHitCollection = std::vector<CalorimeterDigiHit>;

    using CalorimeterDigiHitHdl = datatools::handle<CalorimeterDigiHit>;
    using CalorimeterDigiHitHdlCollection = std::vector<CalorimeterDigiHitHdl>;

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_H
