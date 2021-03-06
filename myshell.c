#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

/*if the new process should be a background process,the last argument of arglist(except
the NULL element on the actual last place) is &,then it should be ran in the background*/
int check_background(int count,char** arglist){
	if(strlen(arglist[count-1])==1 && arglist[count-1][0]=='&'){/*length is 1 and first position is &,means its only &*/
		return 1;
	}
	return 0;
}

/*if there is | it will return its position,else will return 0*/
int check_pipe(int count,char** arglist){
	int i;
	for(i=1;i<count-1;i++){/*assuming if there is a pipe it has something before and
				after it,we can start at 1 and end at count-2*/
		if(strlen(arglist[i])==1 && arglist[i][0]=='|'){/*length is 1 and in the first position it has |,means its only |*/
			return i;
		}
	}
	return 0;
}


// arglist - a list of char* arguments (words) provided by the user
// it contains count+1 items, where the last item (arglist[count]) and *only* the last is NULL
// RETURNS - 1 if should cotinue, 0 otherwise
int process_arglist(int count, char** arglist){
	int location,i;
	int pid=fork();
	int terminated;
	struct sigaction sigdfl={/*the signal will be handled the same it is handled*/
		.sa_handler=SIG_DFL
	};
	if(pid<0){/*fork failed-error in parent*/
		fprintf(stderr,"fork failed: %s\n",strerror(errno));
		return 0;
	}
	if(pid){/*parent,the shell*/
		if(!check_background(count,arglist)){/*check if child process is not background*/
				if(sigaction(SIGCHLD,&sigdfl,NULL)==-1){/*stop ignoring sigchld,the normal action we have for it*/
					fprintf(stderr,"sigaction failed: %s\n",strerror(errno));
					return 0;
				}
				do{terminated=wait(NULL);}/*if foreground,need to wait for it,wait returns the pid as long as sigchld is handled defaultly*/
				while(terminated!=pid);/*sigchld can be from background,need to ignore*/
				struct sigaction sig_no_wait_chld={/*makes so that background processes that finish do not stay in the system
				as zombies,but are removed,no need for wait,and it won't return the pid,it returns -1*/
					.sa_handler=SIG_DFL,
					.sa_flags=SA_NOCLDWAIT,
				};
				if(sigaction(SIGCHLD,&sig_no_wait_chld,NULL)==-1){/*we do this since we finished waiting for the foreground,and we 
					don't want to wait for the background processes*/
					fprintf(stderr,"sigaction failed: %s\n",strerror(errno));
					return 0;
				}
		}
		return 1;/*parent should return 1 when everything is working fine*/
	}
	else{/*child,foreground or background*/
		if(!check_background(count,arglist)){/*foreground*/
			if(sigaction(SIGINT,&sigdfl,NULL)==-1){/*we stop ignoring sigint*/
				fprintf(stderr,"sigaction failed: %s\n",strerror(errno));
				exit(1);
				}
			/*return the handling of sigint to be normal for the foreground*/
			location=check_pipe(count,arglist);/*location has the position '|' is in,and 0 if it is not in the arglist array*/
			if(!location){/*no pipe,only one process to create*/
				if(execvp(arglist[0],arglist)==-1){
						fprintf(stderr,"execvp failed: %s\n",strerror(errno));
						exit(1);
						}
				exit(0);/*should not get here*/
			}
			else{/*has a pipe symbol,we need to seperate the arglist array and
						set up the output of the first part to be input of second part*/
				int pipefd[2];
				int cpid;
				arglist[location]=NULL;
				if(pipe(pipefd)==-1){/*we want to connect the input of the 2nd process to the output of the 1st process*/
					fprintf(stderr,"pipe failed: %s\n",strerror(errno));
					exit(1);
				}
				cpid=fork();/*from stackoverflow:Using pipes in execvp,stdin
						and stdout redirection*/
				if(cpid==-1){
					fprintf(stderr,"fork failed: %s\n",strerror(errno));
					exit(1);
				}
				if(cpid==0){/*the input,the process from the 1st part of the arglist*/
					fclose(stdout);
					if(dup2(pipefd[1],1)==-1){/*closing stdout so that the stdout
					of this child will be the input of the 2nd part of pipe*/
						fprintf(stderr,"dup2 failed: %s\n",strerror(errno));
						exit(1);
					}
					close(pipefd[0]);
					close(pipefd[1]);/*dup will handle the information being forwarded to the 2nd process,closing so
					it will get EOF in the end*/
					if(execvp(arglist[0],arglist)==-1){
						fprintf(stderr,"execvp failed: %s\n",strerror(errno));
						exit(1);
					}
				}
				for(i=location+1;i<count;i++){
					arglist[i-location-1]=arglist[i];/*setting the arglist so that the items we need to use now
					will be in the start and then putting NULL in the last position for execvp*/
				}
				arglist[count-location-1]=NULL;
				cpid=fork();
				if(cpid<0){
					fprintf(stderr,"fork failed: %s\n",strerror(errno));
					exit(1);
				}
				if(cpid==0){/*the 2nd process,the new output*/
					fclose(stdin);
					if(dup2(pipefd[0],0)){/*closing the stdin so that the stdin for this child
						will be from the 1st part of the pipe*/
						fprintf(stderr,"dup2 failed: %s\n",strerror(errno));
						exit(1);
					}
					close(pipefd[1]);
					close(pipefd[0]);/*dup will handle the information be received from the 1st,closing so we won't have
					things we don't need still open*/
					if(execvp(arglist[0],arglist)==-1){
						fprintf(stderr,"execvp failed: %s\n",strerror(errno));
						exit(1);
					}
				}
				close(pipefd[0]);
				close(pipefd[1]);/*closing the pipe for the parent,he doesn't use it at all*/
				wait(NULL);
				wait(NULL);/*wait for both the children*/
				exit(0);/*done*/
			}

		}

		else{/*background,we just want to run it*/
			arglist[count-1]=NULL;
			count--;/*in count-1 we have '&',should not be sent to execvp,we don't really need count-- though,just for correctness*/
			if(execvp(arglist[0],arglist)==-1){
				fprintf(stderr,"execvp failed: %s\n",strerror(errno));
				exit(1);
			}
			exit(0);
		}

	}
		return 1;/*not supposed to get here*/
}

// prepare and finalize calls for initialization and destruction of anything required
int prepare(void){
	struct sigaction sig_ignore_int={
		.sa_handler = SIG_IGN,
		.sa_flags = SA_RESTART,
	};
	if(sigaction(SIGINT,&sig_ignore_int,NULL)==-1){/*make parent ignore sigint*/
		fprintf(stderr,"sigaction failed: %s\n",strerror(errno));
		return 1;
		}
	struct sigaction sig_no_wait_chld={
		.sa_handler = SIG_DFL,
		.sa_flags = SA_NOCLDWAIT,
		};
	if(sigaction(SIGCHLD,&sig_no_wait_chld,NULL)==-1){/*makes background processes not become zombies when they finish as we don't
		wait() for them*/
		fprintf(stderr,"sigaction failed: %s\n",strerror(errno));
		return 1;
		}
	return 0;
}


int finalize(void){/*don't need to do anything*/
	
	return 0;
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
