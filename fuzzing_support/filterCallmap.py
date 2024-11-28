import sys
import re

assert(len(sys.argv) == 4)

# python3 filterCallmap.py cfg_filtered.txt callmap.txt callmap_filtered.txt
print(f"arg1: {sys.argv[1]}")
print(f"arg2: {sys.argv[2]}")
print(f"arg3: {sys.argv[3]}")

# The approach for filtering the Callmap.
# 1. reading cfg_filtered.txt, scan each BasicBlock's BBID into a dict
# 2. scan each line of callmap.txt, if BBID of this line exits in dict, then dump this line to callmap_filtered.txt

# 1. reading cfg_filtered.txt, scan each BasicBlock's BBID into a dict
bbdict = {}
with open(sys.argv[1], 'r', encoding='utf-8') as file:
    # read each line
    for line in file:
        # remove leading/trailing whitespaces
        line = line.strip()
        BasicBlock_match = re.search(r'BasicBlock: (\d+)', line)
        if BasicBlock_match:
            # if this line is "BasicBlock: integer", then record this integer into dict
            bbid = int(BasicBlock_match.group(1))
            bbdict[bbid] = 1
        else:
            # if not, skip this line
            pass

# 2. scan each line of callmap.txt, if BBID of this line exits in dict, then dump this line to callmap_filtered.txt
# open writting privilege of "callmap_filtered.txt"
written_file = open(sys.argv[3], 'w')
# open reading privilege of "callmap.txt"
with open(sys.argv[2], 'r', encoding='utf-8') as file:
    # read each line of "callmap.txt"
    for line in file:
        # remove leading/trailing whitespaces
        line = line.strip()
        # parse the BBID of this line
        bbid_match = re.search(r'(\d+).*', line)
        bbid = int(bbid_match.group(1))
        # if BBID exits in bbdict, then dump this line to "callmap_filtered.txt"
        if bbid in bbdict:
            written_file.write(line + "\n")

