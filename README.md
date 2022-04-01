# JP_respCorr
This is a git repo to calculate HBHE respCorr

1. Set the input ntuple in run.C  
The input ntuple is made with  
https://github.com/wang-hui/HCAL_MET_res/blob/CMSSW_10_6_x/HCAL_Jet_Ana/plugins/HCAL_Jet_Ana.cc  
An example input ntuple can be found in LPC EOS:  
/eos/uscms/store/user/huiwang/for_JP/UL_DoublePion_E-50_RECO_noPU_simHits_fix_HB_TTree.root  

2. Run the macro with ROOT
```
root
root [0] .L SimhitCorrData.cc++
root [1] .x run.C
```
