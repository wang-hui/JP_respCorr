#ifndef __SIMHIT_CORR_DATA_H__
#define __SIMHIT_CORR_DATA_H__

//
// SimhitCorrData.h
//
//    description: Object which contains data relevant to simhit response corrections
//
//

#include "Rtypes.h"
#include "TString.h"

#include <map>
#include <vector>
#include <tuple>
#include <cassert>


#include "TMinuit.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH1F.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <climits>


//
// forward declarations
//

//
// The response corrections are a map to a value and an uncertainty from eta-depth
// Be careful to set default values if necessary, as the map starts out empty
//

class EtaDepthPair : public std::pair<Int_t, Int_t>
{
public:
  EtaDepthPair() {}
  EtaDepthPair(Int_t ieta, Int_t depth) { first=ieta; second=depth; }
  virtual ~EtaDepthPair() {}

  Int_t ieta(void) const { return first; }
  Int_t depth(void) const { return second; }
  void ieta(Int_t i) { first=i; return; }
  void depth(Int_t i) { second=i; return; }

};

class ValErrPair : std::pair<Double_t, Double_t>
{
public:
  ValErrPair() {}
  virtual ~ValErrPair() {}

  Double_t val(void) const { return first; }
  Double_t err(void) const { return second; }
  void val(Double_t v) { first=v; return; }
  void err(Double_t v) { second=v; return; }

};

class RespCorr : public std::map<EtaDepthPair, ValErrPair>
{
public:
  RespCorr() {}
  virtual ~RespCorr() {}

  bool hasVal(Int_t ieta, Int_t depth) const { return (find(EtaDepthPair(ieta, depth))!=end()); }
  void setVal(Int_t ieta, Int_t depth, Double_t val) { (*this)[EtaDepthPair(ieta, depth)].val(val); return; }
  void setErr(Int_t ieta, Int_t depth, Double_t err) { (*this)[EtaDepthPair(ieta, depth)].err(err); return; }
  void setValErr(Int_t ieta, Int_t depth, Double_t val, Double_t err) { (*this)[EtaDepthPair(ieta, depth)].val(val); (*this)[EtaDepthPair(ieta, depth)].err(err); return; }
  Double_t getVal(Int_t ieta, Int_t depth) const { assert(hasVal(ieta, depth)); return at(EtaDepthPair(ieta, depth)).val(); }
  Double_t getErr(Int_t ieta, Int_t depth) const { assert(hasVal(ieta,depth)); return at(EtaDepthPair(ieta, depth)).err(); }

  TH1D* makeHist(TString name, TString title, Int_t depth) const;

};




//
// We also need a map from an ieta, iphi, depth to an HCAL value
//

class EtaPhiDepthTuple : public std::pair<EtaDepthPair, Int_t>
{
public:
  EtaPhiDepthTuple() {}
  EtaPhiDepthTuple(Int_t ieta, Int_t iphi, Int_t depth) { first.ieta(ieta); first.depth(depth); second=iphi; }
  virtual ~EtaPhiDepthTuple() {}

  Int_t ieta(void) const { return first.ieta(); }
  Int_t iphi(void) const { return second; }
  Int_t depth(void) const { return first.depth(); }
  void ieta(Int_t i) { first.ieta(i); return; }
  void iphi(Int_t i) { second=i; return; }
  void depth(Int_t i) { first.depth(i); return; }

};

class HcalValueMap : public std::map<EtaPhiDepthTuple, Double_t>
{
public:
  HcalValueMap() {}
  virtual ~HcalValueMap() {}

  Double_t getVal(Int_t ieta, Int_t iphi, Int_t depth) const { return at(EtaPhiDepthTuple(ieta, iphi, depth)); }
  void setVal(Int_t ieta, Int_t iphi, Int_t depth, Double_t val) { (*this)[EtaPhiDepthTuple(ieta, iphi, depth)]=val; return; }
  bool hasVal(Int_t ieta, Int_t iphi, Int_t depth) const { return (find(EtaPhiDepthTuple(ieta, iphi, depth))!=end()); }
  void addVal(Int_t ieta, Int_t iphi, Int_t depth, Double_t val) { if(hasVal(ieta,iphi,depth)) at(EtaPhiDepthTuple(ieta, iphi, depth))+=val; else setVal(ieta,iphi,depth,val); return; }
  double getSumVal(const RespCorr& respcorr) const;
  double getSumValDefault(void) const;

};


//
// Each jet "datum" consists of some HCAL energy deposits, a total amount of other energy (ECAL, HO, HF), and the truth energy
//

class SimhitCorrDatum
{
public:
  SimhitCorrDatum() {}
  ~SimhitCorrDatum() {}
  friend class SimhitCorrData;
  
  Double_t getHcalE(Int_t ieta, Int_t iphi, Int_t depth) const { return fHcalE.getVal(ieta, iphi, depth); }
  Double_t getSumHcalEDefault(void) const { return fHcalE.getSumValDefault(); }
  Double_t getSumHcalE(const RespCorr& respcorr) const { return fHcalE.getSumVal(respcorr); }
  Double_t getOtherE(void) const { return fOtherE; }
  Double_t getTruthE(void) const { return fTruthE; }
  Double_t getCaloJetEnergy(void) const { return fCaloJetEnergy; }
  Double_t getCaloJetEta(void) const { return fCaloJetEta; }
  Double_t getCaloJetPhi(void) const { return fCaloJetPhi; }
  
  void addOtherE(Double_t e) { fOtherE+=e; return; }
  void setTruthE(Double_t e) { fTruthE=e; return; }
  void setCaloJetEnergy(Double_t e) { fCaloJetEnergy=e; return; }
  void setCaloJetEta(Double_t e) { fCaloJetEta=e; return; }
  void setCaloJetPhi(Double_t e) { fCaloJetPhi=e; return; }
  void setHcalE(Int_t ieta, Int_t iphi, Int_t depth, Double_t e) { fHcalE.setVal(ieta, iphi, depth, e); return; } 
  void addHcalE(Int_t ieta, Int_t iphi, Int_t depth, Double_t e) { fHcalE.addVal(ieta, iphi, depth, e); return; }

 private:

  Double_t fOtherE = 0;
  HcalValueMap fHcalE;
  Double_t fTruthE = 0;
  Double_t fCaloJetEnergy = 0;
  Double_t fCaloJetEta = 0;
  Double_t fCaloJetPhi = 0;
};


class SimhitCorrData : public TObject
{
public:
  SimhitCorrData() : fPrintLevel(0), fParStep(0.10), fParMin(0.), fParMax(0.) {}
  virtual ~SimhitCorrData() {}

  // add datum to the list of data
  void addDatum(const SimhitCorrDatum& datum);
  
  // Compute the NLL given a set of corrections
  Double_t NLL(void) const;

  // fit for the response corrections
  const RespCorr& doFit(void);

  void ClosureTestPrint(int TestSize);
  std::vector<TH1F*> ClosureTestDraw();

  // fitting parameters
  inline void SetPrintLevel(Int_t p) { fPrintLevel=p; }
  inline void SetParStep(Double_t p) { fParStep=p; }
  inline void SetParMin(Double_t p) { fParMin=p; }
  inline void SetParMax(Double_t p) { fParMax=p; }

 private:
  // this is where the magic happens
  static void FCN(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t flag);

  // the data and default respcorrs
  std::vector<SimhitCorrDatum> fData;
  RespCorr fRespCorrs;
   
  // fit parameters
  Int_t fPrintLevel;
  Double_t fParStep;
  Double_t fParMin;
  Double_t fParMax;
};

#endif

