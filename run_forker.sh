#!/bin/bash
timestamp=`date "+%m%d_%H%M%S"`

if [ -e result.csv ] 
then
    rm result.csv
fi
make forker_run
python3 util/visual_forker.py -t $timestamp -d