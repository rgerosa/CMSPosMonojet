#ifndef SelectMonojetAnalysisObjects_H
#define SelectMonojetAnalysisObjects_H

#include <memory>
#include <vector>
#include <iostream>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"


class SelectMonojetAnalysisObjects : public edm::stream::EDProducer<> {
 public:
  explicit SelectMonojetAnalysisObjects(const edm::ParameterSet&);
  ~SelectMonojetAnalysisObjects();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

 private:
  virtual void beginJob();
  virtual void produce(edm::Event&, const edm::EventSetup&) override;
  virtual void endJob();

  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // vertex collection                                                                                                                                                                                 
  const edm::EDGetTokenT<std::vector<reco::Vertex> >  verticesToken;

  // Muon collection and parameter set
  const edm::EDGetTokenT<std::vector<pat::Muon> > muonsToken;
  const std::vector<edm::ParameterSet > muonSelection;

  // Electron collection and parameter set
  const edm::EDGetTokenT<std::vector<pat::Electron> > electronsToken;
  const std::vector<edm::ParameterSet > electronSelection;
  std::vector<edm::EDGetTokenT<edm::ValueMap<bool> > > electronIdMapToken;

  // Photon collection and parameter set                                                                                                                                                         
  const edm::EDGetTokenT<std::vector<pat::Photon> > photonsToken;
  std::vector<edm::EDGetTokenT<edm::ValueMap<bool> > > photonIdMapToken;
  const std::vector<edm::ParameterSet > photonSelection;

  // Tau collection and parameter set
  const edm::EDGetTokenT<std::vector<pat::Tau> >  tausToken;
  const std::vector<edm::ParameterSet > tauSelection;

  // AK8 jets input collection                                                                                                                                                                     
  const edm::EDGetTokenT<std::vector<pat::Jet> >  jetsAK8Token;
  const std::vector<edm::ParameterSet > jetAK8Selection;

  int looseMuonPosition;
  int vetoElectronPosition;
  int loosePhotonPosition;

};

#endif
