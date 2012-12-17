#!/bin/bash
#
# Usage: $0 run_name
#

ls $1 | IR4QA-splitruns2 /data/corpus/ntcir/ntcir8/qrels/ACLIA2-CS.qrels.tid
