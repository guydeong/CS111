#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	// if (argv[0] == NULL){
	// 	return NULL;
	// }
	// else{
	// 	printf("%s\n", argv[0]);
	// 	main(argc--, &argv[1]);
	// }
	//Printing the Whole Array in order to view argument parameters.
	printf("%d\n", argc);

	//Singular Argument Case
	if (argc-1<=1){
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
		while (argv[i] != NULL){
			//Fork the process
			int rc = fork();
			//First Program to Run
			int j = i-1;
			if (i==1){
				//CASE 1: Fork failed; exit
				if (rc < 0){
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				//CASE 2: Child process from fork
				else if(rc==0){
					//Close the read end of the child
					close(pipefd[j][0]);
					//Copy the write end of the child's pipe file descriptor into stdin file descriptor.
					dup2(pipefd[j][0], STDOUT_FILENO);
					//Close the rest of the pipe file descriptors
					for (int k = 0; k < num_pipes; k++){
						close(pipefd[k][0]);
						close(pipefd[k][1]);
					}
					//Run the corresponding program
					if (execlp(argv[i], argv[i], NULL)){
						perror("Error Forking Process");
						exit(1);
					}
				}
				//CASE 3: Parent
				else{
					//Close the write end once finished
					close(pipefd[j][1]);
					int rc_wait = wait(NULL);
				}
			}
			//Rest of the programs to run
			else if(i==argc-1){
				//CASE 1: Fork failed; exit
				if (rc < 0){
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				//CASE 2: Child process from fork
				else if(rc==0){
					//Close the read end of the child
					close(pipefd[j][0]);
					//Copy the write end of the child's pipe file descriptor into stdin file descriptor.
					dup2(pipefd[j][1], STDIN_FILENO);
					//Close the rest of the pipe file descriptors
					for (int k = 0; k < num_pipes; k++){
						close(pipefd[k][0]);
					}
					//Run the corresponding program
					if (execlp(argv[i], argv[i], NULL)){
						perror("Error Forking Process");
						exit(1);
					}

				}
				//CASE 3: Parent
				else{
					//Close the rest of the pipe file descriptors
					//HYPOTHESIS: Close all file descriptors that are [k][0]
					for (int k = 0; k < num_pipes; k++){
						close(pipefd[k][0]);
					}
					int rc_wait = wait(NULL);
				}
			}
			//Last program to run
			else{
				//CASE 1: Fork failed; exit
				if (rc < 0){
					fprintf(stderr, "fork failed\n");
					exit(1);
				}
				//CASE 2: Child process from fork
				else if(rc==0){
					//Copy the read end of the child's pipe file descriptor into stdin file descriptor.
					dup2(pipefd[j-1][0], STDIN_FILENO);
					//Copy the stdout file descriptor to the write end of the child's pipe file descriptor.
					dup2(STDOUT_FILENO, pipefd[j][1]);
					//Close the pipe file descriptors
					close(pipefd[j-1][0]);
					//Run the corresponding program
					if (execlp(argv[i], argv[i], NULL)){
						perror("Error Forking Process");
						exit(1);
					}
				}
				//CASE 3: Parent
				else{
					//Close the pipe file descriptors
					close(pipefd[j-1][0]);
					int rc_wait = wait(NULL);
				}

			}
		i++;
		}
		
	}
}
