import os, re
import sys
import FWCore.ParameterSet.Config as cms

### CMSSW command line parameter parser
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('python')

### recoil/MET cut
options.register (
    'metRecoilCut',250.,VarParsing.multiplicity.singleton,VarParsing.varType.float,
    'met/recoil cut to be applied in the analysis');

### HLT filter options
options.register (
    'filterOnHLT',True,VarParsing.multiplicity.singleton,VarParsing.varType.bool,
    'flag to indicate if apply or not trigger requirements');

### specific to produce trees for trigger studies
options.register (
    'triggerName','HLT',VarParsing.multiplicity.singleton,VarParsing.varType.string,
    'process name used for miniAOD production (target is miniAODv2)');

### GT to be used    
options.register (
    'globalTag','80X_mcRun2_asymptotic_2016_TrancheIV_v8'## Define the CMSSW process
    ,VarParsing.multiplicity.singleton,VarParsing.varType.string,
    'gloabl tag to be uses');

### input cross section in case you want to store a different value wrt to the LHE file
options.register(
    'crossSection',-1.,VarParsing.multiplicity.singleton, VarParsing.varType.float,
    'external value for sample cross section, in case of data it is fixed to 0.001');

### input cross section in case you want to store a different value wrt to the LHE file
options.register(
    'outputFileName',"tree.root",VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'output name for the root file containing the ntuple');

### parsing command line arguments
options.parseArguments()

### Define the CMSSW process
process = cms.Process("MonojetDemo")

### Load some standard set of configuration modules
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

### Message Logger settings
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 200

process.options = cms.untracked.PSet(
    allowUnscheduled = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(True))

### source file
process.source = cms.Source("PoolSource", 
                            fileNames = cms.untracked.vstring(options.inputFiles))



### output file
process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string(options.outputFileName))

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.maxEvents))

### Set the global tag depending on the sample type
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.globaltag = options.globalTag  

### Run Electron-ID through VID tool --> maintained by E-gamma
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
dataFormat = DataFormat.MiniAOD
switchOnVIDElectronIdProducer(process, dataFormat);
ele_id_modules = [];
ele_id_modules.append('RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff');
for idmod in ele_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

### Run Photon-ID through VID tool --> maintained by E-gamma
switchOnVIDPhotonIdProducer(process, dataFormat);
ph_id_modules = []
ph_id_modules.append('RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff')
for idmod in ph_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)

process.goodVertices = cms.EDFilter("VertexSelector",
                                    src = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                    cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2"),
                                    filter = cms.bool(True))


### apply a selection on a set of given trigger paths in MC
process.filterHLTEvents = cms.EDFilter("HLTCheckFilter",
                                       triggerResults = cms.InputTag("TriggerResults","","HLT"),
                                       triggerPaths = cms.vstring(
        "HLT_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight_v",
        "HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_v",
        "HLT_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight_v",
        "HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v",
        "HLT_PFMET90_PFMHT90_IDTight_v",
        "HLT_PFMET100_PFMHT100_IDTight_v",
        "HLT_PFMET110_PFMHT110_IDTight_v",
        "HLT_PFMET120_PFMHT120_IDTight_v",
        "HLT_PFMET170_v",
        "HLT_PFMET170_NoiseCleaned_v",
        "HLT_PFMET170_JetIdCleaned_v",
        "HLT_PFMET170_HBHECleaned_v",
        "HLT_PFMET300_v",
        "HLT_MonoCentralPFJet80_PFMETNoMu90_PFMHTNoMu90_IDTight_v",
        "HLT_MonoCentralPFJet80_PFMETNoMu120_PFMHTNoMu120_IDTight_v",
        "HLT_MonoCentralPFJet80_PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight_v",
        "HLT_MonoCentralPFJet80_PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight_v",
        "HLT_Photon165_HE10_v",
        "HLT_Photon175_v",
        "HLT_Ele27_WPLoose_Gsf_v",
        "HLT_Ele27_eta2p1_WPLoose_Gsf_v",
        "HLT_Ele27_eta2p1_WPTight_Gsf_v",
        "HLT_Ele105_CaloIdVT_GsfTrkIdT_v")
                                       )
                                         


### Use the private plugin to produce a set of object collections i.e.: loose/tight muons, veto/tight electrons, loose taus, loose/medium photons, V-jet candidates
process.selectedObjects = cms.EDProducer("SelectMonojetAnalysisObjects",
                                         ### vertex collection
                                         vertices  = cms.InputTag("goodVertices"),
                                         #### rho
                                         rho       = cms.InputTag("fixedGridRhoFastjetAll"),
                                         ### muon information
                                         muons     = cms.InputTag("slimmedMuons"),
                                         muonSelection = cms.VPSet(
        #### loose muons
        cms.PSet(
            idType = cms.string("loose"),
            muonCollectionName = cms.string("looseMuons"),
            ptMin     = cms.double(10),
            absEta    = cms.double(2.4),
            isolation = cms.double(0.25)),
        #### tight muons
        cms.PSet(
            idType = cms.string("tight"),
            muonCollectionName = cms.string("tightMuons"),
            ptMin     = cms.double(10),
            absEta    = cms.double(2.4),
            isolation = cms.double(0.15)),    
        ),
                                         ### electron information
                                         electrons = cms.InputTag("slimmedElectrons"),
                                         electronSelection = cms.VPSet(
        ### veto electrons
        cms.PSet(
            idType = cms.string("veto"),
            electronCollectionName = cms.string("vetoElectrons"),
            ptMin  = cms.double(10),
            absEta = cms.double(2.5),
            PVSelection = cms.PSet(   
                d0Barrel = cms.double(0.05),
                d0Endcap = cms.double(0.10),
                dzBarrel = cms.double(0.10),
                dzEndcap = cms.double(0.20),
                ),
            eleValueMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto")),
        ### tight electrons
        cms.PSet(
            idType = cms.string("tight"),
            electronCollectionName = cms.string("tightElectrons"),
            ptMin  = cms.double(10),
            absEta = cms.double(2.5),
            PVSelection = cms.PSet(   
                d0Barrel = cms.double(0.05),
                d0Endcap = cms.double(0.10),
                dzBarrel = cms.double(0.10),
                dzEndcap = cms.double(0.20),
                ),            
            eleValueMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight")),
        ),
                                         ### taus
                                         taus      = cms.InputTag("slimmedTaus"),
                                         tauSelection = cms.VPSet(
        cms.PSet(
            #### very loose MVA with new decay mode
            tauCollectionName = cms.string("looseTaus"),
            dRCleaning = cms.double(0.4),
            tauIDName  = cms.string("byVLooseIsolationMVArun2v1DBoldDMwLT"),
            ptMin  = cms.double(18),
            absEta = cms.double(2.3),
            useOldDecayMode  = cms.bool(False)
            )
        ),
                                         ## photons
                                         photons   = cms.InputTag("slimmedPhotons"),
                                         photonSelection = cms.VPSet(
        ### loose photons
        cms.PSet( 
            idType = cms.string("loose"),
            photonCollectionName = cms.string("loosePhotons"),
            ptMin  = cms.double(15),
            absEta = cms.double(2.5),
            photonValueMap = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-loose")),
        ##### medium photons
        cms.PSet(
            idType = cms.string("medium"),
            photonCollectionName = cms.string("mediumPhotons"),
            ptMin  = cms.double(15),
            absEta = cms.double(2.5),
            photonValueMap = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-medium"))
        ),       
                                         ## V-jets
                                         ak8Jets  = cms.InputTag("slimmedJetsAK8"),
                                         vtaggingSelection = cms.VPSet(
        ### standard tagging
        cms.PSet(
            vjetCollectionName = cms.string("puppiVjetCandidates"),
            dRCleaning = cms.double(0.8),
            ptMin  = cms.double (250),
            absEta = cms.double(2.4),
            minMass = cms.double(65),
            maxMass = cms.double(105),
            tau2tau1 = cms.double(0.5),
            usePuppi = cms.bool(True))          
        )
                                         )

### Compute the hadronic recoil
process.metNoMuon = cms.EDProducer("MuonCorrectedMETProducer",
                                   metCollection = cms.InputTag("slimmedMETs"),
                                   candToSubtract = cms.VInputTag(cms.InputTag("selectedObjects", "looseMuons")),
                                   pfCandidates  = cms.InputTag("packedPFCandidates"))

process.metNoElectron = cms.EDProducer("ElectronCorrectedMETProducer",
                                       metCollection = cms.InputTag("slimmedMETs"),
                                       candToSubtract = cms.VInputTag(cms.InputTag("selectedObjects", "vetoElectrons")),
                                       pfCandidates  = cms.InputTag("packedPFCandidates"))

process.metNoPhoton = cms.EDProducer("PhotonCorrectedMETProducer",
                                     metCollection = cms.InputTag("slimmedMETs"),
                                     candToSubtract = cms.VInputTag(cms.InputTag("selectedObjects", "loosePhotons")),
                                     pfCandidates  = cms.InputTag("packedPFCandidates"))


### apply a selection on a logic OR between recoil and missing energy
process.filterHighMETEvents = cms.EDFilter("METRecoilFilter",
                                          metCollections = cms.VInputTag(
            cms.InputTag("slimmedMETs"),
            cms.InputTag("metNoMuon"),
            cms.InputTag("metNoElectron"),
            cms.InputTag("metNoPhoton")),
                                          metRecoilCut = cms.double(options.metRecoilCut),
                                          graterThan   = cms.bool(True),
                                          applyAndInsteadOfOr = cms.bool(False))


            
            

### EDAnalyzer to create the ultimate flat TTree with the informations
process.tree = cms.EDAnalyzer("MonoJetTreeMaker",
                              pileupEventInfo = cms.InputTag("slimmedAddPileupInfo"),
                              genEventInfo = cms.InputTag("generator"),
                              genParticles = cms.InputTag("prunedGenParticles"),
                              xsec = cms.double(options.crossSection), ## sample cross section provided as external option
                              triggerResults = cms.InputTag("TriggerResults", "",options.triggerName),
                              vertices = cms.InputTag("goodVertices"),
                              ####
                              looseMuons = cms.InputTag("selectedObjects","looseMuons"),
                              tightMuons = cms.InputTag("selectedObjects","tightMuons"),
                              ####
                              vetoElectrons = cms.InputTag("selectedObjects","vetoElectrons"),
                              tightElectrons = cms.InputTag("selectedObjects","tightElectrons"),
                              ####                                 
                              loosePhotons = cms.InputTag("selectedObjects","loosePhotons"),
                              mediumPhotons = cms.InputTag("selectedObjects","mediumPhotons"),
                              ####
                              looseTaus = cms.InputTag("selectedObjects","looseTaus"),
                              ####
                              jetsAK4 = cms.InputTag("slimmedJets"),
                              ####
                              vJetsAK8    = cms.InputTag("selectedObjects","puppiVjetCandidates"),
                              isPuppiVJet = cms.bool(True),
                              ####
                              t1PFMET = cms.InputTag("slimmedMETs"),
                              t1PFMETNoMuon = cms.InputTag("metNoMuon"),
                              t1PFMETNoElectron = cms.InputTag("metNoElectron"),
                              t1PFMETNoPhoton = cms.InputTag("metNoPhoton"),
                              ####
                              cleanMuonJet = cms.bool(True),
                              cleanElectronJet = cms.bool(True),
                              cleanPhotonJet = cms.bool(True),
                              ####
                              dRCleaningAK4 = cms.double(0.4),
                              dRCleaningAK8 = cms.double(0.8),
                              ####
                              btaggingCSVWP = cms.double(0.8484),
                              minJetPtCountAK4 = cms.double(30),
                              minJetPtBveto = cms.double(20)
                              )

### EDAnalyzer to create a TTree with gen-informations prior to any selections
process.gentree = cms.EDAnalyzer("MonoJetGenTreeMaker",
                                 pileupEventInfo = cms.InputTag("slimmedAddPileupInfo"),
                                 genEventInfo = cms.InputTag("generator"),
                                 )


### task for unscheduled mode
if options.filterOnHLT:
    process.analysisPath = cms.Path(process.gentree+ ## to be run as a first step
                                    process.goodVertices+
                                    process.filterHLTEvents+
                                    process.filterHighMETEvents+
                                    process.tree)
else:

    process.analysisPath = cms.Path(process.gentree+ ## to be run as a first step
                                    process.goodVertices+
                                    process.filterHighMETEvents+
                                    process.tree)


