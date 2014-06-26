#include "CATUtils/EventDisplay.h"
#include <vector>
#include <mybhep/system_of_units.h>
#include <sys/time.h>

using namespace CAT;
using namespace SULTAN;

//************************************************************
   EventDisplay::EventDisplay(mybhep::gstore st){
//*************************************************************

     level = mybhep::get_info_level(st.fetch_sstore("VERBOSITY"));

    m = mybhep::messenger(level);

    //-- read param --//

    if( st.find_istore("PlotTopView"))
      PlotTopView = st.fetch_istore("PlotTopView");
    else
      PlotTopView = 1;

    if( st.find_istore("PlotSideView"))
      PlotSideView = st.fetch_istore("PlotSideView");
    else
      PlotSideView = 1;

    if( st.find_istore("PlotInitialHits"))
      PlotInitialHits = st.fetch_istore("PlotInitialHits");
    else
      PlotInitialHits = 1;

    if( st.find_istore("PlotHelices"))
      PlotHelices = st.fetch_istore("PlotHelices");
    else
      PlotHelices = 1;

    if( st.find_istore("PlotTangents") )
      PlotTangents = st.fetch_istore("PlotTangents");
    else
      PlotTangents = 0;

    if( st.find_istore("PlotTriplets") )
      PlotTriplets = st.fetch_istore("PlotTriplets");
    else
      PlotTriplets = 0;

    if( st.find_istore("PlotCats") )
      PlotCats = st.fetch_istore("PlotCats");
    else
      PlotCats = 1;

    if( st.find_istore("PlotCalos") )
      PlotCalos = st.fetch_istore("PlotCalos");
    else
      PlotCalos = 1;

    if( st.find_istore("UseCalosInZoom") )
      UseCalosInZoom = st.fetch_istore("UseCalosInZoom");
    else
      UseCalosInZoom = 1;

    if( st.find_istore("PlotTrueTracks") )
      PlotTrueTracks = st.fetch_istore("PlotTrueTracks");
    else
      PlotTrueTracks = 1;

    if( st.find_istore("PlotNemoTracks") )
      PlotNemoTracks = st.fetch_istore("PlotNemoTracks");
    else
      PlotNemoTracks = 1;

    if( st.find_istore("PlotCellIds") )
      PlotCellIds = st.fetch_istore("PlotCellIds");
    else
      PlotCellIds = 1;

    if( st.find_istore("PlotCellChi2s") )
      PlotCellChi2s = st.fetch_istore("PlotCellChi2s");
    else
      PlotCellChi2s = 1;

    if( st.find_istore("PlotCaloEnergy") )
      PlotCaloEnergy = st.fetch_istore("PlotCaloEnergy");
    else
      PlotCaloEnergy = 1;

    if( st.find_istore("PlotLegend"))
      PlotLegend = st.fetch_istore("PlotLegend");
    else
      PlotLegend = 0;

    if( st.find_sstore("PlotFormat"))
      PlotFormat = st.fetch_sstore("PlotFormat");
    else
      PlotFormat = "eps";

    if( st.find_sstore("first_event"))
      first_event_number = st.fetch_istore("first_event");
    else
      first_event_number = -1;

    Nbins = 10;

    event_number=0;

}

//************************************************************
EventDisplay::EventDisplay(void){
  //*************************************************************
  return;
}

//*************************************************************
EventDisplay::~EventDisplay() {
//*************************************************************

}

//*************************************************************
void EventDisplay::readDstProper(mybhep::sstore global, mybhep::EventManager2 *eman) {
//*************************************************************

  if (! global.find("GEOM_MODULES"))
    {
      _MaxBlockSize = 1;
      m.message(" no bar design, MUDULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
      fflush(stdout);
    }
  else
    {
      _MaxBlockSize = mybhep::int_from_string(global.fetch("GEOM_MODULES"));
      m.message(" bar design with MUDULES = ",_MaxBlockSize,"\n",mybhep::NORMAL);
      fflush(stdout);
    }

  if(! global.find("NEMO3"))
    {
      SuperNemo = true;
      m.message("SuperNemo kind of data",mybhep::NORMAL); fflush(stdout);
    }
  else
    {
      SuperNemo = false;
      m.message("Nemo-3 kind of data",mybhep::NORMAL); fflush(stdout);
    }

  CellDistance =mybhep::float_from_string(global.fetch("GEOM_GG_CELL_diam"));

  num_blocks = mybhep::int_from_string(global.fetch("GEOM_num_blocks"));

  mybhep::vector_from_string(global.fetch("GEOM_planes_per_block"),planes_per_block);

  ysize = mybhep::double_from_string(global.fetch("GEOM_CHAMBER_height"));

  if( SuperNemo )
    {

      xsize = mybhep::float_from_string(global.fetch("GEOM_CHAMBER_withd"));
      zsize = mybhep::float_from_string(global.fetch("GEOM_CHAMBER_length"));

      mybhep::vector_from_string(global.fetch("GEOM_gaps_Z"),gaps_Z);


      GG_GRND_diam = mybhep::double_from_string(global.fetch("GEOM_GG_GRND_diam"));

      GG_CELL_diam = CellDistance;

      CHAMBER_X = xsize;
      CHAMBER_height = ysize;
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

  m.message("xsize is read as",xsize,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("ysize is read as",ysize,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("zsize is read as",zsize,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("distance from wire to wire is read as: ",CellDistance,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("first event number", first_event_number, mybhep::NORMAL);
  m.message("",mybhep::NORMAL); fflush(stdout);

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

  return;
}


//*************************************************************
void EventDisplay::readDstProper() {
//*************************************************************


  m = mybhep::messenger(level);

  _MaxBlockSize = 1;
  m.message(" no bar design, MUDULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
  fflush(stdout);

  if(SuperNemo)
    {
      m.message("SuperNemo kind of data",mybhep::NORMAL); fflush(stdout);
    }
  else
    {
      m.message("Nemo-3 kind of data",mybhep::NORMAL); fflush(stdout);
    }


  NemoraOutput = false;

  num_blocks = 3;

  planes_per_block.push_back(4);
  planes_per_block.push_back(2);
  planes_per_block.push_back(3);

  m.message("",mybhep::NORMAL); fflush(stdout);
  m.message("xsize is read as",xsize,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("ysize is read as",ysize,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("zsize is read as",zsize,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("distance from wire to wire is read as: ",CellDistance,"mm",mybhep::NORMAL); fflush(stdout);
  if( !SuperNemo )
    m.message("foil radius: ",FoilRadius,"mm",mybhep::NORMAL); fflush(stdout);
  m.message("print level:", level, mybhep::NORMAL); fflush(stdout);
  m.message("",mybhep::NORMAL); fflush(stdout);

  //parameters for error parametrization

  return;
}


//*************************************************************
void EventDisplay::DeleteDisplay(void){
//*************************************************************

  // Delete TGraph'S

  for(unsigned int i = 0; i < graphlist.size() ; i++ ) {
    delete graphlist[i];
  }

  graphlist.clear();

  for(unsigned int i = 0; i < pavelist.size() ; i++ ) {
    delete pavelist[i];
  }

  pavelist.clear();


  // UnZOOM all displays

  mapzx->GetXaxis()->UnZoom();
  mapzx->GetYaxis()->UnZoom();

  mapzy->GetXaxis()->UnZoom();
  mapzy->GetYaxis()->UnZoom();

  return;

}



  //*************************************************************
void EventDisplay::initialize( ) {
  //*************************************************************

  readDstProper();

  GenerateWires();

  if( !SuperNemo )
    {

      run_time = 0.;
      run_list.clear();
      first_event=true;

    }

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(0);


  mapzx = new TH2F("mapzx","",
                   100,-zsize/2. - calo_Z,
                   zsize/2. + calo_Z,
                   100,-xsize/2. - calo_X,
                   xsize/2. + calo_X);
  mapzy = new TH2F("mapzy","",
                   100,-zsize/2. - calo_Z,
                   zsize/2. + calo_Z,
                   100,-ysize/2. - calo_Y,
                   ysize/2. + calo_Y);
  canvas = new TCanvas("canvas","CANVAS",600,0,800,600);

  FirstStarXY_used = false;

  if( PlotLegend ){
    leg_xz = new TLegend(0.8, 0.8, 1., 1.);
    leg_yz = new TLegend(0.8, 0.8, 1., 1.);
    leg_xz->SetTextSize(0.03);
    leg_yz->SetTextSize(0.03);
    leg_xz->SetFillColor(4000);
    leg_yz->SetFillColor(4000);

    if( PlotTrueTracks ){
      leg_xz_true = new TLegend(0.8, 0.8, 1., 1.);
      leg_yz_true = new TLegend(0.8, 0.8, 1., 1.);
      leg_xz_true->SetTextSize(0.03);
      leg_yz_true->SetTextSize(0.03);
      leg_xz_true->SetFillColor(4000);
      leg_yz_true->SetFillColor(4000);
    }

  }

  size_t sv = 0;
  if( PlotTopView )
    sv ++;
  if( PlotSideView )
    sv ++;
  size_t ncanvas = 0;
  if( !SuperNemo && PlotNemoTracks )
    ncanvas ++;
  if( PlotInitialHits )
    ncanvas ++;
  if( PlotTrueTracks )
    ncanvas ++;
  if( PlotCats || PlotHelices || PlotSultan )
    ncanvas ++;

  canvas->Divide(sv, ncanvas);

  color_detector = kBlack;
  color_cells = kBlue;
  color_fast_cells = kBlue + 2;
  color_cats = kCyan;
  color_sultan = kViolet;
  color_nemora = kRed;
  color_true_track = kPink;
  color_tangent = kBlack;
  color_triplet = kMagenta;
  color_helix = kOrange;
  color_calos = kAzure;


  return;
}

//*************************************************************
bool EventDisplay::InitializeDisplayEvent( mybhep::sstore store, mybhep::gstore gs , mybhep::EventManager2 *eman) {
//*************************************************************

  PlutsMode = false;

  readDstProper(store, eman);

  GenerateWires();

  if( !SuperNemo )
    {

      run_time = 0.;
      run_list.clear();
      first_event=true;

    }

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(0);


  mapzx = new TH2F("mapzx","",
                   100,-zsize/2. - calo_Z,
                   zsize/2. + calo_Z,
                   100,-xsize/2. - calo_X,
                   xsize/2. + calo_X);
  mapzy = new TH2F("mapzy","",
                   100,-zsize/2. - calo_Z,
                   zsize/2. + calo_Z,
                   100,-ysize/2. - calo_Y,
                   ysize/2. + calo_Y);
  canvas = new TCanvas("canvas","CANVAS",600,0,800,600);

  FirstStarXY_used = false;


  if( PlotLegend ){
    leg_xz = new TLegend(0.8, 0.8, 1., 1.);
    leg_yz = new TLegend(0.8, 0.8, 1., 1.);
    leg_xz->SetTextSize(0.03);
    leg_yz->SetTextSize(0.03);
    leg_xz->SetFillColor(4000);
    leg_yz->SetFillColor(4000);

    if( PlotTrueTracks ){
      leg_xz_true = new TLegend(0.8, 0.8, 1., 1.);
      leg_yz_true = new TLegend(0.8, 0.8, 1., 1.);
      leg_xz_true->SetTextSize(0.03);
      leg_yz_true->SetTextSize(0.03);
      leg_xz_true->SetFillColor(4000);
      leg_yz_true->SetFillColor(4000);
    }

  }

  size_t sv = 0;
  if( PlotTopView )
    sv ++;
  if( PlotSideView )
    sv ++;
  size_t ncanvas = 0;
  if( !SuperNemo && PlotNemoTracks )
    ncanvas ++;
  if( PlotInitialHits )
    ncanvas ++;
  if( PlotTrueTracks )
    ncanvas ++;
  if( PlotCats || PlotHelices )
    ncanvas ++;

  canvas->Divide(sv, ncanvas);

    //////////////////////////////////
    //                              //
    //      COLOR CODE              //
    //                              //
    //   0 ........ white           //
    //   1 ........ black           //
    //   2 ........ red             //
    //   3 ........ green 1         //
    //   4 ........ blue            //
    //   5 ........ yellow          //
    //   6 ........ pink            //
    //   7 ........ clear blue      //
    //   8 ........ green 2         //
    //   9 ........ blue 2          //
    //   10 ....... white 2         //
    //   11 ....... grey            //
    //                              //
    //////////////////////////////////

    color_detector = kBlack; // green
    color_cells = kBlue; // clear blue
    color_fast_cells = kBlue + 2; // blue
    color_cats = kCyan; // black
    color_sultan = kViolet;
    color_nemora = kRed; // red
    color_true_track = kPink; // pink
    color_tangent = kGray; // grey
    color_triplet = kMagenta; // pink
    color_helix = kOrange; // red
    color_calos = kAzure; // clear_blue

    return true;
}


//*************************************************************
void EventDisplay::SetPlutsMode( bool plutsmode ) {
//*************************************************************

  PlutsMode = plutsmode;

  return;
}


//*************************************************************
void EventDisplay::GetPlotLimit( std::vector<CAT::topology::calorimeter_hit> calos ) {
//*************************************************************

  float XMin = mybhep::default_min;
  float XMax = mybhep::default_max;
  float YMin = mybhep::default_min;
  float YMax = mybhep::default_max;
  float ZMin = mybhep::default_min;
  float ZMax = mybhep::default_max;


  for(size_t i=0; i<cells_.size(); i++){
    CAT::topology::cell c = cells_[i];

    if( !c.fast() ) continue;

    if( c.ep().x().value() - c.r().value() < XMin )
      XMin = c.ep().x().value() - c.r().value();
    if( c.ep().y().value() < YMin )
      YMin = c.ep().y().value();
    if( c.ep().z().value() - c.r().value() < ZMin )
      ZMin = c.ep().z().value() - c.r().value();

    if( c.ep().x().value() + c.r().value() > XMax )
      XMax = c.ep().x().value() + c.r().value();
    if( c.ep().y().value() > YMax )
      YMax = c.ep().y().value();
    if( c.ep().z().value() + c.r().value() > ZMax )
      ZMax = c.ep().z().value() + c.r().value();

  }


  if( UseCalosInZoom ){
    for(size_t i=0; i<calos.size(); i++)
      {

        CAT::topology::calorimeter_hit h = calos[i];

        if( h.pl().center().x().value() - calo_X/2. < XMin )
          XMin = h.pl().center().x().value() - calo_X/2.;
        if( h.pl().center().y().value() - calo_Y/2. < YMin )
          YMin = h.pl().center().y().value() - calo_Y/2.;
        if( h.pl().center().z().value() - calo_Z/2. < ZMin )
          ZMin = h.pl().center().z().value() - calo_Z/2.;

        if( h.pl().center().x().value() + calo_X/2.  > XMax )
          XMax = h.pl().center().x().value() + calo_X/2.;
        if( h.pl().center().y().value() + calo_Y/2. > YMax )
          YMax = h.pl().center().y().value() + calo_Y/2.;
        if( h.pl().center().z().value() + calo_Z/2.> ZMax )
          ZMax = h.pl().center().z().value() + calo_Z/2.;

      }
  }

  Xmin = std::max(XMin - CellDistance, -xsize/2. - calo_X);
  Xmax = std::min(XMax + CellDistance, xsize/2. + calo_X);

  Ymin = std::max(YMin - CellDistance, -ysize/2. - calo_Y);
  Ymax = std::min(YMax + CellDistance, ysize/2. + calo_Y);

  Zmin = std::max(ZMin - CellDistance, -zsize/2. - calo_Z);
  Zmax = std::min(ZMax + CellDistance, zsize/2. + calo_Z);


  return;

}

//*************************************************************
void EventDisplay::DrawDetectorXZ( void ){
//*************************************************************

  if( !SuperNemo )
    {
      DrawDetectorCircleXZ( InnerRadius, color_detector );

      DrawDetectorCircleXZ( OuterRadius, color_detector );

      DrawDetectorCircleXZ( FoilRadius, color_detector );

      /*
        for(size_t i=0; i<18; i++)
        DrawDetectorCircleXZ( LayerRadius[i], 1 );
      */

    }
  else
    {

      double* zdet = (double*)malloc(sizeof(double)*4);
      double* xdet = (double*)malloc(sizeof(double)*4);
      double* zfoil = (double*)malloc(sizeof(double)*2);
      double* xfoil = (double*)malloc(sizeof(double)*2);


      zdet[0] = zsize/2.;
      zdet[1] = zsize/2.;
      zdet[2] = -zsize/2.;
      zdet[3] = -zsize/2.;

      xdet[0] = -xsize/2.;
      xdet[1] = xsize/2.;
      xdet[2] = xsize/2.;
      xdet[3] = -xsize/2.;

      zfoil[0] = 0.;
      zfoil[1] = 0.;

      xfoil[0] = -xsize/2.;
      xfoil[1] = xsize/2.;

      TGraph *graphdet = new TGraph(4,zdet,xdet);
      graphdet->SetLineColor(color_detector);
      graphdet->Draw();
      graphlist.push_back(graphdet);

      TGraph *graphfoil = new TGraph(2,zfoil,xfoil);
      graphfoil->SetLineColor(color_detector);
      graphfoil->Draw();
      graphlist.push_back(graphfoil);


      free(zdet);
      free(xdet);
      free(zfoil);
      free(xfoil);

    }

  for(size_t j=0; j<DriftWires.size(); j++)
    {

      FirstStarXY_used = true;
      if( SuperNemo )
        {
          FirstStarXY = new TMarker( DriftWires[j].z,
                                     DriftWires[j].x,
                                     7);
        }
      else
        {
          FirstStarXY = new TMarker( DriftWires[j].x,
                                     DriftWires[j].z,
                                     7);

        }

      FirstStarXY->Draw();

    }

  return;
}


//*************************************************************
void EventDisplay::DrawDetectorCircleXZ( double radius, size_t color ){
//*************************************************************

  size_t npoints = 100;

  double* x = (double*)malloc(sizeof(double)*npoints);
  double* z = (double*)malloc(sizeof(double)*npoints);

  for(size_t i=0; i< npoints; i++)  // loop on hits
    {
      double angle = 4.*asin(1.)*((double)i)/((double)(npoints-1));
      x[i] = radius*cos(angle);
      z[i] = radius*sin(angle);
    }

  TGraph *graph = new TGraph(100,z,x);
  graph->SetLineColor(color);
  graph->Draw();
  graphlist.push_back(graph);

  free(x);
  free(z);

  return;
}

//*************************************************************
void EventDisplay::DrawDetectorYZ( void ){
//*************************************************************

  double* zdet = (double*)malloc(sizeof(double)*4);
  double* ydet = (double*)malloc(sizeof(double)*4);

  if( SuperNemo )
    {
      double* zfoil = (double*)malloc(sizeof(double)*2);
      double* yfoil = (double*)malloc(sizeof(double)*2);

      zdet[0] = zsize/2.;
      zdet[1] = zsize/2.;
      zdet[2] = -zsize/2.;
      zdet[3] = -zsize/2.;

      ydet[0] = -ysize/2.;
      ydet[1] = ysize/2.;
      ydet[2] = ysize/2.;
      ydet[3] = -ysize/2.;

      zfoil[0] = 0.;
      zfoil[1] = 0.;

      yfoil[0] = -ysize/2.;
      yfoil[1] = ysize/2.;

      TGraph *graphfoil = new TGraph(2,zfoil,yfoil);
      graphfoil->SetLineColor(color_detector);
      graphfoil->Draw();
      graphlist.push_back(graphfoil);

      free(zfoil);
      free(yfoil);
    }
  else
    {
      double* zinnerdet = (double*)malloc(sizeof(double)*4);
      double* yinnerdet = (double*)malloc(sizeof(double)*4);
      double* zfoil = (double*)malloc(sizeof(double)*4);
      double* yfoil = (double*)malloc(sizeof(double)*4);


      zdet[0] = OuterRadius;
      zdet[1] = OuterRadius;
      zdet[2] = -OuterRadius;
      zdet[3] = -OuterRadius;

      ydet[0] = -ysize/2.;
      ydet[1] = ysize/2.;
      ydet[2] = ysize/2.;
      ydet[3] = -ysize/2.;

      zinnerdet[0] = InnerRadius;
      zinnerdet[1] = InnerRadius;
      zinnerdet[2] = -InnerRadius;
      zinnerdet[3] = -InnerRadius;

      yinnerdet[0] = -ysize/2.;
      yinnerdet[1] = ysize/2.;
      yinnerdet[2] = ysize/2.;
      yinnerdet[3] = -ysize/2.;

      zfoil[0] = FoilRadius;
      zfoil[1] = FoilRadius;
      zfoil[2] = -FoilRadius;
      zfoil[3] = -FoilRadius;

      yfoil[0] = -ysize/2.;
      yfoil[1] = ysize/2.;
      yfoil[2] = ysize/2.;
      yfoil[3] = -ysize/2.;

      TGraph *graphfoil = new TGraph(4,zfoil,yfoil);
      graphfoil->SetLineColor(color_detector);
      graphfoil->Draw();
      graphlist.push_back(graphfoil);

      TGraph *graphinnerdet = new TGraph(4,zinnerdet,yinnerdet);
      graphinnerdet->SetLineColor(color_detector);
      graphinnerdet->Draw();
      graphlist.push_back(graphinnerdet);

      free(zfoil);
      free(yfoil);
      free(zinnerdet);
      free(yinnerdet);
    }


  TGraph *graphdet = new TGraph(4,zdet,ydet);
  graphdet->SetLineColor(color_detector);
  graphdet->Draw();
  graphlist.push_back(graphdet);



  free(zdet);
  free(ydet);


  return;
}

//*************************************************************
void EventDisplay::DrawCircleXZ( void ){
//*************************************************************

  size_t npoints = sunami_position[0].size();

  double *x = new double[npoints];
  double *z = new double[npoints];
  double *r = new double[npoints];
  double *xc = new double[100];
  double *zc = new double[100];

  size_t index = 0;
  for(size_t i=0; i< npoints; i++)  // loop on hits
    {
      x[index] = sunami_position[0][i].circle.center.x;
      z[index] = sunami_position[0][i].circle.center.z;
      r[index] = sunami_position[0][i].circle.radius;

      size_t color;
      if( FastHit( sunami_position[0][i] ) )
        color = color_fast_cells;
      else
        color = color_cells;

      for(size_t j=0; j<100; j++)
        {
          double angle = 4.*asin(1.)*((double)j)/100.;
          xc[j] = x[index] + r[index]*cos(angle);
          zc[j] = z[index] + r[index]*sin(angle);
        }

       TGraph *graph = new TGraph(100,zc,xc);

       graph->SetLineColor(color);
       graph->Draw();

       graphlist.push_back(graph);

      index++;
    }//end of loop over hits

  delete x;
  delete z;
  delete r;
  delete xc;
  delete zc;

  return;
}

//*************************************************************
float EventDisplay::FastHit(POSITION position){
//*************************************************************

  if( position.circle.radius != 0 &&    // fast hit
      position.tdelay == 0.)
    return 1.;
  else if( position.circle.radius == 0 &&    // delayed hit
           position.tdelay != 0.)
    return -1.;
  else
    {
      m.message("Problem: hit has radius", position.circle.radius, "delay time", position.tdelay, mybhep::NORMAL); fflush(stdout);
      return -1.;
    }
}

//*************************************************************
void EventDisplay::DrawCircleYZ( void ){
//*************************************************************

  size_t npoints = sunami_position[0].size();

  for(size_t i=0; i< npoints; i++)  // loop on hits
    {
      double *y = new double[7];
      double *z = new double[7];

      y[0] = sunami_position[0][i].y;
      y[1] = sunami_position[0][i].y + sunami_position[0][i].erry;
      y[2] = y[1];
      y[3] = sunami_position[0][i].y - sunami_position[0][i].erry;
      y[4] = y[3];
      y[5] = y[0];
      y[6] = y[0];

      z[0] = sunami_position[0][i].circle.center.z - sunami_position[0][i].circle.radius;
      z[1] = z[0];
      z[2] = sunami_position[0][i].circle.center.z + sunami_position[0][i].circle.radius;
      z[3] = z[2];
      z[4] = z[0];
      z[5] = z[0];
      z[6] = z[2];

      TGraph *graph = new TGraph(7,z,y);
      graph->SetLineColor(color_fast_cells);
      graph->Draw();
      graphlist.push_back(graph);

      delete y;
      delete z;
    }//end of loop over hits


  return;
}



//*************************************************************
void EventDisplay::GenerateWires( void ){
//*************************************************************

  DriftWires.clear();

  if( SuperNemo )
    {

     // TRACKING GG BLOCKS
      double theta = M_PI/8.;
      double GG_CELL_pitch = CellDistance*cos(theta);
      num_cells_per_plane=(int)((CHAMBER_X-GG_GRND_diam)/GG_CELL_pitch);
      GG_BLOCK_X = num_cells_per_plane*GG_CELL_pitch+GG_GRND_diam;
      GG_BLOCK_Y = CHAMBER_height;
      double GG_BLOCK_thick[num_blocks];
      double GG_BLOCK_posZ[num_blocks];
      double distance = SOURCE_thick/2.;
      //Calculating thickness and positions of blocks
      for(int i=0; i<num_blocks; i++){
        GG_BLOCK_thick[i]=(planes_per_block[i]*GG_CELL_pitch+GG_GRND_diam);
        GG_BLOCK_posZ[i] = distance + gaps_Z[i] + GG_BLOCK_thick[i]/2.;
        distance = GG_BLOCK_posZ[i] + GG_BLOCK_thick[i]/2.;
      }

      int sign[2];
      sign[0] = 1;
      sign[1] = -1;

      double x_offset = -8.;

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
                double xpos = -(CHAMBER_X-GG_GRND_diam)/2.+increment+GG_CELL_pitch + x_offset;

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
              double layerphi = 2.*acos(-1.)/NOfWires[i];
              double ph = FirstWirePhi[i] + j*layerphi;

              CAT::POINT point;
              point.x = LayerRadius[i]*cos(ph);
              point.z = LayerRadius[i]*sin(ph);

              DriftWires.push_back( point );
            }
        }
    }


  return;

}


//*************************************************************
void EventDisplay::Nemo3EventDisplayXZ(void){
//*************************************************************

  if( !SuperNemo )
    {

      icanvas ++;
      canvas->cd(icanvas);

      mapzx->GetXaxis()->SetRangeUser(Zmin, Zmax);
      mapzx->GetYaxis()->SetRangeUser(Xmin, Xmax);
      mapzx->Draw();

      DrawDetectorXZ();

      DrawCircleXZ();

      DrawNemo3HitXZ();

      FirstStarXY_used = true;
      FirstStarXY = new TMarker( n3event.vertex[2],
                                 n3event.vertex[0],
                                 5);

      FirstStarXY->Draw();

      canvas->Update();

    }


  return;
}

//*************************************************************
void EventDisplay::Nemo3EventDisplayYZ(void){
//*************************************************************

  if( !SuperNemo )
    {

      icanvas ++;
      canvas->cd(icanvas);

      //      canvas->SetTitle("Nemo3");

      mapzy->GetYaxis()->SetRangeUser(Ymin, Ymax);
      mapzy->GetXaxis()->SetRangeUser(Zmin, Zmax);
      mapzy->Draw();

      DrawDetectorYZ();

      DrawCircleYZ();

      DrawNemo3HitYZ();

      FirstStarXY_used = true;
      FirstStarXY = new TMarker( n3event.vertex[2],
                                 n3event.vertex[1],
                                 5);

      FirstStarXY->Draw();

      canvas->Update();

    }


  return;
}

//*************************************************************
void EventDisplay::DrawNemo3HitXZ( void ){
//*************************************************************

  size_t color = color_nemora;

  size_t ntr = nemo3_parts.size();

  double* x = (double*)malloc(sizeof(double)*ntr);
  double* z = (double*)malloc(sizeof(double)*ntr);
  double* r = (double*)malloc(sizeof(double)*ntr);
  double* xc = (double*)malloc(sizeof(double)*100);
  double* zc = (double*)malloc(sizeof(double)*100);

  for(size_t itrk = 0; itrk < ntr; itrk++) // Loop over tracks
    {

      mybhep::particle& p = *nemo3_parts[itrk];
      const std::vector<mybhep::hit*>& phits = p.hits("trk");
      const std::vector<mybhep::hit*>& chits = p.hits("cal");

      size_t cnodes = chits.size();
      size_t nnodes = phits.size();
      if( nnodes == 0 ) continue;

      size_t npoints = cnodes + nnodes;
      size_t offset = 0;
      npoints += 1;  // vertex
      offset = 1;

      double* zt = (double*)malloc(sizeof(double)*npoints);
      double* xt = (double*)malloc(sizeof(double)*npoints);

      xt[0] = p.vertex()[0];
      zt[0] = p.vertex()[2];

      for(size_t j=0; j<(nnodes+cnodes); j++)
        {

          bool TrkHit = false;
          if( j < nnodes )
            TrkHit = true;

          mybhep::hit *ahit;

          if( TrkHit )
            ahit = phits[j];
          else
            ahit = chits[j - nnodes];


          if( TrkHit )
            {

              xt[j+offset] = ahit->x().x();
              zt[j+offset] = ahit->x().z();

            }
          else
            {
              float x=0., y=0., z=0.;
              if( ahit->find_property("BLK_POS"))
                {
                  std::string hit_position = ahit->fetch_property("BLK_POS");
                  sscanf(hit_position.c_str(),"%f_%f_%f",&x, &y, &z);
                }
              else
                {
                  m.message(" problem: calo hit without calo position", mybhep::NORMAL); fflush(stdout);
                }
              xt[j+offset] = x;
              zt[j+offset] = z;

            }

        }

      TGraph *graph = new TGraph(npoints,zt,xt);  // line joining hits
      graph->Draw();
      graph->SetLineColor(color);
      graphlist.push_back(graph);


      free(xt);
      free(zt);
    }

  free(x);
  free(z);
  free(r);
  free(xc);
  free(zc);

  return;

}

//*************************************************************
void EventDisplay::DrawNemo3HitYZ( void ){
//*************************************************************

  size_t color = color_nemora;

  size_t ntr = nemo3_parts.size();

  double* z = (double*)malloc(sizeof(double)*ntr);
  double* r = (double*)malloc(sizeof(double)*ntr);
  double* pitch = (double*)malloc(sizeof(double)*ntr);
  double* yc = (double*)malloc(sizeof(double)*100);
  double* zc = (double*)malloc(sizeof(double)*100);

    for(size_t itrk = 0; itrk < ntr; itrk++) // Loop over tracks
    {

      mybhep::particle& p = *nemo3_parts[itrk];

      const std::vector<mybhep::hit*>& phits = p.hits("trk");
      const std::vector<mybhep::hit*>& chits = p.hits("cal");

      size_t cnodes = chits.size();
      size_t nnodes = phits.size();
      if( nnodes == 0 ) continue;

      size_t npoints = cnodes + nnodes;
      size_t offset = 0;

      npoints += 1;  // vertex
      offset = 1;

      double* zt = (double*)malloc(sizeof(double)*npoints);
      double* yt = (double*)malloc(sizeof(double)*npoints);

      yt[0] = p.vertex()[1];
      zt[0] = p.vertex()[2];

      for(size_t j=0; j<(nnodes+cnodes); j++)
        {

          bool TrkHit = false;
          if( j < nnodes )
            TrkHit = true;

          mybhep::hit *ahit;

          if( TrkHit )
            ahit = phits[j];
          else
            ahit = chits[j - nnodes];

          if( TrkHit )
            {
              yt[j+offset] = ahit->x().y();
              zt[j+offset] = ahit->x().z();
            }
          else
            {
              float xx=0., yy=0., zz=0.;
              if( ahit->find_property("BLK_POS"))
                {
                  std::string hit_position = ahit->fetch_property("BLK_POS");
                  sscanf(hit_position.c_str(),"%f_%f_%f",&xx, &yy, &zz);
                }
              else
                {
                  m.message(" problem: calo hit without calo position", mybhep::NORMAL); fflush(stdout);
                }
              yt[j+offset] = yy;
              zt[j+offset] = zz;

            }

        }

      TGraph *graph = new TGraph(npoints,zt,yt);  // line joining hits
      graph->Draw();
      graph->SetLineColor(color);
      graphlist.push_back(graph);


      free(yt);
      free(zt);
    }

    free(z);
    free(r);
    free(pitch);
    free(yc);
    free(zc);

  return;

}

//*************************************************************
void EventDisplay::fill_nemo3_event(mybhep::event& evt){
//*************************************************************

  std::vector<float> n3vtx;
  mybhep::vector_from_string(evt.fetch_property("N3VERTEX"), n3vtx);

  n3event.vertex[0] = n3vtx[0];
  n3event.vertex[1] = n3vtx[1];
  n3event.vertex[2] = n3vtx[2];

  if( evt.find_property("DATE") )
    n3event.date = mybhep::int_from_string(evt.fetch_property("DATE"));
  if( evt.find_property("RUN") )
    n3event.run = mybhep::int_from_string(evt.fetch_property("RUN"));
  if( evt.find_property("TIME") )
    n3event.run = mybhep::int_from_string(evt.fetch_property("TIME"));
  if( evt.find_property("NEVGA") )
    n3event.nevga = mybhep::int_from_string(evt.fetch_property("NEVGA"));


  return;

}



//*************************************************************
void EventDisplay::fill_nemo3_parts_display(  mybhep::event& evt ){
//*************************************************************

  nemo3_parts.clear();

  const std::vector<mybhep::particle*>& allparts = evt.digi_particles();
  for(size_t ipart=0; ipart<allparts.size(); ipart++)
    {

      mybhep::particle& p = *allparts[ipart];

      if( p.find_property("NEMO") )  // nemo3 reconstructed track
        {
          nemo3_parts.push_back( allparts[ipart] );
        }
    }

  return;
}


//*************************************************************
void EventDisplay::Finalize( void ){
  //*************************************************************

  if( FirstStarXY_used )
    delete FirstStarXY;
  if( PlotLegend ){
    delete leg_xz;
    delete leg_yz;
    if( PlotTrueTracks ){
      delete leg_xz_true;
      delete leg_yz_true;
    }
  }

  delete mapzx;
  delete mapzy;
  delete canvas;

  return;
}


//*************************************************************
void EventDisplay::execute(mybhep::event& evt, size_t ievent, CAT::topology::tracked_data & __tracked_data){
//*************************************************************

  m.message(" Display Event \n",mybhep::VERBOSE); fflush(stdout);

  //  event_number ++;
  //  if( event_number < first_event_number ) return false;

  set_cells(__tracked_data.get_cells());
  set_clusters(__tracked_data.get_clusters());
  std::vector<CAT::topology::scenario> CAT_sc = __tracked_data.get_scenarios();

  if( CAT_sc.size() == 0 ){
    m.message(" warning: CAT_scenarios size is ", CAT_sc.size(), mybhep::VVERBOSE);
    return;
  }

  set_CAT_sequences(CAT_sc[0].sequences());

  if( !SuperNemo )
    {
      fill_nemo3_event(evt);
      fill_nemo3_parts_display( evt );
      set_nemo_sequences(__tracked_data.get_nemo_sequences() );
    }


  GetPlotLimit( __tracked_data.get_calos());

  icanvas = 0;

  if( !PlotInitialHits && !PlotTrueTracks
      && !PlotCats && !PlotHelices && !PlotNemoTracks ){
    m.message(" problem: nothing to plot", mybhep::NORMAL);
    return;
  }


  if( PlotInitialHits ){
    if( PlotTopView )
      event_display_xz("InitialHits", __tracked_data);
    if( PlotSideView )
      event_display_yz("InitialHits", __tracked_data);
  }

  if( PlotTrueTracks ){

    if( PlotTopView )
      event_display_xz("true", __tracked_data);
    if( PlotSideView )
      event_display_yz("true", __tracked_data);

  }


  if( PlotNemoTracks ){

    if( PlotTopView )
      event_display_xz("nemo", __tracked_data);
    if( PlotSideView )
      event_display_yz("nemo", __tracked_data);

  }


  if( PlotCats || PlotHelices ){
    if( PlotTopView )
      event_display_xz("cats", __tracked_data);
    if( PlotSideView )
      event_display_yz("cats", __tracked_data);
  }


  /*
  if( !SuperNemo ){
    if( PlotTopView )
    Nemo3event_display_xz();
    if( PlotSideView )
    Nemo3event_display_yz();
  }

  */

  canvas->Update();

  if( PlutsMode )
    {
      std::string sname = "figure/event"+mybhep::to_string(ievent)+"."+PlotFormat;
      const char *name = sname.c_str();
      canvas->Print(name);
    }

  if( PlotCellChi2s && PlutsMode ){
    TPaveText pave_chi2(0.8,0.6,1.,1.);
    pave_chi2.SetFillColor(4000);
    pave_chi2.SetTextSize(0.025);

    for(size_t i=0; i<CAT_sequences_.size(); i++){
      CAT::topology::sequence s = CAT_sequences_[i];
      size_t nnodes = s.nodes().size();
      std::vector<double> helix_chi2s = s.helix_chi2s();

      for(size_t j=0; j<nnodes; j++)
        {
          size_t cid = s.nodes()[j].c().id();
          std::string title=(std::string)("cell "+mybhep::to_string(cid)+" chi2 "+mybhep::to_string_precision(helix_chi2s[j],"2"));
          pave_chi2.AddText(title.c_str());

        }
    }



    pave_chi2.Draw();

    canvas->Update();

    std::string sname = "figure/chi2_event"+mybhep::to_string(ievent)+"."+PlotFormat;
    const char *name = sname.c_str();
    canvas->Print(name);

  }


  return;
}

//*************************************************************
void EventDisplay::execute(size_t ievent, CAT::topology::tracked_data & __CAT_tracked_data, SULTAN::topology::tracked_data & __SULTAN_tracked_data){
//*************************************************************

  m.message(" Display Event \n",mybhep::VERBOSE); fflush(stdout);

  set_cells(__CAT_tracked_data.get_cells());
  set_clusters(__CAT_tracked_data.get_clusters());
  CAT_sequences_.clear();
  std::vector<CAT::topology::scenario> CAT_sc = __CAT_tracked_data.get_scenarios();

  if( !SuperNemo )
    {
      set_nemo_sequences(__CAT_tracked_data.get_nemo_sequences() );
    }

  GetPlotLimit( __CAT_tracked_data.get_calos());

  icanvas = 0;

  if( !PlotInitialHits && !PlotTrueTracks
      && !PlotCats && !PlotHelices && !PlotNemoTracks && !PlotSultan ){
    m.message(" problem: nothing to plot", mybhep::NORMAL);
    return;
  }


  if( PlotInitialHits ){
    if( PlotTopView )
      event_display_xz("InitialHits", __CAT_tracked_data);
    if( PlotSideView )
      event_display_yz("InitialHits", __CAT_tracked_data);
  }


  if( PlotTrueTracks ){
    
    if( PlotTopView )
      event_display_xz("true", __CAT_tracked_data);
    if( PlotSideView )
      event_display_yz("true", __CAT_tracked_data);
    
  }
  
  
  if( PlotNemoTracks ){
    
    if( PlotTopView )
      event_display_xz("nemo", __CAT_tracked_data);
    if( PlotSideView )
      event_display_yz("nemo", __CAT_tracked_data);
    
  }
  
  
  if( CAT_sc.size() == 0 ){
    m.message(" warning: CAT_scenarios size is ", CAT_sc.size(), mybhep::VVERBOSE);
  }
  else{
    set_CAT_sequences(CAT_sc[0].sequences());

    if( PlotCats || PlotHelices ){
      if( PlotTopView )
        event_display_xz("cats", __CAT_tracked_data);
      if( PlotSideView )
        event_display_yz("cats", __CAT_tracked_data);
    }

    /*
      if( !SuperNemo ){
      if( PlotTopView )
      Nemo3event_display_xz();
      if( PlotSideView )
      Nemo3event_display_yz();
      }

    */
  }


  canvas->Update();

  if( PlutsMode )
    {
      std::string sname = "figure/event"+mybhep::to_string(ievent)+"."+PlotFormat;
      const char *name = sname.c_str();
      canvas->Print(name);
    }

  if( PlotCellChi2s && PlutsMode ){
    canvas->Clear();

    TPaveText pave_chi2(0.,0.,1.,1.);
    pave_chi2.SetFillColor(4000);
    pave_chi2.SetTextSize(0.03);

    for(size_t i=0; i<CAT_sequences_.size(); i++){
      CAT::topology::sequence s = CAT_sequences_[i];
      size_t nnodes = s.nodes().size();
      std::vector<double> helix_chi2s = s.helix_chi2s();

      for(size_t j=0; j<nnodes; j++)
        {
          size_t cid = s.nodes()[j].c().id();
          std::string title=(std::string)("cell "+mybhep::to_string(cid)+" chi2 "+mybhep::to_string_precision(helix_chi2s[j],"2"));
          pave_chi2.AddText(title.c_str());
        }
    }

    pave_chi2.Draw();

    canvas->Update();

    std::string sname = "figure/chi2_event"+mybhep::to_string(ievent)+"."+PlotFormat;
    const char *name = sname.c_str();
    canvas->Print(name);

  }

  return;
}

//*************************************************************
void EventDisplay::init_quadrant_counters(){
//*************************************************************

  n_cells_quad[0] = 0;
  n_cells_quad[1] = 0;
  n_cells_quad[2] = 0;
  n_cells_quad[3] = 0;

  return;
}

//*************************************************************
size_t EventDisplay::least_occupied_quadrant(){
  //*************************************************************
  size_t min = 1000000000;
  size_t iquad = 0;
  for(size_t i=0; i<4; i++){

    if( n_cells_quad[i] < min) {
      min = n_cells_quad[i];
      iquad = i;
    }
  }

  return iquad;
}

//*************************************************************
void EventDisplay::locate_legend_xz(){
//*************************************************************

  size_t iquad = least_occupied_quadrant();

  double x, z;
  double dz = 0.20;
  double dx = 0.015*CAT_sequences_.size();

  switch(iquad)
    {
    case 0:
      {
        x = 1. - dx; z = 1. - dz;
        break;
      }
    case 1:
      {
        x = 1. - dx; z = 0.;
        break;
      }
    case 2:
      {
        x = 0.; z = 0.;
        break;
      }
    default:
      {
        x = 0.; z = 1. - dz;
      }
    }

  leg_xz->SetX1NDC(z);
  leg_xz->SetX2NDC(z+dz);
  leg_xz->SetY1NDC(x);
  leg_xz->SetY2NDC(x+dx);

  if( PlotTrueTracks ){
    leg_xz_true->SetX1NDC(z);
    leg_xz_true->SetX2NDC(z+dz);
    leg_xz_true->SetY1NDC(x);
    leg_xz_true->SetY2NDC(x+dx);
  }

  return;
}

//*************************************************************
void EventDisplay::locate_legend_yz(){
//*************************************************************

  size_t iquad = least_occupied_quadrant();

  double y, z;
  double dz = 0.20;
  double dy = 0.015*CAT_sequences_.size();

  switch(iquad)
    {
    case 0:
      {
        y = 1. - dy; z = 1. - dz;
        break;
      }
    case 1:
      {
        y = 1. - dy; z = 0.;
        break;
      }
    case 2:
      {
        y = 0.; z = 0.;
        break;
      }
    default:
      {
        y = 0.; z = 1. - dz;
      }
    }

  leg_yz->SetX1NDC(z);
  leg_yz->SetX2NDC(z+dz);
  leg_yz->SetY1NDC(y);
  leg_yz->SetY2NDC(y+dy);

  if( PlotTrueTracks ){
    leg_yz_true->SetX1NDC(z);
    leg_yz_true->SetX2NDC(z+dz);
    leg_yz_true->SetY1NDC(y);
    leg_yz_true->SetY2NDC(y+dy);
  }

  return;
}


//*************************************************************
void EventDisplay::event_display_xz(std::string mode, CAT::topology::tracked_data td){
//*************************************************************

  icanvas ++;
  canvas->cd(icanvas);

  if( PlotLegend ){
    if( mode == "cats" )
      leg_xz->Clear();
    if( PlotTrueTracks && mode == "true" )
      leg_xz_true->Clear();
    init_quadrant_counters();
  }


  std::string title;
  if(mode=="InitialHits")
    title="TOP VIEW (z, x) plane: initial hits";
  else if(mode=="cats")
    title="TOP VIEW (z, x) plane: cats segments";
  else if(mode=="true")
    title="TOP VIEW (z, x) plane: true tracks";


  mapzx->GetXaxis()->SetRangeUser(Zmin, Zmax);
  mapzx->GetYaxis()->SetRangeUser(Xmin, Xmax);
  mapzx->Draw();

  DrawDetectorXZ();

  draw_initial_hits_xz();

  if( PlotCalos )
    draw_calos_xz(td.get_calos());

  if( mode == "InitialHits" && PlotTangents )
    draw_tangents_xz();
  if( mode == "InitialHits" && PlotTriplets )
    draw_triplets_xz();

  if( mode == "nemo" && PlotNemoTracks)
    draw_helices_xz(mode);

  if( mode == "cats" && PlotCats)
    draw_cats_xz(mode, td.get_true_sequences());

  if( mode == "cats" && PlotHelices )
    draw_helices_xz(mode);

  if( mode == "true" && PlotTrueTracks ){
    draw_cats_xz(mode, td.get_true_sequences());
  }


  if( PlotLegend ){
    locate_legend_xz();
    if( mode == "cats" )
      leg_xz->Draw("same");
    if( PlotTrueTracks && mode == "true ")
      leg_xz_true->Draw("same");
  }


  canvas->Update();

  return;

}

//*************************************************************
void EventDisplay::draw_circle_xz( double x0, double z0, double radius, size_t color, size_t thickness = 1, double phi1 = 0., double phi2 = 2.*M_PI ){
//*************************************************************

  double* xc = (double*)malloc(sizeof(double)*100);
  double* zc = (double*)malloc(sizeof(double)*100);

  for(size_t j=0; j<100; j++)
    {
      double angle = phi1 + (phi2 - phi1)*((double)j)/100.;
      xc[j] = x0 + radius*cos(angle);
      zc[j] = z0 + radius*sin(angle);
    }

  TGraph *graph = new TGraph(100,zc,xc);

  graph->SetLineColor(color);
  graph->SetLineWidth(thickness);
  graph->Draw();

  graphlist.push_back(graph);

  free(xc);
  free(zc);

  return;
}

//*************************************************************
void EventDisplay::draw_sine_yz( double y0, double z0, double radius, double pitch, size_t color, size_t thickness = 1, double phi1 = 0., double phi2 = 2.*M_PI  ){
//*************************************************************

  double* yc = (double*)malloc(sizeof(double)*100);
  double* zc = (double*)malloc(sizeof(double)*100);


  if( fabs(phi1 - phi2) > M_PI){
    if( phi2 > phi1 )
      phi2 -= 2.*M_PI;
    else
      phi2 += 2.*M_PI;
  }

  if( phi1 < 0. || phi2 < 0. ){
    phi1 += 2.*acos(-1.);
    phi2 += 2.*acos(-1.);
    y0 -= 2.*acos(-1.)*pitch;
  }

  mybhep::fix_angles(&phi1, &phi2);

  for(size_t j=0; j<100; j++)
    {
      double angle = phi1 + (phi2 - phi1)*((double)j)/100.;
      yc[j] = y0 + pitch*angle;
      zc[j] = z0 + radius*sin(angle);
    }

  TGraph *graph = new TGraph(100,zc,yc);

  graph->SetLineColor(color);
  graph->SetLineWidth(thickness);
  graph->Draw();

  graphlist.push_back(graph);

  free(yc);
  free(zc);

  return;
}

//*************************************************************
int EventDisplay::quadrant_xz( CAT::topology::cell c ){
//*************************************************************


  double x = get_x_ndc(c.ep().x().value());
  double z = get_z_ndc(c.ep().z().value());

  if( z > 0.5 && x > 0.5) return 0;
  if( z < 0.5 && x > 0.5) return 1;
  if( z < 0.5 && x < 0.5) return 2;

  return 3;

}

//*************************************************************
int EventDisplay::quadrant_yz( CAT::topology::cell c ){
//*************************************************************


  double y = get_y_ndc(c.ep().y().value());
  double z = get_z_ndc(c.ep().z().value());

  if( z > 0.5 && y > 0.5) return 0;
  if( z < 0.5 && y > 0.5) return 1;
  if( z < 0.5 && y < 0.5) return 2;

  return 3;

}

//*************************************************************
void EventDisplay::draw_initial_hits_xz( void ){
//*************************************************************


  size_t npoints = cells_.size();

  double* x = (double*)malloc(sizeof(double)*npoints);
  double* z = (double*)malloc(sizeof(double)*npoints);
  double* r = (double*)malloc(sizeof(double)*npoints);

  size_t index = 0;
  for(size_t i=0; i< npoints; i++)  // loop on hits
    {
      CAT::topology::cell c = cells_[i];

      x[index] = c.ep().x().value();
      z[index] = c.ep().z().value();
      r[index] = c.r().value();

      draw_circle_xz(x[index], z[index], r[index], color_fast_cells, 2);

      if( PlotCellIds ){
        TPaveText *pt = new TPaveText(
                                      get_z_ndc(z[index])-0.01,
                                      get_x_ndc(x[index])-0.01,
                                      get_z_ndc(z[index])+0.01,
                                      get_x_ndc(x[index])+0.01,
                                      "NDC");
        pt->SetFillColor(4000);
        pt->SetTextSize(0.02);
        //       pt->SetTextAlign(12);

        std::string title=mybhep::to_string(c.id());
        pt->SetTextColor(color_cells);
        pt->AddText(title.c_str());
        pt->Draw("same");

        pavelist.push_back(pt);

        n_cells_quad[quadrant_xz(c)] ++;

      }

      index++;
    }//end of loop over hits

  free(x);
  free(z);
  free(r);

  return;
}

//*************************************************************
int EventDisplay::getCalWalli( CAT::topology::calorimeter_hit h ){
//*************************************************************

  m.message(" problem: requesting calo wall", mybhep::NORMAL);

  return 1;

}


//*************************************************************
void EventDisplay::draw_calos_xz( std::vector<CAT::topology::calorimeter_hit> calos ){
//*************************************************************


  size_t npoints = calos.size();

  for(size_t i=0; i< npoints; i++)  // plotting calorimeter hits
    {
      CAT::topology::calorimeter_hit h = calos[i];

      CAT::topology::experimental_vector sizes = h.pl().sizes();
      double local_calo_x = sizes.x().value();
      double local_calo_z = sizes.z().value();

      FirstStarXY = new TMarker( h.pl().center().z().value(),
                                 h.pl().center().x().value(),
                                 3);
      FirstStarXY->Draw();


      double* zcal = (double*)malloc(sizeof(double)*5);
      double* xcal = (double*)malloc(sizeof(double)*5);

      if( SuperNemo ){

        zcal[0] = h.pl().center().z().value() - local_calo_z/2.;
        zcal[1] = h.pl().center().z().value() + local_calo_z/2.;
        zcal[2] = h.pl().center().z().value() + local_calo_z/2.;
        zcal[3] = h.pl().center().z().value() - local_calo_z/2.;
        zcal[4] = h.pl().center().z().value() - local_calo_z/2.;

        xcal[0] = h.pl().center().x().value() - local_calo_x/2.;
        xcal[1] = h.pl().center().x().value() - local_calo_x/2.;
        xcal[2] = h.pl().center().x().value() + local_calo_x/2.;
        xcal[3] = h.pl().center().x().value() + local_calo_x/2.;
        xcal[4] = h.pl().center().x().value() - local_calo_x/2.;
      }
      else{

        double theta = atan2(h.pl().center().z().value(), h.pl().center().x().value());

        double offset = 0.;
        //int wall = getCalWalli(h);
        //if( wall == 6 ) offset = -local_calo_x/2.;  // inner
        //else if( wall == 7 ) offset = local_calo_x/2.;  // outer

        zcal[0] = h.pl().center().z().value() + offset*sin(theta) - local_calo_x/2.*sin(theta) + local_calo_z/2.*cos(theta);
        zcal[1] = h.pl().center().z().value() + offset*sin(theta) + local_calo_x/2.*sin(theta) + local_calo_z/2.*cos(theta);
        zcal[2] = h.pl().center().z().value() + offset*sin(theta) + local_calo_x/2.*sin(theta) - local_calo_z/2.*cos(theta);
        zcal[3] = h.pl().center().z().value() + offset*sin(theta) - local_calo_x/2.*sin(theta) - local_calo_z/2.*cos(theta);
        zcal[4] = h.pl().center().z().value() + offset*sin(theta) - local_calo_x/2.*sin(theta) + local_calo_z/2.*cos(theta);

        xcal[0] = h.pl().center().x().value() + offset*cos(theta) - local_calo_x/2.*cos(theta) - local_calo_z/2.*sin(theta);
        xcal[1] = h.pl().center().x().value() + offset*cos(theta) + local_calo_x/2.*cos(theta) - local_calo_z/2.*sin(theta);
        xcal[2] = h.pl().center().x().value() + offset*cos(theta) + local_calo_x/2.*cos(theta) + local_calo_z/2.*sin(theta);
        xcal[3] = h.pl().center().x().value() + offset*cos(theta) - local_calo_x/2.*cos(theta) + local_calo_z/2.*sin(theta);
        xcal[4] = h.pl().center().x().value() + offset*cos(theta) - local_calo_x/2.*cos(theta) - local_calo_z/2.*sin(theta);

      }


      TGraph *graphcal = new TGraph(5,zcal,xcal);
      graphcal->SetLineColor(color_calos);
      graphcal->Draw();
      graphlist.push_back(graphcal);


      if( PlotCaloEnergy ){
        TPaveText *pt = new TPaveText(
                                      get_z_ndc((zcal[0] + zcal[2])/2.)-0.01,
                                      get_x_ndc((xcal[0] + xcal[2])/2.)-0.05,
                                      get_z_ndc((zcal[0] + zcal[2])/2.)+0.01,
                                      get_x_ndc((xcal[0] + xcal[2])/2.)+0.05,
                                      "NDC");
        pt->SetFillColor(4000);
        pt->SetTextColor(color_calos);
        pt->SetTextSize(0.03);
        //      pt->SetMargin(0.3);
        //       pt->SetTextAlign(12);

        std::string en="E = "+mybhep::to_string(mybhep::round_to(h.e().value(),2))+" MeV";
        pt->AddText(en.c_str());
        std::string ti="t = "+mybhep::to_string(mybhep::round_to(h.t().value(),2))+" ns";
        pt->AddText(ti.c_str());
        pt->Draw("same");

        pavelist.push_back(pt);
      }


      free(zcal);
      free(xcal);

    }



  return;
}


//*************************************************************
void EventDisplay::draw_calos_yz( std::vector<CAT::topology::calorimeter_hit> calos ){
//*************************************************************

  size_t npoints = calos.size();
  for(size_t i=0; i< npoints; i++)  // plotting calorimeter hits
    {
      CAT::topology::calorimeter_hit h = calos[i];

      CAT::topology::experimental_vector sizes = h.pl().sizes();
      double local_calo_x = sizes.x().value();
      double local_calo_y = sizes.y().value();
      double local_calo_z = sizes.z().value();

      FirstStarXY = new TMarker( h.pl().center().z().value(),
                                 h.pl().center().y().value(),
                                 3);
      FirstStarXY->Draw();

      double* zcal = (double*)malloc(sizeof(double)*5);
      double* ycal = (double*)malloc(sizeof(double)*5);

      double offsetY = 0.;

      if( !SuperNemo ){

        int wall = getCalWalli(h);
        if( wall == 8 ) offsetY = -local_calo_y/2.;  // bottom
        else if( wall == 9 ) offsetY = local_calo_y/2.;  // top

      }

      if( SuperNemo ){

        zcal[0] = h.pl().center().z().value() - local_calo_z/2.;
        zcal[1] = h.pl().center().z().value() + local_calo_z/2.;
        zcal[2] = h.pl().center().z().value() + local_calo_z/2.;
        zcal[3] = h.pl().center().z().value() - local_calo_z/2.;
        zcal[4] = h.pl().center().z().value() - local_calo_z/2.;
      }
      else{

        double theta = atan2(h.pl().center().z().value(), h.pl().center().x().value());

        double offset = 0.;
        int wall = getCalWalli(h);
        if( wall == 6 ) offset = -local_calo_x/2.;  // inner
        else if( wall == 7 ) offset = local_calo_x/2.;  // outer

        zcal[0] = h.pl().center().z().value() + offset*sin(theta) - local_calo_x/2.*fabs(sin(theta)) - local_calo_z/2.*fabs(cos(theta));
        zcal[1] = h.pl().center().z().value() + offset*sin(theta) + local_calo_x/2.*fabs(sin(theta)) + local_calo_z/2.*fabs(cos(theta));
        zcal[2] = h.pl().center().z().value() + offset*sin(theta) + local_calo_x/2.*fabs(sin(theta)) + local_calo_z/2.*fabs(cos(theta));
        zcal[3] = h.pl().center().z().value() + offset*sin(theta) - local_calo_x/2.*fabs(sin(theta)) - local_calo_z/2.*fabs(cos(theta));
        zcal[4] = h.pl().center().z().value() + offset*sin(theta) - local_calo_x/2.*fabs(sin(theta)) - local_calo_z/2.*fabs(cos(theta));


      }

      ycal[0] = h.pl().center().y().value() + offsetY - local_calo_y/2.;
      ycal[1] = h.pl().center().y().value() + offsetY - local_calo_y/2.;
      ycal[2] = h.pl().center().y().value() + offsetY + local_calo_y/2.;
      ycal[3] = h.pl().center().y().value() + offsetY + local_calo_y/2.;
      ycal[4] = h.pl().center().y().value() + offsetY - local_calo_y/2.;


      TGraph *graphcal = new TGraph(5,zcal,ycal);
      graphcal->SetLineColor(color_calos);
      graphcal->Draw();
      graphlist.push_back(graphcal);

      if( PlotCaloEnergy ){
        TPaveText *pt = new TPaveText(
                                      get_z_ndc((zcal[0] + zcal[2])/2.)-0.01,
                                      get_y_ndc((ycal[0] + ycal[2])/2.)-0.05,
                                      get_z_ndc((zcal[0] + zcal[2])/2.)+0.01,
                                      get_y_ndc((ycal[0] + ycal[2])/2.)+0.05,
                                      "NDC");
        pt->SetFillColor(4000);
        pt->SetTextColor(color_calos);
        pt->SetTextSize(0.03);
        //      pt->SetMargin(0.3);
        //       pt->SetTextAlign(12);

        std::string en="E = "+mybhep::to_string(mybhep::round_to(h.e().value(),2))+" MeV";
        pt->AddText(en.c_str());
        std::string ti="t = "+mybhep::to_string(mybhep::round_to(h.t().value(),2))+" ns";
        pt->AddText(ti.c_str());
        pt->Draw("same");

        pavelist.push_back(pt);
      }


      free(zcal);
      free(ycal);

    }

  return;




}


//*************************************************************
void EventDisplay::draw_tangents_xz( void ){
//*************************************************************

  for(size_t iclu=0; iclu<clusters_.size(); iclu++){

    std::vector<CAT::topology::node> nodes = clusters_[iclu].nodes();
    for(size_t inode=0; inode<nodes.size(); inode++){
      std::vector<CAT::topology::cell_couplet> ccs = nodes[inode].cc();
      for(size_t icc=0; icc<ccs.size(); icc++){
        std::vector<CAT::topology::line>  tangents = ccs[icc].tangents();
        for(size_t itang=0; itang<tangents.size(); itang++){
          CAT::topology::line l = tangents[itang];

          double* ztan = (double*)malloc(sizeof(double)*2);
          double* xtan = (double*)malloc(sizeof(double)*2);

          ztan[0] = l.epa().z().value();
          ztan[1] = l.epb().z().value();

          xtan[0] = l.epa().x().value();
          xtan[1] = l.epb().x().value();

          TGraph *graphtan = new TGraph(2,ztan,xtan);
          graphtan->SetLineColor(color_tangent);
          graphtan->Draw();
          graphlist.push_back(graphtan);

          free(ztan);
          free(xtan);
        }
      }
    }
  }


  return;
}

//*************************************************************
void EventDisplay::draw_helices_xz(std::string mode){
//*************************************************************

  if( mode == "cats" ){

    for(std::vector<CAT::topology::sequence>::iterator iclu=CAT_sequences_.begin(); iclu != CAT_sequences_.end(); ++iclu){

      if( iclu->nodes_.size() <= 2 ){
        continue;
      }

      double x = iclu->center().x().value();
      double z = iclu->center().z().value();
      double r = iclu->radius().value();
      CAT::topology::experimental_point pi = iclu->nodes().front().ep();
      if( iclu->has_helix_vertex() ){
        const CAT::topology::experimental_point v = iclu->helix_vertex();
        pi = v;
      }
      CAT::topology::experimental_point pf = iclu->nodes().back().ep();
      if( iclu->has_decay_helix_vertex() )
        pf = iclu->decay_helix_vertex();
      double phi1 = CAT::topology::experimental_vector(iclu->center(), pi).phi().value();
      double phi2 = CAT::topology::experimental_vector(iclu->center(), pf).phi().value();
      mybhep::fix_angles(&phi1, &phi2);
      draw_circle_xz(x, z, r, color_helix + (iclu - CAT_sequences_.begin()), 2, phi1, phi2);

    }
  } else if( mode == "nemo" ){

    for(std::vector<CAT::topology::sequence>::iterator iclu=nemo_sequences_.begin(); iclu != nemo_sequences_.end(); ++iclu){

      if( iclu->nodes_.size() <= 2 ){
        continue;
      }

      double x = iclu->center().x().value();
      double z = iclu->center().z().value();
      double r = iclu->radius().value();
      CAT::topology::experimental_point pi = iclu->nodes().front().ep();
      if( iclu->has_helix_vertex() ){
        const CAT::topology::experimental_point v = iclu->helix_vertex();
        pi = v;
      }
      CAT::topology::experimental_point pf = iclu->nodes().back().ep();
      if( iclu->has_decay_helix_vertex() )
        pf = iclu->decay_helix_vertex();
      double phi1 = CAT::topology::experimental_vector(iclu->center(), pi).phi().value();
      double phi2 = CAT::topology::experimental_vector(iclu->center(), pf).phi().value();
      mybhep::fix_angles(&phi1, &phi2);
      draw_circle_xz(x, z, r, color_helix + (iclu - nemo_sequences_.begin()), 2, phi1, phi2);

    }

  }


  return;
}



//*************************************************************
void EventDisplay::draw_helices_yz(std::string mode){
//*************************************************************

  if( mode == "cats" ){

    for(std::vector<CAT::topology::sequence>::iterator iclu=CAT_sequences_.begin(); iclu != CAT_sequences_.end(); ++iclu){
      double y = iclu->center().y().value();
      double z = iclu->center().z().value();
      double r = iclu->radius().value();
      double p = iclu->pitch().value();
      CAT::topology::experimental_point pi = iclu->nodes().front().ep();
      if( iclu->has_helix_vertex() )
        pi = iclu->helix_vertex();
      CAT::topology::experimental_point pf = iclu->nodes().back().ep();
      if( iclu->has_decay_helix_vertex() )
        pf = iclu->decay_helix_vertex();
      CAT::topology::helix h = iclu->get_helix();
      double phi1 = h.phi_of_point(pi).value();
      double phi2 = h.phi_of_point(pf).value();
      draw_sine_yz(y, z, r, p, color_helix + (iclu - CAT_sequences_.begin()), 2, phi1, phi2);
    }

  } else if( mode == "nemo" ){

    for(std::vector<CAT::topology::sequence>::iterator iclu=nemo_sequences_.begin(); iclu != nemo_sequences_.end(); ++iclu){
      double y = iclu->center().y().value();
      double z = iclu->center().z().value();
      double r = iclu->radius().value();
      double p = iclu->pitch().value();
      CAT::topology::experimental_point pi = iclu->nodes().front().ep();
      if( iclu->has_helix_vertex() )
        pi = iclu->helix_vertex();
      CAT::topology::experimental_point pf = iclu->nodes().back().ep();
      if( iclu->has_decay_helix_vertex() )
        pf = iclu->decay_helix_vertex();
      CAT::topology::helix h = iclu->get_helix();
      double phi1 = h.phi_of_point(pi).value();
      double phi2 = h.phi_of_point(pf).value();
      draw_sine_yz(y, z, r, p, color_helix + (iclu - nemo_sequences_.begin()), 2, phi1, phi2);
    }

  }

  return;
}




//*************************************************************
void EventDisplay::draw_triplets_xz( void ){
//*************************************************************

  for(size_t iclu=0; iclu<clusters_.size(); iclu++){

    std::vector<CAT::topology::node> nodes = clusters_[iclu].nodes();
    for(size_t inode=0; inode<nodes.size(); inode++){
      std::vector<CAT::topology::cell_triplet> ccs = nodes[inode].ccc();
      for(size_t icc=0; icc<ccs.size(); icc++){
        std::vector<CAT::topology::joint>  joints = ccs[icc].joints();
        for(size_t itang=0; itang<joints.size(); itang++){
          CAT::topology::joint j = joints[itang];

          double* ztan = (double*)malloc(sizeof(double)*3);
          double* xtan = (double*)malloc(sizeof(double)*3);

          ztan[0] = j.epa().z().value();
          ztan[1] = j.epb().z().value();
          ztan[2] = j.epc().z().value();

          xtan[0] = j.epa().x().value();
          xtan[1] = j.epb().x().value();
          xtan[2] = j.epc().x().value();

          TGraph *graphtan = new TGraph(3,ztan,xtan);
          graphtan->SetLineColor(color_triplet);
          graphtan->Draw();
          graphlist.push_back(graphtan);

          free(ztan);
          free(xtan);
        }
      }
    }
  }


  return;
}


//*************************************************************
void EventDisplay::draw_tangents_yz( void ){
//*************************************************************

  for(size_t iclu=0; iclu<clusters_.size(); iclu++){

    std::vector<CAT::topology::node> nodes = clusters_[iclu].nodes();
    for(size_t inode=0; inode<nodes.size(); inode++){
      std::vector<CAT::topology::cell_couplet> ccs = nodes[inode].cc();
      for(size_t icc=0; icc<ccs.size(); icc++){
        std::vector<CAT::topology::line>  tangents = ccs[icc].tangents();
        for(size_t itripl=0; itripl<tangents.size(); itripl++){
          CAT::topology::line l = tangents[itripl];

          double* ztan = (double*)malloc(sizeof(double)*2);
          double* ytan = (double*)malloc(sizeof(double)*2);

          ztan[0] = l.epa().z().value();
          ztan[1] = l.epb().z().value();

          ytan[0] = l.epa().y().value();
          ytan[1] = l.epb().y().value();

          TGraph *graphtan = new TGraph(2,ztan,ytan);
          graphtan->SetLineColor(color_tangent);
          graphtan->Draw();
          graphlist.push_back(graphtan);

          free(ztan);
          free(ytan);
        }
      }
    }
  }


  return;
}


//*************************************************************
void EventDisplay::draw_triplets_yz( void ){
//*************************************************************

  for(size_t iclu=0; iclu<clusters_.size(); iclu++){

    std::vector<CAT::topology::node> nodes = clusters_[iclu].nodes();
    for(size_t inode=0; inode<nodes.size(); inode++){
      std::vector<CAT::topology::cell_triplet> ccs = nodes[inode].ccc();
      for(size_t icc=0; icc<ccs.size(); icc++){
        std::vector<CAT::topology::joint>  joints = ccs[icc].joints();
        for(size_t itripl=0; itripl<joints.size(); itripl++){
          CAT::topology::joint j = joints[itripl];

          double* ztan = (double*)malloc(sizeof(double)*3);
          double* ytan = (double*)malloc(sizeof(double)*3);

          ztan[0] = j.epa().z().value();
          ztan[1] = j.epb().z().value();
          ztan[2] = j.epc().z().value();

          ytan[0] = j.epa().y().value();
          ytan[1] = j.epb().y().value();
          ytan[2] = j.epc().y().value();

          TGraph *graphtan = new TGraph(3,ztan,ytan);
          graphtan->SetLineColor(color_triplet);
          graphtan->Draw();
          graphlist.push_back(graphtan);

          free(ztan);
          free(ytan);
        }
      }
    }
  }


  return;
}



//*************************************************************
void EventDisplay::draw_cats_xz(std::string mode, std::vector<CAT::topology::sequence> true_seqs ){
//*************************************************************

  if( mode == "true" ){

    for(size_t i=0; i<true_seqs.size(); i++)
      {

        CAT::topology::sequence s = true_seqs[i];

        if( !s.primary() ) continue;

        size_t color = color_true_track;

        size_t nnodes = s.nodes().size();
        if( nnodes == 0 ) continue;

        size_t npoints = nnodes;
        size_t offset = 0;


        if( s.has_tangent_vertex() )
          {
            npoints += 1;  // vertex
            offset = 1;
          }
        if( s.has_decay_tangent_vertex() )
          {
            npoints += 1;  // decay vertex
          }


        double* zt = (double*)malloc(sizeof(double)*npoints);
        double* xt = (double*)malloc(sizeof(double)*npoints);

        if( s.has_tangent_vertex() )
          {
            xt[0] = s.tangent_vertex().x().value();
            zt[0] = s.tangent_vertex().z().value();
          }

        if( s.has_decay_tangent_vertex() )
          {
            xt[npoints-1] = s.decay_tangent_vertex().x().value();
            zt[npoints-1] = s.decay_tangent_vertex().z().value();
          }

        for(size_t j=0; j<nnodes; j++)
          {

            CAT::topology::experimental_point p = s.nodes()[j].ep();


            xt[j+offset] = p.x().value();
            zt[j+offset] = p.z().value();

          }

        TGraph *graph = new TGraph(npoints,zt,xt);
        graph->Draw();
        graph->SetLineColor(color + i);
        graph->SetLineWidth(2);
        graphlist.push_back(graph);
        if( PlotLegend )
          leg_xz_true->AddEntry(graph, s.name().c_str(),"l");
        free(xt);
        free(zt);

      }

  }
  else if( mode == "cats" ){
    for(size_t i=0; i<CAT_sequences_.size(); i++)
      {
        CAT::topology::sequence s = CAT_sequences_[i];

        if( s.name() == "LOST" ) continue;

        size_t color = color_cats;

        size_t nnodes = s.nodes().size();
        if( nnodes == 0 ) continue;

        size_t npoints = nnodes;
        size_t offset = 0;


        if( s.has_tangent_vertex() )
          {
            npoints += 1;  // vertex
            offset = 1;
          }
        if( s.has_decay_tangent_vertex() )
          {
            npoints += 1;  // decay vertex
          }


        double* zt = (double*)malloc(sizeof(double)*npoints);
        double* xt = (double*)malloc(sizeof(double)*npoints);

        if( s.has_tangent_vertex() )
          {
            xt[0] = s.tangent_vertex().x().value();
            zt[0] = s.tangent_vertex().z().value();
          }

        if( s.has_decay_tangent_vertex() )
          {
            xt[npoints-1] = s.decay_tangent_vertex().x().value();
            zt[npoints-1] = s.decay_tangent_vertex().z().value();
          }

        std::vector<double> helix_chi2s = s.helix_chi2s();

        for(size_t j=0; j<nnodes; j++)
          {

            CAT::topology::experimental_point p = s.nodes()[j].ep();

            xt[j+offset] = p.x().value();
            zt[j+offset] = p.z().value();

          }

        TGraph *graph = new TGraph(npoints,zt,xt);
        graph->Draw();
        graph->SetLineColor(color + i);
        graph->SetLineWidth(2);
        if( PlotLegend )
          leg_xz->AddEntry(graph, s.name().c_str(),"l");
        graphlist.push_back(graph);


        free(xt);
        free(zt);


      }
  }
  else if( mode == "nemo" ){

    for(size_t i=0; i<nemo_sequences_.size(); i++)
      {
        CAT::topology::sequence s = nemo_sequences_[i];

        size_t color = color_nemora;

        size_t nnodes = s.nodes().size();
        if( nnodes == 0 ) continue;

        size_t npoints = nnodes;
        size_t offset = 0;

        bool do_vertex = false;

        if( s.has_helix_vertex() &&
            ! (fabs(s.helix_vertex().x().value()) < 0.1 && fabs(s.helix_vertex().z().value() < 0.1 )) )

          do_vertex = true;

        if( do_vertex ){
          npoints += 1;  // vertex
          offset = 1;
          }
        if( s.has_decay_helix_vertex() )
          {
            npoints += 1;  // decay vertex
          }


        double* zt = (double*)malloc(sizeof(double)*npoints);
        double* xt = (double*)malloc(sizeof(double)*npoints);

        if( do_vertex )
          {
            xt[0] = s.helix_vertex().x().value();
            zt[0] = s.helix_vertex().z().value();

          }

        if( s.has_decay_helix_vertex() )
          {
            xt[npoints-1] = s.decay_helix_vertex().x().value();
            zt[npoints-1] = s.decay_helix_vertex().z().value();

          }

        for(size_t j=0; j<nnodes; j++)
          {

            CAT::topology::experimental_point p = s.nodes()[j].ep();


            xt[j+offset] = p.x().value();
            zt[j+offset] = p.z().value();

          }

        TGraph *graph = new TGraph(npoints,zt,xt);
        graph->Draw();
        graph->SetLineColor(color + i);
        graph->SetLineWidth(2);
        if( PlotLegend )
          leg_xz->AddEntry(graph, s.name().c_str(),"l");
        graphlist.push_back(graph);


        free(xt);
        free(zt);


      }
  }

  /*
  for(size_t i=0; i<calo_position.size(); i++)
    {

          FirstStarXY_used = true;
      FirstStarXY = new TMarker( calo_position[i].pos[2],
                                 calo_position[i].pos[0],
                                 5);

      FirstStarXY->Draw();

    }
  */


  return;

}

//*************************************************************
void EventDisplay::event_display_yz(std::string mode, CAT::topology::tracked_data td){
//*************************************************************

  icanvas ++;
  canvas->cd(icanvas);

  if( PlotLegend ){
    init_quadrant_counters();
    if( mode == "cats" )
      leg_yz->Clear();
    if( mode == "true" && PlotTrueTracks )
      leg_yz_true->Clear();
  }

  std::string title;
  if(mode=="InitialHits")
    title="SIDE VIEW (z, y) plane: initial hits";
  else if(mode=="cats")
    title="SIDE VIEW (z, y) plane: cats segments";
  else if(mode=="true")
    title="SIDE VIEW (z, y) plane: true tracks";

  mapzy->GetYaxis()->SetRangeUser(Ymin, Ymax);
  mapzy->GetXaxis()->SetRangeUser(Zmin, Zmax);
  mapzy->Draw();

  DrawDetectorYZ();

  draw_initial_hits_yz( );

  if( PlotCalos )
    draw_calos_yz(td.get_calos());

  if( mode == "InitialHits" && PlotTangents )
    draw_tangents_yz();
  if( mode == "InitialHits" && PlotTriplets )
    draw_triplets_yz();

  if( mode == "cats" && PlotHelices )
    draw_helices_yz(mode);

  if( mode == "nemo" && PlotNemoTracks)
    draw_helices_yz(mode);

  if( mode == "cats" && PlotCats)
    draw_cats_yz(mode, td.get_true_sequences());

  if( mode == "true" && PlotTrueTracks )
    draw_cats_yz(mode, td.get_true_sequences());

  if( PlotLegend ){
    locate_legend_yz();
    if( mode == "cats" )
      leg_yz->Draw("same");
    if( mode == "true" && PlotTrueTracks )
      leg_yz_true->Draw("same");
  }
  canvas->Update();

  return;

}

//*************************************************************
void EventDisplay::draw_initial_hits_yz( void ){
//*************************************************************

  size_t npoints = cells_.size();

  for(size_t i=0; i< npoints; i++)  // loop on hits
    {
      CAT::topology::cell c = cells_[i];

      double* y = (double*)malloc(sizeof(double)*7);
      double* z = (double*)malloc(sizeof(double)*7);

      y[0] = c.ep().y().value();
      y[1] = c.ep().y().value() + c.ep().y().error();
      y[2] = y[1];
      y[3] = c.ep().y().value() - c.ep().y().error();
      y[4] = y[3];
      y[5] = y[0];
      y[6] = y[0];

      z[0] = c.ep().z().value() - c.r().value();
      z[1] = z[0];
      z[2] = c.ep().z().value() + c.r().value();
      z[3] = z[2];
      z[4] = z[0];
      z[5] = z[0];
      z[6] = z[2];

      TGraph *graph = new TGraph(7,z,y);
      graph->SetLineColor(color_fast_cells);
      graph->SetLineWidth(2);
      graph->Draw();
      graphlist.push_back(graph);

      if( PlotCellIds ){
        TPaveText *pt = new TPaveText(
                                      get_z_ndc(c.ep().z().value())-0.01,
                                      get_y_ndc(y[0])-0.01,
                                      get_z_ndc(c.ep().z().value())+0.01,
                                      get_y_ndc(y[0])+0.01,
                                      "NDC");
        pt->SetFillColor(4000);
        pt->SetTextSize(0.02);
        //       pt->SetTextAlign(12);

        std::string title=mybhep::to_string(c.id());
        pt->SetTextColor(color_cells);
        pt->AddText(title.c_str());
        pt->Draw("same");

        pavelist.push_back(pt);
      }


      n_cells_quad[quadrant_yz(c)] ++;

      free(y);
      free(z);
    }//end of loop over hits


  return;
}

//*************************************************************
void EventDisplay::draw_cats_yz(std::string mode, std::vector<CAT::topology::sequence> true_seqs ){
//*************************************************************

  if( mode == "true" ){

    for(size_t i=0; i<true_seqs.size(); i++)
      {
        CAT::topology::sequence s = true_seqs[i];

        if( !s.primary() ) continue;

        size_t color = color_true_track;

        size_t nnodes = s.nodes().size();
        if( nnodes == 0 ) continue;

        size_t npoints = nnodes;
        size_t offset = 0;

        if( s.has_tangent_vertex() )
          {
            npoints += 1;  // vertex
            offset = 1;
          }
        if( s.has_decay_tangent_vertex() )
          {
            npoints += 1;  // decay vertex
          }

        double* zt = (double*)malloc(sizeof(double)*npoints);
        double* yt = (double*)malloc(sizeof(double)*npoints);

        if( s.has_tangent_vertex() )
          {
            yt[0] = s.tangent_vertex().y().value();
            zt[0] = s.tangent_vertex().z().value();
          }

        if( s.has_decay_tangent_vertex() )
          {
            yt[npoints-1] = s.decay_tangent_vertex().y().value();
            zt[npoints-1] = s.decay_tangent_vertex().z().value();
          }

        for(size_t j=0; j<nnodes; j++)
          {

            CAT::topology::experimental_point p = s.nodes()[j].ep();

            yt[j+offset] = p.y().value();
            zt[j+offset] = p.z().value();

          }

        TGraph *graph = new TGraph(npoints,zt,yt);
        graph->Draw();
        graph->SetLineColor(color + i);
        graph->SetLineWidth(2);
        graphlist.push_back(graph);
        if( PlotLegend )
          leg_yz_true->AddEntry(graph, s.name().c_str(),"l");

        free(yt);
        free(zt);

      }

  }
  else if( mode == "cats" ){
    for(size_t i=0; i<CAT_sequences_.size(); i++)
      {
        CAT::topology::sequence s = CAT_sequences_[i];

        if( s.name() == "LOST" ) continue;

        size_t color = color_cats;

        size_t nnodes = s.nodes().size();
        if( nnodes == 0 ) continue;

        size_t npoints = nnodes;
        size_t offset = 0;

        if( s.has_tangent_vertex() )
          {
            npoints += 1;  // vertex
            offset = 1;
          }
        if( s.has_decay_tangent_vertex() )
          {
            npoints += 1;  // decay vertex
          }

        double* zt = (double*)malloc(sizeof(double)*npoints);
        double* yt = (double*)malloc(sizeof(double)*npoints);

        if( s.has_tangent_vertex() )
          {
            yt[0] = s.tangent_vertex().y().value();
            zt[0] = s.tangent_vertex().z().value();
          }

        if( s.has_decay_tangent_vertex() )
          {
            yt[npoints-1] = s.decay_tangent_vertex().y().value();
            zt[npoints-1] = s.decay_tangent_vertex().z().value();
          }


        for(size_t j=0; j<nnodes; j++)
          {

            CAT::topology::experimental_point p = s.nodes()[j].ep();

            color = color_cats;

            yt[j+offset] = p.y().value();
            zt[j+offset] = p.z().value();

          }

        TGraph *graph = new TGraph(npoints,zt,yt);
        graph->Draw();
        graph->SetLineColor(color + i);
        graph->SetLineWidth(2);
        if( PlotLegend )
          leg_yz->AddEntry(graph, s.name().c_str(),"l");
        graphlist.push_back(graph);

        free(yt);
        free(zt);

      }
  }
  else if( mode == "nemo" ){
    for(size_t i=0; i<nemo_sequences_.size(); i++)
      {
        CAT::topology::sequence s = nemo_sequences_[i];

        if( s.name() == "LOST" ) continue;

        size_t color = color_nemora;

        size_t nnodes = s.nodes().size();
        if( nnodes == 0 ) continue;

        size_t npoints = nnodes;
        size_t offset = 0;

        bool do_vertex = false;

        if( s.has_helix_vertex() &&
            ! (fabs(s.helix_vertex().x().value()) < 0.1 && fabs(s.helix_vertex().z().value() < 0.1 )) )

          do_vertex = true;

        if( do_vertex )
          {
            npoints += 1;  // vertex
            offset = 1;
          }
        if( s.has_decay_helix_vertex() )
          {
            npoints += 1;  // decay vertex
          }

        double* zt = (double*)malloc(sizeof(double)*npoints);
        double* yt = (double*)malloc(sizeof(double)*npoints);

        if( do_vertex )
          {
            yt[0] = s.helix_vertex().y().value();
            zt[0] = s.helix_vertex().z().value();
          }

        if( s.has_decay_helix_vertex() )
          {
            yt[npoints-1] = s.decay_helix_vertex().y().value();
            zt[npoints-1] = s.decay_helix_vertex().z().value();
          }

        for(size_t j=0; j<nnodes; j++)
          {

            CAT::topology::experimental_point p = s.nodes()[j].ep();

            yt[j+offset] = p.y().value();
            zt[j+offset] = p.z().value();

          }

        TGraph *graph = new TGraph(npoints,zt,yt);
        graph->Draw();
        graph->SetLineColor(color + i);
        graph->SetLineWidth(2);
        if( PlotLegend )
          leg_yz->AddEntry(graph, s.name().c_str(),"l");
        graphlist.push_back(graph);

        free(yt);
        free(zt);

      }
  }


  return;

}


void EventDisplay::print_cells(void){

  for(size_t icell=0; icell<cells_.size(); icell++){
    cells_[icell].dump();
  }

  return;
}


double EventDisplay::get_z_ndc(double z){

  double factor = 0.1;
  double zmin = Zmin - factor*fabs(Zmin);
  double zmax = Zmax + factor*fabs(Zmin);

  return (z - zmin)/(zmax - zmin);

}


double EventDisplay::get_y_ndc(double y){

  double factor = 0.1;
  double ymin = Ymin - factor*fabs(Ymin);
  double ymax = Ymax + factor*fabs(Ymin);

  return (y - ymin)/(ymax - ymin);


}

double EventDisplay::get_x_ndc(double x){

  double factor = 0.1;
  double xmin = Xmin - factor*fabs(Xmin);
  double xmax = Xmax + factor*fabs(Xmin);

  return (x - xmin)/(xmax - xmin);


}
