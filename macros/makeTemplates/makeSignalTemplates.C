#include "makeHistograms.h"

void makeSignalTemplates(Category category, // analysis category: monojet or monoV
			 string inputFileDIR, // directory where trees are places
			 string outputDIR, // outputDirectory where create file with the histograms,
			 string medMass, // dark matter mediator mass value
			 string dmMass, // dark matter mass value
			 float  luminosity = 35.9, // in fb
			 vector<string> observables = {} // list of observables for which generates the templates
			 ){
  

  // create output directory
  system(("mkdir -p "+outputDIR).c_str());
  
  // initialize binnins for observables
  initializeObservableBinning();

  // create TChains for each background process
  TChain* monojettree = new TChain("tree/tree"); 
  TChain* monoWtree = new TChain("tree/tree");  
  TChain* monoZtree = new TChain("tree/tree");  

  monojettree->Add((inputFileDIR+"/*MonoJ*"+medMass+"*"+dmMass+"*root").c_str());
  monoWtree->Add((inputFileDIR+"/*MonoW*"+medMass+"*"+dmMass+"*root").c_str());
  monoZtree->Add((inputFileDIR+"/*MonoZ*"+medMass+"*"+dmMass+"*root").c_str());
  
  // histograms for each process --> create them
  vector<TH1F*> monojethist;
  vector<TH1F*> monoWhist;
  vector<TH1F*> monoZhist;
  
  vector<double> binning;
  for(auto obs : observables){ // loop on the observables
    binning = selectBinning(obs,category);
    if(binning.size() == 0){
      cerr<<"Binning not found for observables "+obs+" --> skip "<<endl;
      continue;
    }
    monojethist.push_back(new TH1F(("monojethist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    monoWhist.push_back(new TH1F(("monoWhist_"+obs).c_str(),"",binning.size()-1,&binning[0]));
    monoZhist.push_back(new TH1F(("monoZhist_"+obs).c_str(),"",binning.size()-1,&binning[0]));

    monojethist.back()->Sumw2();
    monoWhist.back()->Sumw2();
    monoZhist.back()->Sumw2();
  }

  // make the histograms for the different samples
  cout<<"Run monoJ signal "<<endl;
  makeHistograms(monojethist,monojettree,Region::signal,category,luminosity);
  cout<<"Run monoW signal "<<endl;
  makeHistograms(monoWhist,monoWtree,Region::signal,category,luminosity);
  cout<<"Run monoZ signal "<<endl;
  makeHistograms(monoZhist,monoZtree,Region::signal,category,luminosity);

  cout<<"Save templates"<<endl;  
  string outputFileName;
  if(category == Category::monojet)
    outputFileName = "templates_monojet_DM_mMed_"+medMass+"_mDM"+dmMass+".root";
  else if(category == Category::monoV)
    outputFileName = "templates_monoV_DM_mMed_"+medMass+"_mDM"+dmMass+".root";
  
  // create the output file
  TFile* outputFile = new TFile((outputDIR+"/"+outputFileName).c_str(),"RECREATE");
  outputFile->cd();

  int iobs = 0;
  for(auto obs : observables){
    outputFile->mkdir(obs.c_str()); // make a directory with the observable name
    outputFile->cd(obs.c_str());
    monojethist.at(iobs)->Write();
    monoWhist.at(iobs)->Write();
    monoZhist.at(iobs)->Write();
    outputFile->cd();
    iobs++;
  }  
  outputFile->Close();
  
}
