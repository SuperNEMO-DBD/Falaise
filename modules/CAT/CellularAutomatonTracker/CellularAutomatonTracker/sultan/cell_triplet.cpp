/* -*- mode: c++ -*- */

#include <sultan/cell_triplet.h>

namespace SULTAN{
  namespace topology{
  
    using namespace std;
    using namespace mybhep;
    
    //!Default constructor     
    cell_triplet::cell_triplet()
    {
      appname_= "cell_triplet: ";
    }

    //!Default destructor
    cell_triplet::~cell_triplet(){};

    //! constructor
    cell_triplet::cell_triplet(cell_couplet &cca, cell_couplet &ccb){
      appname_= "cell_triplet: ";
    }

    //! constructor
    cell_triplet::cell_triplet(const cell &ca, const cell &cb, const cell &cc, prlevel level, double probmin){
      set_print_level(level);
      set_probmin(probmin);
      appname_= "cell_triplet: ";
      ca_ = ca;
      cb_ = cb;
      cc_ = cc;
    }

    /*** dump ***/
    void cell_triplet::dump (std::ostream & a_out,
                             const std::string & a_title,
                             const std::string & a_indent,
                             bool a_inherit) const{
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << std::endl;
        }

      a_out << indent << appname_ << " -------------- " << std::endl;
      a_out << indent  << " first cell " << std::endl;
      ca().dump(a_out,"", indent + "   ");
      a_out << indent << " second cell " << std::endl;
      cb().dump(a_out, "",indent + "   ");
      a_out << indent << " third cell " << std::endl;
      cc().dump(a_out, "",indent + "   ");
 
      return;
    }

    //! set cells
    void cell_triplet::set(const cell_couplet &cca, const cell_couplet &ccb){
      cb_ = cca.ca();
      ca_ = cca.cb();
      cc_ = ccb.cb();
      if( cca.ca().id() != ccb.ca().id() ){
        std::clog << " problem: trying to form a triplet of cell with cells " << cca.ca().id() << " " 
             << cca.cb().id() << " " 
             << ccb.ca().id() << " " 
             << ccb.cb().id() << std::endl;
      }
    }

    //! set cells
    void cell_triplet::set(const cell &ca, const cell &cb, const cell &cc){
      ca_ = ca;
      cb_ = cb;
      cc_ = cc;
    }


    //! get first cell couplet
    cell_couplet cell_triplet::cca()
    {
      cell_couplet cc1(cb_, ca_, print_level(), probmin());
      return cc1;
    }      

    //! get second cell couplet
    cell_couplet cell_triplet::ccb()
    {
      cell_couplet cc2(cb_, cc_, print_level(), probmin());
      return cc2;
    }      

    //! get first cell
    const cell& cell_triplet::ca()const
    {
      return ca_;
    }      

    //! get second cell
    const cell& cell_triplet::cb()const
    {
      return cb_;
    }      

    //! get third cell
    const cell& cell_triplet::cc()const
    {
      return cc_;
    }      

    void cell_triplet::calculate_helices(double Rmin = 0., double Rmax = mybhep::plus_infinity, double nsigmas=1.){
      // get helices tangent to three circles
      
      ////////////////////////////////////////////////////////////////////////
      //                                                                    //
      //  see http://mathworld.wolfram.com/ApolloniusProblem.html           //
      //                                                                    //
      ////////////////////////////////////////////////////////////////////////

      
      helices_.clear();
      double tolerance_for_smallness = 1.;
      size_t count_all_circles = 0;
      size_t count_circles_positive_radius = 0;
      size_t count_circles_good_radius = 0;
      size_t count_all_helices = 0;
      size_t count_tangent_circles = 0;
      bool selected_circle;
      int sign3;
      int the_sign;

      int sign[4][2];

      for(size_t i=0; i<4; i++){
	sign[i][0]=-1;
	sign[i][1]=1;
      }

      double a12, b12, d12,
	a13, b13, d13,
	det;
      double x[3], y[3], r[3], rerr[3];
      double X0, Y0, R, X0err, Y0err, Rerr,
	dRdr1, dRdr2, dRdr3, dX0dr1, dX0dr2, dX0dr3, dY0dr1, dY0dr2, dY0dr3,
	det1, det2, det3,
	phi1, phi2, phi3;
      double check[2], H;
      double diff[2], X0tmp[2], Y0tmp[2], Rtmp[2];

      topology::experimental_double H_12, H_23, z1, z2, z3, z0_12, z0_23;
      topology::experimental_helix helix;
      helix.reset_ids();
      helix.add_id(ca().id());
      helix.add_id(cb().id());
      helix.add_id(cc().id());

      double M[3][3], N[3], sol[3];
      double c12, c13;

      x[0] = ca().ep().x().value();
      x[1] = cb().ep().x().value();
      x[2] = cc().ep().x().value();
      y[0] = ca().ep().y().value();
      y[1] = cb().ep().y().value();
      y[2] = cc().ep().y().value();
      z1 = ca().ep().z();
      z2 = cb().ep().z();
      z3 = cc().ep().z();
      r[0] = ca().r().value();
      r[1] = cb().r().value();
      r[2] = cc().r().value();
      rerr[0] = ca().r().error();
      rerr[1] = cb().r().error();
      rerr[2] = cc().r().error();

      a12 = 2.*(x[0] - x[1]);
      b12 = 2.*(y[0] - y[1]);
      d12 = (pow(x[0],2) +
	     pow(y[0],2) -
	     pow(r[0],2) ) - 
	(pow(x[1],2) +
	 pow(y[1],2) -
	 pow(r[1],2) );
      
      a13 = 2.*(x[0] - x[2]);
      b13 = 2.*(y[0] - y[2]);
      d13 = (pow(x[0],2) +
	     pow(y[0],2) -
	     pow(r[0],2) ) - 
	(pow(x[2],2) +
	 pow(y[2],2) -
	 pow(r[2],2) );
      
      det = a12*b13 - a13*b12;

      if( print_level() > mybhep::VERBOSE ){
	std::clog << " triplet (" << x[0] << ", " << y[0] << ", " << r[0] << " +- " << rerr[0] << ", "; z1.dump(); std::clog << "), ("
		  << x[1] << ", " << y[1] << ", " << r[1] << " +- " << rerr[1] << ", " ; z2.dump(); std::clog << "), ("
		  << x[2] << ", " << y[2] << ", " << r[2] << " +- " << rerr[2] << ", " ; z3.dump(); std::clog << ") det " << det << std::endl;
      }

      if( fabs(det) < 0.001 || isnan(det) ){

	double theta = atan2(-b12,-a12);
	a12 = sqrt(pow(x[1] - x[0],2) + pow(y[1] - y[0],2));
	a13 = sqrt(pow(x[2] - x[0],2) + pow(y[2] - y[0],2));
	double e12 = pow(a12,2) + pow(r[0],2) - pow(r[1],2);
	double de12dr1 = 2.*r[0];
	double de12dr2 = -2.*r[1];
	double e13 = pow(a13,2) + pow(r[0],2) - pow(r[2],2);
	double de13dr1 = 2.*r[0];
	double de13dr3 = -2.*r[2];
	double dc12dr1, dc12dr2, dc13dr1, dc13dr3,
	  ddetdr1, ddetdr2, ddetdr3, dX0tmpdr1, dX0tmpdr2, dX0tmpdr3,
	  dY0tmpdr1[2], dY0tmpdr2[2], dY0tmpdr3[2];
	double dist12, dist13, __det, __N1, __N2;

	for(size_t i=0; i<2; i++){
	  dc12dr1 = sign[0][i];
	  for(size_t j=0; j<2; j++){
	    c12 = sign[0][i]*r[0] - sign[1][j]*r[1];
	    dc12dr2 = - sign[1][j];
	    for(size_t k=0; k<2; k++){
	      c13 = sign[0][i]*r[0] - sign[2][k]*r[2];
	      dc13dr1 = sign[0][i];
	      dc13dr3 = - sign[2][k];
	      det = 4.*(-a12*c13 + a13*c12);
	      ddetdr1 = 4.*(-a12*dc13dr1 + a13*dc12dr1);
	      ddetdr2 = 4.*a13*dc12dr2;
	      ddetdr3 = -4.*a12*dc13dr3;
	      X0tmp[0] = 2.*(-e12*c13 + e13*c12)/det;
	      dX0tmpdr1 = 2.*((-e12*dc13dr1 + e13*dc12dr1 - de12dr1*c13 + de13dr1*c12)*det - (-e12*c13 + e13*c12)*ddetdr1)/pow(det,2);
	      dX0tmpdr2 = 2.*((e13*dc12dr2 - de12dr2*c13)*det - (-e12*c13 + e13*c12)*ddetdr2)/pow(det,2);
	      dX0tmpdr3 = 2.*((-e12*dc13dr3 + de13dr3*c12)*det - (-e12*c13 + e13*c12)*ddetdr3)/pow(det,2);
	      R = 2.*fabs((a12*e13 - a13*e12)/det);
	      dRdr1 = 2.*((a12*de13dr1 - a13*de12dr1)*det - (a12*e13 - a13*e12)*ddetdr1)/pow(det,2);
	      dRdr2 = 2.*(- a13*de12dr2*det - (a12*e13 - a13*e12)*ddetdr2)/pow(det,2);
	      dRdr3 = 2.*(a12*de13dr3*det - (a12*e13 - a13*e12)*ddetdr3)/pow(det,2);


	      /// qqq

	      dist12 = sqrt(pow(x[1] - x[0],2) + pow(y[1] - y[0],2));
	      dist13 = sqrt(pow(x[2] - x[0],2) + pow(y[2] - y[0],2));
	      __det = 4.*(-dist12*c13 + dist13*c12);
	      __N1 = pow(dist12,2) - pow(r[1],2) + pow(r[0],2);
	      __N2 = pow(dist13,2) - pow(r[2],2) + pow(r[0],2);
	      X0tmp[0] = 2.*(-__N1*c13 + __N2*c12)/__det;
	      R = 2.*(__N2*dist12 - __N1*dist13)/__det;

	      /// qqq


	      for(size_t m=0; m<2; m++){
		Y0tmp[m] = sign[3][m]*sqrt(pow(R + sign[0][i]*r[0],2) - pow(X0tmp[0],2));
		X0 = X0tmp[0]*cos(theta) - Y0tmp[m]*sin(theta) + x[0];
		Y0 = X0tmp[0]*sin(theta) + Y0tmp[m]*cos(theta) + y[0];
		diff[m] = fabs(sqrt(pow(X0 - x[1],2) + pow(Y0 - y[1], 2)) - (R + sign[1][j]*r[1]));

		if( diff[m]  > tolerance_for_smallness ){ // the circle obtained is a spurious solution
		  continue;
		}
		  
		dY0tmpdr1[m] = ((R + sign[0][i]*r[0])*(dRdr1 + sign[0][i]) - X0tmp[0]*dX0tmpdr1)/Y0tmp[m];
		dY0tmpdr2[m] = ((R + sign[0][i]*r[0])*dRdr2 - X0tmp[0]*dX0tmpdr2)/Y0tmp[m];
		dY0tmpdr3[m] = ((R + sign[0][i]*r[0])*dRdr3 - X0tmp[0]*dX0tmpdr3)/Y0tmp[m];
		dX0dr1 = dX0tmpdr1*cos(theta) - dY0tmpdr1[m]*sin(theta);
		dX0dr2 = dX0tmpdr2*cos(theta) - dY0tmpdr2[m]*sin(theta);
		dX0dr3 = dX0tmpdr3*cos(theta) - dY0tmpdr3[m]*sin(theta);
		dY0dr1 = dX0tmpdr1*sin(theta) + dY0tmpdr1[m]*cos(theta);
		dY0dr2 = dX0tmpdr2*sin(theta) + dY0tmpdr2[m]*cos(theta);
		dY0dr3 = dX0tmpdr3*sin(theta) + dY0tmpdr3[m]*cos(theta);

		count_all_circles++;

		if( R < 0. ) continue;

		count_circles_positive_radius++;

		if( R < r[0] || R < r[1] || R < r[2] || R < Rmin || R > Rmax ) continue;
		
		count_circles_good_radius++;
		
		selected_circle = true;
		
		if( print_level() > mybhep::VERBOSE ){
		  std::clog << " circle  (X0 " << X0 << " +- " << X0err << ") (Y0 " << Y0 << " +- " << Y0err << ") (R " << R << " +- " << Rerr << ") (H " << H << " +- " << helix.H().error() << ") " << std::endl;
		}

		for(size_t ii=0; ii<3; ii++){
		  if( ii == 0 ) the_sign = sign[0][i];
		  else if( ii == 1 ) the_sign = sign[1][j];
		  else the_sign = sign[2][k];
		  check[ii] = fabs(sqrt(pow(X0 - x[ii],2) + pow(Y0 - y[ii],2)) - (R + the_sign*r[ii]));
		  if( print_level() > mybhep::VERBOSE ){
		    std::clog << "  circle " << ii << " check " << check[ii] << std::endl;
		  }
		  
		  if( check[ii] > tolerance_for_smallness ){ // the circle obtained is a spurious solution
		    selected_circle = false;
		  }
		}
		
	      
		if( !selected_circle ) continue;
		
		count_tangent_circles ++;
		
		Rerr = sqrt(pow(dRdr1*rerr[0],2) +
			    pow(dRdr2*rerr[1],2) +
			    pow(dRdr3*rerr[2],2) );
		
		X0err = sqrt(pow(dX0dr1*rerr[0],2) +
			     pow(dX0dr2*rerr[1],2) +
			     pow(dX0dr3*rerr[2],2) );
		
		Y0err = sqrt(pow(dY0dr1*rerr[0],2) +
			     pow(dY0dr2*rerr[1],2) +
			     pow(dY0dr3*rerr[2],2) );
		
		
		phi1 = atan2(y[0] - Y0, x[0] - X0);
		phi2 = atan2(y[1] - Y0, x[1] - X0);
		phi3 = atan2(y[2] - Y0, x[2] - X0);
		
		z0_12 = (z1*phi2 - z2*phi1)/(phi2 - phi1);
		H_12 = (z2 - z1)/(phi2 - phi1);
		
		z0_23 = (z2*phi3 - z3*phi2)/(phi3 - phi2);
		H_23 = (z3 - z2)/(phi3 - phi2);
		
		if( print_level() > mybhep::VERBOSE ){
		  std::clog << " R " << R << " Rerr " << Rerr << " dRdr1 " << dRdr1 << " dRdr2 " << dRdr2 << " dRdr3 " << dRdr3 << " det1 " << det1 << " det2 " << det2 << " det3 " << det3 << " theta " << theta << 
		    " z0_12 num " << (z1*phi2 - z2*phi1).value() << " den " << phi2 - phi1 << " = " << z0_12.value() << " +- " << z0_12.error() << " H_12 num " << (z2 - z1).value() << " = " << H_12.value() << " +- " << H_12.error() <<
		    " z0_23 num " << (z2*phi3 - z3*phi2).value() << " den " << phi3 - phi2 << " = " << z0_23.value() << " +- " << z0_23.error() << " H_23 num " << (z3 - z2).value() << " = " << H_23.value() << " +- " << H_23.error() << 
		    std::endl;
		}
		
		if( z0_12.is_equal_to__optimist(z0_23, nsigmas) &&
		    H_12.is_equal_to__optimist(H_23, nsigmas) ){
		  
		  std::vector<topology::experimental_double> z0s, Hs;
		  z0s.push_back(z0_12);
		  z0s.push_back(z0_23);
		  Hs.push_back(H_12);
		  Hs.push_back(H_23);
		  
		  helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),weighted_average(z0s),experimental_double(R,Rerr),weighted_average(Hs));
		  
		  X0 = helix.x0().value();
		  Y0 = helix.y0().value();
		  R = helix.R().value();
		  H = helix.H().value();
		  X0err = helix.x0().error();
		  Y0err = helix.y0().error();
		  Rerr = helix.R().error();
		  count_all_helices++;
		  
		  helices_.push_back(helix);
		}
	      }
	    }
	  }
	}
      }else{ // large det

	double alpha, beta, gamma, discr;
	double dd12dr1 = - 2.*r[0];
	double dd12dr2 = 2.*r[1];
	double dd13dr1 = - 2.*r[0];
	double dd13dr3 = 2.*r[2];
	double dc12dr1, dc12dr2, dc13dr1, dc13dr3,
	  dalphadr1, dalphadr2, dalphadr3,
	  dbetadr1, dbetadr2, dbetadr3,
	  dgammadr1, dgammadr2, dgammadr3,
	  ddiscrdr1, ddiscrdr2, ddiscrdr3;

	for(size_t i=0; i<2; i++){
	  for(size_t j=0; j<2; j++){
	  
	    c12 = 2.*(sign[0][i]*r[0] + sign[1][j]*r[1]);
	    dc12dr1 = 2.*sign[0][i];
	    dc12dr2 = 2.*sign[1][j];
	  
	    for(size_t k=0; k<2; k++){
	    
	      c13 = 2.*(sign[0][i]*r[0] + sign[2][k]*r[2]);
	      dc13dr1 = 2.*sign[0][i];
	      dc13dr3 = 2.*sign[2][k];
	    
	      alpha = 1. - (
			    pow(c12*b13 - c13*b12,2) + 
			    pow(c13*a12 - c12*a13,2)
			    )/pow(det,2);
	      
	      beta = sign[0][i]*r[0] - (
					(-c12*b13 + c13*b12)*(d12*b13 - d13*b12) +
					(-c13*a12 + c12*a13)*(d13*a12 - d12*a13)
					)/pow(det,2) + (
							x[0]*(-c12*b13 + c13*b12) +
							y[0]*(-c13*a12 + c12*a13) )/det;

	      gamma = pow(r[0],2) - pow(x[0],2) - pow(y[0],2)
		- (
		   pow(d12*b13 - d13*b12,2) + 
		   pow(d13*a12 - d12*a13,2) 
		   )/pow(det,2) 
		+ 2. * (x[0] * (d12*b13 - d13*b12) + 
		    y[0] * (d13*a12 - d12*a13) )/det; 
	    

	      /// qqq

	      c12 = sign[0][i]*r[0] - sign[1][j]*r[1];
	      c13 = sign[0][i]*r[0] - sign[2][k]*r[2];
	      double __A11 = 2.*(x[0] - x[1]);
	      double __A12 = 2.*(x[0] - x[2]);
	      double __A21 = 2.*(y[0] - y[1]);
	      double __A22 = 2.*(y[0] - y[2]);
	      det = __A11*__A22 - __A12*__A21;
	      double __d12 = pow(x[0],2) + pow(y[0],2) - pow(r[0],2) - pow(x[1],2) - pow(y[1],2) + pow(r[1],2);
	      double __d13 = pow(x[0],2) + pow(y[0],2) - pow(r[0],2) - pow(x[2],2) - pow(y[2],2) + pow(r[2],2);
	      alpha = 4.*(
			  pow(-c12*__A22 + c13*__A21,2)
			  + pow(-c13*__A11 + c12*__A12,2)
			  )/pow(det,2) - 1.;
	      beta = 2.*(
			 (-c12*__d12*pow(__A22,2) - c13*__d13*pow(__A21,2) + c12*__d13*__A22*__A21 + c13*__d12*__A21*__A22) 
			 + (-c13*__d13*pow(__A11,2) - c12*__d12*pow(__A12,2) + c13*__d12*__A11*__A12 + c12*__d13*__A12*__A11) 
			 )/pow(det,2) 
		- 2.*(
		      x[1]*(-c12*__A22 + c13*__A21)
		      + y[1]*(-c13*__A11 + c12*__A12)
		      )/det
		- sign[1][j]*r[1]; 
	      gamma = (
		       pow(__d12*__A22 - __d13*__A21,2) +
		       pow(__d13*__A11 - __d12*__A12,2)
		       )/pow(det,2) 
		- 2.*(
		      x[1]*(__d12*__A22 - __d13*__A21)
		      + y[1]*(__d13*__A11 - __d12*__A12)
		      )/det
		+ pow(x[1],2) + pow(y[1],2) - pow(r[1],2);
	      
	      /// qqq



	      discr = pow(beta,2) - alpha*gamma;

              count_all_circles++;

	      if( discr < 0. ){
		continue;
	      }

	      for(size_t l=0; l<2; l++){
		Rtmp[l] = (- beta + sign[3][l]*sqrt(discr))/alpha;
		X0tmp[l] = ((d12 - c12*Rtmp[l])*b13 - (d13 - c13*Rtmp[l])*b12)/det;
		Y0tmp[l] = ((d13 - c13*Rtmp[l])*a12 - (d12 - c12*Rtmp[l])*a13)/det;

		/// qqq
		X0tmp[l] = ((__d12 - 2.*c12*Rtmp[l])*__A22 - (__d13 - 2.*c13*Rtmp[l])*__A21)/det;
		Y0tmp[l] = ((__d13 - 2.*c13*Rtmp[l])*__A11 - (__d12 - 2.*c12*Rtmp[l])*__A12)/det;
		/// qqq

		diff[l] = fabs(sqrt(pow(X0tmp[l] - x[1],2) + pow(Y0tmp[l] - y[1], 2)) - (Rtmp[l] + sign[1][j]*r[1]));

		if( diff[l]  > tolerance_for_smallness ){ // the circle obtained is a spurious solution
		  continue;
		}

		R = Rtmp[l];
		X0 = X0tmp[l];
		Y0 = Y0tmp[l];
		sign3 = sign[3][l];

		if( R < 0. ) continue;
		
		count_circles_positive_radius++;
		
		if( R < r[0] || R < r[1] || R < r[2] || R < Rmin || R > Rmax ) continue;
		
		count_circles_good_radius++;
		
		selected_circle = true;
		
		if( print_level() > mybhep::VERBOSE ){
		  std::clog << " circle  (X0 " << X0 << " +- " << X0err << ") (Y0 " << Y0 << " +- " << Y0err << ") (R " << R << " +- " << Rerr << ") (H " << H << " +- " << helix.H().error() << ") " << std::endl;
		}
		
		for(size_t ii=0; ii<3; ii++){
		  if( ii == 0 ) the_sign = sign[0][i];
		  else if( ii == 1 ) the_sign = sign[1][j];
		  else the_sign = sign[2][k];
		  check[ii] = fabs(sqrt(pow(X0 - x[ii],2) + pow(Y0 - y[ii],2)) - (R + the_sign*r[ii]));
		  
		  if( print_level() > mybhep::VERBOSE ){
		    std::clog << "  circle " << ii << " check " << check[ii] << std::endl;
		  }
		  
		  if( check[ii] > tolerance_for_smallness ){ // the circle obtained is a spurious solution
		    selected_circle = false;
		  }
		}

	     
		if( !selected_circle ) continue;
		
		count_tangent_circles ++;
		
		
		dalphadr1 = - 2.*(
				  (c12*b13 - c13*b12)*(dc12dr1*b13 - dc13dr1*b12) +
				  (c13*a12 - c12*a13)*(dc13dr1*a12 - dc12dr1*a13)
				  )/pow(det,2);
		
		dalphadr2 = - 2.*(
				  (c12*b13 - c13*b12)*dc12dr2*b13 +
				  (c13*a12 - c12*a13)*(- dc12dr2*a13)
				  )/pow(det,2);
		
		dalphadr3 = - 2.*(
				  (c12*b13 - c13*b12)*(- dc13dr3*b12) +
				  (c13*a12 - c12*a13)*dc13dr3*a12
				  )/pow(det,2);
		
		dbetadr1 = sign[0][i] - (
					 (-dc12dr1*b13 + dc13dr1*b12)*(d12*b13 - d13*b12) + (-c12*b13 + c13*b12)*(dd12dr1*b13 - dd13dr1*b12) +
					 (-dc13dr1*a12 + dc12dr1*a13)*(d13*a12 - d12*a13) + (-c13*a12 + c12*a13)*(dd13dr1*a12 - dd12dr1*a13)
					 )/pow(det,2) + (
							 x[0]*(-dc12dr1*b13 + dc13dr1*b12) +
							 y[0]*(-dc13dr1*a12 + dc12dr1*a13) )/det;
		
		dbetadr2 =  - (
			       -dc12dr2*b13*(d12*b13 - d13*b12) + (-c12*b13 + c13*b12)*dd12dr2*b13 +
			       dc12dr2*a13*(d13*a12 - d12*a13) + (-c13*a12 + c12*a13)*(- dd12dr2*a13)
			       )/pow(det,2) + (
					       x[0]*(-dc12dr2*b13) +
					       y[0]*dc12dr2*a13 )/det;
		
		dbetadr3 = - (
			      dc13dr3*b12*(d12*b13 - d13*b12) + (-c12*b13 + c13*b12)*(- dd13dr3*b12) +
			      -dc13dr3*a12*(d13*a12 - d12*a13) + (-c13*a12 + c12*a13)*dd13dr3*a12
			      )/pow(det,2) + (
					      x[0]*dc13dr3*b12 +
					      y[0]*(-dc13dr3*a12) )/det;
		
		dgammadr1 = 2.*r[0]
		  - 2.*(
			(d12*b13 - d13*b12)*(dd12dr1*b13 - dd13dr1*b12) + 
			(d13*a12 - d12*a13)*(dd13dr1*a12 - dd12dr1*a13) 
			)/pow(det,2) 
		  + 2. * (x[0] * (dd12dr1*b13 - dd13dr1*b12) + 
			  y[0] * (dd13dr1*a12 - dd12dr1*a13) )/det; 
		
		dgammadr2 = - 2.*(
				  (d12*b13 - d13*b12)*dd12dr2*b13 +
				  (d13*a12 - d12*a13)*(- dd12dr2*a13) 
				  )/pow(det,2) 
		  + 2. * (x[0] * dd12dr2*b13 +
			  y[0] * (- dd12dr2*a13) )/det; 
		
		dgammadr3 = - 2.*(
				  (d12*b13 - d13*b12)*(- dd13dr3*b12) + 
				  (d13*a12 - d12*a13)*(dd13dr3*a12) 
				  )/pow(det,2) 
		  + 2. * (x[0] * (- dd13dr3*b12) + 
			  y[0] * dd13dr3*a12  )/det; 
		
		ddiscrdr1 = 2.*beta*dbetadr1 - dalphadr1*gamma - alpha*dgammadr1;
		ddiscrdr2 = 2.*beta*dbetadr2 - dalphadr2*gamma - alpha*dgammadr2;
		ddiscrdr3 = 2.*beta*dbetadr3 - dalphadr3*gamma - alpha*dgammadr3;
		
		
		
		dRdr1 = ((- dbetadr1 + sign3*ddiscrdr1/(2.*sqrt(discr)))*alpha - (- beta + sign3*sqrt(discr))*dalphadr1)/pow(alpha,2);
		dRdr2 = ((- dbetadr2 + sign3*ddiscrdr2/(2.*sqrt(discr)))*alpha - (- beta + sign3*sqrt(discr))*dalphadr2)/pow(alpha,2);
		dRdr3 = ((- dbetadr3 + sign3*ddiscrdr3/(2.*sqrt(discr)))*alpha - (- beta + sign3*sqrt(discr))*dalphadr3)/pow(alpha,2);
		
		dX0dr1 = ((dd12dr1 - dc12dr1*R - c12*dRdr1)*b13 - (dd13dr1 - dc13dr1*R - c13*dRdr1)*b12)/det;
		dX0dr2 = ((dd12dr2 - dc12dr2*R - c12*dRdr2)*b13 + c13*dRdr2*b12)/det;
		dX0dr3 = (- c12*dRdr3*b13 - (dd13dr3 - dc13dr3*R - c13*dRdr3)*b12)/det;
		
		dY0dr1 = ((dd13dr1 - dc13dr1*R - c13*dRdr1)*a12 - (dd12dr1 - dc12dr1*R - c12*dRdr1)*a13)/det;
		dY0dr2 = (-c13*dRdr2*a12 - (dd12dr2 - dc12dr2*R - c12*dRdr2)*a13)/det;
		dY0dr3 = ((dd13dr3 - dc13dr3*R - c13*dRdr3)*a12 + c12*dRdr3*a13)/det;
		
		Rerr = sqrt(pow(dRdr1*rerr[0],2) +
			    pow(dRdr2*rerr[1],2) +
			    pow(dRdr3*rerr[2],2) );
		
		X0err = sqrt(pow(dX0dr1*rerr[0],2) +
			     pow(dX0dr2*rerr[1],2) +
			     pow(dX0dr3*rerr[2],2) );
		
		Y0err = sqrt(pow(dY0dr1*rerr[0],2) +
			     pow(dY0dr2*rerr[1],2) +
			     pow(dY0dr3*rerr[2],2) );
		
		phi1 = atan2(y[0] - Y0, x[0] - X0);
		phi2 = atan2(y[1] - Y0, x[1] - X0);
		phi3 = atan2(y[2] - Y0, x[2] - X0);
		
		z0_12 = (z1*phi2 - z2*phi1)/(phi2 - phi1);
		H_12 = (z2 - z1)/(phi2 - phi1);
		
		z0_23 = (z2*phi3 - z3*phi2)/(phi3 - phi2);
		H_23 = (z3 - z2)/(phi3 - phi2);
		
		if( print_level() > mybhep::VERBOSE ){
		  std::clog << " R " << R << " Rerr " << Rerr << " dRdr1 " << dRdr1 << " dRdr2 " << dRdr2 << " dRdr3 " << dRdr3 << " det1 " << det1 << " det2 " << det2 << " det3 " << det3 << 
		    " z0_12 num " << (z1*phi2 - z2*phi1).value() << " den " << phi2 - phi1 << " = " << z0_12.value() << " +- " << z0_12.error() << " H_12 num " << (z2 - z1).value() << " = " << H_12.value() << " +- " << H_12.error() << 
		    " z0_23 num " << (z2*phi3 - z3*phi2).value() << " den " << phi3 - phi2 << " = " << z0_23.value() << " +- " << z0_23.error() << " H_23 num " << (z3 - z2).value() << " = " << H_23.value() << " +- " << H_23.error() << 
		    std::endl;
		}
		
		
		if( z0_12.is_equal_to__optimist(z0_23, nsigmas) &&
		    H_12.is_equal_to__optimist(H_23, nsigmas) ){
		  std::vector<topology::experimental_double> z0s, Hs;
		  z0s.push_back(z0_12);
		  z0s.push_back(z0_23);
		  Hs.push_back(H_12);
		  Hs.push_back(H_23);
		  
		  helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),weighted_average(z0s),experimental_double(R,Rerr),weighted_average(Hs));
		  
		  X0 = helix.x0().value();
		  Y0 = helix.y0().value();
		  R = helix.R().value();
		  H = helix.H().value();
		  X0err = helix.x0().error();
		  Y0err = helix.y0().error();
		  Rerr = helix.R().error();
		  
		  count_all_helices++;
		  
		  helices_.push_back(helix);
		}
	      }
	    }
	  }
	}
      }
    
      if( print_level() > mybhep::VERBOSE ){
	std::clog << " summary of helices production for this triplet " << std::endl;
	std::clog << " count_all_circles " << count_all_circles << std::endl;
	std::clog << " count_circles_positive_radius " << count_circles_positive_radius << std::endl;
	std::clog << " count_circles_good_radius " << count_circles_good_radius << std::endl;
	std::clog << " count_tangent_circles " << count_tangent_circles << std::endl;
	std::clog << " count_all_helices " << count_all_helices << std::endl;
      }

      return;
    }


    bool operator==(const cell_triplet& left,
                    const cell_triplet& right)
    {

      return ((left.ca().id() == right.ca().id()) && (left.cc().id() == right.cc().id())) ||
        ((left.ca().id() == right.cc().id()) && (left.cc().id() == right.ca().id()));

    }


     
  }

}
