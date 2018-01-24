// Ourselves
#include "FLTags.h"

// Standard Library:
#include <fstream>
#include <sstream>
#include <memory>

// Third Party:
// - Bayeux:
#include "bayeux/datatools/kernel.h"
#include "bayeux/datatools/utils.h"
#include "bayeux/datatools/urn_query_service.h"
#include "bayeux/datatools/dependency_graph.h"

// This project:
#include <falaise/version.h>

// This App:
#include "FLTagsArgs.h"
#include "FLTagsUtils.h"

namespace FLTags {

  // static
  const std::string & fltags::default_action()
  {
    static const std::string _a("list");
    return _a;
  }

  fltags::fltags(const FLTags::FLTagsArgs & args_)
    : _args_(args_)
  {
    return;
  }

  void fltags::run()
  {
    if (datatools::logger::is_debug(_args_.logLevel)) {
      DT_LOG_DEBUG(_args_.logLevel, "FLArgs parameters:");
      _args_.print(std::cerr);
    }
    if (_args_.action == "list") {
      _run_print_list_();
    }
    if (_args_.action == "graph") {
      _run_generate_graph_();
    }
    return;
  }

  void fltags::_run_print_list_()
  {
    DT_LOG_DEBUG(_args_.logLevel,"Building the list of registered tags...");
    datatools::kernel & dtk = datatools::kernel::instance();
    datatools::urn_query_service & dtkUrnQuery = dtk.grab_urn_query();

    std::string output_file = _args_.outputFile;
    std::ostream * out = nullptr;
    std::unique_ptr<std::ofstream> hfout;
    if (output_file == "-") {
      out = &std::cout;
    } else {
      if (output_file.empty()) {
        output_file = FLTags::default_list_filename();
      }
      datatools::fetch_path_with_env(output_file);
      hfout.reset(new std::ofstream(output_file.c_str()));
      out = hfout.get();
    }

    std::vector<std::string> all_urns;
    std::string urn_db_regex = "(.*)";
    std::string urn_regex = "(.*)";
    std::string category_regex = "(.*)";
    if (dtkUrnQuery.find_urn_info(all_urns, urn_db_regex, urn_regex, category_regex, true)) {
      if (_args_.list_with_tree) {
        *out << "List of registered tags in Falaise " << falaise::version::get_version() << " : " << std::endl;
      }
      for (std::size_t i = 0; i < all_urns.size(); i++) {
        const std::string & urn = all_urns[i];
        datatools::urn_info urnInfo = dtkUrnQuery.get_urn_info(urn);
        if (_args_.list_with_tree) {
          std::ostringstream indentss;
          if ((i+1) == all_urns.size()) {
            *out << datatools::i_tree_dumpable::last_tag;
            indentss << datatools::i_tree_dumpable::last_skip_tag;
          } else {
            *out << datatools::i_tree_dumpable::tag;
            indentss << datatools::i_tree_dumpable::skip_tag;
          }
          *out << "Tag : " << std::endl;
          urnInfo.tree_dump(*out, "", indentss.str());
        } else {
          *out << urn
               << ";" << urnInfo.get_category()
               << ";" << urnInfo.get_description()
               << std::endl;
        }
      }
    }

    out = nullptr;
    hfout.reset();
    return;
  }

  void fltags::_run_generate_graph_()
  {
    DT_LOG_DEBUG(_args_.logLevel,"Building the dependency graph of registered tags...");
    datatools::kernel & dtk = datatools::kernel::instance();
    datatools::urn_query_service & dtkUrnQuery = dtk.grab_urn_query();
    const datatools::dependency_graph & dg = dtkUrnQuery.get_dependency_graph();
    std::string output_file = _args_.outputFile;
    std::ostream * out = nullptr;
    std::unique_ptr<std::ofstream> hfout;
    if (output_file == "-") {
      out = &std::cout;
    } else {
      if (output_file.empty()) {
        output_file = FLTags::default_dot_filename();
      }
      datatools::fetch_path_with_env(output_file);
      hfout.reset(new std::ofstream(output_file.c_str()));
      out = hfout.get();
    }
    uint32_t xgv_options = 0;
    if (_args_.dot_with_vertex_index) {
      xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_INDEX;
    }
    if (!_args_.dot_without_vertex_category) {
      xgv_options |= datatools::dependency_graph::XGV_WITH_VERTEX_CATEGORY;
    }
    if (!_args_.dot_without_edge_topic) {
      xgv_options |= datatools::dependency_graph::XGV_WITH_EDGE_TOPIC;
    }
    dg.export_graphviz(*out, xgv_options);

    out = nullptr;
    hfout.reset();

    if (output_file != "-") {
      std::clog << std::endl;
      std::clog << "***********************************************" << std::endl;
      std::clog << "The DOT file '" << output_file << "' has been generated. " << std::endl;
      std::clog << "Please use a graphviz-like tool to render the graph." << std::endl;
      std::clog << "Examples: " << std::endl;
      std::clog << std::endl;
      std::clog << " 1) Using xdot :" << std::endl;
      std::clog << std::endl;
      std::clog << "    $ xdot " << output_file << " &" << std::endl;
      std::clog << std::endl;
      std::clog << " 2) Using Python graphviz module :" << std::endl;
      std::clog << std::endl;
      std::clog << "    $ python " << std::endl;
      std::clog << "    >>> import pygraphviz as pgv " << std::endl;
      std::clog << "    >>> B=pgv.AGraph('" << output_file << "')" << std::endl;
      std::clog << "    >>> B.layout('dot')" << std::endl;
      std::clog << "    >>> B.draw('fltags.png')" << std::endl;
      std::clog << "    >>> ^D" << std::endl;
      std::clog << "    $ xdg-open fltags.png & " << std::endl;
      std::clog << std::endl;
      std::clog << "***********************************************" << std::endl;
      std::clog << std::endl;
    }
    return;
  }

}  // namespace FLTags
