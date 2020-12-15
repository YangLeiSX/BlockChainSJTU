#!/bin/bash
timestamp=`date "+%m%d_%H%M%S"`

if [ -e result.csv ] 
then
    rm result.csv
fi
make vanilla_run
python3 util/visual_vanilla.py -t $timestamp -d