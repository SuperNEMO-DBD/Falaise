// Standard library
#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/exception.h>
#include <datatools/io_factory.h>
#include <datatools/things.h>

// This project
#include "foo.h"

void test_standalone_foo()
{
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                "Running test_standalone_foo...");
  {
    // Instantiate a 'foo' object:
    falaise::example::foo f;
    // Modify the 'foo' object:
    f.set_id(3);
    f.grab_auxiliaries().store_flag("test");
    f.grab_auxiliaries().store("function", "test_standalone_foo");
    f.grab_auxiliaries().store("Pi", 3.14159);
    // Print it:
    std::cout << "Object f (class 'falaise::example::foo'): " << std::endl;
    std::cout << "|-- ID : " << f.get_id() << std::endl;
    std::cout << "`-- Auxiliaries : " << std::endl;
    f.get_auxiliaries().tree_dump(std::cout, "", "   ");
    // Serialize the 'foo' object:
    datatools::data_writer serializer("foo.xml",
                                      datatools::using_multiple_archives);
    serializer.store(f);
  }

  {
    // Instantiate a 'foo' object:
    falaise::example::foo f;
    // Deserialize the 'foo' object:
    datatools::data_reader deserializer("foo.xml",
                                        datatools::using_multiple_archives);
    if (deserializer.has_record_tag() &&
        deserializer.record_tag_is(falaise::example::foo::serial_tag())) {
      deserializer.load(f);
      // Print it:
      std::cout << "Loaded object f (class 'falaise::example::foo'): " << std::endl;
      std::cout << "|-- ID : " << f.get_id() << std::endl;
      std::cout << "`-- Auxiliaries : " << std::endl;
      f.get_auxiliaries().tree_dump(std::cout, "", "   ");
    }
  }

  return;
}

void test_bank_foo()
{
  DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                "Running test_bank_foo...");

  {
    // Instantiate a event record object:
    datatools::things event_record;
    // Add/create a 'foo' object in the "Foo" bank:
    falaise::example::foo & f = event_record.add<falaise::example::foo>("Foo");
    // Modify the 'foo' object:
    f.set_id(3);
    f.grab_auxiliaries().store_flag("test");
    f.grab_auxiliaries().store("function", "test_standalone_foo");
    f.grab_auxiliaries().store("Pi", 3.14159);
    // Print the event record:
    event_record.tree_dump(std::cout, "Event record: ");
    //  Print the 'foo' object stored in the "Foo" bank:
    std::cout << "Object in bank 'Foo' (class 'falaise::example::foo'): " << std::endl;
    std::cout << "|-- ID : " << f.get_id() << std::endl;
    std::cout << "`-- Auxiliaries : " << std::endl;
    f.get_auxiliaries().tree_dump(std::cout, "", "   ");
    // Serialize the event record:
    datatools::data_writer serializer("bank_foo.xml",
                                      datatools::using_multiple_archives);
    serializer.store(event_record);
  }


  {
    // Instantiate a event record object:
    datatools::things event_record;
    // Deserialize the event record:
    datatools::data_reader deserializer("bank_foo.xml",
                                        datatools::using_multiple_archives);
    if (deserializer.has_record_tag() &&
        deserializer.record_tag_is(datatools::things::serial_tag())) {
      deserializer.load(event_record);
      // Print it:
      event_record.tree_dump(std::cout, "Loaded event record: ");
      // Extract a 'foo' object from the 'Foo' bank:
      if(event_record.has("Foo") &&
         event_record.is_a<falaise::example::foo>("Foo")) {
        const falaise::example::foo & f = event_record.get<falaise::example::foo>("Foo");
        // Print it:
        std::cout << "Object in bank 'Foo' (class 'falaise::foo'): " << std::endl;
        std::cout << "|-- ID : " << f.get_id() << std::endl;
        std::cout << "`-- Auxiliaries : " << std::endl;
        f.get_auxiliaries().tree_dump(std::cout, "", "   ");
      }
    }
  }

  return;
}

int main(int argc_, char* argv_[]) {
  int error_code = EXIT_SUCCESS;
  try {
    test_standalone_foo();
    test_bank_foo();
  }
  catch (std::exception & error) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
