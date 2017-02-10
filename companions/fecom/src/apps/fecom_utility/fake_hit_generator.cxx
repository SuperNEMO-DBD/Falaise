// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <random>

// - Bayeux:
// - Bayeux/datatools:
#include <datatools/io_factory.h>

void generate_fake_calo_hit(std::mt19937_64 & rng_,
			    std::ofstream & out_,
			    uint64_t hit_id_);

void generate_fake_tracker_hit(std::mt19937_64 & rng_,
			       std::ofstream & out_,
			       uint64_t hit_id_);

int main(int /*argc_*/, char ** /*argv_*/)
{
  try {

    std::string output_file = "${FECOM_RESOURCES_DIR}/output_test/test_generate_fake_hit_large.dat";
    datatools::fetch_path_with_env(output_file);

    std::ofstream file(output_file,
		       std::ios::out | std::ios::trunc); // Open in write access with file delete
    if(file)
      {
	// Run header :
	file << "=== DATA FILE SAVED WITH SN CRATE SOFTWARE VERSION: V1.2  == DATE OF RUN: UnixTime = 1481892599.431 date = 2016.12.16 time = 13h.49m.59s.431ms  ===" << std::endl;
	file << "=== OTHER INFORMATIONS ===" << std::endl;
	file << "=== DATA TYPE : RAW ===" << std::endl;
	file << "=== DATA STRUCTURE INFO ===" << std::endl;
	file << "===HIT number(int)  === (hit_type : 'CALO_HIT' or 'TRACKER_HIT' ===" << std::endl;
	file << "=== SlotIndex (int) Ch (int) EvtID (int) RawTDC (unsigned long long) LT_TrigCount(int) LT_TimeCount(int) RawBaseline (signed short *16) RawPeak (signed shor *8) PeakCell (unsigned short) RawCharge (int ) ChargeOverflow(int) RisingEdgeCell (int) RisingTimeOffset(int * 256) FallingEdgeCell(int) FallingEdgeOffset (int *256) FCR (uint) ===" << std::endl;
	file << "=== DataSamples[array of int16] ===" << std::endl;

	unsigned int seed = 314157;
	std::mt19937_64 rng;
	rng.seed(seed);
	uint64_t hit_id = 0;

	const std::size_t NUMBER_OF_CALO_HITS = 150000;
	const std::size_t NUMBER_OF_TRACKER_HITS = 1500000;


	for (std::size_t icalo = 0; icalo < NUMBER_OF_CALO_HITS; icalo++)
	  {
	    generate_fake_calo_hit(rng,
				   file,
				   hit_id);
	    hit_id++;
	  }

	for (std::size_t itrack = 0; itrack < NUMBER_OF_TRACKER_HITS; itrack++)
	  {
	    generate_fake_tracker_hit(rng,
				      file,
				      hit_id);
	    hit_id++;
	  }

	file.close();
      }
    else std::cerr << "Open file failed !" << std::endl;


  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


void generate_fake_calo_hit(std::mt19937_64 & rng_,
			    std::ofstream & out_,
			    uint64_t hit_id_)
{
  const uint32_t NUMBER_OF_TRIGGER_ID = 100000;
  std::uniform_int_distribution<int> unif_int(0, 1000000);
  // Hit id : external counter
  uint64_t hit_id = hit_id_;
  // Trigger id : [0:99]
  uint32_t trigger_id = unif_int(rng_) % NUMBER_OF_TRIGGER_ID;
  // Slot id : [0:19]
  uint16_t slot_id = unif_int(rng_) % 20;
  // Channel id : [0:15]
  uint16_t channel_id = unif_int(rng_) % 16;
  // Event id : [0:255]
  uint16_t event_id = unif_int(rng_) % 256;
  // Raw_tdc : [0:1000000]
  uint32_t raw_tdc = unif_int(rng_);

  out_ << "===HIT " << hit_id  << "=== CALO_HIT === TRIGGER_ID " << trigger_id << "===" << std::endl;
  out_ << "Slot" << slot_id << " Ch" << channel_id << " EvtID " << event_id << " RawTDC " << raw_tdc << " TrigCount 0 Timecount 153 RawBaseline -3 RawPeak -38 RawCharge -179 Overflow 0 RisingCell 41 RisingOffset 152 FallingCell 39 FallingOffset 168 FCR 224" << std::endl;
  out_ << "2049 2048 2045 2049 2050 2049 2048 2050 2047 2049 2048 2047 2045 2049 2048 2049 2049 2048 2046 2049 2051 2046 2047 2052 2050 2048 2045 2047 2046 2049 2046 2050 2046 2048 2046 2049 2047 2046 2048 2047 2049 2048 2047 2048 2047 2050 2049 2052 2047 2048 2047 2048 2050 2048 2050 2048 2050 2050 2046 2045 2047 2049 2049 2049 2049 2048 2047 2048 2048 2047 2047 2049 2049 2052 2048 2045 2045 2048 2049 2049 2046 2048 2046 2047 2048 2046 2046 2050 2050 2049 2047 2047 2046 2047 2046 2051 2046 2047 2047 2048 2049 2047 2045 2048 2050 2050 2046 2045 2046 2050 2047 2049 2045 2046 2047 2048 2049 2048 2045 2048 2050 2047 2045 2048 2046 2050 2047 2054 2049 2049 2048 2046 2049 2047 2046 2049 2048 2048 2050 2049 2045 2046 2047 2051 2048 2047 2045 2049 2047 2046 2047 2048 2047 2048 2046 2047 2046 2051 2047 2049 2047 2050 2048 2047 2047 2046 2051 2048 2049 2048 2047 2049 2047 2050 2047 2049 2048 2048 2050 2049 2049 2048 2048 2049 2049 2049 2048 2048 2045 2049 2049 2048 2048 2048 2046 2048 2050 2049 2047 2047 2049 2049 2047 2047 2047 2050 2047 2049 2049 2048 2046 2047 2050 2049 2049 2049 2049 2049 2047 2044 2047 2049 2049 2048 2048 2048 2047 2048 2049 2049 2048 2052 2050 2050 2046 2050 2047 2050 2047 2051 2048 2049 2046 2048 2048 2048 2049 2050 2049 2048 2045 2046 2046 2048 2047 2050 2047 2048 2049 2048 2050 2047 2048 2048 2049 2048 2046 2047 2045 2049 2048 2049 2048 2050 2046 2048 2048 2046 2048 2049 2049 2047 2046 2048 2050 2049 2046 2048 2047 2048 2046 2048 2051 2047 2048 2048 2049 2048 2046 2047 2047 2051 2047 2050 2047 2048 2048 2049 2049 2048 2046 2049 2049 2048 2046 2046 2045 2049 2046 2050 2049 2049 2048 2047 2050 2048 2048 2048 2050 2050 2048 2047 2046 2049 2048 2050 2046 2050 2047 2048 2048 2047 2048 2046 2049 2050 2046 2047 2045 2048 2049 2049 2050 2049 2048 2047 2048 2048 2049 2049 2049 2049 2045 2049 2049 2050 2049 2051 2048 2047 2046 2046 2049 2046 2047 2049 2050 2050 2047 2049 2046 2049 2048 2047 2049 2049 2047 2048 2049 2048 2047 2050 2048 2048 2047 2048 2047 2049 2047 2050 2050 2048 2045 2048 2048 2049 2046 2047 2049 2048 2046 2048 2046 2050 2047 2047 2048 2049 2049 2048 2048 2046 2047 2049 2048 2049 2047 2049 2048 2049 2046 2050 2048 2047 2045 2048 2049 2050 2050 2049 2047 2047 2047 2048 2046 2050 2047 2047 2050 2047 2045 2051 2049 2046 2048 2048 2049 2047 2049 2047 2045 2049 2048 2048 2046 2048 2047 2048 2050 2047 2049 2050 2049 2049 2050 2048 2045 2048 2048 2047 2048 2049 2048 2048 2049 2047 2051 2048 2048 2050 2048 2048 2047 2048 2048 2048 2050 2049 2046 2049 2048 2046 2048 2047 2049 2048 2049 2047 2048 2049 2048 2049 2048 2047 2047 2049 2050 2048 2048 2049 2048 2048 2047 2046 2046 2046 2047 2048 2048 2049 2046 2048 2047 2048 2050 2049 2053 2050 2046 2044 2047 2050 2047 2049 2050 2048 2046 2047 2049 2048 2050 2049 2046 2047 2048 2048 2049 2048 2048 2049 2046 2046 2045 2049 2048 2048 2048 2047 2049 2049 2048 2049 2047 2050 2048 2048 2046 2047 2049 2050 2049 2047 2049 2049 2049 2049 2045 2047 2047 2047 2049 2050 2049 2049 2045 2048 2048 2047 2047 2048 2048 2048 2047 2049 2046 2049 2048 2050 2047 2049 2048 2048 2048 2046 2047 2046 2049 2049 2045 2045 2045 2048 2048 2052 2047 2045 2047 2048 2048 2048 2049 2049 2048 2047 2048 2048 2044 2048 2048 2049 2048 2048 2045 2048 2048 2048 2048 2049 2050 2047 2048 2047 2047 2049 2048 2049 2049 2049 2046 2049 2049 2048 2046 2047 2048 2051 2048 2047 2045 2050 2047 2049 2047 2047 2045 2046 2051 2046 2048 2047 2050 2048 2047 2046 2045 2049 2047 2048 2048 2048 2048 2047 2049 2048 2047 2049 2050 2049 2048 2045 2044 2048 2046 2050 2049 2049 2047 2047 2049 2047 2048 2050 2050 2050 2048 2046 2045 2049 2046 2050 2049 2048 2046 2048 2051 2049 2049 2050 2049 2049 2048 2046 2046 2048 2050 2049 2049 2048 2044 2048 2049 2046 2048 2050 2048 2049 2048 2046 2045 2048 2049 2047 2048 2049 2049 2050 2049 2049 2047 2048 2048 2047 2047 2046 2045 2049 2048 2048 2047 2047 2047 2048 2049 2046 2048 2050 2050 2052 2048 2047 2044 2048 2048 2048 2048 2049 2045 2048 2048 2048 2048 2050 2049 2048 2047 2047 2046 2049 2046 2048 2050 2045 2046 2047 2047 2048 2047 2048 2048 2051 2047 2049 2047 2048 2047 2048 2047 2047 2044 2047 2049 2047 2046 2047 2049 2048 2046 2047 2048 2050 2048 2049 2047 2047 2048 2047 2049 2046 2049 2049 2051 2047 2048 2048 2046 2049 2046 2049 2049 2049 2047 2048 2047 2047 2049 2050 2050 2049 2050 2047 2046 2049 2047 2049 2049 2051 2047 2048 2047 2047 2048 2049 2048 2047 2047 2047 2047 2049 2047 2050 2046 2048 2047 2050 2049 2048 2050 2047 2052 2048 2048 2049 2047 2049 2048 2051 2048 2048 2047 2049 2049 2047 2047 2048 2045 2041 2029 2006 1979 1952 1924 1901 1881 1866 1857 1859 1867 1875 1883 1893 1906 1920 1932 1944 1953 1961 1963 1966 1970 1976 1980 1989 1994 2000 2006 2013 2016 2017 2020 2021 2022 2028 2029 2033 2031 2032 2033 2032 2035 2036 2037 2039 2039 2036 2037 2035 2036 2037 2036 2033 2035 2033 2036 2037 2041 2042 2040 2044 2045 2042 2039 2039 2040 2045 2044 2044 2043 2040 2039 2041 2039 2039 2039 2040 2043 2043 2042 2042 2043 2046 2043 2047 2044 2043 2044 2046 2045 2043 2044 2046 2044 2046 2043 2045 2045 2047 2047 2049 2048 2046 2043 2045 2045 2040 2043 2043 2042 2043 2044 2043 2044 2045 2045 2048" << std::endl;

  return;
}

void generate_fake_tracker_hit(std::mt19937_64 & rng_,
			       std::ofstream & out_,
			       uint64_t hit_id_)
{
  const uint32_t NUMBER_OF_TRIGGER_ID = 100000;
  std::uniform_int_distribution<int> unif_int(0, 1000000);
  // Hit id : external counter
  uint64_t hit_id = hit_id_;
  // Trigger id : [0:99]
  uint32_t trigger_id = unif_int(rng_) % NUMBER_OF_TRIGGER_ID;
  // Slot id : [0:19]
  uint16_t slot_id = unif_int(rng_) % 20;
  // Feast id : [0:1]
  uint16_t feast_id = unif_int(rng_) % 2;
  // Channel id : [0:53]
  uint16_t channel_id = unif_int(rng_) % 54;
  // Channel type : Anodic | Cathodic
  std::string channel_type = "INVALID";
  // Timestamp type : t0-t6
  std::string timestamp_type = "INVALID";
  // Timestamp value : [0:1000000]
  uint64_t timestamp_value = unif_int(rng_);

  uint16_t anodic_or_cathodic_choose = unif_int(rng_) % 7;

  if (anodic_or_cathodic_choose < 5) {
    channel_type = "Anodic";
    uint16_t timestamp_value_number = anodic_or_cathodic_choose; //unif_int(rng_) % 5;
    timestamp_type = "t" + std::to_string(timestamp_value_number);
  }
  else {
    channel_type = "Cathodic";
    uint16_t timestamp_value_number = anodic_or_cathodic_choose; // unif_int(rng_) % 2 + 6;
    timestamp_type = "t" + std::to_string(timestamp_value_number);
  }

  DT_THROW_IF(channel_type == "INVALID" || timestamp_type == "INVALID", std::logic_error, "Channel type '" + channel_type + "' or timestamp type '" + timestamp_type + "' is invalid !");

  out_ << "===HIT " << hit_id  << "=== TRACKER_HIT === TRIGGER_ID " << trigger_id << "===" << std::endl;
  out_ << "Slot" << slot_id << " Feast" << feast_id << " Ch" << channel_id << ' ' << channel_type << ' ' << timestamp_type << ' ' << timestamp_value << std::endl;

  return;
}
