#include "sequentiator.h"
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
    nsigma = std::numeric_limits<double>::quiet_NaN ();
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

    if (st.find_dstore("nsigma"))
      nsigma=st.fetch_dstore("nsigma");
    else
      nsigma=10.;

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
  bool sequentiator::initialize( mybhep::sstore store, mybhep::gstore gs , mybhep::EventManager2 *eman) {
    //*************************************************************

    m.message("\n Beginning algorithm sequentiator \n",mybhep::VERBOSE); fflush(stdout);

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

    m.message("\n Beginning algorithm sequentiator \n",mybhep::VERBOSE); fflush(stdout);

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

    m.message("\n Ending algorithm sequentiator \n ",mybhep::NORMAL); fflush(stdout);

    m.message("Initial events: ", InitialEvents, mybhep::NORMAL);
    m.message("Skipped events: ", SkippedEvents, "(", 100.*SkippedEvents/InitialEvents, "\%)", mybhep::NORMAL);

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

    m.message("Writing histograms in ", hfile, mybhep::NORMAL);

    return;
  }

  //*************************************************************
  void sequentiator::readDstProper(mybhep::sstore global, mybhep::EventManager2 *eman) {
    //*************************************************************

    clock.start(" sequentiator: read dst properties ");

    if (! global.find("GEOM_MODULES"))
      {
        _MaxBlockSize = 1;
        m.message(" no bar design, MUDULES Nr set to = ",_MaxBlockSize,"\n",mybhep::VERBOSE);
        fflush(stdout);
      }
    else
      {
        _MaxBlockSize = mybhep::int_from_string(global.fetch("GEOM_MODULES"));
        m.message(" bar design with MUDULES = ",_MaxBlockSize,"\n",mybhep::VERBOSE);
        fflush(stdout);
      }

    if(!global.find("NEMO3"))
      {
        SuperNemo = true;
        m.message("SuperNemo kind of data",mybhep::VERBOSE); fflush(stdout);
      }
    else
      {
        SuperNemo = false;
        m.message("Nemo-3 kind of data",mybhep::VERBOSE); fflush(stdout);
      }

    if(global.find("NEMORA_model"))
      {
        NemoraOutput = true;
        m.message("data already processed by Nemora",mybhep::VERBOSE); fflush(stdout);
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

    m.message("pmax",pmax,"MeV",mybhep::VERBOSE); fflush(stdout);
    m.message("",mybhep::VERBOSE); fflush(stdout);
    m.message("small radius",SmallRadius,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("tangent phi",TangentPhi,mybhep::VERBOSE); fflush(stdout);
    m.message("tangent theta",TangentTheta,mybhep::VERBOSE); fflush(stdout);
    m.message("small number",SmallNumber,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("quadrant angle",QuadrantAngle,mybhep::VERBOSE); fflush(stdout);
    m.message("ratio",Ratio,mybhep::VERBOSE); fflush(stdout);
    m.message("compatibility distance", CompatibilityDistance,mybhep::VERBOSE); fflush(stdout);
    m.message("maximum chi2", MaxChi2, mybhep::VERBOSE); fflush(stdout);
    m.message("maximum time", MaxTime, " ms ", mybhep::NORMAL); fflush(stdout);
    m.message("",mybhep::VERBOSE); fflush(stdout);
    m.message("xsize is read as",xsize,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("ysize is read as",ysize,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("zsize is read as",zsize,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("plasma speed is read as: ",vel,"mm/ns",mybhep::VERBOSE); fflush(stdout);
    m.message("wire length is read as: ",len,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("cell diameter is read as: ",rad,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("distance from wire to wire is read as: ",CellDistance,"mm",mybhep::VERBOSE); fflush(stdout);
    m.message("",mybhep::VERBOSE); fflush(stdout);

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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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

    m.message(" local_tracking: preparing event", event_number, mybhep::VERBOSE);
    event_number ++;

    if( event_number < first_event_number ) return false;

    clock.start(" sequentiator: sequentiate ","cumulative");
    clock.start(" sequentiator: sequentiation ","restart");

    m.message(" sequentiate ", mybhep::VVERBOSE); fflush(stdout);

    // set_clusters(tracked_data_.get_clusters());
    vector<topology::cluster> & the_clusters = tracked_data_.get_clusters ();

    NFAMILY = -1;
    NCOPY = 0;

    // if (clusters_.empty ()) return true;
    if (the_clusters.empty ()) return true;

    sequences_.clear();
    scenarios_.clear();

    tracked_data_.scenarios_.clear();

    for (vector<topology::cluster>::iterator
          icluster = the_clusters.begin();
        icluster != the_clusters.end(); ++icluster)
      {
        topology::cluster & a_cluster = *icluster;

        // 2012/06/26 XG : Make a copy of pointer better than a copy
        // by value
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

    make_families();

    interpret_physics(tracked_data_.get_calos());

    // //  match_through_gaps();
    match_gaps();
    if (late())
      {
        tracked_data_.set_skipped(true);
	SkippedEvents ++;
        return false;
      }

    make_scenarios(tracked_data_);

    match_kinks(tracked_data_);

    if (late())
      {
        tracked_data_.set_skipped(true);
	SkippedEvents ++;
        return false;
      }

    // make_plots(tracked_data_);

    clock.stop(" sequentiator: sequentiate ");

    return true;
  }


  //*************************************************************
  void sequentiator::plot_hard_scattering(topology::tracked_data __tracked_data){
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
  void sequentiator::make_plots(topology::tracked_data __tracked_data){
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
            clog << "\n first node: " << inode->c().id();
          }

        // 2012/06/27 XG: Not used anymore
        // topology::node first_node;

        if (!good_first_node(a_node)) continue;

        make_new_sequence(a_node);

        if (late()) return;

        make_copy_sequence(a_node);
      }

    return;
  }


  //*************************************************************
  void sequentiator::make_new_sequence(topology::node & first_node){
    //*************************************************************

    if (late()) return;

    clock.start(" sequentiator: make new sequence ","cumulative");

    //  A node is added to the newsequence. It has the given cell but no other
    //  requirement. The free level is set to true.
    topology::sequence newsequence(first_node, level, nsigma);

    if (level >= mybhep::VERBOSE)
      {
        clog << " begin track [" << sequences_.size() << "] with node " << first_node.c().id() << endl;
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
        m.message(" finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
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

        m.message("\n begin, with cell", first_node.c().id(), ", parallel track ", sequences_.size(), " to track ", isequence, mybhep::VERBOSE); fflush(stdout);

        if (level >= mybhep::VVERBOSE)
          {
            std::clog << " original sequence before copying: " << std::endl;
            print_a_sequence(sequences_[isequence]);
          }

        clock.stop(" sequentiator: make copy sequence: part A: alpha ");
        clock.start(" sequentiator: copy to lfn ","cumulative");
        size_t ilink, ilfn;
        topology::sequence newcopy = sequences_[isequence].copy_to_last_free_node(&ilfn, &ilink);
        clock.stop(" sequentiator: copy to lfn ");

        clock.start(" sequentiator: make copy sequence: part A: beta ","cumulative");
        m.message(" copied from sequence  ", isequence, mybhep::VVERBOSE); fflush(stdout);

        if (level >= mybhep::VVERBOSE)
          {
            print_a_sequence(sequences_[isequence]);
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
            m.message(" not adding new sequence, since it couldn't evolve past lfn ", mybhep::VERBOSE); fflush(stdout);
            clean_up_sequences();
          }
        else
          {
            // if copy has evolved past level 1, the nodes 0 and 1 it used
            // are set to used in the original
            if( newcopy.nodes().size() > 1 ){
              if( !sequences_[isequence].nodes().empty() ){
                clock.start(" sequentiator: get link index ","cumulative");
                size_t it1 = newcopy.get_link_index_of_cell(0, newcopy.nodes()[1].c());
                clock.stop(" sequentiator: get link index ");
                m.message(" setting as used original node 0  cc ", it1, mybhep::VVERBOSE);
                sequences_[isequence].nodes_[0].cc_[it1].set_all_used();
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
              m.message(" not adding new sequence, contained in ", isequence, "from which it was copied", mybhep::VERBOSE); fflush(stdout);
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
                  m.message(" erased original sequence ", isequence, "contained in sequence", sequences_.size()+1, "which was copied from it", mybhep::VERBOSE); fflush(stdout);
                  clean_up_sequences();

                }

              NCOPY++;
              if (newcopy.nodes().size() != 2)
                {
                  make_name(newcopy);
                  sequences_.push_back( newcopy );
                  m.message(" finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
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
  bool sequentiator::late(void){
    //*************************************************************

    if( clock.read(" sequentiator: sequentiation ") >= MaxTime ){

      m.message(" execution time " , clock.read(" sequentiator: sequentiation ") , " ms  greater than MaxTime" , MaxTime , " quitting! ", mybhep::NORMAL);

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

    m.message(" evolving sequence of size", sequence_size, mybhep::VVERBOSE); fflush(stdout);

    // protection
    if( sequence_size < 1 )
      {
        m.message(" problem: sequence has lenght ", sequence_size, "... stop evolving ", mybhep::NORMAL); fflush(stdout);
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
        m.message(" problem: it1 ", it1, " nodes size ", sequence.nodes_.size(), " cc size ", sequence.nodes_[0].cc_.size(), mybhep::NORMAL); fflush(stdout);
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
      m.message(" no links could be added... stop evolving ", mybhep::VERBOSE); fflush(stdout);
      clock.start(" sequentiator: evolve: part B: set free level ","cumulative");
      clock.start(" sequentiator: set free level ","cumulative");
      sequence.set_free_level();
      clock.stop(" sequentiator: set free level ");
      clock.stop(" sequentiator: evolve: part B: set free level ");
      clock.stop(" sequentiator: evolve: part B ");
      clock.stop(" sequentiator: evolve ");
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
    m.message(" points have been added ", mybhep::VERBOSE); fflush(stdout);

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
        type != "MULTI_VERTEX" ){
      // clock.stop(" sequentiator: good first node ");
      m.message(" not a good first node: type ", type, mybhep::VVERBOSE); fflush(stdout);
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

            m.message(" not a good first node: already used as vertex in seuqence ", iseq - sequences_.begin(), mybhep::VVERBOSE); fflush(stdout);

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
                m.message(" problem: multi-vertex ", node_.c().id(), " belongs to sequence ", iseq->name(), " but not as first or last cell", mybhep::NORMAL);
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
          m.message(" problem: multi-vertex ", node_.c().id(), " should link to cell ", done_connections[i], " but has not such couplet", mybhep::NORMAL);
        else{
          m.message(" multi-vertex ", node_.c().id(), " has already been added to a sequence connecting to cell ", done_connections[i], " so couplet ", cc_index, " will be erased", mybhep::VERBOSE);
          //node_.cc_.erase(node_.cc_.begin() + cc_index);
          node_.remove_couplet(cc_index);
        }
      }
    }

    // 2012/06/27 XG: Not used anymore since the current node is
    //    directly modified
    // first_node = node;

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

    std::clog << " families " << std::endl;

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
  bool sequentiator::match_kinks(topology::tracked_data &td){
    //*************************************************************

    clock.start(" sequentiator: match kinks ", "cumulative");

    if( td.scenarios_.empty() ) return false;

    double limit_diagonal = sqrt(2.)*cos(M_PI/8.)*CellDistance; 

    std::vector<topology::sequence>::iterator iseq = td.scenarios_[0].sequences_.begin();
    std::vector<topology::sequence>::iterator jseq = td.scenarios_[0].sequences_.begin();
    while( iseq != td.scenarios_[0].sequences_.end() ){

      if( (size_t)(iseq - td.scenarios_[0].sequences_.begin()) == td.scenarios_[0].sequences_.size() ){
        ++iseq;
        continue;
      }
      
      while( jseq != td.scenarios_[0].sequences_.end() ){
	
	if( (size_t)(jseq - td.scenarios_[0].sequences_.begin()) == td.scenarios_[0].sequences_.size() ){
	  ++jseq;
	  continue;
	}
	
        if( iseq == jseq ){
          ++jseq;
          continue;
        }

        if( (size_t)(jseq - td.scenarios_[0].sequences_.begin()) > td.scenarios_[0].sequences_.size() - 1 ){
          break;
        }


        m.message(" ... try to match sequence ", iseq->name(), " to ", jseq->name(), " with a kink ", mybhep::VERBOSE);
        if( level >= mybhep::VVERBOSE)
          print_a_sequence(*jseq);

        bool invertA, invertB;
        if( !iseq->good_match_with_kink(*jseq, invertA, invertB, limit_diagonal) ){
          ++jseq;
	  m.message(" ... obviously no good match with kink ", mybhep::VERBOSE);
          continue;
        }

	m.message(" good match with kink, try to extrapolate ", mybhep::VERBOSE);
	topology::experimental_point kink_point;
	if( iseq->intersect_sequence(*jseq, invertA, invertB, &kink_point, limit_diagonal)){
	  m.message(" good kink vertex has been assigned to sequence ", iseq->name(), mybhep::VERBOSE);
	  if( invertA )
	    iseq->set_helix_vertex(kink_point, "kink");
	  else
	    iseq->set_decay_helix_vertex(kink_point, "kink");
	}
	++jseq;
      }
      ++iseq;
    }

    clock.stop(" sequentiator: match kinks ");

    return false;


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

      m.message(" begin scenario with sequence ", iseq->name(), mybhep::VVERBOSE);
      if( level >= mybhep::VVERBOSE)
        print_a_sequence(*iseq);

      topology::scenario sc;
      sc.sequences_.push_back(*iseq);
      sc.calculate_n_free_families(td.get_cells(), td.get_calos());
      sc.calculate_n_overlaps(td.get_cells(), td.get_calos());
      sc.calculate_chi2();

      while( can_add_family(sc, &jmin, &nfree, &Chi2, &noverlaps, &ndof, td) )
        {
          m.message(" best sequence to add is ", jmin, mybhep::VVERBOSE);
          if( level >= mybhep::VVERBOSE)
            print_a_sequence(sequences_[jmin]);
          m.message(" nfree ", nfree, " noverls ", noverlaps, " Chi2 ", Chi2, mybhep::VVERBOSE);
          sc.sequences_.push_back(sequences_[jmin]);
          sc.set_n_free_families(nfree);
          sc.set_chi2(Chi2);
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


      m.message(" made scenario ", mybhep::VERBOSE);

      td.scenarios_.push_back(scenarios_[index_tmp]);

      clock.stop(" sequentiator: make scenarios ");
      return true;

    }

    m.message(" not made scenario ", mybhep::VERBOSE);
    clock.stop(" sequentiator: make scenarios ");

    return false;


  }


  //*************************************************************
  size_t sequentiator::pick_best_scenario(){
    //*************************************************************

    m.message(" pick best scenario ", mybhep::VVERBOSE);

    size_t index = 0;

    for(std::vector<topology::scenario>::iterator sc=scenarios_.begin(); sc!=scenarios_.end(); ++sc){
      m.message("...scenario ", sc - scenarios_.begin(), " nff ", sc->n_free_families(), " noverls ", sc->n_overlaps(), " chi2 ", sc->chi2(), " prob ", sc->Prob(), mybhep::VVERBOSE);

      if( sc->better_scenario_than( scenarios_[index] ) )
        {
          index = sc - scenarios_.begin();
        }
    }

    m.message(" best scenario is ", index, mybhep::VERBOSE);

    return index;

  }

  //*************************************************************
  bool sequentiator::can_add_family(topology::scenario sc, size_t* jmin, size_t* nfree, double* Chi2, size_t* noverlaps, int* ndof, topology::tracked_data td) {
    //*************************************************************

    if( late() )
      return false;

    clock.start(" sequentiator: can add family ", "cumulative");

    bool ok = false;

    if( sc.n_free_families() == 0 ){
      clock.stop(" sequentiator: can add family ");
      return false;
    }

#if 1
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

#if 1
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

        m.message(" ...try to add sequence ", jseq->name(), mybhep::VVERBOSE);
        if( level >= mybhep::VVERBOSE)
          print_a_sequence(*jseq);
        m.message(" ...nfree ", tmp.n_free_families(), " noverls ", tmp.n_overlaps(), " chi2 ", tmp.chi2(), " prob ", tmp.Prob(), mybhep::VVERBOSE);

        clock.start(" sequentiator: better scenario ", "cumulative");
        if( tmp.better_scenario_than(tmpmin) )
          {
            *jmin = jseq - sequences_.begin();
            *nfree = tmp.n_free_families();
            *noverlaps = tmp.n_overlaps();
            *Chi2 = tmp.chi2();
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
      m.message(" problem: asking for foil plane in Nemo3! ", mybhep::NORMAL);
      topology::plane null;
      return null;
    }

    topology::experimental_point center(0., 0., 0., 0., 0., 0.);

    topology::experimental_vector norm(0.,0.,1.,0.,0.,0.);

    topology::experimental_vector sizes(xsize, ysize, 0.,
                                        0., 0., 0.);

    topology::plane pl(center, sizes, norm, level, nsigma);

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
      m.message(" problem: asking for foil circle in SuperNemo ", mybhep::NORMAL);
      topology::circle null;
      return null;
    }

    topology::experimental_point center(0., 0., 0., 0., 0., 0.);

    topology::experimental_double radius(FoilRadius, 0.);

    topology::circle c(center, radius, level, nsigma);

    return c;


  }

  //*************************************************************
  void sequentiator::interpret_physics(std::vector<topology::calorimeter_hit> & calos){
    //*************************************************************

    clock.start(" sequentiator: interpret physics ", "cumulative");

    m.message(" interpreting physics of ", sequences_.size(), " sequences with ", calos.size(), " calorimeter hits ", mybhep::VVERBOSE); fflush(stdout);

    double helix_min = mybhep::default_min;
    size_t ihelix_min = mybhep::default_integer;
    double tangent_min = mybhep::default_min;
    size_t itangent_min = mybhep::default_integer;
    
    topology::experimental_point helix_extrapolation, helix_extrapolation_local;
    bool helix_found = false;
    
    topology::experimental_point tangent_extrapolation, tangent_extrapolation_local;
    bool tangent_found = false;
    double dist;
    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      {
        if( iseq->nodes().size() <= 2 ) continue;

        iseq->calculate_helix();
        iseq->calculate_charge();
        iseq->calculate_momentum(bfield);

        // match to calorimeter
	if (!calos.empty ())
	  {
	    
	    m.message(" extrapolate decay vertex with ", calos.size(), " calo hits " , mybhep::VVERBOSE);
	    
	    helix_min = mybhep::default_min;
	    ihelix_min = mybhep::default_integer;
	    tangent_min = mybhep::default_min;
	    itangent_min = mybhep::default_integer;

	    helix_found = false;

	    tangent_found = false;

	    for(std::vector<topology::calorimeter_hit>::iterator ic=calos.begin(); ic != calos.end(); ++ic){
	      
	      m.message( " trying to extrapolate to calo hit ", ic - calos.begin(), " id ", ic->id(), " on view ", ic->pl_.view(), mybhep::VVERBOSE);

	      if( !near(iseq->last_node().c(), ic->pl()) ){
		m.message( " not near " , mybhep::VVERBOSE);
		continue;
	      }
	      
	      
	      if( !iseq->intersect_plane_from_end(ic->pl(), &helix_extrapolation_local) ){
		m.message( " no helix intersection " , mybhep::VVERBOSE);
	      }
	      else{

		dist = helix_extrapolation_local.distance(ic->pl_.face()).value();
		if( dist < helix_min ){
		  helix_min = dist;
		  ihelix_min = ic->id();
		  helix_extrapolation = helix_extrapolation_local;
		  helix_found = true;
		  m.message( " new helix intersection with minimum distance " , dist , " position: " , helix_extrapolation.x().value() ,   helix_extrapolation.y().value(),  helix_extrapolation.z().value() , mybhep::VVERBOSE);
		}
	      }


	      if( !iseq->intersect_plane_with_tangent_from_end(ic->pl(), &tangent_extrapolation_local) ){
		m.message( " no tangent intersection " , mybhep::VVERBOSE);
	      }
	      else{

		dist = tangent_extrapolation_local.distance(ic->pl_.face()).value();
		if( dist < tangent_min ){
		  tangent_min = dist;
		  itangent_min = ic->id();
		  tangent_extrapolation = tangent_extrapolation_local;
		  tangent_found = true;
		  m.message( " new tangent intersection with minimum distance " , dist , " position: " , tangent_extrapolation.x().value() ,   tangent_extrapolation.y().value(),  tangent_extrapolation.z().value() , mybhep::VVERBOSE);
		}
	      }


	    } // finish loop on calos

	    if( helix_found ){
	      if( ihelix_min >= calos.size() ){
		m.message( " problem: calo hit of id " , ihelix_min , " but n of calo hits is " , calos.size() , mybhep::VVERBOSE);
	      }
	      else
		iseq->set_decay_helix_vertex(helix_extrapolation, "calo", ihelix_min);
	    }
	    
	    if( tangent_found ){
	      if( itangent_min >= calos.size() ){
		m.message( " problem: tangent calo hit of id " , itangent_min , " but n of calo hits is " , calos.size() , mybhep::VVERBOSE);
	      }
	      else
		iseq->set_decay_tangent_vertex(tangent_extrapolation, "calo", itangent_min);
	    }
	    
	  }

        // match to foil
	if( !iseq->nodes_.empty() ){

	  m.message( " extrapolate vertex on foil: supernemo " , SuperNemo, mybhep::VVERBOSE);

	  if( gap_number(iseq->nodes_[0].c() ) != 0 ){
	    m.message( " not near ", mybhep::VVERBOSE); fflush(stdout);
	  }
	  else{
	    if( SuperNemo ){

	      if( !iseq->intersect_plane_from_begin(get_foil_plane(), &helix_extrapolation) ){
                m.message(" no helix intersection ", mybhep::VVERBOSE); fflush(stdout);
	      }
              else{
                iseq->set_helix_vertex(helix_extrapolation, "foil");

	      }

	      if( !iseq->intersect_plane_with_tangent_from_begin(get_foil_plane(), &tangent_extrapolation) ){
                m.message(" no tangent intersection ", mybhep::VVERBOSE); fflush(stdout);
	      }
              else
                iseq->set_tangent_vertex(tangent_extrapolation, "foil");

	    }else{  // nemo3


	      if( !iseq->intersect_circle_from_begin(get_foil_circle(), &helix_extrapolation) ){
		m.message(" no helix intersection ", mybhep::VVERBOSE); fflush(stdout);
	      }
	      else
		iseq->set_helix_vertex(helix_extrapolation, "foil");

	      if( !iseq->intersect_circle_with_tangent_from_begin(get_foil_circle(), &tangent_extrapolation) ){
		m.message(" no tangent intersection ", mybhep::VVERBOSE); fflush(stdout);
	      }
	      else
		iseq->set_tangent_vertex(tangent_extrapolation, "foil");


	    }



	  }
        }


        if( level >= mybhep::VVERBOSE ){
          std::clog << " sequence " << iseq - sequences_.begin() << " has: " << std::endl; fflush(stdout);
          std::clog << " center "; iseq->center().dump(); fflush(stdout);
          std::clog << " radius "; iseq->radius().dump(); std::clog << " " << std::endl; fflush(stdout);
          std::clog << " pitch "; iseq->pitch().dump(); std::clog << " " << std::endl; fflush(stdout);
          std::clog << " momentum "; iseq->momentum().length().dump(); std::clog << " " << std::endl; fflush(stdout);
          std::clog << " charge "; iseq->charge().dump(); std::clog << " " << std::endl; fflush(stdout);
	  if( iseq->has_helix_vertex() ){
	    std::clog << " helix_vertex "; iseq->helix_vertex().dump(); std::clog << " " << std::endl; fflush(stdout);
	  }
	  if( iseq->has_decay_helix_vertex() ){
	    std::clog << " decay helix_vertex "; iseq->decay_helix_vertex().dump(); std::clog << " " << std::endl; fflush(stdout);
	  }
	  if( iseq->has_tangent_vertex() ){
	    std::clog << " tangent_vertex "; iseq->tangent_vertex().dump(); std::clog << " " << std::endl; fflush(stdout);
	  }
	  if( iseq->has_decay_tangent_vertex() ){
	    std::clog << " decay tangent_vertex "; iseq->decay_tangent_vertex().dump(); std::clog << " " << std::endl; fflush(stdout);
	  }
        }


      }


    clock.stop(" sequentiator: interpret physics ");

    return;

  }


  //*************************************************************
  void sequentiator::print_sequences() const {
    //*************************************************************

    m.message(" sequence matrix is", mybhep::NORMAL); fflush(stdout);
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

    clog << "NOTICE: " << "CAT::sequentiator::print_a_sequence: "
         << "Printing sequence: " << endl;
    if( sequence.Free() )
      {
        std::clog << "*";
        fflush(stdout);
      }

    int iccc;
    for(vector<topology::node>::const_iterator inode = sequence.nodes_.begin();
        inode != sequence.nodes_.end(); ++inode)
      {
        std::clog << " " << (int)inode->c().id();

        if( inode->free() )
          std::clog << "* ";

        topology::experimental_vector v(inode->c().ep(),inode->ep());

        if( level >= mybhep::VVERBOSE ){
	  std::clog << "[" << v.x().value() << ", " << v.z().value() << "]";

	  std::clog << "(";

	  for(vector<topology::cell>::const_iterator ilink=(*inode).links_.begin(); ilink != (*inode).links_.end(); ++ilink){
	    iccc = sequence.get_link_index_of_cell(inode - sequence.nodes_.begin(), *ilink);

	    if( iccc < 0 ) continue;  // connection through a gap

	    if( inode - sequence.nodes_.begin() < 1 ){
	      std::clog << "->" << inode->cc()[iccc].cb().id();
	      if( ilink->free() ){
		std::clog << "[*]";
	      }
	      std::clog << "[" << inode->cc_[iccc].iteration() << " "
		   << inode->cc()[iccc].tangents().size() << "]";
	      if( ! ilink->begun() )
		std::clog << "[n]";
	    }else{
	      std::clog << inode->ccc()[iccc].ca().id() << "<->" << inode->ccc()[iccc].cc().id();
	      if( ilink->free() ){
		std::clog << "[*]";
	      }
	      std::clog << "[" << inode->ccc_[iccc].iteration() << " "
		   << inode->ccc()[iccc].joints().size() << "]";
	      if( ! ilink->begun() )
		std::clog << "[n]";
	    }
	  }

	  std::clog << " chi2 = " << inode->chi2();

	  std::clog << " )";

	}
      }

    std::clog << " " << std::endl;

    return;

  }

  //*************************************************************
  void sequentiator::print_scenarios() const {
    //*************************************************************

    clog << "NOTICE: " << "CAT::sequentiator::print_scenarios: "
         << "Printing scenarios " << scenarios_.size() << endl; fflush(stdout);

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

    clog << "NOTICE: " << "CAT::sequentiator::print_a_scenario: "
         << "Print associated sequences: " << endl;
    for (vector<topology::sequence>::const_iterator iseq = scenario.sequences_.begin();
         iseq != scenario.sequences_.end(); ++iseq)
      {
        print_a_sequence(*iseq);
      }

    clog << "NOTICE: " << "CAT::sequentiator::print_a_scenario: "
         << "Print scenario parameters" << endl;
    clog << " |-- nfree " << scenario.n_free_families() << endl;
    clog << " |-- noverls " << scenario.n_overlaps() << endl;
    clog << " |-- chi2 " << scenario.chi2() << endl;
    clog << " `-- prob " << scenario.Prob() << endl;

    return;
  }


  //*************************************************************
  void sequentiator::add_pair(const topology::sequence & newsequence){
    //*************************************************************

    clock.start(" sequentiator: add pair ", "cumulative");

    m.message(" adding pair ", mybhep::VVERBOSE); fflush(stdout);

    if (newsequence.nodes().size() != 2){
      m.message(" problem: pair has size ", newsequence.nodes().size(), mybhep::NORMAL);
      clock.stop(" sequentiator: add pair ");
      return;
    }

    topology::cell_couplet cc;
    if( !newsequence.nodes_[0].has_couplet(newsequence.nodes()[1].c(), &cc) ){
      m.message(" problem: node ", newsequence.nodes_[0].c().id(), " has no pair ", newsequence.nodes()[0].c().id(), "-", newsequence.nodes()[1].c().id(), mybhep::NORMAL);
      clock.stop(" sequentiator: add pair ");
      return;
    }

    topology::node na = newsequence.nodes()[0];
    topology::node nb = newsequence.nodes()[1];

    std::vector<topology::node> nodes;
    nodes.push_back(na);
    nodes.push_back(nb);

    bool erased = true;

    topology::sequence pair(nodes, level, nsigma);
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

      m.message(" made track [", sequences_.size()-1, "] with cells ", na.c().id(), "and", nb.c().id(), mybhep::VVERBOSE); fflush(stdout);

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

      if( (size_t)(iseq - sequences_.begin()) == sequences_.size() - 1 ){
        ++iseq;
        continue;
      }

      //    m.message(" should we erase last sequence ", sequences_.size() - 1, " in favour of ", iseq - sequences_.begin(), " ? " ,mybhep::VVERBOSE); fflush(stdout);
      if( sequences_.back().contained( *iseq ) && ! sequences_.back().Free())
        {
          sequences_.pop_back();
          m.message(" erased last sequence ", sequences_.size() - 1, "  contained in sequence", iseq - sequences_.begin(), mybhep::VERBOSE); fflush(stdout);
          changed =  true;
          continue;
        }

      //    m.message(" should we erase sequence ", iseq - sequences_.begin(), " in favour of the last ", sequences_.size() - 1, " ? " ,mybhep::VVERBOSE); fflush(stdout);

      if( iseq->contained( sequences_.back() )  && ! iseq->Free())
        {
          m.message(" erased sequence ", iseq - sequences_.begin(), "contained in last sequence", sequences_.size()-1, mybhep::VERBOSE); fflush(stdout);
          sequences_.erase(iseq);
          changed = true;
          ++ iseq;
          continue;
        }
      else{
        ++ iseq;
        continue;
      }

      std::vector<topology::sequence>::iterator kseq = sequences_.begin();

      while( kseq != sequences_.end() ){

        if( iseq==kseq ){
          ++ kseq;
          continue;
        }

        if( kseq->is_bridge(*iseq, sequences_.back() ) &&
            (*kseq).nodes().size() < (*iseq).nodes().size() &&
            (*kseq).nodes().size() < sequences_.back().nodes().size()  &&
            !kseq->Free())
          {

            m.message(" erased sequence ", kseq - sequences_.begin(), "bridge between", iseq - sequences_.begin(), "and last ", sequences_.size() - 1, mybhep::VERBOSE); fflush(stdout);
            sequences_.erase(kseq);
            changed = true;
          }
        else{
          ++ kseq;
          continue;
        }

        if( iseq==kseq ){
          ++kseq;
          continue;
        }

        if( sequences_.back().is_bridge(*iseq, *kseq ) &&
            sequences_.back().nodes().size() < (*iseq).nodes().size() &&
            sequences_.back().nodes().size() < (*kseq).nodes().size()  &&
            !sequences_.back().Free())
          {
            m.message(" erased last sequence ", sequences_.size()-1, "bridge between", iseq - sequences_.begin(), "and", kseq - sequences_.begin(), mybhep::VERBOSE); fflush(stdout);
            sequences_.pop_back();
            changed =  true;
            continue;
          }
        else{
          kseq ++;
          continue;
        }

      }
    }


    clock.stop(" sequentiator: clean up sequences ");
    return changed;

  }

  //*************************************************************
  double sequentiator::distance_from_foil(topology::experimental_point ep){
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
        m.message(" sequence ", iseq - sequences_.begin(), " will be directed out of foil ", mybhep::VVERBOSE);
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
	  m.message(" sequence ", iseq - sequences_.begin(), " will be directed out of foil ", mybhep::VVERBOSE);
	  topology::sequence is = iseq->invert();
	  std::swap(*iseq, is);
	}
      }

    }
    clock.stop(" sequentiator: direct scenarios out of foil ");


    return true;

  }


  //*************************************************************
  bool sequentiator::there_is_free_sequence_beginning_with(topology::cell c, size_t *index) {
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
  bool sequentiator::near(topology::cell c, topology::plane pl){
    //*************************************************************

    if( pl.view() == "x" ){

      m.message(" matching cell ", c.id(), " with cell number ", c.cell_number(), " to calo on view ", pl.view(), " max cell number ", cell_max_number, " plane norm x ", pl.norm().x().value(), mybhep::VVERBOSE);

      if( pl.norm().x().value() > 0. )
        return (fabs(cell_max_number + c.cell_number()) < 1);

      return (fabs(cell_max_number - c.cell_number()) < 1);
    }
    else if( pl.view() == "y" ){
      m.message(" problem: matching cell to calo on view ", pl.view(), mybhep::NORMAL);
      return false;
    }

    int g = gap_number(c);

    m.message(" checking if cell ", c.id(), " on gap ", g, " is near plane: ", pl.center().x().value(), pl.center().y().value(), pl.center().z().value(), mybhep::VVERBOSE);

    if( g <= 0 ) return false;

    if ( SuperNemo ){
      if( g == 1 ) return true;
    }
    else{
      if( g == 3 ) return true;
    }

    m.message(" problem: can't match to calo on view ", pl.view(), mybhep::NORMAL);

    return false;

  }


  //*************************************************************
  int sequentiator::gap_number(topology::cell c){
    //*************************************************************

    // returns the index of the gap on which the hit is facing: 1, 2, 3
    // ... returns -1 if not on a gap
    // ... returns 0 if the hit is on layer 0, facing the foil

    size_t ln = abs(c.layer());

    m.message(" cell ", c.id(), " layer ", c.layer(), mybhep::VVERBOSE);

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
  void sequentiator::make_table_of_true_and_reco_sequences(std::vector<topology::sequence> trueseqs){
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
        m.message("problem: imin", imin, "allparts size", sequences_.size(), mybhep::NORMAL);
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
        std::clog << " best matching for reco track " << i << " is true track " << true_sequence_of_reco_[i] << " with " << n_common_hits_for_reco_track_[i] << " common hits " << std::endl;
        std::clog << " hits of reco track: ";
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
        std::clog << " best matching for true track " << i << " is reco track " << reco_sequence_of_true_[i] << std::endl;
    }

    //free(max_common_hits_for_reco_track);
    //free(best_matching_for_reco_track);

    return;
  }

  //*******************************************************************
  void sequentiator::rec_efficiency(std::vector<topology::sequence> trueseqs){
    //*******************************************************************

    if( sequences_.size() == 0 ) return;

    make_table_of_true_and_reco_sequences(trueseqs);
    size_t ireco;
    for(std::vector<topology::sequence>::iterator itrueseq=trueseqs.begin(); itrueseq != trueseqs.end(); ++itrueseq){

      if( !itrueseq->primary() ) continue;

      m.message(" reconstructing efficiencies for true particle ", itrueseq - trueseqs.begin(), " of ", itrueseq->nodes().size(), " points, with name ", itrueseq->name(), mybhep::VVERBOSE);

      if( itrueseq->nodes().size() == 0 )
        continue;

      ireco = reco_sequence_of_true_[itrueseq - trueseqs.begin()];

      if( ireco >= sequences_.size() ){
        m.message(" problem: ireco", ireco, " size", sequences_.size(), mybhep::NORMAL);
      }

      topology::sequence reco = sequences_[ireco];

      FillGGResiduals(*itrueseq, reco);

      int nhits = reco.nodes().size();
      if( nhits > 0 ){

        int ncommon = n_common_hits_for_reco_track_[ireco];

        double purity = (double)ncommon/(double)nhits;

        double efficiency = (double)ncommon/(double)(itrueseq->nodes().size());

        m.message(" ... n reco hits ", nhits, " n true hits ", itrueseq->nodes().size(), " n common hits ", ncommon, " TTPurity ", purity, " TTEfficiency ", efficiency, mybhep::VVERBOSE);

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
  size_t sequentiator::getCommonHits(topology::sequence tp, topology::sequence dp){
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
  void sequentiator::FillGGResiduals(topology::sequence tp, topology::sequence dp){
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
  bool sequentiator::match_through_gaps(void){
    //*************************************************************

    if( gaps_Z.empty() ) return true;
    if( sequences_.size() < 2 ) return true;

    clock.start(" sequentiator: match through gaps ", "cumulative");

    std::vector<topology::sequence>::iterator iseq = sequences_.begin();

    while( iseq != sequences_.end() ){

      if( (size_t)(iseq - sequences_.begin()) > sequences_.size() - 1 ){
        break;
      }

      if( !good_first_to_be_matched(*iseq) ){
        ++iseq;
        continue;
      }

      double probmax = mybhep::default_max;
      double chi2min = mybhep::default_min;
      int ndofbest = 1;
      size_t index = 0;
      topology::sequence bestmatch;
      bool matched = false;

      m.message(" try to match sequence ", iseq->name(), mybhep::VERBOSE);

      if( level >= mybhep::VVERBOSE)
        print_a_sequence(*iseq);

      std::vector<topology::sequence>::iterator jseq = sequences_.begin();

      while( jseq != sequences_.end() ){

        if( (size_t)(jseq - sequences_.begin()) > sequences_.size() - 1 ){
          break;
        }

        if( iseq == jseq ){
          ++jseq;
          continue;
        }

        m.message(" ... try to match sequence ", iseq->name(), " to ", jseq->name(), mybhep::VERBOSE);
        if( level >= mybhep::VVERBOSE)
          print_a_sequence(*jseq);


        bool invertA, invertB;
        if( !iseq->good_match(*jseq, invertA, invertB, NOffLayers) ){
          ++jseq;
	  m.message(" ... obviously no good match ", mybhep::VERBOSE);
          continue;
        }

        topology::sequence news = iseq->match(*jseq, invertA, invertB);

        topology::experimental_point epa, epb;
        size_t gn;
        if( !invertA ){
          epa = iseq->last_node().ep();
          gn = gap_number(iseq->last_node().c());
        }
        else{
          epa = iseq->nodes_[0].ep();
          gn = gap_number(iseq->nodes_[0].c());
        }

        if( !invertB ){
          epb = jseq->nodes_[0].ep();
          if( gn < 0 )
            gn = gap_number(jseq->nodes_[0].c());
        }
        else{
          epb = jseq->last_node().ep();
          if( gn < 0)
            gn = gap_number(jseq->last_node().c());
        }

        double deltaphi = cos(news.delta_phi(epa, epb)/2.);
        double penetration = fabs(news.radius().value()*(1 - cos(deltaphi)));

        if( fabs(deltaphi)*180./acos(-1.) > 180. )
          penetration = fabs(news.radius().value()*(1 + cos(deltaphi)));

        double gapsize = gaps_Z[gn];

        if( penetration > gapsize ){
          m.message("SQ user: no good match because penetrating across the gap, penetration",  penetration, " gap ", gapsize, " deltaphi ", deltaphi*180./acos(-1.),  mybhep::VERBOSE);
          ++jseq;
          continue;
        }


        double p = news.helix_Prob();
        double c = news.helix_chi2();
        int nd = news.ndof();

        m.message(" ... matched with chi2 ", c, " ndof ", nd, " prob ", p, mybhep::VVERBOSE);
        matched = true;

        if( p > probmax ){
          probmax = p;
          index = jseq - sequences_.begin();
          chi2min = c;
          ndofbest = nd;
          bestmatch = news;
        }
        else if( p == probmax ){
          if( c / nd < chi2min /ndofbest ){
            probmax = p;
            index = jseq - sequences_.begin();
            chi2min = c;
            ndofbest = nd;
            bestmatch = news;
          }
        }

        ++ jseq;

      }

      if( !matched ){
        ++iseq;
        continue;
      }

      m.message(" best match to sequence ", iseq->name(),  " is ", sequences_[index].name(), " with prob ", probmax, " chi2 ", chi2min, " ndof ", ndofbest, mybhep::VVERBOSE);

      if( probmax > 0 ){
        m.message(" replace sequence ", iseq->name(), " with matched sequence ", mybhep::VERBOSE); fflush(stdout);
        size_t restart_index = iseq - sequences_.begin();
        std::swap(*iseq, bestmatch);
        iseq = sequences_.begin();
        advance(iseq, restart_index);

        std::string delenda_family = sequences_[index].family();
        m.message(" erase all sequences of family ", delenda_family, mybhep::VERBOSE); fflush(stdout);

        std::vector<topology::sequence>::iterator jseq = sequences_.begin();
        while( jseq != sequences_.end() ){

          if( (size_t)(jseq - sequences_.begin()) > sequences_.size() - 1 ){
            break;
          }

          if( jseq->family() == delenda_family ){
            m.message(" erase sequence ", jseq->name(), mybhep::VVERBOSE); fflush(stdout);
            sequences_.erase(jseq);
          }
          else
            ++jseq;
        }

        continue;
      }


      ++ iseq;

    }


    clock.stop(" sequentiator: match through gaps ");

    return true;

  }

  //*************************************************************
  bool sequentiator::good_first_to_be_matched(topology::sequence& seq){
    //*************************************************************

    if( !seq.fast() ) return false;

    if( seq.nodes().size() < 2 )
      {
        m.message("SQ user: not a good first, because hits size is", seq.nodes().size(), mybhep::VERBOSE);
        return false;
      }

    return true;

  }

  //*************************************************************
  bool sequentiator::match_gaps(void){
    //*************************************************************

    if( gaps_Z.size() <= 1 ) return true;
    if( sequences_.size() < 2 ) return true;

    clock.start(" sequentiator: match gaps ", "cumulative");

    if( level >= mybhep::VERBOSE){
      print_families();
      print_sequences();
    }

    std::vector<bool> matched;
    matched.assign (sequences_.size(), false);
    //   bool* matched= (bool*)mal loc(sizeof(size_t)*sequences_.size());
    for(size_t i=0; i<sequences_.size(); i++)
      matched[i] = false;

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

      m.message(" begin matching with sequence ", iseq->name(), mybhep::VERBOSE);
      if( level >= mybhep::VERBOSE)
        print_a_sequence(*iseq);

      first = true;
      topology::sequence newseq = *iseq;
      while( can_match(newseq, &jmin, invertA, invertB) )
        {
          m.message(" best matching is ", sequences_[jmin].name(), " invertA ", invertA, " invertB ", invertB, mybhep::VERBOSE);
          if( level >= mybhep::VVERBOSE)
            print_a_sequence(sequences_[jmin]);

          newseq = newseq.match(sequences_[jmin], invertA, invertB);
          if( level >= mybhep::VERBOSE)
            print_a_sequence(newseq);

          if( first ){
            //      matched[ifa] = true;
            first = false;
          }
          size_t ifa = mybhep::int_from_string(sequences_[jmin].family());
          matched[ifa] = true;

        }

      if( !first )
        newseqs.push_back(newseq);

    }
    if( late() )
      return false;

    m.message(" made matching through gaps ", mybhep::VERBOSE);

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
  bool sequentiator::can_match(topology::sequence s, size_t* jmin, bool& bestinvertA, bool& bestinvertB) {
    //*************************************************************

    if( late() )
      return false;

    clock.start(" sequentiator: can match ", "cumulative");

    bool ok = false;

    double probmax = mybhep::default_max;
    double chi2min = mybhep::default_min;
    int ndofbest = 1;

    m.message(" try to match sequence", s.name(), " of chi2 = ", chi2min, " ndof ", ndofbest, " prob ", probmax, mybhep::VVERBOSE);
    bool invertA, invertB;
    double p;
    double c;
    int n;

    for(std::vector<topology::sequence>::iterator jseq=sequences_.begin(); jseq!=sequences_.end(); ++jseq)
      {

        m.message(" try to match sequence", s.name(), " to ", jseq->name(), mybhep::VVERBOSE);
        if( level >= mybhep::VVERBOSE){
          print_a_sequence(s);
          print_a_sequence(*jseq);
        }

        // check that jseq's family is not the same as s
        if( s.same_families(*jseq) ){
          m.message(" ... forbidden, same family", jseq->family(), mybhep::VVERBOSE);
          continue;
        }

        if( !s.good_match(*jseq, invertA, invertB, NOffLayers) ){
          continue;
        }


        topology::sequence news = s.match(*jseq, invertA, invertB);
        p = news.helix_Prob();
        c = news.helix_chi2();
        n = news.ndof();

        m.message(" ... matched to ", jseq->name(), ", chi2 =", c, " ndof ", n, " prob ", p, mybhep::VVERBOSE);

        if( p > probmax )
          {
            *jmin = jseq - sequences_.begin();
            probmax = p;
            chi2min = c;
            ndofbest = n;
            bestinvertA = invertA;
            bestinvertB = invertB;
            ok = true;
          }
      }

    if( ok ){
      m.message(" sequence ", s.name(), " can be matched to ", sequences_[*jmin].name(), ", chi2 =", chi2min, " ndof ", ndofbest, " prob ", probmax, mybhep::VVERBOSE);
    }

    clock.stop(" sequentiator: can match ");
    return ok;

  }


  //*************************************************************
  bool sequentiator::select_nemo_tracks(topology::tracked_data & __tracked_data){
    //*************************************************************

    if( SuperNemo ) return true;

    std::vector<topology::sequence> nemo_sequences = __tracked_data.get_nemo_sequences();

    m.message(" selecting events based on nemo tracks ", mybhep::VVERBOSE);

    if( nemo_sequences.size() != 2 ){
      m.message(" reject: ntracks = ", nemo_sequences.size(), mybhep::VVERBOSE);
      return false;
    }
    m.message(" ntracks = ", nemo_sequences.size(), mybhep::VVERBOSE);

    if( !nemo_sequences[0].has_decay_helix_vertex() ){
      m.message(" reject: 1st track has no calo ", mybhep::VVERBOSE);
      return false;
    }


    if( !nemo_sequences[1].has_decay_helix_vertex() ){
      m.message(" reject: 2nd track has no calo ", mybhep::VVERBOSE);
      return false;
    }

    std::vector<topology::calorimeter_hit> calos = __tracked_data.get_calos();

    if( nemo_sequences[0].calo_helix_id() == nemo_sequences[1].calo_helix_id() ){
      m.message(" reject: same calo ", nemo_sequences[0].calo_helix_id(), mybhep::VVERBOSE);
      return false;
    }

    topology::calorimeter_hit caloA = calos[nemo_sequences[0].calo_helix_id()];
    topology::calorimeter_hit caloB = calos[nemo_sequences[1].calo_helix_id()];

    if( caloA.e().value() < 0.2 ){
      m.message(" reject: 1st calo has energy ", caloA.e().value(), mybhep::VVERBOSE);
      return false;

    }

    if( caloB.e().value() < 0.2 ){
      m.message(" reject: 2nd calo has energy ", caloA.e().value(), mybhep::VVERBOSE);
      return false;
    }


    return true;

  }

} // end of namespace CAT
