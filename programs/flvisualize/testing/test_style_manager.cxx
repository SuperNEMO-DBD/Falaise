// -*- mode: c++ ; -*-
// test_style_manager.cxx

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <falaise/falaise.h>
#include <falaise/resource.h>
#include <EventBrowser/view/style_manager.h>

using namespace std;

int main(int argc_, char** argv_) {
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_FAILURE;
  try {
    namespace svv = snemo::visualization::view;

    // Setting style manager
    svv::style_manager& my_style_manager = svv::style_manager::get_instance();
    // my_style_manager.set_debug (debug);
    // my_style_manager.set_verbose (verbose);
    my_style_manager.set_setup_label("snemo");
    my_style_manager.initialize();
    my_style_manager.tree_dump(clog, "Original style manager settings", "NOTICE: ");

    // save settings into another file and load it
    const std::string filename = "test.sty";
    my_style_manager.dump_into_file(filename);
    my_style_manager.reset();
    my_style_manager.initialize(filename);
    my_style_manager.tree_dump(clog, "New style manager settings", "NOTICE: ");

    DT_THROW_IF(remove(filename.c_str()) != 0, std::logic_error,
                filename << " can not be deleted !");
    return EXIT_SUCCESS;
  } catch (exception& x) {
    cerr << x.what() << endl;
  } catch (...) {
    cerr << "unexpected error!" << endl;
  }
  falaise::terminate();
  return error_code;
}

// end of test_style_manager.cxx
