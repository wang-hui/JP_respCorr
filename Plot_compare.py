#lowercase with underline for function/class names
#canmel case for variables

import ROOT as rt
rt.gROOT.SetBatch(True)

Folder = "results_SinglePion_E-300_0p5M"
FileName = "results/" + Folder + "/respCorr.root"

BaseName = "Depth1"
BaseFileList = [FileName]
BaseHistList = ["D1"]

Comp1Name = "Depth2"
Comp1FileList = BaseFileList
Comp1HistList = ["D2"]

Comp2Name = "Depth3"
Comp2FileList = BaseFileList
Comp2HistList = ["D3"]

Comp3Name = "Depth4"
Comp3FileList = BaseFileList
Comp3HistList = ["D4"]

ShapeComp = False
SetLogY = False
PlotRatio = False
FitGaus = False

YTitle = "respCorr"
XTitle = "ieta"

XMin = -26.5
XMax = 26.5

YMax = 2.0
YScale = 1.0

FileDir = ""
HistDir = ""

if ShapeComp: YTitle = "A.U."

class MyStruct:
    def __init__(self, Name, FileList, HistList, Color, StructList):
        self.Name = Name
        self.FileList = FileList
        self.HistList = HistList
        self.Color = Color
        StructList.append(self)

StructList = []
Base = MyStruct(BaseName, BaseFileList, BaseHistList, rt.kBlue, StructList)
Comp1 = MyStruct(Comp1Name, Comp1FileList, Comp1HistList, rt.kRed, StructList)
Comp2 = MyStruct(Comp2Name, Comp2FileList, Comp2HistList, rt.kGreen+1, StructList)
Comp3 = MyStruct(Comp3Name, Comp3FileList, Comp3HistList, rt.kYellow+1, StructList)

def GausFit(myHist):
    mean = myHist.GetMean()
    std = myHist.GetStdDev()
    myHist.Fit("gaus", "0", "", mean-1.5*std, mean+1.5*std)
    gaus = myHist.GetFunction("gaus")
    c = gaus.GetParameter(0)
    cErr = gaus.GetParError(0)
    mu = gaus.GetParameter(1)
    muErr = gaus.GetParError(1)
    sigma = gaus.GetParameter(2)
    sigmaErr = gaus.GetParError(2)
    return c, cErr, mu, muErr, sigma, sigmaErr

rt.TH1.AddDirectory(rt.kFALSE)
#rt.TH1.__init__._creates = False

for j in range(len(BaseHistList)):
    rt.gStyle.SetOptStat(rt.kFALSE)

    MyCanvas = None
    PadUp = None
    PadDown = None
    BaseHist = None
    OutName = Folder.replace("results_", "")

    if PlotRatio:
        MyCanvas = rt.TCanvas("MyCanvas", "MyCanvas", 600, 600)
        PadUp = rt.TPad("PadUp", "PadUp", 0, 0.3, 1, 1.0)
        PadUp.SetBottomMargin(0.01)
        PadUp.Draw()

        PadDown = rt.TPad("PadDown", "PadDown", 0, 0, 1, 0.3)
        PadDown.SetTopMargin(0.03)
        PadDown.SetBottomMargin(0.3)
        PadDown.SetGrid()
        PadDown.Draw()
    else:
        MyCanvas = rt.TCanvas("MyCanvas", "MyCanvas", 800, 600)
        PadUp = rt.TPad("PadUp", "PadUp", 0, 0, 1, 1.0)
        PadUp.Draw()

    MyLeg = rt.TLegend(0.1,0.8,0.9,0.9)
    MyLeg.SetNColumns(4)
    #MyLeg.SetMargin(0.15)
    MyLeg.SetTextSize(0.04)

    for i in range(len(BaseFileList)):
        for k in range(len(StructList)):
            iFileName = StructList[k].FileList[i]
            iHistName = StructList[k].HistList[j]
            iFile = rt.TFile.Open(FileDir + iFileName)
            #print(iFile.ls())
            iHist = iFile.Get(HistDir + iHistName)
            print(k, iHist)
            #for Ibin in range(1,iHist.GetNbinsX()):
            #    print(iHist.GetXaxis().GetBinCenter(Ibin)),
            iHist.SetLineColor(StructList[k].Color)
            iHist.Sumw2()

            OutName = OutName + iHistName

            iStr = ""
            if FitGaus:
                c, cErr, mu, muErr, sigma, sigmaErr = GausFit(iHist)
                iStr = " (#mu %.0f#pm%.1f, #sigma %0.f#pm%.1f)" % (mu, muErr, sigma, sigmaErr)

            if i == 0:
                MyLeg.AddEntry(iHist, StructList[k].Name + iStr, "l")
            if ShapeComp: iHist.Scale(1.0/iHist.GetEntries())

            MyCanvas.cd()
            PadUp.cd()

            YMaxTemp = iHist.GetMaximum(YMax) * YScale
            if k == 0 and i == 0:
                BaseHist = iHist.Clone()

                BaseHist.GetXaxis().SetTitle(XTitle)
                BaseHist.GetYaxis().SetTitle(YTitle)
                BaseHist.SetMaximum(YMaxTemp)
                BaseHist.SetTitle("")
                if XMax > 0:
                    BaseHist.GetXaxis().SetRangeUser(XMin, XMax)
                BaseHist.Draw("e")

                if PlotRatio:
                    MyCanvas.cd()
                    PadDown.cd()
                    BaseFrame = BaseHist.Clone()
                    BaseFrame.Reset()
    
                    BaseFrame.GetYaxis().SetTitle("Ratio")
                    BaseFrame.GetYaxis().SetTitleOffset(0.4)
                    BaseFrame.GetYaxis().SetTitleSize(0.1)
                    BaseFrame.GetYaxis().SetLabelSize(0.08)
                    BaseFrame.GetYaxis().SetRangeUser(0, 2)
                    #BaseFrame.GetYaxis().SetRangeUser(0.8, 1.2)
    
                    BaseFrame.GetXaxis().SetTitle(XTitle)
                    BaseFrame.GetXaxis().SetTitleOffset(0.8)
                    BaseFrame.GetXaxis().SetTitleSize(0.1)
                    BaseFrame.GetXaxis().SetLabelSize(0.08)
                    BaseFrame.Draw()
    
                    MyLine = rt.TLine(BaseFrame.GetXaxis().GetXmin(), 1.0, XMax, 1.0)
                    MyLine.Draw()
            else:
                if YMaxTemp > BaseHist.GetMaximum(): 
                    BaseHist.SetMaximum(YMaxTemp)
                iHist.Draw("esame")

                if PlotRatio:
                    MyCanvas.cd()
                    PadDown.cd()
                    RatioHist = iHist.Clone()
                    rt.SetOwnership(RatioHist, False)
                    RatioHist.Divide(BaseHist)
                    RatioHist.Draw("same")

    MyCanvas.cd()
    PadUp.cd()
    if SetLogY: PadUp.SetLogy()
    MyLeg.Draw()
    MyCanvas.SaveAs("plots_temp/" + OutName + ".png")

