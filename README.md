# mikuTemplar

## Quick start

1. Download tick data from http://www.histdata.com/download-free-forex-data/. 
2. Put all zip files into one folder
3. Run `python/forex_tick_preprocess.py` to get OriginDataFrame (in csv format)
4. Run martin_ext to convert OriginDataFrame into ExtDataFrame
5. Run martin_sampling to get open tick (Optional) 
6. Run martin_sim to get MartinDataFrame