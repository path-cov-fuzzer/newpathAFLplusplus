#!/bin/bash -e

# enter FUZZER dir
export FUZZER="/data/cse12132329/newpathAFLplusplus"
cd $FUZZER

# get path_reduction module
git submodule update --init fuzzing_support/path-cov/
cd fuzzing_support/path-cov/
cargo build --release
cp target/release/libpath_reduction.so ../../
cd -

# generate libhashcompare.a static library
clang++ -c hashcompare.cpp
ar rcs libhashcompare.a hashcompare.o

# -lcrypto: use <openssl/sha.h>
# -lhashcompare: use hashcompare.cpp
# -lstdc++: when use gcc/clang link .o files. If some .o files is compiled from C++, then -lstdc++ is necessary to link these files.
# -lpath_reduction: use libpath_reduction.so
LLVM_CONFIG=llvm-config-17 LD_LIBRARY_PATH="$(pwd)" CFLAGS="-I$(pwd)/fuzzing_support" LDFLAGS="-L$(pwd) -lcrypto -lhashcompare -lstdc++ -lpath_reduction" make -e source-only
sudo LLVM_CONFIG=llvm-config-17 LD_LIBRARY_PATH="$(pwd)" CFLAGS="-I$(pwd)/fuzzing_support" LDFLAGS="-L$(pwd) -lcrypto -lhashcompare -lstdc++ -lpath_reduction" make -e install

