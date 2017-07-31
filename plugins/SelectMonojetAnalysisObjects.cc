#include "SelectMonojetAnalysisObjects.h"

#include <TLorentzVector.h>

SelectMonojetAnalysisObjects::SelectMonojetAnalysisObjects(const edm::ParameterSet& iConfig): 
  verticesToken            (consumes<std::vector<reco::Vertex> > (iConfig.getParameter<edm::InputTag>("vertices"))),
  muonsToken               (consumes<std::vector<pat::Muon> > (iConfig.getParameter<edm::InputTag>("muons"))), 
  muonSelection            (iConfig.getParameter<std::vector<edm::ParameterSet > >("muonSelection")),
  electronsToken           (consumes<std::vector<pat::Electron> > (iConfig.getParameter<edm::InputTag>("electrons"))),
  electronSelection        (iConfig.getParameter<std::vector<edm::ParameterSet> >("electronSelection")),
  photonsToken             (consumes<std::vector<pat::Photon> > (iConfig.getParameter<edm::InputTag>("photons"))),
  photonSelection          (iConfig.getParameter<std::vector<edm::ParameterSet> >("photonSelection")),
  tausToken                (consumes<std::vector<pat::Tau> >  (iConfig.getParameter<edm::InputTag>("taus"))),
  tauSelection             (iConfig.getParameter<std::vector<edm::ParameterSet> >("tauSelection")),
  jetsAK8Token             (consumes<std::vector<pat::Jet> > (iConfig.getParameter<edm::InputTag>("ak8Jets")))
{

  // produces output muon collections
  for(auto imuon : muonSelection)
    produces<pat::MuonRefVector>(imuon.getParameter<std::string>("muonCollectionName"));

  // produces output electron collections
  for (auto iele : electronSelection){
    electronIdMapToken.push_back(consumes<edm::ValueMap<bool> > (iele.getParameter<edm::InputTag>("eleValueMap")));
    produces<pat::ElectronRefVector>(iele.getParameter<std::string>("electronCollectionName"));
  }

  // produces output tau collections
  for(auto itau : tauSelection)
    produces<pat::TauRefVector>(itau.getParameter<std::string>("tauCollectionName"));
  
  // produces output photon collections
  for (auto ipho : photonSelection){
    photonIdMapToken.push_back(consumes<edm::ValueMap<bool> > (ipho.getParameter<edm::InputTag>("photonValueMap")));
    produces<pat::PhotonRefVector>(ipho.getParameter<std::string>("photonCollectionName"));
  }

  // produces output ak8 collection
  for(auto ijet : jetAK8Selection)
    produces<pat::JetRefVector>(ijet.getParameter<std::string>("jetCollectionName"));
  
  // useful for taus cleaning
  looseMuonPosition     = -1;
  vetoElectronPosition  = -1;
  loosePhotonPosition   = -1;

  for(size_t imuon = 0; imuon < muonSelection.size(); imuon++){ // --> find what is the looseMuon position
    if(muonSelection.at(imuon).getParameter<std::string>("idType") == "loose" or muonSelection.at(imuon).getParameter<std::string>("idType") == "Loose")
      looseMuonPosition = imuon;
  }
  
  if(looseMuonPosition < 0 or looseMuonPosition == int(muonSelection.size()))
    throw cms::Exception("SelectMonojetAnalysisObjects") <<" no loose muons found  --> check \n";

  for(size_t iele = 0; iele < electronSelection.size(); iele++){
    if(electronSelection.at(iele).getParameter<std::string>("idType") == "veto" or electronSelection.at(iele).getParameter<std::string>("idType") == "Veto")
      vetoElectronPosition = iele;
  }
  
  if(vetoElectronPosition < 0 or vetoElectronPosition == int(electronSelection.size()))
    throw cms::Exception("SelectMonojetAnalysisObjects") <<" no veto electrons found  --> check \n";

  for(size_t ipho = 0; ipho < photonSelection.size(); ipho++){
    if(photonSelection.at(ipho).getParameter<std::string>("idType") == "loose" or photonSelection.at(ipho).getParameter<std::string>("idType") == "Loose")
      loosePhotonPosition = ipho;
  }
  
  if(loosePhotonPosition < 0 or loosePhotonPosition == int(photonSelection.size()))
    throw cms::Exception("SelectMonojetAnalysisObjects") <<" no loose photons found  --> check \n";

}


SelectMonojetAnalysisObjects::~SelectMonojetAnalysisObjects() {}

void SelectMonojetAnalysisObjects::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;
  using namespace reco;
  using namespace std;

  // vertices
  Handle<std::vector<reco::Vertex> > verticesH;
  iEvent.getByToken(verticesToken, verticesH);
  // muons
  Handle<std::vector<pat::Muon> > muonsH;
  iEvent.getByToken(muonsToken, muonsH);  
  // electrons
  Handle<std::vector<pat::Electron> > electronsH;
  iEvent.getByToken(electronsToken, electronsH);
  std::vector<Handle<edm::ValueMap<bool> > > electronMapH;
  electronMapH.reserve(electronIdMapToken.size());
  for(auto eleToken :  electronIdMapToken){
    electronMapH.push_back(Handle<edm::ValueMap<bool> > ());
    iEvent.getByToken(eleToken,electronMapH.back());
  }
  // photons
  Handle<std::vector<pat::Photon> > photonsH;
  iEvent.getByToken(photonsToken, photonsH);
  std::vector<Handle<edm::ValueMap<bool> > > photonsMapH;
  for(auto phoToken :  photonIdMapToken){
    photonsMapH.push_back(Handle<edm::ValueMap<bool> > ());
    iEvent.getByToken(phoToken,photonsMapH.back());
  }  
  // taus
  Handle<std::vector<pat::Tau> > tausH;
  iEvent.getByToken(tausToken, tausH);
  // AK8 jets
  Handle<std::vector<pat::Jet> > jetsAK8H;
  iEvent.getByToken(jetsAK8Token, jetsAK8H);
  
  // output collections: muons
  std::vector<std::auto_ptr<pat::MuonRefVector> > outputmuons;
  for(auto imuon : muonSelection)
    outputmuons.push_back(std::auto_ptr<pat::MuonRefVector>(new pat::MuonRefVector));
  // output collections: electrons
  std::vector<std::auto_ptr<pat::ElectronRefVector> > outputelectrons;
  for(auto iele  : electronSelection)
    outputelectrons.push_back(std::auto_ptr<pat::ElectronRefVector>(new pat::ElectronRefVector));
  // output collections: photons
  std::vector<std::auto_ptr<pat::PhotonRefVector> > outputphotons;
  for(auto ipho  : photonSelection)
    outputphotons.push_back(std::auto_ptr<pat::PhotonRefVector>(new pat::PhotonRefVector));
  // output collections: taus
  std::vector<std::auto_ptr<pat::TauRefVector> > outputtaus;
  for(auto itau  : tauSelection)
    outputtaus.push_back(std::auto_ptr<pat::TauRefVector>(new pat::TauRefVector));
  // output collections: Vjets
  std::vector<std::auto_ptr<pat::JetRefVector> > outputvjets;
  for(auto ijet  : jetAK8Selection)
    outputvjets.push_back(std::auto_ptr<pat::JetRefVector>(new pat::JetRefVector));
    
  //muon info https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2
  for (vector<pat::Muon>::const_iterator muons_iter = muonsH->begin(); muons_iter != muonsH->end(); ++muons_iter) {
    if (verticesH->size() == 0) continue;
    // loop on the muon selection definition
    size_t ipos = 0;
    for(auto imuon : muonSelection){ /// loop on the muon-ids     
      bool passeskincuts = (muons_iter->pt() > imuon.getParameter<double>("ptMin") && fabs(muons_iter->eta()) < imuon.getParameter<double>("absEta"));
      float isoval       = std::max(0.,muons_iter->pfIsolationR04().sumNeutralHadronEt+muons_iter->pfIsolationR04().sumPhotonEt-0.5*muons_iter->pfIsolationR04().sumPUPt);
      isoval += muons_iter->pfIsolationR04().sumChargedHadronPt;
      isoval /= muons_iter->pt();

      if (passeskincuts) { // fill loose muons
	if(imuon.getParameter<std::string>("idType") == "loose" or imuon.getParameter<std::string>("idType") =="Loose"){
	  if (muon::isLooseMuon(*muons_iter) && isoval < imuon.getParameter<double>("isolation")) 
	    outputmuons.at(ipos)->push_back(pat::MuonRef(muonsH, muons_iter - muonsH->begin()));
	} 
	else if(imuon.getParameter<std::string>("idType") == "tight" or imuon.getParameter<std::string>("idType") =="Tight"){ // fill tight muons
	  if (muon::isTightMuon(*muons_iter,verticesH->at(0)) && isoval < imuon.getParameter<double>("isolation")) 
	    outputmuons.at(ipos)->push_back(pat::MuonRef(muonsH, muons_iter - muonsH->begin()));
	}
      }
      ipos++;
    }
  }
  
  //electron info https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
  for (vector<pat::Electron>::const_iterator electrons_iter = electronsH->begin(); electrons_iter != electronsH->end(); ++electrons_iter) {
    const Ptr<pat::Electron> electronPtr(electronsH, electrons_iter - electronsH->begin());
    size_t ipos = 0;
    for(auto iele : electronSelection){
      // bool for the electron id
      bool passesid = (*electronMapH.at(ipos))[electronPtr];            
      // bool for dxy and dz cut that are taken out from standard electron VID
      bool pass_dxy = false;
      bool pass_dz  = false;
      if(verticesH->size() > 0){
	const reco::Vertex & vtx  = verticesH->at(0);
	// barrel
	if(electrons_iter->isEB() and 
	   fabs(electrons_iter->gsfTrack()->dxy(vtx.position())) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("d0Barrel"))
	  pass_dxy = true;
	// endcalp
	else if(not electrons_iter->isEB() and 
		fabs(electrons_iter->gsfTrack()->dxy(vtx.position())) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("d0Endcap"))
	  pass_dxy = true;
	// barrel
	if(electrons_iter->isEB() and 
	   fabs(electrons_iter->gsfTrack()->dz(vtx.position())) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("dzBarrel"))
	  pass_dz = true;
	//endcap
	else if(not electrons_iter->isEB() and 
		fabs(electrons_iter->gsfTrack()->dz(vtx.position())) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("dzEndcap"))
	  pass_dz = true;
      }
      else{
	//barrel
	if(electrons_iter->isEB() and 
	   fabs(electrons_iter->gsfTrack()->dxy()) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("d0Barrel"))
	  pass_dxy = true;
	//endcap
	else if(not electrons_iter->isEB() and
		fabs(electrons_iter->gsfTrack()->dxy()) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("d0Endcap"))
	  pass_dxy = true;
	
	//barrel
	if(electrons_iter->isEB() and 
	   fabs(electrons_iter->gsfTrack()->dz()) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("dzBarrel"))
	  pass_dz = true;
	//endcap
	else if(not electrons_iter->isEB() and
		fabs(electrons_iter->gsfTrack()->dz()) < iele.getParameter<edm::ParameterSet>("PVSelection").getParameter<double>("dzEndcap"))
	  pass_dz = true;
      }

      // apply selections
      if(not passesid) continue;
      if(not pass_dxy) continue;
      if(not pass_dz) continue;

      // apply basic kinematic cuts
      bool passeskincuts  = (electrons_iter->pt() > iele.getParameter<double>("ptMin") &&
			     fabs(electrons_iter->superCluster()->eta()) < iele.getParameter<double>("absEta"));
      if(not passeskincuts) continue;
      outputelectrons.at(ipos)->push_back(pat::ElectronRef(electronsH, electrons_iter - electronsH->begin()));
      ipos++;
    }
  }


  // photon https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2
  for (vector<pat::Photon>::const_iterator photons_iter = photonsH->begin(); photons_iter != photonsH->end(); ++photons_iter) {
    /// read the super-cluster and iso variables
    const Ptr<pat::Photon> photonPtr(photonsH, photons_iter - photonsH->begin());
    size_t ipos = 0;
    for(auto ipho : photonSelection){
      // standard ID
      bool passesid = (*photonsMapH.at(ipos))[photonPtr];
      if(not passesid) continue;
      if (fabs(photons_iter->superCluster()->eta()) > ipho.getParameter<double>("absEta") ) continue;
      if (photons_iter->pt() < ipho.getParameter<double>("ptMin")) continue;
      if (not photons_iter->passElectronVeto()) continue;
      outputphotons.at(ipos)->push_back(pat::PhotonRef(photonsH, photons_iter - photonsH->begin()));
      ipos++;
    }
  }    
  
  // taus --> according to tau pog
  for (vector<pat::Tau>::const_iterator taus_iter = tausH->begin(); taus_iter != tausH->end(); ++taus_iter) {
    /// loop on the tau selection
    size_t ipos = 0;
    for(auto itau : tauSelection){
      bool skiptau = false;
      for (std::size_t j = 0; j < outputmuons.at(looseMuonPosition)->size(); j++) {
	if (deltaR(outputmuons.at(looseMuonPosition)->at(j)->eta(), outputmuons.at(looseMuonPosition)->at(j)->phi(), 
		   taus_iter->eta(), taus_iter->phi()) < itau.getParameter<double>("dRCleaning")) skiptau = true;
      }
      for (std::size_t j = 0; j < outputelectrons.at(vetoElectronPosition)->size(); j++) {
	if (deltaR(outputelectrons.at(vetoElectronPosition)->at(j)->eta(), outputelectrons.at(vetoElectronPosition)->at(j)->phi(), 
		   taus_iter->eta(), taus_iter->phi()) < itau.getParameter<double>("dRCleaning")) skiptau = true;
      }
      if(skiptau) continue;
      
      string decayMode = "decayModeFinding";
      if(not itau.getParameter<bool>("useOldDecayMode"))
	decayMode = "decayModeFindingNewDMs";
      
      if (taus_iter->pt() > itau.getParameter<double>("ptMin") &&
	  fabs(taus_iter->eta()) < itau.getParameter<double>("absEta") &&
	  taus_iter->tauID(decayMode) > 0.5 && // check if the boolean is true
	  taus_iter->tauID(itau.getParameter<std::string>("tauIDName")) > 0.5 // check if the boolean is true
	  )
	outputtaus.at(ipos)->push_back(pat::TauRef(tausH, taus_iter - tausH->begin()));
      ipos++;      
    }
  }

  //ak8 jets
  for (vector<pat::Jet>::const_iterator jets_iter = jetsAK8H->begin(); jets_iter != jetsAK8H->end(); ++jets_iter) {
    size_t ipos = 0;
    for(auto ijet : jetAK8Selection){
      // clean tau candidates from identified muons and electrons (loosely identified)
      bool skipjet = false;
      for (std::size_t j = 0; j < outputmuons.at(looseMuonPosition)->size(); j++) {
	if (deltaR(outputmuons.at(looseMuonPosition)->at(j)->eta(), outputmuons.at(looseMuonPosition)->at(j)->phi(), 
		   jets_iter->eta(), jets_iter->phi()) < ijet.getParameter<double>("dRCleaning")) skipjet = true;
      }
      for (std::size_t j = 0; j < outputelectrons.at(vetoElectronPosition)->size(); j++) {
	if (deltaR(outputelectrons.at(vetoElectronPosition)->at(j)->eta(), outputelectrons.at(vetoElectronPosition)->at(j)->phi(), 
		   jets_iter->eta(), jets_iter->phi()) < ijet.getParameter<double>("dRCleaning")) skipjet = true;
      }
      for (std::size_t j = 0; j < outputphotons.at(loosePhotonPosition)->size(); j++) {
	if (deltaR(outputphotons.at(loosePhotonPosition)->at(j)->eta(), outputphotons.at(loosePhotonPosition)->at(j)->phi(), 
		   jets_iter->eta(), jets_iter->phi()) < ijet.getParameter<double>("dRCleaning")) skipjet = true;
      }
      if(skipjet) continue;    
      
      //////
      if(not ijet.getParameter<bool>("usePuppi")){ // use Pruning + N-subjettiness on top of Puppi AK8 jets
	if(jets_iter->pt() < ijet.getParameter<double>("ptMin")) continue;
	if(fabs(jets_iter->eta()) > ijet.getParameter<double>("absEta")) continue;
	if(jets_iter->userFloat("NjettinessAK8:tau2")/jets_iter->userFloat("NjettinessAK8:tau1") > ijet.getParameter<double>("tau2tau1")) continue;
	if(jets_iter->userFloat("ak8PFJetsCHSPrunedMass") < ijet.getParameter<double>("massMin")) continue;
	if(jets_iter->userFloat("ak8PFJetsCHSPrunedMass") > ijet.getParameter<double>("massMax")) continue;
      }
      else{ // use Soft-drop + N-subjettiness on top of Puppi AK8 jets
	if(jets_iter->userFloat("ak8PFJetsPuppiValueMap:pt") < ijet.getParameter<double>("ptMin")) continue;
	if(fabs(jets_iter->userFloat("ak8PFJetsPuppiValueMap:mass")) > ijet.getParameter<double>("absEta")) continue;      
	if(jets_iter->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau2")/jets_iter->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1") > ijet.getParameter<double>("tau2tau1")) continue;
	TLorentzVector puppi_softdrop, puppi_softdrop_subjet;
	auto sdSubjetsPuppi = jets_iter->subjets("SoftDropPuppi");
	for(auto it : sdSubjetsPuppi) {
          puppi_softdrop_subjet.SetPtEtaPhiM(it->correctedP4(0).pt(),it->correctedP4(0).eta(),it->correctedP4(0).phi(),it->correctedP4(0).mass());
          puppi_softdrop+=puppi_softdrop_subjet;
	}
	if(puppi_softdrop.M() < ijet.getParameter<double>("massMin")) continue;
	if(puppi_softdrop.M() > ijet.getParameter<double>("massMax")) continue;
      }      
      outputvjets.at(ipos)->push_back(pat::JetRef(jetsAK8H, jets_iter - jetsAK8H->begin()));
      ipos++;      
    }
  }
  
  /////////// store output collections
  for(size_t imuon = 0; imuon < outputmuons.size(); imuon++)
    iEvent.put(outputmuons.at(imuon), muonSelection.at(imuon).getParameter<std::string>("muonCollectionName"));

  for(size_t ielectron = 0; ielectron < outputelectrons.size(); ielectron++)
    iEvent.put(outputelectrons.at(ielectron), electronSelection.at(ielectron).getParameter<std::string>("electronCollectionName"));

  for(size_t itau = 0; itau < outputtaus.size(); itau++)
    iEvent.put(outputtaus.at(itau), tauSelection.at(itau).getParameter<std::string>("tauCollectionName"));

  for(size_t iphoton = 0; iphoton < outputphotons.size(); iphoton++)
    iEvent.put(outputphotons.at(iphoton), photonSelection.at(iphoton).getParameter<std::string>("photonCollectionName"));

  for(size_t ijet = 0; ijet < outputvjets.size(); ijet++)
    iEvent.put(outputvjets.at(ijet), jetAK8Selection.at(ijet).getParameter<std::string>("vjetCollectionName"));

}

void SelectMonojetAnalysisObjects::beginJob() {}
void SelectMonojetAnalysisObjects::endJob() {}
void SelectMonojetAnalysisObjects::beginRun(edm::Run const&, edm::EventSetup const&) {}
void SelectMonojetAnalysisObjects::endRun(edm::Run const&, edm::EventSetup const&) {}
void SelectMonojetAnalysisObjects::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}
void SelectMonojetAnalysisObjects::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

void SelectMonojetAnalysisObjects::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(SelectMonojetAnalysisObjects);
