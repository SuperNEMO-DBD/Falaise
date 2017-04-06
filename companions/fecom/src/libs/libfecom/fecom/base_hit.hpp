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
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>

// This project:
#include <fecom/utils.hpp>

namespace fecom {

	//! \brief Commissioning base hit
  struct base_hit
		: public datatools::i_serializable
  {
    enum hitmode_type {
      SIG_INVALID     = 0,
      SIG_CALORIMETER = 1,
      SIG_TRACKER     = 2
    };

		/// Compare two base hits by hit id
		struct compare
		{
			bool operator()(const base_hit & a, const base_hit & b);
		};

		/// Compare two base hits by timestamp
		struct compare_by_timestamp
		{
			static bool comparetimestamp(const base_hit & a,
																	 const base_hit & b);

			bool operator()(const base_hit & a,
											const base_hit & b)
			{
				return comparetimestamp(a, b);
			}
		};

		/// Default constructor
    base_hit();

		/// Other constructor
    explicit base_hit(const hitmode_type);

		/// Destructor
    virtual ~base_hit();

		/// Check if the base hit has geom_id
		bool has_geom_id() const;

		/// Check if the base hit is valid
    virtual bool is_valid() const;

		///
		virtual double get_timestamp() const = 0;

		/// Reset
    virtual void reset();

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

		/// Convert hitmode into string label
    static std::string hitmode_to_label(const hitmode_type);

  public:

    uint64_t     hit_id;     ///< The hit ID
    hitmode_type hitmode;    ///< The hitmode (CALO or TRACKER ftm)
		geomtools::geom_id electronic_id; ///< The electronic channel ID
		uint64_t     trigger_id; ///< Hit trigger ID

		DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom

#endif // FECOM_BASE_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
