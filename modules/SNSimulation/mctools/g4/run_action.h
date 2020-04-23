/// \file mctools/g4/run_action.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 user run action class
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_RUN_ACTION_H
#define MCTOOLS_G4_RUN_ACTION_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools :
#include <datatools/io_factory.h>
#include <datatools/properties.h>
// - Bayeux/brio :
#include <brio/writer.h>
// - Geant4 :
#include <G4UserRunAction.hh>

// This project:
#include <mctools/g4/loggable_support.h>
#include <mctools/utils.h>

namespace datatools {
class data_writer;
}
namespace brio {
class writer;
}
namespace dpp {
class output_module;
}

namespace mctools {

class simulated_data;

namespace g4 {

// Forward declaration
class manager;
class event_action;

/// \brief The Geant4 run action
class run_action : public G4UserRunAction, public loggable_support {
 public:
  static const int NUMBER_OF_EVENTS_MODULO_NONE = 0;
  static const int NUMBER_OF_EVENTS_MODULO_DEFAULT = 100;

  /// Check initialization flag
  bool is_initialized() const;

  /// Check the run header/footer flag
  bool using_run_header_footer() const;

  /// Set the run header/footer flag
  void set_use_run_header_footer(bool);

  /// Check if the print period on processes number of events is set
  bool has_number_of_events_modulo() const;

  /// Set the print period on processes number of events
  void set_number_of_events_modulo(int);

  /// Return the print period on processes number of events
  int get_number_of_events_modulo() const;

  /// Return the number of processed events
  int32_t get_number_of_processed_events() const;

  /// Increment the number of processed events
  void increment_number_of_processed_events();

  /// Reset the number of processed events
  void reset_number_of_processed_events();

  /// Return the number of saved events
  int32_t get_number_of_saved_events() const;

  /// Increment the number of saved events
  void increment_number_of_saved_events();

  /// Reset the number of saved events
  void reset_number_of_saved_events();

  /// Set the output data format
  void set_output_data_format(io_utils::data_format_type);

  /// Set the output data bank label
  void set_output_data_bank_label(const std::string &);

  /// Set the full path of the output file
  void set_output_file(const std::string &a_filename);

  /// Check if the date save flag is set
  bool save_data() const;

  /// Check if an output module is set to save events
  bool has_out_module() const;

  /// Return a mutable reference to the embedded output module
  dpp::output_module &grab_out_module();

  /// Return const reference to the embedded output module
  const dpp::output_module &get_out_module() const;

  /// Check if a Brio writer is set to save events
  bool has_brio_writer() const;

  /// Return a mutable reference to the embedded Brio writer
  brio::writer &grab_brio_writer();

  /// Return a const reference to the embedded Brio writer
  const brio::writer &get_brio_writer() const;

  /// Check if a basic writer is set to save events
  bool has_writer() const;

  /// Return a mutable reference to the embedded basic writer
  datatools::data_writer &grab_writer();

  /// Return a const reference to the embedded basic writer
  const datatools::data_writer &get_writer() const;

  /// Return a mutable reference to the simulation manager
  manager &grab_manager();

  /// Return a const reference to the simulation manager
  const manager &get_manager() const;

  /// Constructor
  run_action(manager &a_mgr);

  /// Destructor
  virtual ~run_action();

  /// Register the event action
  void register_event_action(event_action &);

  /// Initialize from a collection of configuration properties
  void initialize(const datatools::properties &a_config);

  /// Reset
  void reset();

  /// Basic print
  void dump(std::ostream &a_out = std::clog) const;

  /// Store event data
  void store_data(const mctools::simulated_data &esd_);

  /// Geant4 BeginOfRunAction mandatory interface
  void BeginOfRunAction(const G4Run *);

  /// Geant4 EndOfRunAction mandatory interface
  void EndOfRunAction(const G4Run *);

 protected:
  /// Store event data
  void _store_data(const mctools::simulated_data &esd_);

 protected:
  /// Build the run header
  virtual void _build_run_header();

  /// Build the run footer
  virtual void _build_run_footer();

  /// Set the attributes' default values
  virtual void _set_default();

 private:
  bool _initialized_ = false;                //!< Initialization flag
  bool _use_run_header_footer_ = false;      //!< Flag to save run header and footer
  int _number_of_events_modulo_ = 0;         //!< Event modulo for printing
  int32_t _number_of_processed_events_ = 0;  //!< Number of processed events
  int32_t _number_of_saved_events_ = 0;      //!< Number of saved events
  manager *_manager_ = nullptr;              //!< Handle to the simulation manager
  datatools::properties _run_header_;        //!< Run header
  datatools::properties _run_footer_;        //!< Run footer

  // I/O :
  bool _save_data_ = false;  //!< Flag to save event data
  io_utils::data_format_type
      _output_data_format_;              //!< The data format of the output file ("plain" or "bank")
  std::string _output_data_bank_label_;  //!< The label of the data bank used to store simulated
                                         //!< data ("bank" format only)
  bool _output_file_preserve_ = true;    //!< Flag to preserve an existing data file
  std::string _output_file_dir_;         //!< Directory where to store the output data file
  std::string _output_file_prefix_;      //!< Prefix of the output data file
  std::string _output_file_format_;      //!< Format of the output data file
  std::string _output_file_compression_;                //!< Compression of the output data file
  std::string _output_file_;                            //!< Full path of the output data file
  std::string _brio_general_info_store_label_;          //!< default "GI"
  std::string _brio_plain_simulated_data_store_label_;  //!< default "PSD"
  event_action *_event_action_ = nullptr;               //!< Handle to the event action

  /// \brief PIMPL-ized I/O working resources:
  struct io_work_type;
  boost::scoped_ptr<io_work_type> _io_work_;  //!< I/O
};

}  // end of namespace g4

}  // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::run_action)

#endif  // MCTOOLS_G4_RUN_ACTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
