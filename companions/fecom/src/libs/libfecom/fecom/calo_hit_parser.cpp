// fecom/calo_hit_parser.cpp

// Ourselves:
#include <fecom/calo_hit_parser.hpp>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>

namespace fecom {

  // static
  const std::size_t calo_hit_parser::NB_HEADER_LINES;

  calo_hit_parser::calo_hit_parser()
  {
    return;
  }

  bool calo_hit_parser::parse(std::istream & in_, calo_hit & hit_)
  {
    bool succes = false;

    try {
      std::string hlines[NB_HEADER_LINES];
      for (std::size_t ih = 0; ih < NB_HEADER_LINES; ih++) {
        std::getline(in_, hlines[ih]);
        _decode_header(hlines[ih], ih);
        in_ >> std::ws;
      }

      std::string raw_waveform_data_line;
      std::getline(in_, raw_waveform_data_line);
      _parse_samples(raw_waveform_data_line);
      in_ >> std::ws;
      success = true;
    } catch (std::exception & error) {
      success = false;
    }
    return success;
  }

  void calo_hit_parser::_parse_header(const std::string & header_line_, int index_)
  {
    namespace qi = boost::spirit::qi;
    std::string::const_iterator str_iter = header_line_.begin();
    std::string::const_iterator end_iter = header_line_.end();
    bool res = false;

    if (index_ == 0) {
    }

    if (index_ == 1) {
    }

    return;
  }

  void calo_hit_parser::_parse_samples(const std::string & data_line_)
  {
    _reset_raw_waveform_data_();
    namespace qi = boost::spirit::qi;

    std::string::const_iterator str_iter = data_line_.begin();
    std::string::const_iterator end_iter = data_line_.end();
    bool res = false;
    res = qi::phrase_parse(str_iter,
                           end_iter,
                           //  Begin grammar
                           (
                            qi::repeat(waveform_data_size)[qi::int_]
                            ),
                           //  End grammar
                           qi::space,
                           _reset_raw_waveform_data_);
    if (!res || str_iter != end_iter) {
      throw std::logic_error("Cannot parse event samples!");
    }

    return;
  }

} // namespace fecom
