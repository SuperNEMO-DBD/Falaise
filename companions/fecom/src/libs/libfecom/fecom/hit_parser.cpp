// fecom/hit_parser.cpp

// Ourselves:
#include <fecom/hit_parser.hpp>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/phoenix/phoenix.hpp>
// #include <boost/fusion/include/at_c.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

namespace fecom {

  // static
  const std::size_t hit_parser::NB_HIT_HEADER_LINES;

  hit_parser::hit_parser()
  {
    _hit_mode_type_ = base_hit::SIG_INVALID;
    return;
  }

  bool hit_parser::parse(std::istream & in_,
			 calo_hit     & a_calo_hit_,
			 tracker_channel_hit  & a_tracker_channel_hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    bool success = false;
    try {
      // Header:
      for (std::size_t ih = 0; ih < NB_HIT_HEADER_LINES; ih++) {
        std::string hline;
        std::getline(in_, hline);
        DT_LOG_DEBUG(logging, "Parsing header line number " << ih << " : '" << hline << "'");
        _parse_hit_header_(hline,
			   ih);
        in_ >> std::ws;
      }

      DT_LOG_DEBUG(logging, "Internal hit mode type after hit header parsing = " << _hit_mode_type_ << "'");

      // Calo or tracker parser
      if (_hit_mode_type_ == base_hit::SIG_CALORIMETER){
	a_calo_hit_.hit_id = _hit_id_;
	a_calo_hit_.hitmode = base_hit::SIG_CALORIMETER;
	a_calo_hit_.trigger_id = _trigger_id_;
	_calo_hit_parser_.parse(in_, a_calo_hit_);
      }
      else if (_hit_mode_type_ == base_hit::SIG_TRACKER){
	a_tracker_channel_hit_.hit_id = _hit_id_;
	a_tracker_channel_hit_.hitmode = base_hit::SIG_TRACKER;
	a_tracker_channel_hit_.trigger_id = _trigger_id_;
	_tracker_channel_hit_parser_.parse(in_, a_tracker_channel_hit_);

      }
      else DT_THROW(std::logic_error, "Unknow hit type, check your file !");
      in_ >> std::ws;
      success = true;

    } catch (std::exception & error) {
      DT_LOG_ERROR(logging, error.what());
      success = false;
    }
    DT_LOG_TRACE_EXITING(logging);
    return success;
  }

  void hit_parser::_parse_hit_header_(const std::string & header_line_,
				      const int index_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    namespace qi = boost::spirit::qi;
    bool res = false;
    std::string hit_type;

    if (index_ == 0) {
      std::string::const_iterator str_iter = header_line_.begin();
      std::string::const_iterator end_iter = header_line_.end();
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              qi::lit("===HIT")
			      >> qi::uint_
                              >> "==="
                              >> (+~qi::char_("="))
                              >> "==="
			      >> qi::lit("TRIGGER_ID")
			      >> qi::uint_
			      >> "==="
			      ),
                             //  End grammar
                             qi::space,
                             _hit_id_, hit_type, _trigger_id_);
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse file header line #" << index_);
      DT_LOG_DEBUG(logging, "_hit_id_ = " << _hit_id_);
      DT_LOG_DEBUG(logging, "hit_type = " << hit_type);
      DT_LOG_DEBUG(logging, "_trigger_id_ = " << _trigger_id_);
      DT_THROW_IF(hit_type != "CALO_HIT" && hit_type != "TRACKER_HIT", std::logic_error, "Invalid hit type label '" << hit_type << "'!");

      if (hit_type == "CALO_HIT") _hit_mode_type_ = base_hit::SIG_CALORIMETER;
      else if (hit_type == "TRACKER_HIT") _hit_mode_type_ = base_hit::SIG_TRACKER;
    }


    DT_LOG_TRACE_EXITING(logging);
    return;
  }

} // namespace fecom
