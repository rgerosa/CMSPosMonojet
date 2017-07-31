#ifndef MonoJetGenTreeMaker_H
#define MonoJetGenTreeMaker_H

// basic C++ headers                                                                                                                                                                                   
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>

// FWCore headers                                                                                                                                                                                   
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

// additional                                                                                                                                                                                    
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

// ROOT headers                                                                                                                                                                                 
#include "TTree.h"


class MonoJetGenTreeMaker : public edm::one::EDAnalyzer<edm::one::SharedResources,edm::one::WatchRuns> {
  
  public:
  explicit MonoJetGenTreeMaker(const edm::ParameterSet&);
  ~MonoJetGenTreeMaker();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:

  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;

  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void beginJob() override;
  virtual void endJob() override;

  // InputTags                                                                                                                                                                                         
  const edm::InputTag genEventInfo;
  const edm::InputTag pileupEventInfo;

  // Tokens                                                                                                                                                                                            
  edm::EDGetTokenT<GenEventInfoProduct> genEventInfoToken;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupEventInfoToken;
  
  // output tree
  TTree* tree;  
  uint32_t event, run, lumi;
  int      putrue;
  float    wgt;

};

#endif
