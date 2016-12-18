// wctools/raw_event.cpp

// Ourselves:
#include <wctools/raw_event.hpp>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>

namespace wctools {

  void raw_event::header::print(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }
    out_ << indent_ << "|-- " << "Id = " << id << std::endl;
    out_ << indent_ << "|-- " << "unix_time = " << unix_time << std::endl;
    out_ << indent_ << "|-- " << "date = '" << date << "'" << std::endl;
    out_ << indent_ << "|-- " << "time = '" << time << "'" << std::endl;
    out_ << indent_ << "|-- " << "tdc = " << tdc << std::endl;
    out_ << indent_ << "|-- " << "tdc_corrected_time = '" << tdc_corrected_time << "'" << std::endl;
    out_ << indent_ << "|-- " << "channel_id = " << channel_id << std::endl;
    out_ << indent_ << "|-- " << "fcr = " << fcr << std::endl;
    out_ << indent_ << "|-- " << "baseline = " << baseline << std::endl;
    out_ << indent_ << "|-- " << "amplitude = " << amplitude << std::endl;
    out_ << indent_ << "|-- " << "charge = " << charge << std::endl;
    out_ << indent_ << "|-- " << "leading_edge_time = " << leading_edge_time << std::endl;
    out_ << indent_ << "|-- " << "trailing_edge_time = " << trailing_edge_time << std::endl;
    out_ << indent_ << "`-- " << "rate_counter = " << rate_counter << std::endl;
    return;
  }

  raw_event::raw_event()
  {
    return;
  }

  void raw_event::reset()
  {
    _samples_.clear();
    _header_.reset();
    _nb_samples_ = 0;
    return;
  }

  void raw_event::set_nb_samples(std::size_t nbs_)
  {
    _nb_samples_ = nbs_;
    return;
  }

  std::size_t raw_event::get_nb_samples() const
  {
    return _nb_samples_;
  }

  void raw_event::parse(std::istream & in_)
  {
    _header_.reset(new header);
    _parse(in_);
    return;
  }

  const raw_event::header & raw_event::get_header() const
  {
    return *_header_;
  }

  const std::vector<double> & raw_event::get_samples() const
  {
    return _samples_;
  }

  void raw_event::print(std::ostream & out_,
                        const std::string & title_,
                        const std::string & indent_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << "|-- " << "Header: ";
    if (!_header_) {
      out_ << "<none>";
    }
    out_ << std::endl;
    if (_header_) {
      _header_->print(out_, "", indent_ + "|   ");
    }

    out_ << indent_ << "`-- " << "Samples: ";
    if (!_samples_.size()) {
      out_ << "<none>";
    } else {
      out_ << "[" << _samples_.size() << "]";
    }
    out_ << std::endl;

    return;
  }

  void raw_event::_parse(std::istream & in_)
  {
    std::string hlines[header::NBLINES];
    for (std::size_t ih = 0; ih < header::NBLINES; ih++) {
      std::getline(in_, hlines[ih]);
      _decode_header(hlines[ih], ih);
      in_ >> std::ws;
    }

    _samples_.reserve(_nb_samples_);
    std::string dataline;
    std::getline(in_, dataline);
    _decode_samples(dataline);
    in_ >> std::ws;
    return;
  }

  void raw_event::_decode_samples(const std::string & dataline_)
  {
    _samples_.clear();
    namespace qi = boost::spirit::qi;
    // std::cerr << "wctools::raw_event::_decode_samples: dataline=<" << dataline_ << ">" << std::endl;

    std::string::const_iterator str_iter = dataline_.begin();
    std::string::const_iterator end_iter = dataline_.end();
    bool res = false;
    res = qi::phrase_parse(str_iter,
                           end_iter,
                           //  Begin grammar
                           (
                            qi::repeat(_nb_samples_)[qi::double_]
                            ),
                           //  End grammar
                           qi::space,
                           _samples_);
    if (!res || str_iter != end_iter) {
      throw std::logic_error("Cannot parse event samples!");
    }
    return;
  }

  void raw_event::_decode_header(const std::string & hline_, int index_)
  {
    namespace qi = boost::spirit::qi;
    std::string::const_iterator str_iter = hline_.begin();
    std::string::const_iterator end_iter = hline_.end();
    // std::cerr << "wctools::raw_event::_decode_header: hline=<" << hline_ << ">" << std::endl;
    bool res = false;

    if (index_ == 0) {
      // First line of the header:
      int ev_id;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              "===" >>
                              qi::lit("EVENT") >> qi::int_
                              >> "==="
                              ),
                             //  End grammar
                             qi::space,
                             ev_id);
      // std::cerr << "wctools::raw_event::_decode_header: res = " << res << std::endl;
      if (!res || str_iter != end_iter) {
        throw std::logic_error("Cannot parse event header line #" + boost::lexical_cast<std::string>(index_));
      }
      // std::cerr << "wctools::raw_event::_decode_header: ev_id = " << ev_id << std::endl;
      _header_->id = ev_id;
    }

    if (index_ == 1) {
      // Second line of the header:
      double      ev_unix_time;
      std::string ev_date;
      std::string ev_time;
      int32_t     ev_tdc;
      std::string ev_tdc_corrected_time;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             // Begin grammar
                             (
                              "===" >>
                              qi::lit("UnixTime = ") >> qi::double_ >>
                              qi::lit("date = ") >> (+~qi::char_("t")) >>
                              qi::lit("time = ") >> (+~qi::char_("=")) >>
                              "==" >> qi::lit("TDC") >> '=' >> qi::int_ >>
                              "==" >> qi::lit("TDC corrected time") >> '=' >> (+~qi::char_("="))
                              >> "==="
                              ),
                             // End grammar
                             qi::space,
                             ev_unix_time, ev_date, ev_time, ev_tdc, ev_tdc_corrected_time);
      if (!res || str_iter != end_iter) {
        throw std::logic_error("Cannot parse event header line #" + boost::lexical_cast<std::string>(index_));
      }
      // std::cerr << "wctools::raw_event::_decode_header: ev_unix_time          = " << ev_unix_time << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_date               = " << ev_date << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_time               = " << ev_time << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_tdc                = " << ev_tdc << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_tdc_corrected_time = " << ev_tdc_corrected_time << std::endl;
      _header_->unix_time = ev_unix_time;
      _header_->date = ev_date;
      _header_->time = ev_time;
      _header_->tdc = ev_tdc;
      _header_->tdc_corrected_time = ev_tdc_corrected_time;
    }

    if (index_ == 2) {
      // Third line of the header:
      int         ev_channel_id;
      int         ev_id;
      int         ev_fcr;
      double      ev_baseline;
      double      ev_amplitude;
      double      ev_charge;
      double      ev_leading_edge_time;
      double      ev_trailing_edge_time;
      double      ev_rate_counter;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             // Begin grammar
                             (
                              "===" >>
                              qi::lit("CH:") >> qi::int_ >>
                              qi::lit("EVENTID:") >> qi::int_ >>
                              qi::lit("FCR:") >> qi::int_ >>
                              qi::lit("Baseline:") >> qi::double_ >> qi::lit("V") >>
                              qi::lit("Amplitude:") >> qi::double_ >> qi::lit("V") >>
                              qi::lit("Charge:") >> qi::double_ >> qi::lit("pC") >>
                              qi::lit("LeadingEdgeTime:") >> qi::double_ >> qi::lit("ns") >>
                              qi::lit("TrailingEdgeTime:") >> qi::double_ >> qi::lit("ns") >>
                              qi::lit("RateCounter") >> qi::double_
                              >> "==="
                              ),
                             // End grammar
                             qi::space,
                             ev_channel_id, ev_id, ev_fcr, ev_baseline, ev_amplitude, ev_charge,
                             ev_leading_edge_time, ev_trailing_edge_time, ev_rate_counter);
      if (!res || str_iter != end_iter) {
        throw std::logic_error("Cannot parse event header line #" + boost::lexical_cast<std::string>(index_));
      }
      // std::cerr << "wctools::raw_event::_decode_header: ev_channel_id         = " << ev_channel_id << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_fcr                = " << ev_fcr << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_baseline           = " << ev_baseline << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_amplitude          = " << ev_amplitude << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_charge             = " << ev_charge << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_leading_edge_time  = " << ev_leading_edge_time << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_trailing_edge_time = " << ev_trailing_edge_time << std::endl;
      // std::cerr << "wctools::raw_event::_decode_header: ev_rate_counter       = " << ev_rate_counter << std::endl;
      _header_->channel_id = ev_channel_id;
      _header_->fcr = ev_fcr;
      _header_->baseline = ev_baseline;
      _header_->amplitude = ev_amplitude;
      _header_->charge = ev_charge;
      _header_->leading_edge_time = ev_leading_edge_time;
      _header_->trailing_edge_time = ev_trailing_edge_time;
      _header_->rate_counter = ev_rate_counter;
    }

    return;
  }

} // namespace wctools
