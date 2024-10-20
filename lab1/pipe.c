#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if (argc <= 1){return EINVAL;}
	//Singular Argument Case
	if (argc-1==1){
		//Fork the process
		int rc = fork();
		if (rc < 0){
			fprintf(stderr, "fork failed\n");
			exit(1);
		}
		else if(rc==0){
			if (execlp(argv[1], argv[1], NULL)){
				perror("Error Forking Process");
				exit(1);
			}
		}
		else{
			int rc_wait = wait(NULL);
		}
	}
	//Pipe Case
	else{
		int num_pipes = argc - 2;

		//Initialize the Array of Pipes.
		int pipefd[num_pipes][2];
		for (int i = 0; i < num_pipes; i++){
			if(pipe(pipefd[i])==-1){
				perror("Error In Creating Pipe");
				exit(1);
			}
		}
		
		int i = 1;
		//Forking Processes Algorithm
		while (argv[i] != NULL) {
			// Fork the process
			int rc = fork();
			int j = i - 1;

			// First Program to Run
			if (i == 1) {
				// CASE 1: Fork failed; exit
				if (rc < 0) {
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				// CASE 2: Child process from fork
				else if (rc == 0) {
					// Redirect stdout to the write end of the first pipe
					dup2(pipefd[j][1], STDOUT_FILENO);

					// Close unused pipe ends in the child
					close(pipefd[j][0]);
					close(pipefd[j][1]);

					// Run the corresponding program
					if (execlp(argv[i], argv[i], NULL)) {
						perror("Error Forking Process");
						exit(1);
					}
				}
				// CASE 3: Parent
				else {
					// Close the write end of the pipe in the parent
					close(pipefd[j][1]);

					// Wait for the child process to complete
					int rc_wait = wait(NULL);
					if (rc_wait == -1) {
						return ECHILD;
					}
				}
			}
			// Last Program to Run
			else if (i == argc - 1) {
				// CASE 1: Fork failed; exit
				if (rc < 0) {
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				// CASE 2: Child process from fork
				else if (rc == 0) {
					// Redirect stdin to the read end of the last pipe
					dup2(pipefd[j - 1][0], STDIN_FILENO);

					// Close unused pipe ends in the child
					close(pipefd[j - 1][0]);

					// Run the corresponding program
					if (execlp(argv[i], argv[i], NULL)) {
						perror("Error Forking Process");
						exit(1);
					}
				}
				// CASE 3: Parent
				else {
					// Close the read end of the last pipe in the parent
					close(pipefd[j - 1][0]);

					// Wait for the child process to complete
					int rc_wait = wait(NULL);
					if (rc_wait == -1) {
						return ECHILD;
					}
				}
			}
			// Rest of the Programs to Run
			else {
				// CASE 1: Fork failed; exit
				if (rc < 0) {
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				// CASE 2: Child process from fork
				else if (rc == 0) {
					// Redirect stdin to the read end of the previous pipe
					dup2(pipefd[j - 1][0], STDIN_FILENO);

					// Redirect stdout to the write end of the current pipe
					dup2(pipefd[j][1], STDOUT_FILENO);

					// Close unused pipe ends in the child
					close(pipefd[j - 1][0]);
					close(pipefd[j][0]);
					close(pipefd[j][1]);

					// Run the corresponding program
					if (execlp(argv[i], argv[i], NULL)) {
						perror("Error Forking Process");
						exit(1);
					}
				}
				// CASE 3: Parent
				else {
					// Close the read end of the previous pipe in the parent
					close(pipefd[j - 1][0]);

					// Close the write end of the current pipe in the parent
					close(pipefd[j][1]);

					// Wait for the child process to complete
					int rc_wait = wait(NULL);
					if (rc_wait == -1) {
						return ECHILD;
					}
				}
			}
			// Move to the next argument in argv
		i++;
		}
		
	}
}
