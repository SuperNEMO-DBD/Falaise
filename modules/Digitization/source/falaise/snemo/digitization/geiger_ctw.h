// snemo/digitization/geiger_ctw.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_CTW_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_CTW_H

// Standard library :
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/datatools/handle.h>
// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

// This project :
#include <snemo/digitization/geiger_tp_constants.h>
#include <snemo/digitization/mapping.h>

namespace snemo {

namespace digitization {

/// \brief The geiger crate trigger word (C-CTW)
class geiger_ctw : public geomtools::base_hit {
 public:
  /// \brief Masks to automatically tag the attributes to be stored
  enum store_mask_type {
    STORE_CLOCKTICK_800NS = datatools::bit_mask::bit04,  //!< Serialization mask for the clocktick
    STORE_GG_CTW = datatools::bit_mask::bit05            //!< Serialization mask for the TP
  };

  /// Size of the full ctw bitset
  static const unsigned int CTW_BITSET_FULL_SIZE = 1900;

  /// Default constructor
  geiger_ctw();

  /// Destructor
  virtual ~geiger_ctw();

  /// Preliminary initialization for ctw object
  void set_header(int32_t hit_id_, const geomtools::geom_id& electronic_id_,
                  uint32_t clocktick_800ns_);

  /// Return the timestamp of the geiger crate trigger word
  uint32_t get_clocktick_800ns() const;

  /// Set the timestamp of the geiger crate trigger word
  void set_clocktick_800ns(uint32_t value_);

  /// Check if the timestamp is set
  bool has_clocktick_800ns() const;

  /// Reset the timestamp of the geiger crate trigger word
  void reset_clocktick_800ns();

  /// Get the corresponding 100 bitset from tp for a block index in the ctw bitset
  void get_100_bits_in_ctw_word(unsigned int block_index_,
                                std::bitset<geiger::tp::FULL_SIZE>& my_bitset_) const;

  /// Set the corresponding 100 bitset from tp for a block index in the ctw bitset
  void set_100_bits_in_ctw_word(unsigned int block_index_,
                                const std::bitset<geiger::tp::FULL_SIZE>& my_bitset_);

  /// Get the corresponding 55 bitset from tp for a block index in the ctw bitset
  void get_55_bits_in_ctw_word(unsigned int block_index_,
                               std::bitset<geiger::tp::TP_SIZE>& my_bitset_) const;

  /// Set the corresponding 55 bitset from tp for a block index in the ctw bitset
  void set_55_bits_in_ctw_word(unsigned int block_index_,
                               const std::bitset<geiger::tp::TP_SIZE>& my_bitset_);

  /// Set hardware status for all geiger tp word in the ctw even if there are empty
  void set_full_hardware_status(const std::bitset<geiger::tp::THWS_SIZE>& gg_tp_hardware_status_);

  /// Check if the geiger ctw has trigger primitive value in the 1900 bits
  bool has_trigger_primitive_values() const;

 protected:
  /// Set full board id for all geiger tp word in the ctw even if there are empty
  void _set_full_board_id();

 public:
  /// Set crate id for all geiger tp word in the ctw even if there are empty
  void set_full_crate_id(const std::bitset<geiger::tp::CRATE_ID_WORD_SIZE>& gg_tp_crate_id_);

  /// Reset the geiger crate TW bitset
  void reset_tw_bitset();

  /// Return the internal state of validity of the geiger crate TW is valid
  bool is_valid() const;

  /// Reset the internal data of the geiger crate TW
  void reset();

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  uint32_t
      _clocktick_800ns_;  //!< The timestamp of the trigger primitive in main clock units (40 MHz)
  std::bitset<CTW_BITSET_FULL_SIZE> _gg_ctw_;  //!< The crate trigger word

  DATATOOLS_SERIALIZATION_DECLARATION()
};

}  // end of namespace digitization

}  // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_CTW_H */

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
