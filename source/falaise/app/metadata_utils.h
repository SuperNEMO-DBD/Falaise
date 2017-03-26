//! \file  falaise/app/metadata_utils.h
//! \brief Utilities for accessing metadata
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by The Université de Caen Normandie
//
// This file is part of falaise.
//
// falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with falaise.  If not, see <http://www.gnu.org/licenses/>.

#ifndef FALAISE_APP_METADATA_UTILS_H
#define FALAISE_APP_METADATA_UTILS_H

// Standard library:
#include <string>
#include <iostream>

// Third party:
// - Boost:
#include <boost/variant.hpp>
// - Bayeux:
#include <datatools/multi_properties.h>

namespace falaise {

  namespace app {

    //! \brief Metadata collector
    class metadata_collector
    {
    public:

      //! Constructor
      metadata_collector(const uint32_t flags_ = 0);

      //! Set the input data file from which we extract metadata
      void set_input_data_file(const std::string &);

      //! Set the input plain metadata file
      void set_input_metadata_file(const std::string &);

      //! Extract metadata from input data file (embedded metadata)
      datatools::multi_properties get_metadata_from_data_file() const;

      //! Extract metadata from input metadata file
      datatools::multi_properties get_metadata_from_metadata_file() const;

      //! Extract metadata from input metadata file
      datatools::multi_properties get_metadata() const;

    private:

      uint32_t    _flags_;               //!< Unused flags
      std::string _input_data_file_;     //!< Input data file from which extraction of metadata is done
      std::string _input_metadata_file_; //!< Input metadata plain file

    };

    //! \brief Commonly used parameters extracted from input metadata
    struct metadata_input
    {
      std::string userProfile = "";          //!< the user profile used to produce input data
      std::string experimentalSetupUrn = ""; //!< the experimental setup tag used to produce input data
      std::string variantConfigUrn = "";     //!< the variant service tag used to produce input data
      std::string variantConfigPath = "";    //!< the variant service configuration file path used to produce input data
      std::string variantProfileUrn = "";    //!< the variant profile tag used to produce input data
      std::string variantProfilePath = "";   //!< the variant profile file used to produce input data
      std::string servicesConfigUrn = "";    //!< the services configuration tag used to produce input data
      std::string servicesConfigPath = "";   //!< the services configuration file path used to produce input data
      std::size_t numberOfEvents = 0;        //!< the number of event after input data
      bool        doSimulation = false;      //!< the flag for simulation input
      std::string simuSetupUrn = "";         //!< the simulation setup tag used to produce input data
      bool        doDigitization = false;    //!< the flag for digitization input
      std::string digiSetupUrn = "";         //!< the digitization setup tag used to produce input data
      bool        doReconstruction = false;  //!< the flag for reconstruction input
      std::string recSetupUrn = "";          //!< the reconstruction setup tag used to produce input data

      // Reset parameters to default values
      void reset();

      // Scan the source metadata container and extract parameters' values
      void scan(const datatools::multi_properties &);

      // Raw print
      void print(std::ostream & out_) const;
    };

    //! \brief Metadata scanner
    class metadata_scanner {
    public:

      typedef boost::variant<bool, int, double , std::string> prop_value_type;

      //! Constructor
      metadata_scanner(const datatools::multi_properties &);

      bool check_section(const std::string & section_name_,
			 const std::string & section_type_) const;

      const datatools::properties & get_section(const std::string & section_name_,
						const std::string & section_type_) const;

      bool find_boolean(const std::string & section_name_,
			const std::string & section_type_,
			const std::string & propKey_,
			bool & propValue_) const;

      bool find_integer(const std::string & section_name_,
			const std::string & section_type_,
			const std::string & propKey_,
			int & propValue_) const;

      bool find_size(const std::string & section_name_,
		     const std::string & section_type_,
		     const std::string & propKey_,
		     std::size_t & propValue_) const;

      bool find_real(const std::string & section_name_,
		     const std::string & section_type_,
		     const std::string & propKey_,
		     double & propValue_) const;

      bool find_string(const std::string & section_name_,
		       const std::string & section_type_,
		       const std::string & propKey_,
		       std::string & propValue_) const;

      bool find_path(const std::string & section_name_,
		     const std::string & section_type_,
		     const std::string & propKey_,
		     std::string & propValue_) const;

    private:

      bool _find_data_in_section_(const std::string & section_name_,
				  const std::string & section_type_,
				  const std::string & propKey_,
				  datatools::properties::data & data_) const;

    private:

      const datatools::multi_properties & _mp_; //!< The source input metadata

    };

  } // namespace app

} // namespace falaise

#endif // FALAISE_APP_METADATA_UTILS_H
