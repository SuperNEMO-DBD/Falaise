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
                             bool /* a_inherit */) const{
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

    bool cell_triplet::has_cell_of_id(size_t id)const{
      return (ca_.id() == id ||
	      cb_.id() == id ||
	      cc_.id() == id);
    }


    void cell_triplet::calculate_helices(double Rmin = 0., double Rmax = mybhep::plus_infinity, double nsigmas=1.){
      // get helices tangent to three circles

      ////////////////////////////////////////////////////////////////////////
      //                                                                    //
      //  see http://mathworld.wolfram.com/ApolloniusProblem.html           //
      //                                                                    //
      ////////////////////////////////////////////////////////////////////////


      helices_.clear();
      double tolerance_for_smallness = 1.; // mm
      size_t count_all_circles = 0;
      size_t count_circles_positive_radius = 0;
      size_t count_circles_good_radius = 0;
      size_t count_all_helices = 0;
      size_t count_tangent_circles = 0;
      bool selected_circle;
      int the_sign;

      int sign[4][2];
      for(size_t i=0; i<4; i++){
        sign[i][0]=-1;
        sign[i][1]=1;
      }

      double A11, A12, A21, A22, detA;
      double x[3], y[3];
      experimental_double r[3], z[3];
      double X0, Y0, R, X0err, Y0err, Rerr;
      double dX0dr[3], dY0dr[3], dRdr[3];
      double phi1, phi2, phi3;
      double check[3] ;
      double X0tmp, Y0tmp[2] ;
      double dX0tmpdr[3], dY0tmpdr[2][3] ;

      topology::experimental_double H_12, H_23, z0_12, z0_23;
      topology::experimental_helix helix;
      helix.reset_ids();
      helix.add_id(ca().id());
      helix.add_id(cb().id());
      helix.add_id(cc().id());

      double c12, c13, d12, d13;
      double dd13dr[3];
      double dd12dr[3];
      double dc12dr[3];
      double dc13dr[3];


      x[0] = ca().ep().x().value();
      x[1] = cb().ep().x().value();
      x[2] = cc().ep().x().value();
      y[0] = ca().ep().y().value();
      y[1] = cb().ep().y().value();
      y[2] = cc().ep().y().value();
      z[0] = ca().ep().z();
      z[1] = cb().ep().z();
      z[2] = cc().ep().z();
      r[0] = ca().r();
      r[1] = cb().r();
      r[2] = cc().r();


      // matrix A, exact
      A11 = 2.*(x[0] - x[1]);
      A21 = 2.*(y[0] - y[1]);
      A12 = 2.*(x[0] - x[2]);
      A22 = 2.*(y[0] - y[2]);
      detA = A11*A22 - A12*A21;


      // known terms d, with error
      d12 = (pow(x[0],2) +
             pow(y[0],2) -
             pow(r[0].value(),2) ) -
        (pow(x[1],2) +
         pow(y[1],2) -
         pow(r[1].value(),2) );
      dd12dr[0] = - 2.*r[0].value();
      dd12dr[1] = 2.*r[1].value();
      dd12dr[2] = 0.;

      d13 = (pow(x[0],2) +
             pow(y[0],2) -
             pow(r[0].value(),2) ) -
        (pow(x[2],2) +
         pow(y[2],2) -
         pow(r[2].value(),2) );
      dd13dr[0] = - 2.*r[0].value();
      dd13dr[1] = 0.;
      dd13dr[2] = 2.*r[2].value();


       if( print_level() > mybhep::VERBOSE ){
	std::clog << " triplet (" << x[0] << ", " << y[0] << ", "; r[0].dump(); std::clog << " "; z[0].dump(); std::clog << "), ("
		  << x[1] << ", " << y[1] << ", "; r[1].dump(); std::clog << " "; z[1].dump(); std::clog << "), ("
		  << x[2] << ", " << y[2] << ", "; r[2].dump(); std::clog << " "; z[2].dump(); std::clog << ") detA " << detA << std::endl;
      }


      // the centers of the three cells are aligned
      if( fabs(detA) < 0.001 || isnan(detA) ){

        // phi angle of the line on which the cells lie; exact
        double theta = atan2(-A21,-A11);

        double dist12, dist13, detB, N1, N2;
        double dN1dr[3], dN2dr[3], ddetBdr[3];

        // distances between cells become x coordinates after rotation; exact
        dist12 = sqrt(pow(x[1] - x[0],2) + pow(y[1] - y[0],2));
        dist13 = sqrt(pow(x[2] - x[0],2) + pow(y[2] - y[0],2));

	// if the cells are 1 ... 2 ... 3 we are ok
	// if they are 3 ... 1 ... 2 the dist13 must reverse sign
        double theta13 = atan2(-A22,-A12);
	if( theta * theta13 < 0. ) dist13 *= -1.;


        // known terms in the linear system, with error
        N1 = pow(dist12,2) - pow(r[1].value(),2) + pow(r[0].value(),2);
        N2 = pow(dist13,2) - pow(r[2].value(),2) + pow(r[0].value(),2);
        dN1dr[0] = 2.*r[0].value();
        dN1dr[1] = -2.*r[1].value();
        dN1dr[2] = 0.;
        dN2dr[0] = 2.*r[0].value();
        dN2dr[1] = 0.;
        dN2dr[2] = -2.*r[2].value();

        for(size_t i=0; i<2; i++){
          for(size_t j=0; j<2; j++){
            // known coefficients, with error
            c12 = sign[0][i]*r[0].value() - sign[1][j]*r[1].value();
            dc12dr[0] = sign[0][i];
            dc12dr[1] = -sign[1][j];
            dc12dr[2] = 0.;
            for(size_t k=0; k<2; k++){
              c13 = sign[0][i]*r[0].value() - sign[2][k]*r[2].value();
              dc13dr[0] = sign[0][i];
              dc13dr[1] = 0.;
              dc13dr[2] = -sign[2][k];

              detB = 4.*(-dist12*c13 + dist13*c12);
              for(size_t ierr=0; ierr<3; ierr++)
                ddetBdr[ierr] = 4.*(-dist12*dc13dr[ierr] + dist13*dc12dr[ierr]);


              // obtain circle solution (X0tmp, Y0tmp, R) as if cells lie on x axis
              X0tmp = 2.*(-N1*c13 + N2*c12)/detB;
              for(size_t ierr=0; ierr<3; ierr++)
                dX0tmpdr[ierr] = 2.*(
                                     (-dN1dr[ierr]*c13 + dN2dr[ierr]*c12)/detB
                                     + (-N1*dc13dr[ierr] + N2*dc12dr[ierr])/detB
                                     - (-N1*c13 + N2*c12)/pow(detB,2)*ddetBdr[ierr]
                                     );


              R = 2.*(N2*dist12 - N1*dist13)/detB;
              for(size_t ierr=0; ierr<3; ierr++)
                dRdr[ierr] = 2.*(
                              (dN2dr[ierr]*dist12 - dN1dr[ierr]*dist13)/detB
                              - (N2*dist12 - N1*dist13)/pow(detB,2)*ddetBdr[ierr]
                              );
              Rerr=0.;
              for(size_t ierr=0; ierr<3; ierr++){
                Rerr += pow(dRdr[ierr]*r[ierr].error(),2);
              }
              Rerr = sqrt(Rerr);


              for(size_t m=0; m<2; m++){
                Y0tmp[m] = sign[3][m]*sqrt(pow(R + sign[0][i]*r[0].value(),2) - pow(X0tmp,2));
                dY0tmpdr[m][0] = sign[3][m]  /  sqrt(pow(R + sign[0][i]*r[0].value(),2) - pow(X0tmp,2))  *  ((R + sign[0][i]*r[0].value())*(dRdr[0] + sign[0][i]) - X0tmp*dX0tmpdr[0]);
                dY0tmpdr[m][1] = sign[3][m]  /  sqrt(pow(R + sign[0][i]*r[0].value(),2) - pow(X0tmp,2))  *  ((R + sign[0][i]*r[0].value())*dRdr[1] - X0tmp*dX0tmpdr[1]);
		dY0tmpdr[m][2] = sign[3][m]  /  sqrt(pow(R + sign[0][i]*r[0].value(),2) - pow(X0tmp,2))  *  ((R + sign[0][i]*r[0].value())*dRdr[2] - X0tmp*dX0tmpdr[2]);


                // translate and rotate
                X0 = X0tmp*cos(theta) - Y0tmp[m]*sin(theta) + x[0];
                for(size_t ierr=0; ierr<3; ierr++)
                  dX0dr[ierr] = dX0tmpdr[ierr]*cos(theta) - dY0tmpdr[m][ierr]*sin(theta);

                Y0 = X0tmp*sin(theta) + Y0tmp[m]*cos(theta) + y[0];
                for(size_t ierr=0; ierr<3; ierr++)
                  dY0dr[ierr] = dX0tmpdr[ierr]*sin(theta) + dY0tmpdr[m][ierr]*cos(theta);

                X0err=0.;
                Y0err=0.;
                for(size_t ierr=0; ierr<3; ierr++){
                  X0err += pow(dX0dr[ierr]*r[ierr].error(),2);
                  Y0err += pow(dY0dr[ierr]*r[ierr].error(),2);
                }
                X0err = sqrt(X0err);
                Y0err = sqrt(Y0err);

                count_all_circles++;


                if( print_level() > mybhep::VERBOSE ){
                  std::clog << " circle  (X0 " << X0 << " +- " << X0err << ") (Y0 " << Y0 << " +- " << Y0err << ") (R " << R << " +- " << Rerr << ") " << std::endl;
                }


                selected_circle = true;
                for(size_t ii=0; ii<3; ii++){
                  if( ii == 0 ) the_sign = sign[0][i];
                  else if( ii == 1 ) the_sign = sign[1][j];
                  else the_sign = sign[2][k];
                  check[ii] = fabs(sqrt(pow(X0 - x[ii],2) + pow(Y0 - y[ii],2)) - (R + the_sign*r[ii].value()));
                  if( print_level() > mybhep::VERBOSE ){
                    std::clog << "  circle " << ii << " check " << check[ii] << std::endl;
                  }

                  if( check[ii] > tolerance_for_smallness ){ // the circle obtained is a spurious solution
                    selected_circle = false;
                  }
                }

                if( !selected_circle ) continue;

                count_tangent_circles ++;

                if( R < 0. ) continue;

                count_circles_positive_radius++;

                if( R < r[0].value() || R < r[1].value() || R < r[2].value() || R < Rmin || R > Rmax ) continue;

                count_circles_good_radius++;


                // now extend circle to helix
                // phi angle from center of solution to center of cell i; exact
                phi1 = atan2(y[0] - Y0, x[0] - X0);
                phi2 = atan2(y[1] - Y0, x[1] - X0);
                phi3 = atan2(y[2] - Y0, x[2] - X0);

                // z coordinate and pitch using only cells 1 and 2
                z0_12 = (z[0]*phi2 - z[1]*phi1)/(phi2 - phi1);
                H_12 = (z[1] - z[0])/(phi2 - phi1);

                z0_23 = (z[1]*phi3 - z[2]*phi2)/(phi3 - phi2);
                H_23 = (z[2] - z[1])/(phi3 - phi2);

                if( z0_12.is_equal_to__optimist(z0_23, nsigmas) &&
                    H_12.is_equal_to__optimist(H_23, nsigmas) ){

                  std::vector<topology::experimental_double> z0s, Hs;
                  z0s.push_back(z0_12);
                  z0s.push_back(z0_23);
                  Hs.push_back(H_12);
                  Hs.push_back(H_23);

                  helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),weighted_average(z0s),experimental_double(R,Rerr),weighted_average(Hs));


                  if( print_level() > mybhep::VERBOSE ){
                    std::clog << " circle  (X0 "; helix.x0().dump();
                    std::clog << ") (Y0 "; helix.y0().dump();
                    std::clog << ") (Z0 "; helix.z0().dump();
                    std::clog << ") (R "; helix.R().dump();
                    std::clog << ") (H "; helix.H().dump(); std::clog << ")" << std::endl;
                  }

                  count_all_helices++;

		  if( helix.x0().experimental_isnan() || 
		      helix.y0().experimental_isnan() || 
		      helix.z0().experimental_isnan() || 
		      helix.R().experimental_isnan() || 
		      helix.H().experimental_isnan() || 
		      helix.x0().experimental_isinf() || 
		      helix.y0().experimental_isinf() || 
		      helix.z0().experimental_isinf() || 
		      helix.R().experimental_isinf() || 
		      helix.H().experimental_isinf() ){
		    if( print_level() >= mybhep::NORMAL ){
		      std::clog << " problem: det " << detA << " helix is nan or inf "; helix.dump(); std::clog << " " << std::endl;
		    }
		  }else{
		    helices_.push_back(helix);
		  }
                }
              }
            }
          }
        }
      }else{ // large detA

        double alpha, beta, gamma, discr;
        double dalphadr[3], dbetadr[3], dgammadr[3], ddiscrdr[3];

        for(size_t i=0; i<2; i++){
          for(size_t j=0; j<2; j++){
            c12 = sign[0][i]*r[0].value() - sign[1][j]*r[1].value();
            dc12dr[0] = sign[0][i];
            dc12dr[1] = -sign[1][j];
            dc12dr[2] = 0.;
            for(size_t k=0; k<2; k++){
              c13 = sign[0][i]*r[0].value() - sign[2][k]*r[2].value();
              dc13dr[0] = sign[0][i];
              dc13dr[1] = 0.;
              dc13dr[2] = -sign[2][k];

              alpha = 4.*(
                          pow(-c12*A22 + c13*A21,2)
                          + pow(-c13*A11 + c12*A12,2)
                          )/pow(detA,2) - 1.;
              for(size_t ierr=0; ierr<3; ierr++)
                dalphadr[ierr] = 8.*(
                                  (-c12*A22 + c13*A21)*(-dc12dr[ierr]*A22 + dc13dr[ierr]*A21)
                                  + (-c13*A11 + c12*A12)*(-dc13dr[ierr]*A11 + dc12dr[ierr]*A12)
                                  )/pow(detA,2);


              beta = 2.*(
                         (-c12*d12*pow(A22,2) - c13*d13*pow(A21,2) + c12*d13*A22*A21 + c13*d12*A21*A22)
                         + (-c13*d13*pow(A11,2) - c12*d12*pow(A12,2) + c13*d12*A11*A12 + c12*d13*A12*A11)
                         )/pow(detA,2)
                - 2.*(
                      x[1]*(-c12*A22 + c13*A21)
                      + y[1]*(-c13*A11 + c12*A12)
                      )/detA
                - sign[1][j]*r[1].value();
              for(size_t ierr=0; ierr<3; ierr++)
                dbetadr[ierr] = 2.*(
                                 (-(dc12dr[ierr]*d12 + c12*dd12dr[ierr])*pow(A22,2) - (dc13dr[ierr]*d13 + c13*dd13dr[ierr])*pow(A21,2) + (dc12dr[ierr]*d13 + c12*dd13dr[ierr])*A22*A21 + (dc13dr[ierr]*d12 + c13*dd12dr[ierr])*A21*A22)
                                 + (-(dc13dr[ierr]*d13 + c13*dd13dr[ierr])*pow(A11,2) - (dc12dr[ierr]*d12 + c12*dd12dr[ierr])*pow(A12,2) + (dc13dr[ierr]*d12 + c13*dd12dr[ierr])*A11*A12 + (dc12dr[ierr]*d13 + c12*dd13dr[ierr])*A12*A11)
                                 )/pow(detA,2)
                  - 2.*(
                        x[1]*(-dc12dr[ierr]*A22 + dc13dr[ierr]*A21)
                        + y[1]*(-dc13dr[ierr]*A11 + dc12dr[ierr]*A12)
                        )/detA;
              dbetadr[1] -= sign[1][j];



              gamma = (
                       pow(d12*A22 - d13*A21,2) +
                       pow(d13*A11 - d12*A12,2)
                       )/pow(detA,2)
                - 2.*(
                      x[1]*(d12*A22 - d13*A21)
                      + y[1]*(d13*A11 - d12*A12)
                      )/detA
                + pow(x[1],2) + pow(y[1],2) - pow(r[1].value(),2);
              for(size_t ierr=0; ierr<3; ierr++)
                dgammadr[ierr] = 2.*(
                                  (d12*A22 - d13*A21)*(dd12dr[ierr]*A22 - dd13dr[ierr]*A21) +
                                  (d13*A11 - d12*A12)*(dd13dr[ierr]*A11 - dd12dr[ierr]*A12)
                                  )/pow(detA,2)
                  - 2.*(
                        x[1]*(dd12dr[ierr]*A22 - dd13dr[ierr]*A21)
                        + y[1]*(dd13dr[ierr]*A11 - dd12dr[ierr]*A12)
                        )/detA;
              dgammadr[1] -= 2.*r[1].value();


              discr = pow(beta,2) - alpha*gamma;
              for(size_t ierr=0; ierr<3; ierr++)
                ddiscrdr[ierr] = 2.*beta*dbetadr[ierr] - dalphadr[ierr]*gamma - alpha*dgammadr[ierr];

              if( discr < 0. ){
                continue;
              }

              for(size_t l=0; l<2; l++){
                count_all_circles++;

                R = (- beta + sign[3][l]*sqrt(discr))/alpha;
                for(size_t ierr=0; ierr<3; ierr++)
                  dRdr[ierr] = (- dbetadr[ierr] + sign[3][l]/(2.*sqrt(discr))*ddiscrdr[ierr])/alpha
                    - (- beta + sign[3][l]*sqrt(discr))/pow(alpha,2)*dalphadr[ierr];

                X0 = ((d12 - 2.*c12*R)*A22 - (d13 - 2.*c13*R)*A21)/detA;
                for(size_t ierr=0; ierr<3; ierr++)
                  dX0dr[ierr] = ((dd12dr[ierr] - 2.*dc12dr[ierr]*R - 2.*c12*dRdr[ierr])*A22 - (dd13dr[ierr] - 2.*dc13dr[ierr]*R - 2.*c13*dRdr[ierr])*A21)/detA;

                Y0 = ((d13 - 2.*c13*R)*A11 - (d12 - 2.*c12*R)*A12)/detA;
                for(size_t ierr=0; ierr<3; ierr++)
                  dY0dr[ierr] = ((dd13dr[ierr] - 2.*dc13dr[ierr]*R - 2.*c13*dRdr[ierr])*A11 - (dd12dr[ierr] - 2.*dc12dr[ierr]*R - 2.*c12*dRdr[ierr])*A12)/detA;

                Rerr=0.;
                X0err=0.;
                Y0err=0.;
                for(size_t ierr=0; ierr<3; ierr++){
                  Rerr += pow(dRdr[ierr]*r[ierr].error(),2);
                  X0err += pow(dX0dr[ierr]*r[ierr].error(),2);
                  Y0err += pow(dY0dr[ierr]*r[ierr].error(),2);
                }
                Rerr = sqrt(Rerr);
                X0err = sqrt(X0err);
                Y0err = sqrt(Y0err);


                if( print_level() > mybhep::VERBOSE ){
                  std::clog << " circle  (X0 " << X0 << " +- " << X0err << ") (Y0 " << Y0 << " +- " << Y0err << ") (R " << R << " +- " << Rerr << ") " << std::endl;
                }

                selected_circle = true;
		for(size_t ii=0; ii<3; ii++){
                  if( ii == 0 ) the_sign = sign[0][i];
                  else if( ii == 1 ) the_sign = sign[1][j];
                  else the_sign = sign[2][k];
                  check[ii] = fabs(sqrt(pow(X0 - x[ii],2) + pow(Y0 - y[ii],2)) - (R + the_sign*r[ii].value()));

                  if( print_level() > mybhep::VERBOSE ){
                    std::clog << "  circle " << ii << " check " << check[ii] << std::endl;
                  }

                  if( check[ii] > tolerance_for_smallness ){ // the circle obtained is a spurious solution
                    selected_circle = false;
                  }
                }


                if( !selected_circle ) continue;

                count_tangent_circles ++;

                if( R < 0. ) continue;

                count_circles_positive_radius++;

                if( R < r[0].value() || R < r[1].value() || R < r[2].value() || R < Rmin || R > Rmax ) continue;

                count_circles_good_radius++;


                // now extend circle to helix
                // phi angle from center of solution to center of cell i; exact
                phi1 = atan2(y[0] - Y0, x[0] - X0);
                phi2 = atan2(y[1] - Y0, x[1] - X0);
                phi3 = atan2(y[2] - Y0, x[2] - X0);

                // z coordinate and pitch using only cells 1 and 2
                z0_12 = (z[0]*phi2 - z[1]*phi1)/(phi2 - phi1);
                H_12 = (z[1] - z[0])/(phi2 - phi1);

                z0_23 = (z[1]*phi3 - z[2]*phi2)/(phi3 - phi2);
                H_23 = (z[2] - z[1])/(phi3 - phi2);

                if( z0_12.is_equal_to__optimist(z0_23, nsigmas) &&
                    H_12.is_equal_to__optimist(H_23, nsigmas) ){
                  std::vector<topology::experimental_double> z0s, Hs;
                  z0s.push_back(z0_12);
                  z0s.push_back(z0_23);
                  Hs.push_back(H_12);
                  Hs.push_back(H_23);

                  helix.set(experimental_double(X0,X0err),experimental_double(Y0,Y0err),weighted_average(z0s),experimental_double(R,Rerr),weighted_average(Hs));

                  if( print_level() > mybhep::VERBOSE ){
                    std::clog << " circle  (X0 "; helix.x0().dump();
                    std::clog << ") (Y0 "; helix.y0().dump();
                    std::clog << ") (Z0 "; helix.z0().dump();
                    std::clog << ") (R "; helix.R().dump();
                    std::clog << ") (H "; helix.H().dump(); std::clog << ")" << std::endl;
                  }

                  count_all_helices++;

		  if( helix.x0().experimental_isnan() || 
		      helix.y0().experimental_isnan() || 
		      helix.z0().experimental_isnan() || 
		      helix.R().experimental_isnan() || 
		      helix.H().experimental_isnan() || 
		      helix.x0().experimental_isinf() || 
		      helix.y0().experimental_isinf() || 
		      helix.z0().experimental_isinf() || 
		      helix.R().experimental_isinf() || 
		      helix.H().experimental_isinf() ){
		    if( print_level() >= mybhep::NORMAL ){
		      std::clog << " problem: det " << detA << " helix is nan or inf "; helix.dump(); std::clog << " " << std::endl;
		    }
		  }else{
		    helices_.push_back(helix);
		  }

                  helices_.push_back(helix);
                }
              }
            }
          }
        }
      }

      if( print_level() >= mybhep::NORMAL ){
        if( count_circles_positive_radius != 8 ){
          std::clog << " problem: det " << detA << " one triplet of cells has produced " << count_tangent_circles << " tangent circles of which " << count_circles_positive_radius << " have positive radius (expected to be 8) "  << std::endl;
        }
      }


      if( print_level() > mybhep::VERBOSE ){
        std::clog << " summary of helices production for this triplet " << std::endl;
        std::clog << " count_all_circles " << count_all_circles << std::endl;
        std::clog << " count_tangent_circles " << count_tangent_circles << std::endl;
        std::clog << " count_circles_positive_radius " << count_circles_positive_radius << std::endl;
        std::clog << " count_circles_good_radius " << count_circles_good_radius << std::endl;
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
