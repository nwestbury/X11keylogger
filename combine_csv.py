import argparse
import csv
import os

def combine_csv(csv_filenames, csv_output, gap=0, normalize=False):

    output_rows = []
    overall_time = 0
    for csv_filename in csv_filenames:
        with open(csv_filename) as csv_file:
            csv_reader = csv.reader(csv_file)
            next(csv_reader) # skip header (time, code, release)
            rows = [map(int, row) for row in csv_reader]

        if rows:
            normalize_time = rows[0][0] if normalize else 0
            cur_time = 0
            for row in rows:
                cur_time = row[0] + overall_time - normalize_time
                row[0] = cur_time
                output_rows.append(','.join(str(x) for x in row) + os.linesep)
            overall_time = cur_time + gap

    # Do this at the end to allow for the same file to be written to as opened
    with open(csv_output, 'w') as output:
        output.write('time,code,release' + os.linesep)
        for row in output_rows:
            output.write(row)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Combine or normalize csv key record files')
    parser.add_argument('filenames', type=str, nargs='+', help='csv files to be combine (in order)')
    parser.add_argument('output', type=str, help='output csv file')
    parser.add_argument('--gap', type=int, default=0, help='Millisecond gap between files')
    parser.add_argument('--normalize', default=False, action='store_const', const=True, help='Restart counter at 0 for file(s)')

    args = parser.parse_args()

    combine_csv(args.filenames, args.output, gap=args.gap, normalize=args.normalize)