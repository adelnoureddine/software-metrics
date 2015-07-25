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
  
  # Cross-validation (LOOCV)
  resCV = cv.glmnet(X, Y, nfolds=nrow(dataInNoNA), alpha=0.5)
  
  pdf(paste("graphs2/", benchName, ".pdf", sep=""))
  plot(resCV)
  dev.off()
  
  minLambda = resCV$lambda.min
  predictRes = predict(resCV, s=minLambda, type="coefficients")
  
  indexMinLambda = match(resCV$lambda.min, resCV$lambda) # Index of lambda min
  minLambdaCVM = resCV$cvm[indexMinLambda] # CVM for lambda min
  rmse = sqrt(minLambdaCVM) # Root mean squared error
  print(rmse)
  
  sink(paste("rmse2/R/", benchName, ".csv", sep=""))
  cat("rmse,",rmse, sep='')
  sink()
}