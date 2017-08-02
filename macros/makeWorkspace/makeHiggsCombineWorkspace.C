#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>
#include <sstream>

#include "TSystem.h"
#include "../makeTemplates/templateUtils.h"

// function to create a RooDataHist from TH1F and import it in a workspace                                                                                                                             
void addTemplate(string procname, // process name
		 const RooArgList& varlist, // list of observables
		 RooWorkspace& ws, // workspace
                 TH1F* hist // histogram
		 ) {

  if(hist == 0 || hist == NULL){
    cerr<<"addTemplate: found an null pointer --> check "<<endl;
    return;
  }
  RooDataHist rhist((procname).c_str(), "", varlist, hist);
  ws.import(rhist);
}

void makeBinList(string procname, // name of the process
		 RooRealVar& var, // observable for the fit
		 RooWorkspace& ws, // workspce where to store the RooParametricHist
		 TH1F* hist, // input histogram
		 RooArgList& binlist // list of bins
		 ){

  // loop over bins
  for (int i = 1; i <= hist->GetNbinsX(); i++) {
    stringstream binss;
    binss << procname << "_bin" << i;
    RooRealVar* binvar = new RooRealVar(binss.str().c_str(), "", hist->GetBinContent(i), 0., hist->GetBinContent(i)*10.0); // for each bin create a RooRealVar -> rate in the bin
    binlist.add(*binvar);
  }
  
  stringstream normss;
  normss << procname << "_norm";
  // build a RooParamtric hist using the bin list and the histogram                                                                                                                                   
  RooParametricHist phist(procname.c_str(), "", var, binlist, *hist);
  RooAddition norm(normss.str().c_str(), "", binlist); // total normalization to be stored in the workspace
  ws.import(phist,RooFit::RecycleConflictNodes());
  ws.import(norm, RooFit::RecycleConflictNodes());
}

void makeConnectedBinList(string procname,  // name to be used to fill the workspace                                                                                                                   
                          RooRealVar& var,  // observable used in the fit
                          RooWorkspace& ws, // workspce where to store the RooParametricHist                                                                                           
                          TH1F* rhist,      // Ratio between signal region and control region histograms --> transfer factor
                          const RooArgList& srbinlist  // Binning into the signal region                                                                                                      
                          ) {


  // loop on the transfer factor bin
  RooArgList* crbinlist = new RooArgList();
  float extreme_tmp = 5;
  for (int i = 1; i <= rhist->GetNbinsX(); i++) {

    // one rooreal var for each bin containing the ratio --> fixed --> only nuisance parameters can modify it
    stringstream rbinss;
    rbinss << "r_" << procname << "_bin" << i;

    // Fixed value for each bin of the ratio                                                                                                                                                           
    RooRealVar* rbinvar = new RooRealVar(rbinss.str().c_str(), "", rhist->GetBinContent(i));

    // Bin-By-Bin statistical uncertainty on the ratio --> nuisance parameters
    stringstream rerrbinss;
    rerrbinss << procname << "_bin" << i << "_Runc";
    float extreme = min(5.,0.9*rhist->GetBinContent(i)/rhist->GetBinError(i));
    RooRealVar* rerrbinvar = new RooRealVar(rerrbinss.str().c_str(), "", 0., -extreme, extreme);

    // list of all RooRealVar used by the RooFormulaVar to write the bin content                                                                                                            
    RooArgList fobinlist;
    // signal region bins                                                                                                                                                                              
    fobinlist.add(srbinlist[i-1]);
    // connection bins from rHisto (ratio histo)                                                                                                                                                      
    fobinlist.add(*rbinvar);
    // uncertainty -> stat uncertainty                                                                                                                                                            
    fobinlist.add(*rerrbinvar);
    // bin [i] (CR) = bin [i] (SR) /( Rbin [i] *(1+RhistError[i]/Rbin[i]*Rbin_Err[i] )) --> statstical uncertainty                                                                                   
    stringstream formss;
    formss << "@0/";
    formss << "(";
    formss << "@1";
    if(rhist->GetBinError(i)/rhist->GetBinContent(i) >= 0)
      formss << "*(TMath::Max(0,1+" << rhist->GetBinError(i)/rhist->GetBinContent(i) << "*@2))";
    else
      formss << "*(TMath::Max(0,1-" << fabs(rhist->GetBinError(i)/rhist->GetBinContent(i)) << "*@2))";
    formss << ")";

    //Create a RooFormula for the bin content
    stringstream binss;
    binss << procname << "_bin" << i ;        
    RooFormulaVar* binvar = new RooFormulaVar(binss.str().c_str(), "", formss.str().c_str(), RooArgList(fobinlist));
    crbinlist->add(*binvar);
  }

  stringstream normss;
  normss << procname << "_norm";

  // Make the parametric Histograms for the control regions \mu*SR[i]/(R(...))                                                                                                                         
  RooParametricHist phist(procname.c_str(), "", var, *crbinlist, *rhist);
  RooAddition norm(normss.str().c_str(),"", *crbinlist);
  ws.import(phist,RooFit::RecycleConflictNodes());
  ws.import(norm, RooFit::RecycleConflictNodes());

}



void makeHiggsCombineWorkspace(string inputDirectory, // path of the directory containing all the templates files
			       string inputSignalFileName_SR, // signal templates for a specific mass point
			       string observable, // observable to use for the fit
			       Category category // useful to pick-up the right binning
			       ){

  //open input files
  TFile* inputBackgroundFile_SR    = NULL;
  TFile* inputBackgroundFile_Zmm   = NULL;
  TFile* inputBackgroundFile_Zee   = NULL;
  TFile* inputBackgroundFile_Gamma = NULL;
  
  if(category == Category::monojet){
    inputBackgroundFile_SR  = TFile::Open((inputDirectory+"/templates_monojet_SR.root").c_str(),"READ");
    inputBackgroundFile_Zmm = TFile::Open((inputDirectory+"/templates_monojet_zmm.root").c_str(),"READ");
    inputBackgroundFile_Zee = TFile::Open((inputDirectory+"/templates_monojet_zee.root").c_str(),"READ");
    inputBackgroundFile_Gamma = TFile::Open((inputDirectory+"/templates_monojet_gam.root").c_str(),"READ");
  }
  else if(category == Category::monoV){
    inputBackgroundFile_SR  = TFile::Open((inputDirectory+"/templates_monoV_SR.root").c_str(),"READ");
    inputBackgroundFile_Zmm = TFile::Open((inputDirectory+"/templates_monoV_zmm.root").c_str(),"READ");
    inputBackgroundFile_Zee = TFile::Open((inputDirectory+"/templates_monoV_zee.root").c_str(),"READ");
    inputBackgroundFile_Gamma = TFile::Open((inputDirectory+"/templates_monoV_gam.root").c_str(),"READ");
  }
  
  TFile* inputSignalFile_SR = TFile::Open(inputSignalFileName_SR.c_str(),"READ");

  // basic loads                                                                                                                                                                                   
  gSystem->Load("libHiggsAnalysisCombinedLimit.so");
  RooMsgService::instance().setSilentMode(kTRUE);
  RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING) ;

  // to load all the variables information                                                                                                                                                             
  initializeObservableBinning();

  // for workspace naming                                                                                                                                                                  
  string suffix;
  if(category == Category::monojet)    suffix = "MJ";
  else if(category == Category::monoV) suffix = "MV";
 
  // create the output workspace                                                                                                                                                                    
  cout<<"Create output file ..."<<endl;
  string outputName = "workspace_"+suffix+".root";
  TFile *outfile = new TFile(outputName.c_str(),"RECREATE");
  outfile->cd();

  // Create the observables and binning RooFit objects
  cout<<"Load binning and observable ..."<<endl;
  vector<double> bins = selectBinning(observable,category);
  RooBinning *binning = new RooBinning(bins.size()-1,&bins[0],(observable+"_"+suffix+"_binning").c_str());
  RooRealVar *obs     = new RooRealVar((observable+"_"+suffix).c_str(),"",bins.front(),bins.back());
  obs->setBinning(*binning);
  RooArgList vars(*obs);
  
  // Signal region workspace
  cout<<"Make signal region workspace"<<endl;
  RooWorkspace wspace_SR(("SR_"+suffix).c_str(),(suffix+"_SR").c_str());
  
  TH1F* monoJ = (TH1F*) inputSignalFile_SR->Get((observable+"/monojethist_"+observable).c_str());
  TH1F* monoW = (TH1F*) inputSignalFile_SR->Get((observable+"/monoWhist_"+observable).c_str());
  TH1F* monoZ = (TH1F*) inputSignalFile_SR->Get((observable+"/monoZhist_"+observable).c_str());  
  addTemplate("MonoJ_SR_"+suffix,vars,wspace_SR,monoJ);
  addTemplate("MonoW_SR_"+suffix,vars,wspace_SR,monoJ);
  addTemplate("MonoZ_SR_"+suffix,vars,wspace_SR,monoJ);

  // signal region data
  TH1F* data_SR = (TH1F*) inputBackgroundFile_SR->Get((observable+"/datahist_"+observable).c_str());
  addTemplate("data_obs",vars,wspace_SR,data_SR);

  // signal region MC backgrounds
  TH1F* ttbar_SR = (TH1F*) inputBackgroundFile_SR->Get((observable+"/ttbarhist_"+observable).c_str());
  TH1F* vv_SR    = (TH1F*) inputBackgroundFile_SR->Get((observable+"/vvhist_"+observable).c_str());
  TH1F* gamma_SR = (TH1F*) inputBackgroundFile_SR->Get((observable+"/gammahist_"+observable).c_str());
  TH1F* zll_SR   = (TH1F*) inputBackgroundFile_SR->Get((observable+"/zllhist_"+observable).c_str());
  TH1F* wjet_SR  = (TH1F*) inputBackgroundFile_SR->Get((observable+"/wjethist_"+observable).c_str()); // W+jets from MC --> simplified version of the analysis
  
  addTemplate("Top_SR_"+suffix,vars,wspace_SR,ttbar_SR);
  addTemplate("Diboson_SR_"+suffix,vars,wspace_SR,vv_SR);
  addTemplate("Gamma_SR_"+suffix,vars,wspace_SR,gamma_SR);
  addTemplate("Zll_SR_"+suffix,vars,wspace_SR,zll_SR);
  addTemplate("WJets_SR_"+suffix,vars,wspace_SR,wjet_SR);

  // take the Znunu histogram for which we want to perform a simultaneous fit --> i.e. estimating it from Zmm, Zee and gamma+jets
  TH1F* zvv_SR = (TH1F*) inputBackgroundFile_SR->Get((observable+"/zvvhist_"+observable).c_str());
  RooArgList zvv_SR_bins; // vector which contains one RooRealVar per bin i.e. free parameters in the fit
  makeBinList("Zvv_SR_"+suffix,*obs,wspace_SR,zvv_SR,zvv_SR_bins);

  // Zmm region workspace
  cout<<"Make di-muon control region workspace"<<endl;
  RooWorkspace wspace_Zmm(("Zmm_"+suffix).c_str(),(suffix+"_Zmm").c_str());
  // zmm region data
  TH1F* data_Zmm = (TH1F*) inputBackgroundFile_Zmm->Get((observable+"/datahist_"+observable).c_str());
  addTemplate("data_obs",vars,wspace_Zmm,data_Zmm);

  // Zmm MC backgrounds
  TH1F* ttbar_Zmm = (TH1F*) inputBackgroundFile_Zmm->Get((observable+"/ttbarhist_"+observable).c_str());
  TH1F* vv_Zmm    = (TH1F*) inputBackgroundFile_Zmm->Get((observable+"/vvhist_"+observable).c_str());
  TH1F* wjet_Zmm  = (TH1F*) inputBackgroundFile_Zmm->Get((observable+"/wjethist_"+observable).c_str()); 
  
  addTemplate("Top_Zmm_"+suffix,vars,wspace_Zmm,ttbar_Zmm);
  addTemplate("Diboson_Zmm_"+suffix,vars,wspace_Zmm,vv_Zmm);
  addTemplate("WJets_Zmm_"+suffix,vars,wspace_Zmm,wjet_Zmm);

  //Connect the Zmm histogram with the Zvv in the signal region for the simultaneous fit --> no systematic uncertainties, only bin-by-bin statistical ones
  TH1F* Zll_Zmm = (TH1F*) inputBackgroundFile_Zmm->Get((observable+"/zllhist_"+observable).c_str());
  // transfer factor between SR and Zmm control region for Z+jets
  TH1F* transfer_Zvv_Zmm = (TH1F*) zvv_SR->Clone("transfer_Zvv_Zmm");
  transfer_Zvv_Zmm->Divide(Zll_Zmm);
  makeConnectedBinList("Zvv_Zmm_"+suffix,*obs,wspace_Zmm,transfer_Zvv_Zmm,zvv_SR_bins);

  // Zee region workspace
  cout<<"Make di-electron control region workspace"<<endl;
  RooWorkspace wspace_Zee(("Zee_"+suffix).c_str(),(suffix+"_Zee").c_str());
  // zee region data
  TH1F* data_Zee = (TH1F*) inputBackgroundFile_Zee->Get((observable+"/datahist_"+observable).c_str());
  addTemplate("data_obs",vars,wspace_Zee,data_Zee);

  // Zee MC backgrounds
  TH1F* ttbar_Zee = (TH1F*) inputBackgroundFile_Zee->Get((observable+"/ttbarhist_"+observable).c_str());
  TH1F* vv_Zee    = (TH1F*) inputBackgroundFile_Zee->Get((observable+"/vvhist_"+observable).c_str());
  TH1F* wjet_Zee  = (TH1F*) inputBackgroundFile_Zee->Get((observable+"/wjethist_"+observable).c_str()); 
  
  addTemplate("Top_Zee_"+suffix,vars,wspace_Zee,ttbar_Zee);
  addTemplate("Diboson_Zee_"+suffix,vars,wspace_Zee,vv_Zee);
  addTemplate("WJets_Zee_"+suffix,vars,wspace_Zee,wjet_Zee);

  //Connect the Zee histogram with the Zvv in the signal region for the simultaneous fit --> no systematic uncertainties, only bin-by-bin statistical ones
  TH1F* Zll_Zee = (TH1F*) inputBackgroundFile_Zee->Get((observable+"/zllhist_"+observable).c_str());
  // transfer factor between SR and Zee control region for Z+jets                                                                                                                                      
  TH1F* transfer_Zvv_Zee = (TH1F*) zvv_SR->Clone("transfer_Zvv_Zee");
  transfer_Zvv_Zee->Divide(Zll_Zee);
  makeConnectedBinList("Zvv_Zee_"+suffix,*obs,wspace_Zee,transfer_Zvv_Zee,zvv_SR_bins);

  // gamma region workspace
  cout<<"Make photon control region workspace"<<endl;
  RooWorkspace wspace_Gamma(("Gamma_"+suffix).c_str(),(suffix+"_Gamma").c_str());

  // gamma+jets region data
  TH1F* data_Gamma = (TH1F*) inputBackgroundFile_Gamma->Get((observable+"/datahist_"+observable).c_str());
  addTemplate("data_obs",vars,wspace_Gamma,data_Gamma);

  // Gamma MC backgrounds
  TH1F* ttbar_Gamma = (TH1F*) inputBackgroundFile_Gamma->Get((observable+"/ttbarhist_"+observable).c_str());
  TH1F* vv_Gamma    = (TH1F*) inputBackgroundFile_Gamma->Get((observable+"/vvhist_"+observable).c_str());
  TH1F* wjet_Gamma  = (TH1F*) inputBackgroundFile_Gamma->Get((observable+"/wjethist_"+observable).c_str()); 
  
  addTemplate("Top_Gamma_"+suffix,vars,wspace_Gamma,ttbar_Gamma);
  addTemplate("Diboson_Gamma_"+suffix,vars,wspace_Gamma,vv_Gamma);
  addTemplate("WJets_Gamma_"+suffix,vars,wspace_Gamma,wjet_Gamma);

  //Connect the gamma+jets histogram with the Zvv in the signal region for the simultaneous fit --> no systematic uncertainties, only bin-by-bin statistical ones --> simplified assumption
  TH1F* gamma_Gamma = (TH1F*) inputBackgroundFile_Gamma->Get((observable+"/gammahist_"+observable).c_str());
  // transfer factor between SR and Zee control region for Z+jets                                                                                                                                      
  TH1F* transfer_Zvv_Gamma = (TH1F*) zvv_SR->Clone("transfer_Zvv_Gamma");
  transfer_Zvv_Gamma->Divide(gamma_Gamma);
  makeConnectedBinList("Zvv_Gamma_"+suffix,*obs,wspace_Gamma,transfer_Zvv_Gamma,zvv_SR_bins);
  
  outfile->cd();
  wspace_SR.Write();
  wspace_Zmm.Write();
  wspace_Zee.Write();
  wspace_Gamma.Write();
  outfile->Close();

}
