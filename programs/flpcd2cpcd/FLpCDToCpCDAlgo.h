// FLpCDToCpCDAlgo.h - FLpCDToCpCD algorithm
//
// Copyright (c) 2024 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2024 by Université de Caen Normandie

// Distributed under the OSI-approved BSD 3-Clause License (the "License");
// see accompanying file License.txt for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the License for more information.

#ifndef FLPCDTOCPCDALGO_H
#define FLPCDTOCPCDALGO_H

// Standard library:
#include <string>
#include <vector>

// Third Party
// - Bayeux
#include "bayeux/datatools/handle.h"
#include "bayeux/datatools/things.h"

namespace FLpCDToCpCD {

  /// \brief pcd2cpcd algorithm
  class FLpCDToCpCDAlgorithm
  {
	public:
 
    typedef std::vector<datatools::handle<datatools::things>> data_records_col;
		
		FLpCDToCpCDAlgorithm();

		datatools::logger::priority verbosity() const;

		void set_verbosity(const datatools::logger::priority verbosity_);

		const std::string & eh_tag() const;
 
		const std::string & pcd_tag() const;
 
		const std::string & cpcd_tag() const;

		bool skip_cluster_without_calo_hits() const;
		
    void process(const datatools::things & input_data_record_,
								 data_records_col & output_data_records_);

  private:

		datatools::logger::priority _verbosity_ = datatools::logger::PRIO_FATAL;
    std::string _eh_tag_;
    std::string _pcd_tag_;
    std::string _cpcd_tag_;
		bool _throw_at_missing_input_ = false;
		bool _skip_cluster_without_calo_hits_ = false;
   
  };

} // namespace FLpCDToCpCD

#endif // FLPCDTOCPCDALGO_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
