// -*- mode: c++ ; -*-
/// \file  falaise/snemo/datamodels/tracker_digitized_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 *                Guillaume Oliviero <g.oliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-05-03
 * Last modified: 2022-05-13
 *
 * Description:
 *
 *   SuperNEMO tracker digitized hit in the Raw Event Data (RED) bank
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_DIGITIZED_HIT_H
#define FALAISE_SNEMO_DATAMODELS_TRACKER_DIGITIZED_HIT_H 1

// Standard Library:
#include <vector>

// Third party:
// - Bayeux:
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <geomtools/base_hit.h>

// This project:
#include <falaise/snemo/datamodels/timestamp.h>
#include <falaise/snemo/datamodels/udd_utils.h>

namespace snemo {

  namespace datamodel {

    /// \brief Tracker digitized hit
    class tracker_digitized_hit
      : public geomtools::base_hit
    {
    public:

      /// \brief RTD origin (tracker_hit_record instance) of the tracker hit (for backtracing)
      class rtd_origin
        : public datatools::i_tree_dumpable
      {
      public:

        rtd_origin() = default;
        virtual ~rtd_origin() = default;
        rtd_origin(int32_t hit_number_, int32_t trigger_id_);
        int32_t get_hit_number() const;
        int32_t get_trigger_id() const;
        bool is_valid() const;
        void invalidate();
        void print_tree(std::ostream & out_ = std::clog,
                        const boost::property_tree::ptree & options_ = empty_options()) const override;

      private:

        int32_t _hit_number_ = INVALID_HIT_ID; ///< Tracker hit record number
        int32_t _trigger_id_ = INVALID_TRIGGER_ID; ///< Trigger ID

        BOOST_SERIALIZATION_BASIC_DECLARATION()
      };

      static const uint16_t ANODE_R0 = 0; ///< Anode timestamp with low negative threshold (LNT)
      static const uint16_t ANODE_R1 = 1; ///< Anode timestamp with first high negative threshold (HNT)
      static const uint16_t ANODE_R2 = 2; ///< Anode timestamp with second high negative threshold (HNT)
      static const uint16_t ANODE_R3 = 3; ///< Anode timestamp with first high positive threshold (HPT)
      static const uint16_t ANODE_R4 = 4; ///< Anode timestamp with second high positive threshold (HPT)

      /// \brief Set of Geiger times optionally their respective  RTD origins
      class gg_times
        : public datatools::i_tree_dumpable
      {
      public:

        gg_times() = default;
        virtual ~gg_times() = default;

        void invalidate();

        bool has_anode_origin(uint16_t) const;
        const rtd_origin & get_anode_origin(uint16_t) const;
        void set_anode_origin(uint16_t, const rtd_origin &);
        bool has_anode_time(uint16_t) const;
        void set_anode_time(uint16_t, const int64_t &);
        const int64_t & get_anode_time(uint16_t) const;

        bool has_bottom_cathode_origin() const;
        const rtd_origin & get_bottom_cathode_origin() const;
        void set_bottom_cathode_origin(const rtd_origin &);
        bool has_bottom_cathode_time() const;
        void set_bottom_cathode_time(const int64_t &);
        const int64_t & get_bottom_cathode_time() const;

        bool has_top_cathode_origin() const;
        const rtd_origin & get_top_cathode_origin() const;
        void set_top_cathode_origin(const rtd_origin &);
        bool has_top_cathode_time() const;
        void set_top_cathode_time(const int64_t &);
        const int64_t & get_top_cathode_time() const;
        void print_tree(std::ostream & out_ = std::clog,
                        const boost::property_tree::ptree & options_ = empty_options()) const override;

      private:

        rtd_origin _anode_origins_[5];
        int64_t  _anode_times_[5];
        rtd_origin _bottom_cathode_origin_;
        int64_t  _bottom_cathode_time_;
        rtd_origin _top_cathode_origin_;
        int64_t  _top_cathode_time_;

        BOOST_SERIALIZATION_BASIC_DECLARATION()

      };

      tracker_digitized_hit() = default;
      virtual ~tracker_digitized_hit() = default;

      /// Return the list of timestamps associated to the Geiger hit
      const std::vector<gg_times> & get_times() const;

      /// Add Geiger timestamps
      gg_times & add_times();

      /// Return the mutable list of timestamps associated to the Geiger hit
      std::vector<gg_times> & grab_times();

      /// Check if the internal data of the hit are valid
      bool is_valid() const override;

      /// Invalidate the internal data of hit
      void invalidate() override;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snemo::datamodel::tracker_digitized_hit caloDigiHit
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Tracker Digitized Hit:");
      /// poptions.put("indent", ">>> ");
      /// caloDigiHit.print_tree(std::clog, poptions);
      /// \endcode
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

    private:

      std::vector<gg_times> _times_; ///< 48 bits used at 80 MHz

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

    /// Handle and collection of handles of tracker digitized hit
    using TrackerDigiHit = tracker_digitized_hit;
    using TrackerDigiHitCollection = std::vector<TrackerDigiHit>;

    using TrackerDigiHitHdl = datatools::handle<TrackerDigiHit>;
    using TrackerDigiHitHdlCollection = std::vector<TrackerDigiHitHdl>;

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRACKER_DIGITIZED_HIT_H
