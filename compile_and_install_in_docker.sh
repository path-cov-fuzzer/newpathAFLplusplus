#!/bin/bash -e

pushd $FUZZER/repo

# generate libhashcompare.a static library
clang++ -c hashcompare.cpp
ar rcs libhashcompare.a hashcompare.o

# -lcrypto: use <openssl/sha.h>
# -lhashcompare: use hashcompare.cpp
# -lstdc++: when use gcc/clang link .o files. If some .o files is compiled from C++, then -lstdc++ is necessary to link these files.
# -lpath_reduction: use libpath_reduction.so
LLVM_CONFIG=llvm-config-17 LD_LIBRARY_PATH="$(pwd)" CFLAGS="-I$(pwd)/fuzzing_support" LDFLAGS="-L$(pwd) -lcrypto -lhashcompare -lstdc++ -lpath_reduction" make -e -j$(nproc)

# aflpp_driver is not changed, so its compilation process does not have to change
export CC=clang
export CXX=clang++
export AFL_NO_X86=1
export PYTHON_INCLUDE=/
LLVM_CONFIG=llvm-config-17 LD_LIBRARY_PATH="$(pwd)" CFLAGS="-I$(pwd)/fuzzing_support" LDFLAGS="-L$(pwd) -lcrypto -lhashcompare -lstdc++ -lpath_reduction" make -e -C utils/aflpp_driver || exit 1

popd

