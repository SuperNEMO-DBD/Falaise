#include "CATAlgorithm/sequentiator.h"
#include <vector>
#include <mybhep/system_of_units.h>
#include <sys/time.h>
#include <math.h>

namespace CAT {

  using namespace std;
  using namespace mybhep;

  void sequentiator::_set_defaults ()
  {
    bfield = std::numeric_limits<double>::quiet_NaN ();
    level = mybhep::NORMAL;
    m = mybhep::messenger(level);
    num_blocks = -1;
    planes_per_block.clear ();
    gaps_Z.clear ();
    GG_CELL_pitch = std::numeric_limits<double>::quiet_NaN ();
    GG_GRND_diam = std::numeric_limits<double>::quiet_NaN ();
    GG_CELL_diam = std::numeric_limits<double>::quiet_NaN ();
    CHAMBER_X = std::numeric_limits<double>::quiet_NaN ();
    GG_BLOCK_X = std::numeric_limits<double>::quiet_NaN ();
    num_cells_per_plane = -1;
    SOURCE_thick = std::numeric_limits<double>::quiet_NaN ();
    //    lastlayer = 0;
    vel  = std::numeric_limits<double>::quiet_NaN ();
    rad  = std::numeric_limits<double>::quiet_NaN ();
    len  = std::numeric_limits<double>::quiet_NaN ();
    CellDistance  = std::numeric_limits<double>::quiet_NaN ();
    xsize = ysize = zsize = std::numeric_limits<double>::quiet_NaN ();
    //    calo_X = calo_Y = calo_Z = std::numeric_limits<double>::quiet_NaN ();
    InnerRadius = OuterRadius= FoilRadius = std::numeric_limits<double>::quiet_NaN ();
    pmax = std::numeric_limits<double>::quiet_NaN ();

    SmallRadius = std::numeric_limits<double>::quiet_NaN ();
    TangentPhi = std::numeric_limits<double>::quiet_NaN ();
    TangentTheta = std::numeric_limits<double>::quiet_NaN ();
    SmallNumber = std::numeric_limits<double>::quiet_NaN ();
    QuadrantAngle = std::numeric_limits<double>::quiet_NaN ();
    Ratio = std::numeric_limits<double>::quiet_NaN ();
    CompatibilityDistance = std::numeric_limits<double>::quiet_NaN ();
    MaxChi2 = std::numeric_limits<double>::quiet_NaN ();
    probmin = std::numeric_limits<double>::quiet_NaN ();
    NOffLayers = 0;
    first_event_number = 0;
    PrintMode = false;
    SuperNemo = true;
    SuperNemoChannel = false;
    NemoraOutput = false;
    N3_MC = false;
    MaxTime = std::numeric_limits<double>::quiet_NaN ();
    //    doDriftWires = true;
    //    DriftWires.clear ();
    eman = 0;
    _moduleNR.clear ();
    _MaxBlockSize = -1;
    event_number=0;
    hfile.clear ();

    nevent = 0;
    InitialEvents = 0;
    SkippedEvents = 0;
    run_list.clear ();
    run_time = std::numeric_limits<double>::quiet_NaN ();
    first_event=true;
    return;
  }





  //************************************************************
  sequentiator::sequentiator(mybhep::gstore st){
    //*************************************************************

    level = mybhep::get_info_level(st.fetch_sstore("VERBOSITY"));

    m = mybhep::messenger(level);

    //-- read param --//

    pmax = st.fetch_dstore("pmax")*mybhep::MeV;
    MaxTime = st.fetch_dstore("MaxTime");
    PrintMode = st.fetch_istore("PrintMode");

    SmallRadius=st.fetch_dstore("SmallRadius")*mybhep::mm;
    TangentPhi=st.fetch_dstore("TangentPhi");
    TangentTheta=st.fetch_dstore("TangentTheta");
    SmallNumber=st.fetch_dstore("SmallNumber")*mybhep::mm;
    QuadrantAngle=st.fetch_dstore("QuadrantAngle");
    Ratio=st.fetch_dstore("Ratio");
    CompatibilityDistance=st.fetch_dstore("CompatibilityDistance");
    MaxChi2=st.fetch_dstore("MaxChi2");

    if (st.find_istore("nofflayers"))
      NOffLayers = st.fetch_istore("nofflayers");
    else
      NOffLayers = 1;

    if (st.find_istore("first_event"))
      first_event_number = st.fetch_istore("first_event");
    else
      first_event_number = -1;

    if (st.find_sstore("histo_file"))
      hfile=st.fetch_sstore("histo_file");
    else
      hfile="CatsHistogram.root";

    if (st.find_dstore("probmin"))
      probmin=st.fetch_dstore("probmin");
    else
      probmin=1.e-200;

    /*
      if( PrintMode )
      hman =  NHistoManager2(st);
    */

  }

  //************************************************************
  // Default constructor :
  sequentiator::sequentiator(void){
    //*************************************************************
    _set_defaults ();
    return;
  }


  //*************************************************************
  sequentiator::~sequentiator() {
    //*************************************************************

  }

  //*************************************************************
  bool sequentiator::initialize( const mybhep::sstore & store, const mybhep::gstore & /*gs*/ , mybhep::EventManager2 *eman) {
    //*************************************************************

    m.message("CAT::sequentiator::initialize: Beginning algorithm sequentiator",mybhep::VERBOSE); fflush(stdout);

    clock.start(" sequentiator: initialize ");

    //----------- read dst param -------------//

    readDstProper(store, eman);

    //------- end of read pram -----------//

    if( PrintMode )
      initializeHistos();

    nevent = 0;
    InitialEvents = 0;
    SkippedEvents = 0;

    first_event=true;

    /*
      if( !SuperNemo )
      {

      run_time = 0.;
      run_list.clear();

      }
    */

    clock.stop(" sequentiator: initialize ");

    return true;
  }

  //*************************************************************
  bool sequentiator::initialize( void ){
    //*************************************************************

    m.message("CAT::sequentiator::initialize: Beginning algorithm sequentiator",mybhep::VERBOSE); fflush(stdout);

    //    clock.start(" sequentiator: initialize ");

    //----------- read dst param -------------//

    readDstProper();

    //------- end of read pram -----------//

    if( PrintMode )
      initializeHistos();

    nevent = 0;
    InitialEvents = 0;
    SkippedEvents = 0;

    first_event=true;

    /*
      if( !SuperNemo )
      {

      run_time = 0.;
      run_list.clear();

      }
    */

    //    clock.stop(" sequentiator: initialize ");

    return true;
  }

  //*************************************************************
  void sequentiator::initializeHistos( void ) {
    //*************************************************************

    /*
      if( PrintMode ){
      hman.h1("chi2_connections", "chi2 for each connection of triplets", 100, -0.05, 20.05);
      hman.h1("prob_connections", "probability that chi2 is larger than observed for a good connection of triplets", 100, -0.05, 1.05);
      hman.h1("chi2_sequence", "chi2 for each sequence", 100, -0.05, 20.05);
      hman.h1("prob_sequence", "probability that chi2 is larger than observed for a sequence", 100, -0.05, 1.05);
      hman.h1("helix_chi2_points", "chi2 for each sequence point to be on a helix", 100, -0.05, 20.05);
      hman.h1("helix_chi2", "chi2 for each sequence to be a helix", 100, -0.05, 20.05);
      hman.h1("helix_prob", "probability that chi2 is larger than observed for a sequence to be a helix", 100, -0.05, 1.05);

      hman.h1("HTTPurity", "track purity ", 21, -0.05, 1.05);
      hman.h1("HTTEfficiency", "track efficiency ", 21, -0.05, 1.05);
      hman.h2("PurityVsTheta", "track purity vs true cos theta", 21, -1., 1., 21, -0.05, 1.05);
      hman.h2("EfficiencyVsTheta", "track efficiency vs true cos theta", 21, -1., 1., 21, -0.05, 1.05);
      hman.h2("QResVsTheta", "charge error vs true cos theta", 21, -1., 1., 5, -2.5, 2.5);
      hman.h1("GGresx", "GG residual in x direction ", 100, -30., 30.);
      hman.h1("GGresy", "GG residual in y direction ", 500, -150., 150.);
      hman.h1("GGresz", "GG residual in z direction ", 100, -30., 30.);
      hman.h1("GGHelixresx", "GG residual in x direction ", 100, -30., 30.);
      hman.h1("GGHelixresy", "GG residual in y direction ", 500, -150., 150.);
      hman.h1("GGHelixresz", "GG residual in z direction ", 100, -30., 30.);
      hman.h1("GGpullx", "GG pull in x direction ", 100, -30., 30.);
      hman.h1("GGpully", "GG pull in y direction ", 100, -30., 30.);
      hman.h1("GGpullz", "GG pull in z direction ", 100, -30., 30.);
      hman.h1("VertexResx", "extrapolation to foil residual in x direction ", 100, -50., 50.);
      hman.h1("VertexResy", "extrapolation to foil residual in y direction ", 100, -50., 50.);
      hman.h1("VertexResz", "extrapolation to foil residual in z direction ", 100, -50., 50.);
      hman.h2("VertexResxVsTheta", "extrapolation to foil residual in x direction vs true cos theta ", 21, -1., 1.,  100, -100., 100.);
      hman.h2("VertexResyVsTheta", "extrapolation to foil residual in y direction vs true cos theta ", 21, -1., 1., 100, -500., 500.);
      hman.h2("VertexReszVsTheta", "extrapolation to foil residual in z direction vs true cos theta ", 21, -1., 1., 100, -100., 100.);
      hman.h1("DVertexResx", "extrapolation to calorimeter residual in x direction ", 100, -70., 70.);
      hman.h1("DVertexResy", "extrapolation to calorimeter residual in y direction ", 100, -70., 70.);
      hman.h1("DVertexResz", "extrapolation to calorimeter residual in z direction ", 100, -70., 70.);
      hman.h2("DVertexResxVsTheta", "extrapolation to calorimeter residual in x direction vs true cos theta ", 21, -1., 1., 100, -500., 500.);
      hman.h2("DVertexResyVsTheta", "extrapolation to calorimeter residual in y direction vs true cos theta ", 21, -1., 1., 100, -2000., 2000.);
      hman.h2("DVertexReszVsTheta", "extrapolation to calorimeter residual in z direction vs true cos theta ", 21, -1., 1., 100, -100., 100.);
      hman.h1("VertexPullx", "extrapolation to foil pull in x direction ", 100, -100., 100.);
      hman.h1("VertexPully", "extrapolation to foil pull in y direction ", 100, -500., 500.);
      hman.h1("VertexPullz", "extrapolation to foil pull in z direction ", 100, -100., 100.);
      hman.h1("DVertexPullx", "extrapolation to calorimeter pull in x direction ", 100, -500., 500.);
      hman.h1("DVertexPully", "extrapolation to calorimeter pull in y direction ", 100, -2000., 2000.);
      hman.h1("DVertexPullz", "extrapolation to calorimeter pull in z direction ", 100, -100., 100.);
      hman.h1("PResx", "momentum residual in x direction ", 20, -5., 5.);
      hman.h1("PResy", "momentum residual in y direction ", 20, -5., 5.);
      hman.h1("PResz", "momentum residual in z direction ", 20, -5., 5.);
      hman.h1("PRes", "momentum residual ", 20, -5., 5.);
      hman.h1("QRes", "charge residual", 5, -2.5, 2.5);
      hman.h1("TrueConnections", "number of true connections", 1, 0., 1.);
      hman.h1("RecoConnections", "number of true connections that were reconstructed", 1, 0., 1.);
      hman.h2("all_point_map", "map of position wrt cell center", 100, -rad/1.9, rad/1.9, 100, -rad/1.9, rad/1.9);
      hman.h2("end_point_map", "map of end-point position wrt cell center", 100, -rad/1.9, rad/1.9, 100, -rad/1.9, rad/1.9);
      }

    */
    return;

  }




  //*************************************************************
  bool sequentiator::finalize() {
    //*************************************************************

    m.message("CAT::sequentiator::finalize: Ending algorithm sequentiator",mybhep::NORMAL); fflush(stdout);

    m.message("CAT::sequentiator::finalize: Initial events: ", InitialEvents, mybhep::NORMAL);
    m.message("CAT::sequentiator::finalize: Skipped events: ", SkippedEvents, "(", 100.*SkippedEvents/InitialEvents, "\%)", mybhep::NORMAL);

    clock.start(" sequentiator: finalize ");

    if( PrintMode )
      finalizeHistos();

    clock.stop(" sequentiator: finalize ");

    print_clocks();

    return true;
  }

  //*************************************************************
  void sequentiator::print_clocks() {
    //*************************************************************

    clock.dump();

    return;
  }

  //*************************************************************
  void sequentiator::finalizeHistos( void ) {
    //*************************************************************

    /*
          if( PrintMode )
          hman.save();
    */

    m.message("CAT::sequentiator::finalizeHistos: Writing histograms in ", hfile, mybhep::NORMAL);

    return;
  }

  //*************************************************************
  void sequentiator::readDstProper(const mybhep::sstore & global, mybhep::EventManager2 */*eman*/) {
    //*************************************************************

    clock.start(" sequentiator: read dst properties ");

    if (! global.find("GEOM_MODULES"))
      {
        _MaxBlockSize = 1;
        m.message("CAT::sequentiator::readDstProper: no bar design, MUDULES Nr set to = ",_MaxBlockSize,"\n",mybhep::VERBOSE);
        fflush(stdout);
      }
    else
      {
        _MaxBlockSize = mybhep::int_from_string(global.fetch("GEOM_MODULES"));
        m.message("CAT::sequentiator::readDstProper: bar design with MUDULES = ",_MaxBlockSize,"\n",mybhep::VERBOSE);
        fflush(stdout);
      }

    if(!global.find("NEMO3"))
      {
        SuperNemo = true;
        m.message("CAT::sequentiator::readDstProper:SuperNemo kind of data",mybhep::VERBOSE); fflush(stdout);
      }
    else
      {
        SuperNemo = false;
        m.message("CAT::sequentiator::readDstProper:Nemo-3 kind of data",mybhep::VERBOSE); fflush(stdout);
      }

    if(global.find("NEMORA_model"))
      {
        NemoraOutput = true;
        m.message("CAT::sequentiator::readDstProper:data already processed by Nemora",mybhep::VERBOSE); fflush(stdout);
      }
    else
      NemoraOutput = false;


    vel=mybhep::double_from_string(global.fetch("N3GG_V_PLASMA"));
    len=mybhep::double_from_string(global.fetch("N3GG_CELL_LENGTH"));
    rad=mybhep::double_from_string(global.fetch("GEOM_GG_CELL_diam"));
    CellDistance = rad;

    num_blocks = mybhep::int_from_string(global.fetch("GEOM_num_blocks"));

    mybhep::vector_from_string(global.fetch("GEOM_planes_per_block"),planes_per_block);

    ysize = mybhep::double_from_string(global.fetch("GEOM_CHAMBER_height"));

    mybhep::vector_from_string(global.fetch("GEOM_gaps_Z"),gaps_Z);

    if( SuperNemo )
      {

        xsize = mybhep::double_from_string(global.fetch("GEOM_CHAMBER_width"));
        zsize = mybhep::double_from_string(global.fetch("GEOM_CHAMBER_length"));



        GG_GRND_diam = mybhep::double_from_string(global.fetch("GEOM_GG_GRND_diam"));

        GG_CELL_diam = rad;

        CHAMBER_X = xsize;

        SOURCE_thick= mybhep::double_from_string(global.fetch("GEOM_SOURCE_thick"));

        bfield = mybhep::double_from_string(global.fetch("GEOM_B_int"))/mybhep::tesla;

        cell_max_number = (int) ( (xsize/2. - GG_GRND_diam) / (GG_CELL_diam*cos(M_PI/8.)) );

      }
    else
      {

        bfield = 0.0025;

        InnerRadius = mybhep::double_from_string(global.fetch("CALO_inner_radius"));
        OuterRadius = mybhep::double_from_string(global.fetch("CALO_outer_radius"));
        FoilRadius = mybhep::double_from_string(global.fetch("GEOM_FOIL_radius"));


        xsize = 2.*OuterRadius;
        zsize = 2.*OuterRadius;
      }

    m.message("CAT::sequentiator::readDstProper: pmax",pmax,"MeV",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: small radius",SmallRadius,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: tangent phi",TangentPhi,mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: tangent theta",TangentTheta,mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: small number",SmallNumber,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: quadrant angle",QuadrantAngle,mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: ratio",Ratio,mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: compatibility distance", CompatibilityDistance,mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: maximum chi2", MaxChi2, mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: maximum time", MaxTime, " ms ", mybhep::NORMAL); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: xsize is read as",xsize,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: ysize is read as",ysize,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: zsize is read as",zsize,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: plasma speed is read as: ",vel,"mm/ns",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: wire length is read as: ",len,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: cell diameter is read as: ",rad,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("CAT::sequentiator::readDstProper: distance from wire to wire is read as: ",CellDistance,"mm",mybhep::VERBOSE); fflush(stdout);

    //parameters for error parametrization

    if( SuperNemo )
      {
        std::string pname;

        pname="N3GG_LONGITUDINAL_SIGMA";
        if(global.find(pname))
          {
            sigma0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_K0";
        if(global.find(pname))
          {
            k0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_K1";
        if(global.find(pname))
          {
            k1=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_K2";
        if(global.find(pname))
          {
            k2=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_K3";
        if(global.find(pname))
          {
            k3=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_TH0";
        if(global.find(pname))
          {
            th0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_TH1";
        if(global.find(pname))
          {
            th1=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_TH2";
        if(global.find(pname))
          {
            th2=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_TH3";
        if(global.find(pname))
          {
            th3=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_L0";
        if(global.find(pname))
          {
            l0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }

        pname="N3GG_L1";
        if(global.find(pname))
          {
            l1=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("CAT::sequentiator::readDstProper: +++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
            clock.stop(" sequentiator: read dst properties ");
            exit(1);
          }
      }


    clock.stop(" sequentiator: read dst properties ");

    return;
  }


  //*************************************************************
  void sequentiator::readDstProper(void) {
    //*************************************************************

    //    clock.start(" sequentiator: read dst properties ");

    if (_MaxBlockSize <= 0)
      {
        _MaxBlockSize = 1;
        m.message("CAT::sequentiator::readDstProper: no bar design, MODULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
      }

    if(SuperNemo)
      {
        m.message("CAT::sequentiator::readDstProper: SuperNemo kind of data",mybhep::NORMAL);
        if (num_blocks <= 0)
          {
            // Default :
            set_num_blocks (1);
            planes_per_block.at (0) = 9;
          }
      }
    else
      {
        m.message("CAT::sequentiator::readDstProper: Nemo-3 kind of data",mybhep::NORMAL);
        bfield = 0.0025;
        if (num_blocks <= 0)
          {
            // Default :
            set_num_blocks (3);
            planes_per_block.at (0) = 4;
            planes_per_block.at (1) = 2;
            planes_per_block.at (2) = 3;
          }
      }


    //    clock.stop(" sequentiator: read dst properties ");

    return;
  }


  //*************************************************************
  bool sequentiator::sequentiate(topology::tracked_data & tracked_data_) {
    //*************************************************************

    event_number ++;
    m.message("CAT::sequentiator::sequentiate: local_tracking: preparing event", event_number, mybhep::VERBOSE);

    if( event_number < first_event_number ){
      m.message("CAT::sequentiator::sequentiate:  local_tracking: skip event", event_number, " first event is "
                , first_event_number,  mybhep::VERBOSE);
      return true;
    }

    clock.start(" sequentiator: sequentiate ","cumulative");
    clock.start(" sequentiator: sequentiation ","restart");

    m.message("CAT::sequentiator::sequentiate: sequentiate... ", mybhep::VVERBOSE); fflush(stdout);

    // set_clusters(tracked_data_.get_clusters());
    vector<topology::cluster> & the_clusters = tracked_data_.get_clusters ();

    NFAMILY = -1;
    NCOPY = 0;

    if (the_clusters.empty ()) return true;

    sequences_.clear();
    scenarios_.clear();

    tracked_data_.scenarios_.clear();

    for (vector<topology::cluster>::iterator
          icluster = the_clusters.begin();
        icluster != the_clusters.end(); ++icluster)
      {
        topology::cluster & a_cluster = *icluster;

        local_cluster_ = &(*icluster);

        sequentiate_cluster(a_cluster);
      }

    if (late())
      {
        tracked_data_.set_skipped(true);
        SkippedEvents ++;
        return false;
      }

    clean_up_sequences();
    direct_out_of_foil();

    interpret_physics(tracked_data_.get_calos());
    make_families();

    match_gaps(tracked_data_.get_calos());
    clean_up_sequences();
    direct_out_of_foil();

    refine_sequences_near_walls(tracked_data_.get_calos());
    interpret_physics(tracked_data_.get_calos());

    if (late())
      {
        tracked_data_.set_skipped(true);
        SkippedEvents ++;
        return false;
      }


    make_scenarios(tracked_data_);


    if (late())
      {
        tracked_data_.set_skipped(true);
        SkippedEvents ++;
        return false;
      }

    // make_plots(tracked_data_);

    m.message("CAT::sequentiator::sequentiate: sequentiation done ", mybhep::VVERBOSE); fflush(stdout);

    clock.stop(" sequentiator: sequentiate ");

    return true;
  }


  //*************************************************************
  bool sequentiator::sequentiate_after_sultan(topology::tracked_data & tracked_data_) {
    //*************************************************************

    event_number ++;
    m.message("CAT::sequentiator::sequentiate_after_sultan: local_tracking: preparing event", event_number, mybhep::VERBOSE);

    if( event_number < first_event_number ){
      m.message("CAT::sequentiator::sequentiate_after_sultan:  local_tracking: skip event", event_number, " first event is "
                , first_event_number,  mybhep::VERBOSE);
      return true;
    }

    clock.start(" sequentiator: sequentiate_after_sultan ","cumulative");
    clock.start(" sequentiator: sequentiation ","restart");

    // set_clusters(tracked_data_.get_clusters());
    vector<topology::cluster> & the_clusters = tracked_data_.get_clusters ();

    m.message("CAT::sequentiator::sequentiate_after_sultan: sequentiate ", the_clusters.size(), " clusters ", mybhep::VVERBOSE); fflush(stdout);

    NFAMILY = 0;
    NCOPY = 0;

    if (the_clusters.empty ()) return true;

    sequences_.clear();
    scenarios_.clear();

    tracked_data_.scenarios_.clear();

    for (vector<topology::cluster>::iterator
          icluster = the_clusters.begin();
        icluster != the_clusters.end(); ++icluster)
      {
        local_cluster_ = &(*icluster);

        sequentiate_cluster_after_sultan();
      }

    if (late())
      {
        tracked_data_.set_skipped(true);
        SkippedEvents ++;
        return false;
      }

    clean_up_sequences();
    direct_out_of_foil();

    interpret_physics(tracked_data_.get_calos());
    make_families();

    refine_sequences_near_walls(tracked_data_.get_calos());

    if (late())
      {
        tracked_data_.set_skipped(true);
        SkippedEvents ++;
        return false;
      }


    make_scenarios(tracked_data_);


    if (late())
      {
        tracked_data_.set_skipped(true);
        SkippedEvents ++;
        return false;
      }

    // make_plots(tracked_data_);

    clock.stop(" sequentiator: sequentiate_after_sultan ");

    return true;
  }


  //*************************************************************
  void sequentiator::plot_hard_scattering(topology::tracked_data /*__tracked_data*/){
    //*************************************************************

    /*
      if( PrintMode ){
      if( !__tracked_data.scenarios_.empty() ){
      for(std::vector<topology::sequence>::iterator isequence = __tracked_data.scenarios_[0].sequences_.begin(); isequence != __tracked_data.scenarios_[0].sequences_.end(); ++isequence){

      for(std::vector<topology::node>::iterator in = isequence->nodes_.begin(); in != isequence->nodes_.end(); ++in){
      topology::experimental_vector dist(in->c().ep(), in->ep());
      hman.fill("all_point_map", dist.z().value(), dist.x().value());
      }

      if( !isequence->has_decay_vertex() ){
      // if the last cell is not on a gap
      if( gap_number(isequence->last_node().c()) < 0 ){
      m.message(" sequence ", isequence->name(), "has last cell ", isequence->last_node().c().id(), " disconnected", mybhep::NORMAL);

      for(std::vector<topology::sequence>::iterator jsequence = __tracked_data.scenarios_[0].sequences_.begin(); jsequence != __tracked_data.scenarios_[0].sequences_.end(); ++jsequence){
      if( isequence == jsequence ) continue;
      if( jsequence->nodes_[0].c().id() != isequence->last_node().c().id() &&
      jsequence->last_node().c().id() != isequence->last_node().c().id()) continue;

      m.message(" .. and in common with sequence ", jsequence->name(), mybhep::NORMAL);

      if( isequence->family() == jsequence->family() ) continue;

      m.message(" .... will plot ", mybhep::NORMAL);

      topology::experimental_vector dist(isequence->last_node().c().ep(), isequence->last_node().ep());
      hman.fill("end_point_map", dist.z().value(), dist.x().value());
      }

      }
      }

      }
      }
      }


    */
    return;
  }




  //*************************************************************
  void sequentiator::make_plots(topology::tracked_data /*__tracked_data*/){
    //*************************************************************

    /*
      if( PrintMode ){

      clock.start(" sequentiator: reconstruct efficiency ","cumulative");
      rec_efficiency(__tracked_data.get_true_sequences());
      clock.stop(" sequentiator: reconstruct efficiency ");

      plot_hard_scattering(__tracked_data);

      for(std::vector<topology::sequence>::iterator isequence = sequences_.begin(); isequence != sequences_.end(); ++isequence){

      std::vector<double> chi2s = isequence->chi2s_all();
      for(size_t ichi=0; ichi<chi2s.size(); ichi ++){
      hman.fill("chi2_connections", chi2s[ichi]);
      }
      std::vector<double> probs = isequence->probs_all();
      for(size_t iprob=0; iprob<probs.size(); iprob ++){
      hman.fill("prob_connections", probs[iprob]);
      }

      hman.fill("chi2_sequence", isequence->chi2());
      hman.fill("prob_sequence", isequence->Prob());

      std::vector<double> hchi2s = isequence->helix_chi2s_all();
      for(size_t ichi=0; ichi<hchi2s.size(); ichi ++){
      hman.fill("helix_chi2_points", hchi2s[ichi]);
      }

      hman.fill("helix_chi2", isequence->helix_chi2());
      hman.fill("helix_prob", isequence->helix_Prob());


      }


      }

    */

    return;

  }

  //*************************************************************
  void sequentiator::sequentiate_cluster(topology::cluster & cluster_) {
    //*************************************************************

    for (vector<topology::node>::iterator
          inode = cluster_.nodes_.begin();
        inode != cluster_.nodes_.end(); ++inode)
      {
        topology::node & a_node = *inode;

        if (level >= mybhep::VVERBOSE)
          {
            clog << "CAT::sequentiator::sequentiate_cluster: first node: " << inode->c().id();
          }

        if (!good_first_node(a_node)) continue;

        make_new_sequence(a_node);

        if (late()) return;

        make_copy_sequence(a_node);
      }

    return;
  }


  //*************************************************************
  void sequentiator::sequentiate_cluster_after_sultan(){
    //*************************************************************

    make_new_sequence_after_sultan();
    
    if (late()) return;
    
    //make_copy_sequence_after_sultan();

    return;
  }


  //*************************************************************
  void sequentiator::make_new_sequence(topology::node & first_node){
    //*************************************************************

    if (late()) return;

    clock.start(" sequentiator: make new sequence ","cumulative");

    //  A node is added to the newsequence. It has the given cell but no other
    //  requirement. The free level is set to true.
    topology::sequence newsequence(first_node, level, probmin);

    if (level >= mybhep::VERBOSE)
      {
        clog << "CAT::sequentiator::make_new_sequence: begin track [" << sequences_.size() << "] with node " << first_node.c().id() << endl;
        print_a_sequence(newsequence);
      }

    bool updated = true;

    while (updated)
      {
        updated = evolve(newsequence);
      }

    if (late()) return;

    if (level >= mybhep::VERBOSE)
      print_a_sequence(newsequence);

    NFAMILY++;
    NCOPY = 0;
    if (newsequence.nodes().size() != 2)
      {
        make_name(newsequence);

        sequences_.push_back(newsequence);
        m.message("CAT::sequentiator::make_new_sequence:  finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
        clean_up_sequences();
      }
    else
      {
        add_pair(newsequence);
      }

    clock.stop(" sequentiator: make new sequence ");

    return;
  }


  //*************************************************************
  topology::joint sequentiator::find_best_matching_joint(topology::joint j, std::vector<topology::joint> js, topology::cell A, topology::cell B, double *chi2){
  //*************************************************************

    // match joint 0-A-B and joint A-B-C
    double local_chi2;
    double chi2_best = mybhep::default_min;
    topology::joint jbest, lj;

    for(std::vector<topology::joint>::const_iterator ij=js.begin(); ij!=js.end(); ++ij){
      local_chi2 = ij->calculate_chi2(j, A, B, &lj);
      if( local_chi2 < chi2_best ){
	chi2_best = local_chi2;
	jbest = lj;
      }
    }

    *chi2 = chi2_best;
    return jbest;

  }

  //*************************************************************
  void sequentiator::make_new_sequence_after_sultan(){
    //*************************************************************

    if (late()) return;

    clock.start(" sequentiator: make new sequence after sultan ","cumulative");

    size_t s = local_cluster_->nodes().size();
    NFAMILY = 0;
    NCOPY = 0;

    if (level >= mybhep::VERBOSE){
      std::clog << " CAT::sequentiator::make_new_sequence_after_sultan: cluster size is " <<  s ;
      for(std::vector<topology::node>::const_iterator in=local_cluster_->nodes_.begin(); in!=local_cluster_->nodes_.end(); ++in)
	std::clog << " [" << in->c().id() << "]";
      std::clog << " " << std::endl;
    }

    if( s == 0 ){
      m.message(" problem: CAT::sequentiator::make_new_sequence_after_sultan: cluster size is ", s, mybhep::NORMAL);
      return;
    }

    if( s == 1 ){
      topology::node n = local_cluster_->nodes()[0];
      n.set_ep(n.c().ep());
      topology::sequence newsequence(n, level, probmin);
      make_name(newsequence);
      sequences_.push_back(newsequence);

      if (level >= mybhep::VERBOSE)
	{
	  clog << "CAT::sequentiator::make_new_sequence_after_sultan: made sequence " << endl;
	  print_a_sequence(newsequence);
	}

      clean_up_sequences();
      return;
    }

    if( s == 2 ){
      topology::sequence newsequence(local_cluster_->nodes(), level, probmin);
      add_pair(newsequence);
      
      clean_up_sequences();
      return;
    }

    if( s == 3 ){
      std::vector<topology::joint> joints = local_cluster_->nodes()[1].ccc()[0].joints();
      m.message(" CAT::sequentiator::make_new_sequence_after_sultan: make sequence with size ", s, " and ", joints.size(), " joints ", mybhep::VERBOSE);
      for(std::vector<topology::joint>::const_iterator ij=joints.begin(); ij!=joints.end(); ++ij){
	topology::sequence newsequence(local_cluster_->nodes(), level, probmin);
	newsequence.nodes_[0].set_ep(ij->epa());
	newsequence.nodes_[1].set_ep(ij->epb());
	newsequence.nodes_[2].set_ep(ij->epc());
	make_name(newsequence);
	sequences_.push_back(newsequence);
	if (level >= mybhep::VERBOSE)
	  {
	    clog << "CAT::sequentiator::make_new_sequence_after_sultan: made sequence " << endl;
	    print_a_sequence(newsequence);
	  }
	NFAMILY ++;
      }
      
      clean_up_sequences();
      return;
    }

    
    // make 1 or 2 sequences
    // cluster = A B C D ... X Y Z
    topology::sequence newsequence(local_cluster_->nodes(), level, probmin);
    std::vector<topology::joint> joints, local_joints;
    std::vector<double> chi2s;
    topology::joint best_joint, local_joint;
    topology::cell prev_cell, next_cell;


    // first, work out which gaps are being crossed
    // joint at node: you give it the node index, it gives you back the joint index
    std::map<size_t, size_t > joint_at_node;
    std::vector<size_t> index_of_nodes_on_gap;
    for(std::vector<topology::node>::const_iterator inode=local_cluster_->nodes_.begin() + 2; inode!=local_cluster_->nodes_.end()-1; ++inode){
      next_cell = local_cluster_->nodes_[inode - local_cluster_->nodes_.begin() + 1].c();
      if( inode->c().block() != next_cell.block()){
	index_of_nodes_on_gap.push_back(inode - local_cluster_->nodes_.begin());
	index_of_nodes_on_gap.push_back(inode - local_cluster_->nodes_.begin() + 1);
	if( inode->ccc()[0].joints().size() > 1 ){
	  joint_at_node[inode - local_cluster_->nodes_.begin()] = 0;
	}
      }
    }


    bool there_are_inexplored_gaps_permutations = true;
    while( there_are_inexplored_gaps_permutations ){

      if (level >= mybhep::VVERBOSE)
	{
	  std::clog << " gaps permutations: ";
	  for( std::map<size_t, size_t >::const_iterator im=joint_at_node.begin(); im!=joint_at_node.end(); ++im){
	    std::clog << "[" << local_cluster_->nodes_[im->first].c().id() << "] " << im->second ;
	  }
	  std::clog << " " << std::endl;
	}

      // find first node with a 1
      size_t i_first_node_with_1 = s;
      for(size_t i=0; i<s; i++){
	// skip nodes not at gap or with 1 joint
	if( joint_at_node.count(i) == 0 ) continue;

	if( joint_at_node[i] == 1 ){
	  i_first_node_with_1 = i;
	  break;
	}
      }


      // find last node with a 0 after the first node with a 1
      size_t i_last_node_with_0 = s;
      for(size_t i=s-1; i > i_first_node_with_1; i--){
	// skip nodes not at gap or with 1 joint
	if( joint_at_node.count(i) == 0 ) continue;

	if( joint_at_node[i] == 0 ){
	  i_last_node_with_0 = i;
	  break;
	}
      }

     if (level >= mybhep::VVERBOSE)
       {
	 if( i_first_node_with_1 < s ){
	   std::clog << " first_node_with_1: " << local_cluster_->nodes_[i_first_node_with_1].c().id();
	 }else{
	   std::clog << " i_first_node_with_1 = s " << std::endl;
	 }
	 if( i_last_node_with_0 < s ){
	   std::clog << " last_node_with_0: " << local_cluster_->nodes_[i_last_node_with_0].c().id();
	 }else{
	   std::clog << " i_last_node_with_0 = s " << std::endl;
	 }
       }
     
     // first_joints A-B-C
     std::vector<topology::joint> first_joints = local_cluster_->nodes()[1].ccc()[0].joints();
     double chi2;
     for(std::vector<topology::joint>::const_iterator first_j=first_joints.begin(); first_j!=first_joints.end(); ++first_j){
       m.message(" CAT::sequentiator::make_new_sequence_after_sultan: node ", local_cluster_->nodes()[1].c().id(), " has ", first_joints.size(), " joints, pick the one with index", first_j - first_joints.begin(), mybhep::VERBOSE);
       joints.clear();
       chi2s.clear();
       
       joints.push_back(*first_j);
       chi2s.push_back(0.);
       chi2s.push_back(0.);
       
       // look at nodes C D ... X Y
       for(std::vector<topology::node>::const_iterator inode=local_cluster_->nodes_.begin() + 2; inode!=local_cluster_->nodes_.end()-1; ++inode){
	 prev_cell = local_cluster_->nodes_[inode - local_cluster_->nodes_.begin() - 1].c();

	 bool node_is_on_gap = (std::find(index_of_nodes_on_gap.begin(), index_of_nodes_on_gap.end(), inode - local_cluster_->nodes_.begin()) != index_of_nodes_on_gap.end());

	 if( joint_at_node.count(inode - local_cluster_->nodes_.begin()) == 0 ){ // node is not on gap or has only 1 joint
	   local_joints = inode->ccc()[0].joints();
	   m.message(" CAT::sequentiator::make_new_sequence_after_sultan: node ", inode->c().id(), " has ", local_joints.size(), " joints, is on gap:", node_is_on_gap, mybhep::VERBOSE);
	   best_joint = find_best_matching_joint(joints.back(), local_joints, prev_cell, inode->c(), &chi2);
	   joints.push_back(best_joint);
	   if( !node_is_on_gap )
	     chi2s.push_back(chi2);
	   else
	     chi2s.push_back(0.);
	 }else{ // node is on gap and has 2 joints
	   size_t index_of_local_joint = joint_at_node[inode - local_cluster_->nodes_.begin()];
	   m.message(" CAT::sequentiator::make_new_sequence_after_sultan: node ", inode->c().id(), " is on gap, pick joint ", index_of_local_joint, ", is on gap:", node_is_on_gap, mybhep::VERBOSE);
	   local_joint = inode->ccc()[0].joints()[index_of_local_joint];
	   // local joint:    A B C
	   // joints back:    0 A B
	   chi2 = local_joint.calculate_chi2(joints.back(), prev_cell, inode->c(), &best_joint);
	   joints.push_back(best_joint);
	   chi2s.push_back(0.);
	 }

       }

       chi2s.push_back(0.);
       
       newsequence.nodes_[0].set_ep(joints.front().epa());
       newsequence.nodes_[0].set_chi2(chi2s[0]);
       for(size_t i=1; i<s-1; i++){
	 newsequence.nodes_[i].set_ep(joints[i-1].epb());
	 newsequence.nodes_[i].set_chi2(chi2s[i]);
       }
       newsequence.nodes_[s-1].set_ep(joints.back().epc());
       newsequence.nodes_[s-1].set_chi2(chi2s[s-1]);
       make_name(newsequence);
       sequences_.push_back(newsequence);
       if (level >= mybhep::VERBOSE)
	 {
	   clog << "CAT::sequentiator::make_new_sequence_after_sultan: made sequence " << endl;
	   print_a_sequence(newsequence);
	 }
       NFAMILY ++;
     }

     // update permutations of gaps
     m.message(" CAT::sequentiator::make_new_sequence_after_sultan: updating gaps permutations, i_first_node_with_1", i_first_node_with_1, " i_last_node_with_0 ", i_last_node_with_0, " s ", s, mybhep::VERBOSE);
     there_are_inexplored_gaps_permutations = false;
     if( joint_at_node.size() == 0 ){
       there_are_inexplored_gaps_permutations = false;
     }else if( i_first_node_with_1 == s ){ // (0, ..., 0)
       for(size_t i=s-1; i >= 0; i--){
	 // skip nodes not at gap or with 1 joint
	 if( joint_at_node.count(i) == 0 ) continue;
	 joint_at_node[i] = 1;
	 there_are_inexplored_gaps_permutations = true;
	 break;
       }
     }else{
       if( i_last_node_with_0 == s ){ // (0, ..., 0, 1, ..., 1)
	 // is there a 0 before the 1st 1?
	 bool there_is = false;
	 for(size_t i=0; i<i_first_node_with_1; i++){
	   // skip nodes not at gap or with 1 joint
	   if( joint_at_node.count(i) == 0 ) continue;
	   there_is = true;
	   break;
	 }
	 if( !there_is ){ // (1, ..., 1)
	   there_are_inexplored_gaps_permutations = false;
	 }else{
	   for(size_t i=i_first_node_with_1; i<s; i++){
	     // skip nodes not at gap or with 1 joint
	     if( joint_at_node.count(i) == 0 ) continue;
	     joint_at_node[i] = 0;
	   }
	   for(size_t i=i_first_node_with_1-1; i >= 0; i--){
	     // skip nodes not at gap or with 1 joint
	     if( joint_at_node.count(i) == 0 ) continue;
	     joint_at_node[i] = 1;
	     there_are_inexplored_gaps_permutations = true;
	     break;
	   }
	 }
       }else{ // (0, ..., 0, 1, 0, 0, 1, 1)

	 for(size_t i=i_last_node_with_0 + 1; i<s; i++){
	   // skip nodes not at gap or with 1 joint
	   if( joint_at_node.count(i) == 0 ) continue;
	   joint_at_node[i] = 0;
	 }
	 joint_at_node[i_last_node_with_0] = 1;
	 there_are_inexplored_gaps_permutations = true;
       }
     }
    
     if (level >= mybhep::VVERBOSE)
       {
	  std::clog << " gaps permutations: ";
	  for( std::map<size_t, size_t >::const_iterator im=joint_at_node.begin(); im!=joint_at_node.end(); ++im){
	    std::clog << "[" << local_cluster_->nodes_[im->first].c().id() << "] " << im->second ;
	  }
	  std::clog << " there_are_inexplored_gaps_permutations " << there_are_inexplored_gaps_permutations << std::endl;
       }
     
 
    }
    clean_up_sequences();

    clock.stop(" sequentiator: make new sequence after sultan ");

    return;
  }


  //*************************************************************
  void sequentiator::make_name(topology::sequence & sequence_) {
    //*************************************************************

    std::string number = mybhep::to_string(NFAMILY)+"_"+mybhep::to_string(NCOPY);
    std::string name = "track_"+number;
    sequence_.set_name(name);

    return;

  }

  //*************************************************************
  void sequentiator::make_copy_sequence(topology::node & first_node){
    //*************************************************************

    const bool local_devel = false;
    if (local_devel)
      {
        level = mybhep::VVERBOSE;
        clog << "DEVEL: " << "CAT::sequentiator::make_copy_sequence: "
             << "Entering..." << endl;
      }

    clock.start(" sequentiator: make copy sequence ","cumulative");

    size_t isequence;
    while (there_is_free_sequence_beginning_with(first_node.c(), &isequence))
      {
        if (late()) return;

        clock.start(" sequentiator: make copy sequence: part A ","cumulative");
        clock.start(" sequentiator: make copy sequence: part A: alpha ","cumulative");

        m.message("CAT::sequentiator::make_copy_sequence: begin, with cell", first_node.c().id(), ", parallel track ", sequences_.size(), " to track ", isequence, mybhep::VERBOSE); fflush(stdout);

        if (level >= mybhep::VVERBOSE)
          {
            std::clog << "CAT::sequentiator::make_copy_sequence: original sequence before copying: " << std::endl;
            print_a_sequence(sequences_[isequence]);
          }

        clock.stop(" sequentiator: make copy sequence: part A: alpha ");
        clock.start(" sequentiator: copy to lfn ","cumulative");
        size_t ilink, ilfn;
        topology::sequence newcopy = sequences_[isequence].copy_to_last_free_node(&ilfn, &ilink);
        clock.stop(" sequentiator: copy to lfn ");

        clock.start(" sequentiator: make copy sequence: part A: beta ","cumulative");
        m.message("CAT::sequentiator::make_copy_sequence: copied from sequence  ", isequence, mybhep::VVERBOSE); fflush(stdout);

        if (level >= mybhep::VVERBOSE)
          {
            m.message("CAT::sequentiator::make_copy_sequence: original sequence after copy ", mybhep::VVERBOSE); fflush(stdout);
            print_a_sequence(sequences_[isequence]);
            m.message("CAT::sequentiator::make_copy_sequence: new copy ", mybhep::VVERBOSE); fflush(stdout);
            print_a_sequence(newcopy);
          }

        clock.stop(" sequentiator: make copy sequence: part A: beta ");
        clock.start(" sequentiator: make copy sequence: evolve ","cumulative");

        bool updated = true;
        while (updated)
          updated = evolve(newcopy);
        clock.stop(" sequentiator: make copy sequence: evolve ");

        if (late()) return;

        if(level >= mybhep::VVERBOSE)
          print_a_sequence(newcopy);

        clock.stop(" sequentiator: make copy sequence: part A ");
        clock.start(" sequentiator: manage copy sequence ","cumulative");

        if (local_devel)
          {
            clog << "DEVEL: " << "CAT::sequentiator::make_copy_sequence: "
                 << "Checking study case for sequence #" << isequence << " "
                 << "and node #" << first_node.c ().id () << endl;
          }

        // not adding: case 1: new sequence did not evolve
        if (newcopy.nodes().size() == ilfn + 1)
          {
            m.message("CAT::sequentiator::make_copy_sequence: not adding new sequence, since it couldn't evolve past lfn ", mybhep::VERBOSE); fflush(stdout);
            clean_up_sequences();
          }
        else
          {
            // if copy has evolved past level ilfn, the link from node ilfn it used
            // is set to used in the original
            if( newcopy.nodes().size() > ilfn + 1 ){
              if( !sequences_[isequence].nodes().empty() ){
                clock.start(" sequentiator: get link index ","cumulative");
                size_t it1 = newcopy.get_link_index_of_cell(ilfn, newcopy.nodes()[ilfn + 1].c());
                clock.stop(" sequentiator: get link index ");
                m.message("CAT::sequentiator::make_copy_sequence: setting as used original node ", ilfn, "  cc ", it1, mybhep::VVERBOSE);
                if( ilfn == 0 )
                  sequences_[isequence].nodes_[ilfn].cc_[it1].set_all_used();
                else
                  sequences_[isequence].nodes_[ilfn].ccc_[it1].set_all_used();
              }
              /*
                if( sequences_[isequence].nodes().size() > 1 && ilfn > 0){
                clock.start(" sequentiator: get link index ","cumulative");
                size_t it2 = newcopy.get_link_index_of_cell(1, newcopy.nodes()[2].c());
                clock.stop(" sequentiator: get link index ");
                m.message(" setting as used original node 1  ccc ", it2, mybhep::VVERBOSE);
                sequences_[isequence].nodes_[1].ccc_[it2].set_all_used();
                }
              */
              clock.start(" sequentiator: set free level ","cumulative");
              sequences_[isequence].set_free_level();
              clock.stop(" sequentiator: set free level ");
            }

          // not adding: case 2: new sequence contained
          if (newcopy.contained(sequences_[isequence]) && !newcopy.Free())  // new copy is contained in original
            {
              m.message("CAT::sequentiator::make_copy_sequence: not adding new sequence, contained in ", isequence, "from which it was copied", mybhep::VERBOSE); fflush(stdout);
              clean_up_sequences();
            }
          else
            { // adding: case 3
              if (sequences_[isequence].contained(newcopy))
                { // original is contained in new copy
                  for (size_t k=0; k<ilfn; k++)
                    {
                      newcopy.nodes_[k].set_free( sequences_[isequence].nodes()[k].free());

                      for (vector<topology::cell_couplet>::iterator icc = sequences_[isequence].nodes_[k].cc_.begin();
                          icc != sequences_[isequence].nodes_[k].cc_.end(); ++icc)
                        {
                          newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].set_free( icc->free());
                          newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].set_begun( icc->begun());

                          for(vector<topology::line>::iterator itang = sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.begin(); itang != sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.end(); ++itang)
                            newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_[itang - sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.begin()].set_used(itang->used() );

                        }

                      for (vector<topology::cell_triplet>::iterator iccc = sequences_[isequence].nodes_[k].ccc_.begin();
                           iccc != sequences_[isequence].nodes_[k].ccc_.end(); ++iccc)
                        {
                          newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].set_free( iccc->free());
                          newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].set_begun( iccc->begun());

                          for (vector<topology::joint>::iterator ijoint = sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.begin(); ijoint != sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.end(); ++ijoint)
                            newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_[ijoint - sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.begin()].set_used(ijoint->used() );

                        }
                    }

                  if (ilfn < 2)
                    {
                      for (vector<topology::cell_couplet>::iterator icc = sequences_[isequence].nodes_[ilfn].cc_.begin();
                           icc != sequences_[isequence].nodes_[ilfn].cc_.end(); ++icc)
                        if ((size_t)(icc - sequences_[isequence].nodes_[ilfn].cc_.begin()) != ilink)
                          {
                            newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].set_free( icc->free());
                            newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].set_begun( icc->begun());

                            for (vector<topology::line>::iterator itang = sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.begin(); itang !=sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.end(); ++itang)
                              newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_[itang - sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.begin()].set_used(itang->used() );
                          }
                    }
                  else
                    {
                      for (vector<topology::cell_triplet>::iterator iccc = sequences_[isequence].nodes_[ilfn].ccc_.begin();
                          iccc != sequences_[isequence].nodes_[ilfn].ccc_.end(); ++iccc)
                        if ((size_t)(iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()) != ilink )
                          {
                            newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].set_free( iccc->free());
                            newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].set_begun( iccc->begun());

                            for (vector<topology::joint>::iterator ijoint = sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.begin(); ijoint !=sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.end(); ++ijoint)
                              newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_[ijoint - sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.begin()].set_used(ijoint->used() );
                          }
                    }

                  clock.start(" sequentiator: set free level ","cumulative");
                  newcopy.set_free_level();
                  clock.stop(" sequentiator: set free level ");

                  sequences_.erase(sequences_.begin()+isequence);
                  m.message("CAT::sequentiator::make_copy_sequence: erased original sequence ", isequence, "contained in sequence", sequences_.size()+1, "which was copied from it", mybhep::VERBOSE); fflush(stdout);
                  clean_up_sequences();

                }

              NCOPY++;
              if (newcopy.nodes().size() != 2)
                {
                  make_name(newcopy);
                  sequences_.push_back( newcopy );
                  m.message("CAT::sequentiator::make_copy_sequence: finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
                  clean_up_sequences();
                }
              else
                {
                  add_pair(newcopy);
                }
            }// end of case 3
          }

        clock.stop(" sequentiator: manage copy sequence ");
      }

    NCOPY = 0;

    clock.stop(" sequentiator: make copy sequence ");

    return;
  }

  //*************************************************************
  void sequentiator::make_copy_sequence_after_sultan(){
    //*************************************************************

    const bool local_devel = false;
    topology::node first_node = local_cluster_->nodes()[0];
    if (local_devel)
      {
        level = mybhep::VVERBOSE;
        clog << "DEVEL: " << "CAT::sequentiator::make_copy_sequence_after_sultan: "
             << "Entering..." << endl;
      }

    clock.start(" sequentiator: make copy sequence after sultan ","cumulative");

    size_t isequence;
    while (there_is_free_sequence_beginning_with(first_node.c(), &isequence))
      {
        if (late()) return;

        clock.start(" sequentiator: make copy sequence after sultan: part A ","cumulative");
        clock.start(" sequentiator: make copy sequence after sultan: part A: alpha ","cumulative");

        m.message("CAT::sequentiator::make_copy_sequence_after_sultan: begin, with cell", first_node.c().id(), ", parallel track ", sequences_.size(), " to track ", isequence, mybhep::VERBOSE); fflush(stdout);

        if (level >= mybhep::VVERBOSE)
          {
            std::clog << "CAT::sequentiator::make_copy_sequence_after_sultan: original sequence before copying: " << std::endl;
            print_a_sequence(sequences_[isequence]);
          }

        clock.stop(" sequentiator: make copy sequence after sultan: part A: alpha ");
        clock.start(" sequentiator: copy to lfn ","cumulative");
        size_t ilink, ilfn;
        topology::sequence newcopy = sequences_[isequence].copy_to_last_free_node(&ilfn, &ilink);
        clock.stop(" sequentiator: copy to lfn ");

        clock.start(" sequentiator: make copy sequence after sultan: part A: beta ","cumulative");
        m.message("CAT::sequentiator::make_copy_sequence_after_sultan: copied from sequence  ", isequence, mybhep::VVERBOSE); fflush(stdout);

        if (level >= mybhep::VVERBOSE)
          {
            m.message("CAT::sequentiator::make_copy_sequence_after_sultan: original sequence after copy ", mybhep::VVERBOSE); fflush(stdout);
            print_a_sequence(sequences_[isequence]);
            m.message("CAT::sequentiator::make_copy_sequence_after_sultan: new copy ", mybhep::VVERBOSE); fflush(stdout);
            print_a_sequence(newcopy);
          }

        clock.stop(" sequentiator: make copy sequence after sultan: part A: beta ");
        clock.start(" sequentiator: make copy sequence after sultan: evolve ","cumulative");

        bool updated = true;
        while (updated)
          updated = evolve(newcopy);
        clock.stop(" sequentiator: make copy sequence after sultan: evolve ");

        if (late()) return;

        if(level >= mybhep::VVERBOSE)
          print_a_sequence(newcopy);

        clock.stop(" sequentiator: make copy sequence after sultan: part A ");
        clock.start(" sequentiator: manage copy sequence after sultan ","cumulative");

        if (local_devel)
          {
            clog << "DEVEL: " << "CAT::sequentiator::make_copy_sequence_after_sultan: "
                 << "Checking study case for sequence #" << isequence << " "
                 << "and node #" << first_node.c ().id () << endl;
          }

        // not adding: case 1: new sequence did not evolve
        if (newcopy.nodes().size() == ilfn + 1)
          {
            m.message("CAT::sequentiator::make_copy_sequence_after_sultan: not adding new sequence, since it couldn't evolve past lfn ", mybhep::VERBOSE); fflush(stdout);
            clean_up_sequences();
          }
        else
          {
            // if copy has evolved past level ilfn, the link from node ilfn it used
            // is set to used in the original
            if( newcopy.nodes().size() > ilfn + 1 ){
              if( !sequences_[isequence].nodes().empty() ){
                clock.start(" sequentiator: get link index ","cumulative");
                size_t it1 = newcopy.get_link_index_of_cell(ilfn, newcopy.nodes()[ilfn + 1].c());
                clock.stop(" sequentiator: get link index ");
                m.message("CAT::sequentiator::make_copy_sequence_after_sultan: setting as used original node ", ilfn, "  cc ", it1, mybhep::VVERBOSE);
                if( ilfn == 0 )
                  sequences_[isequence].nodes_[ilfn].cc_[it1].set_all_used();
                else
                  sequences_[isequence].nodes_[ilfn].ccc_[it1].set_all_used();
              }
              /*
                if( sequences_[isequence].nodes().size() > 1 && ilfn > 0){
                clock.start(" sequentiator: get link index ","cumulative");
                size_t it2 = newcopy.get_link_index_of_cell(1, newcopy.nodes()[2].c());
                clock.stop(" sequentiator: get link index ");
                m.message(" setting as used original node 1  ccc ", it2, mybhep::VVERBOSE);
                sequences_[isequence].nodes_[1].ccc_[it2].set_all_used();
                }
              */
              clock.start(" sequentiator: set free level ","cumulative");
              sequences_[isequence].set_free_level();
              clock.stop(" sequentiator: set free level ");
            }

          // not adding: case 2: new sequence contained
          if (newcopy.contained(sequences_[isequence]) && !newcopy.Free())  // new copy is contained in original
            {
              m.message("CAT::sequentiator::make_copy_sequence_after_sultan: not adding new sequence, contained in ", isequence, "from which it was copied", mybhep::VERBOSE); fflush(stdout);
              clean_up_sequences();
            }
          else
            { // adding: case 3
              if (sequences_[isequence].contained(newcopy))
                { // original is contained in new copy
                  for (size_t k=0; k<ilfn; k++)
                    {
                      newcopy.nodes_[k].set_free( sequences_[isequence].nodes()[k].free());

                      for (vector<topology::cell_couplet>::iterator icc = sequences_[isequence].nodes_[k].cc_.begin();
                          icc != sequences_[isequence].nodes_[k].cc_.end(); ++icc)
                        {
                          newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].set_free( icc->free());
                          newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].set_begun( icc->begun());

                          for(vector<topology::line>::iterator itang = sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.begin(); itang != sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.end(); ++itang)
                            newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_[itang - sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.begin()].set_used(itang->used() );

                        }

                      for (vector<topology::cell_triplet>::iterator iccc = sequences_[isequence].nodes_[k].ccc_.begin();
                           iccc != sequences_[isequence].nodes_[k].ccc_.end(); ++iccc)
                        {
                          newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].set_free( iccc->free());
                          newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].set_begun( iccc->begun());

                          for (vector<topology::joint>::iterator ijoint = sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.begin(); ijoint != sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.end(); ++ijoint)
                            newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_[ijoint - sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.begin()].set_used(ijoint->used() );

                        }
                    }

                  if (ilfn < 2)
                    {
                      for (vector<topology::cell_couplet>::iterator icc = sequences_[isequence].nodes_[ilfn].cc_.begin();
                           icc != sequences_[isequence].nodes_[ilfn].cc_.end(); ++icc)
                        if ((size_t)(icc - sequences_[isequence].nodes_[ilfn].cc_.begin()) != ilink)
                          {
                            newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].set_free( icc->free());
                            newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].set_begun( icc->begun());

                            for (vector<topology::line>::iterator itang = sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.begin(); itang !=sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.end(); ++itang)
                              newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_[itang - sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.begin()].set_used(itang->used() );
                          }
                    }
                  else
                    {
                      for (vector<topology::cell_triplet>::iterator iccc = sequences_[isequence].nodes_[ilfn].ccc_.begin();
                          iccc != sequences_[isequence].nodes_[ilfn].ccc_.end(); ++iccc)
                        if ((size_t)(iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()) != ilink )
                          {
                            newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].set_free( iccc->free());
                            newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].set_begun( iccc->begun());

                            for (vector<topology::joint>::iterator ijoint = sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.begin(); ijoint !=sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.end(); ++ijoint)
                              newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_[ijoint - sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.begin()].set_used(ijoint->used() );
                          }
                    }

                  clock.start(" sequentiator: set free level ","cumulative");
                  newcopy.set_free_level();
                  clock.stop(" sequentiator: set free level ");

                  sequences_.erase(sequences_.begin()+isequence);
                  m.message("CAT::sequentiator::make_copy_sequence_after_sultan: erased original sequence ", isequence, "contained in sequence", sequences_.size()+1, "which was copied from it", mybhep::VERBOSE); fflush(stdout);
                  clean_up_sequences();

                }

              NCOPY++;
              if (newcopy.nodes().size() != 2)
                {
                  make_name(newcopy);
                  sequences_.push_back( newcopy );
                  m.message("CAT::sequentiator::make_copy_sequence_after_sultan: finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
                  clean_up_sequences();
                }
              else
                {
                  add_pair(newcopy);
                }
            }// end of case 3
          }

        clock.stop(" sequentiator: manage copy sequence after sultan ");
      }

    NCOPY = 0;

    clock.stop(" sequentiator: make copy sequence after sultan ");

    return;
  }

  //*************************************************************
  bool sequentiator::late(void){
    //*************************************************************

    if( clock.read(" sequentiator: sequentiation ") >= MaxTime ){

      m.message("CAT::sequentiator::late: execution time " , clock.read(" sequentiator: sequentiation ") , " ms  greater than MaxTime" , MaxTime , " quitting! ", mybhep::NORMAL);

      //      clock.stop_all();

      /*
        if( !evt.find_property("MaxTime") )
        evt.add_property("MaxTime","1");
        else
        }
      */

      return true;
    }

    return false;

  }

  //*************************************************************
  bool sequentiator::evolve(topology::sequence & sequence){
    //*************************************************************

    const bool local_devel = false;

    if (local_devel)
      {
        clog << "DEVEL: " << "CAT::sequentiator::evolve: "
             << "Entering..." << endl;
      }

    if (late()) return false;

    clock.start(" sequentiator: evolve ","cumulative");

    clock.start(" sequentiator: evolve: part A ","cumulative");

    const size_t sequence_size = sequence.nodes().size();

    if (local_devel)
      {
        clog << "DEVEL: " << "CAT::sequentiator::evolve: "
             << "Sequence size = " << sequence_size << endl;
      }

    m.message("CAT::sequentiator::evolve: evolving sequence of size", sequence_size, mybhep::VVERBOSE); fflush(stdout);

    // protection
    if( sequence_size < 1 )
      {
        m.message("CAT::sequentiator::evolve: problem: sequence has length ", sequence_size, "... stop evolving ", mybhep::NORMAL); fflush(stdout);
        clock.stop(" sequentiator: evolve: part A ");
        clock.stop(" sequentiator: evolve ");
        return false;
      }


    if( level >= mybhep::VVERBOSE )
      print_a_sequence(sequence);

    if( sequence_size == 3 ){
      clock.start(" sequentiator: get link index ","cumulative");
      size_t it1 = sequence.get_link_index_of_cell(0, sequence.nodes()[1].c());
      if( it1 >= sequence.nodes_[0].cc_.size() ){
        m.message("CAT::sequentiator::evolve: problem: it1 ", it1, " nodes size ", sequence.nodes_.size(), " cc size ", sequence.nodes_[0].cc_.size(), mybhep::NORMAL); fflush(stdout);
        clock.stop(" sequentiator: evolve: part A ");
        clock.stop(" sequentiator: evolve ");
        return false;
      }
      sequence.nodes_[0].cc_[it1].set_all_used();

      clock.stop(" sequentiator: get link index ");
    }

    clock.stop(" sequentiator: evolve: part A ");
    clock.start(" sequentiator: evolve: part B ","cumulative");

    // check if there is a possible link
    size_t ilink;
    topology::experimental_point newp;
    clock.start(" sequentiator: pick new cell ","cumulative");
    bool there_is_link = sequence.pick_new_cell(&ilink, &newp, *local_cluster_);
    clock.stop(" sequentiator: pick new cell ");

    if (local_devel)
      {
        clog << "DEVEL: " << "CAT::sequentiator::evolve: "
             << "Is there a link = " << there_is_link << endl;
      }

    if( !there_is_link ){
      m.message("CAT::sequentiator::evolve: no links could be added... stop evolving ", mybhep::VERBOSE); fflush(stdout);
      clock.start(" sequentiator: evolve: part B: set free level ","cumulative");
      clock.start(" sequentiator: set free level ","cumulative");
      sequence.set_free_level();
      clock.stop(" sequentiator: set free level ");
      clock.stop(" sequentiator: evolve: part B: set free level ");
      clock.stop(" sequentiator: evolve: part B ");
      clock.stop(" sequentiator: evolve ");

      if (sequence.nodes().size() == 1){
        topology::experimental_point ep(sequence.nodes_[0].c().ep());
        ep.set_ex(sequence.nodes_[0].c().r().value());
        ep.set_ez(sequence.nodes_[0].c().r().value());
        sequence.nodes_[0].set_ep(ep);
      }


      return false;
    }

    topology::cell newcell = sequence.last_node().links()[ilink];
    clock.start(" sequentiator: evolve: part B: noc ","cumulative");
    topology::node newnode = local_cluster_->node_of_cell(newcell);
    //  topology::node newnode = local_cluster_->nodes()[local_cluster_->node_index_of_cell(newcell)];
    clock.stop(" sequentiator: evolve: part B: noc ");
    newnode.set_free(false); // standard initialization

    clock.stop(" sequentiator: evolve: part B ");
    clock.start(" sequentiator: evolve: part C ","cumulative");

    if( sequence_size == 1 )
      {
        // since it's the 2nd cell, only the four
        // standard alternatives are available (which will be treated out of "Evolve"), so
        // this link has no freedom left

        sequence.nodes_.push_back( newnode );
        clock.start(" sequentiator: set free level ","cumulative");
        sequence.set_free_level();
        clock.stop(" sequentiator: set free level ");

        clock.stop(" sequentiator: evolve: part C ");
        clock.stop(" sequentiator: evolve ");
        return true;
      }


    newnode.set_ep(newp);

    sequence.nodes_.push_back( newnode );
    m.message("CAT::sequentiator::evolve: points have been added ", mybhep::VERBOSE); fflush(stdout);

    clock.start(" sequentiator: set free level ","cumulative");
    sequence.set_free_level();
    clock.stop(" sequentiator: set free level ");

    clock.stop(" sequentiator: evolve: part C ");
    clock.stop(" sequentiator: evolve ");
    return true;
  }



  //*************************************************************
  bool sequentiator::good_first_node(topology::node & node_) {
    //*************************************************************

    clock.start(" sequentiator: good first node ", "cumulative");

    const string type = node_.topological_type();

    // check that node is not in the middle of a cell_triplet
    if( type != "VERTEX" &&
        type != "MULTI_VERTEX" &&
        type != "ISOLATED" ){
      // clock.stop(" sequentiator: good first node ");
      m.message("CAT::sequentiator::good_first_node: not a good first node: type ", type, mybhep::VVERBOSE); fflush(stdout);
      return false;
    }


    vector<size_t> done_connections;
    size_t connection_node;
    vector<size_t>::iterator fid;
    // check that node has never been added to a sequence
    for(vector<topology::sequence>::const_iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      {
        if( iseq->has_cell(node_.c()) ){
          if( type == "VERTEX" ){

            m.message("CAT::sequentiator::good_first_node: not a good first node: already used as vertex in seuqence ", iseq - sequences_.begin(), mybhep::VVERBOSE); fflush(stdout);

            // clock.stop(" sequentiator: good first node ");
            return false;
          }
          else{ // multi-vertex
            if( iseq->nodes_.size() > 1 ){
              if( iseq->nodes_[0].c().id() == node_.c().id() )
                connection_node = 1;
              else if( iseq->last_node().c().id() == node_.c().id() ){
                connection_node = iseq->nodes_.size() - 2;
              }
              else{
                m.message("CAT::sequentiator::good_first_node: problem: multi-vertex ", node_.c().id(), " belongs to sequence ", iseq->name(), " but not as first or last cell", mybhep::NORMAL);
                continue;
              }
              // add to done_connections cell ids of those cells
              // that have already been connected to NODE in other sequences
              fid = std::find(done_connections.begin(),
                              done_connections.end(),
                              iseq->nodes_[connection_node].c().id());

              if( fid == done_connections.end())
                done_connections.push_back(iseq->nodes_[connection_node].c().id());
            }
          }
        }
      }

    size_t cc_index;
    if( type == "MULTI_VERTEX" ){
      for(size_t i=0; i<done_connections.size(); i++){
        cc_index = 0;
        if( !node_.has_couplet(done_connections[i],  &cc_index) )
          m.message("CAT::sequentiator::good_first_node: problem: multi-vertex ", node_.c().id(), " should link to cell ", done_connections[i], " but has not such couplet", mybhep::NORMAL);
        else{
          m.message("CAT::sequentiator::good_first_node: multi-vertex ", node_.c().id(), " has already been added to a sequence connecting to cell ", done_connections[i], " so couplet ", cc_index, " will be erased", mybhep::VERBOSE);
          //node_.cc_.erase(node_.cc_.begin() + cc_index);
          node_.remove_couplet(cc_index);
        }
      }
    }

    clock.stop(" sequentiator: good first node ");
    return true;


  }


  //*************************************************************
  void sequentiator::make_families() {
    //*************************************************************

    clock.start(" sequentiator: make families ", "cumulative");

    families_.clear();

    bool found, added;
    size_t ifam;
    std::vector<size_t> Fam;
    for (std::vector<topology::sequence>::const_iterator iseq = sequences_.begin();
         iseq != sequences_.end(); ++iseq){

      const size_t ipart = iseq - sequences_.begin();
      const size_t this_family = mybhep::int_from_string(iseq->family());

      found = false;
      for(size_t i=0; i<families_.size(); i++)
        if( families_[i][0] == this_family )
          {
            found = true;
            ifam = i;
            break;
          }

      if( found )
        {
          added = false;
          for(size_t j=0; j<families_[ifam].size(); j++)
            {
              if(j == 0) continue;
              if( families_[ifam][j] == ipart )
                {
                  added = true;
                  break;
                }
            }
          if( !added )
            families_[ifam].push_back( ipart );
        }
      else
        {
          Fam.clear();
          Fam.push_back(this_family);

          Fam.push_back(ipart);
          families_.push_back( Fam );
        }
    }

    clock.stop(" sequentiator: make families ");

    return;

  }

  //*************************************************************
  void sequentiator::print_families( void ) {
    //*************************************************************

    std::clog << "CAT::sequentiator::print_families: families: " << std::endl;

    for(size_t i=0; i<families_.size(); i++)
      {
        std::clog << "[" << families_[i][0] << "] (";
        for(size_t j=0; j<families_[i].size(); j++)
          {
            if( j == 0 ) continue;
            std::clog << " " << families_[i][j];
          }
        std::clog << ")" << std::endl;
      }

    return;
  }


  //*************************************************************
  bool sequentiator::make_scenarios(topology::tracked_data &td){
    //*************************************************************

    clock.start(" sequentiator: make scenarios ", "cumulative");

    if( level >= mybhep::VERBOSE)
      print_families();

    size_t jmin, nfree, noverlaps;
    double Chi2;
    int ndof;
    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){
      if( late() ){
        td.set_skipped(true);
        return false;
      }

      m.message("CAT::sequentiator::make_scenarios: begin scenario with sequence ", iseq->name(), mybhep::VVERBOSE);
      if( level >= mybhep::VVERBOSE)
        print_a_sequence(*iseq);

      topology::scenario sc;
      sc.level_ = level;
      sc.set_probmin(probmin);
      sc.sequences_.push_back(*iseq);
      sc.calculate_n_free_families(td.get_cells(), td.get_calos());
      sc.calculate_n_overlaps(td.get_cells(), td.get_calos());
      sc.calculate_chi2();

      while( can_add_family(sc, &jmin, &nfree, &Chi2, &noverlaps, &ndof, td) )
        {
          m.message("CAT::sequentiator::make_scenarios: best sequence to add is ", jmin, mybhep::VVERBOSE);
          if( level >= mybhep::VVERBOSE)
            print_a_sequence(sequences_[jmin]);
          m.message("CAT::sequentiator::make_scenarios: nfree ", nfree, " noverls ", noverlaps, " Chi2 ", Chi2, mybhep::VVERBOSE);
          sc.sequences_.push_back(sequences_[jmin]);
          sc.set_n_free_families(nfree);
          sc.set_helix_chi2(Chi2);
          sc.set_ndof(ndof);
          sc.set_n_overlaps(noverlaps);
        }

      scenarios_.push_back(sc);


    }

    if( late() ){
      td.set_skipped(true);
      return false;
    }


    direct_scenarios_out_of_foil();

    if( level >= mybhep::VERBOSE)
      print_scenarios();

    if( scenarios_.size() > 0 ){

      size_t index_tmp = pick_best_scenario();

      if( level > mybhep::NORMAL)
        print_a_scenario(scenarios_[index_tmp]);


      m.message("CAT::sequentiator::make_scenarios: made scenario ", mybhep::VERBOSE);

      td.scenarios_.push_back(scenarios_[index_tmp]);

      clock.stop(" sequentiator: make scenarios ");
      return true;

    }

    m.message("CAT::sequentiator::make_scenarios: not made scenario ", mybhep::VERBOSE);
    clock.stop(" sequentiator: make scenarios ");

    return false;


  }


  //*************************************************************
  size_t sequentiator::pick_best_scenario(){
    //*************************************************************

    m.message("CAT::sequentiator::pick_best_scenario: Entering... ", mybhep::VVERBOSE);

    size_t index = 0;

    for(std::vector<topology::scenario>::iterator sc=scenarios_.begin(); sc!=scenarios_.end(); ++sc){
      if( level >= mybhep::VVERBOSE)
        std::clog << "CAT::sequentiator::pick_best_scenario: ...scenario " << sc - scenarios_.begin() << " nff " << sc->n_free_families() << " noverls " << sc->n_overlaps() << " common vertexes " << sc->n_of_common_vertexes(2.*CellDistance) << " n ends on wire " << sc->n_of_ends_on_wire() << " chi2 " << sc->helix_chi2() << " prob " << sc->helix_Prob() << std::endl;

      if( sc->better_scenario_than( scenarios_[index] , 2.*CellDistance ) )
        {
          index = sc - scenarios_.begin();
        }
    }

    m.message("CAT::sequentiator::pick_best_scenario: best scenario is ", index, mybhep::VERBOSE);

    return index;

  }

  //*************************************************************
  bool sequentiator::can_add_family(topology::scenario &sc, size_t* jmin, size_t* nfree, double* Chi2, size_t* noverlaps, int* ndof, topology::tracked_data &td) {
    //*************************************************************

    if( late() )
      return false;

    clock.start(" sequentiator: can add family ", "cumulative");

    bool ok = false;

    if( sc.n_free_families() == 0 ){
      clock.stop(" sequentiator: can add family ");
      return false;
    }

#if 0
    clock.start(" sequentiator: copy logic scenario ", "cumulative");
    topology::logic_scenario tmpmin = topology::logic_scenario(sc);
    topology::logic_scenario tmpsave = tmpmin;
    clock.stop(" sequentiator: copy logic scenario ");
    topology::logic_scenario tmp;
#else
    clock.start(" sequentiator: copy scenario ", "cumulative");
    topology::scenario tmpmin = sc;
    clock.stop(" sequentiator: copy scenario ");
    topology::scenario tmp = sc;
#endif

    std::map<string,int> scnames;
    for(std::vector<topology::sequence>::iterator iseq=sc.sequences_.begin(); iseq!=sc.sequences_.end(); ++iseq)
      scnames[iseq->name()]=iseq-sc.sequences_.begin();

    for(std::vector<topology::sequence>::iterator jseq=sequences_.begin(); jseq!=sequences_.end(); ++jseq)
      {

        if( scnames.count(jseq->name()) ) continue;

#if 0
        clock.start(" sequentiator: copy logic scenario ", "cumulative");
        tmp = tmpsave;
        clock.stop(" sequentiator: copy logic scenario ");
        clock.start(" sequentiator: copy logic sequence ", "cumulative");
        tmp.sequences_.push_back(topology::logic_sequence(*jseq));
        clock.stop(" sequentiator: copy logic sequence ");
#else
        clock.start(" sequentiator: copy scenario ", "cumulative");
        tmp = sc;
        clock.stop(" sequentiator: copy scenario ");
        clock.start(" sequentiator: copy sequence ", "cumulative");
        tmp.sequences_.push_back(*jseq);
        clock.stop(" sequentiator: copy sequence ");
#endif

        clock.start(" sequentiator: calculate scenario ", "cumulative");
        tmp.calculate_n_free_families(td.get_cells(), td.get_calos());
        tmp.calculate_n_overlaps(td.get_cells(), td.get_calos());
        tmp.calculate_chi2();
        clock.stop(" sequentiator: calculate scenario ");

        m.message("CAT::sequentiator::can_add_family: ...try to add sequence ", jseq->name(), mybhep::VVERBOSE);
        if( level >= mybhep::VVERBOSE)
          print_a_sequence(*jseq);
        m.message("CAT::sequentiator::can_add_family: ...nfree ", tmp.n_free_families(), " noverls ", tmp.n_overlaps(), " chi2 ", tmp.helix_chi2(), " prob ", tmp.helix_Prob(), mybhep::VVERBOSE);

        clock.start(" sequentiator: better scenario ", "cumulative");
        if( tmp.better_scenario_than(tmpmin , 2.*CellDistance ) )
          {
            *jmin = jseq - sequences_.begin();
            *nfree = tmp.n_free_families();
            *noverlaps = tmp.n_overlaps();
            *Chi2 = tmp.helix_chi2();
            *ndof = tmp.ndof();
            tmpmin = tmp;
            ok = true;
          }
        clock.stop(" sequentiator: better scenario ");

      }

    clock.stop(" sequentiator: can add family ");
    return ok;

  }

  //*************************************************************
  topology::plane sequentiator::get_foil_plane(){
    //*************************************************************

    if( !SuperNemo ){
      m.message("CAT::sequentiator::get_foil_plane: problem: asking for foil plane in Nemo3! ", mybhep::NORMAL);
      topology::plane null;
      return null;
    }

    topology::experimental_point center(0., 0., 0., 0., 0., 0.);

    topology::experimental_vector norm(0.,0.,1.,0.,0.,0.);

    topology::experimental_vector sizes(xsize, ysize, 0.,
                                        0., 0., 0.);

    topology::plane pl(center, sizes, norm, level, probmin);

    std::string the_type="Nemo3";
    if( SuperNemo )
      the_type="SuperNEMO";

    pl.set_type(the_type);

    return pl;


  }

  //*************************************************************
  topology::circle sequentiator::get_foil_circle(){
    //*************************************************************

    if( SuperNemo ){
      m.message("CAT::sequentiator::get_foil_circle: problem: asking for foil circle in SuperNemo ", mybhep::NORMAL);
      topology::circle null;
      return null;
    }

    topology::experimental_point center(0., 0., 0., 0., 0., 0.);

    topology::experimental_double radius(FoilRadius, 0.);

    topology::circle c(center, radius, level, probmin);

    return c;


  }


  //*************************************************************
  void sequentiator::refine_sequences_near_walls(std::vector<topology::calorimeter_hit> & calos){
    //*************************************************************

    for( std::vector<topology::sequence>::iterator iseq = sequences_.begin(); iseq!=sequences_.end(); ++iseq){

      if( iseq->nodes_.size() < 3 ) continue;
      if( gap_number( iseq->second_last_node().c() ) == 0 &&
          iseq->phi_kink(iseq->nodes_.size()-2)*180./M_PI > 45 &&
          belongs_to_other_family(iseq->last_node().c(), &(*iseq)) ){
        m.message( "CAT::sequentiator::refine_sequences_near_walls: removing last node ", iseq->last_node().c(), " near foil of sequence ", iseq->name(),  "(it belongs to other family and makes large kink) ", mybhep::VVERBOSE); fflush(stdout);
        iseq->remove_last_node();
      }

      if( iseq->nodes_.size() < 3 ) continue;
      if( gap_number( iseq->nodes_[1].c() ) == 0 &&
          iseq->phi_kink(1)*180./M_PI > 45 &&
          belongs_to_other_family(iseq->nodes_[0].c(), &(*iseq)) ){
        m.message( "CAT::sequentiator::refine_sequences_near_walls: removing 1st node ", iseq->last_node().c(), " near foil of sequence ", iseq->name(),  "(it belongs to other family and makes large kink) ", mybhep::VVERBOSE); fflush(stdout);
        iseq->remove_first_node();
      }


      if( iseq->nodes_.size() < 3 ) continue;
      for(std::vector<topology::calorimeter_hit>::iterator ic=calos.begin(); ic != calos.end(); ++ic){
        if( near(iseq->nodes_[1].c(), *ic) &&
            iseq->phi_kink(1)*180./M_PI > 45 &&
            belongs_to_other_family(iseq->nodes_[0].c(), &(*iseq)) ){
          m.message("CAT::sequentiator::refine_sequences_near_walls: removing 1st node ", iseq->last_node().c(), " near calo of sequence ", iseq->name(),  "(it belongs to other family and makes large kink) ", mybhep::VVERBOSE); fflush(stdout);
          iseq->remove_first_node();
          break;
        }
      }

      if( iseq->nodes_.size() < 3 ) continue;
      for(std::vector<topology::calorimeter_hit>::iterator ic=calos.begin(); ic != calos.end(); ++ic){
        if( near(iseq->second_last_node().c(), *ic) &&
            iseq->phi_kink(iseq->nodes_.size()-2)*180./M_PI > 45 &&
            belongs_to_other_family(iseq->last_node().c(), &(*iseq)) ){
          m.message( "CAT::sequentiator::refine_sequences_near_walls: removing last node ", iseq->last_node().c(), " near calo of sequence ", iseq->name(),  "(it belongs to other family and makes large kink) ", mybhep::VVERBOSE); fflush(stdout);
          iseq->remove_last_node();
          break;
        }
      }
    }

    return;

  }


  //*************************************************************
  bool sequentiator::belongs_to_other_family(topology::cell c, topology::sequence *iseq){
    //*************************************************************

    for( std::vector<topology::sequence>::iterator jseq = sequences_.begin(); jseq!=sequences_.end(); ++jseq){
      if( iseq->same_families(*jseq) ) continue;
      if( !jseq->has_cell(c) ) continue;
      return true;
    }

    return false;

  }

  //*************************************************************
  void sequentiator::interpret_physics(std::vector<topology::calorimeter_hit> & calos){
    //*************************************************************

    clock.start(" sequentiator: interpret physics ", "cumulative");

    m.message("CAT::sequentiator::interpret_physics: interpreting physics of ", sequences_.size(), " sequences with ", calos.size(), " calorimeter hits ", mybhep::VVERBOSE); fflush(stdout);

    double helix_min_from_end = mybhep::default_min;
    size_t ihelix_min_from_end = mybhep::default_integer;
    double tangent_min_from_end = mybhep::default_min;
    size_t itangent_min_from_end = mybhep::default_integer;

    double helix_min_from_begin = mybhep::default_min;
    size_t ihelix_min_from_begin = mybhep::default_integer;
    double tangent_min_from_begin = mybhep::default_min;
    size_t itangent_min_from_begin = mybhep::default_integer;

    topology::experimental_point helix_extrapolation_from_end, helix_extrapolation_local_from_end;
    bool helix_found_from_end = false;
    topology::experimental_point helix_extrapolation_from_begin, helix_extrapolation_local_from_begin;
    bool helix_found_from_begin = false;

    topology::experimental_point tangent_extrapolation_from_end, tangent_extrapolation_local_from_end;
    bool tangent_found_from_end = false;
    topology::experimental_point tangent_extrapolation_from_begin, tangent_extrapolation_local_from_begin;
    bool tangent_found_from_begin = false;

    double dist_from_end, dist_from_begin;
    std::vector<topology::sequence>::iterator iseq = sequences_.begin();
    while( iseq != sequences_.end() )
      {
        m.message("CAT::sequentiator::interpret_physics: ... interpreting physics of sequence ", iseq->name(), mybhep::VVERBOSE); fflush(stdout);

        if( iseq->nodes().size() <= 2 ){
          ++iseq;
          continue;
        }

        if( level >= mybhep::VVERBOSE)
          print_a_sequence(*iseq);

        if( !iseq->calculate_helix() && !iseq->has_kink() ){
          size_t index = iseq - sequences_.begin();
          m.message("CAT::sequentiator::interpret_physics: erased sequence ", index, "not a good helix", mybhep::VERBOSE); fflush(stdout);
          sequences_.erase(iseq);
          iseq = sequences_.begin() + index;
          if( index + 1 >= sequences_.size() )
            break;
          continue;
        }
        iseq->calculate_charge();
        iseq->calculate_momentum(bfield);

        // match to calorimeter
        if (!calos.empty ())
          {

            m.message("CAT::sequentiator::interpret_physics: extrapolate decay vertex with ", calos.size(), " calo hits " , mybhep::VVERBOSE);

            helix_min_from_end = mybhep::default_min;
            ihelix_min_from_end = mybhep::default_integer;
            tangent_min_from_end = mybhep::default_min;
            itangent_min_from_end = mybhep::default_integer;

            helix_min_from_begin = mybhep::default_min;
            ihelix_min_from_begin = mybhep::default_integer;
            tangent_min_from_begin = mybhep::default_min;
            itangent_min_from_begin = mybhep::default_integer;

            helix_found_from_end = false;
            helix_found_from_begin = false;

            tangent_found_from_end = false;
            tangent_found_from_begin = false;

            for(std::vector<topology::calorimeter_hit>::iterator ic=calos.begin(); ic != calos.end(); ++ic){

              m.message( "CAT::sequentiator::interpret_physics: trying to extrapolate to calo hit ", ic - calos.begin(), " id ", ic->id(), " on view ", ic->pl_.view(), " energy ", ic->e().value(), mybhep::VVERBOSE);

              if( !near(iseq->last_node().c(), *ic) ){
                m.message( "CAT::sequentiator::interpret_physics: end is not near " , mybhep::VVERBOSE);
              }else{

                if( !iseq->intersect_plane_from_end(ic->pl(), &helix_extrapolation_local_from_end) ){
                  m.message( "CAT::sequentiator::interpret_physics: no helix intersection from end " , mybhep::VVERBOSE);
                }
                else{

                  dist_from_end = helix_extrapolation_local_from_end.distance(ic->pl_.face()).value();
                  if( dist_from_end < helix_min_from_end ){
                    helix_min_from_end = dist_from_end;
                    ihelix_min_from_end = ic->id();
                    helix_extrapolation_from_end = helix_extrapolation_local_from_end;
                    helix_found_from_end = true;
                    m.message( "CAT::sequentiator::interpret_physics: new helix intersection from end with minimum distance " , dist_from_end , " position: " , helix_extrapolation_from_end.x().value() ,   helix_extrapolation_from_end.y().value(),  helix_extrapolation_from_end.z().value() , mybhep::VVERBOSE);
                  }
                }


                if( !iseq->intersect_plane_with_tangent_from_end(ic->pl(), &tangent_extrapolation_local_from_end) ){
                  m.message( "CAT::sequentiator::interpret_physics: no tangent intersection from end " , mybhep::VVERBOSE);
                }
                else{

                  dist_from_end = tangent_extrapolation_local_from_end.distance(ic->pl_.face()).value();
                  if( dist_from_end < tangent_min_from_end ){
                    tangent_min_from_end = dist_from_end;
                    itangent_min_from_end = ic->id();
                    tangent_extrapolation_from_end = tangent_extrapolation_local_from_end;
                    tangent_found_from_end = true;
                    m.message( "CAT::sequentiator::interpret_physics: new tangent intersection from end with minimum distance " , dist_from_end , " position: " , tangent_extrapolation_from_end.x().value() ,   tangent_extrapolation_from_end.y().value(),  tangent_extrapolation_from_end.z().value() , mybhep::VVERBOSE);
                  }
                }
              }

              if( !near(iseq->nodes_[0].c(), *ic) ){
                m.message( "CAT::sequentiator::interpret_physics: beginning is not near " , mybhep::VVERBOSE);
              }else if( ihelix_min_from_end == ic->id() || itangent_min_from_end == ic->id() ){
                m.message( "CAT::sequentiator::interpret_physics: beginning is near, but end was already extrapolated to same calo " , mybhep::VVERBOSE);
              }else{
                if( !iseq->intersect_plane_from_begin(ic->pl(), &helix_extrapolation_local_from_begin) ){
                  m.message( "CAT::sequentiator::interpret_physics: no helix intersection from beginning " , mybhep::VVERBOSE);
                }
                else{

                  dist_from_begin = helix_extrapolation_local_from_begin.distance(ic->pl_.face()).value();
                  if( dist_from_begin < helix_min_from_begin ){
                    helix_min_from_begin = dist_from_begin;
                    ihelix_min_from_begin = ic->id();
                    helix_extrapolation_from_begin = helix_extrapolation_local_from_begin;
                    helix_found_from_begin = true;
                    m.message( "CAT::sequentiator::interpret_physics: new helix intersection from beginning with minimum distance " , dist_from_begin , " position: " , helix_extrapolation_from_begin.x().value() ,   helix_extrapolation_from_begin.y().value(),  helix_extrapolation_from_begin.z().value() , mybhep::VVERBOSE);
                  }
                }


                if( !iseq->intersect_plane_with_tangent_from_begin(ic->pl(), &tangent_extrapolation_local_from_begin) ){
                  m.message( "CAT::sequentiator::interpret_physics: no tangent intersection from beginning " , mybhep::VVERBOSE);
                }
                else{

                  dist_from_begin = tangent_extrapolation_local_from_begin.distance(ic->pl_.face()).value();
                  if( dist_from_begin < tangent_min_from_begin ){
                    tangent_min_from_begin = dist_from_begin;
                    itangent_min_from_begin = ic->id();
                    tangent_extrapolation_from_begin = tangent_extrapolation_local_from_begin;
                    tangent_found_from_begin = true;
                    m.message( "CAT::sequentiator::interpret_physics: new tangent intersection from beginning with minimum distance " , dist_from_begin , " position: " , tangent_extrapolation_from_begin.x().value() ,   tangent_extrapolation_from_begin.y().value(),  tangent_extrapolation_from_begin.z().value() , mybhep::VVERBOSE);
                  }
                }
              }


            } // finish loop on calos

            if( helix_found_from_begin ){
              if( ihelix_min_from_begin >= calos.size() ){
                m.message( "CAT::sequentiator::interpret_physics: problem: calo hit of id " , ihelix_min_from_begin , " but n of calo hits is " , calos.size() , mybhep::NORMAL);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by helix to calo " , ihelix_min_from_begin, mybhep::VVERBOSE);
                iseq->set_helix_vertex(helix_extrapolation_from_begin, "calo", ihelix_min_from_begin);
              }
            }

            if( tangent_found_from_begin ){
              if( itangent_min_from_begin >= calos.size() ){
                m.message( "CAT::sequentiator::interpret_physics: problem: tangent calo hit of id " , itangent_min_from_begin , " but n of calo hits is " , calos.size() , mybhep::NORMAL);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by tangent to calo " , itangent_min_from_begin, mybhep::VVERBOSE);
                iseq->set_tangent_vertex(tangent_extrapolation_from_begin, "calo", itangent_min_from_begin);
              }
            }

            if( helix_found_from_end ){
              if( ihelix_min_from_end >= calos.size() ){
                m.message( "CAT::sequentiator::interpret_physics: problem: calo hit of id " , ihelix_min_from_end , " but n of calo hits is " , calos.size() , mybhep::NORMAL);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by helix to calo " , ihelix_min_from_end, mybhep::VVERBOSE);
                iseq->set_decay_helix_vertex(helix_extrapolation_from_end, "calo", ihelix_min_from_end);
              }
            }

            if( tangent_found_from_end ){
              if( itangent_min_from_end >= calos.size() ){
                m.message( "CAT::sequentiator::interpret_physics: problem: tangent calo hit of id " , itangent_min_from_end , " but n of calo hits is " , calos.size() , mybhep::NORMAL);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by tangent to calo " , itangent_min_from_end, mybhep::VVERBOSE);
                iseq->set_decay_tangent_vertex(tangent_extrapolation_from_end, "calo", itangent_min_from_end);
              }
            }

          }

        // match to foil
        if( !iseq->nodes_.empty() ){

          m.message( "CAT::sequentiator::interpret_physics: extrapolate vertex on foil: supernemo " , SuperNemo, mybhep::VVERBOSE);

          if( gap_number(iseq->last_node().c() ) != 0 ){
            m.message( "CAT::sequentiator::interpret_physics: end not near ", mybhep::VVERBOSE); fflush(stdout);
          }else{
            if( SuperNemo ){

              if( !iseq->intersect_plane_from_end(get_foil_plane(), &helix_extrapolation_from_end) ){
                m.message("CAT::sequentiator::interpret_physics: no helix intersection from end ", mybhep::VVERBOSE); fflush(stdout);
              }
              else{
                iseq->set_decay_helix_vertex(helix_extrapolation_from_end, "foil");

              }

              if( !iseq->intersect_plane_with_tangent_from_end(get_foil_plane(), &tangent_extrapolation_from_end) ){
                m.message("CAT::sequentiator::interpret_physics: no tangent intersection from end ", mybhep::VVERBOSE); fflush(stdout);
              }
              else
                iseq->set_decay_tangent_vertex(tangent_extrapolation_from_end, "foil");

            }else{  // nemo3


              if( !iseq->intersect_circle_from_end(get_foil_circle(), &helix_extrapolation_from_end) ){
                m.message("CAT::sequentiator::interpret_physics: no helix intersection from end ", mybhep::VVERBOSE); fflush(stdout);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by helix to foil from end ", mybhep::VVERBOSE);
                iseq->set_decay_helix_vertex(helix_extrapolation_from_end, "foil");
              }

              if( !iseq->intersect_circle_with_tangent_from_end(get_foil_circle(), &tangent_extrapolation_from_end) ){
                m.message("CAT::sequentiator::interpret_physics: no tangent intersection from end ", mybhep::VVERBOSE); fflush(stdout);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by tangent to foil from end", mybhep::VVERBOSE);
                iseq->set_decay_tangent_vertex(tangent_extrapolation_from_end, "foil");
              }

            }
          }

          if( gap_number(iseq->nodes_[0].c() ) != 0 ){
            m.message( "CAT::sequentiator::interpret_physics: beginning not near ", mybhep::VVERBOSE); fflush(stdout);
          }else{
            if( SuperNemo ){

              if( !iseq->intersect_plane_from_begin(get_foil_plane(), &helix_extrapolation_from_begin) ){
                m.message("CAT::sequentiator::interpret_physics: no helix intersection from beginning ", mybhep::VVERBOSE); fflush(stdout);
              }
              else{
                iseq->set_helix_vertex(helix_extrapolation_from_begin, "foil");

              }

              if( !iseq->intersect_plane_with_tangent_from_begin(get_foil_plane(), &tangent_extrapolation_from_begin) ){
                m.message("CAT::sequentiator::interpret_physics: no tangent intersection from beginning ", mybhep::VVERBOSE); fflush(stdout);
              }
              else
                iseq->set_tangent_vertex(tangent_extrapolation_from_begin, "foil");

            }else{  // nemo3


              if( !iseq->intersect_circle_from_begin(get_foil_circle(), &helix_extrapolation_from_begin) ){
                m.message("CAT::sequentiator::interpret_physics: no helix intersection from beginning ", mybhep::VVERBOSE); fflush(stdout);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by helix to foil from beginning ", mybhep::VVERBOSE);
                iseq->set_helix_vertex(helix_extrapolation_from_begin, "foil");
              }

              if( !iseq->intersect_circle_with_tangent_from_begin(get_foil_circle(), &tangent_extrapolation_from_begin) ){
                m.message("CAT::sequentiator::interpret_physics: no tangent intersection from beginning ", mybhep::VVERBOSE); fflush(stdout);
              }
              else{
                m.message( "CAT::sequentiator::interpret_physics: track extrapolated by tangent to foil from begin", mybhep::VVERBOSE);
                iseq->set_tangent_vertex(tangent_extrapolation_from_begin, "foil");
              }

            }



          }
        }

        iseq->calculate_length();

        if( level >= mybhep::VVERBOSE ){
          std::clog << "CAT::sequentiator::interpret_physics: sequence " << iseq - sequences_.begin() << " has: " << std::endl; fflush(stdout);
          std::clog << "CAT::sequentiator::interpret_physics: center "; iseq->center().dump(); fflush(stdout);
          std::clog << "CAT::sequentiator::interpret_physics: radius "; iseq->radius().dump(); std::clog << " " << std::endl; fflush(stdout);
          std::clog << "CAT::sequentiator::interpret_physics: pitch "; iseq->pitch().dump(); std::clog << " " << std::endl; fflush(stdout);
          std::clog << "CAT::sequentiator::interpret_physics: momentum "; iseq->momentum().length().dump(); std::clog << " " << std::endl; fflush(stdout);
          std::clog << "CAT::sequentiator::interpret_physics: charge "; iseq->charge().dump(); std::clog << " " << std::endl; fflush(stdout);
          if( iseq->has_helix_vertex() ){
            std::clog << "CAT::sequentiator::interpret_physics: helix_vertex " << iseq->helix_vertex_type() << " "; iseq->helix_vertex().dump();
            if( iseq->helix_vertex_type() == "calo" ) std::clog << " icalo " << iseq->helix_vertex_id();
            std::clog << " " << std::endl; fflush(stdout);
          }
          if( iseq->has_decay_helix_vertex() ){
            std::clog << "CAT::sequentiator::interpret_physics: decay helix_vertex " << iseq->decay_helix_vertex_type() << " "; iseq->decay_helix_vertex().dump();
            if( iseq->decay_helix_vertex_type() == "calo" ) std::clog << " icalo " << iseq->calo_helix_id();
            std::clog << " " << std::endl; fflush(stdout);
          }
          if( iseq->has_tangent_vertex() ){
            std::clog << "CAT::sequentiator::interpret_physics: tangent_vertex " << iseq->tangent_vertex_type() << " "; iseq->tangent_vertex().dump();
            if( iseq->tangent_vertex_type() == "calo" ) std::clog << " icalo " << iseq->tangent_vertex_id();
            std::clog << " " << std::endl; fflush(stdout);
          }
          if( iseq->has_decay_tangent_vertex() ){
            std::clog << "CAT::sequentiator::interpret_physics: decay tangent_vertex " << iseq->decay_tangent_vertex_type() << " "; iseq->decay_tangent_vertex().dump();
            if( iseq->decay_tangent_vertex_type() == "calo" ) std::clog << " icalo " << iseq->calo_tangent_id();
            std::clog << " " << std::endl; fflush(stdout);
          }
          if( iseq->has_tangent_length() ){
            std::clog << "CAT::sequentiator::interpret_physics: tangent length "; iseq->tangent_length().dump(); std::clog << " " << std::endl; fflush(stdout);
          }
          if( iseq->has_helix_length() ){
            std::clog << "CAT::sequentiator::interpret_physics: helix length "; iseq->helix_length().dump(); std::clog << " " << std::endl; fflush(stdout);
          }

        }
        ++iseq;
        continue;

      }


    clock.stop(" sequentiator: interpret physics ");

    return;

  }


  //*************************************************************
  void sequentiator::print_sequences() const {
    //*************************************************************

    m.message("CAT::sequentiator::print_sequences: sequence matrix is", mybhep::NORMAL); fflush(stdout);
    for(vector<topology::sequence>::const_iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      {
        std::clog << " [" << iseq - sequences_.begin() << "]";
        print_a_sequence(*iseq);
      }

    return;

  }


  //*************************************************************
  void sequentiator::print_a_sequence(const topology::sequence & sequence) const {
    //*************************************************************

    std::clog << sequence.name();

    if( sequence.Free() )
      {
        std::clog << "*";
        fflush(stdout);
      }

    int iccc;
    for(vector<topology::node>::const_iterator inode = sequence.nodes_.begin();
        inode != sequence.nodes_.end(); ++inode)
      {
        std::clog << " " << (int)inode->c().id();fflush(stdout);

        if( inode->free() )
          std::clog << "* ";fflush(stdout);

        topology::experimental_vector v(inode->c().ep(),inode->ep());

        if( level >= mybhep::VVERBOSE ){
          std::clog << "[" << v.x().value() << ", " << v.z().value() << "]";fflush(stdout);

          std::clog << "(";fflush(stdout);

          for(vector<topology::cell>::const_iterator ilink=(*inode).links_.begin(); ilink != (*inode).links_.end(); ++ilink){
            iccc = sequence.get_link_index_of_cell(inode - sequence.nodes_.begin(), *ilink);

            if( iccc < 0 ) continue;  // connection through a gap

            if( inode - sequence.nodes_.begin() < 1 ){
              std::clog << "->" << inode->cc()[iccc].cb().id();fflush(stdout);
              if( ilink->free() ){
                std::clog << "[*]";fflush(stdout);
              }
              std::clog << "[" << inode->cc_[iccc].iteration() << " "
                   << inode->cc()[iccc].tangents().size() << "]";fflush(stdout);
              if( ! ilink->begun() )
                std::clog << "[n]";fflush(stdout);
            }else{
              std::clog << inode->ccc()[iccc].ca().id() << "<->" << inode->ccc()[iccc].cc().id();fflush(stdout);
              if( ilink->free() ){
                std::clog << "[*]";fflush(stdout);
              }
              std::clog << "[" << inode->ccc_[iccc].iteration() << " "
                   << inode->ccc()[iccc].joints().size() << "]";fflush(stdout);
              if( ! ilink->begun() )
                std::clog << "[n]";fflush(stdout);
            }
          }

          std::clog << " chi2 = " << inode->chi2();fflush(stdout);

          std::clog << " )";fflush(stdout);

        }
      }

    std::clog << " center (" << sequence.center().x().value() << ", " << sequence.center().y().value() << ", " << sequence.center().z().value() << ")  radius " << sequence.radius().value() <<  " pitch " << sequence.pitch().value() << " momentum " << sequence.momentum().length().value() << "  tangent charge " << sequence.charge().value() << " +- " << sequence.charge().error() << " helix charge " << sequence.helix_charge().value()  << " +- " << sequence.helix_charge().error() << " detailed charge " << sequence.detailed_charge().value()  << " +- " << sequence.detailed_charge().error() << " chi2 " << sequence.chi2() << " prob " << sequence.Prob() << " helix chi2 " << sequence.helix_chi2() << " helix prob " << sequence.helix_Prob(); fflush(stdout);
    if( sequence.has_helix_vertex() ){
      std::clog << " helix vertex on " << sequence.helix_vertex_type();fflush(stdout);
      if( sequence.helix_vertex_type() == "calo" ) std::clog << " icalo " << sequence.helix_vertex_id();
    }
    if( sequence.has_decay_helix_vertex() ){
      std::clog << " helix decay vertex on " << sequence.decay_helix_vertex_type(); fflush(stdout);
      if( sequence.decay_helix_vertex_type() == "calo" ) std::clog << " icalo " << sequence.calo_helix_id();
    }
    if( sequence.has_tangent_vertex() ){
      std::clog << " tangent vertex on " << sequence.tangent_vertex_type() << " ";fflush(stdout);
      if( sequence.tangent_vertex_type() == "calo" ) std::clog << " icalo " << sequence.tangent_vertex_id();
    }
    if( sequence.has_decay_tangent_vertex() ){
      std::clog << " tangent decay vertex on " << sequence.decay_tangent_vertex_type(); fflush(stdout);
      if( sequence.decay_tangent_vertex_type() == "calo" ) std::clog << " icalo " << sequence.calo_tangent_id();
    }
    std::clog << " " << std::endl;fflush(stdout);

    return;

  }

  //*************************************************************
  void sequentiator::print_scenarios() const {
    //*************************************************************

    clog << "CAT::sequentiator::print_scenarios: Printing scenarios: " << scenarios_.size() << endl; fflush(stdout);

    for(vector<topology::scenario>::const_iterator isc=scenarios_.begin(); isc!=scenarios_.end(); ++isc)
      {
        std::clog << " scenario " << isc - scenarios_.begin() << std::endl;
        print_a_scenario(*isc);
      }

    return;

  }


  //*************************************************************
  void sequentiator::print_a_scenario(const topology::scenario & scenario) const {
    //*************************************************************

    clog << "Print associated sequences: " << endl;
    for (vector<topology::sequence>::const_iterator iseq = scenario.sequences_.begin();
         iseq != scenario.sequences_.end(); ++iseq)
      {
        print_a_sequence(*iseq);
      }

    clog << "Print scenario parameters" << endl;
    clog << " |-- nfree " << scenario.n_free_families() << endl;
    clog << " |-- noverls " << scenario.n_overlaps() << endl;
    clog << " |-- helix_chi2 " << scenario.helix_chi2() << endl;
    clog << " `-- helix_prob " << scenario.helix_Prob() << endl;

    return;
  }


  //*************************************************************
  void sequentiator::add_pair(const topology::sequence & newsequence){
    //*************************************************************

    clock.start(" sequentiator: add pair ", "cumulative");

    m.message("CAT::sequentiator::add_pair: Entering... ", mybhep::VVERBOSE); fflush(stdout);

    if (newsequence.nodes().size() != 2){
      m.message("CAT::sequentiator::add_pair: problem: pair has size ", newsequence.nodes().size(), mybhep::NORMAL);
      clock.stop(" sequentiator: add pair ");
      return;
    }

    topology::cell_couplet cc;
    if( !newsequence.nodes_[0].has_couplet(newsequence.nodes()[1].c(), &cc) ){
      m.message("CAT::sequentiator::add_pair: problem: node ", newsequence.nodes_[0].c().id(), " has no pair ", newsequence.nodes()[0].c().id(), "-", newsequence.nodes()[1].c().id(), mybhep::NORMAL);
      clock.stop(" sequentiator: add pair ");
      return;
    }

    topology::node na = newsequence.nodes()[0];
    topology::node nb = newsequence.nodes()[1];

    std::vector<topology::node> nodes;
    nodes.push_back(na);
    nodes.push_back(nb);

    bool erased = true;

    m.message("CAT::sequentiator::add_pair: n of tangents: ", cc.tangents_.size(), mybhep::VVERBOSE); fflush(stdout);
    topology::sequence pair(nodes, level, probmin);
    for(std::vector<topology::line>::iterator itangent=cc.tangents_.begin(); itangent != cc.tangents_.end(); ++itangent){

      pair.nodes_[0].set_ep(itangent->epa());
      pair.nodes_[0].set_free(false);

      pair.nodes_[1].set_ep(itangent->epb());
      pair.nodes_[1].set_free(false);

      if( itangent - cc.tangents_.begin() > 0 ){
        NCOPY ++;
        for(std::vector<topology::cell_couplet>::iterator icc=pair.nodes_[0].cc_.begin(); icc!=pair.nodes_[0].cc_.end(); ++icc)
          icc->set_all_used();
        for(std::vector<topology::cell_triplet>::iterator iccc=pair.nodes_[1].ccc_.begin(); iccc!=pair.nodes_[1].ccc_.end(); ++iccc)
          iccc->set_all_used();
      }

      clock.start(" sequentiator: set free level ","cumulative");
      pair.set_free_level();
      clock.stop(" sequentiator: set free level ");

      make_name(pair);
      sequences_.push_back(pair);

      if( level >= mybhep::VVERBOSE ){
	std::clog << "CAT::sequentiator::add_pair: made track [" << sequences_.size()-1 << "] with cells " << na.c().id() << " [" << pair.nodes_[0].ep().x().value() << ", " << pair.nodes_[0].ep().z().value() << "]" <<  "and" << nb.c().id() << " [" << pair.nodes_[1].ep().x().value() << ", " << pair.nodes_[1].ep().z().value() << "]" << std::endl;fflush(stdout);
      }

      if( erased )
        erased = clean_up_sequences();


    }

    clock.stop(" sequentiator: add pair ");
    return;

  }


  //*************************************************************
  bool sequentiator::clean_up_sequences(){
    //*************************************************************

    clock.start(" sequentiator: clean up sequences ", "cumulative");

    if( sequences_.size() < 2 ){
      clock.stop(" sequentiator: clean up sequences ");
      return false;
    }

    bool changed = false;

    std::vector<topology::sequence>::iterator iseq = sequences_.begin();

    while( iseq != sequences_.end() ){

      // compare iseq with last sequence
      if( (size_t)(iseq - sequences_.begin() + 1) >= sequences_.size() ){
        // iseq is last sequence
        // no need to check it against last sequence
        break;
      }

      m.message("CAT::sequentiator::clean_up_sequences: should we erase last sequence [", sequences_.size() - 1, "] " , sequences_.back().name() , " in favour of [", iseq - sequences_.begin(), "] " , iseq->name() , " ? " ,mybhep::VVERBOSE); fflush(stdout);
      if( !sequences_.back().contained( *iseq )){
        m.message("no, it's not contained " ,mybhep::VVERBOSE); fflush(stdout);
        //}else if( sequences_.back().Free() && !sequences_.back().contained_same_extreme_quadrants( *iseq ) ){
        //      m.message("no, it's a free sequence with different extreme quadrants " ,mybhep::VVERBOSE); fflush(stdout);
      }else if( sequences_.back().Free() ){
        m.message("CAT::sequentiator::clean_up_sequences:no, it's a free sequence " ,mybhep::VVERBOSE); fflush(stdout);
      }else
        {
          m.message("CAT::sequentiator::clean_up_sequences: erased last sequence ", sequences_.size() - 1, "  contained in sequence", iseq - sequences_.begin(), mybhep::VERBOSE); fflush(stdout);
          sequences_.pop_back();
          changed =  true;
          // now check again the same iseq against the new last sequence
          continue;
        }

      m.message("CAT::sequentiator::clean_up_sequences: should we erase sequence [", iseq - sequences_.begin(), "] " , iseq->name() , " in favour of the last [", sequences_.size() - 1, "] " , sequences_.back().name() , " ? " ,mybhep::VVERBOSE); fflush(stdout);
      if( !iseq->contained( sequences_.back() ) ){
        m.message("CAT::sequentiator::clean_up_sequences:no, it's not contained " ,mybhep::VVERBOSE); fflush(stdout);
        //}else if( iseq->Free() && !iseq->contained_same_extreme_quadrants( sequences_.back() ) ){
        //m.message("no, it's a free sequence with different extreme quadrants " ,mybhep::VVERBOSE); fflush(stdout);
      }else if( iseq->Free() ){
        m.message("CAT::sequentiator::clean_up_sequences:no, it's a free sequence" ,mybhep::VVERBOSE); fflush(stdout);
      }else
        {
          m.message("CAT::sequentiator::clean_up_sequences: erased sequence ", iseq - sequences_.begin(), "contained in last sequence", sequences_.size()-1, mybhep::VERBOSE); fflush(stdout);
          sequences_.erase(iseq);
          changed = true;
          iseq = sequences_.begin() + (iseq - sequences_.begin());
          // now check the new iseq against the same last sequence
          continue;
        }


      m.message("CAT::sequentiator::clean_up_sequences: should we erase sequence [", iseq - sequences_.begin(), "] " , iseq->name() , " because of connection out of range ? " ,mybhep::VVERBOSE); fflush(stdout);
      for(std::vector<topology::node>::iterator in=iseq->nodes_.begin(); in!=iseq->nodes_.end();in++){
        if( changed ) continue;
        if( in-iseq->nodes_.begin() +1 >= (int) iseq->nodes_.size()) break;
        topology::node nA = *in;
        topology::node nB = iseq->nodes_[in-iseq->nodes_.begin()+1];
        if( !sequence_is_within_range(nA,nB,*iseq) ){
          m.message("CAT::sequentiator::clean_up_sequences: erased sequence ", iseq - sequences_.begin(), " not in range", mybhep::VERBOSE); fflush(stdout);
          sequences_.erase(iseq);
          changed = true;
          iseq = sequences_.begin() + (iseq - sequences_.begin());
          // now check the new iseq
          break;
        }
      }



      // arrive here if no sequences have been deleted
      // now check (kseq, iseq, lastseq) for bridges

      std::vector<topology::sequence>::iterator kseq = sequences_.begin();

      while( kseq != sequences_.end() ){

        if( iseq==kseq ){
          ++ kseq;
          continue;
        }

        if( (size_t)(kseq - sequences_.begin() + 1) >= sequences_.size() ){
          // kseq is last sequence
          break;
        }

        if( (size_t)(iseq - sequences_.begin() + 1) >= sequences_.size() ){
          // iseq is last sequence
          break;
        }

        if( level >= mybhep::VVERBOSE ){
          std::clog << "CAT::sequentiator::clean_up_sequences: should we erase sequence ["<< kseq - sequences_.begin()<< "] " << kseq->name() << " as bridge between ["<< iseq - sequences_.begin()<< "] " << iseq->name()<< " and last ["<< sequences_.size() - 1<< "] "<< sequences_.back().name() << " ? " << std::endl; fflush(stdout);
        }
        if( kseq->is_bridge(*iseq, sequences_.back() ) &&
            (*kseq).nodes().size() < (*iseq).nodes().size() &&
            (*kseq).nodes().size() < sequences_.back().nodes().size()  &&
            !kseq->Free())
          {

            if( level >= mybhep::VVERBOSE ){
              std::clog <<"CAT::sequentiator::clean_up_sequences: erased sequence  ["<< kseq - sequences_.begin()<< "] " << kseq->name() << " as bridge between ["<< iseq - sequences_.begin()<< "] " << iseq->name()<< " and last ["<< sequences_.size() - 1<< "] "<< sequences_.back().name() << std::endl; fflush(stdout);
            }
            sequences_.erase(kseq);
            changed = true;
            kseq = sequences_.begin() + (kseq - sequences_.begin());
            if( kseq - sequences_.begin() < iseq - sequences_.begin() )
              iseq = sequences_.begin() + (iseq - sequences_.begin() - 1);
            else
              iseq = sequences_.begin() + (iseq - sequences_.begin());
            // now check the new kseq
            continue;
          }


        if( level >= mybhep::VVERBOSE ){
          std::clog <<"CAT::sequentiator::clean_up_sequences: should we erase last ["<< sequences_.size() - 1<< "] "<< sequences_.back().name() << " as bridge between ["<< iseq - sequences_.begin()<< "] " << iseq->name()<< " and  ["<< kseq - sequences_.begin()<< "] " << kseq->name() << " ? "<< std::endl; fflush(stdout);
        }
        if( sequences_.back().is_bridge(*iseq, *kseq ) &&
            sequences_.back().nodes().size() < (*iseq).nodes().size() &&
            sequences_.back().nodes().size() < (*kseq).nodes().size()  &&
            !sequences_.back().Free())
          {
            if( level >= mybhep::VVERBOSE ){
              std::clog <<"CAT::sequentiator::clean_up_sequences: erased  last ["<< sequences_.size() - 1<< "] "<< sequences_.back().name() << " as bridge between ["<< iseq - sequences_.begin()<< "] " << iseq->name()<< " and  ["<< kseq - sequences_.begin()<< "] " << kseq->name()<< std::endl; fflush(stdout);
            }
            sequences_.pop_back();
            changed =  true;
            iseq = sequences_.begin() + (iseq - sequences_.begin());
            // now check the same kseq with the new last sequence
            continue;
          }

        kseq ++;
        continue;
      }

      iseq++;
      continue;
    }


    clock.stop(" sequentiator: clean up sequences ");
    return changed;

  }

  //*************************************************************
  double sequentiator::distance_from_foil(const topology::experimental_point &ep){
    //*************************************************************

    if( SuperNemo )
      return fabs(ep.z().value());

    return fabs(ep.radius().value() - FoilRadius);

  }

  //*************************************************************
  bool sequentiator::direct_out_of_foil(void){
    //*************************************************************

    clock.start(" sequentiator: direct out of foil ", "cumulative");

    for(std::vector<topology::sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

      if( distance_from_foil(iseq->nodes().front().ep()) >
          distance_from_foil(iseq->nodes().back().ep()) ){
        m.message("CAT::sequentiator::direct_out_of_foil: sequence ", iseq - sequences_.begin(), " will be directed out of foil ", mybhep::VVERBOSE);
        topology::sequence is = iseq->invert();
        std::swap(*iseq, is);
      }
    }

    /*
      for(size_t iseq=0; iseq < sequences_.size(); iseq++){

      if( distance_from_foil(sequences_[iseq].nodes().front().ep()) <
      distance_from_foil(sequences_[iseq].nodes().back().ep()) ){
      m.message(" sequence ", iseq, " will be directed out of foil ", mybhep::VVERBOSE);
      print_a_sequence(&sequences_[iseq]);
      sequences_[iseq] = sequences_[iseq].invert();
      print_a_sequence(&sequences_[iseq]);

      }
      }

    */
    clock.stop(" sequentiator: direct out of foil ");


    return true;

  }


  //*************************************************************
  bool sequentiator::direct_scenarios_out_of_foil(void){
    //*************************************************************

    clock.start(" sequentiator: direct scenarios out of foil ", "cumulative");

    for(std::vector<topology::scenario>::iterator isc = scenarios_.begin(); isc != scenarios_.end(); ++isc){

      for(std::vector<topology::sequence>::iterator iseq = isc->sequences_.begin(); iseq != isc->sequences_.end(); ++iseq){

        if( distance_from_foil(iseq->nodes().front().ep()) >
            distance_from_foil(iseq->nodes().back().ep()) ){
          m.message("CAT::sequentiator::direct_scenarios_out_of_foil: sequence ", iseq - sequences_.begin(), " in scenario ", isc - scenarios_.begin(), " will be directed out of foil ", mybhep::VVERBOSE);
          topology::sequence is = iseq->invert();
          std::swap(*iseq, is);
        }
      }

    }
    clock.stop(" sequentiator: direct scenarios out of foil ");


    return true;

  }


  //*************************************************************
  bool sequentiator::there_is_free_sequence_beginning_with(const topology::cell &c, size_t *index) {
    //*************************************************************

    clock.start(" sequentiator: there is free sequence beginning with ", "cumulative");

    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq != sequences_.end(); ++iseq)
      if( iseq->nodes()[0].c().id() == c.id() )
        {
          if( iseq->Free() )
            {
              *index = iseq - sequences_.begin();
              clock.stop(" sequentiator: there is free sequence beginning with ");
              return true;
            }
        }

    clock.stop(" sequentiator: there is free sequence beginning with ");
    return false;

  }

  //*************************************************************
  bool sequentiator::near(const topology::cell &c, topology::calorimeter_hit &ch){
    //*************************************************************

    topology::plane pl = ch.pl();
    double chlayer = ch.layer();

    if( pl.view() == "x" ){

      m.message("CAT::sequentiator::near: matching cell ", c.id(), " with cell number ", c.cell_number(), " to calo on view ", pl.view(), " max cell number ", cell_max_number, " plane norm x ", pl.norm().x().value(), mybhep::VVERBOSE);

      if( pl.norm().x().value() > 0. )
        return (fabs(cell_max_number + c.cell_number()) < 1 + NOffLayers);

      return (fabs(cell_max_number - c.cell_number()) < 1 + NOffLayers);
    }
    else if( pl.view() == "y" ){
      m.message("CAT::sequentiator::near:  problem: matching cell to calo on view ", pl.view(), mybhep::NORMAL);
      return false;
    }
    else if( pl.view() == "z" ){

      int g = gap_number(c);

      m.message("CAT::sequentiator::near:  checking if cell ", c.id(), " on gap ", g, " is near plane: ", pl.center().x().value(), pl.center().y().value(), pl.center().z().value(), mybhep::VVERBOSE);

      if( g <= 0 || fabs(chlayer - c.layer()) > NOffLayers ) return false; // cell is not on a gap or is facing the foil

      if( g == 1 || fabs(chlayer - c.layer()) <= NOffLayers ) return true;

      m.message("CAT::sequentiator::near:  problem: can't match to calo on view ", pl.view(), mybhep::NORMAL);

      return false;
    }
    else if( pl.view() == "inner" ){

      int ln = c.layer();
      int g = gap_number(c);
      m.message("CAT::sequentiator::near:  checking if cell ", c.id(), " layer and gap: ", ln, g, " is near plane: ", pl.center().x().value(), pl.center().y().value(), pl.center().z().value(), " on view ", pl.view(), mybhep::VVERBOSE);
      if( ln < 0 && (g == 3 ||
                     fabs(ln - chlayer) <= NOffLayers )) return true;
      return false;
    }
    else if( pl.view() == "outer" ){
      int ln = c.layer();
      int g = gap_number(c);
      m.message("CAT::sequentiator::near:  checking if cell ", c.id(), " on layer and gap: ", ln, g, " is near plane: ", pl.center().x().value(), pl.center().y().value(), pl.center().z().value(), " on view ", pl.view(), mybhep::VVERBOSE);
      if( ln > 0 && (g == 3 ||
                     fabs(ln - chlayer) <= NOffLayers )) return true;
      return false;
    }
    else if( pl.view() == "top" ||  pl.view() == "bottom" ){
      int ln = c.layer();
      int g = gap_number(c);
      m.message("CAT::sequentiator::near:  checking if cell ", c.id(), " on gap ", g, " is near plane: ", pl.center().x().value(), pl.center().y().value(), pl.center().z().value(), " on view ", pl.view(), mybhep::VVERBOSE);
      if( ln > 0 && chlayer == 3.5 && (g == 1 || fabs(ln - chlayer) <= NOffLayers + 0.5 ) ) return true;
      if( ln > 0 && chlayer == 5.5 && (g == 2 || fabs(ln - chlayer) <= NOffLayers + 0.5 ) ) return true;
      if( ln < 0 && chlayer == -3.5 && (g == 1 || fabs(ln - chlayer) <= NOffLayers + 0.5 ) ) return true;
      if( ln < 0 && chlayer == -5.5 && (g == 2 || fabs(ln - chlayer) <= NOffLayers + 0.5 ) ) return true;
      return false;
    }

    m.message("CAT::sequentiator::near:  problem: can't match to calo on view ", pl.view(), mybhep::NORMAL);

    return false;

  }


  //*************************************************************
  int sequentiator::gap_number(const topology::cell &c){
    //*************************************************************

    // returns the index of the gap on which the hit is facing: 1, 2, 3
    // ... returns -1 if not on a gap
    // ... returns 0 if the hit is on layer 0, facing the foil

    size_t ln = abs(c.layer());

    m.message("CAT::sequentiator::gap_number: cell ", c.id(), " layer ", c.layer(), mybhep::VVERBOSE);

    if( ln == 0 ) return 0;

    size_t counter = 0;
    for(size_t i=0; i<planes_per_block.size(); i++){
      counter += (int)planes_per_block[i];  // 4, 6, 9

      if( ln == counter - 1 )  // layer = 3, 5, 8
        return (i + 1);  // gap = 1, 2, 3
      if( ln == counter )  // layer = 4, 6, 9
        return (i + 1);  // gap = 1, 2, 3

    }

    return -1;

  }



  //*******************************************************************
  void sequentiator::make_table_of_true_and_reco_sequences(std::vector<topology::sequence> &trueseqs){
    //*******************************************************************

    if( sequences_.empty() ) return;
    if(  trueseqs.empty() ) return;


    reco_sequence_of_true_.clear();
    true_sequence_of_reco_.clear();
    n_common_hits_for_reco_track_.clear();

    std::vector<size_t> max_common_hits_for_reco_track;
    max_common_hits_for_reco_track.assign (sequences_.size(), 0);
    std::vector<size_t> best_matching_for_reco_track;
    best_matching_for_reco_track.assign (sequences_.size(), 0);
    //size_t* max_common_hits_for_reco_track = (size_t*)mal loc(sizeof(size_t)*sequences_.size());
    //size_t* best_matching_for_reco_track = (size_t*)mal loc(sizeof(size_t)*sequences_.size());

    size_t NCommonMax;
    size_t imin;
    bool first;
    size_t ncommonhits;
    for(std::vector<topology::sequence>::iterator tp=trueseqs.begin(); tp != trueseqs.end(); ++tp){ // loop over true particles

      NCommonMax = 0;
      imin = 0;

      first = false;

      if( tp - trueseqs.begin() == 0 ) // first true particle
        first = true;

      for(std::vector<topology::sequence>::iterator ip = sequences_.begin(); ip != sequences_.end(); ++ip){      // loop over reco particles

        if( first ){
          max_common_hits_for_reco_track[ip - sequences_.begin()] = 0;
          best_matching_for_reco_track[ip - sequences_.begin()] = 0;
        }

        ncommonhits = getCommonHits(*tp, *ip);

        if( ncommonhits > NCommonMax ){
          NCommonMax = ncommonhits;
          imin = ip - sequences_.begin();
        }

        if( ncommonhits > max_common_hits_for_reco_track[ip - sequences_.begin()] ){
          max_common_hits_for_reco_track[ip - sequences_.begin()] = ncommonhits;
          best_matching_for_reco_track[ip - sequences_.begin()] = tp - trueseqs.begin();
        }

      }


      if( imin >= sequences_.size() ){
        m.message("CAT::sequentiator::make_table_of_true_and_reco_sequences: problem: imin", imin, "allparts size", sequences_.size(), mybhep::NORMAL);
        imin=0;
      }

      reco_sequence_of_true_.push_back(imin);

    }

    for(size_t i=0; i<sequences_.size(); i++){
      true_sequence_of_reco_.push_back(best_matching_for_reco_track[i]);
      n_common_hits_for_reco_track_.push_back(max_common_hits_for_reco_track[i]);
    }

    size_t index;
    if( level >= mybhep::VVERBOSE ){
      for(size_t i=0; i<sequences_.size(); i++){
        std::clog << "CAT::sequentiator::make_table_of_true_and_reco_sequences:  best matching for reco track " << i << " is true track " << true_sequence_of_reco_[i] << " with " << n_common_hits_for_reco_track_[i] << " common hits " << std::endl;
        std::clog << "CAT::sequentiator::make_table_of_true_and_reco_sequences:  hits of reco track: ";
        for(size_t j=0; j < sequences_[i].nodes().size(); j++){
          std::clog << " " << sequences_[i].nodes()[j].c().id();
        }
        std::clog << " " << std::endl; std::clog << " hits of true track: ";
        index = true_sequence_of_reco_[i];
        for(size_t j=0; j < trueseqs[index].nodes().size(); j++){
          std::clog << " " << trueseqs[index].nodes()[j].c().id();
        }
        std::clog << " " << std::endl;

      }
      for(size_t i=0; i<trueseqs.size(); i++)
        std::clog << "CAT::sequentiator::make_table_of_true_and_reco_sequences:  best matching for true track " << i << " is reco track " << reco_sequence_of_true_[i] << std::endl;
    }

    //free(max_common_hits_for_reco_track);
    //free(best_matching_for_reco_track);

    return;
  }

  //*******************************************************************
  void sequentiator::rec_efficiency(std::vector<topology::sequence> &trueseqs){
    //*******************************************************************

    if( sequences_.size() == 0 ) return;

    make_table_of_true_and_reco_sequences(trueseqs);
    size_t ireco;
    for(std::vector<topology::sequence>::iterator itrueseq=trueseqs.begin(); itrueseq != trueseqs.end(); ++itrueseq){

      if( !itrueseq->primary() ) continue;

      m.message("CAT::sequentiator::rec_efficiency: reconstructing efficiencies for true particle ", itrueseq - trueseqs.begin(), " of ", itrueseq->nodes().size(), " points, with name ", itrueseq->name(), mybhep::VVERBOSE);

      if( itrueseq->nodes().size() == 0 )
        continue;

      ireco = reco_sequence_of_true_[itrueseq - trueseqs.begin()];

      if( ireco >= sequences_.size() ){
        m.message("CAT::sequentiator::rec_efficiency:  problem: ireco", ireco, " size", sequences_.size(), mybhep::NORMAL);
      }

      topology::sequence reco = sequences_[ireco];

      FillGGResiduals(*itrueseq, reco);

      int nhits = reco.nodes().size();
      if( nhits > 0 ){

        int ncommon = n_common_hits_for_reco_track_[ireco];

        double purity = (double)ncommon/(double)nhits;

        double efficiency = (double)ncommon/(double)(itrueseq->nodes().size());

        m.message("CAT::sequentiator::rec_efficiency:  ... n reco hits ", nhits, " n true hits ", itrueseq->nodes().size(), " n common hits ", ncommon, " TTPurity ", purity, " TTEfficiency ", efficiency, mybhep::VVERBOSE);

        /*
                if( PrintMode ){
                double ctheta  = itrueseq->initial_dir().z().value();

                hman.fill("HTTPurity", purity );
                hman.fill("HTTEfficiency", efficiency );

                hman.fill("PurityVsTheta", ctheta, purity );
                hman.fill("EfficiencyVsTheta", ctheta, efficiency );
                }
        */

      }
    }

    return;
  }


  //*******************************************************************
  size_t sequentiator::getCommonHits(topology::sequence &tp, topology::sequence &dp){
    //*******************************************************************

    size_t counter = 0;

    for(std::vector<topology::node>::iterator dnode = dp.nodes_.begin(); dnode != dp.nodes_.end(); ++dnode){

      for(std::vector<topology::node>::iterator tnode = tp.nodes_.begin(); tnode != tp.nodes_.end(); ++tnode){

        if( tnode->c_.same_cell(dnode->c_) ){
          counter ++;
          break;
        }
      }
    }

    return counter;

  }

  //*******************************************************************
  void sequentiator::FillGGResiduals(topology::sequence & /*tp*/, topology::sequence &/*dp*/){
    //*******************************************************************

    /*
      if( PrintMode ){

      topology::helix he = dp.get_helix();

      for(std::vector<topology::node>::iterator dn = dp.nodes_.begin(); dn != dp.nodes_.end(); ++dn){

      topology::node jnode = *dn;

      for(std::vector<topology::node>::iterator tn = tp.nodes_.begin(); tn != tp.nodes_.end(); ++tn){

      topology::node inode = *tn;


      if( tn->c_.same_cell(dn->c_) ){

      topology::experimental_double residualx = jnode.ep().x() - inode.ep().x();
      topology::experimental_double residualy = jnode.ep().y() - inode.ep().y();
      topology::experimental_double residualz = jnode.ep().z() - inode.ep().z();

      hman.fill("GGresx",residualx.value());
      hman.fill("GGresy",residualy.value());
      hman.fill("GGresz",residualz.value());

      topology::experimental_point hp = he.position(jnode.ep());
      topology::experimental_double Hresidualx = hp.x() - inode.ep().x();
      topology::experimental_double Hresidualy = hp.y() - inode.ep().y();
      topology::experimental_double Hresidualz = hp.z() - inode.ep().z();

      hman.fill("GGHelixresx",Hresidualx.value());
      hman.fill("GGHelixresy",Hresidualy.value());
      hman.fill("GGHelixresz",Hresidualz.value());

      hman.fill("GGpullx",residualx.value()/residualx.error());
      hman.fill("GGpully",residualy.value()/residualy.error());
      hman.fill("GGpullz",residualz.value()/residualz.error());
      break;
      }
      }
      }

      if( tp.nodes().size() > 1 && dp.nodes().size() > 1 ){
      for(std::vector<topology::node>::iterator tn = tp.nodes_.begin(); tn != tp.nodes_.end(); ++tn){

      if( tn - tp.nodes_.begin() == 0 ) advance(tn, 1);

      topology::node inode = *tn;
      std::vector<topology::node>::iterator tnold = tp.nodes_.begin();
      advance(tnold, tn - tp.nodes_.begin() - 1);
      topology::node inodeold = *tnold;

      bool connected = false;

      for(std::vector<topology::node>::iterator dn = dp.nodes_.begin(); dn != dp.nodes_.end(); ++dn){
      if( dn - dp.nodes_.begin() == 0 ) advance(dn, 1);

      topology::node jnode = *dn;
      std::vector<topology::node>::iterator dnold = dp.nodes_.begin();
      advance(tnold, dn - dp.nodes_.begin() - 1);
      topology::node jnodeold = *dnold;



      if( ( inode.c_.same_cell(jnode.c_) &&
      inodeold.c_.same_cell(jnodeold.c_) ) ||
      ( inode.c_.same_cell(jnodeold.c_) &&
      inodeold.c_.same_cell(jnode.c_) )){
      connected = true;
      break;
      }
      }

      hman.fill("TrueConnections",0.5);
      if( connected )
      hman.fill("RecoConnections",0.5);

      }
      }


      topology::experimental_double vtx = dp.vertex().x() - tp.vertex().x();
      topology::experimental_double vty = dp.vertex().y() - tp.vertex().y();
      topology::experimental_double vtz = dp.vertex().z() - tp.vertex().z();
      double ctheta  = tp.initial_dir().z().value();
      hman.fill("VertexResx",vtx.value());
      hman.fill("VertexResy",vty.value());
      hman.fill("VertexResz",vtz.value());
      hman.fill("VertexResxVsTheta",ctheta, vtx.value());
      hman.fill("VertexResyVsTheta",ctheta, vty.value());
      hman.fill("VertexReszVsTheta",ctheta, vtz.value());
      hman.fill("VertexPullx",vtx.value()/vtx.error());
      hman.fill("VertexPully",vty.value()/vty.error());
      hman.fill("VertexPullz",vtz.value()/vtz.error());

      topology::experimental_double dvtx = dp.decay_vertex().x() - tp.decay_vertex().x();
      topology::experimental_double dvty = dp.decay_vertex().y() - tp.decay_vertex().y();
      topology::experimental_double dvtz = dp.decay_vertex().z() - tp.decay_vertex().z();
      hman.fill("DVertexResx",dvtx.value());
      hman.fill("DVertexResy",dvty.value());
      hman.fill("DVertexResz",dvtz.value());
      hman.fill("DVertexResxVsTheta",ctheta, dvtx.value());
      hman.fill("DVertexResyVsTheta",ctheta, dvty.value());
      hman.fill("DVertexReszVsTheta",ctheta, dvtz.value());
      hman.fill("DVertexPullx",dvtx.value()/dvtx.error());
      hman.fill("DVertexPully",dvty.value()/dvty.error());
      hman.fill("DVertexPullz",dvtz.value()/dvtz.error());


      double Px = dp.momentum().x().value() - tp.momentum().x().value();
      double Py = dp.momentum().y().value() - tp.momentum().y().value();
      double Pz = dp.momentum().z().value() - tp.momentum().z().value();

      double Pp = dp.momentum().value() - tp.momentum().value();

      hman.fill("PResx",Px);
      hman.fill("PResy",Py);
      hman.fill("PResz",Pz);

      hman.fill("PRes",Pp);

      hman.fill("QRes",dp.charge().value() - tp.charge().value());
      hman.fill("QResVsTheta",ctheta, dp.charge().value() - tp.charge().value());

      }

    */
    return;

  }


  //*************************************************************
  bool sequentiator::sequence_is_within_range(topology::node nodeA, topology::node nodeB, topology::sequence seq){
    //*************************************************************

    if( gaps_Z.size() == 0 ) return true;

    topology::experimental_point epA = nodeA.ep();
    topology::experimental_point epB = nodeB.ep();
    topology::cell cA = nodeA.c();
    topology::cell cB = nodeB.c();

    int gnA = gap_number(cA);
    int gnB = gap_number(cB);
    int blockA = cA.block();
    int blockB = cB.block();

    double rmin, rmax;

    topology::experimental_point ep_maxr, ep_minr;

    // get the index of the gap through which the matching should occurr
    if( blockA == blockB &&
        ((gnA == -1 && gnB == -1) ||
         gnA != gnB )){
      // matching is within a single block and not through a gap
      // so helix should be contained in the layers of the two cells
      rmin = min(cA.ep().radius().value(),cB.ep().radius().value()) - CellDistance;
      rmax = max(cA.ep().radius().value(),cB.ep().radius().value()) + CellDistance;
    }else{
      if( blockA != blockB ){

        /*
        if( gnA != -1 && gnB != -1 && gnA != gnB ){
          if( level >= mybhep::VVERBOSE ){
            std::clog << " connection between cells " << nodeA.c().id() << " and " << nodeB.c().id() << " blocks " << blockA << " and " << blockB << " gaps " << gnA << " and " << gnB << " is forbidden (cells face different gaps) " << std::endl;
          }
          return false;
        }
        */

        // matching is between different blocks
        rmin = min(cA.ep().radius().value(),cB.ep().radius().value()) - CellDistance;
        rmax = max(cA.ep().radius().value(),cB.ep().radius().value()) + CellDistance;
      }
      else if( blockA == blockB && gnA == -1 && gnB != -1 ){ // B is on gap, A is inside
        size_t gn = gnB;
        if( cA.ep().radius().value() > cB.ep().radius().value() ){ // gap - B - A
          rmax = cA.ep().radius().value() + CellDistance;
          rmin = cB.ep().radius().value() - CellDistance - gaps_Z[gn];
        }else{ // A - B - gap
          rmin = cA.ep().radius().value() - CellDistance;
          rmax = cB.ep().radius().value() + CellDistance + gaps_Z[gn];
        }
      }
      else if( blockA == blockB && gnA != -1 && gnB == -1 ){ // A is on gap, B is inside
        size_t gn = gnA;
        if( cA.ep().radius().value() < cB.ep().radius().value() ){ // gap - A - B
          rmax = cB.ep().radius().value() + CellDistance;
          rmin = cA.ep().radius().value() - CellDistance - gaps_Z[gn];
        }else{ // B - A - gap
          rmin = cB.ep().radius().value() - CellDistance;
          rmax = cA.ep().radius().value() + CellDistance + gaps_Z[gn];
        }
      }
      else if( blockA == blockB && gnA != -1 && gnB != -1 && gnA == gnB ){ // A and B on same gap

        // get the index of the gap through which the matching should occurr

        size_t gn = gnA;
        size_t gaplayer=0;
        for(size_t i=0; i<gn; i++)
          gaplayer += (size_t)(planes_per_block[i]+0.5);  // 0, 4, 6, 9
        if( abs(cA.layer()) < (int)gaplayer ){ // foil - A,B - gap
          if( blockA > 0 ){ // origin - A, B - gap
            rmin = cA.ep().radius().value() - CellDistance;
            rmax = cA.ep().radius().value() + CellDistance + gaps_Z[gn];
          }else{ // origin - gap - A,B
            rmin = cA.ep().radius().value() - CellDistance - gaps_Z[gn];
            rmax = cA.ep().radius().value() + CellDistance;
          }
        }else{ // foil - gap - A, B
          if( blockA > 0 ){ // origin - gap - A,B
            rmax = cA.ep().radius().value() + CellDistance;
            rmin = cA.ep().radius().value() - CellDistance - gaps_Z[gn];
          }else{ // origin - A,B - gap
            rmin = cA.ep().radius().value() - CellDistance;
            rmax = cA.ep().radius().value() + CellDistance + gaps_Z[gn];
          }
        }

      }
      else{
        std::clog << "CAT::sequentiator::sequence_is_within_range: problem: blockA " << blockA << " blockB " << blockB << " gnA " << gnA << " gnB " << gnB << std::endl;
        return true;
      }
    }

    seq.point_of_max_min_radius(epA, epB, &ep_maxr, &ep_minr);

    if( ep_maxr.radius().value() > rmax || ep_minr.radius().value() < rmin ){
      if( level >= mybhep::VVERBOSE ){
        std::clog << "CAT::sequentiator::sequence_is_within_range:  sequence penetrates outside of admissible range between cells " << nodeA.c().id() << " layer " << nodeA.c().layer() << " r " << nodeA.c().ep().radius().value() << " and " << nodeB.c().id() << " layer " << nodeB.c().layer() << " r " << nodeB.c().ep().radius().value() << ": point of max radius has radius " << ep_maxr.radius().value() << " rmax " << rmax << " point of min radius has radius " << ep_minr.radius().value() << " rmin " << rmin << std::endl;
      }
      return false;
    }

    if( level >= mybhep::VVERBOSE ){
      std::clog << "CAT::sequentiator::sequence_is_within_range:  sequence blockA " << blockA << " blockB " << blockB << " gnA " << gnA << " gnB " << gnB << " connecting cells " << nodeA.c().id() << " and " << nodeB.c().id() << ": point of max radius has radius " << ep_maxr.radius().value() << " rmax " << rmax << " point of min radius has radius " << ep_minr.radius().value() << " rmin " << rmin << std::endl;
    }

    return true;

  }

  //*************************************************************
  bool sequentiator::good_first_to_be_matched(topology::sequence& seq){
    //*************************************************************

    if( !seq.fast() ) return false;

    if( seq.nodes().size() < 2 )
      {
        m.message("CAT::sequentiator::good_first_to_be_matched: SQ user: not a good first, because hits size is", seq.nodes().size(), mybhep::VERBOSE);
        return false;
      }

    return true;

  }

  //*************************************************************
  bool sequentiator::match_gaps(std::vector<topology::calorimeter_hit> & calos){
    //*************************************************************

    //if( gaps_Z.size() <= 1 ) return true;
    if( sequences_.size() < 2 ) return true;

    clock.start(" sequentiator: match gaps ", "cumulative");

    if( level >= mybhep::VERBOSE){
      print_families();
      print_sequences();
    }

    std::vector<bool> matched;
    matched.assign (families_.size(), false);

    std::vector<topology::sequence> newseqs;

    size_t ifam;
    size_t jmin;
    bool invertA, invertB, first;
    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){

      if( late() )
        return false;

      ifam = mybhep::int_from_string(iseq->family());

      if( matched[ifam] ) continue;

      if( !good_first_to_be_matched(*iseq) ){
        continue;
      }

      m.message("CAT::sequentiator::match_gaps: begin matching with sequence ", iseq->name(), mybhep::VERBOSE);
      if( level >= mybhep::VERBOSE)
        print_a_sequence(*iseq);

      first = true;
      topology::sequence newseq = *iseq;
      int with_kink = 0;
      int cells_to_delete = 0;
      while( can_match(newseq, &jmin, invertA, invertB, with_kink, cells_to_delete, calos) )
        {
          m.message("CAT::sequentiator::match_gaps:  best matching is ", sequences_[jmin].name(), " invertA ", invertA, " invertB ", invertB, " with kink ", with_kink, " delete cells ", cells_to_delete, mybhep::VERBOSE);
          if( level >= mybhep::VVERBOSE)
            print_a_sequence(sequences_[jmin]);

          bool ok;
          newseq = newseq.match(sequences_[jmin], invertA, invertB, &ok, with_kink,cells_to_delete);

          if( !ok && !with_kink ){
            m.message("CAT::sequentiator::match_gaps:  ... no good helix match ", mybhep::VERBOSE);
            continue;
          }

          if( level >= mybhep::VERBOSE)
            print_a_sequence(newseq);

          if( first ){
            //      matched[ifam] = true;
            first = false;
          }
          size_t ifa = mybhep::int_from_string(sequences_[jmin].family());
          matched[ifa] = true;
          m.message("CAT::sequentiator::match_gaps: setting family ", ifa, " as used for matching", mybhep::VERBOSE);

        }

      if( !first )
        newseqs.push_back(newseq);

    }
    if( late() )
      return false;

    m.message("CAT::sequentiator::match_gaps: made matching through gaps ", mybhep::VERBOSE);

    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){
      if( !matched[mybhep::int_from_string(iseq->family())] )
        newseqs.push_back(*iseq);
    }

    set_sequences(newseqs);
    make_families();

    //free(matched);

    if( level >= mybhep::VERBOSE){
      print_sequences();
    }

    return true;

  }

  //*************************************************************
  bool sequentiator::can_match(topology::sequence &s, size_t* jmin, bool& bestinvertA, bool& bestinvertB, int &with_kink, int &cells_to_delete_best, std::vector<topology::calorimeter_hit> & calos) {
    //*************************************************************

    if( late() )
      return false;

    clock.start(" sequentiator: can match ", "cumulative");

    bool ok = false;
    double limit_diagonal = sqrt(2.)*cos(M_PI/8.)*CellDistance;

    double probmax = -1.;
    double chi2min = mybhep::default_min;
    int ndofbest = 1;
    int cells_to_delete;

    m.message("CAT::sequentiator::can_match: try to match sequence", s.name(), " of chi2 = ", chi2min, " ndof ", ndofbest, " prob ", probmax, mybhep::VVERBOSE);
    bool invertA, invertB, acrossGAP;
    double p;
    double c;
    int n;
    topology::node nodeA, nodeB;

    for(std::vector<topology::sequence>::iterator jseq=sequences_.begin(); jseq!=sequences_.end(); ++jseq)
      {

        cells_to_delete = 0;
        with_kink=0;

        m.message("CAT::sequentiator::can_match:  try to match sequence", s.name(), " to ", jseq->name(), mybhep::VVERBOSE);
        if( level >= mybhep::VVERBOSE){
          print_a_sequence(s);
          print_a_sequence(*jseq);
        }

        // check that jseq's family is not the same as s
        if( s.same_families(*jseq) ){
          m.message("CAT::sequentiator::can_match:  ... forbidden, same family", jseq->family(), mybhep::VVERBOSE);
          continue;
        }

        // match along helix
        bool ok_match = s.good_match(*jseq, invertA, invertB, NOffLayers);
        bool ok_kink_match=false;
        bool ok_kink_match_chi2 = false;
        topology::sequence news;
        if( ok_match )
          news = s.match(*jseq, invertA, invertB, &ok_match,with_kink,0);

        if( !invertA )
          nodeA = s.last_node();
        else
          nodeA = s.nodes_[0];

        if( !invertB )
          nodeB = jseq->nodes_[0];
        else
          nodeB = jseq->last_node();

        ok_match = ok_match && sequence_is_within_range(nodeA, nodeB, news);

        if( ok_match ){
          p = news.helix_Prob();
          c = news.helix_chi2();
          n = news.ndof();

          m.message("CAT::sequentiator::can_match:  ... matched to ", jseq->name(), ", chi2 =", c, " ndof ", n, " prob ", p, mybhep::VVERBOSE);
        }

        if( ok_match && (p > news.probmin()))
          m.message("CAT::sequentiator::can_match:  good helix match", mybhep::VVERBOSE);
        else{
          m.message("CAT::sequentiator::can_match:  ... no good helix match, try to match with kink ", mybhep::VVERBOSE);

          ok_kink_match= s.good_match_with_kink(*jseq, invertA, invertB, acrossGAP, limit_diagonal, NOffLayers, cells_to_delete);
          if( !ok_kink_match ){
            m.message("CAT::sequentiator::can_match:  ... obviously no good match with kink ", mybhep::VVERBOSE);
          }
          else{
            // do not match with kink if the extreme is near a calo or near the foil

            if( !invertA ){
              if( cells_to_delete == 0 || cells_to_delete == 1 )
                nodeA = s.last_node();
              else
                nodeA = s.second_last_node();
            }
            else{
              if( cells_to_delete == 0 || cells_to_delete == 1 )
                nodeA = s.nodes_[0];
              else
                nodeA = s.nodes_[1];
            }

            if( !invertB ){
              if( cells_to_delete != 1 )
                nodeB = jseq->nodes_[0];
              else
                nodeB = jseq->nodes_[1];
            }else{
              if( cells_to_delete != 1 )
                nodeB = jseq->last_node();
              else
                nodeB = jseq->second_last_node();
            }

            m.message("CAT::sequentiator::can_match:  possible match with kink, across GAP", acrossGAP, ", cells to delete ", cells_to_delete, ", try to extrapolate ", mybhep::VVERBOSE);
            for(std::vector<topology::calorimeter_hit>::iterator ic=calos.begin(); ic != calos.end(); ++ic){
              if( near(nodeA.c(), *ic) ||  near(nodeB.c(), *ic) ){
                ok_kink_match = false;
                m.message("CAT::sequentiator::can_match:  will not match with kink because end cell is near calo ", ic - calos.begin(), mybhep::VVERBOSE);
                break;
              }
            }
            if( gap_number(nodeA.c() ) == 0 || gap_number(nodeB.c() ) == 0 ){
              ok_kink_match = false;
              m.message("CAT::sequentiator::can_match:  will not match with kink because end cell is near foil ", mybhep::VVERBOSE);
            }

            topology::experimental_point kink_point;
            ok_kink_match = ok_kink_match && s.intersect_sequence(*jseq, invertA, invertB, acrossGAP, &kink_point, limit_diagonal, &with_kink, cells_to_delete);

            if( ok_kink_match ){

              news = s.match(*jseq, invertA, invertB, &ok_kink_match_chi2, with_kink,cells_to_delete);
              ok_kink_match = sequence_is_within_range(nodeA, nodeB, news);

              if( ok_kink_match )
                m.message("CAT::sequentiator::can_match:  good kink match ( ", kink_point.x().value(), ", ", kink_point.y().value(), ", ", kink_point.z().value(), ") to sequence ", jseq->name(), mybhep::VVERBOSE);

            }else{
              m.message("CAT::sequentiator::can_match:  no good kink match to sequence ", jseq->name(), mybhep::VVERBOSE);
            }
          }
        }

        if( !ok_match && !ok_kink_match ) continue;

        p = news.helix_Prob();
        c = news.helix_chi2();
        n = news.ndof();

        if( level >= mybhep::VVERBOSE){
          std::clog << "CAT::sequentiator::can_match:  ... matched to " << jseq->name() << ", chi2 =" << c << " ndof " << n << " prob " << p << " with kink " << with_kink << " cells_to_delete " << cells_to_delete << " probmax " << probmax << " chimin " << chi2min << std::endl;
        }

        if( (p > probmax || (p == probmax && c < chi2min ))  &&
            ((ok_match && p > news.probmin()) || ok_kink_match ) )
          {
            *jmin = jseq - sequences_.begin();
            probmax = p;
            chi2min = c;
            ndofbest = n;
            bestinvertA = invertA;
            bestinvertB = invertB;
            cells_to_delete_best = cells_to_delete;
            ok = true;
          }
      }

    if( ok ){
      if( level >= mybhep::VVERBOSE){
std::clog << "CAT::sequentiator::can_match:  sequence " << s.name() << " can be matched to " << sequences_[*jmin].name() << ", chi2 =" << chi2min << " ndof " << ndofbest << " prob " << probmax << " cells_to_delete " << cells_to_delete << std::endl;
      }
    }

    clock.stop(" sequentiator: can match ");
    return ok;

  }


  //*************************************************************
  bool sequentiator::select_nemo_tracks(topology::tracked_data & __tracked_data){
    //*************************************************************

    if( SuperNemo ) return true;

    std::vector<topology::sequence> nemo_sequences = __tracked_data.get_nemo_sequences();

    m.message("CAT::sequentiator::select_nemo_track: selecting events based on nemo tracks ", mybhep::VVERBOSE);

    if( nemo_sequences.size() != 2 ){
      m.message("CAT::sequentiator::select_nemo_track: reject: ntracks = ", nemo_sequences.size(), mybhep::VVERBOSE);
      return false;
    }
    m.message("CAT::sequentiator::select_nemo_track:  ntracks = ", nemo_sequences.size(), mybhep::VVERBOSE);

    if( !nemo_sequences[0].has_decay_helix_vertex() ){
      m.message("CAT::sequentiator::select_nemo_track: reject: 1st track has no calo ", mybhep::VVERBOSE);
      return false;
    }


    if( !nemo_sequences[1].has_decay_helix_vertex() ){
      m.message("CAT::sequentiator::select_nemo_track: reject: 2nd track has no calo ", mybhep::VVERBOSE);
      return false;
    }

    std::vector<topology::calorimeter_hit> calos = __tracked_data.get_calos();

    if( nemo_sequences[0].calo_helix_id() == nemo_sequences[1].calo_helix_id() ){
      m.message("CAT::sequentiator::select_nemo_track: reject: same calo ", nemo_sequences[0].calo_helix_id(), mybhep::VVERBOSE);
      return false;
    }

    topology::calorimeter_hit caloA = calos[nemo_sequences[0].calo_helix_id()];
    topology::calorimeter_hit caloB = calos[nemo_sequences[1].calo_helix_id()];

    if( caloA.e().value() < 0.2 ){
      m.message("CAT::sequentiator::select_nemo_track: reject: 1st calo has energy ", caloA.e().value(), mybhep::VVERBOSE);
      return false;

    }

    if( caloB.e().value() < 0.2 ){
      m.message("CAT::sequentiator::select_nemo_track: reject: 2nd calo has energy ", caloA.e().value(), mybhep::VVERBOSE);
      return false;
    }


    return true;

  }

} // end of namespace CAT
