#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{

	//Printing the Whole Array in order to view argument parameters.
	int i = 1;
	while(argv[i] != NULL){
		printf("%s\n", argv[i]);
		//Initialize the pipe
		int pipefd[2];
		if(pipe(pipefd)==-1){
			perror("Error In Creating Pipe");
			exit(1);
		}

		//Fork the process
		int rc = fork();

		//CASE 1: Fork failed; exit
		if (rc < 0){
			fprintf(stderr, "fork failed\n");
			exit(1);
		}
		//CASE 2: Child process from fork
		else if(rc==0){
			//Close the write end of the child
			close(pipefd[1]);
			//Copy the read end of the child's pipe file descriptor into stdin file descriptor.
			dup2(pipefd[0], STDIN_FILENO);
			//Close the read end once finished
			close(pipefd[0]);
			if (execlp(argv[i], argv[i], NULL)){
				perror("Error Forking Process");
				exit(1);
			}
			
		}
		//CASE 3: Parent
		else{
			//Close the read end of the parent
			close(pipefd[0]);
			//Copy the stdout file descriptor into the write end of the pipe's file descriptor
			dup2(STDOUT_FILENO, pipefd[1]);
			//Close the write end once finished
			close(pipefd[0]);
			int rc_wait = wait(NULL);
			i++;
		}
		
	}
	//Calls fork() to create a new child process to run the command
	
	//Calls some variant of exec() to run the command
	//Waits for the command to complete by calling wait()
}
