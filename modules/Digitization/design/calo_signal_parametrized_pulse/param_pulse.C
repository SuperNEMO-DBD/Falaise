void param_pulse()
{
  float tab[2000];

  TF1 *Parametrized_Pulse = new TF1("Parametrized_Pulse"," (x > [0] && x < [1])*gaus(2) + ((x>[1] && x<[5])*gaus(6)) -(x>[5] && x< 200)*([9]*(exp(-(x-[10])/[11])))",0,200);
  TH1F* Histo = new TH1F("Histo","Histo",500,0,200);
  TH1F* Interpolate = new TH1F("Inter","Inter",20000,0,200);

  Parametrized_Pulse->FixParameter(0,0);
  Parametrized_Pulse->FixParameter(1,80.8924);
  Parametrized_Pulse->FixParameter(2,-0.276085);
  Parametrized_Pulse->FixParameter(3,81.4763);
  Parametrized_Pulse->FixParameter(4,2.46067);
  Parametrized_Pulse->FixParameter(5,84.7552);
  Parametrized_Pulse->FixParameter(6,-0.299236);
  Parametrized_Pulse->FixParameter(7,82.5765);
  Parametrized_Pulse->FixParameter(8,4.3108);
  Parametrized_Pulse->FixParameter(9,0.000271238);
  Parametrized_Pulse->FixParameter(10,153.753);
  Parametrized_Pulse->FixParameter(11,10.0078);

  TCanvas * canvas1 = new TCanvas("canvas1", "Photoelectrons Distribution", 1250,1000);
  canvas1->cd();
  Parametrized_Pulse->SetNpx(5000);

  float a =0;
  for (int i =0; i<= 500; i++)
    {
      float tps = a/2.5;
      tab[i] = Parametrized_Pulse->Eval(tps);
      cout << "tab[" << tps << "] = " << tab[i] << endl;
      Histo->SetBinContent(i, tab[i]);
      a++;
    }
  
  float interpolate =0;
  float b=0;
  for (int i =0; i<= 20000; i++)
    {
      float tps = b/100;
      interpolate = Histo->Interpolate(tps);
      Interpolate->SetBinContent(i, interpolate);
      b++;
    }
  
  Parametrized_Pulse->Draw();
  Histo->Draw("same");
  Interpolate->SetLineColor(kGreen);
  Interpolate->Draw("same");
}

// void param_pulse()
// {

//   TF1 *Parametrized_Pulse = new TF1("Parametrized_Pulse"," (x > [0] && x < [1])*gaus(2) + ((x>[1] && x<[5])*gaus(6)) -(x>[5] && x< 200)*([9]*(exp(-(x-[10])/[11])))",0,200);
//   Parametrized_Pulse->FixParameter(0,0);
//   Parametrized_Pulse->FixParameter(1,80.8924);
//   Parametrized_Pulse->FixParameter(2,-0.276085);
//   Parametrized_Pulse->FixParameter(3,81.4763);
//   Parametrized_Pulse->FixParameter(4,2.46067);
//   Parametrized_Pulse->FixParameter(5,84.7552);
//   Parametrized_Pulse->FixParameter(6,-0.299236);
//   Parametrized_Pulse->FixParameter(7,82.5765);
//   Parametrized_Pulse->FixParameter(8,4.3108);
//   Parametrized_Pulse->FixParameter(9,0.000271238);
//   Parametrized_Pulse->FixParameter(10,153.753);
//   Parametrized_Pulse->FixParameter(11,10.0078);


//   TCanvas * canvas1 = new TCanvas("canvas1", "Photoelectrons Distribution", 1250,1000);
//   canvas1->cd();
//   Parametrized_Pulse->SetNpx(5000);
//   //Parametrized_Pulse->Draw();

//   float a =0;
//   for (int i =0; i<= 500; i++)
//     {
//       float tps = a/2.5;
//       tab[i] = Parametrized_Pulse->Eval(tps);
//       cout << "tab[" << tps << "] = " << tab[i] << endl;
//       Histo->SetBinContent(i, tab[i]);
//       a++;
//     }

  
//   float interpolate =0;
//   float b=0;
//   for (int i =0; i<= 20000; i++)
//     {
//       float tps = b/100;
//       interpolate = Histo->Interpolate(tps);
//       Interpolate->SetBinContent(i, interpolate);
//       b++;
//     }
  

//   //Parametrized_Pulse->Draw();
//   Histo->Draw();
//   Interpolate->SetLineColor(kRed);
//   Interpolate->Draw("same");
// }
