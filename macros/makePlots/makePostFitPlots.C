#include "CMS_lumi.h"
#include "../makeTemplates/templateUtils.h"

void makePostFitPlots(string inputMLFitFile,  // input file is produced by the ML fit from combine
		      Region region, // to tell from which region you want to plot
		      Category category, // to tell if plots are for monojet or monoV category
		      string observableLatex, // for x-axis label
		      string outputDIR, // to store the plots
		      bool plotSBFitResult = false, // to plot B-only or S+B fit result
		      bool isLog = true // if use log scale in y or not
		       ) {
  
  system(("mkdir -p "+outputDIR).c_str());
  gROOT->SetBatch(kTRUE);
  setTDRStyle(); 
  gStyle->SetOptStat(0);
  
  TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 800);
  canvas->SetTickx(1);
  canvas->SetTicky(1);
  canvas->cd();
  canvas->SetBottomMargin(0.38);
  canvas->SetRightMargin(0.06);

  // ratio pad
  TPad* pad_ratio = new TPad("pad_ratio","pad_ratio",0,0.,1,1.);
  pad_ratio->SetTopMargin(0.63);
  pad_ratio->SetBottomMargin(0.25);
  pad_ratio->SetRightMargin(0.06);
  pad_ratio->SetFillColor(0);
  pad_ratio->SetFillStyle(0);
  pad_ratio->SetLineColor(0);

  // pull pad
  TPad* pad_pull = new TPad("pad_pull","pad_pull",0,0.,1,1.);
  pad_pull->SetTopMargin(0.76);
  pad_pull->SetRightMargin(0.06);
  pad_pull->SetFillColor(0);
  pad_pull->SetFillStyle(0);
  pad_pull->SetLineColor(0);

  
  // input file --> take the histograms
  TFile* inputFile = new TFile(inputMLFitFile.c_str(),"READ");

  string directory;
  if(region == Region::signal)
    directory = "ch1";
  else if(region == Region::zmm)
    directory = "ch2";
  if(region == Region::zee)
    directory = "ch3";
  if(region == Region::gamma)
    directory = "ch4";

  string fit_dir = "shapes_fit_b";
  if(plotSBFitResult)
    fit_dir = "shapes_fit_s";

  TGraphAsymmErrors* datahist =  (TGraphAsymmErrors*) inputFile->Get((fit_dir+"/"+directory+"/data").c_str());
  TH1* zvvhist  = NULL;
  TH1* wjethist = NULL;
  TH1* zllhist  = NULL;
  TH1* vvhist   = NULL;
  TH1* tophist  = NULL;
  TH1* gammahist  = NULL;
  TH1* signalhist =  NULL;

  if(region == Region::signal)
    zvvhist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Zvv").c_str());

  if(region == Region::signal)
    wjethist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/WJets").c_str()); 
  else if(region == Region::zmm)
    wjethist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/WJets_Zmm").c_str()); 
  else if(region == Region::zee)
    wjethist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/WJets_Zee").c_str()); 
  else if(region == Region::gamma)
    wjethist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/WJets_Gamma").c_str()); 
  
  if(region == Region::gamma){
    tophist  = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Top_Gamma").c_str()); 
    vvhist   = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Diboson_Gamma").c_str()); 
  }
  else{
    tophist  = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Top").c_str()); 
    vvhist   = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Diboson").c_str()); 
  }
  
  if(region == Region::signal)
    gammahist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Gamma").c_str()); 
  else if(region == Region::gamma)
    gammahist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Zvv").c_str()); 
  
  if(region == Region::signal)
    zllhist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Zll").c_str());
  else
    zllhist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/Zvv").c_str());

  if(region == Region::signal and plotSBFitResult)
    signalhist = (TH1*) inputFile->Get((fit_dir+"/"+directory+"/total_signal").c_str());

  TH1* total_background_postfit =  (TH1*) inputFile->Get((fit_dir+"/"+directory+"/total_background").c_str());
  TH1* total_background_prefit =  (TH1*) inputFile->Get(("shapes_prefit/"+directory+"/total_background").c_str());

  // Set style  
  if(zvvhist){
    zvvhist->SetFillColor(TColor::GetColor("#4D975D"));
    zvvhist->SetLineColor(kBlack);
  }
  if(zllhist){    
    zllhist->SetFillColor(TColor::GetColor("#4897D8"));
    zllhist->SetLineColor(kBlack);      
  }
  if(wjethist){
    wjethist->SetFillColor(TColor::GetColor("#FAAF08"));
    wjethist->SetLineColor(kBlack);
  }
  if(tophist){
    tophist->SetFillColor(TColor::GetColor("#CF3721"));
    tophist->SetLineColor(kBlack);
  }
  if(vvhist){
    vvhist->SetFillColor(TColor::GetColor("#F1F1F2"));
    vvhist->SetLineColor(kBlack);
  }
  if(gammahist){ 
    gammahist->SetFillColor(TColor::GetColor("#ab60f7"));
    gammahist->SetLineColor(kBlack);
  }

  if(signalhist){
    signalhist->SetFillColor(0);
    signalhist->SetFillStyle(0);
    signalhist->SetLineColor(kBlack);
    signalhist->SetLineWidth(3);
  }
  if(datahist){
    datahist->SetLineColor(kBlack);
    datahist->SetMarkerColor(kBlack);
    datahist->SetMarkerStyle(20);
    datahist->SetMarkerSize(1.2);
  }

  // use a THStack
  THStack* stack = new THStack("stack", "stack");

  if(region == Region::gamma){
    stack->Add(tophist);
    stack->Add(vvhist);
    stack->Add(wjethist);
    stack->Add(gammahist);
  }
  else if(region == Region::zmm or region == Region::zee){
    stack->Add(wjethist);
    stack->Add(tophist);
    stack->Add(vvhist);
    stack->Add(zllhist);
  }
  else if(region == Region::wmn or region == Region::wen){
    stack->Add(gammahist);
    stack->Add(zllhist);
    stack->Add(tophist);
    stack->Add(vvhist);
    stack->Add(wjethist);
  }
  else if(region == Region::signal){
    stack->Add(gammahist);
    stack->Add(zllhist);
    stack->Add(tophist);
    stack->Add(vvhist);
    stack->Add(wjethist);
    stack->Add(zvvhist);
  }

  /// start drawing
  canvas->cd(); 
  TH1* frame = (TH1*) total_background_postfit->Clone("frame");
  frame->Reset();  
  
  // Set Y-axis range
  if(category == Category::monojet and isLog)
    frame->GetYaxis()->SetRangeUser(max(1.5e-3,total_background_postfit->GetMinimum()*0.01),total_background_postfit->GetMaximum()*500);  
  else if(category == Category::monoV and isLog)
    frame->GetYaxis()->SetRangeUser(max(1.5e-3,total_background_postfit->GetMinimum()*0.01),total_background_postfit->GetMaximum()*500);  
  else
    frame->GetYaxis()->SetRangeUser(max(1.5e-3,total_background_postfit->GetMinimum()*0.5),total_background_postfit->GetMaximum()*2.5);  
    
  frame->GetXaxis()->SetTitle(observableLatex.c_str());
  frame->GetYaxis()->SetTitle("Events / GeV");

  frame->GetXaxis()->SetTitleSize(0);
  frame->GetXaxis()->SetLabelSize(0);
  frame->GetYaxis()->SetLabelSize(0.045);
  frame->GetYaxis()->SetTitleSize(0.055);
  frame->GetYaxis()->SetTitleOffset(1.15);
  frame->GetYaxis()->SetLabelSize(0.040);
  frame->GetYaxis()->SetTitleSize(0.050);

  if(category == Category::monojet)
    frame->GetXaxis()->SetNdivisions(510);
  else
    frame->GetXaxis()->SetNdivisions(504);

 // draw the frame
  frame->Draw();  
  stack->Draw("HIST SAME");  
  datahist->Draw("PE SAME");
  // canvas style
  CMS_lumi(canvas,"35.9");

  if(region == Region::signal and plotSBFitResult) // draw also signal lines
    signalhist->Draw("hist same");
  
  // legend for the upper pad
  TLegend* leg = new TLegend(0.40, 0.65, 0.92, 0.90);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetNColumns(2);

  if(region == Region::gamma){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(gammahist,"#gamma+jets","F");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(tophist,"Top","F");
  }

  else if(region == Region::zmm or region == Region::zee){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(tophist,  "Top","F");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
  }

  else if(region == Region::wmn or region == Region::wen){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(tophist,"Top","F");
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(gammahist,"#gamma+jets","F");
  }

  else if(region == Region::signal){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(zvvhist,  "Z #rightarrow #nu#nu ","F");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(tophist,"Top","F");
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(gammahist,"#gamma+jets","F");
    if(signalhist)
    leg->AddEntry(signalhist,"Fitted signal","L");      
  }

  leg->Draw("SAME");
  
  canvas->RedrawAxis("sameaxis");
  if(isLog)
    canvas->SetLogy();

  // make data/MC ratio plot
  canvas->cd();
  pad_ratio->Draw();
  pad_ratio->cd();

  TH1* frame2 = (TH1*) total_background_postfit->Clone("frame");
  frame2->Reset();
  if(category == Category::monojet)
    frame2->GetYaxis()->SetRangeUser(0.75,1.25);
  else if(category == Category::monoV)
    frame2->GetYaxis()->SetRangeUser(0.65,1.35);

  
  if(category == Category::monojet)
    frame2->GetXaxis()->SetNdivisions(510);
  else
    frame2->GetXaxis()->SetNdivisions(510);
  frame2->GetYaxis()->SetNdivisions(5);

  frame2->GetYaxis()->SetTitleOffset(1.9);
  frame2->GetYaxis()->SetLabelSize(0.03);
  frame2->GetXaxis()->SetTitleOffset(1.1);
  frame2->GetXaxis()->SetLabelSize(0);
  frame2->GetYaxis()->SetTitleSize(0.03);
  frame2->GetYaxis()->SetTitle("Data / Pred.");
  frame2->GetYaxis()->CenterTitle();
  frame2->GetXaxis()->SetTickLength(0.025);
  frame2->Draw();

  // for post-fit pre-fit data/mc
  TGraphAsymmErrors* dposthist = (TGraphAsymmErrors*) datahist->Clone("dposthist");
  TGraphAsymmErrors* dprehist = (TGraphAsymmErrors*) datahist->Clone("dprehist");

  dposthist->SetLineColor(TColor::GetColor("#0066ff"));
  dposthist->SetMarkerColor(TColor::GetColor("#0066ff"));

  dposthist->SetMarkerSize(1);
  dposthist->SetMarkerStyle(20);
  dprehist->SetMarkerSize(1);
  dprehist->SetMarkerStyle(24);
  dprehist->SetMarkerColor(kRed);
  dprehist->SetLineColor(kRed);

  TH1* mphist = (TH1*) total_background_postfit->Clone("mphist");
  TH1* mchist = (TH1*) total_background_prefit->Clone("mchist");
  TH1* unhist = (TH1*) total_background_postfit->Clone("unhist");
  unhist->Reset();

  for (int i = 1; i <= mchist->GetNbinsX(); i++) mchist->SetBinError(i, 0);
  for (int i = 1; i <= mphist->GetNbinsX(); i++) mphist->SetBinError(i, 0);

  for(int iPoint = 0; iPoint < dposthist->GetN(); iPoint++){
    double x,y;
    dposthist->GetPoint(iPoint,x,y);
    dposthist->SetPoint(iPoint,x,y/mphist->GetBinContent(iPoint+1));
    dposthist->SetPointError(iPoint,dposthist->GetErrorXlow(iPoint),dposthist->GetErrorXhigh(iPoint),
			     dposthist->GetErrorYlow(iPoint)/mphist->GetBinContent(iPoint+1),dposthist->GetErrorYhigh(iPoint)/mphist->GetBinContent(iPoint+1));
    dprehist->GetPoint(iPoint,x,y);
    dprehist->SetPoint(iPoint,x,y/mchist->GetBinContent(iPoint+1));
    dprehist->SetPointError(iPoint,dprehist->GetErrorXlow(iPoint),dprehist->GetErrorXhigh(iPoint),
			    dprehist->GetErrorYlow(iPoint)/mchist->GetBinContent(iPoint+1),dprehist->GetErrorYhigh(iPoint)/mchist->GetBinContent(iPoint+1));
  }

  // line at 1
  for (int i = 1; i <= unhist->GetNbinsX(); i++) unhist->SetBinContent(i, 1);
  for (int i = 1; i <= unhist->GetNbinsX(); i++) unhist->SetBinError(i, 0);
  unhist->SetMarkerSize(0);
  unhist->SetLineColor(kBlack);
  unhist->SetLineStyle(2);
  unhist->SetLineWidth(2);
  unhist->SetFillColor(0);
  
  TH1F* band = (TH1F*) total_background_postfit->Clone("band");
  band->Divide(mphist);
  band->SetLineColor(0);
  band->SetMarkerColor(0);
  band->SetMarkerSize(0);
  band->SetFillColor(kGray);

  dprehist->GetXaxis()->SetLabelOffset(999999);
  dprehist->GetXaxis()->SetLabelSize(0);
  dprehist->GetXaxis()->SetTitleOffset(999999);
  dprehist->GetXaxis()->SetTitleSize(0);
 
  band->Draw("E2 SAME");
  unhist->Draw("SAME");
  dprehist->Draw("P0E1 SAME");  
  dposthist->Draw("P0E1 SAME");
  pad_ratio->RedrawAxis("G sameaxis");

  canvas->cd();
  pad_ratio->RedrawAxis("sameaxis");
  canvas->RedrawAxis("sameaxis");

  pad_pull->Draw();
  pad_pull->cd();
  
  TH1* frame3 = (TH1*) total_background_postfit->Clone("frame2");
  frame3->Reset();
  frame3->SetLineColor(kBlack);
  frame3->SetLineWidth(1);
  frame3->GetYaxis()->SetRangeUser(-3,3);
  if(category == Category::monojet)
    frame3->GetXaxis()->SetNdivisions(510);
  else
    frame3->GetXaxis()->SetNdivisions(210);

  frame3->GetXaxis()->SetTitle(observableLatex.c_str());
  frame3->GetYaxis()->SetTitle("#frac{(Data-Pred.)}{#sigma}");

  frame3->GetYaxis()->CenterTitle();
  frame3->GetYaxis()->SetTitleOffset(1.5);
  frame3->GetYaxis()->SetLabelSize(0.03);
  frame3->GetYaxis()->SetTitleSize(0.03);
  frame3->GetXaxis()->SetLabelSize(0.04);
  frame3->GetXaxis()->SetTitleSize(0.05);
  frame3->GetYaxis()->SetNdivisions(504);
  frame3->GetXaxis()->SetTickLength(0.025);
  frame3->Draw("AXIS");
  frame3->Draw("AXIG same");

  TH1F* data_pull_post = (TH1F*) total_background_postfit->Clone("data_pull_post");
  data_pull_post->Reset();
  for(int iPoint = 0; iPoint < datahist->GetN(); iPoint++){
    double x,y;
    datahist->GetPoint(iPoint,x,y);
    data_pull_post->SetBinContent(iPoint+1,y);
    data_pull_post->SetBinError(iPoint+1,(datahist->GetErrorYlow(iPoint)+datahist->GetErrorYhigh(iPoint))/2);
  }
  data_pull_post->Add(mphist,-1);
  data_pull_post->SetMarkerColor(TColor::GetColor("#0066ff"));
  data_pull_post->SetLineColor(TColor::GetColor("#0066ff"));
  data_pull_post->SetFillColor(TColor::GetColor("#0066ff"));
  data_pull_post->SetLineWidth(1);
  for(int iBin = 0; iBin < data_pull_post->GetNbinsX()+1; iBin++){
      data_pull_post->SetBinContent(iBin+1,data_pull_post->GetBinContent(iBin+1)/sqrt(pow(band->GetBinError(iBin+1),2)+pow((datahist->GetErrorYlow(iBin)+datahist->GetErrorYhigh(iBin))/2,2)));
    data_pull_post->SetBinError(iBin+1,+1); // divide by sigma data  
  }
  
  // line at 1                                                                                                                                                                                      
  TH1* unhist2 = (TH1*) total_background_postfit->Clone("unhist");
  unhist2->Reset();
  for (int i = 1; i <= unhist2->GetNbinsX(); i++) unhist2->SetBinContent(i, 0);
  for (int i = 1; i <= unhist2->GetNbinsX(); i++) unhist2->SetBinError(i, 0);
  unhist2->SetMarkerSize(0);
  unhist2->SetLineColor(kBlack);
  unhist2->SetLineStyle(2);
  unhist2->SetLineWidth(2);
  unhist2->SetFillColor(0);
  unhist2->Draw("SAME");
  data_pull_post->Draw("hist same");
  pad_pull->RedrawAxis("G sameaxis");
  pad_pull->Modified();

  string postfix = ""; 
  if(region == Region::signal)
    postfix += "_SR";
  else if(region == Region::zmm)
    postfix += "_zmm";
  else if(region == Region::wmn)
    postfix += "_wmn";
  else if(region == Region::wen)
    postfix += "_wen";
  else if(region == Region::zee)
    postfix += "_zee";
  else if(region == Region::gamma)
    postfix += "_gamma";

  if(category == Category::monojet)
    postfix += "_MJ";
  else if(category == Category::monoV)
    postfix += "_MV";

  if(not plotSBFitResult)
    postfix += "_bonly";
  else
    postfix += "_sb";
    
  canvas->SaveAs((outputDIR+"/postfit"+postfix+".png").c_str());
  canvas->SaveAs((outputDIR+"/postfit"+postfix+".pdf").c_str());

  if(region == Region::signal and plotSBFitResult){ // make data - background plot

    TH1F* dataSubtracted = (TH1F*) total_background_postfit->Clone("dataSubtracted");
    TH1F* uncertainty    = (TH1F*) total_background_postfit->Clone("uncertainty");
    double error_max;
    for(int iBin = 0; iBin < dataSubtracted->GetNbinsX(); iBin++){
      double x,y;
      datahist->GetPoint(iBin,x,y);
      dataSubtracted->SetBinContent(iBin+1,y-total_background_postfit->GetBinContent(iBin+1));
      dataSubtracted->SetBinError(iBin+1,datahist->GetErrorYlow(iBin)+datahist->GetErrorYhigh(iBin)/2);
      uncertainty->SetBinContent(iBin+1,0);
      if(uncertainty->GetBinError(iBin+1) > error_max) error_max = uncertainty->GetBinError(iBin+1);
    }
    
    dataSubtracted->SetLineColor(kBlack);
    dataSubtracted->SetMarkerColor(kBlack);
    dataSubtracted->SetMarkerStyle(20);
    dataSubtracted->SetMarkerSize(1.2);
    
    TCanvas* canvas2 = new TCanvas("canvas2", "canvas2", 600, 600);
    canvas2->SetTickx(1);
    canvas2->SetTicky(1);
    canvas2->cd();
    canvas2->SetRightMargin(0.06);

    TH1* frame_4 = (TH1*) total_background_postfit->Clone("frame");
    frame_4->Reset();  
    frame_4->GetXaxis()->SetTitle(observableLatex.c_str());
    frame_4->GetYaxis()->SetTitle("Events / GeV");    
    frame_4->GetYaxis()->SetLabelSize(0.045);
    frame_4->GetYaxis()->SetTitleSize(0.055);
    frame_4->GetYaxis()->SetTitleOffset(1.15);
    frame_4->GetYaxis()->SetLabelSize(0.040);
    frame_4->GetYaxis()->SetTitleSize(0.050);

    frame_4->GetXaxis()->SetLabelSize(0.045);
    frame_4->GetXaxis()->SetTitleSize(0.055);
    frame_4->GetXaxis()->SetTitleOffset(1.15);
    frame_4->GetXaxis()->SetLabelSize(0.040);
    frame_4->GetXaxis()->SetTitleSize(0.050);

    if(category == Category::monojet)
      frame->GetXaxis()->SetNdivisions(510);
    else
      frame->GetXaxis()->SetNdivisions(504);

    frame_4->GetYaxis()->SetRangeUser(min(-error_max,dataSubtracted->GetMinimum())*1.5,max(error_max,dataSubtracted->GetMaximum())*1.5);    
    frame_4->Draw();

    dataSubtracted->Draw("EPsame");      
    uncertainty->SetLineColor(0);
    uncertainty->SetMarkerColor(0);
    uncertainty->SetMarkerSize(0);
    uncertainty->SetFillColor(kGray);
    uncertainty->SetLineColor(kBlack);
    uncertainty->Draw("E2Lsame");
    signalhist->SetLineColor(kRed);
    signalhist->Draw("hist same");
    dataSubtracted->Draw("EPsame");

    TLegend* leg2 = new TLegend(0.6,0.6,0.9,0.9);
    leg2->SetBorderSize(0);
    leg2->SetFillColor(0);
    leg2->SetFillStyle(0);
    leg2->AddEntry(dataSubtracted,"Data - Post-fit Bkg","PE");
    leg2->AddEntry(uncertainty,"Background uncertainty","F");
    leg2->AddEntry(signalhist,"Fitted signal","L");
    leg2->Draw("same");
    
    CMS_lumi(canvas,"35.9");
    canvas2->cd();
    canvas2->RedrawAxis("sameaxis");

    canvas2->SaveAs((outputDIR+"/postfit"+postfix+"_fitted_signal.png").c_str());
    canvas2->SaveAs((outputDIR+"/postfit"+postfix+"_fitted_signal.pdf").c_str());
  }
      
}

