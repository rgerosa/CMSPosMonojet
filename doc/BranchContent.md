## Branch list for tree/tree ##

| Branch Name | Description |
|-------------|-------------|
| event  | event number |
| run    | run number |
| lumi   | lumi-section number |
| xsec   | cross section for the sample, stored in fb |
| wgt    | generator event weight. Usually 1 for LO events, but can be negative or postitive for NLO events example in aMC@NLO |
| putrue | simulated true pileup distribution |
| nvtx   | number of reconstructed primary vertex in the event passing the goodVertex selection |
|-------------|-------------|
| passingHLTPFMet        | when 1 means that the event has fired at least one of the HLTPFMET triggers |
| passingHLTPFMetNoMu    | when 1 means that the event has fired at least one of the HLTPFMETNoMu triggers |
| passingHLTPFMetMonojet | when 1 means that the event has fired at least one of the Monojet triggers |
| passingHLTPhoton       | when 1 means that the event has fired at least one of the single photon triggers (165_HR or 175) |
| passingHLTElectron     | when 1 means that the event has fired at least one of the single electron triggers |
|-------------|-------------|
| nmuons      | number of loose muons in the event |
| nelectrons  | number of veto electrons in the event |
| nphotons    | number of loose photons in the event |
| ntaus       | number of loose hadronic taus in the event |
| njets       | number of jets in the event with pt > 30 GeV (not overlapping with leptons) |
| nbjets      | number of b-jets in the event with pt > 20 GeV (not overlapping with leptons) |
|-------------|-------------|
| t1met       | missing energy 
| t1metnomu   | hadronic recoil computed subracting loose muons |
| t1metnoel   | hadronic recoil computed subracting veto electrons |
| t1metnopho  | hadronic recoil computed subracting loose photons |
|-------------|-------------|
| jetpt     | vector storing the pT of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
| jeteta    | vector storing the eta of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
| jetphi    | vector storing the phi of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
| jetm      | vector storing the mass of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
| jetbtag   | vector storing the CVS value of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
| jetchfrac | vector storing the charge hadron fraction of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
| jetnhfrac | vector storing the neutral hadron fraction of all the AK4 jets in the event with pT > 30 GeV, not overlapping with loose leptons |
|-------------|-------------|
| vjetpt    | vector storing the pT all the AK8 puppi jets passing the v-tagging selection |
| vjeteta   | vector storing the eta all the AK8 puppi jets passing the v-tagging selection |
| vjetphi   | vector storing the phi all the AK8 puppi jets passing the v-tagging selection |
| vjetm     | vector storing the raw mass all the AK8 puppi jets passing the v-tagging selection |
| vjettau1  | vector storing the 1-subjettiness all the AK8 puppi jets passing the v-tagging selection |
| vjettau2  | vector storing the 2-subjettiness all the AK8 puppi jets passing the v-tagging selection |
| vjetsoftdropm | vector storing the soft-drop mass all the AK8 puppi jets passing the v-tagging selection |
|-------------|-------------|
| mindphijetmet       | minimum delta|phi between AK4 jets with pT > 30 GeV and the missing-energy |
| mindphijetmetnomu   | minimum delta|phi between AK4 jets with pT > 30 GeV and the MET-no-muon |
| mindphijetmetnoele  | minimum delta|phi between AK4 jets with pT > 30 GeV and the MET-no-electron |
| mindphijetmetnopho  | minimum delta|phi between AK4 jets with pT > 30 GeV and the MET-no-photon |
|-------------|-------------|
| mu1pid | leading muon pdgId in the event (if any). It should at least pass the loose-ID |
| mu1pt  | leading muon pT in the event (if any). It should at least pass the loose-ID |
| mu1eta | leading muon eta in the event (if any). It should at least pass the loose-ID |
| mu1phi | leading muon phi in the event (if any). It should at least pass the loose-ID |
| mu1id  | leading muon ID in the event (if any). 0 if it just a loose muon, 1 if it passes the tight criterion |
| mu2pid | trailing muon pdgId in the event (if any). It should at least pass the loose-ID |
| mu2pt  | trailing muon pT in the event (if any). It should at least pass the loose-ID |
| mu2eta | trailing muon eta in the event (if any). It should at least pass the loose-ID |
| mu2phi | trailing muon phi in the event (if any). It should at least pass the loose-ID |
| mu2id  | trailing muon ID in the event (if any). 0 if it just a loose muon, 1 if it passes the tight criterion |
|-------------|-------------|
| el1pid | leading electron pdgId in the event (if any). It should at least pass the loose-ID |
| el1pt  | leading electron pT in the event (if any). It should at least pass the loose-ID |
| el1eta | leading electron eta in the event (if any). It should at least pass the loose-ID |
| el1phi | leading electron phi in the event (if any). It should at least pass the loose-ID |
| el1id  | leading electron ID in the event (if any). 0 if it just a veto electron, 1 if it passes the tight criterion |
| el2pid | trailing electron pdgId in the event (if any). It should at least pass the loose-ID |
| el2pt  | trailing electron pT in the event (if any). It should at least pass the loose-ID |
| el2eta | trailing electron eta in the event (if any). It should at least pass the loose-ID |
| el2phi | trailing electron phi in the event (if any). It should at least pass the loose-ID |
| el2id  | trailing electron ID flag in the event (if any). 0 if it just a veto electron, 1 if it passes the tight criterion |
|-------------|-------------|
| phpt   | leading photon pT in the event (if any) |
| pheta  | leading photon eta in the event (if any) |
| phphi  | leading photon phi in the event (if any) |
| phid   | leading photon ID flag in the event (if any). 0 if it just a loose electron, 1 if it passes the medium criterion |
|-------------|-------------|
| zmmmass | di-muon invariant mass when at least two identified muons are in the event |
| zeemass | di-electron invariant mass when at least two identified electrons are in the event |
|-------------|-------------|
| bosonid | pdgId of the leptonically decaying vector boson found at gen-level (useful for higher order corrections on Z+jets, W+jets and gamma+jets) |
| bosonpt | pT of the leptonically decaying vector boson found at gen-level (useful for higher order corrections on Z+jets, W+jets and gamma+jets) |
|-------------|-------------|
| mediatorid | pdgId of the dark-matter mediator in signal events |
| mediatorpt | pT of the dark-matter mediator in signal events |