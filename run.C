#define CaloJetTree_cxx
#include "CaloJetTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

#include "SimhitCorrData.h"

TString FileName = "/afs/cern.ch/user/z/zhipeng/public/TTree_HCAL_Root/SinglePion_E-50_TTree.root";
//TString FileName = "../HCAL_MET_res/SinglePion_E-300_TTree.root";
//const int MaxEvents = 300000;
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

    Int_t nCaloJetVec = CaloJetVec_Energy->size();
    SimhitCorrDatum datum[nCaloJetVec];
    for(Int_t i=0; i<nCaloJetVec; i++) {
      // currently Truth is a duplicate of Gen.
      // using tracker P as Truth under development
      datum[i].setTruthE(GenJetVec_Energy->at(i));
      datum[i].setGenJetEnergy(GenJetVec_Energy->at(i));
      datum[i].setGenJetEta(GenJetVec_Eta->at(i));
      datum[i].setGenJetPhi(GenJetVec_Phi->at(i));
      datum[i].setCaloJetEnergy(CaloJetVec_Energy->at(i));
      datum[i].setCaloJetEta(CaloJetVec_Eta->at(i));
      datum[i].setCaloJetPhi(CaloJetVec_Phi->at(i));
      //std::cout << "CaloJet" << i << ": " << CaloJetVec_Energy->at(i) << std::endl;
    }
    
    for(unsigned long i=0; i<CaloJetVec_CaloConstituentsVec_Index->size(); i++) {
      int index=CaloJetVec_CaloConstituentsVec_Index->at(i);
      datum[index].addOtherE(CaloJetVec_CaloConstituentsVec_EmEnergy->at(i));
      datum[index].addOtherE(CaloJetVec_CaloConstituentsVec_HFEnergy->at(i));
      datum[index].addOtherE(CaloJetVec_CaloConstituentsVec_HOEnergy->at(i));
      //std::cout << "CaloJet" << index << ": CaloCons" << i << ": "
      //          << CaloJetVec_CaloConstituentsVec_HadEnergy->at(i) << std::endl;
    }

    for(unsigned long i=0; i<CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index->size(); i++) {
      int twrindex=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index->at(i);
      int index=CaloJetVec_CaloConstituentsVec_Index->at(twrindex);
      int ieta=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta->at(i);
      int iphi=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi->at(i);
      int depth=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth->at(i);
      double energy=CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy->at(i);
      datum[index].addHcalE(ieta, iphi, depth, energy);
      //std::cout << "CaloJet" << index << ": CaloCons" << twrindex << ": Channel" << i << ": "
      //          << CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy->at(i) << std::endl;
    }
    
    for(Int_t i=0; i<nCaloJetVec; i++)
      if(datum[i].getOtherE()<datum[i].getTruthE()*0.05) data.addDatum(datum[i]);
  }

  RespCorr rc=data.doFit();
  TFile *rootfile= new TFile("results_temp/respCorr.root", "RECREATE");
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

  data.ClosureTestPrint(10);
  auto HistVec = data.ClosureTestDraw();

  TFile *ClosureTestRoot= new TFile("results_temp/ClosureTest.root", "RECREATE");
  ClosureTestRoot->cd();
  for(auto Hist : HistVec) Hist->Write();
  ClosureTestRoot->Close();

  TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
  for(auto Hist : HistVec) {
    Hist->Draw();
    TString HistName = Hist->GetTitle();
    HistName = "plots_temp/" + HistName + ".png";
    mycanvas->SaveAs(HistName);
  }

  return;
}
