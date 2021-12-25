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

double HcalValueMap::getSumValCorr(const RespCorr& respcorr, double HBPar1, double HBPar2, double HBPar3, double HEPar1, double HEPar2, double HEPar3) const
{
  double sum=0.0;
  for(const_iterator it=this->begin(); it!=this->end(); ++it) {
    EtaPhiDepthTuple id=it->first;
    Double_t energy=it->second;
    double scaling = 1.0;
    if(id.isHB()) scaling=HBPar1*(1.0-HBPar2*pow(energy,-HBPar3));
    if(id.isHE()) scaling=HEPar1*(1.0-HEPar2*pow(energy,-HEPar3));
    
    sum += respcorr.getVal(id.ieta(), id.depth())*energy*scaling;
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
    double mu;
    if(DOSCALING) {
      mu=it->getOtherE()+it->getSumHcalECorr(fRespCorrs, HBPar1, HBPar2, HBPar3, HEPar1, HEPar2, HEPar3)-it->getTruthE();
    } else {
      mu=it->getOtherE()+it->getSumHcalE(fRespCorrs)-it->getTruthE();
    }
    total += 0.5*mu*mu/sigma/sigma;
  }
  return total;
}

const RespCorr& SimhitCorrData::doFit(void)
{
  // set the number of parameters to be the number of distinct hits
  int npars=fRespCorrs.size();
  if(DOSCALING) npars+=6;
  TMinuit *gMinuit=new TMinuit(npars);
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

  // add additional parameters
  if(DOSCALING) {
    gMinuit->DefineParameter(cntr+0, "HBPar1", 1.091, .1, 0., 0.);
    gMinuit->DefineParameter(cntr+1, "HBPar2", 0.202, .1, 0., 0.);
    gMinuit->DefineParameter(cntr+2, "HBPar3", 0.231, .1, 0., 0.);
    gMinuit->DefineParameter(cntr+3, "HEPar1", 1.091, .1, 0., 0.);
    gMinuit->DefineParameter(cntr+4, "HEPar2", 0.202, .1, 0., 0.);
    gMinuit->DefineParameter(cntr+5, "HEPar3", 0.231, .1, 0., 0.);
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
  if(DOSCALING) {
    data->HBPar1=par[cntr];
    data->HBPar2=par[cntr+1];
    data->HBPar3=par[cntr+2];
    data->HEPar1=par[cntr+3];
    data->HEPar2=par[cntr+4];
    data->HEPar3=par[cntr+5];
  }
  
  f = data->NLL();
  return;
}
