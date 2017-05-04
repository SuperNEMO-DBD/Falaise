/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <fstream>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

#include <mybhep/hdf5.h>      // C++ API header file

using namespace std;  
using namespace mybhep;

#define SPACE1_NAME             "Space1"
#define SPACE1_DIM1             1
#define SPACE1_RANK             1

/** Example of use of event and event svc */

int main(int, char* [])
{
  // instantiate the event service
  
  mybhep_svc esvc;
  hsize_t        cdims[1] ;
  hid_t		fid1;		         
  hid_t 	dataset;
  hid_t		sid1;
  hid_t		tid1,cparms;



  herr_t	ret;
  /* Generic return value  */

  // h5 file
   /* Create file */
  fid1 = H5Fcreate ("idst3.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  printf ("H5Fcreate (fid1): %i\n", fid1);

  string def;

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");
  
  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");

  time_t time1;
  time(&time1);
  char* ct1 = ctime(&time1);
  cout << " Start loop: Time= " << ct1 << endl;
  
  for(size_t ievent =1; ievent <= 2000; ievent++){
    if (ievent%50 ==0)
      cout << " storing event number = " << ievent << endl;
    
    generate_event gevt(ievent);
    gevt.create_particles(2);
    gevt.add_tracks(materials);
    gevt.add_hits(detectors);

    event* evt = gevt.retrieve();
    event_cvt cvt(*evt,ievent);

    def = cvt.store();

    /* Create dataspace for datasets */
    sid1 = H5Screate(H5S_SCALAR);

    /* Create a datatype to refer to */
    tid1 = H5Tcopy (H5T_C_S1);
    size_t DIM = def.size();
    ret = H5Tset_size (tid1,DIM);

    cdims[0]=100;
    cparms = H5Pcreate (H5P_DATASET_CREATE);    
    ret = H5Pset_chunk ( cparms, 1, cdims);


    ret = H5Pset_deflate ( cparms, 6);

//    wdata[0] = def.c_str();

    /* Create a dataset */
    string skey = "/"+to_string(ievent);
    const char* key = skey.c_str();
    dataset = H5Dcreate (fid1, key, tid1, sid1,H5P_DEFAULT);
    //dataset = H5Dcreate (fid1, key, tid1, sid1,cparms);

    /* Write dataset to disk */
    ret = H5Dwrite (dataset, tid1, H5S_ALL, H5S_ALL, H5P_DEFAULT, def.c_str());
    //printf ("H5Dwrite: %i\n", ret);

    /* Close Dataset */
    ret = H5Dclose (dataset);
    //printf ("H5Dclose: %i\n", ret);
    ret = H5Tclose (tid1);
    ret = H5Sclose (sid1);
  }

    /* Close Everything */
    ret = H5Fclose (fid1);
    time_t time2;
    time(&time2);
    char* ct2 = ctime(&time2);
    cout << " End loop: Time= " << ct2 << endl;
    
    double telap = difftime(time2,time1);
    cout << " Elapsed time= " << telap << " seconds " << endl;

    // Sequential access
    fid1 = H5Fopen("idst.h5",H5F_ACC_RDWR,H5P_DEFAULT);
    printf ("H5Fopen (fid1): %i\n", fid1);


  for(size_t ievent =1; ievent <= 0; ievent++){
    if (ievent%50 ==0)    
      cout << " reading event number = " << ievent << endl;
    
    /* Open a dataset */
    string skey = "/"+to_string(ievent);
    const char* key = skey.c_str();

    dataset = H5Dopen(fid1,key);
    tid1 = H5Tcopy (H5T_C_S1);
    size_t DIM = 4735;
    ret = H5Tset_size (tid1,DIM);
    printf ("H5Tset returns: %i\n", ret);
    char* rdata = new char[DIM];   /* Information read in */
    ret= H5Dread (dataset, tid1, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);

    printf ("H5Dread returns: %i\n", ret);
    /* Validate and print data read in */

    def = rdata;
    event_cvt cvt(def);

    cout << "making new event" << endl;
    event* new_event = cvt.create();
    cout << *new_event << endl;

    mybhep_svc::instance().clear_event();
    delete new_event;

    /* Close Everything */
    ret = H5Dclose (dataset);
    printf ("H5Dclose: %i\n", ret);
    ret = H5Tclose (tid1);
    printf ("H5Tclose: %i\n", ret);
    delete [] rdata;
  }

  ret = H5Fclose (fid1);
  printf ("H5Fclose: %i\n", ret);

  return 0;
  
}
