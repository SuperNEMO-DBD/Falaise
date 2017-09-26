#include "catch.hpp"

#include <fstream>
#include <boost/tokenizer.hpp>

#include "bayeux/datatools/service_manager.h"
#include "falaise/snemo/cuts/event_header_cut.h"

// Should really test with dummy datasets, so this is only
// a quick and dirty extract of the event list
std::set<std::string> get_eid_list(snemo::cut::event_header_cut& e) {
  std::ostringstream oput;
  e.list_of_event_ids_dump(oput);
  boost::char_separator<char> sep("\n ");
  boost::tokenizer<boost::char_separator<char>> tok(oput.str(),sep);
  std::set<std::string> tmp;
  tmp.insert(tok.begin(), tok.end());
  return tmp;
}


TEST_CASE("Construction works","") {
  using event_header_cut = snemo::cut::event_header_cut;
  event_header_cut EHC;

}


// Can only configure via a properties object...
TEST_CASE("Event ID lists are parsed correctly","") {
  using event_header_cut = snemo::cut::event_header_cut;
  event_header_cut EHC;
  datatools::properties input;
  input.store("EH_label","EH");
  input.store("mode.list_of_event_ids", true);

  std::set<std::string> eid_input = {"0_1", "-1_1", "-2_1", "-2_-2"};
  auto eid_expected_output = eid_input;
  eid_expected_output.erase("-1_1");

  datatools::service_manager dummySM;
  cuts::cut_handle_dict_type dummyCH;

  SECTION("Event list from properties is correct") {
    std::vector<std::string> eid_prop(eid_input.begin(), eid_input.end());
    input.store("list_of_event_ids.ids", eid_prop);

    REQUIRE_NOTHROW( EHC.initialize(input, dummySM, dummyCH) );
    REQUIRE( EHC.is_mode_list_of_event_ids() );

    auto eid_output = get_eid_list(EHC);
    REQUIRE (eid_output.size() == eid_expected_output.size());
    REQUIRE (eid_output == eid_expected_output);
  }

  SECTION("Event list from file is correct") {
    // NB: create file on the fly in CWD so we
    // can match up eid lists...
    std::string lis_file {"test_snemo_cut_event_header_cut.lis"};
    {
      std::ofstream eidfile(lis_file, std::ios::trunc);
      std::copy(eid_input.begin(), eid_input.end(),
                std::ostream_iterator<std::string>(eidfile,"\n"));
    }
    input.store("list_of_event_ids.file", lis_file);

    REQUIRE_NOTHROW( EHC.initialize(input, dummySM, dummyCH) );
    REQUIRE( EHC.is_mode_list_of_event_ids() );

    auto eidl_output = get_eid_list(EHC);
    REQUIRE (eidl_output.size() == eid_expected_output.size());
    REQUIRE (eidl_output == eid_expected_output);
  }
}

