/* ntupler.cpp
 */

#include <CATUtils/ntupler.h>

//________________________________________________________________________         
// ctor:
ntupler::ntupler ()
{
  
  __outfilename="figure/local_tracking.root";
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
  
  __f = new TFile(__outfilename.c_str(),"RECREATE");

  if (! __f->IsWritable ())
    {
      cout << " problem: file " << __outfilename << " is not writable " << endl;
      return;
    }

  __f->cd ();
  __tree = new TTree("SimData","SimData");
  // 2012-07-06 FM : Force affectation of the tree's current directory :
  __tree->SetDirectory (__f);

    __tree->Branch("simulated_vs_tracked_hit.id",&simulated_vs_tracked_hit_.id_);
#if 0
  __tree->Branch("simulated_vs_tracked_hit.simulated_x",&simulated_vs_tracked_hit_.simulated_x_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_y",&simulated_vs_tracked_hit_.simulated_y_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_z",&simulated_vs_tracked_hit_.simulated_z_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_dx",&simulated_vs_tracked_hit_.simulated_dx_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_dz",&simulated_vs_tracked_hit_.simulated_dz_);
#endif

    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_x",&simulated_vs_tracked_hit_.tracked_tangency_x_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_y",&simulated_vs_tracked_hit_.tracked_tangency_y_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_z",&simulated_vs_tracked_hit_.tracked_tangency_z_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_x_error",&simulated_vs_tracked_hit_.tracked_tangency_x_error_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_y_error",&simulated_vs_tracked_hit_.tracked_tangency_y_error_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_z_error",&simulated_vs_tracked_hit_.tracked_tangency_z_error_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_dx",&simulated_vs_tracked_hit_.tracked_tangency_dx_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_tangency_dz",&simulated_vs_tracked_hit_.tracked_tangency_dz_);

#if 0
  __tree->Branch("simulated_vs_tracked_hit.residual_tangency_x",&simulated_vs_tracked_hit_.residual_tangency_x_);
  __tree->Branch("simulated_vs_tracked_hit.residual_tangency_y",&simulated_vs_tracked_hit_.residual_tangency_y_);
  __tree->Branch("simulated_vs_tracked_hit.residual_tangency_z",&simulated_vs_tracked_hit_.residual_tangency_z_);
  __tree->Branch("simulated_vs_tracked_hit.pull_tangency_x",&simulated_vs_tracked_hit_.pull_tangency_x_);
  __tree->Branch("simulated_vs_tracked_hit.pull_tangency_y",&simulated_vs_tracked_hit_.pull_tangency_y_);
  __tree->Branch("simulated_vs_tracked_hit.pull_tangency_z",&simulated_vs_tracked_hit_.pull_tangency_z_);
#endif 

    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_x",&simulated_vs_tracked_hit_.tracked_helix_x_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_y",&simulated_vs_tracked_hit_.tracked_helix_y_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_z",&simulated_vs_tracked_hit_.tracked_helix_z_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_x_error",&simulated_vs_tracked_hit_.tracked_helix_x_error_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_y_error",&simulated_vs_tracked_hit_.tracked_helix_y_error_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_z_error",&simulated_vs_tracked_hit_.tracked_helix_z_error_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_dx",&simulated_vs_tracked_hit_.tracked_helix_dx_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_helix_dz",&simulated_vs_tracked_hit_.tracked_helix_dz_);

#if 0
  __tree->Branch("simulated_vs_tracked_hit.residual_helix_x",&simulated_vs_tracked_hit_.residual_helix_x_);
  __tree->Branch("simulated_vs_tracked_hit.residual_helix_y",&simulated_vs_tracked_hit_.residual_helix_y_);
  __tree->Branch("simulated_vs_tracked_hit.residual_helix_z",&simulated_vs_tracked_hit_.residual_helix_z_);
  __tree->Branch("simulated_vs_tracked_hit.pull_helix_x",&simulated_vs_tracked_hit_.pull_helix_x_);
  __tree->Branch("simulated_vs_tracked_hit.pull_helix_y",&simulated_vs_tracked_hit_.pull_helix_y_);
  __tree->Branch("simulated_vs_tracked_hit.pull_helix_z",&simulated_vs_tracked_hit_.pull_helix_z_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_length",&simulated_vs_tracked_hit_.simulated_length_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_phi",&simulated_vs_tracked_hit_.simulated_phi_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_theta",&simulated_vs_tracked_hit_.simulated_theta_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_kink_hor",&simulated_vs_tracked_hit_.simulated_kink_hor_);
  __tree->Branch("simulated_vs_tracked_hit.simulated_kink_vert",&simulated_vs_tracked_hit_.simulated_kink_vert_);
#endif

    __tree->Branch("simulated_vs_tracked_hit.tracked_length",&simulated_vs_tracked_hit_.tracked_length_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_phi",&simulated_vs_tracked_hit_.tracked_phi_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_theta",&simulated_vs_tracked_hit_.tracked_theta_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_kink_hor",&simulated_vs_tracked_hit_.tracked_kink_hor_);
    __tree->Branch("simulated_vs_tracked_hit.tracked_kink_vert",&simulated_vs_tracked_hit_.tracked_kink_vert_);


#if 0
  __tree->Branch("simulated_vs_tracked_track.id",&simulated_vs_tracked_track_.id_);
  __tree->Branch("simulated_vs_tracked_track.simulated_vertex_x",&simulated_vs_tracked_track_.simulated_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.simulated_vertex_y",&simulated_vs_tracked_track_.simulated_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.simulated_vertex_z",&simulated_vs_tracked_track_.simulated_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.simulated_vertex_phi",&simulated_vs_tracked_track_.simulated_vertex_phi_);
#endif

    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_x",&simulated_vs_tracked_track_.tracked_helix_vertex_x_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_y",&simulated_vs_tracked_track_.tracked_helix_vertex_y_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_z",&simulated_vs_tracked_track_.tracked_helix_vertex_z_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_phi",&simulated_vs_tracked_track_.tracked_helix_vertex_phi_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_x_error",&simulated_vs_tracked_track_.tracked_helix_vertex_x_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_y_error",&simulated_vs_tracked_track_.tracked_helix_vertex_y_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_z_error",&simulated_vs_tracked_track_.tracked_helix_vertex_z_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_vertex_phi_error",&simulated_vs_tracked_track_.tracked_helix_vertex_phi_error_);

#if 0
  __tree->Branch("simulated_vs_tracked_track.residual_helix_vertex_x",&simulated_vs_tracked_track_.residual_helix_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.residual_helix_vertex_y",&simulated_vs_tracked_track_.residual_helix_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.residual_helix_vertex_z",&simulated_vs_tracked_track_.residual_helix_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.residual_helix_vertex_phi",&simulated_vs_tracked_track_.residual_helix_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_vertex_x",&simulated_vs_tracked_track_.pull_helix_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_vertex_y",&simulated_vs_tracked_track_.pull_helix_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_vertex_z",&simulated_vs_tracked_track_.pull_helix_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_vertex_phi",&simulated_vs_tracked_track_.pull_helix_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_track.simulated_decay_vertex_x",&simulated_vs_tracked_track_.simulated_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.simulated_decay_vertex_y",&simulated_vs_tracked_track_.simulated_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.simulated_decay_vertex_z",&simulated_vs_tracked_track_.simulated_decay_vertex_z_);
#endif

    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_x",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_x_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_y",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_y_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_z",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_z_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_phi",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_phi_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_x_error",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_x_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_y_error",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_y_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_z_error",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_z_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_helix_decay_vertex_phi_error",&simulated_vs_tracked_track_.tracked_helix_decay_vertex_phi_error_);

#if 0
  __tree->Branch("simulated_vs_tracked_track.residual_helix_decay_vertex_x",&simulated_vs_tracked_track_.residual_helix_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.residual_helix_decay_vertex_y",&simulated_vs_tracked_track_.residual_helix_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.residual_helix_decay_vertex_z",&simulated_vs_tracked_track_.residual_helix_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_decay_vertex_x",&simulated_vs_tracked_track_.pull_helix_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_decay_vertex_y",&simulated_vs_tracked_track_.pull_helix_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.pull_helix_decay_vertex_z",&simulated_vs_tracked_track_.pull_helix_decay_vertex_z_);
#endif

    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_x",&simulated_vs_tracked_track_.tracked_tangent_vertex_x_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_y",&simulated_vs_tracked_track_.tracked_tangent_vertex_y_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_z",&simulated_vs_tracked_track_.tracked_tangent_vertex_z_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_phi",&simulated_vs_tracked_track_.tracked_tangent_vertex_phi_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_x_error",&simulated_vs_tracked_track_.tracked_tangent_vertex_x_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_y_error",&simulated_vs_tracked_track_.tracked_tangent_vertex_y_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_z_error",&simulated_vs_tracked_track_.tracked_tangent_vertex_z_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_vertex_phi_error",&simulated_vs_tracked_track_.tracked_tangent_vertex_phi_error_);

#if 0
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_vertex_x",&simulated_vs_tracked_track_.residual_tangent_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_vertex_y",&simulated_vs_tracked_track_.residual_tangent_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_vertex_z",&simulated_vs_tracked_track_.residual_tangent_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_vertex_phi",&simulated_vs_tracked_track_.residual_tangent_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_vertex_x",&simulated_vs_tracked_track_.pull_tangent_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_vertex_y",&simulated_vs_tracked_track_.pull_tangent_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_vertex_z",&simulated_vs_tracked_track_.pull_tangent_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_vertex_phi",&simulated_vs_tracked_track_.pull_tangent_vertex_phi_);
#endif

    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_x",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_x_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_y",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_y_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_z",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_z_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_phi",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_phi_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_x_error",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_x_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_y_error",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_y_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_z_error",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_z_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_tangent_decay_vertex_phi_error",&simulated_vs_tracked_track_.tracked_tangent_decay_vertex_phi_error_);

#if 0
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_decay_vertex_x",&simulated_vs_tracked_track_.residual_tangent_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_decay_vertex_y",&simulated_vs_tracked_track_.residual_tangent_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.residual_tangent_decay_vertex_z",&simulated_vs_tracked_track_.residual_tangent_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_decay_vertex_x",&simulated_vs_tracked_track_.pull_tangent_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_decay_vertex_y",&simulated_vs_tracked_track_.pull_tangent_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_track.pull_tangent_decay_vertex_z",&simulated_vs_tracked_track_.pull_tangent_decay_vertex_z_);
#endif

    __tree->Branch("simulated_vs_tracked_track.tracked_kink_x",&simulated_vs_tracked_track_.tracked_kink_x_);
    __tree->Branch("simulated_vs_tracked_track.tracked_kink_y",&simulated_vs_tracked_track_.tracked_kink_y_);
    __tree->Branch("simulated_vs_tracked_track.tracked_kink_z",&simulated_vs_tracked_track_.tracked_kink_z_);
    __tree->Branch("simulated_vs_tracked_track.tracked_kink_x_error",&simulated_vs_tracked_track_.tracked_kink_x_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_kink_y_error",&simulated_vs_tracked_track_.tracked_kink_y_error_);
    __tree->Branch("simulated_vs_tracked_track.tracked_kink_z_error",&simulated_vs_tracked_track_.tracked_kink_z_error_);

#if 0
  __tree->Branch("simulated_vs_tracked_track.residual_kink_x",&simulated_vs_tracked_track_.residual_kink_x_);
  __tree->Branch("simulated_vs_tracked_track.residual_kink_y",&simulated_vs_tracked_track_.residual_kink_y_);
  __tree->Branch("simulated_vs_tracked_track.residual_kink_z",&simulated_vs_tracked_track_.residual_kink_z_);
  __tree->Branch("simulated_vs_tracked_track.pull_kink_x",&simulated_vs_tracked_track_.pull_kink_x_);
  __tree->Branch("simulated_vs_tracked_track.pull_kink_y",&simulated_vs_tracked_track_.pull_kink_y_);
  __tree->Branch("simulated_vs_tracked_track.pull_kink_z",&simulated_vs_tracked_track_.pull_kink_z_);
#endif

#if 0
  __tree->Branch("simulated_vs_tracked_n3track.id",&simulated_vs_tracked_n3track_.id_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_vertex_x",&simulated_vs_tracked_n3track_.simulated_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_vertex_y",&simulated_vs_tracked_n3track_.simulated_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_vertex_z",&simulated_vs_tracked_n3track_.simulated_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_vertex_phi",&simulated_vs_tracked_n3track_.simulated_vertex_phi_);
#endif

    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_x",&simulated_vs_tracked_n3track_.tracked_helix_vertex_x_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_y",&simulated_vs_tracked_n3track_.tracked_helix_vertex_y_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_z",&simulated_vs_tracked_n3track_.tracked_helix_vertex_z_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_phi",&simulated_vs_tracked_n3track_.tracked_helix_vertex_phi_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_x_error",&simulated_vs_tracked_n3track_.tracked_helix_vertex_x_error_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_y_error",&simulated_vs_tracked_n3track_.tracked_helix_vertex_y_error_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_z_error",&simulated_vs_tracked_n3track_.tracked_helix_vertex_z_error_);
    __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_vertex_phi_error",&simulated_vs_tracked_n3track_.tracked_helix_vertex_phi_error_);
#if 0
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_vertex_x",&simulated_vs_tracked_n3track_.residual_helix_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_vertex_y",&simulated_vs_tracked_n3track_.residual_helix_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_vertex_z",&simulated_vs_tracked_n3track_.residual_helix_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_vertex_phi",&simulated_vs_tracked_n3track_.residual_helix_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_vertex_x",&simulated_vs_tracked_n3track_.pull_helix_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_vertex_y",&simulated_vs_tracked_n3track_.pull_helix_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_vertex_z",&simulated_vs_tracked_n3track_.pull_helix_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_vertex_phi",&simulated_vs_tracked_n3track_.pull_helix_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_decay_vertex_x",&simulated_vs_tracked_n3track_.simulated_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_decay_vertex_y",&simulated_vs_tracked_n3track_.simulated_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.simulated_decay_vertex_z",&simulated_vs_tracked_n3track_.simulated_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_decay_vertex_x",&simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_decay_vertex_y",&simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_decay_vertex_z",&simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_decay_vertex_x_error",&simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_x_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_decay_vertex_y_error",&simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_y_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_helix_decay_vertex_z_error",&simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_z_error_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_decay_vertex_x",&simulated_vs_tracked_n3track_.residual_helix_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_decay_vertex_y",&simulated_vs_tracked_n3track_.residual_helix_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_helix_decay_vertex_z",&simulated_vs_tracked_n3track_.residual_helix_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_decay_vertex_x",&simulated_vs_tracked_n3track_.pull_helix_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_decay_vertex_y",&simulated_vs_tracked_n3track_.pull_helix_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_helix_decay_vertex_z",&simulated_vs_tracked_n3track_.pull_helix_decay_vertex_z_);

  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_x",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_y",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_z",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_phi",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_x_error",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_x_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_y_error",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_y_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_z_error",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_z_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_vertex_phi_error",&simulated_vs_tracked_n3track_.tracked_tangent_vertex_phi_error_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_vertex_x",&simulated_vs_tracked_n3track_.residual_tangent_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_vertex_y",&simulated_vs_tracked_n3track_.residual_tangent_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_vertex_z",&simulated_vs_tracked_n3track_.residual_tangent_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_vertex_phi",&simulated_vs_tracked_n3track_.residual_tangent_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_vertex_x",&simulated_vs_tracked_n3track_.pull_tangent_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_vertex_y",&simulated_vs_tracked_n3track_.pull_tangent_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_vertex_z",&simulated_vs_tracked_n3track_.pull_tangent_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_vertex_phi",&simulated_vs_tracked_n3track_.pull_tangent_vertex_phi_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_decay_vertex_x",&simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_decay_vertex_y",&simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_decay_vertex_z",&simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_decay_vertex_x_error",&simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_x_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_decay_vertex_y_error",&simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_y_error_);
  __tree->Branch("simulated_vs_tracked_n3track.tracked_tangent_decay_vertex_z_error",&simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_z_error_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_decay_vertex_x",&simulated_vs_tracked_n3track_.residual_tangent_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_decay_vertex_y",&simulated_vs_tracked_n3track_.residual_tangent_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.residual_tangent_decay_vertex_z",&simulated_vs_tracked_n3track_.residual_tangent_decay_vertex_z_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_decay_vertex_x",&simulated_vs_tracked_n3track_.pull_tangent_decay_vertex_x_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_decay_vertex_y",&simulated_vs_tracked_n3track_.pull_tangent_decay_vertex_y_);
  __tree->Branch("simulated_vs_tracked_n3track.pull_tangent_decay_vertex_z",&simulated_vs_tracked_n3track_.pull_tangent_decay_vertex_z_);
#endif

  return;
}


void ntupler::finalize (void)
{
  __tree->SetDirectory(__f);
  __tree->Write();
  
  __f->Write();
  

  // Destroy internal resources :
  __f = (TFile*)NULL;
  __tree = (TTree*)NULL;
  __outfilename = "";


  return;
}


//________________________________________________________________________    
void ntupler::set_outfilename (std::string title_)
{ 
  
  __outfilename = title_;
}

//________________________________________________________________________    
std::string  ntupler::get_outfilename()
{ 
  return __outfilename ;   
}   
     


//________________________________________________________________________    
void ntupler::set_tracked_data (const CAT::topology::tracked_data& td){

  tracked_data_ = td;

  return;
}  

    
    
//________________________________________________________________________    
const  CAT::topology::tracked_data & ntupler::get_tracked_data () const {

  return tracked_data_ ;   
}


//________________________________________________________________________         
void ntupler::clear ( ) 
{ 
  
  cout << "clearing file " << endl; fflush(stdout);
  delete __f;

}
    



//________________________________________________________________________ 
void ntupler::__fill ()
{
  
  std::vector<int> simulated_vs_tracked_hit_id_;
  std::vector<double> simulated_vs_tracked_simulated_x_;
  std::vector<double> simulated_vs_tracked_simulated_y_;
  std::vector<double> simulated_vs_tracked_simulated_z_;
  std::vector<double> simulated_vs_tracked_simulated_dx_;
  std::vector<double> simulated_vs_tracked_simulated_dz_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_x_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_y_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_z_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_x_error_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_y_error_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_z_error_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_dx_;
  std::vector<double> simulated_vs_tracked_tracked_tangency_dz_;
  std::vector<double> simulated_vs_tracked_residual_tangency_x_;
  std::vector<double> simulated_vs_tracked_residual_tangency_y_;
  std::vector<double> simulated_vs_tracked_residual_tangency_z_;
  std::vector<double> simulated_vs_tracked_pull_tangency_x_;
  std::vector<double> simulated_vs_tracked_pull_tangency_y_;
  std::vector<double> simulated_vs_tracked_pull_tangency_z_;
  std::vector<double> simulated_vs_tracked_tracked_helix_x_;
  std::vector<double> simulated_vs_tracked_tracked_helix_y_;
  std::vector<double> simulated_vs_tracked_tracked_helix_z_;
  std::vector<double> simulated_vs_tracked_tracked_helix_x_error_;
  std::vector<double> simulated_vs_tracked_tracked_helix_y_error_;
  std::vector<double> simulated_vs_tracked_tracked_helix_z_error_;
  std::vector<double> simulated_vs_tracked_tracked_helix_dx_;
  std::vector<double> simulated_vs_tracked_tracked_helix_dz_;
  std::vector<double> simulated_vs_tracked_residual_helix_x_;
  std::vector<double> simulated_vs_tracked_residual_helix_y_;
  std::vector<double> simulated_vs_tracked_residual_helix_z_;
  std::vector<double> simulated_vs_tracked_pull_helix_x_;
  std::vector<double> simulated_vs_tracked_pull_helix_y_;
  std::vector<double> simulated_vs_tracked_pull_helix_z_;
  std::vector<double> simulated_vs_tracked_simulated_length_;
  std::vector<double> simulated_vs_tracked_simulated_phi_;
  std::vector<double> simulated_vs_tracked_simulated_theta_;
  std::vector<double> simulated_vs_tracked_simulated_kink_hor_;
  std::vector<double> simulated_vs_tracked_simulated_kink_vert_;
  std::vector<double> simulated_vs_tracked_tracked_length_;
  std::vector<double> simulated_vs_tracked_tracked_phi_;
  std::vector<double> simulated_vs_tracked_tracked_theta_;
  std::vector<double> simulated_vs_tracked_tracked_kink_hor_;
  std::vector<double> simulated_vs_tracked_tracked_kink_vert_;

  std::vector<int> simulated_vs_tracked_track_id_;
  std::vector<double> simulated_vs_tracked_track_simulated_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_simulated_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_simulated_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_simulated_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_vertex_phi_error_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_simulated_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_simulated_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_simulated_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_error_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_residual_helix_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_pull_helix_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_vertex_phi_error_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_error_; 
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_error_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_residual_tangent_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_track_pull_tangent_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_x_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_y_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_z_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_phi_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_x_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_y_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_z_error_;
  std::vector<double> simulated_vs_tracked_track_tracked_kink_phi_error_;
  std::vector<double> simulated_vs_tracked_track_residual_kink_x_;
  std::vector<double> simulated_vs_tracked_track_residual_kink_y_;
  std::vector<double> simulated_vs_tracked_track_residual_kink_z_;
  std::vector<double> simulated_vs_tracked_track_residual_kink_phi_;
  std::vector<double> simulated_vs_tracked_track_pull_kink_x_;
  std::vector<double> simulated_vs_tracked_track_pull_kink_y_;
  std::vector<double> simulated_vs_tracked_track_pull_kink_z_;
  std::vector<double> simulated_vs_tracked_track_pull_kink_phi_;


  std::vector<int> simulated_vs_tracked_n3track_id_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_vertex_phi_error_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_simulated_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_decay_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_decay_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_helix_decay_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_residual_helix_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_pull_helix_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_vertex_phi_error_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_vertex_phi_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_x_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_y_error_;
  std::vector<double> simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_z_error_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_residual_tangent_decay_vertex_z_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_decay_vertex_x_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_decay_vertex_y_;
  std::vector<double> simulated_vs_tracked_n3track_pull_tangent_decay_vertex_z_;


  /// start tracking study

  bool skipped_event = true;

  if( !tracked_data_.scenarios_.empty() ){

    CAT::topology::scenario scen=tracked_data_.scenarios_[0];

    for(std::vector<CAT::topology::sequence>::iterator iseq=scen.sequences_.begin(); iseq!=scen.sequences_.end(); iseq++){

      CAT::topology::helix hel=iseq->get_helix();
      CAT::topology::experimental_double phi(0.,0.);
      double phi_ref = 0.;
	
      for(std::vector<CAT::topology::node>::iterator inode=iseq->nodes_.begin(); inode!=iseq->nodes_.end(); inode++){

	simulated_vs_tracked_hit_id_.push_back(inode->c_.id());
	CAT::topology::experimental_point tangent=inode->ep();
	simulated_vs_tracked_tracked_tangency_x_.push_back(tangent.x().value());
	simulated_vs_tracked_tracked_tangency_y_.push_back(tangent.y().value());
	simulated_vs_tracked_tracked_tangency_z_.push_back(tangent.z().value());
	simulated_vs_tracked_tracked_tangency_x_error_.push_back(tangent.x().error());
	simulated_vs_tracked_tracked_tangency_y_error_.push_back(tangent.y().error());
	simulated_vs_tracked_tracked_tangency_z_error_.push_back(tangent.z().error());
	simulated_vs_tracked_tracked_tangency_dx_.push_back(tangent.x().value() - inode->c().ep().x().value());
	simulated_vs_tracked_tracked_tangency_dz_.push_back(tangent.z().value() - inode->c().ep().z().value());

	phi_ref=phi.value();
	phi = hel.phi_of_point(inode->c().ep(),phi_ref);

	CAT::topology::experimental_point helixp=hel.position(phi);

	simulated_vs_tracked_tracked_helix_x_.push_back(helixp.x().value());
	simulated_vs_tracked_tracked_helix_y_.push_back(helixp.y().value());
	simulated_vs_tracked_tracked_helix_z_.push_back(helixp.z().value());
	simulated_vs_tracked_tracked_helix_x_error_.push_back(helixp.x().error());
	simulated_vs_tracked_tracked_helix_y_error_.push_back(helixp.y().error());
	simulated_vs_tracked_tracked_helix_z_error_.push_back(helixp.z().error());
	simulated_vs_tracked_tracked_helix_dx_.push_back(helixp.x().value() - inode->c().ep().x().value());
	simulated_vs_tracked_tracked_helix_dz_.push_back(helixp.z().value() - inode->c().ep().z().value());

	size_t indnode=inode-iseq->nodes_.begin();
	if( indnode != 0 ){
	  CAT::topology::experimental_point prevtangent=iseq->nodes_[indnode-1].ep();
	  CAT::topology::experimental_vector step(prevtangent,tangent);
	  simulated_vs_tracked_tracked_length_.push_back(step.length().value());
	  simulated_vs_tracked_tracked_phi_.push_back(step.phi().value());
	  simulated_vs_tracked_tracked_theta_.push_back(step.theta().value());
					
	  if( indnode != 1 ){
	    CAT::topology::experimental_point prevprevtangent=iseq->nodes_[indnode-2].ep();
	    CAT::topology::experimental_vector prevstep(prevprevtangent,prevtangent);
	    simulated_vs_tracked_tracked_kink_hor_.push_back(prevstep.kink_phi(step).value() );
	    simulated_vs_tracked_tracked_kink_vert_.push_back(prevstep.kink_theta(step).value() );
					
	  }
	}
					

      }

      CAT::topology::experimental_point hvertex=iseq->helix_vertex();
      CAT::topology::experimental_point tvertex=iseq->tangent_vertex();
      CAT::topology::experimental_vector vhvertex(hvertex);
      CAT::topology::experimental_vector vtvertex(tvertex);

      if( iseq->helix_vertex_type() == "foil" ){
	simulated_vs_tracked_track_tracked_helix_vertex_x_.push_back(hvertex.x().value());
	simulated_vs_tracked_track_tracked_helix_vertex_y_.push_back(hvertex.y().value());
	simulated_vs_tracked_track_tracked_helix_vertex_z_.push_back(hvertex.z().value());
	simulated_vs_tracked_track_tracked_helix_vertex_phi_.push_back(fabs(vhvertex.phi().value()));
      
	simulated_vs_tracked_track_tracked_helix_vertex_x_error_.push_back(hvertex.x().error());
	simulated_vs_tracked_track_tracked_helix_vertex_y_error_.push_back(hvertex.y().error());
	simulated_vs_tracked_track_tracked_helix_vertex_z_error_.push_back(hvertex.z().error());
	simulated_vs_tracked_track_tracked_helix_vertex_phi_error_.push_back(vhvertex.phi().error());
      }
      else if( iseq->helix_vertex_type() == "calo" ){
	simulated_vs_tracked_track_tracked_helix_decay_vertex_x_.push_back(hvertex.x().value());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_y_.push_back(hvertex.y().value());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_z_.push_back(hvertex.z().value());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_.push_back(fabs(vhvertex.phi().value()));
      
	simulated_vs_tracked_track_tracked_helix_decay_vertex_x_error_.push_back(hvertex.x().error());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_y_error_.push_back(hvertex.y().error());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_z_error_.push_back(hvertex.z().error());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_error_.push_back(vhvertex.phi().error());
      }
      else if( iseq->helix_vertex_type() == "kink" ){
	simulated_vs_tracked_track_tracked_kink_x_.push_back(hvertex.x().value());
	simulated_vs_tracked_track_tracked_kink_y_.push_back(hvertex.y().value());
	simulated_vs_tracked_track_tracked_kink_z_.push_back(hvertex.z().value());
      
	simulated_vs_tracked_track_tracked_kink_x_error_.push_back(hvertex.x().error());
	simulated_vs_tracked_track_tracked_kink_y_error_.push_back(hvertex.y().error());
	simulated_vs_tracked_track_tracked_kink_z_error_.push_back(hvertex.z().error());
      }
      
      if( iseq->tangent_vertex_type() == "foil" ){
	simulated_vs_tracked_track_tracked_tangent_vertex_x_.push_back(tvertex.x().value());
	simulated_vs_tracked_track_tracked_tangent_vertex_y_.push_back(tvertex.y().value());
	simulated_vs_tracked_track_tracked_tangent_vertex_z_.push_back(tvertex.z().value());
	simulated_vs_tracked_track_tracked_tangent_vertex_phi_.push_back(fabs(vtvertex.phi().value()));
	simulated_vs_tracked_track_tracked_tangent_vertex_x_error_.push_back(tvertex.x().error());
	simulated_vs_tracked_track_tracked_tangent_vertex_y_error_.push_back(tvertex.y().error());
	simulated_vs_tracked_track_tracked_tangent_vertex_z_error_.push_back(tvertex.z().error());
	simulated_vs_tracked_track_tracked_tangent_vertex_phi_error_.push_back(vtvertex.phi().error());
      }
      else if( iseq->tangent_vertex_type() == "calo" ){
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_.push_back(tvertex.x().value());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_.push_back(tvertex.y().value());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_.push_back(tvertex.z().value());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_.push_back(fabs(vtvertex.phi().value()));
      
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_error_.push_back(tvertex.x().error());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_error_.push_back(tvertex.y().error());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_error_.push_back(tvertex.z().error());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_error_.push_back(vtvertex.phi().error());
      }


      CAT::topology::experimental_point hdvertex=iseq->decay_helix_vertex();
      CAT::topology::experimental_point tdvertex=iseq->decay_tangent_vertex();
      CAT::topology::experimental_vector vhdvertex(hdvertex);
      CAT::topology::experimental_vector vtdvertex(tdvertex);

      if( iseq->decay_helix_vertex_type() == "foil" ){
	simulated_vs_tracked_track_tracked_helix_vertex_x_.push_back(hdvertex.x().value());
	simulated_vs_tracked_track_tracked_helix_vertex_y_.push_back(hdvertex.y().value());
	simulated_vs_tracked_track_tracked_helix_vertex_z_.push_back(hdvertex.z().value());
	simulated_vs_tracked_track_tracked_helix_vertex_phi_.push_back(fabs(vhdvertex.phi().value()));
      
	simulated_vs_tracked_track_tracked_helix_vertex_x_error_.push_back(hdvertex.x().error());
	simulated_vs_tracked_track_tracked_helix_vertex_y_error_.push_back(hdvertex.y().error());
	simulated_vs_tracked_track_tracked_helix_vertex_z_error_.push_back(hdvertex.z().error());
	simulated_vs_tracked_track_tracked_helix_vertex_phi_error_.push_back(vhdvertex.phi().error());
      }
      else if( iseq->helix_vertex_type() == "calo" ){
	simulated_vs_tracked_track_tracked_helix_decay_vertex_x_.push_back(hdvertex.x().value());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_y_.push_back(hdvertex.y().value());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_z_.push_back(hdvertex.z().value());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_.push_back(fabs(vhdvertex.phi().value()));
      
	simulated_vs_tracked_track_tracked_helix_decay_vertex_x_error_.push_back(hdvertex.x().error());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_y_error_.push_back(hdvertex.y().error());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_z_error_.push_back(hdvertex.z().error());
	simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_error_.push_back(vhdvertex.phi().error());
      }
      else if( iseq->helix_vertex_type() == "kink" ){
	simulated_vs_tracked_track_tracked_kink_x_.push_back(hdvertex.x().value());
	simulated_vs_tracked_track_tracked_kink_y_.push_back(hdvertex.y().value());
	simulated_vs_tracked_track_tracked_kink_z_.push_back(hdvertex.z().value());
      
	simulated_vs_tracked_track_tracked_kink_x_error_.push_back(hdvertex.x().error());
	simulated_vs_tracked_track_tracked_kink_y_error_.push_back(hdvertex.y().error());
	simulated_vs_tracked_track_tracked_kink_z_error_.push_back(hdvertex.z().error());
      }

      if( iseq->decay_tangent_vertex_type() == "foil" ){
	simulated_vs_tracked_track_tracked_tangent_vertex_x_.push_back(tdvertex.x().value());
	simulated_vs_tracked_track_tracked_tangent_vertex_y_.push_back(tdvertex.y().value());
	simulated_vs_tracked_track_tracked_tangent_vertex_z_.push_back(tdvertex.z().value());
	simulated_vs_tracked_track_tracked_tangent_vertex_phi_.push_back(fabs(vtdvertex.phi().value()));
	simulated_vs_tracked_track_tracked_tangent_vertex_x_error_.push_back(tdvertex.x().error());
	simulated_vs_tracked_track_tracked_tangent_vertex_y_error_.push_back(tdvertex.y().error());
	simulated_vs_tracked_track_tracked_tangent_vertex_z_error_.push_back(tdvertex.z().error());
	simulated_vs_tracked_track_tracked_tangent_vertex_phi_error_.push_back(vtdvertex.phi().error());
      }
      else if( iseq->tangent_vertex_type() == "calo" ){
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_.push_back(tdvertex.x().value());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_.push_back(tdvertex.y().value());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_.push_back(tdvertex.z().value());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_.push_back(fabs(vtdvertex.phi().value()));
      
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_error_.push_back(tdvertex.x().error());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_error_.push_back(tdvertex.y().error());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_error_.push_back(tdvertex.z().error());
	simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_error_.push_back(vtdvertex.phi().error());
      }

    }

  }



  if( !tracked_data_.nemo_sequences_.empty() ){

    for(std::vector<CAT::topology::sequence>::iterator iseq=tracked_data_.nemo_sequences_.begin(); iseq!=tracked_data_.nemo_sequences_.end(); iseq++){

      CAT::topology::experimental_point hvertex=iseq->helix_vertex();
      CAT::topology::experimental_vector vhvertex(hvertex);
		

      simulated_vs_tracked_n3track_tracked_helix_vertex_x_.push_back(hvertex.x().value());
      simulated_vs_tracked_n3track_tracked_helix_vertex_y_.push_back(hvertex.y().value());
      simulated_vs_tracked_n3track_tracked_helix_vertex_z_.push_back(hvertex.z().value());
      simulated_vs_tracked_n3track_tracked_helix_vertex_phi_.push_back(fabs(vhvertex.phi().value()));

      simulated_vs_tracked_n3track_tracked_helix_vertex_x_error_.push_back(hvertex.x().error());
      simulated_vs_tracked_n3track_tracked_helix_vertex_y_error_.push_back(hvertex.y().error());
      simulated_vs_tracked_n3track_tracked_helix_vertex_z_error_.push_back(hvertex.z().error());
      simulated_vs_tracked_n3track_tracked_helix_vertex_phi_error_.push_back(vhvertex.phi().error());
    }
  }



  simulated_vs_tracked_hit_.id_ = &simulated_vs_tracked_hit_id_;
#if 0
  simulated_vs_tracked_hit_.simulated_x_ =&simulated_vs_tracked_simulated_x_;
  simulated_vs_tracked_hit_.simulated_y_ =&simulated_vs_tracked_simulated_y_;
  simulated_vs_tracked_hit_.simulated_z_ =&simulated_vs_tracked_simulated_z_;
  simulated_vs_tracked_hit_.simulated_dx_ =&simulated_vs_tracked_simulated_dx_;
  simulated_vs_tracked_hit_.simulated_dz_ =&simulated_vs_tracked_simulated_dz_;
#endif

  simulated_vs_tracked_hit_.tracked_tangency_x_ =&simulated_vs_tracked_tracked_tangency_x_;
  simulated_vs_tracked_hit_.tracked_tangency_y_ =&simulated_vs_tracked_tracked_tangency_y_;
  simulated_vs_tracked_hit_.tracked_tangency_z_ =&simulated_vs_tracked_tracked_tangency_z_;
  simulated_vs_tracked_hit_.tracked_tangency_x_error_ =&simulated_vs_tracked_tracked_tangency_x_error_;
  simulated_vs_tracked_hit_.tracked_tangency_y_error_ =&simulated_vs_tracked_tracked_tangency_y_error_;
  simulated_vs_tracked_hit_.tracked_tangency_z_error_ =&simulated_vs_tracked_tracked_tangency_z_error_;

  simulated_vs_tracked_hit_.tracked_tangency_dx_ =&simulated_vs_tracked_tracked_tangency_dx_;
  simulated_vs_tracked_hit_.tracked_tangency_dz_ =&simulated_vs_tracked_tracked_tangency_dz_;
#if 0
  simulated_vs_tracked_hit_.residual_tangency_x_ =&simulated_vs_tracked_residual_tangency_x_;
  simulated_vs_tracked_hit_.residual_tangency_y_ =&simulated_vs_tracked_residual_tangency_y_;
  simulated_vs_tracked_hit_.residual_tangency_z_ =&simulated_vs_tracked_residual_tangency_z_;
  simulated_vs_tracked_hit_.pull_tangency_x_ =&simulated_vs_tracked_pull_tangency_x_;
  simulated_vs_tracked_hit_.pull_tangency_y_ =&simulated_vs_tracked_pull_tangency_y_;
  simulated_vs_tracked_hit_.pull_tangency_z_ =&simulated_vs_tracked_pull_tangency_z_;
#endif

  simulated_vs_tracked_hit_.tracked_helix_x_ =&simulated_vs_tracked_tracked_helix_x_;
  simulated_vs_tracked_hit_.tracked_helix_y_ =&simulated_vs_tracked_tracked_helix_y_;
  simulated_vs_tracked_hit_.tracked_helix_z_ =&simulated_vs_tracked_tracked_helix_z_;
  simulated_vs_tracked_hit_.tracked_helix_x_error_ =&simulated_vs_tracked_tracked_helix_x_error_;
  simulated_vs_tracked_hit_.tracked_helix_y_error_ =&simulated_vs_tracked_tracked_helix_y_error_;
  simulated_vs_tracked_hit_.tracked_helix_z_error_ =&simulated_vs_tracked_tracked_helix_z_error_;
  simulated_vs_tracked_hit_.tracked_helix_dx_ =&simulated_vs_tracked_tracked_helix_dx_;
  simulated_vs_tracked_hit_.tracked_helix_dz_ =&simulated_vs_tracked_tracked_helix_dz_;

#if 0
  simulated_vs_tracked_hit_.residual_helix_x_ =&simulated_vs_tracked_residual_helix_x_;
  simulated_vs_tracked_hit_.residual_helix_y_ =&simulated_vs_tracked_residual_helix_y_;
  simulated_vs_tracked_hit_.residual_helix_z_ =&simulated_vs_tracked_residual_helix_z_;
  simulated_vs_tracked_hit_.pull_helix_x_ =&simulated_vs_tracked_pull_helix_x_;
  simulated_vs_tracked_hit_.pull_helix_y_ =&simulated_vs_tracked_pull_helix_y_;
  simulated_vs_tracked_hit_.pull_helix_z_ =&simulated_vs_tracked_pull_helix_z_;
#endif

#if 0
  simulated_vs_tracked_hit_.simulated_length_ =&simulated_vs_tracked_simulated_length_;
  simulated_vs_tracked_hit_.simulated_phi_ =&simulated_vs_tracked_simulated_phi_;
  simulated_vs_tracked_hit_.simulated_theta_ =&simulated_vs_tracked_simulated_theta_;
  simulated_vs_tracked_hit_.simulated_kink_hor_ =&simulated_vs_tracked_simulated_kink_hor_;
  simulated_vs_tracked_hit_.simulated_kink_vert_ =&simulated_vs_tracked_simulated_kink_vert_;
#endif

  simulated_vs_tracked_hit_.tracked_length_ =&simulated_vs_tracked_tracked_length_;
  simulated_vs_tracked_hit_.tracked_phi_ =&simulated_vs_tracked_tracked_phi_;
  simulated_vs_tracked_hit_.tracked_theta_ =&simulated_vs_tracked_tracked_theta_;
  simulated_vs_tracked_hit_.tracked_kink_hor_ =&simulated_vs_tracked_tracked_kink_hor_;
  simulated_vs_tracked_hit_.tracked_kink_vert_ =&simulated_vs_tracked_tracked_kink_vert_;
					
#if 0
  simulated_vs_tracked_track_.id_ = &simulated_vs_tracked_track_id_;
  simulated_vs_tracked_track_.simulated_vertex_x_ = &simulated_vs_tracked_track_simulated_vertex_x_;
  simulated_vs_tracked_track_.simulated_vertex_y_ = &simulated_vs_tracked_track_simulated_vertex_y_;
  simulated_vs_tracked_track_.simulated_vertex_z_ = &simulated_vs_tracked_track_simulated_vertex_z_;
  simulated_vs_tracked_track_.simulated_vertex_phi_ = &simulated_vs_tracked_track_simulated_vertex_phi_;
#endif

  simulated_vs_tracked_track_.tracked_helix_vertex_x_ = &simulated_vs_tracked_track_tracked_helix_vertex_x_;
  simulated_vs_tracked_track_.tracked_helix_vertex_y_ = &simulated_vs_tracked_track_tracked_helix_vertex_y_;
  simulated_vs_tracked_track_.tracked_helix_vertex_z_ = &simulated_vs_tracked_track_tracked_helix_vertex_z_;
  simulated_vs_tracked_track_.tracked_helix_vertex_phi_ = &simulated_vs_tracked_track_tracked_helix_vertex_phi_;
  simulated_vs_tracked_track_.tracked_helix_vertex_x_error_ = &simulated_vs_tracked_track_tracked_helix_vertex_x_error_;
  simulated_vs_tracked_track_.tracked_helix_vertex_y_error_ = &simulated_vs_tracked_track_tracked_helix_vertex_y_error_;
  simulated_vs_tracked_track_.tracked_helix_vertex_z_error_ = &simulated_vs_tracked_track_tracked_helix_vertex_z_error_;
  simulated_vs_tracked_track_.tracked_helix_vertex_phi_error_ = &simulated_vs_tracked_track_tracked_helix_vertex_phi_error_;

#if 0
  simulated_vs_tracked_track_.residual_helix_vertex_x_ = &simulated_vs_tracked_track_residual_helix_vertex_x_;
  simulated_vs_tracked_track_.residual_helix_vertex_y_ = &simulated_vs_tracked_track_residual_helix_vertex_y_;
  simulated_vs_tracked_track_.residual_helix_vertex_z_ = &simulated_vs_tracked_track_residual_helix_vertex_z_;
  simulated_vs_tracked_track_.residual_helix_vertex_phi_ = &simulated_vs_tracked_track_residual_helix_vertex_phi_;
  simulated_vs_tracked_track_.pull_helix_vertex_x_ = &simulated_vs_tracked_track_pull_helix_vertex_x_;
  simulated_vs_tracked_track_.pull_helix_vertex_y_ = &simulated_vs_tracked_track_pull_helix_vertex_y_;
  simulated_vs_tracked_track_.pull_helix_vertex_z_ = &simulated_vs_tracked_track_pull_helix_vertex_z_;
  simulated_vs_tracked_track_.pull_helix_vertex_phi_ = &simulated_vs_tracked_track_pull_helix_vertex_phi_;
  simulated_vs_tracked_track_.simulated_decay_vertex_x_ = &simulated_vs_tracked_track_simulated_decay_vertex_x_;
  simulated_vs_tracked_track_.simulated_decay_vertex_y_ = &simulated_vs_tracked_track_simulated_decay_vertex_y_;
  simulated_vs_tracked_track_.simulated_decay_vertex_z_ = &simulated_vs_tracked_track_simulated_decay_vertex_z_;
#endif

  simulated_vs_tracked_track_.tracked_helix_decay_vertex_x_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_x_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_y_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_y_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_z_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_z_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_phi_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_x_error_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_x_error_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_y_error_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_y_error_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_z_error_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_z_error_;
  simulated_vs_tracked_track_.tracked_helix_decay_vertex_phi_error_ = &simulated_vs_tracked_track_tracked_helix_decay_vertex_phi_error_;

#if 0
  simulated_vs_tracked_track_.residual_helix_decay_vertex_x_ = &simulated_vs_tracked_track_residual_helix_decay_vertex_x_;
  simulated_vs_tracked_track_.residual_helix_decay_vertex_y_ = &simulated_vs_tracked_track_residual_helix_decay_vertex_y_;
  simulated_vs_tracked_track_.residual_helix_decay_vertex_z_ = &simulated_vs_tracked_track_residual_helix_decay_vertex_z_;
  simulated_vs_tracked_track_.pull_helix_decay_vertex_x_ = &simulated_vs_tracked_track_pull_helix_decay_vertex_x_;
  simulated_vs_tracked_track_.pull_helix_decay_vertex_y_ = &simulated_vs_tracked_track_pull_helix_decay_vertex_y_;
  simulated_vs_tracked_track_.pull_helix_decay_vertex_z_ = &simulated_vs_tracked_track_pull_helix_decay_vertex_z_;
#endif

  simulated_vs_tracked_track_.tracked_tangent_vertex_x_ = &simulated_vs_tracked_track_tracked_tangent_vertex_x_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_y_ = &simulated_vs_tracked_track_tracked_tangent_vertex_y_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_z_ = &simulated_vs_tracked_track_tracked_tangent_vertex_z_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_phi_ = &simulated_vs_tracked_track_tracked_tangent_vertex_phi_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_x_error_ = &simulated_vs_tracked_track_tracked_tangent_vertex_x_error_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_y_error_ = &simulated_vs_tracked_track_tracked_tangent_vertex_y_error_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_z_error_ = &simulated_vs_tracked_track_tracked_tangent_vertex_z_error_;
  simulated_vs_tracked_track_.tracked_tangent_vertex_phi_error_ = &simulated_vs_tracked_track_tracked_tangent_vertex_phi_error_;

#if 0
  simulated_vs_tracked_track_.residual_tangent_vertex_x_ = &simulated_vs_tracked_track_residual_tangent_vertex_x_;
  simulated_vs_tracked_track_.residual_tangent_vertex_y_ = &simulated_vs_tracked_track_residual_tangent_vertex_y_;
  simulated_vs_tracked_track_.residual_tangent_vertex_z_ = &simulated_vs_tracked_track_residual_tangent_vertex_z_;
  simulated_vs_tracked_track_.residual_tangent_vertex_phi_ = &simulated_vs_tracked_track_residual_tangent_vertex_phi_;
  simulated_vs_tracked_track_.pull_tangent_vertex_x_ = &simulated_vs_tracked_track_pull_tangent_vertex_x_;
  simulated_vs_tracked_track_.pull_tangent_vertex_y_ = &simulated_vs_tracked_track_pull_tangent_vertex_y_;
  simulated_vs_tracked_track_.pull_tangent_vertex_z_ = &simulated_vs_tracked_track_pull_tangent_vertex_z_;
  simulated_vs_tracked_track_.pull_tangent_vertex_phi_ = &simulated_vs_tracked_track_pull_tangent_vertex_phi_;
#endif

  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_x_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_y_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_z_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_phi_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_x_error_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_x_error_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_y_error_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_y_error_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_z_error_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_z_error_;
  simulated_vs_tracked_track_.tracked_tangent_decay_vertex_phi_error_ = &simulated_vs_tracked_track_tracked_tangent_decay_vertex_phi_error_;

#if 0
  simulated_vs_tracked_track_.residual_tangent_decay_vertex_x_ = &simulated_vs_tracked_track_residual_tangent_decay_vertex_x_;
  simulated_vs_tracked_track_.residual_tangent_decay_vertex_y_ = &simulated_vs_tracked_track_residual_tangent_decay_vertex_y_;
  simulated_vs_tracked_track_.residual_tangent_decay_vertex_z_ = &simulated_vs_tracked_track_residual_tangent_decay_vertex_z_;
  simulated_vs_tracked_track_.pull_tangent_decay_vertex_x_ = &simulated_vs_tracked_track_pull_tangent_decay_vertex_x_;
  simulated_vs_tracked_track_.pull_tangent_decay_vertex_y_ = &simulated_vs_tracked_track_pull_tangent_decay_vertex_y_;
  simulated_vs_tracked_track_.pull_tangent_decay_vertex_z_ = &simulated_vs_tracked_track_pull_tangent_decay_vertex_z_;
#endif

  simulated_vs_tracked_track_.tracked_kink_x_ = &simulated_vs_tracked_track_tracked_kink_x_;
  simulated_vs_tracked_track_.tracked_kink_y_ = &simulated_vs_tracked_track_tracked_kink_y_;
  simulated_vs_tracked_track_.tracked_kink_z_ = &simulated_vs_tracked_track_tracked_kink_z_;
  simulated_vs_tracked_track_.tracked_kink_x_error_ = &simulated_vs_tracked_track_tracked_kink_x_error_;
  simulated_vs_tracked_track_.tracked_kink_y_error_ = &simulated_vs_tracked_track_tracked_kink_y_error_;
  simulated_vs_tracked_track_.tracked_kink_z_error_ = &simulated_vs_tracked_track_tracked_kink_z_error_;

#if 0
  simulated_vs_tracked_track_.residual_kink_x_ = &simulated_vs_tracked_track_residual_kink_x_;
  simulated_vs_tracked_track_.residual_kink_y_ = &simulated_vs_tracked_track_residual_kink_y_;
  simulated_vs_tracked_track_.residual_kink_z_ = &simulated_vs_tracked_track_residual_kink_z_;
  simulated_vs_tracked_track_.pull_kink_x_ = &simulated_vs_tracked_track_pull_kink_x_;
  simulated_vs_tracked_track_.pull_kink_y_ = &simulated_vs_tracked_track_pull_kink_y_;
  simulated_vs_tracked_track_.pull_kink_z_ = &simulated_vs_tracked_track_pull_kink_z_;
#endif
				
#if 0
  simulated_vs_tracked_n3track_.id_ = &simulated_vs_tracked_n3track_id_;
  simulated_vs_tracked_n3track_.simulated_vertex_x_ = &simulated_vs_tracked_n3track_simulated_vertex_x_;
  simulated_vs_tracked_n3track_.simulated_vertex_y_ = &simulated_vs_tracked_n3track_simulated_vertex_y_;
  simulated_vs_tracked_n3track_.simulated_vertex_z_ = &simulated_vs_tracked_n3track_simulated_vertex_z_;
  simulated_vs_tracked_n3track_.simulated_vertex_phi_ = &simulated_vs_tracked_n3track_simulated_vertex_phi_;
#endif

  simulated_vs_tracked_n3track_.tracked_helix_vertex_x_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_x_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_y_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_y_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_z_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_z_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_phi_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_phi_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_x_error_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_x_error_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_y_error_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_y_error_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_z_error_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_z_error_;
  simulated_vs_tracked_n3track_.tracked_helix_vertex_phi_error_ = &simulated_vs_tracked_n3track_tracked_helix_vertex_phi_error_;

#if 0
  simulated_vs_tracked_n3track_.residual_helix_vertex_x_ = &simulated_vs_tracked_n3track_residual_helix_vertex_x_;
  simulated_vs_tracked_n3track_.residual_helix_vertex_y_ = &simulated_vs_tracked_n3track_residual_helix_vertex_y_;
  simulated_vs_tracked_n3track_.residual_helix_vertex_z_ = &simulated_vs_tracked_n3track_residual_helix_vertex_z_;
  simulated_vs_tracked_n3track_.residual_helix_vertex_phi_ = &simulated_vs_tracked_n3track_residual_helix_vertex_phi_;
  simulated_vs_tracked_n3track_.pull_helix_vertex_x_ = &simulated_vs_tracked_n3track_pull_helix_vertex_x_;
  simulated_vs_tracked_n3track_.pull_helix_vertex_y_ = &simulated_vs_tracked_n3track_pull_helix_vertex_y_;
  simulated_vs_tracked_n3track_.pull_helix_vertex_z_ = &simulated_vs_tracked_n3track_pull_helix_vertex_z_;
  simulated_vs_tracked_n3track_.pull_helix_vertex_phi_ = &simulated_vs_tracked_n3track_pull_helix_vertex_phi_;
  simulated_vs_tracked_n3track_.simulated_decay_vertex_x_ = &simulated_vs_tracked_n3track_simulated_decay_vertex_x_;
  simulated_vs_tracked_n3track_.simulated_decay_vertex_y_ = &simulated_vs_tracked_n3track_simulated_decay_vertex_y_;
  simulated_vs_tracked_n3track_.simulated_decay_vertex_z_ = &simulated_vs_tracked_n3track_simulated_decay_vertex_z_;
  simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_x_ = &simulated_vs_tracked_n3track_tracked_helix_decay_vertex_x_;
  simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_y_ = &simulated_vs_tracked_n3track_tracked_helix_decay_vertex_y_;
  simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_z_ = &simulated_vs_tracked_n3track_tracked_helix_decay_vertex_z_;
  simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_x_error_ = &simulated_vs_tracked_n3track_tracked_helix_decay_vertex_x_error_;
  simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_y_error_ = &simulated_vs_tracked_n3track_tracked_helix_decay_vertex_y_error_;
  simulated_vs_tracked_n3track_.tracked_helix_decay_vertex_z_error_ = &simulated_vs_tracked_n3track_tracked_helix_decay_vertex_z_error_;
  simulated_vs_tracked_n3track_.residual_helix_decay_vertex_x_ = &simulated_vs_tracked_n3track_residual_helix_decay_vertex_x_;
  simulated_vs_tracked_n3track_.residual_helix_decay_vertex_y_ = &simulated_vs_tracked_n3track_residual_helix_decay_vertex_y_;
  simulated_vs_tracked_n3track_.residual_helix_decay_vertex_z_ = &simulated_vs_tracked_n3track_residual_helix_decay_vertex_z_;
  simulated_vs_tracked_n3track_.pull_helix_decay_vertex_x_ = &simulated_vs_tracked_n3track_pull_helix_decay_vertex_x_;
  simulated_vs_tracked_n3track_.pull_helix_decay_vertex_y_ = &simulated_vs_tracked_n3track_pull_helix_decay_vertex_y_;
  simulated_vs_tracked_n3track_.pull_helix_decay_vertex_z_ = &simulated_vs_tracked_n3track_pull_helix_decay_vertex_z_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_x_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_x_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_y_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_y_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_z_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_z_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_phi_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_phi_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_x_error_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_x_error_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_y_error_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_y_error_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_z_error_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_z_error_;
  simulated_vs_tracked_n3track_.tracked_tangent_vertex_phi_error_ = &simulated_vs_tracked_n3track_tracked_tangent_vertex_phi_error_;
  simulated_vs_tracked_n3track_.residual_tangent_vertex_x_ = &simulated_vs_tracked_n3track_residual_tangent_vertex_x_;
  simulated_vs_tracked_n3track_.residual_tangent_vertex_y_ = &simulated_vs_tracked_n3track_residual_tangent_vertex_y_;
  simulated_vs_tracked_n3track_.residual_tangent_vertex_z_ = &simulated_vs_tracked_n3track_residual_tangent_vertex_z_;
  simulated_vs_tracked_n3track_.residual_tangent_vertex_phi_ = &simulated_vs_tracked_n3track_residual_tangent_vertex_phi_;
  simulated_vs_tracked_n3track_.pull_tangent_vertex_x_ = &simulated_vs_tracked_n3track_pull_tangent_vertex_x_;
  simulated_vs_tracked_n3track_.pull_tangent_vertex_y_ = &simulated_vs_tracked_n3track_pull_tangent_vertex_y_;
  simulated_vs_tracked_n3track_.pull_tangent_vertex_z_ = &simulated_vs_tracked_n3track_pull_tangent_vertex_z_;
  simulated_vs_tracked_n3track_.pull_tangent_vertex_phi_ = &simulated_vs_tracked_n3track_pull_tangent_vertex_phi_;
  simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_x_ = &simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_x_;
  simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_y_ = &simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_y_;
  simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_z_ = &simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_z_;
  simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_x_error_ = &simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_x_error_;
  simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_y_error_ = &simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_y_error_;
  simulated_vs_tracked_n3track_.tracked_tangent_decay_vertex_z_error_ = &simulated_vs_tracked_n3track_tracked_tangent_decay_vertex_z_error_;
  simulated_vs_tracked_n3track_.residual_tangent_decay_vertex_x_ = &simulated_vs_tracked_n3track_residual_tangent_decay_vertex_x_;
  simulated_vs_tracked_n3track_.residual_tangent_decay_vertex_y_ = &simulated_vs_tracked_n3track_residual_tangent_decay_vertex_y_;
  simulated_vs_tracked_n3track_.residual_tangent_decay_vertex_z_ = &simulated_vs_tracked_n3track_residual_tangent_decay_vertex_z_;
  simulated_vs_tracked_n3track_.pull_tangent_decay_vertex_x_ = &simulated_vs_tracked_n3track_pull_tangent_decay_vertex_x_;
  simulated_vs_tracked_n3track_.pull_tangent_decay_vertex_y_ = &simulated_vs_tracked_n3track_pull_tangent_decay_vertex_y_;
  simulated_vs_tracked_n3track_.pull_tangent_decay_vertex_z_ = &simulated_vs_tracked_n3track_pull_tangent_decay_vertex_z_;
#endif


  if( __tree ){
    __tree->Fill();
  }
  
  return;

}

//________________________________________________________________________
bool ntupler::get_true_hit_of_reco_cell(CAT::topology::cell c, CAT::topology::node& n, size_t& index){


  for(std::vector<CAT::topology::sequence>::iterator iseq = tracked_data_.true_sequences_.begin();
      iseq != tracked_data_.true_sequences_.end(); ++iseq){

    for(std::vector<CAT::topology::node>::iterator inode = iseq->nodes_.begin(); inode != iseq->nodes_.end();
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
size_t ntupler::get_cell_index(CAT::topology::cell c){


  for(std::vector<CAT::topology::cell>::iterator icell = tracked_data_.cells_.begin();
      icell != tracked_data_.cells_.end(); ++icell){
    if( c.same_cell(*icell) ) return icell - tracked_data_.cells_.begin();
  }

  cout << " problem: ntupler could not find cell index corresponding to cell "; c.dump();
  fflush(stdout);

  return 0;


}


// end of ntupler.cpp


