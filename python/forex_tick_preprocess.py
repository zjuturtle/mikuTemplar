import argparse
from unicodedata import decimal
import pandas as pd
import numpy as np
from datetime import datetime, timezone, timedelta

if __name__ == '__main__':
    parser = argparse.ArgumentParser('Forex tick raw data preprocess')
    parser.add_argument('--input', help='input tick csv file', type=str, required=True)
    parser.add_argument('--point_factor', help='point factor', type=int, required=False, default=100000)
    parser.add_argument('--point_offset', help='point offset', type=int, required=False, default=-1)
    parser.add_argument('--time_zone', help='time zone offset', type=int, required=False, default=3)
    args = parser.parse_args()

    raw_df = pd.read_csv(args.input, header=None, names=['datetime', 'bid', 'ask', 'volume'])
    raw_df['datetime'] = pd.to_datetime(raw_df['datetime'], format='%Y%m%d %H%M%S%f')
    raw_df['datetime'] = raw_df['datetime'] - timedelta(hours=args.time_zone)
    raw_df=raw_df.drop(columns=['volume'])
    raw_df['bid'] = np.round(args.point_factor * (raw_df['bid'] + args.point_offset)).apply(np.int16)
    raw_df['ask'] = np.round(args.point_factor * (raw_df['ask'] + args.point_offset)).apply(np.int16)

    raw_df[['datetime', 'bid']].to_csv(args.input+'.bid.csv',index_label='index')
    raw_df[['datetime', 'ask']].to_csv(args.input+'.ask.csv',index_label='index')