#!/bin/bash -e

# The purpose of this script is to remove parts from cfg.txt and callmap.txt that do 
# not belong to the PUT (specified by the first argument), and generate cfg_filtered.txt 
# and callmap_filtered.txt.

# - env FUZZER: path to fuzzer work dir

# generate de-compile result of PUT
objdump -d $1 > ./PUT_decomp.txt

# generate cfg_filtered.txt
python3 $FUZZER/fuzzing_support/filterCFG.py ./cfg.txt ./PUT_decomp.txt ./cfg_filtered.txt

# generate callmap_filtered.txt
python3 $FUZZER/fuzzing_support/filterCallmap.py ./cfg_filtered.txt ./callmap.txt ./callmap_filtered.txt
