#! /usr/bin/env python3
"""
Prints markdown summary table of test results for each RNG and output a JSON file with failure details.
"""
from collections import defaultdict, namedtuple
import glob
import json
import os.path
import re
import sys

RESULT_DIR = os.path.join( os.path.dirname(__file__), 'results')

CRUSH_PREFIXES_REX = re.compile( "^(big_crush|crush|small_crush)-(.+)$" )

Failure = namedtuple('Failure', 'test_no name p_value is_suspect')
TestResult = namedtuple('TestResult', 'generator crush_type nb_stat status failures nb_fail nb_suspect')

def print_table( rows, stream ):
    max_sizes_by_col = {}
    for row in rows:
        for col, value in enumerate(row):
            max_sizes_by_col[col] = max(len(value), max_sizes_by_col.get(col, -1))
    for row in rows:
        for col, value in enumerate(row):
            max_sizes_by_col[col] = max(len(value), max_sizes_by_col.get(col, -1))
    def print_row(values):
        stream.write('|')
        for col, value in enumerate(values):
            stream.write( value )
            stream.write('|')
        stream.write('\n')
    def centered(col, value):
        return ' {: ^{}} '.format(value, max_sizes_by_col[col])
    def left_aligned(col, value):
        return ' {: <{}} '.format(value, max_sizes_by_col[col])
    def right_aligned(col, value):
        return ' {: >{}} '.format(value, max_sizes_by_col[col])
    def cell_aligned(col, value):
        align = left_aligned if col == 0 else right_aligned
        return align(col, value)
    print_row([centered(col, value) for col, value in enumerate(rows[0])])
    def header_sep(col, value):
        return ' ' + ('-' * max_sizes_by_col[col]) + ' '
    print_row([header_sep(col, value) for col, value in enumerate(rows[0])])
    for row in rows[1:]:
        print_row([cell_aligned(col, value) for col, value in enumerate(row)])


def main():
    results_by_crush_by_rng = defaultdict( dict ) # {rng: {crush; TestResult} }
    all_results = []
    for path in glob.glob( os.path.join(RESULT_DIR, '*.txt' ) ):
        filename = os.path.splitext( os.path.basename(path) )[0]
        # m = CRUSH_PREFIXES_REX.match(filename)
        # if m is None:
        #     continue
        # crush_type, rng = m.groups()
        # print( crush_type, rng )
        with open(path, 'rt') as fin:
            SUMMARY_PATTERN = "========= Summary results of "
            GENERATOR_HEADER = 'Generator'
            NB_STAT_HEADER = 'Number of statistics'
            LAST_HEADER = "Total CPU time"
            SUCCESS_PATTERN = 'All tests were passed'
            FAILURE_PATTERN = 'All other tests were passed'
            FAILURE_DETAIL_PATTERN = '----------'
            state = 'search_summary'
            crush_type = None
            test_status = None
            headers = {}
            failures = []
            for line in fin:
                line = line.strip()
                if line.startswith(SUMMARY_PATTERN):
                    state = 'in_summary_header'
                    crush_type = line[len(SUMMARY_PATTERN):].strip().split()[0]
                elif state == 'in_summary_header':
                    if line:
                        header, value = line.split(":", 1)
                        header, value = header.strip(), value.strip()
                        headers[header] = value
                        if header == LAST_HEADER:
                            state = 'search_failure_detail'
                elif state == 'search_failure_detail':
                    if line == SUCCESS_PATTERN:
                        test_status = 'pass'
                        break
                    elif line.startswith(FAILURE_DETAIL_PATTERN):
                        state = 'in_failure_detail'
                elif state == 'in_failure_detail':
                    if line == FAILURE_PATTERN:
                        test_status = 'fail'
                        break
                    if line and not line.startswith(FAILURE_DETAIL_PATTERN):
                        parts = line.split()
                        test_id = parts[0]
                        last_name_idx = -1
                        if parts[-2] == '-':
                            last_name_idx = -3
                        name = ' '.join( parts[1:last_name_idx] )
                        p_value = ' '.join( parts[last_name_idx:])
                        is_suspect = 'eps' not in p_value
                        failure = Failure( test_id, name, p_value, is_suspect)
                        failures.append( failure )
            if test_status is None or crush_type is None:
                print( "SKIPPING %s, no result found" % path )
                continue
            if len(failures) and all(f.is_suspect for f in failures):
                test_status = 'suspect' # all failures are "just" suspect
            nb_fail = len([f for f in failures if not f.is_suspect])
            nb_suspect = len([f for f in failures if f.is_suspect])
            result = TestResult(generator=headers[GENERATOR_HEADER],
                                crush_type=crush_type,
                                nb_stat=headers[NB_STAT_HEADER],
                                status=test_status,
                                failures=tuple(failures),
                                nb_fail=nb_fail,
                                nb_suspect=nb_suspect)
            if result.crush_type in results_by_crush_by_rng[result.generator]:
                print( "DUPLICATE result for %s with %s " % (result.generator, result.crush_type) )
                print( "aborting...")
                sys.exit(1)
            results_by_crush_by_rng[result.generator][result.crush_type] = result
            all_results.append( result )

    CRUSH_TYPES = "SmallCrush Crush BigCrush".split()
    table_rows = []
    table_rows.append( ["RNG"] + CRUSH_TYPES )
    for rng in sorted(results_by_crush_by_rng):
        row = [rng]
        for crush_type in CRUSH_TYPES:
            result = results_by_crush_by_rng[rng].get(crush_type)
            status = result.status if result else ''
            if status == 'pass':
                status = 'OK'
            elif status:
                parts = []
                if result.nb_suspect:
                    parts.append( '%d suspect' % result.nb_suspect )
                if result.nb_fail:
                    parts.append( '%d fail' % result.nb_fail )
                status = ', '.join( parts )
            row.append( status )
        table_rows.append( row )


    print_table(table_rows, sys.stdout)

    json_path = os.path.join(RESULT_DIR, 'all_results.json')
    with open(json_path, 'wt') as fjson:
        sorted_results = sorted(all_results, key=lambda r: (r.generator, CRUSH_TYPES.index(r.crush_type)))
        def to_json(r):
            d = r._asdict()
            d['failures'] = [f._asdict() for f in d['failures']]
            return d
        json_results = [to_json(r) for r in sorted_results]
        json.dump(json_results, fjson, sort_keys=True, indent=4)

if __name__ == '__main__':
    main()