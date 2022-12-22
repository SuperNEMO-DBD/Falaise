// -*- mode: c++ ; -*-
/// \file  falaise/snemo/datamodels/trigger_digitized_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 *                Guillaume Oliviero <oliviero@cenbg.in2p3.fr>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-12-21
 * Last modified: 2022-12-22
 *
 * Description:
 *
 *   SuperNEMO trigger digitized hit in the Raw Event Data (RED) bank
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRIGGER_DIGITIZED_HIT_H
#define FALAISE_SNEMO_DATAMODELS_TRIGGER_DIGITIZED_HIT_H 1

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

    /// \brief Trigger digitized hit
    class trigger_digitized_hit : public datatools::i_serializable,
                                  public datatools::i_tree_dumpable
    {
    public:

      /// \brief RTD origin (trigger_hit_record instance) of the trigger hit (for backtracing)
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

        int32_t _hit_number_ = INVALID_HIT_ID; ///< Trigger hit record number
        int32_t _trigger_id_ = INVALID_TRIGGER_ID; ///< Trigger ID

        BOOST_SERIALIZATION_BASIC_DECLARATION()
      };

      trigger_digitized_hit() = default;
      virtual ~trigger_digitized_hit() = default;

      /// Check if the internal data of the hit are valid
      bool is_valid() const;

      /// Invalidate the internal data of hit
      void invalidate();

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snemo::datamodel::trigger_digitized_hit trigDigiHit
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Trigger Digitized Hit:");
      /// poptions.put("indent", ">>> ");
      /// trigDigiHit.print_tree(std::clog, poptions);
      /// \endcode
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const;

    private:


      DATATOOLS_SERIALIZATION_DECLARATION()

    };

    /// Handle of trigger digitized hit
    using TriggerDigiHit = trigger_digitized_hit;
    using TriggerDigiHitHdl = datatools::handle<TriggerDigiHit>;

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TRIGGER_DIGITIZED_HIT_H
