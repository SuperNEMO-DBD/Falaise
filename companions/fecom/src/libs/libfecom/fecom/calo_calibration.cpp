// fecom/calo_calibration.cpp

// Ourselves:
#include <fecom/calo_calibration.hpp>

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

namespace fecom {

  calo_calibration::calo_calibration()
  {
  }

  void calo_calibration::clear_pedestals()
  {
    calo_pedestals.clear();
  }

  void calo_calibration::_parse_pedestals_header_(const std::string & header_line_,
                                                  calo_pedestal_header & header_)
  {
    namespace qi = boost::spirit::qi;
    std::string::const_iterator str_iter = header_line_.begin();
    std::string::const_iterator end_iter = header_line_.end();
    uint16_t start_cell = 0;
    uint16_t stop_cell = 1023;
    uint16_t start_channel = 0;
    uint16_t stop_channel = 19;
    bool res = false;
    {
      std::string sw_version;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              qi::lit("Pedestal Calibration values ( int values ADC Counts *16) from cell ")
                              >> qi::uint_
                              >> qi::lit("to")
                              >> qi::uint_
                              >> qi::lit("for channels")
                              >> qi::uint_
                              >> qi::lit("to")
                              >> qi::uint_
                              >> qi::lit(":")
                              ),
                             //  End grammar
                             qi::space,
                             start_cell, stop_cell, start_channel, stop_channel);
      if (!res || str_iter != end_iter) {
        DT_THROW(std::logic_error, "Cannot parse file header line '" << header_line_ << "'");
      }
      header_.start_cell = start_cell;
      header_.stop_cell = stop_cell;
      header_.start_cell = start_channel;
      header_.stop_cell = stop_channel;
    }
    return;
   }

  void calo_calibration::_parse_pedestals_per_channel_(const std::string & offsets_line_,
                                                       const int channel_id_,
                                                       calo_pedestal_parsing_data & parsing_data_)
  {
    namespace qi = boost::spirit::qi;
    std::string::const_iterator str_iter = offsets_line_.begin();
    std::string::const_iterator end_iter = offsets_line_.end();
    bool res = false;
    std::vector<int> offsets;
    {
      std::size_t ncells = parsing_data_.header.stop_cell - parsing_data_.header.start_cell + 1;
      res = qi::phrase_parse(str_iter,
                             end_iter,
                             //  Begin grammar
                             (
                              qi::repeat(ncells)[qi::int_]
                              ),
                             //  End grammar
                             qi::space,
                             offsets);

      if (!res || str_iter != end_iter) {
        DT_THROW(std::logic_error, "Cannot parse file header line '" << offsets_line_ << "'");
      }

     calo_channel_id id(parsing_data_.board_id, channel_id_);
     calo_pedestal_calib calib;
     calib.slot_index = (uint16_t) id.board_id;
     calib.channel = (uint8_t) id.slot_id;
     calib.offset_size = (uint16_t) offsets.size();
     for (std::size_t ioffset = parsing_data_.header.start_cell;
          ioffset <= parsing_data_.header.stop_cell;
          ioffset++) {
       calib.set_offset(ioffset, (uint16_t) offsets.at(ioffset));
     }
     calo_pedestals.insert(std::pair<calo_channel_id, calo_pedestal_calib>(id, calib));
    }
    return;
  }

  bool calo_calibration::_parse_pedestals_(std::istream & in_,
                                           calo_pedestal_parsing_data & parsing_data_)
  {
    bool success = false;
    try {

      {
        // Parsing header:
        std::string hline;
        std::getline(in_, hline);
        in_ >> std::ws;
        _parse_pedestals_header_(hline, parsing_data_.header);
      }

      {
        // Parsing pedestals data:
        for (int channel_id = parsing_data_.header.start_channel;
             channel_id < parsing_data_.header.stop_channel;
             channel_id++) {
          // Channel header line:
          std::string pdline1;
          std::getline(in_, pdline1);
          in_ >> std::ws;
          {
            namespace qi = boost::spirit::qi;
            std::string::const_iterator str_iter = pdline1.begin();
            std::string::const_iterator end_iter = pdline1.end();
            int parsed_channel_id = -1;
            bool res = false;
            {
              std::string sw_version;
              res = qi::phrase_parse(str_iter,
                                     end_iter,
                                     //  Begin grammar
                                     (
                                      qi::lit("Channel")
                                      >> qi::uint_
                                      ),
                                     //  End grammar
                                     qi::space,
                                     parsed_channel_id);
              if (!res || str_iter != end_iter) {
                DT_THROW(std::logic_error, "Cannot parse channel ID from '" << pdline1 << "'");
              }
            }
            DT_THROW_IF(parsed_channel_id != channel_id,
                        std::logic_error, "Unexpected channel ID [" << parsed_channel_id <<
                        " != " << channel_id << "]!");
          }

          // Offset line:
          std::string pdline2;
          std::getline(in_, pdline2);
          in_ >> std::ws;
          _parse_pedestals_per_channel_(pdline2, channel_id, parsing_data_);
        }
      }

    } catch (std::exception & error) {
      success = false;
    }
    return success;
  }

  void calo_calibration::load_pedestals(const uint16_t board_id_, const std::string & filename_)
  {
    std::string fname = filename_;
    datatools::fetch_path_with_env(fname);
    std::ifstream ipedfile(fname.c_str());

    calo_pedestal_parsing_data parsing_data;
    parsing_data.board_id = board_id_;
    _parse_pedestals_(ipedfile, parsing_data);

    ipedfile.close();
    return;
  }

} // namespace fecom
