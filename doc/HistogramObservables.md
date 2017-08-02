## List of observables ##

These are the variables known by the framework, for which you can produce histograms with a binning defined in templateUtils.h

| Variable      | Description                               | Region Used |  Category |
| ------------- | ------------------------------------------|-------------|-----------|                        
| met           | Missing energy or hadronic recoil         | SR, zmm, zee, wmn, wen, gamma | monojet, monoV |
| jetmetdphi    | Minimum d-phi between jets and MET/recoil | SR, zmm, zee, wmn, wen, gamma | monojet, monoV |
| mu1pt         | Leading muon transverse momentum          | zmm, wmn | monojet, monoV |
| el1pt         | Leading electron transverse momentum      | zee, wen | monojet, monoV |
| mu2pt         | Trailing muon transverse momentum         | zmm      | monojet, monoV |
| el2pt         | Trailing electron transverse momentum     | zee      | monojet, monoV |
| mu1eta        | Leading muon pseudo-rapidity              | zmm, wmn | monojet, monoV |
| el1eta        | Leading electron pseudo-rapidity          | zee, wen | monojet, monoV |
| mu2eta        | Trailing muon pseudo-rapidity             | zmm      | monojet, monoV |
| el2eta        | Trailing electron pseudo-rapidity         | zee      | monojet, monoV |
| wmnmt         | W-boson transverse mass                   | wmn      | monojet, monoV |
| wenmt         | W-boson transverse mass                   | wen      | monojet, monoV |
| zmmmass       | Z-boson invariant mass                    | zmm      | monojet, monoV |
| zeemass       | Z-boson invariant mass                    | zee      | monojet, monoV |
| njet          | Number of jets with pT > 30 GeV           | SR, zmm, zee, wmn, wen, gamma | monojet, monoV |
| jetpt         | Leading jet transverse momentum           | SR, zmm, zee, wmn, wen, gamma | monojet |
| jeteta        | Leading jet pseudo-rapidity               | SR, zmm, zee, wmn, wen, gamma | monojet |
| vjetpt        | Leading AK8 jet transverse momentum       | SR, zmm, zee, wmn, wen, gamma | monoV |
| tau2tau1      | Leading AK8 jet tau2tau1 ratio            | SR, zmm, zee, wmn, wen, gamma | monoV |
| msoftdrop     | Leading AK8 jet soft-drop mass            | SR, zmm, zee, wmn, wen, gamma | monoV |
| zpt           | Z-boson pT distribution                   | zmm, zee | monojet, monoV |
| photonpt      | photon pT distribution                    | gam | monojet, monoV |
| photoneta     | photon psuedo-rapidity distribution       | gam | monojet, monoV |

