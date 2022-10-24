int plot_Gaus()
{
    //TString file_name = "test";
    //TString file_name = "UL_DoublePion_E-50_RECO_PU_DLPHIN_class_no_respCorr_save_to_AUX_TTree";
    TString file_name = "UL_DoublePion_E-50_RECO_noPU_DLPHIN_class_no_respCorr_save_to_AUX_TTree";

    bool plot_ratio = true;

    bool plot_underflow_overflow_bins = false;
    bool compare_to_one = false;

    TFile *f1 = new TFile("results/" + file_name + "/ClosureTest.root");

    std::vector<TString> hist_list = {};

    TString x_title = "";
    TString y_title = "";

    if(plot_ratio)
    {
        hist_list = 
        {
            "CaloJetE_OldRatio_HB", "CaloJetE_OldRatio_ieta1516", "CaloJetE_OldRatio_HE", "CaloJetE_OldRatio_HE_ietaL", "CaloJetE_OldRatio_HE_ietaH",
            "CaloJetE_NewRatio_HB", "CaloJetE_NewRatio_ieta1516", "CaloJetE_NewRatio_HE", "CaloJetE_NewRatio_HE_ietaL", "CaloJetE_NewRatio_HE_ietaH",
            "CaloJetE_MAHIRatio_HB", "CaloJetE_MAHIRatio_ieta1516", "CaloJetE_MAHIRatio_HE", "CaloJetE_MAHIRatio_HE_ietaL", "CaloJetE_MAHIRatio_HE_ietaH",
        };

        plot_underflow_overflow_bins = true;

        x_title = "RecoJet / GenJet";
        //y_title = "";
    }


    for(int i = 0; i < hist_list.size(); i++)
    {
        TString hist_name = hist_list.at(i);

        TString h1_name = hist_name + "_h";
        TH1F *h1 = (TH1F*)f1->Get(h1_name);

        TCanvas* mycanvas = new TCanvas("mycanvas", "mycanvas", 600, 600);
        //gStyle->SetOptStat(kFALSE);

        h1->Draw("e");
        auto h1_entry = h1->GetEntries();
        auto h1_mean = h1->GetMean();
        auto h1_std = h1->GetStdDev();
        auto max_bin = h1->GetMaximumBin();
        if(max_bin != 1)
        {h1_mean = h1->GetXaxis()->GetBinCenter(max_bin);}
        //std::cout << h1_mean << ", " << h1_std << std::endl;
        h1->Fit("gaus", "0", "", h1_mean - 1.5 * h1_std, h1_mean + 1.5 * h1_std);
        auto f1 = h1->GetFunction("gaus");
        auto c = f1->GetParameter(0);
        auto mu = f1->GetParameter(1);
        auto sigma = f1->GetParameter(2);
        auto c_err = f1->GetParError(0);
        auto mu_err = f1->GetParError(1);
        auto sigma_err = f1->GetParError(2);
        std::cout << c << ", " << mu << ", " << sigma << std::endl;

        h1->Fit("gaus", "", "", mu - 1.5 * sigma, mu + 1.5 * sigma);
        f1 = h1->GetFunction("gaus");
        c = f1->GetParameter(0);
        mu = f1->GetParameter(1);
        sigma = f1->GetParameter(2);
        c_err = f1->GetParError(0);
        mu_err = f1->GetParError(1);
        sigma_err = f1->GetParError(2);

        std::stringstream s1;
        s1 << "#bf{#mu = " << std::setprecision(3) << mu << " #pm " << mu_err << "}";
        TString TS1 = s1.str();
        std::stringstream s2;
        s2 << "#bf{#sigma = " << std::setprecision(3) << sigma << " #pm " << sigma_err << "}";
        TString TS2 = s2.str();
        std::stringstream s3;
        s3 << "#bf{#sigma / #mu = " << std::setprecision(3) << sigma / mu << "}";
        TString TS3 = s3.str();
        std::stringstream s4;
        s4 << "#bf{A / N = " << std::setprecision(3) << c / h1_entry << "}";
        TString TS4 = s4.str();

        h1->GetXaxis()->SetTitle(x_title);
        if(plot_underflow_overflow_bins) h1->GetXaxis()->SetRange(0, h1->GetNbinsX() + 1);
        h1->SetTitle(hist_name);
        h1->GetYaxis()->SetTitle(y_title);
        //gPad->SetLogz();

        if(compare_to_one)
        {
            h1->GetXaxis()->SetRangeUser(0.5,1.5);
            TLine *l = new TLine(1, h1->GetMinimum(),1, h1->GetMaximum());
            l->SetLineColor(kBlack);
            l->Draw("same");
        }

        TLatex latex;
        latex.SetTextSize(0.04);
        latex.SetNDC();
        latex.DrawLatex(0.6,0.7,TS1);
        latex.DrawLatex(0.6,0.65,TS2);
        latex.DrawLatex(0.6,0.6,TS3);
        latex.DrawLatex(0.6,0.55,TS4);

        mycanvas->SetLeftMargin(0.15);
        mycanvas->SetRightMargin(0.1);
        mycanvas->SaveAs("plots_temp/" + file_name + "_" + hist_name + ".png");
    }
    return 0;
}
