// wctools/raw_reader.cpp

// Ourselves:
#include <wctools/raw_reader.hpp>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>

// This project:
#include <wctools/utils.hpp>

namespace wctools {

  raw_reader::raw_reader()
  {
    return;
  }

  void raw_reader::set_input_filename(const std::string & input_filename_)
  {
    if (_initialized_) {
      throw std::logic_error("Reader is already initialized!");
    }
    _input_filename_ = input_filename_;
    return;
  }

  const std::string & raw_reader::get_input_filename() const
  {
    return _input_filename_;
  }

  bool raw_reader::has_next_event() const
  {
    if (!_initialized_) {
      throw std::logic_error("Reader is not initialized!");
    }
    if (!*_fin_ || _fin_->eof()) return false;
    return true;
  }

  void raw_reader::load_run_header(run_header & header_)
  {
    if (!is_initialized()) {
      throw std::logic_error("Reader is not initialized!");
    }
    header_ = *_header_.get();
    return;
  }

  void raw_reader::load_next_event(raw_event & event_)
  {
    if (!is_initialized()) {
      throw std::logic_error("Reader is not initialized!");
    }
    event_.set_nb_samples(_header_->nb_data_samples);
    event_.parse(*_fin_);
    return;
  }

  int raw_reader::get_logging() const
  {
    return _logging_;
  }

  void raw_reader::set_logging(int logging_)
  {
    _logging_ = logging_;
    return;
  }

  bool raw_reader::is_initialized() const
  {
    return _initialized_;
  }

  void raw_reader::initialize()
  {
    if (_initialized_) {
      throw std::logic_error("Reader is already initialized!");
    }

    _init_input_file();
    _init_header();
    *_fin_ >> std::ws;

    _initialized_ = true;
    return;
  }

  void raw_reader::reset()
  {
    if (!_initialized_) {
      throw std::logic_error("Reader is not initialized!");
    }
    _initialized_ = false;

    _reset_header();
    _reset_input_file();

    return;
  }

  void raw_reader::_init_input_file()
  {
    _fin_.reset(new std::ifstream);
    std::string ifn =_input_filename_;
    expand_environ(ifn);
    _fin_->open(ifn.c_str());
    if (!*_fin_) {
      std::ostringstream message;
      message << "Cannot open input file '" << _input_filename_ << "'";
      throw std::runtime_error(message.str());
    }
    return;
  }

  void raw_reader::_reset_input_file()
  {
    if (_fin_) {
      _fin_->close();
      _fin_.reset();
    }
    return;
  }

  void raw_reader::_init_header()
  {
    _header_.reset(new run_header);
    std::string hlines[run_header::NBLINES];
    for (std::size_t ih = 0; ih < run_header::NBLINES; ih++) {
      std::getline(*_fin_, hlines[ih]);
      if (_logging_ > 1) std::cerr << "wctools::raw_reader::_init_header: "
                                   << "Header[#" << ih << "] : " << hlines[ih] << std::endl;
      _decode_header(hlines[ih], ih);
    }

    return;
  }

  void raw_reader::_reset_header()
  {
    if (_header_) {
      _header_.reset();
    }
    return;
  }

  void raw_reader::_decode_header(const std::string & hline_, const int index_)
  {
    namespace qi = boost::spirit::qi;
    std::string::const_iterator str_iter = hline_.begin();
    std::string::const_iterator end_iter = hline_.end();
    bool res = false;

    if (index_ == 0) {
      std::string sw_version;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              "===" >>
                              qi::lit("DATA FILE SAVED WITH SOFTWARE VERSION:") >> (+~qi::char_("="))
                              >> "==="
                              ),
                             //  End grammar
                             qi::space,
                             sw_version);
      if (!res || str_iter != end_iter) {
        throw std::logic_error("Cannot parse file header line #" + boost::lexical_cast<std::string>(index_));
      }
      if (_logging_ > 1) {
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "sw_version = " << sw_version << std::endl;
      }
      _header_->sw_version = sw_version;
    }

    if (index_ == 1) {
      int wc_type = -1;
      std::size_t wc_nb_channels = 0;
      double wc_gain = 0.0;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              "===" >>
                              qi::lit("WAVECATCHER SYSTEM OF TYPE") >>  qi::int_
                              >> qi::lit("WITH") >> qi::uint_
                              >> qi::lit("CHANNELS AND GAIN:") >> qi::double_
                              >> "==="
                              ),
                             //  End grammar
                             qi::space,
                             wc_type, wc_nb_channels, wc_gain);
      if (!res || str_iter != end_iter) {
        throw std::logic_error("Cannot parse file header line #" + boost::lexical_cast<std::string>(index_));
      }
      if (_logging_ > 1) {
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_type        = " << wc_type << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_nb_channels = " << wc_nb_channels << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_gain        = " << wc_gain << std::endl;
      }
      _header_->wc_system_type = wc_type;
      _header_->nb_channels = wc_nb_channels;
      _header_->gain = wc_gain;
    }

    if (index_ == 2) {
      // SKip this line
    }

    if (index_ == 3) {
      std::size_t wc_nb_samples = 0;
      std::size_t wc_nb_acq_channels = 0;
      std::string wc_adc_unit;
      double      wc_sampling_period = 0.0;
      std::string wc_time_unit;
      int         wc_inl_correction = -1;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              "===" >>
                              qi::lit("DATA SAMPLES") >> '[' >> qi::uint_ >> ']'
                              >> qi::lit("in") >> (+~qi::char_("="))
                              >> "=="
                              >> qi::lit("NB OF CHANNELS ACQUIRED:") >> qi::uint_
                              >> "=="
                              >> qi::lit("Sampling Period:") >> qi::double_ >> (+~qi::char_("="))
                              >> "=="
                              >> qi::lit("INL Correction:") >> qi::int_
                              >> "==="
                              ),
                             //  End grammar
                             qi::space,
                             wc_nb_samples
                             , wc_adc_unit
                             , wc_nb_acq_channels
                             , wc_sampling_period
                             , wc_time_unit
                             , wc_inl_correction
                             );
      if (!res || str_iter != end_iter) {
        throw std::logic_error("Cannot parse file header line #" + boost::lexical_cast<std::string>(index_));
      }
      if (_logging_ > 1) {
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_nb_samples      = " << wc_nb_samples << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_nb_acq_channels = " << wc_nb_acq_channels << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_adc_unit        = " << wc_adc_unit << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_sampling_period = " << wc_sampling_period << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_time_unit       = " << wc_time_unit << std::endl;
        std::cerr << "wctools::raw_reader::_decode_header: "
                  << "wc_inl_correction  = " << wc_inl_correction << std::endl;
      }
      _header_->nb_data_samples = wc_nb_samples;
      _header_->nb_acq_channels = wc_nb_acq_channels;
      _header_->adc_unit        = wc_adc_unit;
      _header_->sampling_period = wc_sampling_period;
      _header_->tdc_unit        = wc_time_unit;
      _header_->inl_correction  = wc_inl_correction;
    }

    return;
  }

} // namespace wctools
