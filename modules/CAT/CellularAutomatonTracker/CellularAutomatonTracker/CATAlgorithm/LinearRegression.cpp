/* -*- mode: c++ -*- */
#include <CATAlgorithm/LinearRegression.h>
#if CAT_WITH_DEVEL_ROOT == 1
#include <Riostream.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TGraphErrors.h>
#include <TDecompChol.h>
#include <TDecompSVD.h>
#include <TF1.h>
#endif // CAT_WITH_DEVEL_ROOT == 1

namespace CAT {
namespace topology{



    //! set
    void LinearRegression::set(const std::vector<experimental_double> &xi, const std::vector<experimental_double> &yi)
      {
        xi_ = xi;
        yi_ = yi;
      }


    //! set xi
    void LinearRegression::set_xi(const std::vector<experimental_double> &xi)
      {
        xi_ = xi;
      }

    //! set yi
    void LinearRegression::set_yi(const std::vector<experimental_double> &yi)
      {
        yi_ = yi;
      }

    //! set y0
    void LinearRegression::set_y0(const experimental_double &y0)
      {
        y0_ = y0;
      }

    //! set tangent
    void LinearRegression::set_tangent(const experimental_double &tangent)
      {
        tangent_ = tangent;
      }

    //! get xi
    const std::vector<experimental_double>& LinearRegression::xi()const
    {
      return xi_;
    }

    //! get yi
    const std::vector<experimental_double>& LinearRegression::yi()const
    {
      return yi_;
    }

    //! get y0
    const experimental_double& LinearRegression::y0()const
    {
      return y0_;
    }

    //! get tangent
    const experimental_double& LinearRegression::tangent()const
    {
      return tangent_;
    }



    bool LinearRegression::fit(void){


      if( xi_.size() != yi_.size() ){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "CAT::LinearRegression::fit: problem: in least square regression, sizes x " << xi_.size() << " y " << yi_.size() << std::endl;
        }
        return false;
      }

      double Sw = 0.;
      double Swxx = 0.;
      double Swx = 0.;
      double Swxy = 0.;
      double Swy = 0.;

      for(std::vector<experimental_double>::iterator it=xi_.begin(); it != xi_.end(); ++it)
        {
          double w = 1./(mybhep::square(it->error()));
          Sw += w;
          Swxx += w*mybhep::square(it->value());
          Swx += w*it->value();
          double y = yi_[it - xi_.begin()].value();
          Swxy += w*it->value()*y;
          Swy += w*y;
        }

      double delta = Sw*Swxx - mybhep::square(Swx);

      if( delta == 0.){
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "CAT::LinearRegression::fit: problem: in least square regression, delta " << delta << " Sw " << Sw << " Swx " << Swx << " Swxx " << Swxx << std::endl;
        }
        return false;
      }

      double a = (Swxx*Swy - Swx*Swxy)/delta;
      double b = (Sw*Swxy - Swx*Swy)/delta;
      double erra, errb;

      if( Swxx/delta > 0. ){
        erra = std::sqrt(Swxx/delta);
      }
      else{
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "CAT::LinearRegression::fit: problem: linear regression sy02 " << Swxx/delta << " Swxx " << Swxx << " delta " << delta << std::endl;
        }
        return false;
      }

      if( Sw/delta > 0. ){
        errb = std::sqrt(Sw/delta);
      }
      else{
        if( print_level() >= mybhep::NORMAL ){
          std::clog << "CAT::LinearRegression::fit: problem: linear regression stangent2 " << Sw/delta << " Sw " << Sw << " delta " << delta << std::endl;
        }
        return false;
      }

      set_y0(experimental_double(a, erra));
      set_tangent(experimental_double(b, errb));

      return true;

    }


#if CAT_WITH_DEVEL_ROOT == 1
    bool LinearRegression::root_fit(void){

      if( !xi_.size() ) return false;

      Double_t *array_x = new Double_t[xi_.size()];
      Double_t *array_y = new Double_t[xi_.size()];
      Double_t *array_xerr = new Double_t[xi_.size()];
      Double_t *array_yerr = new Double_t[xi_.size()];

      Double_t xmin = 9999999.;
      Double_t xmax = -9999999.;

      for(std::vector<experimental_double>::iterator ix=xi_.begin(); ix!=xi_.end(); ix++){
        size_t index = ix - xi_.begin();
        array_x[index] = ix->value();
        array_y[index] = yi_[index].value();
        array_xerr[index] = xi_[index].error();
        array_yerr[index] = yi_[index].error();
        if( array_x[index] > xmax ) xmax = array_x[index];
        if( array_x[index] < xmin ) xmin = array_x[index];
      }

      // Make the vectors 'Use" the data : they are not copied, the vector data
      // pointer is just set appropriately

      TVectorD x; x.Use(xi_.size(),array_x);
      TVectorD y; y.Use(xi_.size(),array_y);
      //      TVectorD ex; ex.Use(xi_.size(),array_xerr);
      TVectorD ey; ey.Use(xi_.size(),array_yerr);

      TMatrixD A(xi_.size(), 2);
      TMatrixDColumn(A,0) = 1.0;
      TMatrixDColumn(A,1) = x;

      // first bring the weights in place
      TMatrixD Aw = A;
      TVectorD yw = y;
      for (Int_t irow = 0; irow < A.GetNrows(); irow++) {
        TMatrixDRow(Aw,irow) *= 1/ey(irow);
        yw(irow) /= ey(irow);
      }

      TDecompSVD svd(Aw);
      Bool_t ok;

      const TVectorD c_svd = svd.Solve(yw,ok);

      delete array_x;
      delete array_y;
      delete array_xerr;
      delete array_yerr;

      set_y0(experimental_double(c_svd(0), 0.));
      set_tangent(experimental_double(c_svd(1), 0.));

      return true;

    }
#endif // CAT_WITH_DEVEL_ROOT == 1

    experimental_double LinearRegression::position(const experimental_double &x){

      return y0() + tangent()*x;
    }


    void LinearRegression::invert(){
      // go from:
      // y = y0 + tangent x
      // to
      // x = y0' + tangent' y
      // y0' = - y0/tangent = - y0 tangent'
      // tangent' = 1 / tangent

      std::vector<experimental_double> tmp = xi();
      set_xi(yi());
      set_yi(tmp);
      set_y0(- y0()/tangent());
      set_tangent(1./tangent());


    }



  };

}
