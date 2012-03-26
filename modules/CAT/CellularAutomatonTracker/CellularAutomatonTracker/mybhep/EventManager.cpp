#include <mybhep/EventManager.h>

namespace mybhep{

//*************************************************************
EventManager::EventManager(const mybhep::gstore& gs,mybhep::prlevel vl){
//*************************************************************

    
    store = gs;
    
    m = mybhep::messenger(vl);
    
    init = false;
    outInit = false;

    cifile = 0; // init counter for input files
    cofile = 0; // init counter for input files
    tievents = 0; //init counter for total input events
    toevents = 0; //init counter for total output events
    
    m.message("++EventManager Messenger generated++",mybhep::VERBOSE);
    
    readParam();

}

//*************************************************************
EventManager::~EventManager(){
//*************************************************************
  
}


//*************************************************************
bool EventManager::initialize(){
//*************************************************************

    // Open files to read
    
    m.message("+++ Init method of EventManager +++",mybhep::VERBOSE);
    
    // check there are more files to read

    if (nifiles == cifile) {init=false; return false;}
    
    init = true;
    
    // open input files
    
    initIO("r"); 
    
    // read global properties if needed
    
    readGlobalProperties();
    
    return true;

}

//*************************************************************
bool EventManager::initIO(string mode){
//*************************************************************
  
  if (mode=="r"){ //open input files

    ifile = idst_files[cifile];
    m.message("Input dst file:",ifile,mybhep::NORMAL);
    iext = getFileType(ifile); 
    
    if (iext=="gz") rgz.open(ifile); 
#ifdef HDF5
    else if (iext=="h5") rh5.open(ifile); 
#endif
    else if (iext=="txt") rtxt.open(ifile); 
    else{
        m.message("File format not recognized",iext,mybhep::MUTE);
        exit(1);
    }
    cifile += 1; ievent=0;
  }
  
  if (mode=="w"){ //open output files
    
    ofile = odst_files[cofile]; 
    m.message("Output dst file:",ofile,mybhep::NORMAL);
    oext = getFileType(ofile);  
    
    if (oext=="gz") wgz.open(ofile);
#ifdef HDF5 
    else if (oext=="h5") wh5.open(ofile);
#endif 
    else if (oext=="txt") wtxt.open(ofile);
    else{
        m.message("File format not recognized",oext,mybhep::MUTE);
        exit(1);
    }
    outInit=true; cofile += 1; oevent=0;
  }
  
  return true;
}

//*************************************************************
mybhep::event&  EventManager::read(size_t ie) {
//*************************************************************
    
    //----- read event -----//
    
    if (ie) {mybhep::event& e = random_read(ie);return e;} 
    else  {mybhep::event& e = sequential_read(); return e;} 
          
}

//*************************************************************
mybhep::event& EventManager::sequential_read() {
//*************************************************************
    
    if (!init){
        m.message("EventManager not initialized!",mybhep::MUTE);
        exit(1);}

    bool ok=status();
    if (!ok) {m.message("Abort!",mybhep::MUTE);exit(1);}
    m.message("Reading event number:",ievent,mybhep::VERBOSE);
    ievent +=1; tievents += 1;  

    if(iext=="gz"){mybhep::event& ev=rgz.read_next();return ev;}
#ifdef HDF5 
    else if(iext=="h5"){mybhep::event& ev = rh5.read_event(ievent);return ev;}
#endif    
    else {mybhep::event& ev=rtxt.read_next();return ev;}
      
}

//*************************************************************
mybhep::event& EventManager::random_read(size_t ie) {
//*************************************************************
  

    // compute in which file is placed event number ie
  
    m.message("Looking for event",ie,mybhep::NORMAL);
    
    //start looking at the first file

    if (cifile!=1) {cifile=0;cofile=0;closeFiles();initialize();}
    else { if (!init) {closeFiles(); initialize();} }

    // open file where event is placed

    size_t index = searchEvent(ie); 
    m.message("Reading event number:",index,mybhep::VERBOSE);
    ievent+=1; tievents += 1;
    
    // read event

    if(iext=="gz"){mybhep::event& ev=rgz.read_event(index);return ev;}
#ifdef HDF5
    else if(iext=="h5"){mybhep::event& ev=rh5.read_event(index);return ev;}
#endif    
    else {mybhep::event& ev=rtxt.read_event(index);return ev;}
}

//*************************************************************
size_t EventManager::searchEvent(size_t ie){
//*************************************************************    
    
    /* When reading randomly, look for the file 
       where the event is placed
     */ 
    
    size_t index = ie;
   
    size_t max_evt = getNumEventsFile();

    m.message("Events in file:",max_evt,mybhep::NORMAL);
    
    while(index+1 > max_evt){
            
        index -= max_evt;
        bool ok = newFile(); 
        if (!ok){
	  m.message("Abort: event doesn't exist!",mybhep::MUTE);
	  exit(1);}
       
	max_evt = getNumEventsFile();

        m.message("Events in file:",max_evt,mybhep::NORMAL);
    }

    return index;
}

//*************************************************************
size_t EventManager::getNumEventsFile(){
//*************************************************************  

  /*
    Get number of events in current file
   */
  
  size_t max_evt=0;
  
  if (iext=="txt"){max_evt = rtxt.max_events();}
  else if (iext=="gz"){max_evt = rgz.max_events();}
#ifdef HDF5
  else{max_evt = rh5.max_events();}
#endif  
  return max_evt;

}


//*************************************************************
bool EventManager::status(){
//*************************************************************    
    
    m.message("EventManager status function",mybhep::VVERBOSE);

    if (iext=="gz"){
        
        bool file=true;
        bool end = rgz.eof();
        if (end) file=newFile();
        return file;                    
    }
    
#ifdef HDF5
    else if (iext=="h5"){
        bool file=true;
        bool end = rh5.eof(ievent+1);
        if (end) file=newFile();
        return file;} 
#endif    

    else{
        bool file=true;
        bool end = rtxt.eof();
        if (end) file=newFile();
        return file;     
    }

}

//*************************************************************
void EventManager::write(mybhep::event& event) {
//*************************************************************
  
    // write event  
      
    if (!outInit) {

      initIO("w"); // open output file 
      
      //write header if needed
      
      writeGlobalProperties();

    }
    
    m.message("Writing event number:",oevent,mybhep::VERBOSE);

    if (oext=="gz") {wgz.write(event,oevent);}
#ifdef HDF5
    else if (oext=="h5") {wh5.write(event,oevent);}
#endif
    else {wtxt.write(event,oevent);}
    
    oevent += 1;
    toevents += 1;
    
}


//*************************************************************
bool EventManager::finalize() {
//*************************************************************

    // print info
    
    m.message("+++++ End method of EventManager +++++",mybhep::NORMAL);
    m.message("Total number of events read:",tievents,mybhep::NORMAL);
    m.message("Total number of events written:",toevents,mybhep::NORMAL);
    
    closeFiles(true);
    
    //reset counters
    cifile=0;
    cofile=0;
    toevents = 0;
    tievents = 0;

    return true;

}

//*************************************************************
bool EventManager::newFile(){
//*************************************************************
    
    endOfFile();
    bool ok = initialize();
    if (!ok) m.message("No more files to read!",mybhep::MUTE);
    return ok;

}

//*************************************************************
void EventManager::endOfFile(){
//*************************************************************

    m.message("+++ End of file",ifile," +++",mybhep::NORMAL);

    m.message("Number of events read:",ievent,mybhep::NORMAL);
    
    if (outInit){
      m.message("Output File:",ofile,mybhep::NORMAL);
      m.message("Number of events written:",oevent,mybhep::NORMAL);
    }

    closeFiles();
   
}

//*************************************************************
void EventManager::closeFiles(bool force){
//*************************************************************
  
  if (nofiles!=1 || force){ //do not close if nofiles=1 
    if (oext=="gz") wgz.close();
#ifdef HDF5
    if (oext=="h5") wh5.close();
#endif
    if (oext=="txt") wtxt.close();
    outInit=false;
  }
    
  if (iext=="gz") rgz.close();
#ifdef HDF5
  if (iext=="h5") rh5.close();
#endif
  if (iext=="txt") rtxt.close(); 
    
  init=false; 

}

//*************************************************************
void EventManager::readParam(){
//*************************************************************
    
    /*
      deals with input parameters
     */
    
    //input files
    idst_files = store.fetch_svstore("idst_files");  
    nifiles = idst_files.size();
    
    //output files
    bool ofiles = store.find_svstore("odst_files");
    if (ofiles){
        odst_files = store.fetch_svstore("odst_files");
	nofiles = odst_files.size();
        // check number of files: must be 1 or nifiles
        if (nifiles != nofiles && nofiles!=1){
            string ms ="Wrong number of output files!";
            m.message(ms,mybhep::MUTE);
            exit(1);}
    }
    else{ //no output files defined. Generate default ones.
        
        for (size_t i=0; i<nifiles; i++){
            string number = mybhep::to_string(i);
            string name = "output_default_"+number+".gz";
            odst_files.push_back(name);}
    }

    // 
    
    bool allow_warnings = store.find_istore("warnings");
    if (allow_warnings) warnings = store.fetch_istore("warnings");  
    else warnings = true;
}


//*************************************************************
string EventManager::getFileType(string dst){
//*************************************************************

    size_t i2= dst.find_last_of(".")+1;
    size_t il= dst.size()-1;
    string ext=dst.substr(i2,il);

    return ext;

}
}
