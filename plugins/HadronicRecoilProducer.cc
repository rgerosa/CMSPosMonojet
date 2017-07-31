#include <memory>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/LorentzVector.h"

template< class T>
class HadronicRecoilProducerT : public edm::stream::EDProducer<> {

public:
  explicit HadronicRecoilProducerT(const edm::ParameterSet&);
  ~HadronicRecoilProducerT();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  virtual void produce(edm::Event&, const edm::EventSetup&); 
  virtual void beginJob();
  virtual void endJob();

  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  ////
  const edm::InputTag  metCollection;
  std::vector<edm::InputTag> candToSubtract;
  const edm::InputTag  pfCandidates;
  
  ///
  edm::EDGetTokenT<edm::View<pat::MET> > metCollectionToken;
  edm::EDGetTokenT<edm::View<reco::Candidate> > pfCandidatesToken; 
  edm::EDGetTokenT<edm::View<T> > theCandToken;
  std::vector<edm::EDGetTokenT<edm::View<T> > > candToSubtractToken;
};

template< class T>
HadronicRecoilProducerT<T>::HadronicRecoilProducerT(const edm::ParameterSet& iConfig): 
  metCollection(iConfig.getParameter<edm::InputTag>("metCollection")),
  candToSubtract(iConfig.getParameter<std::vector<edm::InputTag> >("candToSubtract")),
  pfCandidates(iConfig.existsAs<edm::InputTag>("pfCandidates") ? iConfig.getParameter<edm::InputTag>("pfCandidates") : edm::InputTag("packedPFCandidates")){
  
  metCollectionToken = consumes<edm::View<pat::MET> > (metCollection);
  pfCandidatesToken = consumes<edm::View<reco::Candidate> >(pfCandidates);
  
  for (std::size_t i = 0; i < candToSubtract.size(); i++) {
    theCandToken = consumes<edm::View<T> > (candToSubtract[i]);
    candToSubtractToken.push_back(theCandToken);
  }

  // output colletion
  produces<pat::METCollection>();  

}

template< class T>
HadronicRecoilProducerT<T>::~HadronicRecoilProducerT() {
  candToSubtract.clear();
  candToSubtractToken.clear();
}

template< class T>
void HadronicRecoilProducerT<T>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;
  using namespace reco;
  using namespace std;
  
  Handle<View<pat::MET> > metH;
  iEvent.getByToken(metCollectionToken, metH);
  
  Handle<View<reco::Candidate> > pfCandH;
  iEvent.getByToken(pfCandidatesToken, pfCandH);
  
  std::vector<Handle<View<T> > > candHs;
  for (std::size_t i = 0; i < candToSubtract.size(); i++) {
    Handle<View<T> > candH;
    iEvent.getByToken(candToSubtractToken[i], candH);
    candHs.push_back(candH);
  }
  
  std::auto_ptr<pat::METCollection> output(new pat::METCollection);
  
  double met    = metH->front().corPt();
  double metphi = metH->front().corPhi();
  
  double ccmetx = met * cos(metphi);
  double ccmety = met * sin(metphi);

  // sum-up the particles to the met again
  for (size_t i = 0; i < candHs.size(); i++) {
    for (auto cands_iter = candHs[i]->begin(); cands_iter != candHs[i]->end(); ++cands_iter) {
      reco::Candidate::LorentzVector total4V;
      total4V = cands_iter->p4();
      ccmetx += cands_iter->p4().pt() * cos(cands_iter->p4().phi());
      ccmety += cands_iter->p4().pt() * sin(cands_iter->p4().phi());
    }
  }

  double ccmet = sqrt(ccmetx*ccmetx + ccmety*ccmety);    
  pat::MET* ccmetcand = metH->front().clone();
  ccmetcand->setP4(reco::Candidate::LorentzVector(ccmetx, ccmety, 0., ccmet));
  output->push_back(*ccmetcand);
  iEvent.put(output);

  if(ccmetcand) delete ccmetcand;

}

template< class T>
void HadronicRecoilProducerT<T>::beginJob() {}

template< class T>
void HadronicRecoilProducerT<T>::endJob() {}

template< class T>
void HadronicRecoilProducerT<T>::beginRun(edm::Run const&, edm::EventSetup const&) {}

template< class T>
void HadronicRecoilProducerT<T>::endRun(edm::Run const&, edm::EventSetup const&) {}

template< class T>
void HadronicRecoilProducerT<T>::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

template< class T>
void HadronicRecoilProducerT<T>::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

template< class T>
void HadronicRecoilProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

// specialization
// specialization for muons
template<>
void HadronicRecoilProducerT<pat::Muon>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;
  using namespace reco;
  using namespace std;

  Handle<View<pat::MET> > metH;
  iEvent.getByToken(metCollectionToken, metH);

  Handle<View<reco::Candidate> > pfCandH;
  iEvent.getByToken(pfCandidatesToken, pfCandH);
  
  std::vector<Handle<View<pat::Muon> > > candHs;
  for (std::size_t i = 0; i < candToSubtractToken.size(); i++) {
    Handle<View<pat::Muon> > candH;
    iEvent.getByToken(candToSubtractToken[i], candH);
    candHs.push_back(candH);
  }
  
  std::auto_ptr<pat::METCollection> output(new pat::METCollection);
  
  double met    = metH->front().corPt();
  double metphi = metH->front().corPhi();  
  double ccmetx = met * cos(metphi);
  double ccmety = met * sin(metphi);

  for (size_t i = 0; i < candHs.size(); i++) {
    for (auto cands_iter = candHs[i]->begin(); cands_iter != candHs[i]->end(); ++cands_iter) {      
      ccmetx += cands_iter->p4().pt() * cos(cands_iter->p4().phi());
      ccmety += cands_iter->p4().pt() * sin(cands_iter->p4().phi());
    }            
  }

  double ccmet = sqrt(ccmetx*ccmetx + ccmety*ccmety);  
  pat::MET* ccmetcand = metH->front().clone();
  ccmetcand->setP4(reco::Candidate::LorentzVector(ccmetx, ccmety, 0., ccmet));
  output->push_back(*ccmetcand);
  iEvent.put(output);
  if(ccmetcand) delete ccmetcand;
}


typedef HadronicRecoilProducerT<pat::Muon> MuonCorrectedMETProducer;
DEFINE_FWK_MODULE(MuonCorrectedMETProducer);

typedef HadronicRecoilProducerT<pat::Electron> ElectronCorrectedMETProducer;
DEFINE_FWK_MODULE(ElectronCorrectedMETProducer);

typedef HadronicRecoilProducerT<pat::Photon> PhotonCorrectedMETProducer;
DEFINE_FWK_MODULE(PhotonCorrectedMETProducer);

typedef HadronicRecoilProducerT<pat::Tau> TauCorrectedMETProducer;
DEFINE_FWK_MODULE(TauCorrectedMETProducer);

typedef HadronicRecoilProducerT<pat::Jet> JetCorrectedMETProducer;
DEFINE_FWK_MODULE(JetCorrectedMETProducer);
