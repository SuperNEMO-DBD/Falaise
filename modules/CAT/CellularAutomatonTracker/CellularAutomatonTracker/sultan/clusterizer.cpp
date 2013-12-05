#include <sultan/clusterizer.h>
#include <mybhep/system_of_units.h>
#include <sys/time.h>
#include <limits>
#include <cmath>
#include <map>

namespace SULTAN {
  using namespace mybhep;
  using namespace std;

  //! get cells
  const std::vector<topology::cell>& clusterizer::get_cells()const
  {
    return cells_;
  }

  //! set cells
  void clusterizer::set_cells(const std::vector<topology::cell> & cells)
  {
    //cells_.clear();
    cells_ = cells;
  }

  //! get clusters
  const std::vector<topology::cluster>& clusterizer::get_clusters()const
  {
    return clusters_;
  }

  //! set clusters
  void clusterizer::set_clusters(const std::vector<topology::cluster> & clusters)
  {
    //clusters_.clear();
    clusters_ = clusters;
  }

  void clusterizer::_set_defaults ()
  {

    level = mybhep::NORMAL;
    m = mybhep::messenger(level);
    num_blocks = -1;
    planes_per_block.clear ();
    gaps_Z.clear ();
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
    cell_distance  = std::numeric_limits<double>::quiet_NaN ();
    xsize = ysize = zsize = std::numeric_limits<double>::quiet_NaN ();
    calo_X = calo_Y = calo_Z = std::numeric_limits<double>::quiet_NaN ();
    inner_radius = outer_radius= foil_radius = std::numeric_limits<double>::quiet_NaN ();
    Emin = std::numeric_limits<double>::quiet_NaN ();

    SmallRadius = std::numeric_limits<double>::quiet_NaN ();
    TangentPhi = std::numeric_limits<double>::quiet_NaN ();
    TangentTheta = std::numeric_limits<double>::quiet_NaN ();
    SmallNumber = std::numeric_limits<double>::quiet_NaN ();
    QuadrantAngle = std::numeric_limits<double>::quiet_NaN ();
    Ratio = std::numeric_limits<double>::quiet_NaN ();
    CompatibilityDistance = std::numeric_limits<double>::quiet_NaN ();
    MaxChi2 = std::numeric_limits<double>::quiet_NaN ();
    probmin = std::numeric_limits<double>::quiet_NaN ();
    nofflayers = 0;
    first_event_number = -1;
    SuperNemoChannel = false;
    max_time = std::numeric_limits<double>::quiet_NaN ();
    _moduleNR.clear ();
    _MaxBlockSize = -1;
    event_number=-1;
    hfile.clear ();

    nevent = 0;
    initial_events = 0;
    skipped_events = 0;
    run_list.clear ();
    run_time = std::numeric_limits<double>::quiet_NaN ();
    first_event=true;

    return;
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
    std::clog << "DEVEL: SULTAN::clusterizer::~clusterizer: Done." << std::endl;
  }

  //*************************************************************
  bool clusterizer::initialize( ) {
    //*************************************************************

    m.message("SULTAN::clusterizer::initialize: Entering...",mybhep::NORMAL);

    m.message("\n Beginning algorithm clusterizer \n",mybhep::VERBOSE);

    //----------- read dst param -------------//

    read_properties();

    //------- end of read pram -----------//

    nevent = 0;
    initial_events = 0;
    skipped_events = 0;

    m.message("SULTAN::clusterizer::initialize: Done.",mybhep::NORMAL);

    return true;
  }


  //*************************************************************
  bool clusterizer::finalize() {
    //*************************************************************

    clock.start(" clusterizer: finalize ");

    m.message("\n Ending algorithm clusterizer \n ",mybhep::NORMAL);

    m.message("Initial events: ", initial_events, mybhep::NORMAL);
    m.message("Skipped events: ", skipped_events, "(", 100.*skipped_events/initial_events, "\%)", mybhep::NORMAL);

    clock.stop(" clusterizer: finalize ");

    clock.dump();

    _set_defaults ();
    return true;
  }


  //*************************************************************
  void clusterizer::read_properties() {
    //*************************************************************
    m.message("SULTAN::clusterizer::read_properties: Entering...",mybhep::NORMAL);

    if (_MaxBlockSize <= 0)
      {
        _MaxBlockSize = 1;
        m.message("SULTAN::clusterizer::read_properties: no bar design, MODULES Nr set to = ",_MaxBlockSize,"\n",mybhep::NORMAL);
      }

    if(SuperNemoChannel)
      {
        m.message("SULTAN::clusterizer::read_properties: SuperNemo kind of data",mybhep::NORMAL);
        if (num_blocks <= 0)
          {
            set_num_blocks (1);
            planes_per_block.at (0) = 9;
          }
      }
    else
      {
        m.message("SULTAN::clusterizer::read_properties: Nemo-3 kind of data",mybhep::NORMAL);
        if (num_blocks <= 0)
          {
            set_num_blocks (3);
            planes_per_block.at (0) = 4;
            planes_per_block.at (1) = 2;
            planes_per_block.at (2) = 3;
          }
      }

    m.message("",mybhep::NORMAL);
    m.message("SULTAN clusterizer parameters",mybhep::NORMAL);
    m.message("verbosity print level", level, mybhep::NORMAL);
    m.message("max_time",max_time,"ms",mybhep::NORMAL);
    m.message("probmin", probmin, mybhep::NORMAL);
    m.message("nofflayers",nofflayers,mybhep::NORMAL);
    m.message("first event number", first_event_number, mybhep::NORMAL);
    m.message("num_blocks",num_blocks,mybhep::NORMAL);
    m.message("num_cells_per_plane",num_cells_per_plane,mybhep::NORMAL);
    m.message("cell_distance",cell_distance,"mm",mybhep::NORMAL);
    m.message("bfield",bfield,"T",mybhep::NORMAL);
    m.message("Emin",Emin,"MeV",mybhep::NORMAL);
    m.message("xsize is",xsize,"mm",mybhep::NORMAL);
    m.message("ysize is",ysize,"mm",mybhep::NORMAL);
    m.message("zsize is",zsize,"mm",mybhep::NORMAL);
    m.message("foil radius: ",foil_radius,"mm",mybhep::NORMAL);

    m.message("SULTAN::clusterizer::read_properties: Done.",mybhep::NORMAL);

    return;
  }


  //*******************************************************************
  bool clusterizer::prepare_event(topology::tracked_data & tracked_data_){
    //*******************************************************************

    clock.start(" clusterizer: prepare event ","cumulative");

    event_number ++;
    m.message(" local_tracking: preparing event", event_number, mybhep::VERBOSE);

    if( event_number < first_event_number ){
      m.message(" local_tracking: skip event", event_number, " first event is ", first_event_number,  mybhep::VERBOSE);
      return false;
    }

    clusters_.clear();

    setup_cells();

    tracked_data_.set_cells(cells_);

    if( level >= mybhep::VVERBOSE )
      print_cells();

    clock.stop(" clusterizer: prepare event ");


    return true;


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

    if( event_number < first_event_number ) return;

    clock.start(" clusterizer: clusterize ","cumulative");

    m.message(" local_tracking: fill clusters ", mybhep::VERBOSE);

    if( cells_.empty() ) return;

    float side[2]; // loop on two sides of the foil
    side[0] =  1.;
    side[1] = -1.;

    bool fast[2]; // loop on fast and slow hits
    fast[0] = true;
    fast[1] = false;

    std::map<int,bool > used;

    for(size_t ip=0; ip<2; ip++)  // loop on two sides of the foil
      {
        for(size_t iq=0; iq<2; iq++) // loop on fast and slow hits
          {
            for(size_t i=0; i<cells_.size(); i++)
              {
		used[cells_[i].id()] = false;
              }

            for(std::vector<topology::cell>::const_iterator icell=cells_.begin(); icell!=cells_.end(); ++icell){
              // pick a cell c that was never added
              const topology::cell & c = *icell;
              if( (cell_side(c) * side[ip]) < 0) continue;
              if( c.fast() != fast[iq] ) continue;
	      if( used[c.id()] ) continue;
	      used[c.id()] = true;

              // cell c will form a new cluster, i.e. a new list of nodes
              topology::cluster cluster_connected_to_c;
              std::vector<topology::node> nodes_connected_to_c;
              m.message(" begin new cluster with cell ", c.id(), mybhep::VERBOSE);

              // let's get the list of all the cells that can be reached from c
              // without jumps
              std::vector<topology::cell> cells_connected_to_c;
              cells_connected_to_c.push_back(c);

              for( size_t i=0; i<cells_connected_to_c.size(); i++){ // loop on connected cells
                // take a connected cell (the first one is just c)
                topology::cell cconn = cells_connected_to_c[i];

                // the connected cell composes a new node
                topology::node newnode(cconn, level, probmin);

                // get the list of cells near the connected cell
                std::vector<topology::cell> cells_near_iconn = get_near_cells(cconn);

                m.message(" cluster ", clusters_.size(), " starts with ", c.id(), " try to add cell ", cconn.id(), " with n of neighbours = ", cells_near_iconn.size(), mybhep::VERBOSE);
                for(std::vector<topology::cell>::const_iterator icnc=cells_near_iconn.begin(); icnc!=cells_near_iconn.end(); ++icnc){

                  topology::cell cnc = *icnc;

		  if( !used[cnc.id()] )
		    {
		      used[cnc.id()] = true ;
		      cells_connected_to_c.push_back(cnc);
		    }
		}
                nodes_connected_to_c.push_back(newnode);

                m.message(" cluster started with ", c.id(), " has been given cell ", cconn.id(), " cluster size ", cells_connected_to_c.size(), " i ", i, mybhep::VERBOSE);

              }
	      
              cluster_connected_to_c.set_nodes(nodes_connected_to_c);
	      
              clusters_.push_back(cluster_connected_to_c);
            }

          }
      }


    setup_clusters();

    m.message(" there are ", clusters_.size(), " clusters of cells for a total of ", cells_.size(), " cells ", mybhep::VERBOSE);
    if( level >= mybhep::VERBOSE ){
      for(std::vector<topology::cluster>::iterator iclu=clusters_.begin(); iclu!=clusters_.end(); ++iclu)
	m.message( " cluster ", iclu - clusters_.begin(), " has ", iclu->nodes().size(), " cells ", mybhep::VERBOSE);
    }

    if( level >= mybhep::VVERBOSE ){
      print_clusters();
    }

    tracked_data_.set_cells(cells_);
    tracked_data_.set_clusters(clusters_);

    clock.stop(" clusterizer: clusterize ");


    return;

  }

  //*************************************************************
  int clusterizer::cell_side( const topology::cell & c){
    //*************************************************************

    if( SuperNemoChannel )
      {
        if( c.ep().x().value() > 0. )
          return 1;

        return -1;
      }


    if( c.ep().radius().value() > foil_radius )
      return 1;

    return -1;

  }


  std::vector<topology::cell> clusterizer::get_near_cells(const topology::cell & c){

    clock.start(" clusterizer: get near cells ","cumulative");

    m.message(" filling list of cells near cell ", c.id(), " fast ", c.fast(), " side ", cell_side(c), mybhep::VVERBOSE);

    std::vector<topology::cell> cells;

    for(std::vector<topology::cell>::iterator kcell=cells_.begin(); kcell != cells_.end(); ++kcell){
      if( kcell->id() == c.id() ) continue;

      if( kcell->fast() != c.fast() ) continue;

      if( cell_side(*kcell) != cell_side(c) ) continue;

      size_t nl = kcell->near_level(c,nofflayers, cell_distance);

      if( nl > 0 )
        {
          if( level >= mybhep::VVERBOSE ){
            std::clog << "*";
          }

          topology::cell ck = *kcell;
          cells.push_back(ck);
        }
    }

    if( level >= mybhep::VVERBOSE )
      std::clog << " " << std::endl;

    clock.stop(" clusterizer: get near cells ");

    return cells;

  }


  //*************************************************************
  void clusterizer::setup_cells(){
    //*************************************************************

    for(std::vector<topology::cell>::iterator icell=cells_.begin(); icell!=cells_.end(); ++icell){
      icell->set_print_level(level);
      icell->set_probmin(probmin);
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
      icl->set_probmin(probmin);

      // loop on nodes
      for(std::vector<topology::node>::iterator inode=(*icl).nodes_.begin(); inode != (*icl).nodes_.end(); ++inode){
        inode->set_print_level(level);
        inode->set_probmin(probmin);

      }

    }

    clock.stop(" clusterizer: setup_clusters ");

    return;
  }


  void clusterizer::set_GG_GRND_diam (double ggd){
    GG_GRND_diam = ggd;
    return;
  }

  void clusterizer::set_GG_CELL_diam (double ggcd){
    GG_CELL_diam = ggcd;
    return;
  }

  void clusterizer::set_lastlayer(int ll_){
    lastlayer = ll_;
    return;
  }

  void clusterizer::set_num_blocks(int nb){
    if (nb > 0)
      {
        num_blocks = nb;
        planes_per_block.assign (num_blocks, 1);
      }
    else
      {
        std::cerr << "WARNING: SULTAN::clusterizer::set_num_blocks: "
                  << "Invalid number of GG layer blocks !" << std::endl;
        planes_per_block.clear ();
        num_blocks = -1; // invalid value
      }
    return;
  }

  void clusterizer::set_planes_per_block(int block, int nplanes){
    if (block< 0 || block>=planes_per_block.size())
      {
        throw std::range_error ("SULTAN::clusterizer::set_planes_per_block: Invalid GG layer block index !");
      }
    if (nplanes > 0)
      {
        planes_per_block.at (block) = nplanes;
      }
    else
      {
        throw std::range_error ("SULTAN::clusterizer::set_planes_per_block: Invalid number of GG layers in block !");
      }
    return;
  }

  void clusterizer::set_num_cells_per_plane(int ncpp){
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

  void clusterizer::set_SOURCE_thick(double st){
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
  void clusterizer::set_module_nr(const std::string & mID){
    _moduleNR=mID;
    return;
  }

  // What is it ?
  int clusterizer::get_module_nr(void){
    return _MaxBlockSize;
  }

  void clusterizer::set_MaxBlockSize(int mbs){
    _MaxBlockSize=mbs;
    return;
  }

  void clusterizer::set_Emin(double v){
    if ( v <= 0.0)
      {
        Emin = std::numeric_limits<double>::quiet_NaN ();
      }
    else
      {
        Emin = v;
      }
    return;
  }

  void clusterizer::set_max_time(double v){
    max_time = v;
    return;
  }

  void clusterizer::set_SmallRadius(double v){
    SmallRadius = v;
    return;
  }

  void clusterizer::set_TangentPhi(double v){
    TangentPhi = v;
    return;
  }

  void clusterizer::set_TangentTheta(double v){
    TangentTheta = v;
    return;
  }

  void clusterizer::set_SmallNumber(double v){
    SmallNumber = v;
    return;
  }

  void clusterizer::set_QuadrantAngle(double v){
    QuadrantAngle = v;
    return;
  }

  void clusterizer::set_Ratio(double v){
    Ratio = v;
    return;
  }

  void clusterizer::set_CompatibilityDistance(double v){
    CompatibilityDistance = v;
    return;
  }

  void clusterizer::set_MaxChi2(double v){
    MaxChi2 = v;
    return;
  }

  void clusterizer::set_hfile(std::string v){
    hfile = v;
    return;
  }

  void clusterizer::set_probmin(double v){
    probmin = v;
    return;
  }

  void clusterizer::set_nofflayers(size_t v){
    nofflayers = v;
    return;
  }

  void clusterizer::set_first_event(int v){
    first_event_number = v;
    return;
  }

  void clusterizer::set_level(std::string v){
    level = mybhep::get_info_level(v);
    m = mybhep::messenger(level);
    return;
  }

  void clusterizer::set_len(double v){
    len = v;
    return;
  }

  void clusterizer::set_vel(double v){
    vel = v;
    return;
  }

  void clusterizer::set_rad(double v){
    rad = v;
    return;
  }

  void clusterizer::set_cell_distance(double v){
    cell_distance = v;
    return;
  }

  void clusterizer::set_SuperNemoChannel(bool v){
    if (v)
      {
        SuperNemoChannel = true;
        set_MaxBlockSize (1);
      }
    else
      {
        SuperNemoChannel = false;
      }
    return;
  }

  void clusterizer::set_foil_radius(double v){
    foil_radius = v;
    return;
  }

  void clusterizer::set_xsize(double v){
    xsize = v;
    return;
  }

  void clusterizer::set_ysize(double v){
    ysize = v;
    return;
  }

  void clusterizer::set_zsize(double v){
    zsize = v;
    return;
  }

  void clusterizer::set_bfield(double v){
    bfield = v;
    return;
  }



}

