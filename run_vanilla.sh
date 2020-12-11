#!/bin/bash
timestamp=`date "+%m%d_%H%M%S"`

rm result.csv
make vanilla_run
python3 util/visual_vanilla.py -t $timestamp -d