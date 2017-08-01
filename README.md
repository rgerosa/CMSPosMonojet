**CMSPosMonojet analysis package**

Simplified framework to run a toy monojet/mono-V(had) analysis on MC-only.

**How to install**

Recipe to install and compile the package targetting 80X release cycles:

```
cmsrel CMSSW_8_0_29;
cd CMSSW_8_0_29/src;
cmsenv;
git-cms-init;
git cms-merge-topic -u ikrav:egm_id_80X_v2;
git cms-merge-topic -u ikrav:egm_id_80X_v3_photons;
git clone https://github.com/rgerosa/CMSPosMonojet.git MonojetFramework/CMSPosMonojet
```

**Ho to run ntuples production**

to produce a flat tree containing all the relevant variables used in the analysis:

```
cd MonojetFramework/CMSPosMonojet/test;
cmsRun makeAnalysisNtuples.py <options> inputFiles=<file to run on>
```

As example, from an lxplus at CERN one can do:

```
cmsRun makeAnalysisNtuples.py <options>	inputFiles=/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/F6C965C9-8AD4-E611-9026-02163E00AFA7.root```
```

Reading directly a MC file from CERN EOS (T2_CERN_CH)

The list of available running options are:

```
a) inputFiles: list of files to run on

b) maxEvents : how many events you want to process from the input file/files

c) globalTag : specify an input global tag (not really useful in this analysis)

d) outputFileName : specify the name of the output root file containing the flat trees and created via TFileService

e) triggerName: specify the processName for the triggerResults collection, which can vary depending on the miniAOD version

f) metRecoilCut: specify which threshold (in GeV) to select events according to the MET/Recoil of the event

g) filterOnHLT: if set to true, require a certain set of triggers on the MC, to preselect events falling in CRs/SR.

h) crossSection: set the cross section of the sample (in pb)
```

**Ho to	submit crab jobs fot ntuples production**

one can run over a full MC sample published on DAS via crab3. To run the makeAnalysisNtuples.py via crab3, one can use the submitCrabJobs.py scripts following these instructions:

```
cd MonojetFramework/CMSPosMonojet/test/crab;
python submitCrabJobs.py <Sample list> <crab configuration file>
```

List of samples are reported in the SampleList_MC_80X_Summer16/ directory, while the crab3 config example is config_MC_80X.py. One needs to modify it to correctly set the stageout site and area. Example:

```
python submitCrabJobs.py SampleList_MC_80X_Summer16/SampleList_MC_DYJets.py config_MC_80X.py
```


