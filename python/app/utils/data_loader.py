import pandas as pd
import numpy as np

def load_data(csv_file: str, point_factor = 10000) -> pd.DataFrame:
    # TODO optimize for fast read
    raw_df = pd.read_csv(csv_file, header=None, names=['datetime', 'bid', 'ask', 'volume'])
    raw_df['datetime'] = pd.to_datetime(raw_df['datetime'], format='%Y%m%d %H%M%S%f')
    raw_df=raw_df.drop(columns=['volume'])
    raw_df['bid'] = (point_factor * raw_df['bid']).apply(np.uint16)
    raw_df['ask'] = (point_factor * raw_df['ask']).apply(np.uint16)
    return raw_df