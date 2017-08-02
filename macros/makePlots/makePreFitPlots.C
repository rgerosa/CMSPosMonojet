#include "CMS_lumi.h"
#include "../makeTemplates/templateUtils.h"

void makePreFitPlots(string templateBackgroundFileName,  // input file name with background the templates
		     string templateSignalFileName, // input file name for signal templates
		     Region region, // to tell from which region the template file comes from
		     Category category, // to tell if plots are for monojet or monoV category
		     string observable, // observable to plot
		     string observableLatex, // for x-axis label
		     string outputDIR, // to store the plots
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
  TFile* inputFileBkg = new TFile(templateBackgroundFileName.c_str());
  TFile* inputFileSig = NULL;
  if(region == Region::signal)
    inputFileSig = new TFile(templateSignalFileName.c_str());

  TH1* datahist = (TH1*) inputFileBkg->Get((observable+"/datahist_"+observable).c_str());
  TH1* vvhist   = (TH1*) inputFileBkg->Get((observable+"/vvhist_"+observable).c_str());
  TH1* zvvhist  = (TH1*) inputFileBkg->Get((observable+"/zvvhist_"+observable).c_str());
  TH1* wjethist = (TH1*) inputFileBkg->Get((observable+"/wjethist_"+observable).c_str());
  TH1* zllhist  = (TH1*) inputFileBkg->Get((observable+"/zllhist_"+observable).c_str());
  TH1* ttbarhist = (TH1*) inputFileBkg->Get((observable+"/ttbarhist_"+observable).c_str());
  TH1* gammahist = (TH1*) inputFileBkg->Get((observable+"/gammahist_"+observable).c_str());

  TH1* monoJhist  = NULL;
  TH1* monoWhist  = NULL;
  TH1* monoZhist  = NULL;

  if(region == Region::signal){
    monoJhist = (TH1*) inputFileSig->Get((observable+"/monojethist_"+observable).c_str());
    monoWhist = (TH1*) inputFileSig->Get((observable+"/monoWhist_"+observable).c_str());
    monoZhist = (TH1*) inputFileSig->Get((observable+"/monoZhist_"+observable).c_str());
  }    

  // Set style  
  if(datahist){
    datahist->SetLineColor(kBlack);
    datahist->SetMarkerColor(kBlack);
    datahist->SetMarkerStyle(20);
    datahist->SetMarkerSize(1.2);
  }

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
  if(ttbarhist){
    ttbarhist->SetFillColor(TColor::GetColor("#CF3721"));
    ttbarhist->SetLineColor(kBlack);
  }
  if(vvhist){
    vvhist->SetFillColor(TColor::GetColor("#F1F1F2"));
    vvhist->SetLineColor(kBlack);
  }
  if(gammahist){ 
    gammahist->SetFillColor(TColor::GetColor("#ab60f7"));
    gammahist->SetLineColor(kBlack);
  }

  if(monoJhist){
    monoJhist->SetFillColor(0);
    monoJhist->SetFillStyle(0);
    monoJhist->SetLineColor(kBlack);
    monoJhist->SetLineWidth(3);
  }

  if(monoWhist){
    monoWhist->SetFillColor(0);
    monoWhist->SetFillStyle(0);
    monoWhist->SetLineColor(kBlack);
    monoWhist->SetLineWidth(2);
    monoWhist->SetLineStyle(7);
  }

  if(monoZhist){
    monoZhist->SetFillColor(0);
    monoZhist->SetFillStyle(0);
    monoZhist->SetLineColor(kBlack);
    monoZhist->SetLineWidth(2);
    monoZhist->SetLineStyle(7);
  }
  
  // add them together
  if(monoWhist and monoZhist)
    monoWhist->Add(monoZhist);


  // use a THStack
  THStack* stack = new THStack("stack", "stack");

  if(region == Region::gamma){
    stack->Add(ttbarhist);
    stack->Add(zllhist);
    stack->Add(vvhist);
    stack->Add(wjethist);
    stack->Add(gammahist);
  }
  else if(region == Region::zmm or region == Region::zee){
    stack->Add(wjethist);
    stack->Add(ttbarhist);
    stack->Add(vvhist);
    stack->Add(zllhist);
  }
  else if(region == Region::wmn or region == Region::wen){
    stack->Add(gammahist);
    stack->Add(zllhist);
    stack->Add(ttbarhist);
    stack->Add(vvhist);
    stack->Add(wjethist);
  }
  else if(region == Region::signal){
    stack->Add(gammahist);
    stack->Add(zllhist);
    stack->Add(ttbarhist);
    stack->Add(vvhist);
    stack->Add(wjethist);
    stack->Add(zvvhist);
  }

  /// start drawing
  canvas->cd();
  TH1* frame = (TH1*) datahist->Clone("frame");
  frame->Reset();  
  
  // Set Y-axis range
  if(category == Category::monojet and isLog)
    frame->GetYaxis()->SetRangeUser(max(1.5e-3,datahist->GetMinimum()*0.01),datahist->GetMaximum()*500);  
  else if(category == Category::monoV and isLog)
    frame->GetYaxis()->SetRangeUser(max(1.5e-3,datahist->GetMinimum()*0.01),datahist->GetMaximum()*500);  
  else
    frame->GetYaxis()->SetRangeUser(max(1.5e-3,datahist->GetMinimum()*0.5),datahist->GetMaximum()*2.5);  
    
  frame->GetXaxis()->SetTitle(observableLatex.c_str());
  frame->GetYaxis()->SetTitle("Events");

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
  stack ->Draw("HIST SAME");
  datahist->Draw("PE SAME");
  // canvas style
  CMS_lumi(canvas,"35.9");

  if(region == Region::signal){ // draw also signal lines
    monoJhist->Draw("hist same");
    monoWhist->Draw("hist same"); // is the sum of monoW and monoZ
  }

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
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(ttbarhist,"Top","F");
  }

  else if(region == Region::zmm or region == Region::zee){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(ttbarhist,"Top","F");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
  }

  else if(region == Region::wmn or region == Region::wen){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(ttbarhist,"Top","F");
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(gammahist,"#gamma+jets","F");
  }

  else if(region == Region::signal){
    leg->AddEntry(datahist, "Data","PLE");
    leg->AddEntry(zvvhist,  "Z #rightarrow #nu#nu ","F");
    leg->AddEntry(wjethist, "W #rightarrow l#nu","F");
    leg->AddEntry(vvhist,   "VV","F");
    leg->AddEntry(ttbarhist,"Top","F");
    leg->AddEntry(zllhist,  "Z/#gamma^{*} #rightarrow ll","F");
    leg->AddEntry(gammahist,"#gamma+jets","F");
    leg->AddEntry(monoJhist,"Mono-jet signal");
    leg->AddEntry(monoWhist,"Mono-V signal");
  }

  leg->Draw("SAME");
  
  canvas->RedrawAxis("sameaxis");
  if(isLog)
    canvas->SetLogy();

  // make data/MC ratio plot
  canvas->cd();
  pad_ratio->Draw();
  pad_ratio->cd();

  TH1* frame2 = (TH1*) datahist->Clone("frame");
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


  TH1* numerator_hist     = (TH1*) datahist->Clone("datahist_tot");
  TH1* uncertainty_hist   = (TH1*) datahist->Clone("unhist");
  TH1* denominator_dhist  = (TH1*) stack->GetStack()->At(stack->GetNhists()-1)->Clone("mchist_tot");
  TH1* denominator_hist_p = (TH1*) stack->GetStack()->At(stack->GetNhists()-1)->Clone("mchist_tot_p");

  numerator_hist->SetStats(kFALSE);
  numerator_hist->SetLineColor(kBlack);
  numerator_hist->SetMarkerColor(kBlack);
  numerator_hist->SetMarkerSize(0.8);

  // set to zero for plotting reasons of error bar and error band
  for (int i = 1; i <= denominator_dhist->GetNbinsX(); i++) denominator_dhist->SetBinError(i, 0);

  numerator_hist->Divide(denominator_dhist);
  denominator_hist_p->Divide(denominator_dhist);

  denominator_hist_p->SetLineColor(0);
  denominator_hist_p->SetMarkerColor(0);
  denominator_hist_p->SetMarkerSize(0);
  denominator_hist_p->SetFillColor(kGray);
  
  for (int i = 1; i <= uncertainty_hist->GetNbinsX(); i++) uncertainty_hist->SetBinContent(i, 1);
  for (int i = 1; i <= uncertainty_hist->GetNbinsX(); i++) uncertainty_hist->SetBinError(i, 0);
  uncertainty_hist->SetMarkerSize(0);
  uncertainty_hist->SetLineColor(kBlack);
  uncertainty_hist->SetLineStyle(2);
  uncertainty_hist->SetFillColor(0);
  
  numerator_hist->Draw("PE1 SAME");
  denominator_hist_p->Draw("E2 SAME");
  uncertainty_hist->Draw("SAME");
  numerator_hist->Draw("PE SAME");
  
  pad_ratio->RedrawAxis("sameaxis");


  // pull plot
  pad_pull->Draw();
  pad_pull->cd();

  TH1* frame3 = (TH1*) datahist->Clone("frame2");
  frame3->Reset();
  frame3->SetLineColor(kBlack);
  frame3->SetLineWidth(1);
  frame3->GetYaxis()->SetRangeUser(-3.5,3.5);
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
  frame3->Draw("AXIS");
  frame3->Draw("AXIG same");
    
  TH1F* data_pull_post = (TH1F*) datahist->Clone("data_pull_post");
  TH1F* total_bkg = (TH1F*) stack->GetStack()->At(stack->GetNhists()-1)->Clone("total_bkg");
  data_pull_post->Add(total_bkg,-1);
  data_pull_post->SetMarkerColor(TColor::GetColor("#0066ff"));
  data_pull_post->SetLineColor(TColor::GetColor("#0066ff"));
  data_pull_post->SetFillColor(TColor::GetColor("#0066ff"));
  data_pull_post->SetLineWidth(1);
  for(int iBin = 0; iBin < data_pull_post->GetNbinsX()+1; iBin++)
    data_pull_post->SetBinContent(iBin+1,data_pull_post->GetBinContent(iBin+1)/sqrt(pow(total_bkg->GetBinError(iBin+1),2)+pow(datahist->GetBinError(iBin+1),2)));

  // line at 1                                                                                                                                                                                      
  TH1* unhist2 = (TH1*) total_bkg->Clone("unhist");
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

  canvas->SaveAs((outputDIR+"/"+observable+postfix+".png").c_str());
  canvas->SaveAs((outputDIR+"/"+observable+postfix+".pdf").c_str());
}

