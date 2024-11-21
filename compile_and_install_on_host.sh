#!/bin/bash -e

LLVM_CONFIG=llvm-config-17 make -e source-only
sudo LLVM_CONFIG=llvm-config-17 LDFLAGS="-lcrypto" make -e install

