// -*- mode: c++ ; -*-
// test_analysis_macro.cxx

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <snvisualization/io/event_server.h>

#include <datatools/ioutils.h>

// For Boost I/O :
// Some pre-processor guard about Boost I/O usage and linkage :
// #include <datatools/serialization/bio_guard.h>
// #include <geomtools/serialization/bio_guard.h>
// #include <genbb_help/serialization/bio_guard.h>
// #include <sncore/serialization/bio_guard.h>

#include <TCanvas.h>
#include <TGraph.h>
#include <TH1I.h>
#include <TRint.h>

using namespace std;

/*****************************************************************************/
int main(int argc_, char** argv_) {
  const string appname_prefix = "test_analysis_macro: ";

  int error_code = EXIT_SUCCESS;
  try {
    clog << datatools::io::notice << appname_prefix << "Test program for analysis stuff!" << endl;

    bool debug = false;
    bool verbose = false;

    bool visu_mode = false;

    size_t noevents = 0;
    size_t noevents_modulo = 100;

    vector<string> input_files;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose")) {
          verbose = true;
        } else if (option == "--visu-mode") {
          visu_mode = true;
        } else if ((option == "-n") || (option == "--number-of-events")) {
          istringstream iss(argv_[++iarg]);
          iss >> noevents;
          if (!iss) {
            throw runtime_error("Invalid format for option '-n|--number-of-events' !");
          }
          if (noevents == 0) {
            throw runtime_error("Invalid value for option '-n|--number-of-events' !");
          }
        } else if ((option == "-m") || (option == "--number-of-events-modulo")) {
          istringstream iss(argv_[++iarg]);
          iss >> noevents_modulo;
          if (!iss) {
            throw runtime_error("Invalid format for option '-m|--number-of-events-modulo' !");
          }
          if (noevents_modulo == 0) {
            throw runtime_error("Invalid value for option '-m|--number-of-events-modulo' !");
          }
        }
      } else {
        string argument = token;
        input_files.push_back(argument);
      }
      iarg++;
    }

    DT_THROW_IF(input_files.empty(), std::logic_error, "Missing input files !");

    namespace svi = snemo::visualization::io;
    svi::event_server my_event_server;

    DT_THROW_IF(!my_event_server.open(input_files), std::logic_error, "Cannot open data source!");
    my_event_server.tree_dump(clog, "Event server:", "NOTICE: ");

    DT_THROW_IF(
        !my_event_server.has_sequential_data() && my_event_server.get_number_of_events() == 0,
        std::logic_error, "No events found in the given file!");

    DT_THROW_IF(!my_event_server.is_mc(), std::logic_error,
                "Files must come from MC simulation for this macro!");

    vector<int> primary_type;
    vector<double> primary_energy;
    map<string, vector<double> > primary_momentum;
    map<string, vector<double> > vertex_position;

    size_t i_event = 0;
    while (my_event_server.next_event()) {
      // typedef from 'snvisualization/io/data_models.h':
      const svi::event_record& my_event = my_event_server.get_event();

      if (++i_event % noevents_modulo == 0) {
        cout << datatools::io::notice << appname_prefix << "Event #" << i_event << endl;
      }

      if (noevents > 0 && i_event > noevents) break;

      // not working yet !
      // if (!my_event.is_simulated ())
      //   continue;

      // typedef from 'snvisualization/io/data_models.h':
      const svi::sim_data_bank& my_sim_data = my_event.get<svi::sim_data_bank>(svi::SD_LABEL);
      if (verbose) my_sim_data.tree_dump();

      const genbb::primary_event& my_primary_event = my_sim_data.get_primary_event();
      const genbb::primary_event::particles_col_t& my_primary_particles =
          my_primary_event.get_particles();

      for (genbb::primary_event::particles_col_t::const_iterator i_primary =
               my_primary_particles.begin();
           i_primary != my_primary_particles.end(); ++i_primary) {
        primary_type.push_back(i_primary->get_type());
        primary_energy.push_back(i_primary->get_kinetic_energy());
        primary_momentum["p_{x}"].push_back(i_primary->get_momentum().x());
        primary_momentum["p_{y}"].push_back(i_primary->get_momentum().y());
        primary_momentum["p_{z}"].push_back(i_primary->get_momentum().z());
      }

      vertex_position["x"].push_back(my_sim_data.get_vertex().x());
      vertex_position["y"].push_back(my_sim_data.get_vertex().y());
      vertex_position["z"].push_back(my_sim_data.get_vertex().z());
    }

    TRint* my_root_app = 0;
    if (visu_mode) {
      my_root_app = new TRint("test analysis macro", 0, 0);
    } else {
      clog << datatools::io::warning << appname_prefix << "No 'visu' mode has been set !" << endl;
      return EXIT_SUCCESS;
    }

    {
      // Primary particle type
      const int last_particle_type = genbb::primary_particle::ELECTRON;
      TH1I* primary_nature_histo =
          new TH1I("primary_nature_histo", "", last_particle_type + 2, 0, last_particle_type + 2);
      primary_nature_histo->SetTitle(";;# primary particles");
      primary_nature_histo->GetXaxis()->SetBinLabel(genbb::primary_particle::GAMMA + 1, "gamma");
      primary_nature_histo->GetXaxis()->SetBinLabel(genbb::primary_particle::POSITRON + 1,
                                                    "positron");
      primary_nature_histo->GetXaxis()->SetBinLabel(genbb::primary_particle::ELECTRON + 1,
                                                    "electron");
      primary_nature_histo->GetXaxis()->SetLabelSize(0.07);
      primary_nature_histo->LabelsOption("h", "X");

      for (vector<int>::const_iterator i = primary_type.begin(); i != primary_type.end(); ++i) {
        primary_nature_histo->Fill(*i);
      }

      TCanvas* canvas = new TCanvas("Test analysis macro", "Test analysis macro", 700, 500);

      primary_nature_histo->Draw();
    }

    {
      // Primary particle energy
      TH1D* primary_energy_histo = new TH1D("primary_energy_histo", "", 100, 0, 0);
      primary_energy_histo->SetTitle(";energy [MeV];# primary particles");
      for (vector<double>::const_iterator i = primary_energy.begin(); i != primary_energy.end();
           ++i) {
        primary_energy_histo->Fill(*i);
      }

      TCanvas* canvas = new TCanvas("Primary energy", "Primary energy", 700, 500);
      primary_energy_histo->Draw();
    }

    // Primary particle momentum
    for (map<string, vector<double> >::const_iterator i = primary_momentum.begin();
         i != primary_momentum.end(); ++i) {
      ostringstream oss;
      oss << "Particle momentum " << i->first;
      TCanvas* canvas = new TCanvas(oss.str().c_str(), oss.str().c_str(), 700, 500);
      oss.str().clear();
      oss << "histo_momentum_p" << i->first;
      TH1D* primary_momentum = new TH1D(oss.str().c_str(), "", 100, 0, 0);

      oss.str().clear();
      oss << ";" << i->first << " [MeV];#event";
      primary_momentum->SetTitle(oss.str().c_str());
      for (vector<double>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
        primary_momentum->Fill((*j) / CLHEP::MeV);
      }
      primary_momentum->Draw();
    }

    // Vertex position
    for (map<string, vector<double> >::const_iterator i = vertex_position.begin();
         i != vertex_position.end(); ++i) {
      ostringstream oss;
      oss << "Vertex position - " << i->first;
      TCanvas* canvas = new TCanvas(oss.str().c_str(), oss.str().c_str(), 700, 500);
      oss.str().clear();
      oss << "histo_vertex_position_" << i->first;
      TH1D* vertex_position = new TH1D(oss.str().c_str(), "", 100, 0, 0);

      oss.str().clear();
      oss << ";vertex position in " << i->first << " [mm];#event";
      vertex_position->SetTitle(oss.str().c_str());
      for (vector<double>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
        vertex_position->Fill(*j);
      }
      vertex_position->Draw();
    }

    // Test
    {
      TCanvas* canvas = new TCanvas("test", "test", 700, 500);
      TGraph* g_test = new TGraph();
      for (size_t i = 0; i < primary_energy.size(); ++i) {
        const double ekin = primary_energy.at(i);
        const double yvertex = vertex_position["y"].at(i);
        g_test->SetPoint(i, yvertex, ekin);
      }
      g_test->SetMarkerStyle(8);
      g_test->SetTitle(";y_{vertex} [mm]; E_{kinetic} [MeV]");
      g_test->Draw("AP");
    }

    if (my_root_app) my_root_app->Run(true);
  } catch (exception& x) {
    cerr << datatools::io::error << appname_prefix << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << datatools::io::error << appname_prefix << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
// end of test_analysis_macro.cxx
