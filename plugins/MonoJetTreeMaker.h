#ifndef MonoJetTreeMaker_H
#define MonoJetTreeMaker_H

// basic C++ headers
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>

// FWCore
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// TFile service
#include "CommonTools/UtilAlgos/interface/TFileService.h" 

// HLT info
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

// Gen Info
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

// DataFormats
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/ValueMap.h"

// ROOT
#include "TH1F.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TPRegexp.h"

class MonoJetTreeMaker : public edm::one::EDAnalyzer<edm::one::SharedResources,edm::one::WatchRuns> {

public:
  explicit MonoJetTreeMaker(const edm::ParameterSet&);
  ~MonoJetTreeMaker();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
private:

  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;

  virtual void beginJob() override;
  virtual void endJob() override;  
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override;

  // Gen Particles and MC info
  edm::EDGetTokenT<std::vector<PileupSummaryInfo> >  pileupEventInfoToken;
  edm::EDGetTokenT<GenEventInfoProduct>              genEventInfoToken;
  edm::EDGetTokenT<edm::View<reco::GenParticle> >    genParticlesToken;

  // cross section set by hand
  float xsec;

  // InputTags for trigger result
  const edm::InputTag triggerResultsTag;
  edm::EDGetTokenT<edm::TriggerResults>         triggerResultsToken;

  // Vertex
  const edm::InputTag verticesTag;
  edm::EDGetTokenT<std::vector<reco::Vertex> > verticesToken;
  
  // Muons
  const edm::InputTag looseMuonsTag;
  const edm::InputTag tightMuonsTag;
  edm::EDGetTokenT<pat::MuonRefVector> looseMuonsToken;
  edm::EDGetTokenT<pat::MuonRefVector> tightMuonsToken;

  // electrons
  const edm::InputTag  vetoElectronsTag;
  const edm::InputTag  tightElectronsTag;
  edm::EDGetTokenT<pat::ElectronRefVector>  vetoElectronsToken;
  edm::EDGetTokenT<pat::ElectronRefVector>  tightElectronsToken;

  // Photons
  const edm::InputTag  loosePhotonsTag;
  const edm::InputTag  mediumPhotonsTag;
  edm::EDGetTokenT<pat::PhotonRefVector>    loosePhotonsToken;
  edm::EDGetTokenT<pat::PhotonRefVector>    mediumPhotonsToken;

  // Taus
  const edm::InputTag looseTausTag;
  edm::EDGetTokenT<pat::TauRefVector> looseTausToken;

  //Jets AK4
  const edm::InputTag jetsAK4Tag;
  edm::EDGetTokenT<std::vector<pat::Jet> >  jetsAK4Token;

  //Jets AK8
  const edm::InputTag vJetsAK8Tag;
  const bool isPuppiVJet;
  edm::EDGetTokenT<pat::JetRefVector >  vJetsAK8Token;
  
  // MET and recoil
  const edm::InputTag t1PFMETTag;
  const edm::InputTag t1PFMETNoMuonTag;
  const edm::InputTag t1PFMETNoElectronTag;
  const edm::InputTag t1PFMETNoPhotonTag;
  edm::EDGetTokenT<edm::View<pat::MET> >  t1PFMETToken;
  edm::EDGetTokenT<edm::View<pat::MET> >  t1PFMETNoMuonToken;
  edm::EDGetTokenT<edm::View<pat::MET> >  t1PFMETNoElectronToken;
  edm::EDGetTokenT<edm::View<pat::MET> >  t1PFMETNoPhotonToken;

  // cleaning options and thresholds
  const bool cleanMuonJet;
  const bool cleanElectronJet;
  const bool cleanPhotonJet;
  const double dRCleaningAK4;
  const double dRCleaningAK8;
  const double btaggingCSVWP;
  const double minJetPtCountAK4;
  const double minJetPtBveto;
 
  // inner vectors
  std::vector<std::string>   triggerPathsVector;
  std::map<std::string, int> triggerPathsMap;

  // output tree branches
  TTree* tree;

  // event info
  uint32_t event, run, lumi;  
  uint32_t nvtx;
  uint32_t nmuons, nelectrons, nphotons, ntaus, njets, nbjets;

  // pileup info
  int32_t putrue; 

  // W/Z boson at gen-level
  int32_t bosonid;
  float   bosonmass,bosonpt,bosoneta,bosonphi;

  // lepton info: muon electrons and photons --> at most two per event, pdgId, id-type and kinematics
  int32_t mu1pid,mu2pid,mu1id,mu2id;
  int32_t el1pid,el2pid,el1id,el2id;
  int32_t phid;
  float mu1pt,mu1eta,mu1phi,mu2pt,mu2eta,mu2phi;
  float el1pt,el1eta,el1phi,el2pt,el2eta,el2phi;
  float phpt,pheta,phphi,phe;
  float zmmmass,zmmpt,zmmeta,zmmphi,zeemass,zeept,zeeeta,zeephi;
  float wmnmt,wenmt;
  
  // trigger flags
  uint8_t passingHLTPFMet, passingHLTPFMetNoMu, passingHLTPFMetMonojet;
  // photon trigger
  uint8_t passingHLTPhoton;
  // electron trigger
  uint8_t passingHLTElectron;

  // met quantities
  float t1met,t1metphi,t1metnomu,t1metnomuphi,t1metnoele,t1metnoelephi,t1metnopho,t1metnophophi;

  // AK4 jet info
  std::vector<float> jetpt,jeteta,jetphi,jetm,jetbtag,jetchfrac,jetnhfrac;

  //jet met dphi
  float mindphijetmet, mindphijetmetnomu, mindphijetmetnoele, mindphijetmetnopho;
  
  // AK8 jets
  std::vector<float> vjetpt, vjeteta, vjetphi, vjetm, vjetprunedm, vjetsoftdropm, vjettau1, vjettau2;

  // DM mediator and DM particles
  float mediatormass,mediatorpt,mediatoreta,mediatorphi;
  
  // weights
  float wgt;
  
};

#endif
