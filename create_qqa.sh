#!/bin/bash

#./qqa /data/corpus/ntcir/ntcir8/topics/CT.TAGGED/* > /data/corpus/ntcir/ntcir8/topics/ACLIA2-CT-QA-V2.txt
./qqa -topic /data/corpus/ntcir/ntcir8/topics/ACLIA2-CS-T.xml /data/corpus/ntcir/ntcir8/topics/CS.PRUNED.TAGGED/* > /data/corpus/ntcir/ntcir8/topics/ACLIA2-CS-QA-V2.txt
#./qqa /data/corpus/ntcir/ntcir8/topics/CS.TAGGED/* > /data/corpus/ntcir/ntcir8/topics/ACLIA2-CS-QA.txt
