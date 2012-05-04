//#############################################################################
//
// HistogramFunctions.cpp - these functions are useful for working with ROOT
//                          histograms.  For example, dividing two histograms 
//                          and assigning binomial errors, or averaging over
//                          a dimension to reduce the dimensionality by 1. 
//
//
// D. Schmitz  October 28, 2005
//
//#############################################################################

#include "CATUtils/NHistogramFunctions2.h"
#include<cmath>

namespace CAT{
  namespace utils{

    //=============================================================================
    // divide3D( )
    //
    // note that -99 is the flag for a 0 denominator. we need to distinguish this 
    // case from the 0 / 4 case, for example.
    // errors are binomial errors!!
    //=============================================================================
    TH3F* Divide3D( TH3F *num, TH3F *den ){

      double* x_bins = new double[num->GetNbinsX()+1];
      double* y_bins = new double[num->GetNbinsY()+1];
      double* z_bins = new double[num->GetNbinsZ()+1];
      for( int i = 0; i <= num->GetNbinsX(); i++ )
        x_bins[i] = num->GetXaxis()->GetBinLowEdge(i+1);
      for( int i = 0; i <= num->GetNbinsY(); i++ )
        y_bins[i] = num->GetYaxis()->GetBinLowEdge(i+1);
      for( int i = 0; i <= num->GetNbinsZ(); i++ )
        z_bins[i] = num->GetZaxis()->GetBinLowEdge(i+1);
    
      TH3F *result = new TH3F( "result", "result", num->GetNbinsX(), x_bins, num->GetNbinsY(), y_bins, num->GetNbinsZ(), z_bins );

      for( int i = 1; i <= num->GetNbinsX(); i++ )
        {
          for( int j = 1; j <= num->GetNbinsY(); j++ )
            {
              for( int k = 1; k <= num->GetNbinsZ(); k++ )
                {
                  if(  den->GetBinContent(i,j,k) != 0 )
                    {
                      result->SetBinContent(i,j,k, num->GetBinContent(i,j,k)/den->GetBinContent(i,j,k) );
                      double error = sqrt( fabs((num->GetBinContent(i,j,k) * ( 1.0 - num->GetBinContent(i,j,k) / den->GetBinContent(i,j,k) )))) / den->GetBinContent(i,j,k);
                      result->SetBinError(i,j,k,error);
                    }
                  else
                    {
                      result->SetBinContent(i,j,k,-99);
                      result->SetBinError(i,j,k,1);
                    }
                }
            }
        }
                           
      return result;

    }


    //=============================================================================
    // divide2D( )
    //
    // note that -99 is the flag for a 0 denominator. we need to distinguish this 
    // case from the 0 / 4 case, for example.
    // errors are binomial errors!!
    //=============================================================================
    TH2F* Divide2D( TH2F *num, TH2F *den ){

      double* x_bins = new double[num->GetNbinsX()+1];
      double* y_bins = new double[num->GetNbinsY()+1];
      for( int i = 0; i <= num->GetNbinsX(); i++ )
        x_bins[i] = num->GetXaxis()->GetBinLowEdge(i+1);
      for( int i = 0; i <= num->GetNbinsY(); i++ )
        y_bins[i] = num->GetYaxis()->GetBinLowEdge(i+1);
    
      TH2F *result = new TH2F( "result", "result", num->GetNbinsX(), x_bins, num->GetNbinsY(), y_bins );

      for( int i = 1; i <= num->GetNbinsX(); i++ )
        {
          for( int j = 1; j <= num->GetNbinsY(); j++ )
            {
              if(  den->GetBinContent(i,j) != 0 )
                {
                  result->SetBinContent(i,j, num->GetBinContent(i,j)/den->GetBinContent(i,j) );
                  double error = sqrt( fabs((num->GetBinContent(i,j) * ( 1.0 - num->GetBinContent(i,j) / den->GetBinContent(i,j) )))) / den->GetBinContent(i,j);
                  result->SetBinError(i,j,error);

                }
              else
                {
                  result->SetBinContent(i,j,-99);
                  result->SetBinError(i,j,1);
                }
            }
        }
  
      return result;

    }


    //=============================================================================
    // divide1D( )
    //
    // note that -99 is the flag for a 0 denominator. we need to distinguish this 
    // case from the 0 / 4 case, for example.
    // errors are binomial errors!!
    //=============================================================================
    TH1F* Divide1D( TH1F *num, TH1F *den ){

      double* x_bins = new double[num->GetNbinsX()+1];
      for( int i = 0; i <= num->GetNbinsX(); i++ )
        x_bins[i] = num->GetXaxis()->GetBinLowEdge(i+1);
    
      TH1F *result = new TH1F( "result", "result", num->GetNbinsX(), x_bins );

      for( int i = 1; i <= num->GetNbinsX(); i++ )
        {
          if(  den->GetBinContent(i) != 0 )
            {
              result->SetBinContent(i, num->GetBinContent(i)/den->GetBinContent(i) );
              double error = sqrt( fabs((num->GetBinContent(i) * ( 1.0 - num->GetBinContent(i) / den->GetBinContent(i) )))) / den->GetBinContent(i);
              result->SetBinError(i,error);
  
            }
          else
            {
              ;
              //      result->SetBinContent(i,-99);
              //      result->SetBinError(i,1);
            }
        }
  
      return result;

    }



    //=============================================================================
    // average3D_to_2D()
    //=============================================================================
    TH2F* Average3D_to_2D( TH3F *object, char *axis1, char *axis2 ){

      int xbins=0, ybins=0, zbins=0;
      double* x_bins;
      double* y_bins;
      double* z_bins;
      char *axis3;
      char axes[3];


      if( (0 == strcmp(axis1,"1") && 0 == strcmp(axis2,"2")) || (0 == strcmp(axis1,"2") && 0 == strcmp(axis2,"1")) )
        axis3 = "3";
      if( (0 == strcmp(axis1,"1") && 0 == strcmp(axis2,"3")) || (0 == strcmp(axis1,"3") && 0 == strcmp(axis2,"1")) )
        axis3 = "2";
      if( (0 == strcmp(axis1,"2") && 0 == strcmp(axis2,"3")) || (0 == strcmp(axis1,"3") && 0 == strcmp(axis2,"2")) )
        axis3 = "1";

      sprintf( axes, "%s%s%s", axis1, axis2, axis3 );

      if( 0 == strcmp(axes,"123") )
        { 
          xbins = object->GetNbinsX();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsY();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
          zbins = object->GetNbinsZ();
          z_bins = new double[zbins+1];
          for( int i = 0; i <= zbins; i++ )
            z_bins[i] = object->GetZaxis()->GetBinLowEdge(i+1);
        }
      else if( 0 == strcmp(axes,"132") )
        { 
          xbins = object->GetNbinsX();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsZ();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetZaxis()->GetBinLowEdge(i+1);
          zbins = object->GetNbinsY();
          z_bins = new double[zbins+1];
          for( int i = 0; i <= zbins; i++ )
            z_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
        }
      else if( 0 == strcmp(axes,"213") )
        {
          xbins = object->GetNbinsY();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsX();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
          zbins = object->GetNbinsZ();
          z_bins = new double[zbins+1];
          for( int i = 0; i <= zbins; i++ )
            z_bins[i] = object->GetZaxis()->GetBinLowEdge(i+1);
        }
      else if( 0 == strcmp(axes,"231") )
        { 
          xbins = object->GetNbinsY();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsZ();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetZaxis()->GetBinLowEdge(i+1);
          zbins = object->GetNbinsX();
          z_bins = new double[zbins+1];
          for( int i = 0; i <= zbins; i++ )
            z_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
        }
      else if( 0 == strcmp(axes,"312") )
        {
          xbins = object->GetNbinsZ();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetZaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsX();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
          zbins = object->GetNbinsY();
          z_bins = new double[zbins+1];
          for( int i = 0; i <= zbins; i++ )
            z_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
        }
      else if( 0 == strcmp(axes,"321") )
        { 
          xbins = object->GetNbinsZ();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetZaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsY();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
          zbins = object->GetNbinsX();
          z_bins = new double[zbins+1];
          for( int i = 0; i <= zbins; i++ )
            z_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
        }


      TH2F *result = new TH2F("result", "result", xbins, x_bins, ybins, y_bins );

      double sum = 0;
      int count = 0;
      double errorSum2=0;

      for( int i = 1; i <= xbins; i++ )
        {
          for( int j = 1; j <= ybins; j++ )
            {
              count = 0;
              sum = 0;
              errorSum2 = 0;
              for( int k = 1; k <= zbins; k++ )
                {
                  if( 0 == strcmp(axes,"123") ){
                    if( object->GetBinContent(i,j,k) != -99 ){
                      sum += object->GetBinContent(i,j,k);
                      count++;
                      errorSum2 += pow(object->GetBinError(i,j,k),2);
                    }
                  }else if( 0 == strcmp(axes,"132") ){
                    if( object->GetBinContent(i,k,j) != -99 ){
                      sum += object->GetBinContent(i,k,j);
                      count++;
                      errorSum2 += pow(object->GetBinError(i,k,j),2);
                    }
                  }else if( 0 == strcmp(axes,"213") ){
                    if( object->GetBinContent(j,i,k) != -99 ){
                      sum += object->GetBinContent(j,i,k);
                      count++;
                      errorSum2 += pow(object->GetBinError(j,i,k),2);
                    }
                  }else if( 0 == strcmp(axes,"231") ){
                    if( object->GetBinContent(k,i,j) != -99 ){
                      sum += object->GetBinContent(k,i,j);
                      count++;
                      errorSum2 += pow(object->GetBinError(k,i,j),2);
                    }
                  }else if( 0 == strcmp(axes,"312") ){
                    if( object->GetBinContent(j,k,i) != -99 ){
                      sum += object->GetBinContent(j,k,i);
                      count++;
                      errorSum2 += pow(object->GetBinError(j,k,i),2);
                    }
                  }else if( 0 == strcmp(axes,"321") ){
                    if( object->GetBinContent(k,j,i) != -99 ){
                      sum += object->GetBinContent(k,j,i);
                      count++;
                      errorSum2 += pow(object->GetBinError(k,j,i),2);
                    }
                  }
                }
              if( count != 0 )
                {
                  result->SetBinContent(i,j,sum/(double)count);
                  double error = sqrt( errorSum2 ) / count;
                  result->SetBinError(i,j, error );
                  //cout << i << ", " << j << " = " << sum << " / " << count << " = " 
                  //<< result->GetBinContent(i,j) << " +- " << error << endl;
                }
              else
                {
                  result->SetBinContent(i,j,0);
                  result->SetBinError(i,j,0);
                }
            }
        }

      return result;
    }


    //=============================================================================
    // average2D_to_1D()
    //=============================================================================
    TH1F* Average2D_to_1D( TH2F* object, char *axis1 ){

      int xbins=0, ybins=0;
      double* x_bins;
      double* y_bins;
      char *axis2;

      if( 0 == strcmp(axis1,"1") )
        {
          axis2 = "2";
          xbins = object->GetNbinsX();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsY();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
        }
      else
        {
          axis2 = "1";
          xbins = object->GetNbinsY();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsX();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
        }

      TH1F *result = new TH1F( "result", "result", xbins, x_bins );

      double sum = 0;
      int count = 0;
      double errorSum2=0;

      for( int i = 1; i <= xbins; i++ )
        {
          sum = 0;
          count = 0;
          errorSum2 = 0;
          for( int j = 1; j <= ybins; j++ )
            {
              if( 0 == strcmp(axis1,"1") ){
                if( object->GetBinContent(i,j) != -99 )
                  {
                    sum += object->GetBinContent(i,j);
                    count++;
                    errorSum2 += pow(object->GetBinError(i,j),2);
                  }
              }else{
                if( object->GetBinContent(j,i) != -99 )
                  {
                    sum += object->GetBinContent(j,i);
                    count++;
                    errorSum2 += pow(object->GetBinError(j,i),2);
                  }
              }
            }
          if( count != 0 )
            {
              result->SetBinContent(i,sum/(double)count);
              double error = sqrt( errorSum2 ) / count;
              result->SetBinError(i,error );
              //cout << i << " = " << sum << " / " << count << " = " 
              //<< result->GetBinContent(i) << " +- " << error << endl;
            }
          else
            {
              result->SetBinContent(i,-99);
              result->SetBinError(i,1);
            }
        }

      return result;
    
    }


    //=============================================================================
    // integrate2D_to_1D()
    //=============================================================================
    TH1F* Integrate2D_to_1D( TH2F* object, char *axis1, double mult, char *option ){

      int xbins=0, ybins=0;
      double* x_bins;
      double* y_bins;
      char *axis2;

      if( 0 == strcmp(axis1,"1") )
        {
          axis2 = "2";
          xbins = object->GetNbinsX();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsY();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
        }
      else
        {
          axis2 = "1";
          xbins = object->GetNbinsY();
          x_bins = new double[xbins+1];
          for( int i = 0; i <= xbins; i++ )
            x_bins[i] = object->GetYaxis()->GetBinLowEdge(i+1);
          ybins = object->GetNbinsX();
          y_bins = new double[ybins+1];
          for( int i = 0; i <= ybins; i++ )
            y_bins[i] = object->GetXaxis()->GetBinLowEdge(i+1);
        }

      TH1F *result = new TH1F( "result", "result", xbins, x_bins );

      double sum = 0;
      int count = 0;
      double errorSum2=0;

      for( int i = 1; i <= xbins; i++ )
        {
          sum = 0;
          count = 0;
          errorSum2 = 0;
          for( int j = 1; j <= ybins; j++ )
            {
              if( 0 == strcmp(axis1,"1") ){
                if( object->GetBinContent(i,j) != -99 )
                  {
                    if( strcmp( option, "omega" ) == 0 )
                      {
                        sum += object->GetBinContent(i,j) * 
                          (cos(object->GetYaxis()->GetBinLowEdge(j)) - cos(object->GetYaxis()->GetBinUpEdge(j))) * mult;
                        count++;
                        errorSum2 += pow(object->GetBinError(i,j),2) *
                          (cos(object->GetYaxis()->GetBinLowEdge(j)) - cos(object->GetYaxis()->GetBinUpEdge(j))) * mult;
                      }
                    else
                      {
                        sum += object->GetBinContent(i,j) * object->GetYaxis()->GetBinWidth(j) * mult;
                        count++;
                        errorSum2 += pow(object->GetBinError(i,j),2);
                      }
                  }
              }else{
                if( object->GetBinContent(j,i) != -99 )
                  {
                    sum += object->GetBinContent(j,i) * object->GetXaxis()->GetBinWidth(j) * mult;
                    count++;
                    errorSum2 += pow(object->GetBinError(j,i),2) * object->GetXaxis()->GetBinWidth(j) * mult;;
                  }
              }
            }
          if( count != 0 )
            {
              result->SetBinContent(i,sum);
              double error = sqrt( errorSum2 );
              result->SetBinError(i,error );
              //cout << i << " = " << sum << " / " << count << " = " 
              //<< result->GetBinContent(i) << " +- " << error << endl;
            }
          else
            {
              result->SetBinContent(i,-99);
              result->SetBinError(i,1);
            }
        }

      return result;
    
    }


    //=============================================================================
    // integrate2D()
    //=============================================================================
    double Integrate2D( TH2F* object, double mult, char *option ){

      double sum = 0;

      for( int i = 1; i <= object->GetNbinsX(); i++ )
        {
          for( int j = 1; j <= object->GetNbinsY(); j++ )
            {
              if( strcmp( option, "omega" ) == 0 )
                {
                  sum += object->GetBinContent(i,j) * 
                    object->GetXaxis()->GetBinWidth(i) *
                    (cos(object->GetYaxis()->GetBinLowEdge(j)) - cos(object->GetYaxis()->GetBinUpEdge(j))) * mult;
                }
              else
                {
                  sum += object->GetBinContent(i,j) * 
                    object->GetXaxis()->GetBinWidth(i) * 
                    object->GetYaxis()->GetBinWidth(j) * mult;
                }
            }
        }

      return sum;
    
    }


    //=============================================================================
    // convert_to_pth( )
    //=============================================================================
    TH2F* Convert_to_pth( TH3F* object ){

      double* p_vector = new double[object->GetNbinsX()+1];
      for( int i = 0; i <= object->GetNbinsX(); i++ )
        p_vector[i] = object->GetXaxis()->GetBinLowEdge(i+1);

      int th_bins;
      double* th_vector;
      // even number of bins with 0 a bin edge
      if( 2*(object->GetNbinsY()/2) == object->GetNbinsY()/2 )
        {    
          th_bins = object->GetNbinsY()/2;
          th_vector = new double[object->GetNbinsY()/2+1];
          int j;
          th_vector[0] = 0;
          for( int i = 1; i <= object->GetNbinsY()/2; i++ )
            {
              j = i + object->GetNbinsY()/2;
              th_vector[i] = object->GetYaxis()->GetBinLowEdge(j+1);
            }
        }
      else   // odd number of bins with 0 at center of middle bin
        {
          th_bins = object->GetNbinsY()/2+1;
          th_vector = new double[object->GetNbinsY()/2+2];
          int j;
          th_vector[0] = 0;
          for( int i = 1; i <= object->GetNbinsY()/2; i++ )
            {
              j = i + object->GetNbinsY()/2;
              th_vector[i] = object->GetYaxis()->GetBinLowEdge(j+1);
            }
          th_vector[object->GetNbinsY()/2+1] = object->GetYaxis()->GetBinUpEdge(object->GetNbinsY());
        }


      TH2F *result = new TH2F("result","result", object->GetNbinsX(), p_vector, th_bins, th_vector );


      for( int i = 1; i <= object->GetNbinsX(); i++ )
        {
          for( int j = 1; j <= object->GetNbinsY(); j++ )
            {
              for( int k = 1; k <= object->GetNbinsZ(); k++ )
                {
                  double p = object->GetXaxis()->GetBinCenter(i);
                  double thx = object->GetYaxis()->GetBinCenter(j);
                  double thy = object->GetZaxis()->GetBinCenter(k);
                  double th = atan( sqrt( tan(thx)*tan(thx) + tan(thy)*tan(thy) ) );
                  if( object->GetBinContent(i,j,k) != -99 )
                    result->Fill( p, th, object->GetBinContent(i,j,k) );
                }
            }
        }
 
      return result;

    }

  

  } // end of namespace utils
} // end of namespace CAT



//#############################################################################
//
// END 
//
//#############################################################################
