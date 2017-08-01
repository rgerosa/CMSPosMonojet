
void makeSumOfWeightBranch(string inputFileName, string outputDIR, string outputFileName){

  system(("mkdir -p "+outputDIR).c_str());

  // read information from the inputfile
  TFile* inputFile = TFile::Open(inputFileName.c_str(),"READ");
  TTree* tree    = (TTree*) inputFile->Get("tree/tree");
  TTree* gentree = (TTree*) inputFile->Get("gentree/gentree");

  // calculate the sum of weights looping on the gentree
  double wgtsum = 0;
  TTreeReader treeReader(gentree);
  TTreeReaderValue<float> wgt (treeReader,"wgt");
  long int nEvents   = 0.;
  
  cout<<"#### Calculate sum of weights"<<endl;
  while(treeReader.Next()){
    if(int(nEvents) %100000 == 0){
      std::cout.flush();
      std::cout<<"\r"<<"Events "<<nEvents/gentree->GetEntries()*100<<"%";
    }
    wgtsum += (*wgt);
    nEvents++;
  }
  cout<<endl;

  // create the outputfile
  TFile* outputFile = new TFile((outputDIR+"/"+outputFileName).c_str(), "RECREATE");
  outputFile->cd();
  // create a directory
  TDirectoryFile* treedir = new TDirectoryFile("tree", "tree");
  treedir->cd();
  
  cout<<"#### Clone the input tree "<<endl;
  TTree* outputTree = (TTree*) tree->CopyTree(""); // clone the tree --> all events and all branches
  
  TBranch* bwgtsum = outputTree->Branch("wgtsum",&wgtsum,"wgtsum/D"); // create a new branch
  TTreeReader reader(outputTree);
  cout<<"#### Fill sum of weight branch"<<endl;
  nEvents=0;

  while(reader.Next()){
    std::cout.flush();
     if(nEvents %100000 == 0)
       std::cout<<"\r"<<"Event: "<<100*float(nEvents)/outputTree->GetEntries()<<" %";
     bwgtsum->Fill();
     nEvents++;
  }
  cout<<endl;
  treedir->cd();
  outputTree->Write();
  outputFile->Close();
   
}

