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

In the first implementation, a single pthread_mutex_lock is assigned to the entire hash table. The critical section starts from obtaining the hash table entry to inserting and updating the list entry, ensuring mutual exclusion. However, since only one thread can add an entry at any time, other threads are blocked, resulting in significant inefficiencies when multiple threads try to access the hash table concurrently.

This implementation is theoretically slower because:

1. Global Lock Contention: All threads must wait for the single lock to be released, creating a bottleneck.
2. Thread Overhead: When threads are forced to wait, they incur synchronization overhead, further slowing down performance.

### Performance
For testing, let us run the following command:

```shell
./hash-table-tester -t 4 -s 150000
```

This command runs the tester with 4 threads, each adding 150,000 entries to the hash table. The results are:

```
Generation: 74,423 usec
Hash table base: 5,600,018 usec
  - 0 missing
Hash table v1: 6,700,874 usec
  - 0 missing
Hash table v2: 2,604,103 usec
  - 0 missing
```

The results confirm our theoretical intuition:

- The Hash Table V1 implementation is significantly slower than the base implementation.
- Explanation of Slowness: The primary issue is thread definition-related overhead caused by all threads contending for a single global lock. This leads to excessive blocking and inefficient CPU utilization.

## Second Implementation
The second implementation improves performance by introducing entry-level granularity for locks. Each hash table entry has its own pthread_mutex_lock, allowing multiple threads to access and modify different entries simultaneously.

This approach is expected to be faster because:

1. Increased Concurrency: Threads can add entries to different parts of the hash table without interfering with each other.
2. Reduced Contention: Lock contention is limited to threads targeting the same entry, significantly reducing blocking.

### Performance
Let us again run the following command:
```shell
./hash-table-tester -t 4 -s 150000
```

This command runs the tester with 4 threads, each adding 150,000 entries. The results are:

```
Generation: 112,025 usec
Hash table base: 6,601,101 usec
  - 0 missing
Hash table v1: 6,629,252 usec
  - 0 missing
Hash table v2: 2,729,054 usec
  - 0 missing
```
The results confirm our theoretical intuition:
1. Hash Table V2 implementation is faster than both V1 and the base implementation.
2. Reason for Improvement: The use of entry-level locks allows threads to exploit concurrency effectively, avoiding the bottleneck of a single global lock.

## Cleaning up
To clean up the project and remove binaries and other generated files:

```shell
make clean
```