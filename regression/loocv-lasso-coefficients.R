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

library(glmnet)

setwd("/home/adel/WorkArea/Metrics")

benchNames = c("parsec2")

for (benchName in benchNames) {
  # Data
  dataIn = read.csv(paste("data/", benchName, ".csv", sep=""), header = TRUE, sep = ",")
  
  # Data discarding rows with at least one NA value
  dataInNoNA = dataIn[rowSums(is.na(dataIn)) == 0,]
  
  Y <- as.matrix(dataInNoNA[,1]) # First column = energy
  X <- as.matrix(dataInNoNA[,2:106]) # All other column
  
  # Fit the model, alpha=1 for Lasso
  resFit = glmnet(X, Y, alpha=0.5)
  
  # Coefficients for all s
  resCoefFit = coef(resFit, s=NULL)
  
  # Convert coefficients dgcMatrix to matrix
  as.matrix
  resCoefFit_matrix = as(resCoefFit, "matrix")
  
  # Number of non zero rows in each S column
  nonZeroCS = colSums(resCoefFit_matrix != 0)
  
  # Number of non zero columns in each attribute
  nonZeroRS = rowSums(resCoefFit_matrix != 0)
  
  # Write matrices to file
  write.table(resCoefFit_matrix, file=paste("coefficients/", benchName, "/resCoefFit.csv", sep=""), row.names=TRUE, col.names=TRUE, sep=",")
  write.table(nonZeroCS, file=paste("coefficients/", benchName, "/nonZeroCS.csv", sep=""), row.names=TRUE, col.names=TRUE, sep=",")
  write.table(nonZeroRS, file=paste("coefficients/", benchName, "/nonZeroRS.csv", sep=""), row.names=TRUE, col.names=TRUE, sep=",")
  
  # View coefficients matrix
  View(resCoefFit_matrix)
}