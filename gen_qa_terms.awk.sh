#!/bin/bash

./gen_qa_terms.sh | awk 'BEGIN { name=""; oct=""; line=0; a=0} { line=(NR - 1)%3; if (line == 0) name=$2; if (line == 1) oct=$2; if (line == 2) print oct ", // " name}'

