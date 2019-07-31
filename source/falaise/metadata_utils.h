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
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/variant.hpp>
// - Bayeux:
#include <datatools/multi_properties.h>

namespace falaise {

namespace app {

//! \brief Metadata collector
class metadata_collector {
 public:
  //! Set the input data file from which we extract metadata
  void set_input_data_file(const std::string &filename);

  //! Set the input plain metadata file
  void set_input_metadata_file(const std::string &filename);

  //! Extract metadata from input data file (embedded metadata)
  datatools::multi_properties get_metadata_from_data_file() const;

  //! Extract metadata from input metadata file
  datatools::multi_properties get_metadata_from_metadata_file() const;

 private:
  std::string brioFile_;  //!< Input data file from which extraction of metadata is done
  std::string textFile_;  //!< Input metadata plain file
};

//! \brief Commonly used parameters extracted from input metadata
struct metadata_input {
  std::string userProfile = "";           //!< the user profile used to produce input data
  std::string experimentalSetupUrn = "";  //!< the experimental setup tag used to produce input data
  std::string variantConfigUrn = "";      //!< the variant service tag used to produce input data
  std::string variantConfigPath =
      "";  //!< the variant service configuration file path used to produce input data
  std::string variantProfileUrn = "";   //!< the variant profile tag used to produce input data
  std::string variantProfilePath = "";  //!< the variant profile file used to produce input data
  std::string servicesConfigUrn =
      "";  //!< the services configuration tag used to produce input data
  std::string servicesConfigPath =
      "";  //!< the services configuration file path used to produce input data
  std::size_t numberOfEvents = 0;  //!< the number of event after input data
  bool doSimulation = false;       //!< the flag for simulation input
  std::string simuSetupUrn = "";   //!< the simulation setup tag used to produce input data
  bool doDigitization = false;     //!< the flag for digitization input
  std::string digiSetupUrn = "";   //!< the digitization setup tag used to produce input data
  bool doReconstruction = false;   //!< the flag for reconstruction input
  std::string recSetupUrn = "";    //!< the reconstruction setup tag used to produce input data

  // Scan the source metadata container and extract parameters' values
  void scan(const datatools::multi_properties &mp);

  // Raw print
  void print(std::ostream &os) const;
};

//! \brief Metadata scanner
class metadata_scanner {
 public:
  //! Constructor
  metadata_scanner(const datatools::multi_properties &mp);

  bool hasSection(const std::string &name, const std::string &type) const;

  const datatools::properties &getSection(const std::string &name, const std::string &type) const;

  bool getBoolean(const std::string &name, const std::string &type, const std::string &key,
                  bool &value) const;

  bool getInteger(const std::string &name, const std::string &type, const std::string &key,
                  int &value) const;

  bool getSize(const std::string &name, const std::string &type, const std::string &key,
               std::size_t &value) const;

  bool getReal(const std::string &name, const std::string &type, const std::string &key,
               double &value) const;

  bool getString(const std::string &name, const std::string &type, const std::string &key,
                 std::string &value) const;

  bool getPath(const std::string &name, const std::string &type, const std::string &key,
               std::string &value) const;

 private:
  bool getDataFromSection(const std::string &name, const std::string &type, const std::string &key,
                          datatools::properties::data &value) const;

 private:
  const datatools::multi_properties &metadata_;  //!< The source input metadata
};

}  // namespace app

}  // namespace falaise

#endif  // FALAISE_APP_METADATA_UTILS_H
