//! \file fecom/base_hit.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_BASE_HIT_HPP
#define FECOM_BASE_HIT_HPP

// Standard library:
#include <string>
#include <iostream>

// This project:
#include <fecom/utils.hpp>

namespace fecom {

  struct base_hit
  {
    enum hitmode_type {
      SIG_INVALID     = 0,
      SIG_CALORIMETER = 1,
      SIG_TRACKER     = 2
    };

    base_hit();

    explicit base_hit(const hitmode_type);

    virtual ~base_hit();

    bool is_valid() const;

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

    static std::string hitmode_to_label(const hitmode_type);

  public:

    hitmode_type hitmode;
    uint16_t     slot_index; ///< Board logical slot (0..19)

  };

} // namespace fecom

#endif // FECOM_BASE_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
