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
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/bit_mask.h>
#include <datatools/utils.h>
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/sncabling_bridge.h>
#include <falaise/snemo/datamodels/snfee_bridge.h>

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

      const timestamp & get_reference_time() const;
      void set_reference_time(const timestamp & reftime_);

      const std::vector<int16_t> & get_waveform() const;
      std::vector<int16_t> & grab_waveform();

      bool is_valid() const override;
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
         STORE_DIGIFLAGS_OM_ID                  = datatools::bit_mask::bit11
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

      bool is_low_threshold_only() const;
      void set_low_threshold_only(bool);

      bool is_high_threshold() const;
      void set_high_threshold(bool);

      static const uint16_t INVALID_FCR = 0xFFFF;

      void set_fcr(uint16_t);
      uint16_t get_fcr() const;

      // Add-on (2022-02-11):
      void set_lt_trigger_counter(uint16_t);
      uint16_t get_lt_trigger_counter() const;

      // Add-on (2022-02-11):
      void set_lt_time_counter(uint32_t);
      uint32_t get_lt_time_counter() const;

      bool has_waveform() const;

      bool has_fwmeas_baseline() const;
      void set_fwmeas_baseline(int16_t);
      void reset_fwmeas_baseline();
      int16_t get_fwmeas_baseline() const;

      bool has_fwmeas_peak_amplitude() const;
      void set_fwmeas_peak_amplitude(int16_t);
      void reset_fwmeas_peak_amplitude();
      int16_t get_fwmeas_peak_amplitude() const;

      bool has_fwmeas_peak_cell() const;
      void set_fwmeas_peak_cell(int16_t);
      void reset_fwmeas_peak_cell();
      int16_t get_fwmeas_peak_cell() const;

      bool has_fwmeas_charge() const;
      void set_fwmeas_charge(int32_t);
      void reset_fwmeas_charge();
      int32_t get_fwmeas_charge() const;

      bool has_fwmeas_rising_cell() const;
      void set_fwmeas_rising_cell(int32_t);
      void reset_fwmeas_rising_cell();
      int32_t get_fwmeas_rising_cell() const;

      bool has_fwmeas_falling_cell() const;
      void set_fwmeas_falling_cell(int32_t);
      void reset_fwmeas_falling_cell();
      int32_t get_fwmeas_falling_cell() const;

      bool has_origin() const;
      const rtd_origin & get_origin() const;
      void set_origin(const rtd_origin &);
      void reset_origin();

      bool has_om_id() const;
      void set_om_id(const sncabling::om_id & om_id_);
      void reset_om_id();
      const sncabling::om_id & get_om_id() const;

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
      // timestamp            _reference_time_; ///< Reference time of the hit (40MHz main clock)
      int64_t              _timestamp_;         ///< Timestamp of the hit (40MHz main clock)
      bool                 _low_threshold_only_ = false; ///< Flag for low threshold only
      bool                 _high_threshold_ = false; ///< Flag for high threshold
      uint16_t             _fcr_ = INVALID_FCR;  ///< First cell read (10 bits)
      // Add-on (2022-02-11):
      uint16_t             _lt_trigger_counter_ = 0; ///< useful for off-line monitoring
      // Add-on (2022-02-11):
      uint32_t             _lt_time_counter_ = 0;  ///< useful for off-line monitoring
      std::vector<int16_t> _waveform_; ///< Digitized signal sampling array

      // Possibly not available attributes:
      fw_measurements_type _fwmeas_;   ///< Firmware measurements
      rtd_origin           _origin_;   ///< RTD origin of the hit
      sncabling::om_id     _om_id_;    ///< Identifier of the optical module

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_CALORIMETER_DIGITIZED_HIT_H
