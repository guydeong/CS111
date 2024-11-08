# You Spin Me Round Robin

This lab project involves writing an implementation for a Round Robin scheduling algorithm in C. The goal is to process a workload using a specified quantum length and calculate the average waiting and response times for the processes.

## Building
To compile the `rr` executable, use the following command:

```shell
make
```

This will build the `rr` program using the provided `Makefile`. Ensure that you are in the `lab-02` directory where the skeleton code resides.

## Running
To run the `rr` executable, use the following command format:

```shell
./rr <path_to_processes_file> <quantum_length>
```

### Example Command
```shell
./rr processes.txt 3
```

### Expected Output Format
The program will output the average waiting time and response time:

```shell
Average waiting time: <calculated_waiting_time>
Average response time: <calculated_response_time>
```

### Example Output
Given a sample `processes.txt` file and a quantum length of `3`, you might see:
```shell
Average waiting time: 7.00
Average response time: 2.75
```

## Cleaning up
To remove all compiled binary files and clean the directory, use the following command:

```shell
make clean
```

This will ensure that all temporary and compiled files are deleted, leaving only the source files and documentation.

