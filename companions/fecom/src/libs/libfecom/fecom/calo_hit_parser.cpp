// fecom/calo_hit_parser.cpp

// Ourselves:
#include <fecom/calo_hit_parser.hpp>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>
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
        _parse_header_(hline, ih, hit_);
        in_ >> std::ws;
      }
      // Data:
      std::string raw_waveform_data_line;
      std::getline(in_, raw_waveform_data_line);
      _parse_samples_(raw_waveform_data_line, hit_);
      in_ >> std::ws;
      success = true;
    } catch (std::exception & error) {
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
    std::string::const_iterator str_iter = header_line_.begin();
    std::string::const_iterator end_iter = header_line_.end();
    bool res = false;

    if (index_ == 0) {
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
      uint32_t slotid;
      uint32_t channelid;
      uint32_t eventid;
      uint32_t rawtdc; // 64bits
      uint32_t trigcount;
      uint32_t timecount;
      int32_t rawbaseline;
      int32_t rawpeak;
      int32_t rawcharge;
      bool overflow;
      uint32_t risingcell;
      uint32_t risingoffset;
      uint32_t fallingcell;
      uint32_t fallingoffset;
      uint32_t fcr;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              qi::lit("Slot") >> qi::uint_[&slotid]
                              >> qi::lit("Ch") >> qi::uint_[&channelid]
                              >> qi::lit("EvtID") >> qi::uint_[&eventid]
                              >> qi::lit("RawTDC") >> qi::uint_[&rawtdc]
                              >> qi::lit("TrigCount") >> qi::uint_[&trigcount]
                              >> qi::lit("Timecount") >> qi::uint_[&timecount]
                              >> qi::lit("RawBaseline") >> qi::int_[&rawbaseline]
                              >> qi::lit("RawPeak") >> qi::int_[&rawpeak]
                              >> qi::lit("RawCharge") >> qi::int_[&rawcharge]
                              >> qi::lit("Overflow") >> qi::bool_[&overflow]
                              >> qi::lit("RisingCell") >> qi::uint_[&risingcell]
                              >> qi::lit("RisingOffset") >> qi::uint_[&risingoffset]
                              >> qi::lit("FallingCell") >> qi::uint_[&fallingcell]
                              >> qi::lit("FallingOffset") >> qi::uint_[&fallingoffset]
                              >> qi::lit("FCR") >> qi::uint_[&fcr]
                             ),
                             //  End grammar
                             qi::space);
      // FIX PARSING ABOVE!
      DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                  "Cannot parse file header line #" << index_);
      DT_LOG_DEBUG(logging, "slotid = " << slotid);
      DT_LOG_DEBUG(logging, "channelid = " << channelid);
      hit_.slot_index = slotid;
      hit_.channel = channelid;
      hit_.event_id = eventid;
      hit_.raw_tdc = rawtdc;
      hit_.low_threshold_trig_count = trigcount;
      hit_.low_threshold_time_count = timecount;
      hit_.waveform_data_size = 1024;
      hit_.fcr = fcr;
      hit_.raw_baseline = rawbaseline;
      hit_.raw_peak = rawpeak;
      hit_.raw_charge = rawcharge;
      hit_.raw_charge_overflow = overflow;
      // XXX
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
    bool res = false;
    res = qi::phrase_parse(str_iter,
                           end_iter,
                           //  Begin grammar
                           (
                            qi::repeat(hit_.waveform_data_size)[qi::int_]
                            ),
                           //  End grammar
                           qi::space,
                           raw_waveform_data);
     DT_THROW_IF(!res || str_iter != end_iter,
                  std::logic_error,
                 "Cannot parse event samples!");
     DT_THROW_IF(raw_waveform_data.size() != hit_.waveform_data_size,
                 std::logic_error, "Invalid waveform size!");
     // Waveform:
     hit_.waveform_data_size = raw_waveform_data.size();
     for (uint16_t icell = 0; icell < raw_waveform_data.size(); icell++) {
       hit_.set_raw_sample(icell, (int16_t) raw_waveform_data[icell]);
     }
     DT_LOG_TRACE_EXITING(logging);
     return;
  }

} // namespace fecom
