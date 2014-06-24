#include <mybhep/EventManager2.h>
#include <mybhep/sreader.h>
#include <mybhep/gstore.h>
#include <string.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <mybhep/system_of_units.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/tracked_data.h>
#include <CATAlgorithm/clusterizer.h>
#include <CATAlgorithm/sequentiator.h>
#include <CATUtils/EventDisplay.h>
#include <CATUtils/ntupler.h>
#include <CATAlgorithm/Clock.h>

#define APPNAME_PREFIX "local_tracking: "

using namespace std;
using namespace CAT;

int main(int argc_, char* argv_[]){
    
  cout << APPNAME_PREFIX << endl;


  int nevents = -1;
  int first_event = -1;
  int first_event_number = -1;
  int verbosity_level = 5;
  int modulo = 1;
  bool plot_mode = false;
  bool root_ntuple_mode = false;
  string param_file="param/cats.param";
  string limits_file="param/limits.param";
  
  int iarg = 1;
  int iarg_old = 1;


  Clock cl;
  cl.start("local_tracking");
  cl.start("read parameters");

  // read input flags
  while (iarg < argc_)
    {
      string token = argv_[iarg];

      if (token[0] == '-')
	{
	  string option = token;
	  if ((option == "-h") || (option == "--help"))
	    {
	      cout << endl
		   << " usage: test_local_tracking [OPTIONS] <INPUT_FILE> <OUTPUT_FILE>" << endl
		   << endl << ""
		   << endl << "   -h --help    :   print this help"
		   << endl << "   --param <string>  :   parameter file for tracking (default: " << param_file << ") "
		   << endl << "   -l --limits  <string> :   limits file for tracking (default: " << limits_file << ") "
		   << endl << "   -m --modulo <integer>   :  modulo for record print  "
		   << endl << "   -n <integer> :      number of events to track , by default all from the DST (recommended) "
		   << endl << "   -f --first <integer>:      number of first event to track"
		   << endl << "   -fen --first_event_number <integer>:      number of first event number to track"
		   << endl << "   -v <integer>:      set verbosity level "
		   << endl << "             0 = mute, "
		   << endl << "             4 = , "
		   << endl << "             5 (default) = print warnings, "
		   << endl << "             6 = event display & reconstruction, "
		   << endl << "             7 = tracks creation, "
		   << endl << "             8 = details, "
		   << endl << "             9 = , "
		   << endl << "            10 = all, "
		   << endl << "   -p or --plot :      plot event display and save picture"
		   << endl << "   --with-root-ntuple :      create root ntuple"
		   << endl << ""
		   << endl << ""
		   << endl << endl;

	      return (0);
	    }
	  else if (option == "-n")
	    {
	      iarg++;
	      nevents = atoi(argv_[iarg]);
	      cout << APPNAME_PREFIX << " number of events to track is: " << nevents << endl;
	    }
	  else if ((option == "-f") || (option == "--first"))
	    {
	      iarg++;
	      first_event = atoi(argv_[iarg]);
	      cout << APPNAME_PREFIX << " first event to track is " << first_event << endl;
	    }
	  else if ((option == "-fen") || (option == "--first_event_number"))
	    {
	      iarg++;
	      first_event_number = atoi(argv_[iarg]);
	      cout << APPNAME_PREFIX << " first event number to track is " << first_event_number << endl;
	    }
	  else if (option == "-v")
	    {
	      iarg++;
	      verbosity_level = atoi(argv_[iarg]);
	      cout << APPNAME_PREFIX << " verbosity level: " << verbosity_level << endl;
	    }
	  else if ((option == "-p") || (option == "--plot"))
	    {
	      plot_mode = true;
	      cout << APPNAME_PREFIX << " will plot event display " << endl;
	    }
	  else if (option == "--with-root-ntuple")
	    {
	      root_ntuple_mode = true;
	      cout << APPNAME_PREFIX << " will create root ntuple " << endl;
	    }
	  else if (option == "--param")
	    {
	      ++iarg;
	      param_file = argv_[iarg];
	    }
	  else if ((option == "-l") || (option == "--limits"))
	    {
	      ++iarg;
	      limits_file = argv_[iarg];
	    }
	  else if ((option == "-m") || (option == "--modulo"))
	    {
	      int mod;
	      istringstream iss (argv_[++iarg]);
	      iss >> mod;
	      if (! iss)
		{
		  cout << APPNAME_PREFIX << " warning: invalid format for 'modulo' (" << argv_[iarg] << ") !";
		}

	      if (mod > 1)
		{
		  modulo = mod;
		}
	    }
	  else
	    {
	      cout << APPNAME_PREFIX << " warning: ignoring option '" << option << "'!" << endl;
	    }
	}
      else
	{
	  string argument = token;
	  
	  if( iarg_old == 1 )
	    nevents = atoi(argv_[iarg]);
	  else if( iarg_old == 2 )
	    param_file= argv_[iarg];
	  else if( iarg_old == 3 )
	    limits_file= argv_[iarg];
	  else
	    {
	      cout << APPNAME_PREFIX << " warning: ignoring argument '" << argument << "'!" << endl;
	    }
	  iarg_old++;
	}
      iarg++;
    }
  

  cl.stop("read parameters");
  cl.start("initialize");

  //read input parameter files
  mybhep::gstore store;
  mybhep::sreader reader(store);
  reader.file(param_file);
  reader.group("DATA");
  reader.read();
  mybhep::sreader reader2(store);
  reader2.file(limits_file);
  reader2.group("ANA");
  reader2.read();

  
  //set verbosity level
  mybhep::prlevel verb_snlocal_tracking = mybhep::NORMAL;

  mybhep::EventManager2* eman = 0; 

  clusterizer* clustering_machine = 0;
  sequentiator* sequentiating_machine = 0;

 
  eman = new mybhep::EventManager2(store,verb_snlocal_tracking);


  clustering_machine = new clusterizer(store);
  sequentiating_machine = new sequentiator(store);
  
  
  //initialize algorithms

  eman->initialize();

  ntupler* nt = new ntupler();
  if( root_ntuple_mode ){
    nt->initialize();
  }

  mybhep::sstore properties = eman->get_dst_properties(); 

  // set event display                                                                                                          
  EventDisplay* display = new EventDisplay(store);
  if( plot_mode ){
    //    TApplication *OpenRoot;
    //    OpenRoot = new  TApplication("OpenRoot",0,0);
    display->InitializeDisplayEvent(properties, store, eman);
    display->SetPlutsMode(true);                                                                                            
  }


  clustering_machine->initialize( properties, store, eman);
  sequentiating_machine->initialize( properties, store, eman);

  cl.stop("initialize");

  if( nevents >= 0 && first_event >= 0 )
    nevents += first_event;

  size_t count_events = 0;
  size_t count_selected = 0;
  size_t count_skipped = 0;

  //loop over events and execute algos
  for(int i=0; i >= 0; i++) {
    cl.start("loop","cumulative");

    if( first_event_number < 0 && nevents >= 0 && i >= nevents ) break;

    bool ok = eman->status();  //  state of event maneger
    
    if (!ok) break; // avoids eof
    
    mybhep::event& event_ref = eman->read();  //  reads event and assigns a reference
    
    if( i < first_event ) continue;

    Int_t evn = -1;
    if( event_ref.find_property("G4_evt_num") ){
      evn = mybhep::int_from_string(event_ref.fetch_property("G4_evt_num"));
      if( evn < first_event_number ) continue;
      if( evn%modulo == 0 )
	cout << " event " << i << " event number " << evn << endl;
    }
    if( event_ref.find_property("NEMOEVNUMBER") ){
      evn = mybhep::int_from_string(event_ref.fetch_property("NEMOEVNUMBER"));
      if( evn < first_event_number ) continue;
      if( evn%modulo == 0 )
	cout << " event " << i << " event number " << evn << endl;
    }
    else{
      evn = i;
      if( evn%modulo == 0 )
	cout << " event " << i << endl;
    }

    if( first_event_number >= 0 && nevents >= 0 && (Int_t)count_events >= nevents ) break;

    for (int im=0; im< clustering_machine->get_module_nr(); im++) //loop over all Modules
      {
	count_events ++;

	clustering_machine->set_module_nr(mybhep::to_string(im));

	topology::tracked_data tracked_data_;

	if( !clustering_machine->read_event(event_ref, tracked_data_) ){
	  continue;
	}

	clustering_machine->clusterize(tracked_data_);
	  
	bool skip_processing = false;

	if( tracked_data_.selected() )
	  count_selected ++;
	else
	  skip_processing = true;
	
	if( !skip_processing ){
	  sequentiating_machine->sequentiate(tracked_data_);
	}

	if( tracked_data_.skipped() ){
	  count_skipped ++;
	  skip_processing = true;
	}

	if( !skip_processing ){
	  if( plot_mode ){
	    display->DeleteDisplay();
	    display->execute(event_ref, evn, tracked_data_ );
	  }
	  
	  if( root_ntuple_mode ){
	    nt->set_tracked_data(tracked_data_);
	    nt->__fill();
	  }
	}
  
	eman->write(event_ref);
      }


    cl.stop("loop");

  }

  cl.start("finalize");
  //finalize algorithms
  clustering_machine->finalize();
  sequentiating_machine->finalize();
  eman->finalize();

  if( plot_mode )
    display->Finalize();

  if( root_ntuple_mode )
    nt->finalize();

  delete eman; 

  delete clustering_machine;
  delete sequentiating_machine;

  cl.stop("finalize");
  cl.stop("local_tracking");
  cl.dump();

  cout << " number of read events: " << count_events << endl;
  cout << " number of selected events: " << count_selected << " ( " << (double)count_selected/(double)count_events << " ) " << endl;
  cout << " number of skipped events: " << count_skipped << " ( " << (double)count_skipped/(double)count_events << " ) " << endl;
  cout << " number of reconstructed events: " << count_selected - count_skipped << " ( " << (double)(count_selected - count_skipped)/(double)(count_events) << " ) " << endl;

  return true;    
}


