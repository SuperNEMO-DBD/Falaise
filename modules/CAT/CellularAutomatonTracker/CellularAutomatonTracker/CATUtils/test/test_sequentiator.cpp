

#include <CATAlgorithm/experimental_double.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/joint.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/cluster.h>


int main(int argc_, char* argv_[]){
  using namespace std;
  using namespace CAT;

  topology::experimental_double a(0.9,0.1);
  topology::experimental_double b(2.,0.2);
  topology::experimental_double c(5.,0.5);

  topology::experimental_double d(0.4,0.7);
  topology::experimental_double e(11.,0.0);
  topology::experimental_double f(0.,0.6);
  
  topology::experimental_double g(-9.,0.3);
  topology::experimental_double h(-4.1,4.1);
  topology::experimental_double i(-0.6,2.3);
  
  topology::experimental_point pa(a, b, c);
  topology::experimental_point pb(d, e, f);
  topology::experimental_point pc(g, h, i);

  topology:: experimental_vector va(a, b, c);
  topology:: experimental_vector vb(3., 4., 0., 0.3, 0.4, 0.);
  
  topology::cell ca(pa, a+b, 0);  ca.set_nsigma(10.);
  topology::cell cb(pb, d+e, 1);  cb.set_nsigma(10.);
  topology::cell cc(pc, g+h, 2);  cc.set_nsigma(10.);

  topology::line l(pa, pb); l.set_nsigma(10.);
  cout << " l: "; l.dump(); cout << " -l: "; l.invert().dump(); cout << " " << endl;
  topology::line l2(pb, pc); l.set_nsigma(10.);
  topology::line l3(pa, pc); l.set_nsigma(10.);

  topology::joint j(pa, pb, pc);
  cout << " j: "; j.dump(); cout << " -j: "; j.invert().dump(); cout << " " << endl;

  std::vector<topology::line> tangents;
  tangents.push_back(l);

  topology::cell_couplet cca(ca, cb, tangents);
  tangents.clear();
  tangents.push_back(l2);
  topology::cell_couplet ccb(cb, ca, tangents); 
  tangents.clear();
  tangents.push_back(l3);
  topology::cell_couplet ccc(cc, cb, tangents); 

  std::vector<topology::cell_couplet> cl1; cl1.push_back(cca);
  std::vector<topology::cell_couplet> cl2; cl2.push_back(ccb);
  std::vector<topology::cell_couplet> cl3; cl3.push_back(ccc);

  std::vector<topology::cell_triplet> ctl1; ctl1.clear();
  std::vector<topology::cell_triplet> ctl2; ctl2.clear();
  std::vector<topology::cell_triplet> ctl3; ctl3.clear();

  topology::cell_triplet cccc(ca, cb, cc); cccc.set_nsigma(10.);
  double Ratio = 3.;
  cccc.calculate_joints(Ratio);
  cout << " cccc: "; cccc.dump(); cout << " -cccc: "; cccc.invert().dump(); cout << " " << endl;

  topology::node na(ca, cl1, ctl1); na.set_nsigma(10.); na.calculate_triplets(Ratio);

  cout << " na: "; na.dump(); cout << " -na: "; na.invert().dump(); cout << " " << endl;
  topology::node nb(cb, cl2, ctl2); nb.set_nsigma(10.); nb.calculate_triplets(Ratio);
  topology::node nc(cc, cl3, ctl3); nc.set_nsigma(10.); nc.calculate_triplets(Ratio);

  std::vector<topology::node> nodes;
  nodes.push_back(na);
  nodes.push_back(nb);
  nodes.push_back(nc);

  topology::cluster cu(nodes); cu.set_nsigma(10.);
  cout << " cu: "; cu.dump(); cout << " -cu: "; cu.invert().dump(); cout << " " << endl;

    
  return 1;    
}


