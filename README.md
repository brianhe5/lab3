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
./hash-table-tester -t 4 -s 200000
Generation: 249,944 usec
Hash table base: 13,822,452 usec
  - 0 missing
Hash table v1: 10,486,287 usec
  - 0 missing
Hash table v2: 2,578,049 usec
  - 0 missing
```
Version 1 is a little slower than the base version. Since I only care about correctness and added a mutex around the whole body, Version 1 is similar to the base except for the added mutexes. These mutex functions such as init, lock, unlock, and destroy all have overhead which adds to the total time of Version 1 which causes Version 1 to be a little slower than the base version.

## Second Implementation
In the `hash_table_v2_add_entry` function, I added a lock around the portion of the code responsible for changing a value if the key already exists and around the portion that adds an entry to the list

### Performance
```shell
To run, enter "./hash-table-tester -t 4 -s 200000"
Results:
./hash-table-tester -t 4 -s 200000
Generation: 249,944 usec
Hash table base: 13,822,452 usec
  - 0 missing
Hash table v1: 10,486,287 usec
  - 0 missing
Hash table v2: 2,578,049 usec
  - 0 missing
```
V2's performance is significantly better than V1 and the base version. This is because our program utilizes lock per bucket implementation in the hash table. Lock per bucket means that multiple threads can add entries if the threads are adding those entries to different buckets.

## Cleaning up
```shell
Run "make clean" to clean up all binary files
```