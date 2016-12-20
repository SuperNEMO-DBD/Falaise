// fecom/calo_hit_parser.cpp

// Ourselves:
#include <fecom/calo_hit_parser.hpp>

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
  const std::size_t calo_hit_parser::NB_HEADER_LINES;

  calo_hit_parser::calo_hit_parser()
  {
    return;
  }

  bool calo_hit_parser::parse(std::istream & in_, calo_hit & hit_)
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
      std::string raw_waveform_data_line;
      std::getline(in_, raw_waveform_data_line);
      DT_LOG_DEBUG(logging, "Parsing raw waveform data line '" << raw_waveform_data_line << "'");
      _parse_samples_(raw_waveform_data_line, hit_);
      in_ >> std::ws;
      success = true;
    } catch (std::exception & error) {
      DT_LOG_ERROR(logging, error.what());
      success = false;
    }
    DT_LOG_TRACE_EXITING(logging);
    return success;
  }

  void calo_hit_parser::_parse_header_(const std::string & header_line_,
                                       const int index_,
                                       calo_hit & hit_)
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
      DT_THROW_IF(hit_type != "CALO_HIT", std::logic_error, "Invalid hit type label '" << hit_type << "'!");
      hit_.hit_id = hit_id;
      hit_.hitmode = base_hit::SIG_CALORIMETER;
    }

    if (index_ == 1) {
      std::string header_line = header_line_;
      boost::replace_all(header_line, "Slot", "Slot ");
      boost::replace_all(header_line, " Ch", " Ch ");
      // XXX
      // header_line = "Slot 0 Ch 0 EvtID 250 RawTDC 2378697594 TrigCount 3 Timecount 19886 RawBaseline 2 RawPeak -1529 RawCharge -4051 Overflow 0 "; //RisingCell 923 RisingOffset 219 "; //FallingCell 908 FallingOffset 242 FCR 656
      DT_LOG_DEBUG(logging, "header_line = '" << header_line << "'");
      std::string::const_iterator str_iter = header_line.begin();
      std::string::const_iterator end_iter = header_line.end();
      // std::string slotidrepr;
      // std::string channelidrepr;
      uint32_t slotid = 0xFFFFFFFF;
      uint32_t channelid = 16;
      uint32_t eventid = 0xFFFFFFFF;
      uint32_t rawtdc = 0; // 64bits
      uint32_t trigcount;
      uint32_t timecount;
      int32_t  rawbaseline;
      int32_t  rawpeak;
      int32_t  rawcharge;
      uint32_t overflow;
      uint32_t risingcell;
      uint32_t risingoffset;
      uint32_t fallingcell;
      uint32_t fallingoffset;
      uint32_t fcr;
      boost::fusion::vector<char, char, char, char, std::vector<char> > slotwork;
      boost::fusion::vector<char, char, std::vector<char> > channelwork;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              // qi::lexeme[qi::lit("Slot") >> qi::uint_[boost::phoenix::ref(eventid) = boost::spirit::qi::_1]]
                              // >> qi::lit("Ch") >> qi::uint_[boost::phoenix::ref(channelid) = boost::spirit::qi::_1]
                              // >> qi::lexeme[qi::lit("Ch") >> +qi::digit][boost::phoenix::ref(channelidrepr) = boost::spirit::qi::_1]
                              // (qi::lexeme[qi::char_('S') >> qi::char_('l') >> qi::char_('o') >> qi::char_('t') >> +qi::digit])[boost::phoenix::ref(slotwork) = boost::spirit::qi::_1]
                              // >> (qi::lexeme[qi::char_('C') >> qi::char_('h') >> +qi::digit])[boost::phoenix::ref(channelwork) = boost::spirit::qi::_1]
                                 qi::lit("Slot")          >> qi::uint_[boost::phoenix::ref(slotid) = boost::spirit::qi::_1]
                              >> qi::lit("Ch")            >> qi::uint_[boost::phoenix::ref(channelid) = boost::spirit::qi::_1]
                              >> qi::lit("EvtID")         >> qi::uint_[boost::phoenix::ref(eventid) = boost::spirit::qi::_1]
                              >> qi::lit("RawTDC")        >> qi::uint_[boost::phoenix::ref(rawtdc) = boost::spirit::qi::_1]
                              >> qi::lit("TrigCount")     >> qi::uint_[boost::phoenix::ref(trigcount) = boost::spirit::qi::_1]
                              >> qi::lit("Timecount")     >> qi::uint_[boost::phoenix::ref(timecount) = boost::spirit::qi::_1]
                              >> qi::lit("RawBaseline")   >> qi::int_[boost::phoenix::ref(rawbaseline) = boost::spirit::qi::_1]
                              >> qi::lit("RawPeak")       >> qi::int_[boost::phoenix::ref(rawpeak) = boost::spirit::qi::_1]
                              >> qi::lit("RawCharge")     >> qi::int_[boost::phoenix::ref(rawcharge) = boost::spirit::qi::_1]
                              >> qi::lit("Overflow")      >> qi::uint_[boost::phoenix::ref(overflow) = boost::spirit::qi::_1]
                              >> qi::lit("RisingCell")    >> qi::uint_[boost::phoenix::ref(risingcell) = boost::spirit::qi::_1]
                              >> qi::lit("RisingOffset")  >> qi::uint_[boost::phoenix::ref(risingoffset) = boost::spirit::qi::_1]
                              >> qi::lit("FallingCell")   >> qi::uint_[boost::phoenix::ref(fallingcell) = boost::spirit::qi::_1]
                              >> qi::lit("FallingOffset") >> qi::uint_[boost::phoenix::ref(fallingoffset) = boost::spirit::qi::_1]
                              >> qi::lit("FCR")           >> qi::uint_[boost::phoenix::ref(fcr) = boost::spirit::qi::_1]
                             ),
                             //  End grammar
                             qi::space);
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse file header line #" << index_ << "; failed at '" << *str_iter << "'!");
      // DT_LOG_DEBUG(logging, "slotidrepr = " << slotidrepr);
      // DT_LOG_DEBUG(logging, "channelidrepr = " << channelidrepr);
      // {
      //   std::vector<char> sw = boost::fusion::at_c<4>(slotwork); //slotidrepr.substr(4);
      //   std::vector<char> cw = boost::fusion::at_c<2>(channelwork); //channelidrepr.substr(4);
      //   std::string s;
      //   std::string c;
      //   for (auto x : sw) s.push_back(x);
      //   for (auto x : cw) c.push_back(x);
      //   // std::copy(sw.begin(), sw.end(), s);
      //   // std::copy(cw.begin(), cw.end(), c);
      //   DT_LOG_DEBUG(logging, "s = " << s);
      //   DT_LOG_DEBUG(logging, "c = " << c);
      //   slotid = boost::lexical_cast<uint32_t>(s);
      //   channelid = boost::lexical_cast<uint32_t>(c);
      // }
      DT_LOG_DEBUG(logging, "slotid        = " << slotid);
      DT_LOG_DEBUG(logging, "channelid     = " << channelid);
      DT_LOG_DEBUG(logging, "eventid       = " << eventid);
      DT_LOG_DEBUG(logging, "rawtdc        = " << rawtdc);
      DT_LOG_DEBUG(logging, "trigcount     = " << trigcount);
      DT_LOG_DEBUG(logging, "timecount     = " << timecount);
      DT_LOG_DEBUG(logging, "rawbaseline   = " << rawbaseline);
      DT_LOG_DEBUG(logging, "rawpeak       = " << rawpeak);
      DT_LOG_DEBUG(logging, "rawcharge     = " << rawcharge);
      DT_LOG_DEBUG(logging, "overflow      = " << overflow);
      DT_LOG_DEBUG(logging, "risingcell    = " << risingcell);
      DT_LOG_DEBUG(logging, "risingoffset  = " << risingoffset);
      DT_LOG_DEBUG(logging, "fallingcell   = " << fallingcell);
      DT_LOG_DEBUG(logging, "fallingoffset = " << fallingoffset);
      DT_LOG_DEBUG(logging, "fcr           = " << fcr);
      hit_.slot_index = slotid;
      hit_.channel = channelid;
      hit_.event_id = eventid;
      hit_.raw_tdc = rawtdc;
      hit_.low_threshold_trig_count = trigcount;
      hit_.low_threshold_time_count = timecount;
      // hit_.waveform_data_size = 1024;
      hit_.fcr = fcr;
      hit_.raw_baseline = rawbaseline;
      hit_.raw_peak = rawpeak;
      hit_.raw_charge = rawcharge;
      hit_.raw_charge_overflow = (overflow == 0 ? false: true);
    }

    DT_LOG_TRACE_EXITING(logging);
    return;
  }

  void calo_hit_parser::_parse_samples_(const std::string & data_line_,
                                        calo_hit & hit_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    hit_._reset_raw_waveform_data_();
    namespace qi = boost::spirit::qi;

    std::string::const_iterator str_iter = data_line_.begin();
    std::string::const_iterator end_iter = data_line_.end();
    std::vector<int> raw_waveform_data;
    std::size_t waveform_data_size = 0; // unknown from the header !!! so we let Spirit guess it...
    bool res = false;
    res = qi::phrase_parse(str_iter,
                           end_iter,
                           //  Begin grammar
                           (
                            // qi::repeat(hit_.waveform_data_size)[qi::int_]
                            +qi::int_
                            ),
                           //  End grammar
                           qi::space,
                           raw_waveform_data);
     DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                 "Cannot parse event samples!");
     // DT_THROW_IF(raw_waveform_data.size() != hit_.waveform_data_size,
     //             std::logic_error, "Invalid waveform size!");
     // Waveform:
     hit_.waveform_data_size = raw_waveform_data.size();
     for (uint16_t icell = 0; icell < raw_waveform_data.size(); icell++) {
       hit_.set_raw_sample(icell, (int16_t) raw_waveform_data[icell]);
     }
     DT_LOG_TRACE_EXITING(logging);
     return;
  }

} // namespace fecom
