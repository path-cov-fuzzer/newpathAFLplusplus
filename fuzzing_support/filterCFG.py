import sys
import re

assert(len(sys.argv) == 4)

# python3 filterCFG.py cfg.txt PUT_decomp.txt cfg_filtered.txt
print(f"arg1: {sys.argv[1]}")
print(f"arg2: {sys.argv[2]}")
print(f"arg3: {sys.argv[3]}")

# The approach for filtering the CFG.
# 1. read cfg.txt into memory, stored as a dict {key="BlockID of the first block", value=[false, "CFG text"]}
# 2. scan PUT_decomp.txt, observe each function:
#       1. if there is no "path_inject_eachbb", then skip this function
#       2. if there is "path_inject_eachbb", get the argument of the first "path_inject_eachbb"
#           then set dict[argument(blockID)][0] = true
# 3. dump the dict entries if dict[i][0] = true

# 1. read cfg.txt into memory, stored as a dict {key="BlockID of the first block", value=[false, "CFG text"]}
# store all CFGs
wholeCFG = {}
with open(sys.argv[1], 'r', encoding='utf-8') as file:
    singleCFG = []
    firstBBID = -1
    # indicate whether is looking for the first BBID
    lookingForFirstBBID = False 
    for line in file:
        # remove leading/trailing whitespaces
        line = line.strip()
        # if nothing left, this is a newline, skip
        if not line:
            continue

        if "Function: " in line:
            # if singleCFG is not empty, it means we finish recording a CFG, store it to wholeCFG
            if singleCFG:
                wholeCFG[firstBBID] = [False, singleCFG]
            # reset singleCFG, and set lookingForFirstBBID
            singleCFG = []
            lookingForFirstBBID = True
        else:
            if lookingForFirstBBID:
                # record 1st BBID
                match = re.search(r'BasicBlock: (\d+)', line)
                assert(match)
                lookingForFirstBBID = False
                firstBBID = int(match.group(1))
            else:
                # skip everyline after record 1st BBID
                pass # do nothing

        # record this CFG 
        singleCFG.append(line)

    file.close()
# after scanning all lines, there is still one CFG which is not in wholeCFG
# add it in
wholeCFG[firstBBID] = [False, singleCFG]

# 2. scan PUT_decomp.txt, observe each function:
#       1. if there is no "path_inject_eachbb", then skip this function
#       2. if there is "path_inject_eachbb", get the argument of the first "path_inject_eachbb"
#           then set dict[argument(blockID)][0] = true
# indicate whether is looking for "path_inject_eachbb"
lookingForInstrumented = False 
with open(sys.argv[2], 'r', encoding='utf-8') as file:
    # record last line of the current reading line
    previousline = None
    # read each line of PUT_decomp.txt
    for line in file:
        # remove leading/trailing whitespaces
        line = line.strip()
        # match function head in PUT's decompile result
        funcNameMatch = re.match(r'^[0-9a-fA-F]+ <[^>]+>:', line)
        if funcNameMatch:
            # set flag: we are looking for the first 'path_inject_eachbb'
            lookingForInstrumented = True
        elif lookingForInstrumented:
            path_inject_match = re.search(r'callq?\s+[0-9a-fA-F]+\s+<path_inject_eachbb>', line)
            if path_inject_match:
                # find 'callq path_inject_eachbb'
                # now, here are two possibilities：
                #     1. xor    %edi,%edi
                #     2. mov    $0x3208,%edi
                print("current line == 'path_inject_eachbb' !")
                xor_match = re.search(r'xor\s+%edi,%edi', previousline)
                mov_match = re.search(r'mov\s+\$(0x[0-9a-fA-F]+),%edi', previousline)
                assert(xor_match or mov_match)
                assert(not (xor_match and mov_match))
                if xor_match:
                    # if previousline == "xor    %edi,%edi", then arg = 0
                    arg = 0
                else:
                    # if previousline == "mov    $0x3208,%edi", then arg = 0x3208
                    arg = int(mov_match.group(1), 16)
                print(f"arg: {arg}")
                # set corresponding singleCFG in wholeCFG as True
                wholeCFG[arg][0] = True
                # already find the first 'path_inject_eachbb', do not look for it anymore
                lookingForInstrumented = False
            else:
                # if this line is not 'callq path_inject_eachbb', then just skip
                pass
        else:
            # readching here means the first 'path_inject_eachbb' of this function has been found
            # just skip this line, until we met next funciton head
            pass
        # record this line, so that previousline == "mov    $0x3208,%edi" when line == "callq path_inject_eachbb"
        previousline = line
    file.close()

# 3. dump the dict entries if dict[i][0] = true
with open(sys.argv[3], "w") as file:
    for key, value in wholeCFG.items():
        if value[0]:
            for line in value[1]:
                file.write(line + "\n")
            file.write("\n")
    file.close()







