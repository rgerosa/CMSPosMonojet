#include "makeHistograms.h"

void makeBackgroundTemplates(Region region, // control region in which selects events
			     Category category, // analysis category: monojet or monoV
			     string inputFileDIR, // directory where trees are places
			     string outputDIR, // outputDirectory where create file with the histograms,
			     float  luminosity = 35.9, // in fb
			     vector<string> observables = {} // list of observables for which generates the templates
			     ){


  // create output directory
  system(("mkdir -p "+outputDIR).c_str());

  // initialize binnins for observables
  initializeObservableBinning();

  // create TChains for each background process
  TChain* zvvtree = new TChain("tree/tree");  // Zvv
  TChain* wjettree = new TChain("tree/tree"); //W+jets
  TChain* zlltree = new TChain("tree/tree");  // DY
  TChain* gammatree = new TChain("tree/tree"); // gamma+jets
  TChain* ttbartree = new TChain("tree/tree"); //ttbar
  TChain* vvtree = new TChain("tree/tree");   // diboson
  
  zvvtree->Add((inputFileDIR+"/*ZJets*root").c_str());
  wjettree->Add((inputFileDIR+"/*WJets*root").c_str());
  zlltree->Add((inputFileDIR+"/*DYJets*root").c_str());
  gammatree->Add((inputFileDIR+"/*GJets*root").c_str());
  ttbartree->Add((inputFileDIR+"/*TT*root").c_str());
  vvtree->Add((inputFileDIR+"/*WW*root").c_str());
  vvtree->Add((inputFileDIR+"/*WZ*root").c_str());
  vvtree->Add((inputFileDIR+"/*ZZ*root").c_str());
  
  // histograms for each process --> create them
  vector<TH1F*> zvvhist;
  vector<TH1F*> wjethist;
  vector<TH1F*> zllhist;
  vector<TH1F*> gammahist;
  vector<TH1F*> ttbarhist;
  vector<TH1F*> vvhist;
  vector<TH1F*> datahist;
  
  vector<double> binning;
  for(auto obs : observables){ // loop on the observables
    binning = selectBinning(obs,category);
    if(binning.size() == 0){
      cerr<<"Binning not found for observables "+obs+" --> skip "<<endl;
      continue;
    }
    zvvhist.push_back(new TH1F(("zvvhist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    wjethist.push_back(new TH1F(("wjethist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    zllhist.push_back(new TH1F(("zllhist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    gammahist.push_back(new TH1F(("gammahist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    ttbarhist.push_back(new TH1F(("ttbarhist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    vvhist.push_back(new TH1F(("vvhist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    datahist.push_back(new TH1F(("datahist_"+obs).c_str(),"",binning.size()-1,&binning[0]));

    zvvhist.back()->Sumw2();
    wjethist.back()->Sumw2();
    zllhist.back()->Sumw2();
    gammahist.back()->Sumw2();
    ttbarhist.back()->Sumw2();
    vvhist.back()->Sumw2();
    datahist.back()->Sumw2();
    
  }

  // make the histograms for the different samples
  makeHistograms(zvvhist,zvvtree,region,category,luminosity);
  makeHistograms(wjethist,wjettree,region,category,luminosity);
  makeHistograms(zllhist,zlltree,region,category,luminosity);
  makeHistograms(gammahist,gammatree,region,category,luminosity);
  makeHistograms(ttbarhist,ttbartree,region,category,luminosity);
  makeHistograms(vvhist,vvtree,region,category,luminosity);

  /// make pseudo data
  TRandom3 random;
  for(size_t ihist = 0; ihist < zvvhist.size(); ihist++){
    for(int iBin = 0; iBin < zvvhist.at(ihist)->GetNbinsX(); iBin++){
      double binval = 0;
      binval += zvvhist.at(ihist)->GetBinContent(iBin+1);
      binval += wjethist.at(ihist)->GetBinContent(iBin+1);
      binval += zllhist.at(ihist)->GetBinContent(iBin+1);
      binval += gammahist.at(ihist)->GetBinContent(iBin+1);
      binval += ttbarhist.at(ihist)->GetBinContent(iBin+1);
      binval += vvhist.at(ihist)->GetBinContent(iBin+1);      
      datahist.at(ihist)->SetBinContent(iBin+1,random.Poisson(binval));
    }    
  }
  

  /// create the output file name
  string outputFileName = "templates_";
  if(category == Category::monojet)
    outputFileName += "monojet_";
  else if(category == Category::monoV)
    outputFileName += "monoV_";

  if(region == Region::signal)
    outputFileName += "SR";
  else if(region == Region::gammajets)
    outputFileName += "gam";
  else if(region == Region::wmunu)
    outputFileName += "wmunu";
  else if(region == Region::wenu)
    outputFileName += "wenu";
  else if(region == Region::zmumu)
    outputFileName += "zmumu";
  else if(region == Region::zee)
    outputFileName += "zee";

  outputFileName += ".root";

  // create the output file
  TFile* outputFile = new TFile((outputDIR+"/"+outputFileName).c_str(),"RECREATE");
  outputFile->cd();

  int iobs = 0;
  for(auto obs : observables){
    outputFile->mkdir(obs.c_str()); // make a directory with the observable name
    outputFile->cd(obs.c_str());
    zvvhist.at(iobs)->Write();
    wjethist.at(iobs)->Write();
    zllhist.at(iobs)->Write();
    gammahist.at(iobs)->Write();
    ttbarhist.at(iobs)->Write();
    vvhist.at(iobs)->Write();
    outputFile->cd();
    iobs++;
  }  
  outputFile->Close();
  
}
