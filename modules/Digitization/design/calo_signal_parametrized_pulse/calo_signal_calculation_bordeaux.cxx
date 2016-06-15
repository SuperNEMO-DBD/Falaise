// Standard library :
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <vector>
#include <stdint.h>

void electron_700keV()
{
  // 700 keV electrons : 
  double borne_1 = 80.3959;
  double borne_2 = 84.2722;
  
  // First gaussian :  
  double gauss1_constant = -0.190845;
  double gauss1_mean     = 80.9305;
  double gauss1_sigma    = 2.46359;
  
  // Second gaussian :
  double gauss2_constant = -0.206519;
  double gauss2_mean     = 82.1373;
  double gauss2_sigma    = 4.25277;
  
  // Exponential :
  double expo_constant    = 0.000205168;
  double expo_translation = 152.199;
  double expo_tau         = 9.98859;

  // The value x is in nanoseconds :
  double x = -1;
  
  // The result value y=f(x)=.... is in volts
  double result = -1;

  // std::clog << "Entrez la valeur de x : " << std::endl;
  // std::cin >> x;
  // std::clog << "valeur de x = " << x << std::endl;

  std::ofstream fichier("calo_signal_electron_700keV.out", std::ios::out | std::ios::trunc);  
  if(fichier)
    {
      for (x = 0; x <= 200; x+=0.01)
	{
      
	  if (x >= 0 && x <= borne_1)
	    {
	      double part_1 = 1 / (gauss1_sigma * std::sqrt(2 * M_PI));
	      double part_2 = - std::pow((x - gauss1_mean),2);
	      double part_3 = (2 * gauss1_sigma * gauss1_sigma);
	      double exp    = std::exp(part_2 / part_3);
	      // std::clog << "Part 1 = " << part_1 << std::endl;
	      // std::clog << "Part 2 = " << part_2 << std::endl;
	      // std::clog << "Part 3 = " << part_3 << std::endl;
	      // std::clog << "exp    = " << exp << std::endl;

	      result = (gauss1_constant) * std::exp(-(std::pow((x-gauss1_mean),2))/(2 * gauss1_sigma * gauss1_sigma));
	    }
	  else if (x > borne_1 && x <= borne_2)
	    {
	      result = (gauss2_constant) * std::exp(-(std::pow((x-gauss2_mean),2))/(2 * gauss2_sigma * gauss2_sigma));
	    }
	  else if (x > borne_2)
	    {
	      result = - (expo_constant * std::exp((-(x - expo_translation))/ expo_tau));	      
	    }
  
	  //std::clog << "Resultat = " << result << std::endl;
  
 
	  fichier << x << " " << result << std::endl;

	}
      fichier.close();
    }
  
  else  std::cerr << "Erreur à l'ouverture !" << std::endl;

  return;
}

void electron_1000keV()
{
  // 1000 keV electrons : 
  double borne_1 = 80.8924;
  double borne_2 = 84.7552;
  
  // First gaussian :  
  double gauss1_constant = -0.276085;
  double gauss1_mean     = 81.4763;
  double gauss1_sigma    = 2.46067;
  
  // Second gaussian :
  double gauss2_constant = -0.299236;
  double gauss2_mean     = 82.5765;
  double gauss2_sigma    = 4.3108;
  
  // Exponential :
  double expo_constant    = 0.000271238;
  double expo_translation = 153.753;
  double expo_tau         = 10.0078;

  // The value x is in nanoseconds :
  double x = -1;
  
  // The result value y=f(x)=.... is in volts
  double result = -1;

  std::ofstream fichier("calo_signal_electron_1000keV.out", std::ios::out | std::ios::trunc);  
  if(fichier)
    {
      for (x = 0; x <= 200; x+=0.01)
	{
      
	  if (x >= 0 && x <= borne_1)
	    {
	      result = (gauss1_constant) * std::exp(-(std::pow((x-gauss1_mean),2))/(2 * gauss1_sigma * gauss1_sigma));
	    }
	  else if (x > borne_1 && x <= borne_2)
	    {
	      result = (gauss2_constant) * std::exp(-(std::pow((x-gauss2_mean),2))/(2 * gauss2_sigma * gauss2_sigma));
	    }
	  else if (x > borne_2)
	    {
	      result = - (expo_constant * std::exp((-(x - expo_translation))/ expo_tau));	      
	    }
 
	  fichier << x << " " << result << std::endl;

	}
      fichier.close();
    }
  
  else  std::cerr << "Erreur à l'ouverture !" << std::endl;

  return;
}

void electron_1300keV()
{
  // 1300 keV electrons : 
  double borne_1 = 81.1960;
  double borne_2 = 85.1490;
  
  // First gaussian :  
  double gauss1_constant = -0.361619;
  double gauss1_mean     = 81.8921;
  double gauss1_sigma    = 2.46434;
  
  // Second gaussian :
  double gauss2_constant = -0.392577;
  double gauss2_mean     = 83.0154;
  double gauss2_sigma    = 4.419;
  
  // Exponential :
  double expo_constant    = 0.00032172;
  double expo_translation = 155.138;
  double expo_tau         = 10.0011;

  // The value x is in nanoseconds :
  double x = -1;
  
  // The result value y=f(x)=.... is in volts
  double result = -1;

  std::ofstream fichier("calo_signal_electron_1300keV.out", std::ios::out | std::ios::trunc);  
  if(fichier)
    {
      for (x = 0; x <= 200; x+=0.01)
	{
      
	  if (x >= 0 && x <= borne_1)
	    {
	      result = (gauss1_constant) *  std::exp(-(std::pow((x-gauss1_mean),2))/(2 * gauss1_sigma * gauss1_sigma));
	    }
	  else if (x > borne_1 && x <= borne_2)
	    {
	      result = (gauss2_constant) * std::exp(-(std::pow((x-gauss2_mean),2))/(2 * gauss2_sigma * gauss2_sigma));
	    }
	  else if (x > borne_2)
	    {
	      result = - (expo_constant * std::exp((-(x - expo_translation))/ expo_tau));	      
	    }
 
	  fichier << x << " " << result << std::endl;

	}
      fichier.close();
    }
  
  else  std::cerr << "Erreur à l'ouverture !" << std::endl;

  return;
}


int main(int  argc_ , char ** argv_)
{
  std::clog << "Litle program to calculate the calorimeter signal. \n"
	    << "Three functions are used to reproduce the calo signal. \n"
	    << "A first gaussian, a second gaussian and then an exponential."
	    << "3 files are produced in your current directory : \n"
            << "calo_signal_electron_700keV.out, calo_signal_electron_1000keV.out and calo_signal_electron_1300keV.out" << std::endl;

  electron_700keV();
  electron_1000keV();
  electron_1300keV();
  return 0;
}
