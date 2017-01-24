// fecom/tracker_hit_parser.cpp

// Ourselves:
#include <fecom/tracker_hit_parser.hpp>

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
  const std::size_t tracker_hit_parser::NB_HEADER_LINES;

  tracker_hit_parser::tracker_hit_parser()
  {
    return;
  }

  bool tracker_hit_parser::parse(std::istream & in_, tracker_hit & hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    bool success = false;
    try {
      // Header:
      for (std::size_t ih = 0; ih < NB_HEADER_LINES; ih++) {
        std::string hline;
        std::getline(in_, hline);
        DT_LOG_DEBUG(logging, "Parsing header line '" << hline << "'");
        _parse_header_(hline, ih, hit_);
        in_ >> std::ws;
      }
      // Data:
      std::string timing_line;
      std::getline(in_, timing_line);
      DT_LOG_DEBUG(logging, "Parsing times line '" << timing_line << "'");
      _parse_time_(timing_line, hit_);
      in_ >> std::ws;
      success = true;
    } catch (std::exception & error) {
      DT_LOG_ERROR(logging, error.what());
      success = false;
    }
    DT_LOG_TRACE_EXITING(logging);
    return success;
  }

  void tracker_hit_parser::_parse_header_(const std::string & header_line_,
					  const int index_,
					  tracker_hit & hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    namespace qi = boost::spirit::qi;
    bool res = false;

    if (index_ == 0) {
      std::string::const_iterator str_iter = header_line_.begin();
      std::string::const_iterator end_iter = header_line_.end();
      uint32_t hit_id;
      std::string hit_type;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              qi::lit("===HIT") >> qi::uint_
                              >> "==="
                              >> (+~qi::char_("="))
                              >> "==="
			      ),
                             //  End grammar
                             qi::space,
                             hit_id, hit_type);
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse file header line #" << index_);
      DT_LOG_DEBUG(logging, "hit_id = " << hit_id);
      DT_LOG_DEBUG(logging, "hit_type = " << hit_type);
      DT_THROW_IF(hit_type != "TRACKER_HIT", std::logic_error, "Invalid hit type label '" << hit_type << "'!");
      hit_.hit_id = hit_id;
      hit_.hitmode = base_hit::SIG_TRACKER;
    }

    if (index_ == 1) {
      std::string header_line = header_line_;
      // We use a trick because of nasty syntax from the DAQ ascii output:
      boost::replace_all(header_line, "Slot", "Slot ");
      boost::replace_all(header_line, " Ch", " Ch ");
      DT_LOG_DEBUG(logging, "header_line = '" << header_line << "'");
      std::string::const_iterator str_iter = header_line.begin();
      std::string::const_iterator end_iter = header_line.end();
      uint32_t slotid    = 0xFFFFFFFF;
      uint32_t channelid = 16;
      uint32_t eventid   = 0xFFFFFFFF;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              qi::lit("Slot")          >> qi::uint_[boost::phoenix::ref(slotid) = boost::spirit::qi::_1]
                              >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(channelid) = boost::spirit::qi::_1]
                              >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(eventid) = boost::spirit::qi::_1]
			      ),
                             //  End grammar
                             qi::space);
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse file header line #" << index_ << "; failed at '" << *str_iter << "'!");
      DT_LOG_DEBUG(logging, "slotid        = " << slotid);
      DT_LOG_DEBUG(logging, "channelid     = " << channelid);
      DT_LOG_DEBUG(logging, "eventid       = " << eventid);
      hit_.slot_index = slotid;
      hit_.channel = channelid;
      hit_.event_id = eventid;
    }

    DT_LOG_TRACE_EXITING(logging);
    return;
  }

  void tracker_hit_parser::_parse_time_(const std::string & data_line_,
					tracker_hit & hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    hit_.reset_times();
    namespace qi = boost::spirit::qi;

    std::string::const_iterator str_iter = data_line_.begin();
    std::string::const_iterator end_iter = data_line_.end();
    bool res = false;
    uint64_t anodic_t0 = tracker_constants::INVALID_TIME;
    uint64_t anodic_t1 = tracker_constants::INVALID_TIME;
    uint64_t anodic_t2 = tracker_constants::INVALID_TIME;
    uint64_t anodic_t3 = tracker_constants::INVALID_TIME;
    uint64_t anodic_t4 = tracker_constants::INVALID_TIME;
    uint64_t cathodic_t5 = tracker_constants::INVALID_TIME;
    uint64_t cathodic_t6 = tracker_constants::INVALID_TIME;
    res = qi::phrase_parse(str_iter,
                           end_iter,
                           //  Begin grammar
			   (
			    qi::lit("T0")     >> qi::uint_[boost::phoenix::ref(anodic_t0) = boost::spirit::qi::_1]
			    >> qi::lit("T1")  >> qi::uint_[boost::phoenix::ref(anodic_t1) = boost::spirit::qi::_1]
			    >> qi::lit("T2")  >> qi::uint_[boost::phoenix::ref(anodic_t2) = boost::spirit::qi::_1]
			    >> qi::lit("T3")  >> qi::uint_[boost::phoenix::ref(anodic_t3) = boost::spirit::qi::_1]
			    >> qi::lit("T4")  >> qi::uint_[boost::phoenix::ref(anodic_t4) = boost::spirit::qi::_1]
			    >> qi::lit("T5")  >> qi::uint_[boost::phoenix::ref(cathodic_t5) = boost::spirit::qi::_1]
			    >> qi::lit("T6")  >> qi::uint_[boost::phoenix::ref(cathodic_t6) = boost::spirit::qi::_1]
			    ),
			   //  End grammar
			   qi::space);
    DT_THROW_IF(!res || str_iter != end_iter,
		std::logic_error,
		"Cannot parse event samples!");
    hit_.anodic_t0 = anodic_t0;
    hit_.anodic_t1 = anodic_t1;
    hit_.anodic_t2 = anodic_t2;
    hit_.anodic_t3 = anodic_t3;
    hit_.anodic_t4 = anodic_t4;
    hit_.cathodic_t5 = cathodic_t5;
    hit_.cathodic_t6 = cathodic_t6;

    DT_LOG_TRACE_EXITING(logging);
    return;
  }

} // namespace fecom
