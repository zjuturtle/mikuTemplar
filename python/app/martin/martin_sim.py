import pandas as pd

class MartinSim:
    def __init__(self, raw_data:pd.DataFrame):
        self.__raw_data = raw_data
        self._merge()

    def _merge(self):
        df_bid_drop = df_bid[df_bid['bid'].diff() != 0]