#include "templateUtils.h"

void makeHistograms(vector<TH1F*> histograms, // input histograms
		    TTree* tree, // input tree
		    const Region & region, // to tell which control region
		    const Category & category, // monojet or monoV analysis
		    const float & luminosity){


  // to loop over the input trees
  TTreeReader reader(tree);

  TTreeReaderValue<float> xsec (reader,"xsec");
  TTreeReaderValue<float> wgt (reader,"wgt");
  TTreeReaderValue<double> wgtsum (reader,"wgtsum");

  TTreeReaderValue<UChar_t> passingHLTPFMet (reader,"passingHLTPFMet");
  TTreeReaderValue<UChar_t> passingHLTPFMetNoMu (reader,"passingHLTPFMetNoMu");
  TTreeReaderValue<UChar_t> passingHLTPFMetMonojet (reader,"passingHLTPFMetMonojet");
  TTreeReaderValue<UChar_t> passingHLTPhoton (reader,"passingHLTPhoton");
  TTreeReaderValue<UChar_t> passingHLTElectron (reader,"passingHLTElectron");

  TTreeReaderValue<unsigned int> nmuons (reader,"nmuons");
  TTreeReaderValue<unsigned int> nelectrons (reader,"nelectrons");
  TTreeReaderValue<unsigned int> nphotons  (reader,"nphotons");
  TTreeReaderValue<unsigned int> ntaus (reader,"ntaus");
  TTreeReaderValue<unsigned int> njets (reader,"njets");
  TTreeReaderValue<unsigned int> nbjets (reader,"nbjets");

  TTreeReaderValue<float> t1met (reader,"t1met");
  TTreeReaderValue<float> t1metnomu (reader,"t1metnomu");
  TTreeReaderValue<float> t1metnoele (reader,"t1metnoele");
  TTreeReaderValue<float> t1metnopho (reader,"t1metnopho");

  TTreeReaderValue<vector<float> > jetpt (reader,"jetpt");
  TTreeReaderValue<vector<float> > jeteta (reader,"jeteta");
  TTreeReaderValue<vector<float> > jetchfrac (reader,"jetchfrac");
  TTreeReaderValue<vector<float> > jetnhfrac (reader,"jetnhfrac");

  TTreeReaderValue<vector<float> > vjetpt (reader,"vjetpt");
  TTreeReaderValue<vector<float> > vjeteta (reader,"vjeteta");
  TTreeReaderValue<vector<float> > vjettau2 (reader,"vjettau2");
  TTreeReaderValue<vector<float> > vjettau1 (reader,"vjettau1");
  TTreeReaderValue<vector<float> > vjetsoftdropm (reader,"vjetsoftdropm");

  TTreeReaderValue<float> mindphijetmet (reader,"mindphijetmet");
  TTreeReaderValue<float> mindphijetmetnomu (reader,"mindphijetmetnomu");
  TTreeReaderValue<float> mindphijetmetnoele (reader,"mindphijetmetnoele");
  TTreeReaderValue<float> mindphijetmetnopho (reader,"mindphijetmetnopho");

  TTreeReaderValue<int> mu1pid (reader,"mu1pid");
  TTreeReaderValue<int> mu2pid (reader,"mu2pid");
  TTreeReaderValue<int> mu1id (reader,"mu1id");
  TTreeReaderValue<int> mu2id (reader,"mu2id");

  TTreeReaderValue<int> el1pid (reader,"el1pid");
  TTreeReaderValue<int> el2pid (reader,"el2pid");
  TTreeReaderValue<int> el1id (reader,"el1id");
  TTreeReaderValue<int> el2id (reader,"el2id");

  TTreeReaderValue<float> mu1pt (reader,"mu1pt");
  TTreeReaderValue<float> mu2pt (reader,"mu2pt");
  TTreeReaderValue<float> mu1eta (reader,"mu1eta");
  TTreeReaderValue<float> mu2eta (reader,"mu2eta");

  TTreeReaderValue<float> el1pt (reader,"el1pt");
  TTreeReaderValue<float> el2pt (reader,"el2pt");
  TTreeReaderValue<float> el1eta (reader,"el1eta");
  TTreeReaderValue<float> el2eta (reader,"el2eta");

  TTreeReaderValue<float> phpt (reader,"phpt");
  TTreeReaderValue<float> pheta (reader,"pheta");
  TTreeReaderValue<int> phid (reader,"phid");

  TTreeReaderValue<float> zmmmass (reader,"zmmmass");
  TTreeReaderValue<float> zeemass (reader,"zeemass");
  TTreeReaderValue<float> zmmpt (reader,"zmmpt");
  TTreeReaderValue<float> zeept (reader,"zeept");
  TTreeReaderValue<float> wmnmt (reader,"wmnmt");
  TTreeReaderValue<float> wenmt (reader,"wenmt");
  
  while(reader.Next()){

    // ask the trigger
    int hlt = 0;
    if(region == Region::signal or region == Region::wmunu or region == Region::zmumu)
      hlt = *passingHLTPFMet + *passingHLTPFMetNoMu + *passingHLTPFMetMonojet;
    else if(region == Region::wenu or region == Region::zee)      
      hlt = *passingHLTElectron;
    else if(region == Region::gamma)
      hlt = *passingHLTPhoton;      
    if(hlt == 0) continue;

    //veto b-jets
    if(*nbjets > 0) continue;
    //ask at least one jet
    if(*njets == 0) continue;
    // veto taus
    if(*ntaus > 0) continue;

    // specific selections
    if(region == Region::signal){
      if(*nmuons > 0) continue; // veto muons
      if(*nelectrons > 0) continue; // veto electrons
      if(*nphotons > 0) continue; // veto photons 
      if(*t1met < 250) continue;  // met cut
      if(*mindphijetmet < 0.5) continue; //min dphi jet met
    }
    else if(region == Region::gamma){
      if(*nmuons > 0) continue; // veto muons                                                                                                                                                      
      if(*nelectrons > 0) continue; // veto electrons                                                                                                                                               
      if(*nphotons != 1) continue; //only one photon
      if(*phid != 1) continue; // medium id
      if(*t1metnopho < 250) continue; // recoil cut
      if(*mindphijetmetnopho < 0.5) continue; // min dphi jet recoil     
      if(*phpt < 175) continue;
      if(fabs(*pheta) > 1.5) continue;
    }
    else if(region == Region::wmunu){
      if(*nmuons != 1) continue; // exactly one muon                                                                                                                                             
      if(*nelectrons > 0) continue; // veto electrons                                                                                                                                               
      if(*nphotons > 0) continue; // veto photons
      if(*mu1id != 1) continue; // tight muon
      if(*mu1pt < 20) continue;
      if(fabs(*mu1eta) > 2.4) continue;
      if(*t1metnomu < 250) continue; // recoil cut
      if(*mindphijetmetnomu < 0.5) continue; // min dphi jet recoil      
    }
    else if(region == Region::wenu){
      if(*nmuons > 0) continue; // veto muons                                                                                                                                                      
      if(*nelectrons != 1) continue; // exactly one electron                                                                                                                                  
      if(*nphotons > 0) continue; // vetp photons
      if(*el1id != 1) continue; // tight id
      if(*el1pt < 40) continue; // pt cut
      if(fabs(*el1eta) > 2.5) continue;
      if(*t1metnoele < 250) continue; // recoil cut
      if(*mindphijetmetnoele< 0.5) continue; // min dphi jet recoil
      if(*t1met < 50) continue; // met cut to reduce QCD
    }
    else if(region == Region::zmumu){
      if(*nmuons != 2) continue; // exactly two muons                                                                                                                                         
      if(*nelectrons > 0) continue; // veto electrons                                                                                                                                               
      if(*nphotons > 0) continue; //veto photons
      if(fabs(*mu1eta) > 2.4) continue;
      if(fabs(*mu2eta) > 2.4) continue;
      bool goodMuon = false;
      if(*mu1pt > 20 and *mu1id == 1) goodMuon = true;
      if(*mu2pt > 20 and *mu2id == 1) goodMuon = true;
      if(not goodMuon) continue;
      if(*mu1pid == *mu2pid) continue; // opposite charge
      if(*t1metnomu < 250) continue; // recoil cut
      if(*mindphijetmetnomu < 0.5) continue;  // min dphi jet recoil     
      if(*zmmmass < 60) continue;
      if(*zmmmass > 120) continue;
    }
    else if(region == Region::zee){
      if(*nmuons > 0) continue; // veto muons                                                                                                                                                      
      if(*nelectrons != 2) continue; // exactly two electrons                                                                                                                                 
      if(*nphotons > 0) continue; //veto photons
      if(fabs(*el1eta) > 2.5) continue;
      if(fabs(*el2eta) > 2.5) continue;
      bool goodElectron = false;
      if(*el1pt > 40 and *el1id == 1) goodElectron = true;
      if(*el2pt > 40 and *el2id == 1) goodElectron = true;
      if(not goodElectron) continue;
      if(*el1pid == *el2pid) continue; // opposite charge
      if(*t1metnoele < 250) continue; // recoil cut
      if(*mindphijetmetnoele < 0.5) continue; // min dphi jet recoil     
      if(*zeemass < 60) continue; 
      if(*zeemass > 120) continue;
    }

    // jet requirements in common between all regions
    if(fabs(jeteta->at(0)) > 2.5) continue; // ask one central jet
    if(fabs(jetpt->at(0)) < 100) continue; // ask central jet to have high pt
    if(fabs(jetchfrac->at(0)) < 0.1) continue; // charged hadron fraction
    if(fabs(jetnhfrac->at(0)) > 0.8) continue; // neutral hadron fraction      

    // mono-V category
    if(category == Category::monoV){
      if(vjetpt->size() == 0) continue;
      if(vjetpt->at(0) < 250) continue;
      if(fabs(vjeteta->at(0)) > 2.4) continue;
      if(vjettau2->at(0)/vjettau1->at(0) > 0.5) continue;
      if(vjetsoftdropm->at(0) < 65) continue;
      if(vjetsoftdropm->at(0) > 105) continue;
    }
    else if(category == Category::monojet){
      bool isGoodMonoJet = true;
      if(vjetpt->size() != 0 and vjetpt->at(0) > 250 and 
	 fabs(vjeteta->at(0)) < 2.4 and 
	 vjettau2->at(0)/vjettau1->at(0) < 0.5 and
	 vjetsoftdropm->at(0) > 65 and
	 vjetsoftdropm->at(0) < 105)
	isGoodMonoJet = false; // the event falls in the monoV category
      
      if(isGoodMonoJet == false) continue;
    }

    // event weight to weight to the luminosity
    float eventWeight = (*xsec)*luminosity*(*wgt)/(*wgtsum);

    /// fill histograms looping on the histogram list  
    float fillValue = 0;
    for(auto hist : histograms){
      TString name (hist->GetName());

      if(name.Contains("jetmetdphi") and region == Region::signal)
	fillValue = *mindphijetmet;
      else if(name.Contains("jetmetdphi") and (region == Region::wmunu or region == Region::zmumu))
	fillValue = *mindphijetmetnomu;
      else if(name.Contains("jetmetdphi") and (region == Region::wenu or region == Region::zee))
	fillValue = *mindphijetmetnoele;
      else if(name.Contains("jetmetdphi") and region == Region::gamma)
	fillValue = *mindphijetmetnopho;
      else if(name.Contains("mu1pt") and (region == Region::wmunu or region == Region::zmumu))
	fillValue = *mu1pt;
      else if(name.Contains("mu2pt") and region == Region::zmumu)
	fillValue = *mu2pt;
      else if(name.Contains("mu1eta") and (region == Region::wmunu or region == Region::zmumu))
	fillValue = *mu1eta;
      else if(name.Contains("mu2eta") and region == Region::zmumu)
	fillValue = *mu2eta;
      else if(name.Contains("el1pt") and (region == Region::wenu or region == Region::zee))
	fillValue = *el1pt;
      else if(name.Contains("el2pt") and region == Region::zee)
	fillValue = *el2pt;
      else if(name.Contains("el1eta") and (region == Region::wenu or region == Region::zee))
	fillValue = *el1eta;
      else if(name.Contains("el2eta") and region == Region::zee)
	fillValue = *el2eta;
      else if(name.Contains("wmnmt") and region == Region::wmunu)
	fillValue = *wmnmt;
      else if(name.Contains("wenmt") and region == Region::wenu)
	fillValue = *wenmt;
      else if(name.Contains("njet"))
	fillValue = *njets;
      else if(name.Contains("jetpt"))
	fillValue = jetpt->at(0);
      else if(name.Contains("jeteta"))
	fillValue = jeteta->at(0);
      else if(name.Contains("vjetpt") and category == Category::monoV and vjetpt->size() > 0)
	fillValue = vjetpt->at(0);
      else if(name.Contains("tau2tau1") and category == Category::monoV and vjetpt->size() > 0)
	fillValue = vjettau2->at(0)/vjettau1->at(0);
      else if(name.Contains("msoftdrop") and category == Category::monoV and vjetpt->size() > 0)
	fillValue = vjetsoftdropm->at(0);
      else if(name.Contains("zpt") and region == Region::zmumu)
	fillValue = *zmmpt;
      else if(name.Contains("zpt") and region == Region::zee)
	fillValue = *zeept;
      else if(name.Contains("photonpt") and region == Region::gamma)
	fillValue = *phpt;
      else if(name.Contains("photoneta") and region == Region::gamma)
	fillValue = *pheta;
      else if(name.Contains("met") and region == Region::signal)
	fillValue = *t1met;
      else if(name.Contains("zmmmass") and region == Region::zmumu)
	fillValue = *zmmmass;
      else if(name.Contains("zeemass") and region == Region::zee)
	fillValue = *zeemass;
      else if(name.Contains("met") and (region == Region::wmunu or region == Region::zmumu))
	fillValue = *t1metnomu;
      else if(name.Contains("met") and (region == Region::wenu or region == Region::zee))
	fillValue = *t1metnoele;
      else if(name.Contains("met") and region == Region::gamma)
	fillValue = *t1metnopho;

      // include the overflow
      if(fillValue > hist->GetBinLowEdge(hist->GetNbinsX()+1))
	fillValue = hist->GetBinLowEdge(hist->GetNbinsX()+1)-hist->GetBinWidth((hist->GetNbinsX())/2);
      
      hist->Fill(fillValue,eventWeight);
    } 
  }   
}
