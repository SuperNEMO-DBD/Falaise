//! \file fecom/calo_hit_reader.hpp
//! \brief Wavecatcher readout raw event reader
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_HIT_READER_HPP
#define FECOM_CALO_HIT_READER_HPP

// Standard library:
#include <string>
#include <memory>
#include <fstream>

// Third party:
// - Boost:
#include <boost/utility.hpp>

// This project:
#include <fecom/raw_event.hpp>
#include <fecom/run_header.hpp>

namespace fecom {

  //! \brief Commissioning calo hit file reader
  class calo_hit_reader
    : private boost::noncopyable
  {
  public:

    static const std::size_t HEADER_NBLINES = 4;


    //! Default constructor
    calo_hit_reader();

    //! Set the input file name
    void set_input_filename(const std::string & input_filename_);

    //! Return the input file name
    const std::string & get_input_filename() const;

    //! Chek if a next event is available
    bool has_next_event() const;

    //! Load the next event
    void load_next_event(calo_hit & hit_);

    //! Load the run header
    void load_run_header(run_header & header_);

    //! Check initialization flag
    bool is_initialized() const;

    //! Initialize the reader
    void initialize();

    //! Reset the reader
    void reset();

    //! Return the logging level
    int get_logging() const;

    //! Set the logging level
    void set_logging(int);

  protected:

    void _init_input_file();

    void _reset_input_file();

    void _init_header();

    void _reset_header();

    void _decode_header(const std::string & hline_, const int index_);

  private:

    // Configuration:
    std::string _input_filename_;

    // Management:
    bool _initialized_ = false;
    int  _logging_ = 0;

    // Working:
    std::unique_ptr<std::ifstream> _fin_;    //!< Handle to the input file stream
    std::unique_ptr<run_header>    _header_; //!< Handle to the input file header

  };

} // namespace fecom

#endif // FECOM_CALO_HIT_READER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
