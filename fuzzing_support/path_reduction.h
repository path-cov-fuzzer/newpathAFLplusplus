#ifndef PATH_REDUCER_H
#define PATH_REDUCER_H

#include <stdint.h>
#include <stddef.h>

typedef struct PathReducer PathReducer;
typedef int32_t BlockID;
typedef int32_t FunID;

// Gets a path reducer
PathReducer* get_path_reducer(const void* top_level, int32_t k);

// Frees a path reducer
void free_path_reducer(PathReducer* ptr);

void free_boxed_array(int* ptr, size_t len);

// Reduces a path, and gets the hash
const char* reduce_path(const PathReducer* reducer, const BlockID* path, int32_t path_size, FunID entry_fun_id);

// Returns the reduced path, the length of which is stored in `out_len`
const BlockID* reduce_path1(const PathReducer* reducer, const BlockID* path, int32_t path_size, FunID entry_fun_id, int* out_len);

#endif

