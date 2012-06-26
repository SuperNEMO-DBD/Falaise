/* -*- mode: c++ -*- */
#ifndef _event_man___
#define _event_man___

#include <mybhep/gstore.h>
#include <mybhep/mybhep_svc.h>
#include <mybhep/messenger.h>
#include <mybhep/dvector.h>

namespace mybhep{
class EventManager{


public:

  EventManager(const mybhep::gstore& gs,
	       mybhep::prlevel vl=mybhep::NORMAL);
  virtual ~EventManager();


  bool status();
  bool initialize();
  mybhep::event& read(size_t ie=0);
  void write(mybhep::event& event);
  bool finalize();

  virtual size_t getNumEventsFile();
  virtual bool isNewFile(){return (ievent==0);}

protected:

  bool initIO(std::string mode);
  mybhep::event& random_read(size_t ie);
  mybhep::event& sequential_read();
  size_t searchEvent(size_t);
  void endOfFile();
  void closeFiles(bool force=false);
  bool newFile();
  std::string getFileType(std::string dst);
  void readParam();

  virtual bool readGlobalProperties(){return false;};
  virtual void writeGlobalProperties(){};

protected:

    // control bools

    bool init; // read file open and counters ready
    bool outInit; // output file open
    bool warnings; // allow warnings

    // input and output files

    size_t nifiles; //max num of input files
    size_t nofiles; //max num of output files
    size_t cifile; //counter for input files
    size_t cofile; //counter for output files
    std::string ifile; //name of current file
    std::string ofile; //name of current file
    mybhep::dvector<std::string> idst_files; //vector of file names
    mybhep::dvector<std::string> odst_files; //vector of file names

    //instantiate event service
    mybhep::mybhep_svc bsvc;

    // readers
    std::string iext;
    size_t ievent;
    size_t tievents;
    mybhep::reader_gz rgz;
    mybhep::reader_txt rtxt;
#ifdef Hdf5
    mybhep::reader_hdf5 rh5;
#endif
    // writters
    std::string oext;
    size_t oevent;
    size_t toevents;
    mybhep::writer_gz wgz;
    mybhep::writer_txt wtxt;
#ifdef Hdf5
    mybhep::writer_hdf5 wh5;
#endif
    // messenger
    mybhep::messenger m;

    // store for parameters
    mybhep::gstore store;



};
}
#endif
