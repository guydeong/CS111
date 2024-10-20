#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        errno = EINVAL;  // Set errno for invalid argument
        perror("Error: No command-line arguments provided");
        return errno % 256;   // Exit with error code within 0-255
    }

    // Singular Argument Case
    if (argc - 1 == 1) {
        // Fork the process
        int i = 1;
        int rc = fork();
        if (rc < 0) {
            perror("fork failed");
            return 1;
        }
        else if (rc == 0) {
            execlp(argv[i], argv[i], NULL);
            perror("execlp failed");  // Print error if execlp fails
            _exit(errno % 256);       // Exit with errno
        }
        else {
            int status;
            int rc_wait = wait(&status);
            if (rc_wait == -1) {
                perror("wait failed");
                return 1;
            }

            // Check if the child exited normally
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                return exit_status;  // Return the exit status of the child
            } else {
                fprintf(stderr, "Child did not exit normally\n");
                return 1;
            }
        }
    }
    // Pipe Case
    else {
        int num_pipes = argc - 2;

        // Initialize the Array of Pipes
        int pipefd[num_pipes][2];
        for (int i = 0; i < num_pipes; i++) {
            if (pipe(pipefd[i]) == -1) {
                perror("Error In Creating Pipe");
                return 1;
            }
        }

        int i = 1;
        // Forking Processes Algorithm
        while (argv[i] != NULL) {
            // Fork the process
            int rc = fork();
            int j = i - 1;

            // First Program to Run
            if (i == 1) {
                if (rc < 0) {
                    perror("fork failed");
                    return 1;
                } else if (rc == 0) {
                    // Redirect stdout to the write end of the first pipe
                    if (dup2(pipefd[j][1], STDOUT_FILENO) == -1) {
                        perror("dup2 failed");
                        _exit(errno % 256);
                    }

                    // Close unused pipe ends in the child
                    close(pipefd[j][0]);
                    close(pipefd[j][1]);

                    // Run the corresponding program
                    execlp(argv[i], argv[i], NULL);
                    perror("execlp failed");
                    _exit(errno % 256);
                } else {
                    // Close the write end of the pipe in the parent
                    close(pipefd[j][1]);

                    // Wait for the child process to complete
                    int status;
                    int rc_wait = wait(&status);
                    if (rc_wait == -1) {
                        perror("wait failed");
                        return 1;
                    }

                    // Check if the child exited normally
                    if (WIFEXITED(status)) {
                        int exit_status = WEXITSTATUS(status);
                        if (exit_status != 0) {
                            return exit_status;
                        }
                    } else {
                        fprintf(stderr, "Child did not exit normally\n");
                        return 1;
                    }
                }
            }
            // Last Program to Run
            else if (i == argc - 1) {
                if (rc < 0) {
                    perror("fork failed");
                    return 1;
                } else if (rc == 0) {
                    // Redirect stdin to the read end of the last pipe
                    if (dup2(pipefd[j - 1][0], STDIN_FILENO) == -1) {
                        perror("dup2 failed");
                        _exit(errno % 256);
                    }

                    // Close unused pipe ends in the child
                    close(pipefd[j - 1][0]);

                    // Run the corresponding program
                    execlp(argv[i], argv[i], NULL);
                    perror("execlp failed");
                    _exit(errno % 256);
                } else {
                    // Close the read end of the last pipe in the parent
                    close(pipefd[j - 1][0]);

                    // Wait for the child process to complete
                    int status;
                    int rc_wait = wait(&status);
                    if (rc_wait == -1) {
                        perror("wait failed");
                        return 1;
                    }

                    // Check if the child exited normally
                    if (WIFEXITED(status)) {
                        int exit_status = WEXITSTATUS(status);
                        if (exit_status != 0) {
                            return exit_status;
                        }
                    } else {
                        fprintf(stderr, "Child did not exit normally\n");
                        return 1;
                    }
                }
            }
            // Rest of the Programs to Run
            else {
                if (rc < 0) {
                    perror("fork failed");
                    return 1;
                } else if (rc == 0) {
                    // Redirect stdin to the read end of the previous pipe
                    if (dup2(pipefd[j - 1][0], STDIN_FILENO) == -1) {
                        perror("dup2 failed for stdin");
                        _exit(errno % 256);
                    }

                    // Redirect stdout to the write end of the current pipe
                    if (dup2(pipefd[j][1], STDOUT_FILENO) == -1) {
                        perror("dup2 failed for stdout");
                        _exit(errno % 256);
                    }

                    // Close unused pipe ends in the child
                    close(pipefd[j - 1][0]);
                    close(pipefd[j][0]);
                    close(pipefd[j][1]);

                    // Run the corresponding program
                    execlp(argv[i], argv[i], NULL);
                    perror("execlp failed");
                    _exit(errno % 256);
                } else {
                    // Close the read end of the previous pipe in the parent
                    close(pipefd[j - 1][0]);

                    // Close the write end of the current pipe in the parent
                    close(pipefd[j][1]);

                    // Wait for the child process to complete
                    int status;
                    int rc_wait = wait(&status);
                    if (rc_wait == -1) {
                        perror("wait failed");
                        return 1;
                    }

                    // Check if the child exited normally
                    if (WIFEXITED(status)) {
                        int exit_status = WEXITSTATUS(status);
                        if (exit_status != 0) {
                            return exit_status;
                        }
                    } else {
                        fprintf(stderr, "Child did not exit normally\n");
                        return 1;
                    }
                }
            }
            // Move to the next argument in argv
            i++;
        }
    }
    return 0;
}