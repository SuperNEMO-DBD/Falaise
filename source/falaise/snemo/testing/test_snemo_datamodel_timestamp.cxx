// -*- mode: c++ ; -*-
// test_timestamp.cxx

// Standard library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <limits>

// Third party
// - Boost:
#include <boost/cstdint.hpp>

// This project
#include <falaise/snemo/datamodels/timestamp.h>

int main(/* int argc_, char ** argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::datamodel::timestamp'!" << std::endl;

    /*
    bool debug = false;

    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];
      if(token[0] == '-') {
        std::string option = token;
        if((option == "-d") ||(option == "--debug")) {
          debug = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }
    */

    std::clog << "Some checks:" << std::endl;
    std::clog << "  size of time_t = " << sizeof(time_t) << std::endl;
    std::clog << "  size of int64_t = " << sizeof(int64_t) << std::endl;
    std::clog << "  min of int64_t = " << std::numeric_limits<int64_t>::min() << std::endl;
    std::clog << "  max of int64_t = " << std::numeric_limits<int64_t>::max() << std::endl;
    std::clog << "  eps of int64_t = " << std::numeric_limits<int64_t>::epsilon() << std::endl;

    snemo::datamodel::timestamp ts1;
    ts1.set_seconds(1268644034);
    ts1.set_picoseconds(666);
    std::clog << "ts1 = " << ts1 << std::endl;

    snemo::datamodel::timestamp ts2;
    ts2.set_seconds(1268644034);
    ts2.set_picoseconds(346);
    std::clog << "ts2 = " << ts2 << std::endl;

    snemo::datamodel::timestamp ts3(ts1);
    std::clog << "ts3 = " << ts3 << std::endl;

    snemo::datamodel::timestamp ts4(ts2);
    ts4.set_seconds(snemo::datamodel::timestamp::INVALID_SECONDS);
    std::clog << "ts4 = " << ts4 << std::endl;

    snemo::datamodel::timestamp ts5(ts2);
    ts5.set_picoseconds(snemo::datamodel::timestamp::INVALID_PICOSECONDS);
    std::clog << "ts5 = " << ts5 << std::endl;

    snemo::datamodel::timestamp ts6;
    std::clog << "ts6 = " << ts6 << std::endl;

    snemo::datamodel::timestamp ts7(1268644034, 1204);
    std::clog << "ts7 = " << ts7 << std::endl;

    if(ts1 < ts2) {
      std::clog << "Comparison result: ts1 < ts2" << std::endl;
    } else {
      std::clog << "Comparison result: ts1 >= ts2" << std::endl;
    }

    if(ts1 == ts3) {
      std::clog << "Comparison result: ts1 == ts3" << std::endl;
    }

    std::clog << "The end." << std::endl;
  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

// end of test_timestamp.cxx
