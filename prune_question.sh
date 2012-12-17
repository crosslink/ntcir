#!/bin/bash

echo "dname=/data/corpus/ntcir/ntcir8/topics/CS; for i in `ls $dname`; do ./prune_question -lang en $dname/$i > $dname.PRUNED/$i ; done"