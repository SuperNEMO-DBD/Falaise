//! \file quantity.h
//! \brief Types for configuration validation of physical quantities
#ifndef FALAISE_QUANTITY_H
#define FALAISE_QUANTITY_H

#include <exception>
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "bayeux/datatools/units.h"
#include "boost/metaparse/string.hpp"
#include "boost/mpl/string.hpp"

//! Register a dimension tag for use as a @ref falaise::config::quantity_t template parameter
/*!
 * Expansion defines a struct for the tag, and a type alias for convenience
 * e.g.
 *
 * ```cpp
 * FALAISE_ADD_DIMENSION_TAG(length)
 *
 * // Expansion results in definition of:
 * namespace falaise {
 *   namespace config {
 *     struct length {...};
 *     using length_t = quantity_t<length>;
 *   }
 * }
 * ```
 * The string equivalent of the passed parameter must be registered with the
 * @ref datatools::units::registry dimension/unit store.
 *
 * Client code can then use the type alias directly, e.g.
 *
 * ```cpp
 * falaise::config::length_t x{3.14, "m"};
 * ```
 *
 * \param Tag tag for the dimension, which must be known to @ref datatools::units::registry
 */
#define FALAISE_ADD_DIMENSION_TAG(Tag)          \
  namespace falaise {                           \
  namespace config {                            \
  struct Tag {                                  \
    typedef BOOST_METAPARSE_STRING(#Tag) label; \
  };                                            \
  using Tag##_t = quantity_t<Tag>;              \
  }                                             \
  }

namespace falaise {
namespace config {
//! Exception reporting wrong or incompatible dimensions
class wrong_dimension_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Exception reporting unknown unit tags
class unknown_unit_error : public std::logic_error {
  using std::logic_error::logic_error;
};

//! Class representing a value and physical unit
/*!
 * @ref quantity is a simple holder for @ref datatools::properties values
 * that are marked with an explicit unit. In properties files, these are declared as
 *
 * ```ini
 * foo : real as length = 3.14 m
 * ```
 *
 * where `length` is the dimension, and `m` is the explicit unit. @ref quantity composes this value,
 * unit, and dimension into a single object allowing these to be queried and the value directly
 * converted to the internal CLHEP::Units numeric representation. For example:
 *
 * ```cpp
 * auto x = falaise::config::quantity{3.14, "m"}; // valid
 * auto y = falaise::config::quantity{4.13, "furlong"}; // throws, "furlong" not a known unit
 *
 * double forCalc = x(); // value is 3.14*CLHEP::meter, so 3140.0
 * ```
 *
 * In general, you should not use @ref quantity directly, but rather the typed @ref falaise_units
 * concrete classes for the physical quantities you need (e.g. @ref length_t for length parameters).
 * @ref quantity only validates that the unit is known to @ref datatools::units, whereas the
 * @ref falaise_units types provide strict dimensional correctness.
 *
 * \sa falaise_units
 * \sa quantity_t
 */
class quantity {
 public:
  //! Default constructor
  /*!
   * Initializes as an effectively zero value dimensionless parameter
   */
  quantity() = default;

  //! Construct a quantity from a value and unit
  /*!
   * \param[in] value Numeric value in units of unit
   * \param[in] unit @ref datatools::units::unit tag
   * \throws falaise::config::unknown_unit_error if unit is not supported by @ref datatools::units
   */
  quantity(double value, std::string const& unit);

  //! Destructor
  virtual ~quantity() = default;

  //! Convert quantity to `double` value in the CLHEP::Units numeric scaling system
  /*!
   * For example:
   *
   * ```cpp
   * falaise::config::quantity q{3.14, "m"};
   * double x = q();
   * std::cout << x << std::endl; // prints 3140.0
   * ```
   *
   * \returns quantity's value multiplied by its unit's CLHEP::Units scaling factor
   */
  inline double operator()() const { return value_ * unit_scale; }

  //! Return the value for the quantity in its units
  /*!
   * For example:
   *
   * ```cpp
   * falaise::config::quantity x{3.14, "m"};
   * std::cout << x.value() << std::endl; // prints 3.14
   * ```
   *
   */
  inline double value() const { return value_; }

  //! Return value for the quantity in given units
  /*
   * For example:
   *
   * ```cpp
   * falaise::config::quantity x{3.14, "m"};
   * std::cout << x.value_in("mm") << std::endl; //prints 3140.0
   *
   * \throws bad_unit_error if supplied unit's dimension is different to
   * that of the quantity
   */
  double value_in(std::string const& unit) const;

  //! Return datatools::units tag for the quantity's unit
  inline std::string const& unit() const { return unit_name; }

  //! Return datatools::unit tag for the quantity's dimension
  inline std::string const& dimension() const { return dimension_name; }

 private:
  double value_{0.0};
  std::string unit_name{""};
  std::string dimension_name{""};
  double unit_scale{1.0};
};

//! Template class for a physical value with a strict dimension
/*!
 * Provides a concrete class of @ref quantity, enforcing
 *
 * Dimensional correctness is enforced by requiring that the unit
 * supplied to the constructor is:
 *
 * 1. Known to the @ref datatools::units system
 * 2. Has a @ref datatools::units dimension tag identical to the value
 *    of the `Dimension` template parameter's `label` type.
 *
 * The template parameter must provide a public typedef `label` to
 * a `boost::mpl::string`.
 *
 * Client code should prefer to use the predefined type aliases for
 * all known @ref datatools::units dimensions (see @ref falaise_units)
 *
 * \tparam Dimension dimensional tag for the quantity
 *
 * \sa FALAISE_ADD_DIMENSION_TAG
 * \sa falaise_units
 * \sa quantity
 */
template <typename Dimension>
class quantity_t : public quantity {
 public:
  //! Default constructor
  /*!
   * Initializes quantity with zero value and default units for Dimension
   *
   * \tparam Dimension dimension tag for this quantity_t
   */
  quantity_t()
      : quantity(0.0, datatools::units::get_default_unit_symbol_from_label(
                          boost::mpl::c_str<typename Dimension::label>::value)) {}

  //! Construct a quantity from a value and unit
  /*!
   * \param[in] value Numeric value
   * \param[in] unit @ref datatools::units::unit tag
   * \throws falaise::config::unknown_unit_error if unit is not supported by @ref
   * datatools::units::unit \throws falaise::config::wrong_dimension_error if unit's dimension does
   * not match the Dimension type parameter tag
   */
  quantity_t(double value, std::string const& unit) : quantity(value, unit) {
    if (boost::mpl::c_str<typename Dimension::label>::value != dimension()) {
      throw wrong_dimension_error("dimension of unit '" + unit + "' is not '" +
                                  boost::mpl::c_str<typename Dimension::label>::value + "'");
    }
  }

  //! Copy constructor from a raw @ref quantity
  /*!
   * \tparam Dimension dimension tag for this quantity_t
   * \param[in] q quantity to copy from
   * \throws falaise::config::wrong_dimension_error if dimension of q does not match the Dimension
   * tag
   */
  quantity_t(quantity const& q) : quantity_t(q.value(), q.unit()) {}

  //! Destructor
  virtual ~quantity_t() = default;
};
}  // namespace config
}  // namespace falaise

/*! \defgroup falaise_units System of Units
 *  \brief System of Units used internally by Falaise
 *
 *  Physical units in Falaise are handled by a two level system. Parameters
 *  passed as configuration via @ref datatools::properties are handled by
 *  concrete classes of @ref falaise::config::quantity_t to enforce dimensional
 *  correctness. For example:
 *
 *  ```cpp
 *  // Throws unless the "x" property was written as "x : real as length = VALUE LENGTHUNIT"
 *  auto x = ps.get<falaise::config::length_t>("x");
 *  ```
 *
 *  Internal C++ code in Falaise or plugins treats physical units as raw
 *  `double`s, using `CLHEP::Units` library to provide scaling
 *  factors for numerical correctness. It is up to the developer to maintain
 *  the dimensional correctness of all calculations.
 *
 *  The concrete classes of @ref falaise::config::quantity_t listed can be
 *  converted to `double` values in the `CLHEP::Units` numeric system via
 *  the overloaded function call operator:
 *
 *  ```cpp
 *  falaise::config::length_t l{3.14, "m"};
 *  double x{l()}; // or double x = l();
 *  std::cout << x << std::endl; // Prints "3140.0"
 *  ```
 *
 *  It is recommended that you *only* use the @ref falaise::config::quantity_t
 *  for parameter validation, storing extracted values in `double`s for actual
 *  calculation. The concrete types do not provide any dimensional analysis
 *  functionality, so the following will not compile:
 *
 *  ```cpp
 *  falaise::config::mass_t m{1.0,"kg"};
 *  falaise::config::acceleration_t a{1.0, "m/s2"};
 *
 *  falaise::config::force_t fa = m*a; // compile error;
 *  falaise::config::force_t fb = m()*a(); //compile error;
 *
 *  double fc = m()*a(); //o.k., dimensionless, but in CLHEP::Units scale
 *  ```
 *
 *  This is intentional so as to closely map the functionality of the
 *  underlying @ref datatools::units interface as a pure numeric/scaling
 *  system. The developer must exercise care in using the extracted `double`
 *  parameters to maintain dimensional correctness, but their values are
 *  guaranteed to be valid in `CLHEP::Units` system.
 *
 * \sa falaise::config::quantity_t
 * \sa falaise::config::property_set
 */

/*! \class falaise::config::absorbed_dose_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `absorbed_dose` ([L2][T-2])
 *
 * A @ref absorbed_dose_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::absorbed_dose_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `_rad`
 * - `gray`
 * - `kilogray`
 * - `microgray`
 * - `milligray`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(absorbed_dose)

/*! \class falaise::config::acceleration_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `acceleration` ([L][T-2])
 *
 * A @ref acceleration_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::acceleration_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `m/s2`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(acceleration)

/*! \class falaise::config::activity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `activity` ([T-1])
 *
 * A @ref activity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::activity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `becquerel`
 * - `curie`
 * - `dpm`
 * - `gigabecquerel`
 * - `kilobecquerel`
 * - `kilocurie`
 * - `megabecquerel`
 * - `megacurie`
 * - `microbecquerel`
 * - `microcurie`
 * - `millibecquerel`
 * - `millicurie`
 * - `nanocurie`
 * - `picocurie`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(activity)

/*! \class falaise::config::amount_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `amount` ([N])
 *
 * A @ref amount_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::amount_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `mole`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(amount)

/*! \class falaise::config::angle_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `angle` ([1])
 *
 * A @ref angle_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::angle_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `arcminute`
 * - `arcsecond`
 * - `degree`
 * - `grade`
 * - `microarcsecond`
 * - `microradian`
 * - `milliarcsecond`
 * - `milliradian`
 * - `radian`
 * - `turn`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(angle)

/*! \class falaise::config::angular_frequency_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `angular_frequency` ([T-1])
 *
 * A @ref angular_frequency_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::angular_frequency_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `rad/s`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(angular_frequency)

/*! \class falaise::config::capacitance_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `capacitance` ([M-1][L-2][T4][I2])
 *
 * A @ref capacitance_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::capacitance_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `farad`
 * - `femtofarad`
 * - `microfarad`
 * - `millifarad`
 * - `nanofarad`
 * - `picofarad`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(capacitance)

/*! \class falaise::config::conductance_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `conductance` ([M-1][L-2][T3][I2])
 *
 * A @ref conductance_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::conductance_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `siemens`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(conductance)

/*! \class falaise::config::conductivity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `conductivity` ([M-2][L-2][T3][I2])
 *
 * A @ref conductivity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::conductivity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `S/m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(conductivity)

/*! \class falaise::config::cross_section_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `cross_section` ([L2])
 *
 * A @ref cross_section_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::cross_section_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `barn`
 * - `femtobarn`
 * - `kilobarn`
 * - `megabarn`
 * - `microbarn`
 * - `millibarn`
 * - `nanobarn`
 * - `picobarn`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(cross_section)

/*! \class falaise::config::data_storage_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `data_storage` ([1])
 *
 * A @ref data_storage_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::data_storage_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `bit`
 * - `byte`
 * - `exabit`
 * - `exabyte`
 * - `exbibit`
 * - `exbibyte`
 * - `gibibit`
 * - `gibibyte`
 * - `gigabit`
 * - `gigabyte`
 * - `kibibit`
 * - `kibibyte`
 * - `kilobit`
 * - `kilobyte`
 * - `mebibit`
 * - `mebibyte`
 * - `megabit`
 * - `megabyte`
 * - `pebibit`
 * - `pebibyte`
 * - `petabit`
 * - `petabyte`
 * - `tebibit`
 * - `tebibyte`
 * - `terabit`
 * - `terabyte`
 * - `yobibit`
 * - `yobibyte`
 * - `yottabit`
 * - `yottabyte`
 * - `zebibit`
 * - `zebibyte`
 * - `zettabit`
 * - `zettabyte`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(data_storage)

/*! \class falaise::config::data_transfer_rate_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `data_transfer_rate` ([T-1])
 *
 * A @ref data_transfer_rate_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::data_transfer_rate_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `bit/s`
 * - `byte/s`
 * - `gibibit/s`
 * - `gibibyte/s`
 * - `gigabit/s`
 * - `gigabyte/s`
 * - `kibibit/s`
 * - `kibibyte/s`
 * - `kilobit/s`
 * - `kilobyte/s`
 * - `mebibit/s`
 * - `mebibyte/s`
 * - `megabit/s`
 * - `megabyte/s`
 * - `tebibit/s`
 * - `tebibyte/s`
 * - `terabit/s`
 * - `terabyte/s`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(data_transfer_rate)

/*! \class falaise::config::density_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `density` ([M][L-3])
 *
 * A @ref density_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::density_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `g/cm3`
 * - `g/mL`
 * - `kg/L`
 * - `kg/m3`
 * - `mg/cm3`
 * - `t/m3`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(density)

/*! \class falaise::config::electric_charge_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_charge` ([T][I])
 *
 * A @ref electric_charge_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_charge_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `coulomb`
 * - `femtocoulomb`
 * - `microcoulomb`
 * - `millicoulomb`
 * - `nanocoulomb`
 * - `picocoulomb`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_charge)

/*! \class falaise::config::electric_current_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_current` ([I])
 *
 * A @ref electric_current_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_current_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `ampere`
 * - `femtoampere`
 * - `kiloampere`
 * - `megaampere`
 * - `microampere`
 * - `milliampere`
 * - `nanoampere`
 * - `picoampere`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_current)

/*! \class falaise::config::electric_displacement_field_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_displacement_field` ([L-2][T][I])
 *
 * A @ref electric_displacement_field_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_displacement_field_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `C/m2`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_displacement_field)

/*! \class falaise::config::electric_field_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_field` ([M][L][T-3][I-1])
 *
 * A @ref electric_field_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_field_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `V/cm`
 * - `V/m`
 * - `kV/cm`
 * - `kV/m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_field)

/*! \class falaise::config::electric_flux_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_flux` ([M][L3][T-3][I-1])
 *
 * A @ref electric_flux_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_flux_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `V.m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_flux)

/*! \class falaise::config::electric_potential_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_potential` ([M][L2][T-3][I-1])
 *
 * A @ref electric_potential_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_potential_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `kilovolt`
 * - `megavolt`
 * - `microvolt`
 * - `millivolt`
 * - `volt`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_potential)

/*! \class falaise::config::electric_resistance_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_resistance` ([M][L2][T-3][I-2])
 *
 * A @ref electric_resistance_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_resistance_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `kiloohm`
 * - `megaohm`
 * - `ohm`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_resistance)

/*! \class falaise::config::electric_signal_integral_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `electric_signal_integral` ([M][L2][T-2][I-1])
 *
 * A @ref electric_signal_integral_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::electric_signal_integral_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `nV.s`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(electric_signal_integral)

/*! \class falaise::config::energy_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `energy` ([M][L2][T-2])
 *
 * A @ref energy_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::energy_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `electronvolt`
 * - `gigaelectronvolt`
 * - `joule`
 * - `kiloelectronvolt`
 * - `megaelectronvolt`
 * - `microelectronvolt`
 * - `millielectronvolt`
 * - `nanoelectronvolt`
 * - `petaelectronvolt`
 * - `teraelectronvolt`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(energy)

/*! \class falaise::config::equivalent_dose_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `equivalent_dose` ([L2][T-2])
 *
 * A @ref equivalent_dose_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::equivalent_dose_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `microsievert`
 * - `millisievert`
 * - `nanosievert`
 * - `picosievert`
 * - `rem`
 * - `sievert`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(equivalent_dose)

/*! \class falaise::config::force_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `force` ([M][L][T-2])
 *
 * A @ref force_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::force_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `dyne`
 * - `kilonewton`
 * - `newton`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(force)

/*! \class falaise::config::fraction_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `fraction` ([1])
 *
 * A @ref fraction_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::fraction_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `percent`
 * - `perthousand`
 * - `ppb`
 * - `ppm`
 * - `ppq`
 * - `ppt`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(fraction)

/*! \class falaise::config::frequency_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `frequency` ([T-1])
 *
 * A @ref frequency_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::frequency_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `cpm`
 * - `gigahertz`
 * - `hertz`
 * - `kilohertz`
 * - `megahertz`
 * - `millihertz`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(frequency)

/*! \class falaise::config::illuminance_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `illuminance` ()
 *
 * A @ref illuminance_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::illuminance_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `lux`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(illuminance)

/*! \class falaise::config::inductance_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `inductance` ([M][L2][T-2][I-2])
 *
 * A @ref inductance_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::inductance_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `henry`
 * - `microhenry`
 * - `millihenry`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(inductance)

/*! \class falaise::config::length_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `length` ([L])
 *
 * A @ref length_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::length_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `angstrom`
 * - `astronomical_unit`
 * - `centimeter`
 * - `decameter`
 * - `decimeter`
 * - `femtometer`
 * - `foot`
 * - `gigalight_year`
 * - `gigaparsec`
 * - `hectometer`
 * - `inch`
 * - `kilolight_year`
 * - `kilometer`
 * - `kiloparsec`
 * - `light_year`
 * - `megalight_year`
 * - `megaparsec`
 * - `meter`
 * - `micrometer`
 * - `mile`
 * - `millimeter`
 * - `nanometer`
 * - `nautical_mile`
 * - `parsec`
 * - `picometer`
 * - `yard`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(length)

/*! \class falaise::config::level_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `level` ([1])
 *
 * A @ref level_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::level_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `bel`
 * - `decibel`
 * - `neper`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(level)

/*! \class falaise::config::luminance_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `luminance` ([L-2][J])
 *
 * A @ref luminance_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::luminance_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `cd/m2`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(luminance)

/*! \class falaise::config::luminous_energy_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `luminous_energy` ()
 *
 * A @ref luminous_energy_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::luminous_energy_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `lm.s`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(luminous_energy)

/*! \class falaise::config::luminous_energy_density_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `luminous_energy_density` ()
 *
 * A @ref luminous_energy_density_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::luminous_energy_density_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `lx.s/m3`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(luminous_energy_density)

/*! \class falaise::config::luminous_exposure_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `luminous_exposure` ()
 *
 * A @ref luminous_exposure_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::luminous_exposure_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `lx.s`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(luminous_exposure)

/*! \class falaise::config::luminous_flux_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `luminous_flux` ()
 *
 * A @ref luminous_flux_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::luminous_flux_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `lumen`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(luminous_flux)

/*! \class falaise::config::luminous_intensity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `luminous_intensity` ([J])
 *
 * A @ref luminous_intensity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::luminous_intensity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `candela`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(luminous_intensity)

/*! \class falaise::config::magnetic_field_strength_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `magnetic_field_strength` ([L-1][I])
 *
 * A @ref magnetic_field_strength_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::magnetic_field_strength_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `A/m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(magnetic_field_strength)

/*! \class falaise::config::magnetic_flux_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `magnetic_flux` ([M][L2][T-2][I-1])
 *
 * A @ref magnetic_flux_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::magnetic_flux_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `maxwell`
 * - `microweber`
 * - `milliweber`
 * - `nanoweber`
 * - `weber`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(magnetic_flux)

/*! \class falaise::config::magnetic_flux_density_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `magnetic_flux_density` ([M][T-2][I-1])
 *
 * A @ref magnetic_flux_density_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::magnetic_flux_density_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `gauss`
 * - `kilogauss`
 * - `microtesla`
 * - `milligauss`
 * - `millitesla`
 * - `nanotesla`
 * - `tesla`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(magnetic_flux_density)

/*! \class falaise::config::mass_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `mass` ([M])
 *
 * A @ref mass_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::mass_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `GeV/c2`
 * - `MeV/c2`
 * - `centigram`
 * - `dalton`
 * - `decagram`
 * - `decigram`
 * - `eV/c2`
 * - `gram`
 * - `hectogram`
 * - `kilogram`
 * - `microgram`
 * - `milligram`
 * - `ton`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(mass)

/*! \class falaise::config::mass_activity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `mass_activity` ([M-1][T-1])
 *
 * A @ref mass_activity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::mass_activity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `Bq/kg`
 * - `GBq/kg`
 * - `MBq/kg`
 * - `dpm/kg`
 * - `kBq/kg`
 * - `mBq/kg`
 * - `uBq/kg`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(mass_activity)

/*! \class falaise::config::permeability_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `permeability` ([M][L][T-2][I-2])
 *
 * A @ref permeability_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::permeability_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `H/m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(permeability)

/*! \class falaise::config::permittivity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `permittivity` ([M-1][L-3][T4][I2])
 *
 * A @ref permittivity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::permittivity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `F/m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(permittivity)

/*! \class falaise::config::power_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `power` ([M][L2][T-3])
 *
 * A @ref power_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::power_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `gigawatt`
 * - `kilowatt`
 * - `megawatt`
 * - `microwatt`
 * - `milliwatt`
 * - `nanowatt`
 * - `watt`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(power)

/*! \class falaise::config::pressure_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `pressure` ([M][L-1][T-2])
 *
 * A @ref pressure_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::pressure_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `atmosphere`
 * - `bar`
 * - `centibar`
 * - `cmHg`
 * - `decapascal`
 * - `decibar`
 * - `gigapascal`
 * - `hectopascal`
 * - `kilobar`
 * - `kilopascal`
 * - `megabar`
 * - `megapascal`
 * - `millibar`
 * - `millipascal`
 * - `pascal`
 * - `torr`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(pressure)

/*! \class falaise::config::procedure_defined_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `procedure_defined` ([?])
 *
 * A @ref procedure_defined_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::procedure_defined_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `arb.unit`
 * - `p.d.u.`
 * - `pdu`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(procedure_defined)

/*! \class falaise::config::resistivity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `resistivity` ([M][L3][T-3][I-2])
 *
 * A @ref resistivity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::resistivity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `ohm_meter`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(resistivity)

/*! \class falaise::config::solid_angle_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `solid_angle` ([1])
 *
 * A @ref solid_angle_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::solid_angle_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `centisteradian`
 * - `decasteradian`
 * - `decisteradian`
 * - `femtosteradian`
 * - `microsteradian`
 * - `millisteradian`
 * - `nanosteradian`
 * - `picosteradian`
 * - `steradian`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(solid_angle)

/*! \class falaise::config::surface_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `surface` ([L2])
 *
 * A @ref surface_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::surface_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `centimeter2`
 * - `femtometer2`
 * - `foot2`
 * - `inch2`
 * - `kilometer2`
 * - `meter2`
 * - `micrometer2`
 * - `millimeter2`
 * - `yard2`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(surface)

/*! \class falaise::config::surface_activity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `surface_activity` ([L-2][T-1])
 *
 * A @ref surface_activity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::surface_activity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `Bq/m2`
 * - `GBq/m2`
 * - `MBq/m2`
 * - `dpm/m2`
 * - `kBq/m2`
 * - `mBq/m2`
 * - `uBq/m2`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(surface_activity)

/*! \class falaise::config::surface_density_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `surface_density` ([M][L-2])
 *
 * A @ref surface_density_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::surface_density_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `g/cm2`
 * - `kg/m2`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(surface_density)

/*! \class falaise::config::surface_tension_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `surface_tension` ([M][T-2])
 *
 * A @ref surface_tension_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::surface_tension_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `N/m`
 * - `dyn/cm`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(surface_tension)

/*! \class falaise::config::temperature_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `temperature` ([theta])
 *
 * A @ref temperature_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::temperature_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `kelvin`
 * - `microkelvin`
 * - `millikelvin`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(temperature)

/*! \class falaise::config::time_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `time` ([T])
 *
 * A @ref time_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::time_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `day`
 * - `femtosecond`
 * - `hour`
 * - `microsecond`
 * - `millisecond`
 * - `minute`
 * - `nanosecond`
 * - `picosecond`
 * - `second`
 * - `week`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(time)

/*! \class falaise::config::velocity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `velocity` ([L][T-1])
 *
 * A @ref velocity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::velocity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `cm/h`
 * - `cm/ms`
 * - `cm/ns`
 * - `cm/s`
 * - `cm/us`
 * - `km/h`
 * - `km/s`
 * - `m/h`
 * - `m/ms`
 * - `m/ns`
 * - `m/s`
 * - `m/us`
 * - `mm/ms`
 * - `mm/ns`
 * - `mm/s`
 * - `mm/us`
 * - `mph`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(velocity)

/*! \class falaise::config::volume_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `volume` ([L3])
 *
 * A @ref volume_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::volume_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `centilitre`
 * - `centimeter3`
 * - `decalitre`
 * - `decimeter3`
 * - `foot3`
 * - `gigalitre`
 * - `hectolitre`
 * - `imperial_gallon`
 * - `inch3`
 * - `kilolitre`
 * - `kilometer3`
 * - `litre`
 * - `megalitre`
 * - `meter3`
 * - `microlitre`
 * - `millilitre`
 * - `millimeter3`
 * - `teralitre`
 * - `us_gallon`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(volume)

/*! \class falaise::config::volume_activity_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `volume_activity` ([L-3][T-1])
 *
 * A @ref volume_activity_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::volume_activity_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `Bq/m3`
 * - `GBq/m3`
 * - `MBq/m3`
 * - `dpm/m3`
 * - `kBq/m3`
 * - `mBq/m3`
 * - `uBq/m3`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(volume_activity)

/*! \class falaise::config::wave_number_t
 *  \ingroup falaise_units
 *  \brief quantity for values with dimension tag `wave_number` ([L-1])
 *
 * A @ref wave_number_t value may be constructed using, for example
 *
 * ```cpp
 * falaise::config::wave_number_t x{3.14, "TAG"};
 * ```
 *
 * where `TAG` is one of the following valid unit tags:
 *
 * - `/m`
 *
 * A @ref wrong_dimension_error will be thrown if the supplied unit tag is not in the above list
 */
FALAISE_ADD_DIMENSION_TAG(wave_number)

#endif /* FALAISE_QUANTITY_H */
