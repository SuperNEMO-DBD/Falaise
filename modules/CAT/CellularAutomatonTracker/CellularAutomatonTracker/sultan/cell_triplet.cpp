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

    void cell_triplet::calculate_helices(double Rmin = 0., double Rmax = mybhep::plus_infinity){
      // get helices tangent to three circles
      
      ////////////////////////////////////////////////////////////////////////
      //                                                                    //
      //  see http://mathworld.wolfram.com/ApolloniusProblem.html           //
      //                                                                    //
      ////////////////////////////////////////////////////////////////////////

      
      helices_.clear();

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

      topology::experimental_double H_12, H_23, z1, z2, z3, z0_12, z0_23;
      topology::experimental_helix helix;
      helix.reset_ids();
      helix.add_id(ca().id());
      helix.add_id(cb().id());
      helix.add_id(cc().id());

      double M[3][3], N[3], sol[3];

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
	double diff[2], X0tmp, Y0tmp[2], c12, c13, dc12dr1, dc12dr2, dc13dr1, dc13dr3,
	  ddetdr1, ddetdr2, ddetdr3, dX0tmpdr1, dX0tmpdr2, dX0tmpdr3,
	  dY0tmpdr1[2], dY0tmpdr2[2], dY0tmpdr3[2];
	for(size_t i=0; i<2; i++){
	  for(size_t j=0; j<2; j++){
	    c12 = sign[0][i]*r[0] - sign[1][j]*r[1];
	    dc12dr1 = sign[0][i];
	    dc12dr2 = - sign[1][j];
	    for(size_t k=0; k<2; k++){
	      c13 = sign[0][i]*r[0] - sign[2][k]*r[2];
	      dc13dr1 = sign[0][i];
	      dc13dr3 = - sign[2][k];
	      det = 4.*(-a12*c13 + a13*c12);
	      ddetdr1 = 4.*(-a12*dc13dr1 + a13*dc12dr1);
	      ddetdr2 = 4.*a13*dc12dr2;
	      ddetdr3 = -4.*a12*dc13dr3;
	      X0tmp = 2.*(-e12*c13 + e13*c12)/det;
	      dX0tmpdr1 = 2.*((-e12*dc13dr1 + e13*dc12dr1 - de12dr1*c13 + de13dr1*c12)*det - (-e12*c13 + e13*c12)*ddetdr1)/pow(det,2);
	      dX0tmpdr2 = 2.*((e13*dc12dr2 - de12dr2*c13)*det - (-e12*c13 + e13*c12)*ddetdr2)/pow(det,2);
	      dX0tmpdr3 = 2.*((-e12*dc13dr3 + de13dr3*c12)*det - (-e12*c13 + e13*c12)*ddetdr3)/pow(det,2);
	      R = 2.*fabs((a12*e13 - a13*e12)/det);
	      dRdr1 = 2.*((a12*de13dr1 - a13*de12dr1)*det - (a12*e13 - a13*e12)*ddetdr1)/pow(det,2);
	      dRdr2 = 2.*(- a13*de12dr2*det - (a12*e13 - a13*e12)*ddetdr2)/pow(det,2);
	      dRdr3 = 2.*(a12*de13dr3*det - (a12*e13 - a13*e12)*ddetdr3)/pow(det,2);
	      for(size_t m=0; m<2; m++){
		Y0tmp[m] = sign[3][m]*sqrt(pow(R + sign[0][i]*r[0],2) - pow(X0tmp,2));
		X0 = X0tmp*cos(theta) - Y0tmp[m]*sin(theta) + x[0];
		Y0 = X0tmp*sin(theta) + Y0tmp[m]*cos(theta) + y[0];
		diff[m] = pow(X0 - x[1],2) + pow(Y0 - y[1], 2) - pow(R + sign[1][j]*r[1],2);
	      }
	      if( fabs(diff[0]) < fabs(diff[1]) ){
		X0 = X0tmp*cos(theta) - Y0tmp[0]*sin(theta) + x[0];
		Y0 = X0tmp*sin(theta) + Y0tmp[0]*cos(theta) + y[0];
		dY0tmpdr1[0] = ((R + sign[0][i]*r[0])*(dRdr1 + sign[0][i]) - X0tmp*dX0tmpdr1)/Y0tmp[0];
		dY0tmpdr2[0] = ((R + sign[0][i]*r[0])*dRdr2 - X0tmp*dX0tmpdr2)/Y0tmp[0];
		dY0tmpdr3[0] = ((R + sign[0][i]*r[0])*dRdr3 - X0tmp*dX0tmpdr3)/Y0tmp[0];
		dX0dr1 = dX0tmpdr1*cos(theta) - dY0tmpdr1[0]*sin(theta);
		dX0dr2 = dX0tmpdr2*cos(theta) - dY0tmpdr2[0]*sin(theta);
		dX0dr3 = dX0tmpdr3*cos(theta) - dY0tmpdr3[0]*sin(theta);
		dY0dr1 = dX0tmpdr1*sin(theta) + dY0tmpdr1[0]*cos(theta);
		dY0dr2 = dX0tmpdr2*sin(theta) + dY0tmpdr2[0]*cos(theta);
		dY0dr3 = dX0tmpdr3*sin(theta) + dY0tmpdr3[0]*cos(theta);
	      }else{
		X0 = X0tmp*cos(theta) - Y0tmp[1]*sin(theta) + x[0];
		Y0 = X0tmp*sin(theta) + Y0tmp[1]*cos(theta) + y[0];
		dY0tmpdr1[1] = ((R + sign[0][i]*r[0])*(dRdr1 + sign[0][i]) - X0tmp*dX0tmpdr1)/Y0tmp[1];
		dY0tmpdr2[1] = ((R + sign[0][i]*r[0])*dRdr2 - X0tmp*dX0tmpdr2)/Y0tmp[1];
		dY0tmpdr3[1] = ((R + sign[0][i]*r[0])*dRdr3 - X0tmp*dX0tmpdr3)/Y0tmp[1];
		dX0dr1 = dX0tmpdr1*cos(theta) - dY0tmpdr1[1]*sin(theta);
		dX0dr2 = dX0tmpdr2*cos(theta) - dY0tmpdr2[1]*sin(theta);
		dX0dr3 = dX0tmpdr3*cos(theta) - dY0tmpdr3[1]*sin(theta);
		dY0dr1 = dX0tmpdr1*sin(theta) + dY0tmpdr1[1]*cos(theta);
		dY0dr2 = dX0tmpdr2*sin(theta) + dY0tmpdr2[1]*cos(theta);
		dY0dr3 = dX0tmpdr3*sin(theta) + dY0tmpdr3[1]*cos(theta);
	      }
	      if( R < 0. ) continue;
	      if( R < r[0] || R < r[1] || R < r[2] || R < Rmin || R > Rmax ) continue;


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
		  " z0_12 num " << (z1*phi2 - z2*phi1).value() << " den " << phi2 - phi1 << " H_12 num " << (z2 - z1).value() << 
		  " z0_23 num " << (z2*phi3 - z3*phi2).value() << " den " << phi3 - phi2 << " H_23 num " << (z3 - z2).value() << 
		  std::endl;
	      }

	      helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),z0_12,experimental_double(R,Rerr),H_12);
	      helices_.push_back(helix);

	      helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),z0_23,experimental_double(R,Rerr),H_23);
	      helices_.push_back(helix);



	    }
	  }
	}
      }else{

	double c12, c13, alpha, beta, gamma, discr;
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
	    
	      discr = pow(beta,2) - alpha*gamma;
	      if( discr < 0. ){
		continue;
	      }

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

	      for(size_t l=0; l<2; l++){
		R = fabs((- beta + sign[3][l]*sqrt(discr))/alpha);
		if( R < 0. ) continue;
		if( R < r[0] || R < r[1] || R < r[2] || R < Rmin || R > Rmax ) continue;

		X0 = ((d12 - c12*R)*b13 - (d13 - c13*R)*b12)/det;
		Y0 = ((d13 - c13*R)*a12 - (d12 - c12*R)*a13)/det;

		dRdr1 = ((- dbetadr1 + sign[3][l]*ddiscrdr1/(2.*sqrt(discr)))*alpha - (- beta + sign[3][l]*sqrt(discr))*dalphadr1)/pow(alpha,2);
		dRdr2 = ((- dbetadr2 + sign[3][l]*ddiscrdr2/(2.*sqrt(discr)))*alpha - (- beta + sign[3][l]*sqrt(discr))*dalphadr2)/pow(alpha,2);
		dRdr3 = ((- dbetadr3 + sign[3][l]*ddiscrdr3/(2.*sqrt(discr)))*alpha - (- beta + sign[3][l]*sqrt(discr))*dalphadr3)/pow(alpha,2);

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
		    " z0_12 num " << (z1*phi2 - z2*phi1).value() << " den " << phi2 - phi1 << " H_12 num " << (z2 - z1).value() << 
		    " z0_23 num " << (z2*phi3 - z3*phi2).value() << " den " << phi3 - phi2 << " H_23 num " << (z3 - z2).value() << 
		    std::endl;
		}

		helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),z0_12,experimental_double(R,Rerr),H_12);
		helices_.push_back(helix);
		helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),z0_23,experimental_double(R,Rerr),H_23);
		helices_.push_back(helix);
		
	      }
	    }
	  }
	}
      }
    
      if( print_level() > mybhep::VERBOSE ){
	double check[2], check_chosen[3];
	for(vector<experimental_helix>::iterator ic=helices_.begin(); ic!=helices_.end(); ++ic){
	  X0 = ic->x0().value();
	  double Y0 = ic->y0().value();
	  Y0 = ic->y0().value();
	  R = ic->R().value();
	  double H = ic->H().value();
	  for(size_t i=0; i<3; i++){
	    for(size_t j=0; j<2; j++){
	      check[j] = pow(X0 - x[i],2) + pow(Y0 - y[i],2) - pow(R + sign[i][j]*r[i],2);
	    }
	    check_chosen[i] = std::min(fabs(check[0]), fabs(check[1]));
	    std::clog << " circle  (X0 " << X0 << " +- " << X0err << ") (Y0 " << Y0 << " +- " << Y0err << ") (R " << R << " +- " << Rerr << ") (H " << H << " +- " << ic->H().error() << ") circle " << i << " check " << check_chosen[i] << std::endl;
	  }
	}
      }
      
      return;
    }


    cell_triplet cell_triplet::invert(){
      cell_triplet inverted;
      inverted.set(ccb(),cca());
      return inverted;
    }



    bool operator==(const cell_triplet& left,
                    const cell_triplet& right)
    {

      return ((left.ca().id() == right.ca().id()) && (left.cc().id() == right.cc().id())) ||
        ((left.ca().id() == right.cc().id()) && (left.cc().id() == right.ca().id()));

    }


     
  }

}
