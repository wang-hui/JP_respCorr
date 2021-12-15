#define CaloJetTree_cxx
#include "CaloJetTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

#include "SimhitCorrData.h"

TString FileName = "for_JP/UL_DoublePion_E-50_RAW_noPU_RECO_simHit_energy_TTree.root";
//const int MaxEvents = 500000;
const int MaxEvents = -1;

void run(void)
{
  CaloJetTree t(FileName);
  t.Loop();

  return;
}

void CaloJetTree::Loop()
{
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  SimhitCorrData data;
  
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    
    if(jentry%10000==0) std::cout << jentry << std::endl;
    if(MaxEvents > 0 && jentry > MaxEvents) break;
    
    SimhitCorrDatum datum[kMaxCaloJetVec_p4];
    for(Int_t i=0; i<kMaxCaloJetVec_p4; i++)
      datum[i].setTruthE(50.);
    
    for(unsigned long i=0; i<CaloJetVec_CaloConstituentsVec_Index->size(); i++) {
      int index=CaloJetVec_CaloConstituentsVec_Index->at(i);
      datum[index].addOtherE(CaloJetVec_CaloConstituentsVec_EmEnergy->at(i));
      datum[index].addOtherE(CaloJetVec_CaloConstituentsVec_HFEnergy->at(i));
      datum[index].addOtherE(CaloJetVec_CaloConstituentsVec_HOEnergy->at(i));
    }

    for(unsigned long i=0; i<CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index->size(); i++) {
      int twrindex=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index->at(i);
      int index=CaloJetVec_CaloConstituentsVec_Index->at(twrindex);
      int ieta=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta->at(i);
      int iphi=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi->at(i);
      int depth=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth->at(i);
      double energy=CaloJetVec_CaloConstituentsVec_HCALChannelVec_TruthEnergy->at(i);
      datum[index].addHcalE(ieta, iphi, depth, energy);
    }
    
    for(Int_t i=0; i<kMaxCaloJetVec_p4; i++)
      if(datum[i].getOtherE()<datum[i].getTruthE()*0.05) data.addDatum(datum[i]);
  }

  RespCorr rc=data.doFit();
  TFile *rootfile= new TFile("out.root", "RECREATE");
  TH1D* h1=rc.makeHist("D1","D1",1);
  TH1D* h2=rc.makeHist("D2","D2",2);
  TH1D* h3=rc.makeHist("D3","D3",3);
  TH1D* h4=rc.makeHist("D4","D4",4);
  TH1D* h5=rc.makeHist("D5","D5",5);
  TH1D* h6=rc.makeHist("D6","D6",6);
  TH1D* h7=rc.makeHist("D7","D7",7);

  rootfile->cd();
  h1->Write();
  h2->Write();
  h3->Write();
  h4->Write();
  h5->Write();
  h6->Write();
  h7->Write();
  rootfile->Close();
  
  return;
}
