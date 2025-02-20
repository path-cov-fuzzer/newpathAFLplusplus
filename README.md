# pathAFL++

### How to compile and install pathAFL++

```bash
bash compile_and_install_on_host.sh
```

---

### How to use pathAFL++

An example is shown as below

```bash
cd example
export BBIDFILE=$(pwd)/bbid.txt
export CALLMAPFILE=$(pwd)/callmap.txt
export CFGFILE=$(pwd)/cfg.txt
export AFL_LLVM_CALLER=1
export LD_LIBRARY_PATH=$(pwd)
afl-clang-fast++ example.cc -o example
```

now we get bbid.txt, callmap.txt, cfg.txt

```bash
cat cfg.txt | grep "BasicBlock: " | wc -l > bbnum.txt
mv callmap.txt callmap_filtered.txt
mv cfg.txt cfg_filtered.txt
cat cfg_filtered.txt | grep "Function: " | nl -v 0 | awk '{print $1, $3, $4, $5, $6, $7, $8, $9}' > function_list.txt
g++ ../fuzzing_support/convert.cpp -I../fuzzing_support -o convert
./convert
```

now we get top.bin

Run below start fuzzing
```bash
TODO: here
```

---





