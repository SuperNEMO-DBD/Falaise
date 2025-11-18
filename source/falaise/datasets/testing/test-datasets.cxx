// Standard library:
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>

// Third party:
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/datasets/model_id.hpp>
#include <falaise/datasets/dataset_model.hpp>
#include <falaise/datasets/dataset_producer_model.hpp>
#include <falaise/datasets/dataset_producer_config.hpp>
#include <falaise/datasets/dataset_producer.hpp>
#include <falaise/datasets/dataset.hpp>
#include <falaise/datasets/factory.hpp>
#include <falaise/datasets/manager.hpp>

void test_model_id();
void test_dataset();
void test_factory();
void test_manager();

int main(void)
{
  int code = EXIT_SUCCESS;

  try {
    test_model_id();
    test_dataset();
    test_factory();
    test_manager();
  } catch (std::exception & err) {
    std::cerr << "error: " << err.what() << std::endl;
    code = EXIT_FAILURE;
  }
  return code;
}

void test_model_id()
{
  std::clog << "====================== test_model_id" << std::endl;
  snemo::datasets::model_id cbdDatasetModelId("cbd");
  snemo::datasets::model_id redDatasetModelId("red", "1.0");
  std::clog << "CBD dataset model ID : " << std::quoted(cbdDatasetModelId.to_string()) << '\n';
  std::clog << "RED dataset model ID : " << std::quoted(redDatasetModelId.to_string()) << '\n';
}

void test_dataset()
{
  std::clog << "====================== test_dataset" << std::endl;
  using namespace snemo::datasets;
  dataset_model cbdDatasetModel({"cbd", "1.0"}, "Raw DAQ output data format");
  dataset_model rtdDatasetModel({"rtd", "1.0"}, "Raw trigger data format (RTD)");
  dataset_model redDatasetModel({"red", "1.0"}, "Raw event data format (RED)");
  dataset_model uddDatasetModel({"udd", "1.0"}, "UDD event data format (UDD)");
  std::clog << "CBD dataset model : " << cbdDatasetModel << '\n';
  std::clog << "RTD dataset model : " << rtdDatasetModel << '\n';
  std::clog << "RED dataset model : " << redDatasetModel << '\n';
  std::clog << "UDD dataset model : " << uddDatasetModel << '\n';

  std::clog << "cbdDatasetModel:\n";
  cbdDatasetModel.print_tree(std::clog);

  std::clog << "rtdDatasetModel:\n";
  rtdDatasetModel.print_tree(std::clog);

  std::clog << "redDatasetModel:\n";
  redDatasetModel.print_tree(std::clog);

  std::clog << "uddDatasetModel:\n";
  uddDatasetModel.print_tree(std::clog);

  dataset_producer_model daqProcessModel({"daq", "1.0"},
					 "DAQ process",
					 std::cref(cbdDatasetModel));
  std::clog << "daqProcessModel:\n";
  daqProcessModel.print_tree(std::clog);
 
  dataset_producer_config daqConfigRun42({"daqcfg", 42}, daqProcessModel);
  daqConfigRun42.parameters().put("category", "betabeta_v2");
  daqConfigRun42.parameters().put("daq_config_path", "@sndaq:config-42");
  daqConfigRun42.parameters().put("run_start_time", "2025-06-23 12:34:23.121031");
  daqConfigRun42.parameters().put("run_stop_time", "2025-06-23 15:14:45.082353");
  std::clog << "daqConfigRun42:\n";
  daqConfigRun42.print_tree(std::clog);

  dataset_producer_config daqConfigRun43({"daqcfg", 43}, daqProcessModel);
  daqConfigRun43.parameters().put("category", "betabeta_v2");
  daqConfigRun43.parameters().put("daq_config_path", "@sndaq:config-43");
  daqConfigRun43.parameters().put("run_start_time", "2025-06-23 15:14:45.082353");
  daqConfigRun43.parameters().put("run_stop_time", "2025-06-23 21:07:08.452134");
  std::clog << "daqConfigRun43:\n";
  daqConfigRun43.print_tree(std::clog);

  dataset_producer daqRun42({"daq", 42}, daqProcessModel, daqConfigRun42);
  std::clog << "daqRun42:\n";
  daqRun42.print_tree(std::clog);

  dataset_producer daqRun43({"daq", 43}, daqProcessModel, daqConfigRun43);
  std::clog << "daqRun43:\n";
  daqRun43.print_tree(std::clog);

  dataset cbdRun42({"cbd", 42}, cbdDatasetModel, daqRun42);
  cbdRun42.add_chunk({instance_id("crate", 0), "snemo_cbd-0.brio", 1234u});
  cbdRun42.add_chunk({instance_id("crate", 1), "snemo_cbd-1.brio", 1324u});
  cbdRun42.add_chunk({instance_id("crate", 2), "snemo_cbd-2.brio", 2142u});
  std::clog << "cbdRun42:\n";
  cbdRun42.print_tree(std::clog);

  dataset cbdRun43({"cbd", 43}, cbdDatasetModel, daqRun43);
  cbdRun43.add_chunk({instance_id("crate", 0), "snemo_cbd-0.brio", 3211u});
  cbdRun43.add_chunk({instance_id("crate", 1), "snemo_cbd-1.brio", 2432u});
  cbdRun43.add_chunk({instance_id("crate", 2), "snemo_cbd-2.brio", 2654u});
  std::clog << "cbdRun43:\n";
  cbdRun43.print_tree(std::clog);

  std::clog << "Run 42: # record entries = " << cbdRun42.number_of_entries() << '\n';
  std::clog << "Run 43: # record entries = " << cbdRun43.number_of_entries() << '\n';

  dataset_producer_model cbdToRtdProcessModel({"cbd2rtd", "1.0"},
					      "Conversion of CBD data to RTD data",
					      rtdDatasetModel,
					      cbdDatasetModel);
  std::clog << "cbdToRtdProcessModel:\n";
  cbdToRtdProcessModel.print_tree(std::clog);

  dataset_producer cbdToRtdProcess42({"cbd2rtd", 42}, cbdToRtdProcessModel);
  std::clog << "cbdToRtdProcess42:\n";
  cbdToRtdProcess42.print_tree(std::clog);

  dataset rtdRun42({"rtd", 42}, rtdDatasetModel, cbdToRtdProcess42, cbdRun42);
  rtdRun42.add_chunk({instance_id("rtd"), "snemo_rtd-42-0.brio", 345u});

  dataset_producer cbdToRtdProcess43({"cbd2rtd", 43}, cbdToRtdProcessModel);
  dataset rtdRun43({"rtd", 43}, rtdDatasetModel, cbdToRtdProcess43, cbdRun43);
  rtdRun43.add_chunk({instance_id("rtd"), "snemo_rtd-43-0.brio", 376u});

  std::clog << "Run 42: # trigger entries = " << rtdRun42.number_of_entries() << '\n';
  std::clog << "Run 43: # trigger entries = " << rtdRun43.number_of_entries() << '\n';

  dataset_producer_model rtdToRedProcessModel({"rtd2red", "1.0"},
					      "Conversion of RTD data to RED data",
					      redDatasetModel,
					      rtdDatasetModel);

  dataset_producer_config eventBuilderConfig1({"ebcfg", 1}, rtdToRedProcessModel);
  eventBuilderConfig1.parameters().put("build_algo", "delta-tdc");

  dataset_producer eventBuilder42({"eb", 42},
				  rtdToRedProcessModel,
				  eventBuilderConfig1);
 
  dataset_producer eventBuilder43({"eb", 43},
				  rtdToRedProcessModel,
				  eventBuilderConfig1);
  
  dataset red42({"red", 42}, redDatasetModel, eventBuilder42, rtdRun42);
  red42.add_chunk({instance_id("red"), "snemo_red-42.brio", 274u});
  std::clog << "red42:\n";
  red42.print_tree(std::clog);

  dataset red43({"red", 43}, redDatasetModel, eventBuilder43, rtdRun43);
  red43.add_chunk({instance_id("red"), "snemo_red-43.brio", 312u});
  std::clog << "red43:\n";
  red43.print_tree(std::clog);
  
  std::clog << "Run 42: # event entries = " << red42.number_of_entries() << '\n';
  std::clog << "Run 43: # event entries = " << red43.number_of_entries() << '\n';
 
  dataset_producer_model redBridgeProcessModel({"redbridge", "1.0"},
					       "Conversion of RED data to UDD data",
					       uddDatasetModel,
					       redDatasetModel);
  std::clog << "redBridgeProcessModel:\n";
  redBridgeProcessModel.print_tree(std::clog);

  dataset_producer redBridgeProcess42({"redBridge", 42}, redBridgeProcessModel);
  std::clog << "redBridgeProcess42:\n";
  redBridgeProcess42.print_tree(std::clog);

  dataset_producer redBridgeProcess43({"redBridge", 43}, redBridgeProcessModel);
  std::clog << "redBridgeProcess43:\n";
  redBridgeProcess43.print_tree(std::clog);
  
  dataset udd42({"udd", 42}, uddDatasetModel, redBridgeProcess42, red42);
  udd42.add_chunk({instance_id("udd"), "snemo_udd-42.brio", red42.number_of_entries()});
  
  dataset udd43({"udd", 43}, uddDatasetModel, redBridgeProcess43, red43);
  udd43.add_chunk({instance_id("udd"), "snemo_udd-43.brio", red43.number_of_entries()});

  std::clog << "udd42:\n";
  udd42.print_tree(std::clog);

  std::clog << "udd43:\n";
  udd43.print_tree(std::clog);
 
}

void test_factory()
{
  std::clog << "====================== test_factory" << std::endl;
  using namespace snemo::datasets;
  factory fact;
  fact.verbosity = datatools::logger::PRIO_DEBUG; 

  // Dataset models:
  boost::property_tree::ptree cbdParams;
  cbdParams.put("id", "cbd-1.0");
  cbdParams.put("description", "Raw DAQ output data model (CBD), version 1.0");
  cbdParams.put("details.sncomdaq_version", "0.1.5");
  fact.create("dataset_model", cbdParams);
   
  boost::property_tree::ptree rtdParams;
  rtdParams.put("id", "rtd-1.0");
  rtdParams.put("description", "Raw Trigger data model (RTD), version 1.0");
  rtdParams.put("details.snfee_version", "1.1.1");
  fact.create("dataset_model", rtdParams);
   
  boost::property_tree::ptree redParams;
  redParams.put("id", "red-1.0");
  redParams.put("description", "Raw Event builder data model (RED), version 1.0");
  redParams.put("details.snfee_version", "1.1.1");
  fact.create("dataset_model", redParams);
   
  boost::property_tree::ptree uddParams;
  uddParams.put("id", "udd-1.0");
  uddParams.put("description", "Unified Digitized Data Falaise's model (UDD), version 1.0");
  uddParams.put("details.falaise_version", "5.1.5");
  uddParams.put("details.boost_serialization_version", "18");
  fact.create("dataset_model", uddParams);
 
  // Dataset producer models:
  boost::property_tree::ptree daqProcessModelParams;
  daqProcessModelParams.put("id", "daq-1.0");
  daqProcessModelParams.put("description", "DAQ process, version 1.0");
  daqProcessModelParams.put("output_dataset_model_id", "cbd-1.0");
  fact.create("dataset_producer_model", daqProcessModelParams);
     
  boost::property_tree::ptree ebProcessModelParams;
  ebProcessModelParams.put("id", "eb-1.0");
  ebProcessModelParams.put("description", "Event builder process, version 1.0");
  ebProcessModelParams.put("output_dataset_model_id", "red-1.0");
  ebProcessModelParams.put("input_dataset_model_id", "cbd-1.0");
  fact.create("dataset_producer_model", ebProcessModelParams);
     
  boost::property_tree::ptree red2uddProcessModelParams;
  red2uddProcessModelParams.put("id", "red2udd-1.0");
  red2uddProcessModelParams.put("description", "RED to UDD convertor, version 1.0");
  red2uddProcessModelParams.put("output_dataset_model_id", "udd-1.0");
  red2uddProcessModelParams.put("input_dataset_model_id", "red-1.0");
  fact.create("dataset_producer_model", red2uddProcessModelParams);
 
  // DAQ processes:
  // - configurations
  boost::property_tree::ptree daqConfigRun42Params;
  daqConfigRun42Params.put("id", "daqcfg:42");
  daqConfigRun42Params.put("producer_model_id", "daq-1.0");
  daqConfigRun42Params.put("parameters.category", "betabeta_v2");
  daqConfigRun42Params.put("parameters.daq_config_path", "@sndaq:config-42");
  daqConfigRun42Params.put("parameters.run_start_time", "2025-06-23 12:34:23.121031");
  daqConfigRun42Params.put("parameters.run_stop_time",  "2025-06-23 15:14:45.082353");
  fact.create("dataset_producer_config", daqConfigRun42Params);
   
  boost::property_tree::ptree daqConfigRun43Params;
  daqConfigRun43Params.put("id", "daqcfg:43");
  daqConfigRun43Params.put("producer_model_id", "daq-1.0");
  daqConfigRun43Params.put("parameters.category", "betabeta_v2");
  daqConfigRun43Params.put("parameters.daq_config_path", "@sndaq:config-43");
  daqConfigRun43Params.put("parameters.run_start_time", "2025-06-23 15:15:23.326351");
  daqConfigRun43Params.put("parameters.run_stop_time",  "2025-06-23 21:07:08.452134");
  fact.create("dataset_producer_config", daqConfigRun43Params);

  // - producers:
  boost::property_tree::ptree daqRun42Params;
  daqRun42Params.put("id", "daq:42");
  daqRun42Params.put("producer_model_id", "daq-1.0");
  daqRun42Params.put("producer_configuration_id", "daqcfg:42");
  fact.create("dataset_producer", daqRun42Params);

  boost::property_tree::ptree daqRun43Params;
  daqRun43Params.put("id", "daq:43");
  daqRun43Params.put("producer_model_id", "daq-1.0");
  daqRun43Params.put("producer_configuration_id", "daqcfg:43");
  fact.create("dataset_producer", daqRun43Params);

  // - datasets:
  boost::property_tree::ptree cbdRun42Params;
  cbdRun42Params.put("id", "cbd:42");
  cbdRun42Params.put("dataset_model_id", "cbd-1.0");
  cbdRun42Params.put("dataset_producer_id", "daq:42");
  cbdRun42Params.put("nchunks", 3);
  cbdRun42Params.put("chunks[0].id", "crate-0");
  cbdRun42Params.put("chunks[0].path", "run-42_cbd_crate0.data");
  cbdRun42Params.put("chunks[0].nentries", 2324u);
  cbdRun42Params.put("chunks[1].id", "crate-1");
  cbdRun42Params.put("chunks[1].path", "run-42_cbd_crate1.data");
  cbdRun42Params.put("chunks[1].nentries", 2789u);
  cbdRun42Params.put("chunks[2].id", "crate-2");
  cbdRun42Params.put("chunks[2].path", "run-42_cbd_crate2.data");
  cbdRun42Params.put("chunks[2].nentries", 3422u);
  fact.create("dataset", cbdRun42Params);

  boost::property_tree::ptree cbdRun43Params;
  cbdRun43Params.put("id", "cbd:43");
  cbdRun43Params.put("dataset_model_id", "cbd-1.0");
  cbdRun43Params.put("dataset_producer_id", "daq:43");
  cbdRun43Params.put("nchunks", 2);
  cbdRun43Params.put("chunks[0].id", "crate-0");
  cbdRun43Params.put("chunks[0].path", "run-43_cbd_crate0.data");
  cbdRun43Params.put("chunks[1].id", "crate-1");
  cbdRun43Params.put("chunks[1].path", "run-43_cbd_crate1.data");
  fact.create("dataset", cbdRun43Params);

  auto daqConfigRun42Cref = fact.get_dataset_producer_config_ref("daqcfg:42");
  std::clog << "daqConfigRun42Cref:\n";
  daqConfigRun42Cref.get().print_tree(std::clog);

  auto daqConfigRun43Cref = fact.get_dataset_producer_config_ref("daqcfg:43");
  std::clog << "daqConfigRun43Cref:\n";
  daqConfigRun43Cref.get().print_tree(std::clog);

  auto cbdRun42Cref = fact.get_dataset_ref("cbd:42");
  std::clog << "cbdRun42Cref:\n";
  cbdRun42Cref.get().print_tree(std::clog);
   
  auto cbdRun43Cref= fact.get_dataset_ref("cbd:43");
  std::clog << "cbdRun43Cref:\n";
  cbdRun43Cref.get().print_tree(std::clog);

  // Event builder:

  // - configurations:
  boost::property_tree::ptree ebConfig1Params;
  ebConfig1Params.put("id", "ebcfg:1");
  ebConfig1Params.put("producer_model_id", "eb-1.0");
  ebConfig1Params.put("parameters.mode", "standard");
  fact.create("dataset_producer_config", ebConfig1Params);
     
  boost::property_tree::ptree ebConfig2Params;
  ebConfig2Params.put("id", "ebcfg:2");
  ebConfig2Params.put("producer_model_id", "eb-1.0");
  ebConfig2Params.put("parameters.mode", "time_window");
  ebConfig2Params.put("parameters.time_window.width", "100");
  fact.create("dataset_producer_config", ebConfig2Params);

  // - producers:
  boost::property_tree::ptree eb1Params;
  eb1Params.put("id", "eb:1");
  eb1Params.put("producer_model_id", "eb-1.0");
  eb1Params.put("producer_configuration_id", "ebcfg:1");
  fact.create("dataset_producer", eb1Params);
   
  boost::property_tree::ptree eb2Params;
  eb2Params.put("id", "eb:2");
  eb2Params.put("producer_model_id", "eb-1.0");
  eb2Params.put("producer_configuration_id", "ebcfg:2");
  fact.create("dataset_producer", eb2Params);

  // - datasets:
  boost::property_tree::ptree redRun42Params;
  redRun42Params.put("id", "red:42");
  redRun42Params.put("dataset_model_id", "red-1.0");
  redRun42Params.put("dataset_producer_id", "eb:1");
  redRun42Params.put("input_dataset_id", "cbd:42");
  redRun42Params.put("nchunks", 1);
  redRun42Params.put("chunks[0].id", "events");
  redRun42Params.put("chunks[0].path", "run-42_red.data");
  fact.create("dataset", redRun42Params);

  boost::property_tree::ptree redRun43Params;
  redRun43Params.put("id", "red:43");
  redRun43Params.put("dataset_model_id", "red-1.0");
  redRun43Params.put("dataset_producer_id", "eb:1");
  redRun43Params.put("input_dataset_id", "cbd:43");
  redRun43Params.put("nchunks", 1);
  redRun43Params.put("chunks[0].id", "events");
  redRun43Params.put("chunks[0].path", "run-43_red.data");
  fact.create("dataset", redRun43Params);

  // Conversion RED-UDD:

  // - configuration:
  boost::property_tree::ptree red2uddConfigParams;
  red2uddConfigParams.put("id", "red2uddcfg:1");
  red2uddConfigParams.put("producer_model_id", "red2udd-1.0");
  fact.create("dataset_producer_config", red2uddConfigParams);
   
  // - producer:
  boost::property_tree::ptree red2uddParams;
  red2uddParams.put("id", "red2udd:1");
  red2uddParams.put("producer_model_id", "red2udd-1.0");
  red2uddParams.put("producer_configuration_id", "red2uddcfg:1");
  fact.create("dataset_producer", red2uddParams);

  // - datasets:
  boost::property_tree::ptree uddRun42Params;
  uddRun42Params.put("id", "udd:42");
  uddRun42Params.put("dataset_model_id", "udd-1.0");
  uddRun42Params.put("dataset_producer_id", "red2udd:1");
  uddRun42Params.put("input_dataset_id", "red:42");
  uddRun42Params.put("nchunks", 1);
  uddRun42Params.put("chunks[0].id", "events");
  uddRun42Params.put("chunks[0].path", "run-42_udd.data");
  fact.create("dataset", uddRun42Params);

  boost::property_tree::ptree uddRun43Params;
  uddRun43Params.put("id", "udd:43");
  uddRun43Params.put("dataset_model_id", "udd-1.0");
  uddRun43Params.put("dataset_producer_id", "red2udd:1");
  uddRun43Params.put("input_dataset_id", "red:43");
  uddRun43Params.put("nchunks", 1);
  uddRun43Params.put("chunks[0].id", "events");
  uddRun43Params.put("chunks[0].path", "run-43_udd.data");
  fact.create("dataset", uddRun43Params);

  std::clog << "Factory:\n";
  fact.print_tree(std::clog);
  
  auto cbdModelCref= fact.get_dataset_model_ref("cbd-1.0");
  std::clog << "cbdModelCref:\n";
  cbdModelCref.get().print_tree(std::clog);
 
  auto uddModelCref= fact.get_dataset_model_ref("udd-1.0");
  std::clog << "uddModelCref:\n";
  uddModelCref.get().print_tree(std::clog);



}

void test_manager()
{
  std::clog << "====================== test_manager" << std::endl;
  using namespace snemo::datasets;

  datatools::properties dataMgrConfig;
  manager datamgr;
  datamgr.verbosity = datatools::logger::PRIO_DEBUG; 
  datamgr.initialize(dataMgrConfig);
  datamgr.reset();
}
