from output_parsing import parse                      # NEW parser 
from analysis.csv.log_file_parsing import parse      # OLD parser
import csv
import os

def _normalize(rows):
    # Not all rows will have the same columns.
    # This function ensures that they do.
    columns = set()
    for row in rows:
        columns = columns.union(set(row.keys()))
    for row in rows:
        for column in columns:
            if column not in row:
                row[column] = ''

def aggregate_logs(path):
    if os.path.isdir(path):  # Directory of log files
            file_names = os.listdir(path)
            log_file_paths = [path + '/' + fn
                              for fn in file_names
                              if fn.endswith('.log')]
            log_file_paths.sort()
            rows = [parse(lfp) for lfp in log_file_paths]
    else:
        raise Exception('Source {0} is not a directory'.format(path))
    if not rows:
        raise Exception('Directory {0} does not contain any log files'.format(path))
    _normalize(rows)
    return rows


def write_to_csv(rows, file_path, delimiter=','):
    fieldnames = set()
    for row in rows:
        fieldnames = fieldnames.union(set(row.keys()))
    with open(file_path, 'w') as file:
        writer = csv.DictWriter(file,
                                fieldnames=fieldnames,
                                delimiter=delimiter)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)