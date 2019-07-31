#include "catch.hpp"

#include "falaise/quantity.h"

#include "bayeux/datatools/units.h"

TEST_CASE("Raw quantity construction", "") {
  falaise::quantity x{};
  REQUIRE(x() == 0.0);
  REQUIRE_THROWS_AS(falaise::quantity y(3.14, "furlong"), falaise::unknown_unit_error);
}

TEST_CASE("Raw quantity conversion", "") {
  falaise::quantity x{3.14, "GeV"};
  REQUIRE(x.dimension() == "energy");
  REQUIRE(x.unit() == "GeV");
  REQUIRE(x() == Approx(3.14 * CLHEP::GeV));
  REQUIRE(x.value_in("eV") == Approx(3.14 * CLHEP::GeV / CLHEP::eV));
  REQUIRE_THROWS_AS(x.value_in("km"), falaise::wrong_dimension_error);
}

TEST_CASE("Explicit dimensions construction", "") {
  REQUIRE_THROWS_AS(falaise::mass_t ma(3.14, "km"), falaise::wrong_dimension_error);
  REQUIRE_THROWS_AS(falaise::mass_t mb(3.14, "foo"), falaise::unknown_unit_error);

  // Default construction has to have a sensible defaults!
  falaise::length_t l{};
  REQUIRE(l() == 0.0);
  REQUIRE(l.unit() == "m");
  REQUIRE(l.dimension() == "length");

  falaise::length_t u{3.14, "m"};
  falaise::energy_t e;
  REQUIRE_THROWS_AS(e = u, falaise::wrong_dimension_error);
  REQUIRE_THROWS_AS(falaise::energy_t e2 = u, falaise::wrong_dimension_error);
  REQUIRE_THROWS_AS(falaise::mass_t v(u), falaise::wrong_dimension_error);
}

TEST_CASE("Explicit dimension conversions", "") {
  falaise::mass_t m{3.14, "GeV/c2"};
  REQUIRE(m() == Approx(3.14 * CLHEP::GeV / CLHEP::c_squared));
}

// Example of autogenerating quantity_t docs
// TODO: Split out to generator program
void foo() {
  // TEST_CASE("Doxygenate known dimensions/units", "") {
  // All dimensions known
  std::vector<std::string> dims{};
  size_t N = datatools::units::registered_unit_dimension_labels(dims);

  const auto& r = datatools::units::registry::system_registry();

  for (auto& d : dims) {
    auto& ud = r.get_dimension(d);

    std::string dimPowers{};
    ud.encode_dimensional_powers(dimPowers);

    std::cout << "/*! \\class falaise::" << d << "_t\n"
              << " *  \\brief quantity for values with dimension tag `" << d << "` (" << dimPowers
              << ")\n"
              << " *\n"
              << " * A @ref " << d << "_t value may be constructed using, for example\n"
              << " *\n"
              << " * ```cpp\n"
              << " * falaise::" << d << "_t x{3.14, \"TAG\"};\n"
              << " * ```\n"
              << " *\n"
              << " * where `TAG` is one of the following valid unit tags:\n"
              << " *\n";

    for (auto& u : ud.get_unit_names()) {
      std::cout << " * - `" << u << "`\n";
    }

    std::cout << " *\n";
    std::cout << " * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not "
                 "in the above list\n";
    std::cout << " */\n";
    std::cout << "FALAISE_ADD_DIMENSION_TAG(" << d << ");\n\n";
  }
}