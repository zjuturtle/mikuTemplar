import argparse
import pandas as pd
import numpy as np
import os
import shutil
import zipfile
from pathlib import Path
from datetime import timedelta
from multiprocessing import Pool

class Task:
    def __init__(self, zip_file_path:str, args):
        self.zip_file_path = zip_file_path
        self.args = args

def read_forex_csv(csv_file:str, args):
    raw_df = pd.read_csv(csv_file, header=None, names=['datetime', 'bid', 'ask', 'volume'])
    raw_df['datetime'] = pd.to_datetime(raw_df['datetime'], format='%Y%m%d %H%M%S%f')
    raw_df['datetime'] = raw_df['datetime'] - timedelta(hours=args.time_zone)
    raw_df=raw_df.drop(columns=['volume'])
    raw_df['bid'] = np.round(args.point_factor * (raw_df['bid'] + args.point_offset)).apply(np.int16)
    raw_df['ask'] = np.round(args.point_factor * (raw_df['ask'] + args.point_offset)).apply(np.int16)
    return raw_df[['datetime', 'bid', 'ask']]

def extract(task:Task):
    zip_file_path = task.zip_file_path
    extract_path = os.path.join(zip_file_path[:-4])
    Path(extract_path).mkdir(parents=True, exist_ok=True)
    zfile = zipfile.ZipFile(zip_file_path)
    zfile.extractall(extract_path)

    csv_file = list(filter(lambda file_name: file_name[-3:] == 'csv', os.listdir(extract_path)))[0]
    df = read_forex_csv(os.path.join(extract_path, csv_file), task.args)
    shutil.rmtree(extract_path)
    return df

if __name__ == '__main__':
    parser = argparse.ArgumentParser('Forex tick raw data preprocess.')
    parser.add_argument('--input', help='input tick csv file or folder', type=str, required=True)
    parser.add_argument('--output', help='output tick csv file', type=str, required=True)
    parser.add_argument('--point_factor', help='point factor', type=int, required=False, default=100000)
    parser.add_argument('--point_offset', help='point offset', type=int, required=False, default=-1)
    parser.add_argument('--time_zone', help='time zone offset', type=int, required=False, default=3)
    parser.add_argument('--worker_num', help='concurrence worker num', type=int, required=False, default=8)
    args = parser.parse_args()

    if os.path.isfile(args.input):
        if args.input[-3:] == 'csv':
            print('[INFO]Detect csv file:{}'.format(args.input))
            df = read_forex_csv(args.input, args)
        if args.input[-3:] == 'zip':
            print('[INFO]Detect zip file:{}'.format(args.input))
            df = extract(Task(zip_file_path=args.input, args=args))
        df.to_csv(args.output, index_label='index')

    if os.path.isdir(args.input):
        print('[INFO]Detect directory:{}'.format(args.input))
        all_zip_files = list(filter(lambda file_name: file_name[-3:] == 'zip', os.listdir(args.input)))
        if not all_zip_files:
            print('[INFO]Not zip found in directory {}'.format(args.input))
            os._exit(0)
        all_zip_date = list(map(lambda file_name: int(file_name.split('.')[0][-6:]), all_zip_files))
        date_to_file_dic = dict()
        for date, zip_file in zip(all_zip_date, all_zip_files):
            date_to_file_dic[date] = zip_file
        all_zip_date.sort()

        # Check file valid
        for i in range(len(all_zip_date)-1):
            date_diff = all_zip_date[i+1] - all_zip_date[i]
            if date_diff == 1 or date_diff == 89:
                continue
            print('[WARNING]zip date is not continuous!Gap found between {} {}'.format(
                date_to_file_dic[all_zip_date[i]],
                date_to_file_dic[all_zip_date[i+1]]
            ))
            os._exit(0)
        all_symbol = list(map(lambda file_name: file_name.split('_')[-2], all_zip_files))
        if len(set(all_symbol)) != 1:
            print('[WARNING]symbol is not the same!')
        
        all_tasks = list(map(lambda date: 
            Task(
                zip_file_path = os.path.join(args.input, date_to_file_dic[date]),
                args=args
            ), all_zip_date))
        with Pool(args.worker_num) as p:
            df_data_list=p.map(extract, all_tasks)
        print('[INFO]writing to output...')
        final_df = pd.concat(df_data_list)
        final_df.to_csv(args.output, index_label='index')
        print('[INFO]All done!')