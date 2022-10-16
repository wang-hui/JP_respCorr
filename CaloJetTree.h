//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Oct 16 10:00:35 2022 by ROOT version 6.14/09
// from TTree CaloJetTree/CaloJetTree
// found on file: for_JP/UL_DoublePion_E-50_RECO_PU_DLPHIN_class_no_respCorr_save_to_AUX_TTree.root
//////////////////////////////////////////////////////////

#ifndef CaloJetTree_h
#define CaloJetTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class CaloJetTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   TString         FileName;

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<float>   *GenJetVec_Energy;
   vector<float>   *GenJetVec_Eta;
   vector<float>   *GenJetVec_Phi;
   vector<float>   *CaloJetVec_Energy;
   vector<float>   *CaloJetVec_Eta;
   vector<float>   *CaloJetVec_Phi;
   vector<int>     *CaloJetVec_CaloConstituentsVec_Index;
   vector<int>     *CaloJetVec_CaloConstituentsVec_Ieta;
   vector<int>     *CaloJetVec_CaloConstituentsVec_Iphi;
   vector<float>   *CaloJetVec_CaloConstituentsVec_Energy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_EmEnergy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HadEnergy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HBEnergy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HEEnergy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HFEnergy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HOEnergy;
   vector<int>     *CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index;
   vector<int>     *CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta;
   vector<int>     *CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi;
   vector<int>     *CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy;
   vector<float>   *CaloJetVec_CaloConstituentsVec_HCALChannelVec_AuxEnergy;

   // List of branches
   TBranch        *b_GenJetVec_Energy;   //!
   TBranch        *b_GenJetVec_Eta;   //!
   TBranch        *b_GenJetVec_Phi;   //!
   TBranch        *b_CaloJetVec_Energy;   //!
   TBranch        *b_CaloJetVec_Eta;   //!
   TBranch        *b_CaloJetVec_Phi;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_Index;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_Ieta;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_Iphi;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_Energy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_EmEnergy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HadEnergy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HBEnergy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HEEnergy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HFEnergy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HOEnergy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy;   //!
   TBranch        *b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_AuxEnergy;   //!

   CaloJetTree(TString FileName_, TTree *tree=0);
   virtual ~CaloJetTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CaloJetTree_cxx

CaloJetTree::CaloJetTree(TString FileName_, TTree *tree) :
   fChain(0),
   FileName(FileName_)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
     TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(FileName);
      if (!f || !f->IsOpen()) {
         f = new TFile(FileName);
      }
      TDirectory * dir = (TDirectory*)f->Get(FileName + ":/myAna");
      dir->GetObject("CaloJetTree",tree);

   }
   Init(tree);
}

CaloJetTree::~CaloJetTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CaloJetTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CaloJetTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void CaloJetTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   GenJetVec_Energy = 0;
   GenJetVec_Eta = 0;
   GenJetVec_Phi = 0;
   CaloJetVec_Energy = 0;
   CaloJetVec_Eta = 0;
   CaloJetVec_Phi = 0;
   CaloJetVec_CaloConstituentsVec_Index = 0;
   CaloJetVec_CaloConstituentsVec_Ieta = 0;
   CaloJetVec_CaloConstituentsVec_Iphi = 0;
   CaloJetVec_CaloConstituentsVec_Energy = 0;
   CaloJetVec_CaloConstituentsVec_EmEnergy = 0;
   CaloJetVec_CaloConstituentsVec_HadEnergy = 0;
   CaloJetVec_CaloConstituentsVec_HBEnergy = 0;
   CaloJetVec_CaloConstituentsVec_HEEnergy = 0;
   CaloJetVec_CaloConstituentsVec_HFEnergy = 0;
   CaloJetVec_CaloConstituentsVec_HOEnergy = 0;
   CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index = 0;
   CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta = 0;
   CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi = 0;
   CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth = 0;
   CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy = 0;
   CaloJetVec_CaloConstituentsVec_HCALChannelVec_AuxEnergy = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("GenJetVec_Energy", &GenJetVec_Energy, &b_GenJetVec_Energy);
   fChain->SetBranchAddress("GenJetVec_Eta", &GenJetVec_Eta, &b_GenJetVec_Eta);
   fChain->SetBranchAddress("GenJetVec_Phi", &GenJetVec_Phi, &b_GenJetVec_Phi);
   fChain->SetBranchAddress("CaloJetVec_Energy", &CaloJetVec_Energy, &b_CaloJetVec_Energy);
   fChain->SetBranchAddress("CaloJetVec_Eta", &CaloJetVec_Eta, &b_CaloJetVec_Eta);
   fChain->SetBranchAddress("CaloJetVec_Phi", &CaloJetVec_Phi, &b_CaloJetVec_Phi);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_Index", &CaloJetVec_CaloConstituentsVec_Index, &b_CaloJetVec_CaloConstituentsVec_Index);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_Ieta", &CaloJetVec_CaloConstituentsVec_Ieta, &b_CaloJetVec_CaloConstituentsVec_Ieta);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_Iphi", &CaloJetVec_CaloConstituentsVec_Iphi, &b_CaloJetVec_CaloConstituentsVec_Iphi);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_Energy", &CaloJetVec_CaloConstituentsVec_Energy, &b_CaloJetVec_CaloConstituentsVec_Energy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_EmEnergy", &CaloJetVec_CaloConstituentsVec_EmEnergy, &b_CaloJetVec_CaloConstituentsVec_EmEnergy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HadEnergy", &CaloJetVec_CaloConstituentsVec_HadEnergy, &b_CaloJetVec_CaloConstituentsVec_HadEnergy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HBEnergy", &CaloJetVec_CaloConstituentsVec_HBEnergy, &b_CaloJetVec_CaloConstituentsVec_HBEnergy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HEEnergy", &CaloJetVec_CaloConstituentsVec_HEEnergy, &b_CaloJetVec_CaloConstituentsVec_HEEnergy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HFEnergy", &CaloJetVec_CaloConstituentsVec_HFEnergy, &b_CaloJetVec_CaloConstituentsVec_HFEnergy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HOEnergy", &CaloJetVec_CaloConstituentsVec_HOEnergy, &b_CaloJetVec_CaloConstituentsVec_HOEnergy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index", &CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index, &b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Index);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta", &CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta, &b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Ieta);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi", &CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi, &b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Iphi);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth", &CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth, &b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Depth);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy", &CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy, &b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_Energy);
   fChain->SetBranchAddress("CaloJetVec_CaloConstituentsVec_HCALChannelVec_AuxEnergy", &CaloJetVec_CaloConstituentsVec_HCALChannelVec_AuxEnergy, &b_CaloJetVec_CaloConstituentsVec_HCALChannelVec_AuxEnergy);
   Notify();
}

Bool_t CaloJetTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CaloJetTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CaloJetTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CaloJetTree_cxx
