#!/bin/bash

#while read line; do
#    echo "line = $line"
#done
tmp_file=/tmp/run_scores

if ! [ -f $tmp_file ]; then
  ./bm25_tune.sh | grep " AP " > $tmp_file
fi
max=`cat $tmp_file | awk '
BEGIN {h=0}
{
  if ($3 > h) 
    h = $3
}
END {print h}
'`
run=`grep $max $tmp_file | awk '{print $1}'`

echo "Run with highest AP($max): $run" 
