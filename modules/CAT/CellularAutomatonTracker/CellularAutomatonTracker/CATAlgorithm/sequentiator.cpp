#include "sequentiator.h"
#include <vector>
#include <mybhep/system_of_units.h>
#include <sys/time.h>
#include <math.h>

namespace CAT {

  void sequentiator::_set_defaults ()
  {
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

  //************************************************************                                                                          // Default constructor : 
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

    clock.start(" sequentiator: initialize ");
  
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

    clock.stop(" sequentiator: initialize ");

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
        string pname;

        pname="N3GG_LONGITUDINAL_SIGMA";
        if(global.find(pname))
          {
            sigma0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); fflush(stdout);
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
            exit(1);
          }
      }
  

    clock.stop(" sequentiator: read dst properties ");

    return;
  }


  //*************************************************************
  void sequentiator::readDstProper(void) {
    //*************************************************************
  
    clock.start(" sequentiator: read dst properties ");

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


    clock.stop(" sequentiator: read dst properties ");

    return;
  }


  //*************************************************************
  bool sequentiator::sequentiate(topology::tracked_data & __tracked_data){
    //*************************************************************
  
    clock.start(" sequentiator: sequentiate ","cumulative");
    clock.start(" sequentiator: sequentiation ","restart");

    m.message(" sequentiate ", mybhep::VVERBOSE); fflush(stdout);

    set_clusters(__tracked_data.get_clusters());

    NFAMILY = -1;
    NCOPY = 0;

    if( clusters_.size() ==0) return true;

    sequences_.clear();
    scenarios_.clear();

    __tracked_data.scenarios_.clear();

    for(vector<topology::cluster>::iterator icluster=clusters_.begin(); icluster != clusters_.end(); ++icluster){
      local_cluster_ = *icluster;

      sequentiate_cluster();

    }
  
    if( late() ){
      __tracked_data.set_skipped(true);
      return false;
    }

    clean_up_sequences();
    direct_out_of_foil();

    make_families();

    interpret_physics(__tracked_data.get_calos());

    //  match_through_gaps();
    match_gaps();
    if( late() ){
      __tracked_data.set_skipped(true);
      return false;
    }


    make_scenarios(__tracked_data);
    if( late() ){
      __tracked_data.set_skipped(true);
      return false;
    }


    make_plots(__tracked_data);


    clock.stop(" sequentiator: sequentiate ");


    return true;
  }


  //*************************************************************
  void sequentiator::plot_hard_scattering(topology::tracked_data __tracked_data){
    //*************************************************************

    /*
      if( PrintMode ){
      if( !__tracked_data.scenarios_.empty() ){
      for(vector<topology::sequence>::iterator isequence = __tracked_data.scenarios_[0].sequences_.begin(); isequence != __tracked_data.scenarios_[0].sequences_.end(); ++isequence){
        
      for(std::vector<topology::node>::iterator in = isequence->nodes_.begin(); in != isequence->nodes_.end(); ++in){
      topology::experimental_vector dist(in->c().ep(), in->ep());
      hman.fill("all_point_map", dist.z().value(), dist.x().value());
      }
        
      if( !isequence->has_decay_vertex() ){
      // if the last cell is not on a gap
      if( gap_number(isequence->last_node().c()) < 0 ){
      m.message(" sequence ", isequence->name(), "has last cell ", isequence->last_node().c().id(), " disconnected", mybhep::NORMAL);
            
      for(vector<topology::sequence>::iterator jsequence = __tracked_data.scenarios_[0].sequences_.begin(); jsequence != __tracked_data.scenarios_[0].sequences_.end(); ++jsequence){
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

      for(vector<topology::sequence>::iterator isequence = sequences_.begin(); isequence != sequences_.end(); ++isequence){

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
  void sequentiator::sequentiate_cluster(){
    //*************************************************************

  
    for(vector<topology::node>::iterator inode = local_cluster_.nodes_.begin(); inode != local_cluster_.nodes_.end(); ++inode){

      if( level >= mybhep::VVERBOSE ){
        clog << "\n first node: " << inode->c().id();
      }
    
      topology::node first_node;

      if(! good_first_node(*inode, first_node) ){
        continue;
      }
    
    
      make_new_sequence(first_node);

      if( late() ){
        return ;
      }


      make_copy_sequence(first_node);
    
    }
  
    return;
  }


  //*************************************************************
  void sequentiator::make_new_sequence(topology::node first_node){
    //*************************************************************
  
    if( late() )
      return;

    clock.start(" sequentiator: make new sequence ","cumulative");

    //  A node is added to the newsequence. It has the given cell but no other
    //  requirement. The free level is set to true.
    topology::sequence newsequence(first_node, level, nsigma);
  
    if( level >= mybhep::VERBOSE ){
      clog << " begin track [" << sequences_.size() << "] with node " << first_node.c().id() << endl;
      print_a_sequence(&newsequence);
    }
  
    bool updated = true;
  
    while( updated ){
      updated = evolve(&newsequence);
    }


    if( late() )
      return;

    if( level >= mybhep::VVERBOSE)
      print_a_sequence( &newsequence );
  
    NFAMILY ++;
    NCOPY = 0;
    if( newsequence.nodes().size() != 2 )
      {

        make_name(&newsequence);

        sequences_.push_back( newsequence );
        m.message(" finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
        clean_up_sequences();
      }
    else
      add_pair(&newsequence);
  
    clock.stop(" sequentiator: make new sequence ");
    
    return;
  }


  //*************************************************************
  void sequentiator::make_name(topology::sequence* seq) {
    //*************************************************************

    string number = mybhep::to_string(NFAMILY)+"_"+mybhep::to_string(NCOPY);
    string name = "track_"+number;
    seq->set_name(name);
  
    return;

  }

  //*************************************************************
  void sequentiator::make_copy_sequence(topology::node first_node){
    //*************************************************************
  
    clock.start(" sequentiator: make copy sequence ","cumulative");

    size_t isequence;
    while( there_is_free_sequence_beginning_with(first_node.c(), &isequence ) )
      {
      
        if( late() )
          return;

        clock.start(" sequentiator: make copy sequence: part A ","cumulative");
        clock.start(" sequentiator: make copy sequence: part A: alpha ","cumulative");

        m.message("\n begin, with cell", first_node.c().id(), ", parallel track ", sequences_.size(), " to track ", isequence, mybhep::VERBOSE); fflush(stdout);
        
        size_t ilink, ilfn;
      
        if( level >= mybhep::VVERBOSE)
          {
            clog << " original sequence before copying: " << endl;
            print_a_sequence(&sequences_[isequence]);
          }
        
        clock.stop(" sequentiator: make copy sequence: part A: alpha ");
        clock.start(" sequentiator: copy to lfn ","cumulative");
        topology::sequence newcopy = sequences_[isequence].copy_to_last_free_node(&ilfn, &ilink);
        clock.stop(" sequentiator: copy to lfn ");
      
        clock.start(" sequentiator: make copy sequence: part A: beta ","cumulative");
        m.message(" copied from sequence  ", isequence, mybhep::VVERBOSE); fflush(stdout);
      
        if( level >= mybhep::VVERBOSE)
          {
            print_a_sequence(&sequences_[isequence]);
            print_a_sequence(&newcopy);
          }
      

        clock.stop(" sequentiator: make copy sequence: part A: beta ");
        clock.start(" sequentiator: make copy sequence: evolve ","cumulative");
        bool updated = true;
        while( updated )
          updated = evolve(&newcopy);
        clock.stop(" sequentiator: make copy sequence: evolve ");
      
        if( late() )
          return;

        if( level >= mybhep::VVERBOSE)
          print_a_sequence( &newcopy );


      
        clock.stop(" sequentiator: make copy sequence: part A ");
        clock.start(" sequentiator: manage copy sequence ","cumulative");


        // not adding: case 1: new sequence did not evolve
        if( newcopy.nodes().size() == ilfn + 1)
          {
            m.message(" not adding new sequence, since it couldn't evolve past lfn ", mybhep::VERBOSE); fflush(stdout);
            clean_up_sequences();
          }
        else{

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
          if( newcopy.contained(sequences_[isequence] )  &&  !newcopy.Free())  // new copy is contained in original
            {
              m.message(" not adding new sequence, contained in ", isequence, "from which it was copied", mybhep::VERBOSE); fflush(stdout);
              clean_up_sequences();
            }
          else{ // adding: case 3

            if( sequences_[isequence].contained( newcopy )  ) { // original is contained in new copy
            
              for(size_t k=0; k<ilfn; k++)
                {
                  newcopy.nodes_[k].set_free( sequences_[isequence].nodes()[k].free());

                  for(vector<topology::cell_couplet>::iterator icc=sequences_[isequence].nodes_[k].cc_.begin(); icc != sequences_[isequence].nodes_[k].cc_.end(); ++icc)
                    {
                      newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].set_free( icc->free());
                      newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].set_begun( icc->begun());

                      for(vector<topology::line>::iterator itang=sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.begin(); itang !=sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.end(); ++itang)
                        newcopy.nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_[itang - sequences_[isequence].nodes_[k].cc_[icc - sequences_[isequence].nodes_[k].cc_.begin()].tangents_.begin()].set_used(itang->used() );

                    }

                  for(vector<topology::cell_triplet>::iterator iccc=sequences_[isequence].nodes_[k].ccc_.begin(); iccc != sequences_[isequence].nodes_[k].ccc_.end(); ++iccc)
                    {
                      newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].set_free( iccc->free());
                      newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].set_begun( iccc->begun());

                      for(vector<topology::joint>::iterator ijoint=sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.begin(); ijoint !=sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.end(); ++ijoint)
                        newcopy.nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_[ijoint - sequences_[isequence].nodes_[k].ccc_[iccc - sequences_[isequence].nodes_[k].ccc_.begin()].joints_.begin()].set_used(ijoint->used() );

                    }



                }
            

              if( ilfn < 2 ){

                for(vector<topology::cell_couplet>::iterator icc=sequences_[isequence].nodes_[ilfn].cc_.begin(); icc != sequences_[isequence].nodes_[ilfn].cc_.end(); ++icc)
                  if( (size_t)(icc - sequences_[isequence].nodes_[ilfn].cc_.begin()) != ilink )
                    {
                      newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].set_free( icc->free());
                      newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].set_begun( icc->begun());

                      for(vector<topology::line>::iterator itang=sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.begin(); itang !=sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.end(); ++itang)
                        newcopy.nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_[itang - sequences_[isequence].nodes_[ilfn].cc_[icc - sequences_[isequence].nodes_[ilfn].cc_.begin()].tangents_.begin()].set_used(itang->used() );
                    }
              }
              else{

                for(vector<topology::cell_triplet>::iterator iccc=sequences_[isequence].nodes_[ilfn].ccc_.begin(); iccc != sequences_[isequence].nodes_[ilfn].ccc_.end(); ++iccc)
                  if( (size_t)(iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()) != ilink )
                    {
                      newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].set_free( iccc->free());
                      newcopy.nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].set_begun( iccc->begun());

                      for(vector<topology::joint>::iterator ijoint=sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.begin(); ijoint !=sequences_[isequence].nodes_[ilfn].ccc_[iccc - sequences_[isequence].nodes_[ilfn].ccc_.begin()].joints_.end(); ++ijoint)
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
          
            NCOPY ++;
            if( newcopy.nodes().size() != 2 )
              {
                make_name(&newcopy);
                sequences_.push_back( newcopy );
                m.message(" finished track [", sequences_.size()-1, "] ", mybhep::VERBOSE); fflush(stdout);
                clean_up_sequences();
              }
            else{
              add_pair(&newcopy);
            }
          }

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

      clock.stop_all();

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
  bool sequentiator::evolve(topology::sequence *sequence){
    //*************************************************************


    if( late() )
      return false;

    clock.start(" sequentiator: evolve ","cumulative");

    clock.start(" sequentiator: evolve: part A ","cumulative");

    size_t s = sequence->nodes().size();

    m.message(" evolving sequence of size", s, mybhep::VVERBOSE); fflush(stdout);

    // protection
    if( s < 1 )
      {
        m.message(" problem: sequence has lenght ", s, "... stop evolving ", mybhep::NORMAL); fflush(stdout);
        clock.stop(" sequentiator: evolve: part A ");
        clock.stop(" sequentiator: evolve ");
        return false;
      }


    if( level >= mybhep::VVERBOSE )
      print_a_sequence(sequence);

    if( s == 3 ){
      clock.start(" sequentiator: get link index ","cumulative");
      size_t it1 = sequence->get_link_index_of_cell(0, sequence->nodes()[1].c());
      if( it1 >= sequence->nodes_[0].cc_.size() ){
        m.message(" problem: it1 ", it1, " nodes size ", sequence->nodes_.size(), " cc size ", sequence->nodes_[0].cc_.size(), mybhep::NORMAL); fflush(stdout);
        clock.stop(" sequentiator: evolve: part A ");
        clock.stop(" sequentiator: evolve ");
        return false;
      }
      sequence->nodes_[0].cc_[it1].set_all_used();

      clock.stop(" sequentiator: get link index ");
    }

    clock.stop(" sequentiator: evolve: part A ");
    clock.start(" sequentiator: evolve: part B ","cumulative");

    // check if there is a possible link
    size_t ilink;
    topology::experimental_point newp;
    clock.start(" sequentiator: pick new cell ","cumulative");
    bool there_is_link = sequence->pick_new_cell(&ilink, &newp, local_cluster_);
    clock.stop(" sequentiator: pick new cell ");

    if( !there_is_link ){
      m.message(" no links could be added... stop evolving ", mybhep::VERBOSE); fflush(stdout);
      clock.start(" sequentiator: evolve: part B: set free level ","cumulative");
      clock.start(" sequentiator: set free level ","cumulative");
      sequence->set_free_level();
      clock.stop(" sequentiator: set free level ");
      clock.stop(" sequentiator: evolve: part B: set free level ");
      clock.stop(" sequentiator: evolve: part B ");
      clock.stop(" sequentiator: evolve ");
      return false;
    }

    topology::cell newcell = sequence->last_node().links()[ilink];
    clock.start(" sequentiator: evolve: part B: noc ","cumulative");
    topology::node newnode = local_cluster_.node_of_cell(newcell);
    //  topology::node newnode = local_cluster_.nodes()[local_cluster_.node_index_of_cell(newcell)];
    clock.stop(" sequentiator: evolve: part B: noc ");
    newnode.set_free(false); // standard initialization

    clock.stop(" sequentiator: evolve: part B ");
    clock.start(" sequentiator: evolve: part C ","cumulative");

    if( s == 1 )
      {
        // since it's the 2nd cell, only the four
        // standard alternatives are available (which will be treated out of "Evolve"), so
        // this link has no freedom left

        sequence->nodes_.push_back( newnode );
        clock.start(" sequentiator: set free level ","cumulative");
        sequence->set_free_level();
        clock.stop(" sequentiator: set free level ");

        clock.stop(" sequentiator: evolve: part C ");
        clock.stop(" sequentiator: evolve ");
        return true;
      }


    newnode.set_ep(newp);

    sequence->nodes_.push_back( newnode );
    m.message(" points have been added ", mybhep::VERBOSE); fflush(stdout);

    clock.start(" sequentiator: set free level ","cumulative");
    sequence->set_free_level();
    clock.stop(" sequentiator: set free level ");

    clock.stop(" sequentiator: evolve: part C ");
    clock.stop(" sequentiator: evolve ");
    return true;



  }



  //*************************************************************
  bool sequentiator::good_first_node(topology::node node, topology::node& first_node){
    //*************************************************************

    clock.start(" sequentiator: good first node ", "cumulative");

    string type = node.topological_type();

    // check that node is not in the middle of a cell_triplet
    if( type != "VERTEX" &&
        type != "MULTI_VERTEX" ){
      clock.stop(" sequentiator: good first node ");
      m.message(" not a good first node: type ", type, mybhep::VVERBOSE); fflush(stdout);
      return false;
    }


    std::vector<size_t> done_connections;
    // check that node has never been added to a sequence
    for(vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      if( iseq->has_cell(node.c()) ){
        if( type == "VERTEX" ){
        
	  m.message(" not a good first node: already used as vertex in seuqence ", iseq - sequences_.begin(), mybhep::VVERBOSE); fflush(stdout);

          clock.stop(" sequentiator: good first node ");
          return false;
        }
        else{ // multi-vertex
          if( iseq->nodes_.size() > 1 ){
            size_t connection_node;
            if( iseq->nodes_[0].c().id() == node.c().id() )
              connection_node = 1;
            else if( iseq->last_node().c().id() == node.c().id() ){
              connection_node = iseq->nodes_.size() - 2;
            }
            else{
              m.message(" problem: multi-vertex ", node.c().id(), " belongs to sequence ", iseq->name(), " but not as first or last cell", mybhep::NORMAL);
              continue;
            }
	    // add to done_connections cell ids of those cells
	    // that have already been connected to NODE in other sequences
            vector<size_t>::iterator fid = std::find(done_connections.begin(),
                                                     done_connections.end(),
                                                     iseq->nodes_[connection_node].c().id());

            if( fid == done_connections.end())
              done_connections.push_back(iseq->nodes_[connection_node].c().id());
          }
        }
      }
  
    if( type == "MULTI_VERTEX" ){
      for(size_t i=0; i<done_connections.size(); i++){
        size_t cc_index = 0;
        if( !node.has_couplet(done_connections[i],  &cc_index) )
          m.message(" problem: multi-vertex ", node.c().id(), " should link to cell ", done_connections[i], " but has not such couplet", mybhep::NORMAL);
        else{
          m.message(" multi-vertex ", node.c().id(), " has already been added to a sequence connecting to cell ", done_connections[i], " so couplet ", cc_index, " will be erased", mybhep::VERBOSE);
          node.cc_.erase(node.cc_.begin() + cc_index);
        }
      }
    }
  
    first_node = node;

    clock.stop(" sequentiator: good first node ");
    return true;


  }


  //*************************************************************
  void sequentiator::make_families() {
    //*************************************************************

    clock.start(" sequentiator: make families ", "cumulative");

    families_.clear();

    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){

      size_t ipart = iseq - sequences_.begin();
      size_t this_family = mybhep::int_from_string(iseq->family());

      bool found = false;
      size_t ifam;
      for(size_t i=0; i<families_.size(); i++)
        if( families_[i][0] == this_family )
          {
            found = true;
            ifam = i;
            break;
          }
    
      if( found )
        {
          bool added = false;
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
          std::vector<size_t> Fam;
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

    clog << " families " << endl;

    for(size_t i=0; i<families_.size(); i++)
      {
        clog << "[" << families_[i][0] << "] (";
        for(size_t j=0; j<families_[i].size(); j++)
          {
            if( j == 0 ) continue;
            clog << " " << families_[i][j];
          }
        clog << ")" << endl;
      }

    return;
  }




  //*************************************************************
  bool sequentiator::make_scenarios(topology::tracked_data &td){
    //*************************************************************

    clock.start(" sequentiator: make scenarios ", "cumulative");

    if( level >= mybhep::VERBOSE)
      print_families();

    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){
      if( late() ){
        td.set_skipped(true);
        return false;
      }

      m.message(" begin scenario with sequence ", iseq->name(), mybhep::VVERBOSE);
      if( level >= mybhep::VVERBOSE)
        print_a_sequence( &(*iseq) );
    
      topology::scenario sc;
      sc.sequences_.push_back(*iseq);
      sc.calculate_n_free_families(td.get_cells(), td.get_calos());
      sc.calculate_n_overlaps(td.get_cells(), td.get_calos());
      sc.calculate_chi2();
    
      size_t jmin, nfree, noverlaps;
      double Chi2;
      int ndof;
      while( can_add_family(sc, &jmin, &nfree, &Chi2, &noverlaps, &ndof, td) )
        {
          m.message(" best sequence to add is ", jmin, mybhep::VVERBOSE);
          if( level >= mybhep::VVERBOSE)
            print_a_sequence( &(sequences_[jmin]) );
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
        print_a_scenario(&(scenarios_[index_tmp]));


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
  
    for(vector<topology::scenario>::iterator sc=scenarios_.begin(); sc!=scenarios_.end(); ++sc){
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
#else
    clock.start(" sequentiator: copy scenario ", "cumulative");
    topology::scenario tmpmin = sc;
    clock.stop(" sequentiator: copy scenario ");
#endif

    for(vector<topology::sequence>::iterator jseq=sequences_.begin(); jseq!=sequences_.end(); ++jseq)
      {
      
        bool found = false;
        for(vector<topology::sequence>::iterator iseq=sc.sequences_.begin(); iseq!=sc.sequences_.end(); ++iseq)
          if( iseq->name() == jseq->name() )
            {
              found = true;
              break;
            }
        if( found )
          continue;

#if 1
        clock.start(" sequentiator: copy logic scenario ", "cumulative");
        topology::logic_scenario tmp = tmpsave;
        clock.stop(" sequentiator: copy logic scenario ");
        clock.start(" sequentiator: copy logic sequence ", "cumulative");
        tmp.sequences_.push_back(topology::logic_sequence(*jseq));
        clock.stop(" sequentiator: copy logic sequence ");
#else
        clock.start(" sequentiator: copy scenario ", "cumulative");
        topology::scenario tmp = sc;
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
          print_a_sequence( &(*jseq) );
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
  void sequentiator::interpret_physics(std::vector<topology::calorimeter_hit> calos){
    //*************************************************************

    clock.start(" sequentiator: interpret physics ", "cumulative");

    m.message(" interpreting physics of ", sequences_.size(), " sequences ", mybhep::VVERBOSE); fflush(stdout);
    for(vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      {
        if( iseq->nodes().size() <= 2 ) continue;

        iseq->calculate_helix();
        iseq->calculate_charge();
        iseq->calculate_momentum(bfield);

        // match to calorimeter
        {

          m.message(" extrapolate decay vertex with ", calos.size(), " calo hits " , mybhep::VVERBOSE);
        
          double min = mybhep::default_min;
          size_t imin = mybhep::default_integer;
          topology::experimental_point extrapolation, extrapolation_local;
          bool found = false;
          for(std::vector<topology::calorimeter_hit>::iterator ic=calos.begin(); ic != calos.end(); ++ic){
          
            m.message( " trying to extrapolate to calo hit ", ic - calos.begin(), " id ", ic->id(), " on view ", ic->pl_.view(), mybhep::VVERBOSE);
          
            if( !near(iseq->last_node().c(), ic->pl()) ){
              m.message( " not near " , mybhep::VVERBOSE);
              continue;
            }
          
          
            if( !iseq->intersect_plane_from_end(ic->pl(), &extrapolation_local) ){
              m.message( " no intersection " , mybhep::VVERBOSE);
              continue;
            }
          
            double dist = extrapolation_local.distance(ic->pl_.face()).value();
            if( dist < min ){
              min = dist;
              imin = ic->id();
              extrapolation = extrapolation_local;
              found = true;
              m.message( " new intersection with minimum distance " , dist , " position: " , extrapolation.x().value() ,   extrapolation.y().value(),  extrapolation.z().value() , mybhep::VVERBOSE);
            }
          
          }
        
          if( found ){
            if( imin >= calos.size() ){
              m.message( " problem: calo hit of id " , imin , " but n of calo hits is " , calos.size() , mybhep::VVERBOSE);
            }
            else
              iseq->set_decay_vertex(extrapolation, "calo", imin);
          }
        
        }
      
        // match to foil
	if( !iseq->nodes_.empty() ){
          
	  m.message( " extrapolate vertex on foil " , mybhep::VVERBOSE);
	  
	  topology::experimental_point extrapolation;
          
	  if( gap_number(iseq->nodes_[0].c() ) != 0 ){
	    m.message( " not near ", mybhep::VVERBOSE);
	  }
	  else{
	    if( SuperNemo ){
	      if( !iseq->intersect_plane_from_begin(get_foil_plane(), &extrapolation) )
                m.message(" no intersection ", mybhep::VVERBOSE);
              else
                iseq->set_vertex(extrapolation, "foil");
	    }else{
	      if( !iseq->intersect_circle_from_begin(get_foil_circle(), &extrapolation) )
		m.message(" no intersection ", mybhep::VVERBOSE);
	      else
		iseq->set_vertex(extrapolation, "foil");
	    }
	  }
        }

        if( level >= mybhep::VVERBOSE ){
          clog << " sequence " << iseq - sequences_.begin() << " has: " << endl;
          clog << " center "; iseq->center().dump();
          clog << " radius "; iseq->radius().dump(); clog << " " << endl;
          clog << " pitch "; iseq->pitch().dump(); clog << " " << endl;
          clog << " momentum "; iseq->momentum().dump(); clog << " " << endl;
          clog << " charge "; iseq->charge().dump(); clog << " " << endl;
          clog << " vertex "; iseq->vertex().dump(); clog << " " << endl;
          clog << " decay vertex "; iseq->decay_vertex().dump(); clog << " " << endl;
        }

      }


    clock.stop(" sequentiator: interpret physics ");

    return;

  }


  //*************************************************************
  void sequentiator::print_sequences(){
    //*************************************************************

    m.message(" sequence matrix is", mybhep::NORMAL); fflush(stdout);
    for(vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq)
      {
        clog << " [" << iseq - sequences_.begin() << "]";
        print_a_sequence( &(*iseq));
      }

    return;

  }


  //*************************************************************
  void sequentiator::print_a_sequence(topology::sequence *sequence){
    //*************************************************************

    if( sequence->Free() )
      {
        clog << "*";
        fflush(stdout);
      }

    for(vector<topology::node>::iterator inode=sequence->nodes_.begin(); inode!=sequence->nodes_.end(); ++inode)
      {
        clog << " " << (int)inode->c().id();

        if( inode->free() )
          clog << "* ";

        topology::experimental_vector v(inode->c().ep(),inode->ep());

        if( level >= mybhep::VVERBOSE ){
	  clog << "[" << v.x().value() << ", " << v.z().value() << "]";
	  
	  clog << "(";
	  
	  for(vector<topology::cell>::iterator ilink=(*inode).links_.begin(); ilink != (*inode).links_.end(); ++ilink){
	    int iccc = sequence->get_link_index_of_cell(inode - sequence->nodes_.begin(), *ilink);
	    
	    if( iccc < 0 ) continue;  // connection through a gap
	    
	    if( inode - sequence->nodes_.begin() < 1 ){
	      clog << "->" << inode->cc()[iccc].cb().id();
	      if( ilink->free() ){
		clog << "[*]";
	      }
	      clog << "[" << inode->cc_[iccc].iteration() << " "
		   << inode->cc()[iccc].tangents().size() << "]";
	      if( ! ilink->begun() )
		clog << "[n]";
	    }else{
	      clog << inode->ccc()[iccc].ca().id() << "<->" << inode->ccc()[iccc].cc().id();
	      if( ilink->free() ){
		clog << "[*]";
	      }
	      clog << "[" << inode->ccc_[iccc].iteration() << " "
		   << inode->ccc()[iccc].joints().size() << "]";
	      if( ! ilink->begun() )
		clog << "[n]";
	    }
	  }
	  
	  clog << " chi2 = " << inode->chi2();
	  
	  clog << " )";
	  
	}
      }
    
    clog << " " << endl;
  
    return;

  }

  //*************************************************************
  void sequentiator::print_scenarios(){
    //*************************************************************

    clog << " scenarios " << scenarios_.size() << endl; fflush(stdout);

    for(vector<topology::scenario>::iterator isc=scenarios_.begin(); isc!=scenarios_.end(); ++isc)
      {
        clog << " scenario " << isc - scenarios_.begin() << endl;
        print_a_scenario( &(*isc));
      }

    return;

  }


  //*************************************************************
  void sequentiator::print_a_scenario(topology::scenario *sc){
    //*************************************************************

    for(vector<topology::sequence>::iterator iseq=sc->sequences_.begin(); iseq!=sc->sequences_.end(); ++iseq){
      print_a_sequence( &(*iseq) );
    }

    clog << " nfree " << sc->n_free_families() << " noverls " << sc->n_overlaps() << " chi2 " << sc->chi2() << " prob " << sc->Prob() << endl;

    return;
  }


  //*************************************************************
  void sequentiator::add_pair(topology::sequence *newsequence){
    //*************************************************************

    clock.start(" sequentiator: add pair ", "cumulative");

    m.message(" adding pair ", mybhep::VVERBOSE); fflush(stdout);

    if( newsequence->nodes().size() != 2 ){
      m.message(" problem: pair has size ", newsequence->nodes().size(), mybhep::NORMAL);
      clock.stop(" sequentiator: add pair ");
      return;
    }

    topology::cell_couplet cc;
    if( !newsequence->nodes_[0].has_couplet(newsequence->nodes()[1].c(), &cc) ){
      m.message(" problem: node ", newsequence->nodes_[0].c().id(), " has no pair ", newsequence->nodes()[0].c().id(), "-", newsequence->nodes()[1].c().id(), mybhep::NORMAL);
      clock.stop(" sequentiator: add pair ");
      return;
    }

    topology::node na = newsequence->nodes()[0];
    topology::node nb = newsequence->nodes()[1];

    std::vector<topology::node> nodes;
    nodes.push_back(na);
    nodes.push_back(nb);

    bool erased = true;

    for(vector<topology::line>::iterator itangent=cc.tangents_.begin(); itangent != cc.tangents_.end(); ++itangent){
      topology::sequence pair(nodes, level, nsigma);

      pair.nodes_[0].set_ep(itangent->epa());
      pair.nodes_[0].set_free(false);

      pair.nodes_[1].set_ep(itangent->epb());
      pair.nodes_[1].set_free(false);

      if( itangent - cc.tangents_.begin() > 0 ){
        NCOPY ++;
        for(vector<topology::cell_couplet>::iterator icc=pair.nodes_[0].cc_.begin(); icc!=pair.nodes_[0].cc_.end(); ++icc)
          icc->set_all_used();
        for(vector<topology::cell_triplet>::iterator iccc=pair.nodes_[1].ccc_.begin(); iccc!=pair.nodes_[1].ccc_.end(); ++iccc)
          iccc->set_all_used();
      }

      clock.start(" sequentiator: set free level ","cumulative");
      pair.set_free_level();
      clock.stop(" sequentiator: set free level ");

      make_name(&pair);
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

    for(vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq != sequences_.end(); ++iseq)
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
  void sequentiator::make_table_of_true_and_reco_sequences(vector<topology::sequence> trueseqs){
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

    for(vector<topology::sequence>::iterator tp=trueseqs.begin(); tp != trueseqs.end(); ++tp){ // loop over true particles

      size_t NCommonMax = 0;
      size_t imin = 0;
    
      bool first = false;

      if( tp - trueseqs.begin() == 0 ) // first true particle
        first = true;

      for(vector<topology::sequence>::iterator ip = sequences_.begin(); ip != sequences_.end(); ++ip){      // loop over reco particles
      
        if( first ){
          max_common_hits_for_reco_track[ip - sequences_.begin()] = 0;
          best_matching_for_reco_track[ip - sequences_.begin()] = 0;
        }

        size_t ncommonhits = getCommonHits(*tp, *ip);

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

    if( level >= mybhep::VVERBOSE ){
      for(size_t i=0; i<sequences_.size(); i++){
        clog << " best matching for reco track " << i << " is true track " << true_sequence_of_reco_[i] << " with " << n_common_hits_for_reco_track_[i] << " common hits " << endl;
        clog << " hits of reco track: ";
        for(size_t j=0; j < sequences_[i].nodes().size(); j++){
          clog << " " << sequences_[i].nodes()[j].c().id();
        }
        clog << " " << endl; clog << " hits of true track: ";
        size_t index = true_sequence_of_reco_[i];
        for(size_t j=0; j < trueseqs[index].nodes().size(); j++){
          clog << " " << trueseqs[index].nodes()[j].c().id();
        }
        clog << " " << endl;

      }
      for(size_t i=0; i<trueseqs.size(); i++)
        clog << " best matching for true track " << i << " is reco track " << reco_sequence_of_true_[i] << endl;
    }

    //free(max_common_hits_for_reco_track);
    //free(best_matching_for_reco_track);

    return;
  }

  //*******************************************************************
  void sequentiator::rec_efficiency(vector<topology::sequence> trueseqs){
    //*******************************************************************

    if( sequences_.size() == 0 ) return;

    make_table_of_true_and_reco_sequences(trueseqs);

    for(vector<topology::sequence>::iterator itrueseq=trueseqs.begin(); itrueseq != trueseqs.end(); ++itrueseq){

      if( !itrueseq->primary() ) continue;

      m.message(" reconstructing efficiencies for true particle ", itrueseq - trueseqs.begin(), " of ", itrueseq->nodes().size(), " points, with name ", itrueseq->name(), mybhep::VVERBOSE);

      if( itrueseq->nodes().size() == 0 )
        continue; 

      size_t ireco = reco_sequence_of_true_[itrueseq - trueseqs.begin()];

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

    for(vector<topology::node>::iterator dnode = dp.nodes_.begin(); dnode != dp.nodes_.end(); ++dnode){

      for(vector<topology::node>::iterator tnode = tp.nodes_.begin(); tnode != tp.nodes_.end(); ++tnode){

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

      for(vector<topology::node>::iterator dn = dp.nodes_.begin(); dn != dp.nodes_.end(); ++dn){
      
      topology::node jnode = *dn;
      
      for(vector<topology::node>::iterator tn = tp.nodes_.begin(); tn != tp.nodes_.end(); ++tn){
        
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
      for(vector<topology::node>::iterator tn = tp.nodes_.begin(); tn != tp.nodes_.end(); ++tn){
        
      if( tn - tp.nodes_.begin() == 0 ) advance(tn, 1);
        
      topology::node inode = *tn;
      vector<topology::node>::iterator tnold = tp.nodes_.begin();
      advance(tnold, tn - tp.nodes_.begin() - 1);
      topology::node inodeold = *tnold;
        
      bool connected = false;
        
      for(vector<topology::node>::iterator dn = dp.nodes_.begin(); dn != dp.nodes_.end(); ++dn){
      if( dn - dp.nodes_.begin() == 0 ) advance(dn, 1);
          
      topology::node jnode = *dn;
      vector<topology::node>::iterator dnold = dp.nodes_.begin();
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
        print_a_sequence( &(*iseq) );
    
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
          print_a_sequence( &(*jseq) );
      
      
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
      
        string delenda_family = sequences_[index].family();
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

    for(std::vector<topology::sequence>::iterator iseq=sequences_.begin(); iseq!=sequences_.end(); ++iseq){

      if( late() )
        return false;

      size_t ifam = mybhep::int_from_string(iseq->family());
      if( matched[ifam] ) continue;

      if( !good_first_to_be_matched(*iseq) ){
        continue;
      }

      m.message(" begin matching with sequence ", iseq->name(), mybhep::VERBOSE);
      if( level >= mybhep::VERBOSE)
        print_a_sequence( &(*iseq) );
    
      size_t jmin;
      bool invertA, invertB, first = true;
      topology::sequence newseq = *iseq;
      while( can_match(newseq, &jmin, invertA, invertB) )
        {
          m.message(" best matching is ", sequences_[jmin].name(), " invertA ", invertA, " invertB ", invertB, mybhep::VERBOSE);
          if( level >= mybhep::VVERBOSE)
            print_a_sequence( &(sequences_[jmin]) );

          newseq = newseq.match(sequences_[jmin], invertA, invertB);
          if( level >= mybhep::VERBOSE)
            print_a_sequence( &newseq );

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

    for(vector<topology::sequence>::iterator jseq=sequences_.begin(); jseq!=sequences_.end(); ++jseq)
      {
      
        m.message(" try to match sequence", s.name(), " to ", jseq->name(), mybhep::VVERBOSE);
        if( level >= mybhep::VVERBOSE){
          print_a_sequence( &s );
          print_a_sequence( &(*jseq) );
        }

        // check that jseq's family is not the same as s
        if( s.same_families(*jseq) ){
          m.message(" ... forbidden, same family", jseq->family(), mybhep::VVERBOSE);
          continue;
        }

        bool invertA, invertB;
        if( !s.good_match(*jseq, invertA, invertB, NOffLayers) ){
          continue;
        }


        topology::sequence news = s.match(*jseq, invertA, invertB);
        double p = news.helix_Prob();
        double c = news.helix_chi2();
        int n = news.ndof();
      
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

    if( !nemo_sequences[0].has_decay_vertex() ){
      m.message(" reject: 1st track has no calo ", mybhep::VVERBOSE);
      return false;
    }


    if( !nemo_sequences[1].has_decay_vertex() ){
      m.message(" reject: 2nd track has no calo ", mybhep::VVERBOSE);
      return false;
    }

    std::vector<topology::calorimeter_hit> calos = __tracked_data.get_calos();

    if( nemo_sequences[0].calo_id() == nemo_sequences[1].calo_id() ){
      m.message(" reject: same calo ", nemo_sequences[0].calo_id(), mybhep::VVERBOSE);
      return false;
    }

    topology::calorimeter_hit caloA = calos[nemo_sequences[0].calo_id()];
    topology::calorimeter_hit caloB = calos[nemo_sequences[1].calo_id()];

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
