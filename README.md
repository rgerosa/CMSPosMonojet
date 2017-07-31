Simplified framework to run a toy monojet/mono-V(had) analysis on MC-only. 

Recipe to install and compile the package targetting 80X release cycles:

       cmsrel CMSSW_8_0_29;
       cd CMSSW_8_0_29/src;
       cmsenv;
       git-cms-init;
       git cms-merge-topic -u ikrav:egm_id_80X_v2;
       git cms-merge-topic -u ikrav:egm_id_80X_v3_photons;
       git clone https://github.com/rgerosa/CMSPosMonojet.git MonojetFramework/CMSPosMonojet
