#!/bin/bash

dname=/data/corpus/ntcir/ntcir8/topics/CS; for i in `ls $dname`; do java -cp stanford-postagger.jar edu.stanford.nlp.tagger.maxent.MaxentTagger -model models/bidirectional-distsim-wsj-0-18.tagger -textFile $dname.PRUNED/$i 1>$dname.PRUNED.TAGGED/$i 2>/dev/null; done