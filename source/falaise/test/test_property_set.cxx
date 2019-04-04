#include "catch.hpp"

#include "falaise/config/property_set.h"

#include <cstdio>
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/datatools/units.h"

// - Fixtures and helpers
datatools::properties makeSampleProperties() {
  datatools::properties test;
  test.store("foo", 1);
  test.store("bar", 3.14);
  test.store("baz", true);
  test.store("flatstring", "foobar");
  test.store_path("apath", "foobar");
  return test;
}

TEST_CASE("property_set default construction works", "") {
  falaise::config::property_set ps;
  REQUIRE(ps.is_empty());
  REQUIRE(ps.get_names() == std::vector<std::string>{});
}

TEST_CASE("property_set construction from datatools work", "") {
  falaise::config::property_set ps{makeSampleProperties()};
  REQUIRE(!ps.is_empty());

  auto names = ps.get_names();
  REQUIRE(names.size() == 5);

  for (auto& n : names) {
    REQUIRE(ps.has_key(n));
  }

  std::cout << ps.to_string() << std::endl;
}

TEST_CASE("Retriever interfaces work", "") {
  falaise::config::property_set ps{makeSampleProperties()};

  REQUIRE(ps.get<int>("foo") == 1);
  REQUIRE_THROWS(ps.get<double>("foo"));

  REQUIRE(ps.get<int>("foo", 42) == 1);
  REQUIRE(ps.get<int>("off", 42) == 42);

  REQUIRE_THROWS_AS(ps.get<falaise::config::path>("flatstring"), falaise::config::wrong_type_error);
  REQUIRE_NOTHROW(ps.get<falaise::config::path>("apath"));
}

TEST_CASE("Insertion/Erase interfaces work", "") {
  falaise::config::property_set ps{};
  ps.put("foo", 1);

  SECTION("putting the same key throws existing_key_error") {
    REQUIRE_THROWS_AS(ps.put("foo", 1), falaise::config::existing_key_error);
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

    REQUIRE_THROWS_AS(ps.get<std::vector<int>>("ascalar"), falaise::config::wrong_type_error);
    REQUIRE_THROWS_AS(ps.get<int>("avector"), falaise::config::wrong_type_error);
  }
}

TEST_CASE("Path type put/get specialization works", "") {
  falaise::config::property_set ps{};

  SECTION("can only retrieve paths as paths") {
    falaise::config::path relpth{"relpath"};
    falaise::config::path abspth{"/tmp"};

    ps.put("my_relpath", relpth);
    ps.put("my_abspath", abspth);

    REQUIRE(ps.get<falaise::config::path>("my_relpath") == relpth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_relpath"), falaise::config::wrong_type_error);

    REQUIRE(ps.get<falaise::config::path>("my_abspath") == abspth);
    REQUIRE_THROWS_AS(ps.get<std::string>("my_abspath"), falaise::config::wrong_type_error);
  }

  SECTION("env vars are expanded on get") {
    ps.put("home", falaise::config::path{"$HOME"});
    REQUIRE(ps.get<falaise::config::path>("home") == getenv("HOME"));
  }
}

TEST_CASE("Quantity type put/get specialization works", "") {
  falaise::config::property_set ps;
  ps.put("number", 3.14);
  ps.put("quantity", falaise::config::quantity{4.13, "m"});
  // TODO: Cannot put quantity_t<U> type yet
  ps.put("amass", falaise::config::mass_t{4.13, "kg"});

  REQUIRE_THROWS_AS(ps.get<falaise::config::quantity>("number"), falaise::config::wrong_type_error);
  REQUIRE_THROWS_AS(ps.get<double>("quantity"), falaise::config::wrong_type_error);

  REQUIRE(ps.get<falaise::config::quantity>("quantity").value() == Approx(4.13));
  REQUIRE(ps.get<falaise::config::quantity>("quantity").unit() == "m");

  falaise::config::length_t q;
  REQUIRE_NOTHROW(q = ps.get<falaise::config::length_t>("quantity"));
  REQUIRE(q.value() == Approx(4.13));
  REQUIRE(q.unit() == "m");
  REQUIRE(q.dimension() == "length");
  REQUIRE(q() == Approx(4.13 * CLHEP::m));
}

TEST_CASE("Creation from file works", "") {
  std::string fname{"kakhjbfdkb.conf"};
  datatools::properties tmp{makeSampleProperties()};
  datatools::properties::write_config(fname, tmp);

  falaise::config::property_set ps;
  REQUIRE_NOTHROW(make_property_set(fname, ps));
  REQUIRE(!ps.is_empty());

  auto names = ps.get_names();
  REQUIRE(names.size() == 5);

  for (auto& n : names) {
    REQUIRE(ps.has_key(n));
  }

  std::cout << ps.to_string() << std::endl;

  remove(fname.c_str());
}

TEST_CASE("Check datatools::properties interface", "") {
  datatools::properties ps;
  std::string myprops{
      "bar : real as velocity = 3.14 m/s \n"
      "foo : real as length = 2.0\n"};

  std::istringstream iput{myprops};
  datatools::properties::config reader;
  reader.read(iput, ps);

  ps.store_with_explicit_unit("weight", 1.2 * CLHEP::kg);
  // Ugh, have to set unit symbol explicitly in code...
  ps.set_unit_symbol("weight", "g");

  ps.tree_dump(std::cout);
  std::cout << "weight: x" << ps.get_unit_symbol("weight") << "x\n";
  double val{0.0};

  // Gives the raw value in internal units (CLHEP) system
  ps.fetch("weight", val);
  // so must be converted
  std::cout << val / CLHEP::g << std::endl;

  // This is the same, but checks that value has explicit unit (though
  // as above, this just marks it as dimensioned)
  val = ps.fetch_real_with_explicit_unit("weight");
  std::cout << val << std::endl;

  // If we have a unit symbol, then we can get the "label"
  std::string unit_label{};
  double cFactor{0.0};
  bool hasLabel = datatools::units::find_unit(ps.get_unit_symbol("weight"), cFactor, unit_label);
  if (hasLabel) {
    std::cout << "weight has label" << unit_label << std::endl;
  }

  // All dimensions known
  std::vector<std::string> dims{};
  size_t N = datatools::units::registered_unit_dimension_labels(dims);
  for (auto& d : dims) {
    std::cout << d << "\n";
  }
}
