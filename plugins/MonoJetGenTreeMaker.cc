#include "MonoJetGenTreeMaker.h"

MonoJetGenTreeMaker::MonoJetGenTreeMaker(const edm::ParameterSet& iConfig): 
  genEventInfo(iConfig.getParameter<edm::InputTag>("genEventInfo")),
  pileupEventInfo(iConfig.getParameter<edm::InputTag>("pileupEventInfo"))
{
  // Token consumes instructions
  genEventInfoToken = consumes<GenEventInfoProduct>(genEventInfo);
  pileupEventInfoToken = consumes<std::vector<PileupSummaryInfo> >(pileupEventInfo);

  // state that TFileService is used
  usesResource();
  usesResource("TFileService");
}


MonoJetGenTreeMaker::~MonoJetGenTreeMaker() {}

void MonoJetGenTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  
  using namespace edm;
  using namespace std;
  using namespace reco;

  Handle<GenEventInfoProduct> genEventInfoH;
  iEvent.getByToken(genEventInfoToken, genEventInfoH);
    
  Handle<std::vector<PileupSummaryInfo> > pileupEventInfoH;
  iEvent.getByToken(pileupEventInfoToken,pileupEventInfoH);

  // Event, lumi, run info
  event = iEvent.id().event();
  run   = iEvent.id().run();
  lumi  = iEvent.luminosityBlock();

  putrue = 0;
  if(pileupEventInfoH.isValid()) {
    for (auto pileupInfo_iter = pileupEventInfoH->begin(); pileupInfo_iter != pileupEventInfoH->end(); ++pileupInfo_iter) {
      if (pileupInfo_iter->getBunchCrossing() == 0) 
	putrue = pileupInfo_iter->getTrueNumInteractions();
    }
  }
  
  // dump the generator event weight
  wgt = 1.0;
  if(genEventInfoH.isValid())
    wgt = genEventInfoH->weight();
  
  // fill output tree
  tree->Fill();
  
}


// create output tree branches
void MonoJetGenTreeMaker::beginJob() {
  edm::Service<TFileService> fs;
  tree = fs->make<TTree>("gentree","gentree");
  // Run, Lumi, Event info
  tree->Branch("event"                , &event                , "event/i");
  tree->Branch("run"                  , &run                  , "run/i");
  tree->Branch("lumi"                 , &lumi                 , "lumi/i");
  // Event weights
  tree->Branch("wgt"                  , &wgt                  , "wgt/F");
  // pileup info
  tree->Branch("putrue"               , &putrue               , "putrue/I");
}

void MonoJetGenTreeMaker::endJob() {}
void MonoJetGenTreeMaker::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {}
void MonoJetGenTreeMaker::endRun(edm::Run const&, edm::EventSetup const&) {}

void MonoJetGenTreeMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(MonoJetGenTreeMaker);

