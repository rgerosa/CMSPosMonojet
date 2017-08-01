#ifndef TEMPLATEUTILS_H
#define TEMPLATEUTILS_H

#include <vector>
#include <map>
#include "TH1F.h"

enum class Region   { signal, gammajets, wmunu, zmumu, wenu, zee};
enum class Category { monojet, monoV};

// observables                                                                                                                                                                                    
map<string,vector<double> > observables_monoV;
map<string,vector<double> > observables_monoJ;

// binning selections for a given observable                                                                                                                                                                     
vector<double> selectBinning (const string & observable, const Category & category){

  if(category == Category::monojet)
    return observables_monoJ[observable];
  else if(category == Category::monoV)
    return observables_monoV[observable];
  else{
    vector<double> dummy;
    return dummy;
  }
}

void initializeObservableBinning(){

  // met distribution
  observables_monoV["met"]    = {250.,350.,500.,650.,1000.};
  observables_monoJ["met"]    = {250.,300.,350.,400.,575.,650.,750,850.,1000.,1200.};
 
  // jet met dphi
  observables_monoV["jetmetdphi"] = {0,0.25,0.50,0.75,1.0,1.25,1.5,1.75,2.,2.25,2.5,2.75,3.14};
  observables_monoJ["jetmetdphi"] = {0,0.25,0.50,0.75,1.0,1.25,1.5,1.75,2.,2.25,2.5,2.75,3.14};

  // leading lepton pT
  observables_monoJ["mu1pt"] = {20.,40.,60.,80.,100,125,150,200,250,300,400,600};
  observables_monoJ["el1pt"] = {20.,40.,60.,80.,100,125,150,200,250,300,400,600};
  observables_monoV["mu1pt"] = {20.,40.,60.,80.,100,125,150,200,250,300,400,600};
  observables_monoV["el1pt"] = {20.,40.,60.,80.,100,125,150,200,250,300,400,600};

  // leading lepton eta
  observables_monoJ["mu1eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  observables_monoV["mu1eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  observables_monoJ["el1eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  observables_monoV["el1eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};

  // trailing lepton pT
  observables_monoJ["mu2pt"] = {10.,20.,30.,40.,60.,80.,100,125,150,200,250};
  observables_monoJ["el2pt"] = {10.,20.,30.,40.,60.,80.,100,125,150,200,250};
  observables_monoV["mu2pt"] = {10.,20.,30.,40.,60.,80.,100,125,150,200,250};
  observables_monoV["el2pt"] = {10.,20.,30.,40.,60.,80.,100,125,150,200,250};

  // trailing lepton eta
  observables_monoJ["mu2eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  observables_monoV["mu2eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  observables_monoV["el2eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  observables_monoJ["el2eta"]  = {-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5};
  
  //W transverse mass
  observables_monoJ["wmnmt"] = {0,10,20,30,40,50,60,70,80,90,100,110,120,140,180,200};
  observables_monoJ["wenmt"] = {0,10,20,30,40,50,60,70,80,90,100,110,120,140,180,200};
  observables_monoV["wmnmt"] = {0,15,30,45,60,75,90,105,120,135,160,200};
  observables_monoV["wenmt"] = {0,15,30,45,60,75,90,105,120,135,160,200};

  // number of jets
  observables_monoJ["njet"]  = {0.,1.,2.,3.,4.,5.,6.,7.};
  observables_monoV["njet"]  = {0.,1.,2.,3.,4.,5.,6.,7.};

  // jet pt and eta
  observables_monoJ["jetpt"] =  {100,140,180,210,240,270,300,340,380,430,480,550,650,750};
  observables_monoJ["jeteta"] =   {-4.7,-4,-3.3,-2.7,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.7,3.3,4,4.7};

  // boson pT i.e. Z-boson and photon
  observables_monoJ["bosonpt"]  = {0,50,100,150,210,260,320,390,470,550,645,745,845,975,1100,1250};
  
  //V-jet quantities
  observables_monoV["tau2tau1"] = {0.,0.05,0.1,0.15,0.2,0.25,0.3,0.4,0.45,0.5};
  observables_monoV["msoftdrop"] = {65.,67.5,70.,72.5,75.,77.5,80.,82.5,85.,87.5,90.,92.5,95.,97.5,100.,102.5,105.};
  observables_monoV["vjetpt"] = {250.,300.,350.,400.,500.,600.,1000.};

}

#endif
