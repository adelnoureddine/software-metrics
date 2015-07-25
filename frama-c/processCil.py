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
#directory = sys.argv[1]
fileToRead = sys.argv[1]
#fileToRead = directory + "/sm-cil.txt"
fileToWrite = fileToRead + ".csv"
f = open(fileToRead)
cf = open(fileToWrite, "w")
for line in f:
    line = line.strip()
    parts = line.split("=")
    if len(parts) > 1:
        name = parts[0]
        value = parts[1]
        if name.strip() == "Sloc":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Decision point":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Global variables":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "If":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Loop":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Goto":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Assignment":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Exit point":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Function":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Function call":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Pointer dereferencing":
            cf.write(name.strip() + ";" + value.strip() + "\n")
        elif name.strip() == "Cyclomatic complexity":
            cf.write(name.strip() + ";" + value.strip() + "\n")
cf.close()
f.close()