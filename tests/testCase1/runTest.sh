#/bin/bash

for i in `seq 1 10`;
do
    ./client.tcl $i 50  > log_runTest_$i &
    sleep 1
done    
