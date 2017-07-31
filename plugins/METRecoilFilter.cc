#include <memory>
#include <vector>
#include <string>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/PatCandidates/interface/MET.h"


template<class T>
class METRecoilFilterT : public edm::stream::EDFilter<> {

public:
  explicit METRecoilFilterT(const edm::ParameterSet&);
  ~METRecoilFilterT();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  
  virtual bool filter(edm::Event&, const edm::EventSetup&) ;        
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override; 
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override; 
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  
  std::vector<edm::InputTag> metCollections;
  const double metRecoilCut;
  const bool graterThan;
  const bool applyAndInsteadOfOr;  
  std::vector<edm::EDGetTokenT<T> > metCollectionsToken;
};

template<class T>
METRecoilFilterT<T>::METRecoilFilterT(const edm::ParameterSet& iConfig):
  metCollections(iConfig.getParameter<std::vector<edm::InputTag> >("metCollections")),
  metRecoilCut(iConfig.getParameter<double>("metRecoilCut")),
  graterThan(iConfig.existsAs<bool>("graterThan") ? iConfig.getParameter<bool>("graterThan") : true),
  applyAndInsteadOfOr(iConfig.existsAs<bool>("applyAndInsteadOfOr") ? iConfig.getParameter<bool>("applyAndInsteadOfOr") : false)
{

  // loop on the met collection info to extract the collection tag and thresholds
  for(auto it = metCollections.begin(); it != metCollections.end(); ++it)
    metCollectionsToken.push_back(consumes<T>(*it));
}

template<class T>
METRecoilFilterT<T>::~METRecoilFilterT() {
  metCollections.clear();
  metCollectionsToken.clear();
}

template<class T>
bool METRecoilFilterT<T>::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  bool eventAccepted = false;
  // get by token
  edm::Handle<T> metCollectionsH;
  for(auto token : metCollectionsToken){ // loop on the met collection
    iEvent.getByToken(token,metCollectionsH); // take the met    
    for(auto met : *metCollectionsH){
      if(graterThan){
	if(not applyAndInsteadOfOr and met.pt() > metRecoilCut)
	  eventAccepted += true;
	else if(applyAndInsteadOfOr and met.pt() > metRecoilCut)
	  eventAccepted *= true;
      }
      else{
	if(not applyAndInsteadOfOr and met.pt() < metRecoilCut)
	  eventAccepted += true;
	else if(applyAndInsteadOfOr and met.pt() < metRecoilCut)
	  eventAccepted *= true;
      }
    }
  }
  return eventAccepted;

}

template<class T>
void METRecoilFilterT<T>::beginRun(edm::Run const& iRun , edm::EventSetup const& iSetup) {}

template<class T>
void METRecoilFilterT<T>::endRun(edm::Run const&, edm::EventSetup const&) {}

template<class T>
void METRecoilFilterT<T>::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

template<class T>
void METRecoilFilterT<T>::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

template<class T>
void METRecoilFilterT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

// specification
typedef METRecoilFilterT<pat::METCollection> METRecoilFilter;
DEFINE_FWK_MODULE(METRecoilFilter);

typedef METRecoilFilterT<reco::METCollection> RecoMETRecoilFilter;
DEFINE_FWK_MODULE(RecoMETRecoilFilter);
