/* ntupler.cpp
 */

#include <CATUtils/ntupler.h>

namespace CAT{
  namespace utils{

    using namespace std;
    //________________________________________________________________________         
    // ctor:
    ntupler::ntupler ()
    {
      __title="local_tracking";
      __has_mc_hits = false;
      return;
    }

    //________________________________________________________________________       
    // dtor:
    ntupler::~ntupler ()
    {
      return;
    }


    void ntupler::initialize (void)
    {
      cout << " initializing ntupler " << endl; fflush(stdout);
  
      __f = new TFile("figure/local_tracking.root","RECREATE");
      h1 = new TH1F("h1","x distribution",100,-4,4);
      __event_ntuple = new TNtuple("__event_ntuple","event",
                                   "number_of_hits:number_of_tracks");
      __hits_ntuple = new TNtuple("__hits_ntuple","calibrated hits",
                                  "x:y:z");
      __vertexes_ntuple = new TNtuple("__vertexes_ntuple","vertexes",
                                      "x:y:z");
      __tracks_ntuple = new TNtuple("__tracks_ntuple","tracks",
                                    "length:cx:cy:cz");
  
      __tree = new TTree("__tree", "tree of event variables");
  
  
      __event.Ngg = 0;
      for(size_t i=0;  i < (size_t)MAXNHITS; i++){
        for(size_t k=0; k<9; k++){
          __event.Gg[i][k] = 0;
        }
      }
  
      __event.Nggt = 0;
      for(size_t i=0;  i < (size_t)MAXNHITS; i++){
        __event.Ggtx[i] = 0;
        __event.Ggty[i] = 0;
        __event.Ggtz[i] = 0;
        __event.Itt1[i] = 0;
        __event.Itt2[i] = 0;
    
      }
  
      __event.Nbr_tks = 0;
      __event.True_Nbr_tks = 0;
      for(size_t i=0;  i < (size_t)MAXNTRACKS; i++){
        __event.Nbr_pts[i] = 0;
    
        for(size_t k=0;  k < (size_t)MAXNHITS; k++){
          __event.Ind_points[i][k] = 0;
          __event.Ptx[i][k] = 0;
          __event.Pty[i][k] = 0;
          __event.Ptz[i][k] = 0;
          __event.Ptid[i][k] = 0;

        }
    
        __event.Tk_le[i]=0;
        __event.Nbr_vtx[i]=0;

        __event.True_Tk_le[i]=0;
        __event.True_Nbr_vtx[i]=0;
    
        for(size_t k=0;  k < (size_t)MAXNVERTEX; k++){
          __event.Vtx_cos_dir[i][k][0] = 0;
          __event.Vtx_cos_dir[i][k][1] = 0;
          __event.Vtx_cos_dir[i][k][2] = 0;
          __event.Vtx_x[i][k] = 0;
          __event.Vtx_y[i][k] = 0;
          __event.Vtx_z[i][k] = 0;

          __event.True_Vtx_cos_dir[i][k][0] = 0;
          __event.True_Vtx_cos_dir[i][k][1] = 0;
          __event.True_Vtx_cos_dir[i][k][2] = 0;
          __event.True_Vtx_x[i][k] = 0;
          __event.True_Vtx_y[i][k] = 0;
          __event.True_Vtx_z[i][k] = 0;
        }
    
      }
  
  
  
      __tree->Branch("Ngg",&__event.Ngg,"Ngg/I");
      __tree->Branch("Gg",__event.Gg,"Gg[Ngg][9]/F");
      __tree->Branch("Nggt",&__event.Nggt,"Nggt/I");
      __tree->Branch("Ggtx",__event.Ggtx,"Ggtx[Nggt]/F");
      __tree->Branch("Ggty",__event.Ggty,"Ggty[Nggt]/F");
      __tree->Branch("Ggtz",__event.Ggtz,"Ggtz[Nggt]/F");
      __tree->Branch("Itt1",__event.Itt1,"Itt1[Nggt]/F");
      __tree->Branch("Itt2",__event.Itt2,"Itt2[Nggt]/F");
      __tree->Branch("Nbr_tks",&__event.Nbr_tks,"Nbr_tks/I");
      __tree->Branch("Nbr_pts",__event.Nbr_pts,"Nbr_pts[Nbr_tks]/I");
      __tree->Branch("Ind_points",__event.Ind_points,"Ind_points[Nbr_tks][Ngg]/F");
      __tree->Branch("Tk_le",__event.Tk_le,"Tk_le[Nbr_tks]/F");
      __tree->Branch("Nbr_vtx",__event.Nbr_vtx,"Nbr_vtx[Nbr_tks]/I");
      __tree->Branch("Vtx_cos_dir",__event.Vtx_cos_dir,"Vtx_cos_dir[Nbr_tks][5][3]/F");
      __tree->Branch("Vtx_x",__event.Vtx_x,"Vtx_x[Nbr_tks][5]/F");
      __tree->Branch("Vtx_y",__event.Vtx_y,"Vtx_y[Nbr_tks][5]/F");
      __tree->Branch("Vtx_z",__event.Vtx_z,"Vtx_z[Nbr_tks][5]/F");
      __tree->Branch("Ptx",__event.Ptx,"Ptx[Nbr_tks][500]/F");
      __tree->Branch("Pty",__event.Pty,"Pty[Nbr_tks][500]/F");
      __tree->Branch("Ptz",__event.Ptz,"Ptz[Nbr_tks][500]/F");
      __tree->Branch("Ptid",__event.Ptid,"Ptid[Nbr_tks][500]/I");
  
      return;
    }


    void ntupler::finalize (void)
    {
      cout << " finalizing ntupler " << endl;
      __tree->SetDirectory(__f);
      __tree->Write();
  
      __f->Write();
  
      return;
    }


    //________________________________________________________________________    
    void ntupler::set_title (const string & title_)
    { 
  
      __title = title_;
    }

    //________________________________________________________________________    
    const string & ntupler::get_title()
    { 
      return __title ;   
    }   
     


    //________________________________________________________________________    
    void ntupler::set_tracked_data (const topology::tracked_data& td){

      tracked_data_ = td;

      return;
    }  

    
    
    //________________________________________________________________________    
    const  topology::tracked_data & ntupler::get_tracked_data () const {

      return tracked_data_ ;   
    }


    //________________________________________________________________________         
    void ntupler::clear ( ) 
    { 
  
      cout << "clearing file " << endl; fflush(stdout);
      delete __f;
      cout << "clearing ntuple " << endl; fflush(stdout);
      delete __event_ntuple; delete __hits_ntuple; delete __vertexes_ntuple; delete __tracks_ntuple;
      cout << "clearing histo " << endl; fflush(stdout);
      delete h1;

    }
    



    //________________________________________________________________________ 
    void ntupler::__fill ()
    {
  
      size_t ncells = tracked_data_.cells_.size();


      __event.Ngg = ncells;
      __event.Nggt = ncells;

      size_t cell_counter = 0;
      for(size_t icell=0; icell< ncells; icell++){
    
        if( icell >= (size_t)MAXNHITS ){
          cout << " warning: icell " << icell << " maxnhits " << MAXNHITS << endl;
          continue;
        }

        __hits_ntuple->Fill(tracked_data_.cells_[icell].ep().x().value(),
                            tracked_data_.cells_[icell].ep().y().value(),
                            tracked_data_.cells_[icell].ep().z().value());

        __event.Gg[cell_counter][0] = 0; // cell status
        __event.Gg[cell_counter][1] = tracked_data_.cells_[icell].id(); // cell number
        __event.Gg[cell_counter][2] = tracked_data_.cells_[icell].ep().x().value();
        __event.Gg[cell_counter][3] = tracked_data_.cells_[icell].ep().y().value();
        __event.Gg[cell_counter][4] = tracked_data_.cells_[icell].ep().z().value();
        __event.Gg[cell_counter][5] = tracked_data_.cells_[icell].r0().value();
        __event.Gg[cell_counter][6] = tracked_data_.cells_[icell].r0().error();
        __event.Gg[cell_counter][7] = tracked_data_.cells_[icell].ep().y().error();
        __event.Gg[cell_counter][8] = icell;
    
        if(__has_mc_hits) {
          topology::node truenode; 
          size_t itrack;
          if( get_true_hit_of_reco_cell(tracked_data_.cells_[icell], truenode, itrack)){
            __event.Ggtx[cell_counter] = truenode.ep().x().value();
            __event.Ggty[cell_counter] = truenode.ep().y().value();
            __event.Ggtz[cell_counter] = truenode.ep().z().value();
            __event.Itt1[cell_counter] = itrack;
          }
        }

        cell_counter ++;
    
      }

      if( !tracked_data_.scenarios_.empty() ){
        __event.Nbr_tks = tracked_data_.scenarios_[0].sequences_.size();
        size_t trk_counter = 0;
        for(std::vector<topology::sequence>::iterator iseq = tracked_data_.scenarios_[0].sequences_.begin(); 
            iseq != tracked_data_.scenarios_[0].sequences_.end(); ++iseq){
      
          if( iseq - tracked_data_.scenarios_[0].sequences_.begin() >= MAXNTRACKS ){
            cout << " warning: iseq " << iseq - tracked_data_.scenarios_[0].sequences_.begin() << " maxntracks " << MAXNTRACKS << endl;
            continue;
          }

          __event.Nbr_pts[trk_counter] = iseq->nodes_.size();
      
          size_t pt_counter = 0;
          for(std::vector<topology::node>::iterator inode = iseq->nodes_.begin(); 
              inode != iseq->nodes_.end(); ++inode){

            if( inode - iseq->nodes_.begin() >= MAXNHITS ){
              cout << " warning: inode " << inode - iseq->nodes_.begin() << " maxnhits " << MAXNHITS << endl;
              continue;
            }


            size_t cindex = get_cell_index(inode->c());
            __event.Ind_points[trk_counter][pt_counter] = cindex;
            __event.Ptx[trk_counter][pt_counter] = inode->ep().x().value();
            __event.Pty[trk_counter][pt_counter] = inode->ep().y().value();
            __event.Ptz[trk_counter][pt_counter] = inode->ep().z().value();
            __event.Ptid[trk_counter][pt_counter] = cindex;
            pt_counter ++;
          }
      
          __event.Tk_le[trk_counter]=iseq->length().value();


          if( iseq->nodes_.size() >= 2 ){
            topology::experimental_vector dir(iseq->nodes_[0].ep(), iseq->nodes_[1].ep());
            __event.Vtx_cos_dir[trk_counter][0][0]=dir.x().value();
            __event.Vtx_cos_dir[trk_counter][0][1]=dir.y().value();
            __event.Vtx_cos_dir[trk_counter][0][2]=dir.z().value();
          }
      
          if( iseq->has_decay_vertex() && iseq->nodes_.size() >= 2){
            size_t s = iseq->nodes_.size();
            topology::experimental_vector dir(iseq->nodes_[s-2].ep(), iseq->nodes_[s-1].ep());
            __event.Vtx_cos_dir[trk_counter][1][0]=dir.x().value();
            __event.Vtx_cos_dir[trk_counter][1][1]=dir.y().value();
            __event.Vtx_cos_dir[trk_counter][1][2]=dir.z().value();
            __event.Vtx_x[trk_counter][0]=iseq->vertex().x().value();
            __event.Vtx_y[trk_counter][0]=iseq->vertex().y().value();
            __event.Vtx_z[trk_counter][0]=iseq->vertex().z().value();
          }
      
          trk_counter ++;
      
        }
      }

      __event.True_Nbr_tks = tracked_data_.true_sequences_.size();
      size_t true_trk_counter = 0;
      for(std::vector<topology::sequence>::iterator iseq = tracked_data_.true_sequences_.begin(); 
          iseq != tracked_data_.true_sequences_.end(); ++iseq){
    
        if( iseq - tracked_data_.true_sequences_.begin() >= MAXNTRACKS ){
          cout << " warning: true iseq " << iseq - tracked_data_.true_sequences_.begin() << " maxntracks " << MAXNTRACKS << endl;
          continue;
        }

        __event.True_Tk_le[true_trk_counter]=iseq->length().value();


        if( iseq->nodes_.size() >= 2 ){
          topology::experimental_vector dir(iseq->nodes_[0].ep(), iseq->nodes_[1].ep());
          __event.True_Vtx_cos_dir[true_trk_counter][0][0]=dir.x().value();
          __event.True_Vtx_cos_dir[true_trk_counter][0][1]=dir.y().value();
          __event.True_Vtx_cos_dir[true_trk_counter][0][2]=dir.z().value();
        }
    
        if( iseq->has_decay_vertex() && iseq->nodes_.size() >= 2){
          size_t s = iseq->nodes_.size();
          topology::experimental_vector dir(iseq->nodes_[s-2].ep(), iseq->nodes_[s-1].ep());
          __event.Vtx_cos_dir[true_trk_counter][1][0]=dir.x().value();
          __event.Vtx_cos_dir[true_trk_counter][1][1]=dir.y().value();
          __event.Vtx_cos_dir[true_trk_counter][1][2]=dir.z().value();
          __event.Vtx_x[true_trk_counter][0]=iseq->vertex().x().value();
          __event.Vtx_y[true_trk_counter][0]=iseq->vertex().y().value();
          __event.Vtx_z[true_trk_counter][0]=iseq->vertex().z().value();
        }
    
    
        true_trk_counter ++;
    
      }

  
  
      __tree->Fill();
  
    }

    //________________________________________________________________________
    bool ntupler::get_true_hit_of_reco_cell(topology::cell c, topology::node& n, size_t& index){


      for(std::vector<topology::sequence>::iterator iseq = tracked_data_.true_sequences_.begin();
          iseq != tracked_data_.true_sequences_.end(); ++iseq){

        for(std::vector<topology::node>::iterator inode = iseq->nodes_.begin(); inode != iseq->nodes_.end();
            ++inode){

          if( inode->c_.same_cell(c)){
            n = *inode;
            index = iseq - tracked_data_.true_sequences_.begin();
            return true;
          }
        }
      }

      cout << " problem: ntupler could not find true hit corresponding to reco hit "; c.dump();

      return false;

    }

    //________________________________________________________________________
    size_t ntupler::get_cell_index(topology::cell c){


      for(std::vector<topology::cell>::iterator icell = tracked_data_.cells_.begin();
          icell != tracked_data_.cells_.end(); ++icell){
        if( c.same_cell(*icell) ) return icell - tracked_data_.cells_.begin();
      }

      cout << " problem: ntupler could not find cell index corresponding to cell "; c.dump();
      fflush(stdout);

      return 0;


    }

  }
}

// end of ntupler.cpp


