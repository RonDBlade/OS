#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/*if the new process should be a background process,the last argument of arglist(except
the NULL element on the actual last place) is &,then it should be ran in the background*/
int check_background(int count,char** arglist){
	if(strlen(arglist[count-1])==1 && arglist[count-1]=='&')
		return 1;
	return 0;
}


// arglist - a list of char* arguments (words) provided by the user
// it contains count+1 items, where the last item (arglist[count]) and *only* the last is NULL
// RETURNS - 1 if should cotinue, 0 otherwise
int process_arglist(int count, char** arglist){
	int pid=fork();
	if(pid){/*parent,the shell*/
		sigaction(2,sig_ignore_int,SIGINT);
		if(!check_background(count,arglist))/*check if child process is not background*/
			wait(NULL);
		else

		sigaction(2,SIGINT,sig_ignore_int);
		return 1;
	}
	else
		while(1){
	
		}
		return 1;
}

// prepare and finalize calls for initialization and destruction of anything required
int prepare(void){
	struct sigaction sig_ignore_int={
		.sa_handler = SIG_DFL,
		.sa_flags = 
	}

}


int finalize(void){
	
	
}

int main(void)
{
	if (prepare() != 0)
		exit(-1);
	
	while (1)
	{
		char** arglist = NULL;
		char* line = NULL;
		size_t size;
		int count = 0;

		if (getline(&line, &size, stdin) == -1) {
			free(line);
			break;
		}
    
		arglist = (char**) malloc(sizeof(char*));
		if (arglist == NULL) {
			printf("malloc failed: %s\n", strerror(errno));
			exit(-1);
		}
		arglist[0] = strtok(line, " \t\n");
    
		while (arglist[count] != NULL) {
			++count;
			arglist = (char**) realloc(arglist, sizeof(char*) * (count + 1));
			if (arglist == NULL) {
				printf("realloc failed: %s\n", strerror(errno));
				exit(-1);
			}
      
			arglist[count] = strtok(NULL, " \t\n");
		}
    
		if (count != 0) {
			if (!process_arglist(count, arglist)) {
				free(line);
				free(arglist);
				break;
			}
		}
    
		free(line);
		free(arglist);
	}
	
	if (finalize() != 0)
		exit(-1);

	return 0;
}
