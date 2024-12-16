#ifndef CFGBINARY_H
#define CFGBINARY_H

#include <stdlib.h>

// definition of the binary used to store CFG ------------------------------------------- start
#define FUNCNAME_LEN 256

// function_name: name of the function
// entry: entry block's BBID
// exit: exit block's BBID (ignore)
typedef struct CFG {
  char function_name[FUNCNAME_LEN];  
  int entry;                        
  int exit;                         
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

// This function parse a cfg binary indicated by argument "filename", then returns the pointer of Top_Level
Top_Level *load_top(const char *filename) {
    // first assign memory of Top_Level
    Top_Level *top = (Top_Level *)malloc(sizeof(Top_Level));
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        // read TopLevel structure at the beginning of "top.bin"
        fread(top, sizeof(Top_Level), 1, file);
        // assign memory to cfg_arr, and relative information
        // WENOTE: CFG funcname will not be read, but its OKAY since it is useless
        top->cfg_arr = (CFG *)malloc(sizeof(CFG) * top->cfg_size);
        fread(top->cfg_arr, sizeof(CFG) * top->cfg_size, 1, file);

        // read the number of non-empty blocks
        int count = 0;
        fread(&count, sizeof(int), 1, file);

        // assign memory and initialize block_arr
        top->block_arr = (BlockEntry **)malloc(sizeof(BlockEntry *) * top->block_size);
        for(int i = 0; i < top->block_size; i++) {
            top->block_arr[i] = NULL;
        }

        // there are "count" pairs of "k-block_arr"
        // WENOTE: sucessors_arr is not read here
        for(int i = 0; i < count; i++) {
            int k = 0;
            fread(&k, sizeof(int), 1, file);
            top->block_arr[k] = (BlockEntry *)malloc(sizeof(BlockEntry));
            fread(top->block_arr[k], sizeof(BlockEntry), 1, file);
        }

        // read successors of each block
        for(int i = 0; i < count; i++) {
            int k = 0;
            fread(&k, sizeof(int), 1, file);
            top->block_arr[k]->successors_arr = (int *)malloc(sizeof(int) * top->block_arr[k]->successor_size);
            fread(top->block_arr[k]->successors_arr, sizeof(int) * top->block_arr[k]->successor_size, 1, file);
        }

        fclose(file);
        printf("Data has been read completely\n");

    } else {
        perror("Failed to open file for writing");
        exit(1);
    }
    return top;
}

// This function free memory of CFG binary
void top_free(Top_Level *top) {
    free(top->cfg_arr);
    for(int i = 0; i < top->block_size; i++) {
        if(top->block_arr[i]) {
            free(top->block_arr[i]->successors_arr);
        }
        free(top->block_arr[i]);
    }
    free(top->block_arr);
    free(top);
}

#endif
