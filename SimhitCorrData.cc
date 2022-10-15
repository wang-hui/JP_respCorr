#include "SimhitCorrData.h"
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

  //gMinuit->SetMaxIterations(1000);

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

void SimhitCorrData::ClosureTestPrint(int TestSize) {
  int TestSize_ = TestSize;
  if (TestSize_ > int(fData.size())) TestSize_ = fData.size();
  for(int i = 0; i < TestSize_; i++) {
    auto& Datum = fData.at(i);
    if (i == 0) std::cout << "CaloJetEnergy, CaloJetEta, CaloJetPhi, old energy, new energy" << std::endl;
    std::cout << Datum.getCaloJetEnergy() << ", " << Datum.getCaloJetEta() << ", " << Datum.getCaloJetPhi()
    << ", " << Datum.getSumHcalEDefault() + Datum.getOtherE()
    << ", " << Datum.getSumHcalE(fRespCorrs) + Datum.getOtherE() << std::endl;
  }
}

std::vector<TH1F*> SimhitCorrData::ClosureTestDraw() {
  std::vector<TH1F*> HistVec;
  TH1F* CaloJetE_OldRatio_HB_h = new TH1F("CaloJetE_OldRatio_HB_h", "CaloJetE_OldRatio_HB_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_OldRatio_ieta1516_h = new TH1F("CaloJetE_OldRatio_ieta1516_h", "CaloJetE_OldRatio_ieta1516_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_OldRatio_HE_h = new TH1F("CaloJetE_OldRatio_HE_h", "CaloJetE_OldRatio_HE_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_OldRatio_HE_ietaL_h = new TH1F("CaloJetE_OldRatio_HE_ietaL_h", "CaloJetE_OldRatio_HE_ietaL_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_OldRatio_HE_ietaH_h = new TH1F("CaloJetE_OldRatio_HE_ietaH_h", "CaloJetE_OldRatio_HE_ietaH_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_NewRatio_HB_h = new TH1F("CaloJetE_NewRatio_HB_h", "CaloJetE_NewRatio_HB_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_NewRatio_ieta1516_h = new TH1F("CaloJetE_NewRatio_ieta1516_h", "CaloJetE_NewRatio_ieta1516_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_NewRatio_HE_h = new TH1F("CaloJetE_NewRatio_HE_h", "CaloJetE_NewRatio_HE_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_NewRatio_HE_ietaL_h = new TH1F("CaloJetE_NewRatio_HE_ietaL_h", "CaloJetE_NewRatio_HE_ietaL_h", 80, 0.0, 4.0);
  TH1F* CaloJetE_NewRatio_HE_ietaH_h = new TH1F("CaloJetE_NewRatio_HE_ietaH_h", "CaloJetE_NewRatio_HE_ietaH_h", 80, 0.0, 4.0);

  TH1F* CaloJetE_OldDiff_HB_h = new TH1F("CaloJetE_OldDiff_HB_h", "CaloJetE_OldDiff_HB_h", 100, -50, 50);
  TH1F* CaloJetE_OldDiff_ieta1516_h = new TH1F("CaloJetE_OldDiff_ieta1516_h", "CaloJetE_OldDiff_ieta1516_h", 100, -50, 50);
  TH1F* CaloJetE_OldDiff_HE_h = new TH1F("CaloJetE_OldDiff_HE_h", "CaloJetE_OldDiff_HE_h", 100, -50, 50);
  TH1F* CaloJetE_OldDiff_HE_ietaL_h = new TH1F("CaloJetE_OldDiff_HE_ietaL_h", "CaloJetE_OldDiff_HE_ietaL_h", 100, -50, 50);
  TH1F* CaloJetE_OldDiff_HE_ietaH_h = new TH1F("CaloJetE_OldDiff_HE_ietaH_h", "CaloJetE_OldDiff_HE_ietaH_h", 100, -50, 50);
  TH1F* CaloJetE_NewDiff_HB_h = new TH1F("CaloJetE_NewDiff_HB_h", "CaloJetE_NewDiff_HB_h", 100, -50, 50);
  TH1F* CaloJetE_NewDiff_ieta1516_h = new TH1F("CaloJetE_NewDiff_ieta1516_h", "CaloJetE_NewDiff_ieta1516_h", 100, -50, 50);
  TH1F* CaloJetE_NewDiff_HE_h = new TH1F("CaloJetE_NewDiff_HE_h", "CaloJetE_NewDiff_HE_h", 100, -50, 50);
  TH1F* CaloJetE_NewDiff_HE_ietaL_h = new TH1F("CaloJetE_NewDiff_HE_ietaL_h", "CaloJetE_NewDiff_HE_ietaL_h", 100, -50, 50);
  TH1F* CaloJetE_NewDiff_HE_ietaH_h = new TH1F("CaloJetE_NewDiff_HE_ietaH_h", "CaloJetE_NewDiff_HE_ietaH_h", 100, -50, 50);

  for(auto& Datum : fData) {
    auto Eta = fabs(Datum.getCaloJetEta());
    auto OldRatio = (Datum.getSumHcalEDefault() + Datum.getOtherE()) / 50.0;
    auto NewRatio = (Datum.getSumHcalE(fRespCorrs) + Datum.getOtherE()) / 50.0;
    auto OldDiff = (Datum.getSumHcalEDefault() + Datum.getOtherE()) - 50.0;
    auto NewDiff = (Datum.getSumHcalE(fRespCorrs) + Datum.getOtherE()) - 50.0;

    if(Eta < 1.2) {
        CaloJetE_OldRatio_HB_h->Fill(OldRatio);
        CaloJetE_NewRatio_HB_h->Fill(NewRatio);
        CaloJetE_OldDiff_HB_h->Fill(OldDiff);
        CaloJetE_NewDiff_HB_h->Fill(NewDiff);
    }
    else if (Eta < 1.4) {
        CaloJetE_OldRatio_ieta1516_h->Fill(OldRatio);
        CaloJetE_NewRatio_ieta1516_h->Fill(NewRatio);
        CaloJetE_OldDiff_ieta1516_h->Fill(OldDiff);
        CaloJetE_NewDiff_ieta1516_h->Fill(NewDiff);
    }
    else {
        CaloJetE_OldRatio_HE_h->Fill(OldRatio);
        CaloJetE_NewRatio_HE_h->Fill(NewRatio);
        CaloJetE_OldDiff_HE_h->Fill(OldDiff);
        CaloJetE_NewDiff_HE_h->Fill(NewDiff);
        if (Eta < 2.3) {
            CaloJetE_OldRatio_HE_ietaL_h->Fill(OldRatio);
            CaloJetE_NewRatio_HE_ietaL_h->Fill(NewRatio);
            CaloJetE_OldDiff_HE_ietaL_h->Fill(OldDiff);
            CaloJetE_NewDiff_HE_ietaL_h->Fill(NewDiff);
        }
        else {
            CaloJetE_OldRatio_HE_ietaH_h->Fill(OldRatio);
            CaloJetE_NewRatio_HE_ietaH_h->Fill(NewRatio);
            CaloJetE_OldDiff_HE_ietaH_h->Fill(OldDiff);
            CaloJetE_NewDiff_HE_ietaH_h->Fill(NewDiff);
        }
    }
  }

  HistVec.push_back(CaloJetE_OldRatio_HB_h);
  HistVec.push_back(CaloJetE_OldRatio_ieta1516_h);
  HistVec.push_back(CaloJetE_OldRatio_HE_h);
  HistVec.push_back(CaloJetE_OldRatio_HE_ietaL_h);
  HistVec.push_back(CaloJetE_OldRatio_HE_ietaH_h);
  HistVec.push_back(CaloJetE_NewRatio_HB_h);
  HistVec.push_back(CaloJetE_NewRatio_ieta1516_h);
  HistVec.push_back(CaloJetE_NewRatio_HE_h);
  HistVec.push_back(CaloJetE_NewRatio_HE_ietaL_h);
  HistVec.push_back(CaloJetE_NewRatio_HE_ietaH_h);

  HistVec.push_back(CaloJetE_OldDiff_HB_h);
  HistVec.push_back(CaloJetE_OldDiff_ieta1516_h);
  HistVec.push_back(CaloJetE_OldDiff_HE_h);
  HistVec.push_back(CaloJetE_OldDiff_HE_ietaL_h);
  HistVec.push_back(CaloJetE_OldDiff_HE_ietaH_h);
  HistVec.push_back(CaloJetE_NewDiff_HB_h);
  HistVec.push_back(CaloJetE_NewDiff_ieta1516_h);
  HistVec.push_back(CaloJetE_NewDiff_HE_h);
  HistVec.push_back(CaloJetE_NewDiff_HE_ietaL_h);
  HistVec.push_back(CaloJetE_NewDiff_HE_ietaH_h);

  return HistVec;
}
