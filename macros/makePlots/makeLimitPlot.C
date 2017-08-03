#include "CMS_lumi.h"

void makeLimitPlot(string inputCombineName,
		   string outputDIR){


  gROOT->SetBatch(kTRUE);
  setTDRStyle();
  system(("mkdir -p "+outputDIR).c_str());

  TFile* inputCombinFile = TFile::Open(inputCombineName.c_str(),"READ");
  TTree* limit = (TTree*) inputCombinFile->Get("limit");

  TGraph* obs_limit = new TGraph();
  TGraph* exp_limit = new TGraph();
  TGraph* exp_limit_1s_up = new TGraph();
  TGraph* exp_limit_1s_dw = new TGraph();
  TGraph* exp_limit_2s_up = new TGraph();
  TGraph* exp_limit_2s_dw = new TGraph();

  TTreeReader reader(limit);
  TTreeReaderValue<double> limitVal (reader,"limit");
  TTreeReaderValue<float> quantile (reader,"quantileExpected");
  TTreeReaderValue<double> mass (reader,"mh");
  
  int ipoint_obs = 0;
  int ipoint_exp = 0;
  int ipoint_1sig_up = 0;
  int ipoint_1sig_dw = 0;
  int ipoint_2sig_up = 0;
  int ipoint_2sig_dw = 0;

  while(reader.Next()){

    if(*quantile == -1){
      obs_limit->SetPoint(ipoint_obs,*mass,*limitVal);
      ipoint_obs++;
    }
    
    else if(*quantile == 0.5){
      exp_limit->SetPoint(ipoint_exp,*mass,*limitVal);
      ipoint_exp++;
    }
    else if(*quantile > 0.15 and *quantile < 0.17){
      exp_limit_1s_dw->SetPoint(ipoint_1sig_dw,*mass,*limitVal);
      ipoint_1sig_dw++;
    }
    else if(*quantile > 0.83 and *quantile < 0.85){
      exp_limit_1s_up->SetPoint(ipoint_1sig_up,*mass,*limitVal);
      ipoint_1sig_up++;
    }
    else if(*quantile > 0.02 and *quantile < 0.03){
      exp_limit_2s_dw->SetPoint(ipoint_2sig_dw,*mass,*limitVal);
      ipoint_2sig_dw++;
    }
    else if(*quantile > 0.97 and *quantile < 0.98){
      exp_limit_2s_up->SetPoint(ipoint_2sig_up,*mass,*limitVal);
      ipoint_2sig_up++;
    }
  }

  TGraphAsymmErrors* sigma1_band = new TGraphAsymmErrors();
  TGraphAsymmErrors* sigma2_band = new TGraphAsymmErrors();

  for(int i = 0; i < exp_limit->GetN(); i++){
    double x,y;
    double x_dw,y_dw;
    double x_up,y_up;
    exp_limit->GetPoint(i,x,y);
    sigma1_band->SetPoint(i,x,y);
    sigma2_band->SetPoint(i,x,y);

    double x_next;
    exp_limit_1s_dw->GetPoint(i+1,x_next,y_dw);

    exp_limit_1s_dw->GetPoint(i,x_dw,y_dw);
    exp_limit_1s_up->GetPoint(i,x_up,y_up);
    sigma1_band->SetPointError(i,(x_next-x)/2,(x_next-x)/2,fabs(y-y_dw),fabs(y-y_up));

    exp_limit_2s_dw->GetPoint(i,x_dw,y_dw);
    exp_limit_2s_up->GetPoint(i,x_up,y_up);
    sigma2_band->SetPointError(i,(x_next-x)/2,(x_next-x)/2,fabs(y-y_dw),fabs(y-y_up));

  }

  TCanvas* canvas = new TCanvas("canvas","canvas",600,600);
  obs_limit->GetYaxis()->CenterTitle();
  obs_limit->GetXaxis()->SetTitle("m_{med} [GeV]");
  obs_limit->GetYaxis()->SetTitle("95%  CL upper limit on #sigma/#sigma_{theory}");
  obs_limit->GetXaxis()->SetTitleOffset(1.15);
  obs_limit->GetYaxis()->SetTitleOffset(1.07);  
  obs_limit->GetYaxis()->SetLabelSize(0.035);  
  obs_limit->SetLineColor(kBlack);
  obs_limit->SetLineWidth(2);
  exp_limit->SetLineColor(kBlack);
  exp_limit->SetLineWidth(2);
  exp_limit->SetLineStyle(7);

  obs_limit->GetYaxis()->SetRangeUser(0,1);
  
  obs_limit->Draw();

  CMS_lumi(canvas,"35.9",false,false);

  sigma2_band->SetFillColor(kOrange);
  sigma1_band->SetFillColor(kGreen+1);
  sigma2_band->SetLineColor(kOrange);
  sigma1_band->SetLineColor(kGreen+1);

  sigma2_band->Draw("3same");
  sigma1_band->Draw("3same");

  exp_limit->Draw("Lsame");
  obs_limit->Draw("Lsame");
  
  TLegend *leg = new TLegend(0.175,0.45,0.57,0.77);  

  leg->AddEntry(obs_limit,"Observed 95% CL","L");
  leg->AddEntry(exp_limit,"Median expected 95% CL","L");
  leg->AddEntry(sigma1_band,"68% expected","F");
  leg->AddEntry(sigma2_band,"95% expected","F");
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->Draw("SAME");
  
  TLatex * tex = new TLatex();
  tex->SetNDC();
  tex->SetTextFont(42);
  tex->SetLineWidth(2);
  tex->SetTextSize(0.030);
  tex->DrawLatex(0.175,0.80,"#bf{Vector med, Dirac DM, m_{DM} = 1 GeV g_{q} = 0.25, g_{DM} = 1}");
  
  gPad->RedrawAxis();
  gPad->Modified(); 
  gPad->Update();

  canvas->SaveAs((outputDIR+"/limit_1D.png").c_str(),"png");
  canvas->SaveAs((outputDIR+"/limit_1D.pdf").c_str(),"pdf");

}
