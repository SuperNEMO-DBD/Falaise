#include <CATAlgorithm/Sultan.h>
#include <mybhep/system_of_units.h>
#include <sys/time.h>
#include <limits>
#include <cmath>
#include <map>

#include <TApplication.h>
#include <TROOT.h>
#include <TChain.h>
#include <TH2.h>
#include <TH1.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TMath.h>
#include <TBox.h>
#include <TMarker.h>

namespace CAT {
  using namespace mybhep;
  using namespace std;
  using namespace topology;

  //************************************************************
  // Default constructor :
  Sultan::Sultan(void){
    //*************************************************************
    _set_defaults ();
    return;
  }

  //*************************************************************
  Sultan::~Sultan() {
    //*************************************************************
    std::clog << "DEVEL: CATAlgorithm::Sultan::~Sultan: Done." << std::endl;
  }

  //*************************************************************
  void Sultan::_set_defaults ()
  //*************************************************************
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

    probmin = std::numeric_limits<double>::quiet_NaN ();
    nofflayers = 0;
    first_event_number = 0;
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
  Sultan::Sultan(const mybhep::gstore &st){
    //*************************************************************

    _set_defaults ();
    set_level (st.fetch_sstore("VERBOSITY"));


    //-- read param --//

    pmax = st.fetch_dstore("pmax")*mybhep::MeV;
    MaxTime = st.fetch_dstore("MaxTime");
    PrintMode = st.fetch_istore("PrintMode");

    if (st.find_sstore("histo_file"))
      hfile=st.fetch_sstore("histo_file");
    else
      hfile="SultansHistogram.root";

    if (st.find_dstore("probmin"))
      probmin=st.fetch_dstore("probmin");
    else
      probmin=1.e-200;

    /*
      if( PrintMode )
      hman =  NHistoManager2(st);
    */

  }

  //*************************************************************
  bool Sultan::_initialize(){
    //*************************************************************

    if (doDriftWires)
      {
        GenerateWires();
      }

    detector_.set_cell_size(CellDistance);

    return true;
  }

  //*************************************************************
  bool Sultan::initialize(const mybhep::sstore &store, const mybhep::gstore gs , mybhep::EventManager2 *eman) {
    //*************************************************************

    m.message("\n Beginning algorithm Sultan \n",mybhep::VERBOSE); fflush(stdout);

    clock.start(" Sultan: initialize ");

    //----------- read dst param -------------//

    readDstProper(store, eman);

    //------- end of read pram -----------//

    _initialize();

    clock.stop(" Sultan: initialize ");

    return true;
  }

  //*************************************************************
  bool Sultan::initialize( ) {
    //*************************************************************

    m.message("CATAlgorithm::Sultan::initialize: Entering...",mybhep::NORMAL);

    m.message("\n Beginning algorithm Sultan \n",mybhep::VERBOSE);

    //----------- read dst param -------------//

    readDstProper();

    //------- end of read pram -----------//

    _initialize();

    m.message("CATAlgorithm::Sultan::initialize: Done.",mybhep::NORMAL);

    return true;
  }



  //*************************************************************
  bool Sultan::finalize() {
    //*************************************************************

    clock.start(" Sultan: finalize ");

    m.message("\n Ending algorithm Sultan \n ",mybhep::NORMAL);

    m.message("Initial events: ", InitialEvents, mybhep::NORMAL);
    m.message("Skipped events: ", SkippedEvents, "(", 100.*SkippedEvents/InitialEvents, "\%)", mybhep::NORMAL);

    clock.stop(" Sultan: finalize ");

    clock.dump();

    _set_defaults ();
    return true;
  }

  //*************************************************************
  void Sultan::readDstProper(const mybhep::sstore & global, mybhep::EventManager2 *eman) {
    //*************************************************************

    clock.start(" Sultan: read dst properties ");

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

        FoilRadius = 0.;
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

    m.message("",mybhep::NORMAL);

    m.message("pmax",pmax,"MeV",mybhep::NORMAL);
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
        std::string pname;

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

    clock.stop(" Sultan: read dst properties ");

    return;
  }


  //*************************************************************
  void Sultan::readDstProper() {
    //*************************************************************
    m.message("CATAlgorithm::Sultan::readDstProper: Entering...",mybhep::NORMAL);

    if (_MaxBlockSize <= 0)
      {
        _MaxBlockSize = 1;
        m.message("CATAlgorithm::Sultan::readDstProper: no bar design, MODULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
      }

    compute_lastlayer();

    m.message("",mybhep::NORMAL);
    m.message("probmin", probmin, mybhep::NORMAL);
    m.message("first event number", first_event_number, mybhep::NORMAL);

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

    m.message("CATAlgorithm::Sultan::readDstProper: Done.",mybhep::NORMAL);

    return;
  }


  //*************************************************************
  double Sultan::long_resolution(double Z, double d[3])const{
    //*************************************************************

    double xp = abs(Z*2./len);
    double kx = k0 + k1*xp + k2*xp*xp + k3*xp*xp*xp;
    double thx = th0 + th1*xp + th2*xp*xp + th3*xp*xp*xp;
    double tgth2=(d[0]*d[0] + d[2]*d[2])/(d[1]*d[1]);
    double sigma=sqrt(kx*sigma0*sigma0*(1-xp*xp)+thx*thx/tgth2);

    return sigma;
  }


  //*************************************************************
  double Sultan::long_resolution_1cthd(double Zdist)const{
    //*************************************************************
    double L = Zdist;
    double sigma = l0 + l1*L;

    return sigma;
  }


  //*************************************************************
  double Sultan::GetYError( double y, float tup, float tdown, double direction[3]){
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
  void Sultan::FillYPositions( mybhep::event& evt ){
    //*************************************************************

    for(size_t i=0; i<evt.digi_particles().size(); i++){
      if( evt.digi_particles()[i]->find_property("SUNAMI") )
        FillYPositions( evt.digi_particles()[i] );
    }

    return;

  }

  //*************************************************************
  void Sultan::FillYPositions( mybhep::particle* p ){
    //*************************************************************

    for(size_t i=0; i<p->hits("trk").size(); i++){
      FillYPosition(p->hits("trk")[i]);
    }

    return;

  }

  //*************************************************************
  void Sultan::FillYPosition( mybhep::hit* h ){
    //*************************************************************

    double erry = 0.;

    if( SuperNemo ){
      float tup, tdown, tdelay;

      std::string ttime = h->fetch_property("TTIME");
      std::string btime = h->fetch_property("BTIME");
      std::string atime = h->fetch_property("ATIME");

      tup = mybhep::float_from_string(ttime);
      tdown = mybhep::float_from_string(btime);
      tdelay = mybhep::float_from_string(atime);

      if( isnan(tup) || isinf(tup) )
        tup = -1.;
      if( isnan(tdown) || isinf(tdown) )
        tdown = -1.;
      if( isnan(tdelay) || isinf(tdelay) )
        tdelay = -1.;


      std::vector<float> cellpos;
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
  void Sultan::FillTrueVertexes( mybhep::event& evt ){
    //*************************************************************

    for(size_t i=0; i<evt.true_particles().size(); i++){
      FillTrueVertexes( evt.true_particles()[i] );
    }

    return;

  }

  //*************************************************************
  void Sultan::FillTrueVertexes( mybhep::particle* tp ){
    //*************************************************************

    if( tp->find_property("foil_vertex") )
      tp->change_property("foil_vertex", "1");
    else
      tp->add_property("foil_vertex", "1");

    return;

  }


  //*************************************************************
  void Sultan::GenerateWires( void ){
    //*************************************************************
    m.message("CATAlgorithm::Sultan::GenerateWires: Entering...",mybhep::NORMAL);

    //clock.start(" Sultan: generate wires ");

    DriftWires.clear();

    if( SuperNemo )
      {
        m.message("CATAlgorithm::Sultan::GenerateWires: SuperNemo geometry...",mybhep::NORMAL);

        // TRACKING GG BLOCKS
        double theta = M_PI/8.;
        m.message("CATAlgorithm::Sultan::GenerateWires: rad=",
                  rad,
                  mybhep::NORMAL);
        if (isnan (GG_CELL_pitch))
          {
            GG_CELL_pitch = rad*cos(theta);
          }
        m.message("CATAlgorithm::Sultan::GenerateWires: GG_CELL_pitch=",
                  GG_CELL_pitch,
                  mybhep::NORMAL);
        m.message("CATAlgorithm::Sultan::GenerateWires: GG_GRND_diam=",
                  GG_GRND_diam,
                  mybhep::NORMAL);
        m.message("CATAlgorithm::Sultan::GenerateWires: CHAMBER_X=",
                  CHAMBER_X,
                  mybhep::NORMAL);
        if (num_cells_per_plane <= 0)
          {
            num_cells_per_plane=(int)((CHAMBER_X-GG_GRND_diam)/GG_CELL_pitch);
          }
        m.message("CATAlgorithm::Sultan::GenerateWires: num_cells_per_plane=",
                  num_cells_per_plane,
                  mybhep::NORMAL);
        GG_BLOCK_X = num_cells_per_plane*GG_CELL_pitch+GG_GRND_diam;
        m.message("CATAlgorithm::Sultan::GenerateWires: GG_BLOCK_X=",
                  GG_BLOCK_X,
                  mybhep::NORMAL);

        std::vector<double> GG_BLOCK_thick;
        std::vector<double> GG_BLOCK_posZ;
        GG_BLOCK_thick.assign (num_blocks, std::numeric_limits<double>::quiet_NaN ());
        GG_BLOCK_posZ.assign (num_blocks, std::numeric_limits<double>::quiet_NaN ());
        double distance = SOURCE_thick/2.;
        m.message("CATAlgorithm::Sultan::GenerateWires: SOURCE_thick=",
                  SOURCE_thick,
                  mybhep::NORMAL);
        //Calculating thickness and positions of blocks
        for(int i=0; i<num_blocks; i++){
          GG_BLOCK_thick[i]=(planes_per_block[i]*GG_CELL_pitch+GG_GRND_diam);
          GG_BLOCK_posZ[i] = distance + gaps_Z[i] + GG_BLOCK_thick[i]/2.;
          distance = GG_BLOCK_posZ[i] + GG_BLOCK_thick[i]/2.;
        }
        m.message("CATAlgorithm::Sultan::GenerateWires: DEVEL: **** STEP 1",mybhep::NORMAL);

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

		  CAT::POINT point;
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

		CAT::POINT point;
                point.x = LayerRadius[i]*cos(ph);
                point.z = LayerRadius[i]*sin(ph);

                DriftWires.push_back( point );
              }
          }
      }


    //clock.stop(" Sultan: generate wires ");
    m.message("CATAlgorithm::Sultan::GenerateWires: Done.",mybhep::NORMAL);

    return;

  }


  //*******************************************************************
  size_t Sultan::get_true_hit_index(mybhep::hit& hit, bool print){
    //*******************************************************************

    topology::node tn(hit, 0, SuperNemo, level, probmin);

    for(std::vector<topology::Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic){
      if( ic->same_cell(tn.c()) )
        return ic->id();
    }

    if( print )
      m.message(" warning: can't find corresponding reco hit for true hit (", tn.c().ep().x().value(), ", ", tn.c().ep().y().value(), ", ", tn.c().ep().z().value(), ") layer", tn.c().layer(), mybhep::VVERBOSE);

    return 0;

  }

  //*******************************************************************
  size_t Sultan::get_nemo_hit_index(mybhep::hit& hit, bool print){
    //*******************************************************************

    topology::node tn(hit, 0, SuperNemo, level, probmin);

    for(std::vector<topology::Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic){
      if( ic->same_cell(tn.c()) )
        return ic->id();
    }

    if( print )
      m.message(" warning: can't find corresponding reco hit for nemo hit (", tn.c().ep().x().value(), ", ", tn.c().ep().y().value(), ", ", tn.c().ep().z().value(), ") layer", tn.c().layer(), mybhep::VVERBOSE);

    return 0;

  }

  //*******************************************************************
  size_t Sultan::get_calo_hit_index(const topology::calorimeter_hit & c){
    //*******************************************************************

    for(std::vector<topology::calorimeter_hit>::iterator ic=calorimeter_hits_.begin(); ic!=calorimeter_hits_.end(); ++ic){
      if( ic->same_calo(c) )
        return ic->id();
    }

    m.message(" warning: can't find corresponding calo hit for nemo calo hit (", c.pl().center().x().value(), ", ", c.pl().center().y().value(), ", ", c.pl().center().z().value(), ") layer", c.layer(), mybhep::VVERBOSE);

    return 0;

  }


  //*******************************************************************
  bool Sultan::read_event(mybhep::event& event_ref, topology::Tracked_data & tracked_data_){
    //*******************************************************************

    clock.start(" Sultan: read event ","cumulative");

    m.message(" sultan: reading event", mybhep::VERBOSE);

    cells_.clear();
    clusters_.clear();
    parts.clear();
    calorimeter_hits_.clear();
    true_sequences_.clear();
    nemo_sequences_.clear();

    bool bhep_input = true;

    if( bhep_input ){

      // read digi particles
      std::vector<mybhep::particle*> digi_parts;

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

      const std::vector<mybhep::hit*>& hits = parts[0]->hits("trk");

      for (size_t ihit=0; ihit<hits.size();ihit++){
        topology::Cell c(*hits[ihit],ihit, SuperNemo, level, probmin);
        cells_.push_back(c);
      }

      clock.start(" Sultan: make calo hit ","cumulative");
      const std::vector<mybhep::hit*>& chits = parts[0]->hits("cal");
      for (size_t ihit=0; ihit<chits.size();ihit++){
        topology::calorimeter_hit ch = make_calo_hit(*chits[ihit], ihit);
        calorimeter_hits_.push_back(ch);
      }
      clock.stop(" Sultan: make calo hit ");

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

    clock.stop(" Sultan: read event ");


    return true;


  }


  //*******************************************************************
  bool Sultan::prepare_event(topology::Tracked_data & tracked_data_){
    //*******************************************************************

    clock.start(" Sultan: prepare event ","cumulative");

    event_number ++;

    tracked_data_.reset();
    clusters_.clear();
    sequences_.clear();
    parts.clear();

    if( event_number < first_event_number ){
      m.message(" sultan: skip event", event_number, " first event is ", first_event_number,  mybhep::VERBOSE);
      return false;
    }

    m.message(" sultan: preparing event", event_number, " with ", cells_.size(), " cells ", mybhep::VERBOSE);

    //order_cells();
    setup_cells();

    tracked_data_.set_cells(cells_);
    detector_.set_cells(cells_);

    clock.stop(" Sultan: prepare event ");


    return true;


  }


  //*******************************************************************
  void Sultan::read_true_sequences(mybhep::event& event_ref){
    //*******************************************************************
    const std::vector<mybhep::particle*>& truep = event_ref.true_particles();
    m.message(" number of true particles is", truep.size(), mybhep::VVERBOSE);
    if (truep.size()!=0){

      for( size_t ipart=0; ipart<truep.size(); ipart++)
        {

          mybhep::particle& tp = *truep[ipart];
          const std::vector<mybhep::hit*>& thits = tp.hits("trk");
          topology::sequence trueseq;
          std::vector<topology::node> truenodes;
          for(size_t i=0; i<thits.size(); i++)
            {
              size_t index = get_true_hit_index(*thits[i], tp.primary());
              topology::node tn(*thits[i], index, SuperNemo, level, probmin);
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
            trueseq.set_helix_length(topology::experimental_double(mybhep::double_from_string(tp.fetch_property("length")), 0.));

          trueseq.set_helix_vertex( topology::experimental_point( tp.vertex().x(), tp.vertex().y(), tp.vertex().z(), 0., 0., 0.),"true");

          size_t cindex = 0;
          const std::vector<mybhep::hit*>& chits = tp.hits("cal");
          if( !chits.empty() ){
            topology::calorimeter_hit ch = make_calo_hit(*chits[0], 0);
            cindex = get_calo_hit_index(ch);
            trueseq.set_decay_helix_vertex( topology::experimental_point( tp.decay_vertex().x(), tp.decay_vertex().y(), tp.decay_vertex().z(), 0., 0., 0.),"calo", cindex);
          }

          trueseq.set_name( tp.name() );

          trueseq.set_momentum( topology::experimental_vector(tp.p3().x(), tp.p3().y(), tp.p3().z(), 0., 0., 0.));

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
  void Sultan::read_nemo_sequences(mybhep::event& event_ref){
    //*******************************************************************

    std::vector<mybhep::particle*> nemo_parts;

    event_ref.filter(mybhep::DIGI,"NEMO","1",nemo_parts);

    if (! nemo_parts.empty()){
      m.message(" number of nemo particles is", nemo_parts.size(), mybhep::VVERBOSE);

      for( size_t ipart=0; ipart<nemo_parts.size(); ipart++)
        {

          mybhep::particle& tp = *nemo_parts[ipart];
          const std::vector<mybhep::hit*>& thits = tp.hits("trk");
          topology::sequence nemoseq;
          std::vector<topology::node> nemonodes;
          for(size_t i=0; i<thits.size(); i++)
            {
              size_t index = get_nemo_hit_index(*thits[i], tp.primary());
              topology::node tn(*thits[i], index, SuperNemo, level, probmin);
              nemonodes.push_back(tn);
            }
          nemoseq.set_nodes(nemonodes);
          if( tp.find_property("charge"))
            nemoseq.set_charge(topology::experimental_double(tp.charge(),0.));//particle charge

          if( tp.find_property("length"))
            nemoseq.set_helix_length(topology::experimental_double(mybhep::double_from_string(tp.fetch_property("length")), 0.));

          nemoseq.set_helix_vertex( topology::experimental_point( tp.vertex().x(), tp.vertex().y(), tp.vertex().z(), 0., 0., 0.),"nemo");


          size_t cindex = 0;
          const std::vector<mybhep::hit*>& chits = tp.hits("cal");
          if( !chits.empty() ){
            topology::calorimeter_hit ch = make_calo_hit(*chits[0], 0);
            cindex = get_calo_hit_index(ch);
            nemoseq.set_decay_helix_vertex( topology::experimental_point( tp.decay_vertex().x(), tp.decay_vertex().y(), tp.decay_vertex().z(), 0., 0., 0.),"calo", cindex);
          }

          nemoseq.set_name( tp.name() );

          nemoseq.set_momentum( topology::experimental_vector(tp.p3().x(), tp.p3().y(), tp.p3().z(), 0., 0., 0.));

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
  void Sultan::print_cells(void)const{
    //*******************************************************************

    for(std::vector<topology::Cell>::const_iterator icell=cells_.begin(); icell!=cells_.end();++icell){
      icell->dump();
    }

    return;
  }


  //*******************************************************************
  void Sultan::print_calos(void)const{
    //*******************************************************************

    for(std::vector<topology::calorimeter_hit>::const_iterator icalo=calorimeter_hits_.begin(); icalo!=calorimeter_hits_.end();++icalo){
      icalo->dump();
    }

    return;
  }

  //*******************************************************************
  void Sultan::print_true_sequences(void)const{
    //*******************************************************************

    for(std::vector<topology::sequence>::const_iterator iseq=true_sequences_.begin(); iseq != true_sequences_.end(); ++iseq){
      iseq->dump();
    }

    return;
  }


  //*******************************************************************
  void Sultan::print_nemo_sequences(void)const{
    //*******************************************************************

    for(std::vector<topology::sequence>::const_iterator iseq=nemo_sequences_.begin(); iseq != nemo_sequences_.end(); ++iseq){
      iseq->dump();
    }

    return;
  }




  //*******************************************************************
  void Sultan::reconstruct(topology::Tracked_data & tracked_data_){
    //*******************************************************************

    if( event_number < first_event_number ) return;

    clock.start(" Sultan: reconstruct ","cumulative");

    clusterize();

    for( std::vector< std::vector< topology::Cell > >::iterator iclu=clusters_.begin(); iclu != clusters_.end(); ++iclu)
      reconstruct_cluster(*iclu);

    tracked_data_.set_sequences(sequences_);

    set_unclustered_cells(tracked_data_);

    m.message(" sultan: reconstructed ", tracked_data_.get_sequences().size(), "tracks, leaving ", tracked_data_.get_unclustered_cells().size(), " unclustered hits", mybhep::VERBOSE);

    clock.stop(" Sultan: reconstruct ");

  }

  //*******************************************************************
  void Sultan::clusterize(void){
    //*******************************************************************
    // build clusters of cells, such that cells within each cluster:
    // - all lie on the same side wrt the foil
    // - all have the same time character (fast or slow)
    // - are near some other cell in the cluster

    clock.start(" Sultan: clusterize ","cumulative");

    m.message(" sultan: fill clusters ", mybhep::VERBOSE);

    if( cells_.empty() ) return;

    std::map<int,bool > used;
    for(size_t i=0; i<cells_.size(); i++)
      used[cells_[i].id()] = false;

    int the_cell_side;
    bool the_cell_fast;
    bool near_existing_cluster;

    for(std::vector<topology::Cell>::const_iterator icell=cells_.begin(); icell!=cells_.end(); ++icell){
      // pick a cell c that was never added
      const topology::Cell & c = *icell;
      if( used[c.id()] ) continue; // skip cells already used
      used[c.id()] = true;
      the_cell_side = cell_side(c);
      the_cell_fast = c.fast();

      m.message(" build cluster for cell ", c.id(), mybhep::VVERBOSE);

      near_existing_cluster = false;
      // check if cell c is near some already existing clusters
      for( std::vector< std::vector< topology::Cell > >::iterator iclu=clusters_.begin(); iclu != clusters_.end(); ++iclu){
	if( near_existing_cluster ) break;
	if( iclu->size() == 0 ){
	  m.message( " problem: cluster contains no cells ", mybhep::NORMAL);
	  continue;
	}
	topology::Cell fc = iclu->front();
	if( cell_side(fc) != the_cell_side ) continue; // cell must be on same side as cluster
	if( fc.fast() != the_cell_fast ) continue; // cell must be as fast as cluster

	for( std::vector< topology::Cell >::const_iterator ccell=iclu->begin(); ccell != iclu->end(); ++ccell ){
	  size_t nl = near_level(c, *ccell);
	  if( nl > 0 ){
	    m.message(" cell ", c.id(), " is near cell ", ccell->id(), " with level ", nl, mybhep::VVERBOSE);
	    iclu->push_back(c);
	    near_existing_cluster = true;
	    break;
	  }
	} // finish loop on cells in cluster
	if( near_existing_cluster ) break;
      } // finish loop on clusters

      if( !near_existing_cluster ){
	m.message(" cell ", c.id(), " will form a new cluster ", mybhep::VVERBOSE);
	std::vector<topology::Cell> cluster;
	cluster.push_back(c);
	clusters_.push_back(cluster);
      }

    } // finish loop on cells

    m.message(" there are ", clusters_.size(), " clusters ", mybhep::VERBOSE);

    clock.stop(" Sultan: clusterize ");


    return;

  }


  //*******************************************************************
  void Sultan::reconstruct_cluster(std::vector< topology::Cell > cluster){
    //*******************************************************************

    clock.start(" Sultan: reconstruct_cluster ","cumulative");

    m.message(" reconstruct cluster with ", cluster.size(), " cells ", mybhep::VERBOSE);

    std::vector<Circle> * hs_reco_rough = new std::vector<Circle>();
    std::vector<Circle> * hs_reco_precise = new std::vector<Circle>();
    Circle h_reco_rough;
    Circle h_reco_precise;
    std::vector<Cell > cells_to_reconstruct = cluster;
    topology::Sequence seq;
    std::vector<Cell> track_cells;
    std::vector<experimental_double> * phis = new std::vector<experimental_double>();
    std::vector<experimental_double> * zs = new std::vector<experimental_double>();

    // need 3 cells to make a circle
    while( cells_to_reconstruct.size() > 3 ){
      phis->clear();
      zs->clear();

      detector_.draw_surfaces_rough(&h_reco_rough, false, cells_to_reconstruct,event_number);

      detector_.assign_reco_points_based_on_circle(cells_to_reconstruct, h_reco_rough, sequences_.size());
      //detector_.fill_residual(&h_residual_rough);

      detector_.draw_surfaces_precise(h_reco_rough, &h_reco_precise, false, cells_to_reconstruct, event_number);
      
      detector_.assign_reco_points_based_on_circle(cells_to_reconstruct, h_reco_precise, sequences_.size());
      //detector_.fill_residual(&h_residual_precise);
      //detector_.fill_residual_circle(&h_residual_x0, &h_residual_y0, &h_residual_r, &h_pull_x0, &h_pull_y0, &h_pull_r, h_true, h_reco_precise);
      

      track_cells = detector_.cells(sequences_.size());

      for(std::vector<Cell>::iterator ic=track_cells.begin(); ic!=track_cells.end(); ++ic){
	phis->push_back(experimental_atan2(ic->p_reco().y() - ic->ep().y(), ic->p_reco().x() - ic->ep().x()));
	zs->push_back(ic->ep().z());
      }

      LinearRegression* l = new LinearRegression(*zs, *phis, level, probmin);

      // fit best helix with through all points (vertical view)
      l->fit();
      l->invert(); // fit with y as more erroneous variable (phi = phi(y)),
      // then invert the result to have y = y(phi)
      l->dump();

      for(std::vector<Cell>::iterator ic=track_cells.begin(); ic!=track_cells.end(); ++ic){
	ic->set_p_reco(experimental_point(ic->p_reco().x(), ic->p_reco().y(),
					  l->position(l->xi()[ic - track_cells.begin()])));
      }

      delete l;

      seq.set_cells(track_cells);
      seq.set_track_id(sequences_.size());
      seq.set_helix(helix(h_reco_precise.center(), h_reco_precise.radius(), l->tangent()));
      sequences_.push_back(seq);

      hs_reco_rough->push_back(h_reco_rough);
      hs_reco_precise->push_back(h_reco_precise);
      cells_to_reconstruct = detector_.leftover_cells();

      m.message(" assigned ", track_cells.size(), " cells to track ", seq.track_id(), " leaving ", cells_to_reconstruct.size(), " leftovers ", mybhep::VERBOSE);
    }

    m.message( " reconstructed ", hs_reco_precise->size(), " circles from this cluster ", mybhep::VERBOSE);

    delete hs_reco_rough;
    delete hs_reco_precise;
    delete phis;
    delete zs;

    clock.stop(" Sultan: reconstruct_cluster ");

    return;
  }


  //*******************************************************************
  void Sultan::set_unclustered_cells(topology::Tracked_data & tracked_data_){
    //*******************************************************************

    tracked_data_.set_unclustered_cells(detector_.cells(-1));


  }

  //*************************************************************
  void Sultan::fill_fast_information( mybhep::event& evt ){
    //*************************************************************

    for(size_t i=0; i<evt.digi_particles().size(); i++){
      if( evt.digi_particles()[i]->find_property("SUNAMI") )
        fill_fast_information( evt.digi_particles()[i] );
    }


    return;

  }

  //*************************************************************
  void Sultan::fill_fast_information( mybhep::particle* p ){
    //*************************************************************

    for(size_t i=0; i<p->hits("trk").size(); i++){
      fill_fast_information(p->hits("trk")[i]);
    }

    return;

  }

  //*************************************************************
  void Sultan::fill_fast_information( mybhep::hit* h ){
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
  int Sultan::cell_side( const topology::cell & c){
    //*************************************************************

    if( SuperNemo )
      {
        if( c.ep().x().value() > 0. )
          return 1;

        return -1;
      }


    if( c.ep().radius().value() > FoilRadius )
      return 1;

    return -1;

  }


  size_t Sultan::near_level( const topology::cell & c1, const topology::cell & c2 ){

    // returns 0 for far-away cell
    // 1 for cells separated by nofflayers
    // 2 for diagonal cells
    // 3 for side-by-side cells

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
      std::clog << " (c " << c2.id() << " d " << distance.value() << " )";

    if( fabs(distance.value() - limit_side) < precision )
      return 3;

    if( fabs(distance.value() - limit_diagonal) < precision )
      return 2;

    if( distance.value() < limit_diagonal*(1. + nofflayers) )
      return 1;

    return 0;


  }


  //*************************************************************
  void Sultan::setup_cells(){
    //*************************************************************

    for(std::vector<topology::Cell>::iterator icell=cells_.begin(); icell!=cells_.end(); ++icell){
      icell->set_print_level(level);
      icell->set_probmin(probmin);
    }

    return;

  }





  //*************************************************************
  int Sultan::get_effective_layer(const mybhep::hit &hit){
    //*************************************************************

    // calculates effective layer of a hit in calorimeter block on +-X wall
    // returns -8, -7, ..., -1, 0, 0, 1, ..., 7, 8
    // returns N if the center of calo block is within layers N-1 and N
    // for instance, if block center is between layers 3 and 4, returns 4

    std::vector<double> block_pos;

    //this deals with true hits
    if (hit.find_property("Ini_Ekin")){
      if(hit.find_property("BLK_Pos")){
        mybhep::vector_from_string(hit.fetch_property("BLK_Pos"), block_pos);
      }
    }
    //this deals with digi hits
    else if (hit.find_property("E")){
      std::string bp = hit.fetch_property("BLK_POS");
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
  topology::calorimeter_hit Sultan::make_calo_hit(const mybhep::hit &ahit, size_t _id){
    //*******************************************************************

    std::vector<double> block_pos;
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
      std::string bp = ahit.fetch_property("BLK_POS");
      mybhep::vector_from_string(bp, block_pos);
      en = mybhep::double_from_string(ahit.fetch_property("E"));
      time = mybhep::double_from_string(ahit.fetch_property("TIME"));
    }

    topology::experimental_point center(block_pos[0], block_pos[1], block_pos[2], 0., 0., 0.);

    std::string block_type = ahit.fetch_property("BLK");
    std::string plane;

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
    topology::plane pl(center, sizes, norm, level, probmin);
    topology::calorimeter_hit ch(pl, e, t, _id, layer, level, probmin);

    return ch;
  }

  //*************************************************************
  void Sultan::order_cells(){
    //*************************************************************

    clock.start(" Sultan: order cells ","cumulative");

    if( cells_.size() ){
      if( level >= mybhep::VVERBOSE ){
        std::clog << " printing cells " << cells_.size() << std::endl;
        print_cells();
        std::clog << " sorting cells " << std::endl;
      }

      //  std::sort( cells_.begin(), cells_.end(), topology::cell::compare );
      std::sort( cells_.begin(), cells_.end());
    }

    clock.stop(" Sultan: order cells ");

    return;

  }


  void Sultan::setDoDriftWires(bool ddw){
    doDriftWires=ddw;
    return;
  }

  void Sultan::compute_lastlayer(){
    lastlayer = 0;
    for(size_t i=0; i<planes_per_block.size(); i++){
      lastlayer += (int)planes_per_block[i];
    }
    return;
  }

  void Sultan::set_GG_GRND_diam (double ggd){
    GG_GRND_diam = ggd;
    return;
  }

  void Sultan::set_GG_CELL_diam (double ggcd){
    GG_CELL_diam = ggcd;
    return;
  }

  void Sultan::set_lastlayer(int ll_){
    lastlayer = ll_;
    return;
  }

  void Sultan::set_num_cells_per_plane(int ncpp){
    if (ncpp <= 0)
      {
        num_cells_per_plane = -1; // invalid value
      }
    else
      {
        num_cells_per_plane = ncpp;
      }
    return;
  }

  void Sultan::set_SOURCE_thick(double st){
    if (st <= 0.0)
      {
        SOURCE_thick = std::numeric_limits<double>::quiet_NaN ();
      }
    else
      {
        SOURCE_thick = st;
      }
    return;
  }

  // What is it ?
  void Sultan::set_module_nr(const std::string & mID){
    _moduleNR=mID;
    return;
  }

  // What is it ?
  int Sultan::get_module_nr(void){
    return _MaxBlockSize;
  }

  void Sultan::set_MaxBlockSize(int mbs){
    _MaxBlockSize=mbs;
    return;
  }

  void Sultan::set_pmax(double v){
    if ( v <= 0.0)
      {
        pmax = std::numeric_limits<double>::quiet_NaN ();
      }
    else
      {
        pmax = v;
      }
    return;
  }

  void Sultan::set_MaxTime(double v){
    MaxTime = v;
    return;
  }

  void Sultan::set_PrintMode(bool v){
    PrintMode = v;
    return;
  }

  void Sultan::set_hfile(std::string v){
    hfile = v;
    return;
  }

  void Sultan::set_probmin(double v){
    probmin = v;
    return;
  }

  void Sultan::set_nofflayers(size_t v){
    nofflayers = v;
    return;
  }

  void Sultan::set_first_event(int v){
    first_event_number = v;
    return;
  }

  void Sultan::set_level(std::string v){
    level = mybhep::get_info_level(v);
    m = mybhep::messenger(level);
    return;
  }

  void Sultan::set_len(double v){
    len = v;
    return;
  }

  void Sultan::set_vel(double v){
    vel = v;
    return;
  }

  void Sultan::set_rad(double v){
    rad = v;
    return;
  }

  void Sultan::set_GG_CELL_pitch (double p){
    GG_CELL_pitch = p;
    set_rad (GG_CELL_pitch / cos(M_PI/8.));
    set_GG_CELL_diam (rad);
    set_CellDistance (rad);
    return;
  }

  void Sultan::set_CellDistance(double v){
    CellDistance = v;
    return;
  }

  void Sultan::set_SuperNemo(bool v){
    SuperNemo = v;
    return;
  }

  void Sultan::set_SuperNemoChannel(bool v){
    if (v)
      {
        set_SuperNemo (true);
        SuperNemoChannel = true;
        set_NemoraOutput (false);
        set_N3_MC (false);
        setDoDriftWires (false);
        set_MaxBlockSize (1);
      }
    else
      {
        SuperNemoChannel = false;
      }
    return;
  }

  void Sultan::set_NemoraOutput(bool no){
    NemoraOutput = no;
    return;
  }

  void Sultan::set_N3_MC(bool v){
    N3_MC = v;
    return;
  }

  void Sultan::set_FoilRadius(double v){
    FoilRadius = v;
    return;
  }

  void Sultan::set_xsize(double v){
    xsize = v;
    return;
  }

  void Sultan::set_ysize(double v){
    ysize = v;
    return;
  }

  void Sultan::set_zsize(double v){
    zsize = v;
    return;
  }

  void Sultan::set_bfield(double v){
    bfield = v;
    return;
  }



}

