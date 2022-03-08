import pandas as pd
import numpy as np
from app.utils.queue import QueueWithMaxMin

class MartinSim:
    def __init__(self, 
                 raw_data:pd.DataFrame, 
                 ext_window_small: int=1024, 
                 ext_window_large: int=1024*20):
        self.__raw_data = raw_data
        self.__ext_window_small = ext_window_small
        self.__ext_window_large = ext_window_large
        self._merge()
        self._generate_ext()

    def _merge(self):
        """去除对马丁过程没有影响的tick
        """

        def merge(data: pd.DataFrame, col_name: str) -> pd.DataFrame:
            # 去除价格没有变化的的tick
            data_unique = data[data[col_name].diff() != 0]

            # 去除价格单调变化的中间tick
            data_diff = data_unique[col_name].diff()
            data_monotonous = data_diff.shift(-1).mul(data_diff)
            return data_unique[data_monotonous < 0]

        self.__bid = merge(self.__raw_data, 'bid')
        self.__ask = merge(self.__raw_data, 'ask')

    def _generate_ext(self):
        def generate_ext(data: pd.DataFrame, col_name: str):
            data['ext_small_window_min'] = np.iinfo(np.uint16).max
            data['ext_small_window_max'] = np.iinfo(np.uint16).min
            data['ext_large_window_min'] = np.iinfo(np.uint16).max
            data['ext_large_window_max'] = np.iinfo(np.uint16).min

            q_small_window=QueueWithMaxMin()
            q_large_window=QueueWithMaxMin()

            for index, row in data.iterrows():
                q_small_window.push(row[col_name])
                q_large_window.push(row[col_name])

                row['ext_small_window_min'] = 




        generate_ext(self.__bid, 'bid')
        generate_ext(self.__ask, 'ask')