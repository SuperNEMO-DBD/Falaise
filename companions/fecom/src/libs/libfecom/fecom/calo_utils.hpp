//! \file fecom/calo_utils.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_UTILS_HPP
#define FECOM_CALO_UTILS_HPP

// Standard library:
#include <iostream>

namespace fecom {

  /// \brief Calorimeter calibration manager
  struct calo_channel_id
  {
    static const int INVALID_ID = -1;
    calo_channel_id();
    calo_channel_id(const int, const int);
    bool is_valid() const;
    static int compare(const calo_channel_id & id1_,
                       const calo_channel_id & id2_);

    bool operator<(const calo_channel_id & id_) const;
    bool operator==(const calo_channel_id & id_) const;

    int board_id;
    int slot_id;

    friend std::ostream & operator<<(std::ostream &, const calo_channel_id &);

  };

} // namespace fecom

#endif // FECOM_CALO_UTILS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
