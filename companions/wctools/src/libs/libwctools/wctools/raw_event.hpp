//! \file wctools/raw_event.hpp
//! \brief Wavecatcher readout event format
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef WCTOOLS_RAW_EVENT_HPP
#define WCTOOLS_RAW_EVENT_HPP

// Standard library:
#include <string>
#include <iostream>
#include <memory>
#include <vector>

namespace wctools {

  //! \brief Wavecatcher raw event
  struct raw_event
  {
    struct header
    {
      static const std::size_t NBLINES = 3;
      int         id;
      double      unix_time;
      std::string date;
      std::string time;
      int         tdc;
      std::string tdc_corrected_time;
      int         channel_id;
      int         fcr;
      double      baseline;
      double      amplitude;
      double      charge;
      double      leading_edge_time;
      double      trailing_edge_time;
      double      rate_counter;

      //! Print
      void print(std::ostream & out_,
                 const std::string & title_ = "",
                 const std::string & indent_ = "") const;

    };

    //! Default constructor
    raw_event();

    //! Reset
    void reset();

    //! Set the expected samples
    void set_nb_samples(std::size_t);

    //! Return the expected samples
    std::size_t get_nb_samples() const;

    //! Parse
    void parse(std::istream & in_);

    //! Return the event header
    const header & get_header() const;

    //! Return the event samples
    const std::vector<double> & get_samples() const;

    //! Print
    void print(std::ostream & out_,
               const std::string & title_ = "",
               const std::string & indent_ = "") const;

  protected:

    void _parse(std::istream & in_);

    void _decode_header(const std::string & hline_, int index_);

    void _decode_samples(const std::string & dataline_);

  private:

    std::unique_ptr<header> _header_;     //!< Handle to the input file header
    std::size_t             _nb_samples_; //!< Expected number of samples from the file header
    std::vector<double>     _samples_;    //!< ADC samples

  };

} // namespace wctools

#endif // WCTOOLS_RAW_EVENT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
