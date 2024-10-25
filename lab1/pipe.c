#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        errno = EINVAL;  // Set errno for invalid argument
        perror("Error: No command-line arguments provided");
        exit(errno);
    }

    // Singular Argument Case
    if (argc - 1 == 1) {
        int i = 1;
        int rc = fork();
        if (rc < 0) {
            perror("fork failed");
            exit(errno);
        } else if (rc == 0) {
            execlp(argv[i], argv[i], NULL);
            perror("execlp failed");
            exit(errno);
        } else {
            int status;
            if (wait(&status) == -1) {
                perror("wait failed");
                exit(errno);
            }

            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else {
                fprintf(stderr, "Child did not exit normally\n");
                exit(errno);
            }
        }
    }
    // Pipe Case
    else {
        int num_pipes = argc - 2;
        int pipefd[num_pipes][2];

        // Initialize the Array of Pipes
        for (int i = 0; i < num_pipes; i++) {
            if (pipe(pipefd[i]) == -1) {
                perror("Error creating pipe");
                exit(errno);
            }
        }

        // Forking Processes Algorithm
        for (int i = 1; i < argc; i++) {
            int rc = fork();
            int j = i - 1;

            if (rc < 0) {
                perror("fork failed");
                exit(errno);
            } else if (rc == 0) {
                // First Program to Run
                if (i == 1) {
                    if (dup2(pipefd[j][1], STDOUT_FILENO) == -1) {
                        perror("dup2 failed");
                        exit(errno);
                    }
                }
                // Last Program to Run
                else if (i == argc - 1) {
                    if (dup2(pipefd[j - 1][0], STDIN_FILENO) == -1) {
                        perror("dup2 failed");
                        exit(errno);
                    }
                }
                // Intermediate Programs to Run
                else {
                    if (dup2(pipefd[j - 1][0], STDIN_FILENO) == -1 ||
                        dup2(pipefd[j][1], STDOUT_FILENO) == -1) {
                        perror("dup2 failed");
                        exit(errno);
                    }
                }

                // Close all pipe ends in the child
                for (int k = 0; k < num_pipes; k++) {
                    close(pipefd[k][0]);
                    close(pipefd[k][1]);
                }

                execlp(argv[i], argv[i], NULL);
                perror("execlp failed");
                exit(errno);
            } else {
                // Close unused pipe ends in the parent
                if (i > 1){
                    close(pipefd[j - 1][0]);
                };
                if (i < argc - 1){
                    close(pipefd[j][1]);
                };

                int status;
                if (wait(&status) == -1) {
                    perror("wait failed");
                    exit(errno);
                }

                if (WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status != 0) {
                        return exit_status;
                    }
                } else {
                    fprintf(stderr, "Child did not exit normally\n");
                    exit(errno);
                }
            }
        }
    }

    return 0;
}