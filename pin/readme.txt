make all TARGET=intel64
../../../pin -t obj-intel64/softCounter.so -- /bin/ls
../../../pin -t obj-intel64/softCounter.so -o output.log -- /bin/ls