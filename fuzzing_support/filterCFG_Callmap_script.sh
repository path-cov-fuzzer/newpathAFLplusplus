#!/bin/bash
set -e

##
# Pre-requirements:
# - env FUZZER: path to fuzzer work dir
# - env TARGET: path to target work dir
# - env OUT: path to directory where artifacts are stored
# - env SHARED: path to directory shared with host (to store results)
# - env PROGRAM: name of program to run (should be found in $OUT)
# - env ARGS: extra arguments to pass to the program
# - env FUZZARGS: extra arguments to pass to the fuzzer
##

# 获取 PUT 的反汇编结果
objdump -d $OUT/afl/$PROGRAM > $OUT/PUT_decomp.txt

# 运行过滤CFG python 脚本
python3 $FUZZER/filterCFG.py $OUT/cfg.txt $OUT/PUT_decomp.txt $OUT/cfg_filtered.txt

# 运行过滤Callmap python 脚本
python3 $FUZZER/filterCallmap.py $OUT/cfg_filtered.txt $OUT/callmap.txt $OUT/callmap_filtered.txt

