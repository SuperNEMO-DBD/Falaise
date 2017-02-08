//! \file fecom/base_hit.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_BASE_HIT_HPP
#define FECOM_BASE_HIT_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/utils.hpp>

namespace fecom {

  struct base_hit
		: public datatools::i_serializable
  {
    enum hitmode_type {
      SIG_INVALID     = 0,
      SIG_CALORIMETER = 1,
      SIG_TRACKER     = 2
    };

		struct compare
		{
			bool operator()(const base_hit & a, const base_hit & b);
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

    int          hit_id;     ///< The hit ID
    hitmode_type hitmode;    ///< The hitmode (CALO or TRACKER ftm)
    uint16_t     slot_index; ///< Board logical slot (0..19)
		uint32_t     trigger_id; ///< Hit trigger ID

		DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom

#endif // FECOM_BASE_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
