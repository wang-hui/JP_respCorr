#include "SimhitCorrData.h"

#include "TMinuit.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <climits>


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TH1D* RespCorr::makeHist(TString name, TString title, Int_t depth) const
{
  // first find the minimum and maximum ieta bins
  Int_t minieta=INT_MAX, maxieta=INT_MIN; 
  for(const_iterator it=begin(); it!=end(); ++it) {
    int ieta=it->first.first;
    if(ieta<minieta) minieta=ieta;
    if(ieta>maxieta) maxieta=ieta;
  }

  // create the histogram
  TH1D* hist=new TH1D(name, title, maxieta-minieta+1, minieta-0.5, maxieta+0.5);
  for(Int_t ieta=minieta; ieta<=maxieta; ++ieta) {
    Int_t bin=ieta-minieta+1;
    if(hasVal(ieta, depth)) {
      hist->SetBinContent(bin, getVal(ieta, depth));
      hist->SetBinError(bin, getErr(ieta, depth));
    } else {
      hist->SetBinContent(bin, 0.0);
      hist->SetBinError(bin, 0.0);
    }
  }
  return hist;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

double HcalValueMap::getSumVal(const RespCorr& respcorr) const
{
  double sum=0.0;
  for(const_iterator it=this->begin(); it!=this->end(); ++it) {
    EtaPhiDepthTuple id=it->first;
    Double_t energy=it->second;
    sum += respcorr.getVal(id.ieta(), id.depth())*energy;
  }
  return sum;
}

double HcalValueMap::getSumValDefault(void) const
{
  double sum=0.0;
  for(const_iterator it=this->begin(); it!=this->end(); ++it)    sum += it->second;
  return sum;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


void SimhitCorrData::addDatum(const SimhitCorrDatum& datum)
{
  //add it to the list
  fData.push_back(datum);

  // check that we don't need to add another channel to the respcorrs
  for(HcalValueMap::const_iterator it=datum.fHcalE.begin(); it!=datum.fHcalE.end(); ++it)
      fRespCorrs.setValErr(it->first.ieta(), it->first.depth(), 1.0, 0.0);

  return;
}


Double_t SimhitCorrData::NLL(void) const
{
  const double sigma=50.;
  Double_t total=0.0;
  for(auto it=fData.begin(); it!=fData.end(); ++it) {
    double mu=it->getOtherE()+it->getSumHcalE(fRespCorrs)-it->getTruthE();
    total += 0.5*mu*mu/sigma/sigma;
  }
  return total;
}


const RespCorr& SimhitCorrData::doFit(void)
{
  // set the number of parameters to be the number of towers
  TMinuit *gMinuit=new TMinuit(fRespCorrs.size());
  gMinuit->SetPrintLevel(fPrintLevel);
  gMinuit->SetErrorDef(0.5); // for a log likelihood
  gMinuit->SetFCN(FCN);
  gMinuit->SetObjectFit(this);

  std::cout << "Number of entries to fit=" << fData.size() << std::endl;
  
  // define the parameters
  int cntr=0;
  for(auto it=fRespCorrs.begin(); it!=fRespCorrs.end(); ++it) {
    std::ostringstream oss;
    oss << "ieta=" << it->first.ieta() << "; depth=" << it->first.depth();
    gMinuit->DefineParameter(cntr, oss.str().c_str(), it->second.val(), fParStep, fParMin, fParMax);
    ++cntr;
  }

  // Minimize
  gMinuit->Migrad();

  // get the results in the same order
  cntr=0;
  for(auto it=fRespCorrs.begin(); it!=fRespCorrs.end(); ++it) {
    Double_t val, error;
    gMinuit->GetParameter(cntr, val, error);
    it->second.val(val); it->second.err(error);
    ++cntr;
  }

  return fRespCorrs;
}


void SimhitCorrData::FCN(Int_t &, Double_t*, Double_t &f, Double_t *par, Int_t)
{
  // get the relevant data
  SimhitCorrData* data=dynamic_cast<SimhitCorrData*>(gMinuit->GetObjectFit());

  int cntr=0;
  for(auto it=data->fRespCorrs.begin(); it!=data->fRespCorrs.end(); ++it) {
    it->second.val(par[cntr]);
    ++cntr;
  }

  f = data->NLL();
  return;
}

