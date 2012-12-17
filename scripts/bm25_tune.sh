#!/bin/bash

for i in {1..10}
do
   for j in {1..10}
   do
     run_id=`basename $1`-`expr $i - 1``expr $j - 1`
     if ! [  -f $run_id ]; then
        echo "No such file: $run_id"
        exit
     fi
     ./split_run.sh $run_id
     ./eval.sh $run_id
   done
done

