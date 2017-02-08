// fecom/tracker_channel_hit_parser.cpp

// Ourselves:
#include <fecom/tracker_channel_hit_parser.hpp>

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
  const std::size_t tracker_channel_hit_parser::NB_TRACKER_HEADER_LINES;

  tracker_channel_hit_parser::tracker_channel_hit_parser()
  {
    return;
  }

  bool tracker_channel_hit_parser::parse(std::istream & in_, tracker_channel_hit & hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    bool success = false;
    try {
      // No header for tracker channel hit
      // // Header:
      // for (std::size_t ih = 0; ih < NB_TRACKER_HEADER_LINES; ih++) {
      //   std::string hline;
      //   std::getline(in_, hline);
      //   DT_LOG_DEBUG(logging, "Parsing header line '" << hline << "'");
      //   _parse_header_(hline, ih, hit_);
      //   in_ >> std::ws;
      // }
      // Data:
      std::string data_line;
      std::getline(in_, data_line);
      DT_LOG_DEBUG(logging, "Parsing data line '" << data_line << "'");
      _parse_timestamp_(data_line, hit_);
      in_ >> std::ws;
      success = true;
    } catch (std::exception & error) {
      DT_LOG_ERROR(logging, error.what());
      success = false;
    }
    DT_LOG_TRACE_EXITING(logging);
    return success;
  }

  void tracker_channel_hit_parser::_parse_timestamp_(const std::string & data_line_,
						     tracker_channel_hit & hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    std::string data_line = data_line_;
    hit_.reset_timestamp();
    namespace qi = boost::spirit::qi;
    bool res = false;

    // We use a trick because of nasty syntax from the DAQ ascii output:
    boost::replace_all(data_line, "Slot", "Slot ");
    boost::replace_all(data_line, " Feast", " Feast ");
    boost::replace_all(data_line, " Ch", " Ch ");

    DT_LOG_DEBUG(logging, "data_line before parsing  = " << data_line);

    std::string::const_iterator str_iter = data_line.begin();
    std::string::const_iterator end_iter = data_line.end();
    uint16_t slot_id    = 0xFFFF;
    uint16_t feast_id   = 0xFFFF;
    uint16_t channel_id = 0xFFFF;
    // uint16_t eventid   = 0xFFFF;
    std::string channel_type ="INVALID";
    std::string timestamp_type = "INVALID";
    uint32_t timestamp_value  = 0xFFFFFFFF;
    res = qi::phrase_parse(str_iter,
    			   end_iter,
    			   //  Begin grammar
    			   (
    			    qi::lit("Slot")     >> qi::uint_[boost::phoenix::ref(slot_id) = boost::spirit::qi::_1]
    			    >> qi::lit("Feast") >> qi::uint_[boost::phoenix::ref(feast_id) = boost::spirit::qi::_1]
    			    >> qi::lit("Ch")    >> qi::uint_[boost::phoenix::ref(channel_id) = boost::spirit::qi::_1]
			    >> (qi::string("Anodic") |
				qi::string("Cathodic"))[boost::phoenix::ref(channel_type) = boost::spirit::qi::_1]
			    >> (qi::string("t0") |
				qi::string("t1") |
				qi::string("t2") |
				qi::string("t3") |
				qi::string("t4") |
				qi::string("t5") |
				qi::string("t6"))[boost::phoenix::ref(timestamp_type) = boost::spirit::qi::_1]
			    >> qi::uint_[boost::phoenix::ref(timestamp_value) = boost::spirit::qi::_1]
    			    ),
    			   //  End grammar
    			   qi::space);
    DT_THROW_IF(!res || str_iter != end_iter,
    		std::logic_error,
    		"Cannot parse file timestamp : " << data_line << "; failed at '" << *str_iter << "'!");
    DT_LOG_DEBUG(logging, "slot_id         = " << slot_id);
    DT_LOG_DEBUG(logging, "feast_id        = " << feast_id);
    DT_LOG_DEBUG(logging, "channel_id      = " << channel_id);
    DT_LOG_DEBUG(logging, "channel_type    = " << channel_type);
    DT_LOG_DEBUG(logging, "timestamp_type  = " << timestamp_type);
    DT_LOG_DEBUG(logging, "timestamp_value = " << timestamp_value);
    hit_.slot_index = slot_id;
    hit_.feast_id = feast_id;
    hit_.channel = channel_id;
    if (channel_type == "Anodic") {
      hit_.channel_type = fecom::tracker_channel_hit::ANODIC_CHANNEL;
    }
    else if (channel_type == "Cathodic") {
      hit_.channel_type = fecom::tracker_channel_hit::CATHODIC_CHANNEL;
    }
    hit_.timestamp_type = timestamp_type;
    hit_.timestamp_value = timestamp_value;

    DT_LOG_TRACE_EXITING(logging);
    return;
  }


  // void tracker_channel_hit_parser::_parse_header_(const std::string & header_line_,
  // 						  const int index_,
  // 						  tracker_channel_hit & hit_)
  // {
  //   DT_LOG_TRACE_ENTERING(logging);
  //   namespace qi = boost::spirit::qi;
  //   bool res = false;

  //   if (index_ == 0) {
  //     std::string header_line = header_line_;
  //     // We use a trick because of nasty syntax from the DAQ ascii output:
  //     boost::replace_all(header_line, "Slot", "Slot ");
  //     boost::replace_all(header_line, " Ch", " Ch ");
  //     DT_LOG_DEBUG(logging, "header_line = '" << header_line << "'");
  //     std::string::const_iterator str_iter = header_line.begin();
  //     std::string::const_iterator end_iter = header_line.end();
  //     uint32_t slotid    = 0xFFFFFFFF;
  //     uint32_t channelid = 16;
  //     uint32_t eventid   = 0xFFFFFFFF;
  //     res = qi::phrase_parse(str_iter,
  //                            end_iter,
  //                            //  Begin grammar
  //                            (
  //                             qi::lit("Slot")          >> qi::uint_[boost::phoenix::ref(slotid) = boost::spirit::qi::_1]
  //                             >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(channelid) = boost::spirit::qi::_1]
  //                             >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(eventid) = boost::spirit::qi::_1]
  // 			      ),
  //                            //  End grammar
  //                            qi::space);
  //     DT_THROW_IF(!res || str_iter != end_iter,
  //                 std::logic_error,
  //                 "Cannot parse file header line #" << index_ << "; failed at '" << *str_iter << "'!");
  //     DT_LOG_DEBUG(logging, "slotid        = " << slotid);
  //     DT_LOG_DEBUG(logging, "channelid     = " << channelid);
  //     DT_LOG_DEBUG(logging, "eventid       = " << eventid);
  //     hit_.slot_index = slotid;
  //     hit_.channel = channelid;
  //     hit_.event_id = eventid;
  //   }

  //   DT_LOG_TRACE_EXITING(logging);
  //   return;
  // }

} // namespace fecom
