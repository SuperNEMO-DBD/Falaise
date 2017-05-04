/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <fstream>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
//#include <mybhep/testhdf5.h>   // C test header file
#include <mybhep/H5Cpp.h>      // C++ API header file

using namespace std;  
using namespace mybhep;
using namespace H5;

#define SPACE1_DIM1             1
#define SPACE1_RANK             1

/** Example of use of event and event svc */

int main(int, char* [])
{
  // instantiate the event service
  
  mybhep_svc esvc;
  const char *wdata[SPACE1_DIM1];  // string to write
  char *rdata[SPACE1_DIM1];   /* Information read in */
  hsize_t       dims1[] = {SPACE1_DIM1};
  hid_t		native_type;       /* Datatype ID */

  // h5 file
  H5File file("idst.h5", H5F_ACC_TRUNC);

  /* Create dataspace for datasets */
  DataSpace sid1(SPACE1_RANK, dims1);

  /* Create a datatype to refer to */
  StrType tid1(PredType::C_S1, H5T_VARIABLE);

  Assert(H5T_STRING==H5Tget_class(tid1.getId()) &&
	 H5Tis_variable_str(tid1.getId()) ,__FILE__,__LINE__,
           internal_logic("This is not a variable lenght string type!"));

  string def;

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");
  
  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");
  
  for(size_t ievent =1; ievent <= 20; ievent++){
    
    cout << " storing event number = " << ievent << endl;
    
    generate_event gevt(ievent);
    gevt.create_particles(2);
    gevt.add_tracks(materials);
    gevt.add_hits(detectors);

    event* evt = gevt.retrieve();
    event_cvt cvt(*evt,ievent);

    def = cvt.store();
    wdata[0] = def.c_str();

    /* Create a dataset */

    DataSet dataset = file.createDataSet(to_string(ievent), tid1, sid1);

    /* Write dataset to disk */
    dataset.write((void*)wdata, tid1);
    /* Close Dataset */
    dataset.close();

  }
  // Sequential access
  for(size_t ievent =1; ievent <= 20; ievent++){
    
    cout << " reading event number = " << ievent << endl;
    
    /* Open a dataset */
    DataSet dataset = file.openDataSet(to_string(ievent));

    /* Get datatype for dataset */
    DataType dtype = dataset.getDataType();

    /* Construct native type */
    if((native_type=H5Tget_native_type(dtype.getId(), H5T_DIR_DEFAULT))<0)
        cerr << "H5Tget_native_type  failed!!! " << endl;

    /* Check if the data type is equal */
    if(!H5Tequal(native_type, tid1.getId()))
        cerr << "native type is not tid1!!!" << endl;

    /* Read dataset from disk */
    dataset.read((void*)rdata, dtype);
    /* Validate and print data read in */
    cout << "data read:" << endl;
    def = rdata[0];
    event_cvt cvt(def);

    cout << "making new event" << endl;
    event* new_event = cvt.create();
    cout << *new_event << endl;

    mybhep_svc::instance().clear_event();
    delete new_event;

  }


  return 0;
  
}
