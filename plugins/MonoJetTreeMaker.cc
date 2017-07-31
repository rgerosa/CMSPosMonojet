#include "MonoJetTreeMaker.h"

MonoJetTreeMaker::MonoJetTreeMaker(const edm::ParameterSet& iConfig): 
  pileupEventInfoToken(consumes<std::vector<PileupSummaryInfo> > (iConfig.getParameter<edm::InputTag>("pileupEventInfo"))),
  genEventInfoToken(consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>("genEventInfo"))),
  genParticlesToken(consumes<edm::View<reco::GenParticle> > (iConfig.getParameter<edm::InputTag>("genParticles"))),
  xsec  (iConfig.existsAs<double>("xsec") ? iConfig.getParameter<double>("xsec") * 1000.0 : -1000.),
  triggerResultsTag  (iConfig.getParameter<edm::InputTag>("triggerResults")),
  verticesTag (iConfig.getParameter<edm::InputTag>("vertices")),
  looseMuonsTag  (iConfig.getParameter<edm::InputTag>("looseMuons")),
  tightMuonsTag  (iConfig.getParameter<edm::InputTag>("tightMuons")),
  vetoElectronsTag (iConfig.getParameter<edm::InputTag>("vetoElectrons")),
  tightElectronsTag (iConfig.getParameter<edm::InputTag>("tightElectrons")),
  loosePhotonsTag   (iConfig.getParameter<edm::InputTag>("loosePhotons")),
  mediumPhotonsTag  (iConfig.getParameter<edm::InputTag>("mediumPhotons")),
  looseTausTag(iConfig.getParameter<edm::InputTag>("looseTaus")),
  jetsAK4Tag(iConfig.getParameter<edm::InputTag>("jetsAK4")),
  vJetsAK8Tag(iConfig.getParameter<edm::InputTag>("vJetsAK8")),
  isPuppiVJet(iConfig.getParameter<bool>("isPuppiVJet")),
  t1PFMETTag(iConfig.getParameter<edm::InputTag>("t1PFMET")),
  t1PFMETNoMuonTag(iConfig.getParameter<edm::InputTag>("t1PFMETNoMuon")),
  t1PFMETNoElectronTag(iConfig.getParameter<edm::InputTag>("t1PFMETNoElectron")),
  t1PFMETNoPhotonTag(iConfig.getParameter<edm::InputTag>("t1PFMETNoPhoton")),
  cleanMuonJet(iConfig.existsAs<bool>("cleanMuonJet") ? iConfig.getParameter<bool>("cleanMuonJet") : true),
  cleanElectronJet(iConfig.existsAs<bool>("cleanElectronJet") ? iConfig.getParameter<bool>("cleanElectronJet") : true),
  cleanPhotonJet(iConfig.existsAs<bool>("cleanPhotonJet") ? iConfig.getParameter<bool>("cleanPhotonJet") : true),
  dRCleaningAK4(iConfig.existsAs<double>("dRCleaningAK4") ? iConfig.getParameter<double>("dRCleaningAK4") : 0.4),
  dRCleaningAK8(iConfig.existsAs<double>("dRCleaningAK8") ? iConfig.getParameter<double>("dRCleaningAK8") : 0.8),
  btaggingCSVWP(iConfig.getParameter<double>("btaggingCSVWP")),
  minJetPtCountAK4(iConfig.existsAs<double>("minJetPtAK4") ? iConfig.getParameter<double>("minJetPtAK4") : 30),
  minJetPtBveto(iConfig.existsAs<double>("minJetPtBveto") ? iConfig.getParameter<double>("minJetPtBveto") : 20){
  
  usesResource();
  usesResource("TFileService");

  // trigger tokens
  triggerResultsToken   = consumes<edm::TriggerResults> (triggerResultsTag);

  // vertex
  verticesToken  = consumes<std::vector<reco::Vertex> > (verticesTag);

  //muons
  looseMuonsToken  = consumes<pat::MuonRefVector> (looseMuonsTag);
  tightMuonsToken  = consumes<pat::MuonRefVector> (tightMuonsTag);

  // electrons
  vetoElectronsToken   = consumes<pat::ElectronRefVector> (vetoElectronsTag);
  tightElectronsToken  = consumes<pat::ElectronRefVector> (tightElectronsTag);

  // photons
  loosePhotonsToken   = consumes<pat::PhotonRefVector> (loosePhotonsTag);
  mediumPhotonsToken  = consumes<pat::PhotonRefVector> (mediumPhotonsTag);

  // taus
  looseTausToken = consumes<pat::TauRefVector> (looseTausTag);
  
  // jets AK4
  jetsAK4Token = consumes<std::vector<pat::Jet> > (jetsAK4Tag);

  // jets AK8
  vJetsAK8Token = consumes<pat::JetRefVector> (vJetsAK8Tag);
  
  // MET
  t1PFMETToken    = consumes<edm::View<pat::MET> > (t1PFMETTag);
  t1PFMETNoMuonToken    = consumes<edm::View<pat::MET> > (t1PFMETNoMuonTag);
  t1PFMETNoElectronToken  = consumes<edm::View<pat::MET> > (t1PFMETNoElectronTag);
  t1PFMETNoPhotonToken    = consumes<edm::View<pat::MET> > (t1PFMETNoPhotonTag);  
}


MonoJetTreeMaker::~MonoJetTreeMaker() {}

void MonoJetTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;
  using namespace reco;
  using namespace std;
  using namespace pat;
  

  // Get handles to all the requisite collections
  Handle<TriggerResults> triggerResultsH;
  iEvent.getByToken(triggerResultsToken, triggerResultsH);
  
  // GEN INFO    
  Handle<vector<PileupSummaryInfo> > pileupEventInfoH;
  Handle<GenEventInfoProduct>        genEventInfoH;
  Handle<View<GenParticle> >         genParticlesH;
  
  iEvent.getByToken(pileupEventInfoToken, pileupEventInfoH);
  iEvent.getByToken(genEventInfoToken, genEventInfoH);
  iEvent.getByToken(genParticlesToken, genParticlesH);
  
  // VERTEX
  Handle<vector<Vertex> > verticesH;
  iEvent.getByToken(verticesToken, verticesH);

  // MUONS
  Handle<pat::MuonRefVector> looseMuonsH;
  iEvent.getByToken(looseMuonsToken, looseMuonsH);
  pat::MuonRefVector looseMuons = *looseMuonsH;

  Handle<pat::MuonRefVector> tightMuonsH;
  iEvent.getByToken(tightMuonsToken, tightMuonsH);
  pat::MuonRefVector tightMuons = *tightMuonsH;
  
  // ELECTRONS
  Handle<pat::ElectronRefVector> vetoElectronsH;
  iEvent.getByToken(vetoElectronsToken, vetoElectronsH);
  pat::ElectronRefVector vetoElectrons = *vetoElectronsH;

  Handle<pat::ElectronRefVector> tightElectronsH;
  iEvent.getByToken(tightElectronsToken, tightElectronsH);
  pat::ElectronRefVector tightElectrons = *tightElectronsH;

  // PHOTONS
  Handle<pat::PhotonRefVector> loosePhotonsH;
  iEvent.getByToken(loosePhotonsToken, loosePhotonsH);
  pat::PhotonRefVector loosePhotons = *loosePhotonsH;
  
  Handle<pat::PhotonRefVector> mediumPhotonsH;
  iEvent.getByToken(mediumPhotonsToken, mediumPhotonsH);
  pat::PhotonRefVector mediumPhotons = *mediumPhotonsH;
   
  // TAUS
  Handle<pat::TauRefVector> looseTausH;
  iEvent.getByToken(looseTausToken, looseTausH);
  pat::TauRefVector looseTaus = *looseTausH;
  
  // AK4 Jets
  Handle<vector<pat::Jet> > jetsAK4H;
  iEvent.getByToken(jetsAK4Token, jetsAK4H);
  pat::JetCollection jetsAK4 = *jetsAK4H;

  // AK8 V-Jets
  Handle<pat::JetRefVector> vJetsAK8H;
  iEvent.getByToken(vJetsAK8Token, vJetsAK8H);
  pat::JetRefVector vJetsAK8 = *vJetsAK8H;

  // MET
  Handle<View<pat::MET> > t1PFMETH;
  iEvent.getByToken(t1PFMETToken, t1PFMETH);
  Handle<View<pat::MET> > t1PFMETNoMuonH;
  iEvent.getByToken(t1PFMETNoMuonToken, t1PFMETNoMuonH);
  Handle<View<pat::MET> > t1PFMETNoElectronH;
  iEvent.getByToken(t1PFMETNoElectronToken, t1PFMETNoElectronH);
  Handle<View<pat::MET> > t1PFMETNoPhotonH;
  iEvent.getByToken(t1PFMETNoPhotonToken, t1PFMETNoPhotonH);


  // Event, lumi, run info
  event = iEvent.id().event();
  run   = iEvent.id().run();
  lumi  = iEvent.luminosityBlock();
  
  // Trigger info
  passingHLTPFMet = 0; passingHLTPFMetNoMu = 0; passingHLTPFMetMonojet = 0;
  passingHLTPhoton = 0;
  passingHLTElectron = 0;
  if(triggerResultsH.isValid()){    
    // lopp on the trigger path vector
    for (size_t i = 0; i < triggerPathsVector.size(); i++) {
      if (triggerPathsMap[triggerPathsVector[i]] == -1) continue;  
      TString name (triggerPathsVector[i]);
      if (name.Contains("HLT_PFMETNoMu") and triggerResultsH->accept(triggerPathsMap[triggerPathsVector[i]])) passingHLTPFMetNoMu += 1;      
      if (name.Contains("HLT_PFMET") and  not name.Contains("HLT_PFMETNoMu") and triggerResultsH->accept(triggerPathsMap[triggerPathsVector[i]])) passingHLTPFMet += 1;
      if (name.Contains("HLT_MonoCentralPF") and triggerResultsH->accept(triggerPathsMap[triggerPathsVector[i]])) passingHLTPFMetMonojet += 1;
      if (name.Contains("HLT_Photon") and triggerResultsH->accept(triggerPathsMap[triggerPathsVector[i]])) passingHLTPhoton += 1;
      if (name.Contains("HLT_Ele") and triggerResultsH->accept(triggerPathsMap[triggerPathsVector[i]])) passingHLTElectron += 1;
    }
  }

  if(passingHLTPFMetNoMu != 0) passingHLTPFMetNoMu = 1;
  if(passingHLTPFMet != 0) passingHLTPFMet = 1;
  if(passingHLTPFMetMonojet != 0) passingHLTPFMetMonojet = 1;
  if(passingHLTPhoton) passingHLTPhoton = 1;
  if(passingHLTElectron) passingHLTElectron = 1;


  // Pileup info 
  nvtx = 0;
  if(verticesH.isValid()) 
    nvtx = verticesH->size();
  
  putrue = 0;
  if (pileupEventInfoH.isValid()) {
    for (auto pileupInfo_iter = pileupEventInfoH->begin(); pileupInfo_iter != pileupEventInfoH->end(); ++pileupInfo_iter) {
      if (pileupInfo_iter->getBunchCrossing() == 0) {
	putrue = pileupInfo_iter->getTrueNumInteractions();
      }
    }
  }
  
  // Weight info
  wgt = 1.0;
  if (genEventInfoH.isValid())
    wgt = genEventInfoH->weight();    

  // MET information 
  t1met = -99. ; t1metphi = -99. ;
  t1metnomu  = -99. ; t1metnomuphi = -99. ;
  t1metnoele = -99. ; t1metnoelephi = -99. ;
  t1metnopho = -99. ; t1metnophophi = -99. ;
    
  if(t1PFMETH.isValid()){      
    t1met    = t1PFMETH->front().corPt();    
    t1metphi = t1PFMETH->front().corPhi();
  }

  if(t1PFMETNoMuonH.isValid()){      
    t1metnomu    = t1PFMETNoMuonH->front().corPt();    
    t1metnomuphi = t1PFMETNoMuonH->front().corPhi();
  }

  if(t1PFMETNoElectronH.isValid()){      
    t1metnoele    = t1PFMETNoElectronH->front().corPt();    
    t1metnoelephi = t1PFMETNoElectronH->front().corPhi();
  }

  if(t1PFMETNoPhotonH.isValid()){      
    t1metnopho    = t1PFMETNoPhotonH->front().corPt();    
    t1metnophophi = t1PFMETNoPhotonH->front().corPhi();
  }
  

  // AK4 Jet information
  vector<pat::JetRef> jets;
  if(jetsAK4H.isValid()){                                                                                                                                                                 
    for (auto jets_iter = jetsAK4.begin(); jets_iter != jetsAK4.end(); ++jets_iter) {                                                                                                         
      //clean from leptons                                                                                                                                                                       
      bool skipjet = false;                                                                                                                                                                      
      for (std::size_t j = 0; j < looseMuons.size(); j++) {                                                                                                                                    
        if (cleanMuonJet && deltaR(looseMuons[j]->eta(), looseMuons[j]->phi(), jets_iter->eta(), jets_iter->phi()) < dRCleaningAK4)                                                 
          skipjet = true;                                                                                                                                                                  
      }                                                                                                                                                                                  
      for (std::size_t j = 0; j < vetoElectrons.size(); j++) {                                                                                                                        
        if (cleanElectronJet && deltaR(vetoElectrons[j]->eta(), vetoElectrons[j]->phi(), jets_iter->eta(), jets_iter->phi()) < dRCleaningAK4)                                           	 
          skipjet = true;                                                                                                                                                                      
      }                                                                                                                                                                                           
      for (std::size_t j = 0; j < loosePhotons.size(); j++) {                                                                                                                          
        if (cleanPhotonJet && deltaR(loosePhotons[j]->eta(), loosePhotons[j]->phi(), jets_iter->eta(), jets_iter->phi()) < dRCleaningAK4)                                                      
          skipjet = true;                                                                                                                                                                       
      }                                                                                                                                                                                             

      // jet in overlap with lepton                                                                                                                                                                 
      if (skipjet) continue;                                                                                                                                                                       
      pat::JetRef jetref(jetsAK4H, jets_iter - jetsAK4H->begin());
      if(not jetref.isAvailable()) continue;
      if(not jetref.isNonnull()) continue;
      if(fabs(jetref->eta()) > 4.7) continue;
      if(jetref->pt() < minJetPtCountAK4) continue;
      jets.push_back(jetref);
    }
  }
  
  // fill jet informations
  njets = 0;  nbjets = 0;
  jetpt.clear(); jeteta.clear(); jetphi.clear(); jetm.clear(); jetbtag.clear(); jetchfrac.clear(); jetnhfrac.clear();
  
  for(size_t i = 0; i < jets.size(); i++){
    if(fabs(jets[i]->eta()) < 2.5) njets++;
    if(jets[i]->pt() > minJetPtBveto and fabs(jets[i]->eta()) < 2.4 and jets[i]->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > btaggingCSVWP) nbjets++;
    jetpt.push_back(jets[i]->pt());
    jeteta.push_back(jets[i]->eta());
    jetphi.push_back(jets[i]->phi());
    jetm.push_back(jets[i]->mass());
    jetbtag.push_back(jets[i]->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    jetchfrac.push_back(jets[i]->chargedHadronEnergyFraction());
    jetnhfrac.push_back(jets[i]->neutralHadronEnergyFraction());

  }

  // Jet met dphi
  mindphijetmet = 0; mindphijetmetnomu = 0; mindphijetmetnoele = 0; mindphijetmetnopho = 0;
  
  std::vector<float> mindphijetmet_vec;
  std::vector<float> mindphijetmetnomu_vec;
  std::vector<float> mindphijetmetnoele_vec;
  std::vector<float> mindphijetmetnopho_vec;

  for(size_t i = 0; i < jets.size(); i++){
    mindphijetmet_vec.push_back(fabs(deltaPhi(jets[i]->phi(),t1metphi)));
    mindphijetmetnomu_vec.push_back(fabs(deltaPhi(jets[i]->phi(),t1metphi)));
    mindphijetmetnoele_vec.push_back(fabs(deltaPhi(jets[i]->phi(),t1metnoelephi)));
    mindphijetmetnopho_vec.push_back(fabs(deltaPhi(jets[i]->phi(),t1metnophophi)));
  }
  if(mindphijetmet_vec.size() > 0) mindphijetmet = *min_element(mindphijetmet_vec.begin(),mindphijetmet_vec.end());
  if(mindphijetmetnomu_vec.size() > 0) mindphijetmetnomu = *min_element(mindphijetmetnomu_vec.begin(),mindphijetmetnomu_vec.end());
  if(mindphijetmetnoele_vec.size() > 0) mindphijetmetnoele = *min_element(mindphijetmetnoele_vec.begin(),mindphijetmetnoele_vec.end());
  if(mindphijetmetnopho_vec.size() > 0) mindphijetmetnopho = *min_element(mindphijetmetnopho_vec.begin(),mindphijetmetnopho_vec.end());
  

  // taus
  ntaus = 0;  
  if(looseTausH.isValid()){
    for(std::size_t itau =0 ; itau < looseTaus.size(); itau++){
      bool skiptau = false;
      for (std::size_t j = 0; j < looseMuons.size(); j++) {
	if (cleanMuonJet && deltaR(looseMuons[j]->eta(),looseMuons[j]->phi(),looseTaus[itau]->eta(),looseTaus[itau]->phi()) < dRCleaningAK4) skiptau = true;
      }
      for (std::size_t j = 0; j < vetoElectrons.size(); j++) {
	if (cleanElectronJet && deltaR(vetoElectrons[j]->eta(), vetoElectrons[j]->phi(),looseTaus[itau]->eta(),looseTaus[itau]->phi()) < dRCleaningAK4) skiptau = true;
      }
      if(skiptau) continue;
      ntaus++;
    }
  }
    
  
  // W, Z control sample information
  zmmmass  = 0.0; zmmpt = 0.0; zmmeta = 0.0; zmmphi = 0.0;
  zeemass  = 0.0; zeept = 0.0; zeeeta = 0.0; zeephi = 0.0;
  wmnmt    = 0.0; wenmt = 0.0; 
  
  mu1pid = 0; mu1pt  = 0.0; mu1eta = 0.0; mu1phi = 0.0; mu1id = 0;
  mu2pid = 0; mu2pt  = 0.0; mu2eta = 0.0; mu2phi = 0.0; mu2id = 0;
  
  el1pid = 0; el1pt = 0.0; el1eta = 0.0; el1phi = 0.0; el1id = 0; 
  el2pid = 0; el2pt = 0.0; el2eta = 0.0; el2phi = 0.0; el2id = 0; 
    
  nmuons     = looseMuons.size();
  nelectrons = vetoElectrons.size();
  nphotons   = loosePhotons.size();
  
  // one or two loose muons
  if (nmuons == 1 || nmuons == 2) {
      
    pat::MuonRef muon = looseMuons[0];
    mu1pid   = muon->pdgId(); 
    mu1pt    = muon->pt(); 
    mu1eta   = muon->eta(); 
    mu1phi   = muon->phi();
      
    // tight muon
    for (std::size_t i = 0; i < tightMuons.size(); i++) {
      if (muon == tightMuons[i]) 
	mu1id = 1; 
    }
      
    if (nmuons == 1) 
      wmnmt = sqrt(2.0 * mu1pt * t1met * (1.0 - cos(deltaPhi(mu1phi, t1metphi))));
  }
   
  // two loose muons
  if (nmuons == 2) {            
      pat::MuonRef muon = looseMuons[1];
      mu2pid   = muon->pdgId(); 
      mu2pt    = muon->pt(); 
      mu2eta   = muon->eta(); 
      mu2phi   = muon->phi();

      // check if belong to the tight / high pt collection
      for (std::size_t i = 0; i < tightMuons.size(); i++) {
	if (muon == tightMuons[i]) 
	  mu2id = 1;
      }
      
      TLorentzVector mu1vec; 
      mu1vec.SetPtEtaPhiE(mu1pt, mu1eta, mu1phi, looseMuons[0]->p());
      TLorentzVector mu2vec; 
      mu2vec.SetPtEtaPhiE(mu2pt, mu2eta, mu2phi, muon->p());
      
      TLorentzVector zvec(mu1vec);
      zvec += mu2vec;
      
      zmmmass = zvec.M();
      zmmpt   = zvec.Pt();
      zmmeta  = zvec.Eta();            
      zmmphi  = zvec.Phi();
  }
  
  // one or two loose electrons
  if (nelectrons == 1 || nelectrons == 2) {
    pat::ElectronRef electron = vetoElectrons[0];
    el1pid = electron->pdgId();
    el1pt  = electron->pt();
    el1eta = electron->eta();
    el1phi = electron->phi();
      
    for (std::size_t i = 0; i < tightElectrons.size(); i++) {
      if (electron == tightElectrons[i]) 
	el1id = 1;
    }

    if (vetoElectrons.size() == 1) 
      wenmt = sqrt(2.0 * el1pt * t1met * (1.0 - cos(deltaPhi(el1phi, t1metphi))));
    }
  
  // two loose electrons
  if (nelectrons == 2) {
    pat::ElectronRef electron = vetoElectrons[1];
    el2pid = electron->pdgId();
    el2pt  = electron->pt();
    el2eta = electron->eta();
    el2phi = electron->phi();

    for (std::size_t i = 0; i < tightElectrons.size(); i++) {
      if (electron == tightElectrons[i]) el2id = 1;
    }

    TLorentzVector el1vec; el1vec.SetPtEtaPhiE(el1pt, el1eta, el1phi, vetoElectrons[0]->p());
    TLorentzVector el2vec; el2vec.SetPtEtaPhiE(el2pt, el2eta, el2phi, electron->p());
    
    TLorentzVector zvec(el1vec);
    zvec += el2vec;
    
    zeemass = zvec.M();
    zeept   = zvec.Pt();
    zeeeta  = zvec.Eta();
    zeephi  = zvec.Phi();
  }
  
    
  // Photon information
  phpt     = 0; pheta    = 0; phphi    = 0; phid = 0;     
  float hardestPhotonPt = -100;
  int   hardestPhotonIndex = -1;
  for (size_t i = 0; i < loosePhotons.size(); i++) {
    if (loosePhotons[i]->pt() > hardestPhotonPt) {
      hardestPhotonIndex = i;
      hardestPhotonPt = loosePhotons[i]->pt();
    }
  }
  if(hardestPhotonIndex >= 0){
    phpt    = loosePhotons[hardestPhotonIndex]->pt();
    pheta   = loosePhotons[hardestPhotonIndex]->eta();
    phphi   = loosePhotons[hardestPhotonIndex]->phi();	
    for(size_t i = 0; i < mediumPhotons.size(); i++){
      if(loosePhotons[hardestPhotonIndex] == mediumPhotons[i])
	phid = 1;
    }
  }
  
  
  // Substructure CHS
  vjetpt.clear(); vjeteta.clear(); vjetphi.clear(); vjetm.clear(); vjetprunedm.clear(); vjetsoftdropm.clear(); vjettau1.clear(); vjettau2.clear();
  
  for (size_t ijet = 0; ijet < vJetsAK8.size(); ijet++){
    //clean from leptons 
    bool skipjet = false;
    for (std::size_t j = 0; j < looseMuons.size(); j++) {
      if (cleanMuonJet && deltaR(looseMuons[j]->eta(), looseMuons[j]->phi(), vJetsAK8[ijet]->eta(), vJetsAK8[ijet]->phi()) < dRCleaningAK8) 
	skipjet = true;
    }
    
    for (std::size_t j = 0; j < vetoElectrons.size(); j++) {
      if (cleanElectronJet && deltaR(vetoElectrons[j]->eta(), vetoElectrons[j]->phi(), vJetsAK8[ijet]->eta(), vJetsAK8[ijet]->phi()) < dRCleaningAK8) 
	skipjet = true;
    }
  
    for (std::size_t j = 0; j < loosePhotons.size(); j++) {
      if (cleanPhotonJet && deltaR(loosePhotons[j]->eta(), loosePhotons[j]->phi(), vJetsAK8[ijet]->eta(), vJetsAK8[ijet]->phi()) < dRCleaningAK8) 
	skipjet = true;
    }    
    
    if (skipjet) continue;

    if(not isPuppiVJet){
      vjetpt.push_back(vJetsAK8[ijet]->pt());
      vjeteta.push_back(vJetsAK8[ijet]->eta());
      vjetphi.push_back(vJetsAK8[ijet]->phi());
      vjetm.push_back(vJetsAK8[ijet]->mass());
      vjettau1.push_back(vJetsAK8[ijet]->userFloat("NjettinessAK8:tau1"));
      vjettau2.push_back(vJetsAK8[ijet]->userFloat("NjettinessAK8:tau2"));
      vjetprunedm.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsCHSPrunedMass"));
      vjetsoftdropm.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsCHSSoftDropMass"));
    }
    else{
      vjetpt.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsPuppiValueMap:pt"));
      vjeteta.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsPuppiValueMap:eta"));
      vjetphi.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsPuppiValueMap:phi"));
      vjetm.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsPuppiValueMap:mass"));
      vjettau1.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1"));
      vjettau2.push_back(vJetsAK8[ijet]->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau2"));
      vjetprunedm.push_back(-1);
      TLorentzVector puppi_softdrop, puppi_softdrop_subjet;
      auto sdSubjetsPuppi = vJetsAK8[ijet]->subjets("SoftDropPuppi");
      for(auto it : sdSubjetsPuppi) {
	puppi_softdrop_subjet.SetPtEtaPhiM(it->correctedP4(0).pt(),it->correctedP4(0).eta(),it->correctedP4(0).phi(),it->correctedP4(0).mass());
	puppi_softdrop+=puppi_softdrop_subjet;
      }
      vjetsoftdropm.push_back(puppi_softdrop.M());
    }
  }
  
  
  // Generator-level information
  bosonid = 0; bosonmass = 0.0; bosonpt = 0.0; bosoneta = 0.0; bosonphi = 0.0;
  mediatormass = 0.; mediatorphi = 0.; mediatoreta = 0.; mediatorpt = 0.;   

  if (genParticlesH.isValid()){
      
    TLorentzVector dm1vec; 
    TLorentzVector dm2vec; 
    bool foundfirst = false;
      
    // loop on gen particles looking for the DM particles --> then to the mediator
    for (auto gens_iter = genParticlesH->begin(); gens_iter != genParticlesH->end(); ++gens_iter) {       
      bool goodParticle = false;
      if (abs(gens_iter->pdgId()) >= 1000001 and abs(gens_iter->pdgId()) <= 1000039)
	goodParticle = true;
      else if(abs(gens_iter->pdgId()) >= 2000001 and abs(gens_iter->pdgId()) <= 2000015)
	  goodParticle = true;
      else if(abs(gens_iter->pdgId()) == 9100012)
	goodParticle = true;
      else if(abs(gens_iter->pdgId()) == 18) // DM particles in MG DMSimp and SMM
	goodParticle = true;
      
      if(not goodParticle)
	continue;
      
      if(!foundfirst) { // first DM particle
	dm1vec.SetPtEtaPhiM(gens_iter->pt(), gens_iter->eta(), gens_iter->phi(), gens_iter->mass());
	foundfirst = true;
      }
      else{
	dm2vec.SetPtEtaPhiM(gens_iter->pt(), gens_iter->eta(), gens_iter->phi(), gens_iter->mass());
	break;
      }
    }
      
    TLorentzVector medvec(dm1vec);
    medvec += dm2vec;
    mediatorpt  = medvec.Pt();
    mediatoreta = medvec.Eta();
    mediatorphi = medvec.Phi();
    mediatormass = medvec.M();
    
    // loop on genParticles (prunedGenParticles) trying to find W/Z decying leptonically
    for (auto gens_iter = genParticlesH->begin(); gens_iter != genParticlesH->end(); ++gens_iter) {

      if ( (gens_iter->pdgId() == 23 || abs(gens_iter->pdgId()) == 24) and // Z or W-boson
	   gens_iter->numberOfDaughters() > 1 and // before the decay (more than one daughter)
	   abs(gens_iter->daughter(0)->pdgId()) > 10 and 
	   abs(gens_iter->daughter(0)->pdgId()) < 17)  { // decays into leptons, neutrinos 

	  bosonid   = gens_iter->pdgId();
	  bosonmass = gens_iter->mass();
	  bosonpt   = gens_iter->pt();
	  bosoneta  = gens_iter->eta();
	  bosonphi  = gens_iter->phi();
      }      
    }
      
    // if a Z/W is not found look for a pair of lepton .. this way with the pdgId is not guaranteed that you catch a Z/W boson and also recover DY production
    if (bosonid == 0) {
      float l1pt = 0., l2pt = 0., l1eta = 0., l2eta = 0., l1phi = 0., l2phi = 0.;
      int   l1id = 0, l2id = 0; 
      
      for (auto gens_iter = genParticlesH->begin(); gens_iter != genParticlesH->end(); ++gens_iter) {
	if (gens_iter->isPromptFinalState() || gens_iter->isPromptDecayed()) {
	  if (gens_iter->pdgId() >  10 && gens_iter->pdgId() <  17) {
	    l1id   = gens_iter->pdgId();
	    l1pt   = gens_iter->pt();
	    l1eta  = gens_iter->eta();
	    l1phi  = gens_iter->phi();
	  }
	  if (gens_iter->pdgId() < -10 && gens_iter->pdgId() > -17) {
	    l2id   = gens_iter->pdgId();
	    l2pt   = gens_iter->pt();
	    l2eta  = gens_iter->eta();
	    l2phi  = gens_iter->phi();
	  }
	}
      }

      if (l1id > 0) {
	TLorentzVector l1vec;
	TLorentzVector l2vec;
	l1vec.SetPtEtaPhiM(l1pt, l1eta, l1phi, 0.);
	l2vec.SetPtEtaPhiM(l2pt, l2eta, l2phi, 0.);
	TLorentzVector bosonvec(l1vec);
	bosonvec += l2vec;
	bosonmass = bosonvec.M();
	bosonpt   = bosonvec.Pt();
	bosoneta  = bosonvec.Eta();
	bosonphi  = bosonvec.Phi();
	if (l1id+l2id == 0) bosonid = 23;
	else                bosonid = 24;
      }
    }

    // no W or Z decay leptonically --> look for prompt photons
    if (bosonid == 0) {      
      for (auto gens_iter = genParticlesH->begin(); gens_iter != genParticlesH->end(); ++gens_iter) { // loop on prunedGenParticles                                                          
	if (gens_iter->pdgId() == 22 && // photons                                                                                                                           
	    gens_iter->status() == 1 && // final state                                                                                                                       
	    gens_iter->isPromptFinalState() &&
	    gens_iter->pt() > bosonpt) {
	  
	  bosonid   = gens_iter->pdgId();
	  bosonpt   = gens_iter->pt();
	  bosoneta  = gens_iter->eta();
	  bosonphi  = gens_iter->phi();
	  bosonmass = gens_iter->mass();
	}
      }
    }
  }
  tree->Fill();    
  
}    

void MonoJetTreeMaker::beginJob() {

  edm::Service<TFileService> fs;
  tree = fs->make<TTree>("tree"       , "tree");

  // Event info
  tree->Branch("event"                , &event                , "event/i");
  tree->Branch("run"                  , &run                  , "run/i");
  tree->Branch("lumi"                 , &lumi                 , "lumi/i");
  tree->Branch("xsec"                 , &xsec                 , "xsec/F");
  tree->Branch("wgt"                  , &wgt                  , "wgt/F");  
  tree->Branch("putrue"               , &putrue               , "putrue/I"); 
  tree->Branch("nvtx"                 , &nvtx                 , "nvtx/i");  

  // Triggers
  tree->Branch("passingHLTPFMet"      , &passingHLTPFMet      , "passingHLTPFMet/b");
  tree->Branch("passingHLTPFMetNoMu"  , &passingHLTPFMetNoMu  , "passingHLTPFMetNoMu/b");
  tree->Branch("passingHLTPFMetMonojet", &passingHLTPFMetMonojet  , "passingHLTPFMetMonojet/b");
  tree->Branch("passingHLTPhoton",   &passingHLTPhoton,   "passingHLTPhoton/b");
  tree->Branch("passingHLTElectron", &passingHLTElectron, "passingHLTElectron/b");

  // Object counts
  tree->Branch("nmuons"               , &nmuons               , "nmuons/i");
  tree->Branch("nelectrons"           , &nelectrons           , "nelectrons/i");
  tree->Branch("nphotons"             , &nphotons             , "nphotons/i");
  tree->Branch("ntaus"                , &ntaus                , "ntaus/i");
  tree->Branch("njets"                , &njets                , "njets/i");
  tree->Branch("nbjets"               , &nbjets               , "nbjets/i");

  // MET
  tree->Branch("t1met"              , &t1met              , "t1met/F");
  tree->Branch("t1metphi"           , &t1metphi           , "t1metphi/F");
  tree->Branch("t1metnomu"          , &t1metnomu          , "t1metnomu/F");
  tree->Branch("t1metnomuphi"       , &t1metnomuphi       , "t1metnomuphi/F");
  tree->Branch("t1metnoele"         , &t1metnoele         , "t1metnoele/F");
  tree->Branch("t1metnoelephi"      , &t1metnoelephi      , "t1metnoelephi/F");
  tree->Branch("t1metnopho"         , &t1metnopho         , "t1metnopho/F");
  tree->Branch("t1metnophophi"      , &t1metnophophi      , "t1metnophophi/F");
  
  // Jet info
  tree->Branch("jetpt",      "std::vector<float>", &jetpt);
  tree->Branch("jeteta",     "std::vector<float>", &jeteta);
  tree->Branch("jetphi",     "std::vector<float>", &jetphi);
  tree->Branch("jetm",       "std::vector<float>", &jetm);
  tree->Branch("jetbtag",    "std::vector<float>", &jetbtag);
  tree->Branch("jetchfrac",  "std::vector<float>", &jetchfrac);
  tree->Branch("jetnhfrac",  "std::vector<float>", &jetnhfrac);

  // V-Jet info
  tree->Branch("vjetpt",      "std::vector<float>", &vjetpt);
  tree->Branch("vjeteta",     "std::vector<float>", &vjeteta);
  tree->Branch("vjetphi",     "std::vector<float>", &vjetphi);
  tree->Branch("vjetm",       "std::vector<float>", &vjetm);
  tree->Branch("vjettau1",    "std::vector<float>", &vjettau1);
  tree->Branch("vjettau2",    "std::vector<float>", &vjettau2);
  tree->Branch("vjetprunedm", "std::vector<float>", &vjetprunedm);
  tree->Branch("vjetsoftdropm", "std::vector<float>", &vjetsoftdropm);

  // jet-met dphi
  tree->Branch("mindphijetmet"    , &mindphijetmet    , "mindphijetmet/F");
  tree->Branch("mindphijetmetnomu"    , &mindphijetmetnomu    , "mindphijetmetnomu/F");
  tree->Branch("mindphijetmetnoele"    , &mindphijetmetnoele    , "mindphijetmetnoele/F");
  tree->Branch("mindphijetmetnopho"    , &mindphijetmetnopho    , "mindphijetmetnopho/F");
  
  // Lepton info
  tree->Branch("mu1pid"               , &mu1pid               , "mu1pid/I");
  tree->Branch("mu1pt"                , &mu1pt                , "mu1pt/F");
  tree->Branch("mu1eta"               , &mu1eta               , "mu1eta/F");
  tree->Branch("mu1phi"               , &mu1phi               , "mu1phi/F");
  tree->Branch("mu1id"                , &mu1id                , "mu1id/I");

  tree->Branch("mu2pid"               , &mu2pid               , "mu2pid/I");
  tree->Branch("mu2pt"                , &mu2pt                , "mu2pt/F");
  tree->Branch("mu2eta"               , &mu2eta               , "mu2eta/F");
  tree->Branch("mu2phi"               , &mu2phi               , "mu2phi/F");
  tree->Branch("mu2id"                , &mu2id                , "mu2id/I");

  tree->Branch("el1pid"               , &el1pid               , "el1pid/I");
  tree->Branch("el1pt"                , &el1pt                , "el1pt/F");
  tree->Branch("el1eta"               , &el1eta               , "el1eta/F");
  tree->Branch("el1phi"               , &el1phi               , "el1phi/F");
  tree->Branch("el1id"                , &el1id                , "el1id/I");


  tree->Branch("el2pid"               , &el2pid               , "el2pid/I");
  tree->Branch("el2pt"                , &el2pt                , "el2pt/F");
  tree->Branch("el2eta"               , &el2eta               , "el2eta/F");
  tree->Branch("el2phi"               , &el2phi               , "el2phi/F");
  tree->Branch("el2id"                , &el2id                , "el2id/I");

  // Dilepton info
  tree->Branch("zmmmass"              , &zmmmass              , "zmmmass/F");
  tree->Branch("zeemass"              , &zeemass              , "zeemass/F");
  tree->Branch("wmnmt"                , &wmnmt                , "wmnmt/F");
  tree->Branch("wenmt"                , &wenmt                , "wenmt/F");  
  tree->Branch("zmmpt"                , &zmmpt                , "zmmpt/F");
  tree->Branch("zmmeta"               , &zmmeta               , "zmmeta/F");
  tree->Branch("zmmphi"               , &zmmphi               , "zmmphi/F");
  tree->Branch("zeept"                , &zeept                , "zeept/F");
  tree->Branch("zeeeta"               , &zeeeta               , "zeeeta/F");
  tree->Branch("zeephi"               , &zeephi               , "zeephi/F");

  tree->Branch("phpt"                 , &phpt                 , "phpt/F");
  tree->Branch("pheta"                , &pheta                , "pheta/F");
  tree->Branch("phphi"                , &phphi                , "phphi/F");
  tree->Branch("phid"                 , &phid                 , "phid/I");

  // Boson information
  tree->Branch("bosonid"              , &bosonid                 , "bosonid/I");
  tree->Branch("bosonmass"            , &bosonmass               , "bosonmass/F");
  tree->Branch("bosonpt"              , &bosonpt                 , "bosonpt/F");
  tree->Branch("bosoneta"             , &bosoneta                , "bosoneta/F");
  tree->Branch("bosonphi"             , &bosonphi                , "bosonphi/F");
  
  // DM mediator
  tree->Branch("mediatormass",&mediatormass,"mediatormass/F");
  tree->Branch("mediatorpt",&mediatorpt,"mediatorpt/F");
  tree->Branch("mediatoreta",&mediatoreta,"mediatoreta/F");
  tree->Branch("mediatorphi",&mediatorphi,"mediatorphi/F");

}
void MonoJetTreeMaker::endJob() {}

void MonoJetTreeMaker::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) {
  // triggers for the Analysis
  triggerPathsVector.push_back("HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMETNoMu90_PFMHTNoMu90_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_v");
  triggerPathsVector.push_back("HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_v");
  triggerPathsVector.push_back("HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMET90_PFMHT90_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMET100_PFMHT100_IDTight_v"); 
  triggerPathsVector.push_back("HLT_PFMET110_PFMHT110_IDTight_v"); 
  triggerPathsVector.push_back("HLT_PFMET120_PFMHT120_IDTight"); 
  triggerPathsVector.push_back("HLT_PFMET170_NoiseCleaned"); 
  triggerPathsVector.push_back("HLT_PFMET170_JetIdCleaned"); 
  triggerPathsVector.push_back("HLT_PFMET170_HBHECleaned"); 
  triggerPathsVector.push_back("HLT_PFMET170_v"); 
  triggerPathsVector.push_back("HLT_PFMET300_NoiseCleaned"); 
  triggerPathsVector.push_back("HLT_PFMET300_JetIdCleaned"); 
  triggerPathsVector.push_back("HLT_PFMET300_v"); 
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight"); 
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight"); 
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90_IDTight");    
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu100_PFMHTNoMu100_IDTight_v");
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu110_PFMHTNoMu110_IDTight_v");
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight"); 
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight"); 
  triggerPathsVector.push_back("HLT_MonoCentralPFJet80_PFMETNoMu120_PFMHTNoMu120_IDTight"); 
  triggerPathsVector.push_back("HLT_Photon165_HE10"); 
  triggerPathsVector.push_back("HLT_Photon175");      
  triggerPathsVector.push_back("HLT_Ele27_WPTight_Gsf_v"); 
  triggerPathsVector.push_back("HLT_Ele27_eta2p1_WPTight_Gsf_v"); 
  triggerPathsVector.push_back("HLT_Ele105_CaloIdVT_GsfTrkIdT_v"); 
  triggerPathsVector.push_back("HLT_Ele115_CaloIdVT_GsfTrkIdT_v"); 
  
  HLTConfigProvider hltConfig; // load triggers
  bool changedConfig = false;
  hltConfig.init(iRun, iSetup, triggerResultsTag.process(), changedConfig);
  
  // intialiaze the trigger map
  for (size_t i = 0; i < triggerPathsVector.size(); i++) {
    triggerPathsMap[triggerPathsVector[i]] = -1;
  }
  
  for(size_t i = 0; i < triggerPathsVector.size(); i++){
    TPRegexp pattern(triggerPathsVector[i]);
    for(size_t j = 0; j < hltConfig.triggerNames().size(); j++){
      std::string pathName = hltConfig.triggerNames()[j];
      if(TString(pathName).Contains(pattern)){
	triggerPathsMap[triggerPathsVector[i]] = j;
      }
    }
  }
}

void MonoJetTreeMaker::endRun(edm::Run const&, edm::EventSetup const&) {}

void MonoJetTreeMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


DEFINE_FWK_MODULE(MonoJetTreeMaker);
