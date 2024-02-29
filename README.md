# Hash Hash Hash
Program to prevent data race conditions in a concurrent hash table data structure used by several threads through the use of pthread locks!

## Building
```shell
We use a make file. To build, enter "make"
```

## Running
```shell
To run, enter "./hash-table-tester -t 4 -s 200000", where 4 is the number of cores and 200000 is the number of hash table entries added per thread
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a single mutex lock for the entire function because I only cared about correctness. Therefore, I placed it around the entire body to ensure 0 missing entries

### Performance
```shell
To run, enter "./hash-table-tester -t 4 -s 200000"
Results:
PASTE RESULTS HERE
```
Version 1 is a little slower than the base version. Since I only care about correctness and added a mutex around the whole body, Version 1 is similar to the base except for the added mutexes. These mutex functions such as init, lock, unlock, and destroy all have overhead which adds to the total time of Version 1 which causes Version 1 to be a little slower than the base version.

## Second Implementation
In the `hash_table_v2_add_entry` function, I TODO

### Performance
```shell
To run, enter "./hash-table-tester -t 4 -s 200000"
Results:
PASTE RESULTS HERE
```
V2's performance is significantly better than V1 and the base version. This is because: ADD MORE HERE
TODO more results, speedup measurement, and analysis on v2

## Cleaning up
```shell
Run "make clean" to clean up all binary files
```