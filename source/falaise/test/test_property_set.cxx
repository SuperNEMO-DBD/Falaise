#include "catch.hpp"

#include "falaise/property_set.h"

#include <cstdio>
#include <sstream>
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/datatools/units.h"

// - Fixtures and helpers
datatools::properties makeSampleProperties() {
  datatools::properties test;
  test.store("foo", 1);
  test.store("bar", 3.14);
  test.store("baz", true);
  test.store("table.x", 4.13);
  test.store("flatstring", "foobar");
  test.store_path("apath", "foobar");
  return test;
}

datatools::properties makeTableLikeProperties() {
  datatools::properties test;

  test.store("myprop", "a property");

  test.store("myseq_i", std::vector<int>{1, 2, 3, 4});
  test.store("myseq_d", std::vector<double>{1., 2., 3., 4.});
  test.store("myseq_s", std::vector<std::string>{"hello", "world"});

  test.store("mix_table", 1);
  test.store("mix_table.x", 3.14);

  test.store("pure_table.x", 11);
  test.store("pure_table.y", 22);
  test.store("pure_table.z", 33);
  test.store("pure_tableinterfere", "badbadbad");

  test.store("pure_table.sub_table.x", 111);
  test.store("pure_table.sub_table.y", 222);
  test.store("pure_table.sub_table.z", 333);

  return test;
}

TEST_CASE("property_set default construction works", "") {
  falaise::property_set ps;
  REQUIRE(ps.is_empty());
  REQUIRE(ps.get_names().empty());
}

TEST_CASE("property_set construction from datatools work", "") {
  falaise::property_set ps{makeSampleProperties()};
  REQUIRE_FALSE(ps.is_empty());

  auto names = ps.get_names();
  REQUIRE(names.size() == 6);

  for (auto& n : names) {
    REQUIRE(ps.has_key(n));
  }

  std::cout << ps.to_string() << std::endl;
}

TEST_CASE("Observer interfaces work", "") {
  falaise::property_set ps{makeTableLikeProperties()};

  REQUIRE(ps.get_names().size() == 13);

  SECTION("properties are correctly identified") {
    REQUIRE(ps.has_key("myprop"));
    REQUIRE(ps.is_key_to_property("myprop"));

    REQUIRE(ps.is_key_to<std::string>("myprop"));
    REQUIRE_FALSE(ps.is_key_to<int>("myprop"));
    REQUIRE_FALSE(ps.is_key_to<std::string>("notpresent"));

    REQUIRE_FALSE(ps.is_key_to_property("notpresent"));
    REQUIRE_FALSE(ps.is_key_to_sequence("myprop"));
  }

  SECTION("sequences are correctly identified") {
    REQUIRE(ps.has_key("myseq_i"));
    REQUIRE(ps.is_key_to_sequence("myseq_i"));
    REQUIRE_FALSE(ps.is_key_to_sequence("notpresent"));
    REQUIRE_FALSE(ps.is_key_to_property("myseq_i"));
  }

  SECTION("mixed property/table keys are identified") {
    REQUIRE(ps.has_key("mix_table"));
    REQUIRE(ps.is_key_to_property("mix_table"));
    REQUIRE_FALSE(ps.is_key_to_property_set("mix_table"));
    REQUIRE_FALSE(ps.is_key_to_sequence("mix_table"));
  }

  SECTION("pure tables are correctly identified") {
    REQUIRE_FALSE(ps.has_key("pure_table"));
    REQUIRE(ps.is_key_to_property_set("pure_table"));
    REQUIRE_FALSE(ps.is_key_to_property_set("notpresent"));
    REQUIRE_FALSE(ps.is_key_to_property("pure_table"));
    REQUIRE_FALSE(ps.is_key_to_sequence("pure_table"));

    REQUIRE(ps.has_key("pure_table.x"));
    REQUIRE(ps.has_key("pure_table.sub_table.y"));
  }
}

TEST_CASE("Retriever interfaces work", "") {
  falaise::property_set ps{makeSampleProperties()};

  REQUIRE(ps.get<int>("foo") == 1);
  REQUIRE_THROWS(ps.get<double>("foo"));

  REQUIRE(ps.get<int>("foo", 42) == 1);
  REQUIRE(ps.get<int>("off", 42) == 42);

  REQUIRE(ps.get<double>("table.x") == Approx(4.13));

  REQUIRE_THROWS_AS(ps.get<falaise::path>("flatstring"), falaise::wrong_type_error);
  REQUIRE_NOTHROW(ps.get<falaise::path>("apath"));

  // Provide easy defaulting interfaces to handle the self default case
  // x = 2;
  // x = ps.get<T>("key", x);
  // Post-conditions are:
  // - x = 2 if "key" not in pset
  // - x = value of key if key exists and of right type
  // - throws wrong_type if key exists and not type of x
  const int kMyDefault = 415627;
  int myDefault = kMyDefault;
  SECTION("No assignment when key does not exist") {
    REQUIRE_NOTHROW(ps.assign_if("thiskeydoesnotexists", myDefault));
    REQUIRE(myDefault == kMyDefault);
  }
  SECTION("Attempted assignment with mismatched types throws") {
    REQUIRE_THROWS_AS(ps.assign_if("baz", myDefault), falaise::wrong_type_error);
    REQUIRE(myDefault == kMyDefault);
  }
  SECTION("Assignment succeeds on matching key and type") {
    REQUIRE_NOTHROW(ps.assign_if("foo", myDefault));
    REQUIRE(myDefault == ps.get<int>("foo"));
  }
}

TEST_CASE("Insertion/Erase interfaces work", "") {
  falaise::property_set ps{};
  ps.put("foo", 1);

  SECTION("putting the same key throws existing_key_error") {
    REQUIRE_THROWS_AS(ps.put("foo", 1), falaise::existing_key_error);
  }

  SECTION("replacing an existing key/value works") {
    REQUIRE_NOTHROW(ps.put_or_replace("foo", 2));
    REQUIRE(ps.get<int>("foo") == 2);

    REQUIRE_NOTHROW(ps.put_or_replace("foo", std::string{"foo"}));
    REQUIRE(ps.get<std::string>("foo") == "foo");
  }

  SECTION("putting/retriving explicit scalar/vector types works") {
    ps.put("ascalar", 1234);
    ps.put("avector", std::vector<int>{1, 2, 3, 4});

    REQUIRE_THROWS_AS(ps.get<std::vector<int>>("ascalar"), falaise::wrong_type_error);
    REQUIRE_THROWS_AS(ps.get<int>("avector"), falaise::wrong_type_error);
  }

  SECTION("putting values as subkeys works") {
    ps.put("table.x", 42);
    ps.put("table.y", 24);

    REQUIRE(ps.get<int>("table.x") == 42);
    REQUIRE(ps.get<int>("table.y") == 24);
  }
}

TEST_CASE("Path type put/get specialization works", "") {
  falaise::property_set ps{};

  SECTION("can only retrieve paths as paths") {
    falaise::path relpth{"relpath"};
    falaise::path abspth{"/tmp"};

    ps.put("my_relpath", relpth);
    ps.put("my_abspath", abspth);

    REQUIRE(ps.get<falaise::path>("my_relpath") == relpth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_relpath"), falaise::wrong_type_error);

    REQUIRE(ps.get<falaise::path>("my_abspath") == abspth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_abspath"), falaise::wrong_type_error);
  }

  SECTION("env vars are expanded on get") {
    ps.put("home", falaise::path{"$HOME"});
    REQUIRE(ps.get<falaise::path>("home") == getenv("HOME"));
  }
}

TEST_CASE("Quantity type put/get specialization works", "") {
  falaise::property_set ps;
  ps.put("number", 3.14);
  ps.put("quantity", falaise::quantity{4.13, "m"});
  ps.put("amass", falaise::mass_t{4.13, "kg"});

  REQUIRE_THROWS_AS(ps.get<falaise::quantity>("number"), falaise::wrong_type_error);
  REQUIRE_THROWS_AS(ps.get<double>("quantity"), falaise::wrong_type_error);

  REQUIRE(ps.get<falaise::quantity>("quantity").value() == Approx(4.13));
  REQUIRE(ps.get<falaise::quantity>("quantity").unit() == "m");

  falaise::length_t q;
  REQUIRE_NOTHROW(q = ps.get<falaise::length_t>("quantity"));
  REQUIRE(q.value() == Approx(4.13));
  REQUIRE(q.unit() == "m");
  REQUIRE(q.dimension() == "length");
  REQUIRE(q() == Approx(4.13 * CLHEP::m));
}

TEST_CASE("property_set type put/get specialization works", "") {
  falaise::property_set ps{makeTableLikeProperties()};

  SECTION("can only create table from pure tables") {
    REQUIRE_THROWS_AS(auto notATable = ps.get<falaise::property_set>("myprop"),
                      falaise::wrong_type_error);
    REQUIRE_THROWS_AS(auto badTable = ps.get<falaise::property_set>("mix_table"),
                      falaise::wrong_type_error);
  }

  SECTION("tables must have correct size") {
    falaise::property_set goodTable = ps.get<falaise::property_set>("pure_table");
    REQUIRE(goodTable.get_names().size() == 6);
    REQUIRE(ps.has_key("pure_table.x"));
  }

  SECTION("subtables can be extracted directly") {
    auto subTable = ps.get<falaise::property_set>("pure_table.sub_table");
    REQUIRE(subTable.get_names().size() == 3);
    REQUIRE(subTable.get<int>("x") == 111);
    REQUIRE(subTable.get<int>("y") == 222);
    REQUIRE(subTable.get<int>("z") == 333);
  }

  SECTION("subtables can be extracted indirectly") {
    auto firstTable = ps.get<falaise::property_set>("pure_table");
    auto secondTable = firstTable.get<falaise::property_set>("sub_table");
    REQUIRE(secondTable.get_names().size() == 3);
    REQUIRE(secondTable.get<int>("x") == 111);
    REQUIRE(secondTable.get<int>("y") == 222);
    REQUIRE(secondTable.get<int>("z") == 333);
  }

  SECTION("Can put property_set into another") {
    falaise::property_set sub{};
    sub.put("bar", 1);

    REQUIRE_NOTHROW(ps.put("sub", sub));
    REQUIRE(ps.is_key_to_property_set("sub"));
    REQUIRE(ps.has_key("sub.bar"));
    REQUIRE(ps.is_key_to_property("sub.bar"));

    falaise::property_set newSub{};
    newSub.put("baz", std::string("hello world"));
    newSub.put("bob", 3.14);
    REQUIRE_NOTHROW(ps.put_or_replace("sub", newSub));
    REQUIRE(ps.has_key("sub.baz"));
    REQUIRE(ps.has_key("sub.bob"));

    REQUIRE_THROWS_AS(ps.put("pure_table", newSub), falaise::existing_key_error);
    REQUIRE_NOTHROW(ps.put_or_replace("pure_table", newSub));

    auto newPs = ps.get<falaise::property_set>("pure_table");
    REQUIRE_FALSE(newPs.has_key("x"));
    REQUIRE_FALSE(newPs.is_key_to_property_set("sub_table"));
    REQUIRE(newPs.has_key("baz"));
    REQUIRE(newPs.has_key("bob"));
    REQUIRE(newPs.get<double>("bob") == Approx(3.14));
  }
}

TEST_CASE("Parameter validation works", "") {
  auto int_predicate = [](const int x) {
    return (0 <= x) && (x <= 8);
  };

  falaise::property_set integrals;
  integrals.put("in_bound", 5);
  integrals.put("out_bound", -4);

  // Required key forms
  REQUIRE(integrals.get_if<int>("in_bound", int_predicate) == 5);
  REQUIRE_THROWS_AS(integrals.get_if<int>("out_bound", int_predicate), falaise::unmet_predicate_error);
  REQUIRE_THROWS_AS(integrals.get_if<int>("badkey", int_predicate), falaise::missing_key_error);

  // Default value forms
  // No key, good default is fine
  REQUIRE(integrals.get_if<int>("badkey", 3, int_predicate) == 3);
  // Good key, bad default must throw
  REQUIRE_THROWS_AS(integrals.get_if<int>("in_bound", -4, int_predicate), falaise::unmet_predicate_error);
  // Bad key, good default must throw
  REQUIRE_THROWS_AS(integrals.get_if<int>("out_bound", 2, int_predicate), falaise::unmet_predicate_error);

  // Assign_if forms
  int myGoodParam = 7;
  int myBadParam = 9;

  // Extracting no key must no modify parameter
  REQUIRE_NOTHROW(integrals.assign_if("nokey", myGoodParam, int_predicate));
  REQUIRE(myGoodParam == 7);

  // Extracting a bad value must throw and not modify
  REQUIRE_THROWS_AS(integrals.assign_if("out_bound", myGoodParam, int_predicate), falaise::unmet_predicate_error);
  REQUIRE(myGoodParam == 7);

  // Supplying a bad value and a good key must throw and not modify
  REQUIRE_THROWS_AS(integrals.assign_if("in_bound", myBadParam, int_predicate), falaise::unmet_predicate_error);
  REQUIRE(myBadParam == 9);

  // Good parameter and good key must correctly modify and not throw
  REQUIRE_NOTHROW(integrals.assign_if("in_bound", myGoodParam, int_predicate));
  REQUIRE(myGoodParam == integrals.get<int>("in_bound"));
  // Investigate if we can make the following work with constraints:
  //uint16_t x;
  //integrals.assign_if("int32_t", x);
  //x = integrals.get<uint16_t>("int32_t");

}

TEST_CASE("Creation from file works", "") {
  std::string fname{"kakhjbfdkb.conf"};
  datatools::properties tmp{makeSampleProperties()};
  datatools::properties::write_config(fname, tmp);

  falaise::property_set ps;
  REQUIRE_NOTHROW(make_property_set(fname, ps));
  REQUIRE_FALSE(ps.is_empty());

  auto names = ps.get_names();
  REQUIRE(names.size() == 6);

  for (auto& n : names) {
    REQUIRE(ps.has_key(n));
  }

  remove(fname.c_str());
}

TEST_CASE("Creation from istream works", "") {
  // Creation from file exercises ifstream case
  // Use stringstream here
  const char* psRaw = R"ps(
    x : integer = 2
    y : string = "hello"
  )ps";
  std::istringstream iss{psRaw};

  falaise::property_set ps;
  REQUIRE_NOTHROW(make_property_set(iss, ps));

  auto names = ps.get_names();
  REQUIRE(names.size() == 2);
  REQUIRE(ps.get<int>("x") == 2);
  REQUIRE(ps.get<std::string>("y") == "hello");
}

// Use the below as examples of how datatools::properties
// units work, and how to extract info we may need for property_set
// and quantity_t
TEST_CASE("Check datatools::properties interface", "") {
  datatools::properties ps;
  std::string myprops{
      "bar : real as velocity = 3.14 m/s \n"
      "alength : real as length = 2.0 cm\n"};

  std::istringstream iput{myprops};
  datatools::properties::config reader;
  reader.read(iput, ps);

  // ps.tree_dump(std::cout);
  std::cout << ps.get_unit_symbol("alength") << "x\n";
  double val{0.0};

  // Gives the raw value in internal units (CLHEP) system
  ps.fetch("alength", val);
  std::cout << "raw fetch: " << val << std::endl;
  // This is the same, but checks that value has explicit unit (though
  // as above, this just marks it as dimensioned)
  val = ps.fetch_real_with_explicit_unit("alength");
  std::cout << "via explicit unit : " << val << std::endl;

  // If we have a unit symbol, then we can get the "label"
  std::string unit_label{};
  double cFactor{0.0};
  bool hasLabel = datatools::units::find_unit(ps.get_unit_symbol("alength"), cFactor, unit_label);
  if (hasLabel) {
    std::cout << "length has label: " << unit_label << std::endl;
  }

  // All dimensions known
  std::vector<std::string> dims{};
  size_t N = datatools::units::registered_unit_dimension_labels(dims);
  // for (auto& d : dims) {
  // std::cout << d << "\n";
  //}

  // Now in code...
  ps.store_with_explicit_unit("mylength", 1.2 * CLHEP::cm);
  ps.set_unit_symbol("mylength", "m");

  ps.fetch("mylength", val);
  std::cout << "mylength raw fetch: " << val << std::endl;
  val = ps.fetch_real_with_explicit_unit("mylength");
  std::cout << "mylength via explicit unit : " << val << std::endl;
  std::cout << "conversion from: " << datatools::units::get_unit(ps.get_unit_symbol("mylength"))
            << std::endl;
  std::cout << "so would be: " << val / datatools::units::get_unit(ps.get_unit_symbol("mylength"))
            << ps.get_unit_symbol("mylength") << std::endl;
}

TEST_CASE("Check units are read correctly", "") {
  std::string myprops{
      "aLength : real as length = 3.14 km \n"
      "anEnergy : real as energy = 50 eV\n"};

  std::istringstream iput{myprops};
  datatools::properties dps;
  datatools::properties::config reader;
  reader.read(iput, dps);
  falaise::property_set ps{dps};

  SECTION("Check Length unit conversion") {
    double myLength = ps.get<falaise::length_t>("aLength")();
    REQUIRE(myLength == Approx(3.14 * CLHEP::km));
  }

  SECTION("Check Energy unit conversion") {
    double myEnergy = ps.get<falaise::energy_t>("anEnergy")();
    REQUIRE(myEnergy == Approx(50 * CLHEP::eV));
  }

  SECTION("Check output") {
    falaise::property_set ops;
    ops.put<falaise::length_t>("foo", {3.14, "um"});
    ops.put<falaise::energy_t>("bar", {4.13, "GeV"});

    datatools::properties dOPS = ops;
    REQUIRE(dOPS.fetch_real("foo") == Approx(3.14 * CLHEP::um));
    REQUIRE(dOPS.get_unit_symbol("foo") == "um");
    REQUIRE(dOPS.fetch_real("bar") == Approx(4.13 * CLHEP::GeV));
    REQUIRE(dOPS.get_unit_symbol("bar") == "GeV");
  }
}
