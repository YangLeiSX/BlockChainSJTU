#!/bin/bash
timestamp=`date "+%m%d_%H%M%S"`

rm result.csv
make forker_run
python3 util/visual_forker.py -t $timestamp