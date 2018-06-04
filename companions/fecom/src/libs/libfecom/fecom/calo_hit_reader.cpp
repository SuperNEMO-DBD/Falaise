// fecom/calo_hit_reader.cpp

// Ourselves:
#include <fecom/calo_hit_reader.hpp>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

// This project:
#include <fecom/utils.hpp>

namespace fecom {

  // static
  const std::size_t calo_hit_reader::HEADER_NBLINES;

  calo_hit_reader::calo_hit_reader()
  {
    return;
  }

  void calo_hit_reader::set_input_filename(const std::string & input_filename_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Reader is already initialized!");
    _input_filename_ = input_filename_;
    return;
  }

  const std::string & calo_hit_reader::get_input_filename() const
  {
    return _input_filename_;
  }

  bool calo_hit_reader::has_next_hit() const
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Reader is not initialized!");
    if (!*_fin_ || _fin_->eof()) return false;
    return true;
  }

  void calo_hit_reader::load_run_header(run_header & header_)
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Reader is not initialized!");
    header_ = *_header_.get();
    return;
  }

  void calo_hit_reader::load_next_hit(calo_hit & hit_)
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Reader is not initialized!");
    bool success = _hit_parser_.parse(*_fin_, hit_);
    if (!success) {
      DT_THROW(std::logic_error, "Parsing failed!");
    }
    *_fin_ >> std::ws;
    return;
  }

  datatools::logger::priority calo_hit_reader::get_logging() const
  {
    return _logging_;
  }

  void calo_hit_reader::set_logging(datatools::logger::priority logging_)
  {
    _logging_ = logging_;
    _hit_parser_.logging = _logging_;
    return;
  }

  bool calo_hit_reader::is_initialized() const
  {
    return _initialized_;
  }

  void calo_hit_reader::initialize()
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Reader is already initialized!");
    _init_input_file_();
    _init_header_();
    *_fin_ >> std::ws;
    _initialized_ = true;
    return;
  }

  void calo_hit_reader::reset()
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Reader is not initialized!");
    _initialized_ = false;
    _reset_header_();
    _reset_input_file_();
    return;
  }

  void calo_hit_reader::_init_input_file_()
  {
    _fin_.reset(new std::ifstream);
    std::string ifn =_input_filename_;
    datatools::fetch_path_with_env(ifn);
    _fin_->open(ifn.c_str());
    DT_THROW_IF(!*_fin_, std::runtime_error, "Cannot open input file '" << _input_filename_ << "'");
    return;
  }

  void calo_hit_reader::_reset_input_file_()
  {
    if (_fin_) {
      _fin_->close();
      _fin_.reset();
    }
    return;
  }

  void calo_hit_reader::_init_header_()
  {
    _header_.reset(new run_header);
    for (std::size_t ih = 0; ih < HEADER_NBLINES; ih++) {
      std::string hlines;
      std::getline(*_fin_, hlines);
      DT_LOG_DEBUG(_logging_, "Header[#" << ih << "] : " << hlines);
      _decode_header_(hlines, ih);
    }
    return;
  }

  void calo_hit_reader::_reset_header_()
  {
    if (_header_) {
      _header_.reset();
    }
    return;
  }

  void calo_hit_reader::_decode_header_(const std::string & hline_, const int index_)
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    namespace qi = boost::spirit::qi;
    std::string::const_iterator str_iter = hline_.begin();
    std::string::const_iterator end_iter = hline_.end();
    bool res = false;

    if (index_ == 0) {
      std::string sw_version;
      double unix_time;
      std::string date;
      std::string time;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              "===" >>
                              qi::lit("DATA FILE SAVED WITH SN CRATE SOFTWARE VERSION:") >> (+~qi::char_("="))
                              >> "=="
                              >> qi::lit("DATE OF RUN:")
                              >> qi::lit("UnixTime =")
                              // >> (+~qi::char_("="))
                              // >> "="
                              >> qi::double_
                              >> qi::lit("date =")
                              >> (+~qi::char_("t"))
                              >> qi::lit("time =")
                              >> (+~qi::char_("="))
                              >> "==="
			      ),
                             //  End grammar
                             qi::space,
                             sw_version, unix_time, date, time);
      DT_THROW_IF(!res || str_iter != end_iter, std::logic_error,
                  "Cannot parse file header line #" << index_);
      DT_LOG_DEBUG(_logging_, "sw_version = " << sw_version);
      DT_LOG_DEBUG(_logging_, "unix_time = " << unix_time);
      DT_LOG_DEBUG(_logging_, "date = " << date);
      DT_LOG_DEBUG(_logging_, "time = " << time);
      _header_->software_version = sw_version;
      _header_->unix_time = unix_time * CLHEP::second;
      _header_->date = date;
      _header_->time = time;
    }

    if (index_ == 2) {
      std::string data_type;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              "===" >>
                              qi::lit("DATA TYPE :") >> (+~qi::char_("="))
                              >> "==="
			      ),
                             //  End grammar
                             qi::space,
                             data_type);
      DT_THROW_IF(!res || str_iter != end_iter, std::logic_error,
                  "Cannot parse file header line #" << index_);
      DT_LOG_DEBUG(_logging_, "data_type = " << data_type);
      _header_->data_type = data_type;
    }

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

} // namespace fecom
