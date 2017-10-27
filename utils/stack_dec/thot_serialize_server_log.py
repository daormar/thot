#!/usr/bin/env python

# thot package for statistical machine translation
# Copyright (C) 2017 Adam Harasimowicz
 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
 
# You should have received a copy of the GNU Lesser General Public License
# along with this program; If not, see <http://www.gnu.org/licenses/>.

"""
Converts log file generated by Thot server to serialize output
from concurrent threads and make it more readable.
"""

import argparse
import re
from collections import defaultdict


def extract_thread_id(line):
    """
    Look for thread ID in passed log line. If there is not the ID
    then it returns None.
    """
    match = re.match('0x[0-9a-f]+\t', line)

    if match:
        return match.group(0)
    else:
        return None


def is_last_request_line(line):
    match = re.search('0x[0-9a-f]+\tElapsed time: [0-9]+\.[0-9]+ secs', line)

    return match is not None


def group_thread_output(logFileName):
    groupedOutput = defaultdict(list)

    with open(logFileName, 'r') as f:
        for line in f:
            tid = extract_thread_id(line)
            if tid is None:
                print line,
            else:
                groupedOutput[tid].append(line)

                # Print request if it is the last line
                if is_last_request_line(line):
                    for tl in groupedOutput[tid]:
                        print tl,
                    del groupedOutput[tid]

    # Print not complete requests
    if len(groupedOutput) > 0:
        print 'WARNING: Some uncompleted information about requests left'
        for _, l in groupedOutput.items():
            print l


if __name__ == '__main__':
    # Parse input arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('logfile', help='Path to log file to convert')
    args = parser.parse_args()

    group_thread_output(args.logfile)