#include "clusterizer.h"
#include <mybhep/system_of_units.h>
#include <sys/time.h>
#include <limits>
#include <cmath>
#include <map>

namespace CAT {
  using namespace mybhep;
  using namespace std;

  void clusterizer::_set_defaults ()
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
    lastlayer = 0;
    vel  = std::numeric_limits<double>::quiet_NaN ();
    rad  = std::numeric_limits<double>::quiet_NaN ();
    len  = std::numeric_limits<double>::quiet_NaN ();
    CellDistance  = std::numeric_limits<double>::quiet_NaN ();
    xsize = ysize = zsize = std::numeric_limits<double>::quiet_NaN ();
    calo_X = calo_Y = calo_Z = std::numeric_limits<double>::quiet_NaN ();
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
    nofflayers = 0;
    PrintMode = false;
    SuperNemo = true;
    SuperNemoChannel = false;
    NemoraOutput = false;
    N3_MC = false;
    MaxTime = std::numeric_limits<double>::quiet_NaN ();
    doDriftWires = true;
    DriftWires.clear ();
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
  clusterizer::clusterizer(mybhep::gstore st){
    //*************************************************************

    _set_defaults ();
    set_level (st.fetch_sstore("VERBOSITY"));

 
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
  clusterizer::clusterizer(void){
    //*************************************************************
    _set_defaults ();
    return;
  }

  //*************************************************************
  clusterizer::~clusterizer() {
    //*************************************************************
    clog << "DEVEL: CAT::clusterizer::~clusterizer: Done." << endl;
  }

  //*************************************************************
  bool clusterizer::_initialize(){
    //*************************************************************
    if( PrintMode )
      {
        initializeHistos();
      }
     /*
      if( !SuperNemo )
      {

      run_time = 0.;
      run_list.clear();

      }
    */
   
    if (doDriftWires)
      {
        GenerateWires();
      }
    return true;
  }

  //*************************************************************
  bool clusterizer::initialize( mybhep::sstore store, mybhep::gstore gs , mybhep::EventManager2 *eman) {
    //*************************************************************
  
    m.message("\n Beginning algorithm clusterizer \n",mybhep::VERBOSE); 

    clock.start(" clusterizer: initialize ");
  
    //----------- read dst param -------------//
  
    readDstProper(store, eman);
  
    //------- end of read pram -----------//

    _initialize();
 
    clock.stop(" clusterizer: initialize ");

    return true;
  }

  //*************************************************************
  bool clusterizer::initialize( ) {
    //*************************************************************
    m.message("CAT::clusterizer::initialize: Entering...",mybhep::NORMAL); 
  
    m.message("\n Beginning algorithm clusterizer \n",mybhep::VERBOSE);
  
    //----------- read dst param -------------//
  
    readDstProper();
  
    //------- end of read pram -----------//

    _initialize();
     
    m.message("CAT::clusterizer::initialize: Done.",mybhep::NORMAL); 
    return true;
  }

  //*************************************************************
  void clusterizer::initializeHistos( void ) {
    //*************************************************************
  
    //  hman.h1("chi2_triplet", "chi2 for each cell triplet", 100, -0.05, 20.05);
    //  hman.h1("prob_triplet", "probability that chi2 is larger than observed for a good triplet", 100, -0.05, 1.05);
    //  hman.h1("largest_true_kink", "largest kink in a true particle", 100, 0., 180.);
    //  hman.h2("largest_true_kink_position", "position of largest kink in a true particle", 100, -rad/1.9, rad/1.9, 100, -rad/1.9, rad/1.9);

    return;

  }


  //*************************************************************
  bool clusterizer::finalize() {
    //*************************************************************
    
    clock.start(" clusterizer: finalize ");

    m.message("\n Ending algorithm clusterizer \n ",mybhep::NORMAL); 
  
    m.message("Initial events: ", InitialEvents, mybhep::NORMAL);  
    m.message("Skipped events: ", SkippedEvents, "(", 100.*SkippedEvents/InitialEvents, "\%)", mybhep::NORMAL);  

    if( PrintMode )
      {
        finalizeHistos();
      }
    clock.stop(" clusterizer: finalize ");

    clock.dump();

    _set_defaults ();
    return true;
  }

  //*************************************************************
  void clusterizer::finalizeHistos( void ) {
    //*************************************************************
    
    //  if( PrintMode )
    //    hman.save();

    m.message("Writing histograms in ", hfile, mybhep::NORMAL);

    return;
  }

  //*************************************************************
  void clusterizer::readDstProper(mybhep::sstore global, mybhep::EventManager2 *eman) {
    //*************************************************************
  
    clock.start(" clusterizer: read dst properties ");

    if (! global.find("GEOM_MODULES"))
      {
        _MaxBlockSize = 1;
        m.message(" no bar design, MODULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
        
      }
    else
      {
        _MaxBlockSize = mybhep::int_from_string(global.fetch("GEOM_MODULES"));
        m.message(" bar design with MODULES = ",_MaxBlockSize,"\n",mybhep::NORMAL);
        
      }

    if(!global.find("NEMO3"))
      {
        SuperNemo = true;
        m.message("SuperNemo kind of data",mybhep::NORMAL); 
      }
    else
      {
        SuperNemo = false;
        m.message("Nemo-3 kind of data",mybhep::NORMAL); 
      }

    if(global.find("NEMORA_model"))
      {
        NemoraOutput = true;
        m.message("data already processed by Nemora",mybhep::NORMAL); 
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

    if( SuperNemo )
      {
        xsize = mybhep::double_from_string(global.fetch("GEOM_CHAMBER_width"));
        zsize = mybhep::double_from_string(global.fetch("GEOM_CHAMBER_length"));

        mybhep::vector_from_string(global.fetch("GEOM_gaps_Z"),gaps_Z);
         
        GG_GRND_diam = mybhep::double_from_string(global.fetch("GEOM_GG_GRND_diam"));
      
        GG_CELL_diam = rad;

        CHAMBER_X = xsize;

        SOURCE_thick= mybhep::double_from_string(global.fetch("GEOM_SOURCE_thick"));
      }
    else
      {
        InnerRadius = mybhep::double_from_string(global.fetch("CALO_inner_radius"));
        OuterRadius = mybhep::double_from_string(global.fetch("CALO_outer_radius"));
        FoilRadius = mybhep::double_from_string(global.fetch("GEOM_FOIL_radius"));


        xsize = 2.*OuterRadius;
        zsize = 2.*OuterRadius;
      }

    calo_X= mybhep::double_from_string(global.fetch("GEOM_CALO_BLOCK_width"));
    calo_Y= mybhep::double_from_string(global.fetch("GEOM_CALO_BLOCK_height"));
    calo_Z= mybhep::double_from_string(global.fetch("GEOM_CALO_BLOCK_thick"));

    compute_lastlayer();
    /*
    lastlayer = 0;
    for(size_t i=0; i<planes_per_block.size(); i++)
      lastlayer += (int)planes_per_block[i];
    */

    m.message("",mybhep::NORMAL); 

    m.message("pmax",pmax,"MeV",mybhep::NORMAL); 
    m.message("small radius",SmallRadius,"mm",mybhep::NORMAL); 
    m.message("tangent phi: ",TangentPhi,mybhep::NORMAL); 
    cout << " aaa cout " << endl;
    clog << " aaa  clog " << endl;
    m.message(" aaa message ", mybhep::NORMAL);
    m.message("tangent theta",TangentTheta,mybhep::NORMAL); 
    m.message("small number",SmallNumber,"mm",mybhep::NORMAL); 
    m.message("quadrant angle",QuadrantAngle,mybhep::NORMAL); 
    m.message("ratio",Ratio,mybhep::NORMAL); 
    m.message("compatibility distance", CompatibilityDistance,mybhep::NORMAL); 
    m.message("maximum chi2", MaxChi2, mybhep::NORMAL); 
    m.message("xsize is read as",xsize,"mm",mybhep::NORMAL); 
    m.message("ysize is read as",ysize,"mm",mybhep::NORMAL); 
    m.message("zsize is read as",zsize,"mm",mybhep::NORMAL); 
    m.message("plasma speed is read as: ",vel,"mm/ns",mybhep::NORMAL); 
    m.message("wire length is read as: ",len,"mm",mybhep::NORMAL); 
    m.message("cell diameter is read as: ",rad,"mm",mybhep::NORMAL); 
    m.message("distance from wire to wire is read as: ",CellDistance,"mm",mybhep::NORMAL); 
    m.message("verbosity level: ", level ,mybhep::NORMAL); 
    m.message("",mybhep::NORMAL); 
  
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
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_K0";
        if(global.find(pname))
          {
            k0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_K1";
        if(global.find(pname))
          {
            k1=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_K2";
        if(global.find(pname))
          {
            k2=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_K3";
        if(global.find(pname))
          {
            k3=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_TH0";
        if(global.find(pname))
          {
            th0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_TH1";
        if(global.find(pname))
          {
            th1=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_TH2";
        if(global.find(pname))
          {
            th2=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_TH3";
        if(global.find(pname))
          {
            th3=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_L0";
        if(global.find(pname))
          {
            l0=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      
        pname="N3GG_L1";
        if(global.find(pname))
          {
            l1=mybhep::double_from_string(global.fetch(pname));
          }
        else
          {
            m.message("+++ NEMO3 GG ERROR, GLOBAL PROPERTY NOT FOUND IN DST",pname,mybhep::NORMAL); 
            exit(1);
          }
      }
  
    clock.stop(" clusterizer: read dst properties ");

    return;
  }


  //*************************************************************
  void clusterizer::readDstProper() {
    //*************************************************************
    m.message("CAT::clusterizer::readDstProper: Entering...",mybhep::NORMAL); 
    
    //clock.start(" clusterizer: read dst properties ");

    if (_MaxBlockSize <= 0)
      {
        _MaxBlockSize = 1;
        m.message("CAT::clusterizer::readDstProper: no bar design, MODULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
      }

    if(SuperNemo)
      {
        m.message("CAT::clusterizer::readDstProper: SuperNemo kind of data",mybhep::NORMAL); 
        if (num_blocks <= 0)
          {
            // Default :
            set_num_blocks (1);
            planes_per_block.at (0) = 9;
          }
     }
    else
      {
        m.message("CAT::clusterizer::readDstProper: Nemo-3 kind of data",mybhep::NORMAL); 
        if (num_blocks <= 0)
          {
            // Default :
            set_num_blocks (3);
            planes_per_block.at (0) = 4;
            planes_per_block.at (1) = 2;
            planes_per_block.at (2) = 3;
          }
      }

    compute_lastlayer();
    /*
    lastlayer = 0;
    for(size_t i=0; i<planes_per_block.size(); i++)
      lastlayer += (int)planes_per_block[i];
    */

    m.message("",mybhep::NORMAL); 
    m.message("small radius",SmallRadius,"mm",mybhep::NORMAL); 
    m.message("tangent phi",TangentPhi,mybhep::NORMAL); 
    m.message("tangent theta",TangentTheta,mybhep::NORMAL); 
    m.message("small number",SmallNumber,"mm",mybhep::NORMAL); 
    m.message("quadrant angle",QuadrantAngle,mybhep::NORMAL); 
    m.message("ratio",Ratio,mybhep::NORMAL); 
    m.message("compatibility distance", CompatibilityDistance,mybhep::NORMAL); 
    m.message("maximum chi2", MaxChi2, mybhep::NORMAL); 
    m.message("nsigma", nsigma, mybhep::NORMAL); 
    if (SuperNemo && SuperNemoChannel)
      {
        m.message("cell pitch: ",GG_CELL_pitch,"mm",mybhep::NORMAL); 
      }
    else
      {
        m.message("pmax",pmax,"MeV",mybhep::NORMAL); 
        m.message("xsize is",xsize,"mm",mybhep::NORMAL); 
        m.message("ysize is",ysize,"mm",mybhep::NORMAL); 
        m.message("zsize is",zsize,"mm",mybhep::NORMAL); 
        m.message("plasma speed is: ",vel,"mm/ns",mybhep::NORMAL); 
        m.message("wire length is: ",len,"mm",mybhep::NORMAL); 
        m.message("cell diameter is: ",rad,"mm",mybhep::NORMAL); 
        m.message("distance from wire to wire is: ",CellDistance,"mm",mybhep::NORMAL); 
      }
    if( !SuperNemo )
      {
        m.message("foil radius: ",FoilRadius,"mm",mybhep::NORMAL); 
      }
    m.message("verbosity print level:", level, mybhep::NORMAL); 
  
    //parameters for error parametrization

    //clock.stop(" clusterizer: read dst properties ");
    m.message("CAT::clusterizer::readDstProper: Done.",mybhep::NORMAL); 

    return;
  }


  //*************************************************************
  double clusterizer::long_resolution(double Z, double d[3])const{
    //*************************************************************
      
    double xp = abs(Z*2./len);
    double kx = k0 + k1*xp + k2*xp*xp + k3*xp*xp*xp;
    double thx = th0 + th1*xp + th2*xp*xp + th3*xp*xp*xp;
    double tgth2=(d[0]*d[0] + d[2]*d[2])/(d[1]*d[1]);
    double sigma=sqrt(kx*sigma0*sigma0*(1-xp*xp)+thx*thx/tgth2);

    return sigma;
  }


  //*************************************************************
  double clusterizer::long_resolution_1cthd(double Zdist)const{
    //*************************************************************
    double L = Zdist;
    double sigma = l0 + l1*L;

    return sigma;
  }


  //*************************************************************  
  double clusterizer::GetYError( double y, float tup, float tdown, double direction[3]){
    //*************************************************************  

    double erry;

    if( tup != 0. && tdown != 0.){
      erry = long_resolution(y, direction);
    }
    else if( tup != 0. && tdown == 0.){
      erry = long_resolution_1cthd(len/2.-y);
    }
    else if( tup == 0. && tdown != 0.){
      erry = long_resolution_1cthd(len/2.+y);
    }
    else{
      erry = len/2.;
    }

    return erry;

  }


  //*************************************************************
  void clusterizer::FillYPositions( mybhep::event& evt ){
    //*************************************************************

    for(size_t i=0; i<evt.digi_particles().size(); i++){
      if( evt.digi_particles()[i]->find_property("SUNAMI") )
        FillYPositions( evt.digi_particles()[i] );
    }

    return;

  }

  //*************************************************************
  void clusterizer::FillYPositions( mybhep::particle* p ){
    //*************************************************************

    for(size_t i=0; i<p->hits("trk").size(); i++){
      FillYPosition(p->hits("trk")[i]);
    }

    return;

  }

  //*************************************************************
  void clusterizer::FillYPosition( mybhep::hit* h ){
    //*************************************************************

    double erry = 0.;

    if( SuperNemo ){
      float tup, tdown, tdelay;
    
      string ttime = h->fetch_property("TTIME");
      string btime = h->fetch_property("BTIME");
      string atime = h->fetch_property("ATIME");
    
      tup = mybhep::float_from_string(ttime);
      tdown = mybhep::float_from_string(btime);
      tdelay = mybhep::float_from_string(atime);
    
      if( isnan(tup) || isinf(tup) )
        tup = -1.;
      if( isnan(tdown) || isinf(tdown) )
        tdown = -1.;
      if( isnan(tdelay) || isinf(tdelay) )
        tdelay = -1.;
    
    
      vector<float> cellpos;
      mybhep::vector_from_string(h->fetch_property("CELL_POS"), cellpos);
    
      if( cellpos.size() != 3 ){
        m.message("problem: cell_pos size is ", cellpos.size(), mybhep::MUTE);
        return;
      }

      ///////////////////////////////////////////////////////////
      //                                                       //
      //  len/2 - y = tup*v                                    //
      //  y - (-len/2) = tdown*v                               //
      //                                                       //
      //  so:                                                  //
      //                                                       //
      //  (tup + tdown)*v = len                                //
      //  (tup - tdown)*v = -2y                                //
      //                                                       //
      ///////////////////////////////////////////////////////////
    
    
      if( tup != 0. && tdown != 0.)
        cellpos[1] = (-1.)*((tup - tdown)/(tup + tdown))*(len/2.);
      else if( tup != 0. && tdown == 0.)
        cellpos[1] = (len/2.) - tup*vel;
      else if( tup == 0. && tdown != 0.)
        cellpos[1] = tdown*vel - (len/2.);
      else
        cellpos[1] = 0.;
    
      h->change_property("CELL_POS", mybhep::vector_to_string(cellpos));
  
      double dir[3];
      dir[0] = 0.;
      dir[1] = 0.;
      dir[2] = 1.;
    
      erry = GetYError((double)cellpos[1], tup, tdown, dir);

      if( isnan( erry ) || isinf(erry) )
        erry = -1.;

      if( !h->find_property("ERRY") )
        h->add_property("ERRY",mybhep::to_string(erry));
    }
    else{
      erry =  mybhep::double_from_string(h->fetch_property("ERRY"));
    }
  
    return;

  }

  //*************************************************************
  void clusterizer::FillTrueVertexes( mybhep::event& evt ){
    //*************************************************************

    for(size_t i=0; i<evt.true_particles().size(); i++){
      FillTrueVertexes( evt.true_particles()[i] );
    }

    return;

  }

  //*************************************************************
  void clusterizer::FillTrueVertexes( mybhep::particle* tp ){
    //*************************************************************

    if( tp->find_property("foil_vertex") )
      tp->change_property("foil_vertex", "1");
    else
      tp->add_property("foil_vertex", "1");

    return;

  }


  //*************************************************************
  void clusterizer::GenerateWires( void ){
    //*************************************************************
    m.message("CAT::clusterizer::GenerateWires: Entering...",mybhep::NORMAL); 

    //clock.start(" clusterizer: generate wires ");

    DriftWires.clear();

    if( SuperNemo )
      {
        m.message("CAT::clusterizer::GenerateWires: SuperNemo geometry...",mybhep::NORMAL); 

        // TRACKING GG BLOCKS
        double theta = M_PI/8.;
        m.message("CAT::clusterizer::GenerateWires: rad=",
                  rad,
                  mybhep::NORMAL); 
        if (isnan (GG_CELL_pitch))
          {
            GG_CELL_pitch = rad*cos(theta);
          }
        m.message("CAT::clusterizer::GenerateWires: GG_CELL_pitch=",
                  GG_CELL_pitch,
                  mybhep::NORMAL); 
        m.message("CAT::clusterizer::GenerateWires: GG_GRND_diam=",
                  GG_GRND_diam,
                  mybhep::NORMAL); 
        m.message("CAT::clusterizer::GenerateWires: CHAMBER_X=",
                  CHAMBER_X,
                  mybhep::NORMAL); 
        if (num_cells_per_plane <= 0)
          {
            num_cells_per_plane=(int)((CHAMBER_X-GG_GRND_diam)/GG_CELL_pitch); 
          }
        m.message("CAT::clusterizer::GenerateWires: num_cells_per_plane=",
                  num_cells_per_plane,
                  mybhep::NORMAL); 
        GG_BLOCK_X = num_cells_per_plane*GG_CELL_pitch+GG_GRND_diam;
        m.message("CAT::clusterizer::GenerateWires: GG_BLOCK_X=",
                  GG_BLOCK_X,
                  mybhep::NORMAL); 

        // 2012-03-24 FM : Use dynamic 'vector<double>' in place of invalid static 'double []':
        std::vector<double> GG_BLOCK_thick;
        std::vector<double> GG_BLOCK_posZ;
        GG_BLOCK_thick.assign (num_blocks, std::numeric_limits<double>::quiet_NaN ());
        GG_BLOCK_posZ.assign (num_blocks, std::numeric_limits<double>::quiet_NaN ());
        double distance = SOURCE_thick/2.;
        m.message("CAT::clusterizer::GenerateWires: SOURCE_thick=",
                  SOURCE_thick,
                  mybhep::NORMAL); 
        //Calculating thickness and positions of blocks
        for(int i=0; i<num_blocks; i++){
          GG_BLOCK_thick[i]=(planes_per_block[i]*GG_CELL_pitch+GG_GRND_diam);
          GG_BLOCK_posZ[i] = distance + gaps_Z[i] + GG_BLOCK_thick[i]/2.;
          distance = GG_BLOCK_posZ[i] + GG_BLOCK_thick[i]/2.; 
        }      
        m.message("CAT::clusterizer::GenerateWires: DEVEL: **** STEP 1",mybhep::NORMAL); 
      
        int sign[2];
        sign[0] = 1;
        sign[1] = -1;

        for(size_t isign=0; isign<2; isign++)
          for (int iblock=0; iblock<num_blocks;iblock++){
          
            double block_pos[3];
            block_pos[2] = GG_BLOCK_posZ[iblock];
          
            // loop over planes in block
          
            double plane_pos_z0 = block_pos[2]-GG_BLOCK_thick[iblock]/2; 
          
            for(int iplane=0; iplane<planes_per_block[iblock];iplane++){
            
              double plane_pos_Z;
              plane_pos_Z=GG_GRND_diam/2.+GG_CELL_pitch/2.+iplane*GG_CELL_pitch;
              plane_pos_Z = plane_pos_Z + plane_pos_z0;
              plane_pos_Z *= sign[isign];

              for(int iwire=0; iwire<num_cells_per_plane; iwire++)
                {
                  double increment = GG_CELL_pitch*((double)iwire);
                  double xpos = -(CHAMBER_X-GG_GRND_diam)/2.+6.+increment+GG_CELL_pitch/2.;
                
                  POINT point;
                  point.x = xpos;
                  point.z = plane_pos_Z;
                  DriftWires.push_back( point );
                }
            
          
            }//end of loop over planes
        
          }//end of loop over blocks
      }
    else
      {
      
        size_t NOfWires[18];
        double FirstWireX[18];
        double FirstWireZ[18];
        double FirstWirePhi[18];
        double LayerRadius[18];
      
        NOfWires[0] = 320;
        NOfWires[1] = 320;
        NOfWires[2] = 320;
        NOfWires[3] = 320;
        NOfWires[4] = 280;
        NOfWires[5] = 280;
        NOfWires[6] = 240;
        NOfWires[7] = 240;
        NOfWires[8] = 240;
        NOfWires[9] = 360;
        NOfWires[10] = 360;
        NOfWires[11] = 360;
        NOfWires[12] = 360;
        NOfWires[13] = 400;
        NOfWires[14] = 400;
        NOfWires[15] = 460;
        NOfWires[16] = 460;
        NOfWires[17] = 460;
      
        FirstWireX[0] = 1521.;
        FirstWireX[1] = 1493.;
        FirstWireX[2] = 1465.;
        FirstWireX[3] = 1437.;
        FirstWireX[4] = 1271.;
        FirstWireX[5] = 1243.;
        FirstWireX[6] = 1077.;
        FirstWireX[7] = 1049.;
        FirstWireX[8] = 1021.;
        FirstWireX[9] = 1579.;
        FirstWireX[10] = 1607.;
        FirstWireX[11] = 1635.;
        FirstWireX[12] = 1663.;
        FirstWireX[13] = 1829.;
        FirstWireX[14] = 1857.;
        FirstWireX[15] = 2023.;
        FirstWireX[16] = 2051.;
        FirstWireX[17] = 2079.;
      
        FirstWireZ[0] = 10.57;
        FirstWireZ[1] = 10.375;
        FirstWireZ[2] = 10.181;
        FirstWireZ[3] = 9.9871;
        FirstWireZ[4] = 10.167;
        FirstWireZ[5] = 9.943;
        FirstWireZ[6] = 10.180;
        FirstWireZ[7] = 9.915;
        FirstWireZ[8] = 9.651;
        FirstWireZ[9] = 9.811;
        FirstWireZ[10] = 9.985;
        FirstWireZ[11] = 10.158;
        FirstWireZ[12] = 10.333;
        FirstWireZ[13] = 10.129;
        FirstWireZ[14] = 10.284;
        FirstWireZ[15] = 9.804;
        FirstWireZ[16] = 9.939;
        FirstWireZ[17] = 10.075;
      
      
        for(size_t i=0; i<18; i++)
          {
            LayerRadius[i] = sqrt(FirstWireX[i]*FirstWireX[i] + FirstWireZ[i]*FirstWireZ[i]);
            FirstWirePhi[i] = acos(FirstWireX[i]/LayerRadius[i]);
          
            for(size_t j=0; j<NOfWires[i]; j++)
              {
                double layerphi = 2.*M_PI/NOfWires[i];
                double ph = FirstWirePhi[i] + j*layerphi;
              
                POINT point;
                point.x = LayerRadius[i]*cos(ph);
                point.z = LayerRadius[i]*sin(ph);
              
                DriftWires.push_back( point );
              }
          }
      }


    //clock.stop(" clusterizer: generate wires ");
    m.message("CAT::clusterizer::GenerateWires: Done.",mybhep::NORMAL); 
   
    return;

  }


  //*******************************************************************
  size_t clusterizer::get_true_hit_index(mybhep::hit& hit, bool print){
    //*******************************************************************

    topology::node tn(hit, 0, SuperNemo, level, nsigma);
 
    for(std::vector<topology::cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic){
      if( ic->same_cell(tn.c()) )
        return ic->id();
    }

    if( print )
      m.message(" warning: can't find corresponding reco hit for true hit (", tn.c().ep().x().value(), ", ", tn.c().ep().y().value(), ", ", tn.c().ep().z().value(), ") layer", tn.c().layer(), mybhep::VVERBOSE);

    return 0;

  }

  //*******************************************************************
  size_t clusterizer::get_nemo_hit_index(mybhep::hit& hit, bool print){
    //*******************************************************************

    topology::node tn(hit, 0, SuperNemo, level, nsigma);
 
    for(std::vector<topology::cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic){
      if( ic->same_cell(tn.c()) )
        return ic->id();
    }

    if( print )
      m.message(" warning: can't find corresponding reco hit for nemo hit (", tn.c().ep().x().value(), ", ", tn.c().ep().y().value(), ", ", tn.c().ep().z().value(), ") layer", tn.c().layer(), mybhep::VVERBOSE);

    return 0;

  }

  //*******************************************************************
  size_t clusterizer::get_calo_hit_index(topology::calorimeter_hit c){
    //*******************************************************************

    for(std::vector<topology::calorimeter_hit>::iterator ic=calorimeter_hits_.begin(); ic!=calorimeter_hits_.end(); ++ic){
      if( ic->same_calo(c) )
        return ic->id();
    }

    m.message(" warning: can't find corresponding calo hit for nemo calo hit (", c.pl().center().x().value(), ", ", c.pl().center().y().value(), ", ", c.pl().center().z().value(), ") layer", c.layer(), mybhep::VVERBOSE);

    return 0;

  }


  //*******************************************************************
  bool clusterizer::read_event(mybhep::event& event_ref, topology::tracked_data & tracked_data_){
    //*******************************************************************

    clock.start(" clusterizer: read event ","cumulative");

    m.message(" local_tracking: reading event", mybhep::VERBOSE);

    cells_.clear();
    parts.clear();
    clusters_.clear();
    calorimeter_hits_.clear();
    true_sequences_.clear();
    nemo_sequences_.clear();

    bool bhep_input = true;

    if( bhep_input ){

      // read digi particles
      vector<mybhep::particle*> digi_parts;

      if( !SuperNemo && first_event)
        {
          first_event=false;

          N3_MC = true;
          if( event_ref.find_property("DATA") )
            if( mybhep::int_from_string(event_ref.fetch_property("DATA")) == 1 )
              N3_MC = false;  // data 
                                                                                                         
          if( N3_MC )
            m.message("Nemo3 MC ", mybhep::NORMAL);
          else
            m.message("Nemo3 data ", mybhep::NORMAL);
        }

      FillYPositions(event_ref);

      if( SuperNemo ){
        FillTrueVertexes(event_ref);
      }
  
      fill_fast_information(event_ref);

      event_ref.filter(mybhep::DIGI,"SUNAMI","1",digi_parts); // scans all event.digi_particles(),
      // and copies to "digi_parts" those ones having property "SUNAMI" = 1 
                                                                   
      if ( digi_parts.empty() )
        {
          m.message("Problem: there are no gg hits in this event.\n",mybhep::MUTE);
          
          return false;
        }

      if (digi_parts[0]->find_property("mID"))               // BAR design?? 
        {
          for (size_t pnr=0; pnr < digi_parts.size(); pnr++)
            if (digi_parts[pnr]->fetch_property("mID") == _moduleNR )
              parts.push_back(digi_parts[pnr]);
        }
      else                                               //NOT BAR DESIGN and OLD data 
        {
          //    for (size_t pnr=0; pnr < digi_parts.size(); pnr++)
          //      parts.push_back(digi_parts[pnr]);
          parts.push_back(digi_parts[0]);
        }

      if( parts.size() != 1 )
        return false;

      const vector<mybhep::hit*>& hits = parts[0]->hits("trk");

      for (size_t ihit=0; ihit<hits.size();ihit++){
        topology::cell c(*hits[ihit],ihit, SuperNemo, level, nsigma);
        cells_.push_back(c);
      }

      clock.start(" clusterizer: make calo hit ","cumulative");
      const vector<mybhep::hit*>& chits = parts[0]->hits("cal");
      for (size_t ihit=0; ihit<chits.size();ihit++){
        topology::calorimeter_hit ch = make_calo_hit(*chits[ihit], ihit);
        calorimeter_hits_.push_back(ch);
      }
      clock.stop(" clusterizer: make calo hit ");
    
      if( level >= mybhep::VVERBOSE )
        print_calos();
    
    

      read_true_sequences(event_ref);

      if( !SuperNemo )
        read_nemo_sequences(event_ref);
  
    }


    order_cells();
  
    setup_cells();

    tracked_data_.set_cells(cells_);

    tracked_data_.set_calos(calorimeter_hits_);

    tracked_data_.set_true_sequences(true_sequences_);

    tracked_data_.set_nemo_sequences(nemo_sequences_);

    clock.stop(" clusterizer: read event ");


    return true;


  }


  //*******************************************************************
  bool clusterizer::prepare_event(topology::tracked_data & tracked_data_){
    //*******************************************************************

    clock.start(" clusterizer: prepare event ","cumulative");

    m.message(" local_tracking: preparing event", mybhep::VERBOSE);

    parts.clear();
    clusters_.clear();

    order_cells();
    setup_cells();

    tracked_data_.set_cells(cells_);

    clock.stop(" clusterizer: prepare event ");


    return true;


  }


  //*******************************************************************
  void clusterizer::read_true_sequences(mybhep::event& event_ref){
    //*******************************************************************
    const vector<mybhep::particle*>& truep = event_ref.true_particles();
    m.message(" number of true particles is", truep.size(), mybhep::VVERBOSE);
    if (truep.size()!=0){
    
      for( size_t ipart=0; ipart<truep.size(); ipart++)
        {
        
          mybhep::particle& tp = *truep[ipart];
          const vector<mybhep::hit*>& thits = tp.hits("trk"); 
          topology::sequence trueseq;
          vector<topology::node> truenodes;
          for(size_t i=0; i<thits.size(); i++)
            {
              size_t index = get_true_hit_index(*thits[i], tp.primary());
              topology::node tn(*thits[i], index, SuperNemo, level, nsigma);
              truenodes.push_back(tn);
            }
          trueseq.set_nodes(truenodes);
          if( tp.find_property("charge"))
            trueseq.set_charge(topology::experimental_double(tp.charge(),0.));//particle charge
          else if( tp.name() == "e+")
            trueseq.set_charge(topology::experimental_double(1.,0.));
          else if( tp.name() == "e-")
            trueseq.set_charge(topology::experimental_double(-1.,0.));
          else
            trueseq.set_charge(topology::experimental_double(-1.,0.));

          if( tp.find_property("length"))
            trueseq.set_length(topology::experimental_double(mybhep::double_from_string(tp.fetch_property("length")), 0.));
        
          trueseq.set_vertex( topology::experimental_point( tp.vertex().x(), tp.vertex().y(), tp.vertex().z(), 0., 0., 0.),"true");
        
          size_t cindex = 0;
          const vector<mybhep::hit*>& chits = tp.hits("cal");
          if( !chits.empty() ){
            topology::calorimeter_hit ch = make_calo_hit(*chits[0], 0);
            cindex = get_calo_hit_index(ch);
            trueseq.set_decay_vertex( topology::experimental_point( tp.decay_vertex().x(), tp.decay_vertex().y(), tp.decay_vertex().z(), 0., 0., 0.),"calo", cindex);
          }
        
          trueseq.set_name( tp.name() );
        
          trueseq.set_momentum( topology::experimental_double(tp.p3().mag(), 0.));
        
          trueseq.set_primary( tp.primary());
        
          true_sequences_.push_back(trueseq);
        
        
        
        }
    
      if( level >= mybhep::VVERBOSE ){
        print_true_sequences();
      }
    
    }
  
    return;
  
  }

  //*******************************************************************
  void clusterizer::read_nemo_sequences(mybhep::event& event_ref){
    //*******************************************************************

    vector<mybhep::particle*> nemo_parts;

    event_ref.filter(mybhep::DIGI,"NEMO","1",nemo_parts);

    if (! nemo_parts.empty()){
      m.message(" number of nemo particles is", nemo_parts.size(), mybhep::VVERBOSE);
    
      for( size_t ipart=0; ipart<nemo_parts.size(); ipart++)
        {
        
          mybhep::particle& tp = *nemo_parts[ipart];
          const vector<mybhep::hit*>& thits = tp.hits("trk"); 
          topology::sequence nemoseq;
          vector<topology::node> nemonodes;
          for(size_t i=0; i<thits.size(); i++)
            {
              size_t index = get_nemo_hit_index(*thits[i], tp.primary());
              topology::node tn(*thits[i], index, SuperNemo, level, nsigma);
              nemonodes.push_back(tn);
            }
          nemoseq.set_nodes(nemonodes);
          if( tp.find_property("charge"))
            nemoseq.set_charge(topology::experimental_double(tp.charge(),0.));//particle charge

          if( tp.find_property("length"))
            nemoseq.set_length(topology::experimental_double(mybhep::double_from_string(tp.fetch_property("length")), 0.));

          nemoseq.set_vertex( topology::experimental_point( tp.vertex().x(), tp.vertex().y(), tp.vertex().z(), 0., 0., 0.),"nemo");
        

          size_t cindex = 0;
          const vector<mybhep::hit*>& chits = tp.hits("cal");
          if( !chits.empty() ){
            topology::calorimeter_hit ch = make_calo_hit(*chits[0], 0);
            cindex = get_calo_hit_index(ch);
            nemoseq.set_decay_vertex( topology::experimental_point( tp.decay_vertex().x(), tp.decay_vertex().y(), tp.decay_vertex().z(), 0., 0., 0.),"calo", cindex);
          }

          nemoseq.set_name( tp.name() );
        
          nemoseq.set_momentum( topology::experimental_double(tp.p3().mag(), 0.));

          nemoseq.set_primary( tp.primary());
        
          nemo_sequences_.push_back(nemoseq);
        
        
        
        }
    
      if( level >= mybhep::VVERBOSE ){
        print_nemo_sequences();
      }
    
    }
  
    return;
  
  }

  //*******************************************************************
  void clusterizer::print_cells(void)const{
    //*******************************************************************

    for(std::vector<topology::cell>::const_iterator icell=cells_.begin(); icell!=cells_.end();++icell){
      icell->dump();
    }

    return;
  }


  //*******************************************************************
  void clusterizer::print_calos(void)const{
    //*******************************************************************

    for(std::vector<topology::calorimeter_hit>::const_iterator icalo=calorimeter_hits_.begin(); icalo!=calorimeter_hits_.end();++icalo){
      icalo->dump();
    }

    return;
  }

  //*******************************************************************
  void clusterizer::print_true_sequences(void)const{
    //*******************************************************************

    for(std::vector<topology::sequence>::const_iterator iseq=true_sequences_.begin(); iseq != true_sequences_.end(); ++iseq){
      iseq->dump();
    }
  
    return;
  }


  //*******************************************************************
  void clusterizer::print_nemo_sequences(void)const{
    //*******************************************************************

    for(std::vector<topology::sequence>::const_iterator iseq=nemo_sequences_.begin(); iseq != nemo_sequences_.end(); ++iseq){
      iseq->dump();
    }
  
    return;
  }



  //*******************************************************************
  void clusterizer::print_clusters(void)const{
    //*******************************************************************

    for(std::vector<topology::cluster>::const_iterator icluster=clusters_.begin(); icluster != clusters_.end(); ++icluster){
      icluster->dump();
    }
  
    return;
  }



  //*******************************************************************
  void clusterizer::clusterize(topology::tracked_data & tracked_data_){
    //*******************************************************************

    bool devel = false;
    //devel = true;
    clock.start(" clusterizer: clusterize ","cumulative");

    m.message(" local_tracking: fill clusters ", mybhep::VERBOSE); 

    if( cells_.empty() ) return;

    if( !select_true_tracks(tracked_data_) ){
      m.message(" clusterizer: event is not selected at true level ", mybhep::NORMAL);
      tracked_data_.set_selected(false);
      return;
    }
  
    float side[2]; // loop on two sides of the foil
    side[0] =  1.;
    side[1] = -1.;

    bool fast[2]; // loop on fast and slow hits
    fast[0] = true;
    fast[1] = false;
  
    vector<unsigned int> flag;
    {
      unsigned int tmp = 0;
      flag.assign(cells_.size(), tmp);
    }

    /** 2012-03-26 FM : using a 'flags' map keyed with cell IDs would be saner than indexing 
     * the 'flag' vector with cell IDs because the last method forces cells' IDs to
     * range *EXACTLY* from 0 to N-1.
     */
    bool use_flag_map = false; // not used for now (was used for tracking memory leak during Channel port)
    std::map<int,unsigned int > flags;

    if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: Flag size =" << flag.size () << std::endl;
    // 2012-03-26 FM: remove C style
    //size_t* flag = (size_t*)m alloc(sizeof(size_t)*cells_.size());
    for(size_t ip=0; ip<2; ip++)  // loop on two sides of the foil
      {
        for(size_t iq=0; iq<2; iq++) // loop on fast and slow hits
          {
            if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: ip=" << ip << " iq=" << iq << std::endl;
            for(size_t i=0; i<cells_.size(); i++)
              {
                if (use_flag_map) 
                  {
                    flags[cells_[i].id()] = 0;
                  }
                else
                  {
                    flag.at(i) = 0;
                  }
              }
            
            for(vector<topology::cell>::const_iterator icell=cells_.begin(); icell!=cells_.end(); ++icell){
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: cell loop..." << std::endl;
              // pick a cell c that was never added
              const topology::cell & c = *icell;
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: pick a cell done..." << std::endl;
              if( (cell_side(c) * side[ip]) < 0) continue;
              if( c.fast() != fast[iq] ) continue;
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: ====> cell ID = "<<  c.id() << std::endl;
              if (use_flag_map) 
                {
                  if( flags[c.id()] == 1 ) continue;
                  flags[c.id()] = 1;
                }
              else
                {
                  if( flag.at (c.id()) == 1 ) continue;
                  flag[c.id()] = 1;
                }
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: flag done..." << std::endl;
              
              // cell c will form a new cluster, i.e. a new list of nodes
              topology::cluster cluster_connected_to_c;
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: cluster_connected create..." << std::endl;
              std::vector<topology::node> nodes_connected_to_c;
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: nodes_connected create..." << std::endl;
              m.message(" begin new cluster with cell ", c.id(), mybhep::VERBOSE);

              // let's get the list of all the cells that can be reached from c
              // without jumps
              std::vector<topology::cell> cells_connected_to_c;
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: cells_connected create... : " << cells_connected_to_c.size () << std::endl;
              cells_connected_to_c.push_back(c);
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: cell loop: cells_connected_to_c.size=" << cells_connected_to_c.size () << std::endl;

             for( size_t i=0; i<cells_connected_to_c.size(); i++){ // loop on connected cells
                if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: connected cell loop..." << std::endl;
                // take a connected cell (the first one is just c)
                topology::cell cconn = cells_connected_to_c[i];

                // the connected cell composes a new node
                topology::node newnode(cconn, level, nsigma);
                std::vector<topology::cell_couplet> cc;

                // get the list of cells near the connected cell
                std::vector<topology::cell> cells_near_iconn = get_near_cells(cconn);
            
                m.message(" cluster ", clusters_.size(), " starts with ", c.id(), " try to add cell ", cconn.id(), " with n of neighbours = ", cells_near_iconn.size(), mybhep::VERBOSE); 
                for(vector<topology::cell>::const_iterator icnc=cells_near_iconn.begin(); icnc!=cells_near_iconn.end(); ++icnc){
                  if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: couplet loop..." << std::endl;
 
                  topology::cell cnc = *icnc;

                  if( !is_good_couplet(& cconn, cnc, cells_near_iconn) ) continue;

                  topology::cell_couplet ccnc(cconn,cnc,level,nsigma);
                  cc.push_back(ccnc);
                  
                  m.message(" ... creating couplet ", cconn.id(), " -> ", cnc.id(), mybhep::VERBOSE); 
                  
                  if (use_flag_map) 
                    {
                      if( flags[cnc.id()] != 1 ) 
                        {
                          flags[cnc.id()] = 1 ;
                          cells_connected_to_c.push_back(cnc);
                        }
                    }
                  else
                    {                
                      if( flag[cnc.id()] != 1 ){
                        flag[cnc.id()] = 1 ;
                        cells_connected_to_c.push_back(cnc);
                      }
                    }
                  if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: end of couplet" << std::endl;
                }
                newnode.set_cc(cc);
                newnode.calculate_triplets(Ratio, QuadrantAngle, TangentPhi, TangentTheta);
                nodes_connected_to_c.push_back(newnode);

                m.message(" cluster started with ", c.id(), " has been given cell ", cconn.id(), " with ", cc.size(), " couplets ", mybhep::VERBOSE); 
                if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: end of connected cell loop..." << std::endl;

              }
              
              cluster_connected_to_c.set_nodes(nodes_connected_to_c);

              clusters_.push_back(cluster_connected_to_c);
              if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: end of cell loop." << std::endl;
            }

          }
      }

    // 2012-03-26 FM: remove C style
    //free(flag);
  
    setup_clusters();
 
    m.message(" there are ", clusters_.size(), " clusters of cells ", mybhep::VVERBOSE); 
  
    if( PrintMode )
      make_plots(tracked_data_);

    if( level >= mybhep::VVERBOSE ){
      print_clusters();
    }

    tracked_data_.set_cells(cells_);
    tracked_data_.set_clusters(clusters_);

    clock.stop(" clusterizer: clusterize ");

    if (devel) clog << "DEVEL: CAT::clusterizer::clusterize: Done." << std::endl;

    return;
  
  }

  //*************************************************************
  void clusterizer::make_plots(topology::tracked_data & tracked_data_){
    //*************************************************************
    /*
      if( PrintMode ){
      for(vector<topology::cluster>::iterator icluster = clusters_.begin(); icluster!=clusters_.end(); ++icluster){
      for(vector<topology::node>::iterator inode = (*icluster).nodes_.begin(); inode != (*icluster).nodes_.end(); ++inode){
      for(vector<topology::cell_triplet>::iterator iccc = (*inode).ccc_.begin(); iccc != (*inode).ccc_.end(); ++iccc){
      topology::cell_triplet ccc = *iccc;
      for(vector<double>::const_iterator ichi = ccc.chi2s().begin(); ichi != ccc.chi2s().end(); ++ichi){
      hman.fill("chi2_triplet", *ichi);
      }
      for(vector<double>::const_iterator iprob = ccc.probs().begin(); iprob != ccc.probs().end(); ++iprob){
      hman.fill("prob_triplet", *iprob);
      }
      }
      }
    
      }

      std::vector<topology::sequence> true_sequences = tracked_data_.get_true_sequences();
      for(std::vector<topology::sequence>::iterator iseq=true_sequences.begin(); iseq != true_sequences.end(); ++iseq){
      topology::node n;
      double phi;
      if( iseq->largest_kink_node(n, phi)){
      phi *= 180./M_PI;
      clog << " largest kink on true sequence " << iseq - true_sequences.begin() << " is " << phi << " on cell " << n.c().id() << endl;
      hman.fill("largest_true_kink", phi);
      if( phi > 20.){
      topology::experimental_vector dist(n.c().ep(), n.ep());
      hman.fill("largest_true_kink_position", dist.z().value(), dist.x().value());
      }
      }
      
      }
    
    
    
    
      }

    */  
    return;


  }

  //*************************************************************
  bool clusterizer::is_good_couplet(topology::cell * mainc, topology::cell candidatec, std::vector<topology::cell> nearmain){
    //*************************************************************

    // the couplet mainc -> candidatec is good only if
    // there is no other cell that is near to both and can form a triplet between them

    clock.start(" clusterizer: is good couplet ","cumulative");

    topology::cell a=*mainc;
    

    for(std::vector<topology::cell>::const_iterator icell=nearmain.begin(); icell != nearmain.end(); ++icell){

      topology::cell b=*icell;
      if( b.id() == candidatec.id()) continue;

      if(near_level(b, candidatec) == 0 ) continue;

      if(near_level(b, candidatec) < near_level(a, candidatec) || 
	 near_level(b, a) < near_level(a, candidatec) )
	continue;  // cannot match a->b or b->c if a->c is nearer

      //    if( icell->intersect(candidatec) || icell->intersect(mainc) ) continue;  
      // don't reject candidate based on a cell that intersects it
       
      m.message(" ... ... check if near node ", b.id(), " has triplet ", a.id(), " <-> ", candidatec.id(), mybhep::VERBOSE); 

      topology::cell_triplet ccc(a,b,candidatec, level, nsigma);
      ccc.calculate_joints(Ratio, QuadrantAngle, TangentPhi, TangentTheta);
      if(ccc.joints().size() > 0 ){
        m.message(" ... ... yes it does: so couplet ", a.id(), " and ", candidatec.id(), " is not good",  mybhep::VERBOSE); 
        clock.stop(" clusterizer: is good couplet ");
        return false;
      }

    }
  

    clock.stop(" clusterizer: is good couplet ");
    return true;

  }


  //*************************************************************
  void clusterizer::fill_fast_information( mybhep::event& evt ){
    //*************************************************************

    for(size_t i=0; i<evt.digi_particles().size(); i++){
      if( evt.digi_particles()[i]->find_property("SUNAMI") )
        fill_fast_information( evt.digi_particles()[i] );
    }


    return;

  }

  //*************************************************************
  void clusterizer::fill_fast_information( mybhep::particle* p ){
    //*************************************************************

    for(size_t i=0; i<p->hits("trk").size(); i++){
      fill_fast_information(p->hits("trk")[i]);
    }

    return;

  }

  //*************************************************************
  void clusterizer::fill_fast_information( mybhep::hit* h ){
    //*************************************************************

    double radius = mybhep::double_from_string(h->fetch_property("DIST"));
    double tdelay = mybhep::double_from_string(h->fetch_property("ATIME"));
    if( isnan(tdelay) || isinf(tdelay) )
      tdelay = -1.;
  
    if( radius != 0. && tdelay == 0. ) // fast hit
      h->add_property("FAST",mybhep::to_string(1));
    else if( radius == 0. && tdelay != 0. ){ // delayed hit
      h->add_property("SLOW",mybhep::to_string(1));
    }
    else
      {
        m.message("Problem: hit has radius", radius, "delay time", tdelay, mybhep::NORMAL); 
      }
  
    return;

  }

  //*************************************************************
  int clusterizer::cell_side( const topology::cell & c){
    //*************************************************************
  
    if( SuperNemo )
      {
        if( c.ep().z().value() > 0. )
          return 1;
      
        return -1;
      }
  
  
    if( c.ep().radius().value() > FoilRadius )
      return 1;
  
    return -1;

  }


  size_t clusterizer::near_level( const topology::cell & c1, const topology::cell & c2 ){

    // returns 0 for far-away cell 
    // 1 for diagonal cells
    // 2 for side-by-side cells

    // side-by-side connection: distance = 1
    // diagonal connection: distance = sqrt(2) = 1.41
    // skip 1 connection, side: distance = 2
    // skip 1 connection, tilt: distance = sqrt(5) = 2.24
    // skip 1 connection, diag: distance = 2 sqrt(2) = 2.83

    topology::experimental_double distance = topology::experimental_vector(c1.ep(),c2.ep()).hor().length();

    double limit_side;
    double limit_diagonal;
    if (SuperNemo && SuperNemoChannel)
      {
        // 2012-03-12 FM: add to conform the SuperNEMO geometry from 'sngeometry'
        limit_side = GG_CELL_pitch;
        limit_diagonal = sqrt(2.)*GG_CELL_pitch;
      }
    else
      {
        double factor = cos(M_PI/8.); // 0.923879532511287 // octogonal factor = 0.92 
        limit_side = factor*CellDistance;
        limit_diagonal = sqrt(2.)*factor*CellDistance; // new factor = 1.31 
      }
    double precision = 0.15*limit_side;

    if( level >= mybhep::VVERBOSE )
      clog << " (c " << c2.id() << " d " << distance.value() << " )";

    if( fabs(distance.value() - limit_side) < precision )
      return 2;
  
    if( fabs(distance.value() - limit_diagonal) < precision )
      return 1;
  
    return 0;

  
  }


  std::vector<topology::cell> clusterizer::get_near_cells(const topology::cell & c){

    clock.start(" clusterizer: get near cells ","cumulative");

    m.message(" filling list of cells near cell ", c.id(), " fast ", c.fast(), " side ", cell_side(c), mybhep::VVERBOSE);

    std::vector<topology::cell> cells;

    for(std::vector<topology::cell>::iterator kcell=cells_.begin(); kcell != cells_.end(); ++kcell){
      if( kcell->id() == c.id() ) continue;
     
      if( kcell->fast() != c.fast() ) continue;
     
      if( cell_side(*kcell) != cell_side(c) ) continue;
     
      size_t nl = near_level(c,*kcell);
     
      if( nl > 0 )
        {
          if( level >= mybhep::VVERBOSE ){
            clog << "*";
	  }

          topology::cell ck = *kcell;
          cells.push_back(ck);
        }
    }

    if( level >= mybhep::VVERBOSE )
      clog << " " << endl;

    clock.stop(" clusterizer: get near cells ");

    return cells;

  }


  //*************************************************************
  void clusterizer::setup_cells(){
    //*************************************************************
  
    for(vector<topology::cell>::iterator icell=cells_.begin(); icell!=cells_.end(); ++icell){
      icell->set_print_level(level);
      icell->set_nsigma(nsigma);
    }

    return;

  }



  //*************************************************************
  void clusterizer::setup_clusters(){
    //*************************************************************

    clock.start(" clusterizer: setup_clusters ","cumulative");

    // loop on clusters
    for(std::vector<topology::cluster>::iterator icl=clusters_.begin(); icl != clusters_.end(); ++icl){
      icl->set_print_level(level);
      icl->set_nsigma(nsigma);

      // loop on nodes
      for(std::vector<topology::node>::iterator inode=(*icl).nodes_.begin(); inode != (*icl).nodes_.end(); ++inode){
        inode->set_print_level(level);
        inode->set_nsigma(nsigma);

        for(std::vector<topology::cell_couplet>::iterator icc=(*inode).cc_.begin(); icc != (*inode).cc_.end(); ++icc){
          icc->set_print_level(level);
          icc->set_nsigma(nsigma);
        }

        for(std::vector<topology::cell_triplet>::iterator iccc=(*inode).ccc_.begin(); iccc != (*inode).ccc_.end(); ++iccc){
          iccc->set_print_level(level);
          iccc->set_nsigma(nsigma);
        }

      }
    
    }

    clock.stop(" clusterizer: setup_clusters ");

    return;
  }


  //*************************************************************
  int clusterizer::get_effective_layer(const mybhep::hit &hit){
    //************************************************************* 

    // calculates effective layer of a hit in calorimeter block on +-X wall
    // returns -8, -7, ..., -1, 0, 0, 1, ..., 7, 8
    // returns N if the center of calo block is within layers N-1 and N
    // for instance, if block center is between layers 3 and 4, returns 4

    vector<double> block_pos;

    //this deals with true hits
    if (hit.find_property("Ini_Ekin")){
      if(hit.find_property("BLK_Pos")){
        mybhep::vector_from_string(hit.fetch_property("BLK_Pos"), block_pos);
      }
    }
    //this deals with digi hits
    else if (hit.find_property("E")){
      string bp = hit.fetch_property("BLK_POS");
      mybhep::vector_from_string(bp, block_pos);
    }

    double pos = GG_CELL_pitch;
    bool found = false;
    int counter = 0;
  
    for(size_t i=0; i<planes_per_block.size(); i++){
      if( found )
        break;
    
      pos += gaps_Z[i];
    
      for(size_t j=0; j<(size_t)planes_per_block[i]; j++){
      
        if( pos > fabs(block_pos[2]) ){
          found = true;
          break;
        }
        pos += GG_CELL_pitch ;
        counter ++;
      
      }
    }
  
    int layer = (int)counter;
    if( block_pos[2] < 0. )
      layer *= -1;
  
    return layer;
  
  }

  //*******************************************************************
  topology::calorimeter_hit clusterizer::make_calo_hit(const mybhep::hit &ahit, size_t _id){
    //*******************************************************************

    vector<double> block_pos;
    double en, time;
    //this deals with true hits
    if (ahit.find_property("Ini_Ekin")){
      if(ahit.find_property("BLK_Pos")){
        mybhep::vector_from_string(ahit.fetch_property("BLK_Pos"), block_pos);
      }
      en = mybhep::double_from_string(ahit.fetch_property("E_dep"));
      time = mybhep::double_from_string(ahit.fetch_property("TOF"));
    }
    //this deals with digi hits
    else if (ahit.find_property("E")){
      string bp = ahit.fetch_property("BLK_POS");
      mybhep::vector_from_string(bp, block_pos);
      en = mybhep::double_from_string(ahit.fetch_property("E"));
      time = mybhep::double_from_string(ahit.fetch_property("TIME"));
    }

    topology::experimental_point center(block_pos[0], block_pos[1], block_pos[2], 0., 0., 0.);

    string block_type = ahit.fetch_property("BLK");
    string plane;

    topology::experimental_vector norm(0.,0.,0.,0.,0.,0.);

    double layer=0;

    double local_calo_X = calo_X;

    if( SuperNemo )
      {
        double x_offset = 110.;
        plane =  block_type.substr(0,11);
      
        if (plane=="CALO_WRAP+X"){
          norm.set_x(topology::experimental_double(-1.,0.));
          layer = (double)get_effective_layer(ahit);
          local_calo_X -= x_offset;
        }
        else  if (plane=="CALO_WRAP-X"){
          norm.set_x(topology::experimental_double(1.,0.));
          layer = (double)get_effective_layer(ahit);
          local_calo_X -= x_offset;
        }
        else  if (plane=="CALO_WRAP-Z"){
          norm.set_z(topology::experimental_double(1.,0.));
        }
        else  if (plane=="CALO_WRAP+Z"){
          norm.set_z(topology::experimental_double(-1.,0.));
        }
        else{
          m.message("problem: calo block not recognized",plane,mybhep::MUTE); 
          exit(1);
        }
      }
    else
      {
        int block,planeid,id,idd;
        sscanf(block_type.c_str(),"%d_%d_%d_%d",&block,&planeid,&id,&idd);

      

        if (planeid==0 || planeid == 1){

          topology::experimental_point origin(0.,0.,0.,0.,0.,0.);
          norm = (topology::experimental_vector(origin, center)).hor().unit();
      
          if (planeid==0){
            //  plane = "inner"; //inner
            layer = -(lastlayer - 1);
          }
          else  if (planeid==1){
            plane = "outer"; //outer
            //  *caloparam = OuterRadius;
            layer = (lastlayer - 1);
            norm *= -1.;
          }

          topology::experimental_vector mysizes(local_calo_X, calo_Y, calo_Z,
                                                0., 0., 0.);
          double offset = fabs((norm * mysizes).value())/2.;
          center = (topology::experimental_vector(center) - norm * offset).point_from_vector();
        }
        else if( planeid==2 || planeid ==3){  // bottom and top, for Nemo3
          if(planeid==2){
            norm.set_y(topology::experimental_double(1.,0.));
            //    center = topology::experimental_point(0,-ysize/2.,0.,0.,0.,0.);
          }
          else{
            norm.set_y(topology::experimental_double(-1.,0.));
            //    center = topology::experimental_point(0,ysize/2.,0.,0.,0.,0.);
          }
          if( id==0 ) layer=-5.5;
          else if(id==1) layer=-3.5;
          else if(id==2) layer=3.5;
          else if(id==3) layer=5.5;
          else{
            m.message("CAL wall not recognized",block_type,mybhep::NORMAL); 
            exit(1);
          }
        }
        else{
          m.message("CAL wall not recognized",block_type,mybhep::NORMAL); 
          exit(1);
        }
      }

    topology::experimental_vector sizes(local_calo_X, calo_Y, calo_Z,
                                        0., 0., 0.);

    double time_resol = 0.250; // ns
    double e_resol = 0.240; // MeV
    topology::experimental_double e(en, e_resol);
    topology::experimental_double t(time, time_resol);
    topology::plane pl(center, sizes, norm, level, nsigma);
    topology::calorimeter_hit ch(pl, e, t, _id, layer, level, nsigma);

    return ch;
  }

  //*************************************************************
  void clusterizer::order_cells(){
    //*************************************************************
  
    clock.start(" clusterizer: order cells ","cumulative");
   
    if( level >= mybhep::VVERBOSE ){
      clog << " printing cells " << cells_.size() << endl;
      print_cells();
      clog << " sorting cells " << endl;
    }

    //  std::sort( cells_.begin(), cells_.end(), topology::cell::compare );
    std::sort( cells_.begin(), cells_.end());

    clock.stop(" clusterizer: order cells ");

    return;

  }

  //*************************************************************
  bool clusterizer::select_true_tracks(topology::tracked_data & tracked_data_){
    //*************************************************************

    return true;

    std::vector<topology::sequence> true_sequences = tracked_data_.get_true_sequences();

    m.message(" selecting events based on true tracks ", mybhep::VVERBOSE);

    size_t counter = 0;
    topology::sequence sa;
    topology::sequence sb;

    size_t n_tracks_ = 1;
    double emin = 0.2;

    for(std::vector<topology::sequence>::iterator iseq=true_sequences.begin(); iseq != true_sequences.end(); ++iseq){
      if( !iseq->primary() ) continue;
      if( counter == 0 )
        sa = *iseq;
      if( counter == 1 )
        sb = *iseq;
      counter ++;
    }


    if( counter != n_tracks_ ){
      m.message(" reject: n primary tracks = ", counter, mybhep::NORMAL);
      return false;
    }
    m.message(" n primary tracks = ", counter, mybhep::VVERBOSE);

    if( !sa.one_side() ){
      m.message(" reject: 1st track crosses the foil ", mybhep::NORMAL);
      return false;
    }

    if( n_tracks_ == 2 ){
      if( !sb.one_side() ){
        m.message(" reject: 2nd track crosses the foil ", mybhep::NORMAL);
        return false;
      }
    }

    if( !sa.has_decay_vertex() ){
      m.message(" reject: 1st track has no calo ", mybhep::NORMAL);
      return false;
    }


    if( n_tracks_ == 2 ){
      if( !sb.has_decay_vertex() ){
        m.message(" reject: 2nd track has no calo ", mybhep::NORMAL);
        return false;
      }
    }

    if( n_tracks_ == 2 ){
      if( sa.calo_id() == sb.calo_id() ){
        m.message(" reject: same calo ", sa.calo_id(), mybhep::NORMAL);
        return false;
      }
    }

    std::vector<topology::calorimeter_hit> calos = tracked_data_.get_calos();

    topology::calorimeter_hit caloA = calos[sa.calo_id()];


    if( caloA.e().value() < emin ){
      m.message(" reject: 1st calo has energy ", caloA.e().value(), mybhep::NORMAL);
      return false;

    }

    if( n_tracks_ == 2 ){
      topology::calorimeter_hit caloB = calos[sb.calo_id()];
      if( caloB.e().value() < emin ){
        m.message(" reject: 2nd calo has energy ", caloA.e().value(), mybhep::NORMAL);
        return false;
      }
    }

    /*
      if( sa.nodes().size() > 15 ){
      m.message(" reject: 1st tracks has nhits = ", sa.nodes().size(), mybhep::NORMAL);
      return false;
      }

      if( sb.nodes().size() > 15 ){
      m.message(" reject: 2nd tracks has nhits = ", sb.nodes().size(), mybhep::NORMAL);
      return false;
      }
    */

    size_t calo_counter = 0;
    size_t calo_counter_thr = 0;

    for(std::vector<topology::calorimeter_hit>::iterator iseq=calos.begin(); iseq != calos.end(); ++iseq){
      calo_counter ++;
      if( iseq->e().value() <= emin ) continue;
      calo_counter_thr ++;
    }

    if( calo_counter != n_tracks_ ){
      m.message(" reject: n calos is ", calo_counter, mybhep::NORMAL);
      return false;
    }

    if( calo_counter_thr != n_tracks_ ){
      m.message(" reject: n calos with E > ", emin, " is ", calo_counter_thr, mybhep::NORMAL);
      return false;
    }

#if 0 // cut on kinks

    size_t all_kinks_counter = 0;
    for(std::vector<topology::sequence>::iterator iseq=true_sequences.begin(); iseq != true_sequences.end(); ++iseq){
      if( !iseq->primary() ) continue;
      topology::node n;
      double phi;
      size_t kinks_counter = 0;
      int layer = -100;
      if( iseq->largest_kink_node(n, phi)){
        phi *= 180./M_PI;
        if( phi > 20.){
          kinks_counter ++;
          all_kinks_counter ++;
          layer = n.c().layer();
        }
      }

      if( kinks_counter > 1 ){
        m.message(" reject: sequence ", iseq - true_sequences_.begin(), " has ", kinks_counter, " kinks ", mybhep::NORMAL);
        return false;
      }
    
      if( kinks_counter > 0 && (layer == 0 || layer == 8 || layer == -8) ){
        m.message(" reject: sequence ", iseq - true_sequences_.begin(), " has kink on plane ", layer, mybhep::NORMAL);
        return false;
      }
    
    }

    if( all_kinks_counter == 0 ){
      m.message(" reject: no kinks in the event ", mybhep::NORMAL);
      return false;
    }
    

#endif

    return true;

  }
}

