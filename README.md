# JP_respCorr
This is a git repo to calculate HBHE respCorr

1. Set input TTree in run.C  
The input ntuple is made with  
https://github.com/wang-hui/HCAL_MET_res/blob/master/HCAL_Jet_Ana/plugins/HCAL_Jet_Ana.cc    

2. Make output dirs (only once)
```
mkdir -p results_temp
mkdir -p plots_temp
```

3. Run the macro with ROOT
```
root -b
root [0] .L SimhitCorrData.cc++
root [1] .x run.C
```
