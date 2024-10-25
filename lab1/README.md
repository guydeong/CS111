## UID: 205916585

## Pipe Up

Pipe Up is a program that simulates the behavior of the Unix Pipe operator, passing the output of one command to the input of the next command.

## Building

To build the program, navigate to the lab directory and run:
```{bash}
make
```
This will compile the source files and generate an executable named pipe.

## Running

To run the program, use the following format:

```{bash}
./pipe <command1> <command2> ... <commandN>
```

For instance, we can run the following

```{bash}
./pipe ls cat wc
```

This command simulates the shell operation ls | cat | wc, listing files in the current directory, passing the output to cat, and then counting the lines, words, and bytes.

## Cleaning up

To remove the binary files and clean up the directory, run:

```{bash}
make clean
```

This will delete all generated binary files, leaving only the source files and other original files intact.
