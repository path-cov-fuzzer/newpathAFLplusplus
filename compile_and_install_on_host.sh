#!/bin/bash -e

# generate libhashcompare.a static library
clang++ -c hashcompare.cpp
ar rcs libhashcompare.a hashcompare.o

# -lcrypto: use <openssl/sha.h>
# -lhashcompare: use hashcompare.cpp
# -lstdc++: when use gcc/clang link .o files. If some .o files is compiled from C++, then -lstdc++ is necessary to link these files.
LLVM_CONFIG=llvm-config-17 LDFLAGS="-L$(pwd) -lcrypto -lhashcompare -lstdc++" make -e source-only
sudo LLVM_CONFIG=llvm-config-17 LDFLAGS="-L$(pwd) -lcrypto -lhashcompare -lstdc++" make -e install

