# Hash Hash Hash
This project implements a thread-safe hash table using two different locking strategies (v1 and v2). The hash table employs separate chaining for collision resolution, where each cell of the hash table is a singly linked list of key-value pairs. 

For v1, we aim for correctness by using a single global mutex. For v2, we aim to improve both correctness and performance by introducing finer-grained locking mechanisms. The provided tester code measures the performance and verifies the correctness of the implementations.

## Building
To build the hash table tester:
```shell
make
```

## Running
In order to run the hash table tester, we run the following:
```shell
./hash-table-tester -t <number_of_threads> -s <entries_per_thread>
```
where the `-t` option sets the number of threads and `-s` option sets the number of entries to add per thread. The tester will then report the following:
1. Generation time
2. Execution time for the base, v1, and v2 implementations
3. Number of missing entries for each implementation
## First Implementation
In the `hash_table_v1_add_entry` function, I added TODO

### Performance
```shell
TODO how to run and results
```
Version 1 is a little slower/faster than the base version. As TODO

## Second Implementation
In the `hash_table_v2_add_entry` function, I TODO

### Performance
```shell
TODO how to run and results
```

TODO more results, speedup measurement, and analysis on v2

## Cleaning up
To clean up the project and remove binaries and other generated files:

```shell
make clean
```