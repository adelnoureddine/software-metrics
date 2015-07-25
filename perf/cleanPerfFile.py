#
# Copyright (c) 2015, Adel Noureddine
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the <organization> nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author : Adel Noureddine
#

import sys
import os
import glob
from collections import OrderedDict
import os.path

directory = sys.argv[1]

for filename in glob.glob(os.path.join(directory, '*.csv')):
    completeCSVData = OrderedDict([
        ("cpu-cycles", []),
        ("instructions", []),
        ("cache-references", []),
        ("cache-misses", []),
        ("branch-instructions", []),
        ("branch-misses", []),
        ("bus-cycles", []),
        ("ref-cycles", []),
        ("cpu-clock", []),
        ("task-clock", []),
        ("page-faults", []),
        ("context-switches", []),
        ("cpu-migrations", []),
        ("minor-faults", []),
        ("major-faults", []),
        ("alignment-faults", []),
        ("emulation-faults", []),
        ("dummy", []),
        ("L1-dcache-loads", []),
        ("L1-dcache-load-misses", []),
        ("L1-dcache-stores", []),
        ("L1-dcache-store-misses", []),
        ("L1-dcache-prefetches", []),
        ("L1-icache-loads", []),
        ("L1-icache-load-misses", []),
        ("LLC-loads", []),
        ("LLC-load-misses", []),
        ("LLC-stores", []),
        ("LLC-store-misses", []),
        ("dTLB-loads", []),
        ("dTLB-load-misses", []),
        ("dTLB-stores", []),
        ("dTLB-store-misses", []),
        ("iTLB-loads", []),
        ("iTLB-load-misses", []),
        ("branch-loads", []),
        ("branch-load-misses", []),
    ])

    f = open(filename)
    lineNum = 1
    for line in f:
        if lineNum < 40:
            line = line.strip()
            parts = line.split(";")
            if len(parts) > 1:
                if parts[2] in completeCSVData:
                    completeCSVData[parts[2]].append(parts[0])
        lineNum += 1
    fileToWrite = directory + "perf-cleaned/" + os.path.basename(filename)
    cf = open(fileToWrite, "w")
    for key in completeCSVData:
        cf.write(key + ";" + ";".join(completeCSVData[key]) + "\n")
    cf.close