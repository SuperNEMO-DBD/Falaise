// DumbSimulation - Trivial simulation to demo structure
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of Falaise.
//
// Falaise is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Falaise is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Falaise.  If not, see <http://www.gnu.org/licenses/>.

// Standard Library
#include <fstream>
#include <ctime>

// Third Party
// - Bayeux
#include <bayeux/datatools/logger.h>

// This Project

//! Dumb simulation engine
class DumbSimulation {
 public:
  DumbSimulation() {;}

  //! configure simulation
  void doConfigure(const std::string& file, size_t n, 
                   datatools::logger::priority p) {
    fileName_ = file;
    eventsToGenerate_ = n;
    logLevel_ = p;
  }

  //! perform simulation
  int doSimulate() {
    DT_LOG_INFORMATION(logLevel_,"simulation startup");
    int errCode(2);

    if (fileName_.empty()) {
      errCode = this->doSimulateImpl(std::cout, eventsToGenerate_);
    } else {
      std::ofstream ofs;
      ofs.open(fileName_.c_str());
      if (ofs.is_open()) {
        errCode = this->doSimulateImpl(ofs, eventsToGenerate_);
        ofs.close();
      }
    }
    DT_LOG_INFORMATION(logLevel_,"simulation complete");
    return errCode;
  }

 private:
    int doSimulateImpl(std::ostream& os, size_t n) {
      DT_LOG_INFORMATION(logLevel_,"starting " << n << " events");
      char code;
      for (size_t i = 0; i < n; ++i) {
        code = static_cast<char>(32 + 145*(rand()/double(RAND_MAX)));
        os << code;
      }
      return 0;
    }

 private:
  std::string fileName_;
  size_t eventsToGenerate_;
  datatools::logger::priority logLevel_;
};


