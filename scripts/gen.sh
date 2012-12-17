#!/bin/bash

trec_output=QUTIS-EN-CT-01-T.TXT

/home/monfee/workspace/ntcir/qtrec -Se -topic /data/corpus/ntcir/ntcir8/topics/ACLIA2-CT-T.xml -run QUTIS-EN-CT-01-T -n 200 > $trec_output
