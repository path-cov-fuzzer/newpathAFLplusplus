#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <unordered_map>

// definition of the binary used to store CFG ------------------------------------------- start
#define FUNCNAME_LEN 256

// function_name: name of the function
// entry: entry block's BBID
// exit: exit block's BBID (ignore)
typedef struct CFG {
  char function_name[FUNCNAME_LEN];  // 函数的字符串形式，整数形式就是 cfg_arr 的下标
  int entry;                        // 整数，表示函数入点 block
  int exit;                         // 整数，表示函数出点 block
} CFG;

// calls: -1 represents not calling functions. -2 represents call library functions. >=0 represents called functions' ID
// successor_size: represents the number of successors of this block
// successors_arr: the array of successors
typedef struct BlockEntry {
  int calls;                   
  int successor_size;
  int *successors_arr;         
} BlockEntry;

// WENOTE: this is the Top_Level of CFG-stored binary
// cfg_size: the number of cfg/functions in PUT
// cfg_arr: the array of cfgs. Indexes of cfg_arr is funcID.
// block_size: the total number of blocks in PUT
// block_arr: the array of blocks. Indexes of block_arr is BBID. NULL represents corresponding block is not in PUT
typedef struct Top_Level {
  int cfg_size;          
  CFG *cfg_arr;          
  int block_size;         
  BlockEntry **block_arr;  
} Top_Level;
// definition of the binary used to store CFG ------------------------------------------- end

// functions used to debug CFG-stored binary ---------------------------------------------------------- start
void dump_block(BlockEntry *block) {
    std::cout << "block->calls = " << block->calls << std::endl;
    std::cout << "block->successor_size = " << block->successor_size << std::endl;
    for (int i = 0; i < block->successor_size; i++) {
        printf("block->successor_arr[%d] = %d\n", i, block->successors_arr[i]);
    }
}

void dump_cfg(CFG *cfg) {
    std::string funcname(cfg->function_name);
    std::cout << "cfg->function_name = " << funcname << std::endl;
    std::cout << "cfg->entry = " << cfg->entry << std::endl;
    std::cout << "cfg->exit = " << cfg->exit << std::endl;
}

void dump_top(Top_Level *top) {
    std::cout << "top->cfg_size = " << top->cfg_size << std::endl;
    std::cout << "top->block_size = " << top->block_size << std::endl;
    for(int i = 0; i < top->cfg_size; i++) {
        dump_cfg(&(top->cfg_arr[i]));
    }
    for(int i = 0; i < top->block_size; i++) {
        if(top->block_arr[i]) {
            std::cout << "BB " << i << std::endl;
            dump_block(top->block_arr[i]);
        }
    }
}
// functions used to debug CFG-stored binary ---------------------------------------------------------- end

// function used to store CFGs-binary on disk ---------------------------------------------------------- start
void store_top(Top_Level *top) {
    // The default name of CFG-binary is "top.bin"
    FILE *file = fopen("top.bin", "wb");
    assert(file);
    // write TopLevel structure at the beginning of "top.bin"
    fwrite(top, sizeof(Top_Level), 1, file);
    // cfg_arr follows then
    fwrite(top->cfg_arr, sizeof(CFG) * top->cfg_size, 1, file);

    // count the number of blocks of the PUT
    int count = 0;
    for(int i = 0; i < top->block_size; i++) {
        if(top->block_arr[i]) {
            count++;
        }
    }
    // the number of blocks follows then
    fwrite(&count, sizeof(int), 1, file);

    // the contents of all blocks follow then
    for(int i = 0; i < top->block_size; i++) {
        if(top->block_arr[i]) {
            fwrite(&i, sizeof(int), 1, file);
            fwrite(top->block_arr[i], sizeof(BlockEntry), 1, file);
        }
    }

    // successors of each block follow then
    for(int i = 0; i < top->block_size; i++) {
        if(top->block_arr[i]) {
            fwrite(&i, sizeof(int), 1, file);
            fwrite(top->block_arr[i]->successors_arr, sizeof(int) * top->block_arr[i]->successor_size, 1, file);
        }
    }

    // close the file
    fclose(file);
    // complete storing
    printf("Data has been dumped into top.bin\n");
}
// function used to store CFGs-binary on disk ---------------------------------------------------------- end

// Remove leading and trailing whitespace characters from a string.
std::string trim(const std::string& str) {
    // find the pos of the first non-whitespace-char
    size_t first = str.find_first_not_of(" \t\n\r");
    // find the pos of the last non-whitespace-char
    size_t last = str.find_last_not_of(" \t\n\r");
    // if there is no non-whitespace-char
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    // else, return str[first : last]
    return str.substr(first, last - first + 1);
}

int main() {
    // the generated binary
    Top_Level top;

    // read bbnum.txt to know total number of blocks in PUT.
    // so that we can assign memory space to top.block_arr
    int numBB = -1;
    FILE *file = fopen("bbnum.txt", "r");
    assert(file);
    assert(fscanf(file, "%d", &numBB) == 1);
    fclose(file);
    top.block_size = numBB;
    top.block_arr = (BlockEntry **) malloc(sizeof(BlockEntry *) * numBB);
    memset(top.block_arr, 0, sizeof(BlockEntry *) * numBB);

    // 1. construct a mapping <funcname, funcIntegerID> using function_list.txt
    // 2. get the total number of functions to assign memory space for cfg_arr
    // the mapping data structure
    std::unordered_map<std::string, int> funcname_funcID;
    // open function_list.txt
    std::ifstream function_list_file("function_list.txt"); 
    assert(function_list_file.is_open()); 

    std::string line;           
    std::string firstPart;
    std::string secondPart;
    // read each line of function_list.txt
    // eachline: funcIntegerID [space] funcName
    while (std::getline(function_list_file, line)) {  
        // delete leading/trailing whitespaces
        std::string input = trim(line);
        // find the pos of the first space
        size_t pos = input.find(' ');            
        assert(pos != std::string::npos);

        // get the funcIntegerID
        firstPart = input.substr(0, pos);   
        // get the funcName
        secondPart = input.substr(pos + 1); 

        // construct the mapping
        funcname_funcID[secondPart] = std::stoi(firstPart);
    }

    // total number of functions = "the last funcIntegerID" + 1
    int numfunctions = std::stoi(firstPart) + 1;
    // assign memory space for cfg_arr
    top.cfg_size = numfunctions;
    top.cfg_arr = (CFG *) malloc(sizeof(CFG) * numfunctions);
    // close function_list.txt
    function_list_file.close(); 

    // reaching here, we already fill TopLevel structure itself
    // now we need to fill BlockEntry array and CFG array

    // fill BlockEntry array's "calls" ---------------------------------------------------------------------------- start 
    // open callmap_filtered.txt
    std::ifstream callmap_file("callmap_filtered.txt"); 
    assert(callmap_file.is_open()); // 检查文件是否成功打开
    // read each line
    // eachline: BlockIntegerID [space] Calls [space] [FuncName]
    // or
    //           BlockIntegerID (if this block doest not call functions)
    line = "";
    while (std::getline(callmap_file, line)) {  
        // remove leading/trailing whitespaces
        std::string input = trim(line);
        // get the pos of the first space
        size_t pos = input.find(' ');            
        // if space is found, then this is a calling block
        if (pos != std::string::npos) {          
            // get block ID
            firstPart = input.substr(0, pos);    
            // get "Calls [FuncName]"
            secondPart = input.substr(pos + 1);  
        }
        else {
            // get block ID
            firstPart = input;
            secondPart = "";
        }

        // assign memory space to corresponding index in BlockEntry* array.
        top.block_arr[std::stoi(firstPart)] = (BlockEntry *) malloc(sizeof(BlockEntry));

        // get the pos of "Call" in the secondPart
        size_t foundCall = secondPart.find("Calls");   

        // if "Call" found in secondPart, this block is a calling block
        if (foundCall != std::string::npos) {  
            // get the callee funcName
            size_t pos = secondPart.find("Calls ");  
            assert(pos != std::string::npos);
            std::string funcName = secondPart.substr(pos + std::strlen("Calls "));  

            // if funcName exits in function_list.txt, it means this is a function defined by PUT itself
            if (funcname_funcID.count(funcName) > 0) {
                top.block_arr[std::stoi(firstPart)]->calls = funcname_funcID[funcName];
            } else {
                // else, it means this function is a C libray function or a system call API
                top.block_arr[std::stoi(firstPart)]->calls = -2; 
            }
        } else {  
            // if "Call" is not found in secondPart, this block is just a normal block
            top.block_arr[std::stoi(firstPart)]->calls = -1; 
        }  
    }
    // close callmap_filtered.txt
    callmap_file.close(); 
    // fill BlockEntry array's "calls" ---------------------------------------------------------------------------- end 

    // fill CFG array ---------------------------------------------------------------------------------------- start
    line = "";
    firstPart = "";
    secondPart = "";
    // open cfg_filtered.txt
    std::ifstream cfg_file("cfg_filtered.txt"); 
    assert(cfg_file.is_open());

    int func_id = 0;
    bool lookingForEntry = false;   

    // read each line
    // 1. Function: FuncName
    // 2. BasicBLock: blockID
    // 3. Successors: blockID blockID ...  or empty
    // 4. just a newline '\n'
    while (std::getline(cfg_file, line)) { 
        // remove leading/trailing whitespaces
        line = trim(line);
        // find pos of 1st space
        size_t pos = line.find(' ');            
        // part before 1st space
        firstPart = line.substr(0, pos);
        // part after 1st space
        secondPart = line.substr(pos + 1);      

        // 1. Function: FuncName
        // firstPart: "Function:"
        // secondPart: "FuncName:"
        if (firstPart == "Function:") {
            // fill funcName
            strcpy(top.cfg_arr[func_id].function_name, secondPart.c_str());
            // set flag to tell we are looking for entryBlock
            lookingForEntry = true;
            // increase FuncID number 
            func_id++;
        }
        else if (firstPart == "BasicBlock:") {
            // 2. BasicBLock: blockID
            // firstPart: "BasicBlock:"
            // secondPart: "blockID"
            // if we are looking for entryBlock, set it
            if(lookingForEntry) {
                top.cfg_arr[func_id - 1].entry = std::stoi(secondPart);
                lookingForEntry = false;
            }
        }
        else if (firstPart == "Successors:") { 
            // 3. Successors: blockID blockID ...  or empty
            // firstPart: "Successors:"
            // secondPart: "blockID blockID ...  or empty"
            // do nothing, just skip this line
        }
        else {
            // 4. just a newline '\n'
            // do nothing, read next line
        }
    }

    cfg_file.close();
    // fill CFG array ---------------------------------------------------------------------------------------- end

    // fill BlockEntry array's successors ---------------------------------------------------------------------------------------- start
    line = "";
    firstPart = "";
    secondPart = "";
    // open cfg_filtered.txt
    std::ifstream cfg_file("cfg_filtered.txt"); 
    assert(cfg_file.is_open());

    int curBBID = -1;

    // read each line
    // 1. Function: FuncName
    // 2. BasicBLock: blockID
    // 3. Successors: blockID blockID ...  or empty
    // 4. just a newline '\n'
    while (std::getline(cfg_file, line)) { 
        // remove leading/trailing whitespaces
        line = trim(line);
        // find pos of 1st space
        size_t pos = line.find(' ');            
        // part before 1st space
        firstPart = line.substr(0, pos);
        // part after 1st space
        secondPart = line.substr(pos + 1);      

        // 1. Function: FuncName
        // firstPart: "Function:"
        // secondPart: "FuncName:"
        if (firstPart == "Function:") {
            // just skip this line
        }
        else if (firstPart == "BasicBlock:") {
            // 2. BasicBLock: blockID
            // firstPart: "BasicBlock:"
            // secondPart: "blockID"
            curBBID = std::stoi(secondPart);
        }
        else if (firstPart == "Successors:") { 
            // 3. Successors: blockID blockID ...  or empty
            // firstPart: "Successors:"
            // secondPart: "blockID blockID ...  or empty"
            std::vector <std::string> blockID_string_list;
            std::vector <int> blockID_integer_list;

            // push blockID list (string format) into vector
            size_t start = 0, end = 0;
            while ((end = secondPart.find(' ', start)) != std::string::npos) {
                blockID_string_list.push_back(secondPart.substr(start, end - start));
                start = end + 1;
            }
            blockID_string_list.push_back(secondPart.substr(start)); // add the last blockID

            // convert blockID(string format) vector to blockID(integer format) vector
            for (const auto& blockID_str : blockID_string_list) {
                assert(!blockID_str.empty() && blockID_str != "Successors:");
                int num = std::stoi(blockID_str);
                blockID_integer_list.push_back(num);
            }

            // memory space should be assigned when reading callmap.txt
            assert(top.block_arr[curBBID]);
            // fill this block's successors' information
            top.block_arr[curBBID]->successor_size = blockID_integer_list.size();
            top.block_arr[curBBID]->successors_arr = (int *) malloc(sizeof(int) * blockID_integer_list.size());
            for(int i = 0; i < blockID_integer_list.size(); i++) {
                top.block_arr[curBBID]->successors_arr[i] = blockID_integer_list[i];
            }
        }
        else {
            // 4. just a newline '\n'
            // do nothing, read next line
        }
    }

    cfg_file.close();
    // fill BlockEntry array's successors ---------------------------------------------------------------------------------------- end

    // dump CFG-binary's readable format
    dump_top(&top);
    // store its content in a file named "top.bin"
    store_top(&top);

    return 0;
}

