# mikuTemplar

## Quick start

1. Download tick data from http://www.histdata.com/download-free-forex-data/. 
2. Put all zip files into one folder
3. Run `python/forex_tick_preprocess.py` to get OriginDataFrame (in csv format)
4. Run martin_ext to convert OriginDataFrame into ExtDataFrame
5. Run martin_sampling to get open tick (Optional) 
6. Run martin_sim to get MartinDataFrame

## Example

```bash
$ python3 python/forex_tick_preprocess.py --input [FOLDER_CONTAIN_ZIPS] --output [ORIGIN_DATAFRAME].csv
$ martin_ext --input [ORIGIN_DATAFRAME].csv --output [EXT_DATAFRAME].csv
$ martin_sampling --input [ORIGIN_DATAFRAME].csv --output [OPEN_DATAFRAME].csv
$ martin_sim --input_ext [EXT_DATAFRAME].csv --input_open [OPEN_DATAFRAME].csv --output [SIM_DATAFRAME].csv --operation buy --martin_position_intervals=0,50,100,150, --martin_stop_profits=50,50,50,50 --martin_stop_loss 200
```