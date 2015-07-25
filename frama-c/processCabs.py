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
#fileToRead = directory + "/sm-cabs.txt"
fileToWrite = fileToRead + ".csv"
f = open(fileToRead)
cf = open(fileToWrite, "w")
programVolume = 0
programDifficulty = 0
for line in f:
    line = line.strip()
    parts = line.split(":")
    if len(parts) > 1:
        name = parts[0]
        value = parts[1]
        if name == "Total operators":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Distinct operators":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Total_operands":
            cf.write("Total operands;" + value.strip() + "\n")
        elif name == "Distinct operands":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Program length":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Vocabulary size":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Program volume":
            cf.write(name + ";" + value.strip() + "\n")
            programVolume = value.strip()
        elif name == "Effort":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Program level":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Difficulty level":
            cf.write(name + ";" + value.strip() + "\n")
            programDifficulty = value.strip()
        elif name == "Time to implement":
            cf.write(name + ";" + value.strip() + "\n")
        elif name == "Bugs delivered":
            cf.write(name + ";" + value.strip() + "\n")
programIntelligentContent = float(programVolume) / float(programDifficulty)
cf.write("Intelligent Content;" + str(programIntelligentContent))
cf.close()
f.close()