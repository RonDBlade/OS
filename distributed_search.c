#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

typedef struct queueStruct{/*queue struct and operations from geeksforgeeks.org*/
	int size,front,rear;
	char* array;
}queueStruct;

queueStruct* initQueue(){
	queueStruct* queue=(queueStruct*) malloc(sizeof(queueStruct));
	if(queue==NULL){
		printf("malloc failed: %s\n",strerror(errno));
		exit(1);
	}
	queue->size=0;
	queue->front=0;
	queue->rear=0;
	queue->array=(char*)malloc(sizeof(char));
	if(queue->array==NULL){
		printf("malloc failed: %s\n",strerror(errno));
		exit(1);
	}
	return queue;
}

void enqueue(queueStruct* queue,char* item){/*inserts new item to the queue*/
	int i;
	printf("%d\n",NAME_MAX*sizeof(char)*((queue->size)+1));
	queue->array=(char*)realloc(queue->array,NAME_MAX*sizeof(char)*((queue->size)+1));
	if(queue->array==NULL){
		printf("ERROR in realloc(): %s\n",strerror(errno));
		exit(1);
	}
	printf("after realloc\n");
	for(i=0;i<NAME_MAX;i++){/*insert item to the last place in the queue*/
		if(i<strlen(item))
			queue->array[(queue->rear)+i]=item[i];
		else
			queue->array[(queue->rear)+i]='\0';
	}
	queue->rear=(queue->rear)+(NAME_MAX*sizeof(char));
	queue->size=(queue->size)+1;
}

void dequeue(queueStruct* queue,char* item){/*pops out the first item in the queue,saves in item*/
	int i=0;
	for(i=0;i<NAME_MAX;i++){
		item[i]=queue->array[(queue->front)+i];
	}
	queue->size=(queue->size)-1;
	memmove(queue->array,queue->array+(NAME_MAX*sizeof(char)),NAME_MAX*sizeof(char)*(queue->size));
	queue->array=(char*)realloc(queue->array,NAME_MAX*sizeof(char)*(queue->size));
	queue->rear=(queue->rear)-(NAME_MAX*sizeof(char));
}

static int count=0;
static char* name;
static queueStruct* current_dirs;/*max size of name of dir is NAME_MAX (from limits.h)*/
pthread_mutex_t countlock;
pthread_mutex_t enqueuelock;
pthread_mutex_t dequeuelock;

int string_to_int(char* thread_count){/*translate the argument which specifies how many threads we want to use to an int*/
	long count=0,i;
	for(i=0;i<strlen(thread_count);i++){
		count*=10;
		count+=thread_count[i]-'0';
	}
	return count;
}

void* thread_func(void* thread_param){/*what each thread does*/
	/*listing items in directory from stackoverflow "how to list files in a directory"*/
	int rc;
	long num=(long)thread_param;
	DIR *d;
	struct dirent *dir;
	char dirname[NAME_MAX];
	rc=pthread_mutex_lock(&dequeuelock);
	if(rc){
		printf("ERROR in pthread_mutex_lock(): %s\n",strerror(rc));
		exit(1);
	}
	if(current_dirs->size==0){/*change it so we wait for a signal that we enqueued a new
		dir and then "continue" when we will put a while loop instead of single iteration*/
		pthread_mutex_unlock(&dequeuelock);
		pthread_exit(NULL);
	}
	dequeue(current_dirs,dirname);
	rc=pthread_mutex_unlock(&dequeuelock);
	if(rc){
		printf("ERROR in pthread_mutex_unlock(): %s\n",strerror(rc));
		exit(1);
	}
	d=opendir((const char*)dirname);
	if(d){
		while((dir=readdir(d))!=NULL){
			printf("%s\n",dir->d_name);
			if((!strcmp(dir->d_name,".")) || (!strcmp(dir->d_name,"..")))
				continue;
			else if(dir->d_type==DT_DIR){
				rc=pthread_mutex_lock(&enqueuelock);
				if(rc){
					printf("ERROR in pthread_mutex_lock(): %s\n",strerror(rc));
					exit(1);
				}
				enqueue(current_dirs,dir->d_name);
				rc=pthread_mutex_unlock(&enqueuelock);
				if(rc){
					printf("ERROR in pthread_mutex_unlock(): %s\n",strerror(rc));
					exit(1);
				}
			}
			else if(!strcmp(dir->d_name,name)){
				printf("yeet\n");
				rc=pthread_mutex_lock(&countlock);
				if(rc){
					printf("ERROR in pthread_mutex_lock(): %s\n",strerror(rc));
					exit(1);
				}
				count++;
				rc=pthread_mutex_unlock(&countlock);
				if(rc){
					printf("ERROR in pthread_mutex_unlock(): %s\n",strerror(rc));
					exit(1);
				}
			}
		}
	}
	closedir(d);


	printf("jeff3\n");
	pthread_exit(NULL);
}

int main(int argc,char** argv){
	long num_threads,i;
	int rc;
	void* status;
	name=argv[2];
	current_dirs=initQueue();
	printf("jeff1\n");
	enqueue(current_dirs,argv[1]);
	rc=pthread_mutex_init(&countlock,NULL);
	if(rc){
		printf("ERROR in pthread_mutex_init(): %s\n",strerror(rc));
		exit(1);
	}
	rc=pthread_mutex_init(&enqueuelock,NULL);
	if(rc){
		printf("ERROR in pthread_mutex_init(): %s\n",strerror(rc));
		exit(1);
	}
	rc=pthread_mutex_init(&dequeuelock,NULL);
	if(rc){
		printf("ERROR in pthread_mutex_init(): %s\n",strerror(rc));
		exit(1);
	}
	num_threads=string_to_int(argv[3]);
	printf("number of threads in total is %ld\n",num_threads);
	pthread_t thread[num_threads];
	for(i=0;i<num_threads;i++){
		rc=pthread_create(&thread[i],NULL,thread_func,(void*)i);
		if(rc){
			printf("ERROR in pthread_create(): %s\n",strerror(rc));
			exit(1);
		}
	}
	printf("jeff2\n");
	for(i=0;i<num_threads;i++){
		rc=pthread_join(thread[i],&status);
		if(rc){
			printf("ERROR in pthread_join(): %s\n",strerror(rc));
			exit(1);
		}
		printf("Main: completed join with thread %ld having a status of %ld\n",i,(long)status);
	}
	printf("Done searching, found  %d files\n",count);
	free(current_dirs->array);
	free(current_dirs);
	pthread_mutex_destroy(&countlock);
	pthread_mutex_destroy(&enqueuelock);
	pthread_mutex_destroy(&dequeuelock);
	exit(0);
}
