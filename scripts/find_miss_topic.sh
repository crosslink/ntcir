#!/bin/bash

for i in {1..100}
do
array[${i}]=0
done

for j in `grep -o ACLIA2-CT-[0-9][0-9][0-9][0-9] $1`
do
  val=`echo $j | cut -f 3 -d -`
  val=`expr $val + 0`
  #echo $val
  array[$val]=$val
done

for i in {1..100}
do
  if [ "${array[${i}]}" -eq "0" ]; then
     echo "Topic $i is missing"
  fi
done
