/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/generate_event.h>
#include <fstream>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

#include <mybhep/hdf5.h>  // C++ API header file

using namespace std;
using namespace mybhep;

#define SPACE1_NAME "Space1"
#define SPACE1_DIM1 1
#define SPACE1_RANK 1

/** Example of use of event and event svc */

int main(int, char* []) {
  // instantiate the event service

  mybhep_svc esvc;
  const char* wdata[SPACE1_DIM1];  // string to write
  char* rdata[SPACE1_DIM1];        /* Information read in */
  hsize_t dims1[] = {SPACE1_DIM1};
  hid_t fid1, grp;
  hid_t dataset;
  hid_t sid1;
  hid_t tid1, cparms, xfer_pid;

  hsize_t maxdims1[1] = {H5S_UNLIMITED};
  hsize_t chkdims1[1];

  herr_t ret;
  /* Generic return value  */

  // h5 file
  /* Create file */
  fid1 = H5Fcreate("gidst.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  printf("H5Fcreate (fid1): %i\n", fid1);

  /* Create dataspace for datasets */
  sid1 = H5Screate_simple(SPACE1_RANK, dims1, maxdims1);
  printf("H5Screate_simple (sid1): %i\n", sid1);

  /* Create a datatype to refer to */
  tid1 = H5Tcopy(H5T_C_S1);
  printf("H5Tcopy (tid1): %i\n", tid1);

  ret = H5Tset_size(tid1, H5T_VARIABLE);
  printf("H5Tset_size: %i\n", ret);

  cparms = H5Pcreate(H5P_DATASET_CREATE);
  printf("H5Pcreate returns: %i\n", cparms);

  chkdims1[0] = 1;
  ret = H5Pset_chunk(cparms, 1, chkdims1);
  printf("H5Pset_chunk returns: %i\n", ret);
  ret = H5Pset_deflate(cparms, 6);
  printf("H5Pset_deflate returns: %d\n", ret);

  string def;

  vector<string> materials;
  materials.push_back("Air");
  materials.push_back("Al");
  materials.push_back("Fe");

  vector<string> detectors;
  detectors.push_back("NDC");
  detectors.push_back("CHE");
  detectors.push_back("TOF");

  for (size_t ievent = 1; ievent <= 200; ievent++) {
    if (ievent % 100 == 0) cout << " storing event number = " << ievent << endl;

    generate_event gevt(ievent);
    gevt.create_particles(2);
    gevt.add_tracks(materials);
    gevt.add_hits(detectors);

    event* evt = gevt.retrieve();
    event_cvt cvt(*evt, ievent);

    def = cvt.store();
    wdata[0] = def.c_str();

    /* Create a dataset */
    string skey = "/" + to_string(ievent);
    const char* key = skey.c_str();

    dataset = H5Dcreate(fid1, key, tid1, sid1, cparms);

    /* Write dataset to disk */
    ret = H5Dwrite(dataset, tid1, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);

    /* Close Dataset */
    ret = H5Dclose(dataset);
  }

  /* Close Everything */
  ret = H5Tclose(tid1);
  printf("%i ", ret);
  ret = H5Sclose(sid1);
  printf("%i ", ret);
  ret = H5Pclose(cparms);
  printf("%i ", ret);
  ret = H5Fclose(fid1);
  printf("%i\n", ret);

  // Sequential access
  fid1 = H5Fopen("gidst.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  printf("H5Fopen (fid1): %i\n", fid1);
  grp = H5Gopen(fid1, "/");
  printf("H5Gopen: %d\n", grp);

  /* Create a datatype to refer to */
  tid1 = H5Tcopy(H5T_C_S1);
  printf("H5Tcopy (tid1): %i\n", tid1);
  ret = H5Tset_size(tid1, H5T_VARIABLE);
  printf("H5Tset_size: %i\n", ret);

  for (size_t ievent = 1; ievent <= 200; ievent++) {
    cout << " reading event number = " << ievent << endl;

    /* Open a dataset */
    const char* key = to_string(ievent).c_str();
    dataset = H5Dopen(grp, key);

    xfer_pid = H5Pcreate(H5P_DATASET_XFER);
    printf("H5Pcreate: %i\n", xfer_pid);

    ret = H5Dread(dataset, tid1, H5S_ALL, H5S_ALL, xfer_pid, rdata);
    printf("H5Dread returns: %i\n", ret);
    /* Validate and print data read in */

    cout << "data read:" << endl;
    def = rdata[0];
    event_cvt cvt(def);

    cout << "making new event" << endl;
    event* new_event = cvt.create();
    cout << *new_event << endl;

    mybhep_svc::instance().clear_event();
    delete new_event;
    /* Close Everything */
    ret = H5Dclose(dataset);
    printf("H5Dclose: %i\n", ret);
  }
  printf("Close everything: ");
  ret = H5Tclose(tid1);
  printf("%i ", ret);
  ret = H5Pclose(xfer_pid);
  printf("%i ", ret);
  ret = H5Fclose(fid1);
  printf("%i\n", ret);

  return 0;
}
