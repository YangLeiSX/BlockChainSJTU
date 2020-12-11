#!/bin/bash
timestamp=`date "+%m%d_%H%M%S"`

if [ -e result.csv ] 
then
    rm result.csv
fi
make selfish_run
python3 util/visual_selfish.py -t $timestamp -d