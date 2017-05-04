// -*- mode: c++; -*-

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <CLHEP/Units/SystemOfUnits.h>
#include <CATAlgorithm/CAT_clusterizer_interface.h>

void plot_cell (std::ostream & out_, 
                const CAT::topology::cell & c_)
{
  double r = c_.r ().value() * CLHEP::mm;
  double er = c_.r ().error() * CLHEP::mm;
  double x0 = c_.ep().z().value() * CLHEP::mm;
  double y0 = c_.ep().x().value() * CLHEP::mm;
  double z0 = c_.ep().y().value() * CLHEP::mm;
  double ez0 = c_.ep().y().error() * CLHEP::mm;
  for (int j = 0; j <= 36; ++j)
    {
      double theta = 2. * M_PI/36. * j;
      double x = x0 + (r+er) * std::cos (theta);
      double y = y0 + (r+er) * std::sin (theta);
      double z = z0;
      out_ << x << ' ' << y << ' ' << z << std::endl;
    }
  out_ << std::endl;
  for (int j = 0; j <= 36; ++j)
    {
      double theta = 2. * M_PI/36. * j;
      double x = x0 + (r-er) * std::cos (theta);
      double y = y0 + (r-er) * std::sin (theta);
      double z = z0;
      out_ << x << ' ' << y << ' ' << z << std::endl;
    }
  out_ << std::endl;
  out_ << x0 << ' ' << y0 << ' ' << z0 + ez0 << std::endl;
  out_ << x0 << ' ' << y0 << ' ' << z0 - ez0 << std::endl;
  out_ << std::endl;
  return;
}

void plot_cluster (std::ostream & out_, 
                   const CAT::topology::cluster & c_, 
                   double cell_size_)
{
  /*
    std::clog << "plot_cluster: Number of nodes: " 
    << c_.nodes ().size () << std::endl;
  */
  int count = 0;
  for(std::vector<CAT::topology::node>::const_iterator inode 
        = c_.nodes ().begin(); 
      inode != c_.nodes_.end(); 
      ++inode)
    {
      count ++;
      const CAT::topology::node & n = *inode;
      double s = cell_size_ / 2;
      double x0 = n.c().ep().z().value() * CLHEP::mm;
      double y0 = n.c().ep().x().value() * CLHEP::mm;
      double z0 = n.c().ep().y().value() * CLHEP::mm;
      out_ << x0 + s  << ' ' << y0 + s << ' ' << z0 << std::endl;
      if (count%2)
        {
          // gnuplot 3D-plot trick
          out_ << x0 + s  << ' ' << y0   << ' ' << z0 << std::endl;
        }
      out_ << x0 + s << ' ' << y0 - s << ' ' << z0 << std::endl;
      out_ << x0 - s  << ' ' << y0 - s << ' ' << z0 << std::endl;
      out_ << x0 - s  << ' ' << y0 + s << ' ' << z0 << std::endl;
      out_ << x0 + s  << ' ' << y0 + s << ' ' << z0 << std::endl;
      out_ << std::endl;
    }
  return;
}

int main (void)
{
  using namespace std;
  
  clog << "CAT client example program." << endl;

  /*** setup ***/
  long seed = 314159;
  bool spurious_hits = false;


  /*** start ***/
  srand48(seed);

  CAT::clusterizer_setup_data CAT_setup;

  // General parameters :
  CAT_setup.MaxTime = 5e-3  * CLHEP::s;
  CAT_setup.level   = "normal"; //verbosity

  // Algorithm parameters :
  CAT_setup.SmallRadius   = 1.e-3 * CLHEP::mm;
  CAT_setup.TangentPhi    = 20. * CLHEP::degree;
  CAT_setup.TangentTheta  = 160. * CLHEP::degree;
  CAT_setup.SmallNumber   = 0.1 * CLHEP::mm;
  CAT_setup.QuadrantAngle = 90. * CLHEP::degree;
  CAT_setup.Ratio         = 10.;
  CAT_setup.CompatibilityDistance = 4.0 * CLHEP::mm;
  CAT_setup.MaxChi2       = 3.;
  CAT_setup.nsigma        = 4.;
  CAT_setup.nofflayers    = 1;

  // Geometry description :
  CAT_setup.num_blocks = 1;
  CAT_setup.planes_per_block.clear ();
  CAT_setup.planes_per_block.push_back (CAT_setup.num_blocks);
  CAT_setup.planes_per_block.at (0) = 9;
  CAT_setup.num_cells_per_plane     = 113;
  CAT_setup.cell_size        =   44.0  * CLHEP::mm;
 
  clog << "NOTICE: Creating the clustering machine..." << endl;
  CAT::clusterizer clustering_machine;
  clog << "NOTICE: Clustering machine is created." << endl;

  clog << "NOTICE: Configuring the clustering machine..." << endl;
  CAT::clusterizer_configure (clustering_machine, CAT_setup);
  clog << "NOTICE: Clustering machine is configured." << endl;

  clog << "NOTICE: Initializing the clustering machine..." << endl;
  clustering_machine.initialize();
  clog << "NOTICE: Clustering machine is initialized." << endl;

  // Namespace shortcut :
  namespace ct = CAT::topology;

  double src_gap = 10.* CLHEP::mm;

  // Declare an input data model : 
  CAT::clusterizer_input_data CAT_input;
  int ihit = 0;

  // A fake track :
  for (int i = 0; i < 10; i++)
    {
      // X-Y position of the anode wire of the hit cell :
      ct::experimental_double x; // == Y in sngeometry
      ct::experimental_double z; // == X in sngeometry
      int block_id = 1;
      int layer_id = i/2;
      int cell_id  = i;
      z.set_value (src_gap + (layer_id + 0.5)* CAT_setup.cell_size);
      z.set_error (0.0);
      x.set_value (10.*CLHEP::cm + cell_id * CAT_setup.cell_size);
      x.set_error (0.0);

      // Prompt hit :
      bool fast = true;

      // Transverse Geiger drift distance :
      ct::experimental_double y ;// == Z in sngeometry
      double rdrift = 0.5 * CAT_setup.cell_size * drand48();
      double rdrift_err = 0.2 * CLHEP::mm;

      // Plasma longitudinal origin along the anode wire :
      y.set_value (50.*CLHEP::cm + i*4.*CLHEP::cm);
      y.set_error (1.*CLHEP::cm);
     
      // Build the Geiger hit position :
      ct::experimental_point gg_hit_position (x,y,z);

      // Add a new cell in the CAT input data model :
      ct::cell & c = CAT_input.add_cell ();
      c.set_print_level(mybhep::NORMAL);
      c.set_type ("SN");
      c.set_id (ihit++); /** Each GG hit must have an unique ID starting from 0 up to N-1
                          *  where N is the total number of hits.
                          */
      c.set_nsigma (10.);
      c.set_p (gg_hit_position);
      c.set_r (rdrift);
      c.set_er (rdrift_err);
      c.set_layer (layer_id);
      c.set_block (block_id);
      c.set_iid (cell_id);
      c.set_fast (fast);
    }

  // Another fake track :
  ct::cell cmore; 
  for (int i = 0; i < 5; i++)
    {
      // X-Y position of the anode wire of the hit cell :
      ct::experimental_double x;
      ct::experimental_double z;
      int block_id = 1;
      int layer_id = i;
      int cell_id  = i/2;
      z.set_value (src_gap + (layer_id + 0.5) * CAT_setup.cell_size);
      z.set_error (0.0);
      x.set_value (0.*CLHEP::cm + cell_id * CAT_setup.cell_size);
      x.set_error (0.0);

      // Prompt hit :
      bool fast = true;
      //fast = false;

      // Transverse Geiger drift distance :
      ct::experimental_double y;
      double rdrift = 0.5 * CAT_setup.cell_size * drand48();
      double rdrift_err = 0.2 * CLHEP::mm;

      // Plasma longitudinal origin along the anode wire :
      y.set_value (20.*CLHEP::cm - i*4.*CLHEP::cm);
      y.set_error (1.*CLHEP::cm);
     
      // Build the Geiger hit position :
      ct::experimental_point gg_hit_position (x,y,z);

      // Add a new cell in the CAT input data model :
      ct::cell & c = CAT_input.add_cell ();
      c.set_print_level(mybhep::NORMAL);
      c.set_type ("SN");
      c.set_id (ihit++);
      c.set_nsigma (10.);
      c.set_p (gg_hit_position);
      c.set_r (rdrift);
      c.set_er (rdrift_err);
      c.set_layer (layer_id);
      c.set_block (block_id);
      c.set_iid (cell_id);
      c.set_fast (fast);
      bool add = true;
      if (add && (i == 1))
        {
          clog << "NOTICE: Add a special cell." << endl;
          
          ct::cell & c_more = CAT_input.add_cell ();
          c_more = c;
          c_more.set_id (ihit++);
          int layer_id_more = layer_id + 1;
          c_more.set_layer (layer_id_more);
          ct::experimental_double z_more;
          z_more.set_value (src_gap + (layer_id_more + 0.5) * CAT_setup.cell_size);
          z_more.set_error (0.0);
          ct::experimental_point gg_hit_position_more (x,
                                                       y,
                                                       z_more);
          c_more.set_p (gg_hit_position_more);
          double rdrift_more = rdrift;
          double rdrift_err_more = 0.15 * CLHEP::mm;
          c_more.set_r (rdrift_more);
          c_more.set_er (rdrift_err_more);
       }
    }

  // Yet another fake track :
  for (int i = 0; i < 7; i++)
    {
      // X-Y position of the anode wire of the hit cell :
      ct::experimental_double x;
      ct::experimental_double z;
      int block_id = -1;
      int layer_id = i;
      int cell_id  = -i;
      z.set_value (-(src_gap + (layer_id  + 0.5) * CAT_setup.cell_size));
      z.set_error (0.0);
      x.set_value (0.*CLHEP::cm + (cell_id) * CAT_setup.cell_size);
      x.set_error (0.0);

      // Prompt hit :
      bool fast = true;

      // Transverse Geiger drift distance :
      ct::experimental_double y;
      double rdrift = 0.5 * CAT_setup.cell_size * drand48();
      double rdrift_err = 0.2 * CLHEP::mm;

      // Plasma longitudinal origin along the anode wire :
      y.set_value (10.*CLHEP::cm + i*4.*CLHEP::cm);
      y.set_error (1.*CLHEP::cm);
     
      // Build the Geiger hit position :
      ct::experimental_point gg_hit_position (x,y,z);

      // Add a new cell in the CAT input data model :
      ct::cell & c = CAT_input.add_cell ();
      c.set_print_level(mybhep::NORMAL);
      c.set_type ("SN");
      c.set_id (ihit++);
      c.set_nsigma (10.);
      c.set_p (gg_hit_position);
      c.set_r (rdrift);
      c.set_er (rdrift_err);
      c.set_layer (layer_id);
      c.set_block (block_id);
      c.set_iid (cell_id);
      c.set_fast (fast);
    }

  // A delayed track :
  for (int i = 0; i < 3; i++)
    {
      // X-Y position of the anode wire of the hit cell :
      ct::experimental_double x;
      ct::experimental_double z;
      int block_id = -1;
      int layer_id = 3+i;
      int cell_id  = i+10;
      z.set_value (- (src_gap + (layer_id  + 0.5) * CAT_setup.cell_size));
      z.set_error (0.0);
      x.set_value (0.*CLHEP::cm + (cell_id) * CAT_setup.cell_size);
      x.set_error (0.0);

      // Delayed hit :
      bool fast = false;

      // Transverse Geiger drift distance :
      ct::experimental_double y;
      double rdrift = 0.5 * CAT_setup.cell_size * drand48();
      double rdrift_err = 0.2 * CLHEP::mm;

      // Plasma longitudinal origin along the anode wire :
      y.set_value (10.*CLHEP::cm + i*4.*CLHEP::cm);
      y.set_error (1.*CLHEP::cm);
     
      // Build the Geiger hit position :
      ct::experimental_point gg_hit_position (x,y,z);

      // Add a new cell in the CAT input data model :
      ct::cell & c = CAT_input.add_cell ();
      c.set_print_level(mybhep::NORMAL);
      c.set_type ("SN");
      c.set_id (ihit++);
      c.set_nsigma (10.);
      c.set_p (gg_hit_position);
      c.set_r (rdrift);
      c.set_er (rdrift_err);
      c.set_layer (layer_id);
      c.set_block (block_id);
      c.set_iid (cell_id);
      c.set_fast (fast);
    }

  // Some spurious hits :
  spurious_hits = true;
  if (spurious_hits)
    {
      for (int i = 0; i < 3; i++)
        {
          // X-Y position of the anode wire of the hit cell :
          ct::experimental_double x;
          ct::experimental_double z;
          int block_id = 1;
          int layer_id = 3;
          int cell_id  = -10; 
          int sign = +1;
          if (i == 1) 
            {
              cell_id = 4;
              layer_id = 6;
              sign = -1;
              block_id = -1;
            }
          if (i == 2) 
            {
              cell_id = 12;
              layer_id = 1;
              sign = -1;
              block_id = -1;
            }     
          z.set_value (sign * (src_gap + (layer_id  + 0.5) * CAT_setup.cell_size));
          z.set_error (0.0);
          x.set_value (0.*CLHEP::cm + (cell_id) * CAT_setup.cell_size);
          x.set_error (0.0);

          // Prompt hit :
          bool fast = true;

          // Transverse Geiger drift distance :
          ct::experimental_double y;
          double rdrift = 0.5 * CAT_setup.cell_size * drand48();
          double rdrift_err = 0.2 * CLHEP::mm;

          // Plasma longitudinal origin along the anode wire :
          y.set_value (10.*CLHEP::cm + i*4.*CLHEP::cm);
          y.set_error (1.*CLHEP::cm);
     
          // Build the Geiger hit position :
          ct::experimental_point gg_hit_position (x,y,z);

          // Add a new cell in the CAT input data model :
          ct::cell & c = CAT_input.add_cell ();
          c.set_print_level(mybhep::NORMAL);
          c.set_type ("SN");
          c.set_id (ihit++);
          c.set_nsigma (10.);
          c.set_p (gg_hit_position);
          c.set_r (rdrift);
          c.set_er (rdrift_err);
          c.set_layer (layer_id);
          c.set_block (block_id);
          c.set_iid (cell_id);
          c.set_fast (fast);
        }
    }

  // Validate the intput data :
  if (! CAT_input.check ())
    {
      std::clog << "ERROR: Invalid CAT input data !" << std::endl;
      return 1;
    }
  std::clog << "NOTICE: Number of hits : " << CAT_input.cells.size () << std::endl;

  // Gnuplot :
  for (int i = 0; i < CAT_input.cells.size (); i++)
    {
      const ct::cell & c = CAT_input.cells[i];
      plot_cell (std::cout, c);
    }
  std::cout << endl;
  std::cout << endl;

  // Install the input data model within the algorithm object :
  clustering_machine.set_cells(CAT_input.cells);

  // Declare an output data model : 
  CAT::clusterizer_output_data CAT_output;
  std::clog << "NOTICE: Clusterization..." << std::endl;
  clustering_machine.prepare_event(CAT_output.tracked_data);
  clustering_machine.clusterize(CAT_output.tracked_data);
  std::clog << "NOTICE: Clusterization is done." << std::endl;

  // clustering_machine.print_clusters ();

  // Then use CAT_output and its embedded clusters :
  const std::vector<ct::cluster> & tcs = CAT_output.tracked_data.get_clusters();
  size_t number_of_clusters = tcs.size();
  clog << "NOTICE: Number of clusters : " << number_of_clusters << endl;

  int icluster = 0;
  for (std::vector<ct::cluster>::const_iterator ic = tcs.begin(); 
       ic != tcs.end(); 
       ++ic)
    {
      clog << "Cluster #" << icluster++ << " : " << endl;
      const ct::cluster & cl = *ic;
      size_t clsz = cl.nodes ().size ();    
      clog << " - number of node(s) in the cluster : " << clsz << " ";
      if (clsz == 1)
        {
          clog << "ID=" << cl.nodes ()[0].c().id () << " (unclustered hit) ";
        }
      else
        {
          clog << "IDs=";
          for (int i = 0; i < clsz; i++)
            {
              clog << cl.nodes ()[i].c().id() << " ";
            }
        }
      clog  << endl;
      // Gnuplot :
      plot_cluster (cout, cl, CAT_setup.cell_size);
      cout << endl;
      cout << endl;
      
      // Navigate through the clustering solution :
      for (std::vector<ct::node>::const_iterator inode 
             = cl.nodes ().begin(); 
           inode != cl.nodes_.end(); 
           ++inode)
        {
          const ct::node & n = *inode;
          for (std::vector<ct::cell_couplet>::const_iterator icc 
                 = n.cc().begin(); 
               icc != n.cc().end(); ++icc)
            {
              const ct::cell_couplet & cc = *icc;
            }
          for (std::vector<ct::cell_triplet>::const_iterator iccc 
                 = n.ccc().begin(); 
               iccc != n.ccc().end(); ++iccc)
            { 
              const ct::cell_triplet & ccc = *iccc;
              for (std::vector<double>::const_iterator ichi = ccc.chi2s().begin(); 
                   ichi != ccc.chi2s().end(); 
                   ++ichi)
                {
                  const double & chi2 = *ichi;
                }
              for (std::vector<double>::const_iterator iprob = ccc.probs().begin(); 
                   iprob != ccc.probs().end(); 
                   ++iprob)
                {
                  const double & prob = *iprob;
                }
            }
        }
    }
  clog << "The end."<< endl;
  return 0;
}
